/// ----------------------------------------------------------------------------------------
/// UDP Demo (Client side)
/// (c) by Vector Informatics GmbH
/// 
/// This is a small UDP Socket base demo. A UDP based communication is made up between a
/// client and a server in one way. The client just triggers something on the server to show
/// how sending and receiving on UDP sockets work.
/// The way back from the server to the client is left away to not mix up sending and
/// receiving.
/// There are 3 Classes that are used.
/// 
/// 1) UdpClientNode  : Represents the simulation node as MeasurementScript
/// 2) SimpleUdpClient: The UdpClientNode contains one to do the udp-based networking.
/// 3) UdpClientLogics: The SimpleUdpClient contains one to do some logics that has
///                     primary nothing to do with the networking stuff.
///                     
/// The UdpClientNode reacts on some keyboard inputs (OnKey-Change)
/// And also on System Variable changes.
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
using System.Net;
using System.Collections.Generic;
using System.Diagnostics;


/// <summary>
/// ----------------------------------------------------------------------------------------
/// UdpServerNode Measurement Script class.
/// This represent the CANoe-Simulation node, that is a basic MeasurementScript 
/// that will initialize a simple udp client doing the networking stuff.
/// 
/// Additionally it reacts on System Variables:
/// - "NSys.UdpServer.Read"   to connect to the server as soon as the server started
/// - "NSys.UdpClient.Action" to execute an action on the client (send something to server)
/// 
/// The OnKey-Handlers just trigger the client to do an action.
/// ----------------------------------------------------------------------------------------
/// </summary>
public class UdpClientNode : MeasurementScript
{
  /// <summary>
  /// initialize measurement
  /// </summary>
  public override void Initialize()
  {
  }

  /// <summary>
  /// start measurement
  /// </summary>
  public override void Start()
  {
    this.Client = new SimpleUdpClient();
  }

  /// <summary>
  /// stop measurement
  /// </summary>
  public override void Stop()
  {
    this.Client.Close();
  }

  /// <summary>
  /// shutdown measurement (clean up)
  /// </summary>
  public override void Shutdown()
  {
  }

  /// <summary>
  /// client executes action with given number.
  /// </summary>
  [OnChange(typeof(NSys.UdpClient.Action))]
  public void OnChangeClientAction()
  {
    var endPoint = new IPEndPoint(IPAddress.Parse(NSys.UdpServer.IP.Value), NSys.UdpServer.Port.Value);
    this.Client.DoAction(endPoint, NSys.UdpClient.Action.Value);
  }

  /// <summary>
  /// on key 1 pressed...
  /// </summary>
  [OnKey('1')]
  public void OnKey_1_Pressed( char key )
  {
    NSys.UdpClient.Action.Value = 1;
  }

  /// <summary>
  /// on key 2 pressed...
  /// </summary>
  [OnKey('2')]
  public void OnKey_2_Pressed( char key )
  {
    NSys.UdpClient.Action.Value = 2;
  }
  /// <summary>
  /// on key 3 pressed...
  /// </summary>
  [OnKey('3')]
  public void OnKey_3_Pressed( char key )
  {
    NSys.UdpClient.Action.Value = 3;
  }
  /// <summary>
  /// on key 4 pressed...
  /// </summary>
  [OnKey('4')]
  public void OnKey_4_Pressed( char key )
  {
    NSys.UdpClient.Action.Value = 4;
  }

  [OnKey('5')]
  public void OnKey_5_Pressed( char key )
  {
    // send a command the server has not implemented.
    NSys.UdpClient.Action.Value = 5;
  }

  [OnKey('6')]
  public void OnKey_6_Pressed( char key )
  {
    NSys.UdpClient.Action.Value = 6;
  }

  /// <summary>
  /// measurement script gets an udp server.
  /// </summary>
  private SimpleUdpClient Client
  {
    get;
    set;
  }

}





/// <summary>
/// ----------------------------------------------------------------------------------------
/// A simple udp client class.
/// It opens an UDP-Socket (UdpClient represents this), and uses this to send something
/// to the server.
/// 
/// A command to the server is simply a sequence of bytes starting with an action number
/// followed by (optionally) parameters used by the action.
/// ----------------------------------------------------------------------------------------
/// </summary>
internal class SimpleUdpClient
{
  /// <summary>
  /// constructor
  /// </summary>
  public SimpleUdpClient()
  {
    this.EcuLogics = new UdpClientLogics(); // create some logic object for the client.
    this.UdpClientSocket = new UdpClient(); // create an udp client socket.
  }

  /// <summary>
  /// Close the client and the underlying socket.
  /// </summary>
  public void Close()
  {
    this.UdpClientSocket.Close();
  }

  /// <summary>
  /// Different Actions that will send something to the server.
  /// It's a simple switch case to know what to do on which action.
  /// </summary>
  /// <param name="actionNr"></param>
  public void DoAction( IPEndPoint endPoint, int actionNr )
  {
    switch (actionNr)
    {
      case 0:
      {
        // do nothing. (initial value).
        break;
      }
      case 1:
      {
        // action 1: Send a string to the server.
        var guid = Guid.NewGuid().ToString();
        var dataStr = String.Format("Hello, this is string #{0} followed by a guid: {1}", this.EcuLogics.StrCount++, guid);
        this.SendToServer(endPoint, 1, System.Text.Encoding.UTF8.GetBytes(dataStr));
        break;
      }
      case 2:
      {
        // action 2: Send a byte for the server's led-strip.
        var r = (byte)(new Random().Next(0, 0xff));
        this.SendToServer(endPoint, 2, new byte[] {r});
        break;
      }
      case 3:
      {
        // action 3: Trigger the server to do an LED strip error indication
        this.SendToServer(endPoint, 3);
        break;
      }
      case 4:
      {
        // action 4: Trigger the server to play with the LEDs for checking all are working.
        this.SendToServer(endPoint, 4);
        break;
      }
      case 5:
      {
        // action 5: send invalid command to server.
        this.SendToServer(endPoint, 5, new byte[] { 1, 2, 3, 4, 5, 6, 7, 8, 9 });
        break;
      }
      default:
      {
        this.EcuLogics.OutputToWrite("Unknown action '{0}'", actionNr);
        break;
      }
    }

    // THIS IS IMPORTANT:
    // Switch back the system variable action to 0 that does nothing, to be able
    // to do the same action repeatedly. Reaction only takes place when the variable changes.
    // When using buttons, the variable will be reset to 0 through the button release,
    // but when setting the variable directly no repetition of the last action is possible.
    NSys.UdpClient.Action.Value = 0;
  }


  /// <summary>
  /// An udp client to use to contact the server.
  /// </summary>
  private UdpClient UdpClientSocket
  {
    get;
    set;
  }

  /// <summary>
  /// Send bytes to the server.
  /// </summary>
  /// <param name="actionNr">the number of the action</param>
  /// <param name="data">optinal additional data to send.</param>
  /// creates a datagram to send to the server
  /// The datagram consists of n bytes where the first byte is the action number followed by the arguments
  /// for the action.
  private void SendToServer( IPEndPoint endPoint, byte actionNr, byte[] data = null )
  {
    // the databytes start with the actionnumber.
    var dataBytes = new List<byte>();

    // first the action byte...
    dataBytes.Add(actionNr);

    // if there is data available, add it to the bytes to send...
    if (data != null)
    {
      dataBytes.AddRange(data);
    }

    var dataByteArray = dataBytes.ToArray();
    this.EcuLogics.OutputToWrite("Sending action {0} (Bytes: {1})", actionNr, BitConverter.ToString(dataByteArray));

    // IMPORTANT: Start sending data on the socket with BeginSend.
    this.UdpClientSocket.BeginSend(dataByteArray, dataByteArray.Length, endPoint, this.OnUdpSendCompleteCallback, null);
  }

  /// <summary>
  /// on sending complete.
  /// </summary>
  /// <param name="result"></param>
  private void OnUdpSendCompleteCallback( IAsyncResult result )
  {
    if (result != null)
    {
      if (result.IsCompleted)
      {
        // IMPORTANT: Sending was done and has to be terminated by EndSend.
        this.UdpClientSocket.EndSend(result);
      }
      else
      {
        // sending incomplete.
        this.EcuLogics.OutputToWrite("Sending incomplete.");
      }
    }
  }

  /// <summary>
  /// a client logics object (something that works for us beside the networking stuff)
  /// </summary>
  private UdpClientLogics EcuLogics
  {
    get;
    set;
  }


}




/// <summary>
/// ----------------------------------------------------------------------------------------
/// This is the clients internal logics that has not much to do with 
/// connecting or networking. It just is a class that represents the ecu's internal
/// logics and precessing.
/// ----------------------------------------------------------------------------------------
/// </summary>
internal class UdpClientLogics
{
  /// <summary>
  /// constructor
  /// </summary>
  public UdpClientLogics()
  {
  }

  /// <summary>
  /// inform by writing given text to write-window in canoe.
  /// </summary>
  /// <param name="text"></param>
  public void OutputToWrite( string text, params object[] args )
  {
    string msg = "UdpClient: " + text;
    Output.WriteLine(msg, args);
  }

  /// <summary>
  /// A simple string counter that is used to count the strings the client sent.
  /// This counter is contained within the sent string to see that it is changing
  /// when sending multiple times.
  /// </summary>
  public int StrCount
  {
    get;
    set;
  }
}