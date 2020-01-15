/*-----------------------------------------------------------------------------
Module: EasyClient
Interfaces: 
-------------------------------------------------------------------------------
Example of the CANoeFDX interface.
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/

#include "stdafx.h"
#include "CANoeFDX.h"
#include "FDXDatagram.h"
#include "FDXSocket.h"
#include "FDXDispatcher.h"
#include <iostream>

using namespace std;


class MyDispatcher : public FDXDispatcher
{
  virtual void OnStatus(CANoeFDX::DatagramHeader* header, CANoeFDX::StatusCommand* command);
  virtual void OnDataExchange(CANoeFDX::DatagramHeader* header, CANoeFDX::DataExchangeCommand* command);

  CANoeFDX::uint32 lastGrossWeight;
};

// the structure needs to be packed for correct alignment in the udp packet
#pragma pack(push, 1)
struct A429InputData
{
  static const CANoeFDX::uint16 cGroupID = 1;
  static const CANoeFDX::uint16 cSize    = 7;

  CANoeFDX::uint16 SigFuel_Temp_And_Advisory_Warning;          // offset 0
  float  SigProbe_Capacitance;                                 // offset 2
  CANoeFDX::uint8 Reserved;                                    // offset 6
};

// Check the memory layout of struct A429InputData
static_assert( A429InputData::cSize == sizeof( A429InputData), "Invalid size of struct A429InputData");
static_assert( 0 == offsetof( A429InputData, SigFuel_Temp_And_Advisory_Warning), "Invalid offset of A429InputData::SigFuel_Temp_And_Advisory_Warning");
static_assert(2 == offsetof(A429InputData, SigProbe_Capacitance), "Invalid offset of A429InputData::SigProbe_Capacitance");
static_assert(6 == offsetof(A429InputData, Reserved), "Invalid offset of A429InputData::Reserved");

struct A429OutputData
{
  static const CANoeFDX::uint16 cGroupID = 2;
  static const CANoeFDX::uint16 cSize    = 5;

  CANoeFDX::int32  SigGross_Weight;    // offset 0
  CANoeFDX::uint8 Reserved;            // offset 4
};

// Check the memory layout of struct A429InputData
static_assert(A429OutputData::cSize == sizeof(A429OutputData), "Invalid size of struct A429OutputData");
static_assert(0 == offsetof(A429OutputData, SigGross_Weight), "Invalid offset of A429OutputData::SigGross_Weight");
static_assert(4 == offsetof(A429OutputData, Reserved), "Invalid offset of A429OutputData::Reserved");

struct A429OvhdData
{
  static const CANoeFDX::uint16 cGroupID = 3;
  static const CANoeFDX::uint16 cSize = 12;

  CANoeFDX::uint32 reserved1; //reserved for internal offset 0
  CANoeFDX::uint32 size;      //size of the array offset 4
  CANoeFDX::uint8 sysVar[2];  // array bytes offset 8
  CANoeFDX::uint16 reserved2; // reserved for padding offset 10
};

// Check the memory layout of struct A429InputData
static_assert(A429OvhdData::cSize == sizeof(A429OvhdData), "Invalid size of struct A429OvhdData");
static_assert(0 == offsetof(A429OvhdData, reserved1), "Invalid offset of A429OvhdData::reserved1");
static_assert(4 == offsetof(A429OvhdData, size), "Invalid offset of A429OvhdData::size");
static_assert(8 == offsetof(A429OvhdData, sysVar), "Invalid offset of A429OvhdData::sysVar");
static_assert(10 == offsetof(A429OvhdData, reserved2), "Invalid offset of A429OvhdData::reserved2");

//restore alignment to default value
#pragma pack(pop)

CANoeFDX::uint8 gCANoeMeasurementState = CANoeFDX::kMeasurementState_NotRunning;
bool keepMeasurementRunning = true;

FDXSocket      gFDXSocket;
FDXDatagram    gFDXDatagram;
MyDispatcher   gFDXDispatcher;


void MyDispatcher::OnStatus(CANoeFDX::DatagramHeader* header, CANoeFDX::StatusCommand* command)
{
  FDXDispatcher::OnStatus(header, command);
  gCANoeMeasurementState = command->measurementState;
}

void MyDispatcher::OnDataExchange(CANoeFDX::DatagramHeader* header, CANoeFDX::DataExchangeCommand* command)
{
  
  if (command->groupID == A429InputData::cGroupID)
  {
    A429InputData * readData = reinterpret_cast<A429InputData*>(command->dataBytes);

     cout << "Fuel_Temp_And_Advisory_Warning = " << readData->SigFuel_Temp_And_Advisory_Warning << " "
          << "Probe_Capacitance = "  << readData->SigProbe_Capacitance << endl;
  }
  else if (command->groupID == A429OutputData::cGroupID)
  {
    A429OutputData * readData = reinterpret_cast<A429OutputData*>(command->dataBytes);
    if (lastGrossWeight != readData->SigGross_Weight)
    {
      cout << "Gross_Weight = " << readData->SigGross_Weight << endl;
      lastGrossWeight = readData->SigGross_Weight;
    }
    
  }
}

//Handler for CTRL-C
BOOL CtrlHandler(DWORD fwdCtrlType)
{
  keepMeasurementRunning = false;
  printf("FDXClient will shutdown\n");
  return TRUE;
}

int main(int argc, char* argv[])
{
  //Register Ctr-C handler for shutdown
  if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
  {
    printf("The FDX-Client can be terminated with CTRL-C\n");
  }
  else
  {
    printf("Problem within FDX-Client, client will shutdown\n");
    keepMeasurementRunning = false;
  }

  //default address
  const char* addr = "127.0.0.1";
  unsigned short port = 2809;
  if (argc==3)
  {
    addr = argv[1];
    port = (unsigned short)atol(argv[2]);
  }

  //Set connection parameter and open the Udp-Socket
  gFDXSocket.SetCANoeAddr(addr, port);
  gFDXSocket.Open();

  const int cCycleInterval = 250;

  //Initial values for exchange data
  CANoeFDX::int16 cFuelTempAndAdvWarning = -2048;
  float cProbCap = 0;
  CANoeFDX::int8 cProbCapModifier = 1;
  bool isFuelConsumingActive = false;


  //start CANoe measurement
  gFDXDatagram.InitWithHeader();
  gFDXDatagram.AddStart();
  gFDXSocket.Send(gFDXDatagram);

  //keep data exchange running until the user quits the client
  while (keepMeasurementRunning)
  {
    // build datagram and add status request
    gFDXDatagram.InitWithHeader();
    gFDXDatagram.AddStatusRequest();

    //modify data only when CANoe measurement is running
    if (gCANoeMeasurementState == CANoeFDX::kMeasurementState_Running)
    {
      if (cFuelTempAndAdvWarning < 2048)
      {
        cFuelTempAndAdvWarning += 4;
      }
      else
      {
        cFuelTempAndAdvWarning = -2048;
      }

      if (cProbCapModifier == 1 && cProbCap > 399)
      {
        cProbCapModifier = -1;
      }
      else if (cProbCapModifier == -1 && cProbCap < 1)
      {
        cProbCapModifier = 1;
      }
      else
      {
        if (isFuelConsumingActive == false && cProbCap > 10)
        {
          void* ovhdDataBytes;
          ovhdDataBytes = gFDXDatagram.AddDataExchange(A429OvhdData::cGroupID, A429OvhdData::cSize);
          A429OvhdData *ovhdData = reinterpret_cast<A429OvhdData*>(ovhdDataBytes);
          ovhdData->size = 2;
          //Enable the single bits in the array
          ovhdData->sysVar[0] |= 0x01;
          ovhdData->sysVar[0] |= 0x02;
          ovhdData->sysVar[0] |= 0x04;
          ovhdData->sysVar[0] |= 0x08;
          ovhdData->sysVar[0] |= 0x40;
          ovhdData->sysVar[0] |= 0x80;
          isFuelConsumingActive = true;
        }
        else if (isFuelConsumingActive == true && cProbCap <= 10)
        {
          void* ovhdDataBytes;
          ovhdDataBytes = gFDXDatagram.AddDataExchange(A429OvhdData::cGroupID, A429OvhdData::cSize);
          A429OvhdData *ovhdData = reinterpret_cast<A429OvhdData*>(ovhdDataBytes);
          ovhdData->size = 2;
          //Disable the single bits in the array
          ovhdData->sysVar[0] &= 0xFE;
          ovhdData->sysVar[0] &= 0xFD;
          ovhdData->sysVar[0] &= 0xFB;
          ovhdData->sysVar[0] &= 0xF7;
          ovhdData->sysVar[0] &= 0xBF;
          ovhdData->sysVar[0] &= 0x7F;
          isFuelConsumingActive = false;
        }
        cProbCap += (0.4f * cProbCapModifier);
      }

      //Add the data request and data exchange
      gFDXDatagram.AddDataRequest(A429OutputData::cGroupID);
      void* dataBytes;
      dataBytes = gFDXDatagram.AddDataExchange(A429InputData::cGroupID, A429InputData::cSize);
      A429InputData* data = reinterpret_cast<A429InputData*>(dataBytes);
      data->SigFuel_Temp_And_Advisory_Warning = cFuelTempAndAdvWarning;
      data->SigProbe_Capacitance = cProbCap;
    }
    

    // send datagram 
    gFDXSocket.Send(gFDXDatagram);

    // receive and dispatch data from CANoe
    int rc = gFDXSocket.Receive(gFDXDatagram);
    if (rc==FDXSocket::OK)
    {
      gFDXDispatcher.DispatchDatagram(gFDXDatagram);
    }

    Sleep(cCycleInterval);
  }

  //end CANoe measurement
  gFDXDatagram.InitWithHeader();
  gFDXDatagram.AddStop();
  gFDXSocket.Send(gFDXDatagram);

  gFDXSocket.Close();
  return 0;
}

