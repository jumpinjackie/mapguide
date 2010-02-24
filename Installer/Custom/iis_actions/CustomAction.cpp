
#include "stdafx.h"


UINT __stdcall PopulateWebSites(MSIHANDLE hInstall)
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;
	PMSIHANDLE hTable;
	PMSIHANDLE hColumns;

	METADATA_HANDLE hMeta;
	METADATA_RECORD mRec;
	MSIHANDLE hDb = NULL;
	PMSIHANDLE hView;
	PMSIHANDLE hRecord;
	TCHAR currKey[METADATA_MAX_NAME_LEN];
	WCHAR subKeyName[METADATA_MAX_NAME_LEN];
	DWORD dwReqBufLen = 0;
	DWORD dwBufLen = 1024;
	char szMsg[256];
	PBYTE pbBuffer = NULL;

	CComPtr<IMSAdminBase> pIMeta;

	hr = WcaInitialize(hInstall, "PopulateWebSites");
	ExitOnFailure(hr, "Failed to initialize");
	hr = CoInitialize(NULL);
	ExitOnFailure(hr, "Could not initialize the COM sub-system");
	hr = CoCreateInstance(CLSID_MSAdminBase, NULL, CLSCTX_ALL, IID_IMSAdminBase, (void**)&pIMeta);
	ExitOnFailure(hr, "Could obtain an IMSAdminBase pointer");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	pbBuffer = new BYTE[dwBufLen];
	
	hDb = MsiGetActiveDatabase(hInstall);
	MsiDatabaseOpenView(hDb, TEXT("SELECT * FROM `ListBox` WHERE Property = `MG_WEBSITE`"), &hView);

	hr = pIMeta->OpenKey(METADATA_MASTER_ROOT_HANDLE, TEXT("/LM"), METADATA_PERMISSION_READ, 20, &hMeta);
	ExitOnFailure(hr, "Could not open the IIS metabase");

	DWORD indx = 0;
	DWORD widx = 0;
	while (SUCCEEDED(hr))
	{
		hr = pIMeta->EnumKeys(hMeta, TEXT("/W3SVC"), subKeyName, indx);
		if (SUCCEEDED(hr))
		{
			mRec.dwMDAttributes = METADATA_NO_ATTRIBUTES;
			mRec.dwMDDataLen = dwBufLen;
			mRec.dwMDDataType = ALL_METADATA;
			mRec.dwMDIdentifier = MD_KEY_TYPE;
			mRec.dwMDUserType = IIS_MD_UT_SERVER;
			mRec.pbMDData = pbBuffer;

			_stprintf(currKey, TEXT("/W3SVC/%s"), subKeyName);

			HRESULT hr2 = pIMeta->GetData(hMeta, currKey, &mRec, &dwReqBufLen);
			//It is a web site node, fetch name and bindings
			if (SUCCEEDED(hr2) && (wcscmp((TCHAR*)mRec.pbMDData, TEXT("IIsWebServer")) == 0))
			{
				WcaLog(LOGMSG_STANDARD, "Found website node. Fetching name and bindings");

				//TCHAR szBinding[256];
				TCHAR szName[256];
				szName[0] = '\0';

				//Name
				mRec.dwMDAttributes = METADATA_NO_ATTRIBUTES;
				mRec.dwMDDataLen = dwBufLen;
				mRec.dwMDDataType = ALL_METADATA;
				mRec.dwMDIdentifier = MD_SERVER_COMMENT;
				mRec.dwMDUserType = IIS_MD_UT_SERVER;
				mRec.pbMDData = pbBuffer;
				HRESULT hr3 = pIMeta->GetData(hMeta, currKey, &mRec, &dwReqBufLen);
				if (SUCCEEDED(hr3))
				{
					wcscpy(szName, (TCHAR*)mRec.pbMDData);
				}

				//Successfully fetched the values, now fill list
				//if (wcslen(szName) > 0 && wcslen(szBinding) > 0)
				if (wcslen(szName) > 0)
				{
					hr = WcaAddTempRecord(&hTable, &hColumns, TEXT("ListBox"), NULL, 0, 3, TEXT("MG_WEBSITE"), indx, szName);
					ExitOnFailure(hr, "Could not add website to list");
					WcaLog(LOGMSG_STANDARD, "Website: %S", szName);
					
					widx++;
				}
			}
		}
		indx++;
	}
	er = ERROR_SUCCESS; //We're done here, so make it sucessful, otherwise MSI will treat it as epic fail!
	goto Final;

LExit:
	er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;

Final:
	WcaLog(LOGMSG_STANDARD, "Finalizing");
	sprintf(szMsg, "Final error code: %d", er);
	WcaLog(LOGMSG_STANDARD, szMsg);
	if (pbBuffer)
	{
		delete pbBuffer;
		pbBuffer = NULL;
	}
	if (hDb)
		MsiCloseHandle(hDb);
	return WcaFinalize(er);
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
