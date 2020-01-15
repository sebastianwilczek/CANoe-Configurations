VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "CANoe Automation"
   ClientHeight    =   3615
   ClientLeft      =   6555
   ClientTop       =   10620
   ClientWidth     =   5535
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3615
   ScaleWidth      =   5535
   Begin VB.CommandButton btnSet 
      Caption         =   "Set"
      Enabled         =   0   'False
      Height          =   375
      Left            =   1560
      TabIndex        =   3
      Top             =   3000
      Visible         =   0   'False
      Width           =   1215
   End
   Begin VB.CommandButton btnStart 
      Caption         =   "Start"
      Enabled         =   0   'False
      Height          =   375
      Left            =   240
      TabIndex        =   2
      Top             =   3000
      Visible         =   0   'False
      Width           =   1215
   End
   Begin VB.CommandButton btnConnect 
      Caption         =   "Connect"
      Height          =   375
      Left            =   240
      TabIndex        =   1
      Top             =   3000
      Width           =   1215
   End
   Begin MSComctlLib.TabStrip TabStrip 
      Height          =   3375
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   5295
      _ExtentX        =   9340
      _ExtentY        =   5953
      _Version        =   393216
      BeginProperty Tabs {1EFB6598-857C-11D1-B16A-00C0F0283628} 
         NumTabs         =   2
         BeginProperty Tab1 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "General"
            ImageVarType    =   2
         EndProperty
         BeginProperty Tab2 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "Disable stop"
            ImageVarType    =   2
         EndProperty
      EndProperty
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim WithEvents CANoeApp As CANoe.Application
Attribute CANoeApp.VB_VarHelpID = -1
Dim WithEvents Measurement As CANoe.Measurement
Attribute Measurement.VB_VarHelpID = -1
Dim systemCAN As CANoe.System
Dim WithEvents gTestCompleted As CANoe.Variable
Attribute gTestCompleted.VB_VarHelpID = -1
Dim WithEvents SvInteger As CANoe.Variable
Attribute SvInteger.VB_VarHelpID = -1
Dim WithEvents SvFloat As CANoe.Variable
Attribute SvFloat.VB_VarHelpID = -1
Dim WithEvents SvString As CANoe.Variable
Attribute SvString.VB_VarHelpID = -1
Dim WithEvents SvIntegerArray As CANoe.Variable
Attribute SvIntegerArray.VB_VarHelpID = -1
Dim WithEvents SvFloatArray As CANoe.Variable
Attribute SvFloatArray.VB_VarHelpID = -1
Dim WithEvents SvData As CANoe.Variable
Attribute SvData.VB_VarHelpID = -1

Dim gWrite As CANoe.Write
Dim gToggle
Dim Tool
Dim MemDisclaim

Private Sub btnConnect_Click()
  If CANoeApp Is Nothing Then
    Connect
  Else
    Disconnect
  End If
End Sub

Private Sub btnStart_Click()
  If Measurement.Running Then
    Measurement.Stop
  Else
    StartMeasurement
  End If
End Sub

Private Sub btnSet_Click()
  On Error Resume Next
  Dim i, f, s, da, fa
  Dim dat() As Byte
  
  If gToggle Then
    gToggle = False
    i = 0
    f = 1.23
    s = "Hello"
    da = Array(0, 1, 2, 3)
    fa = Array(0#, 1.1, 2.2, 3.3)
    ReDim dat(0 To 2)
    dat(0) = 255
    dat(1) = 53
    dat(2) = 116
  Else
    gToggle = True
    i = 1
    f = 3.21
    s = "CANoe"
    da = Array(3, 2, 1, 0)
    fa = Array(3.3, 2.2, 1.1, 0#)
    ReDim dat(0 To 4) As Byte
    dat(0) = 67
    dat(1) = 65
    dat(2) = 78
    dat(3) = 111
    dat(4) = 101
  End If
  SvInteger.Value = i
  SvFloat.Value = f
  SvString.Value = s
  SvIntegerArray.Value = da
  SvFloatArray.Value = fa
  SvData.Value = dat
  EnableStopCommand (True)
End Sub

Private Sub CANoeApp_OnOpen(ByVal Name As String)
  On Error GoTo Fail
  '
  Exit Sub
Fail:
End Sub

Private Sub CANoeApp_OnQuit()
  MsgBox "CANoe shuts down... Program will be closed ! ", vbSystemModal Or vbInformation
  Set TabStrip.SelectedItem = TabStrip.Tabs(1)
  Unload Me
End Sub

Private Sub Measurement_OnStart()
  btnStart.Caption = "Stop"
  EnableStopCommand (False)
End Sub

Private Sub Measurement_OnStop()
  btnStart.Caption = "Start"
End Sub

Private Sub Measurement_OnExit()
  Set gTestCompleted = Nothing
End Sub

Private Sub gTestCompleted_OnChange(ByVal Value As Variant)
  If Value Then
    EnableStopCommand (True)
  End If
End Sub

Private Sub TabStrip_Click()
  Select Case TabStrip.SelectedItem.Index
  Case 1
    btnConnect.Visible = True
    btnStart.Visible = False
    btnSet.Visible = False
  Case 2
    btnConnect.Visible = False
    btnStart.Visible = True
    btnSet.Visible = True
  Case Else
  End Select
End Sub

Private Sub Connect()
  Set Tool = CreateObject("CANutil.ToolInfo")
  
  If MemDisclaim = 0 Then
    If Tool.ShowApplicationDisclaimer("CANoe.Application") <> 1 Then
      MemDisclaim = 0
      Unload Me
      End
    End If
  End If
  
  MemDisclaim = 1
  Set CANoeApp = New Application
  Set Measurement = CANoeApp.Measurement
  Set systemCAN = CANoeApp.System
  Set gWrite = CANoeApp.UI.Write
  btnConnect.Caption = "Disconnect"
  btnStart.Enabled = True
  btnSet.Enabled = True
  If Measurement.Running Then
    btnStart.Caption = "Stop"
  Else
    btnStart.Caption = "Start"
  End If
End Sub

Private Sub Disconnect()
  btnStart.Enabled = False
  btnSet.Enabled = False
  btnConnect.Caption = "Connect"
  Set gWrite = Nothing
  Set systemCAN = Nothing
  Set Measurement = Nothing
  Set CANoeApp = Nothing
End Sub

Private Sub StartMeasurement()
  Dim env
  OpenConfig ("comauto.cfg")
  If Not (systemCAN Is Nothing) Then
    Dim namespaces As CANoe.namespaces
    Set namespaces = systemCAN.namespaces
    
    If Not (namespaces Is Nothing) Then
      Dim nsCOMAutomation As CANoe.Namespace
      Set nsCOMAutomation = namespaces("COMAutomation")
      
      If Not (nsCOMAutomation Is Nothing) Then
        Dim COMAutomationVars As CANoe.Variables
        Set COMAutomationVars = nsCOMAutomation.Variables
        
        If Not (COMAutomationVars Is Nothing) Then
          Set gTestCompleted = COMAutomationVars("TestCompleted")
          Set SvInteger = COMAutomationVars("Integer1")
          Set SvFloat = COMAutomationVars("Float1")
          Set SvString = COMAutomationVars("String1")
          Set SvIntegerArray = COMAutomationVars("IntegerArray1")
          Set SvFloatArray = COMAutomationVars("FloatArray1")
          Set SvData = COMAutomationVars("Data1")
        End If
      End If
    End If
  End If
  
  btnSet.Enabled = True
  gTestCompleted.Value = False
  CANoeApp.CAPL.Compile
  If Not Measurement.Running Then
    Measurement.Start
  End If
End Sub

Sub EnableStopCommand(enable)
  Dim current
  current = CANoeApp.UI.CommandEnabled("stop")
  If current = enable Then
    Exit Sub
  End If
  CANoeApp.UI.CommandEnabled("stop") = enable
  If enable Then
    gWrite.Output "Enabling <Stop> command from Visual Basic..."
  Else
    gWrite.Output "Disabling <Stop> command from Visual Basic..."
  End If
End Sub

Function GetDemoPath(cfgName)
  Dim FSO, folder, file, names, i
  On Error Resume Next
  Set FSO = CreateObject("Scripting.FileSystemObject")
  Set folder = FSO.GetFolder(App.Path).ParentFolder
  Set file = FSO.GetFile(folder.Path & "\" & cfgName)
  Set FSO = Nothing
       
  GetDemoPath = file.Path
 
End Function

Sub OpenConfig(cfgName)
  Dim full
  On Error Resume Next
  full = GetDemoPath(cfgName)

  CANoeApp.Open (full)
  
  If full <> CANoeApp.Configuration.FullName Then
    If Not CANoeApp.Configuration.Name = "comauto" Then
      Do Until CANoeApp.Configuration.Name = "comauto"
        Form1.SetFocus
        MsgBox "You select " & CANoeApp.Configuration.Name & ".cfg and not comauto.cfg. Please correct this!", vbCritical + vbMsgBoxSetForeground
        CANoeApp.Open (full)
        If Err.Number = -2147467259 Then
          Exit Do
        End If
      Loop
    End If
  End If
  
  If Err.Number Then
    MsgBox "VB shuts down...", vbSystemModal Or vbInformation
    End
  End If
End Sub

