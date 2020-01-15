/*-----------------------------------------------------------------------------
CANoe.MOST COM Demo C#
-------------------------------------------------------------------------------
NOTES
This class demonstrates how to use the interfaces
- IMostNetworkInterface
- IMostApplicationFilter
- IMostDisassembler

See also the 'Technical COM Reference' of CANoe for details on the COM
interfaces and their functions and properties.

IMPORTANT
This code is not safe in respect to calls to COM interfaces. When implementing
a "real" COM Client you have to secure all calls to COM interface functions
and properties, because they can throw exceptions (failure HRESULT) as part
of normal operation.
-------------------------------------------------------------------------------
HOW TO: reduce performance issues when interacting with high load MOST systems.

There are two filter mechanisms that can be used to reduce the event load
between the CANoe COM server and a COM client. Both mechanisms filter the events
on server side, so the number of time-consuming transitions to the client are
reduced.

1) Property IMostNetworkInterface.EventSources
   This property is a bitfield that can be used to activate/deactivate the
   different event Handlers of IMostNetworkInterface.

2) Interface IMostApplicationFilter
   This interface can be accessed via the interface IMostNetworkInterface.
   It is a message/packet based filter similar to the MostApplicationFilter
   block of the CANoe Measurement Setup window. It can be used to further
   reduce the event load for the message/packet based event handlers of
   it's associated IMostNetworkInterface.
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/

using System;

namespace MostComClientCS
{
  class CANoeAccess : IDisposable
  {
    #region -------------------- Members --------------------

    //------------------------------
    // general CANoe Interfaces
    //------------------------------

    // --- The CANoe application ---
    CANoe.Application mApplication;

    // --- The Measurement interface ---
    // Control the measurement in CANoe (e.g. Start/Stop)
    CANoe.Measurement mMeasurement;


    //------------------------------
    // MOST specific CANoe Interfaces
    //------------------------------

    // --- The MostNetworkInterface interface ---
    // Access to MOST channel
    // - Messages from control channel (AMS/CMS)
    // - Packets from asynchronous channel
    // - Low-level information (e.g. light/lock, addresses, allocation table)
    CANoe.MostNetworkInterface mMostNetworkInterface;

    // --- The MostApplicationFilter interface ---
    // Similar to MOST application filter block in CANoe Measurement Setup.
    // See also "HOW TO: reduce performance issues ..." on top of this file.
    CANoe.MostApplicationFilter mMostApplicationFilter;

    // --- The MostDisassembler interface ---
    // Access to symbolic and numeric values of the parameters of a message.
    // Disassemble AMS/CMS messages by use of the MOST function catalog
    // of the CANoe configuration.
    CANoe.MostDisassembler mMostDisassembler;


    //------------------------------
    // received values
    //------------------------------
    string mDeckStatusSymbolicValue = String.Empty;
    int mDeckStatusNumericValue = 0;

    #endregion

    #region -------------------- IDisposable --------------------

    /// <summary>
    /// Call this method before destroying an instance of this class.
    /// </summary>
    public void Dispose()
    {
      DisconnectFromCANoe();
    }

    #endregion

    #region -------------------- Properties --------------------

    public string DeckStatusSymbolicValue { get { return mDeckStatusSymbolicValue; } }
    public int DeckStatusNumericValue { get { return mDeckStatusNumericValue; } }

    #endregion

    #region -------------------- provided Events --------------------

    /// <summary>
    /// Notification: indicates that property 'DeckStatus' was received
    /// </summary>
    public event EventHandler DeckStatusReceived;
    void Fire_DeckStatusReceived()
    {
      if( DeckStatusReceived != null )
        DeckStatusReceived( this, new EventArgs() );
    }

    #endregion

    #region -------------------- Access to CANoe COM Interfaces --------------------

    /// <summary>
    /// Get CANoe COM Interfaces and do some initializing steps
    /// </summary>
    public void ConnectToCANoe()
    {
      //----------------------------------------
      // Get CANoe COM interfaces
      //----------------------------------------

      // general CANoe Interfaces
      mApplication = new CANoe.Application();
      mMeasurement = (CANoe.Measurement)mApplication.Measurement;

      // MOST specific CANoe Interfaces
      CANoe.Network network = (CANoe.Network)mApplication.get_Networks( "MOST Ring" ); // The one and only MOST network of any CANoe configuration
      CANoe.Devices devices = (CANoe.Devices)network.Devices;
      CANoe.Device device = (CANoe.Device)devices["MOST 1"]; // The device of the CANoe configuration that has channel MOST1 assigned to
      mMostNetworkInterface = (CANoe.MostNetworkInterface)device.NetworkInterface;

      mMostApplicationFilter = (CANoe.MostApplicationFilter)mMostNetworkInterface.ApplicationFilter;
      mMostDisassembler = (CANoe.MostDisassembler)device.Disassembler;

      //----------------------------------------
      // Initialize MOST Filters
      //----------------------------------------

      // Prohibit MostNetworkInterface to fire any events
      mMostNetworkInterface.EventSources = 0;

      // Set filter-type of MostApplicationFilter to: pass filter
      mMostApplicationFilter.FilterType = 0;
      // Enable MostApplicationFilter
      mMostApplicationFilter.Enabled = true;

      //----------------------------------------
      // Register for events of MostNetworkInterface
      //----------------------------------------

      mMostNetworkInterface.OnAMSRx += new CANoe._IMostNetworkInterfaceEvents_OnAMSRxEventHandler( mostNetworkInterface_OnAMSRx );
    }

    /// <summary>
    /// Unregister from COM events and release referenced COM interfaces
    /// </summary>
    public void DisconnectFromCANoe()
    {
      //----------------------------------------
      // Unregister from events of MostNetworkInterface
      //----------------------------------------

      if( mMostNetworkInterface != null )
        mMostNetworkInterface.OnAMSRx -= new CANoe._IMostNetworkInterfaceEvents_OnAMSRxEventHandler( mostNetworkInterface_OnAMSRx );

      //----------------------------------------
      // Release referenced COM interfaces
      //----------------------------------------

      mMostApplicationFilter = null;
      mMostNetworkInterface = null;
      mMostDisassembler = null;
      mMeasurement = null;
      mApplication = null;
    }

    #endregion

    #region -------------------- IMeasurment --------------------

    /// <summary>
    /// Property indicating whether CANoe measurement is running
    /// </summary>
    public bool MeasurementRunning
    {
      get { return (mMeasurement != null) ? mMeasurement.Running : false; }
    }

    /// <summary>
    /// Start measurement in CANoe
    /// </summary>
    public void StartMeasurement()
    {
      if( !mMeasurement.Running )
        mMeasurement.Start();
    }

    /// <summary>
    /// Stop measurement in CANoe
    /// </summary>
    public void StopMeasurement()
    {
      if( mMeasurement.Running )
        mMeasurement.Stop();
    }

    #endregion

    #region -------------------- IMostNetworkInterface, IMostApplicationFilter, IMostDisassembler --------------------

    /// <summary>
    /// Request property 'DeckStatus' of the 'AudioDiskPlayer.01'
    /// </summary>
    public void GetPropertyDeckStatus()
    {
      //----------------------------------------
      // Configure filters
      //----------------------------------------

      // Permit CANoe to pass the MOST messages 'AudioDiskPlayer.01.DeckStatus.Status' to this client
      mMostApplicationFilter.AddApplicationEvent( -1, -1, 0x31, 0x01, 0x200, 0xC, -1, -1 );

      // Permit CANoe to pass received AMS node-messages to this client
      mMostNetworkInterface.EventSources = (int)CANoe.eMostEventSource.cMostEventSource_AMSRx;

      //----------------------------------------
      // Send the MOST message 'AudioDiskPlayer.01.DeckStatus.Get'
      //----------------------------------------

      // Let the AddressHandler of the CANoe ApplicationSocket resolve the destination address
      // (parameter 'destinationAddress': 0xFFFF)
      Byte[] data = new Byte[0];
      mMostNetworkInterface.OutputAMS( 0xFFFF, 0x31, 0x01, 0x200, 0x1, data.Length, data );
    }

    /// <summary>
    /// Called by CANoe on reception of AMS node-messages
    /// </summary>
    void mostNetworkInterface_OnAMSRx( int timeHigh, int time, int sourceAddress, int destinationAddress, int fBlockID, int instanceID, int functionID, int opType, int dataLength, object dataArray, int state )
    {
      // Only evaluate expected MOST messages (i.e. 'AudioDiskPlayer.01.DeckStatus.Status')
      if( fBlockID == 0x31 && instanceID == 0x01 && functionID == 0x200 && opType == 0xC )
      {
        //----------------------------------------
        // Get values of parameter 'DeckStatus' from message
        //----------------------------------------

        // Get the parameter list of the received MOST message
        object[] paramList = (object[])mMostDisassembler.ThisSymbolicParameterList( 1 );

        // The message 'AudioDiskPlayer.01.DeckStatus.Status' has exactly one parameter
        object[] deckStatusParam = (object[])paramList[0];

        // The values of the array of a parameter contains:
        // 1. property name
        // 2. symbolic value of the parameter (if available)
        // 3. numeric value of the parameter
        mDeckStatusSymbolicValue = (string)deckStatusParam[1];
        mDeckStatusNumericValue = (int)deckStatusParam[2];

        //----------------------------------------
        // Reset filters
        //----------------------------------------
        // This reduces the event load between CANoe COM server and this client to 0.
        // (No events are fired any more)
        mMostApplicationFilter.Clear();
        mMostNetworkInterface.EventSources = 0;

        //----------------------------------------
        // Notify GUI
        //----------------------------------------
        Fire_DeckStatusReceived();
      }
    }

    #endregion
  }
}
