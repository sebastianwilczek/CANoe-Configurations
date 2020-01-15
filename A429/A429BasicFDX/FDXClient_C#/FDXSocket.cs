/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: FDXSocket
-------------------------------------------------------------------------------
Windows socket wrapper for CANoe FDX protocol
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/
using System;

namespace FDXClient
{
  class FDXSocket
  {
    /// <summary>
    /// Default-constructor of a FDXSocket that set default values for destination address (127.0.0.1) 
    /// and port (2809).
    /// </summary>
    public FDXSocket()
    {
      this.mCanoeAddr = System.Net.IPAddress.Parse("127.0.0.1");
      this.mPort = 2809;
    }

    /// <summary>
    /// This functions open the socket for further communication
    /// </summary>
    public void Open()
    {
      this.mSocket = new System.Net.Sockets.UdpClient(this.mCanoeAddr.ToString(), this.mPort);
      this.mNextTransmitSequenceNumber = FDXHelperSequenceNumber.kSequenceNumberStart;
    }

    /// <summary>
    /// This function closes the socket
    /// </summary>
    public void Close()
    {
      this.mSocket.Close();
    }

    /// <summary>
    /// This function sets the destination address and port of the socket connection. This function will not reset
    /// the current connection.
    /// </summary>
    /// <param name="host">new destination adress to the host</param>
    /// <param name="port">new UDP-port to the host</param>
    public void SetCANoeAddr(string host, ushort port)
    {
      this.mCanoeAddr = System.Net.IPAddress.Parse(host);
      this.mPort = port;
    }

    /// <summary>
    /// This function sends out a FDXDatagram 
    /// </summary>
    /// <param name="datagram">the datagram that is to be send</param>
    /// <returns></returns>
    public int Send(ref FDXDatagram datagram)
    {
      //Set the current valid sequence number
      datagram.SetSequenceNumber(this.mNextTransmitSequenceNumber);
      
      // convert the datagram object to a byte array
      var outData = datagram.GenerateBuffer();
      var res = this.mSocket.Send(outData, outData.Length);

      // Increment the sequence number for the next datagram
      this.mNextTransmitSequenceNumber = FDXDatagram.IncrementSequenceNumber(this.mNextTransmitSequenceNumber);
      return res;
    }

    /// <summary>
    /// This function receives a datagram on the socket
    /// </summary>
    /// <param name="datagram">Reference to a datagram to store the information</param>
    /// <returns>size of the received datagram, -1 if no data is received</returns>
    public int Receive(ref FDXDatagram datagram)
    {
      // Get the IPEndpoint
      var ep = new System.Net.IPEndPoint(this.mCanoeAddr, this.mPort);

      try
      {
        if (this.mSocket.Available > 0)
        {
          datagram.Buffer = this.mSocket.Receive(ref ep);
        }       
      }
      catch (System.Net.Sockets.SocketException)
      {
        Console.Out.WriteLine("No answer from CANoe Client");
        return -1;
      }
      
      return datagram.Buffer.Length;
    }

    #region Variables
    private System.Net.Sockets.UdpClient mSocket;
    private System.Net.IPAddress mCanoeAddr;
    private ushort mPort;
    private ushort mNextTransmitSequenceNumber;
    #endregion
  }
}
