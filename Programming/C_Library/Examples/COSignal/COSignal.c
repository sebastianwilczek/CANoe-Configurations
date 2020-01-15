// FBSignal.c : Example of a CANalyzer/CANoe C Library
//
// This sample file demonstrates the usage of Function Bus signals in a 
// CANalyzer/CANoe C Library.
//


#include "CCL/CCL.h"
#include <math.h>
#include <stdio.h>

extern void OnMeasurementPreStart();
extern void OnMeasurementStart();
extern void OnMeasurementStop();
extern void OnTimer(cclTime time, int timerID);
extern void OnFloatSignalReceiver1(cclTime time, cclValueID signalID);
extern void OnFloatSignalReceiver2(cclTime time, cclValueID signalID);

int gTimerID;
cclValueID gFloatSender1ID;
cclValueID gFloatReceiver1ID;
cclValueID gFloatReceiver2ID;
cclValueID gFieldAID;
cclValueID gFieldBID;
cclValueID gFieldCID[10];


void cclOnDllLoad()
{
  cclSetMeasurementPreStartHandler(&OnMeasurementPreStart);
  cclSetMeasurementStartHandler(&OnMeasurementStart);
}


void OnMeasurementPreStart()
{
  gTimerID = cclTimerCreate(&OnTimer);

  // identification string of a Function Bus broadcast signal value has the following form:
  // "namespace::signal[sender]" or "namespace::signal[receiver]"
  gFloatSender1ID = cclValueGetID(CCL_COMMUNICATION_OBJECTS, "Signals::FloatSignal[Sender1]", "", CCL_IMPL);
  if (gFloatSender1ID < 0)
    cclWrite("Error: Signals::FloatSignal[Sender1] not accessible");

  gFloatReceiver1ID = cclValueGetID(CCL_COMMUNICATION_OBJECTS, "Signals::FloatSignal[Receiver1]", "", CCL_IMPL);
  if (gFloatReceiver1ID < 0)
    cclWrite("Error: Signals::FloatSignal[Receiver1] not accessible");

  gFloatReceiver2ID = cclValueGetID(CCL_COMMUNICATION_OBJECTS, "Signals::FloatSignal[Receiver2]", "", CCL_IMPL);
  if (gFloatReceiver2ID < 0)
    cclWrite("Error: Signals::FloatSignal[Receiver2] not accessible");

  if (cclValueSetHandler(gFloatReceiver1ID, false, OnFloatSignalReceiver1) < 0)
    cclWrite("Error: failed to set handler for FloatSignal[Receiver1]");

  if (cclValueSetHandler(gFloatReceiver2ID, false, OnFloatSignalReceiver2) < 0)
    cclWrite("Error: failed to set handler for FloatSignal[Receiver2]");

  // member string syntax is similar to C structs and arrays:
  // "field[.subfield]" for structs
  gFieldAID = cclValueGetID(CCL_COMMUNICATION_OBJECTS, "Signals::StructSignal[Sender1]", "FieldA", CCL_IMPL);
  if (gFieldAID < 0)
    cclWrite("Error: Signals::StructSignal[Sender1].FieldA not accessible");

  gFieldBID = cclValueGetID(CCL_COMMUNICATION_OBJECTS, "Signals::StructSignal[Sender1]", "FieldB", CCL_PHYS);
  if (gFieldBID < 0)
    cclWrite("Error: Signals::StructSignal[Sender1].FieldB not accessible");

  // "field[index]" for arrays
  for (int i = 0; i < 10; ++i)
  {
    static char member[16];
    sprintf_s(member, 16, "FieldC[%d]", i);
    gFieldCID[i] = cclValueGetID(CCL_COMMUNICATION_OBJECTS, "Signals::StructSignal[Sender1]", member, CCL_RAW);
    if (gFieldCID[i] < 0)
      cclWrite("Error: Signals::StructSignal[Sender1].FieldC[i] not accessible");
  }
}


void OnMeasurementStart()
{
  if (cclTimerSet(gTimerID, cclTimeMilliseconds(5)) < 0)
    cclWrite("Error: failed to set timer");

  if (cclValueSetInteger(gFieldBID, 500) < 0)
    cclWrite("Error: failed to set StructSignal.FieldB value");
}


void OnTimer(cclTime time, int timerID)
{
  double value = sin((double)time/1000000000.0);
  if (cclValueSetFloat(gFloatSender1ID, value) < 0)
    cclWrite("Error: failed to set FloatSignal value");

  if (cclTimerSet(gTimerID, cclTimeMilliseconds(5)) < 0)
    cclWrite("Error: failed to set timer");
}


void OnFloatSignalReceiver1(cclTime time, cclValueID signalID)
{
  double value;

  if (cclValueGetFloat(gFloatReceiver1ID, &value) < 0)
    cclWrite("Error: failed to get FloatSignal value from Receiver1");

  if (cclValueSetFloat(gFieldAID, (double)(value * 20000000000000.0 / time)) < 0)
    cclWrite("Error: failed to set StructSignal.FieldA");
}


void OnFloatSignalReceiver2(cclTime time, cclValueID signalID)
{
  long long x;
  if (cclValueGetInteger(gFieldBID, &x) < 0)
    cclWrite("Error: failed to get StructSignal.FieldB value");

  if (cclValueSetInteger(gFieldBID, (x + 10) % 20000) < 0)
    cclWrite("Error: failed to set StructSignal.FieldB value");

  for (int i = 0; i < 10; ++i)
  {
    if (cclValueSetInteger(gFieldCID[i], 500 + i * 100) < 0)
      cclWrite("Error: failed to set StructSignal.FieldC[i] value");
  }
}