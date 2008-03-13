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

#ifndef MG_FEATURE_SERVICE_CACHE_H_
#define MG_FEATURE_SERVICE_CACHE_H_

#include "ServerCache.h"
#include "FeatureServiceCacheEntry.h"

// We may need to change to multi-mapping in the future if required.
typedef std::map<STRING, MgFeatureServiceCacheEntry*> MgFeatureServiceCacheEntries;

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

    MgFeatureServiceCacheEntry* GetEntry(MgResourceIdentifier* resource);

    void RemoveEntry(CREFSTRING resource);
    void RemoveEntry(MgResourceIdentifier* resource);
    void RemoveExpiredEntries();

    void SetFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceCacheItem* featureSource);
    MgFeatureSourceCacheItem* GetFeatureSource(MgResourceIdentifier* resource);

    void SetSpatialContextInfo(MgResourceIdentifier* resource, MgSpatialContextCacheItem* spatialContextInfo);
    MgSpatialContextCacheItem* GetSpatialContextInfo(MgResourceIdentifier* resource);

    void SetSpatialContextReader(MgResourceIdentifier* resource, bool active, MgSpatialContextReader* spatialContextReader);
    MgSpatialContextReader* GetSpatialContextReader(MgResourceIdentifier* resource, bool active);

    void SetFeatureSchemaNames(MgResourceIdentifier* resource, MgStringCollection* featureSchemaNames);
    MgStringCollection* GetFeatureSchemaNames(MgResourceIdentifier* resource);

    void SetFeatureSchemaCollection(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, MgFeatureSchemaCollection* featureSchemaCollection, bool bSerialize);
    MgFeatureSchemaCollection* GetFeatureSchemaCollection(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, bool bSerialize);

    void SetFeatureSchemaXml(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, CREFSTRING featureSchemaXml);
    STRING GetFeatureSchemaXml(MgResourceIdentifier* resource, CREFSTRING featureSchemaName);

    void SetFeatureClassNames(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, MgStringCollection* featureClassNames);
    MgStringCollection* GetFeatureClassNames(MgResourceIdentifier* resource, CREFSTRING featureSchemaName);

    void SetFeatureClassDefinition(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, CREFSTRING featureClassName, MgClassDefinition* featureClassDefinition);
    MgClassDefinition* GetFeatureClassDefinition(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, CREFSTRING featureClassName);

    void SetFeatureClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, CREFSTRING featureClassName, MgPropertyDefinitionCollection* featureClassIdentityProperties);
    MgPropertyDefinitionCollection* GetFeatureClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, CREFSTRING featureClassName);

    void SetFdoFeatureSchemaCollection(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, FdoFeatureSchemaCollection* featureSchemaCollection);
    FdoFeatureSchemaCollection* GetFdoFeatureSchemaCollection(MgResourceIdentifier* resource, CREFSTRING featureSchemaName);

protected:

    void Compact();

    MgFeatureServiceCacheEntry* CreateEntry(MgResourceIdentifier* resource);

    void RemoveOldEntry();

/// Data Members

private:

    MgFeatureServiceCacheEntries m_featureServiceCacheEntries;
};

#endif
