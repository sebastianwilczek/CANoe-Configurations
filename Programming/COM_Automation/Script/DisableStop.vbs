' This VB-Script shows how to disable/enable the Stop-Button of CANoe to 
' avoid breaking off uncompleted Measurements
'-----------------------------------------------------------------------------
' Copyright (c) 1999 - 2000 by Vector Informatik GmbH.  All rights reserved.

  Option Explicit

  Dim App, Measurement, Write, systemCAN, namespaces, nsCOMAutomation, COMAutomationVars, TestCompleted, TestVariable, Tool, FullName

  VerifyScriptVersion()

  ' get the ToolInfo object
  Set Tool = CreateObject("CANutil.ToolInfo")

  ' show disclaimer 
  If Tool.ShowApplicationDisclaimer("CANoe.Application") <> 1 Then
    Wscript.Quit
  End If
 
  Set App         = CreateObject("CANoe.Application")
  Set Measurement = App.Measurement
  Set Write       = App.UI.Write
  Set systemCAN   = App.System
  WScript.ConnectObject Measurement, "Measurement_"

  If Measurement.Running Then
    Measurement.Stop
    WScript.Sleep(1000)
  End If

  OpenConfig ("\..\comauto.cfg")

  App.CAPL.Compile

  If Not (systemCAN Is Nothing) Then
    Set namespaces = systemCAN.namespaces

    If Not (namespaces Is Nothing) Then
      Set nsCOMAutomation = namespaces("COMAutomation")

      If Not (nsCOMAutomation Is Nothing) Then
        Set COMAutomationVars = nsCOMAutomation.Variables

        If Not (COMAutomationVars Is Nothing) Then
          Set TestCompleted = COMAutomationVars("TestCompleted")
          Set TestVariable = COMAutomationVars("TestVariable")
        
          WScript.ConnectObject TestCompleted, "TestCompleted_"

          Write.Clear

          TestVariable.Value = 0

          Measurement.Start

          WScript.Sleep(3000)

          TestVariable.Value = 50

          WScript.Sleep(8000)

          Measurement.Stop

          WScript.Sleep(1000)

          MsgBox "Disable script done..."

          Set TestVariable  = Nothing
          Set TestCompleted = Nothing
        Else
          MsgBox "Disable script done, Systemvariable not found..."
        End If
      Else
        MsgBox "Disable script done, Systemvariable not found..."
      End If
    Else
      MsgBox "Disable script done, Systemvariable not found..."
    End If
  Else
    MsgBox "Disable script done, Systemvariable not found..."
  End If

  WScript.DisconnectObject Measurement
  Set systemCAN     = Nothing
  Set Write         = Nothing
  Set Measurement   = Nothing
  Set App           = Nothing

  Sub VerifyScriptVersion()
    ' The WScript.ConnectObject method isn't available in earlier versions!
    If WScript.Version < "5.1" Then
      MsgBox "This script needs Microsoft Windows Script 5.1", vbSystemModal
      WScript.Quit
    End If
  End Sub 

  Sub Measurement_OnStart()
    EnableStopCommand(False)
  End Sub

  Sub TestCompleted_OnChange(Completed)
    If completed Then
      EnableStopCommand(True)
    End If
  End Sub

  Sub EnableStopCommand(Enable)
    App.UI.CommandEnabled("Stop") = Enable
    If Enable Then
      Write.Output "Enabling <Stop> command from VBScript..."
    Else
      Write.Output "Disabling <Stop> command from VBScript..."
    End If
  End Sub

  Function GetFullName(relpath)
    Dim FSO, FullPath

    On Error Resume Next
    Set FSO      = CreateObject("Scripting.FileSystemObject")
    FullPath     = FSO.GetFile(WScript.ScriptFullName).ParentFolder & relpath
    GetFullName  = FSO.GetFile(FullPath).Path
    Set FSO      = Nothing

    If Err.Number Then
      MsgTxt = "The object returned the following error: " & Chr(13) & Chr(13) & _
               "Code: 0x" & Hex(Err.Number) & Chr(13) & _
               "Source: " & Err.Source & Chr(13) & _
               "Description: " & Err.Description
      MsgBox MsgTxt & Chr(10) & "(" & Err.Description & ")", VBOKOnly + VBCritical, "Error"
    End If
  End Function

  Sub OpenConfig(relpath)  'Open the specified config (if it isn't already)
    On Error Resume Next

    FullName = GetFullName(relpath)
    App.Open(FullName)

    If Err.Number Then
      MsgBox "Error opening configuration!" & Chr(10) & "(" & Err.Description & ")", VBOKOnly + VBCritical, "Error"
      WScript.Quit
    End If
  End Sub

