///////////////////////////////////////////////////////////////////////////////
// Header
///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_CLIENTDOC_H__FF7E0E5D_0166_11D4_861F_00105A3E017B__INCLUDED_)
#define AFX_CLIENTDOC_H__FF7E0E5D_0166_11D4_861F_00105A3E017B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include <string>
#include <vector>

using std::vector;
#if defined (_UNICODE )
using std::wstring;
typedef vector<wstring> COutputContainer;
#else
using std::string;
typedef vector<string> COutputContainer;
#endif

#include "Communication.h"

///////////////////////////////////////////////////////////////////////////////
// Document class
///////////////////////////////////////////////////////////////////////////////
class CClientDoc : public CDocument
                 , public CListOutput
{
protected: // create from serialization only
	CClientDoc();
	DECLARE_DYNCREATE(CClientDoc)

// Attributes
public:

// Operations
public:
  virtual void      Output(LPCSTR text);

  COutputContainer& GetOutputContainer() { return mOutputContainer;}
  CSyncObject*      GetSyncObject(){ return &mCS;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CClientDoc)
	afx_msg void OnConnect();
	afx_msg void OnUpdateConnect(CCmdUI* pCmdUI);
	afx_msg void OnDisconnect();
	afx_msg void OnUpdateDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnStart();
	afx_msg void OnUpdateStart(CCmdUI* pCmdUI);
	afx_msg void OnStop();
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnEnvironment();
	afx_msg void OnUpdateEnvironment(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  COutputContainer mOutputContainer;
  CCriticalSection mCS;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTDOC_H__FF7E0E5D_0166_11D4_861F_00105A3E017B__INCLUDED_)
