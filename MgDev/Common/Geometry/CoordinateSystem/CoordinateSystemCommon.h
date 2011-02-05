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

#ifndef _MGCOORDINATESYSTEMCOMMON_H_
#define _MGCOORDINATESYSTEMCOMMON_H_

#include "CoordinateSystem.h"
#include "CoordinateSystemDatum.h"
#include "CoordinateSystemGeodeticTransformation.h"
#include "CoordinateSystemEllipsoid.h"
#include "CoordinateSystemCategory.h"
#include "CoordinateSystemMeasure.h"
#include "CoordinateSystemTransform.h"
#include "CoordinateSystemType.h"
#include "CoordinateSystemDictionaryBase.h"                 //for MgCoordinateSystemDictionaryBase
#include "CoordinateSystemDictionary.h"                     //for MgCoordinateSystemDictionary
#include "CoordinateSystemDatumDictionary.h"                //for MgCoordinateSystemDatumDictionary
#include "CoordinateSystemEllipsoidDictionary.h"            //for MgCoordinateSystemEllipsoidDictionary
#include "CoordinateSystemCategoryDictionary.h"             //for MgCoordinateSystemCategoryDictionary
#include "CoordinateSystemDictionaryUtility.h"              //for MgCoordinateSystemDictionaryUtility
#include "CoordinateSystemCatalog.h"                        //for MgCoordinateSystemCatalog
#include "CoordinateSystemFactory.h"
#include "CoordinateSystemFilter.h"                         //for MgCoordinateSystemFilter
#include "CoordinateSystemEnum.h"                           //for MgCoordinateSystemEnum
#include "CoordinateSystemMathComparator.h"                 //for MgCoordinateSystemMathComparator
#include "CoordinateSystemWktFlavor.h"                      //for MgCoordinateSystemWktFlavor
#include "CoordinateSystemCodeFormat.h"                     //for MgCoordinateSystemCodeFormat
#include "CoordinateSystemFormatConverter.h"                //for MgCoordinateSystemFormatConverter
#include "CoordinateSystemUnitCode.h"                       //for MgCoordinateSystemUnitCode
#include "CoordinateSystemUnitType.h"                       //for MgCoordinateSystemUnitType
#include "CoordinateSystemFilterInteger32.h"                //for MgCoordinateSystemFilterInteger32
#include "CoordinateSystemEnumInteger32.h"                  //for MgCoordinateSystemEnumInteger32
#include "CoordinateSystemUnitInformation.h"                //for MgCoordinateSystemUnitInformation
#include "CoordinateSystemProjectionCode.h"                 //for MgCoordinateSystemProjectionCode
#include "CoordinateSystemProjectionInformation.h"          //for MgCoordinateSystemProjectionInformation
#include "CoordinateSystemGeodeticTransformationMethod.h"   //for MgCoordinateSystemGeodeticTransformationMethod
#include "CoordinateSystemProjectionLogicalType.h"          //for MgCoordinateSystemProjectionLogicalType
#include "CoordinateSystemProjectionFormatType.h"           //for MgCoordinateSystemProjectionFormatType
#include "CoordinateSystemProjectionParameterType.h"        //for MgCoordinateSystemProjectionParameterType
#include "CoordinateSystemErrorCode.h"                      //for MgCoordinateSystemErrorCode
#include "CoordinateSystemMgrsGridSquarePosition.h"         //for MgCoordinateSystemMgrsGridSquarePosition
#include "CoordinateSystemMgrs.h"                           //for MgCoordinateSystemMgrs
#include "CoordinateSystemMgrsLetteringScheme.h"            //for MgCoordinateSystemMgrsLetteringScheme

#ifndef _WIN32
//Linux: different naming for string functions
#define stricmp strcasecmp
#endif

#endif //_MGCOORDINATESYSTEMCOMMON_H_
