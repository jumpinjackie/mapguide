//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef STYLIZATIONDEFS_H_
#define STYLIZATIONDEFS_H_

// std headers
#include <wchar.h>
#include <string>
#include <cstring>
#include <memory>

#include <float.h>
#define _USE_MATH_DEFINES //for PI, etc
#include <math.h>
#include <limits>
#include <climits>

const double M_PI180 = (M_PI / 180.0);
const double M_180PI = (180.0 / M_PI);
const double METERS_PER_INCH = 0.0254;
const double MILLIMETERS_PER_INCH = 25.4;
const double STANDARD_DISPLAY_DPI = 96.0;

#define SE_INLINE inline

#ifdef _WIN32
#define SE_NOVTABLE __declspec(novtable)
#else
#define SE_NOVTABLE
#define _ASSERT(x)
#endif

#endif
