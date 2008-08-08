//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#pragma once
/* PHP Extension headers */
/* include zend win32 config first */
#include "zend_config.w32.h"
/* include standard header */
#include "php.h"

#include <windows.h>


#define GETENV GetEnvironmentVariable
#define SETENV SetEnvironmentVariable

wchar_t g_phpPath[MAX_PATH] = L"";

BOOL PrepareServerPath()
{
    // Get the location of PHP
    HMODULE hModule;
    GetModuleHandleEx(0, L"php5ts.dll", &hModule);
    wchar_t g_phpPath[MAX_PATH];
    GetModuleFileName(hModule, g_phpPath, MAX_PATH);
    wcsrchr(g_phpPath, '\\')[0] = '\0';
    FreeLibrary(hModule);

    // Prepend to process PATH environment string
    wchar_t srvPath[MAX_PATH] = L"";
    wcscpy(srvPath, g_phpPath);

    wchar_t ePath[1024*4] = {0};
    wcscpy(ePath,srvPath);
    wcscat(ePath,L";");
    DWORD dw = wcslen(ePath);
    return (GETENV(L"PATH",&ePath[dw],sizeof(ePath)-dw) &&
            SETENV(L"PATH",ePath));
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL,
                       DWORD dwReason, LPVOID)
{
   switch (dwReason) {
     case DLL_PROCESS_ATTACH:
       PrepareServerPath();
       break;
   }
   return TRUE;
}


ZEND_FUNCTION(LoadMgDlls);
/* compiled function list so Zend knows what's in this module */
zend_function_entry CustomExtModule_functions[] = {
   ZEND_FE(LoadMgDlls, NULL)
    {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(CustomExtModule)
{
    zend_printf("The name of the current function is %s<br>", get_active_function_name(TSRMLS_C));
    return SUCCESS;
}

/* compiled module information */
zend_module_entry CustomExtModule_module_entry = {
    STANDARD_MODULE_HEADER,
    "MapGuideApiEnvConfig Module",
    CustomExtModule_functions,
    PHP_MINIT(CustomExtModule), NULL, NULL, NULL, NULL,
    NO_VERSION_YET, STANDARD_MODULE_PROPERTIES
};


/* implement standard "stub" routine to introduce ourselves to Zend */
ZEND_GET_MODULE(CustomExtModule)

/* LoadMgDlls function */
ZEND_FUNCTION(LoadMgDlls){
    long theValue = 0;

    SetDllDirectory(g_phpPath);

    // Load the dependent dlls

#ifdef _DEBUG

    LoadLibrary(L"ACEd.dll");
    LoadLibrary(L"lib_jsond.dll");
    LoadLibrary(L"MgFoundationd.dll");
    LoadLibrary(L"MgGeometryd.dll");
    LoadLibrary(L"MgHttpHandlerd.dll");
    LoadLibrary(L"MgMapGuideCommond.dll");
    LoadLibrary(L"MgMdfModeld.dll");
    LoadLibrary(L"MgMdfParserd.dll");
    LoadLibrary(L"MgPlatformBased.dll");
    LoadLibrary(L"MgWebAppd.dll");
    LoadLibrary(L"xerces-c_2_7d.dll");

#else

    LoadLibrary(L"ACE.dll");
    LoadLibrary(L"lib_json.dll");
    LoadLibrary(L"MgFoundation.dll");
    LoadLibrary(L"MgGeometry.dll");
    LoadLibrary(L"MgHttpHandler.dll");
    LoadLibrary(L"MgMapGuideCommon.dll");
    LoadLibrary(L"MgMdfModel.dll");
    LoadLibrary(L"MgMdfParser.dll");
    LoadLibrary(L"MgPlatformBase.dll");
    LoadLibrary(L"MgWebApp.dll");
    LoadLibrary(L"xerces-c_2_7.dll");

#endif

    RETURN_LONG(theValue);
}

