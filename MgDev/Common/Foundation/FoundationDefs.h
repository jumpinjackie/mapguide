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

/// INTERNAL USE ONLY

#ifndef MGFOUNDATIONDEFS_H_
#define MGFOUNDATIONDEFS_H_

#ifdef _WIN32
#include <basetsd.h>
#else
#include <stdint.h>
#endif

// This allows to use the same function name for both Windows and Linux.

#ifdef _WIN32
    #define _wcstok(token, delimit, state)      wcstok_s(token, delimit, state)
#else
    #define _wcstok                             wcstok

    #define _wcsicmp  wcscasecmp
    #define _wcsnicmp wcsncasecmp

    #define _stricmp  strcasecmp
    #define _strnicmp strncasecmp

    #define _isnan    isnan
    #define _finite   finite
#endif

#include <string>
#include <vector>
using namespace std;

#define PUBLISHED_API public // Published API available through SWIG
#define EXTERNAL_API public // Unpublished API available through SWIG (infrastructure and admin)
#define INTERNAL_API public // API only available in C++
#define CLASS_ID private // Class ID made available to SWIG's interface generator

// Foundation exports
#ifdef _WIN32
#ifdef FOUNDATION_EXPORTS
#    define MG_FOUNDATION_API __declspec(dllexport)
#else
#    define MG_FOUNDATION_API __declspec(dllimport)
#endif
#else
#define MG_FOUNDATION_API
#endif

#ifdef _WIN32
#define APICALL __stdcall
#else
#define APICALL
#endif

#define MG_DECL_DYNCREATE() \
public: \
    static MgObject* CreateObject();

#define MG_IMPL_DYNCREATE(class_name) \
    MgObject* class_name::CreateObject() \
        { return new class_name(); }

// Wide character version of __FILE__ macro
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#ifdef _WIN32
    #ifdef GetClassName
        #undef GetClassName
    #endif
#endif

#define DECLARE_CLASSNAME(className) \
public: \
    virtual STRING GetClassName() { return WIDEN(#className) ; } \
    virtual char* GetMultiByteClassName() { return #className ; } \

#ifndef _WIN32
//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 8 bit integer
typedef int8_t INT8;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 8 bit unsigned integer
typedef uint8_t UINT8;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 16 bit integer
typedef int16_t INT16;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 16 bit unsigned integer
typedef uint16_t UINT16;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 32 bit integer
typedef int32_t INT32;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 32 bit unsigned integer
typedef uint32_t UINT32;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 64 bit integer
typedef int64_t INT64;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// 64 bit unsigned integer
typedef uint64_t UINT64;
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

// Keep compilers quiet about unused parameters.
#define MG_UNUSED_ARG(a) static_cast<void>(&(a))

#endif // MGFOUNDATIONDEFS_H_
