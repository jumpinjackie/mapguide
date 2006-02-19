//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef MGCOMMONDEFS_H_
#define MGCOMMONDEFS_H_

/// \ingroup NOT_EXPOSED_module

#ifdef WIN32
#include <basetsd.h>
#endif

// This allows to use the same function name for both Windows and Linux.

#ifdef WIN32
    #define _wcstok(token, delimit, state)      wcstok(token, delimit)
#else
    #define _wcstok                             wcstok

    #define _wcsicmp  wcscasecmp
    #define _wcsnicmp wcsncasecmp

    #define _stricmp  strcasecmp
    #define _strnicmp strncasecmp
#endif

#include <string>
#include <vector>
using namespace std;

#define PUBLISHED_API public // Published API available through SWIG
#define EXTERNAL_API public // Unpublished API available through SWIG (infrastructure and admin)
#define INTERNAL_API public // API only available in C++
#define CLASS_ID private // Class ID made available to SWIG's interface generator

// Common
#ifdef WIN32
#ifdef MG_CORE_EXPORTS
#    define MG_SERVICE_API __declspec(dllexport)
#else
#    define MG_SERVICE_API __declspec(dllimport)
#endif
#else
#define MG_SERVICE_API
#endif

#ifdef WIN32
#define APICALL __stdcall
#else
#define APICALL
#endif

#define DECLARE_CREATE_OBJECT() \
public: \
    static MgObject* CreateObject();

#define IMPLEMENT_CREATE_OBJECT(class_name) \
    MgObject* class_name::CreateObject() \
        { return new class_name(); }


#define DECLARE_DYNCREATE_COREDATA() \
public: \
    static MgObject* CreateObject();

#define IMPLEMENT_DYNCREATE(class_name) \
    MgObject* class_name::CreateObject() \
        { return new class_name(); }

// Wide character version of __FILE__ macro
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#ifdef WIN32
    #ifdef GetClassName
        #undef GetClassName
    #endif
#endif

#define DECLARE_CLASSNAME(className) \
public: \
    virtual STRING GetClassName() { return WIDEN(#className) ; } \

#ifndef WIN32
//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 8 bit integer
typedef char INT8;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 8 bit unsigned integer
typedef unsigned char UINT8;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 16 bit integer
typedef short INT16;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 16 bit unsigned integer
typedef unsigned short UINT16;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 32 bit integer
typedef long INT32;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 32 bit unsigned integer
typedef unsigned long UINT32;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 64 bit integer
typedef long long INT64;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 64 bit unsigned integer
typedef unsigned long long UINT64;
#endif

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// String.
typedef wstring STRING;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// Reference String.
typedef wstring& REFSTRING;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// API string.
typedef const wstring& CREFSTRING;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// Byte
typedef unsigned char BYTE;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// Byte array.
typedef unsigned char* BYTE_ARRAY_IN;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// Byte array.
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

#endif // MGCOMMONDEFS_H_
