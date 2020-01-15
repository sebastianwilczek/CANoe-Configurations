/*-----------------------------------------------------------------------------
Module: VIA (observed)
Interfaces: This is a public interface header.
-------------------------------------------------------------------------------
New external interface for CANoe nodes (Nodelayer-DLLs)
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.

-----------------------------------------------------------------------------*/

#ifndef VIA_H
#define VIA_H

/*-----------------------------------------------------------------------------
Version of the interfaces, which are declared in this header

09.11.1999   1.0   As     Creation
Dec   1999   1.3   As     First usable Version
04.01.1999   1.4   As     SetCanLine (Online/Offline) added
11.01.2000   1.5   As     Support for ErrorFrames completed
12.01.2000   1.5   Kr     Minor corrections, especially in comments
19.01.2000   1.6   As     Bus interface moved into its own object
                          CANoe environment variables added
                          Cleanup of many data types
                          64 bit integer variables used for time
                          Support for OSEK emulation added
24.01.2000   1.7   As     Multiple Databases and minor changes  
02.03.2000   1.7.1 As     Correction for environment variables and
                          handling of needed typedefs
15.03.2000   1.8   As     All event callbacks have now a time parameter.
                          Check of version number now before anything else
                          is done. 
29.03.2000   1.9   As     Utilities added. 
                          Timers are now configured with call to CreateTimer.
                          Calls to CAPL-Functions can use a result parameter
                          of type uint32.  
14.02.2002   1.10  Sn     Multibus support added.
                          CANoe supports multiple instantiations of node layer 
                          DLLs per CAPL block (which now models an entire ECU
                          rather than a single node).
06.12.2002   1.11  As     Database names and paths can now be retrieved from
                          VIAService and VIANode.
15.09.2003   1.12  Bee    Access to the test control api-class added
                          More functionality for write-outputs added
14.10.2003   1.13  Bee    ClearWriteTab() added.
                          Predefined write-sinks added
                          Argument types for the write tab argument size changed
24.10.2003   1.14  Aa     Access to Node Layer Services
24.11.2003   1.15  Bee    Write Sink "Test" introduced
25.11.2003   1.16  As     VIAEnvVar extended to get the current size of variable
                          of type string and data
11.05.2004   1.17  Aa     Signal access introduced
14.09.2004   1.18  Krs    Config item language added
21.09.2004   1.19  As     Additional ServiceFlags added
22.12.2004   1.20  Rue    Database access refactoring
16.08.2005   1.21  Jr     Added GetJ1939Message to VICAN
18.07.2006   1.22  Sha    Added support for LIN ETF-events 
26.09.2006   1.23  Aa     Added new events for the VIDBSignalDefinition-API
29.01.2006   1.23.1 Jsd   Added kVIA_Failed (no version change -> As)
2007-03-12   1.23.2 Trs   Added kVIA_DiVaSupport (no version change)
19.03.2007   1.24  Aa     New method within the VIDBSignalDefinition-API to 
                          set and get raw values of float signals
13.08.2007   1.24.1 As    Error code 'kVIA_TransmitterAlreadySet' added
26.06.2007   1.25  Rue    System variables
10.09.2007   1.26  Rue    Extensions to Database interface; small extension to
                          system variables interface; DebugBreak
28.09.2007   1.26.1 As    Error code 'kVIA_GeneratorNotAvailable' added
10.10.2007   1.27  Jsd    EVAL00043869
18.01.2008   1.28  Msc    Extensions to VIASignalAccessApi
31.03.2008   1.29  Cbo    EVAL00047643 Allow double return value for CAPL callbacks
06.05.2008   1.30  Wbn    VIAOnFlexRayPOCState added
17.02.2009   1.32  As     IsSimulated added to VIAService
01.03.2009   1.33  Sn     Socket Interface and Diagnostic Notifications added
18.06.2009   1.34  Trs    Interface to retrieve diagnostics descriptions added EVAL00049188
28.08.2009   1.35  Jsd    Added FrSetKeySlot (EVAL00057483)
05.09.2009   1.36  Aa     VIDBSignalDefinition - getter/setter for signals > 64 bit
21.01.2010   1.37  Rue    Extensions to system variables
04.02.2010   1.38  Cbo    Added new FlexRay send function for PDU allowing different payload size
31.03.2010   1.39  Jsd    Added VIAECU::GetTitle() to retrieve title of ECU in simulation setup
25.05.2010   1.40  Sn     Serial Interface added
09.12.2010   1.41  As     Access to 'User Files' added. A DLL can now reject the 
                          creation of a nodelayer object.
08.03.2011   1.42  As     System variables: Default Module Handle and type Data added
12.05.2011   1.43  Jsd    Added RxNodeDefinition in VIDBMessageDefinition
30.06.2011   1.44  As     ServiceFlag kVIA_IsSameMachine added
26.09.2011   1.45  Sn     Socket interfaces enhanced
18.01.2011   1.45  Rue    System variables: define with initial value
18.01.2012   1.46  Bma    Raw value flag for signals added
10.11.2011   1.45  Jsd    Added new handler for FlexRay StartCycle events
16.12.2011   1.45  Rue    Unit added to system variables
02.01.2012   1.46  Sbd    Description of GetUserFilePath changed
23.03.2012   1.47  Sbd    Additional return value in GetUserFilePath
28.03.2012   1.48  Cbo    Added some functions for VIDBSignalDefinition
03.04.2012   1.48  Sbd    RegisterUserFile added, additional return value in GetUserFilePath
12.09.2012   1.49  Uj     Added VIASocket::SetOptionEx()
20.02.2012   1.50  Rue    Extensions for system variable structures
03.04.2012   1.51  Jsd    OpenNodePanel / OpenNetworkPanel
22.03.2013   1.51  Fsi    CAN-FD Support
31.05.2013   1.52  Sbj    EVAL00105173
24.06.2013   1.53  Sjo    added functions in VIA_NLServiceCANoeIL.h
08.10.2013   1.54  Fsi    Added functions 'SimIncrementTimerBase' and 'SimIsSlaveMode' in VIAService (EVAL00108694)
10.07.2012   1.55  As     Support for CAPL-on-Board added
08.01.2014   1.56  Aa     Introduce new VIA API for tests (VIATestApi) valid for test modules AND test units.
                          Hint: The VIATestControlApi can only be used for test modules - NOT for test units
12.02.2014   1.57  Jhn    Added functions 'GetSocketServiceEx' and 'ReleaseSocketServiceEx' in VIAService
09.01.2014   1.58  Tnr    Rudimentary ParameterService access added, mainly driven by the new node layer configuration framework (EVAL00108361)
09.05.2014   1.59  Saa    Added extended version of functions for NLService
23.05.2014   1.60  Tnr    GetSynchronizedFilePath added
18.06.2014   1.61  Sjo    EVAL00114743 - added functions in VIA_NLServiceCANoeIL.h
13.08.2014   1.63  Cbo    EVAL00123427: VIA extension for accessing the multiplex information of frames/PDUs
05.10.2014   1.64  Sjo    EVAL00124246 - added functions in VIA_NLServiceCANoeIL.h
10.10.2014   1.65  Rue    Added functions for 64bit integer system variables
05.12.2014   1.66  Rue    Added string get / set for system variables with encoding
13.01.2014   1.67  Lt     Added support for specific struct member/array element initialization
06.02.2015   1.68  Pr     EVAL00129138: Media interface added
10.02.2015   1.69  Jmi    Added functionality in VIA_NLServiceRemoteIL.h
14.04.2015   1.70  Sjo    EVAL00132989 - added function in VIA_NLServiceCANoeIL.h
22.04.2015   1.71  Jsd    EVAL00132309 - added InitMeasurementComplete in VIANodeLayerApi
02.07.2015   1.72  Sjo    EVAL00135271 - access to value tables of signals
23.07.2015   1.73  Uru    EVAL00135201 - extended VIABusInterfaceType by Option.A429
22.09.2015   1.74  Sjo    EVAL00138215 - added functions to set/get current client and bus context
15.10.2015   1.75  Cbo    EVAL00138856: Node layer interface (VIA) must be enhanced for allowing DLL generating system variables and renaming them
21.10.2015   1.76  Jsd    EVAL00138874 - added FrEnableNodeTxEx
22.10.2015   1.77  Rue    EVAL00135958 - SV struct extensions
23.11.2015   1.79  Sjo    EVAL00138886 - added function in VIA_NLServiceCANoeIL.h
19.01.2016   1.80  Fsi    EVAL00140436 - PDU service layer
20.01.2016   1.81  Sjo    EVAL00141225 - Bug fix for EVAL00138886
29.01.2016   1.82  Fsi    EVAL00141225 - EVAL00141873
04.02.2016   1.83  Cbo    EVAL00141988 - Access to End2End protection info of signal group
10.02.2016   1.84  Cbo    EVAL00142348 - VIA Registration of the handlers require pointer to user data
15.02.2016   1.85  Cbo    EVAL00142544 - added registration of VIAOnPDUStartTX by CreatePDURequestStartTX and set VIASignalAccessMinorVersion = 14
10.03.2016   1.85  Rue    EVAL00143582 - Support SV type "data" for struct members
16.03.2016   1.86  Cbo    EVAL00142875 - [E2E] Support End-to-End-Protection via Transformer in VIA (VIA_Pdu.h 1.2)
16.03.2016   1.86  Cbo    EVAL00142894 - Access to AUTOSAR Transfer Property of signal
16.03.2016   1.86  Cbo    EVAL00143837 - Transfer property and Update Bit position must also be available for a signal group
11.04.2016   1.87  Wwi    EVAL00143497 - enable PDU-support in AFDX_IL
11.04.2016   1.88  Cbo    EVAL00141989 - Multiplexed-I-PDUs and Timings in VIA (VIA_Pdu.h 1.4)
23.05.2016   1.89  Srj    EVAL00145865 - PDU Streaming 4 FlexRay: Add GetPDUService to VIA_FexRay
23.05.2016   1.90  Mkb    EVAL00148231 - VIA Interface for Global Time Information
30.09.2016   1.91  Sjo    EVAL00143426 - VIA Interface for ASR PDU IL disturbances
05.10.2016   1.92  Set    EVAL00065243 - Added extended version of OnLinSchedulerModeChange
25.11.2016   1.93  Mkb    EVAL00150751 - VIA PDU-Frame-Mapping bereitstellen (COMdbLib IMappedPDU)
19.12.2016   1.94  Arr    Added FunctionBus interface
17.01.2017   1.95  Rue    Added core processing functions interface
sometime     2.0          By definition version 2.0 will be the first release
in the                    of VIA interface, that is not longer compatible with 
future                    version 1.x 
-----------------------------------------------------------------------------*/

#if !defined ( VIAMajorVersion )
  #define VIAMajorVersion 1
#endif
#if !defined ( VIAMinorVersion )
  #define VIAMinorVersion 95
#endif

// ============================================================================
// Dependencies
// ============================================================================


#ifndef HAS_FIX_SIZE_INT_DEFINED
#define HAS_FIX_SIZE_INT_DEFINED

  typedef signed char       int8;
  typedef signed short      int16;
  typedef signed long       int32;
  typedef signed long long   int64;

  typedef unsigned char     uint8;
  typedef unsigned short    uint16;
  typedef unsigned long     uint32;
  typedef unsigned long long uint64;

#endif // HAS_FIX_SIZE_INT_DEFINED

  // We need only the type HWND from <windows.h>.
  // If you don't need windows, the following dummy declaration is all that you 
  // need. Otherwise include <windows.h> before you process this header.
#if !defined ( _WINDOWS_ ) && !defined ( _WINDEF_ )
   typedef void*   HWND;
#endif // _WINDOWS_

#if defined (__ghs__)
  typedef void* LPVOID;
#endif


// some forward declarations
class VIANode;              
class VIANodeLayerApi;
class VIAModuleApi;
class VIARequest;
class VIAEnvVar;
class VIAUtil;
class VIATestObject;
class VIATestControlApi;
class VIATestApi;
class VIANLService;
class VIANLServiceApi;
class VIASignalAccessApi;

class VIDBAttribute;
class VIDBMessageDefinition;
class VIDBSignalDefinition;
class VIDBNodeDefinition;
class VIDBSignalGroup;
class VIDBDatabase;
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 63)
class VIDBMultiplexerRange;
#endif

class VIANamespace;
class VIASystemVariable;
class VIASocketService;
class VIASocketServiceEx;
class VIASerialService;
class VIACAPLonBoardConstruction;
class VIASystemVariableMemberInitValues;
class VIAMediaService;
class VIAFbViaService;
class VIDBPDUDefinition;
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 83)
class VIDBEnd2EndProtectionInfo;
#endif
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 90)
class VIDBAUTOSARDescriptionInfo;
#endif
class VIDBMappedPDUDefinition;

#if defined(_MSC_VER)
  #if defined ( _BUILDNODELAYERDLL )
    #define VIACLIENT(type)  extern "C" __declspec (dllexport) type __stdcall
  #else
    #define VIACLIENT(type)  extern "C" __declspec (dllimport) type __stdcall
  #endif
#elif defined (__ghs__)
  #define VIACLIENT(type)  extern "C" type
#else
  #error "Unknown Compiler"
#endif



// ============================================================================
// Standard return type and return values for the services
// ============================================================================



// 'VIAResult' is the standard result type of all methods declared below. 
typedef int32 VIAResult;  

// Standard declaration and definition macro for methods with an result of 
// type 'VIAResult'.
#if defined(_MSC_VER)
  #define VIASTDDECL  virtual VIAResult __stdcall
  #define VIABOOLDECL virtual bool __stdcall
  #define VIASTDDEF   VIAResult __stdcall
  #define STDCALL     __stdcall
#elif defined (__ghs__)
  #define VIASTDDECL  virtual VIAResult
  #define VIABOOLDECL virtual bool
  #define VIASTDDEF   VIAResult
  #define STDCALL
#else
  #error "Unknown Compiler"
#endif


// The following are return values of the services
enum VIAResultValue {
  kVIA_OK  = 0,              // normal result value used if a call happens well
  kVIA_WrongVersion = 1,
  kVIA_MissingInterface = 2,
  kVIA_ObjectCreationFailed = 3,
  kVIA_ServiceNotRunning = 4,
  kVIA_ObjectNotFound = 5,
  kVIA_BufferToSmall = 6, 
  kVIA_ObjectInvalid = 7,
  kVIA_FunctionNotImplemented = 8,
  kVIA_SignalGroupInvalidForRequest = 9,
  kVIA_ParameterInvalid = 10,
  kVIA_Failed = 11,
  kVIA_TransmitterAlreadySet = 12,
  kVIA_GeneratorNotAvailable = 13,
  kVIA_SignalAmbiguous = 14,
  kVIA_WrongSignalType = 15,
  kVIA_IoPending = 16,
  kVIA_ObjectUndesired = 17,

  kVIA_DBUnspecificError = 100,
  kVIA_DBAttributeNameNotFound = 101,
  kVIA_DBObjectNameNotFound = 102,
  kVIA_DBParameterInvalid = 103,
  kVIA_DBObjectNotFound = 104,
  kVIA_DBSignalValueRangeWarning = 105,
  kVIA_DBSignalValueLengthError = 106,
  kVIA_DBSignalValueModeError = 107,
}; // VIAResult


// ============================================================================
// VIATime
//
// For time information we are using a 64 bit integer.
// VIATime(1) is one nanosecond
// ============================================================================


// Don't remove the scope operator (::) below, otherwise OWL and MFC 
// have a conflict!!! 
typedef ::int64 VIATime;


inline VIATime VIATimeMilliSec (int ms)
{
 #if defined(__BORLANDC__) && defined(SERVICES_WSYSINC_H)
   // SERVICES_WSYSINC_H form Borland C++ declares 'class int64', which leads
   // to trouble when using VIATime like an intrinsic type. Please don't use
   // it! Otherwise you must use code like this:
   LARGE_INTEGER time;
   time.QuadPart = __int64(1000000) * __int64(ms);
   return VIATime( int64(time) );
 #else
   return VIATime(1000000) * VIATime(ms);
 #endif
}


// ============================================================================
// Request blocks
// ============================================================================


typedef VIARequest* VIARequestHandle;

//
// VIARequest is an empty class, thats used to build a generic handle for 
// Requests. 
//
class VIARequest
{
};  // class VIARequest


// ============================================================================
// Attributes and Iterators
// ============================================================================


#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)

#if defined(_MSC_VER ) && ( _MSC_VER <= 1200 )
#pragma warning( disable: 4661) // 'void fooclass<class type>::foo(void)' : no suitable definition provided for explicit template instantiation request
#endif

//
// Iteration over some database objects.
// The "GetNextElement" function may create the elements it returns, all of them _must_ be 
// released with their normal release mechanism. If GetNextElement or SkipElement are called  
// while HasMoreElements returns false, kVIA_ObjectInvalid is returned.
// Supported by CAPL-on-Board: No
//
template<typename T>
class VIDBIterator
{
public:
    VIABOOLDECL  HasMoreElements() const = 0;
    // Returns the current element and moves the iterator forward (like STL: (*it)++)
    VIASTDDECL GetNextElement(T** element) = 0;
    // Go to next element without creating (like STL: ++it)
    VIASTDDECL SkipElement() = 0;
    // Releases the iterator object
    VIASTDDECL Release() = 0;
};

//
// Macro to declare an iterator quickly. E.g., iteration over attributes of a node is
// thereby declared in VIANode (see below) as
// typedef VIAIterator<VIDBAttribute> VIDBAttributeIterator;
// VIASTDDECL GetAttributeIterator(VIDBAttributeIterator** iterator) = 0;
//
// You may either use the typedef (e.g. VIANode::VIDBAttributeIterator)
// or the templated form (e.g. VIAIterator<VIDBAttribute>).
//
#define VIDB_DECLARE_ITERATABLE(type)                                    \
typedef VIDBIterator<VIDB##type> VIDB##type##Iterator;                     \
VIASTDDECL Get##type##Iterator(VIDB##type##Iterator** iterator) = 0;

//
// Macro to declare a const iterator quickly.
//
// You need this macro when you want to declare two iterators of the same type inside one class.
// The alias allows you to differentiate between your two iterators.
//
#define VIDB_DECLARE_ITERATABLE_ALIAS(alias, type)                 \
  typedef VIDBIterator<VIDB##type> VIDB##alias##Iterator;                \
  VIASTDDECL Get##alias##Iterator(VIDB##alias##Iterator** iterator) = 0;

// Pulled out of VIAService
enum VIA_DBType {
    kVIA_DBFloat     = 160,    // attribute type for attributes of type Float, Integer or Enum
    kVIA_DBString    = 165     // attribute type for attributes of type String 
};

//
// Attribute of an object, e.g. of a network, a message, or a signal. The behaviour is undefined
// if you use GetValue or GetString with attributes of the wrong type.
// Supported by CAPL-on-Board: No
//
class VIDBAttribute 
{
public:
   // Returns the name of the db object
   VIASTDDECL GetName(char* buffer, int32 bufferLength) const = 0;
   // Returns the type (see enum VIA_DBType) of the db object
   VIASTDDECL GetType(uint32& type) const = 0;
   // Returns the value for the type kVIA_DBFloat
   VIASTDDECL GetValue(double& value) const = 0;
   // Returns the value for the type kVIA_DBString
   VIASTDDECL GetString(char* buffer, int32 bufferLength) const = 0;
   // Releases the attribute object
   VIASTDDECL Release() = 0;
};

//
// Macro to declare that an interface gives access to attributes defined in a database.  See
// VIDBAttribute above.
//
#define VIDB_DECLARE_ATTRIBUTE_HOLDER                               \
 public:                                                            \
    /* Gets attribute, specified by name                                         */ \
    /* Returns kVIA_OK: attribute was found and was given back                   */ \
    /*   kVIA_DBAttributeNameNotFound: name of attribute can't found in database */ \
    /*   kVIA_ObjectInvalid: function failed                                     */ \
    VIASTDDECL GetAttribute(const char* pAttrName,                   \
                            VIDBAttribute** attribute) = 0;          \
    /* Gets the type of specified attribute                                           */ \
    /* Returns kVIA_OK: attribute was found and type was given back                   */ \
    /*   kVIA_Failed or kVIA_ObjectInvalid or kVIA_DBUnspecificError: function failed */ \
    /* [OUT] pType (VIA_DBType): kVIA_DBFloat: for database types int, float and hex  */ \
    /*                         kVIA_DBString: for database types string and enum      */ \
    VIASTDDECL GetAttributeType(uint32* pType,                       \
                                const char* pAttrName) const = 0;    \
    /* Gets the numeric value of specified attribute                                  */ \
    /* Returns kVIA_OK: attribute was found and the value was given back              */ \
    /*   kVIA_DBAttributeNameNotFound: name of attribute can't found in database      */ \
    /*   kVIA_Failed or kVIA_ObjectInvalid or kVIA_DBUnspecificError: function failed */ \
    VIASTDDECL GetAttributeValue(double* pValue,                     \
                                 const char* pAttrName) const = 0;   \
    /* Gets the string value of specified attribute                                   */ \
    /* Returns kVIA_OK: attribute was found and the value was given back              */ \
    /*   kVIA_DBAttributeNameNotFound: name of attribute can't found in database      */ \
    /*   kVIA_BufferToSmall: specified buffer is to small for value                   */ \
    /*   kVIA_Failed or kVIA_ObjectInvalid or kVIA_DBUnspecificError: function failed */ \
    VIASTDDECL GetAttributeString(char* pBuffer, int32 bufferLength, \
                                  const char* pAttrName) const = 0;  \
    /*  Iterates over attributes */                                  \
    VIDB_DECLARE_ITERATABLE(Attribute)                                

#endif // minor version >= 20

// ============================================================================
// Bus interface (common part)
// ============================================================================


// Channel of a bus interface. 
// Attention: the numbering starts with one. The value zero is used as a 
// defined undefined value.
typedef uint16   VIAChannel; 
const VIAChannel kVIA_WildcardChannel = 0xFFFF; // any channel
const VIAChannel kVIA_InvalidChannel  = 0x0000; // no channel

// Specifies the direction of a message
enum VIADir
{
  kVIA_Rx        =0,    // receive
  kVIA_Tx        =1,    // transmit
  kVIA_TxRq      =2     // transmit request
};

// Used for creating request about messages on an bus
enum VIARequestType
{
  kVIA_OneId     =0,   // Receive only one Id        
  kVIA_AllId     =1,   // Receive all Ids
  kVIA_MskId     =2    // Receive all Ids specified by Mask 
};

// Specifies constants for method SetLine, which switches parts
// of the bus interface on or off
enum VIAOnlineOffline
{
  kVIA_Online   = 1,   // switch bus interface on
  kVIA_Offline  = 0,   // switch bus interface off
  kVIA_Model    = 2,   // switching impacts CAPL program
  kVIA_DLL      = 4    // switching impacts nodelayer dll
};

// Specifies types of bus interface
enum VIABusInterfaceType
{
  kVIA_Wildcard = -1,
  kVIA_Invalid  =  0,
  kVIA_CAN      =  1, // CAN bus at level 2  
  kVIA_1939     =  2, // CAN bus at J1939 view
  kVIA_VAN      =  3, // VAN 
  kVIA_TTP      =  4, // TTP
  kVIA_LIN      =  5, // LIN
  kVIA_MOST     =  6, // MOST
  kVIA_FLEXRAY  =  7, // FLEXRAY
  kVIA_BEAN     =  8, // BEAN
  kVIA_Ethernet = 11, // Ethernet
  kVIA_WLAN     = 13, // WLAN
  kVIA_AFDX     = 14, // AFDX
  kVIA_A429 	= 16  // A429
};

//
// The bus object
// Supported by CAPL-on-Board: Yes
//
class VIABus
{
public:
   // Returns the type and the version of this bus interface object
   // Supported by CAPL-on-Board: Yes
   VIASTDDECL GetVersion (uint32* busInterfaceType,                      // [OUT] Type (see enum VIABusInterfaceType)
                          int32* majorversion, int32* minorversion) = 0; // [OUT] Version

   // Returns the number of available channels for this bus interface
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetNumberOfChannels(VIAChannel* maxChannelNumber) = 0;     // [OUT] Number of available channels

   // Switch the bus interface of a node on or off.
   // Note: At the moment, kVIA_WildcardChannel is the only supported value for the parameter channel.
   // Supported by CAPL-on-Board: No
   VIASTDDECL SetLine (VIAChannel channel,      // [IN] Describes the channel, that should be turned on or off.
                       uint32     mode,         // [IN] kVIA_Online or kVIA_Offline
                       uint32     part32) = 0;  // [IN] Describes, which part of a node is switched on or off.
                                                //      A combination of the flags kVIA_Model and kVIA_DLL.

   // General method to release a request for messages on the bus.
   // The creation of the events is specific to the concrete bus interface
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseRequest (VIARequestHandle handle) = 0;
};

typedef VIABus VIANetwork;



// ============================================================================
// Interface to timer
// ============================================================================

 
//
// Callback object for a ringing timer
// Supported by CAPL-on-Board: Yes
class VIAOnTimerSink
{
public:
   // The time provided by the callback method 'OnTimer' is the time, when 
   // the timer should have done a ring. (This is like an ideal timer, that 
   // never has a delay even when the CPU is busy.) 
   // Supported by CAPL-on-Board: Yes 
   VIASTDDECL OnTimer (VIATime nanoseconds) = 0;
};

//
// The timer object
// Supported by CAPL-on-Board: Yes
//
class VIATimer
{
public:
   // Sets the callback object of the timer. 
   // The sink-object will be referenced and MUST NOT be deleted until the 
   // timer is released or the sink is set otherwise.
  // Supported by CAPL-on-Board: No 
   VIASTDDECL SetSink (VIAOnTimerSink* sink) = 0; // [IN] Sink will be referenced

   // Sets the name of the timer. The name is used for debugging purpose 
   // and for warnings, which appear in the "Write Window" of CANalyzer/CANoe
   // The name will be copied into the timer object, so you are allowed to 
   // delete the string directly after the call of method.
   // Supported by CAPL-on-Board: No
   VIASTDDECL SetName (const char* name) = 0;     // [IN] Name of timer

   // Sets the duration of timer
   // Supported by CAPL-on-Board: Yes 
   VIASTDDECL SetTimer(VIATime nanoseconds) = 0;  // [IN] Duration of timer

   // Stops an active timer
   // Supported by CAPL-on-Board: Yes 
   VIASTDDECL CancelTimer() = 0;
};



// ============================================================================
// Access to CAPL
// ============================================================================

//
// The CAPL function object
// Supported by CAPL-on-Board: No
//
class VIACaplFunction
{
public:
   // Total number of bytes used for all parameters of this CAPL function 
   VIASTDDECL ParamSize  (int32* size) = 0;
   
   // Number of parameters in the signature of this CAPL function
   VIASTDDECL ParamCount (int32* size) = 0;

   // The type of the nth (0, 1, 2 ...) parameter in the signature of this 
   // CAPL function. See file 'cdll.h' for the meaning of the types.
   VIASTDDECL ParamType  (char* type, int32 nth) = 0;
   
   // The type of the return value. See file 'cdll.h' for the meaning of 
   // parameter type. 
   VIASTDDECL ResultType (char* type) = 0;

   // Calls the CAPL function with the given parameters. Params is a pointer to the parameter 
   // list using the standard call calling convention.
   // The return value is given back as a uint32.
   VIASTDDECL Call (uint32* result, void* params) = 0;       
   // Calls the CAPL function with the given parameters. Params is a pointer to the parameter 
   // list using the standard call calling convention.
   // The return value is given back as a double.
   VIASTDDECL CallReturnsDouble (double* result, void* params) = 0;
};



// ============================================================================
// Access to environment variables
// ============================================================================


// Specifies type of an environment variable
enum VIAEnvVarType
{
  kVIA_EVInteger = 0,
  kVIA_EVFloat   = 1,
  kVIA_EVString  = 2,
  kVIA_EVData    = 3
}; 

//
// Callback object for changes of the environment variable
//
class VIAOnEnvVar
{
public:
  // Indicates that the value of the environment variable has changed
  VIASTDDECL OnEnvVar(VIATime nanoseconds, VIAEnvVar* ev) = 0;
};

//
// The environment variable object
// Supported by CAPL-on-Board: No
//
class VIAEnvVar
{
public:
  //Returns the type (see VIAEnvVarType) of the environment variable
  VIASTDDECL GetType(uint32* type) = 0;
  //Returns name of environment variable
  VIASTDDECL GetName(char* buffer, int32 bufferLength) = 0;

  // Sets the callback object of the environment variable (see VIAOnEnvVar)
  // Note: The sink-object will be referenced!
  VIASTDDECL SetSink (VIAOnEnvVar* sink) = 0;

  // Sets/returns the value (integer/double/string/data) of the environment variable
  VIASTDDECL SetInteger(int32  x) = 0; // [IN]  Integer value of EV
  VIASTDDECL GetInteger(int32* x) = 0; // [OUT] Integer value of EV
  VIASTDDECL SetFloat(double  x) = 0;  // [IN]  Float value of EV
  VIASTDDECL GetFloat(double* x) = 0;  // [OUT] Float value of EV
  VIASTDDECL SetString(const char* text) = 0;                       // [IN]  String value of EV
  VIASTDDECL GetString(char* buffer, int32 bufferLength) = 0;       // [OUT] String value of EV [IN] Byte length
  VIASTDDECL GetData(uint8* buffer, int32 bufferLength) = 0;        // [OUT] Data value of EV [IN] Byte length
  VIASTDDECL SetData(const uint8* buffer, int32 bufferLength ) = 0; // [IN]  Data value of EV [IN] Byte length

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 15)
  // Retrieves data size in bytes
  VIASTDDECL GetSize(int32* datasize) = 0;         // [OUT] Data size in bytes
  // Retrieves the data size and a copy of the data.
  VIASTDDECL GetDataAndSize (uint8* buffer,        // [OUT] Copy of Data
                             int32 bufferLength,   // [IN] Byte length for the copy of data
                             int32* datasize) = 0; // [OUT] Data size in bytes
#endif
};



// ============================================================================
// Interface to an ECU
// ============================================================================

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 10)
//
// The ECU object
// Supported by CAPL-on-Board: No
//
class VIAECU
{
public:
   // Returns the name of the ECU
   VIASTDDECL GetName( char* buffer, int32 bufferLength) = 0;

   // Obsolete (As version 1.20): This function is replaced by the attribute retrieval functions of VIANode
   VIASTDDECL GetDBAttributeType( uint32* pType, uint32 objectType, const char* objectName, const char* pAttrName) = 0;
   // Obsolete (As version 1.20): This function is replaced by the attribute retrieval functions of VIANode
   VIASTDDECL GetDBAttributeValue( double* pValue, uint32 objectType, const char* objectName, const char* pAttrName) = 0;
   // Obsolete (As version 1.20): This function is replaced by the attribute retrieval functions of VIANode
   VIASTDDECL GetDBAttributeString( char* pBuffer, int32 bufferLength, uint32 objectType, const char* objectName, const char* pAttrName) = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 39)
   // Returns title of the ECU in simulation setup
   VIASTDDECL GetTitle( char* buffer, int32 bufferLength ) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 58)
   // Returns default title of the ECU in simulation setup
   VIASTDDECL GetDefaultTitle( char* buffer, int32 bufferLength ) = 0;
#endif
};

#endif



// ============================================================================
// Interface to a node
// ============================================================================

//
// The node object
// Supported by CAPL-on-Board: No
//
class VIANode
{
public:
   // Returns the name of the network node
   VIASTDDECL GetName(char* buffer, int32 bufferLength) = 0;

   // Returns a CAPL function handle. The handle stays valid until end of
   // measurement or a call of ReleaseCaplFunction. This method is best
   // called at InitMeasurement. 
   VIASTDDECL GetCaplFunction(VIACaplFunction** caplfct,     // the created handle
                              const char* functionName) = 0;  // name of the function
   // Releases the CAPL function handle received by function GetCaplFunction
   // Call this method at EndMeasurement for every CAPL function handle, that  
   // you have asked for at InitMeasurement.
   VIASTDDECL ReleaseCaplFunction(VIACaplFunction* caplfct) = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 10)

   // Returns the name of the bus this node is attached to
   VIASTDDECL GetBusName( char* pName, uint32 size) = 0;

   // Returns the type of the bus this node is attached to (see enum VIABusInterfaceType)
   VIASTDDECL GetBusType( uint32* interfaceType) = 0;

   // Returns the channel of the bus this node is attached to
   VIASTDDECL GetChannel( VIAChannel* pChannel) = 0;

   // Obsolete (As version 1.20): This function is replaced by the specific attribute retrieval functions further on
   VIASTDDECL GetDBAttributeType( uint32* pType, uint32 objectType, const char* objectName, const char* pAttrName) = 0;
   // Obsolete (As version 1.20): This function is replaced by the specific attribute retrieval functions further on
   VIASTDDECL GetDBAttributeValue( double* pValue, uint32 objectType, const char* objectName, const char* pAttrName) = 0;
   // Obsolete (As version 1.20): This function is replaced by the specific attribute retrieval functions further on
   VIASTDDECL GetDBAttributeString( char* pBuffer, int32 bufferLength, uint32 objectType, const char* objectName, const char* pAttrName) = 0;

   // Returns the ECU this node belongs to. All nodes which belong to the same ECU return the same VIAECU pointer.
   VIASTDDECL GetECU( VIAECU** pECU) = 0;
#endif 

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 11)
   // Obsolete (As version 1.20): The database access has been reworked; nodelayers shouldn't know about databases
   VIASTDDECL GetDatabaseName( char* pBuffer, int32 bufferLength) = 0;
   // Obsolete (As version 1.20): The database access has been reworked; nodelayers shouldn't know about databases
   VIASTDDECL GetDatabasePath( char* pBuffer, int32 bufferLength) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)
   // Retrieves messages which are sent by this node. Other messages can not be retrieved
   // through these functions; Use the message retrieval of VIABus instead.
   VIASTDDECL GetMessage(unsigned long ID, 
                         VIDBMessageDefinition** message) = 0;
   // You should retrieve messages using the ID. Use the name only if the ID is not available.
   VIASTDDECL GetMessage(const char* name, 
                         VIDBMessageDefinition** message) = 0;
   // Iterates over messages
   VIDB_DECLARE_ITERATABLE(MessageDefinition)
   // Gets access to attributes defined in a database: 
   // Returns the attribute / the attribute type / the numeric value / the string value
   VIDB_DECLARE_ATTRIBUTE_HOLDER
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 26)
   // Retrieves the database where the node is defined
   VIASTDDECL GetDBNode(VIDBNodeDefinition** nodeDefinition) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 51)
   VIASTDDECL OpenNodePanel(uint32 open) = 0;
#endif
};



// ============================================================================
// Interface to message and signal definitions
// ============================================================================

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 47)
// Specifies type of a signal
enum VIASignalValueType
{
  kVIA_SigValueTypeInteger = 0,
  kVIA_SigValueTypeFloat   = 1,
  kVIA_SigValueTypeDouble  = 2,
  kVIA_SigValueTypeData    = 3
}; 
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 86)
// Specifies the Transfer Property of a signal
enum VIASignalTransferProperty
{
  kVIA_TransferProperty_Undefined = 0,
  kVIA_TransferProperty_Pending = 1,
  kVIA_TransferProperty_Triggered = 2,
  kVIA_TransferProperty_TriggeredOnChange = 3,
  kVIA_TransferProperty_TriggeredOnChangeWithoutRepetition = 4,
  kVIA_TransferProperty_TriggeredWithoutRepetition = 5
};
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 26)
//
// Definition of a signal group
// Supported by CAPL-on-Board: No
//
class VIDBSignalGroup
{
public:
    // Returns the name of the signal group
    VIASTDDECL GetName(char* buffer, int32 bufferLength) const = 0;
    // Returns the comment to the signal group
    VIASTDDECL GetComment(char* buffer, int32 bufferLength) const = 0;
    // Iterates over signals
    VIDB_DECLARE_ITERATABLE(SignalDefinition)
    VIASTDDECL Release() = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 83)
    // Access to End2End protection info of signal group
    VIASTDDECL GetEnd2EndProtectionInfo(VIDBEnd2EndProtectionInfo** e2eProtection) const = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 86)
    // Returns signal group's Transfer Property (see VIASignalTransferProperty)
    VIASTDDECL GetTransferProperty(uint32& type) const = 0;

    // Returns position of signal group's Update Bit
    VIASTDDECL GetUpdateBitPosition(int32& pos) const = 0;

    // Returns length of the signal group in bits
    VIASTDDECL GetLength(uint32& length) const = 0;

    // Returns signal group's position in bits
    VIASTDDECL GetPosition(uint32& pos) const = 0;
#endif
};
#endif

//
// Definition of a message. Through the message, you can access the signals it
// contains. You can get the messages through VIANode or the VIABus subclasses.
// Starting from version 1.26, you can get the message also through  
// the VIDBNodeDefinition and the VIDBDatabase.
// Supported by CAPL-on-Board: No
//
class VIDBMessageDefinition
{
public:
   // Returns the ID of the message
   VIASTDDECL GetID(unsigned long& ID) const = 0;
   // Returns the name of message
   VIASTDDECL GetName(char* buffer, int32 bufferLength) const = 0;
   // Returns the DLC of the message
   VIASTDDECL GetDLC(unsigned short& dlc) const = 0;
   // Returns the signal with specified name. signal is set to NULL if no such 
   // signal is found.
   VIASTDDECL GetSignal(const char* name, 
                        VIDBSignalDefinition** signal) = 0;
   // Iterates over signals
   VIDB_DECLARE_ITERATABLE(SignalDefinition)
   // Gets access to attributes defined in a database: 
   // Returns the attribute / the attribute type / the numeric value / the string value
   VIDB_DECLARE_ATTRIBUTE_HOLDER
   VIASTDDECL Release() = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 26)
   // Iterates over transmitter nodes
   VIDB_DECLARE_ITERATABLE(NodeDefinition)
   // Iterates over signal groups
   VIDB_DECLARE_ITERATABLE(SignalGroup)
   // Retrieves the database where the message is defined
   VIASTDDECL GetDatabase(VIDBDatabase** database) = 0;
#endif
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 43)
   // Iterates over rx nodes
   VIASTDDECL GetRxNodeDefinitionIterator(VIDBNodeDefinitionIterator** iterator) = 0;
#endif
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 80)
   // Get AUTOSAR PDU definition
   VIASTDDECL IsAutosarPDU(bool* isAutosarPDUV) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 93)
   VIDB_DECLARE_ITERATABLE(MappedPDUDefinition)
#endif
};

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 63)
//
// Definition of a multiplexer range. 
// Supported by CAPL-on-Board: No
//
class VIDBMultiplexerRange
{
public:
  // Returns the lower bound of a multiplexer range
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetLowerBound(uint32& lowerBound) const = 0;
  // Returns the upper bound of a multiplexer range
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetUpperBound(uint32& upperBound) const = 0;
  // Returns the name of a multiplexer range
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetSubFrameName(char* buffer, int32 bufferLength) const = 0;
  // Supported by CAPL-on-Board: No
  VIASTDDECL Release() = 0;
};
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 72)
//
// Definition of a value table entry. 
// Supported by CAPL-on-Board: No
//
class VIDBValueTableEntry
{
public:
  // Returns the lower bound of a value table entry
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetLowerBound(::int64& lowerBound) const = 0;
  // Returns the upper bound of a value table entry
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetUpperBound(::int64& upperBound) const = 0;
  // Returns the textual encoding name of a table entry
  // Supported by CAPL-on-Board: No
  VIASTDDECL GetTextualEncoding(char* buffer, int32 bufferLength) const = 0;
  // Supported by CAPL-on-Board: No
  VIASTDDECL Release() = 0;
};
#endif

//
// Definition of a signal. Apart from attribute retrieval, it also provides
// functions to access the value of the signal in a real message.
// Supported by CAPL-on-Board: Yes
//
class VIDBSignalDefinition
{
public:
    // Returns the name of signal
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetName(char* buffer, int32 bufferLength) const = 0;
    
    // Returns minimum values as defined in database
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetMinValue(double& minValue) const = 0;

    // Returns maximum values as defined in database
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetMaxValue(double& maxValue) const = 0;

    // Returns the physical value as double.
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetPhysicalValue(unsigned char* rawData, 
                                double& physValue) const = 0;

    // Returns the raw value as unsigned 64bit integer
    // This method can be used for signals with IEEE float or
    // double representation only if the bit field is already given
    // in the parameter. If you need to get the floating-point raw
    // value, use GetRawValueByDouble.
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetRawValue(unsigned char* rawData, 
                           ::uint64& rawValue) const = 0;

    // Sets the signal value with the physical value
    // Supported by CAPL-on-Board: Yes
    VIASTDDECL SetPhysicalValue(double physValue, 
                                unsigned char* rawData) const = 0;

    // This method can be used for signals with IEEE float or
    // double representation only if the bit field is already given
    // in the parameter. If you need to set the floating-point raw
    // value, use SetRawValueByDouble.
    // Supported by CAPL-on-Board: Yes
    VIASTDDECL SetRawValue(::uint64 rawValue, 
                           unsigned char* rawData) const = 0;

    // Gets access to attributes defined in a database: 
    // Returns the attribute / the attribute type / the numeric value / the string value
    // Supported by CAPL-on-Board: No
    VIDB_DECLARE_ATTRIBUTE_HOLDER

    // Supported by CAPL-on-Board: No
    VIASTDDECL Release() = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 24)
    // This method can be used to get the raw value for all types of signals.
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetRawValueByDouble(unsigned char* rawData, 
                                   double& rawValue) const = 0;

    // This method can be used to set the raw value for all types of signals.
    // Supported by CAPL-on-Board: No
    VIASTDDECL SetRawValueByDouble(double rawValue, 
                                   unsigned char* rawData) const = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 26)
    // Returns comment of the signal
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetComment(char* buffer, int32 bufferLength) const = 0;

    // Iterates over receiver nodes
    // Supported by CAPL-on-Board: No
    VIDB_DECLARE_ITERATABLE(NodeDefinition)
    
    // Returns length of the signal in bits
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetLength(uint32& length) const = 0;
    
    // Returns signal position in bits
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetPosition(uint32& pos) const = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 36)
    // This methods can be used to get the raw values for signals > 64 bit.
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetRawValueByteArray(unsigned char* rawData, uint32 rawDataLength, 
                                    unsigned char* rawValue, uint32 rawValueLength) = 0;

    // This methods can be used to set the raw values for signals > 64 bit.
    // Supported by CAPL-on-Board: No
    VIASTDDECL SetRawValueByteArray(unsigned char* rawValue, uint32 rawValueLength, 
                                    unsigned char* rawData, uint32 rawDataLength) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 47)
    // Returns signal's type (see VIASignalValueType)
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetValueType(uint32& type) const = 0;

    // Returns signal's byte order (only valid if GetLength() > 8)
    //   order == 0: Intel/low-high-byte-order
    //   order == 1: Motorola/high-low-byte-order
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetByteOrder(uint32& order) const = 0;

    // Returns signed attribute (only valid if GetValueType() == kVIA_SigValueTypeInteger)
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetIsSigned(bool& isSigned) const = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 63)
    // Returns whether this signal is a multiplexer
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetIsMultiplexer(bool& isMultiplexer) const = 0;

    // Returns whether this signal is a multiplexed signal
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetIsMultiplexed(bool& isMultiplexed) const = 0;

    // Iterates over all multiplexed signals (only valid if this signal is a multiplexer)
    // Supported by CAPL-on-Board: No
    VIDB_DECLARE_ITERATABLE_ALIAS(MultiplexedSignals, SignalDefinition)

    // Iterates over all multiplexer value ranges (only valid if this signal is multiplexed)
    // Supported by CAPL-on-Board: No
    VIDB_DECLARE_ITERATABLE_ALIAS(MultiplexerValueRanges, MultiplexerRange)

    // Returns the multiplexer signal (only valid if this signal is multiplexed)
    // The *aMultiplexerSignal must be released!
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetMultiplexerSignal(VIDBSignalDefinition** aMultiplexerSignal) const = 0;

    // Returns whether this signal is multiplexed by the given value for its multiplexer signal (only valid if this signal is multiplexed)
    // Supported by CAPL-on-Board: No
    VIASTDDECL IsMultiplexedByValue(uint32 value, bool& isMultiplexed) const = 0;

    // Returns whether this signal is currently valid in the rawData according to the multiplexer value in the rawData (only valid if this signal is multiplexed)
    // Supported by CAPL-on-Board: No
    VIASTDDECL IsValidMultiplexedSignal(unsigned char* rawData, uint32 rawValueLength, bool& isValidMultiplexedSignal) const = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 72)
    // Returns whether this signal has a value table
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetHasValueTable(bool& hasValueTable) const = 0;

    // Iterates over all value table entries (only valid if there is a value table)
    // Supported by CAPL-on-Board: No
    VIDB_DECLARE_ITERATABLE_ALIAS(ValueTableEntries, ValueTableEntry)
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 86)
    // Returns signal's Transfer Property (see VIASignalTransferProperty)
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetTransferProperty(uint32& type) const = 0;

    // Returns position of signal's Update Bit
    // Supported by CAPL-on-Board: No
    VIASTDDECL GetUpdateBitPosition(int32& pos) const = 0;
#endif
};

//
// Definition of a node. This is only meant for some special nodelayers which 
// must have access to the attributes of other nodes on the bus. For the 
// node corresponding to the nodelayer itself, use VIANode directly. You can
// get the NodeDefinitions through the VIABus subclasses (e.g. VIACan).
// Supported by CAPL-on-Board: No
//
class VIDBNodeDefinition
{
public:
    // Returns the name of node
    VIASTDDECL GetName(char* buffer, int32 bufferLength) const = 0;
    // Gets access to attributes defined in a database: 
    // Returns the attribute / the attribute type / the numeric value / the string value
    VIDB_DECLARE_ATTRIBUTE_HOLDER
    VIASTDDECL Release() = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 26)
    // Iterates over tx messages
    VIDB_DECLARE_ITERATABLE(MessageDefinition)
    // Retrieves the database where the node definition is defined
    VIASTDDECL GetDatabase(VIDBDatabase** database) = 0;
#endif
};

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 26)
//
// Access to all messages or nodes in one database. You can iterate
// over the databases themselves through the VIAService.
// Supported by CAPL-on-Board: No
//

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 82)
enum VIADBVersionInformation
{
	  kVIA_FormatVersionUnspec = -1
	, kVIA_FIBEXVersion_FIBEXplus = 0
	, kVIA_FIBEXVersion_1_1 = 1
	, kVIA_FIBEXVersion_1_2 = 2
	, kVIA_FIBEXVersion_2_0 = 3
	, kVIA_FIBEXVersion_3_0 = 4
	, kVIA_FIBEXVersion_3_1 = 5
	, kVIA_FIBEXVersion_4_0 = 6
	, kVIA_FIBEXVersion_4_1 = 7
	, kVIA_FormatVersion_AUTOSAR_3_1 = 208
	, kVIA_FormatVersion_AUTOSAR_3_2 = 209
	, kVIA_FormatVersion_AUTOSAR_4_0 = 210
	, kVIA_FormatVersion_AUTOSAR_4_1 = 211
	, kVIA_FormatVersion_AUTOSAR_4_2 = 212
	, kVIA_DBC = 100001

};
#endif
class VIDBDatabase
{
public:
    // Returns the name of the database
    VIASTDDECL GetName(char* buffer, int32 bufferLength) const = 0;
    // Returns the channel where the database is assigned
    VIASTDDECL GetChannel(VIAChannel& channel) const = 0;
    // Returns the bus type
    VIASTDDECL GetBusType(VIABusInterfaceType& type) const = 0;
    // Gets access to attributes defined in a database: 
    // Returns the attribute / the attribute type / the numeric value / the string value
    VIDB_DECLARE_ATTRIBUTE_HOLDER
    // Iterates over messages
    VIDB_DECLARE_ITERATABLE(MessageDefinition)
    // Iterates over nodes
    VIDB_DECLARE_ITERATABLE(NodeDefinition)
    VIASTDDECL Release() = 0;
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 82)
    VIASTDDECL GetVersionInformation(VIADBVersionInformation& type) const = 0;
#endif
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 90)
    VIASTDDECL GetAUTOSARDescriptionInfo(VIDBAUTOSARDescriptionInfo** descriptionInfo) const = 0;
#endif
};
#endif

// explicit template specializations
template class VIDBIterator<VIDBAttribute>;
template class VIDBIterator<VIDBMessageDefinition>;
template class VIDBIterator<VIDBSignalDefinition>;
template class VIDBIterator<VIDBNodeDefinition>;
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 26)
template class VIDBIterator<VIDBSignalGroup>;
template class VIDBIterator<VIDBDatabase>;
#endif

#endif // minor version >= 20

// ============================================================================
// Access to system variables
// ============================================================================

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 25)

// Specifies type of a system variable
enum VIASysVarType
{
  kVIA_SVInteger = 0,
  kVIA_SVFloat   = 1,
  kVIA_SVString  = 2,
  kVIA_SVIntegerArray = 3,
  kVIA_SVFloatArray = 4,
  kVIA_SVData = 5,
  kVIA_SVStruct = 6,
  kVIA_SVGenericArray = 7,
  kVIA_SVUnion = 8
}; 

//
// The callback object for changes of the system variable
// Supported by CAPL-on-Board: No
//
class VIAOnSysVar
{
public:
  // Indicates that the value of the system variable has changed
  VIASTDDECL OnSysVar(VIATime nanoseconds, VIASystemVariable* ev) = 0;
};

// Used to identify the owner of a system variable
typedef void * VIASysVarClientHandle;

#if defined (VIAMinorVersion) && ( VIAMinorVersion >= 50)

class VIASystemVariableMember;

//
// The callback object for changes of a system variable member
//
class VIAOnSysVarMember
{
public:
  // Indicates that the value of the system variable member has changed
  VIASTDDECL OnSysVarMember(VIATime nanoseconds, VIASystemVariableMember* ev, VIASysVarClientHandle origin) = 0;
};

// Byte order for system variable members. Default is Intel byte order.
enum VIAByteOrder
{
  kVIA_ByteOrderIntel = 0,
  kVIA_ByteOrderMotorola = 1
};

// Supported encodings for strings in system variable members. Default is UTF8.
enum VIAStringEncoding
{
  kVIA_UTF8 = 65001,
  kVIA_UTF16 = 1200,
  kVIA_ISO88591 = 28591,
  kVIA_ISO88592 = 28592,
  kVIA_WINDOWS1252 = 1252
};

// Member of a system variable of type struct or generic array
class VIASystemVariableMember
{
public:
  // Release does not remove the variable member; it just deletes the instance. After
  // release, the sink will not be called any more.
  VIASTDDECL Release() = 0;

  //Returns the type (see VIASysVarType) of the system variable member
  VIASTDDECL GetType(VIASysVarType* type) = 0;
  //Returns the name of the system variable member
  VIASTDDECL GetName(char* buffer, int32 bufferLength) = 0;

  // Returns the length in bits
  // for integer arrays and float arrays, returns the length of a single array element
  // for generic arrays, returns the length of the array
  VIASTDDECL GetBitLength(int32* length) = 0;

  // Returns the offset to the previous member of the beginning of the struct, in bits
  // Returns always 0 for generic array elements
  VIASTDDECL GetRelativeBitOffset(int32* offset) = 0;

  // Returns whether the member is of a signed data type
  VIASTDDECL IsSigned(bool* isSigned) = 0;

  // Returns the byte order of the member
  // Fails for generic array elements (their byte order is set by the parent element)
  VIASTDDECL GetByteOrder(VIAByteOrder* byteOrder) = 0;

  // Sets the callback object of the system variable (see VIAOnSysVarMember)
  // Note: The sink-object will be referenced!
  VIASTDDECL SetSink(VIAOnSysVarMember* sink) = 0;

  // Get Functions return kVIA_Failed if the data type is wrong
  // Set Functions return kVIA_Failed if either the data type is wrong
  // or you don't have the right to change the value (of a read-only variable).
  // All these functions are for the raw value of the member.
  VIASTDDECL SetInteger(::int64  x, VIASysVarClientHandle client) = 0; // [IN] Integer value of SV [IN] Owner of SV
  VIASTDDECL GetInteger(::int64* x) = 0;                               // [OUT] Integer value of SV

  VIASTDDECL SetFloat(double  x, VIASysVarClientHandle client) = 0;  // [IN] Float value of SV [IN] Owner of SV
  VIASTDDECL GetFloat(double* x) = 0;                                // [OUT] Float value of SV

  // string values are always given in UTF-8 encoding, even if they are in another encoding in the struct
  VIASTDDECL SetString(const char* text, VIASysVarClientHandle client) = 0; // [IN] String value of SV  [IN] Owner of SV
  VIASTDDECL GetString(char* buffer, int32 bufferLength) = 0;               // [OUT] String value of SV [IN] Buffer length

  VIASTDDECL SetIntegerArray(const ::int64* x, int32 length, VIASysVarClientHandle client) = 0; // [IN] Integer array [IN] length of array [IN] Owner of SV
  VIASTDDECL GetIntegerArray(::int64* x, int32 length) = 0;                                     // [OUT] Integer array [IN] length of array

  VIASTDDECL SetFloatArray(const double* x, int32 length, VIASysVarClientHandle client) = 0; // [IN] Float array [IN] length of array [IN] Owner of SV
  VIASTDDECL GetFloatArray(double* x, int32 length) = 0;                                     // [OUT] Float array [IN] length of array

  // used for type struct and generic array
  VIASTDDECL SetData(const uint8* x, int32 length, VIASysVarClientHandle client) = 0; // [IN] byte array [IN] length of array [IN] Owner of SV
  VIASTDDECL GetData(uint8* buffer, int32 length) = 0;        // [OUT] byte array [IN] length of buffer

  VIASTDDECL GetArraySize(int32* size) = 0;

  // Returns the name for a value; returns kVIA_Failed if no name is defined for the value
  VIASTDDECL GetSymbolicValueName(::int64 value, char* buffer, int32 bufferLength) = 0;

  // physical value for scalar members
  VIASTDDECL SetPhysicalValue(double value, VIASysVarClientHandle client) = 0;
  VIASTDDECL GetPhysicalValue(double* value) = 0;
  // physical value for array members
  VIASTDDECL SetPhysicalValues(const double* values, int32 length, VIASysVarClientHandle client) = 0;
  VIASTDDECL GetPhysicalValues(double* values, int32 length) = 0;
  // note: only int and int array members can have a linear encoding (physical value)

  VIASTDDECL GetStringEncoding(VIAStringEncoding* encoding) = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 77)
  // for optional members or members in unions: returns whether the member is currently valid
  VIASTDDECL IsValueValid(bool* isValid) = 0;
  // for optional members: sets whether the member is currently valid
  VIASTDDECL SetValueValid(bool isValid, VIASysVarClientHandle client) = 0;
  // for members which are variable length arrays: set the current length of the array
  VIASTDDECL SetArrayLength(int32 length, VIASysVarClientHandle client) = 0;
#endif
};

// Used to set additional parameters for a struct member or array element.
// Members (aside from Release) can only be called before the struct definition is committed 
// by calling DefineStruct on it.
class VIASVStructMemberDefinition
{
public:
  // Release does not remove the variable member; it just deletes the instance. 
  VIASTDDECL Release() = 0;

  VIASTDDECL SetByteOrder(VIAByteOrder byteOrder) = 0; // not valid for array elements

  VIASTDDECL SetRelativeBitOffset(int32 offset) = 0; // not valid for array elements

  // Minimum and maximum are optional.
  VIASTDDECL SetPhysicalRange(const double* minValue, const double* maxValue) = 0;

  // For value tables
  // Sets the name for a value; replaces a potentially existing name for the value
  VIASTDDECL SetSymbolicValueName(::int64 value, const char* name) = 0;

  // Adds a linear encoding. Linear encodings must not overlap. Minimum, Maximum and unit are optional.
  VIASTDDECL AddLinearEncoding(double factor, double offset, const ::int64* minValue, const ::int64* maxValue, const char* unit) = 0;

  VIASTDDECL SetComment(const char* comment) = 0;

  VIASTDDECL SetStringEncoding(VIAStringEncoding encoding) = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 77)
  // optional members are only allowed in structs which have no defined binary layout
  VIASTDDECL SetIsOptional(bool isOptional) = 0;
  // hidden members are only allowed in structs which have no defined binary layout
  VIASTDDECL SetIsHidden(bool isHidden) = 0;
#endif
};

// Used to define the elements of a generic array in a system variable (struct).
// Retrieve an instance from a namespace; use the definition in the appropriate Define/AddVariable/AddMember method.
// One of the Define...-Methods must be called before each usage in a Define/AddVariable/AddMember method.
class VIASVArrayElementDefinition
{
public:
  VIASTDDECL Release() = 0;

  VIASTDDECL DefineAsInt(const ::int64* initialValue, int bitLength, bool isSigned, VIASVStructMemberDefinition** memberDefinition) = 0;
  VIASTDDECL DefineAsFloat(const double* initialValue, int bitLength, VIASVStructMemberDefinition** memberDefinition) = 0;
  VIASTDDECL DefineAsIntArray(const ::int64* initialValues, int32 arrayLength, int32 bitLength, bool isSigned, VIASVStructMemberDefinition** memberDefinition) = 0;
  VIASTDDECL DefineAsFloatArray(const double* initialValues, int32 arrayLength, int32 bitLength, VIASVStructMemberDefinition** memberDefinition) = 0;
  VIASTDDECL DefineAsString(int32 maxLength, const char* initialValue, VIASVStructMemberDefinition** memberDefinition) = 0; 
  VIASTDDECL DefineAsGenericArray(int32 arrayLength, VIASVArrayElementDefinition* arrayElementDefinition, VIASVStructMemberDefinition** memberDefinition) = 0;
  VIASTDDECL DefineAsStruct(const char* structPath, int32 bitCount, VIASVStructMemberDefinition** memberDefinition) = 0;
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 77)
  VIASTDDECL DefineAsVariableLengthArray(int32 arrayLength, int32 minLength, int32 maxLength, VIASVArrayElementDefinition* arrayElementDefinition, VIASVStructMemberDefinition** memberDefinition) = 0;
#endif
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 85)
  VIASTDDECL DefineAsData(const ::uint8* initialValue, int32 byteCount, VIASVStructMemberDefinition** memberDefinition) = 0;
#endif

};

// Used to define a struct.
class VIASVStructDefinition
{
public:
  // Release does not remove the struct; it just deletes the instance. 
  VIASTDDECL Release() = 0;

  // Define commits the struct definition to the runtime. Afterwards, the struct
  // can't be changed any more.
  VIASTDDECL Define() = 0;

  // must not be called after Define()
  VIASTDDECL SetComment(const char* comment) = 0;

  // Members can only be added before Define() is called.
  // Initial values are optional and can be NULL
  // The last parameter is optional; if non-null, you can use it for additional member parameters.
  VIASTDDECL AddIntMember(const char* name, const ::int64* initialValue, int32 bitLength, bool isSigned, VIASVStructMemberDefinition** memberDefinition) = 0;
  VIASTDDECL AddFloatMember(const char* name, const double* initialValue, int32 bitLength, VIASVStructMemberDefinition** memberDefinition) = 0; // only bit lengths 32 and 64 are supported
  VIASTDDECL AddIntArrayMember(const char* name, const ::int64* initialValues, int32 arrayLength, int32 bitLength, bool isSigned, VIASVStructMemberDefinition** memberDefinition) = 0;
  VIASTDDECL AddFloatArrayMember(const char* name, const double* initialValues, int32 arraylength, int32 bitlength, VIASVStructMemberDefinition** memberDefinition) = 0;
  VIASTDDECL AddStringMember(const char* name, int32 maxLength, const char* initialValue, VIASVStructMemberDefinition** memberDefinition) = 0;
  VIASTDDECL AddGenericArrayMember(const char* name, int32 arrayLength, VIASVArrayElementDefinition* arrayElementDefinition, VIASVStructMemberDefinition** memberDefinition) = 0;
  // the struct path is [NamespaceName::]*StructName
  VIASTDDECL AddStructMember(const char* name, const char* structPath, int32 bitCount, VIASVStructMemberDefinition** memberDefinition) = 0;

  // must only be called after Define(). Utility to use the bit count in further references to the structs (e.g. AddStructMember).
  VIASTDDECL GetBitCount(int32* bitCount) = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 77)
  // must not be called after Define()
  VIASTDDECL SetHasDefinedBinaryLayout(bool definedBinaryLayout) = 0;
  VIASTDDECL SetIsUnion(bool isUnion) = 0;
  VIASTDDECL AddVariableLengthArrayMember(const char* name, int32 arrayLength, int32 minLength, int32 maxLength, VIASVArrayElementDefinition* arrayElementDefinition, VIASVStructMemberDefinition** memberDefinition) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 85)
  VIASTDDECL AddDataMember(const char* name, const ::uint8* initialValues, int32 byteCount, VIASVStructMemberDefinition** memberDefinition) = 0;
#endif
};

#endif // VIAMinorVersion 50

//
// System variables are accessed through the namespace
// Supported by CAPL-on-Board: No
//
class VIANamespace
{
public:
  // Release does not automatically release or remove subnamespaces and variables
  VIASTDDECL Release() = 0;

  // Retrieves the name of the namespace
  VIASTDDECL GetName(char* buffer, int32 bufferLength) = 0;
  // Retrieves a subnamespace of the namespace. The path may contain several
  // hierarchies, separated with '::'.
  VIASTDDECL GetNamespace(const char* path, VIANamespace*& nameSpace) = 0;
  // Retrieves a variable in the namespace. The path may contain subnamespaces,
  // separated with '::'.
  VIASTDDECL GetVariable(const char* path, VIASystemVariable*& variable) = 0;

  // Adds a new namespace. The added namespace is returned immediately. If the
  // namespace already exists, it is not added, but returned; this is no error.
  VIASTDDECL AddNamespace(const char* name, VIANamespace*& nameSpace) = 0;
  // Adds a new variable. The added variable is returned immediately. If the
  // variable already exists, it is not added, NULL is returned in the parameter,
  // and the function returns kVIA_ObjectCreationFailed. The same holds for
  // variables in a reserved namespace (xcp, ccp, vts).
  // If read only, only the nodelayer instance which created the variable can
  // change its value.
  VIASTDDECL AddVariable(const char* name, VIASysVarType type, bool readOnly,
                         VIASysVarClientHandle client,
                         VIASystemVariable*& variable) = 0;
  // Use this version for IntegerArray or FloatArray; it includes the array length
  VIASTDDECL AddArrayVariable(const char* name, VIASysVarType type, uint32 arrayLength, bool readOnly,
                              VIASysVarClientHandle client,
                              VIASystemVariable*& variable) = 0;

  // Removes a variable. This must be a variable which has been created by the
  // same client as is given in the second parameter; otherwise kVIA_Failed 
  // is returned.
  VIASTDDECL RemoveVariable(const char* name, VIASysVarClientHandle client) = 0;
  // Removes a namespace. All variables in the namespace must have been created
  // by the same client as is given in the second parameter; otherwise 
  // kVIA_Failed is returned.
  VIASTDDECL RemoveNamespace(const char* name, VIASysVarClientHandle client) = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 37)
  VIASTDDECL SetComment(const char* comment) = 0;
#endif
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 45)
  VIASTDDECL AddIntVariableWithInitialValue       (const char* name, int32 initialValue, bool readOnly, 
                                                   VIASysVarClientHandle client, VIASystemVariable*& variable) = 0;
  VIASTDDECL AddFloatVariableWithInitialValue     (const char* name, double initialValue, bool readOnly, 
                                                   VIASysVarClientHandle client, VIASystemVariable*& variable) = 0;
  VIASTDDECL AddStringVariableWithInitialValue    (const char* name, const char* initialValue, bool readOnly, 
                                                   VIASysVarClientHandle client, VIASystemVariable*& variable) = 0;
  VIASTDDECL AddIntArrayVariableWithInitialValue  (const char* name, const int32* initialValue, int32 length, bool readOnly, 
                                                   VIASysVarClientHandle client, VIASystemVariable*& variable) = 0;
  VIASTDDECL AddFloatArrayVariableWithInitialValue(const char* name, const double* initialValue, int32 length, bool readOnly, 
                                                   VIASysVarClientHandle client, VIASystemVariable*& variable) = 0;
  VIASTDDECL AddDataVariableWithInitialValue      (const char* name, const uint8* initialValue, int32 length, bool readOnly, 
                                                   VIASysVarClientHandle client, VIASystemVariable*& variable) = 0;
#endif

#if defined (VIAMinorVersion) && ( VIAMinorVersion >= 50)
  // The struct is only defined after Define() is called on the new structDefinition.
  // That allows to add members and set additional parameters.
  VIASTDDECL AddStruct(const char* name, VIASysVarClientHandle client, VIASVStructDefinition** structDefinition) = 0;
  // struct path is [Namespace::]*StructName
  VIASTDDECL AddStructVariable(const char* name, const char* structPath, bool readOnly, VIASysVarClientHandle client, VIASystemVariable*& variable) = 0;
  VIASTDDECL AddGenericArrayVariable(const char* name, int32 arrayLength, VIASVArrayElementDefinition* arrayElementDefinition, bool readOnly, VIASysVarClientHandle client, VIASystemVariable*& variable) = 0;
  VIASTDDECL CreateArrayElementDefinition(VIASVArrayElementDefinition** arrayElementDefinition) = 0;
#endif
#if defined (VIAMinorVersion) && ( VIAMinorVersion >= 65)
  VIASTDDECL AddIntVariable(const char* name, bool isSigned, bool is64bit, bool readOnly,
                         VIASysVarClientHandle client,
                         VIASystemVariable*& variable) = 0;
  VIASTDDECL AddIntVariableWithInitialValueEx     (const char* name, ::int64 initialValue, bool isSigned, bool is64bit, bool readOnly, 
                                                   VIASysVarClientHandle client, VIASystemVariable*& variable) = 0;  
#endif
#if defined (VIAMinorVersion) && ( VIAMinorVersion >= 66)
  VIASTDDECL AddStringVariableWithInitialValueEx(const char* name, const char* initialValue, uint32 codepage, bool readOnly,
    VIASysVarClientHandle client, VIASystemVariable*& variable) = 0;
#endif

#if defined ( VIAMinorVersion ) && ( VIAMinorVersion >= 67)
  // Extension of interface method AddStructVariable().It allows for initialization of specific members with values 
  // which are independent of generic start values defined in the corresponding struct definition. 
  VIASTDDECL AddStructVariableWithMemberInitValues(const char* name, const char* structPath, bool readOnly, VIASysVarClientHandle client, 
                                                     VIASystemVariableMemberInitValues* memberInitValues, VIASystemVariable*& variable) = 0;
  // Extension of interface method AddGenericArrayVariable(). It allows for initialization of specific elements with values 
  // which are independent of generic start values defined in the corresponding element prototype. 
  VIASTDDECL AddGenericArrayVariableWithMemberInitValues(const char* name, int32 arrayLength, VIASVArrayElementDefinition* arrayElementDefinition, bool readOnly, 
                                                           VIASysVarClientHandle client, VIASystemVariableMemberInitValues* memberInitValues, VIASystemVariable*& variable) = 0;

  // Create and release the interface for managing member initialization values.
  VIASTDDECL CreateMemberInitValues(VIASystemVariableMemberInitValues*& memberInitValues) = 0;
  VIASTDDECL ReleaseMemberInitValues(VIASystemVariableMemberInitValues* memberInitValues) = 0;
#endif
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 77)
  VIASTDDECL AddGenericArrayVariableWithoutBinaryLayout(const char* name, int32 arrayLength, VIASVArrayElementDefinition* arrayElementDefinition, bool readOnly, VIASysVarClientHandle client,VIASystemVariableMemberInitValues* memberInitValues, VIASystemVariable*& variable) = 0;
  VIASTDDECL AddVariableLengthArrayVariable(const char* name, int32 arrayLength, int32 minLength, int32 maxLength, VIASVArrayElementDefinition* arrayElementDefinition, bool readOnly, VIASysVarClientHandle client, VIASystemVariableMemberInitValues* memberInitValues, VIASystemVariable*& variable) = 0;
#endif
};

//
// The system variable object
// Supported by CAPL-on-Board: No
//
class VIASystemVariable
{
public:
  // Release does not remove the variable; it just deletes the instance. After
  // release, the sink will not be called any more.
  VIASTDDECL Release() = 0;

  //Returns the type (see VIASysVarType) of the system variable
  VIASTDDECL GetType(VIASysVarType* type) = 0;
  //Returns the name of the system variable
  VIASTDDECL GetName(char* buffer, int32 bufferLength) = 0;

  // Sets the callback object of the system variable (see VIAOnSysVar)
  // Note: The sink-object will be referenced!
  VIASTDDECL SetSink(VIAOnSysVar* sink) = 0;

  // Get Functions return kVIA_Failed if the data type is wrong
  // Set Functions return kVIA_Failed if either the data type is wrong
  // or you don't have the right to change the value (of a read-only variable).
  VIASTDDECL SetInteger(int32  x, VIASysVarClientHandle client) = 0; // [IN] Integer value of SV [IN] Owner of SV
  VIASTDDECL GetInteger(int32* x) = 0;                               // [OUT] Integer value of SV

  VIASTDDECL SetFloat(double  x, VIASysVarClientHandle client) = 0;  // [IN] Float value of SV [IN] Owner of SV
  VIASTDDECL GetFloat(double* x) = 0;                                // [OUT] Float value of SV

  VIASTDDECL SetString(const char* text, VIASysVarClientHandle client) = 0; // [IN] String value of SV  [IN] Owner of SV
  VIASTDDECL GetString(char* buffer, int32 bufferLength) = 0;               // [OUT] String value of SV [IN] Buffer length

  VIASTDDECL SetIntegerArray(const int32* x, int32 length, VIASysVarClientHandle client) = 0; // [IN] Integer array [IN] length of array [IN] Owner of SV
  VIASTDDECL GetIntegerArray(int32* x, int32 length) = 0;                                     // [OUT] Integer array [IN] length of array

  VIASTDDECL SetFloatArray(const double* x, int32 length, VIASysVarClientHandle client) = 0; // [IN] Float array [IN] length of array [IN] Owner of SV
  VIASTDDECL GetFloatArray(double* x, int32 length) = 0;                                     // [OUT] Float array [IN] length of array

  VIASTDDECL GetArraySize(int32* size) = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 26)
  VIASTDDECL IsReadOnly(int32* readOnly) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 37)
  VIASTDDECL SetComment(const char* comment) = 0;

  // For value tables
  // Sets the name for a value; replaces a potentially existing name for the value
  VIASTDDECL SetSymbolicValueName(int32 value, const char* name) = 0;
  // Returns the name for a value; returns kVIA_Failed if no name is defined for the value
  VIASTDDECL GetSymbolicValueName(int32 value, char* buffer, int32 bufferLength) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 42)
  // used for types data and struct
  VIASTDDECL SetData(const uint8* x, int32 length, VIASysVarClientHandle client) = 0; // [IN] byte array [IN] length of array [IN] Owner of SV
  VIASTDDECL GetData(uint8* buffer, int32 bufferSize, int32* copiedBytes) = 0;        // [OUT] byte array [IN] length of buffer [OUT] number of copied bytes
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 45)
  VIASTDDECL SetUnit(const char* unit) = 0;
#endif

#if defined (VIAMinorVersion) && ( VIAMinorVersion >= 50)
  // member path is everything after the first '.' or including the first '['
  VIASTDDECL GetMember(const char* memberPath, VIASystemVariableMember** member) = 0;
  // for atomic updates of several members (with just one variable change),
  // use BeginStructUpdate / EndStructUpdate
  VIASTDDECL BeginStructUpdate(VIASysVarClientHandle client) = 0;
  VIASTDDECL EndStructUpdate() = 0;
#endif

#if defined (VIAMinorVersion) && ( VIAMinorVersion >= 65)
  // Returns whether the variable is of a signed data type
  VIASTDDECL IsSigned(bool* isSigned) = 0;
  // can be used for all integer types
  VIASTDDECL SetIntegerEx(::int64  x, VIASysVarClientHandle client) = 0; // [IN] Integer value of SV [IN] Owner of SV
  VIASTDDECL GetIntegerEx(::int64* x) = 0;                               // [OUT] Integer value of SV
#endif

#if defined (VIAMinorVersion) && ( VIAMinorVersion >= 66)
  // set / get string value in a particular encoding
  VIASTDDECL SetStringWithEncoding(const char* text, VIASysVarClientHandle client, uint32 codepage) = 0; // [IN] String value of SV  [IN] Owner of SV [IN] codepage
  VIASTDDECL GetStringInEncoding(char* buffer, int32 bufferLength, uint32 codepage) = 0;                 // [OUT] String value of SV [IN] Buffer length [IN] codepage
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 77)
  // for variables which are variable length arrays: set the current length of the array
  VIASTDDECL SetArrayLength(int32 length, VIASysVarClientHandle client) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 89)
  // physical value for scalar variables
  VIASTDDECL SetPhysicalValue(double value, VIASysVarClientHandle client) = 0;
  VIASTDDECL GetPhysicalValue(double* value) = 0;
  // physical value for array variables
  VIASTDDECL SetPhysicalValues(const double* values, int32 length, VIASysVarClientHandle client) = 0;
  VIASTDDECL GetPhysicalValues(double* values, int32 length) = 0;
  // note: only int and int array variables can have a linear encoding (physical value)
#endif
};

#endif // minor version >= 25



// ============================================================================
// class VIAService
// ============================================================================

// VIAService operation flags (see: GetServiceFlags)

const uint32 kVIA_MultiBusSupport = 0x00000001; // if set, the runtime environment supports multiple
                                                // nodes per ECU as well as automatic (transparent)
                                                // channel mapping
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 19)
const uint32 kVIA_DemoVersion     = 0x00000002; // CANoe/DENoe is a Demo-Version
const uint32 kVIA_NMEASupport     = 0x00000004; // CANoe/DENoe is a NMEA Version
const uint32 kVIA_J1939Support    = 0x00000008; // CANoe/DENoe is a J1939
const uint32 kVIA_RemoteKernel    = 0x00000010; // RuntimeKernel is Running in a Remote Process
const uint32 kVIA_DiVaSupport     = 0x00000020; // License for option DiVa exists
#endif 

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 44)
const uint32 kVIA_IsSameMachine   = 0x00000040; // RuntimeKernel is running on same machine as CANoe
#endif

//
// VIADoIPParams: specifies parameters for DoIP
//
struct VIADoIPParams
{
    uint32 size;   // size of this struct
    uint16 source; // logical source address
    uint16 target; // logical target address
};

// Additional information to allow identification of vehicles
struct VIADoIPParamsWithAddress : VIADoIPParams
{
  uint8  entityID[6];   // normally a MAC address
  uint8  groupID[6];    // normally a MAC address
  uint8  VIN[17];       // Vehicle Identification Number (normally a text string with specific restrictions)
  int8   bIPv6;         // 0: IPv4 (4 bytes used), 1: IPv6 (16 bytes used)
  uint32 vehicleIP[4];  // IP address of the _vehicle_ (i.e. not the tester) in IPv4 or IPv6 depending on bIPv6
};

//
// VIAKLineParams: specifies KLine params
//
struct VIAKLineParams
{
    uint32  size;            // size of this struct
    uint32  channel;         // physical channel (e.g. COM2, KLine1, etc...)
    uint32  baudrate;        // current baudrate
    VIATime time;            // timestamp (now)
    VIATime timestamps[255]; // byte timestamps (optional)
};

#if defined ( VIAMinorVersion ) && ( VIAMinorVersion >= 58 )
//visas didn't want to have a namespace ParameterServer, therefore ParameterServer::VIAService was renamed to VIAParameterServerService
//namespace ParameterServer
//{
  class VIAVariantProperty
  {
  public:
    // Get the current value of the control variable
    // Return values
    //   kVIA_OK                The current value has been written to address pValue
    //   kVIA_Failed            Unspecific error, nothing is written to address pValue
    VIASTDDECL GetCurrentIntValue( int32* pValue ) = 0; //[OUT] the current value of the parameter

    // Get the current value of the control variable
    // Return values
    //   kVIA_OK                The current value has been written to address pValue
    //   kVIA_Failed            Unspecific error, nothing is written to address pValue
    VIASTDDECL GetCurrentDoubleValue( double* pValue ) = 0; //[OUT] the current value of the parameter

    // Get the current value of the control variable
    // Return values
    //   kVIA_OK                The current value has been written to address pValue
    //   kVIA_Failed            Unspecific error, nothing is written to address pValue
    VIASTDDECL GetCurrentStringValue( char* pValue,               //[OUT] the current value of the parameter
                                      uint32 nBufferLength ) = 0; //[IN] the buffer length for the current value
  };

  class VIAVariantProperties
  {
  public:
    // Gets a specific variant property
    // Return values
    //   kVIA_OK                The variant property could be found and the address is written to address pVariantProperty
    //   kVIA_Failed            The variant property could not be found, nothing is written to address pVariantProperty
    VIASTDDECL GetVariantPropertyByFullyQualifiedName( const char* pFullyQualifiedName,            //[IN] the fully qualified name
                                                       VIAVariantProperty** pVariantProperty ) = 0;//[OUT] the address to the variant property to be found
  };

  class VIAScope
  {
  public:
    // Gets the variant properties from the scope
    // Return values
    //   kVIA_OK                The variant properties address is written to address pVariantProperties
    //   kVIA_Failed            Unspecific error, nothing is written to address pVariantProperties
    VIASTDDECL GetVariantProperties( VIAVariantProperties** pVariantProperties ) = 0; //[OUT] the address to the variant properties
  };

  class VIAParameterServerService
  {
  public:
    // Gets the node layer configuration parameter server scope
    // Return values
    //   kVIA_OK                The node layer configuration parameter server scope is written to address pNodeLayerConfigurationScope
    //   kVIA_Failed            Unspecific error, nothing is written to address pNodeLayerConfigurationScope
    VIASTDDECL GetNodeLayerConfigurationScope( VIAScope** pNodeLayerConfigurationScope ) = 0; //[OUT] the address to the scope

    // Gets the parameter server scope to the over given ID
    // Return values
    //   kVIA_OK                The parameter server scope could be found and the scope pointer is written to address pParameterServerScope
    //   kVIA_Failed            The parameter server scope could not be found, nothing is written to address pParameterServerScope
    VIASTDDECL GetScope( uint32 scopeID,                         //[IN] the ID for the scope to be retrieved
                         VIAScope** pParameterServerScope ) = 0; //[OUT] the address to the scope
  };
//}//namespace ParameterServer
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 95)

  // Callback which can be called in the core processing loop after sys vars / env vars / value entities have been updated.
  // See VIAService::RegisterCoreProcessingFunction
  class ICoreProcessingFunction
  {
  public:
    // Return true if the function changed variables / has set value entities / has set signals / has called function
    // bus functions. Then, the inner processing loop will run again. Else, return false.
    VIABOOLDECL Execute() = 0;
  };

#endif

//
// The service object
// Supported by CAPL-on-Board: Yes
//
class VIAService
{
public:

   // =============== Configuration items =====================================

   // Returns version of this implementation of VIAService
   // Supported by CAPL-on-Board: Yes
   VIASTDDECL GetVersion (int32* major, int32* minor, int32* patchlevel) = 0;

   // Gets the windows handle for the main window of CANoe. You can use
   // this as the parent handle for windows message boxes.
   // Obsolete (As version 1.10): Node layers DLLs should not pop up message
   // boxes any longer, because when you are using the 'CANoe RT' mode, the 
   // window message box pops up on the CANoe RT Server and not on the 
   // CANoe RT User PC.
   VIASTDDECL GetClientWindow(void** handle) = 0; 

   // Specifies topics for function GetConfigItem
   enum VIA_ConfigTopic {
      kVIA_ConfigurationPath = 1, // CANoe's configuration file (no subtopic)
      kVIA_IniFile           = 2, // the used init file (no subtopic)
      kVIA_SysPath           = 3, // (?)
      kVIA_UsrPath           = 4, // (?)
      kVIA_DatabaseName      = 5, // name used as qualifiers of database or an
                                  // empty string, if not available
                                  // Subtopic enumerates the databases [0,n-1].   
      kVIA_DatabasePath      = 6, // like kVIA_DatabaseName, but returns path and
                                  // filename of the databases.
      kVIA_Language          = 7, // actually used language in CANoe (no subtopic)
   };

   // Returns informations about the configuration of CANoe, like used
   // databases, filename of the configuration, etc.
   // The item is specified by a main topic an a sub topic. The topics are 
   // enumerated (see enum VIA_ConfigTopic). The meaning of sub topic depends on 
   // the main topic. But often the sub topic has no special meaning and should 
   // be set to zero in this case.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetConfigItem(uint32 topic,             // [IN] Main topic (see enum VIA_ConfigTopic)
                            uint32 subtopic,          // [IN] Sub topic (see enum VIA_ConfigTopic)
                            char*  buffer,            // [OUT] Buffer for the result
                            int32  bufferLength) = 0; // [IN] Size of resultbuffer


   // =============== Interface to database ===================================

   // Obsolete (As version 1.20): This interface is replaced by the access functions in the subclasses of VIABus
   // Specifies type of a db object
   enum VIA_DBObject {
      kVIA_DBDatabase  = 151,    // ...  database attribute
      kVIA_DBNode      = 152,    // ...  node attribute
      kVIA_DBMessage   = 153,    // ...  message attribute
      kVIA_DBSignal    = 154,    // ...  signal attribute
      kVIA_DBEnvVar    = 155     // ...  environment variable attribute
   };

   // Obsolete (As version 1.20): This interface is replaced by the access functions in the subclasses of VIABus
   // Specifies type of attributes
   enum VIA_DBType {
      kVIA_DBFloat     = 160,    // attribute type for attributes of type Float or Integer 
      kVIA_DBString    = 165     // attribute type for attributes of type String 
   };

   // Obsolete (As version 1.20): This interface is replaced by the access functions in the subclasses of VIABus
   // Get the Type (kVIA_DBFloat or kVIA_DBString) of a attribute
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetDBAttributeType (uint32*     attributeType,    // [OUT] The requested type (enum VIA_DBType)
                                  uint32      objectType,       // [IN] See enum VIA_DBObject
                                  const char* objectName,       // [IN]
                                  const char* attrName,         // [IN] Name of attribute
                                  const char* dbName=NULL) = 0; // [IN] Database name

   // Obsolete (As version 1.20): This interface is replaced by the access functions in the subclasses of VIABus
   // Get the value of an attribute of type kVIA_DBFloat
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetDBAttributeValue  (double*     attributeValue,    // [OUT] The requested value
                                    uint32      objectType,        // [IN] See enum VIA_DBObject
                                    const char* objectName,        // [IN]
                                    const char* attrName,          // [IN] Name of attribute
                                    const char* dbName=NULL ) = 0; // [IN] Database name

   // Obsolete (As version 1.20): This interface is replaced by the access functions in the subclasses of VIABus
   // Get the value of an attribute of type kVIA_DBString
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetDBAttributeString ( char*       buffer,            // [OUT] Buffer to copy the requested string
                                     int32       bufferLength,      // [IN] Size of buffer
                                     uint32      objectType,        // [IN] See enum VIA_DBObject
                                     const char* objectName,        // [IN]
                                     const char* attrName,          // [IN] Name of attribute
                                     const char* dbName=NULL ) = 0; // [IN] Database name


   // =============== Interface to timer ======================================

   // Creates a timer object. If a node is specified, the timer is 
   // affected by the drift and jitter used by this node.
   // Supported by CAPL-on-Board: No
   VIASTDDECL CreateTimer(VIATimer**      timer,     // [OUT] The created object
                          VIANode*        node,      // [IN] Network node
                          VIAOnTimerSink* sink,      // [IN] Callback object (NULL allowed)
                          const char*     name) = 0; // [IN] Name of the timer
                           
   // Releases the timer, so that VIAService can delete the object 
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseTimer (VIATimer* timer)  = 0;


   // =============== Interface to environment variables  ======================

   // Returns the access to environment variables. The function creates an 
   // interface object for a single environment variable.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetEnvVar (VIAEnvVar**  ev,        // [OUT] The created object
                         VIANode*     node,      // [IN] Network node
                         const char*  name,      // [IN] Name of the variable
                         VIAOnEnvVar* sink) = 0; // [IN] Callback object (NULL allowed)

   // Releases the object created by function GetEnvVar
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseEnvVar (VIAEnvVar* ev) = 0;


   // =============== Interface to bus ========================================

   // Returns an interface object to a bus system. The type of the interface is
   // specified by the parameter interfaceType.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetBusInterface(VIABus** busInterface,      // [OUT] The interface object
                              VIANode* node,              // [IN] Node, which will use the interface
                              uint32   interfaceType,     // [IN] See enum VIABusInterfaceType
                              int32    majorversion,      // [IN] Required major version of bus interface
                              int32    minorversion) = 0; // [IN] Required minor version of bus interface
                                
   // Releases the object created by function GetBusInterface
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseBusInterface(VIABus* busInterface) = 0;


   // =============== utility services ========================================

   // Returns an interface object to a utility service.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetUtilService( VIAUtil** service,           // [OUT] The service object
                              int32     majorversion,      // [IN] Required major version of bus interface
                              int32     minorversion) = 0; // [IN] Required minor version of bus interface

   // Releases the object created by function GetUtilService
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseUtilService(VIAUtil* service)= 0;


   // =============== misc ====================================================

   // Outputs text to the "Write Window"
   // Supported by CAPL-on-Board: No
   VIASTDDECL WriteString (const char* text) = 0;

   // Outputs text to a logging file
   // Supported by CAPL-on-Board: No
   VIASTDDECL WriteToLog  (const char* text) = 0;

   // Popup the well known assertion box of CANalyzer/CANoe
   // Supported by CAPL-on-Board: No
   VIASTDDECL Assertion(char* message, char* condition, char* file, int32 line) = 0;
   
   // Popup a Message Box 
   // Obsolete (As version 1.10):  Node layers DLLs should not pop up message
   // boxes any longer, because when you are using the 'CANoe RT' mode, the 
   // window message box pops up on the CANoe RT Server and not on the 
   // CANoe RT User PC.
   // Supported by CAPL-on-Board: No
   VIASTDDECL MsgBox(char* message) = 0;
   
   // Returns the status of the current measurement:
   // kVIA_OK:                 measurement is running
   // kVIA_ServiceNotRunning:  measurement is not running
   // (This function has nothing to do with 'CANoe RT')
   // Supported by CAPL-on-Board: No
   VIASTDDECL RtKernelIsRunning() = 0;
   
   // Use this function within a call from a CAPL function to retrieve
   // the node, which called the function.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetCurrentNode (VIANode** node) = 0;

   // Use this function within a call from a CAPL function to retrieve 
   // the nodelayer object of the given module, belonging to the current node
   // Supported by CAPL-on-Board: Yes
   VIASTDDECL GetCurrentNodeLayer (VIANodeLayerApi** nodelayer,  // [OUT] Current nodelayer
                                   VIAModuleApi* module) = 0;    // [IN] Module

   // Returns the timestamp of the event, that is processed at the moment 
   // (ringing timer, bus event, environment variable was set, ...)
   // Supported by CAPL-on-Board: Yes
   VIASTDDECL GetCurrentSimTime (VIATime* time) = 0; 

   // Stops the current measurement
   // Supported by CAPL-on-Board: No
   VIASTDDECL Stop()= 0;
   
   // Internal (do not use it for node layers!)
   // Something very important for the OSEK-Emulation
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetSystemFiber(void** fiber)= 0;
                     
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 10)
   // Returns special flags (operation modes, magics, etc.)
   // (e.g. see kVIA_MultiBusSupport)
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetServiceFlags( uint32* flags) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 12)
   // Specifies type to define the severity of an output to the write window
   enum VIAWriteSeverity
   {
      kVIA_WriteSeveritySuccess       = 0,
      kVIA_WriteSeverityInformational = 1,
      kVIA_WriteSeverityWarning       = 2,
      kVIA_WriteSeverityError         = 3,
   };
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 13)
   // Specifies type to define some predefined write sinks that are generally available
   // For these sinks, there is no need to perform the CreateWriteTab() 
   // and the ReleaseWriteTab()
   enum VIAWriteSinksPredefined
   {
     // special sinks
     kVIA_WriteSinkTrace              = -3,
     kVIA_WriteSinkLog                = -2,
     kVIA_WriteSinkDebug              = -1,

     // real write tab sinks
     kVIA_WriteSinkTabSystem          = 0,
     kVIA_WriteSinkTabCAPL            = 1,
     kVIA_WriteSinkTabTest            = 4,
   };

#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 12)
   // Creates a tab in the write-window with aSinkName and returns a handle 
   // onto this tab. With this handle, subsequent writes are possible.
   // Supported by CAPL-on-Board: No
   VIASTDDECL CreateWriteTab(uint32* aSink, const char* aSinkName) = 0;

   // Releases the tab created by function CreateWriteTab
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseWriteTab(uint32 aSink) = 0;

   // Outputs text to a previously created tab in the "write window"
   // Supported by CAPL-on-Board: No
   VIASTDDECL WriteStringToTab(uint32 aSink, VIAWriteSeverity aSeverity, const char* aText) = 0;

   // Retrieves the API object for controlling tests
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetTestControlApi( VIATestControlApi** apTestControlObject,   // [OUT] Supplied Api-object
                                 VIANode*        node) = 0;                 // [IN]    that matches the current node

   // Releases the API object created by function GetTestControlApi
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseTestControlApi( VIATestControlApi* apTestControlObject) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 13)
   // Clears a previously created tab
   // Supported by CAPL-on-Board: No
   VIASTDDECL ClearWriteTab(uint32 aSink) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 14)
   // Registers the Node Layer for service activity
   // Supported by CAPL-on-Board: No
   VIASTDDECL SetNLServiceApi( VIANLServiceApi* apNLServiceMember,
                               VIANode* apMyNode) = 0;

   // Registers a specific service of a provider
   // Supported by CAPL-on-Board: No
   VIASTDDECL ProvideNLService( int8 aMultiUserService,               // single or multiple access to the service
                                VIANLService* apServiceToProvide,
                                VIANode* apMyNode,
                                VIANLServiceApi* apNLServiceProvider) = 0;

   // Registers the acquire of a user for a specific service
   // Supported by CAPL-on-Board: No
   VIASTDDECL AcquireNLService( const char* apServiceName,
                                int32 aInterfaceVersion,
                                VIANLService** appService,
                                VIANode* apMyNode,
                                VIANLServiceApi* apNLServiceUser) = 0;

   // Cancel a specific service of a provider
   // Supported by CAPL-on-Board: No
   VIASTDDECL CancelNLService( VIANLService* apServiceToCancel,
                               VIANode* apMyNode,
                               VIANLServiceApi* apNLServiceProvider) = 0;

   // Cancel the acquire of a user for a specific service
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseNLService( VIANLService* apServiceToRelease,
                                VIANode* apMyNode,
                                VIANLServiceApi* apNLServiceUser) = 0;
#endif
  
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 17)
   // Retrieves the API object for signal access (no release necessary)
   // This function requires to be called during init measurement phase
   // of the VIANodeLayerApi object.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetSignalAccessApi( VIASignalAccessApi** aSignalAccessApi,
                                  VIANode* aNode,
                                  int32 majorversion,
                                  int32 minorversion) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 25)
   // Retrieves the root namespace of system variables
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetSystemVariablesRootNamespace(VIANamespace*& nameSpace) = 0;

   // Register a client owning system variables. You have to do this before
   // you create system variables.
   // Supported by CAPL-on-Board: No
   VIASTDDECL RegisterSystemVariablesClient(VIASysVarClientHandle handle, const char* description) = 0;
   // Unregister a client owning system variables. All variables owned by the 
   // client are removed automatically.
   // Supported by CAPL-on-Board: No
   VIASTDDECL UnregisterSystemVariablesClient(VIASysVarClientHandle handle) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 26)
   // Iterates over databases
   VIDB_DECLARE_ITERATABLE(Database)

   // Breaks the measurement as if by the CAPL function halt(). The function does
   // not return until the measurement is continued!
   // Can only be used in simulated mode.
   // Supported by CAPL-on-Board: No
   VIASTDDECL DebugBreak() = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 32)
   // Gets the information if CANoe is in simulated mode
   // simulated == 1: CANoe is in simulated mode.
   // simulated == 0: CANoe is in real mode.
   // Supported by CAPL-on-Board: No
   VIASTDDECL IsSimulated(int32* simulated) =0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 33)
   // Specifies types of the socket service interface
   enum VIASocketServiceType
   {
       kVIA_SocketServiceWinsock = 0,
       kVIA_SocketServiceRaw = 1,
       kVIA_SocketServiceIPB = 2,
       kVIA_SocketServiceDefault = 3 // use kVIA_SocketServiceWinsock or kVIA_SocketServiceIPB as defined in node configuration
   };
   // Returns the socket service of specified node and socket service type
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetSocketService( VIASocketService** ppService, VIANode* pNode, VIASocketServiceType type) = 0;

   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseSocketService( VIASocketService* pService) = 0;

   // Specifies type of diagnostic notification
   enum VIAProtocolType
   {
       kVIA_ProtocolDoIP = 0,
       kVIA_ProtocolKLine = 1,
   };

   // Internal use  (Deep diagnostic...)
   // Supported by CAPL-on-Board: No
   VIASTDDECL NotifyDiagnosticEvent( VIAProtocolType type, void* params, int32 request, int8 buffer[], uint32 size) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 34)
   // Returns the diagnostics description for a know ECU qualifier that allows accessing the "variant" object
   // Note: If the output buffers are too small, the text is cut!
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetDiagDescription(const char* aEcuQualifier_in, 
     char* apEcuId_out,            int32 aLenEcuId, 
     char* apVariantQualifier_out, int32 aLenVariantQualifier, 
     char* apLanguage_out,         int32 aLenLanguage, 
     char* apPath_out,             int32 aLenPath) = 0; // ODX file sets are NOT supported here, i.e. only PDX and CDD!
#endif


#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 40)
   // Specifies types of the serial service interface
   enum VIASerialServiceType
   {
       kVIA_SerialServiceWIN32 = 0,
       kVIA_SerialServiceXL,
   };
   // Returns the serial service of specified node and serial service type
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetSerialService( VIASerialService** ppService, VIANode* pNode, VIASerialServiceType type) = 0;

   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseSerialService( VIASerialService* pService) = 0;
#endif
   
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 41)
  // GetUserFilePath returns the full path to a CANoe 'User File'.
  // Return values
  //   kVIA_OK                the path was successfully written to pathBuffer.
  //   kVIA_ObjectNotFound    the given file name was not found in the list of configured user files (in distributed case).
  //   kVIA_ParameterInvalid  one ore more of the given parameters is invalid (For example: filename or pathBuffer is 
  //                          a NULL pointer, pathBufferLength is zero or negative)
  //   kVIA_BufferToSmall     pathBuffer is to small for the full path of the user file.
  //   kVIA_ServiceNotRunning function called outside measurement (no search path mgr available)
  //   kVIA_Failed            unspecific error
  // Supported by CAPL-on-Board: No
 VIASTDDECL GetUserFilePath(const char* filename,      // [IN]   The filename of the user file (example: 'settings.ini')
                            char* pathBuffer,          // [OUT]  The full path of the user file is copied to this buffer
                            int32 pathBufferLength)=0; // [IN]   size of parameter pathBuffer
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 42)
   // Get the default system variable handle for the given module.
   // CANoe creates a system variable client handle for each module, that is part of a configuration.
   // The handle remains valid even if node layer DLL is unloaded between measurements. The handle is removed when the
   // the module is removed from the configuration or the complete configuration is is unloaded.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetSystemVariablesDefaultClientHandle(VIASysVarClientHandle* handle, // [OUT] the default handle of the module
                                                    VIAModuleApi* module) = 0;     // [IN]  describes the module for which 
                                                                                   //       we want to get the default handle
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 48)
   // RegisterUserFile registers a new user file during measurement or updates the registered user file path 
   // if a file with that name is already registered.
   // Return values
   //   kVIA_OK                the path was successfully written to pathBuffer.
   //   kVIA_ParameterInvalid  the file path is parameter is invalid (e.g., empty file name)
   //   kVIA_ServiceNotRunning function called outside measurement
   //   kVIA_Failed            unspecific error
   // Supported by CAPL-on-Board: No
   VIASTDDECL RegisterUserFile(const char* filePath,       // [IN]   The file path of the user file on the user PC (absolute or relative to cfg dir)
                               bool isTempRegistration)=0; // [IN]   if true, registration is only valid during this measurement; 
                                                           //        otherwise user file is added to user file list in CANoe configuration
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 54)
   // Increments TimeBase time
   // Return values
   //   kVIA_OK                The TimeBase time was successfully incremented
   //   kVIA_Failed            Unspecific error

   VIASTDDECL IncrementTimerBase(VIATime newTimeBaseTicks, // [IN] new time base (1 tick == 1 ns)
                                 int32 numberOfTicks)=0; // [IN] resolution in steps for the increment specified in first parameter
  
   // Increments TimeBase time
   // Return values
   //   kVIA_OK                Request ok, parameter isSlaveMode has been set
   //   kVIA_Failed            Unspecific error
   VIASTDDECL IsSlaveMode(bool* isSlaveMode /* [OUT] if true, CANoe is running in slave mode */)=0; 
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 55)
   // Retrieves the API object for construction a CAPL-on-Board node layer.
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetCAPLonBoardConstruction (VIACAPLonBoardConstruction** cob) =0; // [OUT]  The retrieved API object.

   // Releases the object created by function GetCAPLonBoardConstruction
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseCAPLonBoardConstruction (VIACAPLonBoardConstruction* cob) =0; // [IN]  The API object to be released.
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 56)
   // Retrieves the API object for tests
   VIASTDDECL GetTestApi( VIATestApi** apTestApi,   // [OUT] Supplied Api-object
                          VIANode* pNode) = 0;      // [IN]  That matches the current node

   // Releases the API object created by function GetTestApi
   VIASTDDECL ReleaseTestApi(VIATestApi* apTestApi) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 57)
   // Returns the extended socket service of specified node 
   VIASTDDECL GetSocketServiceEx( VIASocketServiceEx** ppService, VIANode* pNode) = 0;
   VIASTDDECL ReleaseSocketServiceEx( VIASocketServiceEx* pService) = 0;
#endif

#if defined ( VIAMinorVersion ) && ( VIAMinorVersion >= 58 )
   // Get the VIA parameter server service. The parameter server service is available during measurement,
   // from VIAModuleApi::InitMeasurement until VIAModuleApi::EndMeasurement.
   // Return values
   //   kVIA_OK                The parameter server service could be retrieved
   //   kVIA_Failed            The parameter server service is not available at the moment
   VIASTDDECL GetParameterServerService( VIAParameterServerService** pVIAParameterServerService ) = 0; //[OUT] the VIA parameter server service
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 59)
   // Extended NodeLayer Service methods that take the execution mode as parameter
	
   // Identifies the execution mode
   enum VIANLServiceExecutionMode
   {
     kVIA_ExecutionModeStandard     = 0,
     kVIA_ExecutionModeCAPLonBoard  = 1,
     kVIA_ExecutionModeUnknown,
   };

   // Registers the Node Layer for service activity
   // Supported by CAPL-on-Board: Yes
   VIASTDDECL SetNLServiceApi2(
     VIANLServiceApi* apNLServiceMember,
     VIANode* apMyNode, 
     VIANLServiceExecutionMode execMode) = 0;

   // Registers a specific service of a provider
   // Supported by CAPL-on-Board: Yes
   VIASTDDECL ProvideNLService2( int8 aMultiUserService,
     VIANLService* apServiceToProvide,
     VIANode* apMyNode,
     VIANLServiceApi* apNLServiceProvider,
     VIANLServiceExecutionMode execMode) = 0;

   // Registers the acquire of a user for a specific service
   // Supported by CAPL-on-Board: Yes
   VIASTDDECL AcquireNLService2( const char* apServiceName,
     int32 aInterfaceVersion,
     VIANLService** appService,
     VIANode* apMyNode,
     VIANLServiceApi* apNLServiceUser,
     VIANLServiceExecutionMode execMode) = 0;

   // Cancel a specific service of a provider
   // Supported by CAPL-on-Board: Yes
   VIASTDDECL CancelNLService2( VIANLService* apServiceToCancel,
     VIANode* apMyNode,
     VIANLServiceApi* apNLServiceProvider,
     VIANLServiceExecutionMode execMode) = 0;

   // Cancel the acquire of a user for a specific service
   // Supported by CAPL-on-Board: Yes
   VIASTDDECL ReleaseNLService2( VIANLService* apServiceToRelease,
     VIANode* apMyNode,
     VIANLServiceApi* apNLServiceUser,
     VIANLServiceExecutionMode execMode) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 60)
   // GetSynchronizedFilePath returns the full path to a (synchronized) file. In distributed case the file path to the
   // synchronized files folder will be returned if the file is part of the synchronized files list. If RTK and CANoe
   // are running on the same machine the configuration relative absolute path on that machine will be returned, whether 
   // it's part of the synchronized file list or not.
   // Return values
   //   kVIA_OK                       the file exists on that machine
   //   kVIA_ObjectNotFound           the file does not exist on that machine
   //   kVIA_ParameterInvalid         one ore more of the given parameters is invalid (For example: filename or pathBuffer is 
   //                                 a nullptr, or pathBufferLength is zero)
   //   kVIA_BufferToSmall            pathBuffer is to small for the full path of the synchronized file.
   //   kVIA_FunctionNotImplemented   function not implemented for CANoe.exe
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetSynchronizedFilePath( const char* filename,          // [IN]  The absolute or relative filepath of the synchronized file (example: 'settings.ini', 'C:\temp.txt')
                                       char* pathBuffer,              // [OUT] The full path of the synchronized file is copied to this buffer
                                       uint32 pathBufferLength ) = 0; // [IN]  size of parameter pathBuffer
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 68)
   VIASTDDECL GetMediaService(VIAMediaService** ppService, VIANode* pNode) = 0;
   VIASTDDECL ReleaseMediaService(VIAMediaService* pService) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 74)

   // Get the client which currently delivers the bus context upon GetBusContext() in CAPL (only for restoration purposes)
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetCurrentClient(void** pClient) = 0;

   // Get the client which delivers the bus context for the specified VIANode
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetClient(VIANode* pNode, void** pClient) = 0;

   // Set the current client which delivers the bus context upon GetBusContext() in CAPL (note: use only a pointer retrieved with one of the two previous get functions)
   // Supported by CAPL-on-Board: No
   VIASTDDECL SetCurrentClient(void* pClient) = 0;

   // Get the bus context of the specified VIANode
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetBusContext(VIANode* pNode, uint32* channelType, uint32* channelNumber) = 0;

   // Set the bus context of the specified VIANode
   // Supported by CAPL-on-Board: No
   VIASTDDECL SetBusContext(VIANode* pNode, uint32 channelType, uint32 channelNumber) = 0;

#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 94)
   // Retrieves the API object for FunctionBus access
   // Supported by CAPL-on-Board: No
   VIASTDDECL GetFunctionBusService(VIAFbViaService** outFbViaService,
     int32 majorversion,
     int32 minorversion) = 0;
   // Release a previously obtained service instance
   // Supported by CAPL-on-Board: No
   VIASTDDECL ReleaseFunctionBusService(VIAFbViaService* inFbViaService) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 95)

   // Register a function to be executed after event handling / timer handling.
   // The function is executed after all system variables / environment variables
   // / function bus value entities have been updated.
   // If the parameter 'once' is true, the function will be executed only once
   // after the currently processed event / timer. The out parameter 'handle' is meaningless
   // then and will be ignored.
   // Else (if 'once' is false), the function will be executed after each inner loop 
   // (i.e., after the system variables etc.) until it is deregistered using the
   // returned handle.
   // Functions are called in the order in which they are registered, but 'once' functions are 
   // always called after the other functions.
   VIASTDDECL RegisterCoreProcessingFunction(ICoreProcessingFunction* fct, bool once, uint32* handle) = 0;
   // Unregister a function which was previously registered with the parameter 'once' set to false.
   VIASTDDECL UnregisterCoreProcessingFunction(uint32 handle) = 0;

#endif

}; // class VIAService




// ==========================================================================
// class VIANodeLayerApi 
//
// Interface of the nodelayer, which is called by the runtime kernel.
// This class must be implemented by the nodelayer-DLL.
//
// In case of CAPL-on-Board, this class must be implemented twice (for x86 and 
// ARM), because some of the methods are called on the PC and the others are 
// called on the network device (VN7600, VN1630, ...)
// ==========================================================================
class VIANodeLayerApi
{
public:
   // Returns the Node, to which this Layer belongs. This should be the handle,
   // that was provided by the call of VIAModuleApi::CreateObject(...)
   VIASTDDECL GetNode(VIANode** node) = 0;

   // In case of CAPL-on-Board, this method is called on the PC (x86)
   VIASTDDECL InitMeasurement  () = 0;

   // In case of CAPL-on-Board, this method is called on the network device (ARM)
   VIASTDDECL StartMeasurement () = 0;

   // In case of CAPL-on-Board, this method is called on the network device (ARM)
   VIASTDDECL StopMeasurement  () = 0;

   // In case of CAPL-on-Board, this method is called on the PC (x86)
   VIASTDDECL EndMeasurement   () = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 71)
   VIASTDDECL InitMeasurementComplete() = 0;
#endif
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 89)
    // Is called BEFORE the CAPL Event Handler'on preStart' is triggered
   VIASTDDECL PreInitMeasurement() = 0;
#endif
};



// ==========================================================================
// class VIAModuleApi 
//
// Interface of the module, which is called by the runtime kernel.
//
// This class, together with the functions VIAGetModuleApi and 
// VIAReleaseModuleApi, must be implemented by the nodelayer-DLL.
//
// In case of CAPL-on-Board, this class is only implemented on the PC using 
// x86 code.  There is no ARM code needed for this class.
// ==========================================================================

// Arguments for CreateObject and CreateDeviceModule
#define VIAArgumentCAPLonBoard "-CAPLonBoard"
#define VIAArgumentVRTB        "-VRTB"

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 75)
// Specifies constants for method DoInformOfChange
enum VIANLChangeInfo
{
  kVIA_NLChg_AttachOrDetach = 0x0001,
  kVIA_NLChg_BusName = 0x0002,
  kVIA_NLChg_NodeName = 0x0004,
  kVIA_NLChg_Activity = 0x0008,
  kVIA_NLChg_AppChannel = 0x0010,
  kVIA_NLChg_OldNodeNameSupportedInOldName = 0x0020
};
#endif

class VIAModuleApi
{
public:
   // Initializes the nodelayer. Here you can do additional setup of the DLL.
   // CANoe calls this method after it ask for the object (function 
   // 'VIAGetModuleApi') and before it calls any other method of this object.
   VIASTDDECL Init()= 0;                     
   
   // The text provided by GetVersion(...) is displayed in the info window
   // This is normally the name and version of this DLL.
   VIASTDDECL GetVersion (char* buffer, int32 bufferLength ) = 0;

   // With a Shift-DoubleClick in the Info-Window, the following information 
   // is displayed in the Write-Window.
   // pathBuff: filename and path of this nodelayer
   // versionBuff: name and version number of this nodelayer
   VIASTDDECL GetModuleParameters (char* pathBuff,    int32 pathBuffLength,
                                   char* versionBuff, int32 versionBuffLength) = 0;

   // The information provided by GetNodeInfo(...) appears within the node
   // at the simulation setup window (shortName) and the status indication
   // line of CANoe (longName) .
   VIASTDDECL GetNodeInfo (const char* nodename,      // name of the node
                           char* shortNameBuf, int32 shortBufLength,
                           char* longNameBuf,  int32 longBufLength ) = 0;


   // Creates a nodelayer object for the given node
   // Additional parameters
   //  -CAPLonBoard            Execute node layer on board of network interface (CAPL-on-Board)
   //  -VRTB                   Execute node layer in VRTB mode (Vector Real Time Boost of VN8910)
   // Return values:
   //   kVIA_OK                The nodelayer object was successfully created.
   //   kVIA_Failed            The creation of the nodelayer object failed because of an error.
   //   kVIA_ObjectUndesired   The nodelayer DLL intentionally does not want to create an 
   //                          nodelayer object for the given node.
   VIASTDDECL CreateObject  (VIANodeLayerApi** object,       // [OUT] The created instance
                             VIANode* node,                  // [IN] Node
                             int32 argc, char* argv[]) = 0;  // [IN] Additional parameters

   // Releases the given nodelayer object
   VIASTDDECL ReleaseObject (VIANodeLayerApi*  object) = 0;

   VIASTDDECL InitMeasurement () = 0;
   VIASTDDECL EndMeasurement  () = 0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 26)
   // An extended version of GetNodeInfo which gets given the database information
   // of the node instead of its name. This method is called if the following 
   // conditions are true:
   // -- the desired version of the VIA interface is set at least to 1.26 (see below). 
   // -- a database is assigned to the configuration
   // -- the node in the simulation setup is assigned to a database node
   // Otherwise, the GetNodeInfo method is called. For compatibility reasons,
   // GetNodeInfo may also be called once anyway.
   VIASTDDECL GetNodeInfoEx (VIDBNodeDefinition* nodeDefinition,
                             char* shortNameBuf, int32 shortBufLength,
                             char* longNameBuf, int32 longBufLength ) = 0;
#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 75)
   // Inform the nodelayer (while measurement is NOT running) about changes of 
   // -- bus name in simulation setup the node is attached to
   // -- the node's name itself and its assignment to a database node,
   // -- the node's activity state
   // This method is called if the following conditions are true:
   // -- the desired version of the VIA interface is set at least to 1.75. 
   // -- a database is assigned to the configuration
   // -- the node in the simulation setup is assigned to a database node
   VIASTDDECL DoInformOfChange(VIDBNodeDefinition* nodeDefinition,
     const uint32      changeFlags, // cf. VIANLChangeInfo
     const char*       simBusName,
     const VIABusInterfaceType busType,
     const VIAChannel  channel,
     const char*       oldName,
     const char*       newName,
     const int32       bValue) = 0;
#endif

};


#if defined ( VIAMinorVersion ) && ( VIAMinorVersion >= 67)

// ==========================================================================
// class VIASystemVariableMemberInitValues 
//
// This class provides an interface for a map of system variable 
// struct members or array elements with their corresponding initialization 
// values.
// 
// Initializations can be added with their absolute member paths (e.g. 
// MyNamespace::MySubNamespace::StructVariable.StructMember) within the 
// system variable and the respective init values.
// The interface can then be passed when instantiating system variables 
// (struct or generic array) to directly set the init values. 
// ==========================================================================
class VIASystemVariableMemberInitValues
{
public:
  VIASTDDECL AddDoubleValue(const char* memberPath, double value) = 0;       // add a Double type member
  VIASTDDECL AddInt32Value(const char* memberPath, int32 value) = 0;         // add a Long type member
  VIASTDDECL AddInt64Value(const char* memberPath, ::int64 value) = 0;       // add a LongLong type member
  VIASTDDECL AddStringValue(const char* memberPath, const char* value) = 0;  // add a String type member
  VIASTDDECL Clear() = 0;                                                    // clear the initialization map
};
#endif


// ==========================================================================
// setup functions
//
// The following C-functions are for loading, unloading and initializing 
// of the DLL. They are called just after loading or just before unloading of 
// the DLL.
// ==========================================================================

// CANoe calls the function 'VIARequiredVersion' to ask the nodelayer DLL,
// with which version of the VIA-interface it was compiled.
// The required version is the minimum VIA version with which the nodelayer DLL
// works.
// This is the first function, that CANoe calls after loading the DLL.
VIACLIENT(void) VIARequiredVersion (int32* majorversion, int32* minorversion);

// CANoe calls the function 'VIADesiredVersion' to ask the nodelayer DLL,
// with which version of the VIA-interface it was compiled.
// The desired version must be equal or higher than the required version.
// To access database information outside the measurement, you must set the
// desired version at least to 1.26. Then, instead of GetNodeInfo, GetNodeInfoEx
// will be called by CANoe (see above).
// If the function VIADesiredVersion is not implemented, the desired version is 
// assumed to be equal to the required version.
VIACLIENT(void) VIADesiredVersion (int32* majorversion, int32* minorversion);

// After CANoe has asked, which version of the via-service the DLL expects,
// it assigns a compatible version of the service to the DLL.
// This service object stays valid until the DLL is unloaded or this function 
// is called again. This is the second function, that CANoe calls after loading 
// the DLL.
VIACLIENT(void) VIASetService (VIAService* service);


// VIAGetNodeLayerApi is called by the runtime kernel to get the
// interface of the module.
// Parameter argc, argv: Additional parameters for multiservice modules
//                       This parameters are ignored by most nodelayers
// Note: The interface-object must not be destroyed by the module 
// before VIAReleaseNodeLayerApi() is called. This is the third function
// that CANoe calls on the nodelayer DLL. It is called after CANoe has 
// initialized the DLL with a VIA-Service object.
VIACLIENT(VIAModuleApi*) VIAGetModuleApi(int32 argc, char* argv[]);


// VIAReleaseNodeLayerApi is called by the runtime kernel, when the
// interface of the nodelayer isn't needed any longer.
// This function is called just before the DLL will be unloaded. 
VIACLIENT(void) VIAReleaseModuleApi (VIAModuleApi* api);


// CANoe calls the function NLGetModuleOptions to ask several options
// about the loaded DLL. If the DLL does not know the given options
// it should return the default value 0.
enum VGetModuleOptionsParameter 
{
  kVIA_ModuleOption_LoadOption=0,
  kVIA_GetModuleOption_DLLType=1,
  kVIA_GetModuleOption_CAPLonBoard=2,
};

// return codes for option Load
enum VGetModuleOptions_LoadOptions
{
  kVIA_ModuleOption_LoadOption_Default =0,
  kVIA_ModuleOption_LoadOption_Permanent =1,
  kVIA_ModuleOption_LoadOption_Defer =2,
};

// return codes for option DLLType 
enum VGetModuleOptions_DLLTypeOptions
{
  kVIA_ModuleOption_DLLType_Default=0,
  kVIA_ModuleOption_DLLType_StandardNodeLayer =1,
  kVIA_ModuleOption_DLLType_CLibrary=2,
  kVIA_ModuleOption_DLLType_AnlyzNodelayer =3
};

// return codes for option CAPLonBoard
enum VGetModuleOptions_CAPLonBoard
{
  kVIA_ModuleOption_CAPLonBoard_Default=0,
  kVIA_ModuleOption_CAPLonBoard_Supported=1,
  kVIA_ModuleOption_CAPLonBoard_NotSupported=-1,
};

VIACLIENT(int32) NLGetModuleOptions(int32 option);

#endif // VIA_H

