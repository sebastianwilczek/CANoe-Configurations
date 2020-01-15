  ' This example shows some special features only available in the COM-Exporter.
  ' The example can be used with the CANoe CANSystemDemo Demo.
  '
  '
  ' The COM-Exporter has two special features used here:
  ' 1. The batch export/conversion of more than one source files to the same 
  '    number of destination files (n to n) 
  ' 2. The batch expor/conversion of more than one source file to one 
  '    destination file (n to 1)
  '
  ' As source files the logging files configured in the 2nd and 3rd logging
  ' are used. 
  '
  ' Because by default the 2nd and 3rd logging block in the CANSystemDemo Demo
  ' are not configured to start automatically the logging at measurement start 
  ' you first have to make sure that logging files are created by:
  ' 1. starting the logging blocks manually in measurement setup.
  ' 2. changing the trigger configuration of the logging blocks.
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
  OpenConfig ("\..\CANSystemDemo.cfg")

  Set Logging  = App.Configuration.OnlineSetup.LoggingCollection(2)
  Set Logging2 = App.Configuration.OnlineSetup.LoggingCollection(3)
  Set Exporter = Logging.Exporter

  ' Do n to n conversion.
  ' Specify two source files and two logging files and start conversion.
  With Exporter.Sources
    .Clear
    .Add(Logging.FullName)
    .Add(Logging2.FullName)
  End With

  With Exporter.Destinations
    .Clear
    .Add(CSVFolder & "\N_TO_N_1.asc")
    .Add(CSVFolder & "\N_TO_N_2.asc")
  End With

  Exporter.Load
  Exporter.Save True

  MsgBox "The source files" & Chr(13) & _
         Exporter.Sources(1) & " and" & Chr(13) & Exporter.Sources(2) & Chr(13) & _
         "were successfully converted to" & Chr(13) & _
         "..\Logging" & "\N_TO_N_1.asc and" & Chr(13) & _ 
         "..\Logging" & "\N_TO_N_2.asc"

  ' Do n to 1 conversion.
  ' Two source files are specified. No specify only one destination file.
  With Exporter.Destinations
    .Clear
    .Add(CSVFolder & "\N_TO_1.asc")
  End With

  Exporter.Save True

  MsgBox "The source files" & Chr(13) & _
         Exporter.Sources(1) & " and" & Chr(13) & Exporter.Sources(2) & Chr(13) & _
         "were successfully converted to" & Chr(13) & _
         "..\Logging" & "\N_TO_1.asc"

  Set App      = Nothing
  Set Logging  = Nothing
  Set Logging2 = Nothing
  Set Exporter = Nothing
  
  MsgBox "Logging specials script done..."

  Function GetFullName(relpath)
    Dim FSO, FullPath, MsgTxt

    On Error Resume Next
    Set FSO     = CreateObject("Scripting.FileSystemObject")
    FullPath    = FSO.GetFile(WScript.ScriptFullName).ParentFolder & relpath
    CSVFolder   = FSO.GetFile(WScript.ScriptFullName).ParentFolder & "\..\Logging"
    GetFullName = FSO.GetFile(FullPath).Path
    Set FSO     = Nothing
  
    If Err.Number Then
      MsgTxt = "The object returned the following error: " & Chr(13) & Chr(13) & _
               "Error code: 0x" & Hex(Err.Number) & Chr(13) & _
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
      MsgBox "Error opening configuration!" & Chr(13) & _
             "Error code: 0x" & Hex(Err.Number) & Chr(13) & _
             "Source: " & Err.Source & Chr(13) & _
             "Description: " & Err.Description, VBOKOnly + VBCritical, "Error"
      WScript.Quit
    End If
  End Sub
