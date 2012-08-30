#ifndef DESKTOP_FEATURE_CLASS_CACHE_ITEM_H
#define DESKTOP_FEATURE_CLASS_CACHE_ITEM_H
/// \cond INTERNAL
class MgdFeatureClassCacheItem : public MgGuardDisposable
{
/// Constructors/Destructor

public:

    MgdFeatureClassCacheItem();
    virtual ~MgdFeatureClassCacheItem();

private:

    // Unimplemented copy constructor and assignment operator.
    MgdFeatureClassCacheItem(const MgdFeatureClassCacheItem&);
    MgdFeatureClassCacheItem& operator=(const MgdFeatureClassCacheItem&);

/// Methods

public:

    void SetSchemaXml(CREFSTRING schemaXml);
    STRING GetSchemaXml();

    void SetSchemas(bool serialized, MgFeatureSchemaCollection* schemas);
    MgFeatureSchemaCollection* GetSchemas(bool serialized);

    void SetClassDefinition(MgClassDefinition* classDef);
    MgClassDefinition* GetClassDefinition();

    void SetClassIdentityProperties(MgPropertyDefinitionCollection* idProperties);
    MgPropertyDefinitionCollection* GetClassIdentityProperties();

protected:
    virtual void Dispose() { delete this; }

/// Data Members

private:

    STRING m_schemaXml;

    Ptr<MgFeatureSchemaCollection> m_serializedSchemas;
    Ptr<MgFeatureSchemaCollection> m_unserializedSchemas;

    Ptr<MgClassDefinition> m_classDef;
    Ptr<MgPropertyDefinitionCollection> m_idProperties;
};
/// \endcond
/// Inline Methods

#endif
