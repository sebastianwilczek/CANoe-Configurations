/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: FDXSocket
-------------------------------------------------------------------------------
Windows socket wrapper for CANoe FDX protocol
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/
#ifndef FDXSOCKET_H
#define FDXSOCKET_H


#include "CANoeFDX.h"

class FDXDatagram;


class FDXSocket
{
public:
  static const int OK       = 0;
  static const int FAILED   = 1;
  static const int TIMEOUT  = 2;

  FDXSocket();
  ~FDXSocket();

  void Open();
  void Close();
  void SetCANoeAddr(const char* host, unsigned short port);

  int Send(FDXDatagram& datagram);
  int Receive(FDXDatagram& datagram);

private:
  bool         mWSAstarted;
  SOCKET       mSocket;
  sockaddr_in  mCANoeAddr;

  CANoeFDX::uint16 mNextTransmitSequenceNumber;
};


#endif;

