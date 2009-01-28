//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef MG_FEATURE_SERVICE_CACHE_H_
#define MG_FEATURE_SERVICE_CACHE_H_

#include "ServerCache.h"
#include "FeatureServiceCacheEntry.h"

class MG_SERVER_CACHE_API MgFeatureServiceCache : public MgServerCache
{
    DECLARE_CLASSNAME(MgFeatureServiceCache)

/// Constructors/Destructor

public:

    MgFeatureServiceCache();
    virtual ~MgFeatureServiceCache();

private:

    // Unimplemented copy constructor and assignment operator.
    MgFeatureServiceCache(const MgFeatureServiceCache&);
    MgFeatureServiceCache& operator=(const MgFeatureServiceCache&);

/// Methods

public:

    virtual void Clear();

    void RemoveEntry(CREFSTRING resource);
    void RemoveEntry(MgResourceIdentifier* resource);
    void RemoveExpiredEntries();

    void SetFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceCacheItem* featureSource);
    MgFeatureSourceCacheItem* GetFeatureSource(MgResourceIdentifier* resource);

    void SetSpatialContextInfo(MgResourceIdentifier* resource, MgSpatialContextCacheItem* spatialContextInfo);
    MgSpatialContextCacheItem* GetSpatialContextInfo(MgResourceIdentifier* resource);

    void SetSpatialContextReader(MgResourceIdentifier* resource, bool active, MgSpatialContextReader* spatialContextReader);
    MgSpatialContextReader* GetSpatialContextReader(MgResourceIdentifier* resource, bool active);

    void SetSchemaNames(MgResourceIdentifier* resource, MgStringCollection* schemaNames);
    MgStringCollection* GetSchemaNames(MgResourceIdentifier* resource);

    void SetClassNames(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames);
    MgStringCollection* GetClassNames(MgResourceIdentifier* resource, CREFSTRING schemaName);

    void SetSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, CREFSTRING schemaXml);
    STRING GetSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames);

    void SetFdoSchemas(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, bool classNameHintUsed, FdoFeatureSchemaCollection* schemas);
    FdoFeatureSchemaCollection* GetFdoSchemas(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, bool& classNameHintUsed);

    void SetSchemas(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, bool serialized, MgFeatureSchemaCollection* schemas);
    MgFeatureSchemaCollection* GetSchemas(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, bool serialized);

    void SetClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDef);
    MgClassDefinition* GetClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);

    void SetClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* idProperties);
    MgPropertyDefinitionCollection* GetClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);

protected:

    void Compact();

    MgFeatureServiceCacheEntry* SetEntry(MgResourceIdentifier* resource);
    MgFeatureServiceCacheEntry* GetEntry(MgResourceIdentifier* resource);
    void RemoveOldEntry();

/// Data Members

private:

    friend class MgCacheManager;

    typedef std::map<STRING, MgFeatureServiceCacheEntry*> MgFeatureServiceCacheEntries;
    MgFeatureServiceCacheEntries m_featureServiceCacheEntries;
};

#endif
