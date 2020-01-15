// CAN.c : Example of a CANalyzer/CANoe C Library
//
// This sample file demonstrates the usage of the CAN bus in a 
// CANalyzer/CANoe C Library.
//

#include "CCL/CCL.h"


extern void OnMeasurementPreStart();
extern void OnMeasurementStart();
extern void OnTimer(long long time, int timerID);
extern void OnCanMessage0x101(struct cclCanMessage* message);
extern void OnCanMessage0x102x(struct cclCanMessage* message);


int gTimerID;


void cclOnDllLoad()
{
  cclSetMeasurementPreStartHandler(&OnMeasurementPreStart);
  cclSetMeasurementStartHandler(&OnMeasurementStart);
}


void OnMeasurementPreStart()
{
  int rc;
  gTimerID = cclTimerCreate(&OnTimer);
  rc = cclCanSetMessageHandler(1, cclCanMakeStandardIdentifier(0x101), &OnCanMessage0x101);
  rc = cclCanSetMessageHandler(1, cclCanMakeExtendedIdentifier(0x102), &OnCanMessage0x102x);
}


void OnMeasurementStart()
{
  int rc;
  rc = cclTimerSet(gTimerID, cclTimeMilliseconds(500) );
}


void OnTimer(long long time, int timerID)
{
  int channel = 1;
  unsigned int id = cclCanMakeExtendedIdentifier(0x100);
  unsigned int flags = 0;
  unsigned char dataLength = 8;
  unsigned char data[8] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  int rc;

  rc = cclCanOutputMessage(channel, id, flags, dataLength, data);
  rc = cclTimerSet(gTimerID, cclTimeMilliseconds(500) );
}


void OnCanMessage0x101(struct cclCanMessage* message)
{
  cclWrite("C-API Example: OnCanMessage0x101");
}


void OnCanMessage0x102x(struct cclCanMessage* message)
{
  int isExtendedIdentifier;
  int isStandardIdentifier;
  unsigned int idValue;

  cclWrite("C-API Example: OnCanMessage0x102x");
  isExtendedIdentifier = cclCanIsExtendedIdentifier(message->id);
  isStandardIdentifier = cclCanIsStandardIdentifier(message->id);
  idValue = cclCanValueOfIdentifier(message->id);
}