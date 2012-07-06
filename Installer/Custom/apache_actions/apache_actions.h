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

#pragma once

#define UNICODE 
#define INITGUID 

#define STATUS_STRING_SIZE 2048
const int MAX_BUFFER = 1024;

#include "MSIQuery.h"

__declspec(dllexport) UINT __stdcall GetApacheDir(MSIHANDLE theMSIHandle);
__declspec(dllexport) UINT __stdcall UpdatePhpIni(MSIHANDLE theMSIHandle);
__declspec(dllexport) UINT __stdcall UpdateApacheConfig(MSIHANDLE theMSIHandle);

bool FileExists (const char* szFile);
