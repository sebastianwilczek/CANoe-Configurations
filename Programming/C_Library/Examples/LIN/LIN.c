// LIN.c : Example of a CANalyzer/CANoe C Library
//
// This sample file demonstrates the usage of the LIN bus in a 
// CANalyzer/CANoe C Library.
//

#include "CCL/CCL.h"


extern void OnMeasurementPreStart();
extern void OnMeasurementStart();
extern void OnTimer(long long time, int timerID);
extern void OnLinFrame0x33(struct cclLinFrame* frame);
extern void OnLinFrame0x34(struct cclLinFrame* frame);
extern void OnLinFrame0x01(struct cclLinFrame* frame);
extern void OnSysVar_SchedulerStart(long long time, int sysVarID);
extern void OnSysVar_SchedulerStop(long long time, int sysVarID);
extern void OnSysVar_ChangeScheduleTable(long long time, int sysVarID);
extern void OnSysVar_SendHeader(long long time, int sysVarID);
extern void OnSysVar_UpdateResponse(long long time, int sysVarID);



int gTimerID;

int gSysVarID_SchedulerStart;
int gSysVarID_SchedulerStop;
int gSysVarID_ScheduleChange;
int gSysVarID_SendHeader;
int gSysVarID_UpdateResponse;
int gSysVarID_SOF;      
int gSysVarID_SyncBreak;
int gSysVarID_SyncDel;
int gSysVarID_EOH;
int gSysVarID_EOF;

double gTimeFactorNS = 1000000000.0;
double gTimeFactorUS = 1000000.0;

unsigned char gData[8] = {0, 0, 0, 0, 0, 0, 0, 0};


void cclOnDllLoad()
{
  cclSetMeasurementPreStartHandler(&OnMeasurementPreStart);
  cclSetMeasurementStartHandler(&OnMeasurementStart);
}


void OnMeasurementPreStart()
{
  unsigned char data[8] = {1, 2, 3, 4, 5, 6, 7, 8};

  gTimerID = cclTimerCreate(&OnTimer);

  if  (CCL_SUCCESS != cclLinSetFrameHandler(1, 0x33, &OnLinFrame0x33))
  {
     cclWrite("C-API Example: Could not set frame handler OnLinFrame0x33");
  }

  if  (CCL_SUCCESS != cclLinSetFrameHandler(1, 0x34, &OnLinFrame0x34))
  {
    cclWrite("C-API Example: Could not set frame handler OnLinFrame0x34");
  }

  if  (CCL_SUCCESS != cclLinSetFrameHandler(1, 0x01, &OnLinFrame0x01))
  {
    cclWrite("C-API Example: Could not set frame handler OnLinFrame0x01");
  }

  // configure the LIN responses for simulation
  if (CCL_SUCCESS != cclLinUpdateResponseData(1, 0x33, 6, data))
  {
    cclWrite("C-API Example: Could not update response data if ID 0x33");
  }

  if (CCL_SUCCESS != cclLinUpdateResponseData(1, 0x34, 6, data))
  {
    cclWrite("C-API Example: Could not update response data if ID 0x33");
  }

  if (CCL_SUCCESS != cclLinUpdateResponseData(1, 0x36, 3, data))
  {
    cclWrite("C-API Example: Could not update response data if ID 0x33");
  }

  // Configure ID not defined in the LDF.
  if (CCL_SUCCESS != cclLinUpdateResponseData(1, 0x01, 2, gData))
  {
    cclWrite("C-API Example: Could not update response data if ID 0x01");
  }

  // Configure Frame Handlers on channel 2. Only working when channels of LIN configurations are looped.
  if  (CCL_SUCCESS != cclLinSetFrameHandler(2, 0x33, &OnLinFrame0x33))
  {
    cclWrite("C-API Example: Could not set message handler OnLinFrame0x33");
  }

  if  (CCL_SUCCESS != cclLinSetFrameHandler(2, 0x34, &OnLinFrame0x34))
  {
    cclWrite("C-API Example: Could not set message handler OnLinFrame0x34");
  }

  gSysVarID_SchedulerStart = cclSysVarGetID("LIN::SchedulerStart");
  gSysVarID_SchedulerStop  = cclSysVarGetID("LIN::SchedulerStop");
  gSysVarID_ScheduleChange = cclSysVarGetID("LIN::ChangeScheduleTable");
  gSysVarID_SendHeader     = cclSysVarGetID("LIN::SendHeader");
  gSysVarID_UpdateResponse = cclSysVarGetID("LIN::UpdateResponse");
  gSysVarID_SOF            = cclSysVarGetID("LIN::SOF");
  gSysVarID_SyncBreak      = cclSysVarGetID("LIN::SyncBreak");
  gSysVarID_SyncDel        = cclSysVarGetID("LIN::SyncDel");
  gSysVarID_EOH            = cclSysVarGetID("LIN::EOH");
  gSysVarID_EOF            = cclSysVarGetID("LIN::EOF");
  
  cclSysVarSetHandler(gSysVarID_SchedulerStart, &OnSysVar_SchedulerStart);
  cclSysVarSetHandler(gSysVarID_SchedulerStop, &OnSysVar_SchedulerStop);
  cclSysVarSetHandler(gSysVarID_ScheduleChange, &OnSysVar_ChangeScheduleTable);
  cclSysVarSetHandler(gSysVarID_SendHeader, &OnSysVar_SendHeader);
  cclSysVarSetHandler(gSysVarID_UpdateResponse, &OnSysVar_UpdateResponse);
}


void OnMeasurementStart()
{
  cclWrite("C-API Example: OnMeasurementStart");
}


void OnTimer(long long time, int timerID)
{
  // Change the schedule table back to the first schedule.
  cclLinChangeSchedtable(1, 0);
}

void OnLinFrame0x33(struct cclLinFrame* frame)
{
  if (frame->channel == 1)
  {
    cclWrite("C-API Example: OnLinFrame0x33 on channel 1");
  }
  else
  {
    cclWrite("C-API Example: OnLinFrame0x33 on channel 2");
  }
}

void OnLinFrame0x34(struct cclLinFrame* frame)
{
  if (frame->channel == 1)
  {
    cclWrite("C-API Example: OnLinFrame0x34 on channel 1");
  }
  else
  {
    cclWrite("C-API Example: OnLinFrame0x34 on channel 2");
  }
}

void OnLinFrame0x01(struct cclLinFrame* frame)
{ 
  double sof         = frame->timestampSOF/gTimeFactorNS;
  double syncBreak   = frame->timeSyncBreak/gTimeFactorUS;
  double syncDel     = frame->timeSyncDel/gTimeFactorUS;
  double eoh         = frame->timestampEOH/gTimeFactorNS;
  double eof         = frame->timestampEOF/gTimeFactorNS;

  cclSysVarSetFloat(gSysVarID_SOF       , sof); 
  cclSysVarSetFloat(gSysVarID_SyncBreak , syncBreak); 
  cclSysVarSetFloat(gSysVarID_SyncDel   , syncDel); 
  cclSysVarSetFloat(gSysVarID_EOH       , eoh); 
  cclSysVarSetFloat(gSysVarID_EOF       , eof); 
}

void OnSysVar_SchedulerStart(long long time, int sysVarID)
{
  int startScheduler;
  cclSysVarGetInteger(gSysVarID_SchedulerStart, &startScheduler);

  if (startScheduler == 1)
  {
    cclLinStartScheduler(1);
    cclWrite("C Library Example: OnSysVar_SchedulerStart");
  }
}

void OnSysVar_SchedulerStop(long long time, int sysVarID)
{
  int stopScheduler;
  cclSysVarGetInteger(gSysVarID_SchedulerStop, &stopScheduler);

  if (stopScheduler == 1)
  {
    cclLinStopScheduler(1);
    cclWrite("C Library Example: OnSysVar_SchedulerStop");
  }
}

void OnSysVar_ChangeScheduleTable(long long time, int sysVarID)
{
  int tableIndex;
  cclSysVarGetInteger(gSysVarID_ScheduleChange, &tableIndex);

  if (tableIndex == 1)
  {
    cclLinChangeSchedtable(1, tableIndex); 
    cclTimerSet(gTimerID, cclTimeMilliseconds(5000));
    cclWrite("C Library Example: OnSysVar_ChangeScheduleTable");
  }
}

void OnSysVar_SendHeader(long long time, int sysVarID)
{
  int sendHeader;
  cclSysVarGetInteger(gSysVarID_SendHeader, &sendHeader);

  if (sendHeader == 1)
  {
    cclLinSendHeader(1, 1);
    cclWrite("C Library Example: OnSysVar_SendHeader");
  }
}

void OnSysVar_UpdateResponse(long long time, int sysVarID)
{
  int updateResponse;
  cclSysVarGetInteger(gSysVarID_UpdateResponse, &updateResponse);

  if (updateResponse == 1)
  {
    gData[0] += 1;
    gData[1] += 1;
    cclLinUpdateResponseData(1, 1, 2, gData); 
    cclWrite("C Library Example: OnSysVar_UpdateResponse");
  }
}