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

#ifndef MG_FEATURE_SERVICE_CACHE_ENTRY_H_
#define MG_FEATURE_SERVICE_CACHE_ENTRY_H_

#include "ServerCacheEntry.h"
#include "FeatureSourceCacheItem.h"
#include "SpatialContextCacheItem.h"
#include "FeatureSchemaCacheItem.h"

class MG_SERVER_CACHE_API MgFeatureServiceCacheEntry : public MgServerCacheEntry
{
/// Constructors/Destructor

public:

    MgFeatureServiceCacheEntry();
    virtual ~MgFeatureServiceCacheEntry();

private:

    // Unimplemented copy constructor and assignment operator.
    MgFeatureServiceCacheEntry(const MgFeatureServiceCacheEntry&);
    MgFeatureServiceCacheEntry& operator=(const MgFeatureServiceCacheEntry&);

/// Methods

public:

    void SetFeatureSource(MgFeatureSourceCacheItem* featureSource);
    MgFeatureSourceCacheItem* GetFeatureSource();

    void SetSpatialContextInfo(MgSpatialContextCacheItem* spatialContextInfo);
    MgSpatialContextCacheItem* GetSpatialContextInfo();

    void SetSpatialContextReader(MgSpatialContextReader* spatialContextReader);
    MgSpatialContextReader* GetSpatialContextReader();

    void SetSchemaNames(MgStringCollection* schemaNames);
    MgStringCollection* GetSchemaNames();

    void SetClassNames(CREFSTRING schemaName, MgStringCollection* classNames);
    MgStringCollection* GetClassNames(CREFSTRING schemaName);

    void SetSchemaXml(CREFSTRING schemaName, MgStringCollection* classNames, CREFSTRING schemaXml);
    STRING GetSchemaXml(CREFSTRING schemaName, MgStringCollection* classNames);

    void SetFdoSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool classNameHintUsed, FdoFeatureSchemaCollection* schemas);
    FdoFeatureSchemaCollection* GetFdoSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool& classNameHintUsed);

    void SetSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool serialized, MgFeatureSchemaCollection* schemas);
    MgFeatureSchemaCollection* GetSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool serialized);

    void SetClassDefinition(CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDef);
    MgClassDefinition* GetClassDefinition(CREFSTRING schemaName, CREFSTRING className);

    void SetClassIdentityProperties(CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* idProperties);
    MgPropertyDefinitionCollection* GetClassIdentityProperties(CREFSTRING schemaName, CREFSTRING className);

protected:

    MgFeatureSchemaCacheItem* SetFeatureSchemaCacheItem(CREFSTRING schemaName);
    MgFeatureSchemaCacheItem* GetFeatureSchemaCacheItem(CREFSTRING schemaName);

    void ParseQualifiedClassName(CREFSTRING schemaName, CREFSTRING className,
        REFSTRING parsedSchemaName, REFSTRING parsedClassName);

    void FormatKeys(bool classNameHintUsed,
        CREFSTRING schemaName, CREFSTRING className,
        REFSTRING schemaKey, REFSTRING classKey);
    INT32 FormatKeys(bool classNameHintUsed,
        CREFSTRING schemaName, MgStringCollection* classNames,
        REFSTRING schemaKey, REFSTRING classKey);

private:

    MgFeatureSchemaCollection* FindSchema(MgFeatureSchemaCollection* schemas, CREFSTRING schemaName);
    MgFeatureSchemaCollection* FindSchema(CREFSTRING schemaKey, CREFSTRING classKey, bool serialized, INT32 classCount);
    bool FindClass(MgFeatureSchemaCollection* schemas, CREFSTRING className);

/// Data Members

private:

    bool m_classNameHintUsed;

    Ptr<MgFeatureSourceCacheItem> m_featureSource;
    Ptr<MgSpatialContextCacheItem> m_spatialContextInfo;

    Ptr<MgSpatialContextReader> m_spatialContextReader;

    Ptr<MgStringCollection> m_schemaNames;

    typedef std::map<STRING, MgFeatureSchemaCacheItem*> MgFeatureSchemaCacheItems;
    MgFeatureSchemaCacheItems m_featureSchemaCacheItems;
};

/// Inline Methods

#endif
