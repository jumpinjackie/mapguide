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

#include "MapGuideCommon.h"
#include "FeatureClassCacheItem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgFeatureClassCacheItem::MgFeatureClassCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgFeatureClassCacheItem::~MgFeatureClassCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Methods to manage cache data.
///
void MgFeatureClassCacheItem::SetSchemaXml(CREFSTRING schemaXml)
{
    m_schemaXml = schemaXml;
}

STRING MgFeatureClassCacheItem::GetSchemaXml()
{
    return m_schemaXml;
}

void MgFeatureClassCacheItem::SetSchemas(bool serialized, MgFeatureSchemaCollection* schemas)
{
    if (serialized)
    {
        m_serializedSchemas = SAFE_ADDREF(schemas);
    }
    else
    {
        m_unserializedSchemas = SAFE_ADDREF(schemas);
    }
}

MgFeatureSchemaCollection* MgFeatureClassCacheItem::GetSchemas(bool serialized)
{
    if (serialized)
    {
        return SAFE_ADDREF(m_serializedSchemas.p);
    }
    else
    {
        return SAFE_ADDREF(m_unserializedSchemas.p);
    }
}

void MgFeatureClassCacheItem::SetClassDefinition(MgClassDefinition* classDef)
{
    m_classDef = SAFE_ADDREF(classDef);
}

MgClassDefinition* MgFeatureClassCacheItem::GetClassDefinition()
{
    return SAFE_ADDREF(m_classDef.p);
}

void MgFeatureClassCacheItem::SetClassIdentityProperties(MgPropertyDefinitionCollection* idProperties)
{
    m_idProperties = SAFE_ADDREF(idProperties);
}

MgPropertyDefinitionCollection* MgFeatureClassCacheItem::GetClassIdentityProperties()
{
    return SAFE_ADDREF(m_idProperties.p);
}
