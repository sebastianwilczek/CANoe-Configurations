  ' This example shows how to change the settings of an exporter.
  ' The example can be used with the CANoe CANSystemDemo Demo.
  '
  '
  ' The CSV settings will be changed programatically and two CSV files with 
  ' different settings will be created.
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

  Set Logging  = App.Configuration.OnlineSetup.LoggingCollection(2)
  Set Exporter = Logging.Exporter

  ' Load file 
  Exporter.Load

  ' Do CSV export with 1st settings
  With Exporter.Settings.CSVExt
    .ColumnSeparator = ";"
    .DecimalSymbol = ","
    .DecimalPlacesForSignalValue = 6
    .DecimalPlacesForSignalValue = 6
    .TimeStampMode = 0
    .ExportMode = 0
  End With

  With Exporter.Destinations
    .Clear
    .Add(CSVFolder & "\CSVSettings1.csv")
  End With    

  Exporter.Save True

  ' Do CSV export with 2nd settings
  With Exporter.Settings.CSVExt
    .ColumnSeparator = ";"
    .DecimalSymbol = ","
    .DecimalPlacesForSignalValue = 2
    .DecimalPlacesForSignalValue = 2
    .TimeStampMode = 1
    .ExportMode = 0
  End With

  With Exporter.Destinations
    .Clear
    .Add(CSVFolder & "\CSVSettings2.csv")
  End With

  Exporter.Save True

  MsgBox "The logging file" & Chr(13) & _
         Exporter.Sources(1) & Chr(13) & _
         "was successfully exported to" & Chr(13) & _
         "..\Logging" & "\CSVSettings1.csv and" & Chr(13) & _
         "..\Logging" & "\CSVSettings2.csv"

  Set Exporter = Nothing
  Set Logging  = Nothing
  Set App      = Nothing

  MsgBox "Logging settings script done..."

  Function GetFullName(relpath)
    Dim FSO, FullPath, MsgTxt

    On Error Resume Next
    Set FSO      = CreateObject("Scripting.FileSystemObject")
    CSVFolder    = FSO.GetFile(WScript.ScriptFullName).ParentFolder & "\..\Logging"
    FullPath     = FSO.GetFile(WScript.ScriptFullName).ParentFolder & relpath
    GetFullName  = FSO.GetFile(FullPath).Path
    Set FSO      = Nothing

    MsgTxt = "The object returned the following error: " & Chr(13) & Chr(13) & _
             "Code: 0x" & Hex(Err.Number) & Chr(13) & _
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
