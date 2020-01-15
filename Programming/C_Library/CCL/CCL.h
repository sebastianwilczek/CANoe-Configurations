/*------------------------------------------------------------------ 
<FILE> 
  <NAME> CCL.h
  <PRJ > CANalyzer/CANoe C Library API (CCL)
  <REV > 1.11.0
  <DATE> 2019-07-03
  <DESC> 
   The header file declares the API of an CANalyzer/CANoe C Library (CCL).
  <COPYRIGHT>(c) 2011 by Vector Informatik GmbH.  All rights reserved.
  <HISTORY> 
   1.0  2011-01-24  First version of the API created.
   1.1  2011-02-11  cclPrintf added
   1.2  2014-04-04  Signal access added
   1.3  2014-06-12  cclGetUserFilePath added
   1.4  2014-06-25  Structures for system variables supported
   1.5  2014-12-09  cclSysVarGetType added
   1.6  2015-08-15  System variable (struct members) physical values supported
   1.7  2017-04-21  Function Bus API added, upgrade to VS 2013
   1.8  2017-06-28  System variable Integer64 values supported
   1.9  2017-10-16  Function Bus Service Field support
   1.10 2018-09-25  cclCanGetChannelNumber and cclLinGetChannelNumber added
   1.11 2019-07-03  Support for CAN FD added
</FILE>
----------------------------------------------------------------- */

#ifndef CCL_H
#define CCL_H

#include <stdbool.h>

#if defined __cplusplus
extern "C" {
#endif

// ============================================================================
// Error Codes
// ============================================================================

const int CCL_SUCCESS                       =   0;
const int CCL_WRONGSTATE                    =  -1;
const int CCL_INTERNALERROR                 =  -2;
const int CCL_INVALIDCHANNEL                =  -3;
const int CCL_INVALIDTIMERID                =  -4;
const int CCL_INVALIDFUNCTIONPOINTER        =  -5;
const int CCL_INVALIDTIME                   =  -6;
const int CCL_INVALIDNAME                   =  -7;
const int CCL_SYSVARNOTDEFINED              =  -8;
const int CCL_INVALIDSYSVARID               =  -9;
const int CCL_WRONGTYPE                     = -10;
const int CCL_SYSVARISREADONLY              = -11;
const int CCL_BUFFERTOSMALL                 = -12;
const int CCL_WRONGARRAYSIZE                = -13;
const int CCL_NOTIMPLEMENTED                = -14;
const int CCL_SIGNALNOTDEFINED              = -15;
const int CCL_SIGNALAMBIGUOUS               = -16;
const int CCL_INVALIDSIGNALID               = -17;
const int CCL_USERFILENOTDEFINED            = -18;
const int CCL_PARAMETERINVALID              = -19;
const int CCL_INVALIDPORTPATH               = -20;
const int CCL_INVALIDMEMBERPATH             = -21;
const int CCL_INVALIDVALUEID                = -23;
const int CCL_INVALIDFUNCTIONPATH           = -24;
const int CCL_INVALIDFUNCTIONID             = -25;
const int CCL_INVALIDCALLCONTEXTID          = -26;
const int CCL_INVALIDSERVICEPATH            = -27;
const int CCL_INVALIDSERVICEID              = -28;
const int CCL_INVALIDADDRESSID              = -29;
const int CCL_INVALIDCONSUMERPATH           = -30;
const int CCL_INVALIDCONSUMERID             = -31;
const int CCL_INVALIDPROVIDERPATH           = -32;
const int CCL_INVALIDPROVIDERID             = -33;
const int CCL_INVALIDCONSUMEDSERVICEPATH    = -34;
const int CCL_INVALIDCONSUMEDSERVICEID      = -35;
const int CCL_INVALIDPROVIDEDSERVICEPATH    = -36;
const int CCL_INVALIDPROVIDEDSERVICEID      = -37;
const int CCL_INVALIDPDUSENDERPATH          = -38;
const int CCL_INVALIDPDUSENDERID            = -39;
const int CCL_INVALIDPDURECEIVERPATH        = -40;
const int CCL_INVALIDPDURECEIVERID          = -41;
const int CCL_INVALIDCONSUMEDPDUPATH        = -42;
const int CCL_INVALIDCONSUMEDPDUID          = -43;
const int CCL_INVALIDPROVIDEDPDUPATH        = -44;
const int CCL_INVALIDPROVIDEDPDUID          = -45;
const int CCL_INVALIDPDUPROVIDERPATH        = -46;
const int CCL_INVALIDPDUPROVIDERID          = -47;
const int CCL_INVALIDCONSUMEDEVENTPATH      = -48;
const int CCL_INVALIDCONSUMEDEVENTID        = -49;
const int CCL_INVALIDPROVIDEDEVENTPATH      = -50;
const int CCL_INVALIDPROVIDEDEVENTID        = -51;
const int CCL_INVALIDEVENTPROVIDERPATH      = -52;
const int CCL_INVALIDEVENTPROVIDERID        = -53;
const int CCL_INVALIDCONSUMEDEVENTGROUPPATH = -54;
const int CCL_INVALIDCONSUMEDEVENTGROUPID   = -55;
const int CCL_INVALIDPROVIDEDEVENTGROUPPATH = -56;
const int CCL_INVALIDPROVIDEDEVENTGROUPID   = -57;
const int CCL_INVALIDEVENTGROUPPROVIDERPATH = -58;
const int CCL_INVALIDEVENTGROUPPROVIDERID   = -59;
const int CCL_TOOMANYCALLCONTEXTS           = -60;
const int CCL_INVALIDDOMAIN                 = -61;
const int CCL_VALUEREADONLY                 = -62;
const int CCL_VALUENOTOPTIONAL              = -63;
const int CCL_INVALIDSIDE                   = -64;
const int CCL_INVALIDCALLCONTEXTSTATE       = -65;
const int CCL_INVALIDBINDING                = -66;
const int CCL_CANNOTADDDYNAMICCONSUMER      = -67;
const int CCL_CANNOTADDDYNAMICPROVIDER      = -68;
const int CCL_CONSUMERNOTFOUND              = -69;
const int CCL_PROVIDERNOTFOUND              = -70;
const int CCL_NOTSUPPORTED                  = -71;
const int CCL_CALLCONTEXTEXPIRED            = -72;
const int CCL_VALUENOTACCESSIBLE            = -73;
const int CCL_STATICCONSUMER                = -74;
const int CCL_STATICPROVIDER                = -75;
const int CCL_CONSUMERNOTBOUND              = -76;
const int CCL_PROVIDERNOTBOUND              = -77;
const int CCL_CANNOTCREATEADDRESS           = -78;
const int CCL_WRONGBINDING                  = -79;
const int CCL_INVALIDDATA                   = -80;
const int CCL_ARRAYNOTRESIZABLE             = -81;
const int CCL_UNSUPPORTEDTYPE               = -82;
const int CCL_INVALIDSIGNALNAME             = -83;
const int CCL_INVALIDCONSUMEDFIELDPATH      = -84;
const int CCL_INVALIDPROVIDEDFIELDPATH      = -85;
const int CCL_INVALIDFIELDPROVIDERPATH      = -86;
const int CCL_FIELDNOTSUBSCRIBABLE          = -87;
const int CCL_INVALIDCONSUMEDFIELDID        = -88;
const int CCL_INVALIDPROVIDEDFIELDID        = -89;
const int CCL_INVALIDFIELDPROVIDERID        = -90;
const int CCL_PROVIDEDFIELDDISPOSED         = -91;
const int CCL_NETWORKNOTDEFINED             = -92;
// ============================================================================
// Common defines 
// ============================================================================

const unsigned char CCL_DIR_RX   = 0;
const unsigned char CCL_DIR_TX   = 1;
const unsigned char CCL_DIR_TXRQ = 2;

// ============================================================================
// General Enumerations
// ============================================================================
typedef enum
{
  CCL_COMMUNICATION_OBJECTS
} cclDomain;

// ============================================================================
// General Functions
// ============================================================================

extern void cclOnDllLoad();

extern void cclSetMeasurementPreStartHandler(void (*function)());
extern void cclSetMeasurementStartHandler(void (*function)());
extern void cclSetMeasurementStopHandler(void (*function)());
extern void cclSetDllUnloadHandler(void (*function)());

extern void cclWrite(const char* text);
extern void cclPrintf(const char* format, ...);

extern int cclGetUserFilePath(const char* filename,
                              char* pathBuffer,
                              int pathBufferLength);

// ============================================================================
// Time & Timer 
// ============================================================================

extern long long cclTimeSeconds(long long seconds);
extern long long cclTimeMilliseconds(long long milliseconds);
extern long long cclTimeMicroseconds(long long microseconds);

extern int cclTimerCreate( void (*function)(long long time, int timerID) );
extern int cclTimerSet(int timerID, long long nanoseconds);
extern int cclTimerCancel(int timerID);

// ============================================================================
// System Variables
// ============================================================================
const int CCL_SYSVAR_INTEGER      = 1;
const int CCL_SYSVAR_FLOAT        = 2;
const int CCL_SYSVAR_STRING       = 3;
const int CCL_SYSVAR_DATA         = 4;
const int CCL_SYSVAR_INTEGERARRAY = 5;
const int CCL_SYSVAR_FLOATARRAY   = 6;
const int CCL_SYSVAR_STRUCT       = 7;
const int CCL_SYSVAR_GENERICARRAY = 8;

extern int cclSysVarGetID(char* systemVariablename);
extern int cclSysVarGetType(int sysVarID);
extern int cclSysVarSetHandler(int sysVarID,
                               void (*function)(long long time, int sysVarID) );
extern int cclSysVarGetArraySize(int sysVarID);
extern int cclSysVarSetInteger(int sysVarID, long x);
extern int cclSysVarGetInteger(int sysVarID, long* x);
extern int cclSysVarSetInteger64(int sysVarID, long long x);
extern int cclSysVarGetInteger64(int sysVarID, long long* x);
extern int cclSysVarSetFloat(int sysVarID, double x);
extern int cclSysVarGetFloat(int sysVarID, double* x);
extern int cclSysVarSetString(int sysVarID, const char* text);
extern int cclSysVarGetString(int sysVarID, char* buffer, int bufferLenght);
extern int cclSysVarSetIntegerArray(int sysVarID, const long* x, int length);
extern int cclSysVarGetIntegerArray(int sysVarID, long* x, int length);
extern int cclSysVarSetFloatArray(int sysVarID, const double* x, int length);
extern int cclSysVarGetFloatArray(int sysVarID, double* x, int length);
extern int cclSysVarSetData(int sysVarID, const unsigned char* x, int length);
extern int cclSysVarGetData(int sysVarID, unsigned char* x, int length);
extern int cclSysVarSetPhysical(int sysVarID, double x);
extern int cclSysVarGetPhysical(int sysVarID, double* x);

// ============================================================================
// Signal
// ============================================================================

extern int cclSignalGetID(char* signalname);
extern int cclSignalSetHandler(int signalID, void (*function)(int signalID) );

extern int cclSignalGetRxPhysDouble(int signalID, double* value);
extern int cclSignalGetRxRawInteger(int signalID, long long* value);
extern int cclSignalGetRxRawDouble(int signalID, double* value);

extern int cclSignalSetTxPhysDouble(int signalID, double value);
extern int cclSignalSetTxRawInteger(int signalID, long long value);
extern int cclSignalSetTxRawDouble(int signalID, double value);

extern int cclSignalGetTxPhysDouble(int signalID, double* value);
extern int cclSignalGetTxRawInteger(int signalID, long long* value);
extern int cclSignalGetTxRawDouble(int signalID, double* value);

// ============================================================================
// CAN 
// ============================================================================

const unsigned int CCL_CANFLAGS_RTR    = 1;
const unsigned int CCL_CANFLAGS_WAKEUP = 2;
const unsigned int CCL_CANFLAGS_TE     = 4;
const unsigned int CCL_CANFLAGS_FDF    = 8;
const unsigned int CCL_CANFLAGS_BRS    = 16;
const unsigned int CCL_CANFLAGS_ESI    = 32;

extern int cclCanOutputMessage(int channel,
                               unsigned int identifier,
                               unsigned int flags,
                               unsigned char dataLength,
                               const unsigned char data[]);


const unsigned int CCL_CAN_ALLMESSAGES = 0xFFFFFFFF;

struct cclCanMessage
{
  long long     time;
  int           channel;
  unsigned int  id;
  unsigned int  flags;
  unsigned char dir;
  unsigned char dataLength;
  unsigned char data[64];
};

extern int cclCanSetMessageHandler(int channel,
                                   unsigned int identifier,
                                   void (*function) (struct cclCanMessage* message) );

extern unsigned int cclCanMakeExtendedIdentifier(unsigned int identifier);
extern unsigned int cclCanMakeStandardIdentifier(unsigned int identifier);
extern unsigned int cclCanValueOfIdentifier(unsigned int identifier);
extern int cclCanIsExtendedIdentifier(unsigned int identifier);
extern int cclCanIsStandardIdentifier(unsigned int identifier);
extern int cclCanGetChannelNumber(const char* networkName, int* channel);
// ============================================================================
// LIN
// ============================================================================

struct cclLinFrame
{
  long long     timestampEOF;
  long long     timestampSOF;
  long long     timestampEOH;
  long long     timeSyncBreak;
  long long     timeSyncDel;
  int           channel;
  unsigned int  id;
  unsigned char dir;
  unsigned char dlc;
  unsigned char data[8];
};

const unsigned int CCL_LIN_ALLMESSAGES = 0xFFFFFFFF;

extern int cclLinSetFrameHandler(int channel,
                                 unsigned int identifier,
                                 void (*function) (struct cclLinFrame* frame) );


extern int  cclLinUpdateResponseData(int channel,
                                     unsigned int id,
                                     unsigned char dlc,
                                     unsigned char data[8]);

extern int  cclLinSendHeader(int channel, unsigned int id);
extern int  cclLinStartScheduler(int channel);
extern int  cclLinStopScheduler(int channel);
extern int  cclLinChangeSchedtable(int channel, unsigned int tableIndex);
extern int  cclLinGetChannelNumber(const char* networkName, int* channel);
// ============================================================================
// Function Bus
// ============================================================================

// types
typedef long long cclTime;

const cclTime CCL_TIME_OFFSET_NEVER = -1LL;
const cclTime CCL_TIME_OFFSET_NOW = 0LL;

// enumerations
typedef enum
{
  CCL_IMPL,
  CCL_RAW,
  CCL_PHYS
} cclValueRepresentation;

typedef enum
{
  CCL_SERVICE_STATE_UNAVAILABLE,
  CCL_SERVICE_STATE_INITIALIZING,
  CCL_SERVICE_STATE_AVAILABLE
} cclServiceStateProvider;

typedef enum
{
  CCL_CONNECTION_STATE_CONSUMER_UNAVAILABLE,
  CCL_CONNECTION_STATE_CONSUMER_CONNECTABLE,
  CCL_CONNECTION_STATE_CONSUMER_AVAILABLE
} cclConnectionStateConsumer;

typedef enum
{
  CCL_PROVIDED_CONNECTION_STATE_UNAVAILABLE,
  CCL_PROVIDED_CONNECTION_STATE_CONNECTABLE,
  CCL_PROVIDED_CONNECTION_STATE_CONNECTED
} cclProvidedConnectionState;

typedef enum
{
  CCL_SUBSCRIPTION_STATE_UNAVAILABLE,
  CCL_SUBSCRIPTION_STATE_AVAILABLE,
  CCL_SUBSCRIPTION_STATE_SUBSCRIBED
} cclSubscriptionState;

typedef enum
{
  CCL_BEHAVIOR_USE_DEFAULT_VALUE,
  CCL_BEHAVIOR_USE_IN_VALUE
} cclParameterBehavior;

typedef enum
{
  CCL_ANSWER_MODE_AUTO,
  CCL_ANSWER_MODE_SUSPEND,
  CCL_ANSWER_MODE_DISCARD
} cclAnswerMode;

typedef enum
{
  CCL_CALL_STATE_CALLING,
  CCL_CALL_STATE_CALLED,
  CCL_CALL_STATE_RETURNING,
  CCL_CALL_STATE_RETURNED
} cclCallState;

typedef enum
{
  CCL_SIDE_CONSUMER,
  CCL_SIDE_PROVIDER
} cclSide;

typedef enum
{
  CCL_VALUE_TYPE_INTEGER,
  CCL_VALUE_TYPE_FLOAT,
  CCL_VALUE_TYPE_STRING,
  CCL_VALUE_TYPE_ARRAY,
  CCL_VALUE_TYPE_STRUCT,
  CCL_VALUE_TYPE_UNION,
  CCL_VALUE_TYPE_ENUM
} cclValueType;

typedef enum
{
  CCL_VALUE_STATE_OFFLINE,
  CCL_VALUE_STATE_MEASURED
} cclValueState;

// string constants for member paths
#define CCL_MEMBER_SERVICE_STATE      ".ServiceState"
#define CCL_MEMBER_CONNECTION_STATE   ".ConnectionState"
#define CCL_MEMBER_SUBSCRIPTION_STATE ".SubscriptionState"
#define CCL_MEMBER_LATEST_CALL        ".LatestCall"
#define CCL_MEMBER_LATEST_RETURN      ".LatestReturn"
#define CCL_MEMBER_IN_PARAM           ".In"
#define CCL_MEMBER_OUT_PARAM          ".Out"
#define CCL_MEMBER_RESULT             ".Result"
#define CCL_MEMBER_PARAM_DEFAULTS     ".ParamDefaults"
#define CCL_MEMBER_DEFAULT_RESULT     ".DefaultResult"
#define CCL_MEMBER_VALUE              ".Value"
#define CCL_MEMBER_BEHAVIOR           ".Behavior"
#define CCL_MEMBER_AUTO_ANSWER_MODE   ".AutoAnswerMode"
#define CCL_MEMBER_AUTO_ANSWER_TIME   ".AutoAnswerTimeNS"
#define CCL_MEMBER_CALL_TIME          ".CallTime"
#define CCL_MEMBER_RETURN_TIME        ".ReturnTime"
#define CCL_MEMBER_CALL_STATE         ".State"
#define CCL_MEMBER_REQUEST_ID         ".ReqID"
#define CCL_MEMBER_SIDE               ".Side"
#define CCL_MEMBER_CONSUMER_PORT      ".ConsumerPort"
#define CCL_MEMBER_PROVIDER_PORT      ".ProviderPort"

// Value Entity access
typedef int cclValueID;
cclValueID cclValueGetID(cclDomain domain, const char* path, const char* member, cclValueRepresentation repr);

int cclValueGetInteger(cclValueID valueID, long long* x);
int cclValueSetInteger(cclValueID valueID, long long x);
int cclValueGetFloat(cclValueID valueID, double* x);
int cclValueSetFloat(cclValueID valueID, double x);
int cclValueGetString(cclValueID valueID, char* buffer, int bufferSize);
int cclValueSetString(cclValueID valueID, const char* str);
int cclValueGetData(cclValueID valueID, unsigned char* buffer, int* bufferSize);
int cclValueSetData(cclValueID valueID, const unsigned char* data, int size);
int cclValueGetEnum(cclValueID valueID, int* x);
int cclValueSetEnum(cclValueID valueID, int x);
int cclValueGetUnionSelector(cclValueID valueID, int* selector);
int cclValueGetArraySize(cclValueID valueID, int* size);
int cclValueSetArraySize(cclValueID valueID, int size);

int cclValueGetType(cclValueID valueID, cclValueType* valueType);
int cclValueIsValid(cclValueID valueID, bool* isValid);
int cclValueClear(cclValueID valueID);

int cclValueGetUpdateTimeNS(cclValueID valueID, cclTime* time);
int cclValueGetChangeTimeNS(cclValueID valueID, cclTime* time);
int cclValueGetState(cclValueID valueID, cclValueState* state);

typedef void(*cclValueHandler)(cclTime time, cclValueID valueID);
int cclValueSetHandler(cclValueID valueID, bool onUpdate, cclValueHandler handler);

// Network Function Calls
typedef int cclFunctionID;
cclFunctionID cclFunctionGetID(const char* path);

typedef int cclCallContextID;
cclCallContextID cclCreateCallContext(cclFunctionID functionID);
cclCallContextID cclCallContextMakePermanent(cclCallContextID ccID);
int cclCallContextRelease(cclCallContextID ccID);

cclValueID cclCallContextValueGetID(cclCallContextID ccID, const char* member, cclValueRepresentation repr);

typedef void(*cclCallHandler)(cclTime time, cclCallContextID ccID);
int cclCallAsync(cclCallContextID ccID, cclCallHandler resultHandler);

int cclFunctionSetHandler(cclFunctionID functionID, cclCallState callState, cclCallHandler handler);
int cclCallContextReturn(cclCallContextID ccID, cclTime timeOffset);
int cclCallContextSetDefaultAnswer(cclCallContextID ccID);
int cclCallContextGetFunctionID(cclCallContextID ccID, cclFunctionID* functionID);
int cclCallContextGetFunctionName(cclCallContextID ccID, char* buffer, int bufferSize);
int cclCallContextGetFunctionPath(cclCallContextID ccID, char* buffer, int bufferSize);

// Service access
typedef int cclServiceID;
typedef int cclConsumerID;
typedef int cclProviderID;
typedef int cclConsumedServiceID;
typedef int cclProvidedServiceID;
typedef int cclAddressID;

const cclConsumerID CCL_CONSUMER_NONE = -1;
const cclProviderID CCL_PROVIDER_NONE = -1;

cclServiceID cclServiceGetID(const char* path);

int cclServiceSetSimulator(cclProvidedServiceID providedService, cclCallHandler simulator);

cclConsumerID cclAddConsumerByName(cclServiceID serviceID, const char* name, bool isSimulated);
cclProviderID cclAddProviderByName(cclServiceID serviceID, const char* name, bool isSimulated);
cclConsumerID cclAddConsumerByAddress(cclServiceID serviceID, cclAddressID addressID);
cclProviderID cclAddProviderByAddress(cclServiceID serviceID, cclAddressID addressID);
int cclRemoveConsumer(cclServiceID serviceID, cclConsumerID consumerID);
int cclRemoveProvider(cclServiceID serviceID, cclProviderID providerID);
cclConsumerID cclGetConsumer(cclServiceID serviceID, long bindingID);
cclProviderID cclGetProvider(cclServiceID serviceID, long bindingID);
cclConsumedServiceID cclGetConsumedService(cclServiceID serviceID, long consumerID, long providerID);
cclProvidedServiceID cclGetProvidedService(cclServiceID serviceID, long consumerID, long providerID);

cclConsumerID cclConsumerGetID(const char* path);
cclProviderID cclProviderGetID(const char* path);
int cclProvideService(cclProviderID providerID);
int cclReleaseService(cclProviderID providerID);
int cclIsServiceProvided(cclProviderID providerID, bool* isProvided);
int cclConsumerGetBindingID(cclConsumerID consumerID, long* bindingID);
int cclProviderGetBindingID(cclProviderID providerID, long* bindingID);

cclConsumedServiceID cclConsumedServiceGetID(const char* path);
cclProvidedServiceID cclProvidedServiceGetID(const char* path);
cclValueID cclConsumedServiceValueGetID(cclConsumedServiceID consumedServiceID, const char* member, cclValueRepresentation repr);
cclValueID cclProvidedServiceValueGetID(cclProvidedServiceID providedServiceID, const char* member, cclValueRepresentation repr);

int cclRequestService(cclConsumedServiceID consumedServiceID);
int cclReleaseServiceRequest(cclConsumedServiceID consumedServiceID);
int cclIsServiceRequested(cclConsumedServiceID consumedServiceID, bool* isRequested);

int cclDiscoverProviders(cclConsumerID consumerID);
int cclAnnounceProvider(cclProviderID providerID);
int cclUnannounceProvider(cclProviderID providerID);
int cclAnnounceProviderToConsumer(cclServiceID serviceID, cclProviderID providerID, cclConsumerID consumerID);

int cclSetConsumerAddress(cclConsumerID consumerID, cclAddressID addressID, cclProviderID providerID);
int cclSetProviderAddress(cclProviderID providerID, cclAddressID addressID, cclConsumerID consumerID);

typedef void(*cclConnectionEstablishedHandler)();
typedef void(*cclConnectionFailedHandler)(const char* error);

int cclConnectAsyncConsumer(cclConsumedServiceID consumedServiceID, cclConnectionEstablishedHandler success, cclConnectionFailedHandler failure);
int cclConnectAsyncProvider(cclProvidedServiceID providedServiceID, cclConnectionEstablishedHandler success, cclConnectionFailedHandler failure);
int cclDisconnectConsumer(cclConsumedServiceID consumedServiceID);
int cclDisconnectProvider(cclProvidedServiceID providedServiceID);

typedef void(*cclConsumerDiscoveredHandler)(cclAddressID addressID);
typedef void(*cclProviderDiscoveredHandler)(cclAddressID addressID);

int cclSetConsumerDiscoveredHandler(cclProviderID providerID, cclConsumerDiscoveredHandler handler);
int cclSetProviderDiscoveredHandler(cclConsumerID consumerID, cclProviderDiscoveredHandler handler);

typedef void(*cclServiceDiscoveryHandler)(cclConsumerID consumerID);
typedef void(*cclConnectionRequestedHandler)(cclConsumerID consumerID);

int cclSetServiceDiscoveryHandler(cclServiceID serviceID, cclServiceDiscoveryHandler handler);
int cclSetServiceConsumerDiscoveredHandler(cclServiceID serviceID, cclConsumerDiscoveredHandler handler);
int cclSetServiceProviderDiscoveredHandler(cclServiceID serviceID, cclProviderDiscoveredHandler handler);
int cclSetConnectionRequestedHandler(cclProviderID providerID, cclConnectionRequestedHandler handler);

// PDU Signal access
typedef int cclPDUSenderID;
typedef int cclPDUReceiverID;
typedef int cclPDUProviderID;

const cclPDUSenderID CCL_PDU_SENDER_NONE = -1;
const cclPDUReceiverID CCL_PDU_RECEIVER_NONE = -1;
const cclPDUProviderID CCL_PDU_PROVIDER_NONE = -1;

cclPDUSenderID cclPDUSenderGetID(const char* path);
cclPDUReceiverID cclPDUReceiverGetID(const char* path);
cclPDUProviderID cclPDUProviderGetID(const char* path);

cclValueID cclPDUSenderSignalValueGetID(cclPDUSenderID pduSenderID, const char* signal, const char* member, cclValueRepresentation repr);
cclValueID cclPDUReceiverSignalValueGetID(cclPDUReceiverID pduReceiverID, const char* signal, const char* member, cclValueRepresentation repr);
cclValueID cclPDUProviderSignalValueGetID(cclPDUProviderID pduProviderID, const char* signal, const char* member, cclValueRepresentation repr);

// Service PDU Subscription (binding unspecific)
cclPDUReceiverID cclConsumedPDUGetID(const char* path);
cclPDUSenderID   cclProvidedPDUGetID(const char* path);

int cclPDUGetNrOfSubscribedConsumers(cclPDUProviderID pduProviderID, int* nrOfConsumers);
int cclPDUGetSubscribedConsumers(cclPDUProviderID pduProviderID, cclConsumerID* consumerBuffer, int* bufferSize);

int cclProvidedPDUSetSubscriptionStateIsolated(cclPDUSenderID providedPDUID, bool subscribed);
int cclConsumedPDUSetSubscriptionStateIsolated(cclPDUReceiverID consumedPDUID, bool subscribed);

// Event Subscription (binding unspecific)
typedef int cclConsumedEventID;
typedef int cclProvidedEventID;
typedef int cclEventProviderID;

const cclConsumedEventID CCL_CONSUMED_EVENT_NONE = -1;
const cclProvidedEventID CCL_PROVIDED_EVENT_NONE = -1;
const cclEventProviderID CCL_EVENT_PROVIDER_NONE = -1;

cclConsumedEventID cclConsumedEventGetID(const char* path);
cclProvidedEventID cclProvidedEventGetID(const char* path);
cclEventProviderID cclEventProviderGetID(const char* path);

int cclEventGetNrOfSubscribedConsumers(cclEventProviderID eventProviderID, int* nrOfConsumers);
int cclEventGetSubscribedConsumers(cclEventProviderID eventProviderID, cclConsumerID* consumerBuffer, int* bufferSize);

int cclConsumedEventSetSubscriptionStateIsolated(cclConsumedEventID consumedEventID, bool subscribed);
int cclProvidedEventSetSubscriptionStateIsolated(cclProvidedEventID providedEventID, bool subscribed);

// Service Field Access
typedef int cclConsumedFieldID;
typedef int cclProvidedFieldID;
typedef int cclFieldProviderID;

const cclConsumedFieldID CCL_CONSUMED_FIELD_NONE = -1;
const cclProvidedFieldID CCL_PROVIDED_FIELD_NONE = -1;
const cclFieldProviderID CCL_FIELD_PROVIDER_NONE = -1;

cclConsumedFieldID cclConsumedFieldGetID(const char* path);
cclProvidedFieldID cclProvidedFieldGetID(const char* path);
cclFieldProviderID cclFieldProviderGetID(const char* path);

int cclFieldGetNrOfSubscribedConsumers(cclFieldProviderID providerID, int* nrOfConsumers);
int cclFieldGetSubscribedConsumers(cclFieldProviderID providerID, cclConsumerID* consumerBuffer, int* bufferSize);

int cclConsumedFieldSetSubscriptionStateIsolated(cclConsumedFieldID consumedFieldID, bool subscribed);
int cclProvidedFieldSetSubscriptionStateIsolated(cclProvidedFieldID providedFieldID, bool subscribed);

// Abstract binding
cclAddressID cclAbstractCreateAddress(cclServiceID serviceID, const char* endPointName);
int cclAbstractGetBindingID(cclAddressID addressID, long* bindingID);
int cclAbstractGetDisplayName(cclAddressID addressID, char* buffer, int bufferSize);

cclConsumedEventID cclAbstractConsumedEventGetID(const char* path);
cclEventProviderID cclAbstractEventProviderGetID(const char* path);

int cclAbstractRequestEvent(cclConsumedEventID consumedEventID);
int cclAbstractReleaseEvent(cclConsumedEventID consumedEventID);
int cclAbstractIsEventRequested(cclConsumedEventID consumedEventID, bool* isRequested);

int cclAbstractSubscribeEvent(cclConsumedEventID consumedEventID);
int cclAbstractUnsubscribeEvent(cclConsumedEventID consumedEventID);

typedef void(*cclAbstractEventSubscriptionHandler)(cclProvidedEventID providedEventID);
int cclAbstractSetEventSubscribedHandler(cclEventProviderID providerID, cclAbstractEventSubscriptionHandler handler);
int cclAbstractSetEventUnsubscribedHandler(cclEventProviderID providerID, cclAbstractEventSubscriptionHandler handler);

cclPDUReceiverID cclAbstractConsumedPDUGetID(const char* path);
cclPDUProviderID cclAbstractPDUProviderGetID(const char* path);

int cclAbstractRequestPDU(cclPDUReceiverID consumedPDUID);
int cclAbstractReleasePDU(cclPDUReceiverID consumedPDUID);
int cclAbstractIsPDURequested(cclPDUReceiverID consumedPDUID, bool* isRequested);

int cclAbstractSubscribePDU(cclPDUReceiverID consumedPDUID);
int cclAbstractUnsubscribePDU(cclPDUReceiverID consumedPDUID);

typedef void(*cclAbstractPDUSubscriptionHandler)(cclPDUSenderID providedPDUID);
int cclAbstractSetPDUSubscribedHandler(cclPDUProviderID pduProviderID, cclAbstractPDUSubscriptionHandler handler);
int cclAbstractSetPDUUnsubscribedHandler(cclPDUProviderID pduProviderID, cclAbstractPDUSubscriptionHandler handler);

cclConsumedFieldID cclAbstractConsumedFieldGetID(const char* path);
cclFieldProviderID cclAbstractFieldProviderGetID(const char* path);

int cclAbstractRequestField(cclConsumedFieldID fieldID);
int cclAbstractReleaseField(cclConsumedFieldID fieldID);
int cclAbstractIsFieldRequested(cclConsumedFieldID fieldID, bool* isRequested);

int cclAbstractSubscribeField(cclConsumedFieldID fieldID);
int cclAbstractUnsubscribeField(cclConsumedFieldID fieldID);

typedef void(*cclAbstractFieldSubscriptionHandler)(cclProvidedFieldID fieldID);
int cclAbstractSetFieldSubscribedHandler(cclFieldProviderID fieldID, cclAbstractFieldSubscriptionHandler handler);
int cclAbstractSetFieldUnsubscribedHandler(cclFieldProviderID fieldID, cclAbstractFieldSubscriptionHandler handler);

// SOME/IP binding
cclAddressID cclSomeIPCreateAddress(const char* udpAddress, long udpPort, const char* tcpAddress, long tcpPort);
int cclSomeIPGetBindingID(cclAddressID addressID, long* bindingID);
int cclSomeIPGetDisplayName(cclAddressID addressID, char* buffer, int bufferSize);

typedef int cclSomeIPConsumedEventGroupID;
typedef int cclSomeIPProvidedEventGroupID;
typedef int cclSomeIPEventGroupProviderID;

cclSomeIPConsumedEventGroupID cclSomeIPConsumedEventGroupGetID(const char* path);
cclSomeIPProvidedEventGroupID cclSomeIPProvidedEventGroupGetID(const char* path);
cclSomeIPEventGroupProviderID cclSomeIPEventGroupProviderGetID(const char* path);

int cclSomeIPRequestEventGroup(cclSomeIPConsumedEventGroupID consumedEventGroupID);
int cclSomeIPReleaseEventGroup(cclSomeIPConsumedEventGroupID consumedEventGroupID);
int cclSomeIPIsEventGroupRequested(cclSomeIPConsumedEventGroupID consumedEventGroupID, bool* isRequested);

int cclSomeIPConsumedEventGroupGetEvents(cclSomeIPConsumedEventGroupID consumedEventGroupID, cclConsumedEventID* buffer, int* bufferSize);
int cclSomeIPProvidedEventGroupGetEvents(cclSomeIPProvidedEventGroupID providedEventGroupID, cclProvidedEventID* buffer, int* bufferSize);
int cclSomeIPEventGroupProviderGetEvents(cclSomeIPEventGroupProviderID eventGroupProviderID, cclEventProviderID* buffer, int* bufferSize);

int cclSomeIPConsumedEventGroupGetPDUs(cclSomeIPConsumedEventGroupID consumedEventGroupID, cclPDUReceiverID* buffer, int* bufferSize);
int cclSomeIPProvidedEventGroupGetPDUs(cclSomeIPProvidedEventGroupID providedEventGroupID, cclPDUSenderID* buffer, int* bufferSize);
int cclSomeIPEventGroupProviderGetPDUs(cclSomeIPEventGroupProviderID eventGroupProviderID, cclPDUProviderID* buffer, int* bufferSize);

int cclSomeIPConsumedEventGroupGetFields(cclSomeIPConsumedEventGroupID consumedEventGroupID, cclConsumedFieldID* buffer, int* bufferSize);
int cclSomeIPProvidedEventGroupGetFields(cclSomeIPProvidedEventGroupID providedEventGroupID, cclProvidedFieldID* buffer, int* bufferSize);
int cclSomeIPEventGroupProviderGetFields(cclSomeIPEventGroupProviderID eventGroupProviderID, cclFieldProviderID* buffer, int* bufferSize);

int cclSomeIPEventGroupGetNrOfSubscribedConsumers(cclSomeIPEventGroupProviderID eventGroupProviderID, int* nrOfConsumers);
int cclSomeIPEventGroupGetSubscribedConsumers(cclSomeIPEventGroupProviderID eventGroupProviderID, cclConsumerID* consumerBuffer, int* bufferSize);

int cclSomeIPSubscribeEventGroup(cclSomeIPConsumedEventGroupID consumedEventGroupID);
int cclSomeIPUnsubscribeEventGroup(cclSomeIPConsumedEventGroupID consumedEventGroupID);

typedef void(*cclSomeIPEventGroupSubscriptionHandler)(cclSomeIPProvidedEventGroupID providedEventGroupID);
int cclSomeIPSetEventGroupSubscribedHandler(cclSomeIPEventGroupProviderID eventGroupProviderID, cclSomeIPEventGroupSubscriptionHandler handler);
int cclSomeIPSetEventGroupUnsubscribedHandler(cclSomeIPEventGroupProviderID eventGroupProviderID, cclSomeIPEventGroupSubscriptionHandler handler);

#if defined __cplusplus
}
#endif

#endif // CCL_H
