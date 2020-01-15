' This VB-Script starts and controls the CANSystemDemo-Demo using Environment Variables
' The Variable Pedal is assigned to the Environment Variable EnvPedalPosition
'-----------------------------------------------------------------------------
' Copyright (c) 1999 - 2000 by Vector Informatik GmbH.  All rights reserved.

  Dim FullName
  Dim Count

  ' get the ToolInfo object
  Set Tool = CreateObject("CANutil.ToolInfo")

  ' show disclaimer
  If Tool.ShowApplicationDisclaimer("CANoe.Application") <> 1 Then
    WScript.Quit
  End If

  Set App = CreateObject("CANoe.Application")
  Set Measurement = App.Measurement
  WScript.ConnectObject App, "App_"

  If Measurement.Running Then
    Measurement.Stop
  End If

  Set systemCAN = App.System
  If Not (systemCAN Is Nothing) Then
    WScript.Sleep 500

    OpenConfig ("\..\CANSystemDemo.cfg")

    Set Measurement = App.Measurement
    Set namespaces = systemCAN.namespaces
    
    If Not (namespaces Is Nothing) Then
      Set nsPowerTrain = namespaces("PowerTrain")
      
      If Not (nsPowerTrain Is Nothing) Then
        Set powerTrainVars = nsPowerTrain.Variables
        
        If Not (powerTrainVars Is Nothing) Then
          Set Pedal = powerTrainVars("PedalPosition")
      
          Set Panels = App.Configuration.GeneralSetup.PanelSetup.Panels(0)
          Set Engine = Panels("Control")
          Set Panels = Nothing

          If Engine Is Nothing Then
            MsgBox "No Engine panel avail..."
          Else
            Engine.Visible = False
            WScript.Sleep (500)
            Engine.Visible = True
            Set Engine = Nothing
          End If

          For Count = 0 To 2
            For Position = 0 To 100
              Pedal.Value = (Position * 1.0)
              WScript.Sleep (20)
            Next
          Next

          Set Tool = Nothing
          Set Pedal = Nothing
        End If
      End If
    End If
  End If
  Set systemCAN = Nothing
  Set Measurement = Nothing
  Set App = Nothing
  Set Tool = Nothing

  MsgBox "Environment script done..."

  Function GetFullName(relpath)
    Dim FSO, FullPath

    Set FSO = CreateObject("Scripting.FileSystemObject")
    FullPath = FSO.GetFile(WScript.ScriptFullName).ParentFolder & relpath
    GetFullName = FSO.GetFile(FullPath).Path
    Set FSO = Nothing
  End Function

  Sub OpenConfig(relpath)  'Open the specified config (if it isn't already)
    On Error Resume Next

    FullName = GetFullName(relpath)
    App.Open (FullName)

    If Err.Number Then
      MsgBox "Error opening configuration!" & Chr(13) & _
             "Error code: 0x" & Hex(Err.Number) & Chr(13) & _
             "Source: " & Err.Source & Chr(13) & _
             "Description: " & Err.Description, vbOKOnly + vbCritical, "Error"
      WScript.Quit
    End If
  End Sub

