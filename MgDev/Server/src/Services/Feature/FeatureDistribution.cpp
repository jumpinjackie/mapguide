//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "ServerFeatureServiceDefs.h"
#include "FeatureDistribution.h"
#include "ServerFeatureUtil.h"
#include "math.h"
#include "float.h"
#include <algorithm>
#include "Matrix.h"
#include "UniqueFunction.h"
#include "DataReaderCreator.h"
#include "DoubleDataReaderCreator.h"
#include "StringDataReaderCreator.h"
#include "FeatureNumericFunctions.h"
#include "FeatureGeometricFunctions.h"
#include "FeatureStringFunctions.h"

MgFeatureDistribution::MgFeatureDistribution()
{
}

MgFeatureDistribution::~MgFeatureDistribution()
{
}

MgFeatureDistribution* MgFeatureDistribution::CreateDistributionFunction(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias)
{
    STRING propName;

    Ptr<MgFeatureDistribution> featDist;

    INT32 propType = MgServerFeatureUtil::GetPropertyDefinition(reader, propName);

    switch(propType)
    {
        case MgPropertyType::DateTime:
        case MgPropertyType::Double:
        case MgPropertyType::Byte:
        case MgPropertyType::Int16:
        case MgPropertyType::Int32:
        case MgPropertyType::Int64:
        case MgPropertyType::Single:
        case MgPropertyType::Boolean:
        {
            featDist = new MgFeatureNumericFunctions(reader, customFunction, propertyAlias);
            break;
        }
        case MgPropertyType::String:
        {
            featDist = new MgFeatureStringFunctions(reader, customFunction, propertyAlias);
            break;
        }
        case MgPropertyType::Geometry:
        {
            featDist = new MgFeatureGeometricFunctions(reader, customFunction, propertyAlias);
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(
                L"MgFeatureDistribution.CreateDistributionFunction", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    return featDist.Detach();
}
