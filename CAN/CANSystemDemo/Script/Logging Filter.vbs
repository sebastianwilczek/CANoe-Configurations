  ' This example shows how a filter can be used to specify the signals and
  ' messages that should be exported/converted.
  ' The example can be used with the CANoe CANSystemDemo Demo.
  '
  '
  ' The example adds all the signals and messages found in the source
  ' file after the loading process to the filter and enables the filter.
  ' After that the source file will be converted to Filter.asc and exported
  ' to Filter.csv
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
  '----------------------------------------------------------------------------
  ' Copyright (c) 2005 by Vector Informatik GmbH.  All rights reserved.

  Dim CSVFolder  

  Set Tool = CreateObject("CANutil.ToolInfo")

  ' show disclaimer 
  If Tool.ShowApplicationDisclaimer("CANoe.Application") <> 1 Then
    Wscript.Quit
  End If

  Set App         = CreateObject("CANoe.Application")
  Set Measurement = App.Measurement

  If Measurement.Running Then
    Measurement.Stop
  End If

  Wscript.Sleep 500
  OpenConfig("\..\CANSystemDemo.cfg")

  Set Logging   = App.Configuration.OnlineSetup.LoggingCollection(2)
  Set Exporter  = Logging.Exporter
  Set Expfilter = Exporter.Filter

  ' Load source file
  Exporter.Load

  ' Add symbols (signals or system variables) to filter
  For Each Symbol In Exporter.Symbols
    Expfilter.Add(Symbol.FullName)
  Next

  ' Add messages to filter
  For Each Message In Exporter.Messages
    Expfilter.Add(Message.FullName)
  Next

  ' Enable filter
  Expfilter.Enabled = True

  ' do conversion to ASC
  With Exporter.Destinations
    .Clear
    .Add(CSVFolder & "\Filter.asc")
  End With

  Exporter.Save True

  ' do export to CSV
  With Exporter.Destinations
    .Clear
    .Add(CSVFolder & "\Filter.csv")
  End With

  Exporter.Save True

  MsgBox "The logging file" & Chr(13) & _
         Exporter.Sources(1) & Chr(13) & _
         "was successfully converted/exported to " & Chr(13) & _
         "..\Logging" & "\Filter.csv and" & Chr(13) & _
         "..\Logging" & "\Filter.asc"

  Set Expfilter = Nothing
  Set Exporter  = Nothing
  Set Logging   = Nothing
  Set App       = Nothing
  
  MsgBox "Logging filter script done..."

  Function GetFullName(relpath)
    Dim FSO, FullPath, MsgTxt

    On Error Resume Next
    Set FSO     = CreateObject("Scripting.FileSystemObject")
    FullPath    = FSO.GetFile(WScript.ScriptFullName).ParentFolder & relpath
    CSVFolder   = FSO.GetFile(WScript.ScriptFullName).ParentFolder & "\..\Logging"
    GetFullName = FSO.GetFile(FullPath).Path
    Set FSO     = Nothing

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
