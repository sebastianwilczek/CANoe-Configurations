/*-----------------------------------------------------------------------------
Module: VIA (not observed)
Interfaces: This is a public interface header.
-------------------------------------------------------------------------------
Public interface for access to the FunctionBus
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/

#ifndef VIA_FUNCTIONBUS_H
#define VIA_FUNCTIONBUS_H

/*-----------------------------------------------------------------------------
Version of the interfaces, which are declared in this header

2016-11-16   1.0  Arr     Creation
2017-03-24   1.1  Who     Extended VE access, service discovery features
2017-04-04   1.2  Who     Implementation of PDU access and service discovery
2017-04-11   1.3  Who     Binding Block specific API (events and event groups)
2017-04-20   1.4  Who     Access client/server FEP through service ports
2017-05-08   1.5  Who     Enhancements to call context and VE access
2017-06-20   1.6  Who     Fixed VS 2010 compatibility issue
2017-06-28   1.7  Rue     Added ResetStatus to VIAFBStatus
2017-08-03   1.8  Who     Observe removal of dynamic FEPs and ports
2017-10-10   1.9  Who     Field support
2018-11-05   1.10 Rue     Unbounded arrays, generic PDU signals and measurement points
-----------------------------------------------------------------------------*/

#define VIAFunctionBusServiceMajorVersion 1
#define VIAFunctionBusServiceMinorVersion 10

// ============================================================================
// Dependencies
// ============================================================================

#ifndef VIA_H
#include "VIA.h"
#endif

// Forward declarations
class VIAFbViaService;
class VIAFbValuePort;
class VIAFbSignalSenderPort;
class VIAFbSignalReceiverPort;
class VIAFbPDUSenderPort;
class VIAFbPDUReceiverPort;
class VIAFbFunctionClientPort;
class VIAFbFunctionServerPort;
class VIAFbCallContext;
class VIAFbServiceConsumer;
class VIAFbServiceProvider;
class VIAFbConsumedService;
class VIAFbProvidedService;
class VIAFbEventProvider;
class VIAFbConsumedEvent;
class VIAFbProvidedEvent;
class VIAFbServicePDUProvider;
class VIAFbConsumedServicePDU;
class VIAFbProvidedServicePDU;
class VIAFbFieldProvider;
class VIAFbConsumedField;
class VIAFbProvidedField;
class VIAFbServiceCO;
class VIAFbAbstractBinding;
class VIAFbAbstractEventProvider;
class VIAFbAbstractConsumedEvent;
class VIAFbAbstractServicePDUProvider;
class VIAFbAbstractConsumedServicePDU;
class VIAFbAbstractFieldProvider;
class VIAFbAbstractConsumedField;
class VIAFbSomeIPBinding;
class VIAFbSomeIPEventGroupProvider;
class VIAFbSomeIPConsumedEventGroup;
class VIAFbSomeIPProvidedEventGroup;
class VIAFbValue;
class VIAFbStatus;
class VIAFbType;

// ============================================================================
// Type definitions and constants
// ============================================================================

// FunctionBus types
enum VIAFbTypeTag
{
  eSignedInteger,    // Encoding: 2C
  eUnsignedInteger,  // Encoding: none
  eFloat,            // Encoding: IEEE754
  eString,           // Encoding: To be determined by GetEncoding
  eArray,
  eStruct,
  eUnion,
  eData
};

// The representational type used to access a value
enum VIAFbTypeLevel
{
  ePhys,  // Physical representation, restricted to Integer and Float
  eImpl,  // Implementation representation
  eRaw,   // Bit-serialized representation
};

enum VIAFbValueClass
{
  ePortValue,         // The value entity directly related with the addressed PDU, Signal or Event port
  eServiceState,      // Service state of Service provider side FEP
  eConnectionState,   // Connection state of Service AP
  eSubscriptionState, // Subscription state of PDU/Event AP or MP
  eLatestCall,        // Latest call input parameters of function AP or MP
  eLatestReturn,      // Latest call return parameters of function AP or MP
  eParamDefaults      // VSIM configuration of function provider side AP
};

// Mode with which notifications about value changes are sent
enum VIAFbUpdateMode
{
  eNotifyOnUpdate,  // Notify whenever value is set (on update)
  eNotifyOnChange,  // Notify whenever value is set and the new value is different from the old value (on change)
};

// States a FunctionBus service call can be in
enum VIAFbFunctionCallState
{
  eUndefinedState,
  eCalling,
  eCalled,
  eReturning,
  eReturned,
  eFinalizing,
};

// Whether a call context is being processed on client or server side
enum VIAFbFunctionCallSide
{
  eClient,
  eServer
};

// Whether a value entity (element) was last measured or is still offline
enum VIAFbValueState
{
  eOffline,
  eMeasured
};

// Dynamically added FEP state
enum VIAFbFEPState
{
  eSimulated,
  eReal
};

// Service state at service provider
enum VIAFbServiceState
{
  eUnavailable,
  eInitializing,
  eAvailable
};

// Connection state at consumer side port
enum VIAFbConsumerConnectionState
{
  eProviderUnavailable,
  eProviderConnectable,
  eProviderAvailable
};

// Connection state at provider side port
enum VIAFbProviderConnectionState
{
  eConsumerUnavailable,
  eConsumerConnectable,
  eConsumerConnected
};

// PDU, event and event group subscription state
enum VIAFbSubscriptionState
{
  eSubscribed,
  eSubscribable,
  eUnsubscribable
};

// Address handle internally maps to an NFunctionBus::AddressHandle
struct VIAFbAddressHandle
{
  VIASTDDECL Release() = 0;
};

// Callback handle internally maps to an ICallback object
typedef void* VIAFbCallbackHandle;

// Type handle internally maps to a DataTypeMemberHandle from SystemVariables.h:
typedef long VIAFbTypeMemberHandle;
const VIAFbTypeMemberHandle kVIAFbTypeMemberHandleWhole = VIAFbTypeMemberHandle(-1);
const VIAFbTypeMemberHandle kVIAFbTypeMemberHandleInvalid = VIAFbTypeMemberHandle(-2);

// Binding specific identifier of service consumer and provider
typedef long VIAFbBindingId;
const VIAFbBindingId kVIAFbBindingIdInvalid = VIAFbBindingId(-1);

// Time constants
typedef VIATime VIATimeDiff;
const VIATimeDiff kVIAFbTimeDeltaNow(0);
const VIATimeDiff kVIAFbTimeDeltaNever(-1);
const VIATime kVIAFbTimeUndefined(-1);

// ============================================================================
// Observer classes
// ============================================================================

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
// Callback handler to notify observers when dynamic function bus ports or FEPs are removed.
class VIAFbPortObserver
{
public:
  // Indicates that a dynamic port or FEP is about to be removed
  // [OUT] Return value is ignored
  VIASTDDECL OnPortRemoval(
    unsigned long long portID) = 0;                      // [IN] direction of port being removed
};
#endif

// Callback handler to notify observers whenever a FunctionBus signal port receives a value.
// Note: this is identical to observing value updates of the signal receiver port VE.
class VIAFbSignalReceiverObserver
{
public:
  // Indicates that the signal value at the receiver port has changed
  // [OUT] Return value is ignored
  VIASTDDECL OnSignalReceived(
    VIATime inTime,                                      // [IN] Absolute time the signal value was updated
    const VIAFbSignalReceiverPort* inReceiverPort) = 0;  // [IN] Receiving signal port
};

// Callback handler to notify observers whenever a FunctionBus PDU port receives a value.
// Note: this is identical to observing value updates of the signal receiver port VE.
class VIAFbPDUReceiverObserver
{
public:
  // Indicates that the PDU value at the receiver port has changed
  // [OUT] Return value is ignored
  VIASTDDECL OnPDUReceived(
    VIATime inTime,                                   // [IN] Absolute time the PDU value was updated
    const VIAFbPDUReceiverPort* inReceiverPort) = 0;  // [IN] Receiving PDU port
};

// Callback handler to notify observers about any service discovery events.
class VIAFbServiceDiscoveryObserver
{
public:
  // Indicates that a new service discovery request was sent by some service consumer.
  // [OUT] Return value is ignored
  VIASTDDECL OnServiceDiscovery(
    VIAFbServiceCO* inService,                        // [IN] The service hosting the requesting consumer
    VIAFbServiceConsumer* inConsumer) = 0;            // [IN] The service consumer which initiated service discovery

  // Indicates that a new service consumer was discovered
  // [OUT] Return value is ignored
  VIASTDDECL OnConsumerDiscovered(
    VIAFbServiceCO* inService,                        // [IN] The service hosting the discovered consumer
    VIAFbAddressHandle* inAddress) = 0;               // [IN] The (binding specific) address of the discovered consumer

  // Indicates that a new service provider was discovered
  // [OUT] Return value is ignored
  VIASTDDECL OnProviderDiscovered(
    VIAFbServiceCO* inService,                        // [IN] The service hosting the discovered provider
    VIAFbAddressHandle* inAddress) = 0;               // [IN] The (binding specific) address of the discovered provider
};

// Callback handler to notify observers about discovered consumers and connections for a specific service provider
class VIAFbServiceProviderObserver
{
public:
  // Indicates that a new service consumer was discovered
  // [OUT] Return value is ignored
  VIASTDDECL OnConsumerDiscovered(
    VIAFbServiceProvider* inProvider,                 // [IN] The service provider detecting the consumer
    VIAFbAddressHandle* inAddress) = 0;               // [IN] The (binding specific) address of the discovered consumer

  // Indicates that a consumer requests a connection to the provider
  // [OUT] Return value is ignored
  VIASTDDECL OnConnectionRequested(
    VIAFbServiceProvider* inProvider,                 // [IN] The service provider receiving the connection request
    VIAFbServiceConsumer* inConsumer) = 0;            // [IN] The service consumer requesting the connection
};

// Callback handler to notify observers about discovered providers for a specific service consumer
class VIAFbServiceConsumerObserver
{
public:
  // Indicates that a new service provider was discovered
  // [OUT] Return value is ignored
  VIASTDDECL OnProviderDiscovered(
    VIAFbServiceConsumer* inConsumer,                 // [IN] The service consumer detecting the provider
    VIAFbAddressHandle* inAddress) = 0;               // [IN] The (binding specific) address of the discovered provider
};

// Callback handler to notify about service connection success or failure
class VIAFbConsumedServiceConnectionHandler
{
public:
  // Indicates that the service connection was successfully established
  // [OUT] Return value is ignored
  VIASTDDECL OnConnectionEstablished(
    VIAFbConsumedService* inPort) = 0;                // [IN] the consumer side service port which is now connected

  // Indicates that the service connection could not be established
  // [OUT] Return value is ignored
  VIASTDDECL OnConnectionFailed(
    VIAFbConsumedService* inPort,                     // [IN] the consumer side service port which failed to connect
    const char* inError) = 0;                         // [IN] An error message indicating the cause of connection failure
};

// Callback handler to notify about service connection success or failure
class VIAFbProvidedServiceConnectionHandler
{
public:
  // Indicates that the service connection was successfully established
  // [OUT] Return value is ignored
  VIASTDDECL OnConnectionEstablished(
    VIAFbProvidedService* inPort) = 0;                // [IN] the provider side service port which is now connected

  // Indicates that the service connection could not be established
  // [OUT] Return value is ignored
  VIASTDDECL OnConnectionFailed(
    VIAFbProvidedService* inPort,                     // [IN] the provider side service port which failed to connect
    const char* inError) = 0;                         // [IN] An error message indicating the cause of connection failure
};

// Callback handler to notify client-side observers when the called server-side FunctionBus service port receives the call or either an error or the result arrives.
class VIAFbFunctionClientObserver
{
public:
  // Indicates that the invocation initiated at the client port changed its state
  // Note: The obtained inCallContext must be released explicitly via inCallContext->Release.
  // [OUT] Return value is ignored
  VIASTDDECL OnCallStateChanged(
    VIATime inTime,                                    // [IN] Absolute time this state change was observed
    VIAFbCallContext* inCallContext,                   // [IN] Call context of the affected call
    VIAFbFunctionCallState inCallState,                // [IN] State reached
    const VIAFbFunctionClientPort* inClientPort) = 0;  // [IN] Client function port
};

// Callback handler to notify server-side observers whenever a FunctionBus service port receives a call from a client to be handled.
class VIAFbFunctionServerObserver
{
public:
  // Indicates that an invocation request arrived at the server port
  // Note: The obtained inCallContext must be released explicitly via inCallContext->Release.
  // [OUT] Return value is ignored
  VIASTDDECL OnCallStateChanged(
    VIATime inTime,                                    // [IN] Absolute time this state change was observed
    VIAFbCallContext* inCallContext,                   // [IN] Call context of the affected call
    VIAFbFunctionCallState inCallState,                // [IN] State reached
    const VIAFbFunctionServerPort* inServerPort) = 0;  // [IN] Server function port
};

// Callback handler to notify value entity (element) observers when the value was updated or changed
class VIAFbValueObserver
{
public:
  // Indicates that the value of the value entity (element) was updated
  // Note: this callback is used for value updates the same as for value changes, which events should fire must be specified upon registration.
  VIASTDDECL OnValueUpdated(
    VIATime inTime,                                    // [IN] Absolute time this value update was observed
    VIAFbStatus* inStatus) = 0;                        // [IN] Updated value entity status object (provides access to the current value)
};

// Callback handler to notify observers about event subscription and desubscription
class VIAFbAbstractEventSubscriptionObserver
{
public:
  // Indicates that an event consumer has subscribed at a specific provider.
  // [OUT] Return value is ignored
  VIASTDDECL OnEventSubscribed(
    VIAFbProvidedEvent* inEventPort) = 0;             // [IN] The event provider side port reflecting the subscribed connection

  // Indicates that an event consumer has unsubscribed from a specific provider.
  // [OUT] Return value is ignored
  VIASTDDECL OnEventUnsubscribed(
    VIAFbProvidedEvent* inEventPort) = 0;             // [IN] The event provider side port reflecting the no more subscribed connection
};

// Callback handler to notify observers about PDU subscription and desubscription
class VIAFbAbstractPDUSubscriptionObserver
{
public:
  // Indicates that a PDU consumer has subscribed at a specific provider.
  // [OUT] Return value is ignored
  VIASTDDECL OnPDUSubscribed(
    VIAFbProvidedServicePDU* inPDUPort) = 0;          // [IN] The PDU provider side port reflecting the subscribed connection

  // Indicates that a PDU consumer has unsubscribed from a specific provider.
  // [OUT] Return value is ignored
  VIASTDDECL OnPDUUnsubscribed(
    VIAFbProvidedServicePDU* inPDUPort) = 0;          // [IN] The PDU provider side port reflecting the no more subscribed connection
};

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 9)
// Callback handler to notify observers about field subscription and desubscription
class VIAFbAbstractFieldSubscriptionObserver
{
public:
  // Indicates that a field consumer has subscribed at a specific provider.
  // [OUT] Return value is ignored
  VIASTDDECL OnFieldSubscribed(
    VIAFbProvidedField* inFieldPort) = 0;             // [IN] The field provider side port reflecting the subscribed connection

  // Indicates that a field consumer has unsubscribed from a specific provider.
  // [OUT] Return value is ignored
  VIASTDDECL OnFieldUnsubscribed(
    VIAFbProvidedField* inFieldPort) = 0;             // [IN] The field provider side port reflecting the no more subscribed connection
};
#endif

// Callback handler to notify observers about event group subscription and desubscription
class VIAFbSomeIPEventGroupSubscriptionObserver
{
public:
  // Indicates that an event group consumer has subscribed at a specific provider.
  // [OUT] Return value is ignored
  VIASTDDECL OnEventGroupSubscribed(
    VIAFbSomeIPProvidedEventGroup* inEventGroupPort) = 0;  // [IN] The event group provider side port reflecting the subscribed connection

  // Indicates that an event group consumer has unsubscribed from a specific provider.
  // [OUT] Return value is ignored
  VIASTDDECL OnEventGroupUnsubscribed(
    VIAFbSomeIPProvidedEventGroup* inEventGroupPort) = 0;  // [IN] The event group provider side port reflecting the no more subscribed connection
};

// ============================================================================
// Iterator classes
// ============================================================================

class VIAFbServiceConsumerIterator
{
public:
  // Returns whether more consumers can be iterated
  VIABOOLDECL HasMoreConsumers() const = 0;

  // Returns the current consumer and moves the iterator forward
  VIASTDDECL GetNextConsumer(
    VIAFbServiceConsumer** outConsumer) = 0;          // [OUT] The next service consumer instance

  // Go to next consumer without creating
  VIASTDDECL SkipConsumer() = 0;

  // Releases the iterator object
  VIASTDDECL Release() = 0;
};

template <typename T>
class VIAFbEventIterator
{
public:
  // Returns whether more events can be iterated
  VIABOOLDECL HasMoreEvents() const = 0;

  // Returns the current event and moves the iterator forward
  VIASTDDECL GetNextEvent(
    T** outEvent) = 0;                                // [OUT] The next service event instance

  // Go to next event without creating
  VIASTDDECL SkipEvent() = 0;

  // Releases the iterator object
  VIASTDDECL Release() = 0;
};

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
template <typename T>
class VIAFbPDUIterator
{
public:
  // Returns whether more PDUs can be iterated
  VIABOOLDECL HasMorePDUs() const = 0;

  // Returns the current PDU and moves the iterator forward
  VIASTDDECL GetNextPDU(
    T** outPDU) = 0;                                // [OUT] The next service PDU instance

  // Go to next PDU without creating
  VIASTDDECL SkipPDU() = 0;

  // Releases the iterator object
  VIASTDDECL Release() = 0;
};
#endif

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 9)
template <typename T>
class VIAFbFieldIterator
{
public:
  // Returns whether more fields can be iterated
  VIABOOLDECL HasMoreFields() const = 0;

  // Returns the current field and moves the iterator forward
  VIASTDDECL GetNextField(
    T** outField) = 0;                                // [OUT] The next service field instance

  // Go to next field without creating
  VIASTDDECL SkipField() = 0;

  // Releases the iterator object
  VIASTDDECL Release() = 0;
};
#endif

// ============================================================================
// Classes
// ============================================================================

// -------------------------------------------------------------------
// VIA FunctionBus: Factory service

// class VIAFunctionBusService: This class provides an interface for querying objects of the FunctionBus.
// Note: you should use the generic GetValuePort to access measurement point values.
class VIAFbViaService
{
public:
  // Release the service, same as calling ReleaseFunctionBusService
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Obtain access to a port sending a signal.
  // Note: this method can also access event provider ports, but the use is restricted to signal sender functionality
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetSignalSenderPort(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbSignalSenderPort** outPort) const = 0;                 // [OUT] Port handle, if successful

  // Obtain access to a port receiving a signal.
  // Note: this method can also access event consumer ports, but the use is restricted to signal receiver functionality
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetSignalReceiverPort(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbSignalReceiverPort** outPort) const = 0;               // [OUT] Port handle, if successful

  // Obtain access to a port consuming a service or free function.
  // Path must refer to a specific port of a service or free *function*, e.g. someNs::someObj[someClient,someServer].someFunc"
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetFunctionClientPort(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbFunctionClientPort** outPort) const = 0;               // [OUT] Port handle, if successful

  // Obtain access to a port providing a service or free function.
  // Path must refer to a specific port of a service or free *function*, e.g. someNs::someObj[someClient,someServer].someFunc"
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetFunctionServerPort(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbFunctionServerPort** outPort) const = 0;               // [OUT] Port handle, if successful

  // Obtain access to a port sending a PDU.
  // Note: this method can also access service PDU sender ports, but the use is restricted to PDU sender functionality
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetPDUSenderPort(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbPDUSenderPort** outPort) const = 0;                    // [OUT] Port handle, if successful

  // Obtain access to a port receiving a PDU.
  // Note: this method can also access service PDU receiver ports, but the use is restricted to PDU receiver functionality
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetPDUReceiverPort(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbPDUReceiverPort** outPort) const = 0;                  // [OUT] Port handle, if successful

  // Obtain access to a service communication object.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if service does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetServiceCO(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbServiceCO** outService) const = 0;                     // [OUT] Service handle, if successful

  // Obtain access to a consumer side end point of a service.
  // Note: service consumers can as well be obtained by name from a service
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if service consumer does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetServiceConsumer(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbServiceConsumer** outConsumer) const = 0;              // [OUT] Service consumer handle, if successful

  // Obtain access to a provider side end point of a service.
  // Note: service providers can as well be obtained by name from a service
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if service provider does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetServiceProvider(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbServiceProvider** outProvider) const = 0;              // [OUT] Service consumer handle, if successful

  // Obtain access to a consumer side service port.
  // Note: consumed service ports can as well be obtained by name from a service
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if consumed service port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetConsumedService(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbConsumedService** outConsumerPort) const = 0;          // [OUT] Consumed service port handle, if successful

  // Obtain access to a provider side service port.
  // Note: provided service ports can as well be obtained by name from a service
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if provided service port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetProvidedService(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbProvidedService** outProviderPort) const = 0;          // [OUT] Provided service port handle, if successful

  // Obtain access to a provider side event end point.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if end point does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetEventProvider(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbEventProvider** outProvider) const = 0;                // [OUT] Event end point handle, if successful

  // Obtain access to a consumer side event port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetConsumedEvent(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbConsumedEvent** outPort) const = 0;                    // [OUT] Event consumer side port handle, if successful

  // Obtain access to a provider side event port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetProvidedEvent(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbProvidedEvent** outPort) const = 0;                    // [OUT] Event provider side port handle, if successful

  // Obtain access to a provider side service PDU end point.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if end point does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetServicePDUProvider(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbServicePDUProvider** outProvider) const = 0;           // [OUT] Service PDU end point handle, if successful

  // Obtain access to a consumer side service PDU port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetConsumedServicePDU(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbConsumedServicePDU** outPort) const = 0;               // [OUT] Service PDU consumer side port handle, if successful

  // Obtain access to a provider side service PDU port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetProvidedServicePDU(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbProvidedServicePDU** outPort) const = 0;               // [OUT] Service PDU provider side port handle, if successful

  // Obtain access to Abstract binding functionality.
  // The binding access object returned is stateless and thus does not need to be released.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL GetAbstractBinding(
    VIAFbAbstractBinding** outBinding) const = 0;               // [OUT] Abstract binding API

  // Obtain access to SOME/IP binding functionality.
  // The binding access object returned is stateless and thus does not need to be released.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL GetSomeIPBinding(
    VIAFbSomeIPBinding** outBinding) const = 0;                 // [OUT] SOME/IP binding API

  // Obtain access to a port generically for reading and writing values.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetValuePort(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbValuePort** outPort) const = 0;                        // [OUT] Port handle, if successful

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 9)
  // Obtain access to a provider side field end point.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if end point does not exist, kVIA_ObjectInvalid if field has no notification
  // [THREADSAFE] Yes
  VIASTDDECL GetFieldProvider(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbFieldProvider** outProvider) const = 0;                // [OUT] Field end point handle, if successful

  // Obtain access to a consumer side field port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist, kVIA_ObjectInvalid if field has no notification
  // [THREADSAFE] Yes
  VIASTDDECL GetConsumedField(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbConsumedField** outPort) const = 0;                    // [OUT] Field consumer side port handle, if successful

  // Obtain access to a provider side field port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist, kVIA_ObjectInvalid if field has no notification
  // [THREADSAFE] Yes
  VIASTDDECL GetProvidedField(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbProvidedField** outPort) const = 0;                    // [OUT] Field provider side port handle, if successful
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: Generic value ports (signal, PDU, service FEP, ...)

class VIAFbValuePort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the value of this port that was last set.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbValueClass inValueClass,                               // [IN] Identifies the specific value entity associated with the port
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object, if successful

  // Obtain access to a FunctionBus data type for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] Yes
  VIASTDDECL GetType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbValueClass inValueClass,                               // [IN] Identifies the specific value entity associated with the port
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Update the value of the port. Depending on the port and value class this may fail (read-only values).
  // [OUT] kVIA_OK if the update was successful, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle
  // [THREADSAFE] No
  VIASTDDECL SetValue(
    const VIAFbValue* inValue) = 0;                             // [IN] The value object containing the new value

  // Retrieve status information of this port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetStatus(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbValueClass inValueClass,                               // [IN] Identifies the specific value entity associated with the port
    VIAFbStatus** outStatus) const = 0;                         // [OUT] The status object, if successful

  // Retrieve the statically configured initialization value of this port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetInitValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this initial value is accessed
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbValueClass inValueClass,                               // [IN] Identifies the specific value entity associated with the port
    VIAFbValue** outInitValue) const = 0;                       // [OUT] The initial value object, if successful

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 10)
  // Retrieve the PDU signal value of this port that was last set.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPDUSignalValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    const char* inSignalName,                                   // [IN] Identifies the specific PDU signal associated with the port
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object, if successful

  // Obtain access to a FunctionBus data type of a PDU signal for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] Yes
  VIASTDDECL GetPDUSignalType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    const char* inSignalName,                                   // [IN] Identifies the specific PDU signal associated with the port
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: Signal sender/receiver ports

class VIAFbSignalSenderPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the value of this signal sender port that was last set.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object, if successful

  // Obtain access to a FunctionBus data type for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] Yes
  VIASTDDECL GetType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Update the value of a signal's sender port to be send. Depending on the signal port's configuration, 
  // the updated value will be sent either immediately or with the upcoming cycle.
  // [OUT] kVIA_OK if the update was successful, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle
  // [THREADSAFE] No
  VIASTDDECL SetValue(
    const VIAFbValue* inValue) = 0;                             // [IN] The value object containing the new value

  // Retrieve status information of this port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetStatus(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbStatus** outStatus) const = 0;                         // [OUT] The status object, if successful

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

class VIAFbSignalReceiverPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the value of this signal receiver port that was last received.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object, if successful

  // Obtain access to the signal's FunctionBus data type for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] No
  VIASTDDECL GetType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Retrieve status information of this port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetStatus(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbStatus** outStatus) const = 0;                         // [OUT] The status object, if successful

  // Register an observer to be notified when this signal port is updated, or the updated value changes.
  // The time the value is received depends on the port's binding properties (e.g., cyclic, immediately).
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbSignalReceiverObserver* inObserver,                    // [IN] Pointer to an object implementing the observer template method
    VIAFbUpdateMode inUpdateMode,                               // [IN] Decide to observe only changes in value or any value updates
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: Event sender/receiver ports and provider FEP

class VIAFbConsumedEvent : public VIAFbSignalReceiverPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the value entity representing the subscription state of the consumer side event port.
  // [OUT] kVIA_OK if subscription state is available, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetSubscriptionState(
    VIAFbSubscriptionState* outState) const = 0;                // [OUT] The subscription state, if successful

  // Modifies the port local subscription state of the event without any network interaction.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL SetSubscriptionStateIsolated(
    VIAFbSubscriptionState inState) = 0;                        // [IN] The requested subscription state

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

class VIAFbProvidedEvent : public VIAFbSignalSenderPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the value entity representing the subscription state of the provider side event port.
  // [OUT] kVIA_OK if subscription state is available, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetSubscriptionState(
    VIAFbSubscriptionState* outState) const = 0;                // [OUT] The value object, if successful

  // Modifies the port local subscription state of the event without any network interaction.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL SetSubscriptionStateIsolated(
    VIAFbSubscriptionState inState) = 0;                        // [IN] The requested subscription state

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

class VIAFbEventProvider
{
public:
  // Release the provider instance, must be called for any provider instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve an iterator over all service consumers currently subscribed at the event provider
  // [OUT] kVIA_OK if subscribed consumers could be determined, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetSubscribedConsumers(
    VIAFbServiceConsumerIterator** outIterator) = 0;            // [OUT] An iterator over the currently subscribed consumers

  // Retrieve the latest model value of this event provider.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object, if successful

  // Update the model value of an event provider.
  // [OUT] kVIA_OK if the update was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL SetValue(
    const VIAFbValue* inValue) = 0;                             // [IN] The value object containing the new event value

  // Obtain access to a FunctionBus data type for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] Yes
  VIASTDDECL GetType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Retrieve status information of this event provider's value entity.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetStatus(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbStatus** outStatus) const = 0;                         // [OUT] The status object, if successful

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: PDU sender/receiver ports

class VIAFbPDUSenderPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the latest value of this PDU sender port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object, if successful

  // Update the value of a PDU's sender port to be sent. Depending on the PDU port's configuration, 
  // the updated value will be sent either immediately or with the upcoming cycle.
  // [OUT] kVIA_OK if the update was successful, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle
  // [THREADSAFE] No
  VIASTDDECL SetValue(
    const VIAFbValue* inValue) = 0;                             // [IN] The value object containing the new value

  // Retrieve status information of this port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetStatus(
    VIAFbStatus** outStatus) const = 0;                         // [OUT] The status object, if successful

  // Retrieve the value of a signal mapped into this PDU.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalue in case of a bad parameter, kVIA_ObjectNotFound if the signal does not exist, kVIA_Failed in case of an error
  VIASTDDECL GetSignalValue(
    const char* inSignalName,                                   // [IN] Name of the mapped signal
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object containing the new value

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

class VIAFbPDUReceiverPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the latest received value of this PDU receiver port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object, if successful

  // Retrieve status information of this port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetStatus(
    VIAFbStatus** outStatus) const = 0;                         // [OUT] The status object, if successful

  // Retrieve the value of a signal mapped into this PDU.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalue in case of a bad parameter, kVIA_ObjectNotFound if the signal does not exist, kVIA_Failed in case of an error
  VIASTDDECL GetSignalValue(
    const char* inSignalName,                                   // [IN] Name of the mapped signal
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object containing the new value

  // Register an observer to be notified when this PDU port is updated, or the updated value changes.
  // The time the value is received depends on the port's binding properties (e.g., cyclic, immediately).
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbPDUReceiverObserver* inObserver,                       // [IN] Pointer to an object implementing the observer template method
    VIAFbUpdateMode inUpdateMode,                               // [IN] Decide to observe only changes in value or any value updates
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: Service PDU sender/receiver ports and provider FEP

class VIAFbConsumedServicePDU : public VIAFbPDUReceiverPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the value entity representing the subscription state of the consumer side PDU port.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL GetSubscriptionState(
    VIAFbSubscriptionState* outState) const = 0;                // [OUT] The current state, if successful

  // Modifies the port local subscription state of the PDU without any network interaction.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL SetSubscriptionStateIsolated(
    VIAFbSubscriptionState inState) = 0;                        // [IN] The requested subscription state

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

class VIAFbProvidedServicePDU : public VIAFbPDUSenderPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the value entity representing the subscription state of the provider side PDU port.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL GetSubscriptionState(
    VIAFbSubscriptionState* outState) const = 0;                // [OUT] The value object, if successful

  // Modifies the port local subscription state of the PDU without any network interaction.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL SetSubscriptionStateIsolated(
    VIAFbSubscriptionState inState) = 0;                        // [IN] The requested subscription state

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

class VIAFbServicePDUProvider
{
public:
  // Release the provider instance, must be called for any provider instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve an iterator over all service consumers currently subscribed at the PDU provider
  // [OUT] kVIA_OK if subscribed consumers could be determined, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetSubscribedConsumers(
    VIAFbServiceConsumerIterator** outIterator) = 0;            // [OUT] An iterator over the currently subscribed consumers

  // Retrieve the latest model value of this PDU provider.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object, if successful

  // Update the model value of an PDU provider.
  // [OUT] kVIA_OK if the update was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL SetValue(
    const VIAFbValue* inValue) = 0;                             // [IN] The value object containing the new PDU value

  // Obtain access to a FunctionBus data type for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] Yes
  VIASTDDECL GetType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Retrieve status information of this port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetStatus(
    VIAFbStatus** outStatus) const = 0;                         // [OUT] The status object, if successful

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 

  // Retrieve the value of a signal mapped into this PDU.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalue in case of a bad parameter, kVIA_ObjectNotFound if the signal does not exist, kVIA_Failed in case of an error
  VIASTDDECL GetSignalValue(
    const char* inSignalName,                                   // [IN] Name of the mapped signal
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object containing the new value
#endif
};

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 9)
// -------------------------------------------------------------------
// VIA FunctionBus: Field sender/receiver ports and provider FEP

class VIAFbConsumedField : public VIAFbSignalReceiverPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the value entity representing the subscription state of the consumer side field port.
  // [OUT] kVIA_OK if subscription state is available, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetSubscriptionState(
    VIAFbSubscriptionState* outState) const = 0;                // [OUT] The subscription state, if successful

  // Modifies the port local subscription state of the field without any network interaction.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL SetSubscriptionStateIsolated(
    VIAFbSubscriptionState inState) = 0;                        // [IN] The requested subscription state

  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
};

class VIAFbProvidedField : public VIAFbSignalSenderPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the value entity representing the subscription state of the provider side field port.
  // [OUT] kVIA_OK if subscription state is available, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetSubscriptionState(
    VIAFbSubscriptionState* outState) const = 0;                // [OUT] The value object, if successful

  // Modifies the port local subscription state of the field without any network interaction.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL SetSubscriptionStateIsolated(
    VIAFbSubscriptionState inState) = 0;                        // [IN] The requested subscription state

  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
};

class VIAFbFieldProvider
{
public:
  // Release the provider instance, must be called for any provider instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve an iterator over all service consumers currently subscribed at the field provider
  // [OUT] kVIA_OK if subscribed consumers could be determined, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetSubscribedConsumers(
    VIAFbServiceConsumerIterator** outIterator) = 0;            // [OUT] An iterator over the currently subscribed consumers

  // Retrieve the latest model value of this field provider.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object, if successful

  // Update the model value of a field provider.
  // [OUT] kVIA_OK if the update was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL SetValue(
    const VIAFbValue* inValue) = 0;                             // [IN] The value object containing the new field value

  // Obtain access to a FunctionBus data type for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] Yes
  VIASTDDECL GetType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Retrieve status information of this field provider's value entity.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetStatus(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbStatus** outStatus) const = 0;                         // [OUT] The status object, if successful

  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
};
#endif

// -------------------------------------------------------------------
// VIA FunctionBus: Service CO access

class VIAFbServiceCO
{
public:
  // Release the service instance, must be called for any service instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Adds a consumer side dynamic FEP with given name and state to the service.
  // [OUT] kVIA_OK if successfully added consumer, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL AddConsumerByName(
    const char* inName,                                         // [IN] Name of the new service consumer
    VIAFbFEPState inState,                                      // [IN] Whether the new consumer is simulated or real
    VIAFbServiceConsumer** outConsumer) = 0;                    // [OUT] Service consumer handle if successful

  // Adds a consumer side dynamic FEP with given address to the service.
  // [OUT] kVIA_OK if successfully added consumer, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL AddConsumerByAddress(
    const VIAFbAddressHandle* inAddress,                        // [IN] Address to associate with the new service consumer
    VIAFbServiceConsumer** outConsumer) = 0;                    // [OUT] Service consumer handle if successful

  // Removes a consumer side dynamic FEP from the service.
  // [OUT] kVIA_OK if successfully removed, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RemoveConsumer(
    VIAFbServiceConsumer* inConsumer) = 0;                      // [IN] Service consumer handle to be removed.

  // Retrieves a service consumer identified by its binding ID.
  // [OUT] kVIA_OK if consumer was found, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetConsumer(
    VIAFbBindingId inBindingId,                                 // [IN] Binding specific identifier of the consumer
    VIAFbServiceConsumer** outConsumer) = 0;                    // [OUT] Service consumer handle if successful

  // Retrieves a consumer side service port identified by consumer and provider binding IDs.
  // [OUT] kVIA_OK if port was found, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetConsumedService(
    VIAFbBindingId inConsumerId,                                // [IN] Binding specific identifier of the consumer
    VIAFbBindingId inProviderId,                                // [IN] Binding specific identifier of the provider
    VIAFbConsumedService** outPort) = 0;                        // [OUT] Consumer side service port handle if successful

  // Adds a provider side dynamic FEP with given name and state to the service.
  // [OUT] kVIA_OK if successfully added provider, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL AddProviderByName(
    const char* inName,                                         // [IN] Name of the new service provider
    VIAFbFEPState inState,                                      // [IN] Whether the new provider is simulated or real
    VIAFbServiceProvider** outProvider) = 0;                    // [OUT] Service provider handle if successful

  // Adds a provider side dynamic FEP with given address to the service.
  // [OUT] kVIA_OK if successfully added provider, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL AddProviderByAddress(
    const VIAFbAddressHandle* inAddress,                        // [IN] Address to associate with the new service provider
    VIAFbServiceProvider** outProvider) = 0;                    // [OUT] Service provider handle if successful

  // Removes a provider side dynamic FEP from the service.
  // [OUT] kVIA_OK if successfully removed, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RemoveProvider(
    VIAFbServiceProvider* inProvider) = 0;                      // [IN] Service provider handle to be removed.

  // Retrieves a service provider identified by its binding ID.
  // [OUT] kVIA_OK if provider was found, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetProvider(
    VIAFbBindingId inBindingId,                                 // [IN] Binding specific identifier of the provider
    VIAFbServiceProvider** outProvider) = 0;                    // [OUT] Service provider handle if successful

  // Retrieves a provider side service port identified by consumer and provider binding IDs.
  // [OUT] kVIA_OK if port was found, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetProvidedService(
    VIAFbBindingId inConsumerId,                                // [IN] Binding specific identifier of the consumer
    VIAFbBindingId inProviderId,                                // [IN] Binding specific identifier of the provider
    VIAFbProvidedService** outPort) = 0;                        // [OUT] Provider side service port handle if successful

  // Register an observer for service discovery events.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbServiceDiscoveryObserver* inObserver,                  // [IN] Pointer to an object implementing the observer template methods
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered service discovery observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered service discovery callback
};

// -------------------------------------------------------------------
// VIA FunctionBus: Service Consumer/Provider access

class VIAFbServiceConsumer
{
public:
  // Release the service consumer instance, must be called for any service consumer instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Provides the consumer's binding specific identifier (kVIABindingIdInvalid if not bound to an address)
  // [OUT] kVIA_OK if successfully retrieved binding ID, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetBindingId(
    VIAFbBindingId* outBindingId) = 0;                          // [OUT] Binding identifier, if successful

  // Initiates service discovery from this consumer.
  // [OUT] kVIA_OK if successfully triggered service discovery, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL DiscoverProviders() = 0;

  // Sets the address of the service consumer itself, or (in the consumer model) sets the address of a known provider.
  // [OUT] kVIA_OK if address successfully set, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL SetAddress(
    const VIAFbAddressHandle* inAddress,                        // [IN] Address to store for this consumer
    VIAFbServiceProvider* inProvider) = 0;                      // [IN] Optional provider which connects to given address

  // Register an observer for service consumer events.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbServiceConsumerObserver* inObserver,                   // [IN] Pointer to an object implementing the observer template methods
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered service consumer observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered service consumer callback

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

class VIAFbServiceProvider
{
public:
  // Release the service provider instance, must be called for any service provider instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Provides the provider's binding specific identifier (kVIABindingIdInvalid if not bound to an address)
  // [OUT] kVIA_OK if successfully retrieved binding ID, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetBindingId(
    VIAFbBindingId* outBindingId) = 0;                          // [OUT] Binding identifier, if successful

  // Sets the address of the service provider itself, or (in the provider model) sets the address of a known consumer.
  // [OUT] kVIA_OK if address successfully set, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL SetAddress(
    const VIAFbAddressHandle* inAddress,                        // [IN] Address to store for this provider
    VIAFbServiceConsumer* inConsumer) = 0;                      // [IN] Optional consumer which connects to given address

  // Makes the high level server model announce the service and accept connections
  // [OUT] kVIA_OK if service is successfully provided, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL ProvideService() = 0;

  // Stops the high level server model, making the service unavailable
  // [OUT] kVIA_OK if service is successfully stopped, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL ReleaseService() = 0;

  // Provides whether the high level server model is currently providing the service
  // [OUT] kVIA_OK if service state is successfully retrieved, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL IsServiceProvided(
    bool* outFlag) = 0;                                         // [OUT] Indicates whether the service is currently available

  // Triggers low level service announcement messages
  // [OUT] kVIA_OK if service is successfully stopped, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL AnnounceProvider() = 0;

  // Stops low level service announcement messages
  // [OUT] kVIA_OK if service is successfully stopped, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnannounceProvider() = 0;

  // Retrieve the service state of the service provider.
  // [OUT] kVIA_OK if service state could be determined, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetServiceState(
    VIAFbServiceState* outState) = 0;                           // [OUT] The service state, if successful

  // Register an observer for service provider events.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbServiceProviderObserver* inObserver,                   // [IN] Pointer to an object implementing the observer template methods
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered service provider observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered service provider callback

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: Service Port access

class VIAFbConsumedService
{
public:
  // Release the service port instance, must be called for any service port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Makes the high level client/server model start requesting the service and connecting to the provider
  // [OUT] kVIA_OK if service is successfully stopped, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RequestService() = 0;

  // Stops requesting the service.
  // [OUT] kVIA_OK if service is successfully stopped, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL ReleaseService() = 0;

  // Returns the current client/server model state.
  // [OUT] kVIA_OK if service is successfully stopped, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL IsServiceRequested(
    bool* outFlag) = 0;                                         // [OUT] Indicates whether the service is currently being requested

  // Establishes a connection from the consumer side service port.
  // [OUT] kVIA_OK if connection is successfully initiated, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL ConnectAsync(
    VIAFbConsumedServiceConnectionHandler* inHandler) = 0;      // [IN] Pointer to an object implementing the handler template methods

  // Releases a connection from the consumer side service port.
  // [OUT] kVIA_OK if connection is successfully released, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL Disconnect() = 0;

  // Retrieve the connection state of the consumer side service port.
  // [OUT] kVIA_OK if connection state could be determined, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetConnectionState(
    VIAFbConsumerConnectionState* outState) = 0;                // [OUT] The connection state, if successful

  // Provides the consumer's binding specific identifier (kVIABindingIdInvalid if not bound to an address)
  // [OUT] kVIA_OK if successfully retrieved binding ID, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetConsumerId(
    VIAFbBindingId* outBindingId) = 0;                          // [OUT] Binding identifier, if successful

  // Provides the provider's binding specific identifier (kVIABindingIdInvalid if not bound to an address)
  // [OUT] kVIA_OK if successfully retrieved binding ID, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetProviderId(
    VIAFbBindingId* outBindingId) = 0;                          // [OUT] Binding identifier, if successful

  // Obtain access to this port generically for reading and writing values.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter
  // [THREADSAFE] Yes
  VIASTDDECL GetValuePort(
    VIAFbValuePort** outPort) const = 0;                        // [OUT] Port handle, if successful

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

class VIAFbProvidedService
{
public:
  // Release the service port instance, must be called for any service port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Register an observer for simulating server behavior. The callback is invoked for any function
  // being called on server side and shall modify output values of the function.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterSimulator(
    VIAFbFunctionServerObserver* inSimulator,                   // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered simulator.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterSimulator(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered simulator callback

  // Establishes a connection from the provider side service port.
  // [OUT] kVIA_OK if connection is successfully initiated, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL ConnectAsync(
    VIAFbProvidedServiceConnectionHandler* inHandler) = 0;     // [IN] Pointer to an object implementing the handler template methods

  // Releases a connection from the provider side service port.
  // [OUT] kVIA_OK if connection is successfully released, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL Disconnect() = 0;

  // Responds to a service request at this provider side service port.
  // [OUT] kVIA_OK if successfully announced the service, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL AnnounceProvider() = 0;

  // Retrieve the connection state of the provider side service port.
  // [OUT] kVIA_OK if connection state could be determined, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetConnectionState(
    VIAFbProviderConnectionState* outState) = 0;               // [OUT] The value object, if successful

  // Provides the consumer's binding specific identifier (kVIABindingIdInvalid if not bound to an address)
  // [OUT] kVIA_OK if successfully retrieved binding ID, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetConsumerId(
    VIAFbBindingId* outBindingId) = 0;                          // [OUT] Binding identifier, if successful

  // Provides the provider's binding specific identifier (kVIABindingIdInvalid if not bound to an address)
  // [OUT] kVIA_OK if successfully retrieved binding ID, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetProviderId(
    VIAFbBindingId* outBindingId) = 0;                          // [OUT] Binding identifier, if successful

  // Obtain access to this port generically for reading and writing values.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter
  // [THREADSAFE] Yes
  VIASTDDECL GetValuePort(
    VIAFbValuePort** outPort) const = 0;                        // [OUT] Port handle, if successful

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: Function client/server ports

class VIAFbFunctionClientPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Obtain access to the FunctionBus data type of function's input argument for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] No
  VIASTDDECL GetInParamsType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Obtain access to the FunctionBus data type of function's output argument for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] No
  VIASTDDECL GetOutParamsType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Initiate a function call. The call is invoked by calling InvokeCall on the handle obtained. 
  // Note: The obtained outCallContext must be released explicitly via inCallContext->Release.
  // [OUT] kVIA_OK if a call handle could be retrieved, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL BeginCall(
    VIAFbCallContext** outCallContext) const = 0;               // [OUT] Context of the call, if successful

  // Trigger function call that was previously initiated, typically after all in arguments are updated.
  // The observer is notified about all state changes for this call.
  // [OUT] kVIA_OK if the call was successfully sent, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle
  // [THREADSAFE] No
  VIASTDDECL InvokeCall(
    VIAFbFunctionClientObserver* inObserver,                    // [IN] Pointer to an object implementing the observer template method (optional, can be nullptr)
    VIAFbCallContext* inCallContext) = 0;                       // [IN] Context of a call obtained from BeginCall

  // Register an observer to be notified when calls handled by this port change their state.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbFunctionClientObserver* inObserver,                    // [IN] Pointer to an object implementing the observer template method
    VIAFbFunctionCallState inStateToObserve,                    // [IN] The observer is triggered for any call reaching this state
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inCallbackHandle) = 0;            // [IN] Handle from a previously registered observer callback 

  // Obtain a value handle containing all input arguments of this port's most recent call.
  // [OUT] kVIA_OK if parameters could be retrieved, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetLatestCall(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level the value is accessed
    VIAFbValue** outInParamsValue) const = 0;                   // [OUT] The value object, if successful

  // Obtain a value handle containing all output arguments of this port's most recently received response.
  // [OUT] kVIA_OK if parameters could be retrieved, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetLatestReturn(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level the value is accessed
    VIAFbValue** outOutParamsValue) const = 0;                  // [OUT] The value object, if successful

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

class VIAFbFunctionServerPort
{
public:
  // Release the port instance, must be called for any port instance obtained from the VIA service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Obtain access to the FunctionBus data type of function's input argument for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] No
  VIASTDDECL GetInParamsType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Obtain access to the FunctionBus data type of function's output argument for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] No
  VIASTDDECL GetOutParamsType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Obtain a value handle containing all default output arguments of this port.
  // The first members is a struct with default values for all in/inout arguments (in the same order as the struct obtained from GetOutParams).
  // Inout arguments are accessible as a struct named by the argument. The struct can be used to configure the VSIM for automatic replies:
  // * struct member named "Value" of argument type: value to reply
  // * struct member named "Behavior" with values "UseDefaultValue" = 0, "UseInValue" = 1
  // Additional members of the VSIM functionality are appended: 
  // * A special member named "ReturnValue" refers to the return value, it is only defined if the signature's return type is not void.
  // * A special member named "ServerSimulatorMode" with values "Auto" = 0, "OffOrManual" = 1, "Discard" = 2
  // * A special member named "ReturnDelay" with the return to reply time in nano seconds
  // [OUT] kVIA_OK if a call handle could be retrieved, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetDefaultOutParams(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outDefaultOutParamsValue) const = 0;           // [OUT] The value object, if successful

  // Update the default output arguments for this port.
  // [OUT] kVIA_OK if value successfully set, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL SetDefaultOutParams(
    const VIAFbValue* inDefaultOutParamsValue) const = 0;       // [IN] The value object

  // Register an observer to be notified when calls handled by this port change their state.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbFunctionServerObserver* inObserver,                    // [IN] Pointer to an object implementing the observer template method
    VIAFbFunctionCallState inStateToObserve,                    // [IN] The observer is triggered for any call reaching this state
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inCallbackHandle) = 0;            // [IN] Handle from a previously registered observer callback 

  // Obtain a value handle containing all input arguments of this port's most recently received call request.
  // [OUT] kVIA_OK if parameters could be retrieved, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetLatestCall(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level the value is accessed
    VIAFbValue** outInParamsValue) const = 0;                   // [OUT] The value object, if successful

  // Obtain a value handle containing all output arguments of this port's most recent returned call.
  // [OUT] kVIA_OK if parameters could be retrieved, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetLatestReturn(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level the value is accessed
    VIAFbValue** outOutParamsValue) const = 0;                  // [OUT] The value object, if successful

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: CallContext

// A function call's context. Encapsulates the state and parameters of a function call being processed by client or server side function ports.
class VIAFbCallContext
{
public:
  // Release the call context, must be called for any obtained call context to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Obtain a value handle containing all input arguments of this call.
  // * A special member named "RequestId" refers to the internal ID used to identify this call instance, it is ascertained to be globally unique per simulation run.
  // [OUT] kVIA_OK if parameters could be retrieved, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetInParams(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outInParamsValue) const = 0;                   // [OUT] The value object, if successful

  // Obtain a value handle containing all output arguments of this call.
  // Additional members of the VSIM functionality are appended: 
  // * A special member named "RequestId" refers to the internal ID used to identify this call instance, it is ascertained to be globally unique per simulation run.
  // * A special member named "ReturnValue" refers to the return value, it is only defined if the signature's return type is not void.
  // [OUT] kVIA_OK if parameters could be retrieved, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetOutParams(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbValue** outOutParamsValue) const = 0;                  // [OUT] The value object, if successful

  // Update the in parameters value for a particular call.
  // [OUT] kVIA_OK if value successfully updated, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL SetInParams(
    const VIAFbValue* inInParamsValue) = 0;                     // [IN] The value object

  // Update the out parameters value for a particular call.
  // [OUT] kVIA_OK if value successfully updated, kVIA_ParameterInvalid/kVIA_ObjectInvalid in case of a bad parameter or handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL SetOutParams(
    const VIAFbValue* inOutParamsValue) = 0;                    // [IN] The value object

  // Update the time to reply in a particular call context.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL SetTimeToReply(
    VIATimeDiff inRelativeTimeDeltaToReply) = 0;                // [IN] The new time to reply in ns and relative to the current simulation time

  // Get the simulation time when this call was invoked or received.
  // [OUT] kVIA_OK if call time is available, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetCallTime(
    VIATime* outTime) const = 0;                                // [OUT] The time at which the call was invoked (client side) or received (server side)

  // Get the simulation time when this call was returned or a response received.
  // [OUT] kVIA_OK if return time is available, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetReturnTime(
    VIATime* outTime) const = 0;                                // [OUT] The time at which the call was returned (server side) or a response received (client side)

  // Get the current state of this call.
  // [OUT] kVIA_OK if current state can be determined, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetCallState(
    VIAFbFunctionCallState* outState) const = 0;                // [OUT] The current state of the call

  // Determine whether this call is being processed on client or server side.
  // [OUT] kVIA_OK if side can be determined, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetSide(
    VIAFbFunctionCallSide* outSide) const = 0;                  // [OUT] Whether this call is being processed on client or server side

  // Obtain the path to this call's client port.
  // [OUT] kVIA_OK if path was retrieved, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetClientPort(
    char* outBuffer,                                            // [OUT] Pointer to the string buffer to be filled
    size_t inBufferLength) const = 0;                           // [IN] Size in bytes of the string buffer

  // Obtain the path to this call's server port.
  // [OUT] kVIA_OK if path was retrieved, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetServerPort(
    char* outBuffer,                                            // [OUT] Pointer to the string buffer to be filled
    size_t inBufferLength) const = 0;                           // [IN] Size in bytes of the string buffer

  // Create a permanent call context from this, which can be used beyond teh lifetime of a callback and must be released explicitly.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL MakePermanent(
    VIAFbCallContext** outPermanentContext) const = 0;          // [OUT] The permanent call context object

  // Obtain the unique request ID of this call context.
  // [OUT] kVIA_OK if request ID is valid, kVIA_ObjectInvalid in case of a bad handle
  // [THREADSAFE] Yes
  VIASTDDECL GetRequestID(
    long long* outRequestID) const = 0;                         // [OUT] The call context's request ID

  // Set out parameters and return value to defaults.
  // [OUT] kVIA_OK if answer was set to defaults, kVIA_ObjectInvalid in case of a bad handle
  // [THREADSAFE] No
  VIASTDDECL SetDefaultAnswer() const = 0;
};

// -------------------------------------------------------------------
// VIA FunctionBus: Value

// A ValueEntity's value. Encapsulates a VVariant and provides access to underlying ISVDataType which can be used to modify members.
// Note: the value is cached and read access does not imply updating the value from the value entity. Modification of members or the
// whole value will only be committed to the value entity through explicit calls on a port or call context object.
class VIAFbValue
{
public:
  // Release the value, must be called for any obtained value to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve type of this particular value. Reflection on type is required to obtain a member handle.
  // [OUT] kVIA_OK if type could be returned, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL GetType(
    VIAFbType** outType) const = 0;                             // [OUT] Type of this value

  // Get the access level of this type (Impl/Phys/Raw). To access values at a different level, another value must be retrieved from the port.
  // [OUT] kVIA_OK if type level could be retrieved, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL GetLevel(
    VIAFbTypeLevel* outLevel) const = 0;                        // [OUT] Type level used when accessing this value

  // -------------------------------------------------------------------
  // Getting/setting value of members and root (the latter when inMemberHandle is nullptr)

  // Set value. Value the member handle is pointing to must be of type signed integer.
  // [OUT] kVIA_OK if value could be set, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL SetSignedInteger(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    int64  inValue) = 0;                                        // [IN] Value to be set

  // Get value. Value the member handle is pointing to must be of type signed integer.
  // [OUT] kVIA_OK if value could be retrieved, kVIA_BufferToSmall if buffer is too small, or kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL GetSignedInteger(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    int64* outValue) const = 0;                                 // [OUT] Pointer to a variable the value will be written to

  // Set value. Value the member handle is pointing to must be of type unsigned integer.
  // [OUT] kVIA_OK if value could be set, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL SetUnsignedInteger(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    uint64  inValue) = 0;                                       // [IN] Value to be set

  // Get value. Value the member handle is pointing to must be of type unsigned integer.
  // [OUT] kVIA_OK if value could be retrieved, kVIA_BufferToSmall if buffer is too small, or kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL GetUnsignedInteger(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    uint64* outValue) const = 0;                                // [OUT] Pointer to a variable the value will be written to

  // Set value. Value the member handle is pointing to must be of type float.
  // [OUT] kVIA_OK if value could be set, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL SetFloat(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    double  inValue) = 0;                                       // [IN] Value to be set

  // Get value. Value the member handle is pointing to must be of type float.
  // [OUT] kVIA_OK if value could be retrieved, kVIA_BufferToSmall if buffer is too small, or kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL GetFloat(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    double* outValue) const = 0;                                // [OUT] Pointer to a variable the value will be written to

  // Set value to data already in the same format as serialized by the FunctionBus. 
  // Value the member handle is pointing to must be of type array or string.
  // This can lead to undefined behavior if type or serialization format do not match.
  // [OUT] kVIA_OK if value could be set, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL SetData(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    const void* inValueBuffer,                                  // [IN] Pointer to a memory buffer of values to be set
    size_t inNumBytes) = 0;                                     // [IN] Size of memory buffer in bytes

  // Get data value as serialized by the FunctionBus. Value the member handle is pointing to must be of type array or string.
  // This can lead to undefined behavior if type or serialization format do not match.
  // [OUT] kVIA_OK if value could be retrieved, kVIA_BufferToSmall if buffer is too small, or kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL GetData(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    void* outValueBuffer,                                       // [OUT] Pointer to a memory buffer the values will be written to
    size_t inNumBytes) const = 0;                               // [IN] Size of memory buffer in bytes

  // -------------------------------------------------------------------
  // Getting/setting length of array

  // Set array length. This value must be an array type.
  // [OUT] kVIA_OK if array length could be set, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL SetArrayLength(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    size_t  inLength) = 0;                                      // [IN] Array length to be set for this value

  // Set array length. This value must be an array type.
  // [OUT] kVIA_OK if array length could be determined, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL GetArrayLength(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    size_t* outLength) const = 0;                               // [OUT] Array length of this value

  // -------------------------------------------------------------------
  // Check/Set if struct member is valid

  // Set a member of this value as a valid member. Value must be of composite type.
  // [OUT] kVIA_OK if flag could be set, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL SetIsValid(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    bool inFlag) = 0;                                           // [IN] Flag that is true iff this member is valid

  // Determine if a member of this value is a valid. Value must be of composite type.
  // [OUT] kVIA_OK if flag could be determined, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL IsValid(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    bool* outFlag) const = 0;                                   // [OUT] Flag that is true iff this member is valid

  // Get currently valid member of a union. Value must be of union type.
  // [OUT] kVIA_OK if member index could be retrieved, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL GetValidMemberIndex(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    size_t* outIndex) const = 0;                                // [OUT] Index of the member that is valid

  // -------------------------------------------------------------------
  // Getting/setting string values (in UTF8 encoding)

  // Set value. Value the member handle is pointing to must be of type string.
  // [OUT] kVIA_OK if value could be set, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL SetString(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    const char* inValue) = 0;                                   // [IN] Value to be set

  // Get value. Value the member handle is pointing to must be of type string.
  // [OUT] kVIA_OK if value could be retrieved, kVIA_BufferToSmall if buffer is too small, or kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL GetString(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    char* outBuffer,                                            // [OUT] Pointer to the string buffer to be filled
    size_t inBufferLength) const = 0;                           // [IN] Size in bytes of the string buffer

  // Get access to type of member of complex data type value.
  // [OUT] kVIA_OK if type could be retrieved, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetMemberType(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of the composite value
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level the member type is accessed
    VIAFbType** outMemberType) const = 0;                       // [OUT] The type object, if successful

  // Get access to member of complex data type value.
  // [OUT] kVIA_OK if value could be retrieved, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetMemberValue(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of the composite value
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level the member value is accessed
    VIAFbValue** outMemberValue) const = 0;                     // [OUT] The value object, if successful

  // Get access to status of member of complex data type value.
  // [OUT] kVIA_OK if status could be retrieved, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetMemberStatus(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of the composite value
    VIAFbStatus** outMemberStatus) const = 0;                   // [OUT] The status object, if successful

  // Get the size of a raw data buffer required for use in GetData function.
  // [OUT] kVIA_OK if the size could be determined, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetSize(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of the composite value
    size_t* outSize) const = 0;                                 // [OUT] The number of bytes required for raw data access

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Commits any pending value modifications to the associated value entity.
  // [OUT] kVIA_OK if the value was successfully committed, kVIA_Failed otherwise
  // [THREADSAFE] No
  VIASTDDECL Commit() = 0;
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: Status

// A ValueEntity's status. Provides access to timestamps of value entity updates and changes.
// Note: as opposed to VIAFbValue the status always operates live on the underlying value entity.
class VIAFbStatus
{
public:
  // Release the status, must be called for any obtained status to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Retrieve the value of the associated value entity.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetValue(
    VIAFbTypeLevel inLevel,                                     // [IN] Decide on which type level this value is accessed
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member if this is a composite type
    VIAFbValue** outValue) const = 0;                           // [OUT] The value object, if successful

  // Obtain access to the value entities data type for reflection purposes.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a nullptr
  // [THREADSAFE] No
  VIASTDDECL GetType(
    VIAFbTypeLevel inLevel,                                     // [IN] The type level of the requested type
    VIAFbType** outType) const = 0;                             // [OUT] Type handle, if successful

  // Get the current value state, i.e. whether the value entity was measured or is offline
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL GetValueState(
    VIAFbValueState* outState) const = 0;                       // [OUT] Current state of the value entity

  // Retrieve the absolute time in nanoseconds when this value entity was last updated.
  // [OUT] kVIA_OK if the query was successful, kVIA_Failed in case update never happened
  // [THREADSAFE] No
  VIASTDDECL GetLastUpdateTimestamp(
    VIATime* outTime) const = 0;                                // [OUT] Time of last update, if update occurred

  // Retrieve the absolute time in nanoseconds when this value entity was last changed.
  // [OUT] kVIA_OK if the query was successful, kVIA_Failed in case change never happened
  // [THREADSAFE] No
  VIASTDDECL GetLastChangeTimestamp(
    VIATime* outTime) const = 0;                                // [OUT] Time of last change, if change occurred

  // Register an observer to be notified when this value entity is updated or changed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbValueObserver* inObserver,                             // [IN] Pointer to an object implementing the observer template method
    VIAFbUpdateMode inUpdateMode,                               // [IN] Decide to observe only changes in value or any value updates
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inCallbackHandle) = 0;            // [IN] Handle from a previously registered observer callback 

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 7)
  // Reset the status to a state similar to the one at measurement start:
  // Sets the value to the initial value (or zero); sets the value state to Offline; resets change / update counters and flags.
  // The new value is propagated to the CANoe clients, but the binding block doesn't transmit it.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL ResetStatus() = 0;
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: Type

// FunctionBus type for reflection and value access
class VIAFbType
{
public:
  // Release the type, must be called for any obtained type to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // -------------------------------------------------------------------
  // General type information

  // Retrieve the name of this type
  // [OUT] kVIA_OK if successful, kVIA_ParameterInvalid if buffer too small
  // [THREADSAFE] Yes
  VIASTDDECL GetName(
    char* outBuffer,                                            // [OUT] Pointer to the string buffer to be filled
    size_t inBufferLength) const = 0;                           // [IN] Size in bytes of the string buffer

  // Retrieve the name of this type including the full FunctionBus path.
  // [OUT] kVIA_OK if successful, kVIA_ParameterInvalid if buffer too small
  // [THREADSAFE] Yes
  VIASTDDECL GetFullName(
    char* outBuffer,                                            // [OUT] Pointer to the string buffer to be filled
    size_t inBufferLength) const = 0;                           // [IN] Size in bytes of the string buffer

  // Determine this type's type tag.
  // [OUT] kVIA_OK if type tag could be determined, kVIA_Failed in case of error
  // [THREADSAFE] Yes
  VIASTDDECL GetTypeTag(
    VIAFbTypeTag* outTypeTag) const = 0;                        // [OUT] Type tag

  // -------------------------------------------------------------------
  // Simple types: Integer, Float, String

  // Retrieve the number of bits reserved for this type.
  // [OUT] kVIA_OK if successful, kVIA_Failed in case of error
  // [THREADSAFE] Yes
  VIASTDDECL GetBitCount(
    size_t* outCount) const = 0;                                // [OUT] Number of bits

  // Determine for an integer type if it is signed or unsigned.
  // [OUT] kVIA_OK if successful, kVIA_Failed if this type is not an integer type
  // [THREADSAFE] Yes
  VIASTDDECL IsSigned(
    bool* outFlag) const = 0;                                   // [OUT] True iff this type is signed

  // Determine the length of a string type.
  // This is equivalent to GetArrayMaxLength, because a string is internally defined as an array of uint8.
  // [OUT] kVIA_OK if successful, kVIA_Failed if this type is not a string type
  // [THREADSAFE] Yes
  VIASTDDECL GetStringMaxLength(
    size_t* outLength) const = 0;                               // [OUT] Number of 8 bit characters this string can hold

  // Determine the string encoding as Microsoft code page identifier used for display, e.g. 65001 for UTF-8.
  // [OUT] kVIA_OK if encoding could be determined, kVIA_Failed in case of error
  // [THREADSAFE] Yes
  VIASTDDECL GetStringEncoding(
    VIAStringEncoding* outStringEncoding) const = 0;            // [OUT] Encoding enum value

  // -------------------------------------------------------------------
  // Array type

  // Retrieve type of the members of this array type.
  // [OUT] kVIA_OK if type could be returned, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL GetArrayBaseType(
    VIAFbType** outType) const = 0;                             // [OUT] Type of this particular member

  // Retrieve maximum number of members specified for this array type.
  // [OUT] kVIA_OK if number could be determined, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL GetArrayMaxLength(
    size_t* outLength) const = 0;                               // [OUT] Maximum number of members this array type can hold

  // Retrieve minimum number of members specified for this array type.
  // [OUT] kVIA_OK if number could be determined, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL GetArrayMinLength(
    size_t* outLength) const = 0;                               // [OUT] Minimum number of members this array type must hold

  // Determine if values of this array type have minimum and maximum length set to an equal value.
  // [OUT] kVIA_OK if flag could be determined, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL HasArrayFixedLength(
    bool* outFlag) const = 0;                                   // [OUT] Flag that is true iff arrays of this type are of static size

  // -------------------------------------------------------------------
  // Composite types: Struct, Union

  // Get member handle of this type (relative position within the value entity).
  // [OUT] kVIA_OK
  // [THREADSAFE] Yes
  VIASTDDECL GetMemberHandle(
    VIAFbTypeMemberHandle* outMemberHandle) const = 0;          // [OUT] Member handle of this type

  // Obtain parent member of a previously obtained member handle. This type must be a composite type.
  // [OUT] kVIA_OK if member handle could be determined, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL GetParentMember(
    VIAFbTypeMemberHandle* inoutMemberHandle) const = 0;        // [INOUT] Parent member of ingoing member (can be set to kVIAFbTypeMemberHandleWhole for root)

  // Obtain a member handle relative to a previously obtained member handle, or relative to the root of this composite type.
  // [OUT] kVIA_OK if member handle could be determined, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL GetMember(
    const char* inRelativePath,                                 // [IN] Relative FunctionBus path
    VIAFbTypeMemberHandle* inoutMemberHandle) const = 0;        // [INOUT] Member relative to ingoing member (can be set to kVIAFbTypeMemberHandleWhole for root)

  // Obtain a member handle relative to a previously obtained member handle, or relative to the root of this composite type.
  // [OUT] kVIA_OK if member handle could be determined, else kVIA_Failed
  // [THREADSAFE] No
  VIASTDDECL GetMemberByIndex(
    size_t inIndex,                                             // [IN] Index of n-th sub member of the member specified by the given member handle
    VIAFbTypeMemberHandle* inoutMemberHandle) const = 0;        // [INOUT] Member relative to ingoing member (set to kVIAFbTypeMemberHandleWhole for root)

  // Retrieve type of a member of this composite type.
  // [OUT] kVIA_OK if type could be returned, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL GetMemberType(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    VIAFbType** outType) const = 0;                             // [OUT] Type of this particular member

  // Get number of directly specified members of this composite type.
  // [OUT] kVIA_OK if number could be determined, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL GetMemberCount(
    size_t* outCount) const = 0;                                // [OUT] Number of directly contained members

  // -------------------------------------------------------------------
  // Composite types: Memory layout applied on raw and implementation type level

  // Determine if a member of this composite type is an optional member. Type must not have a fixed binary layout.
  // [OUT] kVIA_OK if layout could be determined, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL HasFixedBinaryLayout(
    bool* outFlag) const = 0;                                   // [OUT] Flag that is true iff layout is fixed

  // Determine padding of a member of this composite type relative to the preceding member. Binary layout must be fixed.
  // [OUT] kVIA_OK if position could be determined, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL GetMemberRelativeBitOffset(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    size_t* outOffset) const = 0;                               // [OUT] Position relative to previous member

  // Determine position of a member of this composite type relative to this composite type. Binary layout must be fixed.
  // [OUT] kVIA_OK if position could be determined, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL GetMemberAbsoluteBitOffset(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    size_t* outOffset) const = 0;                               // [OUT] Position relative to this composite type

  // For composite types with fixed binary layout: Determine whether a member's byte order is Big-Endian instead of Little-Endian
  // [OUT] kVIA_OK if byte order could be determined, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL GetMemberByteOrder(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    VIAByteOrder* outByteOrder) const = 0;                      // [OUT] Byte order of the member

  // Determine if a member of this composite type is an optional member.
  // [OUT] kVIA_OK if flag could be determined, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL IsMemberOptional(
    VIAFbTypeMemberHandle inMemberHandle,                       // [IN] Handle to a member of this composite type
    bool* outFlag) const = 0;                                   // [OUT] Flag that is true iff this member is optional

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 10)
  // For arrays: determine if there is a maximum or if the array is unbounded.
  // [OUT] kVIA_OK if flag could be determined, else kVIA_Failed
  // [THREADSAFE] Yes
  VIASTDDECL IsArrayUnbounded(
    bool* outFlag) const = 0;                                   // [OUT] Flag that is true iff this array is unbounded
#endif
};

// -------------------------------------------------------------------
// VIA FunctionBus: Abstract Binding

class VIAFbAbstractBinding
{
public:
  // Obtain access to a provider side Event end point with Abstract Binding specific functionality.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if end point does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetEventProvider(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbAbstractEventProvider** outProvider) const = 0;        // [OUT] Event end point handle, if successful

  // Obtain access to a consumer side Event port with Abstract Binding specific functionality.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetConsumedEvent(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbAbstractConsumedEvent** outPort) const = 0;            // [OUT] Event consumer side port handle, if successful

  // Obtain access to a provider side service PDU end point with Abstract Binding specific functionality.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if end point does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetServicePDUProvider(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbAbstractServicePDUProvider** outProvider) const = 0;   // [OUT] Service PDU end point handle, if successful

  // Obtain access to a consumer side service PDU port with Abstract Binding specific functionality.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetConsumedServicePDU(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbAbstractConsumedServicePDU** outPort) const = 0;       // [OUT] Service PDU consumer side port handle, if successful

  // Creates an Abstract Binding specific address.
  // [OUT] kVIA_OK if address is successfully created, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL CreateAddress(
    VIAFbServiceCO* inService,                                  // [IN] The service for which to create an address
    const char* inEndPointName,                                 // [IN] Name of the end point to associate with the address
    VIAFbAddressHandle** outAddress) const = 0;                 // [OUT] The new Abstract Binding specific address

  // Provides the binding identifier associated with an Abstract Binding specific address.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetBindingId(
    VIAFbAddressHandle* inAddress,                              // [IN] The Abstract Binding specific address for which to obtain the identifier
    VIAFbBindingId* outBindingId) const = 0;                    // [OUT] The associated binding identifier

  // Provides the display name associated with an Abstract Binding specific address.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetDisplayName(
    VIAFbAddressHandle* inAddress,                              // [IN] The Abstract Binding specific address for which to obtain the display name
    char* outBuffer,                                            // [OUT] Pointer to the string buffer to be filled
    size_t inBufferLength) const = 0;                           // [IN] Size in bytes of the string buffer

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 9)
  // Obtain access to a provider side Field end point with Abstract Binding specific functionality.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if end point does not exist, kVIA_ObjectInvalid if field has no notification
  // [THREADSAFE] Yes
  VIASTDDECL GetFieldProvider(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbAbstractFieldProvider** outProvider) const = 0;        // [OUT] Field end point handle, if successful

  // Obtain access to a consumer side Field port with Abstract Binding specific functionality.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if port does not exist, kVIA_ObjectInvalid if field has no notification
  // [THREADSAFE] Yes
  VIASTDDECL GetConsumedField(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbAbstractConsumedField** outPort) const = 0;            // [OUT] Field consumer side port handle, if successful
#endif
};

class VIAFbAbstractConsumedEvent : public VIAFbConsumedEvent
{
public:
  // Release the Event port instance, must be called for any Event port instance obtained from the Abstract Binding service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Initiates high level event request via Abstract Binding.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RequestEvent() = 0;

  // Releases high level event request via Abstract Binding.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL ReleaseEvent() = 0;

  // Provides high level event request state from Abstract Binding.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL IsEventRequested(
    bool* outFlag) = 0;                                         // [OUT] The event's request state

  // Subscribes the consumer side event port low level.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL SubscribeEvent() = 0;

  // Unsubscribes the consumer side event port low level.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnsubscribeEvent() = 0;
};

class VIAFbAbstractEventProvider : public VIAFbEventProvider
{
public:
  // Release the Event provider instance, must be called for any Event provider instance obtained from the Abstract Binding service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Register an observer to be notified upon event subscription.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbAbstractEventSubscriptionObserver* inObserver,         // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inCallbackHandle) = 0;            // [IN] Handle from a previously registered observer callback 
};

class VIAFbAbstractConsumedServicePDU : public VIAFbConsumedServicePDU
{
public:
  // Release the Service PDU port instance, must be called for any Service PDU port instance obtained from the Abstract Binding service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Triggers the high level PDU subscription model to request PDU subscription
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL RequestPDU() = 0;

  // Releases high level PDU subscription
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL ReleasePDU() = 0;

  // Checks whether PDU subscription is currently requested.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL IsPDURequested(
    bool* outFlag) = 0;                                         // [OUT] The current request state of the PDU

  // Initiates low level PDU subscription at this consumer side port.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL SubscribePDU() = 0;

  // Initiates low level PDU desubscription at this consumer side port.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] No
  VIASTDDECL UnsubscribePDU() = 0;
};

class VIAFbAbstractServicePDUProvider : public VIAFbServicePDUProvider
{
public:
  // Release the Service PDU provider instance, must be called for any Service PDU provider instance obtained from the Abstract Binding service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Register an observer to be notified when a consumer has subscribed at the PDU provider.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbAbstractPDUSubscriptionObserver* inObserver,           // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 
};

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 9)
class VIAFbAbstractConsumedField : public VIAFbConsumedField
{
public:
  // Release the Field port instance, must be called for any Field port instance obtained from the Abstract Binding service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Initiates high level field request via Abstract Binding.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RequestField() = 0;

  // Releases high level field request via Abstract Binding.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL ReleaseField() = 0;

  // Provides high level field request state from Abstract Binding.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL IsFieldRequested(
    bool* outFlag) = 0;                                         // [OUT] The field's request state

  // Subscribes the consumer side field port low level.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL SubscribeField() = 0;

  // Unsubscribes the consumer side field port low level.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnsubscribeField() = 0;
};

class VIAFbAbstractFieldProvider : public VIAFbFieldProvider
{
public:
  // Release the Field provider instance, must be called for any Field provider instance obtained from the Abstract Binding service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Register an observer to be notified upon field subscription.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbAbstractFieldSubscriptionObserver* inObserver,         // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inCallbackHandle) = 0;            // [IN] Handle from a previously registered observer callback 
};
#endif

// -------------------------------------------------------------------
// VIA FunctionBus: SOME/IP Binding

class VIAFbSomeIPBinding
{
public:
  // Obtain access to a provider side event group end point.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if end point does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetEventGroupProvider(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbSomeIPEventGroupProvider** outProvider) const = 0;     // [OUT] Event group end point handle, if successful

  // Obtain access to a consumer side event group port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if end point does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetConsumedEventGroup(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbSomeIPConsumedEventGroup** outPort) const = 0;         // [OUT] Event group port handle, if successful

  // Obtain access to a provider side event group port.
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_ObjectNotFound if end point does not exist
  // [THREADSAFE] Yes
  VIASTDDECL GetProvidedEventGroup(
    const char* inPath,                                         // [IN] FunctionBus model path identifier, pointer to a zero-terminated ASCII string
    VIAFbSomeIPProvidedEventGroup** outPort) const = 0;         // [OUT] Event group port handle, if successful

  // Creates an SOME/IP Binding specific address.
  // [OUT] kVIA_OK if address is successfully created, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL CreateAddress(
    const char* inUDPAddress,                                   // [IN] The IPv4 or IPv6 format address for unreliable communication
    long inUDPPort,                                             // [IN] The port number for unreliable communication
    const char* inTCPAddress,                                   // [IN] The IPv4 or IPv6 format address for reliable communication
    long inTCPPort,                                             // [IN] The port number for reliable communication
    VIAFbAddressHandle** outAddress) const = 0;                 // [OUT] The new SOME/IP Binding specific address

  // Provides the binding identifier associated with an SOME/IP Binding specific address.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetBindingId(
    VIAFbAddressHandle* inAddress,                              // [IN] The SOME/IP Binding specific address for which to obtain the identifier
    VIAFbBindingId* outBindingId) const = 0;                    // [OUT] The associated binding identifier

  // Provides the display name associated with an SOME/IP Binding specific address.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL GetDisplayName(
    VIAFbAddressHandle* inAddress,                              // [IN] The SOME/IP Binding specific address for which to obtain the display name
    char* outBuffer,                                            // [OUT] Pointer to the string buffer to be filled
    size_t inBufferLength) const = 0;                           // [IN] Size in bytes of the string buffer
};

class VIAFbSomeIPEventGroupProvider
{
public:
  // Release the SOME/IP Event Group instance, must be called for any SOME/IP Event Group instance obtained from the SomeIP Binding service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Returns the event group's integer identifier
  // [OUT] kVIA_OK if successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetEventGroupId(
    unsigned long* outEventGroupId) const = 0;                  // [OUT] The integer identifier of this event group

  // Provides an iterator over the events of this event group.
  // [OUT] kVIA_OK if event iterator was successfully created, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetEvents(
    VIAFbEventIterator<VIAFbEventProvider>** outIterator) = 0;  // [OUT] An iterator over the contained event providers

  // Retrieve an iterator over all service consumers currently subscribed at the event group provider
  // [OUT] kVIA_OK if subscribed consumers could be determined, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetSubscribedConsumers(
    VIAFbServiceConsumerIterator** outIterator) = 0;            // [OUT] An iterator over the currently subscribed consumers

  // Register an observer to be notified upon event group subscription.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterObserver(
    VIAFbSomeIPEventGroupSubscriptionObserver* inObserver,      // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterObserver(
    const VIAFbCallbackHandle inCallbackHandle) = 0;            // [IN] Handle from a previously registered observer callback 

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 

  // Provides an iterator over the PDUs of this event group.
  // [OUT] kVIA_OK if PDU iterator was successfully created, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPDUs(
    VIAFbPDUIterator<VIAFbServicePDUProvider>** outIterator) = 0;  // [OUT] An iterator over the contained PDU providers
#endif

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 9)
  // Provides an iterator over the fields of this event group.
  // [OUT] kVIA_OK if field iterator was successfully created, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetFields(
    VIAFbFieldIterator<VIAFbFieldProvider>** outIterator) = 0;  // [OUT] An iterator over the contained field providers
#endif
};

class VIAFbSomeIPConsumedEventGroup
{
public:
  // Release the SOME/IP Event Group instance, must be called for any SOME/IP Event Group instance obtained from the SomeIP Binding service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Returns the event group's integer identifier
  // [OUT] kVIA_OK if successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetEventGroupId(
    unsigned long* outEventGroupId) const = 0;                  // [OUT] The integer identifier of this event group

  // Provides an iterator over the events of this event group.
  // [OUT] kVIA_OK if event iterator was successfully created, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetEvents(
    VIAFbEventIterator<VIAFbConsumedEvent>** outIterator) = 0;  // [OUT] An iterator over the contained consumed events

  // Initiates high level event group request via SOME/IP Binding.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RequestEventGroup() = 0;

  // Releases high level event group request via SOME/IP Binding.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL ReleaseEventGroup() = 0;

  // Provides high level event group request state from SOME/IP Binding.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL IsEventGroupRequested(
    bool* outFlag) = 0;                                         // [OUT] The event group's request state

  // Subscribes the consumer side event group port low level.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL SubscribeEventGroup() = 0;

  // Unsubscribes the consumer side event group port low level.
  // [OUT] kVIA_OK if the query is successful, kVIA_ParameterInvalid in case of a bad parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnsubscribeEventGroup() = 0;

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 

  // Provides an iterator over the PDUs of this event group.
  // [OUT] kVIA_OK if PDU iterator was successfully created, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPDUs(
    VIAFbPDUIterator<VIAFbConsumedServicePDU>** outIterator) = 0;  // [OUT] An iterator over the contained consumed PDUs
#endif

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 9)
  // Provides an iterator over the fields of this event group.
  // [OUT] kVIA_OK if field iterator was successfully created, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetFields(
    VIAFbFieldIterator<VIAFbConsumedField>** outIterator) = 0;  // [OUT] An iterator over the contained consumed fields
#endif
};

class VIAFbSomeIPProvidedEventGroup
{
public:
  // Release the SOME/IP Event Group instance, must be called for any SOME/IP Event Group instance obtained from the SomeIP Binding service to free resources after use.
  // [OUT] kVIA_OK is always returned
  // [THREADSAFE] Yes
  VIASTDDECL Release() = 0;

  // Returns the event group's integer identifier
  // [OUT] kVIA_OK if successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetEventGroupId(
    unsigned long* outEventGroupId) const = 0;                  // [OUT] The integer identifier of this event group

  // Provides an iterator over the events of this event group.
  // [OUT] kVIA_OK if event iterator was successfully created, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetEvents(
    VIAFbEventIterator<VIAFbProvidedEvent>** outIterator) = 0;  // [OUT] An iterator over the contained provided events

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 8)
  // Returns the port server's unique identifier for this port
  // [OUT] kVIA_OK if the query was successful, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPortID(
    unsigned long long* outPortID) const = 0;                   // [OUT] This port's unique identifier, if successful

  // Register an observer to be notified when this port is removed.
  // [OUT] kVIA_OK if registration was successful, kVIA_ParameterInvalid in case of a bad in parameter, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL RegisterPortObserver(
    VIAFbPortObserver* inObserver,                              // [IN] Pointer to an object implementing the observer template method
    VIAFbCallbackHandle* outCallbackHandle) = 0;                // [OUT] Handle to the callback, if successful

  // Unregister a previously registered port observer.
  // [OUT] kVIA_OK if deregistration was successful, kVIA_ParameterInvalid in case of a bad handle, kVIA_Failed in case of an error
  // [THREADSAFE] No
  VIASTDDECL UnregisterPortObserver(
    const VIAFbCallbackHandle inHandle) = 0;                    // [IN] Handle from a previously registered observer callback 

  // Provides an iterator over the PDUs of this event group.
  // [OUT] kVIA_OK if PDU iterator was successfully created, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetPDUs(
    VIAFbPDUIterator<VIAFbProvidedServicePDU>** outIterator) = 0;  // [OUT] An iterator over the contained provided PDUs
#endif

#if defined(VIAFunctionBusServiceMinorVersion) && (VIAFunctionBusServiceMinorVersion >= 9)
  // Provides an iterator over the fields of this event group.
  // [OUT] kVIA_OK if field iterator was successfully created, kVIA_ParameterInvalid in case of a bad parameter
  // [THREADSAFE] No
  VIASTDDECL GetFields(
    VIAFbFieldIterator<VIAFbProvidedField>** outIterator) = 0;  // [OUT] An iterator over the contained provided fields
#endif
};

// ============================================================================

#endif // VIA_FUNCTIONBUS_H
