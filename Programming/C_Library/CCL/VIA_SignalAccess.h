/*-----------------------------------------------------------------------------
Module: VIA (observed)
Interfaces: This is a public interface header.
-------------------------------------------------------------------------------
External interface for access to signals
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/

#ifndef VIA_SIGNALACCESS_H
#define VIA_SIGNALACCESS_H

/*-----------------------------------------------------------------------------
Version of the interfaces, which are declared in this header

2004-05-11   1.0  Aa      Creation
2005-04-11   1.1  Aa      Version increment to differ the behavior within 
                          'GetSignalAccessApi' function of the VIA service
2005-09-05   2.0  As      Refactoring for use of SignalServer (CANoe 5.2)
2006-07-31   2.1  As      SignalGroup added (CANoe 6.0)
2006-08-15   2.2  As      Callbacks for SignalGroup added (CANoe 6.0)
                          OnChange Callbacks for Signals added
2007-08-01   2.3  As      MemorySegment Object added (CANoe 6.1 SP3)
2007-08-01   2.4  As      SignalTransmitter added (CANoe 6.1 SP3)
2007-09-28   2.5  As      Pull-Method for Signal Generators added(CANoe 7.0)
2008-01-18   2.6  Msc     Added new method for Signal creation (CANoe 7.0 SP3)
2009-07-09   2.7  As      Access to the raw value of a signal added (CANoe 7.2)
2010-08-18   2.8  As      VIASignalTransmitter: Flags for registering signals 
                          added (CANoe 7.2 SP6). This allows some performance 
                          optimizations in CANoe.
2012-01-09   2.9  Bma     Added GetValue/SetValue + raw value flag; signals 
                          decide by themselves whether the raw or the physical 
                          value is returned (CANoe 8.0)
2013-01-10   2.10 Wwi     Added String-type signals (CANoe 8.1)
2013-06-06   2.11 Jsd     Added VIAOnTxSignal sink
2014-08-15   2.12 Cbo     Added SetTransmitterMode() (CANoe 8.2 SP3, VIA 1.63)
2015-10-21   2.13 Pir     Added GetSiganlByTxNodeAndPath (CANoe 9.0.8, VIA 1.75)
2016-02-16   2.14 Cbo     Added callback object VIAOnTxSignal2
-----------------------------------------------------------------------------*/

#define VIASignalAccessMajorVersion 2
#define VIASignalAccessMinorVersion 14

// ============================================================================
// Dependencies
// ============================================================================

#ifndef VIA_H
  #include "VIA.h"
#endif


// forward declarations
class VIASignal;
class VIASignalGroup;
class VIASignalTransmitter;
class VIAMemorySegment;
class VIDBSignalDefinition;

// ============================================================================
// VIAOnSignal
// Callback object for updates of a signal
// Supported by CAPL-on-Board: No
// ============================================================================
class VIAOnSignal
{
public:
  VIASTDDECL OnSignal(VIASignal* signal, void* userData) = 0;
};

// ============================================================================
// VIAOnTxSignal
// Callback object for updates of a signal (tx)
// ============================================================================
class VIAOnTxSignal
{
public:
  VIASTDDECL OnTxSignal(VIASignal* signal, void* userData) = 0;
};

#if defined (VIASignalAccessMinorVersion) && (VIASignalAccessMinorVersion >= 14)

// ============================================================================
// VIAOnTxSignal2
// ============================================================================
class VIAOnTxSignal2
{
public:
  // Callback for updates of a signal (tx)
  VIASTDDECL OnTxSignal2(VIASignal* signal, void* userData, uint32 flags) = 0;
  // Notification, that a signal generator was attached to the given signal
  VIASTDDECL OnGeneratorAttached(VIASignal* signal, void* userData) = 0;
  // Notification, that a signal generator was detached from the given signal
  VIASTDDECL OnGeneratorDetached(VIASignal* signal, void* userData) = 0;
};

#endif

// ============================================================================
// VIASignal
// 
// Interface for accessing signals. Must be accessed during init measurement
// phase of the VIANodeLayerApi object.
//
// It is not possible to configure an update notification (method SetSink) and 
// a change notification (method SetSinkOnChangeOnly) at the same time within
// one VIASignal object.
//
// Supported by CAPL-on-Board: Yes
// ============================================================================

// Specifies the types of the raw value of a signal
enum VIASignalRawValueType
{
    kVIA_SigInteger = 1,   // integer
    kVIA_SigDouble  = 2,   // double
    kVIA_SigByteArray = 3, // byte array
    kVIA_SigString = 4,    // string
}; 

class VIASignal
{
public:
  // Retrieves the physical value of signal as double
  // Return codes: kVIA_OK (always)
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetPhysicalValue(double* value) = 0;

  // Sets the physical value as double.
  // Return codes: kVIA_OK: Signal was successfully set
  //    kVIA_ObjectInvalid: Signal doesn't support a physical value
  // Supported by CAPL-on-Board: No
  VIASTDDECL SetPhysicalValue(double  value) = 0;

  // Retrieves the time stamp of the last message, that was used to transmit
  // the signal over a bus. If the signal was not transmitted over a bus since 
  // start of measurement, the time stamp is zero.
  // Return codes: kVIA_OK (always)
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetSignalTime(VIATime* time) = 0;

  // Checks if signal is in range of aLowLimit and aHighLimit
  // Return codes: kVIA_OK (always)
  // [OUT] aRet: 0: value is not in range
  //             1: value is in range
  // Supported by CAPL-on-Board: No
  VIASTDDECL CheckSignalInRange(double aLowLimit, double aHighLimit, long* aRet) = 0;

  // Sets the callback object. The notification is called with every update 
  // (receive) of the signal.
  // Supported by CAPL-on-Board: No
  VIASTDDECL SetSink(VIAOnSignal* sink, void* userData) = 0;

#if (VIASignalAccessMinorVersion >= 2)
  // Sets the callback object. The notification is called only when the value
  // of the signal has changed.
  // Supported by CAPL-on-Board: No
  VIASTDDECL SetSinkOnChangeOnly(VIAOnSignal* sink, void* userData) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 4)
  // Gets the value of the TxBuffer as physical value. 
  // Return codes: kVIA_OK (always)
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetTxPhysicalValue(double* value) = 0;

  // Checks, if a client has already set a tx value for this signal.
  // Return codes: kVIA_ParameterInvalid if parameter isDefault is NULL
  //               otherwise kVIA_OK
  // [OUT] isDefault (if function returns kVIA_OK):
  // false: TxBuffer was set apart from initialization.
  // true:  TxBuffer was never set apart from initialization.
  // Supported by CAPL-on-Board: No
  VIASTDDECL TxValueIsDefault(int32* isDefault) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 5)
  // If a generator is available for the signal, then the Method PullTxValue 
  // triggers the calculation of a new signal value. When the Method 
  // PullTxValueWithTime is used, the given timestamp is passed to the 
  // generator as calculation base for the signal value.
  // Return codes:
  //   kVIA_GeneratorNotAvailable: There is no generator available for this signal
  //   kVIA_OK:     A generator is available for this signal and could calculate 
  //                a new signal value
  //   kVIA_Failed: unexpected error
  // Supported by CAPL-on-Board: No
  VIASTDDECL PullTxValueWithTime(VIATime time) = 0;
  VIASTDDECL PullTxValue() = 0;

  // Checks, if a signal generator is attached to the signal.
  // Return codes: kVIA_ParameterInvalid if parameter hasGenerator is NULL
  //               otherwise kVIA_OK
  // [OUT] hasGenerator (if function returns kVIA_OK):
  // 0: No generator is currently registered
  // 1: A generator is currently registered
  // Supported by CAPL-on-Board: No
  VIASTDDECL HasGenerator(int32* hasGenerator) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 7)
  // Gets the type of the raw value of the signal. (see enum VIASignalRawValueType)
  // Return codes: kVIA_ParameterInvalid if parameter type is NULL
  //               otherwise kVIA_OK or kVIAFailed for unexpected errors
  // [OUT] type (if function returns kVIA_OK):
  // kVIA_SigInteger:   For long or longlong
  // kVIA_SigDouble:    For double
  // kVIA_SigByteArray: For byte arrays
  // Supported by CAPL-on-Board: Yes
  VIASTDDECL RawValueType(int32* type) = 0;

  // Retrieves the raw value of the signal as integer
  // Return codes: kVIA_ParameterInvalid: parameter value is NULL
  //               kVIA_WrongSignalType:  value type not kVIA_SigInteger
  //               otherwise kVIA_OK
  // Supported by CAPL-on-Board: Yes
  VIASTDDECL GetRawValueInt64(::int64* value) = 0;
  // Retrieves the raw value of the signal as double
  // Return codes: kVIA_ParameterInvalid: parameter value is NULL
  //               kVIA_WrongSignalType:  value type not kVIA_SigDouble
  //               otherwise kVIA_OK
  // Supported by CAPL-on-Board: Yes
  VIASTDDECL GetRawValueDouble(double* value) = 0;
  // Retrieves the raw value of the signal as byte array
  // Return codes: kVIA_ParameterInvalid: One of the parameters is invalid
  //               kVIA_WrongSignalType:  Value type not kVIA_SigByteArray
  //               kVIA_BufferToSmall:    Buffer is to small
  //               otherwise kVIA_OK
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetRawValueByteArray(uint8* byteArray, int32 arrayLength, int32* signalLength) = 0;
  
  // Sets the raw value of the signal as integer
  // Return codes: kVIA_WrongSignalType: Value type not kVIA_SigInteger
  //               kVIA_Failed:          Value is not set, unexpected errors
  //               kVIA_OK:              Value is set
  // Supported by CAPL-on-Board: No
  VIASTDDECL SetRawValueInt64(::int64 value) = 0;
  // Sets the raw value of the signal as double
  // Return codes: kVIA_WrongSignalType: Value type not kVIA_SigDouble
  //               kVIA_Failed:          Value is not set, unexpected errors
  //               kVIA_OK:              Value is set
  // Supported by CAPL-on-Board: No
  VIASTDDECL SetRawValueDouble(double value) = 0;
  // Sets the raw value of the signal as byte array
  // Return codes: kVIA_WrongSignalType: Value type not kVIA_SigByteArray
  //               kVIA_ParameterInvalid: One of the parameters is invalid
  //               kVIA_Failed:          Value is not set, unexpected errors
  //               kVIA_OK:              Value is set
  // Supported by CAPL-on-Board: No
  VIASTDDECL SetRawValueByteArray(const uint8* byteArray, int32 arrayLength) = 0;

  // Retrieves the content of tx buffer as raw value.
  // Return codes: kVIA_ParameterInvalid: parameter is NULL
  //               kVIA_WrongSignalType:  value type not kVIA_SigInteger
  //               otherwise kVIA_OK
  // Supported by CAPL-on-Board: Yes
  VIASTDDECL GetTxRawValueInt64(::int64* value) = 0;
  // Retrieves the content of tx buffer as raw value.
  // Return codes: kVIA_ParameterInvalid: parameter is NULL
  //               kVIA_WrongSignalType:  value type not kVIA_SigDouble
  //               otherwise kVIA_OK
  // Supported by CAPL-on-Board: Yes
  VIASTDDECL GetTxRawValueDouble(double* value) = 0;
  // Retrieves the content of tx buffer as raw value.
  // Return codes: kVIA_ParameterInvalid: one of the parameter is NULL
  //               kVIA_WrongSignalType:  value type not kVIA_SigByteArray
  //               kVIA_BufferToSmall:    Buffer is to small
  //               otherwise kVIA_OK
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetTxRawValueByteArray(uint8* byteArray, int32 arrayLength, int32* signalLength) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 9)
  // Sets/Resets the raw value flag of a signal. If this flag is set, GetValue/SetValue gets/sets the signal values
  // as raw values in any case.
  // Params: rawValue: 0 to reset the flag, everything else sets the flag
  // Return codes: kVIA_OK (always)
  // Supported by CAPL-on-Board: No
  VIASTDDECL SetRawValueFlag(int32 rawValue) = 0;

  // Retrieves the value of a signal as double. If the value type of the signal can be casted to double and the raw 
  // value flag is not set, the physical value of the signal is returned. Otherwise, the raw value of the signal is 
  // returned.
  // Return codes: kVIA_WrongSignalType  if the signal cannot be represented as double value
  //               kVIA_OK  if the signal value could be retrieved
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetValue(double* value) = 0;

  // Sets the value of a signal. If the value type of the signal can be casted to double and the raw value flag is not 
  // set, the input value is interpreted as the new physical value of the signal, otherwise it is interpreted as the 
  // new raw value of the signal.
  // Return codes: kVIA_WrongSignalType: Value type not kVIA_SigDouble
  //               kVIA_Failed:          Value is not set, unexpected errors
  //               kVIA_OK:              Value is set
  // Supported by CAPL-on-Board: No
  VIASTDDECL SetValue(double value) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 10)
 // Retrieves the raw value of the signal as string
  // Return codes: kVIA_ParameterInvalid: One of the parameters is invalid
  //               kVIA_WrongSignalType:  Value type not kVIA_SigString
  //               kVIA_BufferToSmall:    Buffer is to small
  //               otherwise kVIA_OK
  VIASTDDECL GetRawValueString(char* stringBuffer, int32 bufferLength) = 0;
  // Sets the raw value of the signal as string
  // Return codes: kVIA_WrongSignalType: Value type not kVIA_SigString
  //               kVIA_ParameterInvalid: One of the parameters is invalid
  //               kVIA_Failed:          Value is not set, unexpected errors
  //               kVIA_OK:              Value is set
  VIASTDDECL SetRawValueString(const char* stringBuffer) = 0;
  // Retrieves the content of tx buffer as string.
  // Return codes: kVIA_ParameterInvalid: one of the parameter is NULL
  //               kVIA_WrongSignalType:  value type not kVIA_SigString
  //               kVIA_BufferToSmall:    Buffer is to small
  //               otherwise kVIA_OK
  VIASTDDECL GetTxRawValueString(char* stringBuffer, int32 bufferLength) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 11)
  // Sets the callback object. The notification is called with every tx 
  // of the signal.
  VIASTDDECL SetTxSink(VIAOnTxSignal* sink, void* userData) = 0;
#endif

#if defined (VIASignalAccessMinorVersion) && (VIASignalAccessMinorVersion >= 14)
  // Sets the callback object. The notification is called with every tx 
  // of the signal.
  VIASTDDECL SetTxSink2(VIAOnTxSignal2* sink, void* userData) = 0;
  VIASTDDECL RemoveTxSink2(VIAOnTxSignal2* sink) = 0;
#endif
};

// ============================================================================
// VIAOnSignalGroup
//
// Callback object for updates of a signal group
// Supported by CAPL-on-Board: No
// ============================================================================
class VIAOnSignalGroup
{
public:
  VIASTDDECL OnSignalGroup(VIASignalGroup* signal, void* userData) = 0;
};

// ============================================================================
// VIASignalGroup
// 
// Interface for accessing a group of signals. The Interface must be accessed  
// during init measurement phase of the VIANodeLayerApi object.
// While the measurement is running, the methods 'GetPhysicalValues' and 
// 'SetPhysicalValues' can be called asynchrony from any thread. 
// Supported by CAPL-on-Board: No
// ============================================================================

// Specifies the request types for a signal group
enum VIASignalGroupRequestType
{
  kVIA_RequestStandardUpdate = 1,
  kVIA_RequestStandardChange = 2,
  kVIA_RequestFlexRayUpdate = 3,
  kVIA_RequestFlexRayChange = 4,
};


class VIASignalGroup
{
public:
  // Retrieves the physical values of signals.
  // Return codes: kVIA_OK (always)
  VIASTDDECL GetPhysicalValues(double value[]) = 0;
  // Sets the physical values of signals.
  // Return codes: kVIA_OK
  VIASTDDECL SetPhysicalValues(double value[]) = 0;

#if (VIASignalAccessMinorVersion >= 2)
  // Sets the callback object. The requestType determines when the 
  // notification is called.
  // kVIA_RequestStandardUpdate: Every time a signal was updated (received)
  // kVIA_RequestStandardChange: Every time a signal value has changed
  // kVIA_RequestFlexRayUpdate:  All signals are received and 
  //     belong to the same FlexRay cycle 
  // kVIA_RequestFlexRayChange:  Like kVIA_RequestFlexRayUpdate, but at least
  //     one of the signal has changed its values.
  VIASTDDECL SetSink(VIAOnSignalGroup* sink, void* userData, uint32 requestType) = 0;
#endif
};


// ============================================================================
// VIAMemorySegment
// 
// Interface for storing signal values and environment variable values in a 
// memory location.
// While the measurement is running, the methods 'Write' and 'Read' can be 
// called asynchrony from any thread. 
// Supported by CAPL-on-Board: No
// ============================================================================

class VIAMemorySegment
{
public:
  // Gets the size of the memory segment
  VIASTDDECL GetSegmentSize(uint32* size)= 0;

  // Adds the physical value of signal to the memory segment.
  // The signal value is stored as a 64 bit floating point variable (double),
  // so 8 data bytes are required to store the value.
  VIASTDDECL AddSignalPhysical(VIASignal* signal, uint32 offset) = 0;

  // Adds a CANoe environment variable to the memory segment at the given offset.
  // For variable of type string or data, the parameter maxsize is required
  //  and it limits the maximum amount of memory in Bytes, that is used for 
  //  storing the content of the variable.
  // Variables of type 'integer' are stored as a 32 bit signed integer value 
  //  (4 Bytes required)
  // Variables of type 'float' are stored as 64 bit  floating point values 
  //  (8 Bytes required). 
  // Variables of type string are stored as null terminated ASCII encoded 
  //  C string. The size of the string is limited by parameter maxsize. Be
  //  aware, that the value maxsize contains the termination zero character.
  //  (maxsize Bytes required)
  // When storing variables of type data, first the size of the array is 
  //  written as a 32 bit integer value to the memory followed by the data 
  //  bytes of the environment variable. The total size of memory is limited 
  //  by the parameter maxsize. Be aware, that the parameter maxsize also 
  //  contains the 4 Bytes that are needed for the size of the array.
  VIASTDDECL AddEnvVar(VIAEnvVar* ev, uint32 offset, uint32 maxsize= 0) = 0; 

  // Writes the values to the given location in memory.
  VIASTDDECL Write(void* address)= 0;

  // Reads the values from the given location in memory.
  VIASTDDECL Read(void* address)= 0;
};


// ============================================================================
// VIATransmitterCallback
//
// Callback object for Signal Transmitters.
// Supported by CAPL-on-Board: No
// ============================================================================
class VIATransmitterCallback
{
public:
  // Gets the major and minor version of header file 'VIA_SignalAccess.h' when
  // compiling the NodeLayer-DLL.
  VIASTDDECL GetVersion (int32* major, int32* minor) = 0;

  // Request from signal server to take a new value to transmit.
  VIASTDDECL OnSetSignal(VIASignal* signal, void* userData) = 0;

  // Confirmation, that after one or more calls to the method 'OnSetSignal'
  // currently no other new tx value is buffered in the signal server.
  VIASTDDECL OnSetSignalsCompleted() = 0;

  // Starts command for the transmitter. Before transmitting messages to a bus,
  // a signal transmitter should wait for this start command giving other 
  // components a chance to set the initial values. The start command is 
  // coming in a late phase of measurement start
  VIASTDDECL OnStartCommand() = 0;

#if (VIASignalAccessMinorVersion >= 5)
  // Notification, that a signal generator was attached to the given signal
  VIASTDDECL OnGeneratorAttached(VIASignal* signal, void* userData) = 0;
  // Notification, that a signal generator was detached from the given signal
  VIASTDDECL OnGeneratorDetached(VIASignal* signal, void* userData) = 0;
#endif
};


// ============================================================================
// VIASignalTransmitter
//
// Transmitter object for signals.
// Supported by CAPL-on-Board: No
// ============================================================================

// Specifies the flags for registering a signal transmitter
// See method VIASignalTransmitter::AddSignal2 for a description of the flags
enum VIASignalTransmitterFlags
{
  kVIA_CallTransmitterOnEveryUpdate = 1,
  kVIA_CallOnSetSignalsCompleted   = 2,
};

// Specifies the flags for operation modes of a signal transmitter
// See method VIASignalTransmitter::SetTransmitterMode for a description of the flags
enum VIASignalTransmitterFlags2
{
  kVIA_Disable = 1,
  kVIA_Enable  = 2
};

// Specifies the operation modes of a signal transmitter
// See method VIASignalTransmitter::SetTransmitterMode for a description of the modes
enum VIASignalTransmitterModes
{
  kVIA_MultiplexerHandling = 1
};

class VIASignalTransmitter
{
public:
  // Obsolete (version 2.7)
  // The method calls 'AddSignal2' with the flags kVIA_CallTransmitterOnEveryUpdate
  // and kVIA_CallOnSetSignalsCompleted 
  VIASTDDECL AddSignal(VIASignal* signal, void* userData) = 0;
  
  // Removes the registration as transmitter from the given signal.
  VIASTDDECL RemoveSignal(VIASignal* sink) = 0;

  // Sets the callback object as notification sink.
  VIASTDDECL SetSink(VIATransmitterCallback* sink) = 0;

#if (VIASignalAccessMinorVersion >= 8)
  // Registers the transmitter object for the given signal as a signal 
  // transmitter. For every signal only one transmitter can be registered. But
  // one transmitter can server several signals.
  // parameter flags: (see enum VIASignalTransmitterFlags)
  //   kVIA_CallTransmitterOnEveryUpdate: If this flag is used, the 'OnSetSignal' 
  //     method of the signal transmitter's callback object is called every time 
  //     a value is assigned to the signal. Otherwise the method is only called 
  //     when the new value differs from the previous value.
  //   kVIA_CallOnSetSignalsCompleted: Enables calls of the completion method
  //     'OnSetSignalsCompleted' of the signal transmitter's callback object.
  // Return value:
  //   kVIA_OK:  Transmitter could be successfully registered for the signal.
  //   kVIA_TransmitterAlreadySet:  Another transmitter is already registered
  //     for the given signal.
  //   kVIA_ParameterInvalid: Signal parameter is NULL or an unknown flag is used
  //   kVIA_Failed:  Transmitter could not be registered for the given signal 
  //     because of an unspecific error.
  VIASTDDECL AddSignal2(VIASignal* signal, void* userData, uint32 flags) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 12)
  // Sets a specific operation mode of the transmitter:
  //   mode == kVIA_MultiplexerHandling:
  //     flags == kVIA_Disable: Disables the multiplexer handling at the signal server in CANoe
  //     flags == kVIA_Enable: Enables the multiplexer handling at the signal server in CANoe (default)
  //   If the multiplexer handling is enabled, then only those signals will be notified on TX
  //   when they belong to the current multiplexer value. Additionally, when the multiplexer is set,
  //   then all signals that belong to this value will also be notified for TX.
  // Return value:
  //   kVIA_OK:  Transmitter mode is successfully set.
  //   kVIA_Failed:  Transmitter mode could not be set
  //   kVIA_FunctionNotImplemented:  Transmitter mode or flags are unknown
  VIASTDDECL SetTransmitterMode(uint32 mode, uint32 flags) = 0;
#endif
};


// ============================================================================
// VIASignalAccessApi 
//
// Interface for accessing signals. Must be accessed during init measurement
// phase of the VIANodeLayerApi object. The interface is used as an factory
// to get the signal interface (VIASignal) of a concrete signal. Every signal,
// that is acquired by this interface, must be release by a call to the member
// function ReleaseSignal().
// 
// The methods GetSignal and  GetSignalByTxNode should also be called during 
// the init measurement phase.
// Supported by CAPL-on-Board: No
// ============================================================================
class VIASignalAccessApi
{
  public:
    // Gets version of the signal access API.
    // Return codes: kVIA_BufferToSmall: both parameters are NULL
    //               otherwise kVIA_OK
    VIASTDDECL GetVersion (int32* major, int32* minor) = 0;

    // Returns the signal, specified with database, message and signal name
    // Return codes: kVIA_ObjectNotFound: One of the object, specified by name, is not found
    //               otherwise kVIA_OK
    VIASTDDECL GetSignal(VIASignal** signal,  
                         const char* databaseName, 
                         const char* messagename, 
                         const char* signalName) = 0;
    // Returns the signal, specified with database, node, message and signal name
    // Return codes: kVIA_ObjectNotFound: One of the object, specified by name, is not found
    //               otherwise kVIA_OK
    VIASTDDECL GetSignalByTxNode(VIASignal** signal, 
                                 const char* databaseName, 
                                 const char* nodeName, 
                                 const char* messagename, 
                                 const char* signalName) = 0;

    // Releases the given VIASignal object.
    VIASTDDECL ReleaseSignal(VIASignal* signal) = 0;

#if (VIASignalAccessMinorVersion >= 2)
    // Creates a VIASignalGroup object, that contains all the signals given by
    // the parameter 'signals. The parameter numberOfSignals specifies the 
    // the size of array 'signals'.
    // Return codes: kVIA_ParameterInvalid: signalGroup is NULL
    //               kVIA_ObjectCreationFailed: numberOfSignals is negative
    //                                          or one of signals is invalid
    //               otherwise kVIA_OK
    VIASTDDECL GetSignalGroup(VIASignalGroup** signalGroup,
                              int32 numberOfSignals,
                              VIASignal* signals[] ) = 0;

    // Release the given VIASignalGroup object.
    VIASTDDECL ReleaseSignalGroup(VIASignalGroup* signal) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 3)
    // Create a VIAMemorySegment object. The parameter direction specifies, if
    // the VIAMemorySegment should be used for reading values from CANoe 
    // (direction=kVIA_Rx) of for setting values to CANoe (direction=kVIA_Tx).
    // Return codes: kVIA_ParameterInvalid: memorysegment or direction is invalid
    //               otherwise kVIA_OK or kVIA_Failed
    VIASTDDECL CreateMemorySegment(VIAMemorySegment** memorysegment,
                                   uint32 segmentSize,
                                   int32 direction)= 0;

    // Release the given VIAMemorySegment object.
    VIASTDDECL ReleaseMemorySegment(VIAMemorySegment* memorysegment) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 4)
    // Creates a VIASignalTransmitter object
    // Return codes: kVIA_ParameterInvalid: parameter is invalid
    //               otherwise kVIA_OK or kVIA_Failed
    VIASTDDECL CreateTransmitter(VIASignalTransmitter** transmitter) = 0;

    // Releases the given VIASignalTransmitter object.
    VIASTDDECL ReleaseTransmitter(VIASignalTransmitter* transmitter) = 0;

    // Gets a signal by using the VIDBSignalDefinition interface object.
    // For protocols, that requires a node name, the node context of 
    //  the VIASignalAccessApi is used (That is the VIANode used when creating 
    //  the interface object)
    // Return codes: kVIA_ParameterInvalid: parameter is invalid
    //               kVIA_ObjectNotFound: signal or node was not found
    //               otherwise kVIA_OK or kVIA_Failed
    VIASTDDECL GetSignalByDBSignalDefinition(VIASignal** signal, 
                                             VIDBSignalDefinition* definition) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 6)
    // Returns a signal by an identification string of the following form:
    // [[[[network_name::]db_name::]node_name::]msg_name::]signal_name
    // Return codes: kVIA_ObjectNotFound: signal was not found
    //               kVIA_SignalAmbiguous: name was not unique
    //               otherwise kVIA_OK
    VIASTDDECL GetSignalByName(VIASignal** signal,  const char* unique_name) = 0;
#endif

#if (VIASignalAccessMinorVersion >= 13)
    
    // Returns the signal, specified with database, node, frame path and signal name
    // Return codes: kVIA_ObjectNotFound: One of the object, specified by name, is not found
    //               otherwise kVIA_OK
    VIASTDDECL GetSignalByTxNodeAndPath(VIASignal** signal,
      const char* databaseName,
      const char* nodeName,
      const char* framePath,
      const char* signalName) = 0;
#endif 
};


// ============================================================================
// Old version of the Interface for accessing signals. Must be accessed during 
// init measurement phase of the VIANodeLayerApi object.
// This API is obsolete and should not be used any longer
// The major version of the API is 1, the minor version is also 1.
// Supported by CAPL-on-Board: No
// ============================================================================

// Obsolete (version 1.1)
class VIASignalAccessApi_V1
{
  public:
    // Obsolete (version 1.1)
    VIASTDDECL GetVersion (int32* major, int32* minor) = 0;
    // Obsolete (version 1.1)
    VIASTDDECL GetSignal(const char * aSignal, const char * aMsg, const char * aDB, double* aValue) = 0;
    // Obsolete (version 1.1)
    VIASTDDECL GetSignalTime(const char * aSignal, const char * aMsg, const char * aDB, VIATime* aTime) = 0;
    // Obsolete (version 1.1)
    VIASTDDECL CheckSignalInRange(const char * aSignal, const char * aMsg, const char * aDB, double aLowLimit, double aHighLimit, long* aRet) = 0;
    // Obsolete (version 1.1)
    VIASTDDECL GetSignalGM(const char * aSignal, const char * aMsg, const char * aDB, unsigned long aSourceId, double* aValue) = 0;
    // Obsolete (version 1.1)
    VIASTDDECL GetSignalTimeGM(const char * aSignal, const char * aMsg, const char * aDB, unsigned long aSourceId, VIATime* aTime) = 0;
    // Obsolete (version 1.1)
    VIASTDDECL CheckSignalInRangeGM(const char * aSignal, const char * aMsg, const char * aDB, unsigned long aSourceId, double aLowLimit, double aHighLimit, long* aRet) = 0;
};

#endif // VIA_SIGNALACCESS_H
