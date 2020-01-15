/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: FDXDatagram
-------------------------------------------------------------------------------
Datagram builder class for CANoe FDX protocol
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/


#include "stdafx.h"
#include "FDXDatagram.h"
#include "assert.h"

using namespace CANoeFDX;

FDXDatagram::FDXDatagram()
: mSize(0)
{
  memset(mBuffer, 0, cCapacity);
}



void FDXDatagram::InitWithHeader()
{
  memset(mBuffer, 0, cCapacity);
  CANoeFDX::DatagramHeader* header = reinterpret_cast<CANoeFDX::DatagramHeader*>(mBuffer);
  header->fdxSignature = CANoeFDX::kFdxSignature;
  header->fdxMajorVersion = CANoeFDX::kFdxMajorVersion;
  header->fdxMinorVersion = CANoeFDX::kFdxMinorVersion;
  header->numberOfCommands = 0;
  header->sequenceNumber = CANoeFDX::kSequenceNumberUnused;
  header->fdxProtocolFlags = kByteOrderLittleEndian;
  header->reserved = 0;
  mSize = sizeof(CANoeFDX::DatagramHeader);
}


void FDXDatagram::SetSequenceNumber(CANoeFDX::uint16 sequenceNumber)
{
  CANoeFDX::DatagramHeader* header = reinterpret_cast<CANoeFDX::DatagramHeader*>(mBuffer);
  header->sequenceNumber = sequenceNumber;
}


void FDXDatagram::AddDataRequest(CANoeFDX::uint16 groupID)
{
  CANoeFDX::DataRequestCommand* command = static_cast<CANoeFDX::DataRequestCommand*>(AddCommand(CANoeFDX::kCommandCode_DataRequest, sizeof(CANoeFDX::DataRequestCommand)));
  command->groupID = groupID;
}



void* FDXDatagram::AddDataExchange(CANoeFDX::uint16 groupID, CANoeFDX::uint16 dataSize)
{
  CANoeFDX::DataExchangeCommand* command = static_cast<CANoeFDX::DataExchangeCommand*>(AddCommand(CANoeFDX::kCommandCode_DataExchange, CANoeFDX::kDataExchangeBaseSize + dataSize));
  command->groupID = groupID;
  command->dataSize = dataSize;
  return command->dataBytes;
}



void FDXDatagram::AddFreeRunningRequest(CANoeFDX::uint16 groupID, CANoeFDX::uint16 flags, CANoeFDX::uint32 cycleTime, CANoeFDX::uint32 firstDuration)
{
  CANoeFDX::FreeRunningRequestCommand* command = static_cast<CANoeFDX::FreeRunningRequestCommand*>(AddCommand(CANoeFDX::kCommandCode_FreeRunningRequest, sizeof(CANoeFDX::FreeRunningRequestCommand)));
  command->groupID = groupID;
  command->flags = flags;
  command->cycleTime = cycleTime;
  command->firstDuration = firstDuration;
}


void FDXDatagram::AddFreeRunningCancel(CANoeFDX::uint16 groupID)
{
  CANoeFDX::FreeRunningCancelCommand* command = static_cast<CANoeFDX::FreeRunningCancelCommand*>(AddCommand(CANoeFDX::kCommandCode_FreeRunningCancel, sizeof(CANoeFDX::FreeRunningCancelCommand)));
  command->groupID = groupID;
}



void FDXDatagram::AddStart()
{
  AddCommand(CANoeFDX::kCommandCode_Start, sizeof(CANoeFDX::CommandHeader));
}



void FDXDatagram::AddStop()
{
  AddCommand(CANoeFDX::kCommandCode_Stop, sizeof(CANoeFDX::CommandHeader));
}



CANoeFDX::CommandHeader* FDXDatagram::AddCommand(CANoeFDX::uint16 commandCode, CANoeFDX::uint16 commandSize)
{
  CANoeFDX::DatagramHeader* header  = reinterpret_cast<CANoeFDX::DatagramHeader*>(mBuffer);
  CANoeFDX::CommandHeader*  command = reinterpret_cast<CANoeFDX::CommandHeader*>(mBuffer+mSize);
  assert(mSize+commandSize <= cCapacity);
  mSize += commandSize;
  header->numberOfCommands++;
  command->commandSize = commandSize;
  command->commandCode = commandCode;
  return command;
}




CANoeFDX::uint16 IncrementSequenceNumber(CANoeFDX::uint16 seqNr)
{
  if (seqNr==CANoeFDX::kSequenceNumberUnused)
  {
    return CANoeFDX::kSequenceNumberUnused;
  }
  else if (seqNr&CANoeFDX::kSequenceNumberSessionEndFlag)
  {
    return CANoeFDX::kSequenceNumberUnused;
  }
  else if (seqNr==0x7FFF)
  {
    return 1;
  }
  else
  {
    return seqNr+1;
  }
}
