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

#ifndef RENDERERS_H_
#define RENDERERS_H_

//OS specific DLL options
#ifdef WIN32
#ifdef RENDERERS_EXPORTS
#define RENDERERS_API __declspec(dllexport)
#else
#define RENDERERS_API __declspec(dllimport)
#endif
#else
#define RENDERERS_API
#endif

//std headers
#include <wchar.h>
#include <string.h>

#define MAP_PRODUCT_AUTHOR  L"Autodesk"
#define MAP_PRODUCT_NAME    L"MapGuide Open Source"
#define MAP_PRODUCT_VERSION L"1.0.0"

#endif
