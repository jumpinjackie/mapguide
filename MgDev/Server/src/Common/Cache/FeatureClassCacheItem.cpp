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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Cache an FDO Feature Schema collection.
/// FDO Feature Schemas are not thread-safe (e.g. reference count) so the cache
/// must take ownership via one thread at a time.
///
/// Note that this cache item is used to to improve the Feature Service
/// performance where providers do not support a Class Name hint for the
/// Describe Schema command.
///
void MgFeatureClassCacheItem::SetFdoSchemas(FdoFeatureSchemaCollection* schemas)
{
    m_fdoSchemas = FDO_SAFE_ADDREF(schemas);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an FDO Feature Schema collection from the cache.
/// FDO Feature Schemas are not thread-safe (e.g. reference count) so the caller
/// must take ownership via one thread at a time.
/// Remove the FDO Feature Schema collection from the cache so that they won't
/// be given to any other thread that calls this function.
/// Caller is responsible for putting this schemas back in the cache when
/// done with them.
/// This prevents a race condition that would occur if the caller does a release
/// at the same time another thread tries to retrieve the schemas from this cache.
///
/// Note that this cache item is used to to improve the Feature Service
/// performance where providers do not support a class name hint for the
/// Describe Schema command.
///
FdoFeatureSchemaCollection* MgFeatureClassCacheItem::GetFdoSchemas()
{
    return m_fdoSchemas.Detach();
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
