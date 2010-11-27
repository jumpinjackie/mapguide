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

#include <string>
#include <vector>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <fstream>

#define NULL_CHAR '\0'

void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);
void FindAndReplace(std::string& str, const std::string& find, const std::string& replace);

void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void FindAndReplace(std::string& str, const std::string& find, const std::string& replace)
{
	size_t pos = 0;
	while ((pos = str.find(find, pos)) != std::string::npos)
	{
		str.replace(pos, find.length(), replace);
		pos += replace.length();
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
	// Method overview:
	//
	// We extract the value of the MSI property "CustomActionData" which will
	// be of the form:
	//
	// [PHPLOCATION];[WEBSERVERTYPE];[WEBTEMPLOCATION]
	//
	// With this, we update php.ini as follows:
	//
	// replace %MG_WEB_PHP% with [PHPLOCATION]
	// replace %MG_WEB_TEMP% with [WEBTEMPLOCATION]
	// if [WEBSERVERTYPE] == "2" replace "; cgi.force_redirect = 1" with "cgi.force_redirect = 0"

	CHAR szCustomActionData[MAX_BUFFER];
	DWORD dwDataLen = MAX_BUFFER;

	MsiGetProperty(hMSI, "CustomActionData", szCustomActionData, &dwDataLen);

	std::string szMsiData = szCustomActionData;
	
	std::vector<std::string> tokens;
	Tokenize(szMsiData, tokens, ";");

	if (tokens.size() == 3)
	{
		std::string szPhpDir = tokens[0];
		std::string szWebType = tokens[1];
		std::string szTmpDir = tokens[2];

		std::string szFileName = szPhpDir + "php.ini";
		const char* szFile = szFileName.c_str();

		if (FileExists(szFile))
		{
			std::ifstream fin(szFile);

			std::string buffer;
			std::string line;
			while(!fin.eof())
			{
				std::getline(fin, line);

				FindAndReplace(line, "%MG_WEB_PHP%", szPhpDir);
				FindAndReplace(line, "%MG_WEB_TEMP%", szTmpDir);
				if (strcmp(szWebType.c_str(), "2") == 0)
				{
					FindAndReplace(line, "; cgi.force_redirect = 1", "cgi.force_redirect = 0");
				}

				buffer += line + "\n";
			}
			fin.close();

			std::ofstream fout(szFile);
			fout << buffer;
			fout.close();
		}
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
	// Method overview:
	//
	// Extract the value of the MSI property: CustomActionData
	//
	// The value is of the form: [WEBEXTENSIONSLOCATION];[APACHE_API_TYPE];[APACHE_PORT];[APACHELOCATION];[PHPLOCATION];[WEBROOTLOCATION];[VIRTUALDIR]
	//
	// Then in [APACHELOCATION]\conf\httpd.conf, perform the following replacements:
	//
	// %MG_WEB_ROOT% for [WEBROOTLOCATION]
	// %MG_WEB_PORT% for [APACHE_PORT]
	// %MG_WEB_PHP% for [PHPLOCATION]
	// %MG_WEB_APACHE% for [APACHELOCATION]
	// %MG_VIRTUAL_DIR% for [VIRTUALDIR]
	//
	// %MG_INCLUDE_TOMCAT% for "Include conf/tomcat.conf" if java config otherwise
	// "#Uncomment to enable the Java API\n#Include conf/tomcat.conf"
	//
	// %MG_PHP_API% for "#" if java config otherwise ""
	// %MG_JAVA_API% for "" if java config otherwise "#"
	//
	// If java config perform the following additional replacements in the
	// file [APACHELOCATION]\conf\tomcat.conf
	//
	// %MG_WEB_APACHE% for [APACHELOCATION]
	// %MG_VIRTUAL_DIR% for [VIRTUALDIR]

	DWORD   dwDataLen = MAX_BUFFER;
	CHAR    szCustomActionData[MAX_BUFFER];

	MsiGetProperty(hMSI, "CustomActionData", szCustomActionData, &dwDataLen);

	std::string szMsiData = szCustomActionData;
	
	std::vector<std::string> tokens;
	Tokenize(szMsiData, tokens, ";");

	if (tokens.size() == 7)
	{
		std::string szWebExtDir = tokens[0];
		std::string szApiType = tokens[1];
		std::string szPortNo = tokens[2];
		std::string szApacheDir = tokens[3];
		std::string szPhpDir = tokens[4];
		std::string szWebRootDir = tokens[5];
		std::string szVirtualDir = tokens[6];

		//Apach-ify these paths
		FindAndReplace(szWebExtDir, "\\", "/");
		FindAndReplace(szPhpDir, "\\", "/");
		FindAndReplace(szWebRootDir, "\\", "/");
		FindAndReplace(szApacheDir, "\\", "/");

		//Strip trailing slashes if there is one
		std::string::size_type pos = szWebExtDir.find_last_of("/");
		if (pos != std::string::npos && pos == szWebExtDir.length() - 1)
			szWebExtDir.erase(pos);

		pos = szPhpDir.find_last_of("/");
		if (pos != std::string::npos && pos == szPhpDir.length() - 1)
			szPhpDir.erase(pos);

		pos = szWebRootDir.find_last_of("/");
		if (pos != std::string::npos && pos == szWebRootDir.length() - 1)
			szWebRootDir.erase(pos);

		pos = szApacheDir.find_last_of("/");
		if (pos != std::string::npos && pos == szApacheDir.length() - 1)
			szApacheDir.erase(pos);

		std::string szFileName = szApacheDir + "\\conf\\httpd.conf";
		const char* szHttpdConf = szFileName.c_str();
		bool bJava = (strcmp(szApiType.c_str(), "JAVA") == 0);

		if (FileExists(szHttpdConf))
		{
			std::ifstream fin(szHttpdConf);
			
			std::string buffer;
			std::string line;
			while(!fin.eof())
			{
				std::getline(fin, line);

				//Process this line

				FindAndReplace(line, "%MG_WEB_ROOT%", szWebRootDir);
				FindAndReplace(line, "%MG_WEB_PORT%", szPortNo);
				FindAndReplace(line, "%MG_WEB_PHP%", szPhpDir);
				FindAndReplace(line, "%MG_WEB_APACHE%", szApacheDir);
				FindAndReplace(line, "%MG_VIRTUAL_DIR%", szVirtualDir);
				if (bJava)
				{
					FindAndReplace(line, "%MG_INCLUDE_TOMCAT%", "Include conf/tomcat.conf");
					FindAndReplace(line, "%MG_PHP_API%", "#");
					FindAndReplace(line, "%MG_JAVA_API%", "");
				}
				else
				{
					FindAndReplace(line, "%MG_INCLUDE_TOMCAT%", "#Uncomment to enable the Java API\n#Include conf/tomcat.conf");
					FindAndReplace(line, "%MG_PHP_API%", "");
					FindAndReplace(line, "%MG_JAVA_API%", "#");
				}

				buffer += line + "\n";
			}
			fin.close();
			
			std::ofstream fout(szHttpdConf);
			fout << buffer;
			fout.close();
		}

		szFileName = szApacheDir + "\\conf\\tomcat.conf";
		const char* szTomcatConf = szFileName.c_str();

		if (FileExists(szTomcatConf))
		{
			std::ifstream fin(szTomcatConf);
			
			std::string buffer;
			std::string line;
			while(!fin.eof())
			{
				std::getline(fin, line);

				//Process this line
				FindAndReplace(line, "%MG_WEB_APACHE%", szApacheDir);
				FindAndReplace(line, "%MG_VIRTUAL_DIR%", szVirtualDir);

				buffer += line + "\n";
			}
			fin.close();

			std::ofstream fout(szTomcatConf);
			fout << buffer;
			fout.close();
		}
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
