//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#ifdef WIN32
#include <basetsd.h>
#endif

//This define allows to use the same name for both WIndows and Linux to compare strings case insensitive
#ifndef WIN32
#define _wcsnicmp wcsncasecmp
#endif

#include <string>
using namespace std;

#define EXTERNAL_API public // API available through SWIG
#define INTERNAL_API public // API only available in C++
#define CLASS_ID private // Class ID made available to SWIG's interface generator

// Static link to MgCommonLib
#define MG_SERVICE_API

#ifdef WIN32
#define APICALL __stdcall
#else
#define APICALL
#endif

#define DECLARE_CREATE_OBJECT() \
public: \
    static SqliteDisposable* CreateObject();

#define IMPLEMENT_CREATE_OBJECT(class_name) \
    SqliteDisposable* class_name::CreateObject() \
        { return new class_name(); }


#define DECLARE_DYNCREATE_COREDATA() \
public: \
    static SqliteDisposable* CreateObject();

#define IMPLEMENT_DYNCREATE(class_name) \
    SqliteDisposable* class_name::CreateObject() \
        { return new class_name(); }

// Wide character version of __FILE__ macro
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)



#ifndef WIN32
//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 8 bit integer
/// </summary>
typedef char INT8;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 8 bit unsigned integer
/// </summary>
typedef unsigned char UINT8;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 16 bit integer
/// </summary>
typedef short INT16;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 16 bit unsigned integer
/// </summary>
typedef unsigned short UINT16;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 32 bit signed integer
/// </summary>
typedef long INT32;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 32 bit unsigned integer
/// </summary>
typedef unsigned long UINT32;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 64 bit integer
/// </summary>
typedef long long INT64;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 64 bit unsigned integer
/// </summary>
typedef unsigned long long UINT64;
#endif

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// String.
/// </summary>
typedef wstring STRING;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Reference String.
/// </summary>
typedef wstring& REFSTRING;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// API string.
/// </summary>
typedef const wstring& CREFSTRING;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Byte
/// </summary>
typedef unsigned char BYTE;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Byte array.
/// </summary>
typedef unsigned char* BYTE_ARRAY_IN;

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Byte array.
/// </summary>
typedef unsigned char* BYTE_ARRAY_OUT;

#ifdef _WIN32
#define UNUSED(var)
#define UNUSED_BOOL(var)
#define UNUSED_ENUM(var, type)
#define UNUSED_STR(var)
#else
#define UNUSED(var)  (var=0)
#define UNUSED_BOOL(var)  (var=false)
#define UNUSED_ENUM(var, type)  (var=(type)0)
#define UNUSED_STR(var)  (var.length())
#endif

#include "ClassId.h"

#endif // COMMON_DEFS_H
