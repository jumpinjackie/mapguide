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
* DESCRIPTION: apache_actions
*/

#include "stdafx.h"
#include "apache_actions.h"

#define NULL_CHAR '\0'

void ConvertToApachePath(char * str);

/*****************************************************************************
* Name			: ConverToApachePath
* Description	: Converts a path to a apache-friendly format
* Parameters	: str
* Return		: none
*****************************************************************************/
void ConvertToApachePath(char * str) 
{
	for(unsigned int i = 0; i < strlen(str); i++)
	{
		if(str[i] == '\\')
			str[i] = '/';
	}
}

/*****************************************************************************
* Name        : DllMain
*
* Description : This is the entry point for construction/destruction.
*
* Parameters  : HANDLE hModule
*               DWORD  ul_reason_for_call
*               LPVOID lpReserved
*
* Return      : bool
*
* Notes    : N/A
*
*****************************************************************************/

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	return TRUE;
}

/*****************************************************************************
* Name        : GetApacheDir
*
* Description : This routine gets the location of the apache install dir 
*
* Parameters  : MSIHANDLE hMSI
*
* Return      : UINT
*
* Notes    : N/A
*
*****************************************************************************/

UINT __stdcall
GetApacheDir(MSIHANDLE hMSI)
{
	HRESULT hRes = 0;
	HKEY    hKey = 0;
	WCHAR   RK_APACHE_ROOT[MAX_BUFFER] = L"SOFTWARE\\Apache Group\\Apache\\";
	DWORD   dwDataLen = MAX_BUFFER;
	WCHAR   wszApacheVersion[MAX_BUFFER + 1] = L"";
	DWORD   dwType = 0;
	WCHAR   data[MAX_BUFFER + 1] = L"";
	DWORD   dwData = MAX_BUFFER;

	// retrieve APACHE_VERSION MSI property
	MsiGetPropertyW(hMSI, L"APACHE_VERSION", wszApacheVersion, &dwDataLen);
	wcscat(RK_APACHE_ROOT, wszApacheVersion);

	//::MessageBoxW(NULL, wszApacheVersion, L"Test", MB_OK);

	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, RK_APACHE_ROOT, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueExW(hKey, L"ServerRoot", NULL, &dwType, (LPBYTE)data, &dwData) == ERROR_SUCCESS)
		{
			MsiSetPropertyW(hMSI, L"APACHE_INSTALLDIR", data);
			//::MessageBoxW(NULL, L"Registry set", L"Test", MB_OK);
		}
	}

	return ERROR_SUCCESS;
}

/*****************************************************************************
* Name        : UpdatePhpIni
*
* Description : Add the MapGuide extensions to the php.ini file 
*
* Parameters  : MSIHANDLE hMSI
*
* Return      : UINT
*
* Notes    : N/A
*
*****************************************************************************/

UINT __stdcall
UpdatePhpIni(MSIHANDLE hMSI)
{
	DWORD   dwDataLen = MAX_BUFFER;
	WCHAR   wszCustomActionData[MAX_BUFFER + 1] = L"";
	CHAR    szCustomActionData[MAX_BUFFER];
	CHAR    szPhpDir[MAX_BUFFER];

	CHAR	szWebTempDir[MAX_BUFFER];
	CHAR    szWebServerType[MAX_BUFFER];
	CHAR    szFile[MAX_BUFFER];
	CHAR    szFind[MAX_BUFFER];
	CHAR    szReplace[MAX_BUFFER];
	int     nLen;

	// retrieve the CustomActionData
	dwDataLen = MAX_BUFFER;
	MsiGetPropertyW(hMSI, L"CustomActionData", wszCustomActionData, &dwDataLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, wszCustomActionData, dwDataLen, szCustomActionData, MAX_BUFFER, NULL, NULL);
	szCustomActionData[nLen] = NULL_CHAR;

	szPhpDir[0] = NULL_CHAR;
	szWebServerType[0] = NULL_CHAR;

	//::MessageBoxW(NULL, wszCustomActionData, L"CustomActionData", MB_OK);

	//[PHPLOCATION];[WEBSERVERTYPE];[WEBTEMPLOCATION]
	char * tok = strtok(szCustomActionData, ";");
	//[PHPLOCATION]
	if(tok)
	{
		strcpy(szPhpDir, tok);
		tok = strtok(NULL, ";");
		//[WEBSERVERTYPE]
		if(tok)
		{
			strcpy(szWebServerType, tok);
			tok = strtok(NULL, ";");
		}
		//[WEBTEMPLOCATION]
		if(tok)
		{
			strcpy(szWebTempDir, tok);
			tok = strtok(NULL, ";");
		}
	}

	// create the file path to php.ini
	strcpy(szFile, szPhpDir);
	strcat(szFile, "php.ini");

	//::MessageBox(NULL, szFile, "Test", MB_OK);

	if( FileExists(szFile) )
	{
		//::MessageBoxW(NULL, L"File found", L"Test", MB_OK);
		// Opening the file to read and write.
		//
		CFile cfInFile(szFile, CFile::modeRead);
		DWORD dwFileLen = cfInFile.GetLength();

		// Read the entire contents of the file.
		//
		char *szBuff = new char[dwFileLen + 1];
		cfInFile.Read(szBuff, dwFileLen);
		szBuff[dwFileLen] = NULL_CHAR;                   // Add ending null to buffer.

		cfInFile.Close();

		// Creating a CString object to store the contents and help
		// us to add the desired configuration.
		//
		CString csBuff(szBuff);
		strcpy(szFind,"%MG_WEB_PHP%");
		csBuff.Replace(szFind,szPhpDir);

		strcpy(szFind,"%MG_WEB_TEMP%");
		csBuff.Replace(szFind,szWebTempDir);

		if ( strcmp(szWebServerType,"2") == 0 )
		{
			strcpy(szFind,"; cgi.force_redirect = 1");
			strcpy(szReplace,"cgi.force_redirect = 0");
			csBuff.Replace(szFind,szReplace);
		}

		CFile cfOutFile(szFile, CFile::modeWrite);

		// Write and modify the contents.
		//
		DWORD dwNewLength = csBuff.GetLength();
		cfOutFile.SeekToBegin();
		cfOutFile.Write(csBuff, dwNewLength);
		cfOutFile.SetLength(dwNewLength);
		cfOutFile.Close();
	}

	return ERROR_SUCCESS;
}

/*****************************************************************************
* Name        : UpdateApacheConfig
*
* Description : Add the Tux Virtual directories to Apache httpd.conf 
*
* Parameters  : MSIHANDLE hMSI
*
* Return      : UINT
*
* Notes    : N/A
*
*****************************************************************************/

UINT __stdcall
UpdateApacheConfig(MSIHANDLE hMSI)
{
	DWORD   dwDataLen = MAX_BUFFER;
	WCHAR   wszCustomActionData[MAX_BUFFER + 1] = L"";
	CHAR    szCustomActionData[MAX_BUFFER];
	CHAR    szInstallDir[MAX_BUFFER];
	CHAR    szApacheDir[MAX_BUFFER];
	CHAR    szPhpDir[MAX_BUFFER];
	CHAR    szWebDir[MAX_BUFFER];
	CHAR    szApiType[MAX_BUFFER];
	CHAR    szServerPort[MAX_BUFFER];
	CHAR    szFile[MAX_BUFFER];
	CHAR    szFind[MAX_BUFFER];
	CHAR    szReplace[MAX_BUFFER];
	bool    bJava = false;
	int     nLen;

	// retrieve the CustomActionData
	dwDataLen = MAX_BUFFER;
	MsiGetPropertyW(hMSI, L"CustomActionData", wszCustomActionData, &dwDataLen);

	nLen = WideCharToMultiByte(CP_ACP, 0, wszCustomActionData, dwDataLen, szCustomActionData, MAX_BUFFER, NULL, NULL);
	szCustomActionData[nLen] = NULL_CHAR;

	szInstallDir[0] = NULL_CHAR;
	szApacheDir[0] = NULL_CHAR;
	szPhpDir[0] = NULL_CHAR;
	szWebDir[0] = NULL_CHAR;
	szApiType[0] = NULL_CHAR;
	szServerPort[0] = NULL_CHAR;

	//[WEBEXTENSIONSLOCATION];[APACHE_API_TYPE];[APACHE_PORT];[APACHELOCATION];[PHPLOCATION];[WEBROOTLOCATION]
	char * tok = strtok(szCustomActionData, ";");
	//[WEBEXTENSIONSLOCATION]
	if(tok)
	{
		strcpy(szInstallDir, tok);
		tok = strtok(NULL, ";");
		//[APACHE_API_TYPE]
		if(tok)
		{
			strcpy(szApiType, tok);
			tok = strtok(NULL, ";");
		}
		//[APACHE_PORT]
		if(tok)
		{
			strcpy(szServerPort, tok);
			tok = strtok(NULL, ";");
		}
		//[APACHELOCATION]
		if(tok)
		{
			strcpy(szApacheDir, tok);
			tok = strtok(NULL, ";");
		}
		//[PHPLOCATION]
		if(tok)
		{
			strcpy(szPhpDir, tok);
			tok = strtok(NULL, ";");
		}
		//[WEBROOTLOCATION]
		if(tok)
		{
			strcpy(szWebDir, tok);
		}
	}

	bJava = ( strcmp(szApiType,"JAVA") == 0 );

	ConvertToApachePath(szInstallDir);
	ConvertToApachePath(szPhpDir);
	ConvertToApachePath(szWebDir);
	ConvertToApachePath(szApacheDir);

	// The directory will have a fwd slash, so the last char should be null-terminated
	szApacheDir[strlen(szApacheDir)-1] = NULL_CHAR;

	// create the file path to http.conf
	strcpy(szFile, szApacheDir);
	strcat(szFile, "\\conf\\httpd.conf");

	if( FileExists(szFile) )
	{
		//::MessageBoxW(NULL, L"File found", L"Test", MB_OK);
		// Opening the file to read and write.
		//
		CFile cfInFile(szFile, CFile::modeRead);
		DWORD dwFileLen = cfInFile.GetLength();

		// Read the entire contents of the file.
		//
		char *szBuff = new char[dwFileLen + 1];
		cfInFile.Read(szBuff, dwFileLen);
		szBuff[dwFileLen] = '\0';                   // Add ending null to buffer.

		cfInFile.Close();

		// Creating a CString object to store the contents and help
		// us to add the desired configuration.
		//
		CString csBuff(szBuff);

		// Replace %MG_WEB_ROOT%
		strcpy(szFind,"%MG_WEB_ROOT%");
		strcpy(szReplace, szWebDir);
		csBuff.Replace(szFind,szReplace);

		// Replace %MG_WEB_PORT%
		strcpy(szFind,"%MG_WEB_PORT%");
		strcpy(szReplace, szServerPort);
		csBuff.Replace(szFind,szReplace);

		// Replace %MG_WEB_PHP%
		strcpy(szFind,"%MG_WEB_PHP%");
		strcpy(szReplace, szPhpDir);
		csBuff.Replace(szFind,szReplace);

		// Replace %MG_WEB_APACHE%
		strcpy(szFind,"%MG_WEB_APACHE%");
		strcpy(szReplace, szApacheDir);
		csBuff.Replace(szFind, szReplace);

		// If java, then include the tomcat configuration
		strcpy(szFind, "%MG_INCLUDE_TOMCAT%");
		csBuff.Replace(szFind, bJava ? "Include conf/tomcat.conf" : "#Uncomment to enable the Java API\n#Include conf/tomcat.conf");

		// Toggle PHP API section
		strcpy(szFind, "%MG_PHP_API%");
		strcpy(szReplace, bJava ? "#" : "");
		csBuff.Replace(szFind, szReplace);

		// Toggle Java API section
		strcpy(szFind, "%MG_JAVA_API%");
		strcpy(szReplace, bJava ? "" : "#");
		csBuff.Replace(szFind, szReplace);

		CFile cfOutFile(szFile, CFile::modeWrite);

		// Write and modify the contents.
		//
		DWORD dwNewLength = csBuff.GetLength();
		cfOutFile.SeekToBegin();
		cfOutFile.Write(csBuff, dwNewLength);
		cfOutFile.SetLength(dwNewLength);
		cfOutFile.Close();
	}

	strcpy(szFile, szApacheDir);
	strcat(szFile, "\\conf\\tomcat.conf");

	if(FileExists(szFile))
	{
		CFile cfInFile(szFile, CFile::modeRead);
		DWORD dwFileLen = cfInFile.GetLength();

		// Read the entire contents of the file.
		//
		char *szBuff = new char[dwFileLen + 1];
		cfInFile.Read(szBuff, dwFileLen);
		szBuff[dwFileLen] = '\0';                   // Add ending null to buffer.

		cfInFile.Close();

		CString csBuff(szBuff);
		// Replace %MG_WEB_APACHE%
		strcpy(szFind,"%MG_WEB_APACHE%");
		strcpy(szReplace, szApacheDir);
		csBuff.Replace(szFind,szReplace);

		CFile cfOutFile(szFile, CFile::modeWrite);

		// Write and modify the contents.
		//
		DWORD dwNewLength = csBuff.GetLength();
		cfOutFile.SeekToBegin();
		cfOutFile.Write(csBuff, dwNewLength);
		cfOutFile.SetLength(dwNewLength);
		cfOutFile.Close();
	}

	strcpy(szFile, szInstallDir);
	strcat(szFile, "Tomcat\\conf\\Catalina\\localhost\\mapguide.xml");

	if( FileExists(szFile) )
	{
		// Opening the file to read and write.
		//
		CFile cfInFile(szFile, CFile::modeRead);
		DWORD dwFileLen = cfInFile.GetLength();

		// Read the entire contents of the file.
		//
		char *szBuff = new char[dwFileLen + 1];
		cfInFile.Read(szBuff, dwFileLen);
		szBuff[dwFileLen] = '\0';                   // Add ending null to buffer.

		cfInFile.Close();

		// Creating a CString object to store the contents and help
		// us to add the desired configuration.
		//
		CString csBuff(szBuff);
		// Replace %MG_WEB_ROOT%
		strcpy(szFind,"%MG_WEB_ROOT%");
		//CString csWebDir(szWebDir);
		//csWebDir.Replace("\\", "/");
		strcpy(szReplace, szWebDir);
		csBuff.Replace(szFind,szReplace);

		CFile cfOutFile(szFile, CFile::modeWrite);

		// Write and modify the contents.
		//
		DWORD dwNewLength = csBuff.GetLength();
		cfOutFile.SeekToBegin();
		cfOutFile.Write(csBuff, dwNewLength);
		cfOutFile.SetLength(dwNewLength);
		cfOutFile.Close();
	}
	return ERROR_SUCCESS;
}

/*****************************************************************************
* Name        : FileExists
*
* Description : This function looks for the existence of a given file. 
*
* Parameters  : szFile - File name that we are searching for.
*
* Return      : true if the file exists and false if it doesn't.
*
* Notes    : N/A
*
*****************************************************************************/

bool FileExists (const char* szFile)
{
	// Same thing here, you should assert for szFile.

	WIN32_FIND_DATA hFindData;
	bool            bRetVal = true;

	// Basically what we do, is create a search for the file that we are
	// interested. If the file exists, we get a handle that we close either
	// way, and return true. If the function returns INVALID_HANDLE_VALUE, we
	// assume that the file does not exists and we return false.
	// 
	HANDLE hFind = FindFirstFile(szFile, &hFindData);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		bRetVal = false;
	}

	FindClose(hFind);

	return bRetVal;
}
