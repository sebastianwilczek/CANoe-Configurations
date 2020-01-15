/// ----------------------------------------------------------------------------------------
/// TCP Demo (Server side)
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


/// <summary>
/// ----------------------------------------------------------------------------------------
/// TcpServerNode Measurement Script class.
/// This represent the CANoe-Simulation node, that is a basic MeasurementScript 
/// that will initialize a TcpServer, doing the networking stuff.
/// ----------------------------------------------------------------------------------------
/// </summary>
public class TcpServerNode : MeasurementScript
{
  /// <summary>
  /// initialize measurement
  /// </summary>
  public override void Initialize()
  {
    this.Server = new TcpServer();
  }


  /// <summary>
  /// start measurement
  /// </summary>
  public override void Start()
  {
    if (NSys.TcpServer.AutoStart.Value.Equals(1))
    {
      NSys.TcpServer.Status.Value = 1; // start server over sysvar to have the panel reflect the state.
    }
  }

  /// <summary>
  /// stop measurement
  /// </summary>
  public override void Stop()
  {
    this.Server.Stop();
  }

  /// <summary>
  /// shutdown measurement
  /// </summary>
  public override void Shutdown()
  {
  }

  /// <summary>
  /// on changing the server's state...
  /// </summary>
  [OnChange(typeof(NSys.TcpServer.Status))]
  public void OnSysVarStatusChanged()
  {
    if (NSys.TcpServer.Status.Value.Equals(1))
    {
      this.Server.Start();
    }
    else
    {
      this.Server.Stop();
    }
  }


  /// <summary>
  /// server instance.
  /// </summary>
  private TcpServer Server
  {
    get;
    set;
  }
}




/// <summary>
/// ----------------------------------------------------------------------------------------
/// The main TCP server class.
/// 
/// Demonstrates how to work with the TCP networking classes.
/// ----------------------------------------------------------------------------------------
/// </summary>
internal class TcpServer
{
  /// <summary>
  /// constant integer for the TCP-Buffer's max size.
  /// this has nothing to do with a MTU or something. Its just the buffer where the data of the 
  /// network stream can be buffered as byte array.
  /// </summary>
  private const int TCP_DATA_SIZE = 8192;

  /// <summary>
  /// constructor
  /// </summary>
  public TcpServer()
  {
    this.Running = false;
  }

  /// <summary>
  /// start the server.
  /// </summary>
  public void Start()
  {
    // stop server (if already running)
    this.Stop();

    // start listening on configured port...
    var serverIP = System.Net.IPAddress.Parse(NSys.TcpServer.IP.Value);
    var serverPort = NSys.TcpServer.Port.Value;
    this.TcpListenSocket = new TcpListener(serverIP, serverPort);
    this.TcpListenSocket.Start();
    this.AwaitIncomingClient();

    this.Running = true;
    Output.WriteLine("Server Started.");
  }

  /// <summary>
  /// await incoming client on listen socket...
  /// </summary>
  private void AwaitIncomingClient()
  {
    this.TcpListenSocket.BeginAcceptTcpClient(this.OnTcpClientConnectingCallback, null);
  }

  /// <summary>
  /// On TCP client connects to listen socket...
  /// </summary>
  /// <param name="result">Operation result is given into this callback</param>
  private void OnTcpClientConnectingCallback( IAsyncResult result )
  {
    if (result != null)
    {
      Output.WriteLine("Server: Incoming client.");

      // accept the client...
      var clientEntry = new ClientEntry();
      this.ConnectedClients.Add(clientEntry);
      clientEntry.Client = this.TcpListenSocket.EndAcceptTcpClient(result);

      // continue awaiting new clients on listen socket...
      this.AwaitIncomingClient();

      // say welcome to the client...
      Output.WriteLine("Server: Welcome client...");
      this.SendResponse(clientEntry, "WELCOME");

      // start receiving data from the client's network stream...
      this.ReceiveRequest(clientEntry);
    }
  }

  /// <summary>
  /// stop the server
  /// </summary>
  public void Stop()
  {
    if (this.Running)
    {
      if (this.TcpListenSocket != null)
      {
        this.TcpListenSocket.Stop();
        this.TcpListenSocket = null;
      }
      this.Running = false;
      if (this.ConnectedClients.Count > 0)
      {
        Output.WriteLine("Server: Stopping...");
      }
      foreach (var clientEntry in this.ConnectedClients.ToArray())
      {
        this.DisconnectClient(clientEntry);
      }
      Output.WriteLine("Server Stopped.");
    }
  }
  
  

  /// <summary>
  /// on reading from nwtwork stream done.
  /// </summary>
  /// <param name="result">The result of the network operation is given into this callback.</param>
  private void OnReadDoneCallback( IAsyncResult result )
  {
    if (result != null)
    {
      var clientEntry = (ClientEntry)result.AsyncState;
      clientEntry.Client.GetStream().EndRead(result);

      if (result.IsCompleted)
      {
        var dataString = System.Text.Encoding.UTF8.GetString(clientEntry.TcpData).Split('\0')[0];
        Output.WriteLine("Server: Received data from client: " + dataString);

        // react on request...
        this.ReactOnRequest(clientEntry, dataString);

        // continue reading from client...
        this.ReceiveRequest(clientEntry);
      }
      else
      {
        // incomplete... disconnect client.
        Output.WriteLine("Server: Reading from network stream not completed.");
        this.DisconnectClient(clientEntry);
      }
    }

  }

  /// <summary>
  /// Disconnect client with given client info
  /// </summary>
  /// <param name="clientEntry"></param>
  private void DisconnectClient( ClientEntry clientEntry )
  {
    Output.WriteLine("Server: Closing server side client socket.");
    clientEntry.Client.Close();
    this.ConnectedClients.Remove(clientEntry);
  }

  /// <summary>
  /// simple server logics acting on received data.
  /// </summary>
  /// <param name="clientEntry"></param>
  /// <param name="dataString">the request to react on.</param>
  private void ReactOnRequest( ClientEntry clientEntry, string dataString )
  {
    // ---------------------------------------------------------------------------
    // THIS IS A SIMPLE SERVER LOGICS THAT ACTS ON RECEIVED DATA...
    // ---------------------------------------------------------------------------

    // send back "OK" followed by the upper-string if the string can be converted to upper,
    // else send "NOT CONVERTED".
    if (dataString.ToUpper() == dataString)
    {
      Output.WriteLine("Server: Client data string is not convertible. Send NOT CONVERTED.");
      this.SendResponse(clientEntry, "NOT CONVERTED");
    }
    else
    {
      Output.WriteLine("Server: converting client's data string and send it back...");
      this.SendResponse(clientEntry, "OK");
      this.SendResponse(clientEntry, dataString.ToUpper());
    }
    // ---------------------------------------------------------------------------
    // DONE
    // --------------------------------------------------------------------------- 

  }

  /// <summary>
  /// send data to the client.
  /// </summary>
  /// <param name="clientEntry"></param>
  /// <param name="data">Data as string to send to the client.</param>
  private void SendResponse( ClientEntry clientEntry, string data )
  {
    if (clientEntry.Client != null)
    {
      try
      {
        clientEntry.Client.GetStream().BeginWrite(System.Text.Encoding.UTF8.GetBytes(data), 0, data.Length, this.OnWriteDoneCallback, clientEntry);
      }
      catch (Exception /*e*/)
      {
        Output.WriteLine("Server: Exception on writing to network stream.");
        this.DisconnectClient(clientEntry);
      }
    }
  }

  /// <summary>
  /// receive from given client...
  /// </summary>
  /// <param name="clientEntry"></param>
  private void ReceiveRequest( ClientEntry clientEntry )
  {
    if (clientEntry.Client.Connected)
    {
      Output.WriteLine("Server: Awaiting request...");

      Array.Clear(clientEntry.TcpData, 0, clientEntry.TcpData.Length);
      try
      {
        clientEntry.Client.GetStream().BeginRead(clientEntry.TcpData, 0, clientEntry.TcpData.Length, this.OnReadDoneCallback, clientEntry);
      }
      catch(Exception /*e*/)
      {
        Output.WriteLine("Server: Exception on reading from network stream.");
        this.DisconnectClient(clientEntry);
      }
    }
    else
    {
      Output.WriteLine("Server: Can't receive from client socket due socket is not connected.");
      this.DisconnectClient(clientEntry);
    }
  }

  /// <summary>
  /// writing data to socket done.
  /// </summary>
  /// <param name="result">Result of network operation is given into this callback</param>
  private void OnWriteDoneCallback( IAsyncResult result )
  {
    if (result != null)
    {
      var clientEntry = (ClientEntry)result.AsyncState;
      clientEntry.Client.GetStream().EndWrite(result);

      if (!result.IsCompleted)
      {
        // incomplete... disconnect client.
        Output.WriteLine("Server: Writing to network stream not completed.");
        this.DisconnectClient(clientEntry);
      }
    }

  }

  /// <summary>
  /// server running indicator.
  /// </summary>
  private bool Running
  {
    get;
    set;
  }

  /// <summary>
  /// client info. consists of a socket, and a Byte-Buffer for the Tcp data.
  /// </summary>
  private class ClientEntry
  {
    public TcpClient Client = null;
    public byte[] TcpData = new byte[TCP_DATA_SIZE];
  }

  /// <summary>
  /// tcp listen socket accepting incoming clients...
  /// </summary>
  private TcpListener TcpListenSocket
  {
    get;
    set;
  }

  /// <summary>
  /// keep track of all connected clients in case the server stops
  /// it has to disconnect all connected clients...
  /// </summary>
  private List<ClientEntry> ConnectedClients = new List<ClientEntry>();
}