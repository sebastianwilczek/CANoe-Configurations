<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form_VBNET_CANoe
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Form_VBNET_CANoe))
        Me.mGroupboxConfigSettings = New System.Windows.Forms.GroupBox()
        Me.mLabelInfoConfig = New System.Windows.Forms.Label()
        Me.mButtonOpenConfiguration = New System.Windows.Forms.Button()
        Me.mLabelDirCheck = New System.Windows.Forms.Label()
        Me.mTextboxConfigDir = New System.Windows.Forms.TextBox()
        Me.mLabelEngTempOut = New System.Windows.Forms.Label()
        Me.mProgressbarEngTempOut = New System.Windows.Forms.ProgressBar()
        Me.mGroupboxSignalValues = New System.Windows.Forms.GroupBox()
        Me.mTextboxEngStatusOut = New System.Windows.Forms.TextBox()
        Me.mTextboxEngSpeedOut = New System.Windows.Forms.TextBox()
        Me.mLabelmsCycle = New System.Windows.Forms.Label()
        Me.mNumericupdownCycleTime = New System.Windows.Forms.NumericUpDown()
        Me.mLabelCycleTime = New System.Windows.Forms.Label()
        Me.mLabelEngTempMax = New System.Windows.Forms.Label()
        Me.mLabelEngTempMin = New System.Windows.Forms.Label()
        Me.mComboboxEngineStatusIn = New System.Windows.Forms.ComboBox()
        Me.mTrackbarEngineTempIn = New System.Windows.Forms.TrackBar()
        Me.mTextboxEngineSpeedIn = New System.Windows.Forms.TextBox()
        Me.mLabelEngineStatus = New System.Windows.Forms.Label()
        Me.mLabelEngineTemp = New System.Windows.Forms.Label()
        Me.mLabelEngineSpeed = New System.Windows.Forms.Label()
        Me.mLabelInfoSignalValues = New System.Windows.Forms.Label()
        Me.mGroupboxSystemVariables = New System.Windows.Forms.GroupBox()
        Me.mLabelSysVarsValue = New System.Windows.Forms.Label()
        Me.mLabelSysVarMax = New System.Windows.Forms.Label()
        Me.mComboboxSysVars = New System.Windows.Forms.ComboBox()
        Me.mProgressbarSysVars = New System.Windows.Forms.ProgressBar()
        Me.mLabelSysVarMin = New System.Windows.Forms.Label()
        Me.mLabelInfoSysVars = New System.Windows.Forms.Label()
        Me.mTrackbarSysVars = New System.Windows.Forms.TrackBar()
        Me.mTextboxOperand2 = New System.Windows.Forms.TextBox()
        Me.mButtonCalculate = New System.Windows.Forms.Button()
        Me.mLabelMeasurementStatus = New System.Windows.Forms.Label()
        Me.mButtonStartStop = New System.Windows.Forms.Button()
        Me.mGroupboxMeasurementControl = New System.Windows.Forms.GroupBox()
        Me.mGroupboxCAPLWriteWindowOut = New System.Windows.Forms.GroupBox()
        Me.mTextboxOperationResult = New System.Windows.Forms.TextBox()
        Me.mTextboxOperand1 = New System.Windows.Forms.TextBox()
        Me.mLabelEquals = New System.Windows.Forms.Label()
        Me.mLabelMultiply = New System.Windows.Forms.Label()
        Me.mLabelInfoCAPL = New System.Windows.Forms.Label()
        Me.mTimerPolling = New System.Windows.Forms.Timer(Me.components)
        Me.mGroupboxConfigSettings.SuspendLayout()
        Me.mGroupboxSignalValues.SuspendLayout()
        CType(Me.mNumericupdownCycleTime, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.mTrackbarEngineTempIn, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.mGroupboxSystemVariables.SuspendLayout()
        CType(Me.mTrackbarSysVars, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.mGroupboxMeasurementControl.SuspendLayout()
        Me.mGroupboxCAPLWriteWindowOut.SuspendLayout()
        Me.SuspendLayout()
        '
        'mGroupboxConfigSettings
        '
        Me.mGroupboxConfigSettings.Controls.Add(Me.mLabelInfoConfig)
        Me.mGroupboxConfigSettings.Controls.Add(Me.mButtonOpenConfiguration)
        Me.mGroupboxConfigSettings.Controls.Add(Me.mLabelDirCheck)
        Me.mGroupboxConfigSettings.Controls.Add(Me.mTextboxConfigDir)
        Me.mGroupboxConfigSettings.ForeColor = System.Drawing.SystemColors.HotTrack
        Me.mGroupboxConfigSettings.Location = New System.Drawing.Point(10, 7)
        Me.mGroupboxConfigSettings.Name = "mGroupboxConfigSettings"
        Me.mGroupboxConfigSettings.Size = New System.Drawing.Size(464, 119)
        Me.mGroupboxConfigSettings.TabIndex = 5
        Me.mGroupboxConfigSettings.TabStop = False
        Me.mGroupboxConfigSettings.Text = "Configuration Settings"
        '
        'mLabelInfoConfig
        '
        Me.mLabelInfoConfig.AutoSize = True
        Me.mLabelInfoConfig.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelInfoConfig.Location = New System.Drawing.Point(6, 23)
        Me.mLabelInfoConfig.Name = "mLabelInfoConfig"
        Me.mLabelInfoConfig.Size = New System.Drawing.Size(445, 13)
        Me.mLabelInfoConfig.TabIndex = 4
        Me.mLabelInfoConfig.Text = "Please make sure the provided demo configuration is located in the directory spec" & _
    "ified below!"
        '
        'mButtonOpenConfiguration
        '
        Me.mButtonOpenConfiguration.Enabled = False
        Me.mButtonOpenConfiguration.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mButtonOpenConfiguration.Location = New System.Drawing.Point(286, 82)
        Me.mButtonOpenConfiguration.Name = "mButtonOpenConfiguration"
        Me.mButtonOpenConfiguration.Size = New System.Drawing.Size(170, 23)
        Me.mButtonOpenConfiguration.TabIndex = 0
        Me.mButtonOpenConfiguration.Text = "Open CANoe configuration"
        Me.mButtonOpenConfiguration.UseVisualStyleBackColor = True
        '
        'mLabelDirCheck
        '
        Me.mLabelDirCheck.AutoSize = True
        Me.mLabelDirCheck.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelDirCheck.Location = New System.Drawing.Point(6, 82)
        Me.mLabelDirCheck.Name = "mLabelDirCheck"
        Me.mLabelDirCheck.Size = New System.Drawing.Size(221, 26)
        Me.mLabelDirCheck.TabIndex = 2
        Me.mLabelDirCheck.Text = "Configuration file found." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Click to open the configuration within CANoe."
        '
        'mTextboxConfigDir
        '
        Me.mTextboxConfigDir.Location = New System.Drawing.Point(9, 44)
        Me.mTextboxConfigDir.Name = "mTextboxConfigDir"
        Me.mTextboxConfigDir.ReadOnly = True
        Me.mTextboxConfigDir.Size = New System.Drawing.Size(447, 20)
        Me.mTextboxConfigDir.TabIndex = 1
        '
        'mLabelEngTempOut
        '
        Me.mLabelEngTempOut.AutoSize = True
        Me.mLabelEngTempOut.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelEngTempOut.Location = New System.Drawing.Point(283, 182)
        Me.mLabelEngTempOut.Name = "mLabelEngTempOut"
        Me.mLabelEngTempOut.Size = New System.Drawing.Size(13, 13)
        Me.mLabelEngTempOut.TabIndex = 7
        Me.mLabelEngTempOut.Text = "0"
        '
        'mProgressbarEngTempOut
        '
        Me.mProgressbarEngTempOut.Location = New System.Drawing.Point(160, 177)
        Me.mProgressbarEngTempOut.Name = "mProgressbarEngTempOut"
        Me.mProgressbarEngTempOut.Size = New System.Drawing.Size(120, 22)
        Me.mProgressbarEngTempOut.TabIndex = 36
        '
        'mGroupboxSignalValues
        '
        Me.mGroupboxSignalValues.Controls.Add(Me.mLabelEngTempOut)
        Me.mGroupboxSignalValues.Controls.Add(Me.mProgressbarEngTempOut)
        Me.mGroupboxSignalValues.Controls.Add(Me.mTextboxEngStatusOut)
        Me.mGroupboxSignalValues.Controls.Add(Me.mTextboxEngSpeedOut)
        Me.mGroupboxSignalValues.Controls.Add(Me.mLabelmsCycle)
        Me.mGroupboxSignalValues.Controls.Add(Me.mNumericupdownCycleTime)
        Me.mGroupboxSignalValues.Controls.Add(Me.mLabelCycleTime)
        Me.mGroupboxSignalValues.Controls.Add(Me.mLabelEngTempMax)
        Me.mGroupboxSignalValues.Controls.Add(Me.mLabelEngTempMin)
        Me.mGroupboxSignalValues.Controls.Add(Me.mComboboxEngineStatusIn)
        Me.mGroupboxSignalValues.Controls.Add(Me.mTrackbarEngineTempIn)
        Me.mGroupboxSignalValues.Controls.Add(Me.mTextboxEngineSpeedIn)
        Me.mGroupboxSignalValues.Controls.Add(Me.mLabelEngineStatus)
        Me.mGroupboxSignalValues.Controls.Add(Me.mLabelEngineTemp)
        Me.mGroupboxSignalValues.Controls.Add(Me.mLabelEngineSpeed)
        Me.mGroupboxSignalValues.Controls.Add(Me.mLabelInfoSignalValues)
        Me.mGroupboxSignalValues.Enabled = False
        Me.mGroupboxSignalValues.ForeColor = System.Drawing.SystemColors.HotTrack
        Me.mGroupboxSignalValues.Location = New System.Drawing.Point(10, 429)
        Me.mGroupboxSignalValues.Name = "mGroupboxSignalValues"
        Me.mGroupboxSignalValues.Size = New System.Drawing.Size(464, 228)
        Me.mGroupboxSignalValues.TabIndex = 8
        Me.mGroupboxSignalValues.TabStop = False
        Me.mGroupboxSignalValues.Text = "Signal Values"
        '
        'mTextboxEngStatusOut
        '
        Me.mTextboxEngStatusOut.Location = New System.Drawing.Point(160, 147)
        Me.mTextboxEngStatusOut.Name = "mTextboxEngStatusOut"
        Me.mTextboxEngStatusOut.ReadOnly = True
        Me.mTextboxEngStatusOut.Size = New System.Drawing.Size(120, 20)
        Me.mTextboxEngStatusOut.TabIndex = 35
        Me.mTextboxEngStatusOut.Text = "Stopped"
        Me.mTextboxEngStatusOut.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'mTextboxEngSpeedOut
        '
        Me.mTextboxEngSpeedOut.Location = New System.Drawing.Point(160, 117)
        Me.mTextboxEngSpeedOut.Name = "mTextboxEngSpeedOut"
        Me.mTextboxEngSpeedOut.ReadOnly = True
        Me.mTextboxEngSpeedOut.Size = New System.Drawing.Size(120, 20)
        Me.mTextboxEngSpeedOut.TabIndex = 32
        Me.mTextboxEngSpeedOut.Text = "0"
        Me.mTextboxEngSpeedOut.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'mLabelmsCycle
        '
        Me.mLabelmsCycle.AutoSize = True
        Me.mLabelmsCycle.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelmsCycle.Location = New System.Drawing.Point(221, 78)
        Me.mLabelmsCycle.Name = "mLabelmsCycle"
        Me.mLabelmsCycle.Size = New System.Drawing.Size(20, 13)
        Me.mLabelmsCycle.TabIndex = 31
        Me.mLabelmsCycle.Text = "ms"
        '
        'mNumericupdownCycleTime
        '
        Me.mNumericupdownCycleTime.Increment = New Decimal(New Integer() {100, 0, 0, 0})
        Me.mNumericupdownCycleTime.Location = New System.Drawing.Point(160, 76)
        Me.mNumericupdownCycleTime.Maximum = New Decimal(New Integer() {10000, 0, 0, 0})
        Me.mNumericupdownCycleTime.Minimum = New Decimal(New Integer() {100, 0, 0, 0})
        Me.mNumericupdownCycleTime.Name = "mNumericupdownCycleTime"
        Me.mNumericupdownCycleTime.Size = New System.Drawing.Size(55, 20)
        Me.mNumericupdownCycleTime.TabIndex = 8
        Me.mNumericupdownCycleTime.Value = New Decimal(New Integer() {100, 0, 0, 0})
        '
        'mLabelCycleTime
        '
        Me.mLabelCycleTime.AutoSize = True
        Me.mLabelCycleTime.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelCycleTime.Location = New System.Drawing.Point(6, 78)
        Me.mLabelCycleTime.Name = "mLabelCycleTime"
        Me.mLabelCycleTime.Size = New System.Drawing.Size(95, 13)
        Me.mLabelCycleTime.TabIndex = 29
        Me.mLabelCycleTime.Text = "Update cycle time:" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10)
        '
        'mLabelEngTempMax
        '
        Me.mLabelEngTempMax.AutoSize = True
        Me.mLabelEngTempMax.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelEngTempMax.Location = New System.Drawing.Point(435, 204)
        Me.mLabelEngTempMax.Name = "mLabelEngTempMax"
        Me.mLabelEngTempMax.Size = New System.Drawing.Size(25, 13)
        Me.mLabelEngTempMax.TabIndex = 27
        Me.mLabelEngTempMax.Text = "100"
        '
        'mLabelEngTempMin
        '
        Me.mLabelEngTempMin.AutoSize = True
        Me.mLabelEngTempMin.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelEngTempMin.Location = New System.Drawing.Point(337, 204)
        Me.mLabelEngTempMin.Name = "mLabelEngTempMin"
        Me.mLabelEngTempMin.Size = New System.Drawing.Size(13, 13)
        Me.mLabelEngTempMin.TabIndex = 7
        Me.mLabelEngTempMin.Text = "0"
        '
        'mComboboxEngineStatusIn
        '
        Me.mComboboxEngineStatusIn.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems
        Me.mComboboxEngineStatusIn.FormattingEnabled = True
        Me.mComboboxEngineStatusIn.Items.AddRange(New Object() {"Stopped", "Running"})
        Me.mComboboxEngineStatusIn.Location = New System.Drawing.Point(338, 147)
        Me.mComboboxEngineStatusIn.Name = "mComboboxEngineStatusIn"
        Me.mComboboxEngineStatusIn.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.mComboboxEngineStatusIn.Size = New System.Drawing.Size(118, 21)
        Me.mComboboxEngineStatusIn.TabIndex = 10
        Me.mComboboxEngineStatusIn.Text = "Stopped"
        '
        'mTrackbarEngineTempIn
        '
        Me.mTrackbarEngineTempIn.LargeChange = 10
        Me.mTrackbarEngineTempIn.Location = New System.Drawing.Point(331, 172)
        Me.mTrackbarEngineTempIn.Maximum = 100
        Me.mTrackbarEngineTempIn.Name = "mTrackbarEngineTempIn"
        Me.mTrackbarEngineTempIn.Size = New System.Drawing.Size(132, 45)
        Me.mTrackbarEngineTempIn.TabIndex = 11
        Me.mTrackbarEngineTempIn.TickFrequency = 10
        '
        'mTextboxEngineSpeedIn
        '
        Me.mTextboxEngineSpeedIn.Location = New System.Drawing.Point(338, 117)
        Me.mTextboxEngineSpeedIn.Name = "mTextboxEngineSpeedIn"
        Me.mTextboxEngineSpeedIn.Size = New System.Drawing.Size(118, 20)
        Me.mTextboxEngineSpeedIn.TabIndex = 9
        Me.mTextboxEngineSpeedIn.Text = "0"
        Me.mTextboxEngineSpeedIn.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'mLabelEngineStatus
        '
        Me.mLabelEngineStatus.AutoSize = True
        Me.mLabelEngineStatus.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelEngineStatus.Location = New System.Drawing.Point(6, 150)
        Me.mLabelEngineStatus.Name = "mLabelEngineStatus"
        Me.mLabelEngineStatus.Size = New System.Drawing.Size(70, 13)
        Me.mLabelEngineStatus.TabIndex = 14
        Me.mLabelEngineStatus.Text = "EngineStatus"
        '
        'mLabelEngineTemp
        '
        Me.mLabelEngineTemp.AutoSize = True
        Me.mLabelEngineTemp.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelEngineTemp.Location = New System.Drawing.Point(6, 180)
        Me.mLabelEngineTemp.Name = "mLabelEngineTemp"
        Me.mLabelEngineTemp.Size = New System.Drawing.Size(101, 13)
        Me.mLabelEngineTemp.TabIndex = 12
        Me.mLabelEngineTemp.Text = "EngineTemp (degC)"
        '
        'mLabelEngineSpeed
        '
        Me.mLabelEngineSpeed.AutoSize = True
        Me.mLabelEngineSpeed.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelEngineSpeed.Location = New System.Drawing.Point(6, 120)
        Me.mLabelEngineSpeed.Name = "mLabelEngineSpeed"
        Me.mLabelEngineSpeed.Size = New System.Drawing.Size(104, 13)
        Me.mLabelEngineSpeed.TabIndex = 11
        Me.mLabelEngineSpeed.Text = "EngineSpeed (RPM)"
        '
        'mLabelInfoSignalValues
        '
        Me.mLabelInfoSignalValues.AutoSize = True
        Me.mLabelInfoSignalValues.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelInfoSignalValues.Location = New System.Drawing.Point(6, 23)
        Me.mLabelInfoSignalValues.Name = "mLabelInfoSignalValues"
        Me.mLabelInfoSignalValues.Size = New System.Drawing.Size(449, 39)
        Me.mLabelInfoSignalValues.TabIndex = 0
        Me.mLabelInfoSignalValues.Text = resources.GetString("mLabelInfoSignalValues.Text")
        '
        'mGroupboxSystemVariables
        '
        Me.mGroupboxSystemVariables.Controls.Add(Me.mLabelSysVarsValue)
        Me.mGroupboxSystemVariables.Controls.Add(Me.mLabelSysVarMax)
        Me.mGroupboxSystemVariables.Controls.Add(Me.mComboboxSysVars)
        Me.mGroupboxSystemVariables.Controls.Add(Me.mProgressbarSysVars)
        Me.mGroupboxSystemVariables.Controls.Add(Me.mLabelSysVarMin)
        Me.mGroupboxSystemVariables.Controls.Add(Me.mLabelInfoSysVars)
        Me.mGroupboxSystemVariables.Controls.Add(Me.mTrackbarSysVars)
        Me.mGroupboxSystemVariables.Enabled = False
        Me.mGroupboxSystemVariables.ForeColor = System.Drawing.SystemColors.HotTrack
        Me.mGroupboxSystemVariables.Location = New System.Drawing.Point(10, 312)
        Me.mGroupboxSystemVariables.Name = "mGroupboxSystemVariables"
        Me.mGroupboxSystemVariables.Size = New System.Drawing.Size(464, 108)
        Me.mGroupboxSystemVariables.TabIndex = 9
        Me.mGroupboxSystemVariables.TabStop = False
        Me.mGroupboxSystemVariables.Text = "System Variables"
        '
        'mLabelSysVarsValue
        '
        Me.mLabelSysVarsValue.AutoSize = True
        Me.mLabelSysVarsValue.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelSysVarsValue.Location = New System.Drawing.Point(283, 73)
        Me.mLabelSysVarsValue.Name = "mLabelSysVarsValue"
        Me.mLabelSysVarsValue.Size = New System.Drawing.Size(13, 13)
        Me.mLabelSysVarsValue.TabIndex = 6
        Me.mLabelSysVarsValue.Text = "0"
        '
        'mLabelSysVarMax
        '
        Me.mLabelSysVarMax.AutoSize = True
        Me.mLabelSysVarMax.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelSysVarMax.Location = New System.Drawing.Point(435, 89)
        Me.mLabelSysVarMax.Name = "mLabelSysVarMax"
        Me.mLabelSysVarMax.Size = New System.Drawing.Size(25, 13)
        Me.mLabelSysVarMax.TabIndex = 5
        Me.mLabelSysVarMax.Text = "255"
        '
        'mComboboxSysVars
        '
        Me.mComboboxSysVars.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems
        Me.mComboboxSysVars.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mComboboxSysVars.FormattingEnabled = True
        Me.mComboboxSysVars.Items.AddRange(New Object() {"SysVar1", "SysVar2"})
        Me.mComboboxSysVars.Location = New System.Drawing.Point(9, 69)
        Me.mComboboxSysVars.Name = "mComboboxSysVars"
        Me.mComboboxSysVars.Size = New System.Drawing.Size(104, 21)
        Me.mComboboxSysVars.TabIndex = 6
        Me.mComboboxSysVars.Text = "SysVar1"
        '
        'mProgressbarSysVars
        '
        Me.mProgressbarSysVars.Location = New System.Drawing.Point(160, 68)
        Me.mProgressbarSysVars.Maximum = 255
        Me.mProgressbarSysVars.Name = "mProgressbarSysVars"
        Me.mProgressbarSysVars.Size = New System.Drawing.Size(120, 22)
        Me.mProgressbarSysVars.Step = 15
        Me.mProgressbarSysVars.TabIndex = 2
        '
        'mLabelSysVarMin
        '
        Me.mLabelSysVarMin.AutoSize = True
        Me.mLabelSysVarMin.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelSysVarMin.Location = New System.Drawing.Point(337, 89)
        Me.mLabelSysVarMin.Name = "mLabelSysVarMin"
        Me.mLabelSysVarMin.Size = New System.Drawing.Size(13, 13)
        Me.mLabelSysVarMin.TabIndex = 4
        Me.mLabelSysVarMin.Text = "0"
        '
        'mLabelInfoSysVars
        '
        Me.mLabelInfoSysVars.AutoSize = True
        Me.mLabelInfoSysVars.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelInfoSysVars.Location = New System.Drawing.Point(6, 23)
        Me.mLabelInfoSysVars.Name = "mLabelInfoSysVars"
        Me.mLabelInfoSysVars.Size = New System.Drawing.Size(452, 26)
        Me.mLabelInfoSysVars.TabIndex = 0
        Me.mLabelInfoSysVars.Text = "Choose a system variable and read its current value out of the progress bar. Writ" & _
    "e a new value" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "by using the slider. Live update of the system variable value is " & _
    "provided by COM server events."
        '
        'mTrackbarSysVars
        '
        Me.mTrackbarSysVars.LargeChange = 15
        Me.mTrackbarSysVars.Location = New System.Drawing.Point(331, 57)
        Me.mTrackbarSysVars.Maximum = 255
        Me.mTrackbarSysVars.Name = "mTrackbarSysVars"
        Me.mTrackbarSysVars.Size = New System.Drawing.Size(132, 45)
        Me.mTrackbarSysVars.SmallChange = 5
        Me.mTrackbarSysVars.TabIndex = 7
        Me.mTrackbarSysVars.TickFrequency = 15
        '
        'mTextboxOperand2
        '
        Me.mTextboxOperand2.Location = New System.Drawing.Point(121, 63)
        Me.mTextboxOperand2.Name = "mTextboxOperand2"
        Me.mTextboxOperand2.Size = New System.Drawing.Size(78, 20)
        Me.mTextboxOperand2.TabIndex = 1
        Me.mTextboxOperand2.Text = "25"
        Me.mTextboxOperand2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'mButtonCalculate
        '
        Me.mButtonCalculate.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mButtonCalculate.Location = New System.Drawing.Point(366, 60)
        Me.mButtonCalculate.Name = "mButtonCalculate"
        Me.mButtonCalculate.Size = New System.Drawing.Size(89, 23)
        Me.mButtonCalculate.TabIndex = 2
        Me.mButtonCalculate.Text = "Calculate"
        Me.mButtonCalculate.UseVisualStyleBackColor = True
        '
        'mLabelMeasurementStatus
        '
        Me.mLabelMeasurementStatus.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelMeasurementStatus.Location = New System.Drawing.Point(202, 30)
        Me.mLabelMeasurementStatus.Name = "mLabelMeasurementStatus"
        Me.mLabelMeasurementStatus.RightToLeft = System.Windows.Forms.RightToLeft.Yes
        Me.mLabelMeasurementStatus.Size = New System.Drawing.Size(257, 18)
        Me.mLabelMeasurementStatus.TabIndex = 2
        Me.mLabelMeasurementStatus.Text = "Status: No measurement is currently running"
        '
        'mButtonStartStop
        '
        Me.mButtonStartStop.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mButtonStartStop.Location = New System.Drawing.Point(9, 25)
        Me.mButtonStartStop.Name = "mButtonStartStop"
        Me.mButtonStartStop.Size = New System.Drawing.Size(171, 23)
        Me.mButtonStartStop.TabIndex = 1
        Me.mButtonStartStop.Text = "Start measurement"
        Me.mButtonStartStop.UseVisualStyleBackColor = True
        '
        'mGroupboxMeasurementControl
        '
        Me.mGroupboxMeasurementControl.Controls.Add(Me.mLabelMeasurementStatus)
        Me.mGroupboxMeasurementControl.Controls.Add(Me.mButtonStartStop)
        Me.mGroupboxMeasurementControl.Enabled = False
        Me.mGroupboxMeasurementControl.ForeColor = System.Drawing.SystemColors.HotTrack
        Me.mGroupboxMeasurementControl.Location = New System.Drawing.Point(10, 135)
        Me.mGroupboxMeasurementControl.Name = "mGroupboxMeasurementControl"
        Me.mGroupboxMeasurementControl.Size = New System.Drawing.Size(464, 63)
        Me.mGroupboxMeasurementControl.TabIndex = 6
        Me.mGroupboxMeasurementControl.TabStop = False
        Me.mGroupboxMeasurementControl.Text = "Measurement Control"
        '
        'mGroupboxCAPLWriteWindowOut
        '
        Me.mGroupboxCAPLWriteWindowOut.Controls.Add(Me.mTextboxOperationResult)
        Me.mGroupboxCAPLWriteWindowOut.Controls.Add(Me.mTextboxOperand1)
        Me.mGroupboxCAPLWriteWindowOut.Controls.Add(Me.mLabelEquals)
        Me.mGroupboxCAPLWriteWindowOut.Controls.Add(Me.mLabelMultiply)
        Me.mGroupboxCAPLWriteWindowOut.Controls.Add(Me.mLabelInfoCAPL)
        Me.mGroupboxCAPLWriteWindowOut.Controls.Add(Me.mButtonCalculate)
        Me.mGroupboxCAPLWriteWindowOut.Controls.Add(Me.mTextboxOperand2)
        Me.mGroupboxCAPLWriteWindowOut.Enabled = False
        Me.mGroupboxCAPLWriteWindowOut.ForeColor = System.Drawing.SystemColors.HotTrack
        Me.mGroupboxCAPLWriteWindowOut.Location = New System.Drawing.Point(10, 208)
        Me.mGroupboxCAPLWriteWindowOut.Name = "mGroupboxCAPLWriteWindowOut"
        Me.mGroupboxCAPLWriteWindowOut.Size = New System.Drawing.Size(464, 94)
        Me.mGroupboxCAPLWriteWindowOut.TabIndex = 7
        Me.mGroupboxCAPLWriteWindowOut.TabStop = False
        Me.mGroupboxCAPLWriteWindowOut.Text = "CAPL Function"
        '
        'mTextboxOperationResult
        '
        Me.mTextboxOperationResult.Location = New System.Drawing.Point(242, 62)
        Me.mTextboxOperationResult.Name = "mTextboxOperationResult"
        Me.mTextboxOperationResult.ReadOnly = True
        Me.mTextboxOperationResult.Size = New System.Drawing.Size(78, 20)
        Me.mTextboxOperationResult.TabIndex = 3
        Me.mTextboxOperationResult.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'mTextboxOperand1
        '
        Me.mTextboxOperand1.Location = New System.Drawing.Point(9, 62)
        Me.mTextboxOperand1.Name = "mTextboxOperand1"
        Me.mTextboxOperand1.Size = New System.Drawing.Size(78, 20)
        Me.mTextboxOperand1.TabIndex = 0
        Me.mTextboxOperand1.Text = "50"
        Me.mTextboxOperand1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'mLabelEquals
        '
        Me.mLabelEquals.AutoSize = True
        Me.mLabelEquals.ForeColor = System.Drawing.SystemColors.Desktop
        Me.mLabelEquals.Location = New System.Drawing.Point(213, 66)
        Me.mLabelEquals.Name = "mLabelEquals"
        Me.mLabelEquals.Size = New System.Drawing.Size(13, 13)
        Me.mLabelEquals.TabIndex = 8
        Me.mLabelEquals.Text = "="
        '
        'mLabelMultiply
        '
        Me.mLabelMultiply.AutoSize = True
        Me.mLabelMultiply.ForeColor = System.Drawing.SystemColors.Desktop
        Me.mLabelMultiply.Location = New System.Drawing.Point(97, 66)
        Me.mLabelMultiply.Name = "mLabelMultiply"
        Me.mLabelMultiply.Size = New System.Drawing.Size(11, 13)
        Me.mLabelMultiply.TabIndex = 7
        Me.mLabelMultiply.Text = "*"
        '
        'mLabelInfoCAPL
        '
        Me.mLabelInfoCAPL.AutoSize = True
        Me.mLabelInfoCAPL.ForeColor = System.Drawing.SystemColors.ControlText
        Me.mLabelInfoCAPL.Location = New System.Drawing.Point(6, 22)
        Me.mLabelInfoCAPL.Name = "mLabelInfoCAPL"
        Me.mLabelInfoCAPL.Size = New System.Drawing.Size(407, 26)
        Me.mLabelInfoCAPL.TabIndex = 4
        Me.mLabelInfoCAPL.Text = "Enter multiplication operands and get the result using a parameterized CAPL funct" & _
    "ion." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "The function will also make an output of the calculation in CANoe's write " & _
    "window."
        '
        'Form_VBNET_CANoe
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(485, 665)
        Me.Controls.Add(Me.mGroupboxConfigSettings)
        Me.Controls.Add(Me.mGroupboxSignalValues)
        Me.Controls.Add(Me.mGroupboxSystemVariables)
        Me.Controls.Add(Me.mGroupboxMeasurementControl)
        Me.Controls.Add(Me.mGroupboxCAPLWriteWindowOut)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.Name = "Form_VBNET_CANoe"
        Me.Text = "CANoe as a COM Server - VB.NET Sample Application"
        Me.mGroupboxConfigSettings.ResumeLayout(False)
        Me.mGroupboxConfigSettings.PerformLayout()
        Me.mGroupboxSignalValues.ResumeLayout(False)
        Me.mGroupboxSignalValues.PerformLayout()
        CType(Me.mNumericupdownCycleTime, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.mTrackbarEngineTempIn, System.ComponentModel.ISupportInitialize).EndInit()
        Me.mGroupboxSystemVariables.ResumeLayout(False)
        Me.mGroupboxSystemVariables.PerformLayout()
        CType(Me.mTrackbarSysVars, System.ComponentModel.ISupportInitialize).EndInit()
        Me.mGroupboxMeasurementControl.ResumeLayout(False)
        Me.mGroupboxCAPLWriteWindowOut.ResumeLayout(False)
        Me.mGroupboxCAPLWriteWindowOut.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents mGroupboxConfigSettings As System.Windows.Forms.GroupBox
    Private WithEvents mLabelInfoConfig As System.Windows.Forms.Label
    Private WithEvents mButtonOpenConfiguration As System.Windows.Forms.Button
    Private WithEvents mLabelDirCheck As System.Windows.Forms.Label
    Private WithEvents mTextboxConfigDir As System.Windows.Forms.TextBox
    Private WithEvents mLabelEngTempOut As System.Windows.Forms.Label
    Private WithEvents mProgressbarEngTempOut As System.Windows.Forms.ProgressBar
    Private WithEvents mGroupboxSignalValues As System.Windows.Forms.GroupBox
    Private WithEvents mTextboxEngStatusOut As System.Windows.Forms.TextBox
    Private WithEvents mTextboxEngSpeedOut As System.Windows.Forms.TextBox
    Private WithEvents mLabelmsCycle As System.Windows.Forms.Label
    Private WithEvents mNumericupdownCycleTime As System.Windows.Forms.NumericUpDown
    Private WithEvents mLabelCycleTime As System.Windows.Forms.Label
    Private WithEvents mLabelEngTempMax As System.Windows.Forms.Label
    Private WithEvents mLabelEngTempMin As System.Windows.Forms.Label
    Private WithEvents mComboboxEngineStatusIn As System.Windows.Forms.ComboBox
    Private WithEvents mTrackbarEngineTempIn As System.Windows.Forms.TrackBar
    Private WithEvents mTextboxEngineSpeedIn As System.Windows.Forms.TextBox
    Private WithEvents mLabelEngineStatus As System.Windows.Forms.Label
    Private WithEvents mLabelEngineTemp As System.Windows.Forms.Label
    Private WithEvents mLabelEngineSpeed As System.Windows.Forms.Label
    Private WithEvents mLabelInfoSignalValues As System.Windows.Forms.Label
    Private WithEvents mGroupboxSystemVariables As System.Windows.Forms.GroupBox
    Private WithEvents mLabelSysVarsValue As System.Windows.Forms.Label
    Private WithEvents mLabelSysVarMax As System.Windows.Forms.Label
    Private WithEvents mComboboxSysVars As System.Windows.Forms.ComboBox
    Private WithEvents mProgressbarSysVars As System.Windows.Forms.ProgressBar
    Private WithEvents mLabelSysVarMin As System.Windows.Forms.Label
    Private WithEvents mLabelInfoSysVars As System.Windows.Forms.Label
    Private WithEvents mTrackbarSysVars As System.Windows.Forms.TrackBar
    Private WithEvents mTextboxOperand2 As System.Windows.Forms.TextBox
    Private WithEvents mButtonCalculate As System.Windows.Forms.Button
    Private WithEvents mLabelMeasurementStatus As System.Windows.Forms.Label
    Private WithEvents mButtonStartStop As System.Windows.Forms.Button
    Private WithEvents mGroupboxMeasurementControl As System.Windows.Forms.GroupBox
    Private WithEvents mGroupboxCAPLWriteWindowOut As System.Windows.Forms.GroupBox
    Private WithEvents mTextboxOperationResult As System.Windows.Forms.TextBox
    Private WithEvents mTextboxOperand1 As System.Windows.Forms.TextBox
    Private WithEvents mLabelEquals As System.Windows.Forms.Label
    Private WithEvents mLabelMultiply As System.Windows.Forms.Label
    Private WithEvents mLabelInfoCAPL As System.Windows.Forms.Label
    Private WithEvents mTimerPolling As System.Windows.Forms.Timer

End Class
