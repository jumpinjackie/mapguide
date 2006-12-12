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

#ifdef WIN32
#pragma once
#endif

#if defined (_MSC_VER)
// Disable deprecation and non-conforming warning introduced by VC8
# define _CRT_SECURE_NO_DEPRECATE 1
# define _CRT_NON_CONFORMING_SWPRINTFS 1
# pragma warning (disable:4996)
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#include <windows.h>
#include <limits.h>
#else
#define _ASSERT(x)
#endif

//#include "io.h"
#include "assert.h"

#include <Fdo.h>
#include <./Common/Xml/Xml.h>
#include <./Common/Xml/Writer.h>
#include <./Common/Xml/Reader.h>
