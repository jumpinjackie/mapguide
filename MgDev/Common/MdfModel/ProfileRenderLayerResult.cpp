//
//  Copyright (C) 2011 by Autodesk, Inc.
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
// The ProfileRenderLayerResult class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ProfileRenderLayerResult.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the ProfileRenderLayerResult class.
//-------------------------------------------------------------------------
ProfileRenderLayerResult::ProfileRenderLayerResult()
{
    this->m_dRenderTime = 0.0;
    this->m_scaleRange = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
ProfileRenderLayerResult::~ProfileRenderLayerResult()
{
    delete this->m_scaleRange;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ResourceId property.
// RETURNS: The resource identifier of the layer.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderLayerResult::GetResourceId() const
{
    return this->m_strResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the ResourceId property.
// PARAMETERS:
//      Input:
//          pstrResourceId - ResourceId of layer
//-------------------------------------------------------------------------
void ProfileRenderLayerResult::SetResourceId(const MdfString& pstrResourceId)
{
    this->m_strResourceId = pstrResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LayerName property.
// RETURNS: The name of the layer.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderLayerResult::GetLayerName() const
{
    return this->m_strLayerName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the LayerName property.
// PARAMETERS:
//      Input:
//          pstrLayerName - The name of the layer.
//-------------------------------------------------------------------------
void ProfileRenderLayerResult::SetLayerName(const MdfString& pstrLayerName)
{
    this->m_strLayerName = pstrLayerName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LayerType property.
// RETURNS: The type (vector or raster) of the layer.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderLayerResult::GetLayerType() const
{
    return this->m_strLayerType;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the LayerType property.
// PARAMETERS:
//      Input:
//          pstrLayerType - The type (vector or raster) of the layer.
//-------------------------------------------------------------------------
void ProfileRenderLayerResult::SetLayerType(const MdfString& pstrLayerType)
{
    this->m_strLayerType = pstrLayerType;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FeatureClassName property.
// RETURNS: The name of the feature class referenced by the layer.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderLayerResult::GetFeatureClassName() const
{
    return this->m_strFeatureClassName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the FeatureClassName property.
// PARAMETERS:
//      Input:
//          pstrLayerName - The name of the feature class referenced by the layer.
//-------------------------------------------------------------------------
void ProfileRenderLayerResult::SetFeatureClassName(const MdfString& pstrFeatureClassName)
{
    this->m_strFeatureClassName = pstrFeatureClassName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the CoordinateSystem property.
// RETURNS: The coordinate system code used by the layer.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderLayerResult::GetCoordinateSystem() const
{
    return this->m_strCoordinateSystem;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the CoordinateSystem property.
// PARAMETERS:
//      Input:
//          pstrCoordinateSystem - The coordinate system code used by the layer.
//-------------------------------------------------------------------------
void ProfileRenderLayerResult::SetCoordinateSystem(const MdfString& pstrCoordinateSystem)
{
    this->m_strCoordinateSystem = pstrCoordinateSystem;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ScaleRange property.
// RETURNS: The current scale range of the layer in the map.
//-------------------------------------------------------------------------
const ScaleRange* ProfileRenderLayerResult::GetScaleRange() const
{
    return this->m_scaleRange;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ScaleRange property.
// RETURNS: The current scale range of the layer in the map.
//-------------------------------------------------------------------------
ScaleRange* ProfileRenderLayerResult::GetScaleRange()
{
    return this->m_scaleRange;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ScaleRange property.
// PARAMETERS:
//        Input:
//          pScaleRange - The current scale range of the layer in the map.
//-------------------------------------------------------------------------
void ProfileRenderLayerResult::AdoptScaleRange(ScaleRange* pScaleRange)
{
    if (this->m_scaleRange != pScaleRange)
    {
        delete this->m_scaleRange;
        this->m_scaleRange = pScaleRange;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ScaleRange property.
//
// RETURNS: The pointer to the orphaned ScaleRange object. It may be NULL.
//-------------------------------------------------------------------------
ScaleRange* ProfileRenderLayerResult::OrphanScaleRange()
{
    ScaleRange* pRet = this->m_scaleRange;
    this->m_scaleRange = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Filter property.
// RETURNS: A boolean FDO expression that specifies which features to return.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderLayerResult::GetFilter() const
{
    return this->m_strFilter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Filter property.
// PARAMETERS:
//      Input:
//          pstrFilter - A boolean FDO expression that specifies which features to return.
//-------------------------------------------------------------------------
void ProfileRenderLayerResult::SetFilter(const MdfString& pstrFilter)
{
    this->m_strFilter = pstrFilter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the RenderTime property.
// RETURNS: The time of stylizing the layer in the map.
//-------------------------------------------------------------------------
const double ProfileRenderLayerResult::GetRenderTime() const
{
    return this->m_dRenderTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the RenderTime property.
// PARAMETERS:
//      Input:
//          dRenderTime - The time of stylizing the layer in the map.
//-------------------------------------------------------------------------
void ProfileRenderLayerResult::SetRenderTime(const double& dRenderTime)
{
    this->m_dRenderTime = dRenderTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Error property.
// RETURNS: The error message which describes layer rendering failure.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderLayerResult::GetError() const
{
    return this->m_strError;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Error property.
// PARAMETERS:
//      Input:
//          pstrError - The error message which describes layer rendering failure.
//-------------------------------------------------------------------------
void ProfileRenderLayerResult::SetError(const MdfString& pstrError)
{
    this->m_strError = pstrError;
}

//-------------------------------------------------------------------------
// PURPOSE: For memory leak testing.
//-------------------------------------------------------------------------
#ifdef _WIN32
#ifdef _DEBUG
int ProfileRenderLayerResult::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif
