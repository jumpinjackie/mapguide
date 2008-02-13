//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "FeatureServiceCacheEntry.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgFeatureServiceCacheEntry::MgFeatureServiceCacheEntry() :
    m_active(false)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgFeatureServiceCacheEntry::~MgFeatureServiceCacheEntry()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Methods to manage cache data.
///
void MgFeatureServiceCacheEntry::SetFeatureSource(MgFeatureSourceCacheItem* featureSource)
{
    m_featureSource = SAFE_ADDREF(featureSource);
}

MgFeatureSourceCacheItem* MgFeatureServiceCacheEntry::GetFeatureSource()
{
    return SAFE_ADDREF(m_featureSource.p);
}

void MgFeatureServiceCacheEntry::SetSpatialContextInfo(MgSpatialContextCacheItem* spatialContextInfo)
{
    m_spatialContextInfo = SAFE_ADDREF(spatialContextInfo);
}

MgSpatialContextCacheItem* MgFeatureServiceCacheEntry::GetSpatialContextInfo()
{
    return SAFE_ADDREF(m_spatialContextInfo.p);
}

void MgFeatureServiceCacheEntry::SetActiveFlag(bool active)
{
    m_active = active;
}

void MgFeatureServiceCacheEntry::SetFeatureSchemaName(CREFSTRING featureSchemaName)
{
    m_featureSchemaName = featureSchemaName;
}

void MgFeatureServiceCacheEntry::SetFeatureClassName(CREFSTRING featureClassName)
{
    m_featureClassName = featureClassName;
}

void MgFeatureServiceCacheEntry::SetSpatialContextReader(MgSpatialContextReader* spatialContextReader)
{
    m_spatialContextReader = SAFE_ADDREF(spatialContextReader);
}

MgSpatialContextReader* MgFeatureServiceCacheEntry::GetSpatialContextReader()
{
    return SAFE_ADDREF(m_spatialContextReader.p);
}

void MgFeatureServiceCacheEntry::SetFeatureSchemaNames(MgStringCollection* featureSchemaNames)
{
    m_featureSchemaNames = SAFE_ADDREF(featureSchemaNames);
}

MgStringCollection* MgFeatureServiceCacheEntry::GetFeatureSchemaNames()
{
    return SAFE_ADDREF(m_featureSchemaNames.p);
}

void MgFeatureServiceCacheEntry::SetFeatureSchemaCollection(MgFeatureSchemaCollection* featureSchemaCollection)
{
    m_featureSchemaCollection = SAFE_ADDREF(featureSchemaCollection);
}

MgFeatureSchemaCollection* MgFeatureServiceCacheEntry::GetFeatureSchemaCollection()
{
    return SAFE_ADDREF(m_featureSchemaCollection.p);
}

void MgFeatureServiceCacheEntry::SetFeatureSchemaXml(CREFSTRING featureSchemaXml)
{
    m_featureSchemaXml = featureSchemaXml;
}

STRING MgFeatureServiceCacheEntry::GetFeatureSchemaXml()
{
    return m_featureSchemaXml;
}

void MgFeatureServiceCacheEntry::SetFeatureClassNames(MgStringCollection* featureClassNames)
{
    m_featureClassNames = SAFE_ADDREF(featureClassNames);
}

MgStringCollection* MgFeatureServiceCacheEntry::GetFeatureClassNames()
{
    return SAFE_ADDREF(m_featureClassNames.p);
}

void MgFeatureServiceCacheEntry::SetFeatureClassDefinition(MgClassDefinition* featureClassDefinition)
{
    m_featureClassDefinition = SAFE_ADDREF(featureClassDefinition);
}

MgClassDefinition* MgFeatureServiceCacheEntry::GetFeatureClassDefinition()
{
    return SAFE_ADDREF(m_featureClassDefinition.p);
}

void MgFeatureServiceCacheEntry::SetFeatureClassIdentityProperties(MgPropertyDefinitionCollection* featureClassIdentityProperties)
{
    m_featureClassIdentityProperties = SAFE_ADDREF(featureClassIdentityProperties);
}

MgPropertyDefinitionCollection* MgFeatureServiceCacheEntry::GetFeatureClassIdentityProperties()
{
    return SAFE_ADDREF(m_featureClassIdentityProperties.p);
}
