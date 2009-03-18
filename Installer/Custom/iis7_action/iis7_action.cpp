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
 * DESCRIPTION: iis7_action
 */
#include "iis7_action.h"
#include <fstream>
#include <string>

#ifdef _UNICODE
#define _string std::wstring
#define _ofstream std::wofstream
#else
#define _string std::string
#define _ofstream std::ofstream
#endif

inline void Replace(_string & str, const TCHAR * find, const TCHAR * replace);

bool ExecuteAppCmd(const TCHAR * szCmdLine);
_string getInstallDir(MSIHANDLE hMSI);

const int MAX_BUFFER = 2048;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	return TRUE;
}

void Replace(_string & str, const TCHAR * find, const TCHAR * replace)
{
	size_t nPos;
	_string strFind = find;
	nPos = str.find(strFind);
	str.replace(nPos, strFind.length(), replace);
}

bool ExecuteAppCmd(const TCHAR * szCmdLine)
{
	bool result = false;
	const int bufSize = 2048;
	TCHAR szWinDir[bufSize];
	TCHAR szDir[MAX_PATH];
	TCHAR szInstallLogPath[MAX_PATH];
	char szEndLine[] = "=====================\r\n";
	
	//::MessageBox(NULL,_T("Enter ExecuteAppCmd()"),NULL, MB_OK);
	if( 0 != GetWindowsDirectory(szWinDir, bufSize) )
	{
		//::MessageBox(NULL,szWinDir,NULL, MB_OK);
		TCHAR szAppCMDExe[bufSize];
		ZeroMemory(szAppCMDExe ,  sizeof(szAppCMDExe));
		_stprintf_s(szAppCMDExe, bufSize, _T("%s\\System32\\inetsrv\\appcmd.exe %s"), szWinDir, szCmdLine);		
		//::MessageBox(NULL,szAppCMDExe,NULL, MB_OK);

		if (!GetTempPath(MAX_PATH-1, szDir))
		{
			_tcscpy_s(szDir, MAX_PATH, _T("c:\\"));
		}
		_stprintf_s(szInstallLogPath, MAX_PATH , _T("%s%s"), szDir, _T("iis7_set.log"));
		//::MessageBox(NULL, szInstallLogPath, NULL, 0);
		STARTUPINFO   si;   
		PROCESS_INFORMATION   pi;

		HANDLE   hWriteFile,hReadPipe;
		SECURITY_ATTRIBUTES   pipeattr;   

		pipeattr.nLength   =   sizeof(SECURITY_ATTRIBUTES);   
		pipeattr.lpSecurityDescriptor   =   NULL;   
		pipeattr.bInheritHandle   =   true;   

		BOOL bPipeReady = CreatePipe(&hReadPipe,&hWriteFile,&pipeattr,0);   

		memset(&si,0,sizeof(si));   
		memset(&pi,0,sizeof(pi));   

		SECURITY_ATTRIBUTES psa={sizeof(psa),NULL,TRUE};
		psa.bInheritHandle=TRUE;   
		HANDLE  houtfile=CreateFile(szInstallLogPath,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE, &psa,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		DWORD dwPtr = SetFilePointer(houtfile, 0, NULL, FILE_END);

		si.cb   =   sizeof(STARTUPINFO);
		si.dwFlags   =   STARTF_USESHOWWINDOW   |   STARTF_USESTDHANDLES;   
		si.wShowWindow   =   SW_HIDE;   
		si.hStdError   =   houtfile;//hwrite   file   
		si.hStdOutput   =   houtfile;   

		DWORD len;
		SYSTEMTIME systemTime;
		::GetSystemTime( &systemTime );
		char szTime[bufSize];
		sprintf_s(szTime, bufSize, "[%4d/%2d/%2d|%2d:%2d] ", 
			systemTime.wYear, 
			systemTime.wMonth,
			systemTime.wDay,
			systemTime.wHour,
			systemTime.wMinute);
		::WriteFile(houtfile, szTime, strlen(szTime), &len, NULL);
		
#ifdef _UNICODE
		char szCmdName[bufSize];
		::WideCharToMultiByte(CP_ACP, 0, szAppCMDExe, _tcslen(szAppCMDExe)+1,szCmdName, bufSize, NULL, NULL);
		::WriteFile(houtfile,szCmdName, strlen(szCmdName), &len, NULL);
#else
		::WriteFile(houtfile, szAppCMDExe, _tcslen(szAppCMDExe), &len, NULL);
#endif
		::WriteFile(houtfile, "\r\n", sizeof("\r\n"), &len, NULL);

		if (0 != CreateProcess(NULL,
			szAppCMDExe,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&si,
			&pi)
			)
		{
			//installLog << _T("Command executed successfully") << std::endl;
			result = true;

			// Wait until child process exits.
			WaitForSingleObject( pi.hProcess, 5000 );
			TerminateProcess(pi.hProcess,1);
			// Close process and thread handles. 
			CloseHandle( pi.hThread );
			CloseHandle( pi.hProcess );
		}
		else
		{
			//installLog << _T("Command executed failed") << std::endl;
		}
		
		::WriteFile(houtfile, szEndLine, sizeof(szEndLine), &len, NULL);
		CloseHandle(houtfile);   
		CloseHandle(hWriteFile);   
		CloseHandle(hReadPipe);  
	}

	return result;
}

UINT __stdcall CreateAPPMapGuideWithIIS7(MSIHANDLE hMSI)
{
	//::MessageBox(NULL,_T("CreateAPPMapGuideWithIIS7()"),NULL, MB_OK);
	
	//appcmd add apppool /name:MG_VDIRAppPool
	//appcmd set config /section:applicationPools /[name='MG_VDIRAppPool'].processModel.idleTimeout:00:00:00
	//appcmd unlock config /section:handlers
	//appcmd unlock config "MG_WEBSITE" /section:handlers
	//appcmd add app /site.name:"MG_WEBSITE" /path:/MG_VDIR /physicalPath:"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www"
	//appcmd set config "MG_WEBSITE/MG_VDIR" /section:handlers /+[name='PHP',path='*.php',verb='*',scriptProcessor='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\Php\php5isapi.dll"',modules='IsapiModule',resourceType='Unspecified']
	//appcmd set config /section:isapiCgiRestriction /+[path='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\Php\php5isapi.dll"',allowed='True']
	//appcmd add app /site.name:"MG_WEBSITE" /path:/MG_VDIR/mapagent /physicalPath:"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapagent"
	//appcmd set config "MG_WEBSITE/MG_VDIR/mapagent" /section:handlers /accessPolicy:Read,Script,Execute
	//appcmd set config "MG_WEBSITE/MG_VDIR/mapagent" /section:handlers /+[name='"MapGuide ISAPI agent"',path='mapagent.fcgi',verb='*',scriptProcessor='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapagent\isapi_MapAgent.dll"',modules='IsapiModule']
	//appcmd set config /section:isapiCgiRestriction /+[path='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapagent\isapi_MapAgent.dll"',allowed='True']
	//appcmd set app "MG_WEBSITE/MG_VDIR" /applicationPool:MG_VDIRAppPool
	//appcmd set app "MG_WEBSITE/MG_VDIR/mapagent" /applicationPool:MG_VDIRAppPool
	//appcmd unlock config /section:httpErrors
	//appcmd unlock config "MG_WEBSITE" /section:httpErrors
	//appcmd unlock config "MG_WEBSITE/MG_VDIR" /section:httpErrors
	//appcmd set config "MG_WEBSITE/MG_VDIR" /section:httpErrors /-"[statusCode='401']"

	_string installDir(getInstallDir(hMSI));
	_string cmd;

	//appcmd add apppool /name:MG_APP_POOL
	//appcmd set config /section:applicationPools /[name='MG_APP_POOL'].processModel.idleTimeout:00:00:00
	cmd = _T("add apppool /name:MG_APP_POOL");
	Replace(cmd, _T("MG_APP_POOL"), MG_APP_POOL);
	ExecuteAppCmd(cmd.c_str());
	cmd = _T("set config /section:applicationPools /[name='MG_APP_POOL'].processModel.idleTimeout:00:00:00");
	Replace(cmd, _T("MG_APP_POOL"), MG_APP_POOL);
	ExecuteAppCmd(cmd.c_str());

	//appcmd unlock config /section:handlers
	//appcmd unlock config "MG_WEBSITE" /section:handlers
	cmd = _T("unlock config /section:handlers");
	ExecuteAppCmd(cmd.c_str());
	cmd = _T("unlock config \"MG_WEBSITE\" /section:handlers");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	ExecuteAppCmd(cmd.c_str());

	//appcmd add app /site.name:"MG_WEBSITE" /path:/MG_VDIR /physicalPath:"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www"
	cmd = _T("add app /site.name:\"MG_WEBSITE\" /path:/MG_VDIR /physicalPath:\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	cmd += installDir;
	cmd += _T("www");
	ExecuteAppCmd(cmd.c_str());

	//appcmd set config "MG_WEBSITE/MG_VDIR" /section:handlers /+[name='PHP',path='*.php',verb='*',scriptProcessor='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\Php\php5isapi.dll"',modules='IsapiModule',resourceType='Unspecified']
	cmd = _T("set config \"MG_WEBSITE/MG_VDIR\" /section:handlers /+[name=\'PHP\',path=\'*.php\',verb=\'*\',scriptProcessor=\'\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	cmd += installDir;
	cmd += _T("Php\\php5isapi.dll\"\',modules=\'IsapiModule\',resourceType=\'Unspecified\']");
	ExecuteAppCmd(cmd.c_str());

	//appcmd set config /section:isapiCgiRestriction /+[path='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\Php\php5isapi.dll"',allowed='True']
	cmd = _T("set config /section:isapiCgiRestriction /+[path=\'\"");
	cmd += installDir;
	cmd += _T("Php\\php5isapi.dll\"\',allowed=\'True\']");
	ExecuteAppCmd(cmd.c_str());

	//appcmd add app /site.name:"MG_WEBSITE" /path:/MG_VDIR/mapagent /physicalPath:"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapagent"
	cmd = _T("add app /site.name:\"MG_WEBSITE\" /path:/MG_VDIR/mapagent /physicalPath:\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	cmd += installDir;
	cmd += _T("www\\mapagent");
	ExecuteAppCmd(cmd.c_str());

	//appcmd set config "MG_WEBSITE/MG_VDIR/mapagent" /section:handlers /accessPolicy:Read,Script,Execute
	cmd = _T("set config \"MG_WEBSITE/MG_VDIR/mapagent\" /section:handlers /accessPolicy:Read,Script,Execute");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());

	//appcmd set config "MG_WEBSITE/MG_VDIR/mapagent" /section:handlers /+[name='"MapGuide ISAPI agent"',path='mapagent.fcgi',verb='*',scriptProcessor='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapagent\isapi_MapAgent.dll"',modules='IsapiModule']
	cmd = _T("set config \"MG_WEBSITE/MG_VDIR/mapagent\" /section:handlers /+[name=\'\"MapGuide ISAPI agent\"\',path=\'mapagent.fcgi\',verb=\'*\',scriptProcessor=\'\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	cmd += installDir;
	cmd += _T("www\\mapagent\\isapi_MapAgent.dll\"\',modules=\'IsapiModule\']");
	ExecuteAppCmd(cmd.c_str());

	//appcmd set config /section:isapiCgiRestriction /+[path='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapagent\isapi_MapAgent.dll"',allowed='True']
	cmd = _T("set config /section:isapiCgiRestriction /+[path=\'\"");
	cmd += installDir;
	cmd += _T("www\\mapagent\\isapi_MapAgent.dll\"\',allowed=\'True\']");
	ExecuteAppCmd(cmd.c_str());

	//appcmd set app "MG_WEBSITE/MG_VDIR" /applicationPool:MG_VDIRAppPool
	//appcmd set app "MG_WEBSITE/MG_VDIR/mapagent" /applicationPool:MG_VDIRAppPool
	cmd = _T("set app \"MG_WEBSITE/MG_VDIR\" /applicationPool:MG_APP_POOL");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	Replace(cmd, _T("MG_APP_POOL"), MG_APP_POOL);
	ExecuteAppCmd(cmd.c_str());
	cmd = _T("set app \"MG_WEBSITE/MG_VDIR/mapagent\" /applicationPool:MG_APP_POOL");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	Replace(cmd, _T("MG_APP_POOL"), MG_APP_POOL);
	ExecuteAppCmd(cmd.c_str());

	//appcmd unlock config /section:httpErrors
	//appcmd unlock config "MG_WEBSITE" /section:httpErrors
	//appcmd unlock config "MG_WEBSITE/MG_VDIR" /section:httpErrors
	//appcmd set config "MG_WEBSITE/MG_VDIR" /section:httpErrors /-"[statusCode='401']"
	cmd = _T("unlock config /section:httpErrors");
	ExecuteAppCmd(cmd.c_str());
	cmd = _T("unlock config \"MG_WEBSITE\" /section:httpErrors");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	ExecuteAppCmd(cmd.c_str());
	cmd = _T("unlock config \"MG_WEBSITE/MG_VDIR\" /section:httpErrors");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());
	cmd = _T("set config \"MG_WEBSITE/MG_VDIR\" /section:httpErrors /-\"[statusCode=\'401\']\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());
	return 0;
}
UINT __stdcall CreateVDirPhpAgentWithIIS7(MSIHANDLE hMSI)
{
	//appcmd add  vdir /app.name:"MG_WEBSITE/" /path:/MG_VDIR/mapviewerajax /physicalPath:"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapviewerphp"
	//appcmd set config "MG_WEBSITE/MG_VDIR/mapviewerajax" /section:defaultDocument /+files.[value='ajaxviewer.php']
	_string cmd;
	_string installDir(getInstallDir(hMSI));

	//appcmd add  vdir /app.name:"MG_WEBSITE/" /path:/MG_VDIR/mapviewerajax /physicalPath:"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapviewerphp"
	cmd = _T("add vdir /app.name:\"MG_WEBSITE/\" /path:/MG_VDIR/mapviewerajax /physicalPath:\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	cmd += installDir;
	cmd += _T("www\\mapviewerphp\"");
	ExecuteAppCmd(cmd.c_str());

	//appcmd set config "MG_WEBSITE/MG_VDIR/mapviewerajax" /section:defaultDocument /+files.[value='ajaxviewer.php']
	cmd = _T("set config \"MG_WEBSITE/MG_VDIR/mapviewerajax\" /section:defaultDocument /+files.[value=\'ajaxviewer.php\']");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());	

	return 0;
}
UINT __stdcall CreateAPPNetAgentWithIIS7(MSIHANDLE hMSI)
{
	//appcmd add app /site.name:"MG_WEBSITE" /path:/MG_VDIR/mapviewerajax /physicalPath:"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapviewernet"
	//appcmd set config "MG_WEBSITE/MG_VDIR/mapviewerajax" /section:defaultDocument /+files.[value='ajaxviewer.aspx']
	//appcmd set app "MG_WEBSITE/MG_VDIR/mapviewerajax" /applicationPool:MG_VDIRAppPool
	_string cmd;
	_string installDir(getInstallDir(hMSI));

	//appcmd add app /site.name:"MG_WEBSITE" /path:/MG_VDIR/mapviewerajax /physicalPath:"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapviewerphp"
	cmd = _T("add app /site.name:\"MG_WEBSITE\" /path:/MG_VDIR/mapviewerajax /physicalPath:\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	cmd += installDir;
	cmd += _T("www\\mapviewernet\"");
	ExecuteAppCmd(cmd.c_str());
	
	//appcmd set config "MG_WEBSITE/MG_VDIR/mapviewerajax" /section:defaultDocument /+files.[value='ajaxviewer.aspx']
	cmd = _T("set config \"MG_WEBSITE/MG_VDIR/mapviewerajax\" /section:defaultDocument /+files.[value=\'ajaxviewer.aspx\']");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());

	//appcmd set app "MG_WEBSITE/MG_VDIR/mapviewerajax" /applicationPool:MG_VDIRAppPool
	cmd = _T("set app \"MG_WEBSITE/MG_VDIR/mapviewerajax\" /applicationPool:MG_APP_POOL");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	Replace(cmd, _T("MG_APP_POOL"), MG_APP_POOL);
	ExecuteAppCmd(cmd.c_str());

	return 0;
}

UINT __stdcall DeleteAPPMapGuideWithIIS7(MSIHANDLE hMSI)
{
	//1. appcmd delete app "MG_WEBSITE/MG_VDIR/mapviewerajax"
	//2. appcmd delete vdir "MG_WEBSITE/MG_VDIR/mapviewerajax"
	//3. appcmd set config /section:isapiCgiRestriction /-[path='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapagent\isapi_MapAgent.dll"',allowed='True']
	//4. appcmd set config "MG_WEBSITE/MG_VDIR/mapagent" /section:handlers /-[name='"MapGuide ISAPI agent"']
	//5. appcmd delete app "MG_WEBSITE/MG_VDIR/mapagent"
	//6. appcmd set config /section:isapiCgiRestriction /-[path='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\Php\php5isapi.dll"',allowed='True']
	//7. appcmd set config "MG_WEBSITE/MG_VDIR" /section:handlers /-[name='PHP']
	//8. appcmd delete app "MG_WEBSITE/MG_VDIR" 

	_string installDir(getInstallDir(hMSI));
	_string cmd;
	//1. appcmd delete app "MG_WEBSITE/MG_VDIR/mapviewerajax"
	//2. appcmd delete vdir "MG_WEBSITE/MG_VDIR/mapviewerajax"
	cmd = _T("delete app \"MG_WEBSITE/MG_VDIR/mapviewerajax\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());
	cmd = _T("delete vdir \"MG_WEBSITE/MG_VDIR/mapviewerajax\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());

	//3. appcmd set config /section:isapiCgiRestriction /-[path='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\www\mapagent\isapi_MapAgent.dll"',allowed='True']
	cmd = _T("set config /section:isapiCgiRestriction /-[path=\'\"");
	cmd += installDir;
	cmd += _T("www\\mapagent\\isapi_MapAgent.dll\"\',allowed=\'True\']");
	ExecuteAppCmd(cmd.c_str());

	//4. appcmd set config "MG_WEBSITE/MG_VDIR/mapagent" /section:handlers /-[name='"MapGuide ISAPI agent"']
	cmd = _T("set config \"MG_WEBSITE/MG_VDIR/mapagent\" /section:handlers /-[name=\'\"MapGuide ISAPI agent\"\']");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());
	
	//5. appcmd delete app "MG_WEBSITE/MG_VDIR/mapagent"
	cmd = _T("delete app \"MG_WEBSITE/MG_VDIR/mapagent\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());

	//6. appcmd set config /section:isapiCgiRestriction /-[path='"C:\Program Files\Autodesk\MapGuideEnterprise2010\WebServerExtensions\Php\php5isapi.dll"',allowed='True']
	cmd = _T("set config /section:isapiCgiRestriction /-[path=\'\"");
	cmd += installDir;
	cmd += _T("Php\\php5isapi.dll\"\',allowed=\'True\']");
	ExecuteAppCmd(cmd.c_str());

	//7. appcmd set config "MG_WEBSITE/MG_VDIR" /section:handlers /-[name='PHP']
	cmd = _T("set config \"MG_WEBSITE/MG_VDIR\" /section:handlers /-[name=\'PHP\']");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());

	//8. appcmd delete app "MG_WEBSITE/MG_VDIR"
	cmd = _T("delete app \"MG_WEBSITE/MG_VDIR\"");
	Replace(cmd, _T("MG_WEBSITE"), MG_WEBSITE);
	Replace(cmd, _T("MG_VDIR"), MG_VDIR);
	ExecuteAppCmd(cmd.c_str());

	//TODO: 9. appcmd delete apppool /apppool.name:MG_VDIRAppPool
	// cmd = _T("delete apppool /name:MG_APP_POOL");
	// Replace(cmd, _T("MG_APP_POOL"), MG_APP_POOL);
	// ExecuteAppCmd(cmd.c_str());

	return 0;
}

_string getInstallDir(MSIHANDLE hMSI)
{
	if( hMSI == NULL ) return _string(MG_DEFAULT_WEB_PATH);
	TCHAR installDir[MAX_BUFFER];
	DWORD installDirLen = MAX_BUFFER;
	MsiGetProperty(hMSI, MSI_PROP_INSTALLDIR, installDir, &installDirLen);
	int len = _tcslen(installDir);
	if( len > 0 && installDir[len-1] != _T('\\') )
	{
		installDir[len++] = _T('\\');
		installDir[len] = _T('\0');
	}
	return _string(installDir);
}
