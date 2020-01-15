///////////////////////////////////////////////////////////////////////////////
// Header
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Communication.h"
#include "Communication_i.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
// Threading
static CWinThread*                      sWinThread = NULL;
static HANDLE                           sEvents[NUM_EVENTS];
static DWORD                            sMilliSeconds = 20;
static int                              sCommand = CMD_UNKNOWN;

static CListOutput*                     sOutput = NULL;
static CCriticalSection                 sOutputCS;

// COM interface pointers
static CComQIPtr<IApplication>          sApplication;
static CComQIPtr<IApplication>          sApplicationUI;
static CComPtr<IStream>                 sApplicationUIStream;

static CComQIPtr<IConfiguration>        sConfigurationUI;
static CComPtr<IStream>                 sConfigurationUIStream;

static CComQIPtr<IMeasurement>          sMeasurement;
static CComQIPtr<IMeasurement>          sMeasurementUI;
static CComPtr<IStream>                 sMeasurementUIStream;

static CComQIPtr<IVariable>             sSysVar;

static CComQIPtr<ISignal>               sCarSpeedSignal;
static CComQIPtr<ISignal>               sEngSpeedSignal;
static CComQIPtr<ISignal>               sEngTempSignal;

static BOOL                             sIsStarted = FALSE;

static CApplicationEventSink*           sApplicationSink = NULL;
static CMeasurementEventSink*           sMeasurementSink = NULL;
static CVariableEventSink*              sVarSink = NULL;

static TCHAR                            sAppName[128] = { 0};

CComModule                              _Module;

///////////////////////////////////////////////////////////////////////////////
// Application Events
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CApplicationEventSink::OnOpen(BSTR name)
{
    USES_CONVERSION;

    // Open event => called when a configuration is opened
    TCHAR msg[512];
    DWORD attributes = ::GetFileAttributes(OLE2T(name));
  
    if (attributes != -1 && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
        wsprintf(msg, _T("%s has opened the configuration: %s"), sAppName, OLE2T(name));
    }
    else
    {
        wsprintf(msg, _T("%s has created a new configuration."), sAppName);
    }

    OUTPUT(msg);
  
    return S_OK;
}

STDMETHODIMP CApplicationEventSink::OnQuit()
{
    // Quit event => called when CANoe is shut down
    if (FAILED(DoDisconnect()))
    {
        TCHAR msg[512];
        wsprintf(msg, _T("%s is shutting down!"), sAppName);
        OUTPUT(msg);
    }

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// Measurement Events
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CMeasurementEventSink::OnInit()
{
    // Init event => called when measurement is initialized
    OUTPUT(_T("Measurement: OnInit"));
    return S_OK;
}

STDMETHODIMP CMeasurementEventSink::OnStart()
{
    // Start event => called when the measurement is started
    OUTPUT(_T("Measurement: OnStart"));
    sIsStarted = TRUE;
    SET_COMMAND(CMD_UNKNOWN);
    return S_OK;
}

STDMETHODIMP CMeasurementEventSink::OnStop()
{
    // Stop event => called the the measurement is stopped
    OUTPUT(_T("Measurement: OnStop"));
    sIsStarted = FALSE;
    return S_OK;
}

STDMETHODIMP CMeasurementEventSink::OnExit()
{
    // Exit event => called when the measurement is finished
    OUTPUT(_T("Measurement: OnExit"));
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// Environment Variable Events
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnvironmentVariableEventSink::OnChange(VARIANT value)
{
    if ((VT_ARRAY | VT_UI1) != value.vt)
    {
        if (VT_R8 == value.vt)
        {
            TCHAR msg[128];
            sprintf_s(msg, "EnvCarSpeedEuro: %f", value.dblVal);
            OUTPUT(msg);
        }
    }   

    return S_OK;
} 

///////////////////////////////////////////////////////////////////////////////
// Variable Events
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CVariableEventSink::OnChange(VARIANT value)
{
	if ((VT_ARRAY | VT_UI1) != value.vt)
	{
		if (VT_R8 == value.vt)
		{
			TCHAR msg[128];
			sprintf_s(msg, "CarSpeedEuropean: %f", value.dblVal);
			OUTPUT(msg);
		}
	}   

	return S_OK;
} 

///////////////////////////////////////////////////////////////////////////////
// Output Signal Value
///////////////////////////////////////////////////////////////////////////////
HRESULT OutputSignal(ISignal* pSignal, LPCTSTR pFormat)
{
    if (NULL == pSignal)
    {
        return E_POINTER;
    }

    double value;
    HRESULT hr = pSignal->get_Value(&value);

    if (SUCCEEDED(hr))
    {
        TCHAR msg[512];
        sprintf_s(msg, pFormat, value);
        OUTPUT(msg);
    }

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
// Connection
///////////////////////////////////////////////////////////////////////////////

static HRESULT OpenConfiguration()
{
    // get path to System Demo
    TCHAR path[MAX_PATH + 1];

    if(0 != ::GetModuleFileName(::GetModuleHandle(NULL), path, MAX_PATH))
    {
        // remove filename and go to directories back
        CComQIPtr<IDispatch>      pDispatch;
        CComQIPtr<IConfiguration> pConfiguration;
        HRESULT hr = S_OK;
        bool openConfig = true;
        CString fullname = path;

        int pos = fullname.ReverseFind('\\');
        fullname = fullname.Mid(0, pos);
        pos = fullname.ReverseFind('\\');
        fullname = fullname.Mid(0, pos);
        pos = fullname.ReverseFind('\\');
        fullname = fullname.Mid(0, pos);

        fullname.Append("\\CANSystemDemo.cfg");

        CComBSTR bstrFullName(fullname);

        // open configuration if not opened yet
        // or if no configuration is opened
        hr = sApplication->get_Configuration(&pDispatch);
        if(SUCCEEDED(hr))
        {
            pConfiguration = pDispatch.Detach();
            
            CComBSTR bstrConfigurationName;
            hr = pConfiguration->get_FullName(&bstrConfigurationName);

            if(SUCCEEDED(hr))
            {
                bstrConfigurationName.ToLower();
                bstrFullName.ToLower();

                if(bstrConfigurationName == bstrFullName)
                {
                    openConfig = false;
                }
            }
        }

        if(openConfig)
        {
            return SUCCEEDED(sApplication->Open(bstrFullName, VARIANT_FALSE, VARIANT_FALSE));
        }
        else
        {
            return S_OK;
        }
    }

    return E_FAIL;
}

static HRESULT DoConnectInternal()
{
    USES_CONVERSION;

    CComPtr<IUnknown>         pUnknown;
    CComQIPtr<IDispatch>      pDispatch;
    CComQIPtr<IConfiguration> pConfiguration;
    CComQIPtr<IEnvironment>   pEnvironment;
    CComQIPtr<IMeasurement2>  sMeasurement2;
	CComQIPtr<IApplication2>  pApplication2;
    CComQIPtr<IBus>           pBus;
	CComQIPtr<ISystem>        pSystem;
	CComQIPtr<INamespaces>    pNamespaces;
	CComQIPtr<INamespace>     pNamespace;
	CComQIPtr<IVariables>     pVariables;
	CComVariant               sVariant;

    VARIANT_BOOL              running;
    CComBSTR                  bstr;
    CComBSTR                  bstr2;
    
    // connect to CANoe COM Server
    HRESULT hr = ::CoCreateInstance(CLSID_Application, 
                                    NULL, 
                                    CLSCTX_SERVER, 
                                    IID_IApplication,
                                    (void**)&sApplication);
    RETURN_ON_FAILED(hr);

    hr = ::CoMarshalInterThreadInterfaceInStream(IID_IApplication, sApplication, &sApplicationUIStream);

    ASSERT(SUCCEEDED(hr));

    // open configuration
    hr = OpenConfiguration(); 

    RETURN_ON_FAILED(hr);

    // get configuration
    hr = sApplication->get_Configuration(&pDispatch);

    ASSERT(SUCCEEDED(hr));

    pConfiguration = pDispatch.Detach();

    hr = ::CoMarshalInterThreadInterfaceInStream(IID_IConfiguration, pConfiguration, &sConfigurationUIStream);

    ASSERT(SUCCEEDED(hr));

    // get application name
    hr = sApplication->get_Name(&bstr);

    if (SUCCEEDED(hr))
    {
        _tcscpy_s(sAppName, OLE2T(bstr));
    }

    // subscribe to application events
    if (NULL != sApplicationSink)
    {
        hr = sApplication.QueryInterface(&pUnknown);

        ASSERT(SUCCEEDED(hr));

        hr = sApplicationSink->GetObjectSourceInterface(pUnknown);

        ASSERT(SUCCEEDED(hr));

        hr = sApplicationSink->DispEventAdvise(pUnknown);

        ASSERT(SUCCEEDED(hr));
    }

    // subscribe to measurement events
    if (NULL != sMeasurementSink)
    {
        hr = sApplication->get_Measurement(&pDispatch);

        ASSERT(SUCCEEDED(hr));

        hr = pDispatch->QueryInterface(IID_IMeasurement2, (void**)&sMeasurement2);

        if (SUCCEEDED(hr))
        {
            // CANalyzer/CANoe 3.0 Build 30 or newer
            hr = sMeasurement2->get_Running(&running);

            ASSERT(SUCCEEDED(hr));

            sIsStarted = running == VARIANT_TRUE;

            hr = sMeasurement2->QueryInterface(IID_IMeasurement, (void**)&sMeasurement);

            ASSERT(SUCCEEDED(hr));

        }
        else
        {
            // CANalyzer/CANoe 3.0 Build 24 or older
            hr = pDispatch->QueryInterface(IID_IMeasurement, (void**)&sMeasurement);

            ASSERT(SUCCEEDED(hr));

            // Assume measurement is not running
            sIsStarted = FALSE;
        }

        hr = ::CoMarshalInterThreadInterfaceInStream(IID_IMeasurement, sMeasurement, &sMeasurementUIStream);

        ASSERT(SUCCEEDED(hr));

        pUnknown.Release();

        hr = sMeasurement->QueryInterface(IID_IUnknown, (void**)&pUnknown);

        ASSERT(SUCCEEDED(hr));

        hr = sMeasurementSink->GetObjectSourceInterface(pUnknown);

        ASSERT(SUCCEEDED(hr));

        hr = sMeasurementSink->DispEventAdvise(pUnknown);

        ASSERT(SUCCEEDED(hr));
    }

    pDispatch.Release();

	// subscribe to system variable events
	hr = sApplication->QueryInterface(IID_IApplication2, (void**)&pApplication2);

	if (SUCCEEDED(hr))
	{
		pDispatch.Release();

		hr = pApplication2->get_System(&pDispatch);

		if (SUCCEEDED(hr))
		{
			pSystem = pDispatch.Detach();

			if (pSystem)
			{
				hr = pSystem->get_Namespaces(&pDispatch);

				if (SUCCEEDED(hr))
				{
					pNamespaces = pDispatch.Detach();

					if (pNamespaces)
					{
						sVariant = OLESTR("ComfortBus");

						hr = pNamespaces->get_Item(sVariant, &pNamespace);

						if (SUCCEEDED(hr))
						{
							if (pNamespace)
							{
								hr = pNamespace->get_Variables(&pDispatch);

								if (SUCCEEDED(hr))
								{
									pVariables = pDispatch.Detach();

									if (NULL != sVarSink)
									{
										if (pVariables)
										{
											sVariant = OLESTR("CarSpeedEuropean");

											hr = pVariables->get_Item(sVariant, &sSysVar);

											if (SUCCEEDED(hr))
											{
												pUnknown.Release();

												hr = sSysVar->QueryInterface(IID_IUnknown, (void**)&pUnknown);

												ASSERT(SUCCEEDED(hr));

												hr = sVarSink->GetObjectSourceInterface(pUnknown);

												ASSERT(SUCCEEDED(hr));

												hr = sVarSink->DispEventAdvise(pUnknown);

												ASSERT(SUCCEEDED(hr));
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

    // find signals
    bstr = "";
    pDispatch.Release();
    hr = sApplication->get_Bus(bstr, &pDispatch);

    if (SUCCEEDED(hr))
    {
        pBus = pDispatch.Detach();

        if (pBus)
        {
            bstr = OLESTR("ABSdata");
            bstr2 = OLESTR("CarSpeed");

            hr = pBus->GetSignal(2, bstr, bstr2, &pDispatch);

            if (SUCCEEDED(hr))
            {
                sCarSpeedSignal = pDispatch.Detach();
            }

            bstr = OLESTR("EngineData");
            bstr2 = OLESTR("EngSpeed");

            hr = pBus->GetSignal(2, bstr, bstr2, &pDispatch);

            if (SUCCEEDED(hr))
            {
                sEngSpeedSignal = pDispatch.Detach();
            }

            bstr = OLESTR("EngineData");
            bstr2 = OLESTR("EngTemp");

            hr = pBus->GetSignal(2, bstr, bstr2, &pDispatch);

            if (SUCCEEDED(hr))
            {
                sEngTempSignal = pDispatch.Detach();
            }
        }
    }

    return hr;
}

static HRESULT DoConnect()
{
    HRESULT hr = DoConnectInternal();

    if ( IsConnected())
    {
        TCHAR msg[512];

        wsprintf(msg, _T("Connected to %s..."), sAppName);

        OUTPUT(msg);
    }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
// Disconnection
///////////////////////////////////////////////////////////////////////////////
HRESULT DoDisconnectInternal()
{
    IUnknown* pUnknown;
    HRESULT   hr = S_OK;

	if (sSysVar)
	{
		if (NULL != sVarSink)
		{
			hr = sSysVar->QueryInterface(IID_IUnknown, (void**)&pUnknown);

			ASSERT(SUCCEEDED(hr));

			hr = sVarSink->DispEventUnadvise(pUnknown);

			pUnknown->Release();
		}

		sSysVar.Release();
	}

    if (sMeasurement)
    {
        if (NULL != sMeasurementSink)
        {
            hr = sMeasurement->QueryInterface(IID_IUnknown, (void**)&pUnknown);

            ASSERT(SUCCEEDED(hr));

            hr = sMeasurementSink->DispEventUnadvise(pUnknown);

            // The last call may fail with RPC_E_DISCONNECTED
            pUnknown->Release();
        }

        sMeasurement.Release();
    }

    sMeasurementUI.Release();
    sMeasurementUIStream.Release();

    if (sApplication)
    {
        if (NULL != sApplicationSink)
        {
            hr = sApplication->QueryInterface(IID_IUnknown, (void**)&pUnknown);

            ASSERT(SUCCEEDED(hr));

            hr = sApplicationSink->DispEventUnadvise(pUnknown);

            // The last call may fail with RPC_E_DISCONNECTED

            pUnknown->Release();
        }

        sApplication.Release();
    }

    sConfigurationUI.Release();
    sConfigurationUIStream.Release();

    sApplicationUI.Release();
    sApplicationUIStream.Release();

    return hr;
}

HRESULT DoDisconnect()
{
    HRESULT hr = DoDisconnectInternal();

    if (!IsConnected())
    {
        TCHAR msg[512];
        wsprintf(msg, _T("Disconnected from %s..."), sAppName);
        OUTPUT(msg);
    }

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
// Measurement Start
///////////////////////////////////////////////////////////////////////////////
HRESULT DoStart()
{
  return sMeasurement ? sMeasurement->Start() : E_FAIL;
}

///////////////////////////////////////////////////////////////////////////////
// Measurement Stop
///////////////////////////////////////////////////////////////////////////////
HRESULT DoStop()
{
  return sMeasurement ? sMeasurement->Stop() : E_FAIL;
}

///////////////////////////////////////////////////////////////////////////////
// Measurement Processing
///////////////////////////////////////////////////////////////////////////////
HRESULT DoMeasurement()
{
    OutputSignal(sCarSpeedSignal, _T("CarSpeed: %f"));
    OutputSignal(sEngSpeedSignal, _T("EngSpeed: %f"));
    OutputSignal(sEngTempSignal, _T("EngTemp: %f"));
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// Helper functions
///////////////////////////////////////////////////////////////////////////////
void SetOutput(CListOutput* output)
{
    CSingleLock lock(&sOutputCS, TRUE);
    sOutput = output;
}

BOOL IsConnected()
{
    return !sApplication ? FALSE : TRUE;
}

BOOL Connect()
{
    SET_COMMAND(CMD_CONNECT);
    return TRUE;
}

BOOL Disconnect()
{
    SET_COMMAND(CMD_DISCONNECT);
    return TRUE;
}


BOOL IsStarted()
{
    return sIsStarted;
}

BOOL Start()
{
    VERIFY_MEASUREMENT_UI();
    return SUCCEEDED(sMeasurementUI->Start());
}

BOOL Stop()
{
    VERIFY_MEASUREMENT_UI();
    return SUCCEEDED(sMeasurementUI->Stop());
}

static void OutputVariableName( CComBSTR& namespaceName, CComBSTR& variableName)
{
    USES_CONVERSION;

    CComBSTR name;

    name.Append( namespaceName);
    name.Append( L"::");
    name.Append( variableName);

    OUTPUT( W2A( name));
}

BOOL ShowEnvironmentVariables()
{
    VERIFY_APPLICATION_UI();
    OUTPUT(_T("Available system variables:"));
    CComQIPtr<IDispatch>        pDispatch;
    CComQIPtr<IApplication2>    pApplication2;
    CComQIPtr<ISystem>          pSystem;
    CComQIPtr<INamespaces>      pNamespaces;
    CComQIPtr<INamespace>       pNamespace;
    HRESULT                     hr;

    hr = sApplicationUI->QueryInterface( IID_IApplication2, (void**)&pApplication2);
    RETURN_FALSE_ON_FAILED(hr);

    hr = pApplication2->get_System(&pDispatch);
    RETURN_FALSE_ON_FAILED(hr);

    pSystem = pDispatch.Detach();
    RETURN_FALSE_ON_FAILED_ITF(pSystem);

    hr = pSystem->get_Namespaces(&pDispatch);
    RETURN_FALSE_ON_FAILED(hr);

    pNamespaces = pDispatch.Detach();

    if ( NULL != pNamespaces)
    {
        LONG lCountNamespaces;

        hr = pNamespaces->get_Count(&lCountNamespaces);
        RETURN_FALSE_ON_FAILED(hr);

        for ( LONG i = 1; i <= lCountNamespaces; ++i)
        {
            CComBSTR namespaceName;
            CComQIPtr<IVariables> pVariables;

            hr = pNamespaces->get_Item(CComVariant( i), &pNamespace);
            RETURN_FALSE_ON_FAILED(hr);

            hr = pNamespace->get_Name( &namespaceName);
            RETURN_FALSE_ON_FAILED(hr);

            // TODO: recurse into further namespaces...

            hr = pNamespace->get_Variables( &pDispatch);
            RETURN_FALSE_ON_FAILED(hr);

            pVariables = pDispatch.Detach();

            if ( NULL != pVariables)
            {
                LONG lCountVariables;

                hr = pVariables->get_Count(&lCountVariables);
                RETURN_FALSE_ON_FAILED(hr);

                for ( LONG j = 1; j <= lCountVariables; ++j)
                {
                    CComBSTR variableName;
                    CComQIPtr<IVariable> pVariable;

                    hr = pVariables->get_Item(CComVariant( j), &pVariable);
                    RETURN_FALSE_ON_FAILED(hr);

                    hr = pVariable->get_Name( &variableName);
                    RETURN_FALSE_ON_FAILED(hr);

                    OutputVariableName( namespaceName, variableName);
                }
            }

            pNamespace.Release();
        }
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Communication Thread
///////////////////////////////////////////////////////////////////////////////
UINT WaitTimeout()
{
    return sMilliSeconds;
}

UINT CommunicationThreadInternal()
{
    BOOL                          loop = TRUE;
    DWORD                         wait;
    DWORD                         result;
    CApplicationEventSink         applicationSink;
    CMeasurementEventSink         measurementSink;
	CVariableEventSink            varSink;
  
    sApplicationSink = &applicationSink;
    sMeasurementSink = &measurementSink;
	sVarSink = &varSink;
  
    while (loop)
    {
        wait = IsConnected() && IsStarted() ? WaitTimeout() : INFINITE;

        result = ::WaitForMultipleObjects(NUM_EVENTS, sEvents, FALSE, wait);

        switch(result)
        {
            case WAIT_OBJECT_0 + EXIT_EVENT:
            {
                // process exit event
                DoDisconnect();
                loop = FALSE;
                break;
            }
            case WAIT_OBJECT_0 + GUI_EVENT:
            {
                // process GUI commands
                switch (sCommand)
                {
                    case CMD_CONNECT:
                        DoConnect();
                        break;
                    case CMD_DISCONNECT:
                        DoDisconnect();
                        break;
                    default:
                    break;
                }
                sCommand = CMD_UNKNOWN;
                break;
            }
            case WAIT_TIMEOUT:
            {
                // if measurement is running do measurement processing
                if (IsStarted())
                {
                    DoMeasurement();
                }
                break;
            }
            default:
                break;
        }
    }

	sVarSink = NULL;
    sMeasurementSink = NULL;
    sApplicationSink = NULL;

    return 0;
}

UINT CommunicationThread(LPVOID pParam)
{
    UINT result;

    ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

    _Module.Init(NULL, ::GetModuleHandle(NULL));
  
    sEvents[EXIT_EVENT] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    sEvents[GUI_EVENT] = ::CreateEvent(NULL, FALSE, FALSE, NULL);

    result = CommunicationThreadInternal();
      
    ::CloseHandle(sEvents[GUI_EVENT]);
    ::CloseHandle(sEvents[EXIT_EVENT]);

    _Module.Term();

    ::CoUninitialize();

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Initialize Communication Thread
///////////////////////////////////////////////////////////////////////////////
BOOL InitCommunication()
{
	if (NULL == sWinThread)
    {
        sWinThread = AfxBeginThread(::CommunicationThread, NULL);
    }

    return NULL != sWinThread;
}

///////////////////////////////////////////////////////////////////////////////
// Exit Communication Thread
///////////////////////////////////////////////////////////////////////////////
BOOL ExitCommunication()
{
    ::SetEvent(sEvents[EXIT_EVENT]);
 
	if (NULL != sWinThread)
    {
        ::WaitForSingleObject(sWinThread->m_hThread, INFINITE);
        sWinThread = NULL;
    }

    return TRUE;
}