//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "php.h"
#include "ext/standard/info.h"

#pragma warning(disable: 4996)


#define GETENV GetEnvironmentVariable
#define SETENV SetEnvironmentVariable

BOOL PrepareServerPath()
{
    wchar_t phpPath[MAX_PATH] = L"";

    // Get the location of PHP
    HMODULE hModule;
    hModule = GetModuleHandle(L"php5ts.dll");
    GetModuleFileName(hModule, phpPath, MAX_PATH);
    wcsrchr(phpPath, '\\')[0] = '\0';
//    FreeLibrary(hModule);  -- DO NOT CALL FreeLibrary() when using GetModuleHandle() as the reference count is not increased

    // Prepend to process PATH environment string
    wchar_t ePath[1024*4] = L"";
    wcscpy(ePath, phpPath);
    wcscat(ePath, L";");
    DWORD dw = (DWORD)wcslen(ePath);
    return (GETENV(L"PATH", &ePath[dw], sizeof(ePath)-dw) &&
            SETENV(L"PATH", ePath));
}


BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            PrepareServerPath();
            break;
    }
    return TRUE;
}


/* compiled function list so Zend knows what's in this module */
zend_function_entry MapGuideApiEnvConfig_functions[] = {
    {NULL, NULL, NULL}
};


PHP_MINIT_FUNCTION(MapGuideApiEnvConfig)
{
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(MapGuideApiEnvConfig)
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(MapGuideApiEnvConfig)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "MapGuideApiEnvConfig", "enabled");
    php_info_print_table_end();
}

/* compiled module information */
zend_module_entry MapGuideApiEnvConfig_module_entry = {
    STANDARD_MODULE_HEADER,
    "MapGuideApiEnvConfig",
    MapGuideApiEnvConfig_functions,
    PHP_MINIT(MapGuideApiEnvConfig),
    PHP_MSHUTDOWN(MapGuideApiEnvConfig),
    NULL,
    NULL,
    PHP_MINFO(MapGuideApiEnvConfig),
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};


/* implement standard "stub" routine to introduce ourselves to Zend */
ZEND_GET_MODULE(MapGuideApiEnvConfig)
