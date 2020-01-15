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
extern void OnServiceFunctionTimer(cclTime time, int timerID);
extern void OnFreeFunctionTimer(cclTime time, int timerID);
extern void SimulateServer(cclTime time, cclCallContextID ccID);

int gServiceFunctionTimerID;
int gFreeFunctionTimerID;
cclServiceID gProvidedServiceID;
cclFunctionID gServiceFunctionClient1ID;
cclFunctionID gServiceFunctionClient2ID;
cclFunctionID gServiceFunctionServer1ID;
cclFunctionID gFreeFunctionClient1ID;
cclFunctionID gFreeFunctionServer1ID;
cclFunctionID gFreeFunctionServer2ID;

const char* gServiceFunctionClient1Port = "FunctionCalls::TheService.consumerSide[Client1,Server1].SimpleFunc";
const char* gServiceFunctionClient2Port = "FunctionCalls::TheService.consumerSide[Client2,Server1].SimpleFunc";
const char* gServiceFunctionServer1Port = "FunctionCalls::TheService.providerSide[Client1,Server1].SimpleFunc";
const char* gFreeFunctionClient1Port = "FunctionCalls::TheFunction.consumerSide[Client1,Server1]";
const char* gFreeFunctionServer1Port = "FunctionCalls::TheFunction.providerSide[Client1,Server1]";
const char* gFreeFunctionServer2Port = "FunctionCalls::TheFunction.providerSide[Client1,Server2]";

void cclOnDllLoad()
{
  cclSetMeasurementPreStartHandler(&OnMeasurementPreStart);
  cclSetMeasurementStartHandler(&OnMeasurementStart);
}


void OnMeasurementPreStart()
{
  gServiceFunctionTimerID = cclTimerCreate(&OnServiceFunctionTimer);
  gFreeFunctionTimerID = cclTimerCreate(&OnFreeFunctionTimer);

  gProvidedServiceID = cclProvidedServiceGetID("FunctionCalls::TheService.providerSide[Client1,Server1]");
  if (gProvidedServiceID < 0)
    cclWrite("Error: FunctionCalls::TheService.providerSide[Client1,Server1] not accessible");

  // identification string of a Function Bus service function has the following form:
  // "namespace::service.consumerSide[consumer,provider].function"
  gServiceFunctionClient1ID = cclFunctionGetID(gServiceFunctionClient1Port);
  if (gServiceFunctionClient1ID < 0)
    cclWrite("Error: FunctionCalls::TheService.consumerSide[Client1,Server1].SimpleFunc not accessible");

  gServiceFunctionClient2ID = cclFunctionGetID(gServiceFunctionClient2Port);
  if (gServiceFunctionClient2ID < 0)
    cclWrite("Error: FunctionCalls::TheService.consumerSide[Client2,Server1].SimpleFunc not accessible");

  // "namespace::service.providerSide[consumer,provider].function"
  gServiceFunctionServer1ID = cclFunctionGetID(gServiceFunctionServer1Port);
  if (gServiceFunctionServer1ID < 0)
    cclWrite("Error: FunctionCalls::TheService.providerSide[Client1,Server1].SimpleFunc not accessible");

  // identification string of a Function Bus free function has the following form:
  // "namespace::function.consumerSide[consumer,provider]"
  gFreeFunctionClient1ID = cclFunctionGetID(gFreeFunctionClient1Port);
  if (gFreeFunctionClient1ID < 0)
    cclWrite("Error: FunctionCalls::TheFunction.consumerSide[Client1,Server1] not accessible");

  // "namespace::function.providerSide[consumer,provider]"
  gFreeFunctionServer1ID = cclFunctionGetID(gFreeFunctionServer1Port);
  if (gFreeFunctionServer1ID < 0)
    cclWrite("Error: FunctionCalls::TheFunction.providerSide[Client1,Server1] not accessible");

  gFreeFunctionServer2ID = cclFunctionGetID(gFreeFunctionServer2Port);
  if (gFreeFunctionServer2ID < 0)
    cclWrite("Error: FunctionCalls::TheFunction.consumerSide[Client2,Server1] not accessible");
}


void OnMeasurementStart()
{
  // configure automatic response for free function at Server1
  cclValueID io1Value = cclValueGetID(CCL_COMMUNICATION_OBJECTS, gFreeFunctionServer1Port, CCL_MEMBER_PARAM_DEFAULTS ".io1" CCL_MEMBER_VALUE, CCL_IMPL);
  if (io1Value < 0)
    cclWrite("Error: failed to access io1 server default value");

  if (cclValueSetFloat(io1Value, 10.0) < 0)
    cclWrite("Error: failed to set io1 server default value");

  cclValueID io1Behavior = cclValueGetID(CCL_COMMUNICATION_OBJECTS, gFreeFunctionServer1Port, CCL_MEMBER_PARAM_DEFAULTS ".io1" CCL_MEMBER_BEHAVIOR, CCL_IMPL);
  if (io1Behavior < 0)
    cclWrite("Error: failed to access io1 server default behavior");

  if (cclValueSetEnum(io1Behavior, CCL_BEHAVIOR_USE_DEFAULT_VALUE) < 0)
    cclWrite("Error: failed to set io1 server default behavior");

  cclValueID out1Value = cclValueGetID(CCL_COMMUNICATION_OBJECTS, gFreeFunctionServer1Port, CCL_MEMBER_PARAM_DEFAULTS ".out1", CCL_IMPL);
  if (out1Value < 0)
    cclWrite("Error: failed to access out1 server default value");

  if (cclValueSetInteger(out1Value, 42) < 0)
    cclWrite("Error: failed to set out1 server default value");

  cclValueID retValue = cclValueGetID(CCL_COMMUNICATION_OBJECTS, gFreeFunctionServer1Port, CCL_MEMBER_DEFAULT_RESULT, CCL_IMPL);
  if (retValue < 0)
    cclWrite("Error: failed to access server default return value");

  if (cclValueSetFloat(retValue, 3.14159265359) < 0)
    cclWrite("Error: failed to set server default return value");

  // configure server simulation callback for service function at Server1
  if (cclServiceSetSimulator(gProvidedServiceID, SimulateServer) < 0)
    cclWrite("Error: failed to register provided service simulator");

  // trigger function calls through timers
  if (cclTimerSet(gServiceFunctionTimerID, cclTimeMilliseconds(1000)) < 0)
    cclWrite("Error: failed to set timer");

  if (cclTimerSet(gFreeFunctionTimerID, cclTimeMilliseconds(2000)) < 0)
    cclWrite("Error: failed to set timer");
}


void SimulateServer(cclTime time, cclCallContextID ccID)
{
  // get input values from context
  cclValueID in1 = cclCallContextValueGetID(ccID, CCL_MEMBER_IN_PARAM ".in1", CCL_IMPL);
  if (in1 < 0)
    cclWrite("Error: failed to access in1 input parameter");

  double in1Value;
  if (cclValueGetFloat(in1, &in1Value) < 0)
    cclWrite("Error: failed to get in1 input parameter value");

  cclValueID io1_in = cclCallContextValueGetID(ccID, CCL_MEMBER_IN_PARAM ".io1", CCL_IMPL);
  if (io1_in < 0)
    cclWrite("Error: failed to access io1 input parameter");

  double io1Value;
  if (cclValueGetFloat(io1_in, &io1Value) < 0)
    cclWrite("Error: failed to get io1 input parameter value");

  // get context call time
  cclValueID callTime = cclCallContextValueGetID(ccID, CCL_MEMBER_CALL_TIME, CCL_IMPL);
  if (callTime < 0)
    cclWrite("Error: failed to access call context call time");

  cclTime callTimeValue;
  if (cclValueGetInteger(callTime, &callTimeValue) < 0)
    cclWrite("Error: failed to get call context call time value");

  // set output and return values
  cclValueID io1_out = cclCallContextValueGetID(ccID, CCL_MEMBER_OUT_PARAM ".io1", CCL_IMPL);
  if (io1_out < 0)
    cclWrite("Error: failed to access io1 output parameter");

  if (cclValueSetFloat(io1_out, in1Value) < 0)
    cclWrite("Error: failed to set io1 output parameter value");

  cclValueID out1 = cclCallContextValueGetID(ccID, CCL_MEMBER_OUT_PARAM ".out1", CCL_IMPL);
  if (out1 < 0)
    cclWrite("Error: failed to access out1 output parameter");

  if (cclValueSetInteger(out1, (long long)(io1Value * 100.0)) < 0)
    cclWrite("Error: failed to set out1 output parameter value");

  cclValueID retVal = cclCallContextValueGetID(ccID, CCL_MEMBER_RESULT, CCL_IMPL);
  if (retVal < 0)
    cclWrite("Error: failed to access return value");

  if (cclValueSetFloat(retVal, (double)callTimeValue / 1000000000.0) < 0)
    cclWrite("Error: failed to set return value");

  // delay function return by 1.5s
  if (cclCallContextReturn(ccID, 1500000000) < 0)
    cclWrite("Error: failed to set return time");
}


void OnFunctionReturn(cclTime time, cclCallContextID ccID)
{
  cclValueID io1 = cclCallContextValueGetID(ccID, CCL_MEMBER_OUT_PARAM ".io1", CCL_IMPL);
  if (io1 < 0)
    cclWrite("Error: failed to access parameter io1");

  double io1Value;
  if (cclValueGetFloat(io1, &io1Value) < 0)
    cclWrite("Error: failed to get io1 parameter out value");

  cclValueID out1 = cclCallContextValueGetID(ccID, CCL_MEMBER_OUT_PARAM ".out1", CCL_IMPL);
  if (out1 < 0)
    cclWrite("Error: failed to access parameter out1");

  long long out1Value;
  if (cclValueGetInteger(out1, &out1Value) < 0)
    cclWrite("Error: failed to get out1 parameter out value");

  cclValueID ret = cclCallContextValueGetID(ccID, CCL_MEMBER_RESULT, CCL_IMPL);
  if (ret < 0)
    cclWrite("Error: failed to access return value");

  double retValue;
  if (cclValueGetFloat(ret, &retValue) < 0)
    cclWrite("Error: failed to get return value");

  static char path[256];
  if (cclCallContextGetFunctionPath(ccID, path, 256) < 0)
    cclWrite("Error: failed to get path of returned function.");

  static char buffer[256];
  sprintf_s(buffer, 256, "Received \"%s\" response at %lldms: io1=%f, out1=%lld, ret=%f", path, time / 1000000LL, io1Value, out1Value, retValue);
  cclWrite(buffer);
}


void CallFunction(cclTime time, cclCallContextID ccID)
{
  cclValueID in1 = cclCallContextValueGetID(ccID, CCL_MEMBER_IN_PARAM ".in1", CCL_IMPL);
  if (in1 < 0)
    cclWrite("Error: failed to access parameter in1");

  if (cclValueSetFloat(in1, sin((double)time / 1000000000.0)) < 0)
    cclWrite("Error: failed to set in1 parameter value");

  cclValueID io1 = cclCallContextValueGetID(ccID, CCL_MEMBER_IN_PARAM ".io1", CCL_IMPL);
  if (io1 < 0)
    cclWrite("Error: failed to access parameter io1");

  if (cclValueSetFloat(io1, cos((double)time / 1000000000.0)) < 0)
    cclWrite("Error: failed to set io1 parameter value");

  if (cclCallAsync(ccID, OnFunctionReturn) < 0)
    cclWrite("Error: failed to start asynchronous function call at Client1");
}


void OnServiceFunctionTimer(cclTime time, int timerID)
{
  // call service function from Client1
  cclCallContextID ccID = cclCreateCallContext(gServiceFunctionClient1ID);
  if (ccID < 0)
    cclWrite("Error: creation of call context failed");

  CallFunction(time, ccID);

  // every 5 seconds
  if (cclTimerSet(gServiceFunctionTimerID, cclTimeMilliseconds(5000)) < 0)
    cclWrite("Error: failed to set timer");
}


void OnFreeFunctionTimer(cclTime time, int timerID)
{
  // call service function from Client1
  cclCallContextID ccID = cclCreateCallContext(gFreeFunctionClient1ID);
  if (ccID < 0)
    cclWrite("Error: creation of call context failed");

  CallFunction(time, ccID);

  // every 7 seconds
  if (cclTimerSet(gFreeFunctionTimerID, cclTimeMilliseconds(7000)) < 0)
    cclWrite("Error: failed to set timer");
}
