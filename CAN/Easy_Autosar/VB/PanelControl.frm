VERSION 5.00
Begin VB.Form PanelControl 
   BackColor       =   &H00C0C0C0&
   Caption         =   "CANoe Automot Control"
   ClientHeight    =   2385
   ClientLeft      =   2550
   ClientTop       =   1920
   ClientWidth     =   7575
   Icon            =   "PanelControl.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   2385
   ScaleWidth      =   7575
   Begin VB.CommandButton CloseAllPanels 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Close"
      Height          =   495
      Left            =   3840
      Style           =   1  'Graphical
      TabIndex        =   12
      Top             =   1680
      Width           =   855
   End
   Begin VB.CommandButton OpenAllPanels 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Open"
      Height          =   495
      Left            =   2880
      MaskColor       =   &H000000FF&
      Style           =   1  'Graphical
      TabIndex        =   11
      Top             =   1680
      UseMaskColor    =   -1  'True
      Width           =   855
   End
   Begin VB.CommandButton ExitCANoe 
      BackColor       =   &H00C0E0FF&
      Cancel          =   -1  'True
      Caption         =   "Exit"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   5160
      Style           =   1  'Graphical
      TabIndex        =   9
      Top             =   1800
      Width           =   2295
   End
   Begin VB.CommandButton OpenControlPanel 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Open"
      Height          =   495
      Left            =   2880
      MaskColor       =   &H000000FF&
      Style           =   1  'Graphical
      TabIndex        =   8
      Top             =   480
      UseMaskColor    =   -1  'True
      Width           =   855
   End
   Begin VB.CommandButton CloseControlPanel 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Close"
      Height          =   495
      Left            =   3840
      Style           =   1  'Graphical
      TabIndex        =   7
      Top             =   480
      Width           =   855
   End
   Begin VB.CommandButton OpenDisplayPanel 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Open"
      Height          =   495
      Left            =   2880
      MaskColor       =   &H000000FF&
      Style           =   1  'Graphical
      TabIndex        =   6
      Top             =   1080
      UseMaskColor    =   -1  'True
      Width           =   855
   End
   Begin VB.CommandButton CloseDisplayPanel 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Close"
      Height          =   495
      Left            =   3840
      Style           =   1  'Graphical
      TabIndex        =   5
      Top             =   1080
      Width           =   855
   End
   Begin VB.CommandButton StopMeasurement 
      BackColor       =   &H00C0E0FF&
      Caption         =   "Stop Measurement"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   5160
      Style           =   1  'Graphical
      TabIndex        =   1
      Top             =   840
      Width           =   2295
   End
   Begin VB.CommandButton StartMeasurement 
      BackColor       =   &H00C0E0FF&
      Caption         =   "Start Measurement"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   5160
      Style           =   1  'Graphical
      TabIndex        =   0
      Top             =   240
      UseMaskColor    =   -1  'True
      Width           =   2295
   End
   Begin VB.Frame Frame1 
      BackColor       =   &H8000000B&
      Caption         =   "Panel Control"
      Height          =   2055
      Left            =   120
      TabIndex        =   2
      Top             =   240
      Width           =   4935
      Begin VB.Label Label3 
         BackStyle       =   0  'Transparent
         Caption         =   "All Panels:"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   9.75
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00800000&
         Height          =   255
         Left            =   240
         TabIndex        =   10
         Top             =   1560
         Width           =   1575
      End
      Begin VB.Label Label2 
         BackStyle       =   0  'Transparent
         Caption         =   "Display Panel:"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   9.75
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00800000&
         Height          =   255
         Left            =   240
         TabIndex        =   4
         Top             =   960
         Width           =   1815
      End
      Begin VB.Label Label1 
         BackStyle       =   0  'Transparent
         Caption         =   "Control Panel:"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   9.75
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00800000&
         Height          =   255
         Left            =   240
         TabIndex        =   3
         Top             =   360
         Width           =   2175
      End
   End
End
Attribute VB_Name = "PanelControl"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim WithEvents CANoe As CANoe.Application
Attribute CANoe.VB_VarHelpID = -1
Dim WithEvents Measurement As CANoe.Measurement
Attribute Measurement.VB_VarHelpID = -1
Dim PanelsControl, PanelsDisplay, PanelsAll
Dim activeForm
Dim Tool
Dim MemDisclaim
  
Private Sub CANoe_OnOpen(ByVal file As String)
  'Enable/Disable of the Start/Stop-Buttons
  StartMeasurement.Enabled = True
  StopMeasurement.Enabled = False
  
  Call AddPanelGroups
End Sub

Private Sub CANoe_OnQuit()
  MsgBox "Disconnecting due to CANoe shutdown... Program will be closed ! "
  Unload Me
End Sub

Private Sub CloseAllPanels_Click()
  If Measurement.Running = True Then
    'Save Panel positions
    CANoe.Configuration.GeneralSetup.PanelSetup.SavePositions
    'Close all panels
    Call SetPanelVisible(PanelsAll, False)
  End If
End Sub

Private Sub CloseControlPanel_Click()
  If Measurement.Running = True Then
    'Save Panel positions
    CANoe.Configuration.GeneralSetup.PanelSetup.SavePositions
    'Close control panel
    Call SetPanelVisible(PanelsControl, False)
  End If
End Sub

Private Sub CloseDisplayPanel_Click()
  If Measurement.Running = True Then
    'Save Panel positions
    CANoe.Configuration.GeneralSetup.PanelSetup.SavePositions
    'Close display panel
    Call SetPanelVisible(PanelsDisplay, False)
  End If
End Sub

Private Sub ExitCANoe_Click()
  'Stop the measurement if still running
  If Not Measurement Is Nothing Then
    If (Measurement.Running) Then Measurement.Stop
  End If
   
  'Exit CANoe
  CANoe.Quit
  
  'Exit
  End
End Sub

Private Sub OpenAllPanels_Click()
  If Measurement.Running = True Then
    'Open all panels
    Call SetPanelVisible(PanelsAll, True)
  End If
End Sub

Private Sub OpenControlPanel_Click()
  If Measurement.Running = True Then
    'Open control panel
    Call SetPanelVisible(PanelsControl, True)
  End If
End Sub

Private Sub OpenDisplayPanel_Click()
  If Measurement.Running = True Then
    'Open display panel
    Call SetPanelVisible(PanelsDisplay, True)
  End If
End Sub

Private Sub StartMeasurement_Click()
 On Error GoTo Fail

  'Enable/Disable of Start/Stop-Buttons
  StartMeasurement.Enabled = False
  StopMeasurement.Enabled = True

  'Compile all CAPL programs
  CANoe.CAPL.Compile

  'Start of maesurement
  Measurement.Start
  Exit Sub
Fail:
  Call ShowError
End Sub
Private Sub StopMeasurement_Click()
  On Error GoTo Fail
  
  'Button Event to stop the measuremnt
  If (Measurement.Running) Then Measurement.Stop
  Exit Sub
Fail:
  Call ShowError
End Sub

Private Sub Measurement_OnStart()
  On Error GoTo Fail
  'Enable/Disable of Start/Stop Buttons
  StartMeasurement.Enabled = False
  StopMeasurement.Enabled = True
  Exit Sub
Fail:
  Call ShowError
End Sub

Private Sub Measurement_OnStop()
  'Enable/Disable of Start/Stop Buttons
  StartMeasurement.Enabled = True
  StopMeasurement.Enabled = False
End Sub

Private Sub Form_Load()
  Set activeForm = PanelControl
  
  Set Tool = CreateObject("CANutil.ToolInfo")
    If MemDisclaim = 0 Then
      If Tool.ShowApplicationDisclaimer("CANoe.Application") <> 1 Then
        MemDisclaim = 0
        Unload Me
        End
      End If
    End If
           
   MemDisclaim = 1
   
  'Load CANoe
  Set CANoe = New CANoe.Application
  Set Measurement = CANoe.Measurement
 
  'Load of the CANoe configuration "Easy.cfg"
  Call OpenConfig("Easy.cfg")

  'If any measurement is already running it will be stopped
  If (Measurement.Running) Then Measurement.Stop
End Sub

Function GetConfigurationFullName(cfgName)
  Dim FSO, folder, file, names, i
  On Error Resume Next
  Set FSO = CreateObject("Scripting.FileSystemObject")
  Set folder = FSO.GetFolder(App.Path).ParentFolder
  Set file = FSO.GetFile(folder.Path & "\" & cfgName)
    
  GetConfigurationFullName = file.Path
End Function

Function OpenConfig(cfgName)
  Dim full
  full = GetConfigurationFullName(cfgName)
  If full <> CANoe.Configuration.fullName Then
    On Error GoTo Fail
    CANoe.Open (full)
  Else
    Call AddPanelGroups
  End If
  Exit Function
Fail:
  Dim Msg
  Msg = "The configuration " & cfgName & " could not be opened." & Chr(13) & _
        "The configuration " & CANoe.Configuration.fullName & " may be open and changed or the measurement is running." & Chr(13) & _
        "In this case, save or discard your changes resp. stop the measurement and start PanelControl.exe again."
  MsgBox Msg, vbSystemModal + vbMsgBoxHelpButton, "Error", Err.HelpFile, Err.HelpContext
  Unload Me
End Function

'Open/Close of panels
Sub SetPanelVisible(panels, visible As Boolean)
  Dim count, i
  On Error GoTo Fail
  count = panels.count
  If count > 0 Then
    For i = 1 To count Step 1
      panels.Item(i).visible = visible
    Next i
  End If
  Exit Sub
Fail:
  Call ShowError
End Sub

'Add a panel to a list of panels
Sub AddPanel(panels, panelName)
  Dim fullName, Panel
  On Error GoTo Fail
  fullName = CANoe.Configuration.Path & "\Panels\" & panelName
  Set Panel = panels.Add(fullName)
  Exit Sub
Fail:
  Call ShowError
End Sub

Sub AddPanelGroups()
  
  'Various panel groups will be builded
  '4 = temporär
  Set PanelsControl = CANoe.Configuration.GeneralSetup.PanelSetup.panels(4)
  Set PanelsDisplay = CANoe.Configuration.GeneralSetup.PanelSetup.panels(4)
  Set PanelsAll = CANoe.Configuration.GeneralSetup.PanelSetup.panels(4)

  'Add the panels to the various goups
  'Control Panels
  Call AddPanel(PanelsControl, "control.xvp")

  'Display Panels
  Call AddPanel(PanelsDisplay, "display.xvp")

  'All Panels
  Call AddPanel(PanelsAll, "control.xvp")
  Call AddPanel(PanelsAll, "display.xvp")
End Sub


Sub ShowError()
  Dim Msg
  Msg = "The object returned the following error: " & Chr(13) & Chr(13) & _
        "Code: 0x" & Hex(Err.Number) & Chr(13) & _
        "Source: " & Err.Source & Chr(13) & _
        "Description: " & Err.Description
  MsgBox Msg, vbSystemModal + vbMsgBoxHelpButton, "Error", Err.HelpFile, Err.HelpContext
End Sub
  
