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

#ifndef MG_FEATURE_SCHEMA_CACHE_ITEM_H_
#define MG_FEATURE_SCHEMA_CACHE_ITEM_H_

#include "FeatureClassCacheItem.h"

class MG_SERVER_CACHE_API MgFeatureSchemaCacheItem : public MgServerCacheItem
{
/// Constructors/Destructor

public:

    MgFeatureSchemaCacheItem();
    virtual ~MgFeatureSchemaCacheItem();

private:

    // Unimplemented copy constructor and assignment operator.
    MgFeatureSchemaCacheItem(const MgFeatureSchemaCacheItem&);
    MgFeatureSchemaCacheItem& operator=(const MgFeatureSchemaCacheItem&);

/// Methods

public:

    void SetClassNames(MgStringCollection* classNames);
    MgStringCollection* GetClassNames();

    void SetSchemaXml(CREFSTRING classNames, CREFSTRING schemaXml);
    STRING GetSchemaXml(CREFSTRING classNames);

    void SetSchemas(CREFSTRING classNames, bool serialized, MgFeatureSchemaCollection* schemas);
    MgFeatureSchemaCollection* GetSchemas(CREFSTRING classNames, bool serialized);

    void SetClassDefinition(CREFSTRING className, MgClassDefinition* classDef);
    MgClassDefinition* GetClassDefinition(CREFSTRING className);

    void SetClassIdentityProperties(CREFSTRING className, MgPropertyDefinitionCollection* idProperties);
    MgPropertyDefinitionCollection* GetClassIdentityProperties(CREFSTRING className);

protected:

    MgFeatureClassCacheItem* SetFeatureClassCacheItem(CREFSTRING classKey);
    MgFeatureClassCacheItem* GetFeatureClassCacheItem(CREFSTRING classKey);

/// Data Members

private:

    Ptr<MgStringCollection> m_classNames;

    typedef std::map<STRING, MgFeatureClassCacheItem*> MgFeatureClassCacheItems;
    MgFeatureClassCacheItems m_featureClassCacheItems;
};

/// Inline Methods

#endif
