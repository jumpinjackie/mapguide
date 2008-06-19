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

#ifndef MG_FEATURE_SERVICE_CACHE_ENTRY_H_
#define MG_FEATURE_SERVICE_CACHE_ENTRY_H_

#include "ServerCacheEntry.h"
#include "FeatureSourceCacheItem.h"
#include "SpatialContextCacheItem.h"
#include "Fdo.h"

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

    MgSpatialContextReader* GetSpatialContextReader();
    void SetSpatialContextReader(MgSpatialContextReader* spatialContextReader);

    MgStringCollection* GetFeatureSchemaNames();
    void SetFeatureSchemaNames(MgStringCollection* featureSchemaNames);

    void SetFeatureSchemaName(CREFSTRING featureSchemaName);
    STRING GetFeatureSchemaName() const;

    bool GetFeatureSchemaCollectionSerialized() const;
    void SetFeatureSchemaCollectionSerialized(bool serialized);

    MgFeatureSchemaCollection* GetFeatureSchemaCollection();
    void SetFeatureSchemaCollection(MgFeatureSchemaCollection* featureSchemaCollection);

    FdoFeatureSchemaCollection* GetFdoFeatureSchemaCollection();
    void SetFdoFeatureSchemaCollection(FdoFeatureSchemaCollection* featureSchemaCollection);

    STRING GetFeatureSchemaXml() const;
    void SetFeatureSchemaXml(CREFSTRING featureSchemaXml);

    MgStringCollection* GetFeatureClassNames();
    void SetFeatureClassNames(MgStringCollection* featureClassNames);

    void SetFeatureClassName(CREFSTRING featureClassName);
    STRING GetFeatureClassName() const;

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

    bool m_featureSchemaCollectionSerialized;
    Ptr<MgFeatureSchemaCollection> m_featureSchemaCollection;

    FdoPtr<FdoFeatureSchemaCollection> m_fdoFeatureSchemaCollection;

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

inline STRING MgFeatureServiceCacheEntry::GetFeatureSchemaName() const
{
    return m_featureSchemaName;
}

inline bool MgFeatureServiceCacheEntry::GetFeatureSchemaCollectionSerialized() const
{
    return m_featureSchemaCollectionSerialized;
}

inline STRING MgFeatureServiceCacheEntry::GetFeatureSchemaXml() const
{
    return m_featureSchemaXml;
}

inline STRING MgFeatureServiceCacheEntry::GetFeatureClassName() const
{
    return m_featureClassName;
}

#endif
