#include "MgDesktop.h"
#include "FeatureClassCacheItem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgFeatureClassCacheItem::MgFeatureClassCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgFeatureClassCacheItem::~MgFeatureClassCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Methods to manage cache data.
///
void MgFeatureClassCacheItem::SetSchemaXml(CREFSTRING schemaXml)
{
    m_schemaXml = schemaXml;
}

STRING MgFeatureClassCacheItem::GetSchemaXml()
{
    return m_schemaXml;
}

void MgFeatureClassCacheItem::SetSchemas(bool serialized, MgFeatureSchemaCollection* schemas)
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

MgFeatureSchemaCollection* MgFeatureClassCacheItem::GetSchemas(bool serialized)
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

void MgFeatureClassCacheItem::SetClassDefinition(MgClassDefinition* classDef)
{
    m_classDef = SAFE_ADDREF(classDef);
}

MgClassDefinition* MgFeatureClassCacheItem::GetClassDefinition()
{
    return SAFE_ADDREF(m_classDef.p);
}

void MgFeatureClassCacheItem::SetClassIdentityProperties(MgPropertyDefinitionCollection* idProperties)
{
    m_idProperties = SAFE_ADDREF(idProperties);
}

MgPropertyDefinitionCollection* MgFeatureClassCacheItem::GetClassIdentityProperties()
{
    return SAFE_ADDREF(m_idProperties.p);
}
