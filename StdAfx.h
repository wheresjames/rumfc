// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E304E0F9_8A09_4D33_BB4D_3CB2D8BBA0E6__INCLUDED_)
#define AFX_STDAFX_H__E304E0F9_8A09_4D33_BB4D_3CB2D8BBA0E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

// Turn off socket 1 interface
#define _MSWSOCK_

#include <afx.h>
#include <afxwin.h>
#include <afxole.h>
//#include <afxsock.h>		// MFC socket extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

/// Socket 2 interface
#include <WinSock2.h>

#include <afxsock.h>		// MFC socket extensions

#include "rumfc.h"
#include <minmax.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E304E0F9_8A09_4D33_BB4D_3CB2D8BBA0E6__INCLUDED_)
