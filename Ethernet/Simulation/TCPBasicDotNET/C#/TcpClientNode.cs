/// ----------------------------------------------------------------------------------------
/// TCP Demo (Client side)
/// (c) by Vector Informatics GmbH
/// 
///
/// Hints:
/// - If reference to "System" is missing you have to add it manually.
///
/// - To use SystemVariables, you have to be sure the generated "...sysvars.dll" is within your references. 
///   If not, add it by browsing to the file.
/// ----------------------------------------------------------------------------------------
/// 
using System;
using Vector.Tools;
using Vector.CANoe.Runtime;
using Vector.CANoe.Sockets;
using System.Runtime.Remoting.Messaging;
using System.Collections.Generic;
using System.Diagnostics;


/// <summary>
/// ----------------------------------------------------------------------------------------
/// TcpClientNode Measurement Script class.
/// This represent the CANoe-Simulation node, that is a basic MeasurementScript 
/// that will initialize a TcpServer, doing the networking stuff.
/// ----------------------------------------------------------------------------------------
/// </summary>
public class TcpClientNode : MeasurementScript
{
  /// <summary>
  /// initialize measurement
  /// </summary>
  public override void Initialize()
  {
    this.Client = new SimpleTcpClient();
    NSys.TcpClient.ServerResponse.Value = "";
  }


  /// <summary>
  /// start measurement
  /// </summary>
  public override void Start()
  {
  }

  /// <summary>
  /// stop measurement
  /// </summary>
  public override void Stop()
  {
    if (this.Client.Connected)
    {
      this.Client.Disconnect();
    }
  }

  /// <summary>
  /// shutdown measurement
  /// </summary>
  public override void Shutdown()
  {
  }


  /// <summary>
  /// server instance.
  /// </summary>
  private SimpleTcpClient Client
  {
    get;
    set;
  }

  /// <summary>
  /// react changing the action variable and do the requested action.
  /// </summary>
  [OnChange(typeof(NSys.TcpClient.Action))]
  public void OnActionChanged()
  {
    this.Client.DoAction(NSys.TcpClient.Action.Value);
  }

}




/// <summary>
/// ----------------------------------------------------------------------------------------
/// The main TCP server class.
/// 
/// Demonstrates how to work with the TCP networking classes.
/// ----------------------------------------------------------------------------------------
/// </summary>
internal class SimpleTcpClient
{
  /// <summary>
  /// constant integer for the TCP-Buffer's size.
  /// </summary>
  private const int TCP_DATA_SIZE = 8192;

  /// <summary>
  /// constructor
  /// </summary>
  public SimpleTcpClient()
  {
  }

  /// <summary>
  /// connect to server.
  /// </summary>
  public void Connect()
  {
    this.Disconnect();
    Output.WriteLine("Client: Connecting to server...");
    // to keep things simple, connect to the server configured within the system variables..
    var serverIP = System.Net.IPAddress.Parse(NSys.TcpServer.IP.Value);
    var serverPort = NSys.TcpServer.Port.Value;
    this.CurrentState = eClientState.eConnecting;
    
    // create a tcp client socket...
    this.ClientSocket = new TcpClient();
    
    // start connecting to the server...
    this.ClientSocket.BeginConnect(serverIP, serverPort, this.OnConnectionRequestDoneCallback, null);
  }

  /// <summary>
  /// connection to server established...
  /// </summary>
  /// <param name="ar"></param>
  private void OnConnectionRequestDoneCallback( IAsyncResult result )
  {
    if (result != null)
    {
      if (result.IsCompleted)
      {
        this.ClientSocket.EndConnect(result);
        Output.WriteLine("Client: Connection established. Await welcome...");

        // start receiving and check for welcome...
        this.ReceiveResponse();

      }
      else
      {
        Output.WriteLine("Client: Connection to server could not be established. (Server started?)");
      }
    }
  }

  /// <summary>
  /// send data to the server and await the server response.
  /// </summary>
  /// <param name="clientSocket"></param>
  /// <param name="data"></param>
  private void SendRequest( string data )
  {
    var noConnectionErrorMsg = "Client: Can not send due there is no connection to the server established.";
    if (this.ClientSocket == null)
    {
      Output.WriteLine(noConnectionErrorMsg);
      return;
    }

    if (!this.ClientSocket.Connected)
    {
      Output.WriteLine(noConnectionErrorMsg);
      return;
    }

    Output.WriteLine(String.Format("Client: Sending '{0}' to server...", data));
    try
    {
      this.ClientSocket.GetStream().BeginWrite(System.Text.Encoding.UTF8.GetBytes(data), 0, data.Length, this.OnWriteDoneCallback, null);
    }
    catch (Exception /*e*/)
    {
      Output.WriteLine("Client: Exception on writing to network stream.");
      this.Disconnect();
    }
    
    /// ... and await /receive the response
    this.ReceiveResponse();
  }

  /// <summary>
  /// writing data to socket done.
  /// </summary>
  /// <param name="ar"></param>
  private void OnWriteDoneCallback( IAsyncResult result )
  {
    if (result != null)
    {
      this.ClientSocket.GetStream().EndWrite(result);
      if (!result.IsCompleted)
      {
        // incomplete... disconnect.
        Output.WriteLine("Client: Writing to network stream not completed.");
        this.Disconnect();
      }
    }
  }

  /// <summary>
  /// receive response from server...
  /// </summary>
  private void ReceiveResponse()
  {
    if (this.ClientSocket.Connected)
    {
      var buffer = new byte[TCP_DATA_SIZE];
      try
      {
        this.ClientSocket.GetStream().BeginRead(buffer, 0, TCP_DATA_SIZE, this.OnReadDoneCallback, buffer);
      }
      catch (Exception /*e*/)
      {
        Output.WriteLine("Client: Exception on reading from network stream.");
        this.Disconnect();
      }
    }
    else
    {
      Output.WriteLine("Client Error: Can't receive from socket due socket is not connected.");
    }
  }

  /// <summary>
  /// on reading from client done.
  /// </summary>
  /// <param name="ar"></param>
  private void OnReadDoneCallback( IAsyncResult result )
  {
    if (result != null)
    {
      if (result.IsCompleted)
      {
        this.ClientSocket.GetStream().EndRead(result);
        var buffer = (byte[])result.AsyncState;
        var dataString = System.Text.Encoding.UTF8.GetString(buffer).Split('\0')[0];
        Output.WriteLine(String.Format("Client: Received data from server: '{0}'", dataString));
        this.ReactOnResponse(dataString);
      }
      else
      {
        // incomplete... disconnect.
        Output.WriteLine("Client: Reading from network stream not completed.");
        this.Disconnect();
      }
    }

  }

  /// <summary>
  /// act on received data, depending on the current client state.
  /// </summary>
  /// <param name="dataString"></param>
  private void ReactOnResponse( string dataString )
  {
    // just write the received data to the panel...
    NSys.TcpClient.ServerResponse.Value = dataString;

    // depending on the current state different reactions are done...
    switch (this.CurrentState)
    {
      case eClientState.eDisconnected:
      {
        // in disconnected state do nothing special.
        break;
      }
      case eClientState.eConnecting:
      {
        // after connecting to server, the server should send a welcome to us...
        if (dataString=="WELCOME")
        {
          Output.WriteLine("Client: Got welcome from server.");
          this.CurrentState = eClientState.eConnected;
          break;
        }
        else
        {
          Output.WriteLine(String.Format("Client: Expected WELCOME from server, but got '{0}'", dataString));
          this.CurrentState = eClientState.eDisconnected;
        }
        break;
      }
      case eClientState.eConnected:
      {
        // during connection the server sends us either an OK followed by the u-case string we sent,
        // OR a plain "FAILED".
        if (dataString.CompareTo("NOT CONVERTED") == 0)
        {
          Output.WriteLine("Client: Server did not convert already uppercase string.");
        }
        else if (dataString.CompareTo("OK") == 0)
        {
          Output.WriteLine("Client: Server converted the string...");
          this.CurrentState = eClientState.eAwaitResultFromServer;

          // continue receiving, due the server now sends the converted string...
          this.ReceiveResponse();
        }
        break;
      }
      case eClientState.eAwaitResultFromServer:
      {
        // after awaited converting result from server, switch back to the connected state.
        this.CurrentState = eClientState.eConnected;
        break;
      }
      default:
      {
        Output.WriteLine("Client: Unsupported state " + this.CurrentState.ToString());
        break; // do nothing due this state is yet unknown.
      }
    }
  }

  /// <summary>
  /// disconnect from server
  /// </summary>
  public void Disconnect()
  {
    if (this.CurrentState == eClientState.eConnected)
    {
      if (this.ClientSocket != null)
      {
        if (this.ClientSocket.Connected)
        {
          Output.WriteLine("Client: Disconnecting.");
          this.ClientSocket.Close();
        }
        this.ClientSocket = null;
      }
    }
    else
    {
      Output.WriteLine("Client: Already disconnected.");
    }
    this.CurrentState = eClientState.eDisconnected;
  }


  /// <summary>
  /// a tcp socket for the client
  /// </summary>
  private TcpClient ClientSocket = null;

  /// <summary>
  /// current client state property to know what is expected from the server.
  /// </summary>
  private eClientState _currentState = eClientState.eDisconnected;
  public eClientState CurrentState
  {
    get
    {
      return this._currentState;
    }
    private set
    {
      this._currentState = value;
      NSys.TcpClient.Connected.Value = (value == eClientState.eDisconnected) ? 0 : 1;
    }
  }

  /// <summary>
  /// client states to know what expected from server...
  /// </summary>
  internal enum eClientState
  {
    eDisconnected = 0,
    eConnecting = 1,
    eConnected = 2,
    eAwaitResultFromServer = 3
  };



  public void DoAction( int actionNr )
  {
    // execute action with given number.
    switch (actionNr)
    {
      case 0:
      {
        // do nothing.
        break;
      }
      case 1:
      {
        // connect to server allowing to reconnect when triggering action multiple times.
        this.Connect();
        break;
      }
      case 2:
      {
        // disconnect from server.
        this.Disconnect();
        break;
      }
      case 3:
      {
        // send data string to server and await the response...
        this.SendRequest(NSys.TcpClient.DataString.Value);
        break;
      }
      default:
      {
        Output.WriteLine(String.Format("Client: Unimplemented action #{0}", actionNr));
        break;
      }
    }

    // reset the action back to 0 to be able to react on a variable change again.
    NSys.TcpClient.Action.Value = 0;
  }

  /// <summary>
  /// get information if client is connected.
  /// </summary>
  public bool Connected
  {
    get
    {
      return this.CurrentState == eClientState.eConnected;
    }
  }
}