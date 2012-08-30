#ifndef DESKTOP_FEATURE_SCHEMA_CACHE_ITEM_H
#define DESKTOP_FEATURE_SCHEMA_CACHE_ITEM_H

class MgGuardDisposable;
class MgStringCollection;
class MgFeatureSchemaCollection;
class MgClassDefinition;
class MgPropertyDefinitionCollection;
class MgdFeatureClassCacheItem;

class MgdFeatureSchemaCacheItem : public MgGuardDisposable
{
/// Constructors/Destructor

public:

    MgdFeatureSchemaCacheItem();
    virtual ~MgdFeatureSchemaCacheItem();

private:

    // Unimplemented copy constructor and assignment operator.
    MgdFeatureSchemaCacheItem(const MgdFeatureSchemaCacheItem&);
    MgdFeatureSchemaCacheItem& operator=(const MgdFeatureSchemaCacheItem&);

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

    MgdFeatureClassCacheItem* SetFeatureClassCacheItem(CREFSTRING classKey);
    MgdFeatureClassCacheItem* GetFeatureClassCacheItem(CREFSTRING classKey);

    virtual void Dispose() { delete this; }

/// Data Members

private:

    Ptr<MgStringCollection> m_classNames;

    typedef std::map<STRING, MgdFeatureClassCacheItem*> MgdFeatureClassCacheItems;
    MgdFeatureClassCacheItems m_featureClassCacheItems;
};

#endif