///////////////////////////////////////////////////////////////////////////////
// Header
///////////////////////////////////////////////////////////////////////////////
#if !defined (Communication_i_H ) 
       #define Communication_i_H

#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>

// undef this to connect to CANalyzer
#include "CANoe.h"
#include "CANoe_i.c"

///////////////////////////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////////////////////////
const int EXIT_EVENT = 0;
const int GUI_EVENT = 1;
const int NUM_EVENTS = 2;

const int CMD_UNKNOWN = 0;
const int CMD_CONNECT = 1;
const int CMD_DISCONNECT = 2;

const int APPLICATION_SINK = 1;
const int MEASUREMENT_SINK = 2;
const int ENVIRONMENTVARIABLE_SINK = 3;
const int VARIABLE_SINK = 4;

///////////////////////////////////////////////////////////////////////////////
// Event sink classes
/////////////////////////////////////////////////////////////////////////////
template <UINT nID, class T, const IID* piid, const GUID* plibid, WORD wMajor, WORD wMinor>
class ATL_NO_VTABLE CDispEventImpl : public IDispEventImpl<nID, T, piid, plibid, wMajor, wMinor>
{
  public:
      CDispEventImpl(){}

      HRESULT GetObjectSourceInterface(IUnknown* pUnknown)
      {
          HRESULT hr = ATL::AtlGetObjectSourceInterface(pUnknown, 
                                                         &m_libid, 
                                                         &m_iid,
                                                         &m_wMajorVerNum,
                                                         &m_wMinorVerNum);
          if (SUCCEEDED(hr))
          {
			  _tih.m_plibid = &m_libid;
			  _tih.m_pguid = &m_iid;
			  _tih.m_wMajor = m_wMajorVerNum;
			  _tih.m_wMinor = m_wMinorVerNum;
          }

          return hr;
      }
};

///////////////////////////////////////////////////////////////////////////////
// Application Events
/////////////////////////////////////////////////////////////////////////////
class CApplicationEventSink : public CDispEventImpl< APPLICATION_SINK, 
                                                     CApplicationEventSink, 
                                                     &DIID__IApplicationEvents, 
                                                     &LIBID_CANoe, 
                                                     LIBRARY_MAJOR, 
                                                     LIBRARY_MINOR>
{
  public:
    CApplicationEventSink(){}

    STDMETHOD(OnOpen)(BSTR name);
    STDMETHOD(OnQuit)();

BEGIN_SINK_MAP(CApplicationEventSink)
  SINK_ENTRY_EX(APPLICATION_SINK, DIID__IApplicationEvents, 1, OnOpen)
  SINK_ENTRY_EX(APPLICATION_SINK, DIID__IApplicationEvents, 2, OnQuit)
END_SINK_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// Measurement Events
/////////////////////////////////////////////////////////////////////////////
class CMeasurementEventSink : public CDispEventImpl< MEASUREMENT_SINK, 
                                                     CMeasurementEventSink, 
                                                     &DIID__IMeasurementEvents, 
                                                     &LIBID_CANoe, 
                                                     LIBRARY_MAJOR, 
                                                     LIBRARY_MINOR>
{
  public:
    CMeasurementEventSink(){}

    STDMETHOD(OnInit)();
    STDMETHOD(OnStart)();
    STDMETHOD(OnStop)();
    STDMETHOD(OnExit)();

BEGIN_SINK_MAP(CMeasurementEventSink)
  SINK_ENTRY_EX(MEASUREMENT_SINK, DIID__IMeasurementEvents, 1, OnInit)
  SINK_ENTRY_EX(MEASUREMENT_SINK, DIID__IMeasurementEvents, 2, OnStart)
  SINK_ENTRY_EX(MEASUREMENT_SINK, DIID__IMeasurementEvents, 3, OnStop)
  SINK_ENTRY_EX(MEASUREMENT_SINK, DIID__IMeasurementEvents, 4, OnExit)
END_SINK_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// Environment Variable Events
/////////////////////////////////////////////////////////////////////////////
class CEnvironmentVariableEventSink : public CDispEventImpl< ENVIRONMENTVARIABLE_SINK, 
                                                             CEnvironmentVariableEventSink, 
                                                             &DIID__IEnvironmentVariableEvents, 
                                                             &LIBID_CANoe, 
                                                             LIBRARY_MAJOR, 
                                                             LIBRARY_MINOR>
{
  public:
    CEnvironmentVariableEventSink(){}

    STDMETHOD(OnChange)(VARIANT value);

BEGIN_SINK_MAP(CEnvironmentVariableEventSink)
  SINK_ENTRY_EX(ENVIRONMENTVARIABLE_SINK, DIID__IEnvironmentVariableEvents, 1, OnChange)
END_SINK_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// Variable Events
/////////////////////////////////////////////////////////////////////////////
class CVariableEventSink : public CDispEventImpl< VARIABLE_SINK, 
	CVariableEventSink, 
	&DIID__IVariableEvents, 
	&LIBID_CANoe, 
	LIBRARY_MAJOR, 
	LIBRARY_MINOR>
{
public:
	CVariableEventSink(){}

	STDMETHOD(OnChange)(VARIANT value);

	BEGIN_SINK_MAP(CVariableEventSink)
		SINK_ENTRY_EX(VARIABLE_SINK, DIID__IVariableEvents, 1, OnChange)
	END_SINK_MAP()
};

///////////////////////////////////////////////////////////////////////////////
// Helper functions and Macros
/////////////////////////////////////////////////////////////////////////////

HRESULT DoDisconnect();
HRESULT DoStart();
HRESULT DoStop();

void    ProcessOnMessage(LPBYTE data, DWORD count);
void    ProcessOnMessageTime(DWORD time);

#define OUTPUT(txt) \
  { \
    CSingleLock lock(&sOutputCS, TRUE); \
    \
    if (NULL != sOutput) \
    { \
      sOutput->Output(txt); \
    } \
  } 

#define SET_COMMAND(command) \
  sCommand = command; \
  \
  ::SetEvent(sEvents[GUI_EVENT]); \
  \
  ::Sleep(0)

#define RETURN_ON_FAILED(hr) \
  if (FAILED(hr)) \
  { \
    return hr; \
  }

#define RETURN_FALSE_ON_FAILED(hr) \
  if (FAILED(hr)) \
  { \
    return FALSE; \
  }

#define RETURN_FALSE_ON_FAILED_ITF(ptr) \
  if (!ptr) \
  { \
    return FALSE; \
  }

#define VERIFY_APPLICATION_UI() \
  if (!sApplicationUI && sApplicationUIStream) \
  { \
    ::CoGetInterfaceAndReleaseStream(sApplicationUIStream.Detach(), IID_IApplication, (void**)&sApplicationUI); \
  } \
  \
  if (!sApplicationUI) \
  { \
    return FALSE; \
  }

#define VERIFY_CONFIGURATION_UI() \
  if (!sConfigurationUI && sConfigurationUIStream) \
  { \
    ::CoGetInterfaceAndReleaseStream(sConfigurationUIStream.Detach(), IID_IConfiguration, (void**)&sConfigurationUI); \
  } \
  \
  if (!sConfigurationUI) \
  { \
    return FALSE; \
  }

#define VERIFY_MEASUREMENT_UI() \
  if (!sMeasurementUI && sMeasurementUIStream) \
  { \
    ::CoGetInterfaceAndReleaseStream(sMeasurementUIStream.Detach(), IID_IMeasurement, (void**)&sMeasurementUI); \
  } \
  \
  if (!sMeasurementUI) \
  { \
    return FALSE; \
  }

#endif // Communication_i_H