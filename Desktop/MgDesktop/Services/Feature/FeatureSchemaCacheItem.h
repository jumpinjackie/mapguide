#ifndef DESKTOP_FEATURE_SCHEMA_CACHE_ITEM_H
#define DESKTOP_FEATURE_SCHEMA_CACHE_ITEM_H

class MgGuardDisposable;
class MgStringCollection;
class MgFeatureSchemaCollection;
class MgClassDefinition;
class MgPropertyDefinitionCollection;
class MgFeatureClassCacheItem;

class MgFeatureSchemaCacheItem : public MgGuardDisposable
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

    virtual void Dispose() { delete this; }

/// Data Members

private:

    Ptr<MgStringCollection> m_classNames;

    typedef std::map<STRING, MgFeatureClassCacheItem*> MgFeatureClassCacheItems;
    MgFeatureClassCacheItems m_featureClassCacheItems;
};

#endif