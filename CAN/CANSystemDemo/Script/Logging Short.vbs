  ' This example shows how simple a logging file can be converted or exported.
  ' The example can be used with the CANoe CANSystemDemo Demo.
  ' 
  ' 
  ' The source file will be the logging file specified in the export dialog of
  ' the 2nd logging block, which is typically the logging file specified in 
  ' this logging block, if it was not changed yet.
  '
  ' Because by default the 2nd logging block in the CANSystemDemo Demo is not 
  ' configured to start automatically the logging at measurement start you 
  ' first have to make sure that a logging file is created by:
  ' 1. starting the logging block manually in measurement setup.
  ' 2. changing the trigger configuration of the logging block.
  '
  ' If no destination file is specified explicitly, the destination file is
  ' typically the file specified in the export dialog.
  ' If there's no file specified there, then a file with the same path and 
  ' filename as the source file will be created, using the CSV format as 
  ' default export format.
  '----------------------------------------------------------------------------
  ' Copyright (c) 2005 by Vector Informatik GmbH.  All rights reserved.

  Set Tool = CreateObject("CANutil.ToolInfo")

  ' show disclaimer 
  If Tool.ShowApplicationDisclaimer("CANoe.Application") <> 1 Then
    Wscript.Quit
  End If

  Set App         = CreateObject("CANoe.Application")
  Set Measurement = App.Measurement

  WScript.ConnectObject App, "App_"

  If Measurement.Running Then
    Measurement.Stop
  End If

  Wscript.Sleep 500
  OpenConfig("\..\CANSystemDemo.cfg")

  Set Logging  = App.Configuration.OnlineSetup.LoggingCollection(2)
  Set Exporter = Logging.Exporter

  ' Load the source file
  Exporter.Load

  ' Save the destination file. Don't inform if destination file exists yet (optional).
  Exporter.Save True
  
  MsgBox "The logging file" & Chr(13) & _
         Exporter.Sources(1) & Chr(13) & _ 
         "was successfully exported/converted to" & Chr(13) & _
         Exporter.Destinations(1) 

  Set Exporter = Nothing
  Set Logging  = Nothing
  Set App      = Nothing

  MsgBox "Logging short script done..."

  Function GetFullName(relpath)
    Dim FSO, FullPath, MsgTxt

    On Error Resume Next
    Set FSO      = CreateObject("Scripting.FileSystemObject")
    CSVFolder    = FSO.GetFile(WScript.ScriptFullName).ParentFolder & "\..\Logging"
    FullPath     = FSO.GetFile(WScript.ScriptFullName).ParentFolder & relpath
    GetFullName  = FSO.GetFile(FullPath).Path
    Set FSO      = Nothing

    MsgTxt = "The object returned the following error: " & Chr(13) & Chr(13) & _
             "Error code: 0x" & Hex(Err.Number) & Chr(13) & _
             "Source: " & Err.Source & Chr(13) & _
             "Description: " & Err.Description
    
    If Err.Number Then
      MsgBox MsgTxt & Chr(10) & "(" & Err.Description & ")", VBOKOnly + VBCritical, "Error" 
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


