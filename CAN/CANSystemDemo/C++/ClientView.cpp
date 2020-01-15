///////////////////////////////////////////////////////////////////////////////
// Header
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Client.h"

#include "ClientDoc.h"
#include "ClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// View class
///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CClientView, CListView)

BEGIN_MESSAGE_MAP(CClientView, CListView)
  ON_COMMAND(ID_UPDATE_LIST, OnUpdateList)
	//{{AFX_MSG_MAP(CClientView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// Construction & Initialization
///////////////////////////////////////////////////////////////////////////////
CClientView::CClientView()
{
}

CClientView::~CClientView()
{
}

BOOL CClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_REPORT;

	return CListView::PreCreateWindow(cs);
}

BOOL CClientView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
    BOOL created = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

    if (created)
    {
        CListCtrl& control = GetListCtrl();

        control.InsertColumn(1, _T("Output"), LVCFMT_LEFT, 400);
    }

    return created;
}

///////////////////////////////////////////////////////////////////////////////
// Drawing
///////////////////////////////////////////////////////////////////////////////
void CClientView::OnDraw(CDC* pDC)
{
	CClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CClientView::OnInitialUpdate()
{
  CListCtrl& control = GetListCtrl();

  control.DeleteAllItems();

	CListView::OnInitialUpdate();
}

///////////////////////////////////////////////////////////////////////////////
// Diagnostics
///////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CClientView::AssertValid() const
{
	CListView::AssertValid();
}

void CClientView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CClientDoc* CClientView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClientDoc)));
	return (CClientDoc*)m_pDocument;
}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////
// Message Handler
///////////////////////////////////////////////////////////////////////////////
void CClientView::OnUpdateList()
{
    CClientDoc* pDoc = GetDocument();
	
    ASSERT_VALID(pDoc);

    CSingleLock                      lock(pDoc->GetSyncObject(), TRUE);
    const COutputContainer&          container = pDoc->GetOutputContainer();
    COutputContainer::const_iterator iter = container.begin();
    CListCtrl&                       control = GetListCtrl();
    int                              index = control.GetItemCount();

    iter += index;

    for (; iter != container.end(); iter++)
    {
        control.InsertItem(index++, iter->c_str());
    }
  
    index = control.GetItemCount();

    if (index > 0)
    {
        control.EnsureVisible(index - 1, FALSE);
    }
}

