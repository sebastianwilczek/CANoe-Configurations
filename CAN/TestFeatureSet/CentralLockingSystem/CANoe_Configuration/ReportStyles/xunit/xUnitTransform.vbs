'-----------------------------------------------------------------------------
' This VBScript uses MSXML to transform CANoe xml report files to JUnit report files
' using the transformation script xunit_coe.xslt.
' requires 2 parameters:
'  1st: path to CANoe test report xml file
'  2nd: path to transformed output file in JUnit format
'-----------------------------------------------------------------------------
' Copyright (c) 2016 by Vector Informatik GmbH. All rights reserved.

Option Explicit

Dim inputFile, outputFile

If WScript.Arguments.Count < 2 Then
  WScript.Echo "Missing required arguments: <CANoe test result xml-file to transform> <path+filename of transformed output>"
  WScrit.Quit(1)
Elseif WScript.Arguments.Count > 2 Then
  WScript.Echo "ERROR: Too many arguments!"
  WScript.Echo "Required arguments: <CANoe test result xml-file to transform> <path+filename of transformed output>"
  For Each arg In WScript.Arguments
    WScript.Echo(" - found: " & arg) 
  Next
  WScrit.Quit(1)
Else
	WScript.Echo("input file: " & WScript.Arguments(0))
    inputFile = "" & WScript.Arguments(0)
	WScript.Echo("output file: " & WScript.Arguments(1))
	outputFile = "" & WScript.Arguments(1)
End If

Dim path
path = CreateObject("Scripting.FileSystemObject").GetParentFolderName(WScript.ScriptFullName)

Dim xmlDocument, xmlStylesheet, fso, jUnitFile
Set xmlDocument = CreateObject("Msxml2.DOMDocument.6.0")
xmlDocument.async = False
xmlDocument.load(inputFile)

Set xmlStylesheet = CreateObject("Msxml2.DOMDocument.6.0")
xmlStylesheet.async = False
xmlStylesheet.load( path & "\xunit_coe.xslt")
WScript.Echo("Opening xslt-file: " & path & "\xunit_coe.xslt")

Set fso=CreateObject("Scripting.FileSystemObject")
Set jUnitFile = fso.CreateTextFile(outputFile, True)
WScript.Echo("** Transforming CANoe-Testresult <" & inputFile &"> to JUnit <"& outputFile &"> **")

jUnitFile.Write xmlDocument.transformNode(xmlStylesheet)
jUnitFile.Close

Set jUnitFile = Nothing
Set fso = Nothing
Set xmlDocument = Nothing
Set xmlStylesheet = Nothing
