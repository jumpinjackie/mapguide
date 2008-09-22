//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "MapGuideCommon.h"
#include "FeatureServiceCacheEntry.h"

#include <set>

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgFeatureServiceCacheEntry::MgFeatureServiceCacheEntry() :
    m_classNameHintUsed(true)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgFeatureServiceCacheEntry::~MgFeatureServiceCacheEntry()
{
    for (MgFeatureSchemaCacheItems::iterator i = m_featureSchemaCacheItems.begin();
        i != m_featureSchemaCacheItems.end(); ++i)
    {
#ifdef _DEBUG
        ACE_ASSERT(NULL != i->second);

        if (NULL != i->second && 1 != i->second->GetRefCount())
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgFeatureServiceCacheEntry::~MgFeatureServiceCacheEntry() - Reference Count of '%W': %d\n"),
                i->first.c_str(), i->second->GetRefCount()));
        }
#endif
        SAFE_RELEASE(i->second);
    }

    m_featureSchemaCacheItems.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Parse the specified qualified class name.
///
void MgFeatureServiceCacheEntry::ParseQualifiedClassName(
    CREFSTRING schemaName, CREFSTRING className,
    REFSTRING parsedSchemaName, REFSTRING parsedClassName)
{
    MgUtil::ParseQualifiedClassName(className, parsedSchemaName, parsedClassName);

    if (!schemaName.empty() && !parsedSchemaName.empty() && schemaName != parsedSchemaName)
    {
        MgStringCollection whatArguments;
        whatArguments.Add(L"1");
        whatArguments.Add(schemaName);

        MgStringCollection whyArguments;
        whyArguments.Add(className);

        throw new MgInvalidArgumentException(L"MgFeatureServiceCacheEntry.ParseQualifiedClassName",
            __LINE__, __WFILE__, &whatArguments, L"MgSchemaNameMismatch", &whyArguments);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Generate a schema key and a class key:
///
/// 1. The schema key will be either the specified schema name or the schema
///    name parsed from the only one given qualified class name.
///
/// 2. The class key will be a period (.) delimited string containing a mixture
///    of one or more qualified and unqualified class names sorted in ascending
///    order, e.g.:
///
///       "<SchemaName1>:<ClassName1>.<ClassName2>.<SchemaNameN>:<ClassNameN>"
///
/// Note that if no class name is specified or the class name hint is not used,
/// then the class key will be blank.
///
void MgFeatureServiceCacheEntry::FormatKeys(bool classNameHintUsed,
    CREFSTRING schemaName, CREFSTRING className,
    REFSTRING schemaKey, REFSTRING classKey)
{
    STRING parsedClassName;
    
    ParseQualifiedClassName(schemaName, className, schemaKey, parsedClassName);

    if (schemaKey.empty())
    {
        schemaKey = schemaName;
    }

    if (classNameHintUsed && !parsedClassName.empty())
    {
        if (schemaKey.empty())
        {
            classKey = parsedClassName;
        }
        else
        {
            MgUtil::FormatQualifiedClassName(schemaKey, parsedClassName, classKey);
        }
    }
    else
    {
        classKey = L"";
    }
}

INT32 MgFeatureServiceCacheEntry::FormatKeys(bool classNameHintUsed,
    CREFSTRING schemaName, MgStringCollection* classNames,
    REFSTRING schemaKey, REFSTRING classKey)
{
    typedef std::set<STRING, less<STRING> > MgClassKeySet;

    INT32 nClasses = 0;
    
    schemaKey = schemaName;
    classKey = L"";

    if (classNameHintUsed)
    {
        MgClassKeySet classKeys;

        if (NULL != classNames)
        {
            INT32 count = classNames->GetCount();
        
            for (INT32 i = 0; i < count; ++i)
            {
                STRING currClassName = classNames->GetItem(i);
                STRING currClassKey;

                if (schemaName.empty())
                {
                    currClassKey = currClassName;
                }
                else
                {
                    STRING parsedSchemaName;

                    FormatKeys(classNameHintUsed, schemaName, currClassName, parsedSchemaName, currClassKey);
                }

                if (!currClassKey.empty())
                {
                    classKeys.insert(currClassKey);
                }
            }                
        }

        for (MgClassKeySet::const_iterator i = classKeys.begin();
            i != classKeys.end(); ++i)
        {
            if (!classKey.empty())
            {
                classKey += MgUtil::sm_classPropertyQualifier;
            }

            classKey += *i;
        }
        
        nClasses = INT32(classKeys.size());

        if (1 == nClasses)
        {
            FormatKeys(classNameHintUsed, schemaName, *classKeys.begin(), schemaKey, classKey);
        }
    }
    else if (NULL != classNames && 1 == classNames->GetCount())
    {
        FormatKeys(classNameHintUsed, schemaName, classNames->GetItem(0), schemaKey, classKey);
    }

    return nClasses;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Find a schema based on name.
///
MgFeatureSchemaCollection* MgFeatureServiceCacheEntry::FindSchema(MgFeatureSchemaCollection* schemas, CREFSTRING schemaName)
{
    Ptr<MgFeatureSchemaCollection> data;
    
    if (NULL != schemas)
    {
        INT32 count = schemas->GetCount();

        for (INT32 i = 0; i < count; ++i)
        {
            Ptr<MgFeatureSchema> currSchema = schemas->GetItem(i);
            STRING currSchemaName = currSchema->GetName();

            if (schemaName == currSchemaName)
            {
                data = new MgFeatureSchemaCollection();
                data->Add(currSchema);
                break;
            }
        }
    }

    return data.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an existing MgFeatureSchemaCacheItem or a newly created one
/// if it does not exist.
///
MgFeatureSchemaCacheItem* MgFeatureServiceCacheEntry::SetFeatureSchemaCacheItem(CREFSTRING schemaName)
{
    Ptr<MgFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaName);
    
    if (NULL == item.p)
    {
        item = new MgFeatureSchemaCacheItem();
        m_featureSchemaCacheItems.insert(MgFeatureSchemaCacheItems::value_type(
            schemaName, SAFE_ADDREF(item.p)));
    }
    
    return item.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an existing MgFeatureSchemaCacheItem.
///
MgFeatureSchemaCacheItem* MgFeatureServiceCacheEntry::GetFeatureSchemaCacheItem(CREFSTRING schemaName)
{
    Ptr<MgFeatureSchemaCacheItem> item;
    MgFeatureSchemaCacheItems::iterator i =
        m_featureSchemaCacheItems.find(schemaName);

    if (m_featureSchemaCacheItems.end() != i)
    {
        item = SAFE_ADDREF(i->second);
    }

    return item.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Methods to manage cache items.
///
void MgFeatureServiceCacheEntry::SetFeatureSource(MgFeatureSourceCacheItem* featureSource)
{
    m_featureSource = SAFE_ADDREF(featureSource);
}

MgFeatureSourceCacheItem* MgFeatureServiceCacheEntry::GetFeatureSource()
{
    return SAFE_ADDREF(m_featureSource.p);
}

void MgFeatureServiceCacheEntry::SetSpatialContextInfo(MgSpatialContextCacheItem* spatialContextInfo)
{
    m_spatialContextInfo = SAFE_ADDREF(spatialContextInfo);
}

MgSpatialContextCacheItem* MgFeatureServiceCacheEntry::GetSpatialContextInfo()
{
    return SAFE_ADDREF(m_spatialContextInfo.p);
}

void MgFeatureServiceCacheEntry::SetSpatialContextReader(bool active, MgSpatialContextReader* spatialContextReader)
{
    if (active)
    {
        m_activeContextReader = SAFE_ADDREF(spatialContextReader);
    }
    else
    {
        m_spatialContextReader = SAFE_ADDREF(spatialContextReader);
    }
}

MgSpatialContextReader* MgFeatureServiceCacheEntry::GetSpatialContextReader(bool active)
{
    if (active)
    {
        return SAFE_ADDREF(m_activeContextReader.p);
    }
    else
    {
        return SAFE_ADDREF(m_spatialContextReader.p);
    }
}

void MgFeatureServiceCacheEntry::SetSchemaNames(MgStringCollection* schemaNames)
{
    m_schemaNames = SAFE_ADDREF(schemaNames);
}

MgStringCollection* MgFeatureServiceCacheEntry::GetSchemaNames()
{
    return SAFE_ADDREF(m_schemaNames.p);
}

void MgFeatureServiceCacheEntry::SetClassNames(CREFSTRING schemaName, MgStringCollection* classNames)
{
    Ptr<MgFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaName);

    item->SetClassNames(classNames);
}

MgStringCollection* MgFeatureServiceCacheEntry::GetClassNames(CREFSTRING schemaName)
{
    Ptr<MgStringCollection> data;
    Ptr<MgFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaName);
    
    if (NULL != item.p)
    {
        data = item->GetClassNames();
    }

    return data.Detach();
}

void MgFeatureServiceCacheEntry::SetSchemaXml(CREFSTRING schemaName, MgStringCollection* classNames, CREFSTRING schemaXml)
{
    STRING schemaKey, classKey;
    FormatKeys(m_classNameHintUsed, schemaName, classNames, schemaKey, classKey);

    Ptr<MgFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaKey);

    item->SetSchemaXml(classKey, schemaXml);
}

STRING MgFeatureServiceCacheEntry::GetSchemaXml(CREFSTRING schemaName, MgStringCollection* classNames)
{
    STRING schemaKey, classKey;
    FormatKeys(m_classNameHintUsed, schemaName, classNames, schemaKey, classKey);

    STRING data;
    Ptr<MgFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaKey);

    if (NULL != item.p)
    {
        data = item->GetSchemaXml(classKey);
    }

    return data;
}

void MgFeatureServiceCacheEntry::SetFdoSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool classNameHintUsed, FdoFeatureSchemaCollection* schemas)
{
    m_classNameHintUsed = classNameHintUsed;

    STRING schemaKey, classKey;
    FormatKeys(classNameHintUsed, schemaName, classNames, schemaKey, classKey);

    Ptr<MgFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaKey);

    item->SetFdoSchemas(classKey, schemas);
}

FdoFeatureSchemaCollection* MgFeatureServiceCacheEntry::GetFdoSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool& classNameHintUsed)
{
    classNameHintUsed = m_classNameHintUsed;

    STRING schemaKey, classKey;
    FormatKeys(m_classNameHintUsed, schemaName, classNames, schemaKey, classKey);

    FdoPtr<FdoFeatureSchemaCollection> data;
    Ptr<MgFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaKey);

    if (NULL != item.p)
    {
        data = item->GetFdoSchemas(classKey);
    }

    return data.Detach();
}

void MgFeatureServiceCacheEntry::SetSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool serialized, MgFeatureSchemaCollection* schemas)
{
    STRING schemaKey, classKey;
    FormatKeys(m_classNameHintUsed, schemaName, classNames, schemaKey, classKey);

    Ptr<MgFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaKey);

    item->SetSchemas(classKey, serialized, schemas);
}

MgFeatureSchemaCollection* MgFeatureServiceCacheEntry::GetSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool serialized)
{
    STRING schemaKey, classKey;
    INT32 nClasses = FormatKeys(m_classNameHintUsed, schemaName, classNames, schemaKey, classKey);

    Ptr<MgFeatureSchemaCollection> data;
    Ptr<MgFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaKey);

    // Get the data from the current item.
    if (NULL != item.p)
    {
        data = item->GetSchemas(classKey, serialized);
    }

    // If the data is not found, then try to find it in the superset data from the current item or superset one.
    if (NULL == data.p)
    {
        if (0 == nClasses) // no class name is given
        {
            ACE_ASSERT(classKey.empty());
            // If the schema name is specified, then try to find the data in the superset data from the superset item.
            if (!schemaKey.empty())
            {
                // Get the superset item.
                Ptr<MgFeatureSchemaCacheItem> supersetItem = GetFeatureSchemaCacheItem(L"");

                if (NULL != supersetItem.p)
                {
                    // Get the superset data from the the superset item.
                    Ptr<MgFeatureSchemaCollection> supersetData = supersetItem->GetSchemas(classKey, serialized);
                    // Find the data in the superset data.
                    data = FindSchema(supersetData.p, schemaKey);
                }
            }
        }
        else if (1 == nClasses) // one class name is given
        {
            Ptr<MgFeatureSchemaCollection> supersetData;

            // Get the superset data from the current item.
            if (NULL != item.p)
            {
                supersetData = item->GetSchemas(L"", serialized);
            }

            // If the data is not found, then try to find it in the superset data from the superset item.
            if (NULL == supersetData.p)
            {
                if (!schemaKey.empty())
                {
                    // Get the superset item.
                    Ptr<MgFeatureSchemaCacheItem> supersetItem = GetFeatureSchemaCacheItem(L"");
                    
                    if (NULL != supersetItem.p)
                    {
                        // Get the superset data from the the superset item.
                        supersetData = supersetItem->GetSchemas(classKey, serialized);
                        // Find the data in the superset data.
                        data = FindSchema(supersetData.p, schemaKey);
                    }
                }
            }
            else if (1 == supersetData->GetCount())
            {
                data = supersetData;
            }
        }

        // Update the cache with the found data.
        if (NULL != data.p)
        {
            SetSchemas(schemaKey, classNames, serialized, data.p);
        }
    }

    return data.Detach();
}

void MgFeatureServiceCacheEntry::SetClassDefinition(CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDef)
{
    STRING schemaKey, classKey;
    FormatKeys(true, schemaName, className, schemaKey, classKey);

    Ptr<MgFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaKey);

    item->SetClassDefinition(classKey, classDef);
}

MgClassDefinition* MgFeatureServiceCacheEntry::GetClassDefinition(CREFSTRING schemaName, CREFSTRING className)
{
    STRING schemaKey, classKey;
    FormatKeys(true, schemaName, className, schemaKey, classKey);

    Ptr<MgClassDefinition> data;
    Ptr<MgFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaKey);

    if (NULL != item.p)
    {
        data = item->GetClassDefinition(classKey);
    }

    return data.Detach();
}

void MgFeatureServiceCacheEntry::SetClassIdentityProperties(CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* idProperties)
{
    STRING schemaKey, classKey;
    FormatKeys(true, schemaName, className, schemaKey, classKey);

    Ptr<MgFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaKey);

    item->SetClassIdentityProperties(classKey, idProperties);
}

MgPropertyDefinitionCollection* MgFeatureServiceCacheEntry::GetClassIdentityProperties(CREFSTRING schemaName, CREFSTRING className)
{
    STRING schemaKey, classKey;
    FormatKeys(true, schemaName, className, schemaKey, classKey);

    Ptr<MgPropertyDefinitionCollection> data;
    Ptr<MgFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaKey);

    if (NULL != item.p)
    {
        data = item->GetClassIdentityProperties(classKey);
    }

    return data.Detach();
}
