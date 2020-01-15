/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: FDXDispatcher
-------------------------------------------------------------------------------
Datagram dispatcher class for CANoe FDX protocol
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/
#ifndef FDXDISPATCHER_H
#define FDXDISPATCHER_H

#include "CANoeFDX.h"
#include "FDXDatagram.h"


class FDXDispatcher
{
public:
  FDXDispatcher();

  void DispatchDatagram(FDXDatagram& datagram);

  virtual void OnFormatError() {};
  virtual void OnSequenceNumberError(CANoeFDX::DatagramHeader* header, CANoeFDX::uint16 expectedSeqNr) {};

  virtual void OnStatus(CANoeFDX::DatagramHeader* header, CANoeFDX::StatusCommand* command) {};
  virtual void OnDataError(CANoeFDX::DatagramHeader* header, CANoeFDX::DataErrorCommand* command) {};
  virtual void OnDataExchange(CANoeFDX::DatagramHeader* header, CANoeFDX::DataExchangeCommand* command) {};

private:
  CANoeFDX::uint16 mNextExpectedSeqNr;
};


#endif
