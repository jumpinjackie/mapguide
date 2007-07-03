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

#ifndef _CCOORDINATESYSTEMCOMMON_H_
#define _CCOORDINATESYSTEMCOMMON_H_

// Common
#ifdef _WIN32
#define COORDINATE_SYSTEM_API __declspec(dllexport)
#else
#define COORDINATE_SYSTEM_API
#endif

#include <limits>
#include <map>
#include <string>
#include <vector>
using namespace std;

typedef wstring STRING;
typedef wstring& REFSTRING;
typedef const wstring& CREFSTRING;
typedef vector<STRING> StringVector;

// Wide character version of __FILE__ macro
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#ifndef _WIN32
//Linux: different naming for string functions
#define _stricmp strcasecmp
#define _wcsicmp  wcscasecmp
#endif

#endif //_CCOORDINATESYSTEMCOMMON_H_
