// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__734F1522_1F81_4CA5_A031_8C2D94540578__INCLUDED_)
#define AFX_STDAFX_H__734F1522_1F81_4CA5_A031_8C2D94540578__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <msiquery.h>

// TODO: reference additional headers your program requires here

//#define MSGBOX_DEBUGGING

#ifdef MSGBOX_DEBUGGING
#define MSGBOX_DEBUG_MSG(msg) ::MessageBoxW(NULL, L##msg, L"Debugging apache_actions", MB_OK)
#else
#define MSGBOX_DEBUG_MSG(msg)
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__734F1522_1F81_4CA5_A031_8C2D94540578__INCLUDED_)
