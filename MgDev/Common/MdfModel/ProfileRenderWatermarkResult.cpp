//
//  Copyright (C) 2010-2011 by Autodesk, Inc.
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

//-------------------------------------------------------------------------
// DESCRIPTION:
// The ProfileRenderWatermarkResult class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ProfileRenderWatermarkResult.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the ProfileRenderWatermarkResult class.
//-------------------------------------------------------------------------
ProfileRenderWatermarkResult::ProfileRenderWatermarkResult()
{
    this->m_dRenderTime = 0.0;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
ProfileRenderWatermarkResult::~ProfileRenderWatermarkResult()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ResourceId property.
// RETURNS: The resource identifier of the watermark.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderWatermarkResult::GetResourceId() const
{
    return this->m_strResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the ResourceId property.
// PARAMETERS:
//      Input:
//          pstrResourceId - ResourceId of watermark
//-------------------------------------------------------------------------
void ProfileRenderWatermarkResult::SetResourceId(const MdfString& pstrResourceId)
{
    this->m_strResourceId = pstrResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the PositionType property.
// RETURNS: The position type (XY or Tile) of the watermark.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderWatermarkResult::GetPositionType() const
{
    return this->m_strPositionType;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Position property.
// PARAMETERS:
//      Input:
//          pstrPositionType - PositionType of watermark
//-------------------------------------------------------------------------
void ProfileRenderWatermarkResult::SetPositionType(const MdfString& pstrPositionType)
{
    this->m_strPositionType = pstrPositionType;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the RenderTime property.
// RETURNS: The time of stylizing a paricular watermark in the map.
//-------------------------------------------------------------------------
const double ProfileRenderWatermarkResult::GetRenderTime() const
{
    return this->m_dRenderTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the RenderTime property.
// PARAMETERS:
//      Input:
//          dRenderTime - RenderTime of the watermark
//-------------------------------------------------------------------------
void ProfileRenderWatermarkResult::SetRenderTime(const double& dRenderTime)
{
    this->m_dRenderTime = dRenderTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Error property.
// RETURNS: The error message which describes watermark rendering failure.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderWatermarkResult::GetError() const
{
    return this->m_strError;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Error property.
// PARAMETERS:
//      Input:
//          pstrError - The error message which describes watermark rendering failure.
//-------------------------------------------------------------------------
void ProfileRenderWatermarkResult::SetError(const MdfString& pstrError)
{
    this->m_strError = pstrError;
}

//-------------------------------------------------------------------------
// PURPOSE: For memory leak testing.
//-------------------------------------------------------------------------
#ifdef _WIN32
#ifdef _DEBUG
int ProfileRenderWatermarkResult::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif
