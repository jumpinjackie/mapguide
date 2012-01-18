#ifndef DESKTOP_FEATURE_SERVICE_CACHE_ENTRY_H
#define DESKTOP_FEATURE_SERVICE_CACHE_ENTRY_H

#include "FeatureSchemaCacheItem.h"
#include "FeatureSourceCacheItem.h"
#include "SpatialContextCacheItem.h"

class MgFeatureServiceCacheEntry : public MgGuardDisposable
{
public:
    MgFeatureServiceCacheEntry();
    virtual ~MgFeatureServiceCacheEntry();

private:

    // Unimplemented copy constructor and assignment operator.
    MgFeatureServiceCacheEntry(const MgFeatureServiceCacheEntry&);
    MgFeatureServiceCacheEntry& operator=(const MgFeatureServiceCacheEntry&);

protected:
    MgFeatureServiceCacheEntry* SetEntry(MgResourceIdentifier* resource);
    MgFeatureServiceCacheEntry* GetEntry(MgResourceIdentifier* resource);

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

    virtual void Dispose() { delete this; }

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

#endif