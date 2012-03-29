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

#define CS_MAP_DEF_VARIABLE this->mulRegParams

#include "CoordSysMacro.h"

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysUtil.h"
#include "MentorUtil.h"

#include "CoordSysGeodeticTransformGridFile.h"
#include "CoordSysGeodeticTransformDefParams.h"
#include "CoordSysGeodeticMultipleRegressionTransformDefParams.h"

#include <cs_map.h>

using namespace CSLibrary;

CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::CCoordinateSystemGeodeticMultipleRegressionTransformDefParams(
    const csMultipleRegressionXformParams& params, INT32 mulRegTransformMethod, bool isProtected)
    : CCoordinateSystemGeodeticTransformDefParams(isProtected)
    , mulRegParams(NULL), transformationMethod(mulRegTransformMethod)
{
    this->Reset();
    *this->mulRegParams = params;
}

CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::~CCoordinateSystemGeodeticMultipleRegressionTransformDefParams()
{
    this->ReleaseInstance();
}

void CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::Dispose()
{
    delete this;
}

void CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::Reset()
{
    this->ReleaseInstance();

    this->mulRegParams = (csMultipleRegressionXformParams*) CS_malc(sizeof(csMultipleRegressionXformParams));
    if (NULL == this->mulRegParams)
        throw new MgOutOfMemoryException(L"CCoordinateSystemGeodeticMultipleRegressionTransformDefParams.Reset", __LINE__, __WFILE__, NULL, L"", NULL);

    memset(this->mulRegParams, 0, sizeof(csMultipleRegressionXformParams));
}

void CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::ReleaseInstance()
{
    if (NULL != this->mulRegParams)
    {
        CS_free(this->mulRegParams);
        this->mulRegParams = NULL;
    }
}

void CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::CopyTo(void* target) const
{
    ENSURE_NOT_NULL(target, L"CCoordinateSystemGeodeticMultipleRegressionTransformDefParams.CopyTo");

    memcpy(target, this->mulRegParams, sizeof(csMultipleRegressionXformParams));
}

bool CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::IsValid()
{
    //we're relying on the CS_gxchk() method that will be invoked, before the parent transformation
    //definition is finally passed to CS_Map
    return true;
}

bool CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::IsProtected()
{
    return CCoordinateSystemGeodeticTransformDefParams::IsProtected();
}

INT32 CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::GetTransformationMethod()
{
    return this->transformationMethod;
}

void CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::SetTransformationMethod(INT32 mulRegTransformationMethod)
{
    VERIFY_NOT_PROTECTED(L"CCoordinateSystemGeodeticMultipleRegressionTransformDefParams::SetTransformationMethod");

    switch(mulRegTransformationMethod)
    {
    case MgCoordinateSystemGeodeticMultipleRegressionTransformationMethod::None:
    case MgCoordinateSystemGeodeticMultipleRegressionTransformationMethod::GeneralPolynomialEpsg:
    case MgCoordinateSystemGeodeticMultipleRegressionTransformationMethod::MultipleRegression:
        break;

    default:
        throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticMultipleRegressionTransformDefParams.SetTransformationMethod", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    this->transformationMethod = mulRegTransformationMethod;
}

//DEFINE_GET_SET_NUMERIC_IDX(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,PhiBitmap,UINT32,this->mulRegParams->phiBitMap,3)
//DEFINE_GET_SET_NUMERIC_IDX(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,LambdaBitmap,UINT32,this->mulRegParams->lambdaBitMap,3)
//DEFINE_GET_SET_NUMERIC_IDX(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,HeightBitmap,UINT32,this->mulRegParams->heightBitMap,3)

DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,Validation,double,this->mulRegParams->validation)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,TestPhi,double,this->mulRegParams->testPhi)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,TestLambda,double,this->mulRegParams->testLambda)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,DeltaPhi,double,this->mulRegParams->deltaPhi)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,DeltaLambda,double,this->mulRegParams->deltaLambda)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,DeltaHeight,double,this->mulRegParams->deltaHeight)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,PhiOffset,double,this->mulRegParams->phiOffset)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,LambdaOffset,double,this->mulRegParams->lambdaOffset)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,NormalizationScale,double,this->mulRegParams->normalizationScale)

DEFINE_GET_SET_NUMERIC_IDX(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,CoefficientPhi,double,this->mulRegParams->coeffPhi,104)
DEFINE_GET_SET_NUMERIC_IDX(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,CoefficientLambda,double,this->mulRegParams->coeffLambda,104)
DEFINE_GET_SET_NUMERIC_IDX(CCoordinateSystemGeodeticMultipleRegressionTransformDefParams,CoefficientHeight,double,this->mulRegParams->coeffHeight,104)
