// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <msiquery.h>

// WiX Header Files:
#include <wcautil.h>

// TODO: reference additional headers your program requires here
#define STATUS_STRING_SIZE 2048
const int MAX_BUFFER = 1024;

#include "iadmw.h"  // COM Interface header file. 
#include "iiscnfg.h"  // MD_ & IIS_MD_ #defines header file.
#include "atlBase.h"  // ATL support header file.
#include "iwamreg.h"

__declspec(dllexport) UINT __stdcall AddWebServiceExtension(MSIHANDLE hMSI);
__declspec(dllexport) UINT __stdcall DeleteWebServiceExtension(MSIHANDLE hMSI);
__declspec(dllexport) UINT __stdcall RegisterScriptMaps(MSIHANDLE hMSI);
__declspec(dllexport) UINT __stdcall SetCustomErrors(MSIHANDLE hMSI);
__declspec(dllexport) UINT __stdcall CreateVDirMapAgent(MSIHANDLE hMSI);
__declspec(dllexport) UINT __stdcall CreateVDirMapGuide(MSIHANDLE hMSI);
__declspec(dllexport) UINT __stdcall CreateVDirPhpAgent(MSIHANDLE hMSI);
__declspec(dllexport) UINT __stdcall CreateVDirNetAgent(MSIHANDLE hMSI);
__declspec(dllexport) UINT __stdcall CreateVDirJavaAgent(MSIHANDLE hMSI);
__declspec(dllexport) UINT __stdcall DeleteVDirMapGuide(MSIHANDLE hMSI);

BOOL CreateVirtualRoot(
    LPSTR szName,
    LPSTR szPhysicalPath,
    DWORD dwPermissions,
    DWORD dwSite,
	BOOL  bCreateApp,
    CHAR szStatus[STATUS_STRING_SIZE]);

BOOL DeleteVirtualRoot(
    LPSTR szName,
    DWORD dwSite,
	BOOL  bDeleteApp,
    CHAR szStatus[STATUS_STRING_SIZE]
    );

LPWSTR MakeUnicode(
    LPSTR szString
    );

BOOL WrAddKey(
    LPSTR szMetaPath
    );

BOOL WrSetData(
    LPSTR szMetaPath,
    DWORD dwIdentifier,
    DWORD dwAttributes,
    DWORD dwUserType,
    DWORD dwDataType,
    DWORD dwDataSize,
    LPVOID pData
    );

#define MSI_PROP_INSTALLDIR L"WEBEXTENSIONSLOCATION"
#define MSI_PROP_API_TYPE L"IIS_API_TYPE"
#define MSI_PROP_IIS_VERSION L"IISVERSIONMAJOR"