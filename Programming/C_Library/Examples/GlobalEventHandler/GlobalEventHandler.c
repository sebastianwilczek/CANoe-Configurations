// GlobalEventHandler.c : Example of a CANalyzer/CANoe C Library
//
// This sample file demonstrates general structure of a  
// CANalyzer/CANoe C Library.
//
// The function 'cclOnDllLoad' must be implemented in every 
// CANalyzer/CANoe C Library. In the body of this function additional 
// event handlers (DllUnLoad, MeasurementPreStart, MeasurementStart, 
// MeasurementStop) can be registered.
// 

#include "CCL/CCL.h"


extern void OnDllUnLoad();
extern void OnMeasurementPreStart();
extern void OnMeasurementStart();
extern void OnMeasurementStop();


void cclOnDllLoad()
{
  cclWrite("C Library Example: cclOnDllLoad");
  
  cclSetDllUnloadHandler(&OnDllUnLoad);
  cclSetMeasurementPreStartHandler(&OnMeasurementPreStart);
  cclSetMeasurementStartHandler(&OnMeasurementStart);
  cclSetMeasurementStopHandler(&OnMeasurementStop);
}


void OnDllUnLoad()
{
  cclWrite("C Library Example: OnDllUnLoad");
}


void OnMeasurementPreStart()
{
  char path[256];
  int rc;

  cclPrintf("C Library Example: %s", "OnMeasurementPreStart");
  rc = cclGetUserFilePath("CCL-Info.txt", path, sizeof(path));
  cclPrintf("C Library Example: Path of file CCL-Info.txt is '%s'", (rc==CCL_SUCCESS)?path:"---");
}


void OnMeasurementStart()
{
  cclWrite("C Library Example: OnMeasurementStart");
}


void OnMeasurementStop()
{
  cclWrite("C Library Example: OnMeasurementStop");
}
