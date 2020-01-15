' Example: TestFeatureSet
'
' This script demonstrates the usage of some COM-functions to control a 
' test setup.
'
' Exact description of the script:
' 1) First test configuration is chosen.
' 2) Check if the selected test configuration has already a verdict
'    a) if no verdict is available
'       - show information
'       - Start measurement
'       - Start test configuration
'    b) if verdict is available
'       - show information 
' 3) The test units of the selected test configuration 
'    are displayed
' 4) All passed and all failed testcases are listed
' 5) All passed testcases are disabled
' 6) The test configuration is started again
' 7) The verdict of the test configuration is shown
' 8) All  failed testcases are listed
' 
'
' The display is done in the Write-Window of the configuration.  
'
'-----------------------------------------------------------------------------
' Copyright (c) 2008 by Vector Informatik GmbH.  All rights reserved.

  Dim testconfigCount
  Dim currentTestConfig
  Dim IsTestConfig, IsMeasurementRunning
  Dim foundTestConfig, testConfigIsFound
  
  IsTestConfig = False
  IsMeasurementRunning = False
  testConfigIsFound = False

  ' -----------------------------------------------------------------------------
  ' Display the CANoe disclaimer
  ' -----------------------------------------------------------------------------
  ' get the ToolInfo object
  Dim tool
  Set tool = CreateObject( "CANutil.ToolInfo")

  ' show disclaimer (language, strict, beta, education)
  Dim language, strict, beta, education
  language = 1
  strict = 1
  beta = 0
  education = 0
  If tool.ShowDisclaimer( language, strict, beta, education) <> 1 Then
    WScript.quit 1
  End If

  ' release the ToolInfo object
  Set tool = nothing
  
  ' -----------------------------------------------------------------------------
' Start CANoe
' -----------------------------------------------------------------------------
  ' connect to CANoe
  On Error Resume Next
  Set App = CreateObject("canoe.application")
  ExceptionHandling Err.Number, "CANoe run failure"
  On Error Resume Next

  If WScript.Version < "5.1" Then
    busy = 0
  Else
    busy = 1 ' set busy flag
    WScript.ConnectObject App, "App_"
  End If

  
  Do 
    Err.Clear
    If Len(App.configuration.Name) = 0 Then
        PopUpMsgBoxError "No CANoe configuration is opened."
    Else
        busy = 0
    End If

    WScript.Sleep 500
  Loop While busy = 1 'Error: User interface is busy
  
  On Error Resume Next
  Set WriteWindow = App.UI.Write
  ExceptionHandling Err.Number, "Write window connection failure"
  
  WriteWindow.Output "TFS Example: TestFeatureSet Example is started"

  ' -----------------------------------------------------------------------------
  ' Get first test configuration
  ' -----------------------------------------------------------------------------
 
  WScript.ConnectObject App.Measurement, "Meas_"
  
  On Error Resume Next
  ' get the number of all test configurations
  testconfigCount = App.configuration.TestConfigurations.Count
  
  'no test configuration available
  If testconfigCount = 0 Then
    PopUpMsgBoxError "The current configuration does not contain any test configurations. The script is aborted."
  End If
   
  Set currentTestConfig = App.configuration.TestConfigurations.Item(1)

  ' -----------------------------------------------------------------------------
  ' Get verdict of first test configuration
  ' Write the structure of the test configuration in the write window
  ' List all passed and failed test cases in the write window
  ' -----------------------------------------------------------------------------
  
  ' set up connection to event handlers
  WScript.ConnectObject currentTestConfig, "TestConfig_"
  
  if currentTestConfig.Verdict = 0 then 'verdict not available
    MsgBox "TFS Example: The verdict of the first test configuration is not available."_
                & vbNewLine & vbNewLine & _
                "Therefore the script starts the test configuration."_
                & vbNewLine & "Afterwards the structure of the test configuration "_
                & vbNewLIne &  "and the verdicts of all test cases are displayed in the write window."
    
    If Not App.Measurement.Running Then
      'Start measurement
      On Error Resume Next
      App.CAPL.Compile
      ExceptionHandling Err.Number, "CAPL compile failure"
  
      App.Measurement.Start
    
      While (Not IsMeasurementRunning)
        WScript.Sleep(200)
      Wend
    End If
              
    currentTestConfig.Start()
    WScript.Sleep(200)
    
    ' Wait until test configuration is stopped
    While (IsTestConfig)
      WScript.Sleep(200)
    Wend
    WScript.Sleep(2000)
    
    
  else
    MsgBox "TFS Example: The first test configuration has already been executed."_
                & vbNewLine & vbNewLine & _
                "The structure of the test configuration and the verdicts"_
                & vbNewLIne &  "of all test cases are displayed in the write window."
  End If
  

      
  'iterate through all test configuration items (test units, test groups, test fixtures, test sequences and testcases)
  WriteWindow.Output
  WriteWindow.Output "TFS Example: Test structure of the current test configuration '" & currentTestConfig.Name & "':"
  WriteWindow.Output
  
  GetAllItems currentTestConfig, ""
  WriteWindow.Output
 
  ' List the passed and failed test cases
  WriteWindow.Output
  WriteWindow.Output "TFS Example: List of the passed test cases:"
  GetAllTestcasesWithVerdict currentTestConfig, 1, false
  
  WriteWindow.Output
  WriteWindow.Output "TFS Example: List of the failed test cases:"
  GetAllTestcasesWithVerdict currentTestConfig, 2, false
  WriteWindow.Output
   
  ' -----------------------------------------------------------------------------
  ' Disable all testcases with verdict "passed"
  ' -----------------------------------------------------------------------------
  
  GetAllTestcasesWithVerdict currentTestConfig, 1, true
  
  MsgBox "TFS Example: All test cases with verdict 'passed' are disabled by the script."_
              & vbNewLine & "Please review the selection of the test cases."_
              & vbNewLine & vbNewLine & "Click 'OK' to start the test configuration again."     
  
  ' -----------------------------------------------------------------------------
  ' Start the current test configuration again
  ' -----------------------------------------------------------------------------
  
  if Not App.Measurement.Running Then
    
    'Start measurement
    On Error Resume Next
    App.CAPL.Compile
    ExceptionHandling Err.Number, "CAPL compile failure"
  
    App.Measurement.Start
    
    While (Not IsMeasurementRunning)
      WScript.Sleep(200)
    Wend
  End If
                
  WriteWindow.Output "TFS Example: Start the current test configuration again."
  WriteWindow.Output
  currentTestConfig.Start()
  WScript.Sleep(200)
  
  ' Wait until test configuration is stopped
  While (IsTestConfig)
    WScript.Sleep(200)
  Wend
  WScript.Sleep(2000)

  ' -----------------------------------------------------------------------------
  ' Display the verdict of the test configuration
  ' -----------------------------------------------------------------------------
  
  WriteWindow.Output
  Dim stringVerdict
  Select Case (currentTestConfig.Verdict)
      Case 0
        stringVerdict = "Verdict not available"
      Case 1
        stringVerdict = "PASSED"
      Case 2
        stringVerdict = "FAILED"
    End Select
  WriteWindow.Output "TFS Example: Verdict of the test configuration is: " & stringVerdict
  
  
  MsgBox "TFS Example: Verdict of the test configuration is: " & stringVerdict _
                & vbNewLine & vbNewLine & _
                "The script is finished."_
                & vbNewLine & "Its sequence is documented in the write window."
  Set stringVerdict = Nothing
  
  WriteWindow.Output
  WriteWindow.Output "TFS Example: List of the failed test cases:"
  GetAllTestcasesWithVerdict currentTestConfig, 2, false
  
  ' -----------------------------------------------------------------------------
  ' Finish the script
  ' -----------------------------------------------------------------------------             
  WriteWindow.Output
  WriteWindow.Output "TFS Example: TestFeatureSet Example finished."

  Set testconfigCount = Nothing
  Set setupCount = Nothing
  Set currentTestConfig = Nothing
  Set foundTestConfig = Nothing
  Set testConfigIsFound = Nothing
  Set IsTestConfig = Nothing
  Set IsMeasurementRunning = Nothing
  Set WriteWindow = Nothing
  Set App = Nothing
  Set FSO = Nothing
  
  







' =============================================================================
' script warning: pop up MsgBox
' =============================================================================
  Sub PopUpMsgBoxWarning(text)
    msgbox text , vbOKOnly , cMsgBoxLabel
  End Sub

' =============================================================================
' script error: pop up MsgBox and exit script
' =============================================================================
  Sub PopUpMsgBoxError(text)
    msgbox text , vbOKOnly , cMsgBoxLabel
    
    ' stops the measurement
    If App.Measurement.Running Then
      App.Measurement.Stop
    End If
 
    WScript.quit 1
  End Sub
  
  ' =============================================================================
' Exception handling
' =============================================================================
  Private Sub ExceptionHandling (errNum, msgDescr)
    If Err.Number <> CLng (0) Then
      If (IsNull (msgDescr) OR IsEmpty (msgDescr) OR msgDescr = "") Then
        msgDescr = "???"
      End If
      MsgBox "Error # " & CStr(Err.Number) & " " & Err.Description & " " & Err.Source & "."_
                        & vbNewLine & msgDescr  & "."_
                        & vbNewLine & "Invalid CANoe installation?" & vbNewLine & "The Script is aborted!"
      Err.Clear   ' Clear the error.
      Set App = Nothing
      Set FSO = Nothing
      Set F = Nothing
      WScript.quit 1
    End If
  End sub

  
  ' =============================================================================
  ' Returns all items of the first test configuration
  ' =============================================================================
  Private Function GetAllItems(element, engageString)
    Dim subElementsCount, i, subElement
    
    subElementsCount = 0
    
    on error resume next
    subElementsCount = element.Elements.Count
    
    if subElementsCount <> 0 Then
      
      for i = 1 to subElementsCount
        Set subElement = element.Elements.Item(i)
        WriteWindow.Output engageString & subElement.Caption
        
        engageString = engageString & "    "
        GetAllItems subElement, engageString
        engageString = Left(engageString, Len(engageString)-4) 
      Next
      
    End If
    
    Set subElementsCount = Nothing
    Set subElement = Nothing 
  End Function
  
  ' =============================================================================
  ' Goes through all test cases of a test configuration.
  ' If "disable" is true, all test cases with verdict "verdict" are disabled.
  ' If "disable" is false, then the names of the testcases with verdict "verdict"
  ' are written to the write window.
  ' =============================================================================
  Private Function GetAllTestcasesWithVerdict(element, verdict, disabled)
    Dim subElementsCount, i, subElement, verdictSubElement
    
    subElementsCount = 0
    
    on error resume next
    subElementsCount = element.Elements.Count
    
    if subElementsCount <> 0 Then
      
      for i = 1 to subElementsCount
        Set subElement = element.Elements.Item(i)
        verdictSubElement = 0
        verdictSubElement = subElement.Verdict
         
        if verdictSubElement = verdict Then
          if disabled = false then
            WriteWindow.Output "    " & subElement.Caption
          else
            subElement.Enabled = false
          End If
        End If
        
        GetAllTestcasesWithVerdict subElement, verdict, disabled
      Next
      
    End If
    
    Set subElementsCount = Nothing
    Set subElement = Nothing
    Set verdictSubElement = Nothing
  End Function
  
  ' =============================================================================
  '  Events 
  ' =============================================================================
  
  Sub TestConfig_OnStart()
    IsTestConfig = True
  End Sub
  
  Sub TestConfig_OnStop(reason)
    IsTestConfig = False
    Select Case (reason)
      Case 1
        PopUpMsgBoxError "Test configuration was stopped by the user."
      Case 2
        PopUpMsgBoxError "Test configuration was stopped by measurement stop"
    End Select

  End Sub

  Sub Meas_OnStart()
    IsMeasurementRunning = True
  End Sub
  
  Sub Meas_OnStop()
    IsMeasurementRunning = False
  End Sub
