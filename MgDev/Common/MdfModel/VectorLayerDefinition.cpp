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

//-------------------------------------------------------------------------
// DESCRIPTION:
// VectorLayerDefinition Class Implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "VectorLayerDefinition.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the VectorLayerDefinition class.
// PARAMETERS:
//     Input:
//          strName - the VectorLayerDefinition name. Cannot be an empty string.
//-------------------------------------------------------------------------
VectorLayerDefinition::VectorLayerDefinition(const MdfString& strDataResourceID, const MdfString &strFeatureName)
: LayerDefinition(strDataResourceID)
, m_strFeatureName(strFeatureName)
{
    // default values
    this->m_featureNameType = FeatureClass;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all the adopted objects.
//-------------------------------------------------------------------------
VectorLayerDefinition::~VectorLayerDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FeatureName property. The
//          FeatureName is either the name of a feature class or an extension
//          of the feature source.
// RETURNS: A feature class or extension name.
//-------------------------------------------------------------------------
const MdfString& VectorLayerDefinition::GetFeatureName() const
{
    return this->m_strFeatureName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the FeatureName property. The
//          FeatureName is the name of a feature class or extension of
//          the feature source. This method does not check for
//          the validity of the feature class arguement.
// PARAMETERS:
//      Input:
//          strFeatureName - A feature class or extension name
//-------------------------------------------------------------------------
void VectorLayerDefinition::SetFeatureName(const MdfString& strFeatureName)
{
    this->m_strFeatureName = strFeatureName;
}

//-------------------------------------------------------------------------
// PURPOSE: Gets a value determining the meaning of the FeatureName property value.
// RETURNS: The type of value stored in the FeatureName property.
//-------------------------------------------------------------------------
VectorLayerDefinition::FeatureNameType VectorLayerDefinition::GetFeatureNameType() const
{
    return this->m_featureNameType;
}

//-------------------------------------------------------------------------
// PURPOSE: Sets a value determining the meaning of the FeatureName property value.
// PARAMETERS:
//          featureNameType - The type of value stored in the FeatureName property.
//-------------------------------------------------------------------------
void VectorLayerDefinition::SetFeatureNameType(VectorLayerDefinition::FeatureNameType featureNameType)
{
    this->m_featureNameType = featureNameType;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the PropertyMappings property. The PropertyMappings
//          is a collection of objects, each of which defines a pair of
//          strings. It represents the attributes that are to be associated
//          with each feature returned.
// RETURNS: A pointer to a modifiable NameStringPairCollection.
//-------------------------------------------------------------------------
NameStringPairCollection* VectorLayerDefinition::GetPropertyMappings()
{
    return &this->m_collPropertyMappings;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Geometry property.
// RETURNS: A string representing the Geometry.
//-------------------------------------------------------------------------
const MdfString& VectorLayerDefinition::GetGeometry() const
{
    return this->m_strGeometry;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Geometry property.
//-------------------------------------------------------------------------
void VectorLayerDefinition::SetGeometry(const MdfString&  strGeometry)
{
    this->m_strGeometry = strGeometry;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Url property.
// RETURNS: A string representing the Url.
//-------------------------------------------------------------------------
const MdfString& VectorLayerDefinition::GetUrl() const
{
    return this->m_strUrl;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Url property.
//-------------------------------------------------------------------------
void VectorLayerDefinition::SetUrl(const MdfString&  strUrl)
{
    this->m_strUrl = strUrl;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ToolTip property.
// RETURNS: A string representing the ToolTip.
//-------------------------------------------------------------------------
const MdfString& VectorLayerDefinition::GetToolTip() const
{
    return this->m_strToolTip;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ToolTip property.
//-------------------------------------------------------------------------
void VectorLayerDefinition::SetToolTip(const MdfString&  strToolTip)
{
    this->m_strToolTip = strToolTip;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Filter property. The Filter is a SQL
//          where clause that determines which features are returned.
// RETURNS: SQL where clause.
//-------------------------------------------------------------------------
const MdfString& VectorLayerDefinition::GetFilter() const
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
void VectorLayerDefinition::SetFilter(const MdfString& strFilter)
{
    this->m_strFilter = strFilter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the VectorScaleRange collection that is defined
//          in this Layer.
// RETURNS: A pointer to the modifiable VectorScaleRangeCollection.
//-------------------------------------------------------------------------
VectorScaleRangeCollection* VectorLayerDefinition::GetScaleRanges()
{
    return &this->m_collScaleRanges;
}
