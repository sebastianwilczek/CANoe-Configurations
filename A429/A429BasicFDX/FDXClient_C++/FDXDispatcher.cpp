/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: FDXDispatcher
-------------------------------------------------------------------------------
Datagram dispatcher class for CANoe FDX protocol
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/
#include "stdafx.h"
#include "FDXDispatcher.h"



FDXDispatcher::FDXDispatcher() 
 : mNextExpectedSeqNr (CANoeFDX::kSequenceNumberSessionStart)
{}



void FDXDispatcher::DispatchDatagram(FDXDatagram& datagram)
{
  //
  // A) Check datagram header
  //
  CANoeFDX::DatagramHeader* header = reinterpret_cast<CANoeFDX::DatagramHeader*>(datagram.Buffer());
  {
    if (datagram.Size()<sizeof(CANoeFDX::DatagramHeader))
    {
      OnFormatError(std::string("Size of datagram is too small"));
      return;  // datagram is smaller as a header
    }

    if (header->fdxSignature != CANoeFDX::kFdxSignature )
    {
      OnFormatError(std::string("Signature doesn't match"));
      return;  // different value for magic cookie 
    }

    if (header->fdxMajorVersion != CANoeFDX::kFdxMajorVersion) // major version does not fit
    {
      OnFormatError(std::string("Major FDX Version doesn't match"));
      return; // version error
    }

    // Check Sequence Number
    if (header->sequenceNumber == CANoeFDX::kSequenceNumberUnused)
    {
       // sequence numbering unused => check nothing
    }
    else
    {
      if ( (header->sequenceNumber & 0x7FFF) != mNextExpectedSeqNr)
      {
        OnSequenceNumberError(header, mNextExpectedSeqNr);
      }
      if (header->sequenceNumber & CANoeFDX::kSequenceNumberSessionEndFlag)
      {
        mNextExpectedSeqNr = CANoeFDX::kSequenceNumberSessionStart;
      }
      else
      {
        mNextExpectedSeqNr = IncrementSequenceNumber(header->sequenceNumber);
      }
    }
  }


  //
  // B) Check The memory structure of the following commands.
  //    Do this check for the whole datagram before any request is processed
  {
    DWORD offset = sizeof(CANoeFDX::DatagramHeader);
    DWORD remainingBytes = datagram.Size() - offset;

    for (DWORD i=0; i < header->numberOfCommands; i++)
    {
      if (remainingBytes<sizeof(CANoeFDX::CommandHeader))
      {
        OnFormatError(std::string("command header is too small"));
        return;  // too small for command header
      }

      CANoeFDX::CommandHeader* command = reinterpret_cast<CANoeFDX::CommandHeader*>((char*)datagram.Buffer()+offset);
      if (remainingBytes<command->commandSize)
      {
        OnFormatError(std::string("datagram is too small for command"));
        return;  // datagram is to small for this command
      }

      offset += command->commandSize;
      remainingBytes -= command->commandSize;
    }

    if (remainingBytes != 0)
    {
      OnFormatError(std::string("unused data at the end of the datagram"));
      return;  // there are some unused data bytes at the end of the datagram
    }
  }


  //
  // C) Check the internal structure of the commands
  //
  {
    DWORD offset = sizeof(CANoeFDX::DatagramHeader);
    for (DWORD i=0; i < header->numberOfCommands; i++)
    {
      CANoeFDX::CommandHeader* command = reinterpret_cast<CANoeFDX::CommandHeader*>((char*)datagram.Buffer()+offset);
      switch(command->commandCode)
      {
      case CANoeFDX::kCommandCode_DataExchange:
        if (command->commandSize < CANoeFDX::kDataExchangeBaseSize)
        {
          OnFormatError(std::string("data exchange command is too small"));
          return;  // invalid size for DataExchange command, command is to small
        }
        if (command->commandSize < CANoeFDX::kDataExchangeBaseSize+ static_cast<CANoeFDX::DataExchangeCommand*>(command)->dataSize )
        {
          OnFormatError(std::string("invalid size for data exchange command"));
          return;  // invalid size for DataExchange command, payload does not fit into command
        }
        break;

      case CANoeFDX::kCommandCode_Status:
        if (command->commandSize<sizeof(CANoeFDX::StatusCommand))
        {
          OnFormatError(std::string("status command is too small"));
          return;
        }

      case CANoeFDX::kCommandCode_DataError:
        if (command->commandSize<sizeof(CANoeFDX::DataErrorCommand))
        {
          OnFormatError(std::string("data error command is too small"));
          return;
        }
                
      case CANoeFDX::kCommandCode_HardwareChanged:
        if (command->commandSize<sizeof(CANoeFDX::CommandHeader))
        {
          OnFormatError(std::string("hardware changed command is too small"));
          return;
        }
        break;

      default:
        // ignore any unknown command
        break;
      }
      offset += command->commandSize;
    }
  }

  //
  // Dispatch command
  //
  {
    DWORD offset = sizeof(CANoeFDX::DatagramHeader);
    for (DWORD i=0; i < header->numberOfCommands; i++)
    {
      CANoeFDX::CommandHeader* command = reinterpret_cast<CANoeFDX::CommandHeader*>((char*)datagram.Buffer()+offset);
      switch(command->commandCode)
      {
      case CANoeFDX::kCommandCode_DataExchange:
        OnDataExchange(header, static_cast<CANoeFDX::DataExchangeCommand*>(command));
        break;
      case CANoeFDX::kCommandCode_Status:
        OnStatus(header, static_cast<CANoeFDX::StatusCommand*>(command));
        break;
      case CANoeFDX::kCommandCode_DataError:
        OnDataError(header, static_cast<CANoeFDX::DataErrorCommand*>(command));
        break;
      default:
        // ignore any other command
        break;
      }
      offset += command->commandSize;
    }
  }
}

void FDXDispatcher::OnFormatError(std::string &error)
{
  printf("Format Error: %s\n", error);
}

void FDXDispatcher::OnSequenceNumberError(CANoeFDX::DatagramHeader* header, CANoeFDX::uint16 expectedSeqNr)
{
  printf("Sequence Number Error: expected SeqNr: %d, received SeqNr: %d\n", expectedSeqNr, header->sequenceNumber);
}

void FDXDispatcher::OnStatus(CANoeFDX::DatagramHeader* header, CANoeFDX::StatusCommand* command)
{
  if (command->measurementState == CANoeFDX::kMeasurementState_NotRunning)
  {
    printf("CANoe measurement is not running\n");
  }
}

void FDXDispatcher::OnDataError(CANoeFDX::DatagramHeader* header, CANoeFDX::DataErrorCommand* command)
{
  if (command->dataErrorCode != CANoeFDX::kDataErrorCode_MeasurementNotRunning)
  {
    printf("Data error for group ID %d, error code %d\n", command->groupID, command->dataErrorCode);
  }
}
