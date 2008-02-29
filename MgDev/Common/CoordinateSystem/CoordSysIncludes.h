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

#ifndef _CCOORDINATESYSTEMINCLUDES_H_
#define _CCOORDINATESYSTEMINCLUDES_H_

#include "ogr_spatialref.h"
#include "proj_api.h"

#ifdef _WIN32
// To undefine GetMessage macro defined in windows
#undef GetMessage
#endif

#include "CoordSys.h"
#include "CoordSysCatalog.h"
#include "CoordSysCategory.h"
#include "CoordSysDimension.h"
#include "CoordSysEnvelope.h"
#include "CoordSysFactory.h"
#include "CoordSysInformation.h"
#include "CoordSysMeasure.h"
#include "CoordSysTransform.h"
#include "CoordSysTransformation.h"
#include "CoordSysType.h"
using namespace CSLibrary;

// Exceptions
#include "CoordSysException.h"
#include "CoordSysComputationFailedException.h"
#include "CoordSysConversionFailedException.h"
#include "CoordSysInitializationFailedException.h"
#include "CoordSysInvalidArgumentException.h"
#include "CoordSysInvalidCoordinateSystemException.h"
#include "CoordSysInvalidCoordinateSystemTypeException.h"
#include "CoordSysInvalidCoordinateSystemUnitsException.h"
#include "CoordSysMeasureFailedException.h"
#include "CoordSysNullArgumentException.h"
#include "CoordSysNullReferenceException.h"
#include "CoordSysOutOfMemoryException.h"
#include "CoordSysTransformFailedException.h"

#endif //_CCOORDINATESYSTEMINCLUDES_H_
