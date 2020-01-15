/*-----------------------------------------------------------------------------
Module: VIA (observed)
Interfaces: This is a public interface header.
-------------------------------------------------------------------------------
External interface for to the CAN bus (for Nodelayer-DLLs)
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/

#ifndef VIA_CAN_H
#define VIA_CAN_H

/*-----------------------------------------------------------------------------
Version of the interfaces, which are declared in this header

19.01.2000   1.0  As      Creation
24.01.2000   1.1  As      Handling of Node improved
11.04.2003   1.2  As      Status Handling added (Busoff,ErrorActive,...)
                          Methods 'SetBtr' and 'SetCanCabsMode' added
25.02.2005   1.3  Rue     Method 'GetBtr' added
03.05.2007   1.4  As      SyncPulse added
08.06.2010   1.5  Bgr     Added VIAOnCanMessage2 - EVAL00066511
13.09.2012   1.6  As      TxFrameUpdate for interaction layer added
22.03.2013   1.8  Fsi     CAN-FD support: VIAOnCanMessage3, outputMessage3...
05.02.2014   1.9  Chk     Added SetCANFDConfiguration/GetCANFDConfiguration (EVAL00116698)
16.07.2015   1.10 Gw      Added VIAOnJ1939WideEvent
2015-10-02   1.11 Trs     Add TP channel access EVAL00083816
2015-12-18   1.12 Mkb     Added AUTOSAR PDU-Service
sometime     2.0          By definition version 2.0 will be the first release
in the                    of VIA interface, that is not longer compatible with 
future                    version 1.x 
-----------------------------------------------------------------------------*/

#define  VIACANMajorVersion 1
#define  VIACANMinorVersion 12

// ============================================================================
// Dependencies
// ============================================================================

#ifndef VIA_H
#include "VIA.h"
#endif

// ============================================================================
// CAN bus interface
// ============================================================================

//Specifies flags in a CAN-Message
enum VIAFlagsCanMessage
{
  kVIA_Rtr   = 1,  // RTR-Flag within a CAN-Message
  kVIA_Wu    = 2,  // Wakeup (high voltage message on single wire)
  kVIA_Te    = 4,  // transceiver error (two wire lowspeed CAN bus only, one
                   // of the two wires failed, the other is still working)
};

//Specifies flags for CAN and CAN-FD messages sent/received using VIAOnCanMessage3::OnMessage and VIACan::OutputMessage3
enum VIAFlagsCanMessage3
{
  kVIA_CAN_NERR              =    0x04, // NERR active during this message (single wire)
  kVIA_CAN_Wakeup            =    0x08, // message received in wakeup mode (high voltage)
  kVIA_CAN_RemoteFrame       =    0x10, // message is a remote frame
  kVIA_CAN_Srr               =  0x0200, // SRR (substitute remote request) bit for ExtId Can msgs, SRR is sent dominant if set
  kVIA_CAN_R0                =  0x0400, // R0 bit
  kVIA_CAN_R1                =  0x0800, // R1 bit
  kVIA_CAN_EDL               =  0x1000, // EDL (extended data length) bit for CAN FD
  kVIA_CAN_BRS               =  0x2000, // BRS (bit rate switch) bit for CAN FD
  kVIA_CAN_ESI               =  0x4000, // ESI (error state indicator) bit for CAN FD
  kVIA_CAN_ESICtrl           =  0x8000, // ESI control; if flag is set use ESI flag, else use node state
  kVIA_CAN_Consecutive       = 0x20000, // msg is a consecutive frame of a burst on the bus
};

// 
// Callback object to receive a message from the CAN bus 
// IMPORTANT: This function does only support CAN frames with up to 8 data bytes
// Supported by CAPL-on-Board: Yes
class VIAOnCanMessage
{
public:
  VIASTDDECL OnMessage(VIATime     time,         // timestamp of the message 
                       VIAChannel  channel,      // CAN channel
                       uint8       dir,          // direction of message (see enum VIADir)
                       uint32      id,           // CAN identifier
                       uint32      flags,        // CAN message flags (see enum VIAFlagsCanMessage)
                       uint8       dlc,          // data length code
                       const uint8 data[8]) = 0; // data bytes of the message  
};


#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 5)
// 
// Callback object to receive a message from the CAN bus in extended format
// IMPORTANT: This function does only support CAN frames with up to 8 data bytes
// Supported by CAPL-on-Board: No
class VIAOnCanMessage2
{
public:
  VIASTDDECL OnMessage(VIATime     time,             // timestamp of the message 
                       VIAChannel  channel,          // CAN channel
                       uint8       dir,              // see enum VIADir
                       uint32      id,               // CAN identifier
                       uint32      flags,            // see enum VIAFlagsCanMessage
                       uint8       dlc,              // data length code
                       const uint8 data[8],          // data bytes of the message
                       uint32      frameLength,      // frame length in ns
                       VIATime     startOfFrame,     // start of frame timestamp of the message
                       uint8       isConsecutive)=0; // 1-is consecutive frame, 0-is not a consecutive frame in a burst 
};
#endif
#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 8)
// 
// Callback object to receive a message from the CAN or CAN-FD bus
// This function supports CAN and CAN-FD frames with up to 64 data bytes
class VIAOnCanMessage3
{
public:
  VIASTDDECL OnMessage(VIATime     time,             // timestamp of the message 
    VIAChannel   channel,                            // CAN channel
    uint8        dir,                                // see enum VIADir
    uint32       id,                                 // CAN identifier
    uint32       flags,                              // see enum VIAFlagsCanMessage3
    uint32       frameLength,                        // frame length in ns
    VIATime      startOfFrame,                       // start of frame timestamp of the message
    uint32       mBtrCfgArb,                         // Configuration of data rate for arbitration phase
    uint32       mBtrCfgData,                        // Configuration of data rate for data phase
    uint32       mTimeOffsetBrsNs,                   // Timeoffset of BRS bit
    uint32       mTimeOffsetCrcDelNs,                // Timeoffset of CRC delimiter bit 
    uint16       mBitCount,                          // Bitcount of CAN frame
    uint32       mCRC,                                // CRC of CAN frame
    uint8        dataLength,                         // data length, 0 .. 64
    const uint8* data)=0;                               // data bytes of the message
};
#endif

#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 10)
// 
// Callback object to receive a virtual J1939 Wide event with DLC > 8 which is no message on the bus
// This event shall only used in CANoe internal!
// Supported by CAPL-on-Board: No
class VIAOnJ1939WideEvent
{
public:
  VIASTDDECL OnJ1939WideEvent(VIATime  time,  // timestamp of the message 
    VIAChannel  channel,                    // CAN channel
    uint8       dir,                        // direction of message (see enum VIADir)
    uint32      id,                         // CAN identifier
    uint8       da,                         // destination Address
    uint32      flags,                      // CAN message flags (see enum VIAFlagsCanMessage)
    uint8       dlc,                        // data length code
    const uint8* data) = 0;                 // data bytes of the message  
};
#endif

#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 9)

#if defined ( _MSC_VER )
#pragma warning( disable: 4103)
#pragma pack( push, 8)
#elif defined ( __BORLANDC__ )
#pragma option push -a8
#pragma nopushoptwarn
#pragma nopackwarning
#endif

struct VIACanSettings
{
  double baudrate; //in bits/s
  uint8  tseg1, tseg2;
  uint8  sjw;
  uint8  sam;
  uint32 flags;
};

#if defined ( _MSC_VER )
#pragma pack( pop)
#elif defined ( __BORLANDC__ )
#pragma option pop
#pragma nopushoptwarn
#pragma nopackwarning
#endif

#endif
//
// Callback object to receive error frames from the CAN bus  
// Supported by CAPL-on-Board: No
//
class VIAOnCanErrorFrame
{
public:
   VIASTDDECL OnErrorFrame(VIATime    time,        // timestamp of the message 
                           VIAChannel channel,     // CAN channel
                           uint8      dir,         // direction of message (see enum VIADir)
                           uint8      length,      // length of the frame (Number of bits)
                           uint32     flags ) = 0; // reserved for future use
};


// Specifies the error code for error status messages
enum VIACanErrorCode
{
  kVIA_CanErrorActive   = 1,
  kVIA_CanErrorPassiv   = 2,
  kVIA_CanWarningLimit  = 3,    
  kVIA_CanBusOff        = 4     
};

//
// Callback object to receive error status messages from the can controller
// (BusOff, ErrorPassiv, ...)
// Supported by CAPL-on-Board: No
//
class VIAOnCanError
{
public:
   VIASTDDECL OnError(VIATime    time,         // timestamp of the error status 
                      VIAChannel channel,      // CAN channel
                      uint32     errorCode,    // error code (see enum VIACanErrorCode)
                      uint8      errorCountTx, // Tx error counter of can controller
                      uint8      errorCountRx  // Rx error counter of can controller
                      ) = 0;
};


//Specifies pulse code for sync event
enum VIASyncPulseCode
{
  kVIA_SyncPulseExternal = 0x00,   // sync pulse triggered externally
  kVIA_SyncPulseOur      = 0x01,   // sync pulse triggered by the hardware itself
  kVIA_SyncPulseShared   = 0x02,   
};

//
// Callback object for the sync event
// Supported by CAPL-on-Board: No
//
class VIAOnSyncPulse
{
public:
  VIASTDDECL OnSyncPulse(VIATime     time,           // timestamp of the error status 
                         VIAChannel  channel,        // CAN channel
                         uint8       pulsecode) = 0; // pulse code of sync event (see enum VIASyncPulseCode)
};


// VIAOnTxFrameUpdate
// Callback object for update of the Payload of a Frame
// This callback is used by interaction layer Dlls to update the complete Tx buffer of a frame
// Supported by CAPL-on-Board: No
//
class VIAOnTxFrameUpdate
{
public:
  VIASTDDECL OnTxFrameUpdate(void*        userData,       // [in] data handle from NodeLayer-DLL, that was given CANoe when this TxFrameUpdate request was registered
                             const uint8* data,           // [in] the payload of the frame as array of bytes
                             uint32       dataSize) = 0;  // [in] size of the above array
};


// Forward declarations
class VIAPDUService;


// ============================================================================
// class VIACan
// Supported by CAPL-on-Board: Yes
// ============================================================================
class VIACan : public VIABus
{
public:
   // Registration for messages on the CAN bus
   // The Method should be called at 'InitMeasurement' only.
  // Supported by CAPL-on-Board: No
   VIASTDDECL CreateMessageRequest(VIARequestHandle* handle,   // [OUT] Created request handle
                                   VIAOnCanMessage* sink,      // [IN] Callback object
                                   uint8      requestType,     // [IN] Request type about message (see enum VIARequestType)
                                   uint32     id,              // [IN] Id
                                   VIAChannel channel,         // [IN] CAN channel
                                   uint32     mask) = 0;       // [IN] (?)

   // Registration for Error Frames on the CAN bus
   // The Method should be called at 'InitMeasurement' only.
   // Supported by CAPL-on-Board: No
   VIASTDDECL CreateErrorFrameRequest(VIARequestHandle* handle,   // [OUT] Created request handle
                                      VIAOnCanErrorFrame* sink,   // [IN] Callback object
                                      VIAChannel channel) = 0;    // [IN] CAN channel

   // Releases a Request (MessageRequest or ErrorFrameRequest)
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseRequest (VIARequestHandle handle) = 0;

   // Puts a message on the CAN bus
   // IMPORTANT: Only CAN frames with up to 8 data bytes are supported
   // Supported by CAPL-on-Board: Yes
   VIASTDDECL OutputMessage(VIAChannel  channel,       // [IN] CAN channel
                            uint32      id,            // [IN] CAN identifier
                            uint32      flags,         // [IN] CAN message flags (see enum VIAFlagsCanMessage)
                            uint8       dlc,           // [IN] Data length code
                            const uint8 data[8] ) = 0; // [IN] Data bytes of the message

   // Puts an error frame on the CAN bus
   // Supported by CAPL-on-Board: No
   VIASTDDECL OutputErrorFrame(VIAChannel channel,      // [IN] CAN channel
                               uint8      length,       // [IN] Length of the frame (Number of bits)
                               uint32     flags=0) = 0; // [IN] Reserved for future use

   // I don't know, what the following is doing, but it looks very important and 
   // is used by some of the old nodelayers
   // Supported by CAPL-on-Board: No
   VIASTDDECL InitPortBit      (uint8 bits) = 0;
   VIASTDDECL SetPortBit       (uint8 bits) = 0;
   VIASTDDECL InitCanRxInputs  (uint8 bits) = 0;
   VIASTDDECL SetCanRxInputs   (uint8 bits) = 0;
   VIASTDDECL SetPortBitEx     (uint8 mask, uint8 bits) = 0;
   VIASTDDECL ResetCan         (VIAChannel channel) = 0;
   VIASTDDECL SetCanOcr        (VIAChannel channel, uint8 ocr) = 0;

//
// The following methods are added with Version 1.2 of VIA_CAN
// 

   // Registration for Error Status message (BufOff, ErrorPassiv, ...)
   // The Method should be called at 'InitMeasurement' only.
   // Supported by CAPL-on-Board: No
   VIASTDDECL CreateStatusRequest (VIARequestHandle* handle,  // [OUT] Created request handle
                                   VIAOnCanError* sink,       // [IN] Callback object
                                   VIAChannel channel) = 0;   // [IN] CAN channel

   // Sets another baud rate. The values do not become active until the next 
   // call of the method ResetCan().
   // channel:  The CAN channel, where to change the baud rate 
   // btr0:     Value of Bit Timing Register 0
   // btr1:     Value of Bit Timing Register 0
   // Method returns always kVIA_OK
   // Supported by CAPL-on-Board: No
   VIASTDDECL SetBtr(VIAChannel channel, uint8 btr0, uint8 btr1) = 0;

   // Switch the mode of the transceiver (see CAPL function SetCanCabsMode)
   // Method returns kVIA_OK on success or kVIA_ServiceNotRunning, when calling the 
   // method is not allowed.
   // Supported by CAPL-on-Board: No
   VIASTDDECL SetCanCabsMode(VIAChannel channel, int32 type, int32 mode, int32 flags) = 0;
   
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)
   // Retrieves messages by ID.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetMessage(unsigned long ID, VIDBMessageDefinition** message) = 0;
   // You should retrieve messages using the ID. Use the name only if the ID is not available.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetMessage(const char* name, VIDBMessageDefinition** message) = 0;

   // Gets access to attributes of the network / bus itself:
   // Returns the attribute / the attribute type / the numeric value / the string value
   VIDB_DECLARE_ATTRIBUTE_HOLDER

   // Retrieves the definition of an arbitrary node on the bus
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetNodeDefinition(const char* nodeName, VIDBNodeDefinition** nodeDefinition) = 0;
   // Iterates over nodes
   VIDB_DECLARE_ITERATABLE(NodeDefinition)
#endif

   // Returns the currently set baud rate. See also SetBtr.
   // channel:  The CAN channel whose baud rate shall be returned. Wildcard is not allowed.
   // btr0:     Value of Bit Timing Register 0
   // btr1:     Value of Bit Timing Register 1
   // Returns kVIA_OK on success or kVIA_ServiceNotRunning on failure.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetBtr(VIAChannel channel, uint8* btr0, uint8* btr1) = 0; 

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 21)
   // Retrieves J1939 message with specified ID
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetJ1939Message(unsigned long ID, unsigned long flags, VIDBMessageDefinition** message) = 0;
#endif
   
   // Registration for the sync pulse event
   // The method should be called at 'InitMeasurement' only.
   // Supported by CAPL-on-Board: No
   VIASTDDECL CreateSyncPulseRequest (VIARequestHandle* handle,   // [OUT] Created request handle
                                      VIAOnSyncPulse* sink,       // [IN] Callback object
                                      VIAChannel channel) = 0;    // [IN] CAN channel


#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 5)
   // Registration for messages on the CAN bus
   // The Method should be called at 'InitMeasurement' only.
   // Supported by CAPL-on-Board: No
   VIASTDDECL CreateMessageRequest2   (VIARequestHandle* handle,   // the created request handle 
                                       VIAOnCanMessage2*  sink,    // callback object 
                                       uint8      requestType,     // see enum VIARequestType
                                       uint32     id,              // (?)           
                                       VIAChannel channel,         // (?)     
                                       uint32     mask) =0;        // (?)
#endif

#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 6)
   // Register for frame updates on the CAN bus
   // Supported by CAPL-on-Board: No
   VIASTDDECL CreateTxFrameUpdateRequest(VIARequestHandle* handle,        // [OUT] the created request handle 
                                         VIAOnTxFrameUpdate* sink,        // [IN] callback object
                                         void* userData,                   // [IN] handle from NodeLayer-DLL, that is given back when calling VIAOnTxFrameUpdate
                                         VIAChannel channel,              // [IN] channel
                                         VIDBMessageDefinition* message,  // [IN] database message object
                                         VIDBNodeDefinition* node) =0;    // [IN] node context (needed only by some protocols)
#endif

#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 7)
   VIASTDDECL OpenNetworkPanel(uint32 open) = 0;
#endif

#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 8)
   VIASTDDECL CreateMessageRequest3   (VIARequestHandle* handle,   // the created request handle 
     VIAOnCanMessage3*  sink,    // callback object 
     uint8      requestType,     // see enum VIARequestType
     uint32     id,              // (?)           
     VIAChannel channel,         // (?)     
     uint32     mask) =0;        // (?)

   // Output a CAN or CAN-FD message onto the bus, up to 64 data bytes are supported
   VIASTDDECL OutputMessage3(VIAChannel  channel,       // [IN] CAN channel
     uint32      id,         // [IN] CAN identifier
     uint32      flags,      // [IN] CAN message flags (see enum VIAFlagsCanMessage3)
     uint8       txReqCount, // [IN] Maximum number of transmission attempts
     uint8       dataLength, // [IN] Data length, 0 .. 64
     const uint8* data) =0;  // [IN] Data bytes of the message
     
#endif

#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 9)
   // Set the configuration of the CAN controller for given channel
   VIASTDDECL SetCanFdConfiguration(VIAChannel channel, //[IN] CAN channel
     VIACanSettings* arbSettings,                       //[IN] settings for arb. phase
     VIACanSettings* dbrSettings) = 0;                  //[IN] settings for data phase

   // Get the configuration of the CAN controller for given channel
   VIASTDDECL GetCanFdConfiguration(VIAChannel channel, //[IN] CAN channel
     VIACanSettings* arbSettings,                       //[OUT] settings for arb. phase
     VIACanSettings* dbrSettings) = 0;                  //[OUT] settings for data phase
#endif

#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 11)
   // Create a handle for a "TP channel" that provides hardware-based separation time for CAN messages
   VIASTDDECL CreateHWBasedSTminHandle(VIAChannel channel, //[IN] CAN channel
     uint32   id,                                          //[IN] CAN id
     int32    extension,                                   //[IN] address extension byte, or -1 if not used
     uint32   STmin_us,                                    //[IN] separation time to use, in µs
     uint32*  pHandleOut) = 0;                             //[OUT] handle
   // Release handle created with CreateHWBasedSTminHandle
   VIASTDDECL ReleaseHWBasedSTminHandle(VIAChannel channel, //[IN] CAN channel
     uint32   handle) = 0;                                  //[IN] handle

   // Like OutputMessage3, but with additional TP channel handle
   VIASTDDECL OutputMessage4(VIAChannel  channel,       // [IN] CAN channel
     uint32      id,         // [IN] CAN identifier
     uint32      flags,      // [IN] CAN message flags (see enum VIAFlagsCanMessage3)
     uint8       txReqCount, // [IN] Maximum number of transmission attempts
     uint8       dataLength, // [IN] Data length, 0 .. 64
     uint32      tpHandle,   // [IN] handle created with CreateHWBasedSTminHandle
     const uint8* data) = 0;  // [IN] Data bytes of the message
#endif

#if defined ( VIACANMinorVersion) && ( VIACANMinorVersion >= 11)
   VIASTDDECL GetPDUService(VIAPDUService** pduService) = 0;
#endif
}; // class VIACanBus


#endif // VIA_CAN_H
