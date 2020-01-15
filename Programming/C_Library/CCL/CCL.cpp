/*------------------------------------------------------------------ 
<FILE> 
  <NAME> CCL.cpp
  <PRJ > CANalyzer/CANoe C Library API (CCL)
  <REV > 1.11.0
  <DATE> 2019-07-03
  <DESC> 
   The module implements the CANalyzer/CANoe C Library API
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


#include "CCL.h"

#include <windows.h>
#define _BUILDNODELAYERDLL
#include "VIA.h"
#include "VIA_CAN.h"
#include "VIA_LIN.h"
#include "VIA_SignalAccess.h"
#include "VIA_FunctionBus.h"

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <string>
#include <limits>
#include <memory.h>
#include <string.h>
#include <malloc.h>


namespace CANalyzerCANoeCLibrary
{
  class VNodeLayerModule;
  class VNodeLayer;

  #pragma region global Variables

  // ============================================================================
  // global Variables
  // ============================================================================

  HINSTANCE           gDLLInstanceHandle;
  VNodeLayerModule*   gModule      = NULL;
  VIAService*         gVIAService  = NULL;
  int                 gVIAMinorVersion = 0;
  bool                gHasBusNames = false;
  VIANamespace*       gSysVarRoot  = NULL;
  VNodeLayer*         gMasterLayer = NULL;
  VIASignalAccessApi* gSignalAccessAPI = NULL;
  VIAFbViaService*    gFunctionBusAPI = NULL;

  const int cMaxChannel = 32;

  struct CanBusContext
  {
    CanBusContext() : mBus(nullptr), mNode(nullptr), mLayer(nullptr), mBusName() {}

    VIACan*       mBus;
    VIANode*      mNode;
    VNodeLayer*   mLayer;
    std::string   mBusName;
  } gCanBusContext[cMaxChannel+1];

  struct LinBusContext
  {
    LinBusContext() : mBus(nullptr), mNode(nullptr), mLayer(nullptr), mBusName() {}

    VIALin*       mBus;
    VIANode*      mNode;
    VNodeLayer*   mLayer;
    std::string   mBusName;
  } gLinBusContext[cMaxChannel+1];

  #pragma endregion

  #pragma region State

  // ============================================================================
  // State 
  // ============================================================================
  
  enum State
  {
    eNotInitialized  =  1,
    eLoaded          =  2,
    eInitMeasurement =  4,
    eRunMeasurement  =  8,
    eStopMeasurement = 16,
    eUnloaded        = 32,
  } gState = eNotInitialized;


  inline bool IsInState(int states)
  {
    return (gState & states) != 0;
  }

  #define CCL_STATECHECK(states)  if (!IsInState(states)) { return CCL_WRONGSTATE; }

  #pragma endregion

  #pragma region FunctionBus API and Utilities

  // ============================================================================
  // Function Bus API
  // ============================================================================

  VIAFbViaService* GetFunctionBusAPI()
  {
    if (gFunctionBusAPI == nullptr)
    {
      gVIAService->GetFunctionBusService(&gFunctionBusAPI, VIAFunctionBusServiceMajorVersion, VIAFunctionBusServiceMinorVersion);
    }
    return gFunctionBusAPI;
  }

  namespace NDetail
  {
    const int cInvalidCCLObjectID = -1;
    const unsigned long long cInvalidPortID = static_cast<unsigned long long>(-1LL);

    bool MapTypeLevel(cclValueRepresentation repr, VIAFbTypeLevel& typeLevel)
    {
      switch (repr)
      {
      case cclValueRepresentation::CCL_IMPL:
        typeLevel = VIAFbTypeLevel::eImpl;
        return true;
      case cclValueRepresentation::CCL_PHYS:
        typeLevel = VIAFbTypeLevel::ePhys;
        return true;
      case cclValueRepresentation::CCL_RAW:
        typeLevel = VIAFbTypeLevel::eRaw;
        return true;
      }
      return false;
    }

    /// Simple guard class for releasing VIA objects from stack, which requires calling method
    /// Release instead of deleting the pointer.
    template <typename T>
    class VIAObjectGuard
    {
    public:
      VIAObjectGuard(T* viaObject = nullptr)
        : mVIAObject(viaObject)
      {}

      ~VIAObjectGuard()
      {
        if (mVIAObject != nullptr) mVIAObject->Release();
      }

      VIAObjectGuard(const VIAObjectGuard&) = delete;
      VIAObjectGuard(VIAObjectGuard&&) = delete;
      VIAObjectGuard& operator=(const VIAObjectGuard&) = delete;
      VIAObjectGuard& operator=(VIAObjectGuard&&) = delete;

      // Support releasing ownership of the VIA object, i.e. not destroying it upon guard destruction
      T* Release()
      {
        T* retval = mVIAObject;
        mVIAObject = nullptr;
        return retval;
      }

      T* mVIAObject;
    };

    /// Helper class for identifying path specifications which start with a reserved keyword.
    /// A simple (ordered) collection of these keywords in turn try to identify the prefix.
    template <typename T>
    class MemberKeyword
    {
    public:
      MemberKeyword(const char* kw, T vc, const char* pf)
        : mKeyword(kw), mValueClass(vc), mPrefix(pf)
      {}

      bool TryReplace(std::string& memberPath, T& valueClass)
      {
        if (memberPath.compare(0, mKeyword.length(), mKeyword) == 0)
        {
          if (mPrefix != nullptr)
            memberPath.replace(0, mKeyword.length(), mPrefix);
          else
            memberPath.erase(0, mKeyword.length());

          valueClass = mValueClass;
          return true;
        }
        return false;
      }

    private:
      std::string mKeyword;          // CCL keyword
      T mValueClass;                 // value class associated with the keyword
      const char* mPrefix;           // hard-coded member in value entity struct
    };

    /// Identifies the use of a specific keyword for C API value objects and returns an enumerated
    /// value for either the keyword or general port value use.
    void ExtractValueClass(std::string& remaining, VIAFbValueClass& valueClass)
    {
      static std::vector<MemberKeyword<VIAFbValueClass>> sMemberKeywords;
      if (sMemberKeywords.empty())
      {
        sMemberKeywords.push_back(MemberKeyword<VIAFbValueClass>(CCL_MEMBER_SERVICE_STATE + 1, VIAFbValueClass::eServiceState, nullptr));
        sMemberKeywords.push_back(MemberKeyword<VIAFbValueClass>(CCL_MEMBER_CONNECTION_STATE + 1, VIAFbValueClass::eConnectionState, nullptr));
        sMemberKeywords.push_back(MemberKeyword<VIAFbValueClass>(CCL_MEMBER_SUBSCRIPTION_STATE + 1, VIAFbValueClass::eSubscriptionState, nullptr));
        sMemberKeywords.push_back(MemberKeyword<VIAFbValueClass>(CCL_MEMBER_LATEST_CALL + 1, VIAFbValueClass::eLatestCall, nullptr));
        sMemberKeywords.push_back(MemberKeyword<VIAFbValueClass>(CCL_MEMBER_LATEST_RETURN CCL_MEMBER_RESULT + 1, VIAFbValueClass::eLatestReturn, "ReturnValue")); // keep before LATEST_RETURN
        sMemberKeywords.push_back(MemberKeyword<VIAFbValueClass>(CCL_MEMBER_LATEST_RETURN + 1, VIAFbValueClass::eLatestReturn, nullptr));
        sMemberKeywords.push_back(MemberKeyword<VIAFbValueClass>(CCL_MEMBER_PARAM_DEFAULTS + 1, VIAFbValueClass::eParamDefaults, nullptr));
        sMemberKeywords.push_back(MemberKeyword<VIAFbValueClass>(CCL_MEMBER_DEFAULT_RESULT + 1, VIAFbValueClass::eParamDefaults, "ReturnValue"));
        sMemberKeywords.push_back(MemberKeyword<VIAFbValueClass>(CCL_MEMBER_AUTO_ANSWER_MODE + 1, VIAFbValueClass::eParamDefaults, "ServerSimulatorMode"));
        sMemberKeywords.push_back(MemberKeyword<VIAFbValueClass>(CCL_MEMBER_AUTO_ANSWER_TIME + 1, VIAFbValueClass::eParamDefaults, "ReturnDelay"));
      }

      for (auto&& entry : sMemberKeywords)
      {
        if (entry.TryReplace(remaining, valueClass))
          return;
      }

      // if no keywords match the member path, the value must refer to the port's value entity directly
      valueClass = VIAFbValueClass::ePortValue;
    }

    enum class CallContextValueClass
    {
      eUnknown,
      eInParam,
      eOutParam,
      eResultValue,
      eCallTime,
      eReturnTime,
      eCallState,
      eRequestID,
      eSide,
      eConsumerPort,
      eProviderPort
    };

    /// Identifies the use of a specific keyword for C API call context objects and returns an
    /// enumerated value (call context value path must always start with a keyword)
    void ExtractValueClass(std::string& remaining, CallContextValueClass& valueClass)
    {
      static std::vector<MemberKeyword<CallContextValueClass>> sMemberKeywords;
      if (sMemberKeywords.empty())
      {
        sMemberKeywords.push_back(MemberKeyword<CallContextValueClass>(CCL_MEMBER_IN_PARAM + 1, CallContextValueClass::eInParam, nullptr));
        sMemberKeywords.push_back(MemberKeyword<CallContextValueClass>(CCL_MEMBER_OUT_PARAM + 1, CallContextValueClass::eOutParam, nullptr));
        sMemberKeywords.push_back(MemberKeyword<CallContextValueClass>(CCL_MEMBER_RESULT + 1, CallContextValueClass::eResultValue, "ReturnValue"));
        sMemberKeywords.push_back(MemberKeyword<CallContextValueClass>(CCL_MEMBER_CALL_TIME + 1, CallContextValueClass::eCallTime, nullptr));
        sMemberKeywords.push_back(MemberKeyword<CallContextValueClass>(CCL_MEMBER_RETURN_TIME + 1, CallContextValueClass::eReturnTime, nullptr));
        sMemberKeywords.push_back(MemberKeyword<CallContextValueClass>(CCL_MEMBER_CALL_STATE + 1, CallContextValueClass::eCallState, nullptr));
        sMemberKeywords.push_back(MemberKeyword<CallContextValueClass>(CCL_MEMBER_REQUEST_ID + 1, CallContextValueClass::eRequestID, nullptr));
        sMemberKeywords.push_back(MemberKeyword<CallContextValueClass>(CCL_MEMBER_SIDE + 1, CallContextValueClass::eSide, nullptr));
        sMemberKeywords.push_back(MemberKeyword<CallContextValueClass>(CCL_MEMBER_CONSUMER_PORT + 1, CallContextValueClass::eConsumerPort, nullptr));
        sMemberKeywords.push_back(MemberKeyword<CallContextValueClass>(CCL_MEMBER_PROVIDER_PORT + 1, CallContextValueClass::eProviderPort, nullptr));
      }

      for (auto&& entry : sMemberKeywords)
      {
        if (entry.TryReplace(remaining, valueClass))
          return;
      }

      // call context member path must always start with a keyword
      valueClass = CallContextValueClass::eUnknown;
    }

    /// Special map implementation for tracking C API wrapper objects and their VIA object delegates.
    /// The map organizes reuse in case of "port" objects, which provide valid port server identifiers.
    /// It further listens for dynamic port destruction and destroys corresponding objects implicitly.
    template <typename ValueType>
    class VIAObjectMap : VIAFbPortObserver
    {
    public:
      VIAObjectMap()
        : mNextId(0)
      {}

      ~VIAObjectMap()
      {
        Clear();
      }

      VIAObjectMap(const VIAObjectMap&) = delete;
      VIAObjectMap(VIAObjectMap&&) = delete;
      VIAObjectMap& operator=(const VIAObjectMap&) = delete;
      VIAObjectMap& operator=(VIAObjectMap&&) = delete;

      // Retrieve a known wrapper object by it's C API identifier
      ValueType* Get(int id)
      {
        auto it = mIdMap.find(id);
        if (it == mIdMap.end())
          return nullptr;

        return it->second.pValue;
      }

      // Add a new wrapper object to be destroyed latest at measurement end (ownership is passed to the map)
      int Add(ValueType* value)
      {
        unsigned long long portID = value->GetPortID();
        auto& range = mPortIDMap.equal_range(portID);
        for (auto& i = range.first; i != range.second; ++i)
        {
          auto it = mIdMap.find(i->second);
          if ((it != mIdMap.end()) && value->IsEqual(it->second.pValue))
          {
            // got the same object stored already, delete new object and return identifier of original wrapper
            delete value;
            it->second.refCount++;
            return i->second;
          }
        }

        // find an unused identifier
        while (mIdMap.find(mNextId) != mIdMap.end()) ++mNextId;

        // initial reference count is set to 1
        mIdMap.insert(std::make_pair(mNextId, ValueReference{ value }));
        value->SetId(mNextId);

        if (portID != cInvalidPortID)
        {
          // it's a port object, register for port removal notifications (will only be effective for dynamic ports)
          mPortIDMap.insert(std::make_pair(portID, mNextId));
          value->RegisterPortObserver(this);
        }

        return mNextId++;
      }

      // Remove a known wrapper object by it's C API identifier
      bool Remove(int id)
      {
        auto it = mIdMap.find(id);
        if (it == mIdMap.end())
          return false;

        if (--it->second.refCount == 0)
        {
          // this was the last reference -> actually remove the map entries
          if (it->second.pValue != nullptr)
          {
            unsigned long long portID = it->second.pValue->GetPortID();
            if (portID != cInvalidPortID)
            {
              // explicit removal of wrapper object -> remove only the exact mapped internal entry
              auto& range = mPortIDMap.equal_range(portID);
              for (auto& i = range.first; i != range.second; ++i)
              {
                if (i->second == id)
                {
                  mPortIDMap.erase(i);
                  break;
                }
              }
            }

            delete it->second.pValue;
          }
          mIdMap.erase(it);

          // reuse the removed identifier
          if (id < mNextId) mNextId = id;
        }

        return true;
      }

      // Notification about a dynamic port being removed
      VIASTDDECL OnPortRemoval(unsigned long long portID)
      {
        auto& range = mPortIDMap.equal_range(portID);

        // remove all mapped wrapper objects (may be multiple in case of SOME/IP event group ports or FEPs)
        for (auto& i = range.first; i != range.second; ++i)
        {
          int id = i->second;

          const auto& it = mIdMap.find(id);
          if (it != mIdMap.end())
          {
            if (it->second.pValue != nullptr) delete it->second.pValue;
            it->second.pValue = nullptr;
          }

          // reuse the removed identifier
          if (id < mNextId) mNextId = id;
        }

        mPortIDMap.erase(portID);

        return kVIA_OK;
      }

      // Remove all objects upon measurement stop
      void Clear()
      {
        for (const auto& entry : mIdMap)
        {
          if (entry.second.pValue != nullptr) delete entry.second.pValue;
        }
        mIdMap.clear();
        mPortIDMap.clear();
        mNextId = 0;
      }

    private:
      int mNextId;

      // holds underlying object pointer and reference count
      struct ValueReference
      {
        ValueReference(ValueType* v) : pValue(v), refCount(1) {}
        ValueType* pValue;
        int refCount;
      };
      std::map<int, ValueReference> mIdMap;

      // Maps port objects from port server identifier to wrapper object identifier.
      // The C API allows clients to create multiple wrapper objects for the same port path,
      // but above Add method checks for identical wrapper objects in case of the same
      // port identifier. Only SOME/IP event groups may still have different wrappers for
      // the same port, if the event group identifier is different.
      std::multimap<unsigned long long, int> mPortIDMap;
    };
  }

  #pragma endregion

  // ============================================================================
  // VTimer
  // ============================================================================

  class VTimer : public VIAOnTimerSink
  {
  public:
    VIASTDDECL OnTimer(VIATime nanoseconds);

    VIATimer*  mViaTimer;
    int        mTimerID;
    void(*mCallbackFunction)(long long time, int ID);
  };

  // ============================================================================
  // VSysVar
  // ============================================================================

  class VSysVar : public VIAOnSysVar, public VIAOnSysVarMember
  {
  public:
    VIASTDDECL OnSysVar(VIATime nanoseconds, VIASystemVariable* ev);
    VIASTDDECL OnSysVarMember(VIATime nanoseconds, VIASystemVariableMember* ev, VIASysVarClientHandle origin);

    static int LookupVariable(int sysVarID, VSysVar*& sysVar);
    static int LookupVariable(int sysVarID, VSysVar*& sysVar, 
                              VIASysVarType t, bool writable);

    int CheckWriteable();

    VIASystemVariable*        mViaSysVar;
    VIASystemVariableMember*  mViaSysVarMember;
    std::string               mVariableName;
    std::string               mMemberName;
    int                       mSysVarID;
    void(*mCallbackFunction)(long long time, int sysVarID);
  };

  // ============================================================================
  // VSignal
  // ============================================================================

  class VSignal : public VIAOnSignal
  {
  public:
    VIASTDDECL OnSignal(VIASignal* signal, void* userData);

    static int LookupSignal(int signalID, VSignal*& signal);

    VIASignal* mViaSignal;
    int        mSignalID;
    void(*mCallbackFunction)(int sysVarID);
  };

  // ============================================================================
  // VCanMessageRequest
  // ============================================================================

  class VCanMessageRequest : public VIAOnCanMessage3
  {
  public:
    VIASTDDECL OnMessage(VIATime      time,
                         VIAChannel   channel,
                         uint8        dir,
                         uint32       id,
                         uint32       flags,
                         uint32       frameLength,
                         VIATime      startOfFrame,
                         uint32       mBtrCfgArb,
                         uint32       mBtrCfgData,
                         uint32       mTimeOffsetBrsNs,
                         uint32       mTimeOffsetCrcDelNs,
                         uint16       mBitCount,
                         uint32       mCRC,
                         uint8        dataLength,
                         const uint8* data);

    VIARequestHandle  mHandle;
    CanBusContext*    mContext;
    void(*mCallbackFunction) (cclCanMessage* message);
  };

  // ============================================================================
  // VLinMessageRequest
  // ============================================================================

  class VLinMessageRequest : public VIAOnLinMessage2
  {
  public:
    VIASTDDECL OnMessage(VIATime  timeFrameEnd,
                         VIATime  timeFrameStart,
                         VIATime     timeHeaderEnd,
                         VIATime     timeSynchBreak,
                         VIATime     timeSynchDel,
                         VIAChannel  channel,
                         uint8       dir,
                         uint32      id,
                         uint8       dlc,
                         uint8       simulated,
                         const uint8 data[8] );

    VIARequestHandle  mHandle;
    LinBusContext*    mContext;
    void(*mCallbackFunction) (cclLinFrame* frame);
  };

  // ============================================================================
  // VPortBase
  // ============================================================================

  template <typename PortType>
  class VPortBase
  {
  public:
    VPortBase(PortType* port, bool release)
      : mPort(port)
      , mRelease(release)
      , mPortID(NDetail::cInvalidPortID)
      , mPortObserverHandle(nullptr)
    {
      if (port != nullptr) port->GetPortID(&mPortID);
    }

    virtual ~VPortBase()
    {
      if (mPort != nullptr)
      {
        if (mPortObserverHandle != nullptr) mPort->UnregisterPortObserver(mPortObserverHandle);
        if (mRelease) mPort->Release();
      }
    }

    virtual unsigned long long GetPortID() const
    {
      return mPortID;
    }

    // override in subclasses if objects are not uniquely identified by their port ID
    virtual bool IsEqual(const VPortBase<PortType>* other) const
    {
      return (mPortID == other->mPortID);
    }

    virtual void RegisterPortObserver(VIAFbPortObserver* observer)
    {
      if (mPort != nullptr) mPort->RegisterPortObserver(observer, &mPortObserverHandle);
    }

    PortType* mPort;

  protected:
    bool mRelease;
    unsigned long long mPortID;
    VIAFbCallbackHandle mPortObserverHandle;
  };

  // ============================================================================
  // IValue
  // ============================================================================

  // Common value interface for storing regular value entities, call context values
  // and PDU signal values in a single VIAObjectMap, which provides a common ID
  // space for these objects.
  struct IValue
  {
    virtual ~IValue() {}

    virtual bool IsEqual(const IValue* other) const = 0;
    virtual void SetId(cclValueID id) = 0;
    virtual unsigned long long GetPortID() const = 0;
    virtual void RegisterPortObserver(VIAFbPortObserver* observer) = 0;

    virtual int GetInteger(long long* x) const = 0;
    virtual int SetInteger(long long x) = 0;
    virtual int GetFloat(double* x) const = 0;
    virtual int SetFloat(double x) = 0;
    virtual int GetString(char* buffer, int bufferSize) const = 0;
    virtual int SetString(const char* str) = 0;
    virtual int GetData(unsigned char* buffer, int* bufferSize) const = 0;
    virtual int SetData(const unsigned char* data, int size) = 0;
    virtual int GetEnum(int* x) const = 0;
    virtual int SetEnum(int x) = 0;
    virtual int GetUnionSelector(int* selector) const = 0;
    virtual int GetArraySize(int* size) const = 0;
    virtual int SetArraySize(int size) = 0;

    virtual int GetValueType(cclValueType* valueType) const = 0;
    virtual int IsValid(bool* isValid) const = 0;
    virtual int ClearValue() = 0;

    virtual int GetUpdateTimeNS(cclTime* time) const = 0;
    virtual int GetChangeTimeNS(cclTime* time) const = 0;
    virtual int GetState(cclValueState* state) const = 0;

    virtual int SetHandler(bool onUpdate, cclValueHandler handler) = 0;

    template <typename T>
    static int LookupValue(cclValueID valueID, T*& value);
  };

  // ============================================================================
  // VValueEntity
  // ============================================================================

  class VValueBase : public VPortBase<VIAFbValuePort>, public IValue
  {
  public:
    virtual ~VValueBase();

    // IValue interface
    virtual int GetInteger(long long* x) const { return CCL_NOTSUPPORTED; }
    virtual int SetInteger(long long x) { return CCL_NOTSUPPORTED; }
    virtual int GetFloat(double* x) const { return CCL_NOTSUPPORTED; }
    virtual int SetFloat(double x) { return CCL_NOTSUPPORTED; }
    virtual int GetString(char* buffer, int bufferSize) const { return CCL_NOTSUPPORTED; }
    virtual int SetString(const char* str) { return CCL_NOTSUPPORTED; }
    virtual int GetData(unsigned char* buffer, int* bufferSize) const { return CCL_NOTSUPPORTED; }
    virtual int SetData(const unsigned char* data, int size) { return CCL_NOTSUPPORTED; }
    virtual int GetEnum(int* x) const { return CCL_NOTSUPPORTED; }
    virtual int SetEnum(int x) { return CCL_NOTSUPPORTED; }
    virtual int GetUnionSelector(int* selector) const { return CCL_NOTSUPPORTED; }
    virtual int GetArraySize(int* size) const { return CCL_NOTSUPPORTED; }
    virtual int SetArraySize(int size) { return CCL_NOTSUPPORTED; }

    virtual int GetValueType(cclValueType* valueType) const { return CCL_NOTSUPPORTED; }
    virtual int IsValid(bool* isValid) const { return CCL_NOTSUPPORTED; }
    virtual int ClearValue() { return CCL_NOTSUPPORTED; }

    virtual int GetUpdateTimeNS(cclTime* time) const { return CCL_NOTSUPPORTED; }
    virtual int GetChangeTimeNS(cclTime* time) const { return CCL_NOTSUPPORTED; }
    virtual int GetState(cclValueState* state) const { return CCL_NOTSUPPORTED; }

    virtual int SetHandler(bool onUpdate, cclValueHandler handler) { return CCL_NOTSUPPORTED; }

    virtual void SetId(cclValueID id) { mValueID = id; }
    virtual bool IsEqual(const IValue* other) const;
    virtual unsigned long long GetPortID() const { return VPortBase<VIAFbValuePort>::GetPortID(); }
    virtual void RegisterPortObserver(VIAFbPortObserver* observer) { VPortBase<VIAFbValuePort>::RegisterPortObserver(observer); }

    cclValueID mValueID;

  protected:
    VValueBase(VIAFbValuePort* port);
    int Init(VIAFbValue* implValue, const std::string& memberPath);

    VIAFbTypeLevel mTypeLevel;
    VIAFbTypeTag mMemberType;
    VIAFbStatus* mMemberStatus;
    VIAFbTypeMemberHandle mMemberHandle;
  };

  class VValueEntity : public VValueBase, public VIAFbValueObserver
  {
  public:
    virtual ~VValueEntity();

    static int CreateValueEntity(const char* portPath, const char* memberPath, cclValueRepresentation repr);
    static int CreateValueEntity(VIAFbValuePort* port, const char* memberPath, cclValueRepresentation repr);

    VIASTDDECL OnValueUpdated(VIATime inTime, VIAFbStatus* inStatus);

    // IValue interface overrides
    virtual int GetInteger(long long* x) const;
    virtual int SetInteger(long long x);
    virtual int GetFloat(double* x) const;
    virtual int SetFloat(double x);
    virtual int GetString(char* buffer, int bufferSize) const;
    virtual int SetString(const char* str);
    virtual int GetData(unsigned char* buffer, int* bufferSize) const;
    virtual int SetData(const unsigned char* data, int size);
    virtual int GetEnum(int* x) const;
    virtual int SetEnum(int x);
    virtual int GetUnionSelector(int* selector) const;
    virtual int GetArraySize(int* size) const;
    virtual int SetArraySize(int size);

    virtual int GetValueType(cclValueType* valueType) const;
    virtual int IsValid(bool* isValid) const;
    virtual int ClearValue();

    virtual int GetUpdateTimeNS(cclTime* time) const;
    virtual int GetChangeTimeNS(cclTime* time) const;
    virtual int GetState(cclValueState* state) const;

    virtual int SetHandler(bool onUpdate, cclValueHandler handler);

    virtual bool IsEqual(const IValue* other) const;

  private:
    VValueEntity(VIAFbValuePort* port);

    int Init(const char* memberPath, cclValueRepresentation repr);
    int InitPduSignal(const char* signalName, const char* memberPath, cclValueRepresentation repr);
    int InitImplementation(const char* signalName, const char* memberPath, cclValueRepresentation repr);

    bool IsMemberValid() const;

    inline VIAResult AccessValue(VIAFbTypeLevel typeLevel, VIAFbTypeMemberHandle memberHandle, VIAFbValue** value) const;

    VIAFbValueClass mValueClass;
    cclValueHandler mCallbackFunction;
    VIAFbCallbackHandle mCallbackHandle;
    VIAFbCallbackHandle mPortObserverHandle;
    std::string mSignalName;
  };

  class VCallContextValue : public VValueBase
  {
  public:
    virtual ~VCallContextValue();

    static int CreateCallContextValue(cclCallContextID ctxtID, const char* memberPath, cclValueRepresentation repr);

    // IValue interface overrides
    virtual int GetInteger(long long* x) const;
    virtual int SetInteger(long long x);
    virtual int GetFloat(double* x) const;
    virtual int SetFloat(double x);
    virtual int GetString(char* buffer, int bufferSize) const;
    virtual int SetString(const char* str);
    virtual int GetData(unsigned char* buffer, int* bufferSize) const;
    virtual int SetData(const unsigned char* data, int size);
    virtual int GetEnum(int* x) const;
    virtual int SetEnum(int x);
    virtual int GetUnionSelector(int* selector) const;
    virtual int GetArraySize(int* size) const;
    virtual int SetArraySize(int size);

    virtual int GetValueType(cclValueType* valueType) const;
    virtual int IsValid(bool* isValid) const;
    virtual int ClearValue();

  private:
    VCallContextValue();
    int Init(cclCallContextID ctxtID, const char* memberPath, cclValueRepresentation repr);

    VIAFbValue* GetParamsValue(VIAFbCallContext* ctxt, bool* isValidMember) const;
    int SetParamsValue(VIAFbCallContext* ctxt, VIAFbValue* value) const;
    VIAFbCallContext* GetCallContext() const;

    cclCallContextID mCallContextID;
    NDetail::CallContextValueClass mValueClass;

    static cclCallState MapCallState(VIAFbFunctionCallState callState);
    static VIAFbFunctionCallState MapCallState(cclCallState callState);
  };

  template <typename PortType>
  class VPDUSignalValue : public VPortBase<PortType>, public IValue, public VIAFbValueObserver
  {
  public:
    virtual ~VPDUSignalValue();

    static int CreateSignalValue(PortType* port, const char* signalName, const char* member, cclValueRepresentation repr);

    VIASTDDECL OnValueUpdated(VIATime inTime, VIAFbStatus* inStatus);

    // IValue interface
    virtual int GetInteger(long long* x) const;
    virtual int SetInteger(long long x);
    virtual int GetFloat(double* x) const;
    virtual int SetFloat(double x);
    virtual int GetString(char* buffer, int bufferSize) const;
    virtual int SetString(const char* str);
    virtual int GetData(unsigned char* buffer, int* bufferSize) const;
    virtual int SetData(const unsigned char* data, int size);
    virtual int GetEnum(int* x) const;
    virtual int SetEnum(int x);
    virtual int GetUnionSelector(int* selector) const;
    virtual int GetArraySize(int* size) const;
    virtual int SetArraySize(int size);

    virtual int GetValueType(cclValueType* valueType) const;
    virtual int IsValid(bool* isValid) const;
    virtual int ClearValue();

    virtual int GetUpdateTimeNS(cclTime* time) const;
    virtual int GetChangeTimeNS(cclTime* time) const;
    virtual int GetState(cclValueState* state) const;

    virtual int SetHandler(bool onUpdate, cclValueHandler handler);

    virtual void SetId(cclValueID id) { mValueID = id; }
    virtual bool IsEqual(const IValue* other) const;
    virtual unsigned long long GetPortID() const { return VPortBase<PortType>::GetPortID(); }
    virtual void RegisterPortObserver(VIAFbPortObserver* observer) { VPortBase<PortType>::RegisterPortObserver(observer); }

    cclValueID mValueID;

  private:
    VPDUSignalValue(PortType* port);
    int Init(const char* signalName, const char* member, cclValueRepresentation repr);

    VIAFbValue* GetMemberValue() const;
    bool IsMemberValid() const;

    std::string mSignalName;
    VIAFbTypeLevel mTypeLevel;
    VIAFbTypeTag mMemberType;
    VIAFbStatus* mMemberStatus;
    VIAFbTypeMemberHandle mMemberHandle;

    cclValueHandler mCallbackFunction;
    VIAFbCallbackHandle mCallbackHandle;
    VIAFbCallbackHandle mPortObserverHandle;
  };

  // ============================================================================
  // V[Client|Server]Function
  // ============================================================================

  // need a common interface for storing any side function ports in NDetail::VIAObjectMap
  struct IFunctionPort
  {
    virtual ~IFunctionPort() {}

    // stores the objects own identifier when added to a VIAObjectMap
    virtual void SetId(cclFunctionID id) = 0;

    // retrieves the native port server's unique identifier
    virtual unsigned long long GetPortID() const = 0;

    // registers with the port server for dynamic port removal
    virtual void RegisterPortObserver(VIAFbPortObserver* observer) = 0;

    // compares this port to given other port for reusing wrapper objects
    virtual bool IsEqual(const IFunctionPort* other) const = 0;

    // register a callback for a specific call state of the function
    virtual int SetHandler(int callbackIndex, VIAFbFunctionCallState state, cclCallHandler handler) = 0;

    // returns whether the function port is consumer side or provider side
    virtual cclSide GetSide() = 0;

    // returns the objects C API identifier
    virtual cclFunctionID GetFunctionID() = 0;

    // returns the function port's path
    virtual std::string GetPath() = 0;
  };

  template <typename T, typename IF>
  class VFunctionBase : public VPortBase<T>, public IFunctionPort, public IF
  {
  protected:
    VFunctionBase(T* port, cclSide side, const char* portPath)
      : VPortBase<T>(port, true)
      , mFunctionID(-1)
      , mSide(side)
      , mPath(portPath)
      , mFinalizingHandle(nullptr)
    {
      if (mPort != nullptr)
        mPort->RegisterObserver(this, VIAFbFunctionCallState::eFinalizing, &mFinalizingHandle);
    }

  public:
    virtual ~VFunctionBase()
    {
      for (int i = 0; i < CALLSTATE_COUNT; ++i)
      {
        if (mCallbacks[i].mCallbackHandle != nullptr)
        {
          if (mPort != nullptr)
            mPort->UnregisterObserver(mCallbacks[i].mCallbackHandle);

          mCallbacks[i].mCallbackHandle = nullptr;
          mCallbacks[i].mCallbackFunction = nullptr;
        }
      }

      if (mFinalizingHandle != nullptr)
      {
        if (mPort != nullptr)
          mPort->UnregisterObserver(mFinalizingHandle);

        mFinalizingHandle = nullptr;
      }
    }

    virtual void SetId(cclFunctionID id) override { mFunctionID = id; }
    virtual unsigned long long GetPortID() const override { return VPortBase::GetPortID(); }
    virtual void RegisterPortObserver(VIAFbPortObserver* observer) override { VPortBase::RegisterPortObserver(observer); }
    virtual bool IsEqual(const IFunctionPort* other) const override { return other->GetPortID() == mPortID; }

    virtual cclSide GetSide() override { return mSide; }
    virtual cclFunctionID GetFunctionID() override { return mFunctionID; }
    virtual std::string GetPath() override { return mPath; }

    cclFunctionID mFunctionID;
    cclSide mSide;
    std::string mPath;

    // handling callbacks
    virtual int SetHandler(int callbackIndex, VIAFbFunctionCallState state, cclCallHandler handler) override
    {
      if (mPort == nullptr)
        return CCL_INTERNALERROR;

      auto& callback = mCallbacks[callbackIndex];
      if (callback.mCallbackHandle != nullptr)
      {
        auto rc = mPort->UnregisterObserver(callback.mCallbackHandle);
        callback.mCallbackHandle = nullptr;
        callback.mCallbackFunction = nullptr;
        if (rc != kVIA_OK)
          return CCL_INTERNALERROR;
      }

      callback.mCallbackFunction = handler;

      if (callback.mCallbackFunction != nullptr)
      {
        if (mPort->RegisterObserver(this, state, &callback.mCallbackHandle) != kVIA_OK)
        {
          callback.mCallbackFunction = nullptr;
          callback.mCallbackHandle = nullptr;
          return CCL_INTERNALERROR;
        }
      }

      return CCL_SUCCESS;
    }

    struct Callback
    {
      Callback() : mCallbackFunction(nullptr), mCallbackHandle(nullptr) {}
      cclCallHandler mCallbackFunction;
      VIAFbCallbackHandle mCallbackHandle;
    };

    static const int CALLSTATE_COUNT = 4;
    Callback mCallbacks[CALLSTATE_COUNT]; // one callback per CallState (excluding Finalizing)
    VIAFbCallbackHandle mFinalizingHandle;
  };

  namespace VFunctionUtils
  {
    static int LookupFunction(cclFunctionID functionID, IFunctionPort*& function);

    static int MapCallState(cclCallState callState, VIAFbFunctionCallState& mapped);
    static int MapCallState(VIAFbFunctionCallState callState, cclCallState& mapped);

    // call context management
    static cclCallContextID CreateCallContext(VIAFbCallContext* ctxt, cclFunctionID funcID, bool release);
    static bool DestroyCallContext(cclCallContextID ctxtID);
  }

  class VClientFunction : public VFunctionBase<VIAFbFunctionClientPort, VIAFbFunctionClientObserver>
  {
  public:
    VClientFunction(VIAFbFunctionClientPort* port, const char* portPath);
    virtual ~VClientFunction();

    static int CreateClientFunction(const char* portPath);

    VIASTDDECL OnCallStateChanged(VIATime inTime, VIAFbCallContext* inCallContext,
      VIAFbFunctionCallState inCallState, const VIAFbFunctionClientPort* inClientPort);

    // tracking created (non-temporary) call contexts for reuse in callbacks
    std::map<long long, cclCallContextID> mRequestIDMap;

    cclCallContextID FindCallContext(VIAFbCallContext* ctxt);
    cclCallContextID CreateCallContext(VIAFbCallContext* ctxt);
    bool DestroyCallContext(VIAFbCallContext* ctxt);
  };

  class VServerFunction : public VFunctionBase<VIAFbFunctionServerPort, VIAFbFunctionServerObserver>
  {
  public:
    VServerFunction(VIAFbFunctionServerPort* port, const char* portPath);
    virtual ~VServerFunction();

    static int CreateServerFunction(const char* portPath);

    VIASTDDECL OnCallStateChanged(VIATime inTime, VIAFbCallContext* inCallContext,
      VIAFbFunctionCallState inCallState, const VIAFbFunctionServerPort* inServerPort);
  };

  class VCallContext : public VIAFbFunctionClientObserver
  {
  public:
    VCallContext(cclFunctionID funcID, VIAFbCallContext* ctxt, bool release);
    virtual ~VCallContext();

    VIASTDDECL OnCallStateChanged(VIATime inTime, VIAFbCallContext* inCallContext,
      VIAFbFunctionCallState inCallState, const VIAFbFunctionClientPort* inClientPort);

    void SetId(cclCallContextID id) { mContextID = id; }

    unsigned long long GetPortID() const { return NDetail::cInvalidPortID; }
    void RegisterPortObserver(VIAFbPortObserver* observer) {}
    bool IsEqual(const VCallContext* other) const { return (mRequestID == other->mRequestID) && (mSide == other->mSide);  }

    cclFunctionID mFunctionID;
    cclCallContextID mContextID;
    VIAFbCallContext* mContext;
    long long mRequestID;
    VIAFbFunctionCallSide mSide;
    bool mRelease;

    std::vector<cclValueID> mContextValues;

    cclCallHandler mResultCallbackFunction;
  };

  // ============================================================================
  // VService
  // ============================================================================

  class VService : public VIAFbServiceDiscoveryObserver
  {
  public:
    virtual ~VService();

    int SetServiceDiscoveryHandler(cclServiceDiscoveryHandler handler);
    int SetConsumerDiscoveredHandler(cclConsumerDiscoveredHandler handler);
    int SetProviderDiscoveredHandler(cclProviderDiscoveredHandler handler);

    static int CreateService(const char* path);
    static int LookupService(cclServiceID serviceID, VService*& service);
    static void Cleanup();

    VIASTDDECL OnServiceDiscovery(VIAFbServiceCO* inService, VIAFbServiceConsumer* inConsumer);
    VIASTDDECL OnConsumerDiscovered(VIAFbServiceCO* inService, VIAFbAddressHandle* inAddress);
    VIASTDDECL OnProviderDiscovered(VIAFbServiceCO* inService, VIAFbAddressHandle* inAddress);

    cclServiceID mServiceID;
    VIAFbServiceCO* mService;

  private:
    VService();
    int UpdateRegistration();

    cclServiceDiscoveryHandler mServiceDiscoveryHandler;
    cclConsumerDiscoveredHandler mConsumerDiscoveredHandler;
    cclProviderDiscoveredHandler mProviderDiscoveredHandler;
    VIAFbCallbackHandle mCallbackHandle;
  };

  // ============================================================================
  // V[Consumer|Provider]
  // ============================================================================

  class VConsumer : public VPortBase<VIAFbServiceConsumer>, public VIAFbServiceConsumerObserver
  {
  public:
    virtual ~VConsumer();

    int SetProviderDiscoveredHandler(cclProviderDiscoveredHandler handler);

    static int CreateConsumer(const char* path);
    static int CreateConsumer(VIAFbServiceConsumer* consumer, bool release);
    static int LookupConsumer(cclConsumerID consumerID, VConsumer*& consumer);
    static int RemoveConsumer(cclConsumerID consumerID);

    VIASTDDECL OnProviderDiscovered(VIAFbServiceConsumer* inConsumer, VIAFbAddressHandle* inAddress);

    void SetId(cclConsumerID consumerID) { mConsumerID = consumerID; }

    cclConsumerID mConsumerID;

  private:
    VConsumer(VIAFbServiceConsumer* consumer, bool release);

    cclProviderDiscoveredHandler mProviderDiscoveredHandler;
    VIAFbCallbackHandle mCallbackHandle;
  };

  class VProvider : public VPortBase<VIAFbServiceProvider>, public VIAFbServiceProviderObserver
  {
  public:
    virtual ~VProvider();

    int SetConnectionRequestedHandler(cclConnectionRequestedHandler handler);
    int SetConsumerDiscoveredHandler(cclConsumerDiscoveredHandler handler);

    static int CreateProvider(const char* path);
    static int CreateProvider(VIAFbServiceProvider* provider, bool release);
    static int LookupProvider(cclProviderID providerID, VProvider*& provider);
    static int RemoveProvider(cclProviderID providerID);

    VIASTDDECL OnConsumerDiscovered(VIAFbServiceProvider* inProvider, VIAFbAddressHandle* inAddress);
    VIASTDDECL OnConnectionRequested(VIAFbServiceProvider* inProvider, VIAFbServiceConsumer* inConsumer);

    void SetId(cclProviderID providerID) { mProviderID = providerID; }

    cclProviderID mProviderID;

  private:
    VProvider(VIAFbServiceProvider* provider, bool release);

    int UpdateRegistration();

    cclConnectionRequestedHandler mConnectionRequestedHandler;
    cclConsumerDiscoveredHandler mConsumerDiscoveredHandler;
    VIAFbCallbackHandle mCallbackHandle;
  };

  // ============================================================================
  // V[Consumed|Provided]Service
  // ============================================================================

  class VConsumedService : public VPortBase<VIAFbConsumedService>
  {
  public:
    virtual ~VConsumedService();

    int ConnectAsync(cclConnectionEstablishedHandler successCallback, cclConnectionFailedHandler failureCallback);

    static int CreateConsumedService(const char* path);
    static int CreateConsumedService(VIAFbConsumedService* consumedService);
    static int LookupConsumedService(cclConsumedServiceID consumedServiceID, VConsumedService*& consumedService);

    void SetId(cclConsumedServiceID consumedServiceID) { mConsumedServiceID = consumedServiceID; }

    cclConsumedServiceID mConsumedServiceID;

  private:
    VConsumedService(VIAFbConsumedService* consumedService);

    class ConnectionHandler : public VIAFbConsumedServiceConnectionHandler
    {
    public:
      VIASTDDECL OnConnectionEstablished(VIAFbConsumedService* inPort);
      VIASTDDECL OnConnectionFailed(VIAFbConsumedService* inPort, const char* inError);

      cclConnectionEstablishedHandler mSuccessCallback;
      cclConnectionFailedHandler mFailureCallback;

      VConsumedService* mParent;
    };

    std::vector<ConnectionHandler*> mConnectionHandlerPool;
    std::set<ConnectionHandler*> mActiveConnectionHandlers;
  };

  class VProvidedService : public VPortBase<VIAFbProvidedService>, public VIAFbFunctionServerObserver
  {
  public:
    virtual ~VProvidedService();

    int SetSimulator(cclCallHandler simulator);
    int ConnectAsync(cclConnectionEstablishedHandler successCallback, cclConnectionFailedHandler failureCallback);

    VIASTDDECL OnCallStateChanged(VIATime inTime, VIAFbCallContext* inCallContext, VIAFbFunctionCallState inCallState, const VIAFbFunctionServerPort* inServerPort);

    static int CreateProvidedService(const char* path);
    static int CreateProvidedService(VIAFbProvidedService* providedService);
    static int LookupProvidedService(cclProvidedServiceID providedServiceID, VProvidedService*& providedService);

    void SetId(cclProvidedServiceID providedServiceID) { mProvidedServiceID = providedServiceID; }

    cclProvidedServiceID mProvidedServiceID;

    cclCallHandler mSimulatorCallback;
    VIAFbCallbackHandle mSimulatorHandle;

    std::map<std::string, cclFunctionID> mFunctionMap;

  private:
    VProvidedService(VIAFbProvidedService* providedService);

    cclFunctionID FindFunction(VIAFbCallContext* callContext);

    class ConnectionHandler : public VIAFbProvidedServiceConnectionHandler
    {
    public:
      VIASTDDECL OnConnectionEstablished(VIAFbProvidedService* inPort);
      VIASTDDECL OnConnectionFailed(VIAFbProvidedService* inPort, const char* inError);

      cclConnectionEstablishedHandler mSuccessCallback;
      cclConnectionFailedHandler mFailureCallback;

      VProvidedService* mParent;
    };

    std::vector<ConnectionHandler*> mConnectionHandlerPool;
    std::set<ConnectionHandler*> mActiveConnectionHandlers;
  };

  // ============================================================================
  // VAddress
  // ============================================================================

  class VAddress
  {
  public:
    ~VAddress();

    static cclAddressID CreateAddress(VIAFbAddressHandle* address, bool release);
    static int LookupAddress(cclAddressID addressID, VAddress*& address);
    static int RemoveAddress(cclAddressID addressID);

    unsigned long long GetPortID() const { return NDetail::cInvalidPortID; }
    void RegisterPortObserver(VIAFbPortObserver* observer) {}
    bool IsEqual(const VAddress* other) const { return mAddress == other->mAddress; }

    void SetId(cclAddressID id) { mAddressID = id; }

    cclAddressID mAddressID;
    VIAFbAddressHandle* mAddress;
    bool mRelease;

  private:
    VAddress(VIAFbAddressHandle* address, bool release);
  };

  // ============================================================================
  // VPDU[Sender|Receiver]
  // ============================================================================

  class VPDUSender : public VPortBase < VIAFbPDUSenderPort >
  {
  public:
    virtual ~VPDUSender();

    static int CreatePDUSender(const char* path);
    static int LookupPDUSender(cclPDUSenderID pduSenderID, VPDUSender*& pduSender);

    void SetId(cclPDUSenderID pduSenderID) { mPDUSenderID = pduSenderID; }

    cclPDUSenderID mPDUSenderID;

  protected:
    VPDUSender(VIAFbPDUSenderPort* pduSender, bool release);
  };

  class VPDUReceiver : public VPortBase < VIAFbPDUReceiverPort >
  {
  public:
    virtual ~VPDUReceiver();

    static int CreatePDUReceiver(const char* path);
    static int LookupPDUReceiver(cclPDUReceiverID pduReceiverID, VPDUReceiver*& pduReceiver);

    void SetId(cclPDUReceiverID pduReceiverID) { mPDUReceiverID = pduReceiverID; }

    cclPDUReceiverID mPDUReceiverID;

  protected:
    VPDUReceiver(VIAFbPDUReceiverPort* pduReceiver);
  };

  // ============================================================================
  // V[Abstract][Consumed|Provided]ServicePDU
  // ============================================================================

  class VConsumedServicePDU : public VPDUReceiver
  {
  public:
    virtual ~VConsumedServicePDU();

    static int CreateConsumedPDU(const char* path);
    static int CreateConsumedPDU(VIAFbConsumedServicePDU* consumedPDU);
    static int LookupConsumedPDU(cclPDUReceiverID consumedPDUID, VConsumedServicePDU*& consumedPDU);

    int SetSubscriptionStateIsolated(bool subscribed);

  protected:
    VConsumedServicePDU(VIAFbConsumedServicePDU* consumedPDU);

    VIAFbConsumedServicePDU* mConsumedPDU;
  };

  class VAbstractConsumedServicePDU : public VConsumedServicePDU
  {
  public:
    virtual ~VAbstractConsumedServicePDU();

    static int CreateConsumedPDU(const char* path);
    static int LookupConsumedPDU(cclPDUReceiverID consumedPDUID, VAbstractConsumedServicePDU*& consumedPDU);

    VIAFbAbstractConsumedServicePDU* mAbstractConsumedPDU;

  private:
    VAbstractConsumedServicePDU(VIAFbAbstractConsumedServicePDU* abstractConsumedPDU);
  };

  class VProvidedServicePDU : public VPDUSender
  {
  public:
    ~VProvidedServicePDU();

    static int CreateProvidedPDU(const char* path);
    static int CreateProvidedPDU(VIAFbProvidedServicePDU* providedPDU, bool release);
    static int LookupProvidedPDU(cclPDUSenderID providedPDUID, VProvidedServicePDU*& providedPDU);
    static int RemoveProvidedPDU(cclPDUSenderID providedPDUID);

    int SetSubscriptionStateIsolated(bool subscribed);

  private:
    VProvidedServicePDU(VIAFbProvidedServicePDU* providedPDU, bool release);

    VIAFbProvidedServicePDU* mProvidedPDU;
  };

  // ============================================================================
  // V[Abstract]ServicePDUProvider
  // ============================================================================

  class VServicePDUProvider : public VPortBase<VIAFbServicePDUProvider>
  {
  public:
    virtual ~VServicePDUProvider();

    static int CreatePDUProvider(const char* path);
    static int CreatePDUProvider(VIAFbServicePDUProvider* pduProvider);
    static int LookupPDUProvider(cclPDUProviderID pduProviderID, VServicePDUProvider*& pduProvider);

    int GetNrOfSubscribedConsumers(int* nrOfConsumers);
    int GetSubscribedConsumers(cclConsumerID* consumerBuffer, int* bufferSize);

    void SetId(cclPDUProviderID pduProviderID) { mPDUProviderID = pduProviderID; }

    cclPDUProviderID mPDUProviderID;

  protected:
    VServicePDUProvider(VIAFbServicePDUProvider* pduProvider);
  };

  class VAbstractServicePDUProvider : public VServicePDUProvider, public VIAFbAbstractPDUSubscriptionObserver
  {
  public:
    virtual ~VAbstractServicePDUProvider();

    static int CreatePDUProvider(const char* path);
    static int LookupPDUProvider(cclPDUProviderID pduProviderID, VAbstractServicePDUProvider*& pduProvider);

    int SetPDUSubscribedHandler(cclAbstractPDUSubscriptionHandler handler);
    int SetPDUUnsubscribedHandler(cclAbstractPDUSubscriptionHandler handler);

    VIASTDDECL OnPDUSubscribed(VIAFbProvidedServicePDU* inPDUPort);
    VIASTDDECL OnPDUUnsubscribed(VIAFbProvidedServicePDU* inPDUPort);

    VIAFbAbstractServicePDUProvider* mAbstractPDUProvider;

  private:
    VAbstractServicePDUProvider(VIAFbAbstractServicePDUProvider* abstractServicePDUProvider);

    int UpdateRegistration();

    cclAbstractPDUSubscriptionHandler mPDUSubscribedHandler;
    cclAbstractPDUSubscriptionHandler mPDUUnsubscribedHandler;
    VIAFbCallbackHandle mCallbackHandle;
  };

  // ============================================================================
  // V[Abstract][Consumed|Provided]Event
  // ============================================================================

  class VConsumedEvent : public VPortBase<VIAFbConsumedEvent>
  {
  public:
    virtual ~VConsumedEvent();

    static int CreateConsumedEvent(const char* path);
    static int CreateConsumedEvent(VIAFbConsumedEvent* consumedEvent);
    static int LookupConsumedEvent(cclConsumedEventID consumedEventID, VConsumedEvent*& consumedEvent);

    int SetSubscriptionStateIsolated(bool subscribed);

    void SetId(cclConsumedEventID consumedEventID) { mConsumedEventID = consumedEventID; }

    cclConsumedEventID mConsumedEventID;

  protected:
    VConsumedEvent(VIAFbConsumedEvent* consumedEvent);
  };

  class VAbstractConsumedEvent : public VConsumedEvent
  {
  public:
    virtual ~VAbstractConsumedEvent();

    static int CreateConsumedEvent(const char* path);
    static int LookupConsumedEvent(cclConsumedEventID consumedEventID, VAbstractConsumedEvent*& consumedEvent);

    VIAFbAbstractConsumedEvent* mAbstractConsumedEvent;

  private:
    VAbstractConsumedEvent(VIAFbAbstractConsumedEvent* abstractConsumedEvent);
  };

  class VProvidedEvent : public VPortBase<VIAFbProvidedEvent>
  {
  public:
    ~VProvidedEvent();

    static int CreateProvidedEvent(const char* path);
    static int CreateProvidedEvent(VIAFbProvidedEvent* providedEvent, bool release);
    static int LookupProvidedEvent(cclProvidedEventID providedEventID, VProvidedEvent*& providedEvent);
    static int RemoveProvidedEvent(cclProvidedEventID providedEventID);

    int SetSubscriptionStateIsolated(bool subscribed);

    void SetId(cclProvidedEventID providedEventID) { mProvidedEventID = providedEventID; }

    cclProvidedEventID mProvidedEventID;

  private:
    VProvidedEvent(VIAFbProvidedEvent* providedEvent, bool release);
  };

  // ============================================================================
  // V[Abstract]EventProvider
  // ============================================================================

  class VEventProvider : public VPortBase<VIAFbEventProvider>
  {
  public:
    virtual ~VEventProvider();

    static int CreateEventProvider(const char* path);
    static int CreateEventProvider(VIAFbEventProvider* eventProvider);
    static int LookupEventProvider(cclEventProviderID eventProviderID, VEventProvider*& eventProvider);

    int GetNrOfSubscribedConsumers(int* nrOfConsumers);
    int GetSubscribedConsumers(cclConsumerID* consumerBuffer, int* bufferSize);

    void SetId(cclEventProviderID eventProviderID) { mEventProviderID = eventProviderID; }

    cclEventProviderID mEventProviderID;

  protected:
    VEventProvider(VIAFbEventProvider* eventProvider);
  };

  class VAbstractEventProvider : public VEventProvider, public VIAFbAbstractEventSubscriptionObserver
  {
  public:
    virtual ~VAbstractEventProvider();

    static int CreateEventProvider(const char* path);
    static int LookupEventProvider(cclEventProviderID eventProviderID, VAbstractEventProvider*& eventProvider);

    int SetEventSubscribedHandler(cclAbstractEventSubscriptionHandler handler);
    int SetEventUnsubscribedHandler(cclAbstractEventSubscriptionHandler handler);

    VIASTDDECL OnEventSubscribed(VIAFbProvidedEvent* inEventPort);
    VIASTDDECL OnEventUnsubscribed(VIAFbProvidedEvent* inEventPort);

    VIAFbAbstractEventProvider* mAbstractEventProvider;

  private:
    VAbstractEventProvider(VIAFbAbstractEventProvider* abstractEventProvider);

    int UpdateRegistration();

    cclAbstractEventSubscriptionHandler mEventSubscribedHandler;
    cclAbstractEventSubscriptionHandler mEventUnsubscribedHandler;
    VIAFbCallbackHandle mCallbackHandle;
  };

  // ============================================================================
  // V[Abstract][Consumed|Provided]Field
  // ============================================================================

  class VConsumedField : public VPortBase < VIAFbConsumedField >
  {
  public:
    virtual ~VConsumedField();

    static int CreateConsumedField(const char* path);
    static int CreateConsumedField(VIAFbConsumedField* consumedField);
    static int LookupConsumedField(cclConsumedFieldID consumedFieldID, VConsumedField*& consumedField);

    int SetSubscriptionStateIsolated(bool subscribed);

    void SetId(cclConsumedFieldID consumedFieldID) { mConsumedFieldID = consumedFieldID; }

    cclConsumedFieldID mConsumedFieldID;

  protected:
    VConsumedField(VIAFbConsumedField* consumedField);
  };

  class VAbstractConsumedField : public VConsumedField
  {
  public:
    virtual ~VAbstractConsumedField();

    static int CreateConsumedField(const char* path);
    static int LookupConsumedField(cclConsumedFieldID consumedFieldID, VAbstractConsumedField*& consumedField);

    VIAFbAbstractConsumedField* mAbstractConsumedField;

  private:
    VAbstractConsumedField(VIAFbAbstractConsumedField* abstractConsumedField);
  };

  class VProvidedField : public VPortBase < VIAFbProvidedField >
  {
  public:
    ~VProvidedField();

    static int CreateProvidedField(const char* path);
    static int CreateProvidedField(VIAFbProvidedField* providedField, bool release);
    static int LookupProvidedField(cclProvidedFieldID providedFieldID, VProvidedField*& providedField);
    static int RemoveProvidedField(cclProvidedFieldID providedFieldID);

    int SetSubscriptionStateIsolated(bool subscribed);

    void SetId(cclProvidedFieldID providedFieldID) { mProvidedFieldID = providedFieldID; }

    cclProvidedFieldID mProvidedFieldID;

  private:
    VProvidedField(VIAFbProvidedField* providedField, bool release);
  };

  // ============================================================================
  // V[Abstract]FieldProvider
  // ============================================================================

  class VFieldProvider : public VPortBase < VIAFbFieldProvider >
  {
  public:
    virtual ~VFieldProvider();

    static int CreateFieldProvider(const char* path);
    static int CreateFieldProvider(VIAFbFieldProvider* fieldProvider);
    static int LookupFieldProvider(cclFieldProviderID fieldProviderID, VFieldProvider*& fieldProvider);

    int GetNrOfSubscribedConsumers(int* nrOfConsumers);
    int GetSubscribedConsumers(cclConsumerID* consumerBuffer, int* bufferSize);

    void SetId(cclFieldProviderID fieldProviderID) { mFieldProviderID = fieldProviderID; }

    cclFieldProviderID mFieldProviderID;

  protected:
    VFieldProvider(VIAFbFieldProvider* fieldProvider);
  };

  class VAbstractFieldProvider : public VFieldProvider, public VIAFbAbstractFieldSubscriptionObserver
  {
  public:
    virtual ~VAbstractFieldProvider();

    static int CreateFieldProvider(const char* path);
    static int LookupFieldProvider(cclFieldProviderID fieldProviderID, VAbstractFieldProvider*& fieldProvider);

    int SetFieldSubscribedHandler(cclAbstractFieldSubscriptionHandler handler);
    int SetFieldUnsubscribedHandler(cclAbstractFieldSubscriptionHandler handler);

    VIASTDDECL OnFieldSubscribed(VIAFbProvidedField* inFieldPort);
    VIASTDDECL OnFieldUnsubscribed(VIAFbProvidedField* inFieldPort);

    VIAFbAbstractFieldProvider* mAbstractFieldProvider;

  private:
    VAbstractFieldProvider(VIAFbAbstractFieldProvider* abstractFieldProvider);

    int UpdateRegistration();

    cclAbstractFieldSubscriptionHandler mFieldSubscribedHandler;
    cclAbstractFieldSubscriptionHandler mFieldUnsubscribedHandler;
    VIAFbCallbackHandle mCallbackHandle;
  };

  // ============================================================================
  // V[Consumed|Provided]EventGroup
  // ============================================================================

  class VConsumedEventGroup : public VPortBase<VIAFbSomeIPConsumedEventGroup>
  {
  public:
    ~VConsumedEventGroup();

    static int CreateConsumedEventGroup(const char* path);
    static int LookupConsumedEventGroup(cclSomeIPConsumedEventGroupID consumedEventGroupID, VConsumedEventGroup*& consumedEventGroup);

    int GetEvents(cclConsumedEventID* eventBuffer, int* bufferSize);
    int GetPDUs(cclPDUReceiverID* pduBuffer, int* bufferSize);
    int GetFields(cclConsumedFieldID* fieldBuffer, int* bufferSize);

    void SetId(cclSomeIPConsumedEventGroupID consumedEventGroupID) { mConsumedEventGroupID = consumedEventGroupID; }

    virtual bool IsEqual(const VPortBase<VIAFbSomeIPConsumedEventGroup>* other) const;

    cclSomeIPConsumedEventGroupID mConsumedEventGroupID;
    unsigned long mEventGroupID;

  private:
    VConsumedEventGroup(VIAFbSomeIPConsumedEventGroup* consumedEventGroup);

    VIAFbCallbackHandle mPortObserverHandle;
  };

  class VProvidedEventGroup : public VPortBase<VIAFbSomeIPProvidedEventGroup>
  {
  public:
    ~VProvidedEventGroup();

    static int CreateProvidedEventGroup(const char* path);
    static int CreateProvidedEventGroup(VIAFbSomeIPProvidedEventGroup* providedEventGroup);
    static int LookupProvidedEventGroup(cclSomeIPProvidedEventGroupID providedEventGroupID, VProvidedEventGroup*& providedEventGroup);
    static int RemoveProvidedEventGroup(cclSomeIPProvidedEventGroupID providedEventGroupID);

    int GetEvents(cclProvidedEventID* eventBuffer, int* bufferSize);
    int GetPDUs(cclPDUSenderID* pduBuffer, int* bufferSize);
    int GetFields(cclProvidedFieldID* fieldBuffer, int* bufferSize);

    void SetId(cclSomeIPProvidedEventGroupID providedEventGroupID) { mProvidedEventGroupID = providedEventGroupID; }

    virtual bool IsEqual(const VPortBase<VIAFbSomeIPProvidedEventGroup>* other) const;

    cclSomeIPProvidedEventGroupID mProvidedEventGroupID;
    unsigned long mEventGroupID;

  private:
    VProvidedEventGroup(VIAFbSomeIPProvidedEventGroup* providedEventGroup, bool release);
  };

  // ============================================================================
  // VEventGroupProvider
  // ============================================================================

  class VEventGroupProvider : public VPortBase<VIAFbSomeIPEventGroupProvider>, public VIAFbSomeIPEventGroupSubscriptionObserver
  {
  public:
    virtual ~VEventGroupProvider();

    static int CreateEventGroupProvider(const char* path);
    static int LookupEventGroupProvider(cclSomeIPEventGroupProviderID eventGroupProviderID, VEventGroupProvider*& eventGroupProvider);

    int GetNrOfSubscribedConsumers(int* nrOfConsumers);
    int GetSubscribedConsumers(cclConsumerID* consumerBuffer, int* bufferSize);

    int SetEventGroupSubscribedHandler(cclSomeIPEventGroupSubscriptionHandler handler);
    int SetEventGroupUnsubscribedHandler(cclSomeIPEventGroupSubscriptionHandler handler);

    int GetEvents(cclEventProviderID* eventBuffer, int* bufferSize);
    int GetPDUs(cclPDUProviderID* pduBuffer, int* bufferSize);
    int GetFields(cclFieldProviderID* fieldBuffer, int* bufferSize);

    VIASTDDECL OnEventGroupSubscribed(VIAFbSomeIPProvidedEventGroup* inEventPort);
    VIASTDDECL OnEventGroupUnsubscribed(VIAFbSomeIPProvidedEventGroup* inEventPort);

    void SetId(cclSomeIPEventGroupProviderID eventGroupProviderID) { mEventGroupProviderID = eventGroupProviderID; }

    virtual bool IsEqual(const VPortBase<VIAFbSomeIPEventGroupProvider>* other) const;

    cclSomeIPEventGroupProviderID mEventGroupProviderID;
    unsigned long mEventGroupID;

  private:
    VEventGroupProvider(VIAFbSomeIPEventGroupProvider* eventGroupProvider);

    int UpdateRegistration();

    cclSomeIPEventGroupSubscriptionHandler mEventGroupSubscribedHandler;
    cclSomeIPEventGroupSubscriptionHandler mEventGroupUnsubscribedHandler;
    VIAFbCallbackHandle mCallbackHandle;
  };

  // ============================================================================
  // VNodeLayerModule
  // ============================================================================

  class VNodeLayerModule
    : public VIAModuleApi
  {
  public:
    VNodeLayerModule();

    VIASTDDECL Init();
    VIASTDDECL GetVersion (char* buffer, int32 bufferLength );
    VIASTDDECL GetModuleParameters (char* pathBuff, int32 pathBuffLength,
      char* versionBuff, int32 versionBuffLength); 
    VIASTDDECL CreateObject  (VIANodeLayerApi** object,
      VIANode* node,
      int32 argc, char* argv[]);
    VIASTDDECL ReleaseObject (VIANodeLayerApi*  object);
    VIASTDDECL GetNodeInfo   (const char*  nodename,
      char* shortNameBuf, int32 shortBufLength,
      char* longNameBuf,  int32 longBufLength );
    VIASTDDECL InitMeasurement ();
    VIASTDDECL EndMeasurement  ();
    VIASTDDECL GetNodeInfoEx (VIDBNodeDefinition* nodeDefinition,
      char* shortNameBuf, int32 shortBufLength,
      char* longNameBuf, int32 longBufLength );
    VIASTDDECL DoInformOfChange(VIDBNodeDefinition* nodeDefinition,
      const uint32      changeFlags,
      const char*       simBusName,
      const VIABusInterfaceType busType,
      const VIAChannel  channel,
      const char*       oldName,
      const char*       newName,
      const int32       bValue);

    void (*mMeasurementPreStartHandler)();
    void (*mMeasurementStartHandler)();
    void (*mMeasurementStopHandler)();
    void (*mDllUnloadHandler)();

    // Objects which cannot be deleted during measurement are stored in vector: their identifier
    // is identical to the index in the vector. Dynamic objects are stored in a VIAObjectMap helper
    // class, which assigns dynamic IDs and manages object removal.
    std::vector<VTimer*>                       mTimer;
    std::vector<VSysVar*>                      mSysVar;
    std::vector<VSignal*>                      mSignal;
    std::vector<VCanMessageRequest*>           mCanMessageRequests;
    std::vector<VLinMessageRequest*>           mLinMessageRequests;
    std::vector<VService*>                     mServices;
    NDetail::VIAObjectMap<IValue>              mValues;
    NDetail::VIAObjectMap<IFunctionPort>       mFunctions;
    NDetail::VIAObjectMap<VCallContext>        mCallContexts;
    NDetail::VIAObjectMap<VConsumer>           mConsumers;
    NDetail::VIAObjectMap<VProvider>           mProviders;
    NDetail::VIAObjectMap<VConsumedService>    mConsumedServices;
    NDetail::VIAObjectMap<VProvidedService>    mProvidedServices;
    NDetail::VIAObjectMap<VAddress>            mAddresses;
    NDetail::VIAObjectMap<VPDUSender>          mPDUSender;
    NDetail::VIAObjectMap<VPDUReceiver>        mPDUReceiver;
    NDetail::VIAObjectMap<VServicePDUProvider> mPDUProviders;
    NDetail::VIAObjectMap<VConsumedEvent>      mConsumedEvents;
    NDetail::VIAObjectMap<VProvidedEvent>      mProvidedEvents;
    NDetail::VIAObjectMap<VEventProvider>      mEventProviders;
    NDetail::VIAObjectMap<VConsumedField>      mConsumedFields;
    NDetail::VIAObjectMap<VProvidedField>      mProvidedFields;
    NDetail::VIAObjectMap<VFieldProvider>      mFieldProviders;
    NDetail::VIAObjectMap<VConsumedEventGroup> mConsumedEventGroups;
    NDetail::VIAObjectMap<VProvidedEventGroup> mProvidedEventGroups;
    NDetail::VIAObjectMap<VEventGroupProvider> mEventGroupProviders;
  };

  // ============================================================================
  // VNodeLayer
  // ============================================================================

  class VNodeLayer
    : public VIANodeLayerApi
  {
  public:
    VNodeLayer(VIANode* node, uint32 busType, VIAChannel channel, VIABus* bus);
    virtual ~VNodeLayer();
    VIASTDDECL GetNode(VIANode **node);
    VIASTDDECL InitMeasurement();
    VIASTDDECL StartMeasurement();
    VIASTDDECL StopMeasurement();
    VIASTDDECL EndMeasurement();
    VIASTDDECL InitMeasurementComplete();
    VIASTDDECL PreInitMeasurement();
  public:

    VIANode*      mNode;
    uint32        mBusType;
    VIAChannel    mChannel;
    VIABus*       mBus;
  };

  #pragma region VTimer Implementation

  // ============================================================================
  // Implementation of class VTimer
  // ============================================================================

  VIASTDDEF VTimer::OnTimer(VIATime nanoseconds)
  {
    if (mCallbackFunction != NULL)
    {
      mCallbackFunction(nanoseconds, mTimerID);
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VSysVar Implementation

  // ============================================================================
  // Implementation of class VSysVar
  // ============================================================================

  VIASTDDEF VSysVar::OnSysVar(VIATime nanoseconds, VIASystemVariable* ev)
  {
    if (mCallbackFunction != NULL)
    {
      mCallbackFunction(nanoseconds, mSysVarID);
    }
    return kVIA_OK;
  }


  VIASTDDEF VSysVar::OnSysVarMember(VIATime nanoseconds, VIASystemVariableMember* ev, VIASysVarClientHandle origin)
  {
    if (mCallbackFunction != NULL)
    {
      mCallbackFunction(nanoseconds, mSysVarID);
    }
    return kVIA_OK;
  }


  int VSysVar::LookupVariable(int sysVarID, VSysVar*& sysVar)
  {
    if (gModule == NULL)
    {
      return CCL_INTERNALERROR;
    }

    if (sysVarID < 0 || sysVarID >= static_cast<int>(gModule->mSysVar.size()))
    {
      return CCL_INVALIDSYSVARID;
    }

    sysVar = gModule->mSysVar[sysVarID];
    if (sysVar == NULL || sysVar->mSysVarID != sysVarID)
    {
      return CCL_INVALIDSYSVARID;
    }

    return CCL_SUCCESS;
  }


  int VSysVar::LookupVariable(int sysVarID, VSysVar*& sysVar, VIASysVarType t, bool writable)
  {
    int result = LookupVariable(sysVarID, sysVar);
    if (result != CCL_SUCCESS)
    {
      return result;
    }

    // check type of system variable
    VIASysVarType varType;
    VIAResult rc;
    if (sysVar->mViaSysVarMember != NULL)
    {
      rc = sysVar->mViaSysVarMember->GetType(&varType);
    }
    else
    {
      rc = sysVar->mViaSysVar->GetType(&varType);
    }
    if (rc != kVIA_OK)
    {
      if (rc == kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
    if (!((t == varType) ||
          (t == kVIA_SVData && sysVar->mViaSysVarMember != NULL && varType == kVIA_SVGenericArray) ||
          (t == kVIA_SVData && varType == kVIA_SVStruct) ||
          (t == kVIA_SVIntegerArray && sysVar->mViaSysVarMember != NULL && varType == kVIA_SVGenericArray)))
    {
      return CCL_WRONGTYPE;
    }

    // check, that variable is not readonly 
    if (writable)
    {
      int result = sysVar->CheckWriteable();
      if (result != CCL_SUCCESS)
      {
        return result;
      }
    }

    return CCL_SUCCESS;
  }


  int VSysVar::CheckWriteable()
  {
    // check, that variable is not readonly 
    VIAResult rc;
    int32 readOnly;
    rc = mViaSysVar->IsReadOnly(&readOnly);
    if (rc != kVIA_OK)
    {
      if (rc == kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
    if (readOnly)
    {
      return CCL_SYSVARISREADONLY;
    }

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VSignal Implementation

  // ============================================================================
  // Implementation of class VSignal
  // ============================================================================

  VIASTDDEF VSignal::OnSignal(VIASignal* signal, void* userData)
  {
    if (mCallbackFunction != NULL)
    {
      mCallbackFunction(mSignalID);
    }
    return kVIA_OK;
  }

  int VSignal::LookupSignal(int signalID, VSignal*& signal)
  {
    if (gModule == NULL)
    {
      return CCL_INTERNALERROR;
    }

    if (signalID < 0 || signalID >= static_cast<int>(gModule->mSignal.size()))
    {
      return CCL_INVALIDSIGNALID;
    }

    signal = gModule->mSignal[signalID];
    if (signal == NULL || signal->mSignalID != signalID)
    {
      return CCL_INVALIDSIGNALID;
    }

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VCanMessageRequest Implementation

  // ============================================================================
  // Implementation of class VCanMessageRequest
  // ============================================================================

  VIASTDDEF VCanMessageRequest::OnMessage(VIATime      time,
                                          VIAChannel   channel,
                                          uint8        dir,
                                          uint32       id,
                                          uint32       flags,
                                          uint32       frameLength,
                                          VIATime      startOfFrame,
                                          uint32       mBtrCfgArb,
                                          uint32       mBtrCfgData,
                                          uint32       mTimeOffsetBrsNs,
                                          uint32       mTimeOffsetCrcDelNs,
                                          uint16       mBitCount,
                                          uint32       mCRC,
                                          uint8        dataLength,
                                          const uint8* data)
  {
    cclCanMessage message;
    message.time = time;
    message.channel = channel;
    message.id = id;
    message.flags =
      ((flags & kVIA_CAN_RemoteFrame) ? CCL_CANFLAGS_RTR : 0) |
      ((flags & kVIA_CAN_Wakeup) ? CCL_CANFLAGS_WAKEUP : 0) |
      ((flags & kVIA_CAN_NERR) ? CCL_CANFLAGS_TE : 0) |
      ((flags & kVIA_CAN_EDL) ? CCL_CANFLAGS_FDF : 0) |
      ((flags & kVIA_CAN_BRS) ? CCL_CANFLAGS_BRS : 0) |
      ((flags & kVIA_CAN_ESI) ? CCL_CANFLAGS_ESI : 0);
    message.dir = dir;
    message.dataLength = dataLength;
    memset(message.data, 0, 64);
    int size = (flags&CCL_CANFLAGS_RTR) ? 0 : ((dataLength <= 64) ? dataLength : 64);
    memcpy_s(message.data, 64, data, size);

    if (mCallbackFunction != NULL)
    {
      mCallbackFunction(&message);
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VLinMessageRequest Implementation

  // ============================================================================
  // Implementation of class VLinMessageRequest
  // ============================================================================

  VIASTDDEF VLinMessageRequest::OnMessage(VIATime     timeFrameEnd,
                                          VIATime     timeFrameStart,
                                          VIATime     timeHeaderEnd,
                                          VIATime     timeSynchBreak,
                                          VIATime     timeSynchDel,
                                          VIAChannel  channel,
                                          uint8       dir,
                                          uint32      id,
                                          uint8       dlc,
                                          uint8       simulated,
                                          const uint8 data[8])
  {
    cclLinFrame frame;
    frame.timestampEOF = timeFrameEnd;
    frame.timestampSOF = timeFrameStart;
    frame.timestampEOH = timeHeaderEnd;
    frame.timeSyncBreak = timeSynchBreak;
    frame.timeSyncDel = timeSynchDel;
    frame.channel = channel;
    frame.id = id;
    frame.dir = dir;
    frame.dlc = dlc;
    memset(frame.data, 0, 8);
    int size = (dlc <= 8) ? dlc : 8;
    memcpy_s(frame.data, 8, data, size);

    if (mCallbackFunction != NULL)
    {
      mCallbackFunction(&frame);
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VValueBase Implementation

  // generic lookup of any type of value by ID
  template<typename T>
  int IValue::LookupValue(cclValueID valueID, T*& value)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    value = dynamic_cast<T*>(gModule->mValues.Get(valueID));
    if (value == nullptr)
      return CCL_INVALIDVALUEID;

    return CCL_SUCCESS;
  }

  // base class for Value Entities and Call Context Values
  VValueBase::VValueBase(VIAFbValuePort* port)
    : VPortBase(port, true)
    , mValueID(NDetail::cInvalidCCLObjectID)
    , mMemberStatus(nullptr)
    , mMemberHandle(kVIAFbTypeMemberHandleWhole)
  {}

  VValueBase::~VValueBase()
  {
    if (mMemberStatus != nullptr) mMemberStatus->Release();
  }

  int VValueBase::Init(VIAFbValue* implValue, const std::string& memberPath)
  {
    if (implValue != nullptr)
    {
      if (!memberPath.empty())
      {
        NDetail::VIAObjectGuard<VIAFbType> implType;
        if (implValue->GetType(&(implType.mVIAObject)) != kVIA_OK)
          return CCL_INTERNALERROR;

        if (implType.mVIAObject->GetMember(memberPath.c_str(), &mMemberHandle) != kVIA_OK)
          return CCL_INVALIDMEMBERPATH;
      }

      NDetail::VIAObjectGuard<VIAFbType> memberType;
      if (implValue->GetMemberType(mMemberHandle, mTypeLevel, &(memberType.mVIAObject)) != kVIA_OK)
        return CCL_INTERNALERROR;

      if (memberType.mVIAObject->GetTypeTag(&mMemberType) != kVIA_OK)
        return CCL_INTERNALERROR;

      if (implValue->GetMemberStatus(mMemberHandle, &mMemberStatus) != kVIA_OK)
        return CCL_INTERNALERROR;
    }

    return CCL_SUCCESS;
  }

  bool VValueBase::IsEqual(const IValue* other) const
  {
    if (other == this) return true;
    const VValueBase* value = dynamic_cast<const VValueBase*>(other);
    return (value != nullptr)
      && VPortBase<VIAFbValuePort>::IsEqual(value)
      && (mMemberHandle == value->mMemberHandle)
      && (mTypeLevel == value->mTypeLevel);
  }

  #pragma endregion

  #pragma region VValueEntity Implementation

  // Value Entity class
  VValueEntity::VValueEntity(VIAFbValuePort* port)
    : VValueBase(port)
    , mCallbackFunction(nullptr)
    , mCallbackHandle(nullptr)
  {}

  VValueEntity::~VValueEntity()
  {
    if (mCallbackHandle != nullptr)
    {
      if (mMemberStatus != nullptr) mMemberStatus->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
      mCallbackFunction = nullptr;
    }
  }

  int VValueEntity::Init(const char* memberPath, cclValueRepresentation repr)
  {
    return InitImplementation(nullptr, memberPath, repr);
  }

  int VValueEntity::InitPduSignal(const char* signalName, const char* memberPath, cclValueRepresentation repr)
  {
    return InitImplementation(signalName, memberPath, repr);
  }

  int VValueEntity::InitImplementation(const char* signalName, const char* memberPath, cclValueRepresentation repr)
  {
    mCallbackHandle = nullptr;

    mValueClass = VIAFbValueClass::ePortValue;
    mMemberHandle = kVIAFbTypeMemberHandleWhole;
    mSignalName = std::string{};

    std::string remaining{ (memberPath != nullptr) ? memberPath : "" };
    if (!remaining.empty())
    {
      // leading . is optional
      if (remaining.front() == '.')
        remaining.erase(0, 1);

      if (!remaining.empty())
      {
        // customer may escape keywords
        if (remaining.front() == '#')
          remaining.erase(0, 1);
        else
          NDetail::ExtractValueClass(remaining, mValueClass);
      }
    }

    if (!NDetail::MapTypeLevel(repr, mTypeLevel))
      return CCL_PARAMETERINVALID;

    NDetail::VIAObjectGuard<VIAFbValue> implValue;
    if (signalName == nullptr)
    {
      if (mPort->GetValue(VIAFbTypeLevel::eImpl, kVIAFbTypeMemberHandleWhole, mValueClass, &(implValue.mVIAObject)) != kVIA_OK)
        return CCL_INVALIDMEMBERPATH;
    }
    else if (mValueClass != VIAFbValueClass::ePortValue)
    {
      return CCL_PARAMETERINVALID;
    }
    else
    {
      if (mPort->GetPDUSignalValue(VIAFbTypeLevel::eImpl, signalName, kVIAFbTypeMemberHandleWhole, &(implValue.mVIAObject)) != kVIA_OK)
        return CCL_INVALIDMEMBERPATH;
      mSignalName = signalName;
    }

    return VValueBase::Init(implValue.mVIAObject, remaining);
  }

  int VValueEntity::CreateValueEntity(const char* portPath, const char* memberPath, cclValueRepresentation repr)
  {
    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbValuePort* port;
    if (fbAPI->GetValuePort(portPath, &port) != kVIA_OK)
    {
      // PDU signal support
      const char* lastPoint = strrchr(portPath, '.');
      if (lastPoint != nullptr)
      {
        std::string possiblePduPath{ portPath, lastPoint };
        if (fbAPI->GetValuePort(possiblePduPath.c_str(), &port) == kVIA_OK)
        {
          std::unique_ptr<VValueEntity> valueEntity{ new VValueEntity(port) };
          int result = valueEntity->InitPduSignal(lastPoint + 1, memberPath, repr);
          if (result != CCL_SUCCESS)
            return result;

          return gModule->mValues.Add(valueEntity.release());
        }
      }
      return CCL_INVALIDPORTPATH;
    }

    std::unique_ptr<VValueEntity> valueEntity{ new VValueEntity(port) };
    int result = valueEntity->Init(memberPath, repr);
    if (result != CCL_SUCCESS)
      return result;

    return gModule->mValues.Add(valueEntity.release());
  }

  int VValueEntity::CreateValueEntity(VIAFbValuePort* port, const char* memberPath, cclValueRepresentation repr)
  {
    std::unique_ptr<VValueEntity> valueEntity{ new VValueEntity(port) };
    int result = valueEntity->Init(memberPath, repr);
    if (result != CCL_SUCCESS)
      return result;

    return gModule->mValues.Add(valueEntity.release());
  }

  inline VIAResult VValueEntity::AccessValue(VIAFbTypeLevel typeLevel, VIAFbTypeMemberHandle memberHandle, VIAFbValue** value) const
  {
    if (mSignalName.empty())
    {
      return mPort->GetValue(typeLevel, memberHandle, mValueClass, value);
    }
    else
    {
      return mPort->GetPDUSignalValue(typeLevel, mSignalName.c_str(), memberHandle, value);
    }
  }

  bool VValueEntity::IsMemberValid() const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberHandle == kVIAFbTypeMemberHandleWhole)
      return true;

    NDetail::VIAObjectGuard<VIAFbValue> implValue;
    if (AccessValue(VIAFbTypeLevel::eImpl, kVIAFbTypeMemberHandleWhole, &(implValue.mVIAObject)) != kVIA_OK)
      return false;

    bool isValid;
    if (implValue.mVIAObject->IsValid(mMemberHandle, &isValid) != kVIA_OK)
      return false;

    return isValid;
  }

  VIASTDDEF VValueEntity::OnValueUpdated(VIATime inTime, VIAFbStatus* inStatus)
  {
    if (mCallbackFunction != nullptr)
    {
      // VValueEntity objects are never removed during measurement, therefore their ID stays valid
      mCallbackFunction(static_cast<cclTime>(inTime), mValueID);
    }
    return kVIA_OK;
  }

  int VValueEntity::GetInteger(long long* x) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    if (mMemberType == VIAFbTypeTag::eSignedInteger)
    {
      int64 value;
      if (fbValue.mVIAObject->GetSignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = value;
      return CCL_SUCCESS;
    }
    else if (mMemberType == VIAFbTypeTag::eUnsignedInteger)
    {
      uint64 value;
      if (fbValue.mVIAObject->GetUnsignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = static_cast<long long>(value);
      return CCL_SUCCESS;
    }

    return CCL_WRONGTYPE;
  }

  int VValueEntity::SetInteger(long long x)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    if (mMemberType == VIAFbTypeTag::eSignedInteger)
    {
      int64 value{ x };
      if (fbValue.mVIAObject->SetSignedInteger(kVIAFbTypeMemberHandleWhole, value) != kVIA_OK)
        return CCL_INTERNALERROR;
    }
    else if (mMemberType == VIAFbTypeTag::eUnsignedInteger)
    {
      uint64 value{ static_cast<uint64>(x) };
      if (fbValue.mVIAObject->SetUnsignedInteger(kVIAFbTypeMemberHandleWhole, value) != kVIA_OK)
        return CCL_INTERNALERROR;
    }
    else
    {
      return CCL_WRONGTYPE;
    }

    if (mPort->SetValue(fbValue.mVIAObject) != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  int VValueEntity::GetFloat(double* x) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberType != VIAFbTypeTag::eFloat)
      return CCL_WRONGTYPE;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    if (fbValue.mVIAObject->GetFloat(kVIAFbTypeMemberHandleWhole, x) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  int VValueEntity::SetFloat(double x)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberType != VIAFbTypeTag::eFloat)
      return CCL_WRONGTYPE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    if (fbValue.mVIAObject->SetFloat(kVIAFbTypeMemberHandleWhole, x) != kVIA_OK)
      return CCL_INTERNALERROR;

    if (mPort->SetValue(fbValue.mVIAObject) != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  int VValueEntity::GetString(char* buffer, int bufferSize) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberType != VIAFbTypeTag::eString)
      return CCL_WRONGTYPE;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    switch (fbValue.mVIAObject->GetString(kVIAFbTypeMemberHandleWhole, buffer, bufferSize))
    {
    case kVIA_OK:
      return CCL_SUCCESS;
    case kVIA_BufferToSmall:
      return CCL_BUFFERTOSMALL;
    default:
      break;
    }

    return CCL_INTERNALERROR;
  }

  int VValueEntity::SetString(const char* str)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberType != VIAFbTypeTag::eString)
      return CCL_WRONGTYPE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    if (fbValue.mVIAObject->SetString(kVIAFbTypeMemberHandleWhole, str) != kVIA_OK)
      return CCL_INTERNALERROR;

    if (mPort->SetValue(fbValue.mVIAObject) != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  int VValueEntity::GetData(unsigned char* buffer, int* bufferSize) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    if (fbValue.mVIAObject->GetData(kVIAFbTypeMemberHandleWhole, buffer, *bufferSize) != kVIA_OK)
      return CCL_BUFFERTOSMALL;

    size_t size;
    if (fbValue.mVIAObject->GetSize(kVIAFbTypeMemberHandleWhole, &size) != kVIA_OK)
      return CCL_INTERNALERROR;
    *bufferSize = size;

    return CCL_SUCCESS;
  }

  int VValueEntity::SetData(const unsigned char* data, int size)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    if (fbValue.mVIAObject->SetData(kVIAFbTypeMemberHandleWhole, data, size) != kVIA_OK)
      return CCL_INVALIDDATA;

    if (mPort->SetValue(fbValue.mVIAObject) != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  int VValueEntity::GetEnum(int* x) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    switch (mMemberType)
    {
    case VIAFbTypeTag::eSignedInteger: {
      int64 value;
      if (fbValue.mVIAObject->GetSignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = static_cast<int>(value);
      return CCL_SUCCESS;
    }
    case VIAFbTypeTag::eUnsignedInteger: {
      uint64 value;
      if (fbValue.mVIAObject->GetUnsignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = static_cast<int>(value);
      return CCL_SUCCESS;
    }
    default:
      break;
    }

    return CCL_WRONGTYPE;
  }

  int VValueEntity::SetEnum(int x)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    switch (mMemberType)
    {
    case VIAFbTypeTag::eSignedInteger:
      if (fbValue.mVIAObject->SetSignedInteger(kVIAFbTypeMemberHandleWhole, static_cast<int64>(x)) != kVIA_OK)
        return CCL_INTERNALERROR;
      break;
    case VIAFbTypeTag::eUnsignedInteger:
      if (fbValue.mVIAObject->SetUnsignedInteger(kVIAFbTypeMemberHandleWhole, static_cast<uint64>(x)) != kVIA_OK)
        return CCL_INTERNALERROR;
      break;
    default:
      return CCL_WRONGTYPE;
    }

    if (mPort->SetValue(fbValue.mVIAObject) != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  int VValueEntity::GetUnionSelector(int* selector) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberType != VIAFbTypeTag::eUnion)
      return CCL_WRONGTYPE;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    size_t index;
    if (fbValue.mVIAObject->GetValidMemberIndex(kVIAFbTypeMemberHandleWhole, &index) != kVIA_OK)
      return CCL_INTERNALERROR;

    *selector = static_cast<int>(index);
    return CCL_SUCCESS;
  }

  int VValueEntity::GetArraySize(int* size) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberType != VIAFbTypeTag::eArray)
      return CCL_WRONGTYPE;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    size_t length;
    if (fbValue.mVIAObject->GetArrayLength(kVIAFbTypeMemberHandleWhole, &length) != kVIA_OK)
      return CCL_INTERNALERROR;

    *size = static_cast<int>(length);
    return CCL_SUCCESS;
  }

  int VValueEntity::SetArraySize(int size)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberType != VIAFbTypeTag::eArray)
      return CCL_WRONGTYPE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue;
    if (AccessValue(mTypeLevel, mMemberHandle, &(fbValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbType> fbType;
    if (fbValue.mVIAObject->GetType(&(fbType.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbTypeTag typeTag;
    if (fbType.mVIAObject->GetTypeTag(&typeTag) != kVIA_OK)
      return CCL_INTERNALERROR;

    if (typeTag != VIAFbTypeTag::eArray)
      return CCL_WRONGTYPE;

    size_t length{ static_cast<size_t>(size) };
    if (fbValue.mVIAObject->SetArrayLength(kVIAFbTypeMemberHandleWhole, length) != kVIA_OK)
      return CCL_ARRAYNOTRESIZABLE;

    if (mPort->SetValue(fbValue.mVIAObject) != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  int VValueEntity::GetValueType(cclValueType* valueType) const
  {
    switch (mMemberType)
    {
    case VIAFbTypeTag::eSignedInteger:
    case VIAFbTypeTag::eUnsignedInteger:
      *valueType = cclValueType::CCL_VALUE_TYPE_INTEGER;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eFloat:
      *valueType = cclValueType::CCL_VALUE_TYPE_FLOAT;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eString:
      *valueType = cclValueType::CCL_VALUE_TYPE_STRING;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eArray:
      *valueType = cclValueType::CCL_VALUE_TYPE_ARRAY;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eStruct:
      *valueType = cclValueType::CCL_VALUE_TYPE_STRUCT;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eUnion:
      *valueType = cclValueType::CCL_VALUE_TYPE_UNION;
      return CCL_SUCCESS;
    default:
      break;
    }
    return CCL_UNSUPPORTEDTYPE;
  }

  int VValueEntity::IsValid(bool* isValid) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (isValid == nullptr)
      return CCL_PARAMETERINVALID;

    NDetail::VIAObjectGuard<VIAFbValue> fullValue;
    if (AccessValue(VIAFbTypeLevel::eImpl, kVIAFbTypeMemberHandleWhole, &(fullValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    if (fullValue.mVIAObject->IsValid(mMemberHandle, isValid) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  int VValueEntity::ClearValue()
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberHandle == kVIAFbTypeMemberHandleWhole)
      return CCL_VALUENOTOPTIONAL;

    NDetail::VIAObjectGuard<VIAFbValue> fullValue;
    if (AccessValue(VIAFbTypeLevel::eImpl, kVIAFbTypeMemberHandleWhole, &(fullValue.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    if (fullValue.mVIAObject->SetIsValid(mMemberHandle, false) != kVIA_OK)
      return CCL_VALUENOTOPTIONAL;

    if (mPort->SetValue(fullValue.mVIAObject) != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  int VValueEntity::GetUpdateTimeNS(cclTime* time) const
  {
    if (mMemberStatus == nullptr)
      return CCL_INTERNALERROR;

    VIATime value;
    if (mMemberStatus->GetLastUpdateTimestamp(&value) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    *time = value;
    return CCL_SUCCESS;
  }

  int VValueEntity::GetChangeTimeNS(cclTime* time) const
  {
    if (mMemberStatus == nullptr)
      return CCL_INTERNALERROR;

    VIATime value;
    if (mMemberStatus->GetLastChangeTimestamp(&value) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    *time = value;
    return CCL_SUCCESS;
  }

  int VValueEntity::GetState(cclValueState* state) const
  {
    if (mMemberStatus == nullptr)
      return CCL_INTERNALERROR;

    VIAFbValueState valueState;
    if (mMemberStatus->GetValueState(&valueState) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    switch (valueState)
    {
    case VIAFbValueState::eOffline:
      *state = cclValueState::CCL_VALUE_STATE_OFFLINE;
      return CCL_SUCCESS;
    case VIAFbValueState::eMeasured:
      *state = cclValueState::CCL_VALUE_STATE_MEASURED;
      return CCL_SUCCESS;
    default:
      break;
    }

    return CCL_INTERNALERROR;
  }

  int VValueEntity::SetHandler(bool onUpdate, cclValueHandler handler)
  {
    if (mMemberStatus == nullptr)
      return CCL_INTERNALERROR;

    if (mCallbackHandle != nullptr)
    {
      auto rc = mMemberStatus->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
      mCallbackFunction = nullptr;
      if (rc != kVIA_OK)
        return CCL_INTERNALERROR;
    }

    mCallbackFunction = handler;

    if (mCallbackFunction != nullptr)
    {
      if (mMemberStatus->RegisterObserver(this, onUpdate ? VIAFbUpdateMode::eNotifyOnUpdate : VIAFbUpdateMode::eNotifyOnChange, &mCallbackHandle) != kVIA_OK)
      {
        mCallbackFunction = nullptr;
        mCallbackHandle = nullptr;
        return CCL_INTERNALERROR;
      }
    }

    return CCL_SUCCESS;
  }

  bool VValueEntity::IsEqual(const IValue* other) const
  {
    if (other == this) return true;
    const VValueEntity* ve = dynamic_cast<const VValueEntity*>(other);
    return (ve != nullptr) 
      && VValueBase::IsEqual(ve)
      && ve->mValueClass == mValueClass 
      && ve->mSignalName == mSignalName;
  }

  #pragma endregion

  #pragma region VCallContextValue Implementation

  // Call Context Value class
  VCallContextValue::VCallContextValue()
    : VValueBase(nullptr)
    , mCallContextID(NDetail::cInvalidCCLObjectID)
  {}

  VCallContextValue::~VCallContextValue()
  {}

  int VCallContextValue::Init(cclCallContextID ctxtID, const char* memberPath, cclValueRepresentation repr)
  {
    mCallContextID = ctxtID;

    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_INVALIDCALLCONTEXTID;

    if (!NDetail::MapTypeLevel(repr, mTypeLevel))
      return CCL_PARAMETERINVALID;

    std::string remaining{ (memberPath != nullptr) ? memberPath : "" };
    if (remaining.empty())
      return CCL_INVALIDMEMBERPATH;

    // leading . is optional
    if (remaining.front() == '.') remaining.erase(0, 1);
    NDetail::ExtractValueClass(remaining, mValueClass);

    NDetail::VIAObjectGuard<VIAFbValue> implValue;
    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INVALIDMEMBERPATH;

    case NDetail::CallContextValueClass::eInParam:
      if (ctxt->GetInParams(VIAFbTypeLevel::eImpl, &(implValue.mVIAObject)) != kVIA_OK)
        return CCL_INTERNALERROR;
      break;

    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      if (ctxt->GetOutParams(VIAFbTypeLevel::eImpl, &(implValue.mVIAObject)) != kVIA_OK)
        return CCL_INTERNALERROR;
      break;

    default:
      return remaining.empty() ? CCL_SUCCESS : CCL_INVALIDMEMBERPATH;
    }

    return VValueBase::Init(implValue.mVIAObject, remaining);
  }

  int VCallContextValue::CreateCallContextValue(cclCallContextID ctxtID, const char* memberPath, cclValueRepresentation repr)
  {
    std::unique_ptr<VCallContextValue> callContextValue{ new VCallContextValue() };
    int result = callContextValue->Init(ctxtID, memberPath, repr);
    if (result != CCL_SUCCESS)
      return result;

    return gModule->mValues.Add(callContextValue.release());
  }

  VIAFbCallContext* VCallContextValue::GetCallContext() const
  {
    // call context may have expired, always obtain the pointer from global map
    VCallContext* callContext = gModule->mCallContexts.Get(mCallContextID);
    return (callContext != nullptr) ? callContext->mContext : nullptr;
  }

  VIAFbValue* VCallContextValue::GetParamsValue(VIAFbCallContext* ctxt, bool* isValidMember) const
  {
    NDetail::VIAObjectGuard<VIAFbValue> implValue;
    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
      if (ctxt->GetInParams(VIAFbTypeLevel::eImpl, &(implValue.mVIAObject)) != kVIA_OK) return nullptr;
      break;

    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      if (ctxt->GetOutParams(VIAFbTypeLevel::eImpl, &(implValue.mVIAObject)) != kVIA_OK) return nullptr;
      break;

    default:
      return nullptr;
    }

    if (implValue.mVIAObject->IsValid(mMemberHandle, isValidMember) != kVIA_OK)
      return nullptr;

    VIAFbValue* memberValue;
    if (implValue.mVIAObject->GetMemberValue(mMemberHandle, mTypeLevel, &memberValue) != kVIA_OK)
      return nullptr;

    return memberValue;
  }

  int VCallContextValue::SetParamsValue(VIAFbCallContext* ctxt, VIAFbValue* value) const
  {
    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
      if (ctxt->SetInParams(value) != kVIA_OK)
        return CCL_WRONGSTATE;
      break;

    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      if (ctxt->SetOutParams(value) != kVIA_OK)
        return CCL_WRONGSTATE;
      break;

    default:
      break;
    }
    return CCL_SUCCESS;
  }

  int VCallContextValue::GetInteger(long long* x) const
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eCallTime:
    {
      VIATime time;
      if (ctxt->GetCallTime(&time) != kVIA_OK)
        return CCL_INTERNALERROR;
      if (time == kVIAFbTimeUndefined)
        return CCL_INVALIDCALLCONTEXTSTATE;
      *x = time;
      return CCL_SUCCESS;
    }
    case NDetail::CallContextValueClass::eReturnTime:
    {
      VIATime time;
      if (ctxt->GetReturnTime(&time) != kVIA_OK)
        return CCL_INTERNALERROR;
      if (time == kVIAFbTimeUndefined)
        return CCL_INVALIDCALLCONTEXTSTATE;
      *x = time;
      return CCL_SUCCESS;
    }
    case NDetail::CallContextValueClass::eRequestID:
      return (ctxt->GetRequestID(x) != kVIA_OK) ? CCL_INTERNALERROR : CCL_SUCCESS;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if ((paramsValue.mVIAObject == nullptr) || !isValid)
      return CCL_VALUENOTACCESSIBLE;

    if (mMemberType == VIAFbTypeTag::eSignedInteger)
    {
      int64 value;
      if (paramsValue.mVIAObject->GetSignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = value;
      return CCL_SUCCESS;
    }
    else if (mMemberType == VIAFbTypeTag::eUnsignedInteger)
    {
      uint64 value;
      if (paramsValue.mVIAObject->GetUnsignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = static_cast<long long>(value);
      return CCL_SUCCESS;
    }

    return CCL_WRONGTYPE;
  }

  int VCallContextValue::SetInteger(long long x)
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eCallTime:
    case NDetail::CallContextValueClass::eReturnTime:
    case NDetail::CallContextValueClass::eRequestID:
      return CCL_VALUEREADONLY;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if (paramsValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (mMemberType == VIAFbTypeTag::eSignedInteger)
    {
      int64 value{ x };
      if (paramsValue.mVIAObject->SetSignedInteger(kVIAFbTypeMemberHandleWhole, value) != kVIA_OK)
        return CCL_INTERNALERROR;
    }
    else if (mMemberType == VIAFbTypeTag::eUnsignedInteger)
    {
      uint64 value{ static_cast<uint64>(x) };
      if (paramsValue.mVIAObject->SetUnsignedInteger(kVIAFbTypeMemberHandleWhole, value) != kVIA_OK)
        return CCL_INTERNALERROR;
    }
    else
    {
      return CCL_WRONGTYPE;
    }

    return SetParamsValue(ctxt, paramsValue.mVIAObject);
  }

  int VCallContextValue::GetFloat(double* x) const
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    if (mMemberType != VIAFbTypeTag::eFloat)
      return CCL_WRONGTYPE;

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if ((paramsValue.mVIAObject == nullptr) || !isValid)
      return CCL_VALUENOTACCESSIBLE;

    if (paramsValue.mVIAObject->GetFloat(kVIAFbTypeMemberHandleWhole, x) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  int VCallContextValue::SetFloat(double x)
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    if (mMemberType != VIAFbTypeTag::eFloat)
      return CCL_WRONGTYPE;

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if (paramsValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (paramsValue.mVIAObject->SetFloat(kVIAFbTypeMemberHandleWhole, x) != kVIA_OK)
      return CCL_INTERNALERROR;

    return SetParamsValue(ctxt, paramsValue.mVIAObject);
  }

  int VCallContextValue::GetString(char* buffer, int bufferSize) const
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eConsumerPort:
      return (ctxt->GetClientPort(buffer, bufferSize) != kVIA_OK) ? CCL_INVALIDSIDE : CCL_SUCCESS;
    case NDetail::CallContextValueClass::eProviderPort:
      return (ctxt->GetServerPort(buffer, bufferSize) != kVIA_OK) ? CCL_INVALIDSIDE : CCL_SUCCESS;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    if (mMemberType != VIAFbTypeTag::eString)
      return CCL_WRONGTYPE;

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if ((paramsValue.mVIAObject == nullptr) || !isValid)
      return CCL_VALUENOTACCESSIBLE;

    switch (paramsValue.mVIAObject->GetString(kVIAFbTypeMemberHandleWhole, buffer, bufferSize))
    {
    case kVIA_OK:
      return CCL_SUCCESS;
    case kVIA_BufferToSmall:
      return CCL_BUFFERTOSMALL;
    default:
      break;
    }

    return CCL_INTERNALERROR;
  }

  int VCallContextValue::SetString(const char* str)
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eConsumerPort:
    case NDetail::CallContextValueClass::eProviderPort:
      return CCL_VALUEREADONLY;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    if (mMemberType != VIAFbTypeTag::eString)
      return CCL_WRONGTYPE;

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if (paramsValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (paramsValue.mVIAObject->SetString(kVIAFbTypeMemberHandleWhole, str) != kVIA_OK)
      return CCL_INTERNALERROR;

    return SetParamsValue(ctxt, paramsValue.mVIAObject);
  }

  int VCallContextValue::GetData(unsigned char* buffer, int* bufferSize) const
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if ((paramsValue.mVIAObject == nullptr) || !isValid)
      return CCL_VALUENOTACCESSIBLE;

    if (paramsValue.mVIAObject->GetData(kVIAFbTypeMemberHandleWhole, buffer, *bufferSize) != kVIA_OK)
      return CCL_BUFFERTOSMALL;

    size_t size;
    if (paramsValue.mVIAObject->GetSize(kVIAFbTypeMemberHandleWhole, &size) != kVIA_OK)
      return CCL_INTERNALERROR;
    *bufferSize = size;

    return CCL_SUCCESS;
  }

  int VCallContextValue::SetData(const unsigned char* data, int size)
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if (paramsValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (paramsValue.mVIAObject->SetData(kVIAFbTypeMemberHandleWhole, data, size) != kVIA_OK)
      return CCL_INVALIDDATA;

    return SetParamsValue(ctxt, paramsValue.mVIAObject);
  }

  int VCallContextValue::GetEnum(int* x) const
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eSide:
    {
      VIAFbFunctionCallSide side;
      if (ctxt->GetSide(&side) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = static_cast<int>((side == VIAFbFunctionCallSide::eClient) ? cclSide::CCL_SIDE_CONSUMER : cclSide::CCL_SIDE_PROVIDER);
      return CCL_SUCCESS;
    }
    case NDetail::CallContextValueClass::eCallState:
    {
      VIAFbFunctionCallState state;
      if (ctxt->GetCallState(&state) != kVIA_OK)
        return CCL_INTERNALERROR;
      cclCallState mapped;
      if (VFunctionUtils::MapCallState(state, mapped) < 0)
        return CCL_INVALIDCALLCONTEXTSTATE;
      *x = static_cast<int>(mapped);
      return CCL_SUCCESS;
    }
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    if (mMemberType != VIAFbTypeTag::eSignedInteger)
      return CCL_WRONGTYPE;

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if ((paramsValue.mVIAObject == nullptr) || !isValid)
      return CCL_VALUENOTACCESSIBLE;

    int64 value;
    if (paramsValue.mVIAObject->GetSignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
      return CCL_INTERNALERROR;

    *x = static_cast<int>(value);
    return CCL_SUCCESS;
  }

  int VCallContextValue::SetEnum(int x)
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eSide:
    case NDetail::CallContextValueClass::eCallState:
      return CCL_VALUEREADONLY;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    if (mMemberType != VIAFbTypeTag::eSignedInteger)
      return CCL_WRONGTYPE;

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if (paramsValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    int64 value{ x };
    if (paramsValue.mVIAObject->SetSignedInteger(kVIAFbTypeMemberHandleWhole, value) != kVIA_OK)
      return CCL_INTERNALERROR;

    return SetParamsValue(ctxt, paramsValue.mVIAObject);
  }

  int VCallContextValue::GetUnionSelector(int* selector) const
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    if (mMemberType != VIAFbTypeTag::eUnion)
      return CCL_WRONGTYPE;

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if ((paramsValue.mVIAObject == nullptr) || !isValid)
      return CCL_VALUENOTACCESSIBLE;

    size_t index;
    if (paramsValue.mVIAObject->GetValidMemberIndex(kVIAFbTypeMemberHandleWhole, &index) != kVIA_OK)
      return CCL_INTERNALERROR;

    *selector = static_cast<int>(index);
    return CCL_SUCCESS;
  }

  int VCallContextValue::GetArraySize(int* size) const
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    if (mMemberType != VIAFbTypeTag::eArray)
      return CCL_WRONGTYPE;

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if ((paramsValue.mVIAObject == nullptr) || !isValid)
      return CCL_VALUENOTACCESSIBLE;

    size_t length;
    if (paramsValue.mVIAObject->GetArrayLength(kVIAFbTypeMemberHandleWhole, &length) != kVIA_OK)
      return CCL_INTERNALERROR;

    *size = static_cast<int>(length);
    return CCL_SUCCESS;
  }

  int VCallContextValue::SetArraySize(int size)
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    if (mMemberType != VIAFbTypeTag::eArray)
      return CCL_WRONGTYPE;

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if (paramsValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    size_t length{ static_cast<size_t>(size) };
    if (paramsValue.mVIAObject->SetArrayLength(kVIAFbTypeMemberHandleWhole, length) != kVIA_OK)
      return CCL_INTERNALERROR;

    return SetParamsValue(ctxt, paramsValue.mVIAObject);
  }

  int VCallContextValue::GetValueType(cclValueType* valueType) const
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eCallTime:
    case NDetail::CallContextValueClass::eReturnTime:
    case NDetail::CallContextValueClass::eRequestID:
      *valueType = cclValueType::CCL_VALUE_TYPE_INTEGER;
      return CCL_SUCCESS;
    case NDetail::CallContextValueClass::eSide:
    case NDetail::CallContextValueClass::eCallState:
      *valueType = cclValueType::CCL_VALUE_TYPE_ENUM;
      return CCL_SUCCESS;
    case NDetail::CallContextValueClass::eConsumerPort:
    case NDetail::CallContextValueClass::eProviderPort:
      *valueType = cclValueType::CCL_VALUE_TYPE_STRING;
      return CCL_SUCCESS;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    switch (mMemberType)
    {
    case VIAFbTypeTag::eSignedInteger:
    case VIAFbTypeTag::eUnsignedInteger:
      *valueType = cclValueType::CCL_VALUE_TYPE_INTEGER;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eFloat:
      *valueType = cclValueType::CCL_VALUE_TYPE_FLOAT;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eString:
      *valueType = cclValueType::CCL_VALUE_TYPE_STRING;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eArray:
      *valueType = cclValueType::CCL_VALUE_TYPE_ARRAY;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eStruct:
      *valueType = cclValueType::CCL_VALUE_TYPE_STRUCT;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eUnion:
      *valueType = cclValueType::CCL_VALUE_TYPE_UNION;
      return CCL_SUCCESS;
    default:
      break;
    }
    return CCL_INTERNALERROR;
  }

  int VCallContextValue::IsValid(bool* isValid) const
  {
    if (isValid == nullptr)
      return CCL_PARAMETERINVALID;

    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eCallTime:
    case NDetail::CallContextValueClass::eReturnTime:
    case NDetail::CallContextValueClass::eRequestID:
    case NDetail::CallContextValueClass::eSide:
    case NDetail::CallContextValueClass::eCallState:
    case NDetail::CallContextValueClass::eConsumerPort:
    case NDetail::CallContextValueClass::eProviderPort:
      return CCL_VALUENOTOPTIONAL;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, isValid));
    if (paramsValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    return CCL_SUCCESS;
  }

  int VCallContextValue::ClearValue()
  {
    auto* ctxt = GetCallContext();
    if (ctxt == nullptr)
      return CCL_CALLCONTEXTEXPIRED;

    switch (mValueClass)
    {
    case NDetail::CallContextValueClass::eInParam:
    case NDetail::CallContextValueClass::eOutParam:
    case NDetail::CallContextValueClass::eResultValue:
      break;
    case NDetail::CallContextValueClass::eCallTime:
    case NDetail::CallContextValueClass::eReturnTime:
    case NDetail::CallContextValueClass::eRequestID:
    case NDetail::CallContextValueClass::eSide:
    case NDetail::CallContextValueClass::eCallState:
    case NDetail::CallContextValueClass::eConsumerPort:
    case NDetail::CallContextValueClass::eProviderPort:
      return CCL_VALUEREADONLY;
    case NDetail::CallContextValueClass::eUnknown:
      return CCL_INTERNALERROR;
    default:
      return CCL_WRONGTYPE;
    }

    bool isValid;
    NDetail::VIAObjectGuard<VIAFbValue> paramsValue(GetParamsValue(ctxt, &isValid));
    if ((paramsValue.mVIAObject == nullptr) || !isValid)
      return CCL_VALUENOTACCESSIBLE;

    if (paramsValue.mVIAObject->SetIsValid(kVIAFbTypeMemberHandleWhole, false) != kVIA_OK)
      return CCL_VALUENOTOPTIONAL;

    return SetParamsValue(ctxt, paramsValue.mVIAObject);
  }

  #pragma endregion

  #pragma region VPDUSignalValue Implementation

  template<typename PortType>
  VPDUSignalValue<PortType>::VPDUSignalValue(PortType* port)
    : VPortBase<PortType>(port, false)
    , mValueID(NDetail::cInvalidCCLObjectID)
    , mMemberStatus(nullptr)
    , mMemberHandle(kVIAFbTypeMemberHandleWhole)
    , mCallbackFunction(nullptr)
    , mCallbackHandle(nullptr)
  {}

  template<typename PortType>
  VPDUSignalValue<PortType>::~VPDUSignalValue()
  {
    if (mCallbackHandle != nullptr)
    {
      if (mMemberStatus != nullptr) mMemberStatus->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
      mCallbackFunction = nullptr;
    }
    if (mMemberStatus != nullptr) mMemberStatus->Release();
  }

  template<typename PortType>
  VIAFbValue* VPDUSignalValue<PortType>::GetMemberValue() const
  {
    if (mMemberHandle == kVIAFbTypeMemberHandleWhole)
    {
      NDetail::VIAObjectGuard<VIAFbValue> signalValue;
      if (mPort->GetSignalValue(mSignalName.c_str(), mTypeLevel, &(signalValue.mVIAObject)) != kVIA_OK)
        return nullptr;

      return signalValue.Release();
    }

    NDetail::VIAObjectGuard<VIAFbValue> signalValue;
    if (mPort->GetSignalValue(mSignalName.c_str(), VIAFbTypeLevel::eImpl, &(signalValue.mVIAObject)) != kVIA_OK)
      return nullptr;

    NDetail::VIAObjectGuard<VIAFbValue> memberValue;
    if (signalValue.mVIAObject->GetMemberValue(mMemberHandle, mTypeLevel, &(memberValue.mVIAObject)) != kVIA_OK)
      return nullptr;

    return memberValue.Release();
  }

  template<typename PortType>
  bool VPDUSignalValue<PortType>::IsMemberValid() const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberHandle == kVIAFbTypeMemberHandleWhole)
      return true;

    NDetail::VIAObjectGuard<VIAFbValue> signalValue;
    if (mPort->GetSignalValue(mSignalName.c_str(), VIAFbTypeLevel::eImpl, &(signalValue.mVIAObject)) != kVIA_OK)
      return false;

    bool isValid;
    if (signalValue.mVIAObject->IsValid(mMemberHandle, &isValid) != kVIA_OK)
      return false;

    return isValid;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::Init(const char* signalName, const char* member, cclValueRepresentation repr)
  {
    if (signalName == nullptr)
      return CCL_PARAMETERINVALID;

    if (!NDetail::MapTypeLevel(repr, mTypeLevel))
      return CCL_PARAMETERINVALID;

    NDetail::VIAObjectGuard<VIAFbValue> signalValue;
    if (mPort->GetSignalValue(signalName, VIAFbTypeLevel::eImpl, &(signalValue.mVIAObject)) != kVIA_OK)
      return CCL_INVALIDSIGNALNAME;

    mSignalName = signalName;
    if ((member != nullptr) && (strlen(member) > 0))
    {
      NDetail::VIAObjectGuard<VIAFbType> signalType;
      if (signalValue.mVIAObject->GetType(&(signalType.mVIAObject)) != kVIA_OK)
        return CCL_INTERNALERROR;

      if (signalType.mVIAObject->GetMember(member, &mMemberHandle) != kVIA_OK)
        return CCL_INVALIDMEMBERPATH;
    }

    NDetail::VIAObjectGuard<VIAFbType> memberType;
    if (signalValue.mVIAObject->GetMemberType(mMemberHandle, mTypeLevel, &(memberType.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    if (memberType.mVIAObject->GetTypeTag(&mMemberType) != kVIA_OK)
      return CCL_INTERNALERROR;

    if (signalValue.mVIAObject->GetMemberStatus(mMemberHandle, &mMemberStatus) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::CreateSignalValue(PortType* port, const char* signalName, const char* member, cclValueRepresentation repr)
  {
    std::unique_ptr<VPDUSignalValue> signalValue{ new VPDUSignalValue(port) };
    int result = signalValue->Init(signalName, member, repr);
    if (result != CCL_SUCCESS)
      return result;

    return gModule->mValues.Add(signalValue.release());
  }

  template<typename PortType>
  bool VPDUSignalValue<PortType>::IsEqual(const IValue* other) const
  {
    if (other == this) return true;
    const VPDUSignalValue<PortType>* value = dynamic_cast<const VPDUSignalValue<PortType>*>(other);
    return (value != nullptr)
      && (VPortBase<PortType>::IsEqual(value))
      && (mMemberHandle == value->mMemberHandle)
      && (mTypeLevel == value->mTypeLevel)
      && (mSignalName == value->mSignalName);
  }

  template<typename PortType>
  VIASTDDEF VPDUSignalValue<PortType>::OnValueUpdated(VIATime inTime, VIAFbStatus* inStatus)
  {
    if (mCallbackFunction != nullptr)
    {
      // VPDUSignalValue objects are never removed during measurement, therefore their ID stays valid
      mCallbackFunction(static_cast<cclTime>(inTime), mValueID);
    }
    return kVIA_OK;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetInteger(long long* x) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (mMemberType == VIAFbTypeTag::eSignedInteger)
    {
      int64 value;
      if (fbValue.mVIAObject->GetSignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = value;
      return CCL_SUCCESS;
    }
    else if (mMemberType == VIAFbTypeTag::eUnsignedInteger)
    {
      uint64 value;
      if (fbValue.mVIAObject->GetUnsignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = static_cast<long long>(value);
      return CCL_SUCCESS;
    }

    return CCL_WRONGTYPE;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::SetInteger(long long x)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (mMemberType == VIAFbTypeTag::eSignedInteger)
    {
      int64 value{ x };
      if (fbValue.mVIAObject->SetSignedInteger(kVIAFbTypeMemberHandleWhole, value) != kVIA_OK)
        return CCL_INTERNALERROR;
    }
    else if (mMemberType == VIAFbTypeTag::eUnsignedInteger)
    {
      uint64 value{ static_cast<uint64>(x) };
      if (fbValue.mVIAObject->SetUnsignedInteger(kVIAFbTypeMemberHandleWhole, value) != kVIA_OK)
        return CCL_INTERNALERROR;
    }
    else
    {
      return CCL_WRONGTYPE;
    }

    if (fbValue.mVIAObject->Commit() != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetFloat(double* x) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    if (mMemberType != VIAFbTypeTag::eFloat)
      return CCL_WRONGTYPE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (fbValue.mVIAObject->GetFloat(kVIAFbTypeMemberHandleWhole, x) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::SetFloat(double x)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberType != VIAFbTypeTag::eFloat)
      return CCL_WRONGTYPE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (fbValue.mVIAObject->SetFloat(kVIAFbTypeMemberHandleWhole, x) != kVIA_OK)
      return CCL_INTERNALERROR;

    if (fbValue.mVIAObject->Commit() != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetString(char* buffer, int bufferSize) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    if (mMemberType != VIAFbTypeTag::eString)
      return CCL_WRONGTYPE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    switch (fbValue.mVIAObject->GetString(kVIAFbTypeMemberHandleWhole, buffer, bufferSize))
    {
    case kVIA_OK:
      return CCL_SUCCESS;
    case kVIA_BufferToSmall:
      return CCL_BUFFERTOSMALL;
    default:
      break;
    }

    return CCL_INTERNALERROR;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::SetString(const char* str)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberType != VIAFbTypeTag::eString)
      return CCL_WRONGTYPE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (fbValue.mVIAObject->SetString(kVIAFbTypeMemberHandleWhole, str) != kVIA_OK)
      return CCL_INTERNALERROR;

    if (fbValue.mVIAObject->Commit() != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetData(unsigned char* buffer, int* bufferSize) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (fbValue.mVIAObject->GetData(kVIAFbTypeMemberHandleWhole, buffer, *bufferSize) != kVIA_OK)
      return CCL_BUFFERTOSMALL;

    size_t size;
    if (fbValue.mVIAObject->GetSize(kVIAFbTypeMemberHandleWhole, &size) != kVIA_OK)
      return CCL_INTERNALERROR;
    *bufferSize = size;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::SetData(const unsigned char* data, int size)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    if (fbValue.mVIAObject->SetData(kVIAFbTypeMemberHandleWhole, data, size) != kVIA_OK)
      return CCL_INVALIDDATA;

    if (fbValue.mVIAObject->Commit() != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetEnum(int* x) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    switch (mMemberType)
    {
    case VIAFbTypeTag::eSignedInteger: {
      int64 value;
      if (fbValue.mVIAObject->GetSignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = static_cast<int>(value);
      return CCL_SUCCESS;
    }
    case VIAFbTypeTag::eUnsignedInteger: {
      uint64 value;
      if (fbValue.mVIAObject->GetUnsignedInteger(kVIAFbTypeMemberHandleWhole, &value) != kVIA_OK)
        return CCL_INTERNALERROR;
      *x = static_cast<int>(value);
      return CCL_SUCCESS;
    }
    default:
      break;
    }

    return CCL_WRONGTYPE;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::SetEnum(int x)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    switch (mMemberType)
    {
    case VIAFbTypeTag::eSignedInteger:
      if (fbValue.mVIAObject->SetSignedInteger(kVIAFbTypeMemberHandleWhole, static_cast<int64>(x)) != kVIA_OK)
        return CCL_INTERNALERROR;
      break;
    case VIAFbTypeTag::eUnsignedInteger:
      if (fbValue.mVIAObject->SetUnsignedInteger(kVIAFbTypeMemberHandleWhole, static_cast<uint64>(x)) != kVIA_OK)
        return CCL_INTERNALERROR;
      break;
    default:
      return CCL_WRONGTYPE;
    }

    if (fbValue.mVIAObject->Commit() != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetUnionSelector(int* selector) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    if (mMemberType != VIAFbTypeTag::eUnion)
      return CCL_WRONGTYPE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    size_t index;
    if (fbValue.mVIAObject->GetValidMemberIndex(kVIAFbTypeMemberHandleWhole, &index) != kVIA_OK)
      return CCL_INTERNALERROR;

    *selector = static_cast<int>(index);
    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetArraySize(int* size) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (!IsMemberValid())
      return CCL_VALUENOTACCESSIBLE;

    if (mMemberType != VIAFbTypeTag::eArray)
      return CCL_WRONGTYPE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    size_t length;
    if (fbValue.mVIAObject->GetArrayLength(kVIAFbTypeMemberHandleWhole, &length) != kVIA_OK)
      return CCL_INTERNALERROR;

    *size = static_cast<int>(length);
    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::SetArraySize(int size)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberType != VIAFbTypeTag::eArray)
      return CCL_WRONGTYPE;

    NDetail::VIAObjectGuard<VIAFbValue> fbValue(GetMemberValue());
    if (fbValue.mVIAObject == nullptr)
      return CCL_VALUENOTACCESSIBLE;

    NDetail::VIAObjectGuard<VIAFbType> fbType;
    if (fbValue.mVIAObject->GetType(&(fbType.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbTypeTag typeTag;
    if (fbType.mVIAObject->GetTypeTag(&typeTag) != kVIA_OK)
      return CCL_INTERNALERROR;

    if (typeTag != VIAFbTypeTag::eArray)
      return CCL_WRONGTYPE;

    size_t length{ static_cast<size_t>(size) };
    if (fbValue.mVIAObject->SetArrayLength(kVIAFbTypeMemberHandleWhole, length) != kVIA_OK)
      return CCL_ARRAYNOTRESIZABLE;

    if (fbValue.mVIAObject->Commit() != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetValueType(cclValueType* valueType) const
  {
    switch (mMemberType)
    {
    case VIAFbTypeTag::eSignedInteger:
    case VIAFbTypeTag::eUnsignedInteger:
      *valueType = cclValueType::CCL_VALUE_TYPE_INTEGER;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eFloat:
      *valueType = cclValueType::CCL_VALUE_TYPE_FLOAT;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eString:
      *valueType = cclValueType::CCL_VALUE_TYPE_STRING;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eArray:
      *valueType = cclValueType::CCL_VALUE_TYPE_ARRAY;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eStruct:
      *valueType = cclValueType::CCL_VALUE_TYPE_STRUCT;
      return CCL_SUCCESS;
    case VIAFbTypeTag::eUnion:
      *valueType = cclValueType::CCL_VALUE_TYPE_UNION;
      return CCL_SUCCESS;
    default:
      break;
    }
    return CCL_UNSUPPORTEDTYPE;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::IsValid(bool* isValid) const
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (isValid == nullptr)
      return CCL_PARAMETERINVALID;

    NDetail::VIAObjectGuard<VIAFbValue> signalValue;
    if (mPort->GetSignalValue(mSignalName.c_str(), VIAFbTypeLevel::eImpl, &(signalValue.mVIAObject)) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    if (signalValue.mVIAObject->IsValid(mMemberHandle, isValid) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::ClearValue()
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mMemberHandle == kVIAFbTypeMemberHandleWhole)
      return CCL_VALUENOTOPTIONAL;

    NDetail::VIAObjectGuard<VIAFbValue> signalValue;
    if (mPort->GetSignalValue(mSignalName.c_str(), VIAFbTypeLevel::eImpl, &(signalValue.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    if (signalValue.mVIAObject->SetIsValid(mMemberHandle, false) != kVIA_OK)
      return CCL_VALUENOTOPTIONAL;

    if (signalValue.mVIAObject->Commit() != kVIA_OK)
      return CCL_VALUEREADONLY;

    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetUpdateTimeNS(cclTime* time) const
  {
    if (mMemberStatus == nullptr)
      return CCL_INTERNALERROR;

    VIATime value;
    if (mMemberStatus->GetLastUpdateTimestamp(&value) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    *time = value;
    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetChangeTimeNS(cclTime* time) const
  {
    if (mMemberStatus == nullptr)
      return CCL_INTERNALERROR;

    VIATime value;
    if (mMemberStatus->GetLastChangeTimestamp(&value) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    *time = value;
    return CCL_SUCCESS;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::GetState(cclValueState* state) const
  {
    if (mMemberStatus == nullptr)
      return CCL_INTERNALERROR;

    VIAFbValueState valueState;
    if (mMemberStatus->GetValueState(&valueState) != kVIA_OK)
      return CCL_VALUENOTACCESSIBLE;

    switch (valueState)
    {
    case VIAFbValueState::eOffline:
      *state = cclValueState::CCL_VALUE_STATE_OFFLINE;
      return CCL_SUCCESS;
    case VIAFbValueState::eMeasured:
      *state = cclValueState::CCL_VALUE_STATE_MEASURED;
      return CCL_SUCCESS;
    default:
      break;
    }

    return CCL_INTERNALERROR;
  }

  template<typename PortType>
  int VPDUSignalValue<PortType>::SetHandler(bool onUpdate, cclValueHandler handler)
  {
    if (mMemberStatus == nullptr)
      return CCL_INTERNALERROR;

    if (mCallbackHandle != nullptr)
    {
      auto rc = mMemberStatus->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
      mCallbackFunction = nullptr;
      if (rc != kVIA_OK)
        return CCL_INTERNALERROR;
    }

    mCallbackFunction = handler;

    if (mCallbackFunction != nullptr)
    {
      if (mMemberStatus->RegisterObserver(this, onUpdate ? VIAFbUpdateMode::eNotifyOnUpdate : VIAFbUpdateMode::eNotifyOnChange, &mCallbackHandle) != kVIA_OK)
      {
        mCallbackFunction = nullptr;
        mCallbackHandle = nullptr;
        return CCL_INTERNALERROR;
      }
    }

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VFunction Implementation

  // Static function utilities
  namespace VFunctionUtils
  {
    int LookupFunction(cclFunctionID functionID, IFunctionPort*& function)
    {
      if (gModule == NULL)
        return CCL_INTERNALERROR;

      function = gModule->mFunctions.Get(functionID);
      if (function == nullptr)
        return CCL_INVALIDFUNCTIONID;

      return CCL_SUCCESS;
    }

    int MapCallState(cclCallState callState, VIAFbFunctionCallState& mapped)
    {
      switch (callState)
      {
      case cclCallState::CCL_CALL_STATE_CALLING:
        mapped = VIAFbFunctionCallState::eCalling;
        return 0;
      case cclCallState::CCL_CALL_STATE_CALLED:
        mapped = VIAFbFunctionCallState::eCalled;
        return 1;
      case cclCallState::CCL_CALL_STATE_RETURNING:
        mapped = VIAFbFunctionCallState::eReturning;
        return 2;
      case cclCallState::CCL_CALL_STATE_RETURNED:
        mapped = VIAFbFunctionCallState::eReturned;
        return 3;
      default:
        break;
      }
      return -1;
    }

    int MapCallState(VIAFbFunctionCallState callState, cclCallState& mapped)
    {
      switch (callState)
      {
      case VIAFbFunctionCallState::eCalling:
        mapped = cclCallState::CCL_CALL_STATE_CALLING;
        return 0;
      case VIAFbFunctionCallState::eCalled:
        mapped = cclCallState::CCL_CALL_STATE_CALLED;
        return 1;
      case VIAFbFunctionCallState::eReturning:
        mapped = cclCallState::CCL_CALL_STATE_RETURNING;
        return 2;
      case VIAFbFunctionCallState::eReturned:
        mapped = cclCallState::CCL_CALL_STATE_RETURNED;
        return 3;
      default:
        break;
      }
      return -1;
    }

    cclCallContextID CreateCallContext(VIAFbCallContext* ctxt, cclFunctionID funcID, bool release)
    {
      if (gModule == nullptr)
        return CCL_INTERNALERROR;

      return gModule->mCallContexts.Add(new VCallContext(funcID, ctxt, release));
    }

    bool DestroyCallContext(cclCallContextID ctxtID)
    {
      if (gModule == nullptr)
        return false;

      return gModule->mCallContexts.Remove(ctxtID);
    }
  }

  // Consumer side function class
  VClientFunction::VClientFunction(VIAFbFunctionClientPort* port, const char* portPath)
    : VFunctionBase(port, CCL_SIDE_CONSUMER, portPath)
  {}

  VClientFunction::~VClientFunction()
  {}

  int VClientFunction::CreateClientFunction(const char* portPath)
  {
    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbFunctionClientPort* port;
    if (fbAPI->GetFunctionClientPort(portPath, &port) != kVIA_OK)
      return CCL_INVALIDFUNCTIONPATH;

    return gModule->mFunctions.Add(new VClientFunction(port, portPath));
  }

  cclCallContextID VClientFunction::FindCallContext(VIAFbCallContext* ctxt)
  {
    long long requestID;
    if (ctxt->GetRequestID(&requestID) != kVIA_OK)
      return -1;

    auto it = mRequestIDMap.find(requestID);
    if (it != mRequestIDMap.end())
      return it->second;

    return NDetail::cInvalidCCLObjectID;
  }

  cclCallContextID VClientFunction::CreateCallContext(VIAFbCallContext* ctxt)
  {
    long long requestID;
    if (ctxt->GetRequestID(&requestID) != kVIA_OK)
    {
      ctxt->Release();
      return CCL_INTERNALERROR;
    }

    cclCallContextID ctxtID = VFunctionUtils::CreateCallContext(ctxt, mFunctionID, true);
    if (ctxtID == NDetail::cInvalidCCLObjectID)
    {
      ctxt->Release();
      return CCL_TOOMANYCALLCONTEXTS;
    }

    mRequestIDMap[requestID] = ctxtID;
    return ctxtID;
  }

  bool VClientFunction::DestroyCallContext(VIAFbCallContext* ctxt)
  {
    long long requestID;
    if (ctxt->GetRequestID(&requestID) != kVIA_OK)
      return false;

    auto rqit = mRequestIDMap.find(requestID);
    if (rqit == mRequestIDMap.end())
      return false;

    cclCallContextID ctxtID = rqit->second;
    mRequestIDMap.erase(rqit);

    return VFunctionUtils::DestroyCallContext(ctxtID);
  }

  VIASTDDEF VClientFunction::OnCallStateChanged(VIATime inTime, VIAFbCallContext* inCallContext,
    VIAFbFunctionCallState inCallState, const VIAFbFunctionClientPort* inClientPort)
  {
    if (inCallState == VIAFbFunctionCallState::eFinalizing)
    {
      return DestroyCallContext(inCallContext) ? kVIA_OK : kVIA_Failed;
    }

    cclCallState cclState;
    int callbackIndex = VFunctionUtils::MapCallState(inCallState, cclState);
    if ((callbackIndex < 0) || (callbackIndex > 3))
      return kVIA_Failed;

    if (mCallbacks[callbackIndex].mCallbackFunction != nullptr)
    {
      cclCallContextID ctxtID = FindCallContext(inCallContext);
      if (ctxtID == NDetail::cInvalidCCLObjectID)
      {
        // call may have been started through different APIs, create temporary context
        ctxtID = VFunctionUtils::CreateCallContext(inCallContext, mFunctionID, false);
        if (ctxtID == NDetail::cInvalidCCLObjectID)
          return kVIA_Failed;

        mCallbacks[callbackIndex].mCallbackFunction(static_cast<cclTime>(inTime), ctxtID);

        if (!VFunctionUtils::DestroyCallContext(ctxtID))
          return kVIA_Failed;
      }
      else
      {
        mCallbacks[callbackIndex].mCallbackFunction(static_cast<cclTime>(inTime), ctxtID);
      }
    }
    return kVIA_OK;
  }

  // Provider side function class
  VServerFunction::VServerFunction(VIAFbFunctionServerPort* port, const char* portPath)
    : VFunctionBase(port, CCL_SIDE_PROVIDER, portPath)
  {}

  VServerFunction::~VServerFunction()
  {}

  int VServerFunction::CreateServerFunction(const char* portPath)
  {
    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbFunctionServerPort* port;
    if (fbAPI->GetFunctionServerPort(portPath, &port) != kVIA_OK)
      return CCL_INVALIDFUNCTIONPATH;

    return gModule->mFunctions.Add(new VServerFunction(port, portPath));
  }

  VIASTDDEF VServerFunction::OnCallStateChanged(VIATime inTime, VIAFbCallContext* inCallContext,
    VIAFbFunctionCallState inCallState, const VIAFbFunctionServerPort* inServerPort)
  {
    cclCallState cclState;
    int callbackIndex = VFunctionUtils::MapCallState(inCallState, cclState);
    if ((callbackIndex < 0) || (callbackIndex > 3))
      return kVIA_Failed;

    if (mCallbacks[callbackIndex].mCallbackFunction == nullptr)
      return kVIA_OK;

    // call may have been started through different APIs, create temporary context
    auto ctxtID = VFunctionUtils::CreateCallContext(inCallContext, mFunctionID, false);
    if (ctxtID == NDetail::cInvalidCCLObjectID)
      return kVIA_Failed;

    mCallbacks[callbackIndex].mCallbackFunction(static_cast<cclTime>(inTime), ctxtID);
    return VFunctionUtils::DestroyCallContext(ctxtID) ? kVIA_OK : kVIA_Failed;
  }

  // Call context class
  VCallContext::VCallContext(cclFunctionID funcID, VIAFbCallContext* ctxt, bool release)
    : mFunctionID(funcID)
    , mContext(ctxt)
    , mRequestID(-1LL)
    , mSide(VIAFbFunctionCallSide::eClient)
    , mRelease(release)
  {
    mContext->GetRequestID(&mRequestID);
    mContext->GetSide(&mSide);
  }

  VCallContext::~VCallContext()
  {
    if (mRelease && (mContext != nullptr)) mContext->Release();

    for (auto valueID : mContextValues)
    {
      gModule->mValues.Remove(valueID);
    }
  }

  VIASTDDEF VCallContext::OnCallStateChanged(VIATime inTime, VIAFbCallContext* inCallContext,
    VIAFbFunctionCallState inCallState, const VIAFbFunctionClientPort* inClientPort)
  {
    if ((mResultCallbackFunction != nullptr) && (inCallState == VIAFbFunctionCallState::eReturned))
      mResultCallbackFunction(static_cast<cclTime>(inTime), mContextID);

    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VService Implementation

  VService::VService()
    : mServiceID(-1)
    , mService(nullptr)
    , mServiceDiscoveryHandler(nullptr)
    , mConsumerDiscoveredHandler(nullptr)
    , mProviderDiscoveredHandler(nullptr)
    , mCallbackHandle(nullptr)
  {}

  VService::~VService()
  {
    if (mCallbackHandle != nullptr)
    {
      if (mService != nullptr) mService->UnregisterObserver(mCallbackHandle);
      mServiceDiscoveryHandler = nullptr;
      mConsumerDiscoveredHandler = nullptr;
      mProviderDiscoveredHandler = nullptr;
      mCallbackHandle = nullptr;
    }
    if (mService != nullptr) mService->Release();
  }

  int VService::CreateService(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == nullptr)
      return CCL_INTERNALERROR;

    cclServiceID serviceID = static_cast<cclServiceID>(gModule->mServices.size());
    std::unique_ptr<VService> service{ new VService() };
    service->mServiceID = serviceID;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    if (fbAPI->GetServiceCO(path, &(service->mService)) != kVIA_OK)
      return CCL_INVALIDSERVICEPATH;

    gModule->mServices.push_back(service.release());

    return serviceID;
  }

  int VService::LookupService(cclServiceID serviceID, VService*& service)
  {
    if (gModule == nullptr)
      return CCL_INTERNALERROR;

    if (serviceID < 0 || serviceID >= static_cast<int>(gModule->mServices.size()))
      return CCL_INVALIDSERVICEID;

    service = gModule->mServices[serviceID];
    if ((service == nullptr) || (service->mServiceID != serviceID))
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  void VService::Cleanup()
  {
    for (auto* service : gModule->mServices) delete service;
    gModule->mServices.clear();
  }

  int VService::UpdateRegistration()
  {
    if (mService == nullptr)
      return CCL_INTERNALERROR;

    VIAResult rv = kVIA_OK;
    if ((mServiceDiscoveryHandler != nullptr) || (mConsumerDiscoveredHandler != nullptr) || (mProviderDiscoveredHandler != nullptr))
    {
      if (mCallbackHandle == nullptr)
        rv = mService->RegisterObserver(this, &mCallbackHandle);
    }
    else
    {
      if (mCallbackHandle != nullptr)
        rv = mService->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
    }
    return (rv == kVIA_OK) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VService::SetServiceDiscoveryHandler(cclServiceDiscoveryHandler handler)
  {
    mServiceDiscoveryHandler = handler;
    return UpdateRegistration();
  }

  int VService::SetConsumerDiscoveredHandler(cclConsumerDiscoveredHandler handler)
  {
    mConsumerDiscoveredHandler = handler;
    return UpdateRegistration();
  }

  int VService::SetProviderDiscoveredHandler(cclProviderDiscoveredHandler handler)
  {
    mProviderDiscoveredHandler = handler;
    return UpdateRegistration();
  }

  VIASTDDEF VService::OnServiceDiscovery(VIAFbServiceCO* inService, VIAFbServiceConsumer* inConsumer)
  {
    if (mService != inService)
      return kVIA_Failed;

    if (mServiceDiscoveryHandler != nullptr)
    {
      cclConsumerID consumerID = VConsumer::CreateConsumer(inConsumer, false);
      if (consumerID < 0)
        return kVIA_Failed;

      mServiceDiscoveryHandler(consumerID);
      return VConsumer::RemoveConsumer(consumerID);
    }
    return kVIA_OK;
  }

  VIASTDDEF VService::OnConsumerDiscovered(VIAFbServiceCO* inService, VIAFbAddressHandle* inAddress)
  {
    if (mService != inService)
      return kVIA_Failed;

    if (mConsumerDiscoveredHandler != nullptr)
    {
      auto addressID = VAddress::CreateAddress(inAddress, false);
      if (addressID < 0)
        return kVIA_Failed;

      mConsumerDiscoveredHandler(addressID);
      return VAddress::RemoveAddress(addressID);
    }
    return kVIA_OK;
  }

  VIASTDDEF VService::OnProviderDiscovered(VIAFbServiceCO* inService, VIAFbAddressHandle* inAddress)
  {
    if (mService != inService)
      return kVIA_Failed;

    if (mProviderDiscoveredHandler != nullptr)
    {
      auto addressID = VAddress::CreateAddress(inAddress, false);
      if (addressID < 0)
        return kVIA_Failed;

      mProviderDiscoveredHandler(addressID);
      return VAddress::RemoveAddress(addressID);
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VConsumer Implementation

  VConsumer::VConsumer(VIAFbServiceConsumer* consumer, bool release)
    : VPortBase(consumer, release)
    , mConsumerID(NDetail::cInvalidCCLObjectID)
    , mProviderDiscoveredHandler(nullptr)
    , mCallbackHandle(nullptr)
  {}

  VConsumer::~VConsumer()
  {
    if (mCallbackHandle != nullptr)
    {
      if (mPort != nullptr)
        mPort->UnregisterObserver(mCallbackHandle);

      mProviderDiscoveredHandler = nullptr;
      mCallbackHandle = nullptr;
    }
  }

  int VConsumer::CreateConsumer(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbServiceConsumer* consumer = nullptr;
    if (fbAPI->GetServiceConsumer(path, &consumer) != kVIA_OK)
      return CCL_INVALIDCONSUMERPATH;

    return gModule->mConsumers.Add(new VConsumer(consumer, true));
  }

  int VConsumer::CreateConsumer(VIAFbServiceConsumer* consumer, bool release)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mConsumers.Add(new VConsumer(consumer, release));
  }

  int VConsumer::LookupConsumer(cclConsumerID consumerID, VConsumer*& consumer)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    consumer = gModule->mConsumers.Get(consumerID);
    if (consumer == nullptr)
      return CCL_INVALIDCONSUMERID;

    return CCL_SUCCESS;
  }

  int VConsumer::RemoveConsumer(cclConsumerID consumerID)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VConsumer* consumer = gModule->mConsumers.Get(consumerID);
    if (consumer == nullptr)
      return CCL_INVALIDCONSUMERID;

    return gModule->mConsumers.Remove(consumerID) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VConsumer::SetProviderDiscoveredHandler(cclProviderDiscoveredHandler handler)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    mProviderDiscoveredHandler = handler;

    VIAResult rv = kVIA_OK;
    if (mProviderDiscoveredHandler != nullptr)
    {
      if (mCallbackHandle == nullptr)
        rv = mPort->RegisterObserver(this, &mCallbackHandle);
    }
    else
    {
      if (mCallbackHandle != nullptr)
        rv = mPort->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
    }
    return (rv == kVIA_OK) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  VIASTDDEF VConsumer::OnProviderDiscovered(VIAFbServiceConsumer* inConsumer, VIAFbAddressHandle* inAddress)
  {
    if (mProviderDiscoveredHandler != nullptr)
    {
      auto addressID = VAddress::CreateAddress(inAddress, false);
      if (addressID < 0)
        return kVIA_Failed;

      mProviderDiscoveredHandler(addressID);
      return VAddress::RemoveAddress(addressID);
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VProvider Implementation

  VProvider::VProvider(VIAFbServiceProvider* provider, bool release)
    : VPortBase(provider, release)
    , mProviderID(NDetail::cInvalidCCLObjectID)
    , mConnectionRequestedHandler(nullptr)
    , mConsumerDiscoveredHandler(nullptr)
    , mCallbackHandle(nullptr)
  {}

  VProvider::~VProvider()
  {
    if (mCallbackHandle != nullptr)
    {
      if (mPort != nullptr)
        mPort->UnregisterObserver(mCallbackHandle);

      mConnectionRequestedHandler = nullptr;
      mConsumerDiscoveredHandler = nullptr;
      mCallbackHandle = nullptr;
    }
  }

  int VProvider::CreateProvider(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbServiceProvider* provider = nullptr;
    if (fbAPI->GetServiceProvider(path, &provider) != kVIA_OK)
      return CCL_INVALIDPROVIDERPATH;

    return gModule->mProviders.Add(new VProvider(provider, true));
  }

  int VProvider::CreateProvider(VIAFbServiceProvider* provider, bool release)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mProviders.Add(new VProvider(provider, release));
  }

  int VProvider::LookupProvider(cclProviderID providerID, VProvider*& provider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    provider = gModule->mProviders.Get(providerID);
    if (provider == nullptr)
      return CCL_INVALIDPROVIDERID;

    return CCL_SUCCESS;
  }

  int VProvider::RemoveProvider(cclProviderID providerID)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VProvider* provider = gModule->mProviders.Get(providerID);
    if (provider == nullptr)
      return CCL_INVALIDPROVIDERID;

    return gModule->mProviders.Remove(providerID) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VProvider::UpdateRegistration()
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    VIAResult rv = kVIA_OK;
    if ((mConnectionRequestedHandler != nullptr) || (mConsumerDiscoveredHandler != nullptr))
    {
      if (mCallbackHandle == nullptr)
        rv = mPort->RegisterObserver(this, &mCallbackHandle);
    }
    else
    {
      if (mCallbackHandle != nullptr)
        rv = mPort->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
    }
    return (rv == kVIA_OK) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VProvider::SetConnectionRequestedHandler(cclConnectionRequestedHandler handler)
  {
    mConnectionRequestedHandler = handler;
    return UpdateRegistration();
  }

  int VProvider::SetConsumerDiscoveredHandler(cclConsumerDiscoveredHandler handler)
  {
    mConsumerDiscoveredHandler = handler;
    return UpdateRegistration();
  }

  VIASTDDEF VProvider::OnConsumerDiscovered(VIAFbServiceProvider* inProvider, VIAFbAddressHandle* inAddress)
  {
    if (mConsumerDiscoveredHandler != nullptr)
    {
      auto addressID = VAddress::CreateAddress(inAddress, false);
      if (addressID < 0)
        return kVIA_Failed;

      mConsumerDiscoveredHandler(addressID);
      return VAddress::RemoveAddress(addressID);
    }
    return kVIA_OK;
  }

  VIASTDDEF VProvider::OnConnectionRequested(VIAFbServiceProvider* inProvider, VIAFbServiceConsumer* inConsumer)
  {
    if (mConnectionRequestedHandler != nullptr)
    {
      cclConsumerID consumerID = VConsumer::CreateConsumer(inConsumer, false);
      if (consumerID < 0)
        return kVIA_Failed;

      mConnectionRequestedHandler(consumerID);
      return VConsumer::RemoveConsumer(consumerID);
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VConsumedService Implementation

  VConsumedService::VConsumedService(VIAFbConsumedService* consumedService)
    : VPortBase(consumedService, true)
    , mConsumedServiceID(NDetail::cInvalidCCLObjectID)
  {}

  VConsumedService::~VConsumedService()
  {
    for (auto* handler : mConnectionHandlerPool) delete handler;
    for (auto* handler : mActiveConnectionHandlers) delete handler;
  }

  int VConsumedService::CreateConsumedService(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbConsumedService* consumedService = nullptr;
    if (fbAPI->GetConsumedService(path, &consumedService) != kVIA_OK)
      return CCL_INVALIDCONSUMEDSERVICEPATH;

    return gModule->mConsumedServices.Add(new VConsumedService(consumedService));
  }

  int VConsumedService::CreateConsumedService(VIAFbConsumedService* consumedService)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mConsumedServices.Add(new VConsumedService(consumedService));
  }

  int VConsumedService::LookupConsumedService(cclConsumedServiceID consumedServiceID, VConsumedService*& consumedService)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    consumedService = gModule->mConsumedServices.Get(consumedServiceID);
    if (consumedService == nullptr)
      return CCL_INVALIDCONSUMEDSERVICEID;

    return CCL_SUCCESS;
  }

  int VConsumedService::ConnectAsync(cclConnectionEstablishedHandler successCallback, cclConnectionFailedHandler failureCallback)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mConnectionHandlerPool.empty())
      mConnectionHandlerPool.push_back(new ConnectionHandler());

    ConnectionHandler* handler = mConnectionHandlerPool.back();

    handler->mParent = this;
    handler->mSuccessCallback = successCallback;
    handler->mFailureCallback = failureCallback;

    mConnectionHandlerPool.pop_back();
    mActiveConnectionHandlers.insert(handler);

    if (mPort->ConnectAsync(handler) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  VIASTDDEF VConsumedService::ConnectionHandler::OnConnectionEstablished(VIAFbConsumedService* inPort)
  {
    if (mSuccessCallback != nullptr) mSuccessCallback();
    mSuccessCallback = nullptr;
    mFailureCallback = nullptr;

    if (mParent != nullptr)
    {
      mParent->mActiveConnectionHandlers.erase(this);
      mParent->mConnectionHandlerPool.push_back(this);
      mParent = nullptr;
    }
    return kVIA_OK;
  }

  VIASTDDEF VConsumedService::ConnectionHandler::OnConnectionFailed(VIAFbConsumedService* inPort, const char* inError)
  {
    if (mFailureCallback != nullptr) mFailureCallback(inError);
    mSuccessCallback = nullptr;
    mFailureCallback = nullptr;

    if (mParent != nullptr)
    {
      mParent->mActiveConnectionHandlers.erase(this);
      mParent->mConnectionHandlerPool.push_back(this);
      mParent = nullptr;
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VProvidedService Implementation

  VProvidedService::VProvidedService(VIAFbProvidedService* providedService)
    : VPortBase(providedService, true)
    , mProvidedServiceID(NDetail::cInvalidCCLObjectID)
    , mSimulatorCallback(nullptr)
    , mSimulatorHandle(nullptr)
  {}

  VProvidedService::~VProvidedService()
  {
    if (mSimulatorHandle != nullptr)
    {
      if (mPort != nullptr)
        mPort->UnregisterSimulator(mSimulatorHandle);

      mSimulatorHandle = nullptr;
      mSimulatorCallback = nullptr;
    }

    for (auto* handler : mConnectionHandlerPool) delete handler;
    for (auto* handler : mActiveConnectionHandlers) delete handler;
  }

  int VProvidedService::CreateProvidedService(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbProvidedService* providedService = nullptr;
    if (fbAPI->GetProvidedService(path, &providedService) != kVIA_OK)
      return CCL_INVALIDPROVIDEDSERVICEPATH;

    return gModule->mProvidedServices.Add(new VProvidedService(providedService));
  }

  int VProvidedService::CreateProvidedService(VIAFbProvidedService* providedService)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mProvidedServices.Add(new VProvidedService(providedService));
  }

  int VProvidedService::LookupProvidedService(cclProvidedServiceID providedServiceID, VProvidedService*& providedService)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    providedService = gModule->mProvidedServices.Get(providedServiceID);
    if (providedService == nullptr)
      return CCL_INVALIDPROVIDEDSERVICEID;

    return CCL_SUCCESS;
  }

  int VProvidedService::SetSimulator(cclCallHandler simulator)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mSimulatorHandle != nullptr)
    {
      auto rc = mPort->UnregisterSimulator(mSimulatorHandle);
      mSimulatorHandle = nullptr;
      mSimulatorCallback = nullptr;
      if (rc != kVIA_OK)
        return CCL_INTERNALERROR;
    }

    mSimulatorCallback = simulator;

    if (mSimulatorCallback != nullptr)
    {
      if (mPort->RegisterSimulator(this, &mSimulatorHandle) != kVIA_OK)
      {
        mSimulatorHandle = nullptr;
        mSimulatorCallback = nullptr;
        return CCL_INTERNALERROR;
      }
    }

    return CCL_SUCCESS;
  }

  cclFunctionID VProvidedService::FindFunction(VIAFbCallContext* callContext)
  {
    static char buffer[4096];
    if (callContext->GetServerPort(buffer, 4096) != kVIA_OK)
      return CCL_INTERNALERROR;

    std::string serverPort(buffer);
    auto it = mFunctionMap.find(serverPort);
    if (it != mFunctionMap.end()) return it->second;

    // first call state notification for this function, create new function
    cclFunctionID functionID = VServerFunction::CreateServerFunction(serverPort.c_str());
    if (functionID >= 0) mFunctionMap[serverPort] = functionID;
    return functionID;
  }

  VIASTDDEF VProvidedService::OnCallStateChanged(VIATime inTime, VIAFbCallContext* inCallContext, VIAFbFunctionCallState inCallState, const VIAFbFunctionServerPort* inServerPort)
  {
    if (inCallState != VIAFbFunctionCallState::eCalled)
      return kVIA_OK;

    if (mSimulatorCallback == nullptr)
      return kVIA_OK;

    auto funcID = FindFunction(inCallContext);
    if (funcID == NDetail::cInvalidCCLObjectID)
      return kVIA_Failed;

    auto ctxtID = VFunctionUtils::CreateCallContext(inCallContext, funcID, false);
    if (ctxtID == NDetail::cInvalidCCLObjectID)
      return kVIA_Failed;

    mSimulatorCallback(static_cast<cclTime>(inTime), ctxtID);

    return VFunctionUtils::DestroyCallContext(ctxtID) ? kVIA_OK : kVIA_Failed;
  }

  int VProvidedService::ConnectAsync(cclConnectionEstablishedHandler successCallback, cclConnectionFailedHandler failureCallback)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    if (mConnectionHandlerPool.empty())
      mConnectionHandlerPool.push_back(new ConnectionHandler());

    ConnectionHandler* handler = mConnectionHandlerPool.back();

    handler->mParent = this;
    handler->mSuccessCallback = successCallback;
    handler->mFailureCallback = failureCallback;

    mConnectionHandlerPool.pop_back();
    mActiveConnectionHandlers.insert(handler);

    if (mPort->ConnectAsync(handler) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  VIASTDDEF VProvidedService::ConnectionHandler::OnConnectionEstablished(VIAFbProvidedService* inPort)
  {
    if (mSuccessCallback != nullptr) mSuccessCallback();
    mSuccessCallback = nullptr;
    mFailureCallback = nullptr;

    if (mParent != nullptr)
    {
      mParent->mActiveConnectionHandlers.erase(this);
      mParent->mConnectionHandlerPool.push_back(this);
      mParent = nullptr;
    }
    return kVIA_OK;
  }

  VIASTDDEF VProvidedService::ConnectionHandler::OnConnectionFailed(VIAFbProvidedService* inPort, const char* inError)
  {
    if (mFailureCallback != nullptr) mFailureCallback(inError);
    mSuccessCallback = nullptr;
    mFailureCallback = nullptr;

    if (mParent != nullptr)
    {
      mParent->mActiveConnectionHandlers.erase(this);
      mParent->mConnectionHandlerPool.push_back(this);
      mParent = nullptr;
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VAddress Implementation

  VAddress::VAddress(VIAFbAddressHandle* address, bool release)
    : mAddressID(NDetail::cInvalidCCLObjectID)
    , mAddress(address)
    , mRelease(release)
  {}

  VAddress::~VAddress()
  {
    if (mAddress && mRelease) mAddress->Release();
  }

  cclAddressID VAddress::CreateAddress(VIAFbAddressHandle* address, bool release)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mAddresses.Add(new VAddress(address, release));
  }

  int VAddress::LookupAddress(cclAddressID addressID, VAddress*& address)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    address = gModule->mAddresses.Get(addressID);
    if (address == nullptr)
      return CCL_INVALIDADDRESSID;

    return CCL_SUCCESS;
  }

  int VAddress::RemoveAddress(cclAddressID addressID)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VAddress* address = gModule->mAddresses.Get(addressID);
    if (address == nullptr)
      return CCL_INVALIDADDRESSID;

    return gModule->mAddresses.Remove(addressID) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  #pragma endregion

  #pragma region VPDUSender Implementation

  // base class for generic PDU Sender Port functionality
  VPDUSender::VPDUSender(VIAFbPDUSenderPort* pduSender, bool release)
    : VPortBase(pduSender, release)
    , mPDUSenderID(NDetail::cInvalidCCLObjectID)
  {}

  VPDUSender::~VPDUSender()
  {}

  int VPDUSender::CreatePDUSender(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbPDUSenderPort* pduSender = nullptr;
    if (fbAPI->GetPDUSenderPort(path, &pduSender) != kVIA_OK)
      return CCL_INVALIDPDUSENDERPATH;

    return gModule->mPDUSender.Add(new VPDUSender(pduSender, true));
  }

  int VPDUSender::LookupPDUSender(cclPDUSenderID pduSenderID, VPDUSender*& pduSender)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    pduSender = gModule->mPDUSender.Get(pduSenderID);
    if (pduSender == nullptr)
      return CCL_INVALIDPDUSENDERID;

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VPDUReceiver Implementation

  // base class for generic PDU Receiver Port functionality
  VPDUReceiver::VPDUReceiver(VIAFbPDUReceiverPort* pduReceiver)
    : VPortBase(pduReceiver, true)
    , mPDUReceiverID(NDetail::cInvalidCCLObjectID)
  {}

  VPDUReceiver::~VPDUReceiver()
  {}

  int VPDUReceiver::CreatePDUReceiver(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbPDUReceiverPort* pduReceiver = nullptr;
    if (fbAPI->GetPDUReceiverPort(path, &pduReceiver) != kVIA_OK)
      return CCL_INVALIDPDURECEIVERPATH;

    return gModule->mPDUReceiver.Add(new VPDUReceiver(pduReceiver));
  }

  int VPDUReceiver::LookupPDUReceiver(cclPDUReceiverID pduReceiverID, VPDUReceiver*& pduReceiver)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    pduReceiver = gModule->mPDUReceiver.Get(pduReceiverID);
    if (pduReceiver == nullptr)
      return CCL_INVALIDPDURECEIVERID;

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VConsumedServicePDU Implementation

  // base class for generic PDU functionality
  VConsumedServicePDU::VConsumedServicePDU(VIAFbConsumedServicePDU* consumedPDU)
    : VPDUReceiver(consumedPDU)
    , mConsumedPDU(consumedPDU)
  {}

  VConsumedServicePDU::~VConsumedServicePDU()
  {}

  int VConsumedServicePDU::CreateConsumedPDU(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbConsumedServicePDU* consumedPDU = nullptr;
    if (fbAPI->GetConsumedServicePDU(path, &consumedPDU) != kVIA_OK)
      return CCL_INVALIDCONSUMEDPDUPATH;

    return gModule->mPDUReceiver.Add(new VConsumedServicePDU(consumedPDU));
  }

  int VConsumedServicePDU::CreateConsumedPDU(VIAFbConsumedServicePDU* consumedPDU)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mPDUReceiver.Add(new VConsumedServicePDU(consumedPDU));
  }

  int VConsumedServicePDU::LookupConsumedPDU(cclPDUReceiverID consumedPDUID, VConsumedServicePDU*& consumedPDU)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    auto* pduReceiver = gModule->mPDUReceiver.Get(consumedPDUID);
    if (pduReceiver == nullptr)
      return CCL_INVALIDCONSUMEDPDUID;

    consumedPDU = dynamic_cast<VConsumedServicePDU*>(pduReceiver);
    if (consumedPDU == nullptr)
      return CCL_INVALIDCONSUMEDPDUID;

    return CCL_SUCCESS;
  }

  int VConsumedServicePDU::SetSubscriptionStateIsolated(bool subscribed)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    auto state = subscribed ? VIAFbSubscriptionState::eSubscribed : VIAFbSubscriptionState::eSubscribable;
    if (mConsumedPDU->SetSubscriptionStateIsolated(state) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  // abstract binding specific extensions
  VAbstractConsumedServicePDU::VAbstractConsumedServicePDU(VIAFbAbstractConsumedServicePDU* abstractConsumedPDU)
    : VConsumedServicePDU(abstractConsumedPDU)
    , mAbstractConsumedPDU(abstractConsumedPDU)
  {}

  VAbstractConsumedServicePDU::~VAbstractConsumedServicePDU()
  {}

  int VAbstractConsumedServicePDU::CreateConsumedPDU(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbAbstractBinding* abstractBinding;
    if (fbAPI->GetAbstractBinding(&abstractBinding) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbAbstractConsumedServicePDU* consumedPDU = nullptr;
    if (abstractBinding->GetConsumedServicePDU(path, &consumedPDU) != kVIA_OK)
      return CCL_INVALIDCONSUMEDPDUPATH;

    return gModule->mPDUReceiver.Add(new VAbstractConsumedServicePDU(consumedPDU));
  }

  int VAbstractConsumedServicePDU::LookupConsumedPDU(cclPDUReceiverID consumedPDUID, VAbstractConsumedServicePDU*& consumedPDU)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VPDUReceiver* pduReceiver = gModule->mPDUReceiver.Get(consumedPDUID);
    if (pduReceiver == nullptr)
      return CCL_INVALIDCONSUMEDPDUID;

    consumedPDU = dynamic_cast<VAbstractConsumedServicePDU*>(pduReceiver);
    if (consumedPDU == nullptr)
      return CCL_WRONGBINDING;

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VProvidedServicePDU Implementation

  VProvidedServicePDU::VProvidedServicePDU(VIAFbProvidedServicePDU* providedPDU, bool release)
    : VPDUSender(providedPDU, release)
    , mProvidedPDU(providedPDU)
  {}

  VProvidedServicePDU::~VProvidedServicePDU()
  {}

  int VProvidedServicePDU::CreateProvidedPDU(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbProvidedServicePDU* providedPDU = nullptr;
    if (fbAPI->GetProvidedServicePDU(path, &providedPDU) != kVIA_OK)
      return CCL_INVALIDPROVIDEDPDUPATH;

    return gModule->mPDUSender.Add(new VProvidedServicePDU(providedPDU, true));
  }

  int VProvidedServicePDU::CreateProvidedPDU(VIAFbProvidedServicePDU* providedPDU, bool release)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mPDUSender.Add(new VProvidedServicePDU(providedPDU, release));
  }

  int VProvidedServicePDU::LookupProvidedPDU(cclPDUSenderID providedPDUID, VProvidedServicePDU*& providedPDU)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    auto* pduSender = gModule->mPDUSender.Get(providedPDUID);
    if (pduSender == nullptr)
      return CCL_INVALIDPROVIDEDPDUID;

    providedPDU = dynamic_cast<VProvidedServicePDU*>(pduSender);
    if (providedPDU == nullptr)
      return CCL_INVALIDPROVIDEDPDUID;

    return CCL_SUCCESS;
  }

  int VProvidedServicePDU::RemoveProvidedPDU(cclPDUSenderID providedPDUID)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VPDUSender* providedPDU = gModule->mPDUSender.Get(providedPDUID);
    if (providedPDU == nullptr)
      return CCL_INVALIDPROVIDEDPDUID;

    return gModule->mPDUSender.Remove(providedPDUID) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VProvidedServicePDU::SetSubscriptionStateIsolated(bool subscribed)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    auto state = subscribed ? VIAFbSubscriptionState::eSubscribed : VIAFbSubscriptionState::eSubscribable;
    if (mProvidedPDU->SetSubscriptionStateIsolated(state) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VServicePDUProvider Implementation

  // base class for generic PDU functionality
  VServicePDUProvider::VServicePDUProvider(VIAFbServicePDUProvider* pduProvider)
    : VPortBase(pduProvider, true)
    , mPDUProviderID(NDetail::cInvalidCCLObjectID)
  {}

  VServicePDUProvider::~VServicePDUProvider()
  {}

  int VServicePDUProvider::CreatePDUProvider(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbServicePDUProvider* pduProvider = nullptr;
    if (fbAPI->GetServicePDUProvider(path, &pduProvider) != kVIA_OK)
      return CCL_INVALIDPDUPROVIDERPATH;

    return gModule->mPDUProviders.Add(new VServicePDUProvider(pduProvider));
  }

  int VServicePDUProvider::CreatePDUProvider(VIAFbServicePDUProvider* pduProvider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mPDUProviders.Add(new VServicePDUProvider(pduProvider));
  }

  int VServicePDUProvider::LookupPDUProvider(cclPDUProviderID pduProviderID, VServicePDUProvider*& pduProvider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    pduProvider = gModule->mPDUProviders.Get(pduProviderID);
    if (pduProvider == nullptr)
      return CCL_INVALIDPDUPROVIDERID;

    return CCL_SUCCESS;
  }

  int VServicePDUProvider::GetNrOfSubscribedConsumers(int* nrOfConsumers)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbServiceConsumerIterator> it;
    if (mPort->GetSubscribedConsumers(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    *nrOfConsumers = 0;
    while (it.mVIAObject->HasMoreConsumers())
    {
      ++(*nrOfConsumers);
      it.mVIAObject->SkipConsumer();
    }

    return CCL_SUCCESS;
  }

  int VServicePDUProvider::GetSubscribedConsumers(cclConsumerID* consumerBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbServiceConsumerIterator> it;
    if (mPort->GetSubscribedConsumers(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclConsumerID* next = consumerBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMoreConsumers())
      {
        *bufferSize = i;
        break;
      }

      VIAFbServiceConsumer* consumer;
      if (it.mVIAObject->GetNextConsumer(&consumer) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VConsumer::CreateConsumer(consumer, true);
    }

    return it.mVIAObject->HasMoreConsumers() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  // abstract binding specific extensions
  VAbstractServicePDUProvider::VAbstractServicePDUProvider(VIAFbAbstractServicePDUProvider* abstractPDUProvider)
    : VServicePDUProvider(abstractPDUProvider)
    , mAbstractPDUProvider(abstractPDUProvider)
    , mPDUSubscribedHandler(nullptr)
    , mPDUUnsubscribedHandler(nullptr)
    , mCallbackHandle(nullptr)
  {}

  VAbstractServicePDUProvider::~VAbstractServicePDUProvider()
  {
    if (mCallbackHandle != nullptr)
    {
      if (mAbstractPDUProvider != nullptr)
        mAbstractPDUProvider->UnregisterObserver(mCallbackHandle);

      mPDUSubscribedHandler = nullptr;
      mPDUUnsubscribedHandler = nullptr;
      mCallbackHandle = nullptr;
    }
  }

  int VAbstractServicePDUProvider::CreatePDUProvider(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbAbstractBinding* abstractBinding;
    if (fbAPI->GetAbstractBinding(&abstractBinding) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbAbstractServicePDUProvider* pduProvider = nullptr;
    if (abstractBinding->GetServicePDUProvider(path, &pduProvider) != kVIA_OK)
      return CCL_INVALIDPDUPROVIDERPATH;

    return gModule->mPDUProviders.Add(new VAbstractServicePDUProvider(pduProvider));
  }

  int VAbstractServicePDUProvider::LookupPDUProvider(cclPDUProviderID pduProviderID, VAbstractServicePDUProvider*& pduProvider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VServicePDUProvider* baseProvider = gModule->mPDUProviders.Get(pduProviderID);
    if (baseProvider == nullptr)
      return CCL_INVALIDPDUPROVIDERID;

    pduProvider = dynamic_cast<VAbstractServicePDUProvider*>(baseProvider);
    if (pduProvider == nullptr)
      return CCL_WRONGBINDING;

    return CCL_SUCCESS;
  }

  int VAbstractServicePDUProvider::UpdateRegistration()
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    VIAResult rv = kVIA_OK;
    if ((mPDUSubscribedHandler != nullptr) || (mPDUUnsubscribedHandler != nullptr))
    {
      if (mCallbackHandle == nullptr)
        rv = mAbstractPDUProvider->RegisterObserver(this, &mCallbackHandle);
    }
    else
    {
      if (mCallbackHandle != nullptr)
        rv = mAbstractPDUProvider->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
    }
    return (rv == kVIA_OK) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VAbstractServicePDUProvider::SetPDUSubscribedHandler(cclAbstractPDUSubscriptionHandler handler)
  {
    mPDUSubscribedHandler = handler;
    return UpdateRegistration();
  }

  int VAbstractServicePDUProvider::SetPDUUnsubscribedHandler(cclAbstractPDUSubscriptionHandler handler)
  {
    mPDUUnsubscribedHandler = handler;
    return UpdateRegistration();
  }

  VIASTDDEF VAbstractServicePDUProvider::OnPDUSubscribed(VIAFbProvidedServicePDU* inPDUPort)
  {
    if (mPDUSubscribedHandler != nullptr)
    {
      auto portID = VProvidedServicePDU::CreateProvidedPDU(inPDUPort, false);
      if (portID < 0)
        return kVIA_Failed;

      mPDUSubscribedHandler(portID);
      return VProvidedServicePDU::RemoveProvidedPDU(portID);
    }
    return kVIA_OK;
  }

  VIASTDDEF VAbstractServicePDUProvider::OnPDUUnsubscribed(VIAFbProvidedServicePDU* inPDUPort)
  {
    if (mPDUUnsubscribedHandler != nullptr)
    {
      auto portID = VProvidedServicePDU::CreateProvidedPDU(inPDUPort, false);
      if (portID < 0)
        return kVIA_Failed;

      mPDUUnsubscribedHandler(portID);
      return VProvidedServicePDU::RemoveProvidedPDU(portID);
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VConsumedEvent Implementation

  // base class for generic event functionality
  VConsumedEvent::VConsumedEvent(VIAFbConsumedEvent* consumedEvent)
    : VPortBase(consumedEvent, true)
    , mConsumedEventID(NDetail::cInvalidCCLObjectID)
  {}

  VConsumedEvent::~VConsumedEvent()
  {}

  int VConsumedEvent::CreateConsumedEvent(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbConsumedEvent* consumedEvent = nullptr;
    if (fbAPI->GetConsumedEvent(path, &consumedEvent) != kVIA_OK)
      return CCL_INVALIDCONSUMEDEVENTPATH;

    return gModule->mConsumedEvents.Add(new VConsumedEvent(consumedEvent));
  }

  int VConsumedEvent::CreateConsumedEvent(VIAFbConsumedEvent* consumedEvent)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mConsumedEvents.Add(new VConsumedEvent(consumedEvent));
  }

  int VConsumedEvent::LookupConsumedEvent(cclConsumedEventID consumedEventID, VConsumedEvent*& consumedEvent)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    consumedEvent = gModule->mConsumedEvents.Get(consumedEventID);
    if (consumedEvent == nullptr)
      return CCL_INVALIDCONSUMEDEVENTID;

    return CCL_SUCCESS;
  }

  int VConsumedEvent::SetSubscriptionStateIsolated(bool subscribed)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    auto state = subscribed ? VIAFbSubscriptionState::eSubscribed : VIAFbSubscriptionState::eSubscribable;
    if (mPort->SetSubscriptionStateIsolated(state) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  // abstract binding specific extensions
  VAbstractConsumedEvent::VAbstractConsumedEvent(VIAFbAbstractConsumedEvent* abstractConsumedEvent)
    : VConsumedEvent(abstractConsumedEvent)
    , mAbstractConsumedEvent(abstractConsumedEvent)
  {}

  VAbstractConsumedEvent::~VAbstractConsumedEvent()
  {}

  int VAbstractConsumedEvent::CreateConsumedEvent(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbAbstractBinding* abstractBinding;
    if (fbAPI->GetAbstractBinding(&abstractBinding) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbAbstractConsumedEvent* consumedEvent = nullptr;
    if (abstractBinding->GetConsumedEvent(path, &consumedEvent) != kVIA_OK)
      return CCL_INVALIDCONSUMEDEVENTPATH;

    return gModule->mConsumedEvents.Add(new VAbstractConsumedEvent(consumedEvent));
  }

  int VAbstractConsumedEvent::LookupConsumedEvent(cclConsumedEventID consumedEventID, VAbstractConsumedEvent*& consumedEvent)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VConsumedEvent* baseEvent = gModule->mConsumedEvents.Get(consumedEventID);
    if (baseEvent == nullptr)
      return CCL_INVALIDCONSUMEDEVENTID;

    consumedEvent = dynamic_cast<VAbstractConsumedEvent*>(baseEvent);
    if (consumedEvent == nullptr)
      return CCL_WRONGBINDING;

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VProvidedEvent Implementation

  VProvidedEvent::VProvidedEvent(VIAFbProvidedEvent* providedEvent, bool release)
    : VPortBase(providedEvent, release)
    , mProvidedEventID(NDetail::cInvalidCCLObjectID)
  {}

  VProvidedEvent::~VProvidedEvent()
  {}

  int VProvidedEvent::CreateProvidedEvent(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbProvidedEvent* providedEvent = nullptr;
    if (fbAPI->GetProvidedEvent(path, &providedEvent) != kVIA_OK)
      return CCL_INVALIDPROVIDEDEVENTPATH;

    return gModule->mProvidedEvents.Add(new VProvidedEvent(providedEvent, true));
  }

  int VProvidedEvent::CreateProvidedEvent(VIAFbProvidedEvent* providedEvent, bool release)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mProvidedEvents.Add(new VProvidedEvent(providedEvent, release));
  }

  int VProvidedEvent::LookupProvidedEvent(cclProvidedEventID providedEventID, VProvidedEvent*& providedEvent)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    providedEvent = gModule->mProvidedEvents.Get(providedEventID);
    if (providedEvent == nullptr)
      return CCL_INVALIDPROVIDEDEVENTID;

    return CCL_SUCCESS;
  }

  int VProvidedEvent::RemoveProvidedEvent(cclProvidedEventID providedEventID)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VProvidedEvent* providedEvent = gModule->mProvidedEvents.Get(providedEventID);
    if (providedEvent == nullptr)
      return CCL_INVALIDPROVIDEDEVENTID;

    return gModule->mProvidedEvents.Remove(providedEventID) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VProvidedEvent::SetSubscriptionStateIsolated(bool subscribed)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    auto state = subscribed ? VIAFbSubscriptionState::eSubscribed : VIAFbSubscriptionState::eSubscribable;
    if (mPort->SetSubscriptionStateIsolated(state) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VEventProvider Implementation

  // base class for generic event functionality
  VEventProvider::VEventProvider(VIAFbEventProvider* eventProvider)
    : VPortBase(eventProvider, true)
    , mEventProviderID(NDetail::cInvalidCCLObjectID)
  {}

  VEventProvider::~VEventProvider()
  {}

  int VEventProvider::CreateEventProvider(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbEventProvider* eventProvider = nullptr;
    if (fbAPI->GetEventProvider(path, &eventProvider) != kVIA_OK)
      return CCL_INVALIDEVENTPROVIDERPATH;

    return gModule->mEventProviders.Add(new VEventProvider(eventProvider));
  }

  int VEventProvider::CreateEventProvider(VIAFbEventProvider* eventProvider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mEventProviders.Add(new VEventProvider(eventProvider));
  }

  int VEventProvider::LookupEventProvider(cclEventProviderID eventProviderID, VEventProvider*& eventProvider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    eventProvider = gModule->mEventProviders.Get(eventProviderID);
    if (eventProvider == nullptr)
      return CCL_INVALIDEVENTPROVIDERID;

    return CCL_SUCCESS;
  }

  int VEventProvider::GetNrOfSubscribedConsumers(int* nrOfConsumers)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbServiceConsumerIterator> it;
    if (mPort->GetSubscribedConsumers(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    *nrOfConsumers = 0;
    while (it.mVIAObject->HasMoreConsumers())
    {
      ++(*nrOfConsumers);
      it.mVIAObject->SkipConsumer();
    }

    return CCL_SUCCESS;
  }

  int VEventProvider::GetSubscribedConsumers(cclConsumerID* consumerBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbServiceConsumerIterator> it;
    if (mPort->GetSubscribedConsumers(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclConsumerID* next = consumerBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMoreConsumers())
      {
        *bufferSize = i;
        break;
      }

      VIAFbServiceConsumer* consumer;
      if (it.mVIAObject->GetNextConsumer(&consumer) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VConsumer::CreateConsumer(consumer, true);
    }

    return it.mVIAObject->HasMoreConsumers() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  // abstract binding specific extensions
  VAbstractEventProvider::VAbstractEventProvider(VIAFbAbstractEventProvider* abstractEventProvider)
    : VEventProvider(abstractEventProvider)
    , mAbstractEventProvider(abstractEventProvider)
    , mEventSubscribedHandler(nullptr)
    , mEventUnsubscribedHandler(nullptr)
    , mCallbackHandle(nullptr)
  {}

  VAbstractEventProvider::~VAbstractEventProvider()
  {
    if (mCallbackHandle != nullptr)
    {
      if (mAbstractEventProvider != nullptr)
        mAbstractEventProvider->UnregisterObserver(mCallbackHandle);

      mEventSubscribedHandler = nullptr;
      mEventUnsubscribedHandler = nullptr;
      mCallbackHandle = nullptr;
    }
  }

  int VAbstractEventProvider::CreateEventProvider(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbAbstractBinding* abstractBinding;
    if (fbAPI->GetAbstractBinding(&abstractBinding) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbAbstractEventProvider* eventProvider = nullptr;
    if (abstractBinding->GetEventProvider(path, &eventProvider) != kVIA_OK)
      return CCL_INVALIDEVENTPROVIDERPATH;

    return gModule->mEventProviders.Add(new VAbstractEventProvider(eventProvider));
  }

  int VAbstractEventProvider::LookupEventProvider(cclEventProviderID eventProviderID, VAbstractEventProvider*& eventProvider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VEventProvider* baseProvider = gModule->mEventProviders.Get(eventProviderID);
    if (baseProvider == nullptr)
      return CCL_INVALIDEVENTPROVIDERID;

    eventProvider = dynamic_cast<VAbstractEventProvider*>(baseProvider);
    if (eventProvider == nullptr)
      return CCL_WRONGBINDING;

    return CCL_SUCCESS;
  }

  int VAbstractEventProvider::UpdateRegistration()
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    VIAResult rv = kVIA_OK;
    if ((mEventSubscribedHandler != nullptr) || (mEventUnsubscribedHandler != nullptr))
    {
      if (mCallbackHandle == nullptr)
        rv = mAbstractEventProvider->RegisterObserver(this, &mCallbackHandle);
    }
    else
    {
      if (mCallbackHandle != nullptr)
        rv = mAbstractEventProvider->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
    }
    return (rv == kVIA_OK) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VAbstractEventProvider::SetEventSubscribedHandler(cclAbstractEventSubscriptionHandler handler)
  {
    mEventSubscribedHandler = handler;
    return UpdateRegistration();
  }

  int VAbstractEventProvider::SetEventUnsubscribedHandler(cclAbstractEventSubscriptionHandler handler)
  {
    mEventUnsubscribedHandler = handler;
    return UpdateRegistration();
  }

  VIASTDDEF VAbstractEventProvider::OnEventSubscribed(VIAFbProvidedEvent* inEventPort)
  {
    if (mEventSubscribedHandler != nullptr)
    {
      auto portID = VProvidedEvent::CreateProvidedEvent(inEventPort, false);
      if (portID < 0)
        return kVIA_Failed;

      mEventSubscribedHandler(portID);
      return VProvidedEvent::RemoveProvidedEvent(portID);
    }
    return kVIA_OK;
  }

  VIASTDDEF VAbstractEventProvider::OnEventUnsubscribed(VIAFbProvidedEvent* inEventPort)
  {
    if (mEventUnsubscribedHandler != nullptr)
    {
      auto portID = VProvidedEvent::CreateProvidedEvent(inEventPort, false);
      if (portID < 0)
        return kVIA_Failed;

      mEventUnsubscribedHandler(portID);
      return VProvidedEvent::RemoveProvidedEvent(portID);
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VConsumedField Implementation

  // base class for generic field functionality
  VConsumedField::VConsumedField(VIAFbConsumedField* consumedField)
    : VPortBase(consumedField, true)
    , mConsumedFieldID(NDetail::cInvalidCCLObjectID)
  {}

  VConsumedField::~VConsumedField()
  {}

  int VConsumedField::CreateConsumedField(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbConsumedField* consumedField = nullptr;
    auto err = fbAPI->GetConsumedField(path, &consumedField);
    if (err == kVIA_ObjectInvalid)
      return CCL_FIELDNOTSUBSCRIBABLE;
    if (err != kVIA_OK)
      return CCL_INVALIDCONSUMEDFIELDPATH;

    return gModule->mConsumedFields.Add(new VConsumedField(consumedField));
  }

  int VConsumedField::CreateConsumedField(VIAFbConsumedField* consumedField)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mConsumedFields.Add(new VConsumedField(consumedField));
  }

  int VConsumedField::LookupConsumedField(cclConsumedFieldID consumedFieldID, VConsumedField*& consumedField)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    consumedField = gModule->mConsumedFields.Get(consumedFieldID);
    if (consumedField == nullptr)
      return CCL_INVALIDCONSUMEDFIELDID;

    return CCL_SUCCESS;
  }

  int VConsumedField::SetSubscriptionStateIsolated(bool subscribed)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    auto state = subscribed ? VIAFbSubscriptionState::eSubscribed : VIAFbSubscriptionState::eSubscribable;
    if (mPort->SetSubscriptionStateIsolated(state) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  // abstract binding specific extensions
  VAbstractConsumedField::VAbstractConsumedField(VIAFbAbstractConsumedField* abstractConsumedField)
    : VConsumedField(abstractConsumedField)
    , mAbstractConsumedField(abstractConsumedField)
  {}

  VAbstractConsumedField::~VAbstractConsumedField()
  {}

  int VAbstractConsumedField::CreateConsumedField(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbAbstractBinding* abstractBinding;
    if (fbAPI->GetAbstractBinding(&abstractBinding) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbAbstractConsumedField* consumedField = nullptr;
    auto err = abstractBinding->GetConsumedField(path, &consumedField);
    if (err == kVIA_ObjectInvalid)
      return CCL_FIELDNOTSUBSCRIBABLE;
    if (err != kVIA_OK)
      return CCL_INVALIDCONSUMEDFIELDPATH;

    return gModule->mConsumedFields.Add(new VAbstractConsumedField(consumedField));
  }

  int VAbstractConsumedField::LookupConsumedField(cclConsumedFieldID consumedFieldID, VAbstractConsumedField*& consumedField)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VConsumedField* baseField = gModule->mConsumedFields.Get(consumedFieldID);
    if (baseField == nullptr)
      return CCL_INVALIDCONSUMEDFIELDID;

    consumedField = dynamic_cast<VAbstractConsumedField*>(baseField);
    if (consumedField == nullptr)
      return CCL_WRONGBINDING;

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VProvidedField Implementation

  VProvidedField::VProvidedField(VIAFbProvidedField* providedField, bool release)
    : VPortBase(providedField, release)
    , mProvidedFieldID(NDetail::cInvalidCCLObjectID)
  {}

  VProvidedField::~VProvidedField()
  {}

  int VProvidedField::CreateProvidedField(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbProvidedField* providedField = nullptr;
    auto err = fbAPI->GetProvidedField(path, &providedField);
    if (err == kVIA_ObjectInvalid)
      return CCL_FIELDNOTSUBSCRIBABLE;
    if (err != kVIA_OK)
      return CCL_INVALIDPROVIDEDFIELDPATH;

    return gModule->mProvidedFields.Add(new VProvidedField(providedField, true));
  }

  int VProvidedField::CreateProvidedField(VIAFbProvidedField* providedField, bool release)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mProvidedFields.Add(new VProvidedField(providedField, release));
  }

  int VProvidedField::LookupProvidedField(cclProvidedFieldID providedFieldID, VProvidedField*& providedField)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    providedField = gModule->mProvidedFields.Get(providedFieldID);
    if (providedField == nullptr)
      return CCL_INVALIDPROVIDEDFIELDID;

    return CCL_SUCCESS;
  }

  int VProvidedField::RemoveProvidedField(cclProvidedFieldID providedFieldID)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VProvidedField* providedField = gModule->mProvidedFields.Get(providedFieldID);
    if (providedField == nullptr)
      return CCL_INVALIDPROVIDEDFIELDID;

    return gModule->mProvidedFields.Remove(providedFieldID) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VProvidedField::SetSubscriptionStateIsolated(bool subscribed)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    auto state = subscribed ? VIAFbSubscriptionState::eSubscribed : VIAFbSubscriptionState::eSubscribable;
    if (mPort->SetSubscriptionStateIsolated(state) != kVIA_OK)
      return CCL_INTERNALERROR;

    return CCL_SUCCESS;
  }

  #pragma endregion

  #pragma region VFieldProvider Implementation

  // base class for generic field functionality
  VFieldProvider::VFieldProvider(VIAFbFieldProvider* fieldProvider)
    : VPortBase(fieldProvider, true)
    , mFieldProviderID(NDetail::cInvalidCCLObjectID)
  {}

  VFieldProvider::~VFieldProvider()
  {}

  int VFieldProvider::CreateFieldProvider(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbFieldProvider* fieldProvider = nullptr;
    auto err = fbAPI->GetFieldProvider(path, &fieldProvider);
    if (err == kVIA_ObjectInvalid)
      return CCL_FIELDNOTSUBSCRIBABLE;
    if (err != kVIA_OK)
      return CCL_INVALIDFIELDPROVIDERPATH;

    return gModule->mFieldProviders.Add(new VFieldProvider(fieldProvider));
  }

  int VFieldProvider::CreateFieldProvider(VIAFbFieldProvider* fieldProvider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mFieldProviders.Add(new VFieldProvider(fieldProvider));
  }

  int VFieldProvider::LookupFieldProvider(cclFieldProviderID fieldProviderID, VFieldProvider*& fieldProvider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    fieldProvider = gModule->mFieldProviders.Get(fieldProviderID);
    if (fieldProvider == nullptr)
      return CCL_INVALIDFIELDPROVIDERID;

    return CCL_SUCCESS;
  }

  int VFieldProvider::GetNrOfSubscribedConsumers(int* nrOfConsumers)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbServiceConsumerIterator> it;
    if (mPort->GetSubscribedConsumers(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    *nrOfConsumers = 0;
    while (it.mVIAObject->HasMoreConsumers())
    {
      ++(*nrOfConsumers);
      it.mVIAObject->SkipConsumer();
    }

    return CCL_SUCCESS;
  }

  int VFieldProvider::GetSubscribedConsumers(cclConsumerID* consumerBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbServiceConsumerIterator> it;
    if (mPort->GetSubscribedConsumers(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclConsumerID* next = consumerBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMoreConsumers())
      {
        *bufferSize = i;
        break;
      }

      VIAFbServiceConsumer* consumer;
      if (it.mVIAObject->GetNextConsumer(&consumer) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VConsumer::CreateConsumer(consumer, true);
    }

    return it.mVIAObject->HasMoreConsumers() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  // abstract binding specific extensions
  VAbstractFieldProvider::VAbstractFieldProvider(VIAFbAbstractFieldProvider* abstractFieldProvider)
    : VFieldProvider(abstractFieldProvider)
    , mAbstractFieldProvider(abstractFieldProvider)
    , mFieldSubscribedHandler(nullptr)
    , mFieldUnsubscribedHandler(nullptr)
    , mCallbackHandle(nullptr)
  {}

  VAbstractFieldProvider::~VAbstractFieldProvider()
  {
    if (mCallbackHandle != nullptr)
    {
      if (mAbstractFieldProvider != nullptr)
        mAbstractFieldProvider->UnregisterObserver(mCallbackHandle);

      mFieldSubscribedHandler = nullptr;
      mFieldUnsubscribedHandler = nullptr;
      mCallbackHandle = nullptr;
    }
  }

  int VAbstractFieldProvider::CreateFieldProvider(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbAbstractBinding* abstractBinding;
    if (fbAPI->GetAbstractBinding(&abstractBinding) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbAbstractFieldProvider* fieldProvider = nullptr;
    auto err = abstractBinding->GetFieldProvider(path, &fieldProvider);
    if (err == kVIA_ObjectInvalid)
      return CCL_FIELDNOTSUBSCRIBABLE;
    if (err != kVIA_OK)
      return CCL_INVALIDFIELDPROVIDERPATH;

    return gModule->mFieldProviders.Add(new VAbstractFieldProvider(fieldProvider));
  }

  int VAbstractFieldProvider::LookupFieldProvider(cclFieldProviderID fieldProviderID, VAbstractFieldProvider*& fieldProvider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VFieldProvider* baseProvider = gModule->mFieldProviders.Get(fieldProviderID);
    if (baseProvider == nullptr)
      return CCL_INVALIDFIELDPROVIDERID;

    fieldProvider = dynamic_cast<VAbstractFieldProvider*>(baseProvider);
    if (fieldProvider == nullptr)
      return CCL_WRONGBINDING;

    return CCL_SUCCESS;
  }

  int VAbstractFieldProvider::UpdateRegistration()
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    VIAResult rv = kVIA_OK;
    if ((mFieldSubscribedHandler != nullptr) || (mFieldUnsubscribedHandler != nullptr))
    {
      if (mCallbackHandle == nullptr)
        rv = mAbstractFieldProvider->RegisterObserver(this, &mCallbackHandle);
    }
    else
    {
      if (mCallbackHandle != nullptr)
        rv = mAbstractFieldProvider->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
    }
    return (rv == kVIA_OK) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VAbstractFieldProvider::SetFieldSubscribedHandler(cclAbstractFieldSubscriptionHandler handler)
  {
    mFieldSubscribedHandler = handler;
    return UpdateRegistration();
  }

  int VAbstractFieldProvider::SetFieldUnsubscribedHandler(cclAbstractFieldSubscriptionHandler handler)
  {
    mFieldUnsubscribedHandler = handler;
    return UpdateRegistration();
  }

  VIASTDDEF VAbstractFieldProvider::OnFieldSubscribed(VIAFbProvidedField* inFieldPort)
  {
    if (mFieldSubscribedHandler != nullptr)
    {
      auto portID = VProvidedField::CreateProvidedField(inFieldPort, false);
      if (portID < 0)
        return kVIA_Failed;

      mFieldSubscribedHandler(portID);
      return VProvidedField::RemoveProvidedField(portID);
    }
    return kVIA_OK;
  }

  VIASTDDEF VAbstractFieldProvider::OnFieldUnsubscribed(VIAFbProvidedField* inFieldPort)
  {
    if (mFieldUnsubscribedHandler != nullptr)
    {
      auto portID = VProvidedField::CreateProvidedField(inFieldPort, false);
      if (portID < 0)
        return kVIA_Failed;

      mFieldUnsubscribedHandler(portID);
      return VProvidedField::RemoveProvidedField(portID);
    }
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VConsumedEventGroup Implementation

  VConsumedEventGroup::VConsumedEventGroup(VIAFbSomeIPConsumedEventGroup* consumedEventGroup)
    : VPortBase(consumedEventGroup, true)
    , mConsumedEventGroupID(NDetail::cInvalidCCLObjectID)
  {
    consumedEventGroup->GetEventGroupId(&mEventGroupID);
  }

  VConsumedEventGroup::~VConsumedEventGroup()
  {}

  int VConsumedEventGroup::CreateConsumedEventGroup(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbSomeIPBinding* someIPBinding;
    if (fbAPI->GetSomeIPBinding(&someIPBinding) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbSomeIPConsumedEventGroup* consumedEventGroup = nullptr;
    if (someIPBinding->GetConsumedEventGroup(path, &consumedEventGroup) != kVIA_OK)
      return CCL_INVALIDCONSUMEDEVENTGROUPPATH;

    return gModule->mConsumedEventGroups.Add(new VConsumedEventGroup(consumedEventGroup));
  }

  int VConsumedEventGroup::LookupConsumedEventGroup(cclSomeIPConsumedEventGroupID consumedEventGroupID, VConsumedEventGroup*& consumedEventGroup)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    consumedEventGroup = gModule->mConsumedEventGroups.Get(consumedEventGroupID);
    if (consumedEventGroup == nullptr)
      return CCL_INVALIDCONSUMEDEVENTGROUPID;

    return CCL_SUCCESS;
  }

  int VConsumedEventGroup::GetEvents(cclConsumedEventID* eventBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbEventIterator<VIAFbConsumedEvent>> it;
    if (mPort->GetEvents(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclConsumedEventID* next = eventBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMoreEvents())
      {
        *bufferSize = i;
        break;
      }

      VIAFbConsumedEvent* consumedEvent;
      if (it.mVIAObject->GetNextEvent(&consumedEvent) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VConsumedEvent::CreateConsumedEvent(consumedEvent);
    }

    return it.mVIAObject->HasMoreEvents() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  int VConsumedEventGroup::GetPDUs(cclPDUReceiverID* pduBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbPDUIterator<VIAFbConsumedServicePDU>> it;
    if (mPort->GetPDUs(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclConsumedEventID* next = pduBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMorePDUs())
      {
        *bufferSize = i;
        break;
      }

      VIAFbConsumedServicePDU* consumedPDU;
      if (it.mVIAObject->GetNextPDU(&consumedPDU) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VConsumedServicePDU::CreateConsumedPDU(consumedPDU);
    }

    return it.mVIAObject->HasMorePDUs() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  int VConsumedEventGroup::GetFields(cclConsumedFieldID* fieldBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbFieldIterator<VIAFbConsumedField>> it;
    if (mPort->GetFields(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclConsumedFieldID* next = fieldBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMoreFields())
      {
        *bufferSize = i;
        break;
      }

      VIAFbConsumedField* consumedField;
      if (it.mVIAObject->GetNextField(&consumedField) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VConsumedField::CreateConsumedField(consumedField);
    }

    return it.mVIAObject->HasMoreFields() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  bool VConsumedEventGroup::IsEqual(const VPortBase<VIAFbSomeIPConsumedEventGroup>* other) const
  {
    unsigned long otherID;
    if ((other->mPort == nullptr) || (other->mPort->GetEventGroupId(&otherID) != kVIA_OK))
      return false;

    return VPortBase::IsEqual(other) && (otherID == mEventGroupID);
  }

  #pragma endregion

  #pragma region VProvidedEventGroup Implementation

  VProvidedEventGroup::VProvidedEventGroup(VIAFbSomeIPProvidedEventGroup* providedEventGroup, bool release)
    : VPortBase(providedEventGroup, release)
    , mProvidedEventGroupID(NDetail::cInvalidCCLObjectID)
  {
    providedEventGroup->GetEventGroupId(&mEventGroupID);
  }

  VProvidedEventGroup::~VProvidedEventGroup()
  {}

  int VProvidedEventGroup::CreateProvidedEventGroup(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbSomeIPBinding* someIPBinding;
    if (fbAPI->GetSomeIPBinding(&someIPBinding) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbSomeIPProvidedEventGroup* providedEventGroup = nullptr;
    if (someIPBinding->GetProvidedEventGroup(path, &providedEventGroup) != kVIA_OK)
      return CCL_INVALIDPROVIDEDEVENTGROUPPATH;

    return gModule->mProvidedEventGroups.Add(new VProvidedEventGroup(providedEventGroup, true));
  }

  int VProvidedEventGroup::CreateProvidedEventGroup(VIAFbSomeIPProvidedEventGroup* providedEventGroup)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    return gModule->mProvidedEventGroups.Add(new VProvidedEventGroup(providedEventGroup, false));
  }

  int VProvidedEventGroup::LookupProvidedEventGroup(cclSomeIPProvidedEventGroupID providedEventGroupID, VProvidedEventGroup*& providedEventGroup)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    providedEventGroup = gModule->mProvidedEventGroups.Get(providedEventGroupID);
    if (providedEventGroup == nullptr)
      return CCL_INVALIDPROVIDEDEVENTGROUPID;

    return CCL_SUCCESS;
  }

  int VProvidedEventGroup::RemoveProvidedEventGroup(cclSomeIPProvidedEventGroupID providedEventGroupID)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VProvidedEventGroup* providedEventGroup = gModule->mProvidedEventGroups.Get(providedEventGroupID);
    if (providedEventGroup == nullptr)
      return CCL_INVALIDPROVIDEDEVENTGROUPID;

    return gModule->mProvidedEventGroups.Remove(providedEventGroupID) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VProvidedEventGroup::GetEvents(cclProvidedEventID* eventBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbEventIterator<VIAFbProvidedEvent>> it;
    if (mPort->GetEvents(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclProvidedEventID* next = eventBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMoreEvents())
      {
        *bufferSize = i;
        break;
      }

      VIAFbProvidedEvent* providedEvent;
      if (it.mVIAObject->GetNextEvent(&providedEvent) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VProvidedEvent::CreateProvidedEvent(providedEvent, true);
    }

    return it.mVIAObject->HasMoreEvents() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  int VProvidedEventGroup::GetPDUs(cclPDUSenderID* pduBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbPDUIterator<VIAFbProvidedServicePDU>> it;
    if (mPort->GetPDUs(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclPDUSenderID* next = pduBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMorePDUs())
      {
        *bufferSize = i;
        break;
      }

      VIAFbProvidedServicePDU* providedPDU;
      if (it.mVIAObject->GetNextPDU(&providedPDU) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VProvidedServicePDU::CreateProvidedPDU(providedPDU, true);
    }

    return it.mVIAObject->HasMorePDUs() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  int VProvidedEventGroup::GetFields(cclProvidedFieldID* fieldBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbFieldIterator<VIAFbProvidedField>> it;
    if (mPort->GetFields(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclProvidedFieldID* next = fieldBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMoreFields())
      {
        *bufferSize = i;
        break;
      }

      VIAFbProvidedField* providedField;
      if (it.mVIAObject->GetNextField(&providedField) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VProvidedField::CreateProvidedField(providedField, true);
    }

    return it.mVIAObject->HasMoreFields() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  bool VProvidedEventGroup::IsEqual(const VPortBase<VIAFbSomeIPProvidedEventGroup>* other) const
  {
    unsigned long otherID;
    if ((other->mPort == nullptr) || (other->mPort->GetEventGroupId(&otherID) != kVIA_OK))
      return false;

    return VPortBase::IsEqual(other) && (otherID == mEventGroupID);
  }

  #pragma endregion

  #pragma region VEventGroupProvider Implementation

  VEventGroupProvider::VEventGroupProvider(VIAFbSomeIPEventGroupProvider* eventGroupProvider)
    : VPortBase(eventGroupProvider, true)
    , mEventGroupProviderID(NDetail::cInvalidCCLObjectID)
    , mEventGroupSubscribedHandler(nullptr)
    , mEventGroupUnsubscribedHandler(nullptr)
    , mCallbackHandle(nullptr)
  {}

  VEventGroupProvider::~VEventGroupProvider()
  {
    if (mCallbackHandle != nullptr)
    {
      if (mPort != nullptr)
        mPort->UnregisterObserver(mCallbackHandle);

      mEventGroupSubscribedHandler = nullptr;
      mEventGroupUnsubscribedHandler = nullptr;
      mCallbackHandle = nullptr;
    }
  }

  int VEventGroupProvider::CreateEventGroupProvider(const char* path)
  {
    if (path == nullptr)
      return CCL_PARAMETERINVALID;

    if (gModule == NULL)
      return CCL_INTERNALERROR;

    VIAFbViaService* fbAPI = GetFunctionBusAPI();
    if (fbAPI == nullptr)
      return CCL_NOTIMPLEMENTED;

    VIAFbSomeIPBinding* someIPBinding;
    if (fbAPI->GetSomeIPBinding(&someIPBinding) != kVIA_OK)
      return CCL_INTERNALERROR;

    VIAFbSomeIPEventGroupProvider* eventGroupProvider = nullptr;
    if (someIPBinding->GetEventGroupProvider(path, &eventGroupProvider) != kVIA_OK)
      return CCL_INVALIDEVENTGROUPPROVIDERPATH;

    return gModule->mEventGroupProviders.Add(new VEventGroupProvider(eventGroupProvider));
  }

  int VEventGroupProvider::LookupEventGroupProvider(cclSomeIPEventGroupProviderID eventGroupProviderID, VEventGroupProvider*& eventGroupProvider)
  {
    if (gModule == NULL)
      return CCL_INTERNALERROR;

    eventGroupProvider = gModule->mEventGroupProviders.Get(eventGroupProviderID);
    if (eventGroupProvider == nullptr)
      return CCL_INVALIDEVENTGROUPPROVIDERID;

    return CCL_SUCCESS;
  }

  int VEventGroupProvider::GetNrOfSubscribedConsumers(int* nrOfConsumers)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbServiceConsumerIterator> it;
    if (mPort->GetSubscribedConsumers(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    *nrOfConsumers = 0;
    while (it.mVIAObject->HasMoreConsumers())
    {
      ++(*nrOfConsumers);
      it.mVIAObject->SkipConsumer();
    }

    return CCL_SUCCESS;
  }

  int VEventGroupProvider::GetSubscribedConsumers(cclConsumerID* consumerBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbServiceConsumerIterator> it;
    if (mPort->GetSubscribedConsumers(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclConsumerID* next = consumerBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMoreConsumers())
      {
        *bufferSize = i;
        break;
      }

      VIAFbServiceConsumer* consumer;
      if (it.mVIAObject->GetNextConsumer(&consumer) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VConsumer::CreateConsumer(consumer, true);
    }

    return it.mVIAObject->HasMoreConsumers() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  int VEventGroupProvider::GetEvents(cclEventProviderID* eventBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbEventIterator<VIAFbEventProvider>> it;
    if (mPort->GetEvents(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclEventProviderID* next = eventBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMoreEvents())
      {
        *bufferSize = i;
        break;
      }

      VIAFbEventProvider* eventProvider;
      if (it.mVIAObject->GetNextEvent(&eventProvider) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VEventProvider::CreateEventProvider(eventProvider);
    }

    return it.mVIAObject->HasMoreEvents() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  int VEventGroupProvider::GetPDUs(cclPDUProviderID* pduBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbPDUIterator<VIAFbServicePDUProvider>> it;
    if (mPort->GetPDUs(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclPDUProviderID* next = pduBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMorePDUs())
      {
        *bufferSize = i;
        break;
      }

      VIAFbServicePDUProvider* pduProvider;
      if (it.mVIAObject->GetNextPDU(&pduProvider) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VServicePDUProvider::CreatePDUProvider(pduProvider);
    }

    return it.mVIAObject->HasMorePDUs() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  int VEventGroupProvider::GetFields(cclFieldProviderID* fieldBuffer, int* bufferSize)
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    NDetail::VIAObjectGuard<VIAFbFieldIterator<VIAFbFieldProvider>> it;
    if (mPort->GetFields(&(it.mVIAObject)) != kVIA_OK)
      return CCL_INTERNALERROR;

    cclFieldProviderID* next = fieldBuffer;
    for (int i = 0; i < *bufferSize; ++i)
    {
      if (!it.mVIAObject->HasMoreFields())
      {
        *bufferSize = i;
        break;
      }

      VIAFbFieldProvider* fieldProvider;
      if (it.mVIAObject->GetNextField(&fieldProvider) != kVIA_OK)
        return CCL_INTERNALERROR;

      *next++ = VFieldProvider::CreateFieldProvider(fieldProvider);
    }

    return it.mVIAObject->HasMoreFields() ? CCL_BUFFERTOSMALL : CCL_SUCCESS;
  }

  int VEventGroupProvider::UpdateRegistration()
  {
    if (mPort == nullptr)
      return CCL_INTERNALERROR;

    VIAResult rv = kVIA_OK;
    if ((mEventGroupSubscribedHandler != nullptr) || (mEventGroupUnsubscribedHandler != nullptr))
    {
      if (mCallbackHandle == nullptr)
        rv = mPort->RegisterObserver(this, &mCallbackHandle);
    }
    else
    {
      if (mCallbackHandle != nullptr)
        rv = mPort->UnregisterObserver(mCallbackHandle);
      mCallbackHandle = nullptr;
    }
    return (rv == kVIA_OK) ? CCL_SUCCESS : CCL_INTERNALERROR;
  }

  int VEventGroupProvider::SetEventGroupSubscribedHandler(cclSomeIPEventGroupSubscriptionHandler handler)
  {
    mEventGroupSubscribedHandler = handler;
    return UpdateRegistration();
  }

  int VEventGroupProvider::SetEventGroupUnsubscribedHandler(cclSomeIPEventGroupSubscriptionHandler handler)
  {
    mEventGroupUnsubscribedHandler = handler;
    return UpdateRegistration();
  }

  VIASTDDEF VEventGroupProvider::OnEventGroupSubscribed(VIAFbSomeIPProvidedEventGroup* inEventGroupPort)
  {
    if (mEventGroupSubscribedHandler != nullptr)
    {
      auto portID = VProvidedEventGroup::CreateProvidedEventGroup(inEventGroupPort);
      if (portID < 0)
        return kVIA_Failed;

      mEventGroupSubscribedHandler(portID);
      return VProvidedEventGroup::RemoveProvidedEventGroup(portID);
    }
    return kVIA_OK;
  }

  VIASTDDEF VEventGroupProvider::OnEventGroupUnsubscribed(VIAFbSomeIPProvidedEventGroup* inEventGroupPort)
  {
    if (mEventGroupUnsubscribedHandler != nullptr)
    {
      auto portID = VProvidedEventGroup::CreateProvidedEventGroup(inEventGroupPort);
      if (portID < 0)
        return kVIA_Failed;

      mEventGroupUnsubscribedHandler(portID);
      return VProvidedEventGroup::RemoveProvidedEventGroup(portID);
    }
    return kVIA_OK;
  }

  bool VEventGroupProvider::IsEqual(const VPortBase<VIAFbSomeIPEventGroupProvider>* other) const
  {
    unsigned long otherID;
    if ((other->mPort == nullptr) || (other->mPort->GetEventGroupId(&otherID) != kVIA_OK))
      return false;

    return VPortBase::IsEqual(other) && (otherID == mEventGroupID);
  }

  #pragma endregion

  #pragma region VNodeLayerModule Implementation

  // ============================================================================
  // Implementation of class VNodeLayerModule
  // ============================================================================

  VNodeLayerModule::VNodeLayerModule()
    : mMeasurementPreStartHandler(NULL)
    , mMeasurementStartHandler(NULL)
    , mMeasurementStopHandler(NULL)
    , mDllUnloadHandler(NULL)
  {}


  VIASTDDEF VNodeLayerModule::Init()
  {
    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::GetVersion(char* buffer, int32 bufferLength)
  {
    if (buffer != NULL && bufferLength > 0)
    {
      strncpy_s(buffer, bufferLength, "CANaylzer/CANoe C Library Version 1.4", _TRUNCATE);
    }
    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::GetModuleParameters (char* pathBuff,    int32 pathBuffLength,
                                                   char* versionBuff, int32 versionBuffLength)
  {
    ::GetModuleFileNameA(gDLLInstanceHandle, pathBuff, pathBuffLength);
    GetVersion(versionBuff, versionBuffLength);
    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::CreateObject(VIANodeLayerApi** object, VIANode* node, 
                                           int32 argc, char* argv[])
  {
    uint32 busType;
    node->GetBusType(&busType);

    VIAChannel channel;
    node->GetChannel(&channel);

    std::string busName;
    if (gHasBusNames)
    {
      char text[256];
      VIAResult result = node->GetBusName(text, 256);
      if (result == kVIA_OK)
      {
        busName = text;
      }
    }

    VIABus* bus = NULL;
    if (busType == kVIA_CAN)
    {
      gVIAService->GetBusInterface(&bus, node, kVIA_CAN, VIACANMajorVersion, VIACANMinorVersion);
    }
    else if (busType == kVIA_LIN)
    {
      gVIAService->GetBusInterface(&bus, node, kVIA_LIN, VIALINMajorVersion, VIALINMinorVersion);
    }

    VNodeLayer* layer = new VNodeLayer(node, busType, channel, bus);
    *object = layer;

    if (busType == kVIA_CAN && channel >= 1 && channel <= cMaxChannel)
    {
      gCanBusContext[channel].mBus = static_cast<VIACan*>(bus);
      gCanBusContext[channel].mNode = node;
      gCanBusContext[channel].mLayer = layer;
      gCanBusContext[channel].mBusName = busName;
    }
    else if (busType == kVIA_LIN && channel >= 1 && channel <= cMaxChannel)
    {
      gLinBusContext[channel].mBus = static_cast<VIALin*>(bus);;
      gLinBusContext[channel].mNode = node;
      gLinBusContext[channel].mLayer = layer;
      gLinBusContext[channel].mBusName = busName;
    }

    if (gMasterLayer == NULL)
    {
      gMasterLayer = layer;
    }

    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::ReleaseObject(VIANodeLayerApi* object)
  {
    VNodeLayer* layer = static_cast<VNodeLayer*>(object);
    uint32 busType = layer->mBusType;
    VIAChannel channel = layer->mChannel;

    if (gMasterLayer == layer)
    {
      gMasterLayer = NULL;
      for (unsigned int i = 0; i < mSignal.size(); i++)
      {
        if (mSignal[i] != NULL)
        {
          gSignalAccessAPI->ReleaseSignal(mSignal[i]->mViaSignal);
          delete mSignal[i];
          mSignal[i] = NULL;
        }
      }
    }

    if (busType == kVIA_CAN && channel >= 1 && channel <= cMaxChannel)
    {
      gCanBusContext[channel].mBus = NULL;
      gCanBusContext[channel].mNode = NULL;
      gCanBusContext[channel].mLayer = NULL;
      gCanBusContext[channel].mBusName.clear();
    }
    else if (busType == kVIA_LIN && channel >= 1 && channel <= cMaxChannel)
    {
      gLinBusContext[channel].mBus = NULL;
      gLinBusContext[channel].mNode = NULL;
      gLinBusContext[channel].mLayer = NULL;
      gLinBusContext[channel].mBusName.clear();
    }

    if (layer->mBus != NULL)
    {
      gVIAService->ReleaseBusInterface(layer->mBus);
    }

    delete layer;

    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::GetNodeInfo(const char * nodename, 
                                          char* shortNameBuf, int32 shortBufLength,
                                          char* longNameBuf,  int32 longBufLength )
  {
    if (shortNameBuf != NULL && shortBufLength > 0)
    {
      shortNameBuf[0] = char(0);
    }

    if (longNameBuf != NULL && longBufLength > 0)
    {
      longNameBuf[0] = char(0);
    }

    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::InitMeasurement()
  {
    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::EndMeasurement()
  {
    gState = eLoaded;

    // delete Timers
    for (unsigned int i = 0; i < mTimer.size(); i++)
    {
      VTimer* timer = mTimer[i];
      if (timer != NULL)
      {
        if (timer->mViaTimer != NULL)
        {
          VIAResult rc = gVIAService->ReleaseTimer(timer->mViaTimer);
          timer->mViaTimer = NULL;
        }
        delete timer;
        mTimer[i] = NULL;
      }
    }
    mTimer.clear();

    // delete system variables
    for (unsigned int i = 0; i < mSysVar.size(); i++)
    {
      VSysVar* sysVar = mSysVar[i];
      if (sysVar != NULL)
      {
        if (sysVar->mViaSysVarMember != NULL)
        {
          VIAResult rc = sysVar->mViaSysVarMember->Release();
          sysVar->mViaSysVarMember = NULL;
        }
        if (sysVar->mViaSysVar != NULL)
        {
          VIAResult rc = sysVar->mViaSysVar->Release();
          sysVar->mViaSysVar = NULL;
        }
        delete sysVar;
        mSysVar[i] = NULL;
      }
    }
    mSysVar.clear();

    // delete message requests for CAN
    for (unsigned int i = 0; i < mCanMessageRequests.size(); i++)
    {
      VCanMessageRequest* request = mCanMessageRequests[i];
      if (request != NULL)
      {
        if (request->mHandle != NULL && request->mContext != NULL)
        {
          request->mContext->mBus->ReleaseRequest(request->mHandle);
          request->mHandle = NULL;
        }
        delete request;
        mCanMessageRequests[i] = NULL;
      }
    }
    mCanMessageRequests.clear();

    // delete message requests for LIN
    for (unsigned int i = 0; i < mLinMessageRequests.size(); i++)
    {
      VLinMessageRequest* request = mLinMessageRequests[i];
      if (request != NULL)
      {
        if (request->mHandle != NULL && request->mContext != NULL)
        {
          request->mContext->mBus->ReleaseRequest(request->mHandle);
          request->mHandle = NULL;
        }
        delete request;
        mLinMessageRequests[i] = NULL;
      }
    }
    mLinMessageRequests.clear();

    // delete function bus entities
    VService::Cleanup();
    mValues.Clear();
    mFunctions.Clear();
    mCallContexts.Clear();
    mConsumers.Clear();
    mProviders.Clear();
    mConsumedServices.Clear();
    mProvidedServices.Clear();
    mAddresses.Clear();
    mPDUSender.Clear();
    mPDUReceiver.Clear();
    mPDUProviders.Clear();
    mConsumedEvents.Clear();
    mProvidedEvents.Clear();
    mEventProviders.Clear();
    mConsumedFields.Clear();
    mProvidedFields.Clear();
    mFieldProviders.Clear();
    mConsumedEventGroups.Clear();
    mProvidedEventGroups.Clear();
    mEventGroupProviders.Clear();

    return kVIA_OK;
  }


  VIASTDDEF VNodeLayerModule::GetNodeInfoEx (VIDBNodeDefinition* nodeDefinition,
                                             char* shortNameBuf, int32 shortBufLength,
                                             char* longNameBuf, int32 longBufLength )
  {
    if (shortNameBuf != NULL && shortBufLength > 0)
    {
      shortNameBuf[0] = char(0);
    }

    if (longNameBuf != NULL && longBufLength > 0)
    {
      longNameBuf[0] = char(0);
    }

    return kVIA_OK;
  }

  VIASTDDEF VNodeLayerModule::DoInformOfChange(VIDBNodeDefinition* nodeDefinition,
                                               const uint32      changeFlags,
                                               const char*       simBusName,
                                               const VIABusInterfaceType busType,
                                               const VIAChannel  channel,
                                               const char*       oldName,
                                               const char*       newName,
                                               const int32       bValue)
  {
    return kVIA_OK;
  }

  #pragma endregion

  #pragma region VNodeLayer Implementation

  // ============================================================================
  // Implementation of class VNodeLayer
  // ============================================================================

  VNodeLayer::VNodeLayer(VIANode* node, uint32 busType, VIAChannel channel, VIABus* bus)
    : mNode(node)
    , mBusType(busType)
    , mChannel(channel)
    , mBus(bus)
  {}


  VNodeLayer::~VNodeLayer() 
  {}


  VIASTDDEF  VNodeLayer::GetNode(VIANode **node)
  {
    *node = mNode;
    return kVIA_OK;
  }


  VIASTDDEF  VNodeLayer::InitMeasurement()
  {
    if (gState==eLoaded)
    {
      VIAResult rc = gVIAService->GetSignalAccessApi(&gSignalAccessAPI, mNode, 2, 10);

      gState = eInitMeasurement;
      if (gModule->mMeasurementPreStartHandler!=NULL)
      {
        gModule->mMeasurementPreStartHandler();
      }
    }
    return kVIA_OK;
  }


  VIASTDDEF  VNodeLayer::StartMeasurement()
  {
    if (gState==eInitMeasurement)
    {
      gState=eRunMeasurement;
      if (gModule->mMeasurementStartHandler!=NULL)
      {
        gModule->mMeasurementStartHandler();
      }
    }
    return kVIA_OK;
  }


  VIASTDDEF  VNodeLayer::StopMeasurement()
  {
    if (gState==eRunMeasurement)
    {
      gState=eStopMeasurement;
      if (gModule->mMeasurementStopHandler!=NULL)
      {
        gModule->mMeasurementStopHandler();
      }
    }
    return kVIA_OK;
  }


  VIASTDDEF VNodeLayer::EndMeasurement()
  {
    return kVIA_OK;
  }


  VIASTDDEF VNodeLayer::InitMeasurementComplete()
  {
    return kVIA_OK;
  }


  VIASTDDEF VNodeLayer::PreInitMeasurement()
  {
    return kVIA_OK;
  }

  #pragma endregion
}


using namespace CANalyzerCANoeCLibrary;

#pragma region Node Layer DLL

// ============================================================================
// Define the entry point for the DLL application.
// ============================================================================


BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved )
{
  if ( DLL_PROCESS_ATTACH == ul_reason_for_call )
  {
    gDLLInstanceHandle = static_cast<HINSTANCE>(hModule);
  }

  return TRUE;
}


// ============================================================================
// NodeLayer DLL Interface Section
// ============================================================================



VIACLIENT(void) VIARequiredVersion(int32* majorversion, int32* minorversion)
{
  *majorversion = VIAMajorVersion;
  *minorversion = 40;  // VIAMinorVersion;
}



VIACLIENT(void) VIADesiredVersion(int32* majorversion, int32* minorversion)
{
  *majorversion = VIAMajorVersion;
  *minorversion = VIAMinorVersion;
}



VIACLIENT(void) VIASetService(VIAService* service)
{
  VIAResult rc;
  gVIAService = service;
  rc = gVIAService->GetSystemVariablesRootNamespace(gSysVarRoot);

  {
    int32 major, minor, patchlevel;
    rc = gVIAService->GetVersion (&major, &minor, &patchlevel);
    if (rc==kVIA_OK)
    {
      gVIAMinorVersion = minor;
    }

    // C-Libraries are executed in a context of a dummy node. Because the C-Library was feature was created primary for
    // CANalyzer, this special context has no bus names in older versions of CANoe.
    gHasBusNames = minor >= 105;
  }
}



VIACLIENT(VIAModuleApi*) VIAGetModuleApi(int32 argc, char* argv[])
{
  gState = eLoaded;

  if (gModule==NULL)
  {
    gModule = new VNodeLayerModule();
  }

  cclOnDllLoad();

  return gModule;
} // VIAGetModuleApi



VIACLIENT(void) VIAReleaseModuleApi(VIAModuleApi* api)
{
  gState = eUnloaded;

  if (gModule!=NULL && gModule->mDllUnloadHandler!=NULL)
  {
    gModule->mDllUnloadHandler();
  }

  if (gVIAService!=NULL && gSysVarRoot!=NULL)
  {
    gSysVarRoot->Release();
  }
  gSysVarRoot = NULL;
  
  gFunctionBusAPI = NULL;

  gVIAService = NULL;
  

  delete gModule;
  gModule = NULL;
} // VIAReleaseModuleApi



extern "C" __declspec(dllexport) int32 __stdcall NLGetModuleOptions(int32 option)
{
  if (option==kVIA_ModuleOption_LoadOption)  // Load Option (0 default, 1 permanent, 2 defer)
  {
    return kVIA_ModuleOption_LoadOption_Defer;
  }
  else if (option==kVIA_GetModuleOption_DLLType) // DLL Type Option (0 default, 1 standard node layer, 2 C library)
  {
    return kVIA_ModuleOption_DLLType_CLibrary; // DLL Type C Library
  }
  else
  {
    return 0;
  }
}

#pragma endregion


#pragma region Global API Functions

// ============================================================================
// CCL API functions
// ============================================================================



void cclSetMeasurementPreStartHandler(void (*function)())
{
  if (gModule!=NULL)
  {
    gModule->mMeasurementPreStartHandler = function;
  }
}



void cclSetMeasurementStartHandler(void (*function)())
{
  if (gModule!=NULL)
  {
    gModule->mMeasurementStartHandler = function;
  }
}



void cclSetMeasurementStopHandler(void (*function)())
{
  if (gModule!=NULL)
  {
    gModule->mMeasurementStopHandler = function;
  }
}



void cclSetDllUnloadHandler(void (*function)())
{
  if (gModule!=NULL)
  {
    gModule->mDllUnloadHandler = function;
  }
}



void cclWrite(const char* text)
{
  if (gVIAService!=NULL)
  {
    VIAResult rc = gVIAService->WriteString(text);
  }
}



void cclPrintf(const char* format, ...)
{
  char buffer[4096];
  va_list arg;
  int result;

  va_start(arg, format);
  result = _vsnprintf_s( buffer, 4096, _TRUNCATE, format, arg);
  va_end(arg);


  if (result<0)
  {
    return;
  }

  if (gVIAService!=NULL)
  {
    VIAResult rc = gVIAService->WriteString(buffer);
  }
};



int cclGetUserFilePath(const char* filename,
                       char* pathBuffer,
                       int pathBufferLength)
{
  if (gVIAService==NULL)
  {
    return CCL_INTERNALERROR;
  }
  if (gVIAMinorVersion<41)
  {
    return CCL_NOTIMPLEMENTED;
  }
  if (filename==NULL && strlen(filename)==0)
  {
    return CCL_INVALIDNAME;
  }

  VIAResult rc = gVIAService->GetUserFilePath(filename, pathBuffer, pathBufferLength);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_ObjectNotFound:
    return CCL_USERFILENOTDEFINED;
  case kVIA_ParameterInvalid:
    return CCL_PARAMETERINVALID;
  case kVIA_BufferToSmall:
    return CCL_BUFFERTOSMALL;
  case kVIA_ServiceNotRunning:
    return CCL_WRONGSTATE;
  case kVIA_Failed:
    return CCL_INTERNALERROR;
  default:
    return CCL_INTERNALERROR;
  }
}

#pragma endregion

#pragma region Time & Timer

// ============================================================================
// Time & Timer 
// ============================================================================


long long cclTimeSeconds(long long seconds)
{
  return seconds * 1000000000LL;
}


long long cclTimeMilliseconds(long long milliseconds)
{
  return milliseconds * 1000000LL;
}


long long cclTimeMicroseconds(long long microseconds)
{
  return microseconds * 1000LL;
}


int cclTimerCreate( void (*function)(long long time, int timerID) )
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement)
  if (gModule==NULL || gVIAService==NULL)
  {
    return CCL_INTERNALERROR;
  }
  if (function==NULL)
  {
    return CCL_INVALIDFUNCTIONPOINTER;
  }

  VTimer* cclTimer = new VTimer;
  VIATimer* viaTimer = NULL;
  
  VIAResult rc = gVIAService->CreateTimer(&viaTimer, gMasterLayer->mNode, cclTimer, "C-Library Timer");
  if (rc!=kVIA_OK)
  {
    delete cclTimer;
    return CCL_INTERNALERROR;
  }

  cclTimer->mCallbackFunction = function;
  cclTimer->mViaTimer = viaTimer;
  cclTimer->mTimerID = static_cast<int>(gModule->mTimer.size());
  gModule->mTimer.push_back(cclTimer);

  return cclTimer->mTimerID;
}



int cclTimerSet(int timerID, long long nanoseconds)
{
  CCL_STATECHECK(eRunMeasurement)
  if (gModule==NULL)
  {
    return CCL_INTERNALERROR;
  }
  if (nanoseconds<=0)
  {
    return CCL_INVALIDTIME;
  }

  if (timerID < 0  ||  timerID >= static_cast<int>(gModule->mTimer.size()))
  {
    return CCL_INVALIDTIMERID;
  }

  VTimer* cclTimer = gModule->mTimer[timerID];
  if (cclTimer==NULL || cclTimer->mTimerID!=timerID)
  {
    return CCL_INVALIDTIMERID;
  }

  VIAResult rc = cclTimer->mViaTimer->SetTimer(nanoseconds);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}



int cclTimerCancel(int timerID)
{
  CCL_STATECHECK(eRunMeasurement)
  if (gModule==NULL)
  {
    return CCL_INTERNALERROR;
  }

  if (timerID < 0  ||  timerID >= static_cast<int>(gModule->mTimer.size()))
  {
    return CCL_INVALIDTIMERID;
  }

  VTimer* cclTimer = gModule->mTimer[timerID];
  if (cclTimer==NULL || cclTimer->mTimerID!=timerID)
  {
    return CCL_INVALIDTIMERID;
  }

  VIAResult rc = cclTimer->mViaTimer->CancelTimer();
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}

#pragma endregion

#pragma region System Variables

// ============================================================================
// System Variables
// ============================================================================


int cclSysVarGetID(char* systemVariableName)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)
  if (gSysVarRoot==NULL || gModule==NULL)
  {
    return CCL_INTERNALERROR;
  }
  if (systemVariableName==NULL && strlen(systemVariableName)==0)
  {
    return CCL_INVALIDNAME;
  }

  bool isStructMember = false;
  char* varName = NULL;
  char* memberName = NULL;
  {
    size_t pos = strcspn(systemVariableName, ".[");
    size_t len = strlen(systemVariableName);
    if (pos<len)
    {
      isStructMember = true;
      if(systemVariableName[pos]=='.')
      {
        memberName = systemVariableName+pos+1; // exclude the '.'
      }
      else
      {
        memberName = systemVariableName+pos; // include the '['
      }
      
      size_t varNameLength = pos;
      if (varNameLength==0)
      {
        return CCL_INVALIDNAME;
      }
      if (strlen(memberName)==0)
      {
        return CCL_INVALIDNAME;
      }
      if (gVIAMinorVersion<50)
      {
        return CCL_NOTIMPLEMENTED;
      }
      varName = (char*)malloc(varNameLength+1);
      if (varName==NULL)
      {
        return CCL_INTERNALERROR;
      }
      strncpy_s(varName, varNameLength+1, systemVariableName, varNameLength);
    }
    else
    {
      isStructMember = false;
      varName = systemVariableName;
      memberName = NULL;
    }
  }

  VIASystemVariable* viaSysVar=NULL;
  VIAResult rc = gSysVarRoot->GetVariable( varName, viaSysVar);
  if (isStructMember)
  {
    free(varName);
    varName=NULL;
  }
  if (rc!=kVIA_OK || viaSysVar==NULL)
  {
    if (rc==kVIA_ObjectNotFound)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  VIASystemVariableMember* viaSysVarMember=NULL;
  if (isStructMember)
  {
    rc = viaSysVar->GetMember(memberName, &viaSysVarMember);
    if (rc!=kVIA_OK || viaSysVar==NULL)
    {
      viaSysVar->Release();
      viaSysVar=NULL;
      if (rc==kVIA_ObjectNotFound)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }

  VSysVar* cclSysVar = new VSysVar;
  cclSysVar->mViaSysVar = viaSysVar;
  cclSysVar->mViaSysVarMember = viaSysVarMember;
  cclSysVar->mVariableName = (varName != NULL) ? varName : "";
  cclSysVar->mMemberName = (memberName != NULL) ? memberName : "";
  cclSysVar->mCallbackFunction = NULL;
  cclSysVar->mSysVarID = static_cast<int>(gModule->mSysVar.size());
  gModule->mSysVar.push_back(cclSysVar);

  return cclSysVar->mSysVarID;
}


int cclSysVarGetType(int sysVarID)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIASysVarType type;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIAResult rc;
    rc = cclSysVar->mViaSysVarMember->GetType(&type);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectNotFound)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }
  else
  {
    VIAResult rc;
    rc = cclSysVar->mViaSysVar->GetType(&type);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }

  switch(type)
  {
  case kVIA_SVInteger:
    return CCL_SYSVAR_INTEGER;
  case  kVIA_SVFloat:
    return CCL_SYSVAR_FLOAT;
  case  kVIA_SVString:
    return CCL_SYSVAR_STRING;
  case  kVIA_SVIntegerArray:
    return CCL_SYSVAR_INTEGERARRAY;
  case  kVIA_SVFloatArray:
    return CCL_SYSVAR_FLOATARRAY;
  case  kVIA_SVData:
    return CCL_SYSVAR_DATA;
  case  kVIA_SVStruct:
    return CCL_SYSVAR_STRUCT;
  case  kVIA_SVGenericArray:
    return CCL_SYSVAR_GENERICARRAY;
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSysVarSetHandler(int sysVarID, void (*function)(long long time, int sysVarID) )
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)
  
  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  cclSysVar->mCallbackFunction = function;
  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIAOnSysVarMember* sink = (cclSysVar->mCallbackFunction == NULL) ? NULL : cclSysVar;
    rc = cclSysVar->mViaSysVarMember->SetSink(sink);
  }
  else
  {
    VIAOnSysVar* sink = (cclSysVar->mCallbackFunction == NULL) ? NULL : cclSysVar;
    rc = cclSysVar->mViaSysVar->SetSink(sink);
  }

  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclSysVarGetArraySize(int sysVarID)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  // check that system variable has an array type
  VIASysVarType varType;
  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetType(&varType);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetType(&varType);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }
  if (varType!=kVIA_SVIntegerArray && varType!=kVIA_SVFloatArray && varType!=kVIA_SVData && varType!=kVIA_SVGenericArray)
  {
    return CCL_WRONGTYPE;
  }

  int32 size;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetArraySize(&size);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
    if (varType==kVIA_SVGenericArray && size > 0)
    {
      std::string elementName = cclSysVar->mMemberName;
      elementName += "[0]";
      
      VIASystemVariableMember* arrayElement;
      rc = cclSysVar->mViaSysVar->GetMember(elementName.c_str(), &arrayElement);
      if (rc != kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }

      int32 bitLength = 0;
      rc = arrayElement->GetBitLength(&bitLength);
      arrayElement->Release();
      if (rc != kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }
      size = ( 8 * size ) / bitLength;
    }
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetArraySize(&size);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }

    if (varType==kVIA_SVGenericArray && size > 0)
    {
      std::string elementName = cclSysVar->mMemberName;
      elementName += "[0]";

      VIASystemVariableMember* arrayElement;
      rc = cclSysVar->mViaSysVar->GetMember(elementName.c_str(), &arrayElement);
      if (rc != kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }

      int32 bitLength = 0;
      rc = arrayElement->GetBitLength(&bitLength);
      arrayElement->Release();
      if (rc != kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }
      size = ( 8 * size ) / bitLength;
    }
  }

  return size;
}


int cclSysVarSetInteger(int sysVarID, long x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVInteger, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetInteger(static_cast<::int64>(x), NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetInteger(x, NULL);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetInteger(int sysVarID, long* x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)
  
  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVInteger, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    ::int64 y;
    rc = cclSysVar->mViaSysVarMember->GetInteger(&y);
    *x = static_cast<long>(y);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetInteger(x);
  }

  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }
  
  return CCL_SUCCESS;
}


int cclSysVarSetInteger64(int sysVarID, long long x)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVInteger, true);
    if (result != CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember != NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetInteger(x, NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetIntegerEx(x, NULL);
  }
  if (rc != kVIA_OK)
  {
    if (rc == kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetInteger64(int sysVarID, long long* x)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement)

    VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVInteger, false);
    if (result != CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember != NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetInteger(x);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetIntegerEx(x);
  }

  if (rc != kVIA_OK)
  {
    if (rc == kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarSetFloat(int sysVarID, double x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVFloat, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetFloat(x, NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetFloat(x, NULL);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetFloat(int sysVarID, double* x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVFloat, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetFloat(x);
  }
  else
  {
    rc = rc = cclSysVar->mViaSysVar->GetFloat(x);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarSetString(int sysVarID, const char* text)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVString, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetString(text, NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetString(text, NULL);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetString(int sysVarID, char* buffer, int bufferLenght)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVString, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }
  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetString(buffer, bufferLenght);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetString(buffer, bufferLenght);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else if (rc==kVIA_BufferToSmall)
    {
      return CCL_BUFFERTOSMALL;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarSetIntegerArray(int sysVarID, const long* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVIntegerArray, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  int32 size;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIASysVarType varType;
    { 
      VIAResult rc = cclSysVar->mViaSysVarMember->GetType(&varType);
      if (rc!=kVIA_OK)
      {
        return CCL_INTERNALERROR;
      }
    }
    if (varType==kVIA_SVGenericArray)
    {
      size = cclSysVarGetArraySize(sysVarID);
      if (size<0)
      {
        return size;
      }
      rc = kVIA_OK;
    }
    else
    {
      rc = cclSysVar->mViaSysVarMember->GetArraySize(&size);
    }
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetArraySize(&size);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }
  if (length!=size)
  {
    return CCL_WRONGARRAYSIZE;
  }

  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIASysVarType varType;
    rc = cclSysVar->mViaSysVarMember->GetType(&varType);
    if (rc!=kVIA_OK)
    {
      return CCL_INTERNALERROR;
    }

    if (varType==kVIA_SVGenericArray)
    {
      char memberName[1024];
      char arryIndex[24];
      rc = cclSysVar->mViaSysVarMember->GetName(memberName, 1000);
      if (rc!=kVIA_OK)
      {
        return CCL_INTERNALERROR;
      }
      size_t memberNameLength = strlen(memberName);

      for (int i=0; i<length; i++)
      {
        sprintf_s(arryIndex, 24, "[%d]", i);
        strcpy_s(memberName+memberNameLength, 24, arryIndex);

        VIASystemVariableMember* element = NULL;
        cclSysVar->mViaSysVar->GetMember(memberName, &element);
        if (rc!=kVIA_OK)
        {
          return CCL_INTERNALERROR;
        }

        VIASysVarType elementType;
        rc = element->GetType(&elementType);
        if (rc!=kVIA_OK)
        {
          element->Release();
          return CCL_INTERNALERROR;
        }
        if (elementType!=kVIA_SVInteger)
        {
          element->Release();
          return CCL_WRONGTYPE;
        }

        ::int64 elementValue = static_cast<int64>(x[i]);;
        rc = element->SetInteger(elementValue, NULL);
        if (rc!=kVIA_OK)
        {
          element->Release();
          return CCL_INTERNALERROR;
        }

        element->Release();
      }
    }
    else if (varType==kVIA_SVIntegerArray)
    {
      ::int64* buffer = static_cast<::int64*>( alloca (length * sizeof(::int64) ) );
      for (int i=0; i<length; i++)
      {
        buffer[i] = static_cast<::int64>(x[i]);
      }
      rc = cclSysVar->mViaSysVarMember->SetIntegerArray(buffer, length, NULL);
      if (rc!=kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }
    }
    else
    {
      return CCL_WRONGTYPE;
    }
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetIntegerArray(x, length, NULL);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetIntegerArray(int sysVarID, long* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVIntegerArray, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  if (length<0)
  {
    return CCL_BUFFERTOSMALL;
  }

  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIASysVarType varType;
    { 
      VIAResult rc = cclSysVar->mViaSysVarMember->GetType(&varType);
      if (rc!=kVIA_OK)
      {
        return CCL_INTERNALERROR;
      }
    }
    if (varType==kVIA_SVGenericArray)
    {
      int32 arraySize = cclSysVarGetArraySize(sysVarID);
      if (arraySize < 0)
      {
        return arraySize;
      }
      if (arraySize>length)
      {
        return CCL_BUFFERTOSMALL;
      }

      char memberName[1024];
      char arryIndex[24];
      VIAResult rc = cclSysVar->mViaSysVarMember->GetName(memberName, 1000);
      if (rc!=kVIA_OK)
      {
        return CCL_INTERNALERROR;
      }
      size_t memberNameLength = strlen(memberName);

      for (int i=0; i<arraySize; i++)
      {
        sprintf_s(arryIndex, 24, "[%d]", i);
        strcpy_s(memberName+memberNameLength, 24, arryIndex);

        VIASystemVariableMember* element = NULL;
        cclSysVar->mViaSysVar->GetMember(memberName, &element);
        if (rc!=kVIA_OK)
        {
          return CCL_INTERNALERROR;
        }
        
        VIASysVarType elementType;
        rc = element->GetType(&elementType);
        if (rc!=kVIA_OK)
        {
          element->Release();
          return CCL_INTERNALERROR;
        }
        if (elementType!=kVIA_SVInteger)
        {
          element->Release();
          return CCL_WRONGTYPE;
        }

        ::int64 elementValue;
        rc = element->GetInteger(&elementValue);
        if (rc!=kVIA_OK)
        {
          element->Release();
          return CCL_INTERNALERROR;
        }
        x[i] = static_cast<long>(elementValue);

        element->Release();
      }
    }
    else if (varType==kVIA_SVIntegerArray)
    {
      ::int64* buffer = static_cast<::int64*>( alloca (length * sizeof(::int64) ) );
      VIAResult rc = cclSysVar->mViaSysVarMember->GetIntegerArray(buffer, length);
      if (rc!=kVIA_OK)
      {
        if (rc==kVIA_ObjectInvalid)
        {
          return CCL_SYSVARNOTDEFINED;
        }
        else if (rc==kVIA_BufferToSmall)
        {
          return CCL_BUFFERTOSMALL;
        }
        else
        {
          return CCL_INTERNALERROR;
        }
      }
      for (int i=0; i<length; i++)
      {
        x[i] = static_cast<long>(buffer[i]);
      }
    }
    else
    {
      return CCL_WRONGTYPE;
    }
  }
  else
  {
    VIAResult rc = cclSysVar->mViaSysVar->GetIntegerArray(x, length);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else if (rc==kVIA_BufferToSmall)
      {
        return CCL_BUFFERTOSMALL;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarSetFloatArray(int sysVarID, const double* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVFloatArray, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  int32 size;
  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetArraySize(&size);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetArraySize(&size);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }
  if (length!=size)
  {
    return CCL_WRONGARRAYSIZE;
  }

  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetFloatArray(x, length, NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetFloatArray(x, length, NULL);
  }
  
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetFloatArray(int sysVarID, double* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVFloatArray, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->GetFloatArray(x, length);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->GetFloatArray(x, length);
  }

  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else if (rc==kVIA_BufferToSmall)
    {
      return CCL_BUFFERTOSMALL;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarSetData(int sysVarID, const unsigned char* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  if (gVIAMinorVersion<42)
  {
    return CCL_NOTIMPLEMENTED;
  }

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVData, true);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    rc = cclSysVar->mViaSysVarMember->SetData(x, length, NULL);
  }
  else
  {
    rc = cclSysVar->mViaSysVar->SetData(x, length, NULL);
  }
  if (rc!=kVIA_OK)
  {
    if (rc==kVIA_ObjectInvalid)
    {
      return CCL_SYSVARNOTDEFINED;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  return CCL_SUCCESS;
}


int cclSysVarGetData(int sysVarID, unsigned char* x, int length)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  if (gVIAMinorVersion<42)
  {
    return CCL_NOTIMPLEMENTED;
  }

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar, kVIA_SVData, false);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  int32 bytesCopied;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    int32 arraySize=0;
    VIAResult rc = cclSysVar->mViaSysVarMember->GetArraySize(&arraySize);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
    if (arraySize>length)
    {
      return CCL_BUFFERTOSMALL;
    }
    rc = cclSysVar->mViaSysVarMember->GetData(x, arraySize);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
    bytesCopied = arraySize;
  }
  else
  {
    VIAResult rc = cclSysVar->mViaSysVar->GetData(x, length, &bytesCopied);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else if (rc==kVIA_BufferToSmall)
      {
        return CCL_BUFFERTOSMALL;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }

  return bytesCopied;
}



int cclSysVarSetPhysical(int sysVarID, double x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
    result = cclSysVar->CheckWriteable();
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIASysVarType varType;
    rc = cclSysVar->mViaSysVarMember->GetType(&varType);
    if (rc!=kVIA_OK)
    {
      return CCL_INTERNALERROR;
    }
    if (varType!=kVIA_SVInteger && varType!=kVIA_SVFloat)
    {
      return CCL_WRONGTYPE;
    }

    rc = cclSysVar->mViaSysVarMember->SetPhysicalValue(x, NULL);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }
  else
  {
    return CCL_NOTIMPLEMENTED;  // physical value is only implemented for member of struct 
  }

  return CCL_SUCCESS;
}


int cclSysVarGetPhysical(int sysVarID, double* x)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSysVar* cclSysVar = NULL;
  {
    int result = VSysVar::LookupVariable(sysVarID, cclSysVar);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc;
  if (cclSysVar->mViaSysVarMember!=NULL)
  {
    VIASysVarType varType;
    rc = cclSysVar->mViaSysVarMember->GetType(&varType);
    if (rc!=kVIA_OK)
    {
      return CCL_INTERNALERROR;
    }
    if (varType!=kVIA_SVInteger && varType!=kVIA_SVFloat)
    {
      return CCL_WRONGTYPE;
    }

    rc = cclSysVar->mViaSysVarMember->GetPhysicalValue(x);
    if (rc!=kVIA_OK)
    {
      if (rc==kVIA_ObjectInvalid)
      {
        return CCL_SYSVARNOTDEFINED;
      }
      else
      {
        return CCL_INTERNALERROR;
      }
    }
  }
  else
  {
    return CCL_NOTIMPLEMENTED;  // physical value is only implemented for member of struct 
  }

  return CCL_SUCCESS;
}

#pragma endregion

#pragma region Signals

// ============================================================================
// Signal
// ============================================================================

int cclSignalGetID(char* signalname)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)
  if (gModule==NULL)
  {
    return CCL_INTERNALERROR;
  }
  if (gSignalAccessAPI==NULL)
  {
    return CCL_NOTIMPLEMENTED;
  }
  if (signalname==NULL && strlen(signalname)==0)
  {
    return CCL_INVALIDNAME;
  }

  VIASignal* viaSignal=NULL;
  VIAResult rc = gSignalAccessAPI->GetSignalByName(&viaSignal, signalname);
  if (rc!=kVIA_OK || viaSignal==NULL)
  {
    if (rc==kVIA_ObjectNotFound)
    {
      return CCL_SIGNALNOTDEFINED;
    }
    else if (rc==kVIA_SignalAmbiguous)
    {
      return CCL_SIGNALAMBIGUOUS;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  VSignal* cclSignal = new VSignal;
  cclSignal->mViaSignal = viaSignal;
  cclSignal->mSignalID = static_cast<int>(gModule->mSignal.size());
  gModule->mSignal.push_back(cclSignal);

  return cclSignal->mSignalID;
}


extern int cclSignalSetHandler(int signalID, void (*function)(int signalID) )
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  cclSignal->mCallbackFunction = function;
  VIAOnSignal* sink = (cclSignal->mCallbackFunction == NULL) ? NULL : cclSignal;
  VIAResult rc = cclSignal->mViaSignal->SetSinkOnChangeOnly(sink, NULL);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclSignalGetRxPhysDouble(int signalID, double* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetPhysicalValue(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_ObjectInvalid:
    return CCL_NOTIMPLEMENTED; // signal has no physical value
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalGetRxRawInteger(int signalID, long long* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetRawValueInt64(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalGetRxRawDouble (int signalID, double* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetRawValueDouble(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalSetTxPhysDouble(int signalID, double value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement)

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->SetPhysicalValue(value);
  switch (rc)
  {  
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_ObjectInvalid:
    return CCL_NOTIMPLEMENTED; // signal is read only or has no physical value
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalSetTxRawInteger(int signalID, long long value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->SetRawValueInt64(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalSetTxRawDouble(int signalID, double value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->SetRawValueDouble(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}



int cclSignalGetTxPhysDouble(int signalID, double* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetTxPhysicalValue(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  case kVIA_ObjectInvalid:
    return CCL_NOTIMPLEMENTED; // signal is read only or has no physical value
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalGetTxRawInteger(int signalID, long long* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetTxRawValueInt64(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}


int cclSignalGetTxRawDouble(int signalID, double* value)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement|eStopMeasurement);

  VSignal* cclSignal = NULL;
  {
    int result = VSignal::LookupSignal(signalID, cclSignal);
    if (result!=CCL_SUCCESS)
    {
      return result;
    }
  }

  VIAResult rc = cclSignal->mViaSignal->GetTxRawValueDouble(value);
  switch (rc)
  {
  case kVIA_OK:
    return CCL_SUCCESS;
  case kVIA_WrongSignalType:
    return CCL_WRONGTYPE;
  default:
    return CCL_INTERNALERROR;
  }
}

#pragma endregion

#pragma region CAN Messages

// ============================================================================
// CAN 
// ============================================================================


int cclCanOutputMessage(int channel, unsigned int identifier, unsigned int flags, unsigned char dataLength, const unsigned char data[])
{
  CCL_STATECHECK(eRunMeasurement)

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  VIACan* canBus = gCanBusContext[channel].mBus;
  if (canBus == NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  uint32 viaFlags =
    ((flags & CCL_CANFLAGS_RTR) ? kVIA_CAN_RemoteFrame : 0) |
    ((flags & CCL_CANFLAGS_WAKEUP) ? kVIA_CAN_Wakeup : 0) |
    ((flags & CCL_CANFLAGS_FDF) ? kVIA_CAN_EDL : 0) |
    ((flags & CCL_CANFLAGS_BRS) ? kVIA_CAN_BRS : 0);
  uint8 txReqCount = 0;  // value zero means no limitation of transmit attempts
  VIAResult rc = canBus->OutputMessage3(channel, identifier, viaFlags, txReqCount, dataLength, data);
  if (rc != kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclCanSetMessageHandler(int channel, unsigned int identifier, void (*function) (cclCanMessage* message) )
{
  CCL_STATECHECK(eInitMeasurement)

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gCanBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (function==NULL)
  {
    return CCL_INVALIDFUNCTIONPOINTER;
  }

  uint8 requestType = (identifier==CCL_CAN_ALLMESSAGES) ? kVIA_AllId : kVIA_OneId;

  VCanMessageRequest* request = new VCanMessageRequest;
  request->mCallbackFunction = function;
  request->mContext = &gCanBusContext[channel];
  request->mHandle = NULL;

  VIAResult rc = gCanBusContext[channel].mBus->CreateMessageRequest3(&(request->mHandle), request, requestType, identifier, channel, 0);
  if (rc!=kVIA_OK)
  {
    delete request;
    if (rc==kVIA_ServiceNotRunning)
    {
      return CCL_WRONGSTATE;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  gModule->mCanMessageRequests.push_back(request);
  return CCL_SUCCESS;
}



unsigned int cclCanMakeExtendedIdentifier(unsigned int identifier)
{
  return identifier | 0x80000000;
}


unsigned int cclCanMakeStandardIdentifier(unsigned int identifier)
{
  return identifier & 0x7FFFFFFF;
}


unsigned int cclCanValueOfIdentifier(unsigned int identifier)
{
  return identifier & 0x7FFFFFFF;
}


int cclCanIsExtendedIdentifier(unsigned int identifier)
{
  return ((identifier & 0x80000000) == 0x80000000) ? 1 : 0;
}


int cclCanIsStandardIdentifier(unsigned int identifier)
{
  return ((identifier & 0x80000000) == 0x00000000) ? 1 : 0;
}


int cclCanGetChannelNumber(const char* networkName, int* channel)
{
  if (!gHasBusNames)
  {
    return CCL_NOTIMPLEMENTED;
  }

  if (networkName == nullptr || strlen(networkName) == 0)
  {
    return CCL_INVALIDNAME;
  }

  for (int i = 1; i <= cMaxChannel; i++)
  {
    if (gCanBusContext[i].mBusName == networkName)
    {
      if (channel != nullptr)
      {
        *channel = i;
      }
      return CCL_SUCCESS;
    }
  }

  return CCL_NETWORKNOTDEFINED; // There is no CAN network with the given name.
}

#pragma endregion

#pragma region LIN Frames

// ============================================================================
// LIN 
// ============================================================================


int cclLinSetFrameHandler(int channel, 
                          unsigned int identifier, 
                          void (*function) (struct cclLinFrame* frame) )
{
  CCL_STATECHECK(eInitMeasurement)

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (function==NULL)
  {
    return CCL_INVALIDFUNCTIONPOINTER;
  }

  uint8 requestType = (identifier==CCL_LIN_ALLMESSAGES) ? kVIA_AllId : kVIA_OneId;

  VLinMessageRequest* request = new VLinMessageRequest;
  request->mCallbackFunction = function;
  request->mContext = &gLinBusContext[channel];
  request->mHandle = NULL;

  VIAResult rc = gLinBusContext[channel].mBus->CreateMessageRequest2(&(request->mHandle), request, requestType, identifier, channel);
  if (rc!=kVIA_OK)
  {
    delete request;
    if (rc==kVIA_ServiceNotRunning)
    {
      return CCL_WRONGSTATE;
    }
    else
    {
      return CCL_INTERNALERROR;
    }
  }

  gModule->mLinMessageRequests.push_back(request);
  return CCL_SUCCESS;
}


int cclLinUpdateResponseData(int           channel,
                             unsigned int  id,
                             unsigned char dlc,
                             unsigned char data[8])
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement);

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  uint32 flags = kVIA_ReconfigureData;
  int32 response = -2;
  VIAResult rc = gLinBusContext[channel].mBus->OutputMessage(channel, id, flags, dlc, data, response);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


extern int cclLinSendHeader(int channel, unsigned int id)
{
  CCL_STATECHECK(eRunMeasurement);

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  uint32 flags = kVIA_ApplyHeader;
  int32 response = 0;
  uint8 dlc =0;
  uint8 data[8];
  VIAResult rc = gLinBusContext[channel].mBus->OutputMessage(channel, id, flags, dlc, data, response);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclLinStartScheduler(int channel)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement);

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  VIAResult rc = gLinBusContext[channel].mBus->LINStartScheduler(channel);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclLinStopScheduler(int channel)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement);

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  VIAResult rc = gLinBusContext[channel].mBus->LINStopScheduler(channel);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclLinChangeSchedtable(int channel, unsigned int tableIndex)
{
  CCL_STATECHECK(eInitMeasurement|eRunMeasurement);

  if (channel<1 && channel>cMaxChannel)
  {
    return CCL_INVALIDCHANNEL;
  }

  if (gLinBusContext[channel].mBus==NULL)
  {
    return CCL_INVALIDCHANNEL;
  }

  VIAResult rc = gLinBusContext[channel].mBus->LINChangeSchedtableEx(channel, tableIndex);
  if (rc!=kVIA_OK)
  {
    return CCL_INTERNALERROR;
  }

  return CCL_SUCCESS;
}


int cclLinGetChannelNumber(const char* networkName, int* channel)
{
  if (!gHasBusNames)
  {
    return CCL_NOTIMPLEMENTED;
  }

  if (networkName == nullptr || strlen(networkName) == 0)
  {
    return CCL_INVALIDNAME;
  }

  for (int i = 1; i <= cMaxChannel; i++)
  {
    if (gLinBusContext[i].mBusName == networkName)
    {
      if (channel != nullptr)
      {
        *channel = i;
      }
      return CCL_SUCCESS;
    }
  }

  return CCL_NETWORKNOTDEFINED; // There is no LIN network with the given name.
}

#pragma endregion

#pragma region Function Bus Value Entities

cclValueID cclValueGetID(cclDomain domain, const char* path, const char* member, cclValueRepresentation repr)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  if (domain != CCL_COMMUNICATION_OBJECTS)
    return CCL_INVALIDDOMAIN;

  return VValueEntity::CreateValueEntity(path, member, repr);
}

int cclValueGetInteger(cclValueID valueID, long long* x)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetInteger(x);
}

int cclValueSetInteger(cclValueID valueID, long long x)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->SetInteger(x);
}

int cclValueGetFloat(cclValueID valueID, double* x)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetFloat(x);
}

int cclValueSetFloat(cclValueID valueID, double x)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->SetFloat(x);
}

int cclValueGetString(cclValueID valueID, char* buffer, int bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetString(buffer, bufferSize);
}

int cclValueSetString(cclValueID valueID, const char* str)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->SetString(str);
}

int cclValueGetData(cclValueID valueID, unsigned char* buffer, int* bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetData(buffer, bufferSize);
}

int cclValueSetData(cclValueID valueID, const unsigned char* data, int size)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->SetData(data, size);
}

int cclValueGetEnum(cclValueID valueID, int* x)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetEnum(x);
}

int cclValueSetEnum(cclValueID valueID, int x)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->SetEnum(x);
}

int cclValueGetUnionSelector(cclValueID valueID, int* selector)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetUnionSelector(selector);
}

int cclValueGetArraySize(cclValueID valueID, int* size)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetArraySize(size);
}

int cclValueSetArraySize(cclValueID valueID, int size)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  if (size < 0)
    return CCL_PARAMETERINVALID;

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->SetArraySize(size);
}

int cclValueGetType(cclValueID valueID, cclValueType* valueType)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetValueType(valueType);
}

int cclValueIsValid(cclValueID valueID, bool* isValid)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->IsValid(isValid);
}

int cclValueClear(cclValueID valueID)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->ClearValue();
}

int cclValueGetUpdateTimeNS(cclValueID valueID, cclTime* time)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetUpdateTimeNS(time);
}

int cclValueGetChangeTimeNS(cclValueID valueID, cclTime* time)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetChangeTimeNS(time);
}

int cclValueGetState(cclValueID valueID, cclValueState* state)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->GetState(state);
}

int cclValueSetHandler(cclValueID valueID, bool onUpdate, cclValueHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IValue* valueEntity = nullptr;
  if (IValue::LookupValue(valueID, valueEntity) != CCL_SUCCESS)
    return CCL_INVALIDVALUEID;

  return valueEntity->SetHandler(onUpdate, handler);
}

#pragma endregion

#pragma region Function Bus Functions and Call Contexts

cclFunctionID cclFunctionGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  bool isClient = ::strstr(path, ".consumerSide") != nullptr;

  VIAFbViaService* fbAPI = GetFunctionBusAPI();
  if (isClient)
    return VClientFunction::CreateClientFunction(path);
  else
    return VServerFunction::CreateServerFunction(path);
}

int cclFunctionSetHandler(cclFunctionID functionID, cclCallState callState, cclCallHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  IFunctionPort* function = nullptr;
  if (VFunctionUtils::LookupFunction(functionID, function) != CCL_SUCCESS)
    return CCL_INVALIDFUNCTIONID;

  VIAFbFunctionCallState mappedState;
  int callbackIndex = VFunctionUtils::MapCallState(callState, mappedState);
  if ((callbackIndex < 0) || (callbackIndex > 3))
    return CCL_PARAMETERINVALID;

  return function->SetHandler(callbackIndex, mappedState, handler);
}

cclCallContextID cclCreateCallContext(cclFunctionID functionID)
{
  CCL_STATECHECK(eRunMeasurement);

  IFunctionPort* function = nullptr;
  if (VFunctionUtils::LookupFunction(functionID, function) != CCL_SUCCESS)
    return CCL_INVALIDFUNCTIONID;

  if (function->GetSide() != cclSide::CCL_SIDE_CONSUMER)
    return CCL_INVALIDSIDE;

  VClientFunction* clientFunction = dynamic_cast<VClientFunction*>(function);
  if (clientFunction == nullptr)
    return CCL_INTERNALERROR;

  VIAFbCallContext* ctxt;
  if (clientFunction->mPort->BeginCall(&ctxt) != kVIA_OK)
    return CCL_INTERNALERROR;

  return clientFunction->CreateCallContext(ctxt);
}

cclCallContextID cclCallContextMakePermanent(cclCallContextID ccID)
{
  CCL_STATECHECK(eRunMeasurement);

  VCallContext* cc = gModule->mCallContexts.Get(ccID);
  if (cc == nullptr)
    return CCL_INVALIDCALLCONTEXTID;

  IFunctionPort* function = nullptr;
  if (VFunctionUtils::LookupFunction(cc->mFunctionID, function) != CCL_SUCCESS)
    return CCL_INTERNALERROR;

  VIAFbCallContext* ctxt;
  if (cc->mContext->MakePermanent(&ctxt) != kVIA_OK)
    return CCL_INTERNALERROR;

  return VFunctionUtils::CreateCallContext(ctxt, function->GetFunctionID(), true);
}

int cclCallContextRelease(cclCallContextID ccID)
{
  CCL_STATECHECK(eRunMeasurement | eStopMeasurement);

  return VFunctionUtils::DestroyCallContext(ccID) ? CCL_SUCCESS : CCL_INVALIDCALLCONTEXTID;
}

int cclCallAsync(cclCallContextID ccID, cclCallHandler resultHandler)
{
  CCL_STATECHECK(eRunMeasurement);

  VCallContext* cc = gModule->mCallContexts.Get(ccID);
  if (cc == nullptr)
    return CCL_INVALIDCALLCONTEXTID;

  IFunctionPort* function = nullptr;
  if (VFunctionUtils::LookupFunction(cc->mFunctionID, function) != CCL_SUCCESS)
    return CCL_INTERNALERROR;

  if (function->GetSide() != cclSide::CCL_SIDE_CONSUMER)
    return CCL_INVALIDSIDE;

  VIAFbFunctionCallState state;
  if (cc->mContext->GetCallState(&state) != CCL_SUCCESS)
    return CCL_INTERNALERROR;

  if (state != VIAFbFunctionCallState::eCalling)
    return CCL_INVALIDCALLCONTEXTSTATE;

  VClientFunction* clientFunction = dynamic_cast<VClientFunction*>(function);
  if (clientFunction == nullptr)
    return CCL_INTERNALERROR;

  cc->mResultCallbackFunction = resultHandler;
  if (clientFunction->mPort->InvokeCall(cc, cc->mContext) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

cclValueID cclCallContextValueGetID(cclCallContextID ccID, const char* member, cclValueRepresentation repr)
{
  CCL_STATECHECK(eRunMeasurement);

  VCallContext* cc = gModule->mCallContexts.Get(ccID);
  if (cc == nullptr)
    return CCL_INVALIDCALLCONTEXTID;

  cclValueID valueID = VCallContextValue::CreateCallContextValue(cc->mContextID, member, repr);
  if (valueID >= 0) cc->mContextValues.push_back(valueID);
  return valueID;
}

int cclCallContextReturn(cclCallContextID ccID, cclTime timeOffset)
{
  CCL_STATECHECK(eRunMeasurement);

  VCallContext* cc = gModule->mCallContexts.Get(ccID);
  if (cc == nullptr)
    return CCL_INVALIDCALLCONTEXTID;

  VIAFbFunctionCallState callState;
  if (cc->mContext->GetCallState(&callState) != kVIA_OK)
    return CCL_INTERNALERROR;

  if ((callState != VIAFbFunctionCallState::eCalled) && (callState != VIAFbFunctionCallState::eCalling))
    return CCL_INVALIDCALLCONTEXTSTATE;

  VIAFbFunctionCallSide side;
  if (cc->mContext->GetSide(&side) != kVIA_OK)
    return CCL_INTERNALERROR;

  if (side != VIAFbFunctionCallSide::eServer)
    return CCL_INVALIDSIDE;

  if ((timeOffset < 0) && (timeOffset != CCL_TIME_OFFSET_NEVER))
    return CCL_INVALIDTIME;

  if (cc->mContext->SetTimeToReply(timeOffset) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclCallContextSetDefaultAnswer(cclCallContextID ccID)
{
  CCL_STATECHECK(eRunMeasurement);

  VCallContext* cc = gModule->mCallContexts.Get(ccID);
  if (cc == nullptr)
    return CCL_INVALIDCALLCONTEXTID;

  VIAFbFunctionCallState callState;
  if (cc->mContext->GetCallState(&callState) != kVIA_OK)
    return CCL_INTERNALERROR;

  if ((callState != VIAFbFunctionCallState::eCalled) && (callState != VIAFbFunctionCallState::eCalling))
    return CCL_INVALIDCALLCONTEXTSTATE;

  VIAFbFunctionCallSide side;
  if (cc->mContext->GetSide(&side) != kVIA_OK)
    return CCL_INTERNALERROR;

  if (side != VIAFbFunctionCallSide::eServer)
    return CCL_INVALIDSIDE;

  if (cc->mContext->SetDefaultAnswer() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclCallContextGetFunctionID(cclCallContextID ccID, cclFunctionID* functionID)
{
  CCL_STATECHECK(eRunMeasurement);

  VCallContext* cc = gModule->mCallContexts.Get(ccID);
  if (cc == nullptr)
    return CCL_INVALIDCALLCONTEXTID;

  *functionID = cc->mFunctionID;
  return CCL_SUCCESS;
}

int cclCallContextGetFunctionName(cclCallContextID ccID, char* buffer, int bufferSize)
{
  CCL_STATECHECK(eRunMeasurement);

  VCallContext* cc = gModule->mCallContexts.Get(ccID);
  if (cc == nullptr)
    return CCL_INVALIDCALLCONTEXTID;

  IFunctionPort* function = nullptr;
  if (VFunctionUtils::LookupFunction(cc->mFunctionID, function) != CCL_SUCCESS)
    return CCL_INTERNALERROR;

  size_t start = std::string::npos;
  size_t len = 0;

  const std::string& path = function->GetPath();

  if (path.back() == ']')
  {
    // free function "ValueEntities::TheFunction.providerSide[Client1,Server1]";
    start = path.rfind(':');
    if (start == std::string::npos)
      start = 0; // no namespace
    else
      ++start;

    size_t end = path.find('.', start);
    if (end == std::string::npos)
      return CCL_INTERNALERROR;

    len = end - start;
  }
  else
  {
    // service function "ValueEntities::TheService.providerSide[Client1,Server1].ServiceFunction";
    start = path.rfind('.');
    if (start == std::string::npos)
      return CCL_INTERNALERROR;
    ++start;

    len = path.length() - start;
  }

  if (bufferSize <= (int)len)
    return CCL_BUFFERTOSMALL;

  if (strncpy_s(buffer, bufferSize, path.c_str() + start, len) != 0)
    return CCL_INTERNALERROR;
  buffer[len] = '\0';

  return CCL_SUCCESS;
}

int cclCallContextGetFunctionPath(cclCallContextID ccID, char* buffer, int bufferSize)
{
  CCL_STATECHECK(eRunMeasurement);

  VCallContext* cc = gModule->mCallContexts.Get(ccID);
  if (cc == nullptr)
    return CCL_INVALIDCALLCONTEXTID;

  IFunctionPort* function = nullptr;
  if (VFunctionUtils::LookupFunction(cc->mFunctionID, function) != CCL_SUCCESS)
    return CCL_INTERNALERROR;

  const std::string& path = function->GetPath();

  size_t len = path.length();
  if (bufferSize <= (int)len)
    return CCL_BUFFERTOSMALL;

  if (strncpy_s(buffer, bufferSize, path.c_str(), len) != 0)
    return CCL_INTERNALERROR;
  buffer[len] = '\0';

  return CCL_SUCCESS;
}

#pragma endregion

#pragma region Services

cclServiceID cclServiceGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VService::CreateService(path);
}

int cclServiceSetSimulator(cclProvidedServiceID providedServiceID, cclCallHandler simulator)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VProvidedService* providedService;
  int result = VProvidedService::LookupProvidedService(providedServiceID, providedService);
  if (result != CCL_SUCCESS)
    return result;

  return providedService->SetSimulator(simulator);
}

cclConsumerID cclAddConsumerByName(cclServiceID serviceID, const char* name, bool isSimulated)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbServiceConsumer* consumer;
  VIAFbFEPState fepState = isSimulated ? VIAFbFEPState::eSimulated : VIAFbFEPState::eReal;
  if (service->mService->AddConsumerByName(name, fepState, &consumer) != kVIA_OK)
    return CCL_CANNOTADDDYNAMICCONSUMER;

  return VConsumer::CreateConsumer(consumer, true);
}

cclProviderID cclAddProviderByName(cclServiceID serviceID, const char* name, bool isSimulated)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbServiceProvider* provider;
  VIAFbFEPState fepState = isSimulated ? VIAFbFEPState::eSimulated : VIAFbFEPState::eReal;
  if (service->mService->AddProviderByName(name, fepState, &provider) != kVIA_OK)
    return CCL_CANNOTADDDYNAMICPROVIDER;

  return VProvider::CreateProvider(provider, true);
}

cclConsumerID cclAddConsumerByAddress(cclServiceID serviceID, cclAddressID addressID)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VAddress* address;
  result = VAddress::LookupAddress(addressID, address);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbServiceConsumer* consumer;
  if (service->mService->AddConsumerByAddress(address->mAddress, &consumer) != kVIA_OK)
    return CCL_CANNOTADDDYNAMICCONSUMER;

  return VConsumer::CreateConsumer(consumer, true);
}

cclProviderID cclAddProviderByAddress(cclServiceID serviceID, cclAddressID addressID)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VAddress* address;
  result = VAddress::LookupAddress(addressID, address);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbServiceProvider* provider;
  if (service->mService->AddProviderByAddress(address->mAddress, &provider) != kVIA_OK)
    return CCL_CANNOTADDDYNAMICPROVIDER;

  return VProvider::CreateProvider(provider, true);
}

int cclRemoveConsumer(cclServiceID serviceID, cclConsumerID consumerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VConsumer* consumer;
  result = VConsumer::LookupConsumer(consumerID, consumer);
  if (result != CCL_SUCCESS)
    return result;

  if (service->mService->RemoveConsumer(consumer->mPort) != kVIA_OK)
    return CCL_STATICCONSUMER;

  // RemoveConsumer callback implicitly releases the VIA Object and C API object
  return CCL_SUCCESS;
}

int cclRemoveProvider(cclServiceID serviceID, cclProviderID providerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VProvider* provider;
  result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  if (service->mService->RemoveProvider(provider->mPort) != kVIA_OK)
    return CCL_STATICPROVIDER;

  // RemoveProvider callback implicitly releases the VIA Object and C API object
  return CCL_SUCCESS;
}

cclConsumerID cclGetConsumer(cclServiceID serviceID, long bindingID)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbServiceConsumer* consumer;
  if (service->mService->GetConsumer(bindingID, &consumer) != kVIA_OK)
    return CCL_CONSUMERNOTFOUND;

  return VConsumer::CreateConsumer(consumer, true);
}

cclProviderID cclGetProvider(cclServiceID serviceID, long bindingID)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbServiceProvider* provider;
  if (service->mService->GetProvider(bindingID, &provider) != kVIA_OK)
    return CCL_PROVIDERNOTFOUND;

  return VProvider::CreateProvider(provider, true);
}

cclConsumedServiceID cclGetConsumedService(cclServiceID serviceID, long consumerID, long providerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbServiceConsumer* consumer;
  if (service->mService->GetConsumer(consumerID, &consumer) != kVIA_OK)
    return CCL_CONSUMERNOTFOUND;

  VIAFbServiceProvider* provider;
  if (service->mService->GetProvider(providerID, &provider) != kVIA_OK)
    return CCL_PROVIDERNOTFOUND;

  VIAFbConsumedService* consumedService;
  if (service->mService->GetConsumedService(consumerID, providerID, &consumedService) != kVIA_OK)
    return CCL_INTERNALERROR;

  return VConsumedService::CreateConsumedService(consumedService);
}

cclProvidedServiceID cclGetProvidedService(cclServiceID serviceID, long consumerID, long providerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbServiceConsumer* consumer;
  if (service->mService->GetConsumer(consumerID, &consumer) != kVIA_OK)
    return CCL_CONSUMERNOTFOUND;

  VIAFbServiceProvider* provider;
  if (service->mService->GetProvider(providerID, &provider) != kVIA_OK)
    return CCL_PROVIDERNOTFOUND;

  VIAFbProvidedService* providedService;
  if (service->mService->GetProvidedService(consumerID, providerID, &providedService) != kVIA_OK)
    return CCL_INTERNALERROR;

  return VProvidedService::CreateProvidedService(providedService);
}

cclConsumerID cclConsumerGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VConsumer::CreateConsumer(path);
}

cclProviderID cclProviderGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VProvider::CreateProvider(path);
}

int cclProvideService(cclProviderID providerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvider* provider;
  int result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  if (provider->mPort->ProvideService() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclReleaseService(cclProviderID providerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvider* provider;
  int result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  if (provider->mPort->ReleaseService() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclIsServiceProvided(cclProviderID providerID, bool* isProvided)
{
  CCL_STATECHECK(eRunMeasurement);

  if (isProvided == nullptr)
    return CCL_PARAMETERINVALID;

  VProvider* provider;
  int result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  if (provider->mPort->IsServiceProvided(isProvided) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclConsumerGetBindingID(cclConsumerID consumerID, long* bindingID)
{
  CCL_STATECHECK(eRunMeasurement);

  if (bindingID == nullptr)
    return CCL_PARAMETERINVALID;

  VConsumer* consumer;
  int result = VConsumer::LookupConsumer(consumerID, consumer);
  if (result != CCL_SUCCESS)
    return result;

  if (consumer->mPort->GetBindingId(bindingID) != kVIA_OK)
    return CCL_CONSUMERNOTBOUND;

  return CCL_SUCCESS;
}

int cclProviderGetBindingID(cclProviderID providerID, long* bindingID)
{
  CCL_STATECHECK(eRunMeasurement);

  if (bindingID == nullptr)
    return CCL_PARAMETERINVALID;

  VProvider* provider;
  int result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  if (provider->mPort->GetBindingId(bindingID) != kVIA_OK)
    return CCL_PROVIDERNOTBOUND;

  return CCL_SUCCESS;
}

cclConsumedServiceID cclConsumedServiceGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VConsumedService::CreateConsumedService(path);
}

cclProvidedServiceID cclProvidedServiceGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VProvidedService::CreateProvidedService(path);
}

cclValueID cclConsumedServiceValueGetID(cclConsumedServiceID consumedServiceID, const char* member, cclValueRepresentation repr)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VConsumedService* consumedService;
  int result = VConsumedService::LookupConsumedService(consumedServiceID, consumedService);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbValuePort* valuePort;
  if (consumedService->mPort->GetValuePort(&valuePort) != kVIA_OK)
    return CCL_INTERNALERROR;

  return VValueEntity::CreateValueEntity(valuePort, member, repr);
}

cclValueID cclProvidedServiceValueGetID(cclProvidedServiceID providedServiceID, const char* member, cclValueRepresentation repr)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VProvidedService* providedService;
  int result = VProvidedService::LookupProvidedService(providedServiceID, providedService);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbValuePort* valuePort;
  if (providedService->mPort->GetValuePort(&valuePort) != kVIA_OK)
    return CCL_INTERNALERROR;

  return VValueEntity::CreateValueEntity(valuePort, member, repr);
}

int cclRequestService(cclConsumedServiceID consumedServiceID)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedService* consumedService;
  int result = VConsumedService::LookupConsumedService(consumedServiceID, consumedService);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedService->mPort->RequestService() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclReleaseServiceRequest(cclConsumedServiceID consumedServiceID)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedService* consumedService;
  int result = VConsumedService::LookupConsumedService(consumedServiceID, consumedService);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedService->mPort->ReleaseService() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclIsServiceRequested(cclConsumedServiceID consumedServiceID, bool* isRequested)
{
  CCL_STATECHECK(eRunMeasurement);

  if (isRequested == nullptr)
    return CCL_PARAMETERINVALID;

  VConsumedService* consumedService;
  int result = VConsumedService::LookupConsumedService(consumedServiceID, consumedService);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedService->mPort->IsServiceRequested(isRequested) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclDiscoverProviders(cclConsumerID consumerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumer* consumer;
  int result = VConsumer::LookupConsumer(consumerID, consumer);
  if (result != CCL_SUCCESS)
    return result;

  if (consumer->mPort->DiscoverProviders() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAnnounceProvider(cclProviderID providerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvider* provider;
  int result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  if (provider->mPort->AnnounceProvider() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclUnannounceProvider(cclProviderID providerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvider* provider;
  int result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  if (provider->mPort->UnannounceProvider() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAnnounceProviderToConsumer(cclServiceID serviceID, cclProviderID providerID, cclConsumerID consumerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvider* provider;
  int result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  VConsumer* consumer;
  result = VConsumer::LookupConsumer(consumerID, consumer);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbBindingId providerBindingID;
  VIAFbBindingId consumerBindingID;
  if (provider->mPort->GetBindingId(&providerBindingID) != kVIA_OK)
    return CCL_INVALIDBINDING;
  if (consumer->mPort->GetBindingId(&consumerBindingID) != kVIA_OK)
    return CCL_INVALIDBINDING;

  VService* service;
  result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbProvidedService* providedService;
  if (service->mService->GetProvidedService(consumerBindingID, providerBindingID, &providedService) != kVIA_OK)
    return CCL_INTERNALERROR;

  if (providedService->AnnounceProvider() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclSetConsumerAddress(cclConsumerID consumerID, cclAddressID addressID, cclProviderID providerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumer* consumer;
  int result = VConsumer::LookupConsumer(consumerID, consumer);
  if (result != CCL_SUCCESS)
    return result;

  VAddress* address;
  result = VAddress::LookupAddress(addressID, address);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbServiceProvider* fbProvider = nullptr;
  if (providerID != CCL_PROVIDER_NONE)
  {
    VProvider* provider;
    result = VProvider::LookupProvider(providerID, provider);
    if (result != CCL_SUCCESS)
      return result;

    fbProvider = provider->mPort;
  }

  if (consumer->mPort->SetAddress(address->mAddress, fbProvider) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclSetProviderAddress(cclProviderID providerID, cclAddressID addressID, cclConsumerID consumerID)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvider* provider;
  int result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  VAddress* address;
  result = VAddress::LookupAddress(addressID, address);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbServiceConsumer* fbConsumer = nullptr;
  if (consumerID != CCL_CONSUMER_NONE)
  {
    VConsumer* consumer;
    result = VConsumer::LookupConsumer(consumerID, consumer);
    if (result != CCL_SUCCESS)
      return result;

    fbConsumer = consumer->mPort;
  }

  if (provider->mPort->SetAddress(address->mAddress, fbConsumer) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclConnectAsyncConsumer(cclConsumedServiceID consumedServiceID, cclConnectionEstablishedHandler success, cclConnectionFailedHandler failure)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedService* consumedService;
  int result = VConsumedService::LookupConsumedService(consumedServiceID, consumedService);
  if (result != CCL_SUCCESS)
    return result;

  return consumedService->ConnectAsync(success, failure);
}

int cclConnectAsyncProvider(cclProvidedServiceID providedServiceID, cclConnectionEstablishedHandler success, cclConnectionFailedHandler failure)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvidedService* providedService;
  int result = VProvidedService::LookupProvidedService(providedServiceID, providedService);
  if (result != CCL_SUCCESS)
    return result;

  return providedService->ConnectAsync(success, failure);
}

int cclDisconnectConsumer(cclConsumedServiceID consumedServiceID)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedService* consumedService;
  int result = VConsumedService::LookupConsumedService(consumedServiceID, consumedService);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedService->mPort->Disconnect() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclDisconnectProvider(cclProvidedServiceID providedServiceID)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvidedService* providedService;
  int result = VProvidedService::LookupProvidedService(providedServiceID, providedService);
  if (result != CCL_SUCCESS)
    return result;

  if (providedService->mPort->Disconnect() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclSetConsumerDiscoveredHandler(cclProviderID providerID, cclConsumerDiscoveredHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VProvider* provider;
  int result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  return provider->SetConsumerDiscoveredHandler(handler);
}

int cclSetProviderDiscoveredHandler(cclConsumerID consumerID, cclProviderDiscoveredHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VConsumer* consumer;
  int result = VConsumer::LookupConsumer(consumerID, consumer);
  if (result != CCL_SUCCESS)
    return result;

  return consumer->SetProviderDiscoveredHandler(handler);
}

int cclSetConnectionRequestedHandler(cclProviderID providerID, cclConnectionRequestedHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VProvider* provider;
  int result = VProvider::LookupProvider(providerID, provider);
  if (result != CCL_SUCCESS)
    return result;

  return provider->SetConnectionRequestedHandler(handler);
}

int cclSetServiceDiscoveryHandler(cclServiceID serviceID, cclServiceDiscoveryHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  return service->SetServiceDiscoveryHandler(handler);
}

int cclSetServiceConsumerDiscoveredHandler(cclServiceID serviceID, cclConsumerDiscoveredHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  return service->SetConsumerDiscoveredHandler(handler);
}

int cclSetServiceProviderDiscoveredHandler(cclServiceID serviceID, cclProviderDiscoveredHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  return service->SetProviderDiscoveredHandler(handler);
}

#pragma endregion

#pragma region PDU Signal access

cclPDUSenderID cclPDUSenderGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VPDUSender::CreatePDUSender(path);
}

cclPDUReceiverID cclPDUReceiverGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VPDUReceiver::CreatePDUReceiver(path);
}

cclPDUProviderID cclPDUProviderGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VServicePDUProvider::CreatePDUProvider(path);
}

cclValueID cclPDUSenderSignalValueGetID(cclPDUSenderID pduSenderID, const char* signal, const char* member, cclValueRepresentation repr)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VPDUSender* pduSender;
  int result = VPDUSender::LookupPDUSender(pduSenderID, pduSender);
  if (result != CCL_SUCCESS)
    return result;

  return VPDUSignalValue<VIAFbPDUSenderPort>::CreateSignalValue(pduSender->mPort, signal, member, repr);
}

cclValueID cclPDUReceiverSignalValueGetID(cclPDUReceiverID pduReceiverID, const char* signal, const char* member, cclValueRepresentation repr)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VPDUReceiver* pduReceiver;
  int result = VPDUReceiver::LookupPDUReceiver(pduReceiverID, pduReceiver);
  if (result != CCL_SUCCESS)
    return result;

  return VPDUSignalValue<VIAFbPDUReceiverPort>::CreateSignalValue(pduReceiver->mPort, signal, member, repr);
}

cclValueID cclPDUProviderSignalValueGetID(cclPDUProviderID pduProviderID, const char* signal, const char* member, cclValueRepresentation repr)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VServicePDUProvider* pduProvider;
  int result = VServicePDUProvider::LookupPDUProvider(pduProviderID, pduProvider);
  if (result != CCL_SUCCESS)
    return result;

  return VPDUSignalValue<VIAFbServicePDUProvider>::CreateSignalValue(pduProvider->mPort, signal, member, repr);
}

#pragma endregion

#pragma region General Service PDU Subscription

cclPDUReceiverID cclConsumedPDUGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VConsumedServicePDU::CreateConsumedPDU(path);
}

cclPDUSenderID cclProvidedPDUGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VProvidedServicePDU::CreateProvidedPDU(path);
}

int cclPDUGetNrOfSubscribedConsumers(cclPDUProviderID pduProviderID, int* nrOfConsumers)
{
  CCL_STATECHECK(eRunMeasurement);

  VServicePDUProvider* pduProvider;
  int result = VServicePDUProvider::LookupPDUProvider(pduProviderID, pduProvider);
  if (result != CCL_SUCCESS)
    return result;

  return pduProvider->GetNrOfSubscribedConsumers(nrOfConsumers);
}

int cclPDUGetSubscribedConsumers(cclPDUProviderID pduProviderID, cclConsumerID* consumerBuffer, int* bufferSize)
{
  CCL_STATECHECK(eRunMeasurement);

  VServicePDUProvider* pduProvider;
  int result = VServicePDUProvider::LookupPDUProvider(pduProviderID, pduProvider);
  if (result != CCL_SUCCESS)
    return result;

  return pduProvider->GetSubscribedConsumers(consumerBuffer, bufferSize);
}

int cclProvidedPDUSetSubscriptionStateIsolated(cclPDUSenderID pduSenderID, bool subscribed)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvidedServicePDU* providedPDU;
  int result = VProvidedServicePDU::LookupProvidedPDU(pduSenderID, providedPDU);
  if (result != CCL_SUCCESS)
    return result;

  if (providedPDU->SetSubscriptionStateIsolated(subscribed) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclConsumedPDUSetSubscriptionStateIsolated(cclPDUReceiverID pduReceiverID, bool subscribed)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedServicePDU* consumedPDU;
  int result = VConsumedServicePDU::LookupConsumedPDU(pduReceiverID, consumedPDU);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedPDU->SetSubscriptionStateIsolated(subscribed) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

#pragma endregion

#pragma region General Event Subscription

cclConsumedEventID cclConsumedEventGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VConsumedEvent::CreateConsumedEvent(path);
}

cclProvidedEventID cclProvidedEventGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VProvidedEvent::CreateProvidedEvent(path);
}

cclEventProviderID cclEventProviderGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VEventProvider::CreateEventProvider(path);
}

int cclEventGetNrOfSubscribedConsumers(cclEventProviderID eventProviderID, int* nrOfConsumers)
{
  CCL_STATECHECK(eRunMeasurement);

  VEventProvider* eventProvider;
  int result = VEventProvider::LookupEventProvider(eventProviderID, eventProvider);
  if (result != CCL_SUCCESS)
    return result;

  return eventProvider->GetNrOfSubscribedConsumers(nrOfConsumers);
}

int cclEventGetSubscribedConsumers(cclEventProviderID eventProviderID, cclConsumerID* consumerBuffer, int* bufferSize)
{
  CCL_STATECHECK(eRunMeasurement);

  VEventProvider* eventProvider;
  int result = VEventProvider::LookupEventProvider(eventProviderID, eventProvider);
  if (result != CCL_SUCCESS)
    return result;

  return eventProvider->GetSubscribedConsumers(consumerBuffer, bufferSize);
}

int cclConsumedEventSetSubscriptionStateIsolated(cclConsumedEventID consumedEventID, bool subscribed)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedEvent* consumedEvent;
  int result = VConsumedEvent::LookupConsumedEvent(consumedEventID, consumedEvent);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEvent->SetSubscriptionStateIsolated(subscribed) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclProvidedEventSetSubscriptionStateIsolated(cclProvidedEventID providedEventID, bool subscribed)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvidedEvent* providedEvent;
  int result = VProvidedEvent::LookupProvidedEvent(providedEventID, providedEvent);
  if (result != CCL_SUCCESS)
    return result;

  if (providedEvent->SetSubscriptionStateIsolated(subscribed) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

#pragma endregion

#pragma region Service Field Access

cclConsumedFieldID cclConsumedFieldGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VConsumedField::CreateConsumedField(path);
}

cclProvidedFieldID cclProvidedFieldGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VProvidedField::CreateProvidedField(path);
}

cclFieldProviderID cclFieldProviderGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VFieldProvider::CreateFieldProvider(path);
}

int cclFieldGetNrOfSubscribedConsumers(cclFieldProviderID fieldProviderID, int* nrOfConsumers)
{
  CCL_STATECHECK(eRunMeasurement);

  VFieldProvider* fieldProvider;
  int result = VFieldProvider::LookupFieldProvider(fieldProviderID, fieldProvider);
  if (result != CCL_SUCCESS)
    return result;

  return fieldProvider->GetNrOfSubscribedConsumers(nrOfConsumers);
}

int cclFieldGetSubscribedConsumers(cclFieldProviderID fieldProviderID, cclConsumerID* consumerBuffer, int* bufferSize)
{
  CCL_STATECHECK(eRunMeasurement);

  VFieldProvider* fieldProvider;
  int result = VFieldProvider::LookupFieldProvider(fieldProviderID, fieldProvider);
  if (result != CCL_SUCCESS)
    return result;

  return fieldProvider->GetSubscribedConsumers(consumerBuffer, bufferSize);
}

int cclConsumedFieldSetSubscriptionStateIsolated(cclConsumedFieldID consumedFieldID, bool subscribed)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedField* consumedField;
  int result = VConsumedField::LookupConsumedField(consumedFieldID, consumedField);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedField->SetSubscriptionStateIsolated(subscribed) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclProvidedFieldSetSubscriptionStateIsolated(cclProvidedFieldID providedFieldID, bool subscribed)
{
  CCL_STATECHECK(eRunMeasurement);

  VProvidedField* providedField;
  int result = VProvidedField::LookupProvidedField(providedFieldID, providedField);
  if (result != CCL_SUCCESS)
    return result;

  if (providedField->SetSubscriptionStateIsolated(subscribed) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

#pragma endregion

#pragma region Abstract Binding

cclAddressID cclAbstractCreateAddress(cclServiceID serviceID, const char* endPointName)
{
  CCL_STATECHECK(eRunMeasurement);

  VService* service;
  int result = VService::LookupService(serviceID, service);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbViaService* fbAPI = GetFunctionBusAPI();
  VIAFbAbstractBinding* abstractBinding;
  if (fbAPI->GetAbstractBinding(&abstractBinding) != kVIA_OK)
    return CCL_INTERNALERROR;

  NDetail::VIAObjectGuard<VIAFbAddressHandle> address;
  if (abstractBinding->CreateAddress(service->mService, endPointName, &(address.mVIAObject)) != kVIA_OK)
    return CCL_CANNOTCREATEADDRESS;

  return VAddress::CreateAddress(address.Release(), true);
}

int cclAbstractGetBindingID(cclAddressID addressID, long* bindingID)
{
  CCL_STATECHECK(eRunMeasurement);

  VIAFbViaService* fbAPI = GetFunctionBusAPI();
  VIAFbAbstractBinding* abstractBinding;
  if (fbAPI->GetAbstractBinding(&abstractBinding) != kVIA_OK)
    return CCL_INTERNALERROR;

  VAddress* address;
  int result = VAddress::LookupAddress(addressID, address);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbBindingId fbBindingID;
  if (abstractBinding->GetBindingId(address->mAddress, &fbBindingID) != kVIA_OK)
    return CCL_INVALIDBINDING;

  *bindingID = static_cast<long>(fbBindingID);
  return CCL_SUCCESS;
}

int cclAbstractGetDisplayName(cclAddressID addressID, char* buffer, int bufferSize)
{
  CCL_STATECHECK(eRunMeasurement);

  VIAFbViaService* fbAPI = GetFunctionBusAPI();
  VIAFbAbstractBinding* abstractBinding;
  if (fbAPI->GetAbstractBinding(&abstractBinding) != kVIA_OK)
    return CCL_INTERNALERROR;

  VAddress* address;
  int result = VAddress::LookupAddress(addressID, address);
  if (result != CCL_SUCCESS)
    return result;

  result = abstractBinding->GetDisplayName(address->mAddress, buffer, bufferSize);
  if (result == kVIA_BufferToSmall)
    return CCL_BUFFERTOSMALL;

  return (result == kVIA_OK) ? CCL_SUCCESS : CCL_INTERNALERROR;
}

cclConsumedEventID cclAbstractConsumedEventGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VAbstractConsumedEvent::CreateConsumedEvent(path);
}

cclEventProviderID cclAbstractEventProviderGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VAbstractEventProvider::CreateEventProvider(path);
}

int cclAbstractRequestEvent(cclConsumedEventID consumedEventID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedEvent* consumedEvent;
  int result = VAbstractConsumedEvent::LookupConsumedEvent(consumedEventID, consumedEvent);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEvent->mAbstractConsumedEvent->RequestEvent() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractReleaseEvent(cclConsumedEventID consumedEventID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedEvent* consumedEvent;
  int result = VAbstractConsumedEvent::LookupConsumedEvent(consumedEventID, consumedEvent);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEvent->mAbstractConsumedEvent->ReleaseEvent() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractIsEventRequested(cclConsumedEventID consumedEventID, bool* isRequested)
{
  CCL_STATECHECK(eRunMeasurement);

  if (isRequested == nullptr)
    return CCL_PARAMETERINVALID;

  VAbstractConsumedEvent* consumedEvent;
  int result = VAbstractConsumedEvent::LookupConsumedEvent(consumedEventID, consumedEvent);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEvent->mAbstractConsumedEvent->IsEventRequested(isRequested) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractSubscribeEvent(cclConsumedEventID consumedEventID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedEvent* consumedEvent;
  int result = VAbstractConsumedEvent::LookupConsumedEvent(consumedEventID, consumedEvent);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEvent->mAbstractConsumedEvent->SubscribeEvent() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractUnsubscribeEvent(cclConsumedEventID consumedEventID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedEvent* consumedEvent;
  int result = VAbstractConsumedEvent::LookupConsumedEvent(consumedEventID, consumedEvent);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEvent->mAbstractConsumedEvent->UnsubscribeEvent() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractSetEventSubscribedHandler(cclEventProviderID providerID, cclAbstractEventSubscriptionHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VAbstractEventProvider* eventProvider;
  int result = VAbstractEventProvider::LookupEventProvider(providerID, eventProvider);
  if (result != CCL_SUCCESS)
    return result;

  if (eventProvider->SetEventSubscribedHandler(handler) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractSetEventUnsubscribedHandler(cclEventProviderID providerID, cclAbstractEventSubscriptionHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VAbstractEventProvider* eventProvider;
  int result = VAbstractEventProvider::LookupEventProvider(providerID, eventProvider);
  if (result != CCL_SUCCESS)
    return result;

  if (eventProvider->SetEventUnsubscribedHandler(handler) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

cclPDUReceiverID cclAbstractConsumedPDUGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VAbstractConsumedServicePDU::CreateConsumedPDU(path);
}

cclPDUProviderID cclAbstractPDUProviderGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VAbstractServicePDUProvider::CreatePDUProvider(path);
}

int cclAbstractRequestPDU(cclPDUReceiverID consumedPDUID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedServicePDU* consumedPDU;
  int result = VAbstractConsumedServicePDU::LookupConsumedPDU(consumedPDUID, consumedPDU);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedPDU->mAbstractConsumedPDU->RequestPDU() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractReleasePDU(cclPDUReceiverID consumedPDUID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedServicePDU* consumedPDU;
  int result = VAbstractConsumedServicePDU::LookupConsumedPDU(consumedPDUID, consumedPDU);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedPDU->mAbstractConsumedPDU->ReleasePDU() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractIsPDURequested(cclPDUReceiverID consumedPDUID, bool* isRequested)
{
  CCL_STATECHECK(eRunMeasurement);

  if (isRequested == nullptr)
    return CCL_PARAMETERINVALID;

  VAbstractConsumedServicePDU* consumedPDU;
  int result = VAbstractConsumedServicePDU::LookupConsumedPDU(consumedPDUID, consumedPDU);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedPDU->mAbstractConsumedPDU->IsPDURequested(isRequested) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractSubscribePDU(cclPDUReceiverID consumedPDUID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedServicePDU* consumedPDU;
  int result = VAbstractConsumedServicePDU::LookupConsumedPDU(consumedPDUID, consumedPDU);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedPDU->mAbstractConsumedPDU->SubscribePDU() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractUnsubscribePDU(cclPDUReceiverID consumedPDUID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedServicePDU* consumedPDU;
  int result = VAbstractConsumedServicePDU::LookupConsumedPDU(consumedPDUID, consumedPDU);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedPDU->mAbstractConsumedPDU->UnsubscribePDU() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractSetPDUSubscribedHandler(cclPDUProviderID pduProviderID, cclAbstractPDUSubscriptionHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VAbstractServicePDUProvider* pduProvider;
  int result = VAbstractServicePDUProvider::LookupPDUProvider(pduProviderID, pduProvider);
  if (result != CCL_SUCCESS)
    return result;

  if (pduProvider->SetPDUSubscribedHandler(handler) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractSetPDUUnsubscribedHandler(cclPDUProviderID pduProviderID, cclAbstractPDUSubscriptionHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VAbstractServicePDUProvider* pduProvider;
  int result = VAbstractServicePDUProvider::LookupPDUProvider(pduProviderID, pduProvider);
  if (result != CCL_SUCCESS)
    return result;

  if (pduProvider->SetPDUUnsubscribedHandler(handler) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

cclConsumedFieldID cclAbstractConsumedFieldGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VAbstractConsumedField::CreateConsumedField(path);
}

cclFieldProviderID cclAbstractFieldProviderGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VAbstractFieldProvider::CreateFieldProvider(path);
}

int cclAbstractRequestField(cclConsumedFieldID consumedFieldID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedField* consumedField;
  int result = VAbstractConsumedField::LookupConsumedField(consumedFieldID, consumedField);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedField->mAbstractConsumedField->RequestField() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractReleaseField(cclConsumedFieldID consumedFieldID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedField* consumedField;
  int result = VAbstractConsumedField::LookupConsumedField(consumedFieldID, consumedField);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedField->mAbstractConsumedField->ReleaseField() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractIsFieldRequested(cclConsumedFieldID consumedFieldID, bool* isRequested)
{
  CCL_STATECHECK(eRunMeasurement);

  if (isRequested == nullptr)
    return CCL_PARAMETERINVALID;

  VAbstractConsumedField* consumedField;
  int result = VAbstractConsumedField::LookupConsumedField(consumedFieldID, consumedField);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedField->mAbstractConsumedField->IsFieldRequested(isRequested) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractSubscribeField(cclConsumedFieldID consumedFieldID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedField* consumedField;
  int result = VAbstractConsumedField::LookupConsumedField(consumedFieldID, consumedField);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedField->mAbstractConsumedField->SubscribeField() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractUnsubscribeField(cclConsumedFieldID consumedFieldID)
{
  CCL_STATECHECK(eRunMeasurement);

  VAbstractConsumedField* consumedField;
  int result = VAbstractConsumedField::LookupConsumedField(consumedFieldID, consumedField);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedField->mAbstractConsumedField->UnsubscribeField() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractSetFieldSubscribedHandler(cclFieldProviderID providerID, cclAbstractFieldSubscriptionHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VAbstractFieldProvider* fieldProvider;
  int result = VAbstractFieldProvider::LookupFieldProvider(providerID, fieldProvider);
  if (result != CCL_SUCCESS)
    return result;

  if (fieldProvider->SetFieldSubscribedHandler(handler) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclAbstractSetFieldUnsubscribedHandler(cclFieldProviderID providerID, cclAbstractFieldSubscriptionHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VAbstractFieldProvider* fieldProvider;
  int result = VAbstractFieldProvider::LookupFieldProvider(providerID, fieldProvider);
  if (result != CCL_SUCCESS)
    return result;

  if (fieldProvider->SetFieldUnsubscribedHandler(handler) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

#pragma endregion

#pragma region SOME/IP Binding

cclAddressID cclSomeIPCreateAddress(const char* udpAddress, long udpPort, const char* tcpAddress, long tcpPort)
{
  CCL_STATECHECK(eRunMeasurement);

  VIAFbViaService* fbAPI = GetFunctionBusAPI();
  VIAFbSomeIPBinding* someIPBinding;
  if (fbAPI->GetSomeIPBinding(&someIPBinding) != kVIA_OK)
    return CCL_INTERNALERROR;

  NDetail::VIAObjectGuard<VIAFbAddressHandle> address;
  if (someIPBinding->CreateAddress(udpAddress, udpPort, tcpAddress, tcpPort, &(address.mVIAObject)) != kVIA_OK)
    return CCL_CANNOTCREATEADDRESS;

  return VAddress::CreateAddress(address.Release(), true);
}

int cclSomeIPGetBindingID(cclAddressID addressID, long* bindingID)
{
  CCL_STATECHECK(eRunMeasurement);

  VIAFbViaService* fbAPI = GetFunctionBusAPI();
  VIAFbSomeIPBinding* someIPBinding;
  if (fbAPI->GetSomeIPBinding(&someIPBinding) != kVIA_OK)
    return CCL_INTERNALERROR;

  VAddress* address;
  int result = VAddress::LookupAddress(addressID, address);
  if (result != CCL_SUCCESS)
    return result;

  VIAFbBindingId fbBindingID;
  if (someIPBinding->GetBindingId(address->mAddress, &fbBindingID) != kVIA_OK)
    return CCL_INVALIDBINDING;

  *bindingID = static_cast<long>(fbBindingID);
  return CCL_SUCCESS;
}

int cclSomeIPGetDisplayName(cclAddressID addressID, char* buffer, int bufferSize)
{
  CCL_STATECHECK(eRunMeasurement);

  VIAFbViaService* fbAPI = GetFunctionBusAPI();
  VIAFbSomeIPBinding* someIPBinding;
  if (fbAPI->GetSomeIPBinding(&someIPBinding) != kVIA_OK)
    return CCL_INTERNALERROR;

  VAddress* address;
  int result = VAddress::LookupAddress(addressID, address);
  if (result != CCL_SUCCESS)
    return result;

  result = someIPBinding->GetDisplayName(address->mAddress, buffer, bufferSize);
  if (result == kVIA_BufferToSmall)
    return CCL_BUFFERTOSMALL;

  return (result == kVIA_OK) ? CCL_SUCCESS : CCL_INTERNALERROR;
}

cclSomeIPConsumedEventGroupID cclSomeIPConsumedEventGroupGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VConsumedEventGroup::CreateConsumedEventGroup(path);
}

cclSomeIPProvidedEventGroupID cclSomeIPProvidedEventGroupGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VProvidedEventGroup::CreateProvidedEventGroup(path);
}

cclSomeIPEventGroupProviderID cclSomeIPEventGroupProviderGetID(const char* path)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  return VEventGroupProvider::CreateEventGroupProvider(path);
}

int cclSomeIPRequestEventGroup(cclSomeIPConsumedEventGroupID consumedEventGroupID)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedEventGroup* consumedEventGroup;
  int result = VConsumedEventGroup::LookupConsumedEventGroup(consumedEventGroupID, consumedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEventGroup->mPort->RequestEventGroup() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclSomeIPReleaseEventGroup(cclSomeIPConsumedEventGroupID consumedEventGroupID)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedEventGroup* consumedEventGroup;
  int result = VConsumedEventGroup::LookupConsumedEventGroup(consumedEventGroupID, consumedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEventGroup->mPort->ReleaseEventGroup() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclSomeIPIsEventGroupRequested(cclSomeIPConsumedEventGroupID consumedEventGroupID, bool* isRequested)
{
  CCL_STATECHECK(eRunMeasurement);

  if (isRequested == nullptr)
    return CCL_PARAMETERINVALID;

  VConsumedEventGroup* consumedEventGroup;
  int result = VConsumedEventGroup::LookupConsumedEventGroup(consumedEventGroupID, consumedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEventGroup->mPort->IsEventGroupRequested(isRequested) != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclSomeIPConsumedEventGroupGetEvents(cclSomeIPConsumedEventGroupID consumedEventGroupID, cclConsumedEventID* buffer, int* bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VConsumedEventGroup* consumedEventGroup;
  int result = VConsumedEventGroup::LookupConsumedEventGroup(consumedEventGroupID, consumedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  return consumedEventGroup->GetEvents(buffer, bufferSize);
}

int cclSomeIPProvidedEventGroupGetEvents(cclSomeIPProvidedEventGroupID providedEventGroupID, cclProvidedEventID* buffer, int* bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VProvidedEventGroup* providedEventGroup;
  int result = VProvidedEventGroup::LookupProvidedEventGroup(providedEventGroupID, providedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  return providedEventGroup->GetEvents(buffer, bufferSize);
}

int cclSomeIPEventGroupProviderGetEvents(cclSomeIPEventGroupProviderID eventGroupProviderID, cclEventProviderID* buffer, int* bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VEventGroupProvider* eventGroupProvider;
  int result = VEventGroupProvider::LookupEventGroupProvider(eventGroupProviderID, eventGroupProvider);
  if (result != CCL_SUCCESS)
    return result;

  return eventGroupProvider->GetEvents(buffer, bufferSize);
}

int cclSomeIPConsumedEventGroupGetPDUs(cclSomeIPConsumedEventGroupID consumedEventGroupID, cclConsumedEventID* buffer, int* bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VConsumedEventGroup* consumedEventGroup;
  int result = VConsumedEventGroup::LookupConsumedEventGroup(consumedEventGroupID, consumedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  return consumedEventGroup->GetPDUs(buffer, bufferSize);
}

int cclSomeIPProvidedEventGroupGetPDUs(cclSomeIPProvidedEventGroupID providedEventGroupID, cclProvidedEventID* buffer, int* bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VProvidedEventGroup* providedEventGroup;
  int result = VProvidedEventGroup::LookupProvidedEventGroup(providedEventGroupID, providedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  return providedEventGroup->GetPDUs(buffer, bufferSize);
}

int cclSomeIPEventGroupProviderGetPDUs(cclSomeIPEventGroupProviderID eventGroupProviderID, cclEventProviderID* buffer, int* bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VEventGroupProvider* eventGroupProvider;
  int result = VEventGroupProvider::LookupEventGroupProvider(eventGroupProviderID, eventGroupProvider);
  if (result != CCL_SUCCESS)
    return result;

  return eventGroupProvider->GetPDUs(buffer, bufferSize);
}

int cclSomeIPConsumedEventGroupGetFields(cclSomeIPConsumedEventGroupID consumedEventGroupID, cclConsumedFieldID* buffer, int* bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VConsumedEventGroup* consumedEventGroup;
  int result = VConsumedEventGroup::LookupConsumedEventGroup(consumedEventGroupID, consumedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  return consumedEventGroup->GetFields(buffer, bufferSize);
}

int cclSomeIPProvidedEventGroupGetFields(cclSomeIPProvidedEventGroupID providedEventGroupID, cclProvidedFieldID* buffer, int* bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VProvidedEventGroup* providedEventGroup;
  int result = VProvidedEventGroup::LookupProvidedEventGroup(providedEventGroupID, providedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  return providedEventGroup->GetFields(buffer, bufferSize);
}

int cclSomeIPEventGroupProviderGetFields(cclSomeIPEventGroupProviderID eventGroupProviderID, cclFieldProviderID* buffer, int* bufferSize)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VEventGroupProvider* eventGroupProvider;
  int result = VEventGroupProvider::LookupEventGroupProvider(eventGroupProviderID, eventGroupProvider);
  if (result != CCL_SUCCESS)
    return result;

  return eventGroupProvider->GetFields(buffer, bufferSize);
}

int cclSomeIPEventGroupGetNrOfSubscribedConsumers(cclSomeIPEventGroupProviderID eventGroupProviderID, int* nrOfConsumers)
{
  CCL_STATECHECK(eRunMeasurement);

  VEventGroupProvider* eventGroupProvider;
  int result = VEventGroupProvider::LookupEventGroupProvider(eventGroupProviderID, eventGroupProvider);
  if (result != CCL_SUCCESS)
    return result;

  return eventGroupProvider->GetNrOfSubscribedConsumers(nrOfConsumers);
}

int cclSomeIPEventGroupGetSubscribedConsumers(cclSomeIPEventGroupProviderID eventGroupProviderID, cclConsumerID* consumerBuffer, int* bufferSize)
{
  CCL_STATECHECK(eRunMeasurement);

  VEventGroupProvider* eventGroupProvider;
  int result = VEventGroupProvider::LookupEventGroupProvider(eventGroupProviderID, eventGroupProvider);
  if (result != CCL_SUCCESS)
    return result;

  return eventGroupProvider->GetSubscribedConsumers(consumerBuffer, bufferSize);
}

int cclSomeIPSubscribeEventGroup(cclSomeIPConsumedEventGroupID consumedEventGroupID)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedEventGroup* consumedEventGroup;
  int result = VConsumedEventGroup::LookupConsumedEventGroup(consumedEventGroupID, consumedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEventGroup->mPort->SubscribeEventGroup() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclSomeIPUnsubscribeEventGroup(cclSomeIPConsumedEventGroupID consumedEventGroupID)
{
  CCL_STATECHECK(eRunMeasurement);

  VConsumedEventGroup* consumedEventGroup;
  int result = VConsumedEventGroup::LookupConsumedEventGroup(consumedEventGroupID, consumedEventGroup);
  if (result != CCL_SUCCESS)
    return result;

  if (consumedEventGroup->mPort->UnsubscribeEventGroup() != kVIA_OK)
    return CCL_INTERNALERROR;

  return CCL_SUCCESS;
}

int cclSomeIPSetEventGroupSubscribedHandler(cclSomeIPEventGroupProviderID eventGroupProviderID, cclSomeIPEventGroupSubscriptionHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VEventGroupProvider* eventGroupProvider;
  int result = VEventGroupProvider::LookupEventGroupProvider(eventGroupProviderID, eventGroupProvider);
  if (result != CCL_SUCCESS)
    return result;

  return eventGroupProvider->SetEventGroupSubscribedHandler(handler);
}

int cclSomeIPSetEventGroupUnsubscribedHandler(cclSomeIPEventGroupProviderID eventGroupProviderID, cclSomeIPEventGroupSubscriptionHandler handler)
{
  CCL_STATECHECK(eInitMeasurement | eRunMeasurement | eStopMeasurement);

  VEventGroupProvider* eventGroupProvider;
  int result = VEventGroupProvider::LookupEventGroupProvider(eventGroupProviderID, eventGroupProvider);
  if (result != CCL_SUCCESS)
    return result;

  return eventGroupProvider->SetEventGroupUnsubscribedHandler(handler);
}

#pragma endregion