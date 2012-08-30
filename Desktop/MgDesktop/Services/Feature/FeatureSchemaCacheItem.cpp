#include "MgDesktop.h"
#include "FeatureSchemaCacheItem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgdFeatureSchemaCacheItem::MgdFeatureSchemaCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgdFeatureSchemaCacheItem::~MgdFeatureSchemaCacheItem()
{
    for (MgdFeatureClassCacheItems::iterator i = m_featureClassCacheItems.begin();
        i != m_featureClassCacheItems.end(); ++i)
    {
#ifdef _DEBUG
        ACE_ASSERT(NULL != i->second);

        if (NULL != i->second && 1 != i->second->GetRefCount())
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgdFeatureSchemaCacheItem::~MgdFeatureSchemaCacheItem() - Reference Count of '%W': %d\n"),
                i->first.c_str(), i->second->GetRefCount()));
        }
#endif
        SAFE_RELEASE(i->second);
    }

    m_featureClassCacheItems.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an existing MgdFeatureClassCacheItem or a newly created one
/// if it does not exist.
///
MgdFeatureClassCacheItem* MgdFeatureSchemaCacheItem::SetFeatureClassCacheItem(CREFSTRING classKey)
{
    Ptr<MgdFeatureClassCacheItem> item = GetFeatureClassCacheItem(classKey);

    if (NULL == item.p)
    {
        item = new MgdFeatureClassCacheItem();
        m_featureClassCacheItems.insert(MgdFeatureClassCacheItems::value_type(
            classKey, SAFE_ADDREF(item.p)));
    }

    return item.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an existing MgdFeatureClassCacheItem.
///
MgdFeatureClassCacheItem* MgdFeatureSchemaCacheItem::GetFeatureClassCacheItem(CREFSTRING classKey)
{
    Ptr<MgdFeatureClassCacheItem> item;
    MgdFeatureClassCacheItems::iterator i =
        m_featureClassCacheItems.find(classKey);

    if (m_featureClassCacheItems.end() != i)
    {
        item = SAFE_ADDREF(i->second);
    }

    return item.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Methods to manage cache data.
///
void MgdFeatureSchemaCacheItem::SetClassNames(MgStringCollection* classNames)
{
    m_classNames = SAFE_ADDREF(classNames);
}

MgStringCollection* MgdFeatureSchemaCacheItem::GetClassNames()
{
    return SAFE_ADDREF(m_classNames.p);
}

void MgdFeatureSchemaCacheItem::SetSchemaXml(CREFSTRING classNames, CREFSTRING schemaXml)
{
    Ptr<MgdFeatureClassCacheItem> item = SetFeatureClassCacheItem(classNames);

    item->SetSchemaXml(schemaXml);
}

STRING MgdFeatureSchemaCacheItem::GetSchemaXml(CREFSTRING classNames)
{
    STRING data;
    Ptr<MgdFeatureClassCacheItem> item = GetFeatureClassCacheItem(classNames);

    if (NULL != item.p)
    {
        data = item->GetSchemaXml();
    }

    return data;
}

void MgdFeatureSchemaCacheItem::SetSchemas(CREFSTRING classNames, bool serialized, MgFeatureSchemaCollection* schemas)
{
    Ptr<MgdFeatureClassCacheItem> item = SetFeatureClassCacheItem(classNames);

    item->SetSchemas(serialized, schemas);
}

MgFeatureSchemaCollection* MgdFeatureSchemaCacheItem::GetSchemas(CREFSTRING classNames, bool serialized)
{
    Ptr<MgFeatureSchemaCollection> data;
    Ptr<MgdFeatureClassCacheItem> item = GetFeatureClassCacheItem(classNames);

    if (NULL != item.p)
    {
        data = item->GetSchemas(serialized);
    }

    return data.Detach();
}

void MgdFeatureSchemaCacheItem::SetClassDefinition(CREFSTRING className, MgClassDefinition* classDef)
{
    Ptr<MgdFeatureClassCacheItem> item = SetFeatureClassCacheItem(className);

    item->SetClassDefinition(classDef);
}

MgClassDefinition* MgdFeatureSchemaCacheItem::GetClassDefinition(CREFSTRING className)
{
    Ptr<MgClassDefinition> data;
    Ptr<MgdFeatureClassCacheItem> item = GetFeatureClassCacheItem(className);

    if (NULL != item.p)
    {
        data = item->GetClassDefinition();
    }

    return data.Detach();
}

void MgdFeatureSchemaCacheItem::SetClassIdentityProperties(CREFSTRING className, MgPropertyDefinitionCollection* idProperties)
{
    Ptr<MgdFeatureClassCacheItem> item = SetFeatureClassCacheItem(className);

    item->SetClassIdentityProperties(idProperties);
}

MgPropertyDefinitionCollection* MgdFeatureSchemaCacheItem::GetClassIdentityProperties(CREFSTRING className)
{
    Ptr<MgPropertyDefinitionCollection> data;
    Ptr<MgdFeatureClassCacheItem> item = GetFeatureClassCacheItem(className);

    if (NULL != item.p)
    {
        data = item->GetClassIdentityProperties();
    }

    return data.Detach();
}
