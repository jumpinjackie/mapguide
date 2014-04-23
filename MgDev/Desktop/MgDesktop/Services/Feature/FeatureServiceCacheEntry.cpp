#include "MgDesktop.h"
#include "FeatureServiceCacheEntry.h"


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgdFeatureServiceCacheEntry::MgdFeatureServiceCacheEntry() :
    m_classNameHintUsed(true)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgdFeatureServiceCacheEntry::~MgdFeatureServiceCacheEntry()
{
    for (MgdFeatureSchemaCacheItems::iterator i = m_featureSchemaCacheItems.begin();
        i != m_featureSchemaCacheItems.end(); ++i)
    {
#ifdef _DEBUG
        ACE_ASSERT(NULL != i->second);

        if (NULL != i->second && 1 != i->second->GetRefCount())
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgdFeatureServiceCacheEntry::~MgdFeatureServiceCacheEntry() - Reference Count of '%W': %d\n"),
                i->first.c_str(), i->second->GetRefCount()));
        }
#endif
        SAFE_RELEASE(i->second);
    }

    m_featureSchemaCacheItems.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an existing MgdFeatureSchemaCacheItem or a newly created one
/// if it does not exist.
///
MgdFeatureSchemaCacheItem* MgdFeatureServiceCacheEntry::SetFeatureSchemaCacheItem(CREFSTRING schemaName)
{
    Ptr<MgdFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaName);

    if (NULL == item.p)
    {
        item = new MgdFeatureSchemaCacheItem();
        m_featureSchemaCacheItems.insert(MgdFeatureSchemaCacheItems::value_type(
            schemaName, SAFE_ADDREF(item.p)));
    }

    return item.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an existing MgdFeatureSchemaCacheItem.
///
MgdFeatureSchemaCacheItem* MgdFeatureServiceCacheEntry::GetFeatureSchemaCacheItem(CREFSTRING schemaName)
{
    Ptr<MgdFeatureSchemaCacheItem> item;
    MgdFeatureSchemaCacheItems::iterator i =
        m_featureSchemaCacheItems.find(schemaName);

    if (m_featureSchemaCacheItems.end() != i)
    {
        item = SAFE_ADDREF(i->second);
    }

    return item.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Parse the specified qualified class name.
///
void MgdFeatureServiceCacheEntry::ParseQualifiedClassName(
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

        throw new MgInvalidArgumentException(L"MgdFeatureServiceCacheEntry.ParseQualifiedClassName",
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
void MgdFeatureServiceCacheEntry::FormatKeys(bool classNameHintUsed,
    CREFSTRING schemaName, CREFSTRING className,
    REFSTRING schemaKey, REFSTRING classKey)
{
    ParseQualifiedClassName(schemaName, className, schemaKey, classKey);

    if (classNameHintUsed)
    {
        // When the class name hint is used, the schema will be cached based
        // on a mixture of one or more qualified and unqualified class names.
        if (schemaKey.empty())
        {
            if (!schemaName.empty())
            {
                schemaKey = schemaName;

                if (!classKey.empty())
                {
                    MgUtil::FormatQualifiedClassName(schemaKey, STRING(classKey), classKey);
                }
            }
        }
        else if (!classKey.empty())
        {
            classKey = className;
        }
    }
    else
    {
        // When the class name hint is used, the schema will be cached based
        // on a blank class key.
        if (schemaKey.empty())
        {
            schemaKey = schemaName;
        }

        classKey = L"";
    }
}

INT32 MgdFeatureServiceCacheEntry::FormatKeys(bool classNameHintUsed,
    CREFSTRING schemaName, MgStringCollection* classNames,
    REFSTRING schemaKey, REFSTRING classKey)
{
    typedef std::set<STRING, less<STRING> > MgClassKeySet;

    schemaKey = schemaName;
    classKey = L"";

    INT32 classCount = (NULL == classNames) ? 0 : classNames->GetCount();

    if (1 == classCount)
    {
        FormatKeys(classNameHintUsed, schemaName, classNames->GetItem(0), schemaKey, classKey);
    }
    else if (classNameHintUsed)
    {
        // Use a sorted key set to avoid duplicate class names and ensure uniqueness.
        MgClassKeySet classKeys;
        bool hasOneSchema = true;
        STRING savedSchemaKey;

        for (INT32 i = 0; i < classCount; ++i)
        {
            STRING currClassName = classNames->GetItem(i);
            STRING currSchemaKey, currClassKey;

            if (schemaName.empty())
            {
                MgUtil::ParseQualifiedClassName(currClassName, currSchemaKey, currClassKey);

                if (!currClassKey.empty())
                {
                    if (0 == classKeys.size())
                    {
                        savedSchemaKey = currSchemaKey;
                    }
                    else if (hasOneSchema && currSchemaKey != savedSchemaKey)
                    {
                        hasOneSchema = false;
                    }

                    classKeys.insert(currClassName);
                }
            }
            else
            {
                FormatKeys(classNameHintUsed, schemaName, currClassName, currSchemaKey, currClassKey);

                if (!currClassKey.empty())
                {
                    classKeys.insert(currClassKey);
                }
            }
        }

        // Convert a class name collection to a period (.) delimited string.
        for (MgClassKeySet::const_iterator i = classKeys.begin();
            i != classKeys.end(); ++i)
        {
            if (!classKey.empty())
            {
                classKey += MgUtil::sm_classPropertyQualifier;
            }

            classKey += *i;
        }

        classCount = INT32(classKeys.size());

        if (1 == classCount)
        {
            FormatKeys(classNameHintUsed, schemaName, *classKeys.begin(), schemaKey, classKey);
        }
        else if (schemaKey.empty() && hasOneSchema)
        {
            schemaKey = savedSchemaKey;
        }
    }

    if (classKey.empty())
    {
        classCount = 0;
    }

    return classCount;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Find a schema based on name.
///
MgFeatureSchemaCollection* MgdFeatureServiceCacheEntry::FindSchema(MgFeatureSchemaCollection* schemas, CREFSTRING schemaName)
{
    Ptr<MgFeatureSchemaCollection> data;

    if (NULL != schemas)
    {
        INT32 schemaCount = schemas->GetCount();

        for (INT32 i = 0; i < schemaCount; ++i)
        {
            Ptr<MgFeatureSchema> currSchema = schemas->GetItem(i);
            STRING currSchemaName = currSchema->GetName();

            // Make a schema copy when a match is found.
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
/// Find a schema in a superset data.
///
MgFeatureSchemaCollection* MgdFeatureServiceCacheEntry::FindSchema(CREFSTRING schemaKey, CREFSTRING classKey, bool serialized, INT32 classCount)
{
    Ptr<MgFeatureSchemaCollection> data;

    // If the schema key is specified, then try to find the data in the superset data from the superset item.
    if (!schemaKey.empty())
    {
        // Get the superset item.
        Ptr<MgdFeatureSchemaCacheItem> supersetItem = GetFeatureSchemaCacheItem(L"");

        if (NULL != supersetItem.p)
        {
            // Get the superset data from the the superset item.
            Ptr<MgFeatureSchemaCollection> supersetData = supersetItem->GetSchemas(classKey, serialized);
            // Find the data in the superset data.
            data = FindSchema(supersetData.p, schemaKey);

            // If the data is not found and the class count is 1,
            // then try using the unqualified class name instead of.
            if (NULL == data.p && 1 == classCount)
            {
                STRING parsedSchemaName, parsedClassName;

                MgUtil::ParseQualifiedClassName(classKey, parsedSchemaName, parsedClassName);

                // Determine if the superset data from the superset item is reusable.
                if (!parsedSchemaName.empty())
                {
                    supersetData = supersetItem->GetSchemas(parsedClassName, serialized);

                    if (FindClass(supersetData.p, parsedClassName))
                    {
                        data = supersetData;
                    }
                }
            }
        }
    }

    return data.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determine if the specified schemas only contain the specified class.
///
bool MgdFeatureServiceCacheEntry::FindClass(MgFeatureSchemaCollection* schemas, CREFSTRING className)
{
    bool found = false;

    if (NULL != schemas && 1 == schemas->GetCount())
    {
        Ptr<MgFeatureSchema> firstSchema = schemas->GetItem(0);

        if (NULL != firstSchema)
        {
            Ptr<MgClassDefinitionCollection> classes = firstSchema->GetClasses();

            if (NULL != classes && 1 == classes->GetCount())
            {
                Ptr<MgClassDefinition> firstClass = classes->GetItem(0);

                if (NULL != firstClass)
                {
                    STRING parsedSchemaName1, parsedClassName1;
                    STRING parsedSchemaName2, parsedClassName2;

                    MgUtil::ParseQualifiedClassName(className, parsedSchemaName1, parsedClassName1);
                    MgUtil::ParseQualifiedClassName(firstClass->GetName(), parsedSchemaName2, parsedClassName2);

                    if (parsedClassName1 == parsedClassName2)
                    {
                        found = true;
                    }
                }
            }
        }
    }

    return found;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Methods to manage cache items.
///
void MgdFeatureServiceCacheEntry::SetFeatureSource(MgdFeatureSourceCacheItem* featureSource)
{
    m_featureSource = SAFE_ADDREF(featureSource);
}

MgdFeatureSourceCacheItem* MgdFeatureServiceCacheEntry::GetFeatureSource()
{
    return SAFE_ADDREF(m_featureSource.p);
}

void MgdFeatureServiceCacheEntry::SetSpatialContextInfo(MgdSpatialContextCacheItem* spatialContextInfo)
{
    m_spatialContextInfo = SAFE_ADDREF(spatialContextInfo);
}

MgdSpatialContextCacheItem* MgdFeatureServiceCacheEntry::GetSpatialContextInfo()
{
    return SAFE_ADDREF(m_spatialContextInfo.p);
}

void MgdFeatureServiceCacheEntry::SetSpatialContextReader(MgSpatialContextReader* spatialContextReader)
{
    m_spatialContextReader = SAFE_ADDREF(spatialContextReader);
}

MgSpatialContextReader* MgdFeatureServiceCacheEntry::GetSpatialContextReader()
{
    return SAFE_ADDREF(m_spatialContextReader.p);
}

void MgdFeatureServiceCacheEntry::SetSchemaNames(MgStringCollection* schemaNames)
{
    m_schemaNames = SAFE_ADDREF(schemaNames);
}

MgStringCollection* MgdFeatureServiceCacheEntry::GetSchemaNames()
{
    return SAFE_ADDREF(m_schemaNames.p);
}

void MgdFeatureServiceCacheEntry::SetClassNames(CREFSTRING schemaName, MgStringCollection* classNames)
{
    Ptr<MgdFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaName);

    item->SetClassNames(classNames);
}

MgStringCollection* MgdFeatureServiceCacheEntry::GetClassNames(CREFSTRING schemaName)
{
    Ptr<MgStringCollection> data;
    Ptr<MgdFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaName);

    if (NULL != item.p)
    {
        data = item->GetClassNames();
    }

    return data.Detach();
}

void MgdFeatureServiceCacheEntry::SetSchemaXml(CREFSTRING schemaName, MgStringCollection* classNames, CREFSTRING schemaXml)
{
    STRING schemaKey, classKey;
    FormatKeys(m_classNameHintUsed, schemaName, classNames, schemaKey, classKey);

    Ptr<MgdFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaKey);

    item->SetSchemaXml(classKey, schemaXml);
}

STRING MgdFeatureServiceCacheEntry::GetSchemaXml(CREFSTRING schemaName, MgStringCollection* classNames)
{
    STRING schemaKey, classKey;
    FormatKeys(m_classNameHintUsed, schemaName, classNames, schemaKey, classKey);

    STRING data;
    Ptr<MgdFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaKey);

    if (NULL != item.p)
    {
        data = item->GetSchemaXml(classKey);
    }

    return data;
}

void MgdFeatureServiceCacheEntry::SetSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool serialized, MgFeatureSchemaCollection* schemas)
{
    STRING schemaKey, classKey;
    FormatKeys(m_classNameHintUsed, schemaName, classNames, schemaKey, classKey);

    Ptr<MgdFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaKey);

    item->SetSchemas(classKey, serialized, schemas);
}

MgFeatureSchemaCollection* MgdFeatureServiceCacheEntry::GetSchemas(CREFSTRING schemaName, MgStringCollection* classNames, bool serialized)
{
    STRING schemaKey, classKey;
    INT32 classCount = FormatKeys(m_classNameHintUsed, schemaName, classNames, schemaKey, classKey);

    Ptr<MgFeatureSchemaCollection> data;
    Ptr<MgdFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaKey);

    // Get the data from the current item.
    if (NULL != item.p)
    {
        data = item->GetSchemas(classKey, serialized);
    }

    // If the data is not found, then try to find it in the superset data from the current item or superset one.
    if (NULL == data.p)
    {
        if (0 == classCount)
        {
            ACE_ASSERT(classKey.empty());
            // If there is no class, then try to find the data in the superset data from the superset item.
            data = FindSchema(schemaKey, classKey, serialized, classCount);
        }
        else if (1 == classCount)
        {
            Ptr<MgFeatureSchemaCollection> supersetData;

            // Get the superset data from the current item.
            if (NULL != item.p && !classKey.empty())
            {
                supersetData = item->GetSchemas(L"", serialized);
            }

            // If the data is not found, then try to find it in the superset data from the superset item.
            if (NULL == supersetData.p)
            {
                data = FindSchema(schemaKey, classKey, serialized, classCount);
            }
            // Otherwise, determine if the superset data from the current item is reusable.
            else if (FindClass(supersetData.p, classKey))
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

void MgdFeatureServiceCacheEntry::SetClassDefinition(CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDef)
{
    STRING schemaKey, classKey;
    // Since this method requires a specific class name, the hint must be explicitly used.
    FormatKeys(true, schemaName, className, schemaKey, classKey);

    Ptr<MgdFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaKey);

    item->SetClassDefinition(classKey, classDef);
}

MgClassDefinition* MgdFeatureServiceCacheEntry::GetClassDefinition(CREFSTRING schemaName, CREFSTRING className)
{
    STRING schemaKey, classKey;
    // Since this method requires a specific class name, the hint must be explicitly used.
    FormatKeys(true, schemaName, className, schemaKey, classKey);

    Ptr<MgClassDefinition> data;
    Ptr<MgdFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaKey);

    if (NULL != item.p)
    {
        data = item->GetClassDefinition(classKey);
    }

    return data.Detach();
}

void MgdFeatureServiceCacheEntry::SetClassIdentityProperties(CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* idProperties)
{
    STRING schemaKey, classKey;
    // Since this method requires a specific class name, the hint must be explicitly used.
    FormatKeys(true, schemaName, className, schemaKey, classKey);

    Ptr<MgdFeatureSchemaCacheItem> item = SetFeatureSchemaCacheItem(schemaKey);

    item->SetClassIdentityProperties(classKey, idProperties);
}

MgPropertyDefinitionCollection* MgdFeatureServiceCacheEntry::GetClassIdentityProperties(CREFSTRING schemaName, CREFSTRING className)
{
    STRING schemaKey, classKey;
    // Since this method requires a specific class name, the hint must be explicitly used.
    FormatKeys(true, schemaName, className, schemaKey, classKey);

    Ptr<MgPropertyDefinitionCollection> data;
    Ptr<MgdFeatureSchemaCacheItem> item = GetFeatureSchemaCacheItem(schemaKey);

    if (NULL != item.p)
    {
        data = item->GetClassIdentityProperties(classKey);
    }

    return data.Detach();
}
