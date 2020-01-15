' This VB-Script starts and checks the CANSystemDemo-Demo using Signals
' The Variable EngSpeed is assigned to the Signal  EngSpeed of the message EngineData
' The Variable CarSpeed is assigned to the Signal  CarSpeed of the message ABSdata
'-----------------------------------------------------------------------------
' Copyright (c) 1999 - 2000 by Vector Informatik GmbH.  All rights reserved.

  Dim FullName
  Dim TimeoutCounter
  Dim StartValue

  TimeoutCounter = 0
  StartValue     = 0

  ' get the ToolInfo object
  Set Tool = CreateObject("CANutil.ToolInfo")

  ' show disclaimer 
  If Tool.ShowApplicationDisclaimer("CANoe.Application") <> 1 Then
    Wscript.Quit
  End If

  Set App         = CreateObject( "CANoe.Application")
  Set Measurement = App.Measurement
  WScript.ConnectObject App, "App_"

  If Measurement.Running Then
    Measurement.Stop
  End If

  Set systemCAN = App.System
  If Not (systemCAN Is Nothing) Then
    Wscript.Sleep 500
    OpenConfig ("\..\CANSystemDemo.cfg")

      Set namespaces = systemCAN.namespaces
      If Not (namespaces Is Nothing) Then
        Set nsComfortBus = namespaces("ComfortBus")
        
        If Not (nsComfortBus Is Nothing) Then
          Set comfortbusVars = nsComfortBus.Variables

          If Not (comfortbusVars Is Nothing) Then
            If Not Measurement.Running Then
              Measurement.Start

            Set Start = comfortbusVars("GatewayStarter")
            Set EngSpeed = App.Bus.GetSignal(2, "EngineData", "EngSpeed")
            Set CarSpeed = App.Bus.GetSignal(2, "ABSdata", "CarSpeed")

            While Not StartValue = 1
              StartValue = Start.Value      
              Wscript.Sleep 10
              TimeoutCounter = TimeoutCounter + 1
              If TimeoutCounter = 10 Then 
                StartValue = 1
              End If
            Wend

            WScript.Sleep 7000

            ShowSignal "EngSpeed", EngSpeed
            ShowSignal "CarSpeed", CarSpeed

            Set CarSpeed = Nothing
            Set EngSpeed = Nothing
          Else
            MsgBox "No systemvariables found for namespace ComfortBus..."
          End If
        Else
          MsgBox "Namespace ComfortBus not found..."
        End If
      Else 
        MsgBox "No namespaces found..."
      End If

      set Tool     = Nothing
      Set App      = Nothing

      MsgBox "Signal script done..."

    End If
  End If 

  Sub ShowSignal( name, signal)
    On Error Resume Next
    MsgBox "The current " & name & " is " & signal.Value, vbSystemModal
    If Err.Number = &H8000000A Then
      MsgBox "The current " & name & " is not available.", vbSystemModal
    End If
  End Sub

  Function GetFullName( relpath)
    Dim FSO, FullPath, MsgTxt

    On Error Resume Next
    Set FSO     = CreateObject("Scripting.FileSystemObject")
    FullPath    = FSO.GetFile(WScript.ScriptFullName).ParentFolder & relpath
    GetFullName = FSO.GetFile(FullPath).Path
    Set FSO     = Nothing

    MsgTxt = "The object returned the following error: " & Chr(13) & Chr(13) & _
             "Error code: 0x" & Hex(Err.Number) & Chr(13) & _
             "Source: " & Err.Source & Chr(13) & _
             "Description: " & Err.Description

    If Err.Number Then
      MsgBox MsgTxt & Chr(10) & "(" & Err.Description & ")", VBOKOnly + VBCritical, "Error"
      WScript.Quit
    End If
  End Function


  Sub OpenConfig(relpath)  'Open the specified config (if it isn't already)
    On Error Resume Next

    FullName = GetFullName(relpath)
    App.Open(FullName)

    If Err.Number Then
      MsgBox "Error opening configuration!" & Chr(13) & _
             "Error code: 0x" & Hex(Err.Number) & Chr(13) & _
             "Source: " & Err.Source & Chr(13) & _
             "Description: " & Err.Description, VBOKOnly + VBCritical, "Error"
      WScript.Quit
    End If
  End Sub

