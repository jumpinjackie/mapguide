#ifndef DESKTOP_FEATURE_SERVICE_CACHE_ENTRY_H
#define DESKTOP_FEATURE_SERVICE_CACHE_ENTRY_H

#include "FeatureSchemaCacheItem.h"
#include "FeatureSourceCacheItem.h"
#include "SpatialContextCacheItem.h"

class MgdFeatureServiceCacheEntry : public MgGuardDisposable
{
public:
    MgdFeatureServiceCacheEntry();
    virtual ~MgdFeatureServiceCacheEntry();

private:

    // Unimplemented copy constructor and assignment operator.
    MgdFeatureServiceCacheEntry(const MgdFeatureServiceCacheEntry&);
    MgdFeatureServiceCacheEntry& operator=(const MgdFeatureServiceCacheEntry&);

protected:
    MgdFeatureServiceCacheEntry* SetEntry(MgResourceIdentifier* resource);
    MgdFeatureServiceCacheEntry* GetEntry(MgResourceIdentifier* resource);

/// Methods

public:

    void SetFeatureSource(MgdFeatureSourceCacheItem* featureSource);
    MgdFeatureSourceCacheItem* GetFeatureSource();

    void SetSpatialContextInfo(MgdSpatialContextCacheItem* spatialContextInfo);
    MgdSpatialContextCacheItem* GetSpatialContextInfo();

    void SetSpatialContextReader(MgSpatialContextReader* spatialContextReader);
    MgSpatialContextReader* GetSpatialContextReader();

    void SetSchemaNames(MgStringCollection* schemaNames);
    MgStringCollection* GetSchemaNames();

    void SetClassNames(CREFSTRING schemaName, MgStringCollection* classNames);
    MgStringCollection* GetClassNames(CREFSTRING schemaName);

    void SetSchemaXml(CREFSTRING schemaName, MgStringCollection* classNames, CREFSTRING schemaXml);
    STRING GetSchemaXml(CREFSTRING schemaName, MgStringCollection* classNames);

    void SetSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool serialized, MgFeatureSchemaCollection* schemas);
    MgFeatureSchemaCollection* GetSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool serialized);

    void SetClassDefinition(CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDef);
    MgClassDefinition* GetClassDefinition(CREFSTRING schemaName, CREFSTRING className);

    void SetClassIdentityProperties(CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* idProperties);
    MgPropertyDefinitionCollection* GetClassIdentityProperties(CREFSTRING schemaName, CREFSTRING className);

protected:

    MgdFeatureSchemaCacheItem* SetFeatureSchemaCacheItem(CREFSTRING schemaName);
    MgdFeatureSchemaCacheItem* GetFeatureSchemaCacheItem(CREFSTRING schemaName);

    void ParseQualifiedClassName(CREFSTRING schemaName, CREFSTRING className,
        REFSTRING parsedSchemaName, REFSTRING parsedClassName);

    void FormatKeys(bool classNameHintUsed,
        CREFSTRING schemaName, CREFSTRING className,
        REFSTRING schemaKey, REFSTRING classKey);
    INT32 FormatKeys(bool classNameHintUsed,
        CREFSTRING schemaName, MgStringCollection* classNames,
        REFSTRING schemaKey, REFSTRING classKey);

    virtual void Dispose() { delete this; }

private:

    MgFeatureSchemaCollection* FindSchema(MgFeatureSchemaCollection* schemas, CREFSTRING schemaName);
    MgFeatureSchemaCollection* FindSchema(CREFSTRING schemaKey, CREFSTRING classKey, bool serialized, INT32 classCount);
    bool FindClass(MgFeatureSchemaCollection* schemas, CREFSTRING className);

/// Data Members

private:

    bool m_classNameHintUsed;

    Ptr<MgdFeatureSourceCacheItem> m_featureSource;
    Ptr<MgdSpatialContextCacheItem> m_spatialContextInfo;

    Ptr<MgSpatialContextReader> m_spatialContextReader;

    Ptr<MgStringCollection> m_schemaNames;

    typedef std::map<STRING, MgdFeatureSchemaCacheItem*> MgdFeatureSchemaCacheItems;
    MgdFeatureSchemaCacheItems m_featureSchemaCacheItems;
};

#endif