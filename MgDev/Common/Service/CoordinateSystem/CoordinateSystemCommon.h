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

#ifndef _MGCOORDINATESYSTEMCOMMON_H_
#define _MGCOORDINATESYSTEMCOMMON_H_

#include "AceCommon.h"

#include "CoordSysCommon.h"
using namespace CSLibrary;

#include "CoordinateSystem.h"
#include "CoordinateSystemFactory.h"
#include "CoordinateSystemMeasure.h"
#include "CoordinateSystemTransform.h"
#include "CoordinateSystemType.h"

#ifndef _WIN32
//Linux: different naming for string functions
#define stricmp strcasecmp
#endif

#endif //_MGCOORDINATESYSTEMCOMMON_H_
