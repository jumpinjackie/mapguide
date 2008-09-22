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

#ifndef _MG_SERVER_FEATURE_UTIL_H_
#define _MG_SERVER_FEATURE_UTIL_H_

#include "MapGuideCommon.h"
#include "Fdo.h"

#define EQUAL_CATEGORY      0
#define STDEV_CATEGORY      1
#define QUANTILE_CATEGORY   2
#define JENK_CATEGORY       3
#define MINIMUM             4
#define MAXIMUM             5
#define MEAN                6
#define STANDARD_DEV        7
#define UNIQUE              8
#define EXTENT              9

typedef vector <double> VECTOR;
typedef vector <INT64> VECTOR_INT64;

#define DoubleMaxValue  DBL_MAX

#define DoubleNegativeInfinity numeric_limits<double>::infinity()
#define DoublePositiveInfinity numeric_limits<double>::infinity()

static const STRING MG_SCHEMA_NAME_HASH_PREFIX      = L"sn"; // NOXLATE
static const STRING MG_FEATURE_SOURCE_HASH_PREFIX   = L"ns"; // NOXLATE

class MgReader;

class MgServerFeatureUtil
{
public:

    static INT32 GetMgPropertyType(FdoDataType fdoDataType);
    static FdoOrderingOption GetFdoOrderingOption(INT32 orderingOption);
    static STRING GetMessage(CREFSTRING messageId);
    static bool FindCustomFunction(CREFSTRING funcName, INT32 &index);
    static INT16 GetMgPropertyType(MgPropertyDefinition* propDef);
    static MgPropertyDefinitionCollection* GetPropertyDefinitions(MgReader* reader);
    static INT32 GetPropertyDefinition(MgReader* reader, REFSTRING propName);
    static bool FindCustomFunction(FdoFunction* customFunction, INT32 &index);
    static void GetArguments(FdoFunction* customFunction, REFSTRING name, int &numCats,
                             double& dataMin, double& dataMax, INT16 type);

    static void ValidateCustomConstraints(FdoFunction* customFunction);
    static double Minimum(VECTOR& v);
    static INT64 Minimum(VECTOR_INT64& v);
    static double Maximum(VECTOR& v);
    static INT64 Maximum(VECTOR_INT64& v);
    static FdoSpatialOperations GetFdoSpatialOperation(INT32 spatialOp);
    static INT32 GetMgFeaturePropertyType(FdoPropertyType fdoPropType);
    static MgRaster* GetMgRaster(FdoIRaster* raster, STRING propName);
    static MgByteReader* GetRaster(FdoIReader* reader, CREFSTRING rasterPropName,INT32 xSize, INT32 ySize);
    static bool ValidatePoolEntry(INT32 readerId);
    static FdoPropertyValue* MgPropertyToFdoProperty(MgProperty* srcProp);
    static FdoParameterValue* MgPropertyToFdoParameter(MgProperty* srcProp);
    static FdoLiteralValue* MgPropertyToFdoDataValue(MgProperty* srcProp);
    static FdoParameterValueCollection* CreateFdoParameterCollection(MgPropertyCollection* srcCol);
    static void FillFdoPropertyCollection(MgPropertyCollection* srcCol, FdoPropertyValueCollection* paramCol);
    static INT32 FdoOrderTypeToMgOrderingOption(FdoOrderType type);
    static FdoOrderType MgOrderingOptionToFdoOrderType(INT32 type);
    static INT32 FdoObjectTypeToMgObjectPropertyType(FdoObjectType type);
    static FdoObjectType MgObjectPropertyTypeToFdoObjectType(INT32 type);

    static MgStringCollection* FdoToMgStringCollection(FdoStringCollection* fdoStrs, bool includeEmptyStrings);
    static FdoStringCollection* MgToFdoStringCollection(MgStringCollection* mgStrs, bool includeEmptyStrings);

private:
    static bool Initialize();

private:
    static bool m_isInitialized;
};

#endif
