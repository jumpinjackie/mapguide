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

#include "CoordSysMacro.h"

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysGeodeticTransformDefParams.h"
#include "CoordSysGeodeticStandaloneTransformDefParams.h"

#include <cs_map.h>

using namespace CSLibrary;

CCoordinateSystemGeodeticStandaloneTransformDefParams::CCoordinateSystemGeodeticStandaloneTransformDefParams(
    INT32 standaloneMethodCode, bool isProtected)
    : CCoordinateSystemGeodeticTransformDefParams(isProtected)
    , m_nStandaloneMethodCode(standaloneMethodCode)
{
}

CCoordinateSystemGeodeticStandaloneTransformDefParams::~CCoordinateSystemGeodeticStandaloneTransformDefParams()
{
}

void CCoordinateSystemGeodeticStandaloneTransformDefParams::Dispose()
{
    delete this;
}

bool CCoordinateSystemGeodeticStandaloneTransformDefParams::IsValid()
{
    return true;
}

bool CCoordinateSystemGeodeticStandaloneTransformDefParams::IsProtected()
{
    return CCoordinateSystemGeodeticTransformDefParams::IsProtected();
}

void CCoordinateSystemGeodeticStandaloneTransformDefParams::CopyTo(void* target) const
{
    ENSURE_NOT_NULL(target, L"CCoordinateSystemGeodeticStandaloneTransformDefParams.CopyTo");
    
    //wipe out the information - we don't have any parameters to set
    memset(target, 0, sizeof(cs_GeodeticTransform_::csGeodeticXformParameters::csGeodeticXfromParmsSize_));
}

INT32 CCoordinateSystemGeodeticStandaloneTransformDefParams::GetTransformationMethod()
{
    return this->m_nStandaloneMethodCode;
}

void CCoordinateSystemGeodeticStandaloneTransformDefParams::SetTransformationMethod(INT32 standaloneMethodCode)
{
    VERIFY_NOT_PROTECTED(L"CCoordinateSystemGeodeticStandaloneTransformDefParamsSetTransformationMethod");

    switch(standaloneMethodCode)
    {
    case MgCoordinateSystemGeodeticStandaloneTransformationMethod::NullX:
    case MgCoordinateSystemGeodeticStandaloneTransformationMethod::Wgs72:
        break;

    default:
        throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticStandaloneDefParams.SetTransformationMethod", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    this->m_nStandaloneMethodCode = standaloneMethodCode;
}
