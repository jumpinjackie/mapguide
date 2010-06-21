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

#ifndef MG_FEATURE_CLASS_CACHE_ITEM_H_
#define MG_FEATURE_CLASS_CACHE_ITEM_H_

#include "ServerCacheItem.h"
#include "Fdo.h"

class MG_SERVER_CACHE_API MgFeatureClassCacheItem : public MgServerCacheItem
{
/// Constructors/Destructor

public:

    MgFeatureClassCacheItem();
    virtual ~MgFeatureClassCacheItem();

private:

    // Unimplemented copy constructor and assignment operator.
    MgFeatureClassCacheItem(const MgFeatureClassCacheItem&);
    MgFeatureClassCacheItem& operator=(const MgFeatureClassCacheItem&);

/// Methods

public:

    void SetSchemaXml(CREFSTRING schemaXml);
    STRING GetSchemaXml();

    void SetSchemas(bool serialized, MgFeatureSchemaCollection* schemas);
    MgFeatureSchemaCollection* GetSchemas(bool serialized);

    void SetClassDefinition(MgClassDefinition* classDef);
    MgClassDefinition* GetClassDefinition();

    void SetClassIdentityProperties(MgPropertyDefinitionCollection* idProperties);
    MgPropertyDefinitionCollection* GetClassIdentityProperties();

/// Data Members

private:

    STRING m_schemaXml;

    Ptr<MgFeatureSchemaCollection> m_serializedSchemas;
    Ptr<MgFeatureSchemaCollection> m_unserializedSchemas;

    Ptr<MgClassDefinition> m_classDef;
    Ptr<MgPropertyDefinitionCollection> m_idProperties;
};

/// Inline Methods

#endif
