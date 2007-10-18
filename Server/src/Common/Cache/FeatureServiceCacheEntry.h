//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

    void SetActiveFlag(bool active);
    bool GetActiveFlag() const;

    void SetFeatureSchemaName(CREFSTRING featureSchemaName);
    CREFSTRING GetFeatureSchemaName() const;

    void SetFeatureClassName(CREFSTRING featureClassName);
    CREFSTRING GetFeatureClassName() const;

    MgSpatialContextReader* GetSpatialContextReader();
    void SetSpatialContextReader(MgSpatialContextReader* spatialContextReader);

    MgStringCollection* GetFeatureSchemaNames();
    void SetFeatureSchemaNames(MgStringCollection* featureSchemaNames);

    MgFeatureSchemaCollection* GetFeatureSchemaCollection();
    void SetFeatureSchemaCollection(MgFeatureSchemaCollection* featureSchemaCollection);

    STRING GetFeatureSchemaXml();
    void SetFeatureSchemaXml(CREFSTRING featureSchemaXml);

    MgStringCollection* GetFeatureClassNames();
    void SetFeatureClassNames(MgStringCollection* featureClassNames);

    MgClassDefinition* GetFeatureClassDefinition();
    void SetFeatureClassDefinition(MgClassDefinition* featureClassDefinition);

    MgPropertyDefinitionCollection* GetFeatureClassIdentityProperties();
    void SetFeatureClassIdentityProperties(MgPropertyDefinitionCollection* featureClassIdentityProperties);

/// Data Members

private:

    Ptr<MgFeatureSourceCacheItem> m_featureSource;
    Ptr<MgSpatialContextCacheItem> m_spatialContextInfo;

    bool m_active;
    Ptr<MgSpatialContextReader> m_spatialContextReader;

    Ptr<MgStringCollection> m_featureSchemaNames;

    STRING m_featureSchemaName;

    Ptr<MgFeatureSchemaCollection> m_featureSchemaCollection;
    STRING m_featureSchemaXml;
    Ptr<MgStringCollection> m_featureClassNames;

    STRING m_featureClassName;

    Ptr<MgClassDefinition> m_featureClassDefinition;
    Ptr<MgPropertyDefinitionCollection> m_featureClassIdentityProperties;
};

/// Inline Methods

inline bool MgFeatureServiceCacheEntry::GetActiveFlag() const
{
    return m_active;
}

inline CREFSTRING MgFeatureServiceCacheEntry::GetFeatureSchemaName() const
{
    return m_featureSchemaName;
}

inline CREFSTRING MgFeatureServiceCacheEntry::GetFeatureClassName() const
{
    return m_featureClassName;
}

#endif
