#include "MgDesktop.h"
#include "FeatureSchemaCacheItem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgFeatureSchemaCacheItem::MgFeatureSchemaCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgFeatureSchemaCacheItem::~MgFeatureSchemaCacheItem()
{
    for (MgFeatureClassCacheItems::iterator i = m_featureClassCacheItems.begin();
        i != m_featureClassCacheItems.end(); ++i)
    {
#ifdef _DEBUG
        ACE_ASSERT(NULL != i->second);

        if (NULL != i->second && 1 != i->second->GetRefCount())
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgFeatureSchemaCacheItem::~MgFeatureSchemaCacheItem() - Reference Count of '%W': %d\n"),
                i->first.c_str(), i->second->GetRefCount()));
        }
#endif
        SAFE_RELEASE(i->second);
    }

    m_featureClassCacheItems.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an existing MgFeatureClassCacheItem or a newly created one
/// if it does not exist.
///
MgFeatureClassCacheItem* MgFeatureSchemaCacheItem::SetFeatureClassCacheItem(CREFSTRING classKey)
{
    Ptr<MgFeatureClassCacheItem> item = GetFeatureClassCacheItem(classKey);

    if (NULL == item.p)
    {
        item = new MgFeatureClassCacheItem();
        m_featureClassCacheItems.insert(MgFeatureClassCacheItems::value_type(
            classKey, SAFE_ADDREF(item.p)));
    }

    return item.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an existing MgFeatureClassCacheItem.
///
MgFeatureClassCacheItem* MgFeatureSchemaCacheItem::GetFeatureClassCacheItem(CREFSTRING classKey)
{
    Ptr<MgFeatureClassCacheItem> item;
    MgFeatureClassCacheItems::iterator i =
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
void MgFeatureSchemaCacheItem::SetClassNames(MgStringCollection* classNames)
{
    m_classNames = SAFE_ADDREF(classNames);
}

MgStringCollection* MgFeatureSchemaCacheItem::GetClassNames()
{
    return SAFE_ADDREF(m_classNames.p);
}

void MgFeatureSchemaCacheItem::SetSchemaXml(CREFSTRING classNames, CREFSTRING schemaXml)
{
    Ptr<MgFeatureClassCacheItem> item = SetFeatureClassCacheItem(classNames);

    item->SetSchemaXml(schemaXml);
}

STRING MgFeatureSchemaCacheItem::GetSchemaXml(CREFSTRING classNames)
{
    STRING data;
    Ptr<MgFeatureClassCacheItem> item = GetFeatureClassCacheItem(classNames);

    if (NULL != item.p)
    {
        data = item->GetSchemaXml();
    }

    return data;
}

void MgFeatureSchemaCacheItem::SetSchemas(CREFSTRING classNames, bool serialized, MgFeatureSchemaCollection* schemas)
{
    Ptr<MgFeatureClassCacheItem> item = SetFeatureClassCacheItem(classNames);

    item->SetSchemas(serialized, schemas);
}

MgFeatureSchemaCollection* MgFeatureSchemaCacheItem::GetSchemas(CREFSTRING classNames, bool serialized)
{
    Ptr<MgFeatureSchemaCollection> data;
    Ptr<MgFeatureClassCacheItem> item = GetFeatureClassCacheItem(classNames);

    if (NULL != item.p)
    {
        data = item->GetSchemas(serialized);
    }

    return data.Detach();
}

void MgFeatureSchemaCacheItem::SetClassDefinition(CREFSTRING className, MgClassDefinition* classDef)
{
    Ptr<MgFeatureClassCacheItem> item = SetFeatureClassCacheItem(className);

    item->SetClassDefinition(classDef);
}

MgClassDefinition* MgFeatureSchemaCacheItem::GetClassDefinition(CREFSTRING className)
{
    Ptr<MgClassDefinition> data;
    Ptr<MgFeatureClassCacheItem> item = GetFeatureClassCacheItem(className);

    if (NULL != item.p)
    {
        data = item->GetClassDefinition();
    }

    return data.Detach();
}

void MgFeatureSchemaCacheItem::SetClassIdentityProperties(CREFSTRING className, MgPropertyDefinitionCollection* idProperties)
{
    Ptr<MgFeatureClassCacheItem> item = SetFeatureClassCacheItem(className);

    item->SetClassIdentityProperties(idProperties);
}

MgPropertyDefinitionCollection* MgFeatureSchemaCacheItem::GetClassIdentityProperties(CREFSTRING className)
{
    Ptr<MgPropertyDefinitionCollection> data;
    Ptr<MgFeatureClassCacheItem> item = GetFeatureClassCacheItem(className);

    if (NULL != item.p)
    {
        data = item->GetClassIdentityProperties();
    }

    return data.Detach();
}
