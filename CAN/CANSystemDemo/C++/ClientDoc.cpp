///////////////////////////////////////////////////////////////////////////////
// Header
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Client.h"

#include "ClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////////////
void DoOutput(COutputContainer* container, CSyncObject* obj, LPCSTR text)
{
    CSingleLock lock(obj, TRUE);
    container->push_back(text);
}

void DoClear(COutputContainer* container, CSyncObject* obj)
{
    CSingleLock lock(obj, TRUE);
    container->clear();
}

///////////////////////////////////////////////////////////////////////////////
// Document class
///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CClientDoc, CDocument)
	//{{AFX_MSG_MAP(CClientDoc)
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, OnUpdateConnect)
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, OnUpdateDisconnect)
	ON_COMMAND(ID_START, OnStart)
	ON_UPDATE_COMMAND_UI(ID_START, OnUpdateStart)
	ON_COMMAND(ID_STOP, OnStop)
	ON_UPDATE_COMMAND_UI(ID_STOP, OnUpdateStop)
	ON_COMMAND(ID_ENVIRONMENT, OnEnvironment)
	ON_UPDATE_COMMAND_UI(ID_ENVIRONMENT, OnUpdateEnvironment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// Construction & Initialization
///////////////////////////////////////////////////////////////////////////////
CClientDoc::CClientDoc()
{
	SetOutput(this);
}

CClientDoc::~CClientDoc()
{
    SetOutput(NULL);
}

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////
void CClientDoc::Output(LPCSTR text)
{
    DoOutput(&mOutputContainer, &mCS, text);
    POSITION pos = GetFirstViewPosition();
    CView*   view = GetNextView(pos);

    if (NULL != view)
    {
        view->PostMessage(WM_COMMAND, ID_UPDATE_LIST, 0);
    }
}

BOOL CClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	DoClear(&mOutputContainer, &mCS);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Serialization
///////////////////////////////////////////////////////////////////////////////
void CClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

///////////////////////////////////////////////////////////////////////////////
// Diagnostics
///////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////
// Commands
///////////////////////////////////////////////////////////////////////////////
void CClientDoc::OnConnect() 
{
    Connect();
}

void CClientDoc::OnUpdateConnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsConnected());
}

void CClientDoc::OnDisconnect() 
{
    Disconnect();
}

void CClientDoc::OnUpdateDisconnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsConnected());
}

void CClientDoc::OnStart() 
{
	Start();
}

void CClientDoc::OnUpdateStart(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(IsConnected() && !IsStarted());
}

void CClientDoc::OnStop() 
{
	Stop();
}

void CClientDoc::OnUpdateStop(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(IsConnected() && IsStarted());
}

void CClientDoc::OnEnvironment() 
{
    ShowEnvironmentVariables();
}

void CClientDoc::OnUpdateEnvironment(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(IsConnected());
}

BOOL CClientDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
    return TRUE;
}

BOOL CClientDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
    return TRUE;
}
