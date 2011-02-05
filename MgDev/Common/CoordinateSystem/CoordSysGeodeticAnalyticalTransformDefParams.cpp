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

#define CS_MAP_DEF_VARIABLE this->geocentricTransformParams

#include "CoordSysMacro.h"

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysGeodeticTransformDefParams.h"
#include "CoordSysGeodeticAnalyticalTransformDefParams.h"

#include <cs_map.h>

using namespace CSLibrary;

CCoordinateSystemGeodeticAnalyticalTransformDefParams::CCoordinateSystemGeodeticAnalyticalTransformDefParams(
    const csGeocentricXformParams& params, INT32 analyticalTransformMethod, bool isProtected)
    : CCoordinateSystemGeodeticTransformDefParams(isProtected)
    , geocentricTransformParams(NULL), transformationMethod(analyticalTransformMethod)
{
    this->geocentricTransformParams = (csGeocentricXformParams*) CS_malc(sizeof(csGeocentricXformParams));
    if (NULL == this->geocentricTransformParams)
        throw new MgOutOfMemoryException(L"CCoordinateSystemGeodeticAnalyticalTransformDefParams.Ctor", __LINE__, __WFILE__, NULL, L"", NULL);

    *this->geocentricTransformParams = params;
}

CCoordinateSystemGeodeticAnalyticalTransformDefParams::~CCoordinateSystemGeodeticAnalyticalTransformDefParams()
{
    this->ReleaseInstance();
}

void CCoordinateSystemGeodeticAnalyticalTransformDefParams::Dispose()
{
    delete this;
}

void CCoordinateSystemGeodeticAnalyticalTransformDefParams::ReleaseInstance()
{
    if (NULL != this->geocentricTransformParams)
    {
        CS_free(this->geocentricTransformParams);
        this->geocentricTransformParams = NULL;
    }
}

void CCoordinateSystemGeodeticAnalyticalTransformDefParams::CopyTo(void* target) const
{
    ENSURE_NOT_NULL(target, L"CCoordinateSystemGeodeticAnalyticalTransformDefParams.CopyTo");

    memcpy(target, this->geocentricTransformParams, sizeof(cs_GeodeticTransform_::csGeodeticXformParameters::csGeodeticXformParmsGeocentric));
}

bool CCoordinateSystemGeodeticAnalyticalTransformDefParams::IsValid()
{
    //we're relying on the CS_gxchk() method that will be invoked, before the parent transformation
    //definition is finally passed to CS_Map
    return true;
}

bool CCoordinateSystemGeodeticAnalyticalTransformDefParams::IsProtected()
{
    return CCoordinateSystemGeodeticTransformDefParams::IsProtected();
}

INT32 CCoordinateSystemGeodeticAnalyticalTransformDefParams::GetTransformationMethod()
{
    return this->transformationMethod;
}

void CCoordinateSystemGeodeticAnalyticalTransformDefParams::SetTransformationMethod(INT32 analyticalMethodCode)
{
    VERIFY_NOT_PROTECTED(L"CCoordinateSystemGeodeticAnalyticalTransformDefParams::SetTransformationMethod");

    switch(analyticalMethodCode)
    {
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::None:
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::ThreeParameter:
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::Molodensky:
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::AbridgedMolodensky:
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::Geocentric:
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::FourParameter:
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::SixParameter:
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::Bursa:
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::Frame:
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::SevenParameter:
    case MgCoordinateSystemGeodeticAnalyticalTransformationMethod::MolodenskyBadekas:
        break;

    default:
        throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticAnalyticalTransformDefParams.SetTransformationMethod", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    this->transformationMethod = analyticalMethodCode;
}

DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticAnalyticalTransformDefParams,DeltaX,double,this->geocentricTransformParams->deltaX)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticAnalyticalTransformDefParams,DeltaY,double,this->geocentricTransformParams->deltaY)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticAnalyticalTransformDefParams,DeltaZ,double,this->geocentricTransformParams->deltaZ)

DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticAnalyticalTransformDefParams,RotateX,double,this->geocentricTransformParams->rotateX)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticAnalyticalTransformDefParams,RotateY,double,this->geocentricTransformParams->rotateY)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticAnalyticalTransformDefParams,RotateZ,double,this->geocentricTransformParams->rotateZ)

DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticAnalyticalTransformDefParams,Scale,double,this->geocentricTransformParams->scale)

DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticAnalyticalTransformDefParams,TranslateX,double,this->geocentricTransformParams->translateX)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticAnalyticalTransformDefParams,TranslateY,double,this->geocentricTransformParams->translateY)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticAnalyticalTransformDefParams,TranslateZ,double,this->geocentricTransformParams->translateZ)
