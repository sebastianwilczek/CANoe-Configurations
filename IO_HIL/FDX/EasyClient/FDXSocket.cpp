/*-----------------------------------------------------------------------------
Module: CANoeFDX
-------------------------------------------------------------------------------
Windows socket wrapper for CANoe FDX protocol
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/


#include "stdafx.h"
#include "FDXSocket.h"
#include "FDXDatagram.h"



FDXSocket::FDXSocket()
 : mWSAstarted(false)
 , mSocket(INVALID_SOCKET)
 , mNextTransmitSequenceNumber(CANoeFDX::kSequenceNumberSessionStart)
{
  mCANoeAddr.sin_family = AF_INET;
  mCANoeAddr.sin_port = htons(2809);
  mCANoeAddr.sin_addr.s_addr = INADDR_LOOPBACK;
}



FDXSocket::~FDXSocket()
{
  Close();
}



void FDXSocket::Open()
{
  //---------------------------------------------
  // Initialize Winsock
  if (!mWSAstarted)
  {
    WSADATA wsaData;
    int startupResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (startupResult!=0)
    {
      printf("FDXSocket: WSAStartup failed \n");
      return;
    }
    mWSAstarted = true;
  }

  //---------------------------------------------
  // Create a socket for sending data
  mSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
  if (mSocket==INVALID_SOCKET)
  {
    printf("FDXSocket: creation of socket failed \n");
    return;
  }

  // set a 1000 ms timeout for receive operations
  int timeout = 1000; 
  int rc = setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));
  if (rc!=0)
  {
    printf("FDXSocket: Failed to set receive timeout \n");
  }

  //-----------------------------------------------
  // Bind the socket to any address and the specified port.
  int myPort = 2010;
  
  sockaddr_in RecvAddr;
  RecvAddr.sin_family = AF_INET;
  RecvAddr.sin_port = htons(myPort);
  RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  int rcBind = bind(mSocket, (SOCKADDR*) &RecvAddr, sizeof(RecvAddr));
  if (rcBind!=0)
  {
    printf("FDXSocket: bind operation failed \n");
    Close();
    return;
  }

  //---------------------------------------------------
  // reset sequence numbering
  mNextTransmitSequenceNumber = CANoeFDX::kSequenceNumberSessionStart;
}



void FDXSocket::Close()
{
  if (mSocket!=INVALID_SOCKET && mNextTransmitSequenceNumber!=CANoeFDX::kSequenceNumberSessionStart)
  {
    // send a datagram (with datagram header but without any command) to CANoe to end sequence numbering
    mNextTransmitSequenceNumber = mNextTransmitSequenceNumber | CANoeFDX::kSequenceNumberSessionEndFlag;
    
    FDXDatagram datagram;
    datagram.InitWithHeader();
    Send(datagram);
  }

  if (mSocket!=INVALID_SOCKET)
  {
    closesocket(mSocket);
    mSocket = INVALID_SOCKET;
  }

  if (mWSAstarted)
  {
    WSACleanup();
    mWSAstarted = false;
  }
}



void FDXSocket::SetCANoeAddr(const char* hostaddr, unsigned short port)
{
  mCANoeAddr.sin_family = AF_INET;
  mCANoeAddr.sin_port = htons(port);
  int rc = inet_pton(AF_INET, hostaddr, &mCANoeAddr.sin_addr);
  if (rc == 1)
  {
    printf("FDXSocket: target address set to '%s:%d' \n", hostaddr, port);
  }
  else
  {
    printf("FDXSocket: failed to set target address to '%s:%d' \n", hostaddr, port);
    mCANoeAddr.sin_addr.s_addr = INADDR_NONE;
  }
}



int FDXSocket::Send(FDXDatagram& datagram)
{
  WSABUF buf;
  buf.buf = reinterpret_cast<char*>(datagram.Buffer());
  buf.len = datagram.Size();
  DWORD numberOfBytesSent;
  DWORD flags =0;

  datagram.SetSequenceNumber(mNextTransmitSequenceNumber);

  int rcSend = WSASendTo(mSocket, &buf, 1, &numberOfBytesSent, flags, (SOCKADDR*)&mCANoeAddr, sizeof(sockaddr_in), NULL, NULL);
  if (rcSend!=0)
  {
    // send operation failed
    int wsaErrorCode = WSAGetLastError();
    if (wsaErrorCode == WSAETIMEDOUT)
    {
      printf("FDXSocket: send operation timeout \n");
      return TIMEOUT;
    }
    else
    {
      printf("FDXSocket: send operation failed \n");
      return FAILED;
    }
  }

  mNextTransmitSequenceNumber = IncrementSequenceNumber(mNextTransmitSequenceNumber);

  return OK;
}



int FDXSocket::Receive(FDXDatagram& datagram)
{
  WSABUF buf;
  buf.buf = reinterpret_cast<char*>(datagram.Buffer());
  buf.len = datagram.Capacity();
  DWORD numberOfBytesRecvd;
  DWORD flags =0;
  sockaddr_in from;
  int         fromLength = sizeof(sockaddr_in);

  int rcRecv = WSARecvFrom(mSocket, &buf, 1, &numberOfBytesRecvd, &flags, (SOCKADDR*)&from, &fromLength, NULL, NULL);
  if (rcRecv!=0)
  {
    // receive operation failed
    datagram.Size(0);
    int wsaErrorCode = WSAGetLastError();
    if (wsaErrorCode == WSAETIMEDOUT)
    {
      printf("FDXSocket: receive operation timeout \n");
      return TIMEOUT;
    }
    else
    {
      printf("FDXSocket: receive operation failed \n");
      return FAILED;
    }
  }

  datagram.Size(numberOfBytesRecvd);
  return OK;
}




