#ifndef DESKTOP_FEATURE_SERVICE_CACHE_H
#define DESKTOP_FEATURE_SERVICE_CACHE_H

#include "MgDesktop.h"
#include "FeatureServiceCacheEntry.h"
#include "SpatialContextCacheItem.h"

class MgdFeatureServiceCache : public MgGuardDisposable
{
private:
    static Ptr<MgdFeatureServiceCache> smInstance;
    MgdFeatureServiceCache();

public:
    static MgdFeatureServiceCache* GetInstance();
    virtual ~MgdFeatureServiceCache();

public:
    void Clear();

    void RemoveEntry(CREFSTRING resource);
    void RemoveEntry(MgResourceIdentifier* resource);

    MgdFeatureSourceCacheItem* GetFeatureSource(MgResourceIdentifier* resource);
    MgdSpatialContextCacheItem* GetSpatialContextInfo(MgResourceIdentifier* resource);

    void SetFeatureSource(MgResourceIdentifier* resource, MgdFeatureSourceCacheItem* featureSource);
    
    void SetSpatialContextInfo(MgResourceIdentifier* resource, MgdSpatialContextCacheItem* spatialContextInfo);
    
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
    MgdFeatureSourceCacheItem* GetFeatureSourceInternal(MgResourceIdentifier* resource);
    MgdSpatialContextCacheItem* GetSpatialContextInfoInternal(MgResourceIdentifier* resource);

    MgdFeatureServiceCacheEntry* SetEntry(MgResourceIdentifier* resource);
    MgdFeatureServiceCacheEntry* GetEntry(MgResourceIdentifier* resource);

    virtual void Dispose() { delete this; }

private:
    /// Needed for thread-safety
    ACE_Recursive_Thread_Mutex m_MgdMutex;

    typedef std::map<STRING, MgdFeatureServiceCacheEntry*> MgdFeatureServiceCacheEntries;
    MgdFeatureServiceCacheEntries m_featureServiceCacheEntries;
};

#endif