/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: -
-------------------------------------------------------------------------------
Fast Data eXchange (FDX)
Protocol definition for connecting a HIL system over UPD to CANoe
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/

#ifndef CANoeFDX_H
#define CANoeFDX_H

namespace CANoeFDX
{
  // ==========================================================================
  // Common type definitions
  // ==========================================================================

  typedef  signed   char        int8;
  typedef  unsigned char        uint8;
  typedef  signed   short       int16;
  typedef  unsigned short       uint16;
  typedef  signed   long        int32;
  typedef  unsigned long        uint32;
  typedef  signed   long long   int64;
  typedef  unsigned long long   uint64;

  // ==========================================================================
  // Version of CANoe FDX Protocol
  // ==========================================================================

  const uint16 kFdxMajorVersion = 1;
  const uint16 kFdxMinorVersion = 1;

  // ==========================================================================
  // DatagramHeader
  // Each datagram transmitted by the UPD protocol start with a header. The
  // datagram header contains a signature (magic cookie) and the version of 
  // the CANoe FDX protocol.
  // The header is followed by one ore more command structures. The field 
  // numberOfCommands specifies, how many command structures are following.
  // The datagram header is equipped with a sequence number, that enable CANoe
  // and his counterpart to detect lost datagrams.
  // ==========================================================================
  
  const uint64 kFdxSignature = 0x584446656F4E4143;

  const uint16 kSequenceNumberUnused         = 0x8000;
  const uint16 kSequenceNumberSessionStart   = 0x0000;
  const uint16 kSequenceNumberSessionEndFlag = 0x8000;

  struct DatagramHeader
  {
    uint64 fdxSignature;     // FDX signature, that is used as a magic cookie.
                             // This field must contain the value kFDXSignature 
                             // otherwise the datagram is ignored.
    uint8  fdxMajorVersion;  // major version of CANoe FDX protocol
    uint8  fdxMinorVersion;  // minor version of CANoe FDX protocol
    uint16 numberOfCommands; // number of commands, that are contained in the datagram
    uint16 sequenceNumber;   // datagram sequence number of FDX session
    uint16 reserved;         // 2 unused bytes for a better alignment, may be used in
                             // a future version of the protocol.
                             // This field should be initialized to zero.
  };

  // ==========================================================================
  // CommandHeader
  // Each command structure starts with a command header. The header contains 
  // the size of the complete command structure and the command code. 
  // ==========================================================================

  // The command code for the field  CommandHeader::commandCode
  const uint16 kCommandCode_Start = 0x0001;
  const uint16 kCommandCode_Stop = 0x0002;
  const uint16 kCommandCode_Key = 0x0003;
  const uint16 kCommandCode_Status = 0x004;
  const uint16 kCommandCode_DataExchange = 0x0005;
  const uint16 kCommandCode_DataRequest = 0x0006;
  const uint16 kCommandCode_DataError = 0x0007;
  const uint16 kCommandCode_FreeRunningRequest = 0x0008;
  const uint16 kCommandCode_FreeRunningCancel = 0x0009;
  const uint16 kCommandCode_StatusRequest = 0x000A;
  const uint16 kCommandCode_SequenceNumberError = 0x000B;
  const uint16 kCommandCode_HardwareChanged = 0x0010;


  struct CommandHeader
  {
    uint16 commandSize;  // size of this command in bytes
    uint16 commandCode;  // the kind of command
  };

  // ==========================================================================
  // KeyCommand
  // command code: kCommandCode_Key
  // Transmit a key stroke to the runtime kernel. This can invoke a 'on key' 
  // handler in CAPL.
  // ==========================================================================

  struct KeyCommand : CommandHeader
  {
    uint32 canoeKeyCode;       // key code as used CAPL programs of CANoe
                               // use code 0 for KEYUP events
  };

  // ==========================================================================
  // StatusCommand
  // command code: kCommandCode_Status
  // ==========================================================================

  const uint8 kMeasurementState_NotRunning = 1;
  const uint8 kMeasurementState_PreStart = 2;
  const uint8 kMeasurementState_Running = 3;
  const uint8 kMeasurementState_Stop = 4;


  struct StatusCommand : CommandHeader
  {
    uint8 measurementState;    // state of measurement (NotRunning, PreStart,
                               // Running, Stop)
    uint8 reserved1;
    uint8 reserved2;
    uint8 reserved3;
    int64 timestamp;           // current time of measurement in nanoseconds
  };

  // ==========================================================================
  // DataExchangeCommand
  // command code: kCommandCode_DataExchange
  // ==========================================================================

  // size of a DataExchangeCommand without data bytes
  const uint16 kDataExchangeBaseSize = sizeof(CommandHeader) + 4;

  struct DataExchangeCommand : CommandHeader
  {
    uint16 groupID;      // ID of the data group
    uint16 dataSize;     // size of the following array
    uint8  dataBytes[1]; // the data bytes
  };

  // ==========================================================================
  // DataRequestCommand
  // command code: kCommandCode_DataRequest
  // ==========================================================================

  struct DataRequestCommand : CommandHeader
  {
    uint16 groupID;   // ID of the requested data group
  };

  // ==========================================================================
  // DataErrorCommand
  // command code: kCommandCode_DataError
  // ==========================================================================

  const uint16 kDataErrorCode_NoError = 0;
  const uint16 kDataErrorCode_MeasurementNotRunning = 1;
  const uint16 kDataErrorCode_GroupIdInvalid = 2;
  const uint16 kDataErrorCode_DataSizeTooLarge = 3;

  struct DataErrorCommand : CommandHeader
  {
    uint16 groupID;        // ID of the data group, that was requested
    uint16 dataErrorCode;  // The reason, why CANoe cannot process the 
                           // data request command
  };

  // ==========================================================================
  // FreeRunningRequestCommand
  // command code: kCommandCode_FreeRunningRequest
  // ==========================================================================

  const uint16 kFreeRunningFlag_TransmitAtPreStart = 1;
  const uint16 kFreeRunningFlag_TransmitAtStop     = 2;
  const uint16 kFreeRunningFlag_TransmitCyclic     = 4;
  const uint16 kFreeRunningFlag_TransmitAtTrigger  = 8;

  struct FreeRunningRequestCommand : CommandHeader
  {
    uint16 groupID;         //  ID of the data group, that is requested
    uint16 flags;           //
    uint32 cycleTime;       //
    uint32 firstDuration;   //
  };


  // ==========================================================================
  // FreeRunningCancelCommand
  // command code: kCommandCode_FreeRunningCancel
  // ==========================================================================

  struct FreeRunningCancelCommand : CommandHeader
  {
    uint16 groupID;          // ID of the data group, for which the free 
                             // running mode is canceled
  };

  // ==========================================================================
  // SequenceNumberErrorCommand
  // command code: kCommandCode_SequenceNumberError
  // ==========================================================================

  struct SequenceNumberErrorCommand : CommandHeader
  {
    uint16 receivedSeqNr;
    uint16 expectedSeqNr;
  };

  // ==========================================================================
  // HardwareChangedCommand
  // command code: kCommandCode_HardwareChanged
  // ==========================================================================

  struct HardwareChangedCommand : CommandHeader
  {
    uint32 reserved;
  };


}

#endif
