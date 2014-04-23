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
// The ProfileRenderMapResult class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ProfileRenderMapResult.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the ProfileRenderMapResult class.
//-------------------------------------------------------------------------
ProfileRenderMapResult::ProfileRenderMapResult()
: m_boxExtents(0.0, 0.0)
, m_dScale(0.0)
, m_dRenderTime(0.0)
, m_dCreateImageTime(0.0)
{
    this->m_profileRenderLayersResult = NULL;
    this->m_profileRenderSelectionResult = NULL;
    this->m_profileRenderWatermarksResult = NULL;
    this->m_profileRenderLabelsResult = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
ProfileRenderMapResult::~ProfileRenderMapResult()
{
    delete this->m_profileRenderLayersResult;
    delete this->m_profileRenderSelectionResult;
    delete this->m_profileRenderWatermarksResult;
    delete this->m_profileRenderLabelsResult;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ResourceId property.
// RETURNS: The resource identifier of the map.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderMapResult::GetResourceId() const
{
    return this->m_strResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the ResourceId property.
// PARAMETERS:
//      Input:
//          pstrResourceId - ResourceId of map
//-------------------------------------------------------------------------
void ProfileRenderMapResult::SetResourceId(const MdfString& pstrResourceId)
{
    this->m_strResourceId = pstrResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the CoordinateSystem property.
// RETURNS: The coordinate system code used by the map.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderMapResult::GetCoordinateSystem() const
{
    return this->m_strCoordinateSystem;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the CoordinateSystem property.
// PARAMETERS:
//      Input:
//          pstrCoordinateSystem - The coordinate system code used by the map.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::SetCoordinateSystem(const MdfString& pstrCoordinateSystem)
{
    this->m_strCoordinateSystem = pstrCoordinateSystem;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the extents property.
// RETURNS: A bounding box around the area of the map.
//-------------------------------------------------------------------------
const Box2D& ProfileRenderMapResult::GetExtents() const
{
    return this->m_boxExtents;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the extents property.
// PARAMETERS:
//     Input:
//         boxExtents - A bounding box around the area of the map.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::SetExtents(const Box2D& boxExtents)
{
    this->m_boxExtents = boxExtents;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Scale property.
// RETURNS: The initial display scale of the map.
//-------------------------------------------------------------------------
const double ProfileRenderMapResult::GetScale() const
{
    return this->m_dScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Scale property.
// PARAMETERS:
//      Input:
//          pScale- The initial display scale of the map.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::SetScale(const double& dScale)
{
    this->m_dScale = dScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LayerCount property.
// RETURNS: The number of layers in the map.
//-------------------------------------------------------------------------
const int ProfileRenderMapResult::GetLayerCount() const
{
    return this->m_iLayerCount;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the LayerCount property.
// PARAMETERS:
//      Input:
//          pScale- The number of layers in the map.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::SetLayerCount(const int& iLayerCount)
{
    this->m_iLayerCount = iLayerCount;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ImageFormat property.
// RETURNS: The format of the map image.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderMapResult::GetImageFormat() const
{
    return this->m_strImageFormat;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the ImageFormat property.
// PARAMETERS:
//      Input:
//          pstrImageFormat - The format of the map image.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::SetImageFormat(const MdfString& pstrImageFormat)
{
    this->m_strImageFormat = pstrImageFormat;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the RendererType property.
// RETURNS: The renderer used in the image rendering process.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderMapResult::GetRendererType() const
{
    return this->m_strRendererType;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the RendererType property.
// PARAMETERS:
//      Input:
//          pstrRendererType - The renderer used in the image rendering process.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::SetRendererType(const MdfString& pstrRendererType)
{
    this->m_strRendererType = pstrRendererType;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the RenderTime property.
// RETURNS: The total render time to generate the map image.
//-------------------------------------------------------------------------
const double ProfileRenderMapResult::GetRenderTime() const
{
    return this->m_dRenderTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the RenderTime property.
// PARAMETERS:
//      Input:
//          dRenderTime - The total render time to generate the map image.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::SetRenderTime(const double& dRenderTime)
{
    this->m_dRenderTime = dRenderTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderLayersResult property.
// RETURNS: The pointer of ProfileRenderLayersResult.
//-------------------------------------------------------------------------
const ProfileRenderLayersResult* ProfileRenderMapResult::GetProfileRenderLayersResult() const
{
    return this->m_profileRenderLayersResult;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderLayersResult property.
// RETURNS: The pointer of ProfileRenderLayersResult.
//-------------------------------------------------------------------------
ProfileRenderLayersResult* ProfileRenderMapResult::GetProfileRenderLayersResult()
{
    return this->m_profileRenderLayersResult;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderLayersResult property.
// PARAMETERS:
//        Input:
//          pProfileRenderLayersResult - Adopted ProfileRenderLayersResult object that is created on the heap.
//                                         It may be NULL.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::AdoptProfileRenderLayersResult(ProfileRenderLayersResult* pProfileRenderLayersResult)
{
    if (this->m_profileRenderLayersResult != pProfileRenderLayersResult)
    {
        delete this->m_profileRenderLayersResult;
        this->m_profileRenderLayersResult = pProfileRenderLayersResult;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderLayersResult property.
//
// RETURNS: The pointer to the orphaned ProfileRenderLayersResult object. It may be NULL.
//-------------------------------------------------------------------------
ProfileRenderLayersResult* ProfileRenderMapResult::OrphanProfileRenderLayersResult()
{
    ProfileRenderLayersResult* pRet = this->m_profileRenderLayersResult;
    this->m_profileRenderLayersResult = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderSelectionResult property.
// RETURNS: The pointer of ProfileRenderSelectionResult.
//-------------------------------------------------------------------------
const ProfileRenderSelectionResult* ProfileRenderMapResult::GetProfileRenderSelectionResult() const
{
    return this->m_profileRenderSelectionResult;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderSelectionResult property.
// RETURNS: The pointer of ProfileRenderSelectionResult.
//-------------------------------------------------------------------------
ProfileRenderSelectionResult* ProfileRenderMapResult::GetProfileRenderSelectionResult()
{
    return this->m_profileRenderSelectionResult;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderSelectionResult property.
// PARAMETERS:
//        Input:
//          pProfileRenderSelectionResult - Adopted ProfileRenderSelectionResult object that is created on the heap.
//                                         It may be NULL.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::AdoptProfileRenderSelectionResult(ProfileRenderSelectionResult* pProfileRenderSelectionResult)
{
    if (this->m_profileRenderSelectionResult != pProfileRenderSelectionResult)
    {
        delete this->m_profileRenderSelectionResult;
        this->m_profileRenderSelectionResult = pProfileRenderSelectionResult;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderSelectionResult property.
//
// RETURNS: The pointer to the orphaned ProfileRenderSelectionResult object. It may be NULL.
//-------------------------------------------------------------------------
ProfileRenderSelectionResult* ProfileRenderMapResult::OrphanProfileRenderSelectionResult()
{
    ProfileRenderSelectionResult* pRet = this->m_profileRenderSelectionResult;
    this->m_profileRenderSelectionResult = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderWatermarksResult property.
// RETURNS: The pointer of ProfileRenderWatermarksResult.
//-------------------------------------------------------------------------
const ProfileRenderWatermarksResult* ProfileRenderMapResult::GetProfileRenderWatermarksResult() const
{
    return this->m_profileRenderWatermarksResult;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderWatermarksResult property.
// RETURNS: The pointer of ProfileRenderWatermarksResult.
//-------------------------------------------------------------------------
ProfileRenderWatermarksResult* ProfileRenderMapResult::GetProfileRenderWatermarksResult()
{
    return this->m_profileRenderWatermarksResult;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderWatermarksResult property.
// PARAMETERS:
//        Input:
//          pProfileRenderWatermarksResult - Adopted ProfileRenderWatermarksResult object that is created on the heap.
//                                         It may be NULL.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::AdoptProfileRenderWatermarksResult(ProfileRenderWatermarksResult* pProfileRenderWatermarksResult)
{
    if (this->m_profileRenderWatermarksResult != pProfileRenderWatermarksResult)
    {
        delete this->m_profileRenderWatermarksResult;
        this->m_profileRenderWatermarksResult = pProfileRenderWatermarksResult;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderWatermarksResult property.
//
// RETURNS: The pointer to the orphaned ProfileRenderWatermarksResult object. It may be NULL.
//-------------------------------------------------------------------------
ProfileRenderWatermarksResult* ProfileRenderMapResult::OrphanProfileRenderWatermarksResult()
{
    ProfileRenderWatermarksResult* pRet = this->m_profileRenderWatermarksResult;
    this->m_profileRenderWatermarksResult = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderLabelsResult property.
// RETURNS: The pointer of ProfileRenderLabelsResult.
//-------------------------------------------------------------------------
const ProfileRenderLabelsResult* ProfileRenderMapResult::GetProfileRenderLabelsResult() const
{
    return this->m_profileRenderLabelsResult;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderLabelsResult property.
// RETURNS: The pointer of ProfileRenderLabelsResult.
//-------------------------------------------------------------------------
ProfileRenderLabelsResult* ProfileRenderMapResult::GetProfileRenderLabelsResult()
{
    return this->m_profileRenderLabelsResult;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderLabelsResult property.
// PARAMETERS:
//        Input:
//          pProfileRenderLabelsResult - Adopted ProfileRenderLabelsResult object that is created on the heap.
//                                         It may be NULL.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::AdoptProfileRenderLabelsResult(ProfileRenderLabelsResult* pProfileRenderLabelsResult)
{
    if (this->m_profileRenderLabelsResult != pProfileRenderLabelsResult)
    {
        delete this->m_profileRenderLabelsResult;
        this->m_profileRenderLabelsResult = pProfileRenderLabelsResult;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderLabelsResult property.
//
// RETURNS: The pointer to the orphaned ProfileRenderLabelsResult object. It may be NULL.
//-------------------------------------------------------------------------
ProfileRenderLabelsResult* ProfileRenderMapResult::OrphanProfileRenderLabelsResult()
{
    ProfileRenderLabelsResult* pRet = this->m_profileRenderLabelsResult;
    this->m_profileRenderLabelsResult = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the CreateImageTime property.
// RETURNS: The time of rendering stylized map to an image.
//-------------------------------------------------------------------------
const double ProfileRenderMapResult::GetCreateImageTime() const
{
    return this->m_dCreateImageTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the CreateImageTime property.
// PARAMETERS:
//      Input:
//          dCreateImageTime - The time of rendering stylized map to an image.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::SetCreateImageTime(const double& dCreateImageTime)
{
    this->m_dCreateImageTime = dCreateImageTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Error property.
// RETURNS: The error message which describes map rendering failure.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderMapResult::GetError() const
{
    return this->m_strError;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Error property.
// PARAMETERS:
//      Input:
//          pstrError - The error message which describes map rendering failure.
//-------------------------------------------------------------------------
void ProfileRenderMapResult::SetError(const MdfString& pstrError)
{
    this->m_strError = pstrError;
}

//-------------------------------------------------------------------------
// PURPOSE: For memory leak testing.
//-------------------------------------------------------------------------
#ifdef _WIN32
#ifdef _DEBUG
int ProfileRenderMapResult::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif
