/******************************************************************************
 *
 * Copyright (C) 1993-2006 by Autodesk, Inc.
 *
 * Permission to use, copy, modify, and distribute this software in
 * object code form for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both that copyright notice and the limited warranty and
 * restricted rights notice below appear in all supporting 
 * documentation.
 *
 * AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
 * AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
 * DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR FREE.
 *
 * Use, duplication, or disclosure by the U.S. Government is subject to 
 * restrictions set forth in FAR 52.227-19 (Commercial Computer
 * Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
 * (Rights in Technical Data and Computer Software), as applicable. 
 *
 * DESCRIPTION: iis_actions
 */
#include "stdafx.h"

CComPtr <IMSAdminBase> g_spAdminBase;  
CComPtr <IWamAdmin> g_spWamAdmin;  

UINT __stdcall AddWebServiceExtension(MSIHANDLE hMSI)
{
	HRESULT hResult = S_OK;
	UINT er = ERROR_SUCCESS;

	hResult = WcaInitialize(hMSI, "AddWebServiceExtension");
	ExitOnFailure(hResult, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.
	METADATA_HANDLE hMetaData;
	IMSAdminBase*   pIAdminBase;
	METADATA_RECORD metaDataRecord;		// The MetaData_Record structure
	DWORD dwBuffer = 65535;
	PBYTE pbBuffer = new BYTE[dwBuffer];
	DWORD dwBuffer_new;
	PBYTE pbBuffer_new = new BYTE[dwBuffer];
	PBYTE temp;
	DWORD dwMemIndex = 0;
	WCHAR wcsListTerminator = L'\0';
	CHAR  szInstallDir[MAX_BUFFER];
	_TCHAR installDir[MAX_BUFFER] = _T("");
	DWORD installDirLen = MAX_BUFFER;
	WCHAR wszApiType[MAX_BUFFER + 1] = L"";
	CHAR  szApiType[MAX_BUFFER];
	CHAR  phpIsApiExe[MAX_BUFFER];
	WCHAR wcsPhpIsApiWebServiceExtension[METADATA_MAX_NAME_LEN];
	int   wcsPhpIsApiLen = 0;
	CHAR  fcgiCgiExe[MAX_BUFFER];
	WCHAR wcsFastCGIWebServiceExtension[METADATA_MAX_NAME_LEN];
	int   wcsFastCGILen = 0;
	int   nLen = 0;

	MsiGetProperty(hMSI, MSI_PROP_INSTALLDIR, installDir, &installDirLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, installDir, installDirLen, szInstallDir, MAX_BUFFER, NULL, NULL);
	if ((nLen>0) && (szInstallDir[nLen-1] != '\\'))
		szInstallDir[nLen++] = '\\';
	szInstallDir[nLen] = '\0';

	//::MessageBoxA(NULL, szInstallDir, "AddWebServiceExtension: Install Directory", MB_OK);

	// retrieve API_TYPE MSI property
	DWORD dwDataLen = MAX_BUFFER;
	MsiGetPropertyW(hMSI, MSI_PROP_API_TYPE, wszApiType, &dwDataLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, wszApiType, dwDataLen, szApiType, MAX_BUFFER, NULL, NULL);
	szApiType[nLen] = '\0';

	strcpy(phpIsApiExe, szInstallDir);
	strcat(phpIsApiExe, "Php\\php5isapi.dll");

	strcpy(fcgiCgiExe, szInstallDir);
	strcat(fcgiCgiExe, "www\\mapagent\\isapi_MapAgent.dll");

	memset(wcsPhpIsApiWebServiceExtension, 0, sizeof(wcsPhpIsApiWebServiceExtension));
	swprintf(wcsPhpIsApiWebServiceExtension, L"1,%S,1,,%S PHP ISAPI",phpIsApiExe, MG_PRODUCT);
	wcsPhpIsApiLen = (wcslen(wcsPhpIsApiWebServiceExtension) + 1) * sizeof(WCHAR);

	memset(wcsFastCGIWebServiceExtension, 0, sizeof(wcsFastCGIWebServiceExtension));
	swprintf(wcsFastCGIWebServiceExtension, L"1,%S,1,,%S ISAPI",fcgiCgiExe, MG_PRODUCT);
	wcsFastCGILen = (wcslen(wcsFastCGIWebServiceExtension) + 1) * sizeof(WCHAR);

	// Get the IMSAdminBase object

	CoInitialize(NULL);
	hResult = CoCreateInstance(CLSID_MSAdminBase, 
		NULL, 
		CLSCTX_ALL, 
		IID_IMSAdminBase, 
		(void**) &pIAdminBase); 

	if (FAILED(hResult))  
	{
		CoUninitialize();
		return FALSE;  
	}

	// Open key with "/LM" path
	// Get the read and write permission

	hResult = pIAdminBase->OpenKey(METADATA_MASTER_ROOT_HANDLE,
		L"/LM", 
		METADATA_PERMISSION_READ|METADATA_PERMISSION_WRITE, 
		60000, 
		&hMetaData); 

	if (FAILED(hResult))
	{	
		pIAdminBase->Release();
		CoUninitialize();
		return FALSE;
	}

	// First set the data size of this multi-string Web Service Extension List to 0.
	// Probe into this MD_WEB_SVC_EXT_RESTRICTION_LIST property for the required
	// data size of this multi-string.
	// During the second loop, we know the real data size of this multi-string,
	// go ahead and retrieve the existing Web Service Extension List.

	ZeroMemory((LPVOID)&metaDataRecord, sizeof(metaDataRecord));

	//If METADATA_INHERIT is not set, GetData will fail (0x800cc801 - not found).
	metaDataRecord.dwMDIdentifier = IIS_MD_HTTP_BASE+168; // IIS_MD_HTTP_BASE+168 = MD_WEB_SVC_EXT_RESTRICTION_LIST
	metaDataRecord.dwMDAttributes = METADATA_INHERIT;
	metaDataRecord.dwMDUserType = IIS_MD_UT_SERVER;
	metaDataRecord.dwMDDataType = MULTISZ_METADATA;
	metaDataRecord.dwMDDataTag = 0;
	metaDataRecord.dwMDDataLen = dwBuffer;
	metaDataRecord.pbMDData = pbBuffer;

	DWORD dwRequiredSize = 0;
	// Script maps are set under the node of /W3Svc. 
	hResult = pIAdminBase->GetData(hMetaData,
		L"/W3SVC", //relative to /LM
		&metaDataRecord,
		&dwRequiredSize);

	if (hResult != ERROR_SUCCESS)
	{
		pIAdminBase->CloseKey(hMetaData); 
		pIAdminBase->Release();
		CoUninitialize();
		WcaLog(LOGMSG_STANDARD, "AddWebServiceExtension: GetData failed: 0x%x", hResult);
		return FALSE;
	}

	dwBuffer_new = metaDataRecord.dwMDDataLen;
	memset(pbBuffer_new, 0, sizeof(pbBuffer_new));

	temp = pbBuffer;
	while (*temp)
	{
		int tempLen = (wcslen((wchar_t *)temp) + 1) * sizeof(WCHAR);
		memcpy(pbBuffer_new + dwMemIndex, temp, tempLen);
		dwMemIndex += tempLen;
		temp += tempLen;
	}

	memcpy(pbBuffer_new + dwMemIndex, (PBYTE)wcsPhpIsApiWebServiceExtension, wcsPhpIsApiLen);		
	dwMemIndex += wcsPhpIsApiLen;
	dwBuffer_new += wcsPhpIsApiLen;

	memcpy(pbBuffer_new + dwMemIndex, (PBYTE)wcsFastCGIWebServiceExtension, wcsFastCGILen);		
	dwMemIndex += wcsFastCGILen;
	dwBuffer_new += wcsFastCGILen;

	memcpy(pbBuffer_new + dwMemIndex, &wcsListTerminator, sizeof(WCHAR));

	metaDataRecord.pbMDData = (UCHAR*)pbBuffer_new;
	metaDataRecord.dwMDDataLen = dwBuffer_new;

	hResult = pIAdminBase->SetData(hMetaData, L"/W3SVC", &metaDataRecord);
	if (FAILED (hResult) )
	{
		WcaLog(LOGMSG_STANDARD, "AddWebServiceExtension: SetData failed: 0x%x", hResult);
	}

	pIAdminBase->CloseKey(hMetaData); 
	pIAdminBase->Release ();
	CoUninitialize ();

LExit:
	er = SUCCEEDED(hResult) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall DeleteWebServiceExtension(MSIHANDLE hMSI)
{
	HRESULT hResult = S_OK;
	UINT er = ERROR_SUCCESS;

	hResult = WcaInitialize(hMSI, "DeleteWebServiceExtension");
	ExitOnFailure(hResult, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.
	METADATA_HANDLE hMetaData;
	IMSAdminBase*   pIAdminBase;
	METADATA_RECORD metaDataRecord;		// The MetaData_Record structure
	DWORD dwBuffer = 65535;
	PBYTE pbBuffer = new BYTE[dwBuffer];
	DWORD dwBuffer_new;
	PBYTE pbBuffer_new = new BYTE[dwBuffer];
	PBYTE temp;
	DWORD dwMemIndex = 0;
	WCHAR wcsListTerminator = L'\0';
	CHAR  szInstallDir[MAX_BUFFER];
	_TCHAR installDir[MAX_BUFFER] = _T("");
	DWORD installDirLen = MAX_BUFFER;
	CHAR  fcgiCgiExe[MAX_BUFFER];
	WCHAR wcsFastCGIWebServiceExtension[METADATA_MAX_NAME_LEN];
	int   wcsFastCGILen = 0;
	CHAR  phpIsApiExe[MAX_BUFFER];
	WCHAR wcsPhpIsApiWebServiceExtension[METADATA_MAX_NAME_LEN];
	int   wcsPhpIsApiLen = 0;
	int   nLen = 0;

	MsiGetProperty(hMSI, MSI_PROP_INSTALLDIR, installDir, &installDirLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, installDir, installDirLen, szInstallDir, MAX_BUFFER, NULL, NULL);
	if ((nLen>0) && (szInstallDir[nLen-1] != '\\'))
		szInstallDir[nLen++] = '\\';
	szInstallDir[nLen] = '\0';

	strcpy(phpIsApiExe, szInstallDir);
	strcat(phpIsApiExe, "Php\\php5isapi.dll");

	strcpy(fcgiCgiExe, szInstallDir);
	strcat(fcgiCgiExe, "www\\mapagent\\isapi_MapAgent.dll");

	memset(wcsPhpIsApiWebServiceExtension, 0, sizeof(wcsPhpIsApiWebServiceExtension));
	swprintf(wcsPhpIsApiWebServiceExtension, L"1,%S,1,,MapGuide Open Source v2.0 PHP ISAPI",phpIsApiExe);
	wcsPhpIsApiLen = (wcslen(wcsPhpIsApiWebServiceExtension) + 1) * sizeof(WCHAR);

	memset(wcsFastCGIWebServiceExtension, 0, sizeof(wcsFastCGIWebServiceExtension));
	swprintf(wcsFastCGIWebServiceExtension, L"1,%S,1,,MapGuide Open Source v2.0 ISAPI",fcgiCgiExe);
	wcsFastCGILen = (wcslen(wcsFastCGIWebServiceExtension) + 1) * sizeof(WCHAR);

	// Get the IMSAdminBase object

	CoInitialize(NULL);
	hResult = CoCreateInstance(CLSID_MSAdminBase, 
		NULL, 
		CLSCTX_ALL, 
		IID_IMSAdminBase, 
		(void**) &pIAdminBase); 

	if (FAILED(hResult))  
	{
		CoUninitialize();
		return FALSE;  
	}

	// Open key with "/LM" path
	// Get the read and write permission

	hResult = pIAdminBase->OpenKey(METADATA_MASTER_ROOT_HANDLE,
		L"/LM", 
		METADATA_PERMISSION_READ|METADATA_PERMISSION_WRITE, 
		60000, 
		&hMetaData); 

	if (FAILED(hResult))
	{	
		pIAdminBase->Release();
		CoUninitialize();
		return FALSE;
	}

	// First set the data size of this multi-string Web Service Extension List to 0.
	// Probe into this MD_WEB_SVC_EXT_RESTRICTION_LIST property for the required
	// data size of this multi-string.
	// During the second loop, we know the real data size of this multi-string,
	// go ahead and retrieve the existing Web Service Extension List.

	ZeroMemory((LPVOID)&metaDataRecord, sizeof(metaDataRecord));

	//If METADATA_INHERIT is not set, GetData will fail (0x800cc801 - not found).
	metaDataRecord.dwMDIdentifier = IIS_MD_HTTP_BASE+168; // IIS_MD_HTTP_BASE+168 = MD_WEB_SVC_EXT_RESTRICTION_LIST
	metaDataRecord.dwMDAttributes = METADATA_INHERIT;
	metaDataRecord.dwMDUserType = IIS_MD_UT_SERVER;
	metaDataRecord.dwMDDataType = MULTISZ_METADATA;
	metaDataRecord.dwMDDataTag = 0;
	metaDataRecord.dwMDDataLen = dwBuffer;
	metaDataRecord.pbMDData = pbBuffer;

	DWORD dwRequiredSize = 0;
	// Script maps are set under the node of /W3Svc. 
	hResult = pIAdminBase->GetData(hMetaData,
		L"/W3SVC", //relative to /LM
		&metaDataRecord,
		&dwRequiredSize);

	if (hResult != ERROR_SUCCESS)
	{
		pIAdminBase->CloseKey(hMetaData); 
		pIAdminBase->Release();
		CoUninitialize();
		WcaLog(LOGMSG_STANDARD, "DeleteWebServiceExtension: GetData failed: 0x%x", hResult);
		return FALSE;
	}

	dwBuffer_new = metaDataRecord.dwMDDataLen;
	memset(pbBuffer_new, 0, sizeof(pbBuffer_new));

	temp = pbBuffer;
	while (*temp)
	{
		int tempLen = (wcslen((wchar_t *)temp) + 1) * sizeof(WCHAR);
		if (wcsicmp((wchar_t *)temp, wcsPhpIsApiWebServiceExtension) == 0 ||
			wcsicmp((wchar_t *)temp, wcsFastCGIWebServiceExtension) == 0 )
		{
			dwBuffer_new -= tempLen;
		}
		else
		{
			memcpy(pbBuffer_new + dwMemIndex, temp, tempLen);
			dwMemIndex += tempLen;
		}
		temp += tempLen;
	}

	memcpy(pbBuffer_new + dwMemIndex, &wcsListTerminator, sizeof(WCHAR));

	metaDataRecord.pbMDData = (UCHAR*)pbBuffer_new;
	metaDataRecord.dwMDDataLen = dwBuffer_new;

	hResult = pIAdminBase->SetData(hMetaData, L"/W3SVC", &metaDataRecord);
	if (FAILED (hResult) )
	{
		WcaLog(LOGMSG_STANDARD, "DeleteWebServiceExtension: SetData failed: 0x%x", hResult);
	}

	pIAdminBase->CloseKey(hMetaData); 
	pIAdminBase->Release ();
	CoUninitialize ();

LExit:
	er = SUCCEEDED(hResult) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall RegisterScriptMaps(MSIHANDLE hMSI)
{
	CComPtr <IMSAdminBase> pIMeta;

	HRESULT hRes = S_OK;
	UINT er = ERROR_SUCCESS;

	hRes = WcaInitialize(hMSI, "RegisterScriptMaps");
	ExitOnFailure(hRes, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.
	DWORD indx = 0;
	DWORD buffer = 0; 
	DWORD dwBuffer = 65535;
	DWORD dwBuffer_new = 0;
	METADATA_HANDLE MyHandle; 
	METADATA_RECORD MyRecord; 
	PBYTE pbBuffer = new BYTE[dwBuffer];
	PBYTE pbBuffer_new = new BYTE[dwBuffer];
	PBYTE temp;
	WCHAR wcsScriptMap[METADATA_MAX_NAME_LEN]; 
	WCHAR SubKeyName[METADATA_MAX_NAME_LEN];

	DWORD ServerInstance = 1;
	char *VirtualPath = "/mapguide";
	DWORD dwMemIndex = 0;
	CHAR   fcgiDir[MAX_BUFFER];
	CHAR   phpDir[MAX_BUFFER];
	_TCHAR wcsBuffer[MAX_BUFFER] = _T("");
	DWORD  wcsBufferLen;
	_TCHAR installDir[MAX_BUFFER] = _T("");
	DWORD installDirLen = MAX_BUFFER;
	CHAR   szInstallDir[MAX_BUFFER];
	_TCHAR shortInstallDir[MAX_BUFFER] = _T("");
	DWORD  shortInstallDirLen;
	_TCHAR wcsIISVersion[MAX_BUFFER] = _T("");
	DWORD  wcsIISVersionLen = MAX_BUFFER;
	CHAR   szIISVersion[MAX_BUFFER];
	BOOL   shortPath = false;
	char extBuffer[METADATA_MAX_NAME_LEN];
	int smLen;
	int nLen;

	MsiGetProperty(hMSI, MSI_PROP_INSTALLDIR, installDir, &installDirLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, installDir, installDirLen, szInstallDir, MAX_BUFFER, NULL, NULL);
	if ((nLen>0) && (szInstallDir[nLen-1] != '\\'))
		szInstallDir[nLen++] = '\\';
	szInstallDir[nLen] = '\0';

	// retrieve API_TYPE MSI property
	MsiGetPropertyW(hMSI, MSI_PROP_IIS_VERSION, wcsIISVersion, &wcsIISVersionLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, wcsIISVersion, wcsIISVersionLen, szIISVersion, MAX_BUFFER, NULL, NULL);
	szIISVersion[nLen] = '\0';

	if ( strcmp(szIISVersion,"#5") == 0 )
		shortPath = true;

	if ( shortPath )
	{
		shortInstallDirLen = GetShortPathName(installDir, shortInstallDir, MAX_BUFFER);
		if ( !shortInstallDirLen )
		{
			WcaLog(LOGMSG_STANDARD, "RegisterScriptMaps: GetShortPathName returned %d", GetLastError() );
		}

		nLen = WideCharToMultiByte(CP_ACP, 0, shortInstallDir, shortInstallDirLen, szInstallDir, MAX_BUFFER, NULL, NULL);

		if ((nLen>0) && (szInstallDir[nLen-1] != '\\'))
			szInstallDir[nLen++] = '\\';
		szInstallDir[nLen] = '\0';
	}

	if ( shortPath )
	{
		strcpy(phpDir, szInstallDir);
		strcat(phpDir, "Php\\php5is~1.dll");
		strcpy(fcgiDir, szInstallDir);
		strcat(fcgiDir, "www\\mapagent\\isapi_~1.dll");
	}
	else
	{
		strcpy(phpDir, "\"");
		strcat(phpDir, szInstallDir);
		strcat(phpDir, "Php\\php5isapi.dll\"");
		strcpy(fcgiDir, "\"");
		strcat(fcgiDir, szInstallDir);
		strcat(fcgiDir, "www\\mapagent\\isapi_MapAgent.dll\"");
	}

	wcsBufferLen = MultiByteToWideChar(CP_ACP, 0, fcgiDir, strlen(fcgiDir), wcsBuffer, MAX_BUFFER);
	wcsBuffer[wcsBufferLen] = '\0';
	WcaLog(LOGMSG_STANDARD, "RegisterScriptMaps: FcgiMapping=%s", wcsBuffer);

	wcsBufferLen = MultiByteToWideChar(CP_ACP, 0, phpDir, strlen(phpDir),wcsBuffer, MAX_BUFFER);
	wcsBuffer[wcsBufferLen] = '\0';
	WcaLog(LOGMSG_STANDARD, "RegisterScriptMaps: PhpMapping=%s", wcsBuffer);

	CoInitialize(NULL);
	hRes = CoCreateInstance( CLSID_MSAdminBase, 
		NULL,
		CLSCTX_ALL,
		IID_IMSAdminBase,
		(void **) &pIMeta );

	if (FAILED(hRes))
	{
		return 0;
	}

	hRes = pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE,
		L"/LM/W3SVC",
		METADATA_PERMISSION_WRITE | METADATA_PERMISSION_READ,
		20,
		&MyHandle ); 
	if (SUCCEEDED(hRes)) 
	{
		memset(SubKeyName , 0, sizeof(SubKeyName));
		swprintf(SubKeyName,L"/%ld/ROOT%S",ServerInstance,VirtualPath);
		MyRecord.dwMDIdentifier = MD_SCRIPT_MAPS;
		MyRecord.dwMDAttributes = METADATA_INHERIT;
		MyRecord.dwMDUserType = IIS_MD_UT_FILE;
		MyRecord.dwMDDataType = MULTISZ_METADATA;
		MyRecord.pbMDData = pbBuffer;
		MyRecord.dwMDDataLen = dwBuffer;

		hRes = pIMeta->GetData(MyHandle, SubKeyName, &MyRecord, &buffer);
		if (FAILED (hRes) )
		{
			printf("RegisterScriptMaps: GetData failed: 0x%x!\n", hRes);
		}
		else
		{
			printf("RegisterScriptMaps: GetData returned: %d\n", hRes);
		}
		dwBuffer_new = MyRecord.dwMDDataLen;
		temp = pbBuffer;
		memset(pbBuffer_new, 0, sizeof(pbBuffer_new));
		while (*temp)
		{
			int tempLen = (wcslen((wchar_t *)temp) + 1) * sizeof(WCHAR);
			memset(extBuffer, 0, sizeof(extBuffer));
			sprintf(extBuffer, "%ls\n", temp);
			if (wcslen((wchar_t *)temp) > 5)
				for (int i=1; i<5; i++) extBuffer[i] = tolower(extBuffer[i]);
			if (strncmp(extBuffer, ".php,", 5) || strncmp(extBuffer, ".fcgi,", 6))
			{
				memcpy(pbBuffer_new + dwMemIndex, temp, tempLen);
				dwMemIndex += tempLen;
			}
			else
			{
				dwBuffer_new -= tempLen;
			}
			temp += tempLen;
		}

		WCHAR wcsListTerminator = L'\0';

		memset(wcsScriptMap, 0, sizeof(wcsScriptMap));
		swprintf(wcsScriptMap, L".fcgi,%S,5,GET,POST",fcgiDir);
		smLen = wcslen(wcsScriptMap) + 1;

		memcpy(pbBuffer_new + dwMemIndex, &wcsScriptMap, sizeof(wcsScriptMap));		
		dwMemIndex += smLen * sizeof(WCHAR);
		dwBuffer_new += smLen * sizeof(WCHAR);

		memset(wcsScriptMap, 0, sizeof(wcsScriptMap));
		swprintf(wcsScriptMap, L".php,%S,5,GET,POST",phpDir);
		smLen = wcslen(wcsScriptMap) + 1;

		memcpy(pbBuffer_new + dwMemIndex, &wcsScriptMap, sizeof(wcsScriptMap));		
		dwMemIndex += smLen * sizeof(WCHAR);
		dwBuffer_new += smLen * sizeof(WCHAR);

		memcpy(pbBuffer_new + dwMemIndex, &wcsListTerminator, sizeof(WCHAR));

		MyRecord.pbMDData = (UCHAR*)pbBuffer_new;
		MyRecord.dwMDDataLen = dwBuffer_new;

		hRes = pIMeta->SetData(MyHandle, SubKeyName, &MyRecord);
		if (FAILED (hRes) )
		{
			printf("RegisterScriptMaps: SetData failed: 0x%x!\n", hRes);
		}
		else
		{
			printf("RegisterScriptMaps: SetData returned: %d\n", hRes);
		}
	}

	pIMeta->CloseKey(MyHandle);

LExit:
	er = SUCCEEDED(hRes) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall SetCustomErrors(MSIHANDLE hInstall)
{
	CComPtr <IMSAdminBase> pIMeta;
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;

	hr = WcaInitialize(hInstall, "SetCustomErrors");
	ExitOnFailure(hr, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.
	HRESULT hRes = 0;
	DWORD indx = 0;
	DWORD buffer = 0; 
	DWORD dwBuffer = 65535;
	DWORD dwBuffer_new = 0;
	METADATA_HANDLE MyHandle; 
	METADATA_RECORD MyRecord; 
	PBYTE pbBuffer = new BYTE[dwBuffer];
	PBYTE pbBuffer_new = new BYTE[dwBuffer];
	WCHAR SubKeyName[METADATA_MAX_NAME_LEN];

	PBYTE temp;
	char extBuffer[METADATA_MAX_NAME_LEN];
	DWORD ServerInstance = 1;
	char *VirtualPath = "/mapguide";
	DWORD dwMemIndex = 0;

	CoInitialize(NULL);
	hRes = CoCreateInstance( CLSID_MSAdminBase, 
		NULL,
		CLSCTX_ALL,
		IID_IMSAdminBase,
		(void **) &pIMeta );

	if (FAILED(hRes))
	{
		return 0;
	}

	hRes = pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE,
		L"/LM/W3SVC",
		METADATA_PERMISSION_WRITE | METADATA_PERMISSION_READ,
		20,
		&MyHandle ); 
	if (SUCCEEDED(hRes)) 
	{
		memset(SubKeyName , 0, sizeof(SubKeyName));
		swprintf(SubKeyName,L"/%ld/ROOT%S",ServerInstance,VirtualPath);
		MyRecord.dwMDIdentifier = MD_CUSTOM_ERROR;
		MyRecord.dwMDAttributes = METADATA_INHERIT;
		MyRecord.dwMDUserType = IIS_MD_UT_FILE;
		MyRecord.dwMDDataType = MULTISZ_METADATA;
		MyRecord.pbMDData = pbBuffer;
		MyRecord.dwMDDataLen = dwBuffer;

		hRes = pIMeta->GetData(MyHandle, SubKeyName, &MyRecord, &buffer);
		if (FAILED (hRes) )
		{
			printf("SetCustomErrors: GetData failed: 0x%x!\n", hRes);
		}
		else
		{
			printf("SetCustomErrors: GetData returned: %d\n", hRes);
		}
		dwBuffer_new = MyRecord.dwMDDataLen;
		temp = pbBuffer;
		memset(pbBuffer_new, 0, sizeof(pbBuffer_new));
		while (*temp)
		{
			int tempLen = (wcslen((wchar_t *)temp) + 1) * sizeof(WCHAR);
			memset(extBuffer, 0, sizeof(extBuffer));
			sprintf(extBuffer, "%ls\n", temp);
			if (!strncmp(extBuffer, "401", 3))
			{
				dwBuffer_new -= tempLen;
			}
			else
			{
				memcpy(pbBuffer_new + dwMemIndex, temp, tempLen);
				dwMemIndex += tempLen;
			}
			temp += tempLen;
		}

		WCHAR wcsListTerminator = L'\0';
		memcpy(pbBuffer_new + dwMemIndex, &wcsListTerminator, sizeof(WCHAR));		

		MyRecord.pbMDData = (UCHAR*)pbBuffer_new;
		MyRecord.dwMDDataLen = dwBuffer_new;

		hRes = pIMeta->SetData(MyHandle, SubKeyName, &MyRecord);
		if (FAILED (hRes) )
		{
			printf("SetCustomErrors: SetData failed: 0x%x!\n", hRes);
		}
		else
		{
			printf("SetCustomErrors: SetData returned: %d\n", hRes);
		}
	}
	pIMeta->CloseKey(MyHandle);

LExit:
	er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall CreateVDirMapAgent(MSIHANDLE hMSI)
{
	CComPtr <IMSAdminBase> pIMeta;
	HRESULT hRes = S_OK;
	UINT er = ERROR_SUCCESS;

	hRes = WcaInitialize(hMSI, "CreateVDirMapAgent");
	ExitOnFailure(hRes, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.

	DWORD indx = 0;
	DWORD buffer = 0; 
	DWORD dwBuffer = 65535;
	DWORD dwBuffer_new = 0;
	METADATA_HANDLE MyHandle; 
	METADATA_RECORD MyRecord; 
	PBYTE pbBuffer = new BYTE[dwBuffer];
	PBYTE pbBuffer_new = new BYTE[dwBuffer];
	WCHAR SubKeyName[METADATA_MAX_NAME_LEN];
	WCHAR TestKey[METADATA_MAX_NAME_LEN];

	DWORD  ServerInstance = 1;
	char  *VirtualPath = "/mapguide/mapagent";
	char  *AppName = "mapagent";
	DWORD  dwMemIndex = 0;
	_TCHAR installDir[MAX_BUFFER] = _T("");
	DWORD  installDirLen = MAX_BUFFER;
	_TCHAR wcsMapAgentDir[MAX_BUFFER] = _T("");
	DWORD  wcsMapAgentDirLen;
	CHAR   szStatus[STATUS_STRING_SIZE];
	CHAR   szInstallDir[MAX_BUFFER];
	CHAR   mapAgentDir[MAX_BUFFER];
	int    nLen;

	MsiGetProperty(hMSI, MSI_PROP_INSTALLDIR, installDir, &installDirLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, installDir, installDirLen, szInstallDir, MAX_BUFFER, NULL, NULL);
	if ((nLen>0) && (szInstallDir[nLen-1] != '\\'))
		szInstallDir[nLen++] = '\\';
	szInstallDir[nLen] = '\0';

	strcpy(mapAgentDir, szInstallDir);
	strcat(mapAgentDir, "www\\mapagent");

	wcsMapAgentDirLen = MultiByteToWideChar(CP_ACP, 0, mapAgentDir, strlen(mapAgentDir), wcsMapAgentDir, MAX_BUFFER);
	wcsMapAgentDir[wcsMapAgentDirLen] = '\0';
	WcaLog(LOGMSG_STANDARD, "CreateVDirMapAgent: VirtualRoot=%s", wcsMapAgentDir );

	DeleteVirtualRoot("mapguide/mapagent", 1, TRUE, szStatus);
	CreateVirtualRoot("mapguide/mapagent", mapAgentDir, 517, 1, TRUE, szStatus);

	CoInitialize(NULL);
	hRes = CoCreateInstance( CLSID_MSAdminBase, NULL, CLSCTX_ALL,
		IID_IMSAdminBase, (void **) &pIMeta );

	if (FAILED(hRes))
	{
		CoUninitialize();
		return 0;
	}

	hRes = pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE,
		L"/LM/W3SVC",
		METADATA_PERMISSION_WRITE | METADATA_PERMISSION_READ,
		20,
		&MyHandle ); 
	if (SUCCEEDED(hRes)) 
	{
		memset(SubKeyName , 0, sizeof(SubKeyName));
		swprintf(SubKeyName,L"/%ld/ROOT%S",ServerInstance,VirtualPath);

		memset(TestKey , 0, sizeof(TestKey));
		swprintf(TestKey, L"%S", AppName);
		MyRecord.dwMDIdentifier = MD_APP_FRIENDLY_NAME;
		MyRecord.dwMDAttributes = METADATA_INHERIT;
		MyRecord.dwMDUserType = IIS_MD_UT_WAM;
		MyRecord.dwMDDataType = STRING_METADATA;
		MyRecord.pbMDData = (PBYTE)&TestKey;
		MyRecord.dwMDDataLen = (wcslen(TestKey) + 1) * sizeof(WCHAR);

		hRes = pIMeta->SetData(MyHandle, SubKeyName, &MyRecord);

	}

	pIMeta->CloseKey(MyHandle);

	return ERROR_SUCCESS;

LExit:
	er = SUCCEEDED(hRes) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall CreateVDirMapGuide(MSIHANDLE hMSI)
{
	CComPtr <IMSAdminBase> pIMeta;
	HRESULT hRes = S_OK;
	UINT er = ERROR_SUCCESS;

	hRes = WcaInitialize(hMSI, "CreateVDirMapGuide");
	ExitOnFailure(hRes, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.
	CHAR szStatus[STATUS_STRING_SIZE];
	_TCHAR installDir[MAX_BUFFER] = _T("");
	DWORD  installDirLen = MAX_BUFFER;
	CHAR   szInstallDir[MAX_BUFFER];
	CHAR   mapAgentDir[MAX_BUFFER];
	METADATA_HANDLE MyHandle; 
	METADATA_RECORD MyRecord; 
	WCHAR SubKeyName[METADATA_MAX_NAME_LEN];
	WCHAR TestKey[METADATA_MAX_NAME_LEN];

	DWORD  ServerInstance = 1;
	char  *VirtualPath = "/mapguide";
	char  *AppName = "mapguide";
	int nLen;

	MsiGetProperty(hMSI, MSI_PROP_INSTALLDIR, installDir, &installDirLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, installDir, installDirLen, szInstallDir, MAX_BUFFER, NULL, NULL);
	if ((nLen>0) && (szInstallDir[nLen-1] != '\\'))
		szInstallDir[nLen++] = '\\';
	szInstallDir[nLen] = '\0';

	strcpy(mapAgentDir, szInstallDir);
	strcat(mapAgentDir, "www");

	DeleteVirtualRoot("mapguide", 1, TRUE, szStatus);

	CreateVirtualRoot("mapguide", mapAgentDir, 517, 1, TRUE, szStatus);

	CoInitialize(NULL);
	hRes = CoCreateInstance( CLSID_MSAdminBase, NULL, CLSCTX_ALL,
		IID_IMSAdminBase, (void **) &pIMeta );

	if (FAILED(hRes))
	{
		CoUninitialize();
		return 0;
	}

	hRes = pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE,
		L"/LM/W3SVC",
		METADATA_PERMISSION_WRITE | METADATA_PERMISSION_READ,
		20,
		&MyHandle ); 

	if (SUCCEEDED(hRes))
	{
		memset(SubKeyName , 0, sizeof(SubKeyName));
		swprintf(SubKeyName,L"/%ld/ROOT%S",ServerInstance,VirtualPath);

		memset(TestKey , 0, sizeof(TestKey));
		swprintf(TestKey, L"%S", AppName);
		MyRecord.dwMDIdentifier = MD_APP_FRIENDLY_NAME;
		MyRecord.dwMDAttributes = METADATA_INHERIT;
		MyRecord.dwMDUserType = IIS_MD_UT_WAM;
		MyRecord.dwMDDataType = STRING_METADATA;
		MyRecord.pbMDData = (PBYTE)&TestKey;
		MyRecord.dwMDDataLen = (wcslen(TestKey) + 1) * sizeof(WCHAR);

		hRes = pIMeta->SetData(MyHandle, SubKeyName, &MyRecord);
	}

	pIMeta->CloseKey(MyHandle);

LExit:
	er = SUCCEEDED(hRes) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall CreateVDirPhpAgent(MSIHANDLE hMSI)
{
	CComPtr <IMSAdminBase> pIMeta;
	HRESULT hRes = S_OK;
	UINT er = ERROR_SUCCESS;

	hRes = WcaInitialize(hMSI, "CreateVDirPhpAgent");
	ExitOnFailure(hRes, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.
	DWORD indx = 0;
	DWORD buffer = 0; 
	WCHAR szValue[1024];
	WCHAR szValue_new[1024];
	METADATA_HANDLE MyHandle; 
	METADATA_RECORD MyRecord; 
	WCHAR SubKeyName[METADATA_MAX_NAME_LEN];

	DWORD ServerInstance = 1;

	CHAR szStatus[STATUS_STRING_SIZE];
	_TCHAR installDir[MAX_BUFFER] = _T("");
	DWORD  installDirLen = MAX_BUFFER;
	CHAR   szInstallDir[MAX_BUFFER];
	CHAR   phpMapAgentDir[MAX_BUFFER];
	int nLen;

	MsiGetProperty(hMSI, MSI_PROP_INSTALLDIR, installDir, &installDirLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, installDir, installDirLen, szInstallDir, MAX_BUFFER, NULL, NULL);
	if ((nLen>0) && (szInstallDir[nLen-1] != '\\'))
		szInstallDir[nLen++] = '\\';
	szInstallDir[nLen] = '\0';

	strcpy(phpMapAgentDir, szInstallDir);
	strcat(phpMapAgentDir, "www\\mapviewerphp");

	DeleteVirtualRoot("mapguide/mapviewerajax", 1, FALSE, szStatus);
	DeleteVirtualRoot("mapguide/mapviewerdwf", 1, FALSE, szStatus);

	CreateVirtualRoot("mapguide/mapviewerajax", phpMapAgentDir, 513, 1, FALSE, szStatus);
	CreateVirtualRoot("mapguide/mapviewerdwf", phpMapAgentDir, 513, 1, FALSE, szStatus);

	CoInitialize(NULL);
	hRes = CoCreateInstance( CLSID_MSAdminBase, 
		NULL,
		CLSCTX_ALL,
		IID_IMSAdminBase,
		(void **) &pIMeta );

	if (FAILED(hRes))
	{
		return 0;
	}

	hRes = pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE,
		L"/LM/W3SVC",
		METADATA_PERMISSION_WRITE | METADATA_PERMISSION_READ,
		20,
		&MyHandle ); 
	if (SUCCEEDED(hRes)) 
	{
		memset(SubKeyName , 0, sizeof(SubKeyName));
		swprintf(SubKeyName,L"/%ld/ROOT%S",ServerInstance,"/mapguide/mapviewerajax");

		MyRecord.dwMDIdentifier = MD_DEFAULT_LOAD_FILE;
		MyRecord.dwMDAttributes = METADATA_INHERIT;
		MyRecord.dwMDUserType = IIS_MD_UT_WAM;
		MyRecord.dwMDDataType = STRING_METADATA;
		MyRecord.pbMDData = (LPBYTE) szValue;
		MyRecord.dwMDDataLen = 1024;

		hRes = pIMeta->GetData(MyHandle, SubKeyName, &MyRecord, &buffer);

		wcscpy(szValue_new,L"ajaxviewer.php,");
		wcscat(szValue_new,szValue);

		MyRecord.pbMDData = (LPBYTE) szValue_new;
		MyRecord.dwMDDataLen = (wcslen(szValue_new)+1)*sizeof(WCHAR);

		hRes = pIMeta->SetData(MyHandle, SubKeyName, &MyRecord);

		memset(SubKeyName , 0, sizeof(SubKeyName));
		swprintf(SubKeyName,L"/%ld/ROOT%S",ServerInstance,"/mapguide/mapviewerdwf");

		MyRecord.dwMDIdentifier = MD_DEFAULT_LOAD_FILE;
		MyRecord.dwMDAttributes = METADATA_INHERIT;
		MyRecord.dwMDUserType = IIS_MD_UT_WAM;
		MyRecord.dwMDDataType = STRING_METADATA;
		MyRecord.pbMDData = (LPBYTE) szValue;
		MyRecord.dwMDDataLen = 1024;

		hRes = pIMeta->GetData(MyHandle, SubKeyName, &MyRecord, &buffer);

		wcscpy(szValue_new,L"dwfviewer.php,");
		wcscat(szValue_new,szValue);

		MyRecord.pbMDData = (LPBYTE) szValue_new;
		MyRecord.dwMDDataLen = (wcslen(szValue_new)+1)*sizeof(WCHAR);

		hRes = pIMeta->SetData(MyHandle, SubKeyName, &MyRecord);
	}

LExit:
	er = SUCCEEDED(hRes) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall CreateVDirNetAgent(MSIHANDLE hMSI)
{
	CComPtr <IMSAdminBase> pIMeta;
	HRESULT hRes = S_OK;
	UINT er = ERROR_SUCCESS;

	hRes = WcaInitialize(hMSI, "CreateVDirNetAgent");
	ExitOnFailure(hRes, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.
	DWORD indx = 0;
	DWORD buffer = 0; 
	WCHAR szValue[1024];
	WCHAR szValue_new[1024];
	METADATA_HANDLE MyHandle; 
	METADATA_RECORD MyRecord; 
	WCHAR SubKeyName[METADATA_MAX_NAME_LEN];

	DWORD ServerInstance = 1;

	CHAR szStatus[STATUS_STRING_SIZE];
	_TCHAR installDir[MAX_BUFFER] = _T("");
	DWORD  installDirLen = MAX_BUFFER;
	CHAR   szInstallDir[MAX_BUFFER];
	CHAR   netMapAgentDir[MAX_BUFFER];
	int nLen;

	MsiGetProperty(hMSI, MSI_PROP_INSTALLDIR, installDir, &installDirLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, installDir, installDirLen, szInstallDir, MAX_BUFFER, NULL, NULL);
	if ((nLen>0) && (szInstallDir[nLen-1] != '\\'))
		szInstallDir[nLen++] = '\\';
	szInstallDir[nLen] = '\0';

	strcpy(netMapAgentDir, szInstallDir);
	strcat(netMapAgentDir, "www\\mapviewernet");

	DeleteVirtualRoot("mapguide/mapviewerajax", 1, TRUE, szStatus);
	DeleteVirtualRoot("mapguide/mapviewerdwf", 1, TRUE, szStatus);
	DeleteVirtualRoot("mapguide/mapviewernet", 1, TRUE, szStatus);

	CreateVirtualRoot("mapguide/mapviewerajax", netMapAgentDir, 513, 1, TRUE, szStatus);
	CreateVirtualRoot("mapguide/mapviewerdwf", netMapAgentDir, 513, 1, TRUE, szStatus);
	CreateVirtualRoot("mapguide/mapviewernet", netMapAgentDir, 513, 1, TRUE, szStatus);

	CoInitialize(NULL);
	hRes = CoCreateInstance( CLSID_MSAdminBase, 
		NULL,
		CLSCTX_ALL,
		IID_IMSAdminBase,
		(void **) &pIMeta );

	if (FAILED(hRes))
	{
		return 0;
	}

	hRes = pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE,
		L"/LM/W3SVC",
		METADATA_PERMISSION_WRITE | METADATA_PERMISSION_READ,
		20,
		&MyHandle ); 
	if (SUCCEEDED(hRes)) 
	{
		memset(SubKeyName , 0, sizeof(SubKeyName));
		swprintf(SubKeyName,L"/%ld/ROOT%S",ServerInstance,"/mapguide/mapviewerajax");

		MyRecord.dwMDIdentifier = MD_DEFAULT_LOAD_FILE;
		MyRecord.dwMDAttributes = METADATA_INHERIT;
		MyRecord.dwMDUserType = IIS_MD_UT_WAM;
		MyRecord.dwMDDataType = STRING_METADATA;
		MyRecord.pbMDData = (LPBYTE) szValue;
		MyRecord.dwMDDataLen = 1024;

		hRes = pIMeta->GetData(MyHandle, SubKeyName, &MyRecord, &buffer);

		wcscpy(szValue_new,L"ajaxviewer.aspx,");
		wcscat(szValue_new,szValue);

		MyRecord.pbMDData = (LPBYTE) szValue_new;
		MyRecord.dwMDDataLen = (wcslen(szValue_new)+1)*sizeof(WCHAR);

		hRes = pIMeta->SetData(MyHandle, SubKeyName, &MyRecord);

		memset(SubKeyName , 0, sizeof(SubKeyName));
		swprintf(SubKeyName,L"/%ld/ROOT%S",ServerInstance,"/mapguide/mapviewerdwf");

		MyRecord.dwMDIdentifier = MD_DEFAULT_LOAD_FILE;
		MyRecord.dwMDAttributes = METADATA_INHERIT;
		MyRecord.dwMDUserType = IIS_MD_UT_WAM;
		MyRecord.dwMDDataType = STRING_METADATA;
		MyRecord.pbMDData = (LPBYTE) szValue;
		MyRecord.dwMDDataLen = 1024;

		hRes = pIMeta->GetData(MyHandle, SubKeyName, &MyRecord, &buffer);

		wcscpy(szValue_new,L"dwfviewer.aspx,");
		wcscat(szValue_new,szValue);

		MyRecord.pbMDData = (LPBYTE) szValue_new;
		MyRecord.dwMDDataLen = (wcslen(szValue_new)+1)*sizeof(WCHAR);

		hRes = pIMeta->SetData(MyHandle, SubKeyName, &MyRecord);
	}


LExit:
	er = SUCCEEDED(hRes) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall CreateVDirJavaAgent(MSIHANDLE hMSI)
{
	CComPtr <IMSAdminBase> pIMeta;
	HRESULT hRes = S_OK;
	UINT er = ERROR_SUCCESS;

	hRes = WcaInitialize(hMSI, "CreateVDirJavaAgent");
	ExitOnFailure(hRes, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.
	DWORD indx = 0;
	DWORD buffer = 0; 
	WCHAR szValue[1024];
	WCHAR szValue_new[1024];
	METADATA_HANDLE MyHandle; 
	METADATA_RECORD MyRecord; 
	WCHAR SubKeyName[METADATA_MAX_NAME_LEN];

	DWORD ServerInstance = 1;

	CHAR szStatus[STATUS_STRING_SIZE];
	_TCHAR installDir[MAX_BUFFER] = _T("");
	DWORD  installDirLen = MAX_BUFFER;
	CHAR   szInstallDir[MAX_BUFFER];
	CHAR   javaMapAgentDir[MAX_BUFFER];
	int nLen;

	MsiGetProperty(hMSI, MSI_PROP_INSTALLDIR, installDir, &installDirLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, installDir, installDirLen, szInstallDir, MAX_BUFFER, NULL, NULL);
	if ((nLen>0) && (szInstallDir[nLen-1] != '\\'))
		szInstallDir[nLen++] = '\\';
	szInstallDir[nLen] = '\0';

	strcpy(javaMapAgentDir, szInstallDir);
	strcat(javaMapAgentDir, "www\\mapviewerjava");

	DeleteVirtualRoot("mapguide/mapviewerajax", 1, FALSE, szStatus);
	DeleteVirtualRoot("mapguide/mapviewerdwf", 1, FALSE, szStatus);

	CreateVirtualRoot("mapguide/mapviewerajax", javaMapAgentDir, 513, 1, FALSE, szStatus);
	CreateVirtualRoot("mapguide/mapviewerdwf", javaMapAgentDir, 513, 1, FALSE, szStatus);

	CoInitialize(NULL);
	hRes = CoCreateInstance( CLSID_MSAdminBase, 
		NULL,
		CLSCTX_ALL,
		IID_IMSAdminBase,
		(void **) &pIMeta );

	if (FAILED(hRes))
	{
		return 0;
	}

	hRes = pIMeta->OpenKey( METADATA_MASTER_ROOT_HANDLE,
		L"/LM/W3SVC",
		METADATA_PERMISSION_WRITE | METADATA_PERMISSION_READ,
		20,
		&MyHandle ); 
	if (SUCCEEDED(hRes)) 
	{
		memset(SubKeyName , 0, sizeof(SubKeyName));
		swprintf(SubKeyName,L"/%ld/ROOT%S",ServerInstance,"/mapguide/mapviewerajax");

		MyRecord.dwMDIdentifier = MD_DEFAULT_LOAD_FILE;
		MyRecord.dwMDAttributes = METADATA_INHERIT;
		MyRecord.dwMDUserType = IIS_MD_UT_WAM;
		MyRecord.dwMDDataType = STRING_METADATA;
		MyRecord.pbMDData = (LPBYTE) szValue;
		MyRecord.dwMDDataLen = 1024;

		hRes = pIMeta->GetData(MyHandle, SubKeyName, &MyRecord, &buffer);

		wcscpy(szValue_new,L"ajaxviewer.jsp,");
		wcscat(szValue_new,szValue);

		MyRecord.pbMDData = (LPBYTE) szValue_new;
		MyRecord.dwMDDataLen = (wcslen(szValue_new)+1)*sizeof(WCHAR);

		hRes = pIMeta->SetData(MyHandle, SubKeyName, &MyRecord);

		memset(SubKeyName , 0, sizeof(SubKeyName));
		swprintf(SubKeyName,L"/%ld/ROOT%S",ServerInstance,"/mapguide/mapviewerdwf");

		MyRecord.dwMDIdentifier = MD_DEFAULT_LOAD_FILE;
		MyRecord.dwMDAttributes = METADATA_INHERIT;
		MyRecord.dwMDUserType = IIS_MD_UT_WAM;
		MyRecord.dwMDDataType = STRING_METADATA;
		MyRecord.pbMDData = (LPBYTE) szValue;
		MyRecord.dwMDDataLen = 1024;

		hRes = pIMeta->GetData(MyHandle, SubKeyName, &MyRecord, &buffer);

		wcscpy(szValue_new,L"dwfviewer.jsp,");
		wcscat(szValue_new,szValue);

		MyRecord.pbMDData = (LPBYTE) szValue_new;
		MyRecord.dwMDDataLen = (wcslen(szValue_new)+1)*sizeof(WCHAR);

		hRes = pIMeta->SetData(MyHandle, SubKeyName, &MyRecord);
	}


LExit:
	er = SUCCEEDED(hRes) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

UINT __stdcall DeleteVDirMapGuide(MSIHANDLE hInstall)
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;

	hr = WcaInitialize(hInstall, "DeleteVDirMapGuide");
	ExitOnFailure(hr, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	// TODO: Add your custom action code here.
	CHAR szStatus[STATUS_STRING_SIZE];

	DeleteVirtualRoot("mapguide/mapviewerajax", 1, FALSE, szStatus);
	DeleteVirtualRoot("mapguide/mapviewerdwf", 1, FALSE, szStatus);
	DeleteVirtualRoot("mapguide/mapagent", 1, FALSE, szStatus);
	DeleteVirtualRoot("mapguide", 1, FALSE, szStatus);

LExit:
	er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

/*****************************************************************************
* Name        : CreateVirtualRoot
*
* Description : Creates the specified virtual root 
*
* Parameters  : szName - Name of the virtual root to add
*               szPhysicalPath - Physical path of the virtual root
*               dwPermissions - Access permissions for the virtual root
*               dwSite - The site to which the virtual root is to be added
*               szStatus - The function can report error descriptions in this string
*
* Return      : Returns TRUE if successfull; otherwise FALSE.
*
* Notes       : N/A
*
*****************************************************************************/

BOOL CreateVirtualRoot(
					   LPSTR szName,
					   LPSTR szPhysicalPath,
					   DWORD dwPermissions,
					   DWORD dwSite,
					   BOOL  bCreateApp,
					   CHAR szStatus[STATUS_STRING_SIZE]
)
{
	HRESULT hres = S_OK;
	CHAR szMetaPath[MAX_PATH];
	WCHAR NewType[METADATA_MAX_NAME_LEN];
	LPWSTR szwMetaPath = NULL;
	BOOL bResult;
	DWORD dwSecurity = MD_AUTH_ANONYMOUS;

	// initialize COM
	hres = CoInitialize(NULL);

	if (FAILED(hres))
	{
		sprintf(szStatus, "CoInitializeEx failed: %d (0x%08x)", hres, hres);
		printf("%s\n", szStatus);
		return 0;
	}

	// get a pointer to the IIS Admin Base Object
	hres = CoCreateInstance(CLSID_MSAdminBase, NULL, CLSCTX_ALL, 
		IID_IMSAdminBase, (void **) &g_spAdminBase);  

	if (FAILED(hres))  
	{
		sprintf(szStatus, "CoCreateInstance failed for CLSID_MSAdminBase: %d (0x%08x)", hres, hres);
		printf("%s\n", szStatus);
		CoUninitialize();
		return 0;  
	}

	// get a pointer to the IWamAdmin Object
	hres = CoCreateInstance(CLSID_WamAdmin, NULL, CLSCTX_ALL, 
		IID_IWamAdmin, (void **) &g_spWamAdmin);  

	if (FAILED(hres))  
	{
		sprintf(szStatus, "CoCreateInstance failed for CLSID_WamAdmin: %d (0x%08x)", hres, hres);
		g_spAdminBase.Release();
		CoUninitialize();
		return 0;  
	}

	// Create the metabase path

	sprintf(szMetaPath, "/LM/W3SVC/%d/ROOT/%s", dwSite, szName);

	// Create a new key for the virtual directory

	bResult = WrAddKey((LPSTR)szMetaPath);

	if (!bResult)
	{
		sprintf(
			szStatus,
			"CreateVirtualRoot: Error %d (0x%08x) creating key for virtual root",
			GetLastError(),
			GetLastError()
			);

		goto Failed;
	}

	// Set the key type for the virtual directory

	bResult = WrSetData(
		(LPSTR)szMetaPath,  // metabase path
		MD_KEY_TYPE,        // identifier
		METADATA_INHERIT,   // attributes
		IIS_MD_UT_FILE,     // user type
		STRING_METADATA,    // data type
		0,                  // data size (not used for STRING_METADATA)
		"IIsWebVirtualDir"  // data
		);

	if (!bResult)
	{
		sprintf(szStatus, "CreateVirtualRoot: Error %d (0x%08x)setting key type for virtual root",
			GetLastError(), GetLastError());

		goto Failed;
	}

	// Set the json mime type for the virtual directory 

	memset(NewType, 0, sizeof(NewType));
	swprintf(NewType,L"%S",".json,application/json");

	bResult = WrSetData(
		(LPSTR)szMetaPath,  // metabase path
		MD_MIME_MAP,        // identifier
		METADATA_INHERIT,   // attributes
		IIS_MD_UT_FILE,     // user type
		MULTISZ_METADATA,   // data type
		(wcslen(NewType) + 1) * sizeof(WCHAR) + 1 * sizeof(WCHAR),   // data size
		NewType             // data
		);

	if (!bResult)
	{
		sprintf(szStatus, "CreateVirtualRoot: Error %d (0x%08x)setting mime type for virtual root",
			GetLastError(), GetLastError());

		goto Failed;
	}

	// Set the VRPath for the virtual directory

	bResult = WrSetData(
		(LPSTR)szMetaPath,  // metabase path
		MD_VR_PATH,         // identifier
		METADATA_INHERIT,   // attributes
		IIS_MD_UT_FILE,     // user type
		STRING_METADATA,    // data type
		0,                  // data size (not used for STRING_METADATA)
		szPhysicalPath      // data
		);

	if (!bResult)
	{
		sprintf(szStatus, "CreateVirtualRoot: Error %d (0x%08x) setting vrpath for virtual root",
			GetLastError(), GetLastError());

		goto Failed;
	}

	// Set the permissions for the virtual directory

	bResult = WrSetData(
		(LPSTR)szMetaPath,  // metabase path
		MD_ACCESS_PERM,     // identifier
		METADATA_INHERIT,   // attributes
		IIS_MD_UT_FILE,     // user type
		DWORD_METADATA,     // data type
		0,                  // data size (not used for DWORD_METADATA)
		&dwPermissions      // data
		);

	if (!bResult)
	{
		sprintf(szStatus, "CreateVirtualRoot: Error %d (0x%08x) setting permissions for virtual root",
			GetLastError(), GetLastError());

		goto Failed;
	}

	// Set the directory security for the virtual directory

	bResult = WrSetData(
		(LPSTR)szMetaPath,  // metabase path
		MD_AUTHORIZATION,   // identifier
		METADATA_INHERIT,   // attributes
		IIS_MD_UT_FILE,     // user type
		DWORD_METADATA,     // data type
		0,                  // data size (not used for DWORD_METADATA)
		&dwSecurity         // data
		);

	if (!bResult)
	{
		sprintf(szStatus, "CreateVirtualRoot: Error %d (0x%08x) setting directory security for virtual root",
			GetLastError(), GetLastError());

		goto Failed;
	}

	if ( bCreateApp ) 
	{

		szwMetaPath = MakeUnicode(szMetaPath);

		if (!szwMetaPath)
		{
			sprintf(
				szStatus,
				"CreateApplication failed: %d",
				GetLastError()
				);

			goto Failed;
		}

		// Create the Application

		hres = g_spWamAdmin->AppCreate(
			szwMetaPath,
			FALSE
			);

		if (FAILED(hres))
		{
			SetLastError(hres);

			sprintf(
				szStatus,
				"CreateApplication: IWamAdmin->AppCreate failed: %d (0x%08x)",
				GetLastError(),
				GetLastError()
				);

			LocalFree(szwMetaPath);

			goto Failed;
		}

		LocalFree(szwMetaPath);
	}

	// Commit the changes and return

	g_spAdminBase->SaveData();

	// this needs to be released before we uninitialize COM

	g_spAdminBase.Release();
	g_spWamAdmin.Release();

	CoUninitialize();

	sprintf(szStatus, "CreateVirtualRoot completed successfully.");

	return TRUE;

Failed:

	// this needs to be released before we uninitialize COM

	g_spAdminBase.Release();
	g_spWamAdmin.Release();

	CoUninitialize();

	sprintf(szStatus, "CreateVirtualRoot failed.");

	return FALSE;
}

/*****************************************************************************
* Name        : DeleteVirtualRoot
*
* Description : Deletes the specified virtual root 
*
* Parameters  : szName - Name of the virtual root to be deleted
*               dwSite - The site from which the virtual root will be deleted
*               szStatus - The function can report error descriptions in this string
*
* Return      : Returns TRUE if successfull; otherwise FALSE.
*
* Notes       : N/A
*
*****************************************************************************/

BOOL DeleteVirtualRoot(
					   LPSTR szName,
					   DWORD dwSite,
					   BOOL  bDeleteApp,
					   CHAR szStatus[STATUS_STRING_SIZE]
)
{
	CHAR szMetaPath[MAX_PATH];
	CHAR szParent[MAX_PATH];
	CHAR szVDir[MAX_PATH];
	LPSTR szPtr;
	LPWSTR szwParent = NULL;
	LPWSTR szwVDir = NULL;
	LPWSTR szwMetaPath = NULL;
	METADATA_HANDLE hMetaData;
	BOOL fMetaData = FALSE;
	HRESULT hres;
	DWORD dwLastError;

	sprintf(szMetaPath, "/LM/W3SVC/%d/ROOT/%s", dwSite, szName);

	strcpy(szParent, szMetaPath);

	szPtr = strrchr(szParent, '/');

	strcpy(szVDir, szPtr + 1);

	*szPtr = 0;

	szwParent = MakeUnicode(szParent);
	szwVDir = MakeUnicode(szVDir);

	if (!szwParent || !szwVDir)
	{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);

		sprintf(
			szStatus,
			"DeleteVirtualDirectory failed: %d",
			GetLastError()
			);

		goto Failed;
	}

	// initialize COM
	hres = CoInitialize(NULL);

	if (FAILED(hres))
	{
		sprintf(szStatus, "CoInitializeEx failed: %d (0x%08x)", hres, hres);
		printf("%s\n", szStatus);
		return 0;
	}

	// get a pointer to the IIS Admin Base Object
	hres = CoCreateInstance(CLSID_MSAdminBase, NULL, CLSCTX_ALL, 
		IID_IMSAdminBase, (void **) &g_spAdminBase);  

	if (FAILED(hres))  
	{
		sprintf(szStatus, "CoCreateInstance failed for CLSID_MSAdminBase: %d (0x%08x)", hres, hres);
		printf("%s\n", szStatus);
		CoUninitialize();
		return 0;  
	}

	// get a pointer to the IWamAdmin Object
	hres = CoCreateInstance(CLSID_WamAdmin, NULL, CLSCTX_ALL, 
		IID_IWamAdmin, (void **) &g_spWamAdmin);  

	if (FAILED(hres))  
	{
		sprintf(szStatus, "CoCreateInstance failed for CLSID_WamAdmin: %d (0x%08x)", hres, hres);
		g_spAdminBase.Release();
		CoUninitialize();
		return 0;  
	}

	// Get a handle to the metabase

	hres = g_spAdminBase->OpenKey(
		METADATA_MASTER_ROOT_HANDLE,
		szwParent,
		METADATA_PERMISSION_READ|METADATA_PERMISSION_WRITE,
		20,
		&hMetaData
		);

	if (FAILED(hres))
	{
		SetLastError(hres);

		sprintf(
			szStatus,
			"DeleteVirtualRoot: Error %d (0x%08x) getting handle to metabase",
			hres,
			hres
			);

		goto Failed;
	}
	else
		fMetaData = TRUE;

	// Do the work

	hres = g_spAdminBase->DeleteKey(
		hMetaData,
		szwVDir
		);

	if (FAILED(hres))
	{
		SetLastError(hres);

		sprintf(
			szStatus,
			"DeleteVirtualRoot: IMSAdminBase->DeleteKey returned %d (0x%08x)",
			hres,
			hres
			);

		goto Failed;
	}

	if ( bDeleteApp )
	{
		szwMetaPath = MakeUnicode(szMetaPath);

		hres = g_spWamAdmin->AppDelete(szwMetaPath, FALSE);

		if (FAILED(hres))
		{
			SetLastError(hres);

			sprintf(
				szStatus,
				"DeleteApplication: IWamAdmin->AppDelete failed: %d (0x%08x)",
				hres,
				hres
				);

			LocalFree(szwMetaPath);

			goto Failed;
		}

		LocalFree(szwMetaPath);
	}

	// Commit the changes

	g_spAdminBase->SaveData();

	// Clean up and return

	g_spAdminBase->CloseKey(hMetaData);

	// this needs to be released before we uninitialize COM
	g_spAdminBase.Release();

	CoUninitialize();

	LocalFree(szwParent);
	LocalFree(szwVDir);

	sprintf(szStatus, "DeleteVirtualRoot completed successfully.");

	return TRUE;

Failed:

	dwLastError = GetLastError();

	if (fMetaData)
		g_spAdminBase->CloseKey(hMetaData);

	// this needs to be released before we uninitialize COM
	g_spAdminBase.Release();

	CoUninitialize();

	LocalFree(szwParent);
	LocalFree(szwVDir);

	SetLastError(dwLastError);

	sprintf(szStatus, "DeleteVirtualRoot failed.");

	return FALSE;
}

/*****************************************************************************
* Name        : MakeUnicode
*
* Description : Returns a unicode version of the provided string 
*
* Parameters  : szString - The string to be translated
*
* Return      : Returns a pointer to the unicode string if successful, NULL if not
*
* Notes       : N/A
*
*****************************************************************************/

LPWSTR MakeUnicode(
				   LPSTR szString
				   )
{
	LPWSTR szwRet;
	DWORD dwNumChars;

	// Allocate buffer for the returned wide string

	dwNumChars = MultiByteToWideChar(
		CP_ACP,         // code page
		MB_PRECOMPOSED, // flags
		szString,       // ANSI source string
		-1,             // source string length
		NULL,           // destination buffer
		0               // size of destination buffer in chars
		);

	szwRet = (LPWSTR)LocalAlloc(LPTR, dwNumChars * sizeof(WCHAR));

	if (!szwRet)
		return NULL;

	// Do the conversion

	MultiByteToWideChar(
		CP_ACP,         // code page
		MB_PRECOMPOSED, // flags
		szString,       // ANSI source string
		-1,             // source string length
		szwRet,         // destination buffer
		dwNumChars      // size of destination buffer in chars
		);

	return szwRet;
}

/*****************************************************************************
* Name        : WrAddKey
*
* Description : Creates the specified metabase path 
*
* Parameters  : szMetaPath - The metabase path to be created
*
* Return      : Returns TRUE if successfull; otherwise FALSE.
*
* Notes       : N/A
*
*****************************************************************************/

BOOL WrAddKey(
			  LPSTR szMetaPath
			  )
{
	CHAR szParent[MAX_PATH];
	CHAR szDir[MAX_PATH];
	LPWSTR szwPath = NULL;
	LPWSTR szwNew = NULL;
	LPSTR szPtr;
	METADATA_HANDLE hMetaData;
	BOOL fMetaData = FALSE;
	HRESULT hres;
	DWORD dwLastError;

	// Parse the supplied metabase path into parent and new directory

	strcpy(szParent, szMetaPath);

	szPtr = strrchr(szParent, '/');

	if (!szPtr)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		goto Failed;
	}

	*szPtr = 0;

	strcpy(szDir, szPtr + 1);

	// Open the metabase

	szwPath = MakeUnicode(szParent);
	szwNew = MakeUnicode(szDir);

	if (!szwPath || !szwNew)
	{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		goto Failed;
	}

	hres = g_spAdminBase->OpenKey(
		METADATA_MASTER_ROOT_HANDLE,
		szwPath,
		METADATA_PERMISSION_READ|METADATA_PERMISSION_WRITE,
		20,
		&hMetaData
		);

	if (FAILED(hres))
	{
		SetLastError(hres);
		goto Failed;
	}
	else
		fMetaData = TRUE;

	// Create the new key

	hres = g_spAdminBase->AddKey(
		hMetaData,
		szwNew
		);

	if (FAILED(hres))
	{
		SetLastError(hres);
		goto Failed;
	}

	// Clean up

	hres = g_spAdminBase->CloseKey(hMetaData);

	fMetaData = FALSE;

	LocalFree(szwNew);
	LocalFree(szwPath);

	return TRUE;

Failed:

	dwLastError = GetLastError();

	if (fMetaData)
		g_spAdminBase->CloseKey(hMetaData);

	LocalFree(szwNew);
	LocalFree(szwPath);


	SetLastError(dwLastError);

	return FALSE;
}

/*****************************************************************************
* Name        : WrSetData
*
* Description : Sets the specified data in the metabase 
*
* Parameters  : szMetaPath - The metabase path to be created
*               dwIdentifier - The metabase data identifier
*               dwAttributes - The data attributes
*               dwUserType - The metabase user type
*               dwDataType - The type of data being set
*               dwDataSize - The size of the data being set
*               pData - The actual data
*
* Return      : Returns TRUE if successfull; otherwise FALSE.
*
* Notes       : N/A
*
*****************************************************************************/

BOOL WrSetData(
			   LPSTR szMetaPath,
			   DWORD dwIdentifier,
			   DWORD dwAttributes,
			   DWORD dwUserType,
			   DWORD dwDataType,
			   DWORD dwDataSize,
			   LPVOID pData
			   )
{
	LPWSTR szwPath = NULL;
	LPWSTR szwValue = NULL;
	METADATA_RECORD mdRecord;
	METADATA_HANDLE hMetaData;
	BOOL fMetaData = FALSE;
	HRESULT hres;
	DWORD dwLastError;


	// Get a handle to the metabase location specified

	szwPath = MakeUnicode(szMetaPath);

	if (!szwPath)
		goto Failed;

	hres = g_spAdminBase->OpenKey(
		METADATA_MASTER_ROOT_HANDLE,
		szwPath,
		METADATA_PERMISSION_READ|METADATA_PERMISSION_WRITE,
		20,
		&hMetaData
		);

	if (FAILED(hres))
	{
		SetLastError(hres);
		goto Failed;
	}
	else
		fMetaData = TRUE;

	//
	// Populate the metadata record
	//

	mdRecord.dwMDIdentifier = dwIdentifier;
	mdRecord.dwMDAttributes = dwAttributes;
	mdRecord.dwMDUserType = dwUserType;
	mdRecord.dwMDDataType = dwDataType;
	mdRecord.dwMDDataTag = 0;

	switch (mdRecord.dwMDDataType)
	{
	case ALL_METADATA:
		SetLastError(ERROR_NOT_SUPPORTED);
		goto Failed;

	case BINARY_METADATA:
		mdRecord.dwMDDataLen = dwDataSize;
		mdRecord.pbMDData = (PBYTE)pData;
		break;

	case DWORD_METADATA:
		mdRecord.dwMDDataLen = sizeof(DWORD);
		mdRecord.pbMDData = (PBYTE)pData;
		break;

	case EXPANDSZ_METADATA:
	case STRING_METADATA:
		szwValue = MakeUnicode((LPSTR)pData);

		if (!szwValue)
			goto Failed;

		mdRecord.dwMDDataLen = sizeof(WCHAR) * (wcslen(szwValue) + 1);
		mdRecord.pbMDData = (PBYTE)szwValue;
		break;

	case MULTISZ_METADATA:
		mdRecord.dwMDDataLen = dwDataSize;
		mdRecord.pbMDData = (PBYTE)pData;
		break;

	default:
		SetLastError(ERROR_INVALID_PARAMETER);
		goto Failed;
	}

	//
	// Do the work
	//

	hres = g_spAdminBase->SetData(
		hMetaData,
		L"/",
		&mdRecord
		);

	if (FAILED(hres))
	{
		SetLastError(hres);
		goto Failed;
	}

	//
	// Close the metabase
	//

	g_spAdminBase->CloseKey(hMetaData);
	fMetaData = FALSE;

	//
	// Clean up
	//

	LocalFree(szwPath);
	LocalFree(szwValue);

	return TRUE;

Failed:

	dwLastError = GetLastError();

	if (fMetaData)
		g_spAdminBase->CloseKey(hMetaData);

	LocalFree(szwPath);
	LocalFree(szwValue);

	SetLastError(dwLastError);

	return FALSE;
}

// DllMain - Initialize and cleanup WiX custom action utils.
extern "C" BOOL WINAPI DllMain(
							   __in HINSTANCE hInst,
							   __in ULONG ulReason,
							   __in LPVOID
							   )
{
	switch(ulReason)
	{
	case DLL_PROCESS_ATTACH:
		WcaGlobalInitialize(hInst);
		break;

	case DLL_PROCESS_DETACH:
		WcaGlobalFinalize();
		break;
	}

	return TRUE;
}
