#include "MgDesktop.h"
#include "FeatureClassCacheItem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgdFeatureClassCacheItem::MgdFeatureClassCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgdFeatureClassCacheItem::~MgdFeatureClassCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Methods to manage cache data.
///
void MgdFeatureClassCacheItem::SetSchemaXml(CREFSTRING schemaXml)
{
    m_schemaXml = schemaXml;
}

STRING MgdFeatureClassCacheItem::GetSchemaXml()
{
    return m_schemaXml;
}

void MgdFeatureClassCacheItem::SetSchemas(bool serialized, MgFeatureSchemaCollection* schemas)
{
    if (serialized)
    {
        m_serializedSchemas = SAFE_ADDREF(schemas);
    }
    else
    {
        m_unserializedSchemas = SAFE_ADDREF(schemas);
    }
}

MgFeatureSchemaCollection* MgdFeatureClassCacheItem::GetSchemas(bool serialized)
{
    if (serialized)
    {
        return SAFE_ADDREF(m_serializedSchemas.p);
    }
    else
    {
        return SAFE_ADDREF(m_unserializedSchemas.p);
    }
}

void MgdFeatureClassCacheItem::SetClassDefinition(MgClassDefinition* classDef)
{
    m_classDef = SAFE_ADDREF(classDef);
}

MgClassDefinition* MgdFeatureClassCacheItem::GetClassDefinition()
{
    return SAFE_ADDREF(m_classDef.p);
}

void MgdFeatureClassCacheItem::SetClassIdentityProperties(MgPropertyDefinitionCollection* idProperties)
{
    m_idProperties = SAFE_ADDREF(idProperties);
}

MgPropertyDefinitionCollection* MgdFeatureClassCacheItem::GetClassIdentityProperties()
{
    return SAFE_ADDREF(m_idProperties.p);
}
