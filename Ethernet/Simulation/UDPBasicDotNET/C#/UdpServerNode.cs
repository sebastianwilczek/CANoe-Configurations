/// ----------------------------------------------------------------------------------------
/// UDP Demo (Server side)
/// (c) by Vector Informatics GmbH
/// 
/// This is a small UDP Socket base demo. A UDP based communication is made up between a
/// client and a server in one way. The client just triggers something on the server to show
/// how sending and receiving on UDP sockets work.
/// The way back from the server to the client is left away to not mix up sending and
/// receiving.
/// There are 3 Classes that are used.
/// 
/// 1) UdpServerNode  : Represents the simulation node as MeasurementScript
/// 2) UdpServer      : The UdpServerNode contains one to do the udp-based networking.
/// 3) UdpServerLogics: The UdpServer contains one to do some logics that has
///                     primary nothing to do with the networking stuff.
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
/// UdpServerNode Measurement Script class.
/// This represent the CANoe-Simulation node, that is a basic MeasurementScript 
/// that will initialize an UdpServer, doing the networking stuff.
/// 
/// Additionally it reacts on System Variables:
/// - "NSys.UdpServer.Status"   to control the starting and stopping of the server over panel
/// - "NSys.UdpServer.LEDValue" to reflect the value within the LED's on the panel.
/// 
/// The server consists of a queue, that will be processed over a timer every 120 ms.
/// If a client sends an action over the socket the server immediately enqueues the bytes
/// for later execution after a timer has elapsed.
/// 
/// This is one possible way to realize a non-blocking action based software node.
/// Be careful: It's not "allowed" to have a blocking code. 
/// When blocking (e.g. thread.sleep) the node, the complete measurement is not "reacting"
/// anymore, and you will not get what you expect...
/// (The same behavior like working with CAPL based nodes)
/// ----------------------------------------------------------------------------------------
/// </summary>
public class UdpServerNode : MeasurementScript
{
  /// <summary>
  /// initialize measurement
  /// </summary>
  public override void Initialize()
  {
    this.Server = new UdpServer();
  }


  /// <summary>
  /// start measurement
  /// </summary>
  public override void Start()
  {
    this.Server.Start();
  }

  /// <summary>
  /// stop measurement
  /// </summary>
  public override void Stop()
  {
    this.Server.Stop();
  }

  /// <summary>
  /// shutdown measurement (clean up)
  /// </summary>
  public override void Shutdown()
  {
  }

  /// <summary>
  /// React on sysvar of the server-state change.
  /// </summary>
  [OnChange(typeof(NSys.UdpServer.Status))]
  public void OnSysVarStatusChange()
  {
    if (NSys.UdpServer.Status.Value.Equals(1))
    {
      this.Server.Start();
    }
    else
    {
      this.Server.Stop();
    }
  }

  /// <summary>
  /// on the led value change reflect the value on the leds.
  /// </summary>
  [OnChange(typeof(NSys.UdpServer.LEDValue))]
  public void OnSysVarLEDValueChange()
  {
    var ledValue = NSys.UdpServer.LEDValue.Value;
    var ledStates = new int[] { 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i <= 7; i++)
    {
      int v = 1 << i;
      ledStates[i] = ((ledValue & v) == v) ? 1 : 0;
    }
    NSys.UdpServer.LEDStates.Value = ledStates;
  }

  /// <summary>
  /// measurement script has an udp server.
  /// </summary>
  private UdpServer Server
  {
    get;
    set;
  }
}





/// <summary>
/// ----------------------------------------------------------------------------------------
/// UDP Server main class.
/// 
/// This implements (encapsulates) an UDP-Server that is used by the UdpServerNode.
/// 
/// This shows how to work and receive data on an udp-socket.
/// 
/// The server consists of a listening udp socket. (do not struggle around with the
/// naming, The UDP-socket is encapsulated within the class that is called "UdpClient".
/// 
/// Within UDP there is no real difference between a receiving or sending socket.
/// The main difference is that data on the socket is started to "receive".
/// To show the difference between a client and a server, this demo shows a one way from
/// client as trigger and the server that just reacts to the received data.
/// 
/// A command to the server is simply a sequence of bytes starting with an action number
/// followed by (optionally) parameters used by the action.
/// (e.g. Action #2 set LED value to 0x12 consists of 2 bytes: 0x02, 0x12)
/// 
/// As a special case, due the server internally also uses the actions itself,
/// "silent" actions have a leading "0". This "trick" is a server internal behavior and
/// should not be triggered this way from outside.
/// (e.g. Action #2 set LED value silent to 0x12 consists of 3 bytes: 0x00, 0x02, 0x12)
/// 
/// The main difference between a normal action and a silent action is the output that is
/// done on the write window. In silent mode there is no output done to the write.
/// 
/// Finally there is some additional stuff and logic beside, that has nothing to do with
/// the plain networking, but to have something that works within the demo.
/// For information about how to work with the UDP-Sockets, you can "ignore" things of the
/// EcuLogics-class.
/// ----------------------------------------------------------------------------------------
/// </summary>
internal class UdpServer
{
  /// <summary>
  /// constructor
  /// </summary>
  public UdpServer()
  {
    this.EcuLogics = new UdpServerLogics();
    this.EcuLogics.OutputToWrite("initializing...");
  }

  /// <summary>
  /// start the udp server listening on configured port (system variable)
  /// and start receiving incoming data.
  /// </summary>
  public void Start()
  {
    if (!this.Running)
    {
      this.Stop();
      var port = NSys.UdpServer.Port.Value;
      
      // open the udp-socket on configured port, and start receiving data on it.
      this.UdpListenSocket = new UdpClient(port);   // IMPORTANT: CREATE A SOCKET
      this.ReceiveUdpData();                        // IMPORTANT: LISTEN BY START RECEIVING DATA ON IT.
      
      this.EcuLogics.OutputToWrite("Start. (listening on port {0})...", port);
      this.EcuLogics.Start();
      this.Running = true;
    }
  }

  /// <summary>
  /// stop the server.
  /// </summary>
  public void Stop()
  {
    if (this.Running)
    {
      this.EcuLogics.OutputToWrite("Stop.");
      this.Running = false;
    }

    this.EcuLogics.Stop();

    if (this.UdpListenSocket != null)
    {
      this.UdpListenSocket.Close(); // IMPORTANT: CLOSE THE SOCKET WHEN DONE WORKING WITH IT!
    }
    this.UdpListenSocket = null;
  }

  /// <summary>
  /// server running indication.
  /// </summary>
  private bool _running = false;
  public bool Running
  {
    get
    {
      return this._running;
    }
    set
    {
      this._running = value;
      NSys.UdpServer.Status.Value = value ? 1 : 0;
    }
  }

  /// <summary>
  /// receive data on an udp socket...
  /// </summary>
  private void ReceiveUdpData()
  {
    try
    {
      this.UdpListenSocket.BeginReceive(this.OnUdpDataReceivedCallback, null);
    }
    catch (Exception /*e*/)
    {
      this.EcuLogics.OutputToWrite("Exception during receive from socket.");
      this.Stop();
    }
  }

  /// <summary>
  /// callback on receiving an udp datagram.
  /// </summary>
  /// <param name="asyncResult"></param>
  private void OnUdpDataReceivedCallback( IAsyncResult asyncResult )
  {
    if (asyncResult.IsCompleted)
    {
      var remoteEP = new System.Net.IPEndPoint(new System.Net.IPAddress(0), 0);
      byte[] data = this.UdpListenSocket.EndReceive(asyncResult, ref remoteEP);

      // work with the received data... (output to write and enqueue the data as action)
      var dataByteString = BitConverter.ToString(data);
      this.EcuLogics.OutputToWrite("UDP server received databytes: {0}", dataByteString);
      this.EcuLogics.ActionQ.Enqueue(data);

      // continue receiving data (receive data loop - this important to continue receiving!)
      this.ReceiveUdpData();  // IMPORTANT TO KEEP THE SERVER CONTINUE ITS WORK!
    }
    else
    {
      this.EcuLogics.OutputToWrite("Asynchronous receive was not finished successfully.");
      this.Stop();
    }
  }

  /// <summary>
  /// the udp-listen socket.
  /// </summary>
  private UdpClient UdpListenSocket
  {
    get;
    set;
  }

  /// <summary>
  /// a server logics (something that works for us)
  /// </summary>
  private UdpServerLogics EcuLogics
  {
    get;
    set;
  }
}





/// <summary>
/// ----------------------------------------------------------------------------------------
/// This is the server's internal logics that has not much to do with 
/// connecting or networking. It just is a class that represents the ecu's internal
/// logics and processing.
/// It consists of an action queue that is processed using a timer.
/// Also it contains the execution of an action.
/// 
/// So the UdpServer receives data on the UdpSocket, uses this logic to enqueue an action
/// and afterwards execute it timer based.
/// Executing the action just sets some system variables so the panel will reflect and contain
/// the proper data.
/// For steering the led's the LED-Value is changed. The measurementScript reacts on this
/// change and "translates" the value for setting the LED states on the panel.
/// ----------------------------------------------------------------------------------------
/// </summary>
internal class UdpServerLogics
{
  /// <summary>
  /// constructor
  /// </summary>
  public UdpServerLogics()
  {
    this.ActionQ = new Queue<byte[]>();
    this.ActionQTimer = new Timer(this.ActionQTimerElapsed);
    this.ActionQTimer.Interval = new TimeSpan(0, 0, 0, 0, 120); // every n ms
  }

  /// <summary>
  /// stop action timer
  /// </summary>
  public void Stop()
  {
    this.ActionQTimer.Stop();
    this.ActionQ.Clear();
  }

  /// <summary>
  /// start the action timer if not already started.
  /// </summary>
  public void Start()
  {
    if (!this.ActionQTimer.Enabled)
    {
      this.ActionQTimer.Start();
    }
  }

  /// <summary>
  /// action queue timer elapsed... execute the queued action.
  /// </summary>
  /// <param name="sender"></param>
  /// <param name="e"></param>
  void ActionQTimerElapsed( object sender, ElapsedEventArgs e )
  {
    if (this.ActionQ.Count > 0)
    {
      this.ExecuteAction(this.ActionQ.Dequeue());
    }
  }

  /// <summary>
  /// action queue timer to reduce the queue actions one by one...
  /// </summary>
  private Timer ActionQTimer
  {
    get;
    set;
  }

  /// <summary>
  /// action queue that will enqueue different byte-sequences to execute
  /// after a timer elapsed. An action is just a simple sequence of bytes.
  /// The first byte is an action number followed by action parameters the action
  /// supports.
  /// As a special case the queue is used internally also to do some known actions 
  /// in sequence. To not output too much on the write, an action can be prefixed with
  /// a leading "0" to indicate this action as a silent action.
  /// Silent means not to write to the write window in canoe.
  /// </summary>
  public Queue<byte[]> ActionQ
  {
    get;
    set;
  }

  /// <summary>
  /// inform by writing given text to write-window in canoe.
  /// </summary>
  /// <param name="text"></param>
  public void OutputToWrite( string text, params object[] args )
  {
    string msg = "UdpServer: " + text;
    Output.WriteLine(msg, args);
  }

  /// <summary>
  /// server executes given data.
  /// </summary>
  private void ExecuteAction( byte[] data )
  {
    if (data == null)
    {
      return;
    }

    // first byte is the action number.
    var actionNr = data[0];

    // ... but a "silent" action starts with 0
    bool silent = false;
    if ((data.Length > 1) && actionNr.Equals(0))
    {
      actionNr = data[1];
      var temp = new List<byte>(data);
      temp.RemoveAt(0);
      data = temp.ToArray();
      silent = true;
    }

    // depending on the action number now do some processing...
    switch (actionNr)
    {
      case 1:
      {
        // client action 1 - client sent a string...
        var receivedData = "";
        if (data.Length > 1)
        {
          receivedData = System.Text.Encoding.UTF8.GetString(data, 1, data.Length - 1);
        }
        if (!silent)
        {
          this.OutputToWrite("Executing action 1. Got string {0}", receivedData);
        }

        // reflect the data on the panel by setting it into the system variable
        // that is connected on the text-box on the panel.
        NSys.UdpServer.ReceiveData.Value = receivedData;
        break;
      }

      case 2:
      {
        // client action 2 - sent us a LED value...
        if (!silent)
        {
          this.OutputToWrite("Executing action 2. Settings LED value to {0}", data[1]);
        }

        // reflect the received led value on the panel by setting the value
        // to the panel-connected system variable for the led value.
        // In consequence a LEDValue change leads within the MeasurmentScript of the 
        // server node to reflect the value as LED-Strip on the panel.
        NSys.UdpServer.LEDValue.Value = data[1];
        break;
      }
      case 3:
      {
        // client action 3 - trigger error indication on leds...
        // wrap this with (silent) action 2, else the script is blocking.
        if (!silent)
        {
          this.OutputToWrite("Executing action 3. LED error indication...");
        }

        for (int i = 0; i < 5; i++)
        {
          this.ActionQ.Enqueue(new byte[] { 0, 2, 0 });
          this.ActionQ.Enqueue(new byte[] { 0, 2, 0xff });
        }
        for (int i = 0; i <= 2; i++)
        {
          this.ActionQ.Enqueue(new byte[] { 0, 2, 0x55 });
          this.ActionQ.Enqueue(new byte[] { 0, 2, 0xaa });
        }
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0 });
        break;
      }
      case 4:
      {
        // client action 4 - trigger led test pattern...
        // wrap this with (slient) action 2, else the script is blocking.
        if (!silent)
        {
          this.OutputToWrite("Executing action 3. LED test pattern...");
        }

        for (int i = 0; i < 7; i++)
        {
          this.ActionQ.Enqueue(new byte[] { 0, 2, (byte)(1 << i) });
        }
        for (int i = 7; i >= 0; i--)
        {
          this.ActionQ.Enqueue(new byte[] { 0, 2, (byte)(1 << i) });
        }

        this.ActionQ.Enqueue(new byte[] { 0, 2, 0 });
        break;
      }
      default:
      {
        // for an action that is not known, inform on the write,
        // followed up by a LED error sequence.
        this.OutputToWrite("Detected unsupported action {0}", actionNr);
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0xf0 });
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0x0f });
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0xf0 });
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0x0f });
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0xf0 });
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0x0f });
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0xf0 });
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0x0f });
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0xff });
        this.ActionQ.Enqueue(new byte[] { 0, 2, 0x00 });
        break;
      }
    }
  }

}
