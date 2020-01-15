'-----------------------------------------------------------------------------
' This VB-Script demonstrates how to get access to the COM interface
' "MostNetworkInterface" and how to send MOST messages using this interface.
'-----------------------------------------------------------------------------
' Copyright (c) 2005 - 2008 by Vector Informatik GmbH.  All rights reserved.
'-----------------------------------------------------------------------------

Option Explicit

Dim ToolInfo, CANoeApp, Measurement, MostNetworkInterface, i

' Get the 'ToolInfo' interface
Set ToolInfo = CreateObject("CANutil.ToolInfo")

' Show disclaimer 
If ToolInfo.ShowApplicationDisclaimer("CANoe.Application") <> 1 Then
  Set ToolInfo = Nothing
  WScript.Quit
End If
Set ToolInfo = Nothing

'-----------------------------------------------------------------------------
' Get the 'Application' interfaces
'-----------------------------------------------------------------------------
Set CANoeApp = CreateObject("CANoe.Application")

' Check the name of the loaded configuration
If Not CANoeApp.Configuration.Name = "MOSTSystemDemo" Then
  MsgBox "Please load 'MOST System Demo' before running this script.", vbOKOnly, "Error"
  CANoeApp = Nothing
  WScript.Quit
End If

' Get the 'Measurement' interface
Set Measurement = CANoeApp.Measurement

'-----------------------------------------------------------------------------
' Get the 'MostNetworkInterface' interface
' There are several possibilities to traverse the COM hierarchy to get the
' NetworkInterface of a MOST Channel. The one used below is the easiest.
' Consult the CANoe help for more details.
'-----------------------------------------------------------------------------
Set MostNetworkInterface = CANoeApp.Networks("MOST Ring").Devices("MOST 1").NetworkInterface

' Start measurement
If Not Measurement.Running Then
  WScript.Echo "Start measurement"
  Measurement.Start()
  Do While Not Measurement.Running
    WScript.Sleep(100)
  Loop
End If
WScript.Sleep(2000)

'-----------------------------------------------------------------------------
' Send some messages to manipulate the AudioDiskPlayer and the AudioAmplifier
' of the MOSTSystemDemo simulation.
'-----------------------------------------------------------------------------

For i = 1 To 3
  ' Send 'AudioDiskPlayer.00.DeckStatus.Set(Pause)'
  WScript.Echo "Pause playing CD"
  MostNetworkInterface.OutputAMS &HFFFF, &H31, &H0, &H200, &H0, 1, Array(&H2)
  WScript.Sleep(2000)

  ' Send 'AudioDiskPlayer.00.DeckStatus.Set(Play)'
  WScript.Echo "Resume playing CD"
  MostNetworkInterface.OutputAMS &HFFFF, &H31, &H0, &H200, &H0, 1, Array(&H0)
  WScript.Sleep(2000)

  ' Send 'AudioAmplifier.00.Volume.Increment(05)'
  WScript.Echo "Increment volume"
  MostNetworkInterface.OutputAMS &HFFFF, &H22, &H0, &H400, &H3, 1, Array(&H5)
  WScript.Sleep(2000)

  ' Send 'AudioAmplifier.00.Volume.Decrement(05)'
  WScript.Echo "Decrement volume"
  MostNetworkInterface.OutputAMS &HFFFF, &H22, &H0, &H400, &H4, 1, Array(&H5)
  WScript.Sleep(2000)
Next

' Send 'AudioDiskPlayer.00.DeckStatus.Set(Stop)'
WScript.Echo "Stop playing CD"
MostNetworkInterface.OutputAMS &HFFFF, &H31, &H0, &H200, &H0, 1, Array(&H1)
WScript.Sleep(2000)

' Stop measurement
WScript.Echo "Stop measurement"
Measurement.Stop()

'-----------------------------------------------------------------------------
' Cleanup: Release COM interfaces
'-----------------------------------------------------------------------------
Set MostNetworkInterface = Nothing
Set Measurement = Nothing
Set CANoeApp = Nothing
