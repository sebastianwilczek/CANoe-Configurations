/*----------------------------------------------------------------------------
|
|  File Name: VIA.h 
|
|  Comment:   New external interface for CANoe nodes (Nodelayer-DLLs)
|                                         
|
|-----------------------------------------------------------------------------
|               A U T H O R   I D E N T I T Y
|-----------------------------------------------------------------------------
| Initials     Name                      Company
| --------     ---------------------     -------------------------------------
| Lm           Marc    Lobmeyer          Vector Informatik GmbH
| Ri           Thomas  Riegraf           Vector Informatik GmbH
| Sn           Michael Stephan           Vector Informatik GmbH           
| As           Stefan  Albus             Vector Informatik GmbH  
| Kr           Carsten Kellner           Vector Informatik GmbH
| Bee          Siegfried Beeh            Vector Informatik GmbH
| Rue          Joerg   Ruedenauer        Vector Informatik GmbH
| Jsd          Julian Sondergeld         Vector Informatik GmbH
|-----------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
|-----------------------------------------------------------------------------
| Date         Ver  Author  Description
| ----------   ---  ------  --------------------------------------------------
| 09.11.1999   1.0   As     Creation
| Dec   1999   1.3   As     First useable Version
| 04.01.1999   1.4   As     SetCanLine (Online/Offline) added
| 11.01.2000   1.5   As     Support for ErrorFrames completed
| 12.01.2000   1.5   Kr     Minor corrections, especially in comments
| 19.01.2000   1.6   As     Bus interface moved into its own object
|                           CANoe environment variables added
|                           Cleanup of many datatypes
|                           64 bit integer variables used for time           
|                           Support for OSEK emulation added
| 24.01.2000   1.7   As     Multiple Databases and minor changes  
| 02.03.2000   1.7.1 As     Correction for environment variables and
|                           handling of needed typedefs
| 15.03.2000   1.8   As     All event callbacks have now a time parameter.
|                           Check of version number now before anything else
|                           is done. 
| 29.03.2000   1.9   As     Utilities added. 
|                           Timers are now configered with call to CreateTimer.
|                           Calls to CAPL-Functions can use a result parameter
|                           of type uint32.  
| 14.02.2002   1.10  Sn     Multibus support added.
|                           CANoe supports multiple instantiations of Nodelayer-
|                           DLLs per CAPL block (which now models an entire ECU
|                           rather than a single node).
| 06.12.2002   1.11  As     Database names and pathes can now be retrieved from
|                           VIAService and VIANode.
| 15.09.2003   1.12  Bee    Access to the test control api-class added
|                           More functionality for write-outputs added
| 14.10.2003   1.13  Bee    ClearWriteTab() added.
|                           Predefined write-sinks added
|                           Argument types for the write tab argument size changed
| 24.10.2003   1.14  Aa     Access to Node Layer Services
| 24.11.2003   1.15  Bee    Write Sink "Test" introduced
| 25.11.2003   1.16  As     VIAEnvVar extended to get the current size of variable
|                           of type string and data
| 11.05.2004   1.17  Aa     Signal access introduced
| 14.09.2004   1.18  Krs    Config item language added
| 21.09.2004   1.19  As     Additional ServiceFlags added
| 22.12.2004   1.20  Rue    Database access refactoring
| 16.08.2005   1.21  Jr     Added GetJ1939Message to VICAN
| 18.07.2006   1.22  Sha    Added support for LIN ETF-events 
| 26.09.2006   1.23  Aa     Added new events for the VIDBSignalDefinition-API
| 29.01.2006   1.23.1 Jsd   Added kVIA_Failed (no version change -> As)
| 2007-03-12   1.23.2 Trs   Added kVIA_DiVaSupport (no version change)
| ...
| sometime     2.0          By definition version 2.0 will be the first release 
| in the                    of VIA interface, that is not longer compatible with 
| future                    version 1.x 
|                           
|-----------------------------------------------------------------------------
|               C O P Y R I G H T
|-----------------------------------------------------------------------------
| Copyright (c) 1994 - 1999 by Vector Informatik GmbH.  All rights reserved.
-----------------------------------------------------------------------------*/


#ifndef VIA_H
#define VIA_H


// ============================================================================
// Version of the interfaces, which are declared in this header
// See  REVISION HISTORY for a description of the versions
// ============================================================================

#if !defined ( VIAMajorVersion )
  #define VIAMajorVersion 1
#endif
#if !defined ( VIAMinorVersion )
  #define VIAMinorVersion 23
#endif

// ============================================================================
// Dependencies
// ============================================================================


#ifndef HAS_FIX_SIZE_INT_DEFINED
#define HAS_FIX_SIZE_INT_DEFINED

  typedef signed char       int8;
  typedef signed short      int16;
  typedef signed long       int32;
  typedef signed __int64    int64;

  typedef unsigned char     uint8;
  typedef unsigned short    uint16;
  typedef unsigned long     uint32;
  typedef unsigned __int64  uint64;

#endif // HAS_FIX_SIZE_INT_DEFINED



// We need only the type HWND from <windows.h>.
// If you don't need windows, the following dummy declaration is all what you 
// need. Ohterwise include <windows.h> befor you process this header.
#if !defined ( _WINDOWS_ ) && !defined ( _WINDEF_ )
   typedef void*   HWND;
#endif // _WINDOWS_



// some forward declarations
class VIANode;              
class VIANodeLayerApi;
class VIAModuleApi;
class VIARequest;
class VIAEnvVar;
class VIAUtil;
class VIATestObject;
class VIATestControlApi;
class VIANLService;
class VIANLServiceApi;
class VIASignalAccessApi;

class VIDBAttribute;
class VIDBMessageDefinition;
class VIDBSignalDefinition;
class VIDBNodeDefinition;

#if defined ( _BUILDNODELAYERDLL )
  #define VIACLIENT(type)  extern "C" __declspec (dllexport) type __stdcall
#else
  #define VIACLIENT(type)  extern "C" __declspec (dllimport) type __stdcall
#endif



// ============================================================================
// Standard return type and return values for the services
// ============================================================================



// 'VIAResult' is the standard result type of all methods declared below. 
typedef int32 VIAResult;  

// Standard declaration and definition macro for methods with an result of 
// type 'VIAResult'.
#define VIASTDDECL  virtual VIAResult __stdcall
#define VIASTDDEF   VIAResult __stdcall


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

#if ( _MSC_VER <= 1200 )
#pragma warning( disable: 4661) // 'void fooclass<class type>::foo(void)' : no suitable definition provided for explicit template instantiation request
#endif

/*
 * Iteration over some database objects.
 * The "GetNextElement" function may create the elements it returns, all of them _must_ be 
 * released with their normal release mechanism. If GetNextElement or SkipElement are called  
 * while HasMoreElements returns false, kVIA_ObjectInvalid is returned.
 */
template<typename T>
class VIDBIterator
{
public:
    virtual bool __stdcall HasMoreElements() const = 0;
    // GetNextElement returns the current element and moves the iterator forward (like STL: (*it)++)
    VIASTDDECL GetNextElement(T** element) = 0;
    VIASTDDECL SkipElement() = 0; // go to next element without creating (like STL: ++it)
    VIASTDDECL Release() = 0; // releases the iterator object
};

/*
 * Macro to declare an iterator quickly. E.g., iteration over attributes of a node is
 * thereby declared in VIANode (see below) as
 * typedef VIAIterator<VIDBAttribute> VIDBAttributeIterator;
 * VIASTDDECL GetAttributeIterator(VIDBAttributeIterator** iterator) = 0;
 *
 * You may either use the typedef (e.g. VIANode::VIDBAttributeIterator)
 * or the templated form (e.g. VIAIterator<VIDBAttribute>).
 */
#define VIDB_DECLARE_ITERATABLE(type)                                    \
typedef VIDBIterator<VIDB##type> VIDB##type##Iterator;                     \
VIASTDDECL Get##type##Iterator(VIDB##type##Iterator** iterator) = 0;     

// pulled out of VIAService
enum VIA_DBType {
    kVIA_DBFloat     = 160,    // attribute type for attributes of type Float, Integer or Enum
    kVIA_DBString    = 165     // attribute type for attributes of type String 
};

/*
 * Attribute of an object, e.g. of a network, a message, or a signal. The behaviour is undefined
 * if you use GetValue or GetString with attributes of the wrong type.
 */
class VIDBAttribute 
{
public:
   VIASTDDECL GetName(char* buffer, int32 bufferLength) const = 0;
   VIASTDDECL GetType(uint32& type) const = 0; // a VIA_DBType from above
   VIASTDDECL GetValue(double& value) const = 0; // for type kVIA_DBFloat
   VIASTDDECL GetString(char* buffer, int32 bufferLength) const = 0; // for type kVIA_DBString
   VIASTDDECL Release() = 0;
};

/*
 * Macro to declare that an interface gives access to attributes defined in a database.  See
 * VIDBAttribute above.
 */
#define VIDB_DECLARE_ATTRIBUTE_HOLDER                               \
 public:                                                            \
   VIASTDDECL GetAttribute(const char* pAttrName,                   \
                           VIDBAttribute** attribute) = 0;           \
                                                                    \
   VIASTDDECL GetAttributeType(uint32* pType,                       \
                               const char* pAttrName) const = 0;    \
   VIASTDDECL GetAttributeValue(double* pValue,                     \
                                const char* pAttrName) const = 0;   \
   VIASTDDECL GetAttributeString(char* pBuffer, int32 bufferLength, \
                                 const char* pAttrName) const = 0;  \
                                                                    \
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



enum VIADir             // Direction of a message
{          
  kVIA_Rx        =0,    // receive
  kVIA_Tx        =1,    // transmit
  kVIA_TxRq      =2     // transmit request
};  



enum VIARequestType    // used for creating request about messages on an bus
{ 
  kVIA_OneId     =0,   // Receive only one Id        
  kVIA_AllId     =1,   // Receive all Ids
  kVIA_MskId     =2    // Receive all Ids specified by Mask 
};



enum VIAOnlineOffline // Constants for method SetLine, which switches parts
{                      // of the bus interface on or off
  kVIA_Online   = 1,   // switch bus interface on
  kVIA_Offline  = 0,   // switch bus interface off
  kVIA_Model    = 2,   // switching impacts CAPL programm
  kVIA_DLL      = 4    // switching impacts nodelayer dll
};  



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
  kVIA_BEAN     =  8  // BEAN
};

class VIABus
{
public:
   // Get the type (see enum VIABusInterfaceType) and the version of this 
   // bus interface object.
   VIASTDDECL GetVersion (uint32* busInterfaceType,
                          int32* majorversion, int32* minorversion) =0;

   // Get the number of available channels for this bus interface
   VIASTDDECL GetNumberOfChannels(VIAChannel* maxChannelNumber) =0;

   // SetLine(...) is used to switch the bus interface of a node on or off.
   // Parameter channel describes the channel, that should be turned on or off.
   //   At the moment, kVIA_WildcardChannel is the only supported value for 
   //   the parameter channel
   // Parameter mode is kVIA_Online or kVIA_Offline
   // Parameter part is a combination of the Flags kVIA_Model and kVIA_DLL,
   //   which describes, which part of a node is switched on or off. 
   VIASTDDECL SetLine (VIAChannel channel, uint32 mode, uint32 part32) =0; 

   // General method to release a request for messages on the bus.
   // The creation of the events is specific to the concrete  bus interface
   VIASTDDECL ReleaseRequest (VIARequestHandle handle) =0;

}; // class VIABus

typedef VIABus VIANetwork;

// ============================================================================
// Interface to timer
// ============================================================================

 
//
// callback object for a ringing timer
//
class VIAOnTimerSink                   
{
public:
   // The time provided by the callback method 'OnTimer' is the time, when 
   // the timer should have done a ring. (This is like an ideal timer, that 
   // never has a delay even when the CPU is busy.) 
   VIASTDDECL OnTimer (VIATime nanoseconds) = 0;
}; // class VIAOnTimer


//
// The timer object itself
//
class VIATimer
{
public:

   // Set the callback object of the timer. The sink-object will be referenced
   // and MUST NOT be deleted until the timer is released or the sink is set 
   // otherwise.
   VIASTDDECL SetSink (VIAOnTimerSink* sink) = 0; // sink will be referenced

   // Set the name of the timer. The name is used for debugging purpose 
   // and for warnings, which appear in the "Write Window" of CANalyzer/CANoe
   // The name will be copied into the timer object, so you are allowed to 
   // delete the string directly after the call of method.
   VIASTDDECL SetName (const char* name) = 0;     
 
   VIASTDDECL SetTimer(VIATime nanoseconds) = 0;
   VIASTDDECL CancelTimer() = 0;
}; // class VIATimer



// ============================================================================
// Access to CAPL
// ============================================================================



class VIACaplFunction
{
public:
   // Total number of bytes used for all parameters of this CAPL function 
   VIASTDDECL ParamSize  (int32* size) =0;
   
   // Number of parameters in the signature of this CAPL function
   VIASTDDECL ParamCount (int32* size) =0;

   // The type of the nth (0, 1, 2 ...) parameter in the signature of this 
   // CAPL function. See file 'cdll.h' for the meaning of the types.
   VIASTDDECL ParamType  (char* type, int32 nth) =0;         
   
   // The type of the return value. See file 'cdll.h' for the meaning of 
   // parameter type.. 
   VIASTDDECL ResultType (char* type) =0;    

   // Call the CAPL function with the given parameters. 
   // Params is a pointer to the parameterlist using the standard call calling 
   // convention.
   VIASTDDECL Call (uint32* result, void* params) =0;       
   // Calls the CAPL function with the given parameters. Params is a pointer to the parameter 
   // list using the standard call calling convention.
   // The return value is given back as a double.
   VIASTDDECL CallReturnsDouble (double* result, void* params) = 0;
}; // class VIACaplFunction



// ============================================================================
// Access to environment variables
// ============================================================================


enum VIAEnvVarType   // the type of an environment variable
{
  kVIA_EVInteger = 0,
  kVIA_EVFloat   = 1,
  kVIA_EVString  = 2,
  kVIA_EVData    = 3
}; 


//
// callback object for changes of the environment variable
//
class VIAOnEnvVar
{
public:
  VIASTDDECL OnEnvVar(VIATime nanoseconds, VIAEnvVar* ev) =0;
};



class VIAEnvVar
{
public:
  VIASTDDECL GetType(uint32* type) =0;   // see enum VIAEnvVarType for type 
  VIASTDDECL GetName(char* buffer, int32 bufferLength) =0;

  VIASTDDECL SetSink (VIAOnEnvVar* sink) =0;

  VIASTDDECL SetInteger(int32  x) =0;
  VIASTDDECL GetInteger(int32* x) =0;

  VIASTDDECL SetFloat(double  x) =0;
  VIASTDDECL GetFloat(double* x) =0;

  VIASTDDECL SetString(const char* text) =0;
  VIASTDDECL GetString(char* buffer, int32 bufferLength) =0;

  VIASTDDECL GetData(uint8* buffer, int32 bufferLength) =0;
  VIASTDDECL SetData(const uint8* buffer, int32 bufferLength ) =0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 15)
  VIASTDDECL GetSize(int32* datasize) =0;
  VIASTDDECL GetDataAndSize (uint8* buffer, int32 bufferLength, int32* datasize) =0;
#endif
}; // class VIAEnvVar



#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 10)

// ============================================================================
// Interface to an ECU
// ============================================================================

class VIAECU
{
public:
   // Get the name of the ECU. 
   VIASTDDECL GetName( char* buffer, int32 bufferLength) = 0;

   // As of version 1.20, please don't use the following functions any more.
   // They are replaced by the attribute retrieval functions of VIANode.
   VIASTDDECL GetDBAttributeType( uint32* pType, uint32 objectType, const char* objectName, const char* pAttrName) = 0;
   VIASTDDECL GetDBAttributeValue( double* pValue, uint32 objectType, const char* objectName, const char* pAttrName) = 0;
   VIASTDDECL GetDBAttributeString( char* pBuffer, int32 bufferLength, uint32 objectType, const char* objectName, const char* pAttrName) = 0;
};

#endif

// ============================================================================
// Interface to a node
// ============================================================================

class VIANode
{
public:
   // Get the name of the network node. 
   VIASTDDECL GetName(char* buffer, int32 bufferLength) =0;

   // Get a CAPL function handle. The handle stays valid until end of
   // measurement or a call of ReleaseCaplFunction. This method is best
   // called at InitMeasurement. 
   VIASTDDECL GetCaplFunction(VIACaplFunction** caplfct,     // the created handle
                              const char* functionName) =0;  // name of the function

   // Release the CAPL function handle received by 'GetCaplFunction' (see above)
   // Call this method at EndMeasurement for every CAPL function handle, that  
   // you have asked for at InitMeasurement.
   VIASTDDECL ReleaseCaplFunction(VIACaplFunction* caplfct) =0;

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 10)

   // Get the name of the bus this node is attached to
   VIASTDDECL GetBusName( char* pName, uint32 size) = 0;

   // Get the type of the bus this node is attached to (see enum VIABusInterfaceType)
   VIASTDDECL GetBusType( uint32* interfaceType) = 0;

   // Get the channel of the bus this node is attached to
   VIASTDDECL GetChannel( VIAChannel* pChannel) = 0;

   // As of version 1.20, please don't use these attribute retrieval functions any more.
   // They are replaced by the specific attribute retrieval functions further on.
   VIASTDDECL GetDBAttributeType( uint32* pType, uint32 objectType, const char* objectName, const char* pAttrName) = 0;
   VIASTDDECL GetDBAttributeValue( double* pValue, uint32 objectType, const char* objectName, const char* pAttrName) = 0;
   VIASTDDECL GetDBAttributeString( char* pBuffer, int32 bufferLength, uint32 objectType, const char* objectName, const char* pAttrName) = 0;

   // Get the ECU this node belongs to. All nodes which belong to the same ECU return the same VIAECU pointer
   VIASTDDECL GetECU( VIAECU** pECU) = 0;
#endif 

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 11)
   // As of version 1.20, don't use these two functions any more. The database access
   // has been reworked; nodelayers shouldn't know about databases.
   VIASTDDECL GetDatabaseName( char* pBuffer, int32 bufferLength) =0;
   VIASTDDECL GetDatabasePath( char* pBuffer, int32 bufferLength) =0;
#endif // defined ( VIAMinorVersion) && ( VIAMinorVersion >= 11)

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)

   // Retrieve messages which are sent _by this node_. Other messages can not be retrieved
   // through these functions; use the message retrieval of VIABus instead.
   VIASTDDECL GetMessage(unsigned long ID, 
                         VIDBMessageDefinition** message) = 0;
   // You should retrieve messages using the ID. Use the name only if the ID is not available.
   VIASTDDECL GetMessage(const char* name, 
                         VIDBMessageDefinition** message) = 0;
  
   VIDB_DECLARE_ITERATABLE(MessageDefinition)
   VIDB_DECLARE_ATTRIBUTE_HOLDER

#endif
}; // class VIANode

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 20)

// ============================================================================
// Interface to message and signal definitions
// ============================================================================

/*
 * Definition of a message. Through the message, you can access the signals it
 * contains. You can get the messages through VIANode or the VIABus subclasses.
 */
class VIDBMessageDefinition
{
public:
   VIASTDDECL GetID(unsigned long& ID) const = 0;
   VIASTDDECL GetName(char* buffer, int32 bufferLength) const = 0;
   VIASTDDECL GetDLC(unsigned short& dlc) const = 0;

   VIASTDDECL GetSignal(const char* name, 
                        VIDBSignalDefinition** signal) = 0;
   
   VIDB_DECLARE_ITERATABLE(SignalDefinition)
   VIDB_DECLARE_ATTRIBUTE_HOLDER
   VIASTDDECL Release() = 0;
};

/*
 * Definition of a signal. Apart from attribute retrieval, it also provides
 * functions to access the value of the signal in a real message.
 */
class VIDBSignalDefinition
{
public:
    VIASTDDECL GetName(char* buffer, int32 bufferLength) const = 0;

    VIASTDDECL GetMinValue(double& minValue) const = 0;
    VIASTDDECL GetMaxValue(double& maxValue) const = 0;

    VIASTDDECL GetPhysicalValue(unsigned char* rawData, 
                                double& physValue) const = 0;
    VIASTDDECL GetRawValue(unsigned char* rawData, 
                           ::uint64& rawValue) const = 0;

    VIASTDDECL SetPhysicalValue(double physValue, 
                                unsigned char* rawData) const = 0;
    VIASTDDECL SetRawValue(::uint64 rawValue, 
                           unsigned char* rawData) const = 0;

    VIDB_DECLARE_ATTRIBUTE_HOLDER
    VIASTDDECL Release() = 0;
};

/*
 * Definition of a node. This is only meant for some special nodelayers which 
 * must have access to the attributes of other nodes on the bus. For the 
 * node corresponding to the nodelayer itself, use VIANode directly. You can
 * get the NodeDefinitions through the VIABus subclasses (e.g. VIACan).
 */
class VIDBNodeDefinition
{
public:
    VIASTDDECL GetName(char* buffer, int32 bufferLength) const = 0;

    VIDB_DECLARE_ATTRIBUTE_HOLDER
    VIASTDDECL Release() = 0;
};

// explicit template specializations
template class VIDBIterator<VIDBAttribute>;
template class VIDBIterator<VIDBMessageDefinition>;
template class VIDBIterator<VIDBSignalDefinition>;
template class VIDBIterator<VIDBNodeDefinition>;

#endif // minor version >= 20

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
const uint32 kVIA_DiVaSupport     = 0x00000020; // Licence for option DiVa exists
#endif 



class VIAService
{
public:

   // =============== Configuration items =====================================


   // Version of this implementation of VIAService
   VIASTDDECL GetVersion (int32* major, int32* minor, int32* patchlevel) =0;

   // Get the windows handle for the main window of canoe. You can use
   // this as the parent handle for windows message boxes.
   VIASTDDECL GetClientWindow(HWND* handle) =0; 


   // With 'GetConfigItem(...)' you can ask CANoe much about its configuration
   // like used databeses, filename of the configuration, etc. You specifie
   // the item by a main topic and a sub topic. The topics are enumerated below
   // (see enum VIA_ConfigTopic). The meaning of sub topic depends on the main 
   // topic. But often the sub topic has no special meaning and should be 
   // set to zero in this case.
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

   VIASTDDECL GetConfigItem(uint32 topic,            // main topic 
                            uint32 subtopic,         // sub topic
                            char*  buffer,           // buffer for the result
                            int32  bufferLength) =0; // size of buffer 


   // =============== Interface to database ===================================
   // As of version 1.20, please don't use this interface any more! It has been
   // replaced by the access functions in the subclasses of VIABus.
   
   enum VIA_DBObject {           // object type for a ...
      kVIA_DBDatabase  = 151,    // ...  database attribute
      kVIA_DBNode      = 152,    // ...  node attribute
      kVIA_DBMessage   = 153,    // ...  message attribute
      kVIA_DBSignal    = 154,    // ...  signal attribute
      kVIA_DBEnvVar    = 155     // ...  environment variable attribute
   };

   enum VIA_DBType {
      kVIA_DBFloat     = 160,    // attribute type for attributes of type Float or Integer 
      kVIA_DBString    = 165     // attribute type for attributes of type String 
   };

   // Get the Type (kVIA_DBFloat or kVIA_DBString) of a attribute
   VIASTDDECL GetDBAttributeType (uint32*     attributeType, // the requested type (enum VIA_DBType)
                                  uint32      objectType,    // see enum VIA_DBObject
                                  const char* objectName,    // 
                                  const char* attrName,      // name of attribute
                                  const char* dbName=NULL)=0;// database name

   // Get the value of an attribute of type kVIA_DBFloat
   VIASTDDECL GetDBAttributeValue  (double*     attributeValue,  // the requested value
                                    uint32      objectType,      // see enum VIA_DBObject
                                    const char* objectName,      // 
                                    const char* attrName,        // name of attribute
                                    const char* dbName=NULL )=0; // database name 
                                     
   // Get the value of an attribute of type kVIA_DBString
   VIASTDDECL GetDBAttributeString ( char*       buffer,          // buffer to copy the requested string
                                     int32       bufferLength,    // size of buffer
                                     uint32      objectType,      // see enum VIA_DBObject
                                     const char* objectName,      // 
                                     const char* attrName,        // name of attribute
                                     const char* dbName=NULL )=0; // database name


   // =============== Interface to timer ======================================


   // Create a timer object. If a node is specified, the timer is 
   // affected by the drift and jitter used by this node.
   VIASTDDECL CreateTimer(VIATimer**      timer,    // the created object
                          VIANode*        node,     // network node
                          VIAOnTimerSink* sink,     // callback object (NULL allowed)
                          const char*     name) =0; // name of the timer
                           
   // release the timer, so that VIAService can delete the object 
   VIASTDDECL ReleaseTimer (VIATimer* timer)  =0;


   // =============== Interface to environmet variables  ======================


   // Access to environment variables, GetEnvVar(...) creates an interface object
   // for a single environment variable
   VIASTDDECL GetEnvVar (VIAEnvVar**  ev,      // the created object
                         VIANode*     node,    // node
                         const char*  name,    // name of the variable
                         VIAOnEnvVar* sink)=0; // callback object (NULL allowed)

   // Release the object created by GetEnvVar(...)
   VIASTDDECL ReleaseEnvVar (VIAEnvVar* ev) =0;


   // =============== Interface to bus ========================================


   // Get an interface object to a bus system. The type of the interface is
   // specified by the parameter interfaceType. At the moment, only kVIA_CAN 
   // is supported. 
   VIASTDDECL GetBusInterface(VIABus** busInterface,    // the interface object
                              VIANode* node,            // node, which will use the interface
                              uint32   interfaceType,   // see enum VIABusInterfaceType
                              int32    majorversion,    // the required version of
                              int32    minorversion)=0; //   the bus interface
                                
   // Release the object created by 'GetBusInterface(...)'
   VIASTDDECL ReleaseBusInterface(VIABus* busInterface) =0;


   // =============== utility services=========================================

   VIASTDDECL GetUtilService( VIAUtil** service,        // the service object
                              int32     majorversion,   // the required version 
                              int32     minorversion)=0;//  the util service

   VIASTDDECL ReleaseUtilService(VIAUtil* service)=0;


   // =============== misc ====================================================


   // Output text to the "Write Window"
   VIASTDDECL WriteString (const char* text) =0;

   // Output text to a logging file
   VIASTDDECL WriteToLog  (const char* text) =0;

   // Popup the well known assertion box of CANalyzer/CANoe
   VIASTDDECL Assertion(char* message, char* condition, char* file, int32 line) =0;
   
   // Popup a Message Box 
   VIASTDDECL MsgBox(char* message) =0;
   
   // Status of the current Measurement, 
   // returns kVIA_OK or kVIA_ServiceNotRunning
   VIASTDDECL RtKernelIsRunning() =0;
   

   // Use GetCurrentNode() within a call from a capl function to get the
   // the node, which called the function.
   VIASTDDECL GetCurrentNode (VIANode** node) =0; 


   // Use GetCurrentNodeLayer() within a call from a capl function to get the 
   // nodelayer object of this given module, belonging to the current node
   VIASTDDECL GetCurrentNodeLayer (VIANodeLayerApi** nodelayer,  // current nodleayer
                                   VIAModuleApi* module) =0;     // module

   // Timestamp of the event, that is processed at the moment (ringing timer,  
   // bus event, environment variable was set, ...)
   VIASTDDECL GetCurrentSimTime (VIATime* time) =0; 

   // Stop the current measurement
   VIASTDDECL Stop()=0;       
   
   // Something very important for the OSEK-Emulation
   VIASTDDECL GetSystemFiber(LPVOID* fiber)=0;
                     
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 10)

   // Get special flags (operation modes, magics and so on)
   // See kVIA_MultiBusSupport, ...
   VIASTDDECL GetServiceFlags( uint32* flags) = 0;

#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 12)

   // Type to define the severity of an output to the write window
   typedef enum
   {
      kVIA_WriteSeveritySuccess       = 0,
      kVIA_WriteSeverityInformational = 1,
      kVIA_WriteSeverityWarnung       = 2,
      kVIA_WriteSeverityError         = 3,

   } VIAWriteSeverity;

#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 13)
   // Type to define some predefined write sinks that are generally available
   // For these sinks, there is no need to perform the CreateWriteTab() 
   // and the ReleaseWriteTab()
   typedef enum
   {
     // special sinks
     kVIA_WriteSinkTrace              = -3,
     kVIA_WriteSinkLog                = -2,
     kVIA_WriteSinkDebug              = -1,

     // real write tab sinks
     kVIA_WriteSinkTabSystem          = 0,
     kVIA_WriteSinkTabCAPL            = 1,
     kVIA_WriteSinkTabTest            = 4,

   } VIAWriteSinksPredefined;

#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 12)
   // Create a tab in the write-window with aSinkName. Return a handle onto this tab.
   // With this handle, subsequent writes are possible
   VIASTDDECL CreateWriteTab(uint32* aSink, const char* aSinkName) = 0;

   // Release a previously created tab
   VIASTDDECL ReleaseWriteTab(uint32 aSink) = 0;

   // Outputs text to a previously created tab in the "write window"
   VIASTDDECL WriteStringToTab(uint32 aSink, VIAWriteSeverity aSeverity, const char* aText) = 0;

   // Resolves the api-object for controlling tests
   VIASTDDECL GetTestControlApi( VIATestControlApi** apTestControlObject,   // Supplied Api-object
                                 VIANode*        node) = 0;                 // that matches the current node

   // Releases the Api again
   VIASTDDECL ReleaseTestControlApi( VIATestControlApi* apTestControlObject) = 0;

#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 13)
   
   // Clear a previously created tab
   VIASTDDECL ClearWriteTab(uint32 aSink) = 0;

#endif

#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 14)
   
   // Register the Node Layer for service activity
   VIASTDDECL SetNLServiceApi( VIANLServiceApi* apNLServiceMember,
                               VIANode* apMyNode) = 0;

   // Register a specific service of a provider
   VIASTDDECL ProvideNLService( int8 aMultiUserService,               // single or multiple access to the service
                                VIANLService* apServiceToProvide,
                                VIANode* apMyNode,
                                VIANLServiceApi* apNLServiceProvider) = 0;

   // Register the acquire of a user for a specific service
   VIASTDDECL AcquireNLService( const char* apServiceName,
                                int32 aInterfaceVersion,
                                VIANLService** appService,
                                VIANode* apMyNode,
                                VIANLServiceApi* apNLServiceUser) = 0;

   // Cancel a specific service of a provider
   VIASTDDECL CancelNLService( VIANLService* apServiceToCancel,
                               VIANode* apMyNode,
                               VIANLServiceApi* apNLServiceProvider) = 0;

   // Cancel the acquire of a user for a specific service
   VIASTDDECL ReleaseNLService( VIANLService* apServiceToRelease,
                                VIANode* apMyNode,
                                VIANLServiceApi* apNLServiceUser) = 0;

#endif
  
#if defined ( VIAMinorVersion) && ( VIAMinorVersion >= 17)
   
   // Resolves the Api object for signal access (no release neccessary)
   // This function requires to be called during init measurement phase
   // of the VIANodeLayerApi object.
   VIASTDDECL GetSignalAccessApi( VIASignalAccessApi** aSignalAccessApi,
                                  VIANode* aNode,
                                  int32 majorversion,
                                  int32 minorversion) = 0;

#endif

}; // class VIAService




// ==========================================================================
// class VIANodeLayerApi 
//
// Interface of the nodelayer, which is called by the runtime kernel.
// This class must be implemented by the nodelayer-DLL.
// ==========================================================================



class VIANodeLayerApi
{
public:
   // Get the Node, to which this Layer belongs. This should be the handle,
   // that was provided by the call of VIAModuleApi::CreateObject(...)
   VIASTDDECL GetNode(VIANode** node) =0;

   VIASTDDECL InitMeasurement  () =0;
   VIASTDDECL StartMeasurement () =0;
   VIASTDDECL StopMeasurement  () =0;
   VIASTDDECL EndMeasurement   () =0;  
}; // class VIANodeLayerApi


// ==========================================================================
// class VIAModuleApi 
//
// Interface of the module, which is called by the runtime kernel.
//
// This class, together with the functions VIAGetModuleApi and 
// VIAReleaseModuleApi, must be implemented by the nodelayer-DLL.
// ==========================================================================



class VIAModuleApi
{
public:
   // Initialize the nodelayer. Here you can do additional setup of the DLL.
   // CANoe calls this method after it ask for the object (function 
   // 'VIAGetModuleApi') and before it calls any other method of this object.
   VIASTDDECL Init()=0;                     
   
   // The text provided by GetVersion(...) is displayed in the info window
   // This is normally the name and version of this DLL.
   VIASTDDECL GetVersion (char* buffer, int32 bufferLength ) =0;

   // With a Shift-DoubleClick in the Info-Window, the following information 
   // is displayed in the Write-Window.
   // pathBuff: filename und path of this nodelayer
   // versionBuff: name und versionnumber of this nodelayer
   VIASTDDECL GetModuleParameters (char* pathBuff,    int32 pathBuffLength,
                                   char* versionBuff, int32 versionBuffLength) =0;

   // The information provided by GetNodeInfo(...) appears within the node
   // at the simulation setup window (shortName) and the status indication
   // line of CANoe (longName) .
   VIASTDDECL GetNodeInfo (const char* nodename,      // name of the node
                           char* shortNameBuf, int32 shortBufLength,
                           char* longNameBuf,  int32 longBufLength ) = 0;


   // Create a nodelayer object for the given node
   VIASTDDECL CreateObject  (VIANodeLayerApi** object,    // the created instance
                             VIANode* node,               // the node
                             int32 argc, char* argv[]) =0;  // additional parameters

   // Release the given nodelayer object
   VIASTDDECL ReleaseObject (VIANodeLayerApi*  object) =0;


   VIASTDDECL InitMeasurement () =0;
   VIASTDDECL EndMeasurement  () =0;
}; // class VIAModuleApi

// ==========================================================================
// setup functions
//
// The following C-functions are for loading, unloading and initializing 
// of the DLL. They are called just after loading or yust befor unloading of 
// the DLL.
// ==========================================================================

// CANoe calls the function 'VIARequiredVersion' to ask the nodelayer DLL,
// with which version of the VIA-interface it was compiled.
// This is the first function, that CANoe calls after loading the DLL.
VIACLIENT(void) VIARequiredVersion (int32* majorversion, int32* minorversion);


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


#endif // VIA_H

