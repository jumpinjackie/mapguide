//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef MGSECURITYDEFS_H_
#define MGSECURITYDEFS_H_

#include <assert.h>
#include <string>
#include <time.h>
#include <math.h>
#include <sys/stat.h>

#ifdef _WIN32

    #ifndef STDAFX_H
    #define STDAFX_H

    #define _WIN32_WINNT 0x0400
    #define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers

    // Windows Header Files:
    #include <windows.h>

    #endif

#else

    // This allows to use the same function name for both Windows and Linux.
    #define _stat stat

#endif

using namespace std;

enum MgErrorCode
{
    ecOk                =  0,
/// Common errors.
    ecInvalidOperation      ,
    ecNullArgument          ,
    ecInvalidArgument       ,
    ecLengthError           ,
/// Date Time errors.
    ecDateTimeError         ,
/// File errors.
    ecFileIoError           ,
    ecReadError             ,
    ecWriteError            ,
/// Cryptography errors.
    ecEncryptionError       ,
    ecDecryptionError       ,
/// License errors.
    ecInvalidLicense        ,
    ecLicenseExpired        ,
    ecEvaluationExpired     ,
    ecInvalidSerialNumber   ,
/// Unknown errors.
    ecUnclassified      = 99
};

enum MgReleaseType
{
    rtProduction        =  0,
    rtAlpha             =  1,
    rtBeta              =  2,
    rtPreview           =  3
};

#endif
