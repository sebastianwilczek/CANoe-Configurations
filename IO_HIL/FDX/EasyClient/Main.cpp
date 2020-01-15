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
  virtual void OnFormatError();
  virtual void OnSequenceNumberError(CANoeFDX::DatagramHeader* header, CANoeFDX::uint16 expectedSeqNr);
  virtual void OnStatus(CANoeFDX::DatagramHeader* header, CANoeFDX::StatusCommand* command);
  virtual void OnDataError(CANoeFDX::DatagramHeader* header, CANoeFDX::DataErrorCommand* command);
  virtual void OnDataExchange(CANoeFDX::DatagramHeader* header, CANoeFDX::DataExchangeCommand* command);
};



struct EasyDataRead
{
  static const CANoeFDX::uint16 cGroupID = 1;
  static const CANoeFDX::uint16 cSize    = 8;

  CANoeFDX::int16  SigEngineSpeed;          // offset 0
  CANoeFDX::uint8  SigOnOff;                // offset 2
  CANoeFDX::uint8  SigFlashLight;           // offset 3
  CANoeFDX::uint8  SigHeadLight;            // offset 4
  CANoeFDX::uint8  VarHazardLightsSwitch;   // offset 5
  CANoeFDX::uint8  VarHeadLightSwitch;      // offset 6
  CANoeFDX::uint8  Reserved1;               // offset 7
};

// Check the memory layout of struct EasyDataRead
static_assert(EasyDataRead::cSize == sizeof(EasyDataRead), "Invalid size of struct EasyDataRead" );
static_assert( 0 == offsetof(EasyDataRead, SigEngineSpeed), "Invalid offset of EasyDataRead::SigEngineSpeed" );
static_assert( 2 == offsetof(EasyDataRead, SigOnOff), "Invalid offset of EasyDataRead::SigOnOff" );
static_assert( 3 == offsetof(EasyDataRead, SigFlashLight), "Invalid offset of EasyDataRead::SigFlashLight" );
static_assert( 4 == offsetof(EasyDataRead, SigHeadLight), "Invalid offset of EasyDataRead::SigHeadLight" );
static_assert( 5 == offsetof(EasyDataRead, VarHazardLightsSwitch), "Invalid offset of EasyDataRead::VarHazardLightsSwitch" );
static_assert( 6 == offsetof(EasyDataRead, VarHeadLightSwitch), "Invalid offset of EasyDataRead::VarHeadLightSwitch" );



struct EasyDataWrite
{
  static const CANoeFDX::uint16 cGroupID = 2;
  static const CANoeFDX::uint16 cSize    = 8;

  CANoeFDX::int16  VarEngineSpeedEntry;    // offset 0
  CANoeFDX::uint8  VarEngineStateSwitch;   // offset 2
  CANoeFDX::uint8  SigFlashLigth;          // offset 3
  CANoeFDX::uint8  SigHeadLight;           // offset 4
  CANoeFDX::uint8  Reserved1;              // offset 5
  CANoeFDX::uint8  Reserved2;              // offset 6
  CANoeFDX::uint8  Reserved3;              // offset 7
};

// Check the memory layout of struct EasyDataWrite
static_assert(EasyDataWrite::cSize == sizeof(EasyDataWrite), "Invalid size of struct EasyDataWrite" );
static_assert( 0 == offsetof(EasyDataWrite, VarEngineSpeedEntry), "Invalid offset of EasyDataWrite::VarEngineSpeedEntry" );
static_assert( 2 == offsetof(EasyDataWrite, VarEngineStateSwitch), "Invalid offset of EasyDataWrite::VarEngineStateSwitch" );
static_assert( 3 == offsetof(EasyDataWrite, SigFlashLigth), "Invalid offset of EasyDataWrite::SigFlashLight" );
static_assert( 4 == offsetof(EasyDataWrite, SigHeadLight), "Invalid offset of EasyDataWrite::SigHeadLight" );



struct EasyFrameAccess
{
  static const CANoeFDX::uint16 cGroupID = 3;
  static const CANoeFDX::uint16 cSize = 16;

  CANoeFDX::uint32 EngineState_byteArraySize;
  struct EngineStateFrame
  {
    static const CANoeFDX::uint16 cSize = 2;

    CANoeFDX::uint16 OnOff : 1;
    CANoeFDX::int16  EngineSpeed : 15;
  } engineState;

  // automatic padding to 4 byte boundary
  CANoeFDX::uint32 LightState_byteArraySize;
  struct LightStateFrame
  {
    static const CANoeFDX::uint16 cSize = 1;

    CANoeFDX::uint8  HeadLight : 1;
    CANoeFDX::uint8  _no_data_1 : 1;
    CANoeFDX::uint8  FlashLight : 1;
    CANoeFDX::uint8  _no_data_2 : 5;
  } lightState;
};

// Check the memory layout of struct EasyFrameAccess
static_assert(EasyFrameAccess::cSize == sizeof(EasyFrameAccess), "Invalid size of struct EasyFrameAccess");
static_assert(EasyFrameAccess::EngineStateFrame::cSize == sizeof(EasyFrameAccess::EngineStateFrame), "Invalid size of struct EngineStateFrame");
static_assert(EasyFrameAccess::LightStateFrame::cSize == sizeof(EasyFrameAccess::LightStateFrame), "Invalid size of struct LightStateFrame");
static_assert(0 == offsetof(EasyFrameAccess, EngineState_byteArraySize), "Invalid offset of EasyFrameAccess::EngineState_byteArraySize");
static_assert(4 == offsetof(EasyFrameAccess, engineState), "Invalid offset of EasyFrameAccess::engineState");
static_assert(8 == offsetof(EasyFrameAccess, LightState_byteArraySize), "Invalid offset of EasyFrameAccess::LightState_byteArraySize");
static_assert(12 == offsetof(EasyFrameAccess, lightState), "Invalid offset of EasyFrameAccess::lightState");



int  gHazardLightsSwitch = false;         // state of the hazard lights switch (on|off)
int  gHazardLightsState  = false;         // current state of hazard lights (on|off)
int  gHazardLightsTime   = 0;             // time in milliseconds to the next change of gHazardLightsState if
                                          // the hazard lights switch is turned on.
const int cHazardLightsFrequency = 500;   // [ms]

int  gHeadLightsState = false;

CANoeFDX::uint8 gCANoeMeasurementState = CANoeFDX::kMeasurementState_NotRunning;

FDXSocket      gFDXSocket;
FDXDatagram    gFDXDatagram;
MyDispatcher   gFDXDispatcher;


void MyDispatcher::OnFormatError()
{
  printf("Format Error\n");
}

void MyDispatcher::OnSequenceNumberError(CANoeFDX::DatagramHeader* header, CANoeFDX::uint16 expectedSeqNr)
{
  printf("Sequence Number Error\n");
}

void MyDispatcher::OnStatus(CANoeFDX::DatagramHeader* header, CANoeFDX::StatusCommand* command)
{
  if (command->measurementState==CANoeFDX::kMeasurementState_NotRunning)
  {
    printf("CANoe measurement is not running\n");
  }
  gCANoeMeasurementState = command->measurementState;
}

void MyDispatcher::OnDataError(CANoeFDX::DatagramHeader* header, CANoeFDX::DataErrorCommand* command)
{
  if (command->dataErrorCode!=CANoeFDX::kDataErrorCode_MeasurementNotRunning)
  {
    printf("Data error for group ID %d, error code %d\n", command->groupID, command->dataErrorCode);
  }
}

void MyDispatcher::OnDataExchange(CANoeFDX::DatagramHeader* header, CANoeFDX::DataExchangeCommand* command)
{
  if (command->groupID == EasyDataRead::cGroupID)
  {
    EasyDataRead * readData = reinterpret_cast<EasyDataRead*>(command->dataBytes);
    cout << "Variables Read:" << endl;
    cout << "EngineSpeed = " << readData->SigEngineSpeed << " "
         << "HeadLight = "  << ((readData->SigHeadLight)?"on":"off") << " "
         << "FlashLight = " << ((readData->SigFlashLight)?"on":"off") << endl;

    if (gHazardLightsSwitch==0 && readData->VarHazardLightsSwitch==1)
    {
      // activate hazard lights
      gHazardLightsSwitch = 1;
      gHazardLightsState = 1;
      gHazardLightsTime = cHazardLightsFrequency;
    }
    else if (gHazardLightsSwitch==1 && readData->VarHazardLightsSwitch==0)
    {
      // deactivate hazard lights
      gHazardLightsSwitch = 0;
      gHazardLightsState = 0;
      gHazardLightsTime = 0;
    }

    gHeadLightsState = readData->VarHeadLightSwitch;
  }
  else if (command->groupID == EasyFrameAccess::cGroupID)
  {
    EasyFrameAccess* readData = reinterpret_cast<EasyFrameAccess*>(command->dataBytes);
    cout << "Frames Read:" << "EngineOnOff = " << (readData->engineState.OnOff ? "on" : "off") << endl;
    cout << "EngineSpeed = " << readData->engineState.EngineSpeed << " "
         << "HeadLight = " << (readData->lightState.HeadLight ? "on" : "off") << " "
         << "FlashLight = " << (readData->lightState.FlashLight ? "on" : "off") << endl;
  }
}



int main(int argc, char* argv[])
{
  const char* addr = "127.0.0.1";
  if (argc==2)
  {
    addr = argv[1];
  }
  gFDXSocket.SetCANoeAddr(addr, 2809);
  gFDXSocket.Open();

  const int cCycleInterval = 100;

  int currentEngSpeed  = 0;
  int deltaEngineSpeed = +50;


  for (int i=0; i<10000; i++)
  {
    // change engine speed
    if (gCANoeMeasurementState==CANoeFDX::kMeasurementState_Running)
    {
      if (currentEngSpeed >= 3000)
      {
        deltaEngineSpeed = -50;
      }
      else if (currentEngSpeed <= 0)
      {
        deltaEngineSpeed = +50;
      }
      currentEngSpeed += deltaEngineSpeed;
    }
    else
    {
      currentEngSpeed = 0;
      deltaEngineSpeed = +50;
    }


    // toggle hazard lights
    if (gHazardLightsSwitch==1)
    {
      gHazardLightsTime -= cCycleInterval;
      if (gHazardLightsTime<=0)
      {
        gHazardLightsState = 1 - gHazardLightsState;  // toggle state
        gHazardLightsTime = cHazardLightsFrequency;
      }
    }

    if (i % 2 == 0)
    {
      // build datagram using individual signal and variable access
      gFDXDatagram.InitWithHeader();
      gFDXDatagram.AddDataRequest(EasyDataRead::cGroupID);
      void* dataBytes;
      dataBytes = gFDXDatagram.AddDataExchange(EasyDataWrite::cGroupID, EasyDataWrite::cSize);
      EasyDataWrite* writeData = reinterpret_cast<EasyDataWrite*>(dataBytes);
      writeData->VarEngineSpeedEntry = currentEngSpeed;
      writeData->VarEngineStateSwitch = (gCANoeMeasurementState == CANoeFDX::kMeasurementState_Running) ? 1 : 0;
      writeData->SigFlashLigth = gHazardLightsState;
      writeData->SigHeadLight = gHeadLightsState;
    }
    else
    {
      // build datagram using full frame access (byte array)
      gFDXDatagram.InitWithHeader();
      gFDXDatagram.AddDataRequest(EasyDataRead::cGroupID);
      gFDXDatagram.AddDataRequest(EasyFrameAccess::cGroupID);
      void* dataBytes;
      dataBytes = gFDXDatagram.AddDataExchange(EasyFrameAccess::cGroupID, EasyFrameAccess::cSize);
      EasyFrameAccess* writeData = reinterpret_cast<EasyFrameAccess*>(dataBytes);

      writeData->EngineState_byteArraySize = EasyFrameAccess::EngineStateFrame::cSize;
      writeData->engineState.OnOff = 1;
      writeData->engineState.EngineSpeed = currentEngSpeed;

      writeData->LightState_byteArraySize = EasyFrameAccess::LightStateFrame::cSize;
      writeData->lightState.HeadLight = gHeadLightsState;
      writeData->lightState.FlashLight = gHazardLightsState;
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

  gFDXSocket.Close();
  return 0;
}

