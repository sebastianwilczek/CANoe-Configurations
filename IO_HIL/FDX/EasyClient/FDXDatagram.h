/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: FDXDatagram
-------------------------------------------------------------------------------
Datagram builder class for CANoe FDX protocol
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/
#ifndef FDXDATAGRAM_H
#define FDXDATAGRAM_H

#include "CANoeFDX.h"


class FDXDatagram
{
public:
  FDXDatagram();

  void  InitWithHeader();
  void  SetSequenceNumber(CANoeFDX::uint16 sequenceNumber);
  void  AddDataRequest(CANoeFDX::uint16 groupID);
  void* AddDataExchange(CANoeFDX::uint16 groupID, CANoeFDX::uint16 dataSize);
  void  AddFreeRunningRequest(CANoeFDX::uint16 groupID, CANoeFDX::uint16 flags, CANoeFDX::uint32 cycleTime, CANoeFDX::uint32 firstDuration);
  void  AddFreeRunningCancel(CANoeFDX::uint16 groupID);
  void  AddStart();
  void  AddStop();

  inline void* Buffer() { return mBuffer; };
  inline CANoeFDX::uint32 Size() { return mSize; }
  inline void Size(CANoeFDX::uint32 s) { mSize = s; }
  inline CANoeFDX::uint32 Capacity() { return cCapacity; }

private:
  CANoeFDX::CommandHeader* AddCommand(CANoeFDX::uint16 commandCode, CANoeFDX::uint16 commandSize);

  static const CANoeFDX::uint32 cCapacity = 65536;
  CANoeFDX::uint8  mBuffer[cCapacity];
  CANoeFDX::uint32 mSize;
};



CANoeFDX::uint16 IncrementSequenceNumber(CANoeFDX::uint16 seqNr);


#endif;