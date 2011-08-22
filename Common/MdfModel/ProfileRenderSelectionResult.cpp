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
// The ProfileRenderLayersResult class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ProfileRenderSelectionResult.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the ProfileRenderSelectionResult class.
//-------------------------------------------------------------------------
ProfileRenderSelectionResult::ProfileRenderSelectionResult()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
ProfileRenderSelectionResult::~ProfileRenderSelectionResult()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the RenderTime property.
//          
// RETURNS: The time of stylizing all selected layers in the map.
//-------------------------------------------------------------------------
const double ProfileRenderSelectionResult::GetRenderTime() const
{
    return this->m_dRenderTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the RenderTime property.
// PARAMETERS:
//      Input:
//          dRenderTime - The time of stylizing all selected layers in the map.
//-------------------------------------------------------------------------
void ProfileRenderSelectionResult::SetRenderTime(const double& dRenderTime)
{
    this->m_dRenderTime = dRenderTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Filter property.
//          
// RETURNS: A boolean FDO expression that specifies which features to return.
//-------------------------------------------------------------------------
const MdfString& ProfileRenderSelectionResult::GetFilter() const
{
    return this->m_strFilter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Filter property.
// PARAMETERS:
//      Input:
//          pstrFilter - A boolean FDO expression that specifies which features to return.
//-------------------------------------------------------------------------
void ProfileRenderSelectionResult::SetFilter(const MdfString& pstrFilter)
{
    this->m_strFilter = pstrFilter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderLayerResults property.
//          
// RETURNS: A collection of ProfileRenderLayerResult.
//-------------------------------------------------------------------------
ProfileRenderLayerResultCollection* ProfileRenderSelectionResult::GetProfileRenderSelectedLayerResults()
{
    return &this->m_profileRenderSelectedLayerResults;
}