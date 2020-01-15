// Signal.c : Example of a CANalyzer/CANoe C Library
//
// This sample file demonstrates the usage of signals in a 
// CANalyzer/CANoe C Library.
//


#include "CCL/CCL.h"


extern void OnMeasurementPreStart();
extern void OnMeasurementStart();
extern void OnMeasurementStop();
extern void OnTimer(long long time, int timerID);
extern void OnSignal_B2(int signalID);

int gTimerID;
int gSignalID_A1;
int gSignalID_A2;
int gSignalID_B1;
int gSignalID_B2;


void cclOnDllLoad()
{
  cclSetMeasurementPreStartHandler(&OnMeasurementPreStart);
  cclSetMeasurementStartHandler(&OnMeasurementStart);
}


void OnMeasurementPreStart()
{
  gTimerID = cclTimerCreate(&OnTimer);

  // identification string of a signal has the following form:
  // [[[[network_name::]db_name::]node_name::]msg_name::]signal_name
  gSignalID_A1 = cclSignalGetID("Signal_A1");  // if signal name is unique in the system, we don't need any more
  gSignalID_A2 = cclSignalGetID("PDU_A::Signal_A2");
  gSignalID_B1 = cclSignalGetID("ECU_B::PDU_B::Signal_B1");
  gSignalID_B2 = cclSignalGetID("X_Bus::Example_DB::ECU_B::PDU_B::Signal_B2"); // full qualified

  cclSignalSetHandler(gSignalID_A2, OnSignal_B2);
}


void OnMeasurementStart()
{
  cclTimerSet(gTimerID, cclTimeMilliseconds(5) );
}


void OnTimer(long long time, int timerID)
{
  int rc;
  double value_A1;
  double value_B1;

  rc = cclSignalGetRxPhysDouble(gSignalID_A1, &value_A1);
  value_B1 = -value_A1;
  rc = cclSignalSetTxPhysDouble(gSignalID_B1, value_B1);

  cclTimerSet(gTimerID, cclTimeMilliseconds(5) );
}


void OnSignal_B2(int signalID)
{
  int rc;
  long long value_A2;
  long long value_B2;

  rc = cclSignalGetRxRawInteger(gSignalID_A2, &value_A2);
  value_B2 = (value_A2 + 50) & 0xFF;
  rc = cclSignalSetTxRawInteger(gSignalID_B2, value_B2);
}