Imports CANoe
Imports System.IO

Public Class Form_VBNET_CANoe

#Region "Declarations"

    ' Literals
    Private Const mRelativeConfigPath As String = "..\..\..\COM.Net\COM.Net.cfg"
    ' Fields
    Private mAbsoluteConfigPath As String
    Private mConfigDirToolTip As ToolTip = New ToolTip
    Private mCANoeInstanceRunning As Boolean
    ' Delegates
    Private Delegate Sub DelSafeInvocation()
    Private Delegate Sub DelSafeInvocationWithParam(sysvarName As String, value As Object)
    ' CANoe objects
    Private WithEvents mCANoeApp As Application
    Private WithEvents mCANoeMeasurement As Measurement
    ' CANoe CAPL functions
    Private mCANoeMultiply As CAPLFunction
    ' CANoe system variables    
    Private WithEvents mCANoeSysVar1 As Variable
    Private WithEvents mCANoeSysVar2 As Variable
    ' CANoe signals    
    Private mCANoeEngineStatus As Signal
    Private mCANoeEngineSpeed As Signal
    Private mCANoeEngineTemp As Signal
#End Region

#Region "Construction & Initialization"

    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.

        ' Check if the necessary configuration is at the correct location.
        PrepareOpenConfiguration()
    End Sub
#End Region

#Region "Opening Tasks"

    ' Shows the expected directory of the related configuration file and checks if it's located there.
    Private Sub PrepareOpenConfiguration()

        ' Creates an absolute path out of the provided relative path.
        mAbsoluteConfigPath = Path.GetFullPath(mRelativeConfigPath)

        mTextboxConfigDir.Text = mAbsoluteConfigPath

        ' Sets the selection after the last character in the textbox.
        mTextboxConfigDir.SelectionStart = mTextboxConfigDir.Text.Length

        If File.Exists(mAbsoluteConfigPath) Then
            mLabelDirCheck.Text = My.Resources.ConfigFileFound()
            mButtonOpenConfiguration.Enabled = True
        Else
            mLabelDirCheck.Text = My.Resources.ConfigFileNotFound()
        End If
    End Sub

    ' Occurs when a configuration was successfully opened.
    Private Sub ConfigurationOpened()

        Try
            ' Assign system variables from namespaces.            
            Dim CANoeNamespaceGeneral As CANoe.Namespace = mCANoeApp.System.Namespaces("General")
            mCANoeSysVar1 = CANoeNamespaceGeneral.Variables("SysVar1")
            mCANoeSysVar2 = CANoeNamespaceGeneral.Variables("SysVar2")

            ' Assign signals.
            Dim CANoeBus As Bus = mCANoeApp.Bus("CAN")
            mCANoeEngineStatus = CANoeBus.GetSignal(1, "EngineData", "EngineStatus")
            mCANoeEngineSpeed = CANoeBus.GetSignal(1, "EngineData", "EngineSpeed")
            mCANoeEngineTemp = CANoeBus.GetSignal(1, "EngineData", "EngineTemp")
        Catch ex As Exception
            MessageBox.Show("Possible cause: Wrong namespace names, bus name, system variable names or signal names in source code or configuration.",
                            "Error while assigning system variables and signals", MessageBoxButtons.OK, MessageBoxIcon.Hand)

            Return
        End Try

        ' Enables the start/stop measurement button.
        mGroupboxConfigSettings.Enabled = False
        mGroupboxMeasurementControl.Enabled = True

        mButtonStartStop.Focus()

        If (Not mCANoeApp Is Nothing) Then
            ' Wire OnQuit event handler.
            AddHandler mCANoeApp.OnQuit, AddressOf CANoeQuit
        End If

        If (Not mCANoeMeasurement Is Nothing) Then
            ' Create on event handlers.
            AddHandler mCANoeMeasurement.OnInit, AddressOf MeasurementInitiated
            AddHandler mCANoeMeasurement.OnExit, AddressOf MeasurementExited
        End If

        ' Indicate that an instance of CANoe is running.
        mCANoeInstanceRunning = True
    End Sub
#End Region

#Region "Measurement Init/Exit"

    ' Occurs when the measurement has been initiated.
    Private Sub MeasurementInitiated()

        ' Compile CAPL code of the CANoe configuration.
        Dim CANoeCAPL As CANoe.CAPL = mCANoeApp.CAPL
        If (Not CANoeCAPL Is Nothing) Then
            CANoeCAPL.Compile(Nothing)
        End If

        Try
            ' Assign CAPL functions.
            mCANoeMultiply = CANoeCAPL.GetFunction("Multiply")
        Catch ex As Exception
            MessageBox.Show("Possible cause: Wrong CAPL function name in source code or configuration.",
                            "Error while assigning CAPL functions", MessageBoxButtons.OK, MessageBoxIcon.Hand)

            Return
        End Try

        ' Pay attention to thread safety, as the accessed controls were created in the GUI thread!
        Dim safeinvocation As DelSafeInvocation = New DelSafeInvocation(AddressOf MeasurementInitiatedInternal)
        Invoke(safeinvocation)

        ' Create system variable on event handlers.
        If (Not mCANoeSysVar1 Is Nothing) Then
            AddHandler mCANoeSysVar1.OnChange, AddressOf SysVar1Changed
        End If

        If (Not mCANoeSysVar2 Is Nothing) Then
            AddHandler mCANoeSysVar2.OnChange, AddressOf SysVar2Changed
        End If
    End Sub

    ' Used for thread safe call to manipulate GUI controls when measurement has been initiated.
    Private Sub MeasurementInitiatedInternal()

        mGroupboxCAPLWriteWindowOut.Enabled = True
        mGroupboxSystemVariables.Enabled = True
        mGroupboxSignalValues.Enabled = True
        mButtonStartStop.Text = My.Resources.StopMeasurement
        mLabelMeasurementStatus.Text = My.Resources.StatusMeasurementStarted

        ' SysVar1 is chosen by default at startup.
        mProgressbarSysVars.Value = mCANoeSysVar1.Value
        mLabelSysVarsValue.Text = mCANoeSysVar1.Value.ToString
        mLabelSysVarMin.Text = mCANoeSysVar1.MinValue.ToString
        mLabelSysVarMax.Text = mCANoeSysVar1.MaxValue.ToString

        ' Activate polling of signal values.
        mTimerPolling.Interval = mNumericupdownCycleTime.Value
        AddHandler mTimerPolling.Tick, AddressOf PollSignalValues
        mTimerPolling.Start()
    End Sub

    ' Occurs when the measurement has exited.
    Private Sub MeasurementExited()

        ' Pay attention to thread safety, as the accessed controls were created in the GUI thread!
        Dim safeinvocation As DelSafeInvocation = New DelSafeInvocation(AddressOf MeasurementExitedInternal)
        Invoke(safeinvocation)

        ' Unregister system variables on event handlers.
        If (Not mCANoeSysVar1 Is Nothing) Then
            RemoveHandler mCANoeSysVar1.OnChange, AddressOf SysVar1Changed
        End If

        If (Not mCANoeSysVar2 Is Nothing) Then
            RemoveHandler mCANoeSysVar2.OnChange, AddressOf SysVar2Changed
        End If
    End Sub

    ' Used for thread safe call to manipulate GUI controls when measurement has been exited.
    Private Sub MeasurementExitedInternal()

        mGroupboxCAPLWriteWindowOut.Enabled = False
        mGroupboxSystemVariables.Enabled = False
        mGroupboxSignalValues.Enabled = False
        mButtonStartStop.Text = My.Resources.StartMeasurement
        mComboboxSysVars.SelectedIndex = 0
        mComboboxEngineStatusIn.SelectedIndex = 0
        mLabelMeasurementStatus.Text = My.Resources.StatusMeasurementStopped
        mLabelEngTempOut.Text = "0"
        mLabelSysVarsValue.Text = "0"
        mTextboxOperand2.Clear()
        mTextboxEngineSpeedIn.Text = "0"
        mTextboxEngSpeedOut.Text = "0"
        mTextboxEngStatusOut.Text = "Stopped"
        mTextboxOperand2.Clear()
        mTextboxOperand1.Clear()
        mTextboxOperationResult.Clear()
        mProgressbarSysVars.Value = 0
        mProgressbarEngTempOut.Value = 0
        mTrackbarSysVars.Value = 0
        mTrackbarEngineTempIn.Value = 0

        ' Deactivate polling of signal values. 
        mTimerPolling.Stop()
        RemoveHandler mTimerPolling.Tick, AddressOf PollSignalValues
    End Sub
#End Region

#Region "SysVar and Signal values"

    ' Occurs when the polling timer elapses.
    Private Sub PollSignalValues(ByVal sender As Object, ByVal e As EventArgs)

        ' Display the current signal values in the output controls.            
        mTextboxEngSpeedOut.Text = mCANoeEngineSpeed.Value.ToString
        mTextboxEngStatusOut.Text = If((mCANoeEngineStatus.Value = 1), "Running", "Stopped")
        mProgressbarEngTempOut.Value = mCANoeEngineTemp.Value
        mLabelEngTempOut.Text = mCANoeEngineTemp.Value.ToString
    End Sub

    ' Occurs when the value of a particular system variable changes.
    Private Sub SysVar1Changed(ByVal value As Object)

        ' Pay attention to thread safety, as the accessed controls were created in the GUI thread!
        Dim safeinvocationwithparam As DelSafeInvocationWithParam = New DelSafeInvocationWithParam(AddressOf SysVarsChangedInternal)
        Invoke(safeinvocationwithparam, "SysVar1", value)
    End Sub

    ' Occurs when the value of a particular system variable changes.
    Private Sub SysVar2Changed(ByVal value As Object)

        ' Pay attention to thread safety, as the accessed controls were created in the GUI thread!
        Dim safeinvocationwithparam As DelSafeInvocationWithParam = New DelSafeInvocationWithParam(AddressOf SysVarsChangedInternal)
        Invoke(safeinvocationwithparam, "SysVar2", value)
    End Sub

    ' Used for thread safe call to manipulate GUI controls when the value of a system variable has changed.
    Private Sub SysVarsChangedInternal(sysvarName As String, value As Object)

        If (((Not mCANoeMeasurement Is Nothing) AndAlso mCANoeMeasurement.Running)) Then

            ' Set new value of the currently selected system variable.
            If (mComboboxSysVars.Text.Equals(sysvarName)) Then
                mProgressbarSysVars.Value = value
                mLabelSysVarsValue.Text = value.ToString
            End If
        End If
    End Sub
#End Region

#Region "Closing Tasks"

    ' Occurs when CANoe has quit.
    Private Sub CANoeQuit()

        ' Pay attention to thread safety, as the accessed controls were created in the GUI thread!
        Dim safeinvocation As DelSafeInvocation = New DelSafeInvocation(AddressOf CANoeQuitInternal)
        Invoke(safeinvocation)

        UnregisterCANoeEventHandlers()

        ' Indicate that the instance of CANoe was closed.
        mCANoeInstanceRunning = False
    End Sub

    ' Used for thread safe call to manipulate GUI controls when CANoe has quit.
    Private Sub CANoeQuitInternal()

        ' Disables the start/stop measurement button.
        mGroupboxMeasurementControl.Enabled = False

        ' Set the correct caption of the start/stop button.
        mButtonStartStop.Text = My.Resources.StartMeasurement

        ' Set the correct caption of the status label.
        mLabelMeasurementStatus.Text = My.Resources.StatusMeasurementStopped

        ' Disable the measurement related controls.
        MeasurementExited()
    End Sub

    ' Releases all wired event handlers.
    Private Sub UnregisterCANoeEventHandlers()

        If (Not mCANoeApp Is Nothing) Then
            RemoveHandler mCANoeApp.OnQuit, AddressOf CANoeQuit
        End If

        If (Not mCANoeMeasurement Is Nothing) Then
            RemoveHandler mCANoeMeasurement.OnInit, AddressOf MeasurementInitiated
            RemoveHandler mCANoeMeasurement.OnExit, AddressOf MeasurementExited
        End If
    End Sub
#End Region

#Region ".Net Control EventHandlers"

    ' Occurs when the user clicks the button to open the configuration.
    Private Sub mButtonOpenConfiguration_Click(sender As System.Object, e As System.EventArgs) Handles mButtonOpenConfiguration.Click

        ' Init new CANoe application.
        mCANoeApp = New CANoe.Application

        ' Init measurement object.
        mCANoeMeasurement = mCANoeApp.Measurement

        ' Stopps a running measurement.
        If mCANoeMeasurement.Running Then
            mCANoeMeasurement.Stop()
        End If

        If (Not mCANoeApp Is Nothing) Then

            ' Open the demo configuration.
            mCANoeApp.Open(mAbsoluteConfigPath, True, True)

            ' Make sure the configuration was successfully loaded.
            Dim ocresult As CANoe.OpenConfigurationResult = mCANoeApp.Configuration.OpenConfigurationResult
            If (ocresult.result = 0) Then
                ConfigurationOpened()
            End If
        End If
    End Sub

    ' Occurs when the user clicks the button to start or stop a measurement.
    Private Sub mButtonStartStop_Click(sender As System.Object, e As System.EventArgs) Handles mButtonStartStop.Click

        If (Not mCANoeMeasurement Is Nothing) Then
            If mCANoeMeasurement.Running Then
                mCANoeMeasurement.Stop()
            Else
                mCANoeMeasurement.Start()
            End If
        End If
    End Sub

    ' Occurs when the user wants to calculate the multiplication result.
    Private Sub mButtonCalculate_Click(sender As System.Object, e As System.EventArgs) Handles mButtonCalculate.Click

        ' Call "Multiply" CAPL function and pass parameters, also get return value.
        If (Not mCANoeMultiply Is Nothing) Then

            Dim operand1 As Integer
            Dim operand2 As Integer
            Dim result As Integer

            Integer.TryParse(mTextboxOperand1.Text, operand1)
            Integer.TryParse(mTextboxOperand2.Text, operand2)

            ' Return from a CAPL function works in evaluation branch of measurement set up only.
            result = mCANoeMultiply.Call(operand1, operand2)

            mTextboxOperationResult.Text = result.ToString
        End If
    End Sub

    ' Occurs when the mouse pointer hovers over the textbox.
    Private Sub mTextboxConfigDir_MouseHover(sender As System.Object, e As System.EventArgs) Handles mTextboxConfigDir.MouseHover

        ' Show tooltip with the absolute configuration path.
        mConfigDirToolTip.Show(mAbsoluteConfigPath, mTextboxConfigDir)
    End Sub

    ' Occurs when the textbox lost focus.
    Private Sub mTextboxEngineSpeedIn_Leave(sender As System.Object, e As System.EventArgs) Handles mTextboxEngineSpeedIn.Leave

        ' Set new value of signal EngineSpeed.
        Dim newEngineSpeed As Double
        If Double.TryParse(mTextboxEngineSpeedIn.Text, newEngineSpeed) Then
            mCANoeEngineSpeed.Value = newEngineSpeed
        End If
    End Sub

    ' Occurs when a key was pressed.
    Private Sub mTextboxEngineSpeedIn_KeyPress(sender As System.Object, e As System.Windows.Forms.KeyPressEventArgs) Handles mTextboxEngineSpeedIn.KeyPress

        If (e.KeyChar = Chr(Keys.Return)) Then

            ' Set new value of signal EngineSpeed.
            Dim newEngineSpeed As Double
            If Double.TryParse(mTextboxEngineSpeedIn.Text, newEngineSpeed) Then

                mCANoeEngineSpeed.Value = newEngineSpeed
            End If
        End If
    End Sub

    ' Occurs when the selection of the combobox has changed.
    Private Sub mComboboxSysVars_SelectedIndexChanged(sender As System.Object, e As System.EventArgs) Handles mComboboxSysVars.SelectedIndexChanged

        ' Sets the content of controls depending on the selected entry.
        Dim selectedEntry As String = mComboboxSysVars.Text

        If ((Not mCANoeSysVar1 Is Nothing) AndAlso (Not mCANoeSysVar2 Is Nothing)) Then
            If (selectedEntry = "SysVar1") Then
                mProgressbarSysVars.Value = mCANoeSysVar1.Value
                mLabelSysVarsValue.Text = mCANoeSysVar1.Value.ToString
                mLabelSysVarMin.Text = mCANoeSysVar1.MinValue.ToString
                mLabelSysVarMax.Text = mCANoeSysVar1.MaxValue.ToString
            ElseIf (selectedEntry = "SysVar2") Then
                mProgressbarSysVars.Value = mCANoeSysVar2.Value
                mLabelSysVarsValue.Text = mCANoeSysVar2.Value.ToString
                mLabelSysVarMin.Text = mCANoeSysVar2.MinValue.ToString
                mLabelSysVarMax.Text = mCANoeSysVar2.MaxValue.ToString
            End If
        End If

        mTrackbarSysVars.Value = 0
    End Sub

    ' Occurs when the selection of the combobox has changed.
    Private Sub mComboboxEngineStatusIn_SelectedIndexChanged(sender As System.Object, e As System.EventArgs) Handles mComboboxEngineStatusIn.SelectedIndexChanged

        Dim selectedEntry As String = mComboboxEngineStatusIn.Text

        If (Not mCANoeEngineStatus Is Nothing) Then
            ' Set new value of signal EngineStatus.
            If (selectedEntry = "Running") Then
                mCANoeEngineStatus.Value = 1
            ElseIf (selectedEntry = "Stopped") Then
                mCANoeEngineStatus.Value = 0
            End If
        End If
    End Sub

    ' Occurs when the slider of the trackbar is scrolled.
    Private Sub mTrackbarSysVars_Scroll(sender As System.Object, e As System.EventArgs) Handles mTrackbarSysVars.Scroll

        Dim selectedEntry As String = mComboboxSysVars.Text

        ' Communicates new SysVarX value to CANoe.
        If (selectedEntry = "SysVar1") Then
            mCANoeSysVar1.Value = mTrackbarSysVars.Value
        ElseIf (selectedEntry = "SysVar2") Then
            mCANoeSysVar2.Value = mTrackbarSysVars.Value
        End If
    End Sub

    ' Occurs when the slider of the trackbar is scrolled.
    Private Sub mTrackbarEngineTempIn_Scroll(sender As System.Object, e As System.EventArgs) Handles mTrackbarEngineTempIn.Scroll

        ' Set new value of signal EngineTemp.
        mCANoeEngineTemp.Value = mTrackbarEngineTempIn.Value
    End Sub

    ' Occurs when the update cycle time is intended to be changed.
    Private Sub mNumericupdownCycleTime_ValueChanged(sender As System.Object, e As System.EventArgs) Handles mNumericupdownCycleTime.ValueChanged

        ' Set a new update cycle time.
        mTimerPolling.Interval = mNumericupdownCycleTime.Value
    End Sub

    ' Occurs when the form is closing.
    Private Sub Form_VBNET_CANoe_FormClosing(sender As System.Object, e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing

        UnregisterCANoeEventHandlers()

        ' If an instance of CANoe is running and a measurement is running assure that it is stopped before the demo closes.
        If (mCANoeInstanceRunning AndAlso (Not mCANoeMeasurement Is Nothing) AndAlso mCANoeMeasurement.Running) Then
            mCANoeMeasurement.Stop()
        End If
    End Sub
#End Region
End Class
