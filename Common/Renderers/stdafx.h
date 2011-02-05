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

// OS specific includes
#ifdef _WIN32

// Exclude rarely-used stuff from Windows headers.
#define WIN32_LEAN_AND_MEAN

// Windows Header Files:
#include <windows.h>

// for memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define _wcstok(token, delimit, state)  wcstok_s(token, delimit, state)
#else
#define _wcstok                         wcstok

#define _ASSERT(x)
typedef unsigned long DWORD;
#define _wcsnicmp wcsncasecmp
#define _wcsicmp wcscasecmp
#define _isnan isnan
#define _finite finite

#endif //_WIN32

#include <float.h>
#define _USE_MATH_DEFINES //for PI, etc
#include <math.h>
#include <limits>

