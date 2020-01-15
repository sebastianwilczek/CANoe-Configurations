Imports CANoe

Public Class PanelControl

    'Declare types
    Dim WithEvents CANoe As CANoe.Application
    Dim WithEvents Measurement As CANoe.Measurement
    Dim WithEvents Configuration As CANoe.Configuration
    Dim WithEvents GeneralSetup As CANoe.GeneralSetup
    Dim WithEvents PanelSetup As CANoe.PanelSetup
    Dim WithEvents CAPL As CANoe.CAPL
    Dim WithEvents PanelsAll As CANoe.Panels
    Dim MemDisclaim = 0

    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()
    End Sub

    Private Function GetConfigurationFullName(cfgName)
        Dim folderName
        On Error Resume Next

        folderName = AppDomain.CurrentDomain.BaseDirectory
        folderName = System.IO.Path.GetDirectoryName(folderName)
        folderName = System.IO.Path.GetDirectoryName(folderName)

        GetConfigurationFullName = folderName & "\" & cfgName
    End Function


    Private Sub OpenConfig(cfgName)
        Dim full
        full = GetConfigurationFullName(cfgName)
        If full <> CANoe.Configuration.FullName Then
            On Error GoTo Fail
            CANoe.Open(full)
        End If
        Configuration = CANoe.Configuration
        GeneralSetup = Configuration.GeneralSetup
        PanelSetup = GeneralSetup.PanelSetup
        CAPL = CANoe.CAPL

        If full <> CANoe.Configuration.FullName Then
            Call AddPanels()
        End If

        Exit Sub
Fail:
        Dim Msg
        Msg = "The configuration " & cfgName & " could not be opened." & Chr(13) & _
              "The configuration " & CANoe.Configuration.fullName & " may be open and changed or the measurement is running." & Chr(13) & _
              "In this case, save or discard your changes resp. stop the measurement and start PanelControl.exe again."
        MsgBox(Msg, vbSystemModal + vbInformation, "Error")
    End Sub

    Private Sub PanelControl_FormClosed(sender As Object, e As FormClosedEventArgs) Handles MyBase.FormClosed
        MsgBox("Disconnecting due to CANoe shutdown... Program will be closed ! ")
    End Sub

    Private Sub btnStartCANoe_Click(sender As Object, e As EventArgs) Handles btnStartCANoe.Click
        Dim Tool = CreateObject("CANutil.ToolInfo")
        If MemDisclaim = 0 Then
            If Tool.ShowApplicationDisclaimer("CANoe.Application") <> 1 Then
                MemDisclaim = 0
                System.Windows.Forms.Application.Exit()
                Return
            End If
        End If

        MemDisclaim = 1

        ' Add any initialization after the InitializeComponent() call.
        CANoe = New CANoe.Application
        Measurement = CANoe.Measurement

        'Load of the CANoe configuration "Easy.cfg"
        Call OpenConfig("Easy.cfg")

        'If any measurement is already running it will be stopped
        If (Measurement.Running) Then Measurement.Stop()

        PanelsAll = PanelSetup.Panels(0)

        'Enable/Disable of the Start/Stop-Buttons
        btnStartMeasurement.Enabled = True
        btnStopMeasurement.Enabled = False

        Call AddPanels()
    End Sub

    Private Sub btnOpenControlPanel_Click(sender As Object, e As EventArgs) Handles btnOpenControlPanel.Click
        'If Measurement.Running = True Then
        'Open control panel
        SetPanelVisible("Control", True)
        'End If
    End Sub

    Private Sub btnCloseControlPanel_Click(sender As Object, e As EventArgs) Handles btnCloseControlPanel.Click
        'If Measurement.Running = True Then
        'Save Panel positions
        PanelSetup.SavePositions()
        'Close control panel
        SetPanelVisible("Control", False)
        'End If
    End Sub

    Private Sub btnOpenDisplayPanel_Click(sender As Object, e As EventArgs) Handles btnOpenDisplayPanel.Click
        'If Measurement.Running = True Then
        'Open display panel
        SetPanelVisible("Display", True)
        'End If
    End Sub

    Private Sub btnCloseDisplayPanel_Click(sender As Object, e As EventArgs) Handles btnCloseDisplayPanel.Click
        'If Measurement.Running = True Then
        'Save Panel positions
        PanelSetup.SavePositions()
        'Close display panel
        SetPanelVisible("Display", False)
        'End If
    End Sub

    Private Sub btnOpenAllPanels_Click(sender As Object, e As EventArgs) Handles btnOpenAllPanels.Click
        'If Measurement.Running = True Then
        'Open all panels
        SetPanelVisible("Control", True)
        SetPanelVisible("Display", True)
        'End If
    End Sub

    Private Sub btnCloseAllPanels_Click(sender As Object, e As EventArgs) Handles btnCloseAllPanels.Click
        'If Measurement.Running = True Then
        'Save Panel positions
        PanelSetup.SavePositions()
        'Close display panel
        SetPanelVisible("Control", False)
        SetPanelVisible("Display", False)
        'End If
    End Sub

    Private Sub btnStartMeasurement_Click(sender As Object, e As EventArgs) Handles btnStartMeasurement.Click
        On Error GoTo Fail

        'Enable/Disable of Start/Stop-Buttons
        btnStartMeasurement.Enabled = False
        btnStopMeasurement.Enabled = True

        'Compile all CAPL programs
        CAPL.Compile()

        'Start of maesurement
        Measurement.Start()
        Exit Sub
Fail:
        Call ShowError()
    End Sub

    Private Sub btnStopMeasurement_Click(sender As Object, e As EventArgs) Handles btnStopMeasurement.Click
        On Error GoTo Fail

        'Enable/Disable of Start/Stop-Buttons
        btnStartMeasurement.Enabled = True
        btnStopMeasurement.Enabled = False

        'Button Event to stop the measuremnt
        If (Measurement.Running) Then Measurement.Stop()
        Exit Sub
Fail:
        Call ShowError()
    End Sub

    Private Sub btnExit_Click(sender As Object, e As EventArgs) Handles btnExit.Click
        'Stop the measurement if still running
        If Not Measurement Is Nothing Then
            If (Measurement.Running) Then Measurement.Stop()
        End If

        'Exit CANoe
        CANoe.Quit()

        'End
    End Sub

    'Add a panel to a list of panels
    Sub AddPanel(panels, panelName)
        Dim fullName, Panel
        On Error GoTo Fail
        fullName = Configuration.Path & "\Panels\" & panelName
        Panel = panels.Add(fullName)
        Exit Sub
Fail:
        Call ShowError()
    End Sub

    Sub AddPanels()

        PanelsAll = PanelSetup.Panels(0)

        If (PanelsAll.Item("Control") Is Nothing) Then
            Call AddPanel(PanelsAll, "control.xvp")
        End If

        If (PanelsAll.Item("Display") Is Nothing) Then
            Call AddPanel(PanelsAll, "display.xvp")
        End If
    End Sub

    'Open/Close of panels
    Sub SetPanelVisible(panel As String, visible As Boolean)
        On Error GoTo Fail
        PanelsAll.Item(panel).Visible = visible
        Exit Sub
Fail:
        Call ShowError()
    End Sub

    Sub ShowError()
        Dim Msg
        Msg = "The object returned the following error: " & Chr(13) & Chr(13) & _
              "Code: 0x" & Hex(Err.Number) & Chr(13) & _
              "Source: " & Err.Source & Chr(13) & _
              "Description: " & Err.Description
        MsgBox(Msg, vbSystemModal + vbInformation, "Error")
    End Sub

End Class
