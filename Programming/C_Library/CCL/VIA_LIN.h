/*-----------------------------------------------------------------------------
Module: VIA (observed)
Interfaces: This is a public interface header.
-------------------------------------------------------------------------------
External interface for to the LIN bus (for Nodelayer-DLLs)
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/

#ifndef VIA_LIN_H
#define VIA_LIN_H

/*-----------------------------------------------------------------------------
Version of the interfaces, which are declared in this header

2002-09-03   1.0  As      Creation
2003-04-24   2.0  Hb,Sha  LIN Extension
2003-11-11   2.1  Hb      LIN Extension
2004-07-22   2.2  Sc      Value -2 for parameter response in OutputMessage
2005-05-10   2.3  Sha     New functions
2007-03-22   2.4  Sc      New functions to query protocol versions
2010-02-09   2.5  Sha     New event handlers for LIN bus events (extended with EarlyStopBitOffset for header and response)
2010-02-09   2.6  Sc?     OpenNetworkPanel
2010-02-09   2.7  Mth     LIN NM 2.0 (new set of LIN NM API functions)
2010-02-09   2.8  Mth     added simulated mode specific callbacks: OnSlaveResponseChanged, OnPreDispatchMessage
2016-10-05   2.9  Set     Added extended version of OnLinSchedulerModeChange
-----------------------------------------------------------------------------*/

#ifndef VIALINMajorVersion
  #define VIALINMajorVersion 2
#endif
#ifndef VIALINMinorVersion
  #define VIALINMinorVersion 9
#endif

// ============================================================================
// Dependencies
// ============================================================================

#ifndef VIA_H
#include "VIA.h"
#endif

// ============================================================================
// LIN bus interface
// ============================================================================


enum VIALinMessageFlag
{
  kVIA_ApplyHeader      =1,   // message header is applied to the bus (LIN Hardware master mode only)
  kVIA_ReconfigureData  =2    // transmission data will be reconfigured
};

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)
enum VIA_LINResultValue
{
  kVIA_LIN_Fail           = 400,  // LIN API Call failed (e.g. because the call in the current context is not allowed )
};
#endif

// 
// Callback object to receive a ctrl message from the LIN bus 
//
class VIAOnLinMessage
{
public:
  VIASTDDECL OnMessage(VIATime     time,       // event timestamp [in ns]
                       VIAChannel  channel,    // LIN channel
                       uint8       dir,        // see enum VIADir
                       uint32      id,         // LIN frame identifier
                       uint32      flags,      // reserved for future use
                       uint8       dlc,        // expected frame length [in bytes]
                       const uint8 data[8])= 0; // data bytes of the frame  
}; // class VIAOnLinMessage


#if defined ( VIALINMinorVersion) && ( VIALINMinorVersion >= 8)
// 
// Callback object to receive slave response updates from any node
// THIS IS ONLY SUPPORTED IN SIMULATED MODE!
//
class VIAOnLinSlaveResponseChanged
{
public:
  VIASTDDECL OnLinSlaveResponseChanged(VIATime     time,       // event timestamp [in ns]
                                       VIAChannel  channel,    // LIN channel
                                       uint8       dir,        // see enum VIADir
                                       uint32      id,         // LIN frame identifier
                                       uint32      flags,      // reserved for future use
                                       uint8       dlc,        // expected frame length [in bytes]
                                       const uint8 data[8]) = 0; // data bytes of the frame  
}; // class VIAOnLinSlaveResponseChanged

// 
// Callback object to notify about a LIN message to be dispatched.
// The response bytes may be modified by the callback function (IN/OUT parameter).
// The CRC is automatically recalculated by the bus simulator.
// THIS IS ONLY SUPPORTED IN SIMULATED MODE!
//
class VIAOnLinPreDispatchMessage
{
public:
  VIASTDDECL OnLinPreDispatchMessage(VIATime     time,       // [IN] event timestamp [in ns]
                                     VIAChannel  channel,    // [IN] LIN channel
                                     uint8       dir,        // [IN] see enum VIADir
                                     uint32      id,         // [IN] LIN frame identifier
                                     uint32      flags,      // [IN] reserved for future use
                                     uint8       dlc,        // [IN] expected frame length [in bytes]
                                     uint8       data[8]) = 0;     // [IN/OUT] data bytes of the frame (may be modified by the callback function)
}; // class VIAOnLinSlaveResponseChanged

#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)
// 
// Callback object to receive a frame from the LIN bus in extended format
//
class VIAOnLinMessage2
{
public:
  VIASTDDECL OnMessage(VIATime  timeFrameEnd,   // event timestamp [in ns]
                    VIATime     timeFrameStart, // start-of-frame timestamp [in ns]
                    VIATime     timeHeaderEnd,  // end-of-header timestamp [in ns]
                    VIATime     timeSynchBreak, // duration of Sync Break Low Phase transmission [in ns]
                    VIATime     timeSynchDel,   // duration of Sync Delimiter transmission [in ns]
                    VIAChannel  channel,        // LIN channel
                    uint8       dir,            // see enum VIADir
                    uint32      id,             // LIN frame identifier
                    uint8       dlc,            // expected frame length [in bytes]
                    uint8       simulated,      // flag specifies whether the frame was simulated
                    const uint8 data[8]         // data bytes of the frame  
                       )= 0;
}; // class VIAOnLinMessage
#endif

#if defined (VIAMinorVersion) && (VIAMinorVersion >= 22)
// 
// Callback object to receive a frame from the LIN bus (with ETF support)
//
class VIAOnLinMessage3
{
public:
  VIASTDDECL OnMessage(VIATime    timeFrameEnd,   // event timestamp [in ns]
    VIATime     timeFrameStart, // start-of-frame timestamp [in ns]
    VIATime     timeHeaderEnd,  // end-of-header timestamp [in ns]
    VIATime     timeSynchBreak, // duration of Sync Break Low Phase transmission [in ns]
    VIATime     timeSynchDel,   // duration of Sync Delimiter transmission [in ns]
    VIAChannel  channel,        // LIN channel
    uint8       dir,            // see enum VIADir
    uint32      id,             // LIN frame identifier
    uint8       dlc,            // expected frame length [in bytes]
    uint8       simulated,      // flag specifies whether the frame was simulated
    const uint8 data[8],        // data bytes of the frame  
    uint8       isETF           // true if Event-triggered frame is concerned 
    ) = 0;
};
#endif

#if defined ( VIALINMinorVersion) && ( VIALINMinorVersion >= 5)
// 
// Callback object to receive a frame from the LIN bus in format adjusted for CANoe/CANalyzer 7.5
//
class VIAOnLinMessage4
{
public:
  VIASTDDECL OnMessage(VIATime  timestampEvent,   // event timestamp [in ns]
    VIATime     timestampSOF,   // start-of-frame timestamp [in ns]
    VIATime     timestampEOH,   // end-of-header timestamp [in ns]
    VIATime     timeSynchBreak, // duration of Sync Break Low Phase transmission [in ns]
    VIATime     timeSynchDel,   // duration of Sync Delimiter transmission [in ns]
    VIAChannel  channel,        // LIN channel
    uint8       dir,            // see enum VIADir
    uint32      id,             // LIN frame identifier
    uint8       dlc,            // expected frame length [in bytes]
    uint8       simulated,      // flag specifies whether the frame was simulated
    const uint8 data[8],        // data bytes of the frame  
    uint8       isETF,          // true if Event-triggered frame is concerned 
    VIATime     timestampEOF,   // end-of-frame timestamp of the event [in ns] 
    uint32      earlyStopbitOffsetHeader,   // Early stop bit offset for UART timestamps in header [in ns]
    uint32      earlyStopbitOffsetResponse, // Early stop bit offset for UART timestamps in response [in ns]
    double      headerBaudrate  // measured header baudrate [in bps]
  )= 0;
}; // class VIAOnLinMessage4
#endif

// 
// Callback object to receive Transmission error frame from the LIN bus 
//
class VIAOnLinTransmissionError
{
public:
  VIASTDDECL OnTransmError(VIATime time,       // event timestamp [in ns]
                       VIAChannel  channel,    // LIN channel
                       uint8       FSMId,      // Identifier of the FSM that have responded to the message
                       uint32      id,         // LIN identifier
                       uint8       FSMState,   // the current state of the FSM 
                       uint8       dlc,        // expected frame length [in bytes]
                       uint8       FullTime,   // Length of the entire transmission procedure in bit times
                       uint8       HeaderTime  // Length of the message header in bit times
                       ) = 0;
};

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)
// 
// Callback object to receive Transmission error frame from the LIN bus in extended format
//
class VIAOnLinTransmissionError2
{
public:
  VIASTDDECL OnTransmError(VIATime timeEventEnd,  // event timestamp [in ns]
                      VIATime     timeEventStart, // start-of-frame timestamp [in ns]
                      VIATime     timeHeaderEnd,  // end-of-header timestamp [in ns]
                      VIATime     timeSynchBreak, // duration of Sync Break Low Phase transmission [in ns]
                      VIATime     timeSynchDel,   // duration of Sync Delimiter transmission [in ns]
                      VIAChannel  channel,        // LIN channel
                      uint32      id,             // LIN identifier
                      uint8       dlc             // expected frame length [in bytes]
                      ) = 0;
};
#endif

#if defined (VIAMinorVersion) && (VIAMinorVersion >= 22)
// 
// Callback object to receive Transmission Error event from the LIN bus (with ETF support)
//
class VIAOnLinTransmissionError3
{
public:
  VIASTDDECL OnTransmError(VIATime     timeEventEnd,   // event timestamp [in ns]
    VIATime     timeFrameStart, // start-of-frame timestamp [in ns]
    VIATime     timeHeaderEnd,  // end-of-header timestamp [in ns]
    VIATime     timeSynchBreak, // duration of Sync Break Low Phase transmission [in ns]
    VIATime     timeSynchDel,   // duration of Sync Delimiter transmission [in ns]
    VIAChannel  channel,        // LIN channel
    uint32      id,             // LIN frame identifier
    uint8       dlc,            // expected frame length [in bytes]
    uint8       isETF           // true if Event-triggered frame is concerned 
    ) = 0;
};
#endif

#if defined ( VIALINMinorVersion) && ( VIALINMinorVersion >= 5)
// 
// Callback object to receive Transmission error frame from the LIN bus in format adjusted for CANoe/CANalyzer 7.5
//
class VIAOnLinTransmissionError4
{
public:
  VIASTDDECL OnTransmError(VIATime timestampEvent,  // event timestamp [in ns]
    VIATime     timestampSOF,   // start-of-frame timestamp [in ns]
    VIATime     timestampEOH,   // end-of-header timestamp [in ns]
    VIATime     timeSynchBreak, // duration of Sync Break Low Phase transmission [in ns]
    VIATime     timeSynchDel,   // duration of Sync Delimiter transmission [in ns]
    VIAChannel  channel,        // LIN channel
    uint32      id,             // LIN frame identifier
    uint8       dlc,            // expected frame length [in bytes]
    uint8       isETF,          // true if Event-triggered frame is concerned 
    uint32      earlyStopbitOffsetHeader,   // Early stop bit offset for UART timestamps in header [in ns]
    double      headerBaudrate  // Measured header baudrate [in bps]
    ) = 0;
};
#endif

// 
// Callback object to receive CheckSum error frame from the LIN bus 
//
class VIAOnLinCheckSumError
{
public:
  VIASTDDECL OnCsError(VIATime     time,       // event timestamp [in ns]
                       VIAChannel  channel,    // LIN channel
                       uint16      crc,        // CRC of the frame
                       uint32      id,         // LIN identifier
                       uint8       dlc,        // expected frame length [in bytes]
                       uint8       dir,        // see enum VIADir
                       uint8       FSMId,      // Identifier of the FSM that transmitted to the message
                       uint8       FSMState,   // the current state of the FSM 
                       uint8       FullTime,   // Length of the entire message in bit times
                       uint8       HeaderTime, // Length of the message header in bit times
                       uint8       simulated,  // This flag specifies whether the original frame was simulated
                       const uint8 data[8]     // data bytes of the message  
                       ) = 0;
};

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)
// 
// Callback object to receive CheckSum error frame from the LIN bus in extended format
//
class VIAOnLinCheckSumError2
{
public:
  VIASTDDECL OnCsError(VIATime     timeEventEnd, // event timestamp [in ns]
                       VIATime     timeEventStart, // start-of-frame timestamp [in ns]
                       VIATime     timeHeaderEnd,  // end-of-header timestamp [in ns]
                       VIATime     timeSynchBreak, // duration of Sync Break Low Phase transmission [in ns]
                       VIATime     timeSynchDel,   // duration of Sync Delimiter transmission [in ns]
                       VIAChannel  channel,    // LIN channel
                       uint16      crc,        // CRC of the frame
                       uint32      id,         // LIN identifier
                       uint8       dlc,        // expected frame length [in bytes]
                       uint8       dir,        // see enum VIADir
                       uint8       simulated,  // This flag specifies whether the original frame was simulated
                       const uint8 data[8]     // data bytes of the message  
                       ) = 0;
};
#endif

#if defined ( VIALINMinorVersion) && ( VIALINMinorVersion >= 5)
// 
// Callback object to receive CheckSum error frame from the LIN bus in format adjusted for CANoe/CANalyzer 7.5
//
class VIAOnLinCheckSumError3
{
public:
  VIASTDDECL OnCsError(VIATime     timestampEvent, // event timestamp [in ns]
    VIATime     timestampSOF,   // start-of-frame timestamp [in ns]
    VIATime     timestampEOH,   // end-of-header timestamp [in ns]
    VIATime     timeSynchBreak, // duration of Sync Break Low Phase transmission [in ns]
    VIATime     timeSynchDel,   // duration of Sync Delimiter transmission [in ns]
    VIAChannel  channel,        // LIN channel
    uint16      crc,            // CRC of the frame
    uint32      id,             // LIN frame identifier
    uint8       dlc,            // expected frame length [in bytes]
    uint8       dir,            // see enum VIADir
    uint8       simulated,      // This flag specifies whether the original frame was simulated
    const uint8 data[8],        // data bytes of the message  
    VIATime     timestampEOF,   // end-of-frame timestamp [in ns] 
    uint32      earlyStopbitOffsetHeader,   // Early stop bit offset for UART timestamps in header [in ns]
    uint32      earlyStopbitOffsetResponse, // Early stop bit offset for UART timestamps in response [in ns]
    double      headerBaudrate  // Measured header baudrate [in bps]
  ) = 0;
};
#endif

// 
// Callback object to receive Receive error frame from the LIN bus 
//
class VIAOnLinReceiveError
{
public:
  VIASTDDECL OnReceiveError(VIATime     time,       // event timestamp [in ns]
                       VIAChannel  channel,         // LIN channel
                       uint8       FSMId,           // Identifier of the FSM that transmitted the message
                       uint32      id,              // LIN frame identifier
                       uint8       FSMState,        // the current state of the FSM
                       uint8       dlc,             // expected frame length [in bytes]
                       uint8       FullTime,        // Length of the entire transmission procedure in bit times
                       uint8       HeaderTime,      // Length of the message header in bit times
                       uint8       offendingByte,   // The byte that resulted in the protocol violation
                       uint8       shortError,      // Specifies the detail level of the event
                       uint8       stateReason,     // the lower 4 bits indicate the state, the upper 4 bits indicate the reason 
                       uint8       timeoutDuringDlcDetection
                       ) = 0;
};

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)
// 
// Callback object to receive Receive error frame from the LIN bus in extended format
//
class VIAOnLinReceiveError2
{
public:
  VIASTDDECL OnReceiveError(VIATime timeEventEnd,   // event timestamp [in ns]
                       VIATime     timeEventStart,  // start-of-event timestamp [in ns]
                       VIATime     timeHeaderEnd,   // end-of-header timestamp [in ns] 
                       VIATime     timeSynchBreak,  // duration of Sync Break Low Phase transmission [in ns]
                       VIATime     timeSynchDel,    // duration of Sync Delimiter transmission [in ns]
                       VIAChannel  channel,         // LIN channel
                       uint32      id,              // LIN identifier
                       uint8       dlc,             // expected frame length [in bytes]
                       uint8       offendingByte,   // The byte that resulted in the protocol violation
                       uint8       shortError,      // Specifies the detail level of the event
                       uint8       stateReason,     // the lower 4 bits indicate the state, the upper 4 bits indicate the reason 
                       uint8       timeoutDuringDlcDetection 
                       ) = 0;
};
#endif

#if defined (VIAMinorVersion) && (VIAMinorVersion >= 22)
// 
// Callback object to receive Receive Error event from the LIN bus (with ETF support)
//
class VIAOnLinReceiveError3
{
public:
  VIASTDDECL OnReceiveError(VIATime timeEventEnd,   // event timestamp [in ns]
    VIATime     timeEventStart,  // start-of-event timestamp [in ns]
    VIATime     timeHeaderEnd,   // end-of-header timestamp [in ns] 
    VIATime     timeSynchBreak,  // duration of Sync Break Low Phase transmission [in ns]
    VIATime     timeSynchDel,    // duration of Sync Delimiter transmission [in ns]
    VIAChannel  channel,         // LIN channel
    uint32      id,              // LIN frame identifier
    uint8       dlc,             // expected frame length [in bytes]
    uint8       offendingByte,   // The byte that resulted in the protocol violation
    uint8       shortError,      // Specifies the detail level of the event
    uint8       stateReason,     // the lower 4 bits indicate the state, the upper 4 bits indicate the reason 
    uint8       timeoutDuringDlcDetection, 
    uint8       isETF            // true if Event-triggered frame is concerned 
    ) = 0;

};

#endif 

#if defined ( VIALINMinorVersion) && ( VIALINMinorVersion >= 5)
// 
// Callback object to receive Receive error frame from the LIN bus in format adjusted for CANoe/CANalyzer 7.5
//
class VIAOnLinReceiveError4
{
public:
  VIASTDDECL OnReceiveError(VIATime timestampEvent,   // event timestamp [in ns]
    VIATime     timestampSOF,    // start-of-event timestamp [in ns]
    VIATime     timestampEOH,    // end-of-header timestamp [in ns] 
    VIATime     timeSynchBreak,  // duration of Sync Break Low Phase transmission [in ns]
    VIATime     timeSynchDel,    // duration of Sync Delimiter transmission [in ns]
    VIAChannel  channel,         // LIN channel
    uint32      id,              // LIN frame identifier
    uint8       dlc,             // data length code
    uint8       offendingByte,   // The byte that resulted in the protocol violation
    uint8       shortError,      // Specifies the detail level of the event
    uint8       stateReason,     // the lower 4 bits indicate the state, the upper 4 bits indicate the reason 
    uint8       timeoutDuringDlcDetection, // timeout for DLC detection
    uint8       isETF,           // true if Event-triggered frame is concerned 
    uint8       isHeaderError,   // true if this events caused by an error detected in header
    VIATime     timestampEOF,    // end-of-error timestamp [in ns] 
    uint32      earlyStopbitOffsetHeader,   // Early stop bit offset for UART timestamps in header [in ns]
    uint32      earlyStopbitOffsetResponse, // Early stop bit offset for UART timestamps in response [in ns]
    double      headerBaudrate   // Measured header baudrate [in bps]
  ) = 0;
};
#endif

// 
// Callback object to receive Sync error frame from the LIN bus 
//
class VIAOnLinSyncError
{
public:
  VIASTDDECL OnSyncError(VIATime     time,     // event timestamp [in ns]
                       VIAChannel  channel     // LIN channel
                       ) = 0;
};

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)
// 
// Callback object to receive Sync error frame from the LIN bus in extended format
//
class VIAOnLinSyncError2
{
public:
  VIASTDDECL OnSyncError(VIATime    timeEventEnd,    // event timestamp [in ns]
                       VIATime      timeEventStart,  // start timestamp of the event [in ns]
                       VIATime      timeSynchBreak,  // duration of Synch Break Low Phase transmission [in ns]
                       VIATime      timeSynchDel,    // duration of Synch Delimiter transmission [in ns]
                       VIAChannel   channel,         // LIN channel
                       const uint16 syncInterval[4]  // time intervals detected between the falling signal edges of the Synch field
                       ) = 0;
};
#endif

// 
// Callback object to receive Sleep Mode event from the LIN bus 
//
class VIAOnLinSleepModeEvent
{
public:
  VIASTDDECL OnSleepModeEvent(VIATime     time,  // event timestamp [in ns]
    VIAChannel  channel,      // LIN channel
    uint8       external,     // Indicates whether the change of state is the result of an external event
    uint8       isAwake,      // Indicates the current state of LIN Hardware
    uint8       reason,       // the reason for the event
    uint8       wasAwake      // Indicates which status LIN Hardware was in before the LinSleepModeEvent occurred
    ) = 0;
};

// 
// Callback object to receive Wakeup frame from the LIN bus 
//
class VIAOnLinWakeupFrame
{
public:
  VIASTDDECL OnWakeupFrame(VIATime     time,     // event timestamp [in ns]
    VIAChannel  channel,      // LIN channel
    uint8       external,     // Indicates whether the signal was transmitted by LIN Hardware itseelf
    uint8       signal        // Signal actually received
    ) = 0;
};

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)
// 
// Callback object to receive Wakeup frame from the LIN bus in extended format
//
class VIAOnLinWakeupFrame2
{
public:
  VIASTDDECL OnWakeupFrame(VIATime  timeFrameEnd,    // event timestamp [in ns]
    VIATime      timeFrameStart,  // start timestamp of the event [in ns] 
    VIAChannel   channel,         // LIN channel
    uint8        external,        // Indicates whether the signal was transmitted by LIN Hardware itself
    uint8        signal           // Signal actually received
    ) = 0;
};
#endif

// 
// Callback object to receive Baudrate event from the LIN bus 
//
class VIAOnLinBaudrateEvent
{
public:
  VIASTDDECL OnBaudrateEvent(VIATime     time,  // event timestamp [in ns]
                       VIAChannel  channel,     // LIN channel
                       int32       baudrate     // Measured baud rate in bits/sec
                       ) = 0;
};

// 
// Callback object to receive Dlc Info event from the LIN bus 
//
class VIAOnLinDLCInfo
{
public:
  VIASTDDECL OnDLCInfo(VIATime     time,     // event timestamp [in ns]
                       VIAChannel  channel,  // LIN channel
                       uint8       dlc,      // measured length of the message
                       uint32      id        // Identifier of the measured message
                       ) = 0;
};

// 
// Callback object to receive Slave Timeout error frame from the LIN bus 
//
class VIAOnLinSlaveTimeout
{
public:
  VIASTDDECL OnSlaveTimeout(VIATime     time,   // event timestamp [in ns]
    VIAChannel  channel,     // LIN channel
    uint8       followState, // follow up state of the FSM (Finite State Machine)
    uint8       slaveId,     // Identifier of the Slave
    uint8       stateId      // Identifier of the state of the FSM
    ) = 0;
};

// 
// Callback object to receive Scheduler Mode Change event from the LIN bus 
//
class VIAOnLinSchedulerModeChange
{
public:
  VIASTDDECL OnSchedulerModeChange(VIATime time,  // event timestamp [in ns]
    VIAChannel  channel,   // LIN channel
    uint8       newMode,   // new scheduler mode
    uint8       oldMode    // prior scheduler mode
    ) = 0;
};

#if defined (VIALINMinorVersion) && (VIALINMinorVersion >= 9)
// 
// Extended callback object to receive Scheduler Mode Change event from the LIN bus 
//
class VIAOnLinSchedulerModeChange2
{
public:
  VIASTDDECL OnSchedulerModeChange(VIATime time,  // event timestamp [in ns]
    VIAChannel  channel,   // LIN channel
    uint8       newMode,   // new scheduler mode
    uint8       oldMode,   // prior scheduler mode
    uint8       newSlot,   // new scheduler slot
    uint8       oldSlot    // prior scheduler slot
    ) = 0;
};

#endif

// ============================================================================
// class VIALin
// ============================================================================
class VIALin : public VIABus
{
public:
   // Releases a message request
   VIASTDDECL ReleaseRequest (VIARequestHandle handle) = 0;

   // Puts a message on the LIN bus; Checksum is automatically calculated
   VIASTDDECL OutputMessage(VIAChannel    channel,        // [IN] LIN channel
                            uint32        id,             // [IN] LIN identifier
                            uint32        flags,          // [IN] LIN message flags (see enum VIALinMessageFlags)
                            uint8         dlc,            // [IN] Data length code
                            const uint8   data[8],        // [IN] Data bytes of the message
                            int32         response) = 0;  // used for lin responses, for master requests it is ignored
                                                          // response counter 0 = stop, -1 = infinite, -2 = don't change current value
                                                          // n = number of responses

   // Send "Wakeup frames". "Wakeup frames" can only be sent while LIN hardware is in Sleep mode
   VIASTDDECL LINSendWakeup(VIAChannel    channel,    // LIN channel 
                            uint32        ttobrk ,    // time difference (bit times) between two consecutive frames
                            uint32        count) = 0;  // the number of "Wakeup frames" to be sent. 


   VIASTDDECL LINSendSleepModFrm(VIAChannel channel,          // LIN channel 
                                 uint32     silent,           // LIN Hardware doesn't send a "SleepModeFrame" before
                                 uint32     restartScheduler, // the scheduler is reset in the actual state
                                 uint32     id) = 0;          // LIN identifier to be sent, before a scheduler restarts

   // Begins cyclical traversal of a configured scheduling table
   VIASTDDECL LINStartScheduler(VIAChannel channel) = 0;      // LIN channel 

   // Stops the scheduler. Cyclical traversal of a scheduling table is stopped.
   VIASTDDECL LINStopScheduler(VIAChannel channel) = 0;       // LIN channel 

   // Registration for messages on the LIN bus
   VIASTDDECL CreateMessageRequest(VIARequestHandle* handle,   // [IN] Created request handle
                                   VIAOnLinMessage* sink,      // [IN] Callback object
                                   uint8      requestType,     // [IN] See enum VIARequestType
                                   uint32     id,              // [IN] Lin identifier
                                   VIAChannel channel) = 0;    // [IN] LIN channel or -1 (for all channels)

  // Registration for Slave Timeout error on the LIN bus
  VIASTDDECL CreateSlaveTimeoutRequest(VIARequestHandle* handle,      // [IN] Created request handle
                                       VIAOnLinSlaveTimeout*  sink,   // [IN] Callback object
                                       uint8 requestType,             // [IN] See enum VIARequestType
                                       uint32 id,                     // [IN] Lin identifier
                                       VIAChannel channel) = 0;       // [IN] LIN channel or -1 (for all channels)

  // Registration for Scheduler Mode Change event on the LIN bus
  VIASTDDECL CreateSchedulerModeChangeRequest(VIARequestHandle* handle,      // [IN] Created request handle
                                         VIAOnLinSchedulerModeChange*  sink, // [IN] Callback object
                                         VIAChannel channel)= 0;              // [IN] LIN channel or -1 (for all channels)

  // Registration for Transmission error on the LIN bus
  VIASTDDECL CreateTransmissionErrorRequest(VIARequestHandle* handle,          // [IN] Created request handle
                                            VIAOnLinTransmissionError*  sink,  // [IN] Callback object
                                            uint8 requestType,                 // [IN] See enum VIARequestType
                                            uint32 id,                         // [IN] Lin identifier
                                            VIAChannel channel) = 0;           // [IN] LIN channel or -1 (for all channels)

  // Registration for Check Sum error on the LIN bus
  VIASTDDECL CreateCheckSumErrorRequest(VIARequestHandle* handle,          // [IN] Created request handle
                                        VIAOnLinCheckSumError*  sink,      // [IN] Callback object
                                        uint8 requestType,                 // [IN] See enum VIARequestType
                                        uint32 id,                         // [IN] Lin identifier
                                        VIAChannel channel) = 0;           // [IN] LIN channel or -1 (for all channels)

  // Registration for Receive error on the LIN bus
  VIASTDDECL CreateReceiveErrorRequest(VIARequestHandle* handle,    // [IN] Created request handle
                                       VIAOnLinReceiveError* sink,  // [IN] Callback object
                                       VIAChannel channel) = 0;     // [IN] LIN channel or -1 (for all channels)

  // Registration for Sync error on the LIN bus
  VIASTDDECL CreateSyncErrorRequest(VIARequestHandle* handle,  // [IN] Created request handle
                                    VIAOnLinSyncError*  sink,  // [IN] Callback object
                                    VIAChannel channel) = 0;   // [IN] LIN channel or -1 (for all channels)

  // Registration for Baudrate event on the LIN bus
  VIASTDDECL CreateBaudrateEventRequest(VIARequestHandle* handle,      // [IN] Created request handle
                                        VIAOnLinBaudrateEvent*  sink,  // [IN] Callback object
                                        VIAChannel channel) = 0;       // [IN] LIN channel or -1 (for all channels)

  // Registration for Dlc Info event on the LIN bus
  VIASTDDECL CreateDLCInfoRequest(VIARequestHandle* handle,  // [IN] Created request handle
                                  VIAOnLinDLCInfo*  sink,    // [IN] Callback object
                                  uint8 requestType,         // [IN] See enum VIARequestType
                                  uint32 id,                 // [IN] Lin identifier
                                  VIAChannel channel) = 0;   // [IN] LIN channel or -1 (for all channels)

  // Registration for Sleep Mode event on the LIN bus
  VIASTDDECL CreateSleepModeEventRequest(VIARequestHandle* handle,      // [IN] Created request handle
                                         VIAOnLinSleepModeEvent*  sink, // [IN] Callback object
                                         VIAChannel channel) = 0;       // [IN] LIN channel or -1 (for all channels)

  // Registration for Wakeup Frame on the LIN bus
  VIASTDDECL CreateWakeupFrameRequest(VIARequestHandle* handle,    // [IN] Created request handle
                                      VIAOnLinWakeupFrame*  sink,  // [IN] Callback object
                                      VIAChannel channel) = 0;     // [IN] LIN channel or -1 (for all channels)

  // new methods in CANoe 5.0, which were not in the original VIALin 20 I/F

  VIASTDDECL ConfigureLin(VIAChannel     channel,         // LIN channel 
                            uint32       flags,           //see VIALinCfgFlgs
                            uint32       baudrate,        // Baud rate in bit/sec. Range: [1..20000]
                            uint8        syncBreak,       // Sync break length in bit times. Range: [10..30]. Set only together with syncDel
                            uint8        syncDel,         // Sync delimiter length in bit times. Range: [1..30]. Set only together with syncBreak
                            uint8        interbyteSpace) = 0; // configuration of interbyte space is not supported here
  
  VIASTDDECL LINChangeSchedtable(VIAChannel channel,         // LIN channel 
                            uint32 schedTableNo,          // -1 = stop scheduler
                            uint32 entry)= 0;

  // NOT IMPLEMENTED FUNCTION!!!
  VIASTDDECL LINModifySchedtable(VIAChannel channel,         // LIN channel 
                            uint32 schedTableNo,          // -1 = stop scheduler
                            uint32 entry,
                            uint32 flags,                 //1=disable entry, 2= enable entry, 4 = change entry to the idnew 
                            uint32 idnew )= 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)
   // Retrieves messages by ID.
   VIASTDDECL GetMessage(unsigned long ID, VIDBMessageDefinition** message) = 0;
   // You should retrieve messages using the ID. Use the name only if the ID is not available.
   VIASTDDECL GetMessage(const char* name, VIDBMessageDefinition** message) = 0;

   // Gets access to attributes of the network / bus itself:
   // Returns the attribute / the attribute type / the numeric value / the string value
   VIDB_DECLARE_ATTRIBUTE_HOLDER

   // Retrieves the definition of an arbitrary node on the bus
   VIASTDDECL GetNodeDefinition(const char* nodeName, VIDBNodeDefinition** nodeDefinition) = 0;
   // Iterates over nodes
   VIDB_DECLARE_ITERATABLE(NodeDefinition)
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)

  // Registration for messages on the LIN bus
  VIASTDDECL CreateMessageRequest2(VIARequestHandle* handle,   // [IN] Created request handle
                                   VIAOnLinMessage2* sink,     // [IN] Callback object
                                   uint8      requestType,     // [IN] See enum VIARequestType
                                   uint32     id,              // [IN] Lin identifier
                                   VIAChannel channel) = 0;    // [IN] LIN channel or -1 (for all channels)

  // Registration for Transmission error on the LIN bus in extended format
  VIASTDDECL CreateTransmissionErrorRequest2(VIARequestHandle* handle,      // [IN] Created request handle
                                         VIAOnLinTransmissionError2*  sink, // [IN] Callback object
                                         uint8 requestType,                 // [IN] See enum VIARequestType
                                         uint32 id,                         // [IN] Lin identifier
                                         VIAChannel channel) = 0;           // [IN] LIN channel or -1 (for all channels)

  // Registration for Check Sum error on the LIN bus in extended format
  VIASTDDECL CreateCheckSumErrorRequest2(VIARequestHandle* handle,          // [IN] Created request handle
                                         VIAOnLinCheckSumError2*  sink,     // [IN] Callback object
                                         uint8 requestType,                 // [IN] See enum VIARequestType
                                         uint32 id,                         // [IN] Lin identifier
                                         VIAChannel channel) = 0;           // [IN] LIN channel or -1 (for all channels)

  // Registration for Receive error on the LIN bus in extended format
  VIASTDDECL CreateReceiveErrorRequest2(VIARequestHandle* handle,     // [IN] Created request handle
                                        VIAOnLinReceiveError2*  sink, // [IN] Callback object
                                        VIAChannel channel) = 0;      // [IN] LIN channel or -1 (for all channels)

  // Registration for Sync error on the LIN bus in extended format
  VIASTDDECL CreateSyncErrorRequest2(VIARequestHandle* handle,   // [IN] Created request handle
                                     VIAOnLinSyncError2*  sink,  // [IN] Callback object
                                     VIAChannel channel) = 0;    // [IN] LIN channel or -1 (for all channels)

  // Registration for Wakeup Frame on the LIN bus in extended format
  VIASTDDECL CreateWakeupFrameRequest2(VIARequestHandle* handle,    // [IN] Created request handle
                                       VIAOnLinWakeupFrame2*  sink, // [IN] Callback object
                                       VIAChannel channel) = 0;     // [IN] LIN channel or -1 (for all channels)

  VIASTDDECL LINIsMasterNode(VIAChannel  channel,        // [IN]  LIN channel
                             const char* nodeName,       // [IN]  Node name
                             uint8&      isMaster) = 0;  // [OUT] 1- positive, 0-negative

  VIASTDDECL LINGetNodeNAD(VIAChannel  channel,        // [IN]  LIN channel
                           const char* nodeName,       // [IN]  Node name
                           int32&      nad) = 0;       // [OUT] on success-NAD, otherwise -1

  // Set a response with checksum value that is an invalid one. This is to cause CSError.
  // To reset the incorrect checksum and to return to valid checksum call OutputMessage() or use LINCalcChecksum()
  VIASTDDECL LINSetRespWithManualChecksum(VIAChannel    channel,        // [IN] LIN channel
                                          uint32        id,             // [IN] LIN identifier
                                          uint8         dlc,            // [IN] Data length code
                                          const uint8   data[8],        // [IN] Data bytes of the message 
                                          int32         checksum) = 0;  // [IN] Checksum to be set

  VIASTDDECL LINInitSetMaster(VIAChannel channel,            // [IN] LIN channel
                              int32    masterMode,           // [IN] != 0 -> Switch the LIN hardware to the Master mode
                              int32    masterResistor) = 0;  // [IN] != 0 -> Switch the terminating resistor for Master on the internal LIN hardware transceiver

  VIASTDDECL LINSetRespCounter(VIAChannel channel,         // [IN] LIN channel
                               uint32        id,           // [IN] LIN identifier
                               uint8         counter) = 0; // [IN] Number of responses to be sent. [-1] - unlimited, [0..254] - n times

  VIASTDDECL LINSetRespLength(VIAChannel  channel,          // [IN] LIN channel 
                              uint32       id,              // [IN] LIN identifier
                              int32        numOfBytes) = 0; // [IN] Number of bytes to be sent (including checksum)

  // Activates the frame responses for all frames published by the specified node. 
  VIASTDDECL LINActivateResps(VIAChannel    channel,      // [IN] LIN channel 
                              const char* nodeName) = 0;  // [IN] node name, whose responses have to be cancelled

  // Deactivates the frame responses for all frames published by the specified node. 
  VIASTDDECL LINDeactivateResps(VIAChannel  channel,        // [IN] LIN channel 
                                const char* nodeName) = 0;  // [IN] node name, whose responses have to be cancelled

  // Following functions do not overload LINSendWakeup for backward compatibility reasons
  VIASTDDECL LINSendWakeupEx(VIAChannel channel) = 0;        // [IN] LIN channel 

  VIASTDDECL LINSendWakeupEx(VIAChannel channel,      // [IN] LIN channel 
                             uint32     ttobrk ,      // [IN] Time difference (bit times) between two consecutive frames
                             uint32     count,        // [IN] The number of "Wakeup frames" to be sent.
                             uint32     length) = 0;  // [IN] Length of the wakeup frame to be sent in µs. The resolution is 50 µs

  VIASTDDECL LINSetWakeupParams(VIAChannel channel,                // [IN] LIN channel 
                                uint32     wakeupDelimiter,        // [IN] length of the "WakeupDelimiter" in bit times [3..255]                        
                                uint32     numOfWakeupFrames) = 0; // [IN] number of Wakeup frames, after that the LIN hardware is reactivated

  VIASTDDECL LINChangeWakeupSettings(VIAChannel  channel,               // [IN] LIN channel 
                                     uint8       restartScheduler,      // [IN] Determines whether the scheduler should be restarted
                                     uint32      wakeupIdentifier) = 0; // [IN] The frame identifier to be sent additionally directly after sending a wakeup signal. 0xFF - first header sent will be determined by the scheduler

  // Following functions do not overload LINChangeSchedtable for backward compatibility reasons
  VIASTDDECL LINChangeSchedtableEx(VIAChannel channel,         // [IN] LIN channel 
                                   uint32     tableIndex) = 0; // [IN] Index of the schedule table to be changed to

  VIASTDDECL LINChangeSchedtableEx(VIAChannel channel,          // [IN] LIN channel 
                                   uint32     tableIndex,       // [IN] Index of the schedule table to be changed to
                                   uint32     slotIndex,        // [IN] Index of slot to be started with in the new table. Default= 0
                                   uint32     onSlotIndex )= 0; // [IN] Slot to be sent before changing. -1 = table change on reaching end of current table

  VIASTDDECL LINSetDlc(VIAChannel channel,     // [IN] LIN channel 
                       uint32     id,          // [IN] LIN identifier
                       uint8      dlc) = 0;    // [IN] dlc

  VIASTDDECL LINGetDlc(VIAChannel channel,     // [IN] LIN channel 
                       uint32     id,          // [IN] LIN identifier
                       uint8&     dlc) = 0;    // [OUT] on success-DLC, otherwise -1

  VIASTDDECL LINChangeDlc(VIAChannel channel,            // [IN] LIN channel 
                          uint32     id,                 // [IN] LIN identifier
                          uint8      dlc) = 0;           // [IN] dlc

   VIASTDDECL LINSetBreakLength(VIAChannel channel,          // [IN] LIN channel
                                uint8      syncBreak,        // [IN] Sync break length in bit times. Range: [10..30]. Default: 18
                                uint8      syncDel) = 0;     // [IN] Sync delimiter length in bit times. Range: [1..30]. Default: 2 

  VIASTDDECL LINSetInterframeSpace(VIAChannel channel,       // [IN] LIN channel 
                                   uint32     bitTimes) = 0; // [IN] Length of the interframe space in bit times [0..255]

  VIASTDDECL LINSetGlobalInterByteSpace(VIAChannel channel,            // [IN] LIN channel 
                                        uint32     sixteenthBits) = 0; // [IN] Interbyte space with a resolution of 1/16th of bit [0..255]

  VIASTDDECL LINSetInterByteSpaces(VIAChannel    channel,                 // [IN] LIN channel 
                                   uint32        id,                      // [IN] LIN identifier
                                   uint32 const* arraySixteenthBits) = 0; // [IN] An array (of size 9!) of interbyte-spaces for all data bytes
             
  VIASTDDECL LINSetInterByteSpace(VIAChannel  channel,            // [IN] LIN channel 
                                  uint32      id,                 // [IN] LIN identifier
                                  int32       index,              // Index of the data byte, in front of which the interbyte space should be inserted
                                  uint32      sixteenthBits) = 0; // [IN] Interbyte space with a resolution of 1/16th of bit

  VIASTDDECL LINSetHeaderError(VIAChannel channel,             // [IN] LIN channel 
                               uint8      syncByte,            // [IN] sync-byte to be sent. Correct value is 0x55
                               uint8      idWithParity,        // [IN] ID-byte to be sent. The lower 6 bits specify the frame to be used. The upper 2 bits specify the parity to be used
                               uint8      stopAfterError) = 0; // [IN] Specifies whether the transmission should be stopped after an error has occurred

  VIASTDDECL LINInvertRespBit(VIAChannel  channel,          // [IN] LIN channel 
                              uint32      id,               // [IN] LIN identifier
                              uint32      byteIndex,        // [IN] index of the byte to be manipulated; 0-first byte, 8-checksum byte
                              uint32      bitIndex) = 0;    // [IN] index of the bit to be manipulated; [0..7]-data bit, 8-stop bit, [9..255]-interbyte space


  VIASTDDECL LINSendAsSporadic(VIAChannel  channel,         // [IN] LIN channel 
                               uint32      id) = 0;         // [IN] LIN identifier

  // Resets the NAD of the specified Slave node
  // If the node is not Slave node, then this function will have no effect.
  VIASTDDECL LINResetNAD(VIAChannel  channel,               // [IN] LIN channel 
                         const char* nodeName) = 0;         // [IN] Slave node name

  // Resets the NAD of the specified Slave node and marks all its protected identifiers as invalid.
  // If the node is not Slave node, then this function will have no effect.
  VIASTDDECL LINResetSlave(VIAChannel  channel,             // [IN] LIN channel 
                           const char* nodeName) = 0;       // [IN] Slave node name

  VIASTDDECL LINETFSetDirtyFlag(VIAChannel  channel,        // [IN] LIN channel 
                                uint32      assocId,        // [IN] frameID of an unconditional frame
                                uint8       dirty) = 0;     // [IN] Dirty flag; 1-mark as dirty, 0-clean the dirty flag

  VIASTDDECL LINETFSendOnSignalUpdate(VIAChannel  channel,     // [IN] LIN channel 
                                      uint32      etfId,       // [IN] frameID of event triggered frame to activate
                                      uint8       active) = 0; // [IN] 1-activate, 0-deactivate

  VIASTDDECL LINDetectMultipleErrors(VIAChannel  channel,      // [IN] LIN channel 
                                     uint8       active) = 0;  // [IN] 1-activate, 0-deactivate

  VIASTDDECL LINActivateCollisionResolution(VIAChannel  channel,      // [IN] LIN channel 
                                            uint32      etfId,        // [IN] frameID of event triggered frame to activate
                                            uint8       active) = 0;  // [IN] 1-activate, 0-deactivate
  
  VIASTDDECL LINSetBaudrate(VIAChannel channel,        // [IN] LIN channel
                            uint32     baudrate) = 0;  // [IN] Baud rate in bit/sec. Range: [1..20000]

  VIASTDDECL LINMeasHeaderBaudrate(VIAChannel channel,     // [IN] LIN channel 
                                   int16      index) = 0;  // [IN] 0 – Measure baudrate using the sync field, 1 – using the identifier

  VIASTDDECL LINMeasRespBaudrate(VIAChannel channel,         // [IN] LIN channel 
                                 uint32     id,              // [IN] LIN Identifier of the frame to be measured
                                 int16      byteIndex) = 0;  // [IN] data byte index to be measured; value range [0..DLC]

  VIASTDDECL LINGetMeasBaudrate(VIAChannel channel,        // [IN] LIN channel 
                                int32&     baudrate) = 0;  // [OUT] measured baudrate in bit/sec, -1 on failure

  VIASTDDECL LINTime2Bits(VIAChannel channel,         // [IN] LIN channel 
                          VIATime    time,            // [IN] time to be converted in nanoseconds
                          double&    bitTimes) = 0;   // [OUT] result of the convertion in bit times

  VIASTDDECL LINBits2Time(VIAChannel channel,         // [IN] LIN channel 
                          double     bitTimes,        // [IN] bit times time to be converted
                          VIATime&   time) = 0;       // [OUT] result of the convertion in nanoseconds

  VIASTDDECL LINGetProtectedID(uint32  id,               // [IN] Valid LIN frame identifier without parity bits
                               uint32& protectedId) = 0; // [OUT] Resulting LIN frame identifier with parity bits
  
  // Calculates checksum. Model (classic/enhanced) is determined by call context that's why channel is required
  VIASTDDECL LINCalcChecksum(VIAChannel  channel,         // [IN] LIN channel 
                             uint32      id,              // [IN] LIN Identifier of the frame to be used
                             uint32      numOfBytes,      // [IN] Number of valid bytes to be taken into consideration 
                             const uint8 data[8],         // [IN] Data bytes from which the checksum will be determined
                             int32&      checksum) = 0;   // [OUT] Calculated checksum

  VIASTDDECL LINActivateSlot(VIAChannel channel,         // [IN] LIN channel 
                             uint32     tableIndex,      // [IN] Index of the schedule table
                             uint32     slotIndex)= 0;   // [IN] Index of slot to be activated

  VIASTDDECL LINDeactivateSlot(VIAChannel channel,       // [IN] LIN channel 
                               uint32     tableIndex,    // [IN] Index of the schedule table
                               uint32     slotIndex)= 0; // [IN] Index of slot to be deactivated

  VIASTDDECL LINSetMasterRequestDirtyFlag(VIAChannel channel,
                                          uint8      dirty) = 0;
#endif

#if defined (VIAMinorVersion) && (VIAMinorVersion >= 22)
   // Registration for messages on the LIN bus in extended format
  // Registration for messages on the LIN bus
   VIASTDDECL CreateMessageRequest3(VIARequestHandle* handle,   // [IN] Created request handle
                                    VIAOnLinMessage3* sink,     // [IN] Callback object
                                    uint8      requestType,     // [IN] See enum VIARequestType
                                    uint32     id,              // [IN] Lin identifier
                                    VIAChannel channel) = 0;    // [IN] LIN channel or -1 (for all channels)

  // Registration for Transmission error on the LIN bus in extended format
  VIASTDDECL CreateTransmissionErrorRequest3(VIARequestHandle* handle,          // [IN] Created request handle
                                             VIAOnLinTransmissionError3*  sink, // [IN] Callback object
                                             uint8 requestType,                 // [IN] See enum VIARequestType
                                             uint32 id,                         // [IN] Lin identifier
                                             VIAChannel channel) = 0;           // [IN] LIN channel or -1 (for all channels)

  // Registration for Receive error on the LIN bus in extended format
  VIASTDDECL CreateReceiveErrorRequest3(VIARequestHandle* handle,     // [IN] Created request handle
                                        VIAOnLinReceiveError3*  sink, // [IN] Callback object
                                        VIAChannel channel) = 0;      // [IN] LIN channel or -1 (for all channels)

#endif

#if defined (VIALINMinorVersion) && (VIALINMinorVersion >= 4)
  VIASTDDECL LINGetGlobalProtVersion(VIAChannel  channel,           // [IN] LIN channel
                                     double     &version) = 0;      // [OUT] version

  VIASTDDECL LINGetSlaveProtVersion(VIAChannel  channel,           // [IN] LIN channel
                                    char const *slaveName,         // [IN] node name
                                    double     &version) = 0;      // [OUT] version
#endif

#if defined (VIALINMinorVersion) && (VIALINMinorVersion >= 5)
  // Registration for messages on the LIN bus
  VIASTDDECL CreateMessageRequest4(VIARequestHandle* handle,   // [IN] Created request handle
    VIAOnLinMessage4* sink,     // [IN] Callback object
    uint8      requestType,     // [IN] See enum VIARequestType
    uint32     id,              // [IN] Lin identifier
    VIAChannel channel) = 0;    // [IN] LIN channel or -1 (for all channels) (see enum VIAChannel)

  // Registration for Transmission error on the LIN bus in extended format
  VIASTDDECL CreateTransmissionErrorRequest4(VIARequestHandle* handle,          // [IN] Created request handle
    VIAOnLinTransmissionError4*  sink, // [IN] Callback object
    uint8 requestType,                 // [IN] See enum VIARequestType
    uint32 id,                         // [IN] Lin identifier
    VIAChannel channel) = 0;           // [IN] LIN channel or -1 (for all channels) (see enum VIAChannel)

  // Registration for Receive error on the LIN bus in extended format
  VIASTDDECL CreateReceiveErrorRequest4(VIARequestHandle* handle,     // [IN] Created request handle
    VIAOnLinReceiveError4*  sink, // [IN] Callback object
    VIAChannel channel) = 0;      // [IN] LIN channel or -1 (for all channels) (see enum VIAChannel)

  VIASTDDECL CreateCheckSumErrorRequest3(VIARequestHandle* handle,     // [IN] Created request handle
    VIAOnLinCheckSumError3*  sink, // [IN] Callback object
    uint8 requestType,             // [IN] See enum VIARequestType
    uint32 id,                     // [IN] Lin identifier
    VIAChannel channel) = 0;       // [IN] LIN channel or -1 (for all channels) (see enum VIAChannel)

#endif

#if defined ( VIALINMinorVersion) && ( VIALINMinorVersion >= 6)
  VIASTDDECL OpenNetworkPanel(uint32 open) = 0;
#endif

#if defined ( VIALINMinorVersion) && ( VIALINMinorVersion >= 7)

  // set hardware channel into sleep mode, without sending a goto-sleep master request
  VIASTDDECL LINGotoSleepInternal(VIAChannel channel) = 0;          // [IN] LIN channel 

  // set hardware channel into awake mode, without sending wakeup signals
  VIASTDDECL LINWakeupInternal(VIAChannel channel) = 0;             // [IN] LIN channel 

  // send a goto-sleep master request and set hardware channel into sleep mode
  VIASTDDECL LINGotoSleep(VIAChannel channel) = 0;                  // [IN] LIN channel 

  // send wakeup master signal(s) and set hardware channel into awake mode
  // node-specific timing parameters will be used if available, default parameters based on protocol version & type otherwise
  VIASTDDECL LINWakeup(VIAChannel channel) = 0;                     // [IN] LIN channel 

  // send wakeup master signal(s) and set hardware channel into awake mode
  // specified timing parameters will be used (overriding any parameters set by LINSetWakeupTimings)
  VIASTDDECL LINWakeupEx(VIAChannel channel,                        // [IN] LIN channel 
    uint32        ttobrk,                                           // [IN] time difference (bit times) between two consecutive frames
    uint32        count,                                            // [IN] the number of "Wakeup frames" to be sent. 
    uint32        widthinMicSec) = 0;                               // [IN] the length of each wakeup signal, in usec

  // set timing parameters, on the current node, for consecutive LINWakeup invocation.
  // will return 1 (success) if the current node is an assigned LIN node, 0 otherwise
  VIASTDDECL LINSetWakeupTimings(VIAChannel channel,                // [IN] LIN channel 
    uint32        ttobrk,                                           // [IN] time difference (bit times) between two consecutive frames
    uint32        count,                                            // [IN] the number of "Wakeup frames" to be sent. 
    uint32        widthinMicSec) = 0;                               // [IN] the length of each wakeup signal, in uSec

  // set timing parameters, on node specified by nodeName, for consecutive LINWakeup invocation
  // will return 1 (success) if the specified node is an assigned LIN node, 0 otherwise
  VIASTDDECL LINSetWakeupTimingsEx(VIAChannel channel,              // [IN] LIN channel 
    uint32        ttobrk,                                           // time difference (bit times) between two consecutive frames
    uint32        count,                                            // the number of "Wakeup frames" to be sent. 
    uint32        widthinMicSec,                                    // the length of each wakeup signal, in usec
    const char*   nodeName) = 0;                                    // the name of the node on which to set the timing parameters

  // set wakeup signal parameters, defining the condition when to actually set the hardware channel into awake mode
  VIASTDDECL LINSetWakeupCondition(VIAChannel channel,              // [IN] LIN channel
    uint32 numWakeupSignals,                                        // [IN] number of wakeup signals received, after which to put the HW channel into awake mode
    uint32 forgetWupsAfterMS) = 0;                                  // [IN] time span after which the count of received wakeup signals will be reset (while not causing a HW channel wakeup)

  // specify the action to take right after the hardware channel has been set into awake mode
  // NOTE: a short header (SynchBreak / SynchField) pair will be sent after the HW channel entered awake state, if
  //      wakeupIdentifier is an invalid LIN id
  //      AND sendShortHeader == 1 
  //      AND no master node defining schedule tables has been assigned to it's corresponding LIN DB node
  VIASTDDECL LINSetWakeupBehavior(VIAChannel channel,               // [IN] LIN channel
    uint32 restartScheduler,                                        // [IN] restart scheduler after HW channel entered awake state
    uint32 wakeupIdentifier,                                        // [IN] LIN frame identifier to be sent additionally directly after sending a wake-up signal.
    uint32 sendShortHeader,                                         // [IN] activates/deactivates 'short header' automatism (use 1 for default behavior as in CANoe)
    uint32 wakeupDelimiterLength) = 0;                              // [IN] time, after wakeup, when to send the first header, if restartScheduler == 1 (use 0 for default value)
#endif

#if defined ( VIALINMinorVersion) && ( VIALINMinorVersion >= 8)
  VIASTDDECL CreateSlaveResponseChangedRequest(VIARequestHandle* handle,   // [IN] Created request handle
                                               VIAOnLinSlaveResponseChanged* sink,      // [IN] Callback object
                                               uint8      requestType,     // [IN] See enum VIARequestType
                                               uint32     id,              // [IN] Lin identifier
                                               VIAChannel channel) = 0;    // [IN] LIN channel or -1 (for all channels)

  VIASTDDECL CreatePreDispatchMessageRequest(VIARequestHandle* handle,   // [IN] Created request handle
                                             VIAOnLinPreDispatchMessage* sink,      // [IN] Callback object
                                             uint8      requestType,     // [IN] See enum VIARequestType
                                             uint32     id,              // [IN] Lin identifier
                                             VIAChannel channel) = 0;    // [IN] LIN channel or -1 (for all channels)
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 9)
  // Registration for extended Scheduler Mode Change event on the LIN bus
  VIASTDDECL CreateSchedulerModeChangeRequest2(VIARequestHandle* handle,      // [IN] Created request handle
    VIAOnLinSchedulerModeChange2*  sink, // [IN] Callback object
    VIAChannel channel) = 0;              // [IN] LIN channel or -1 (for all channels)
#endif
};

enum VIALinCfgFlgs
{
  kVIALinCfgFlgs_baudrate    = 1<<0,
  kVIALinCfgFlgs_syncBreak   = 1<<1,
  kVIALinCfgFlgs_syncDel     = 1<<2,
  kVIALinCfgFlgs_IBS         = 1<<3,
};


#endif // VIA_LIN_H
