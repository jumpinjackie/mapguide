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

//-------------------------------------------------------------------------
// DESCRIPTION:
// GridLayerDefinition Class Implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "GridLayerDefinition.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GridLayerDefinition class
//-------------------------------------------------------------------------
GridLayerDefinition::GridLayerDefinition(const MdfString& strDataResourceID)
: LayerDefinition(strDataResourceID)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
GridLayerDefinition::~GridLayerDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Scale Ranges property.
//          The scale ranges stores the styles info for each scale range.
// RETURNS: The pointer to the scale ranges.
//-------------------------------------------------------------------------
GridScaleRangeCollection* GridLayerDefinition::GetScaleRanges()
{
    return &this->m_collScaleRanges;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FeatureName property. The
//          FeatureName is the name of the feature class that is to be
//          used from the feature source.
// RETURNS: A feature class name.
//-------------------------------------------------------------------------
const MdfString& GridLayerDefinition::GetFeatureName() const
{
    return this->m_strFeatureName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the FeatureName property. The
//          FeatureName is the name of the feature class that is to be
//          used from the feature source. This method does not check for
//          the validity of the feature class arguement.
// PARAMETERS:
//      Input:
//          strFeatureName - A feature class name
//-------------------------------------------------------------------------
void GridLayerDefinition::SetFeatureName(const MdfString& strFeatureName)
{
    this->m_strFeatureName = strFeatureName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Geometry property.
// RETURNS: A string representing the Geometry.
//-------------------------------------------------------------------------
const MdfString& GridLayerDefinition::GetGeometry() const
{
    return this->m_strGeometry;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Geometry property.
//-------------------------------------------------------------------------
void GridLayerDefinition::SetGeometry(const MdfString&  strGeometry)
{
    this->m_strGeometry = strGeometry;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Filter property. The Filter is a SQL
//          where clause that determines which features are returned.
// RETURNS: SQL where clause.
//-------------------------------------------------------------------------
const MdfString& GridLayerDefinition::GetFilter() const
{
    return this->m_strFilter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Filter property. The Filter is a SQL
//          where clause that determines which features are returned.
// PARAMETERS:
//      Input:
//          strFilter - SQL where clause.
//-------------------------------------------------------------------------
void GridLayerDefinition::SetFilter(const MdfString& strFilter)
{
    this->m_strFilter = strFilter;
}
