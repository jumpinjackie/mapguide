#ifndef DESKTOP_FEATURE_SERVICE_CACHE_H
#define DESKTOP_FEATURE_SERVICE_CACHE_H

#include "MgDesktop.h"
#include "FeatureServiceCacheEntry.h"
#include "SpatialContextCacheItem.h"

class MgFeatureServiceCache : public MgGuardDisposable
{
private:
    static Ptr<MgFeatureServiceCache> smInstance;
    MgFeatureServiceCache();

public:
    static MgFeatureServiceCache* GetInstance();
    virtual ~MgFeatureServiceCache();

public:
    void Clear();

    void RemoveEntry(CREFSTRING resource);
    void RemoveEntry(MgResourceIdentifier* resource);

    MgFeatureSourceCacheItem* GetFeatureSource(MgResourceIdentifier* resource);
    MgSpatialContextCacheItem* GetSpatialContextInfo(MgResourceIdentifier* resource);

    void SetFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceCacheItem* featureSource);
    
    void SetSpatialContextInfo(MgResourceIdentifier* resource, MgSpatialContextCacheItem* spatialContextInfo);
    
    void SetSpatialContextReader(MgResourceIdentifier* resource, MgSpatialContextReader* spatialContextReader);
    MgSpatialContextReader* GetSpatialContextReader(MgResourceIdentifier* resource);

    void SetSchemaNames(MgResourceIdentifier* resource, MgStringCollection* schemaNames);
    MgStringCollection* GetSchemaNames(MgResourceIdentifier* resource);

    void SetClassNames(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames);
    MgStringCollection* GetClassNames(MgResourceIdentifier* resource, CREFSTRING schemaName);

    void SetSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, CREFSTRING schemaXml);
    STRING GetSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames);

    void SetSchemas(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, bool serialized, MgFeatureSchemaCollection* schemas);
    MgFeatureSchemaCollection* GetSchemas(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, bool serialized);

    void SetClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDef);
    MgClassDefinition* GetClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);

    void SetClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* idProperties);
    MgPropertyDefinitionCollection* GetClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);

protected:
    MgFeatureSourceCacheItem* GetFeatureSourceInternal(MgResourceIdentifier* resource);
    MgSpatialContextCacheItem* GetSpatialContextInfoInternal(MgResourceIdentifier* resource);

    MgFeatureServiceCacheEntry* SetEntry(MgResourceIdentifier* resource);
    MgFeatureServiceCacheEntry* GetEntry(MgResourceIdentifier* resource);

    virtual void Dispose() { delete this; }

private:
    /// Needed for thread-safety
    ACE_Recursive_Thread_Mutex m_mutex;

    typedef std::map<STRING, MgFeatureServiceCacheEntry*> MgFeatureServiceCacheEntries;
    MgFeatureServiceCacheEntries m_featureServiceCacheEntries;
};

#endif