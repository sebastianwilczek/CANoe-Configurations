// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__FF7E0E59_0166_11D4_861F_00105A3E017B__INCLUDED_)
#define AFX_STDAFX_H__FF7E0E59_0166_11D4_861F_00105A3E017B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined ( _MSC_VER ) && ( _MSC_VER >= 1310 )
#define WINVER         0x0A00 // Win10
#define _WIN32_WINNT   0x0A00 // Win10
#define _WIN32_IE      0x0501 // Win2K Shell and Common Controls
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_DCOM         // Use DCOM features

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxmt.h>          // Use MT features

#include <string>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__FF7E0E59_0166_11D4_861F_00105A3E017B__INCLUDED_)
