#include "MgDesktop.h"
#include "FeatureServiceCache.h"

Ptr<MgFeatureServiceCache> MgFeatureServiceCache::smInstance = (MgFeatureServiceCache*)NULL;

MgFeatureServiceCache::MgFeatureServiceCache() { }

MgFeatureServiceCache::~MgFeatureServiceCache()
{
    MG_TRY()

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgFeatureServiceCache::~MgFeatureServiceCache()\n")));

    Clear();

    MG_CATCH_AND_RELEASE()
}

MgFeatureServiceCache* MgFeatureServiceCache::GetInstance()
{
    if (NULL == MgFeatureServiceCache::smInstance)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (NULL == MgFeatureServiceCache::smInstance)
        {
            MgFeatureServiceCache::smInstance = new MgFeatureServiceCache();
        }
    }
    return smInstance;
}

void MgFeatureServiceCache::Clear()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    for (MgFeatureServiceCacheEntries::iterator i = m_featureServiceCacheEntries.begin();
        i != m_featureServiceCacheEntries.end(); ++i)
    {
#ifdef _DEBUG
        ACE_ASSERT(NULL != i->second);

        if (NULL != i->second && 1 != i->second->GetRefCount())
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgFeatureServiceCache::Clear() - Reference Count of '%W': %d\n"),
                i->first.c_str(), i->second->GetRefCount()));
        }
#endif
        SAFE_RELEASE(i->second);
    }

    m_featureServiceCacheEntries.clear();
}

void MgFeatureServiceCache::RemoveEntry(CREFSTRING resource)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    MgFeatureServiceCacheEntries::iterator i =
        m_featureServiceCacheEntries.find(resource);

    if (m_featureServiceCacheEntries.end() != i)
    {
        SAFE_RELEASE(i->second);
        m_featureServiceCacheEntries.erase(i);
    }
}

void MgFeatureServiceCache::RemoveEntry(MgResourceIdentifier* resource)
{
    if (NULL != resource)
    {
        RemoveEntry(resource->ToString());
    }
}

MgFeatureServiceCacheEntry* MgFeatureServiceCache::SetEntry(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgFeatureServiceCacheEntry> entry;

    entry = GetEntry(resource);
    if (NULL == entry.p)
    {
        //Compact();

        entry = new MgFeatureServiceCacheEntry();
        m_featureServiceCacheEntries.insert(MgFeatureServiceCacheEntries::value_type(
            resource->ToString(), SAFE_ADDREF(entry.p)));
    }

    return entry.Detach();
}

MgFeatureServiceCacheEntry* MgFeatureServiceCache::GetEntry(MgResourceIdentifier* resource)
{
    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgFeatureServiceCache::GetEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    resource->Validate();

    if (!resource->IsResourceTypeOf(MgResourceType::FeatureSource))
    {
        throw new MgInvalidResourceTypeException(
            L"MgFeatureServiceCache::GetEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgFeatureServiceCacheEntry> entry;
    MgFeatureServiceCacheEntries::iterator i =
        m_featureServiceCacheEntries.find(resource->ToString());

    if (m_featureServiceCacheEntries.end() != i)
    {
        entry = SAFE_ADDREF(i->second);
        //entry->UpdateTimestamp();
    }

    return entry.Detach();
}

void MgFeatureServiceCache::SetFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceCacheItem* featureSource)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetFeatureSource(featureSource);
}

MgFeatureSourceCacheItem* MgFeatureServiceCache::GetFeatureSourceInternal(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgFeatureSourceCacheItem> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetFeatureSource();
    }

    return data.Detach();
}
void MgFeatureServiceCache::SetSpatialContextInfo(MgResourceIdentifier* resource, MgSpatialContextCacheItem* spatialContextInfo)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetSpatialContextInfo(spatialContextInfo);
}

MgSpatialContextCacheItem* MgFeatureServiceCache::GetSpatialContextInfoInternal(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgSpatialContextCacheItem> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetSpatialContextInfo();
    }

    return data.Detach();
}

void MgFeatureServiceCache::SetSpatialContextReader(MgResourceIdentifier* resource, MgSpatialContextReader* spatialContextReader)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetSpatialContextReader(spatialContextReader);
}

MgSpatialContextReader* MgFeatureServiceCache::GetSpatialContextReader(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgSpatialContextReader> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetSpatialContextReader();

        // Make sure this cached data is only used by one thread at a time.
        if (NULL != data.p && data->GetRefCount() > 2)
        {
            data = NULL;
        }

        // Reset the reader.
        if (NULL != data.p)
        {
            data->Reset();
        }
    }

    return data.Detach();
}

void MgFeatureServiceCache::SetSchemaNames(MgResourceIdentifier* resource, MgStringCollection* schemaNames)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetSchemaNames(schemaNames);
}

MgStringCollection* MgFeatureServiceCache::GetSchemaNames(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgStringCollection> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetSchemaNames();
    }

    return data.Detach();
}

void MgFeatureServiceCache::SetClassNames(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetClassNames(schemaName, classNames);
}

MgStringCollection* MgFeatureServiceCache::GetClassNames(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgStringCollection> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetClassNames(schemaName);
    }

    return data.Detach();
}

void MgFeatureServiceCache::SetSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, CREFSTRING schemaXml)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetSchemaXml(schemaName, classNames, schemaXml);
}

STRING MgFeatureServiceCache::GetSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));

    STRING data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetSchemaXml(schemaName, classNames);
    }

    return data;
}

void MgFeatureServiceCache::SetSchemas(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, bool serialized, MgFeatureSchemaCollection* schemas)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetSchemas(schemaName, classNames, serialized, schemas);
}

MgFeatureSchemaCollection* MgFeatureServiceCache::GetSchemas(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, bool serialized)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgFeatureSchemaCollection> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetSchemas(schemaName, classNames, serialized);
    }

    return data.Detach();
}

void MgFeatureServiceCache::SetClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDef)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetClassDefinition(schemaName, className, classDef);
}

MgClassDefinition* MgFeatureServiceCache::GetClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgClassDefinition> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetClassDefinition(schemaName, className);
    }

    return data.Detach();
}

void MgFeatureServiceCache::SetClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* idProperties)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetClassIdentityProperties(schemaName, className, idProperties);
}

MgPropertyDefinitionCollection* MgFeatureServiceCache::GetClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgPropertyDefinitionCollection> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetClassIdentityProperties(schemaName, className);
    }

    return data.Detach();
}

MgFeatureSourceCacheItem* MgFeatureServiceCache::GetFeatureSource(MgResourceIdentifier* resource)
{
    Ptr<MgFeatureSourceCacheItem> cacheItem;

    MG_TRY()

    cacheItem = GetInstance()->GetFeatureSourceInternal(resource);

    if (NULL == cacheItem.p)
    {
        // Get the Resource Service.
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> resourceService = static_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        // Retrieve the XML document from the repository.
        string xmlContent;
        Ptr<MgByteReader> byteReader = resourceService->GetResourceContent(
            resource, MgResourcePreProcessingType::Substitution);

        byteReader->ToStringUtf8(xmlContent);

        // Validate the XML content and parse it.
        MdfParser::FSDSAX2Parser parser;

        if (string::npos != xmlContent.find("<FeatureSource"))
        {
            parser.ParseString(xmlContent.c_str(), xmlContent.length()*sizeof(char));
            if (!parser.GetSucceeded())
            {
                STRING errorMsg = parser.GetErrorMessage();
                MgStringCollection arguments;
                arguments.Add(errorMsg);
                throw new MgInvalidFeatureSourceException(
                    L"MgCacheManager::GetFeatureSourceCacheItem",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }

        // Detach the feature source from the parser.
        // It's now the caller's responsibility to delete it.
        auto_ptr<MdfModel::FeatureSource> featureSource;
        featureSource.reset(parser.DetachFeatureSource());

        if (NULL == featureSource.get())
        {
            MgResources* resources = MgResources::GetInstance();
            ACE_ASSERT(NULL != resources);
            STRING message = resources->GetResourceMessage(MgResources::FeatureService,
                L"MgInvalidFdoProvider", NULL);
            MgStringCollection arguments;
            arguments.Add(message);

            throw new MgInvalidFeatureSourceException(
                L"MgCacheManager.GetFeatureSourceCacheItem",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        cacheItem = new MgFeatureSourceCacheItem(featureSource.release());
        GetInstance()->SetFeatureSource(resource, cacheItem.p);
    }
    else
    {
        //CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_CATCH_AND_THROW(L"MgCacheManager.GetFeatureSourceCacheItem")

    return cacheItem.Detach();
}

MgSpatialContextCacheItem* MgFeatureServiceCache::GetSpatialContextInfo(MgResourceIdentifier* resource)
{
    Ptr<MgSpatialContextCacheItem> cacheItem;

    MG_TRY()

    cacheItem = GetInstance()->GetSpatialContextInfoInternal(resource);

    if (NULL == cacheItem.p)
    {
        Ptr<MgFeatureSourceCacheItem> featureSourceCacheItem = GetFeatureSource(resource);
        MdfModel::FeatureSource* featureSource = featureSourceCacheItem->Get();
        CHECKNULL(featureSource, L"MgCacheManager.GetSpatialContextCacheItem");

        MdfModel::SupplementalSpatialContextInfoCollection* spatialContexts =
            featureSource->GetSupplementalSpatialContextInfo();
        CHECKNULL(spatialContexts, L"MgCacheManager.GetSpatialContextCacheItem");

        auto_ptr<MgSpatialContextInfo> spatialContextInfo;
        spatialContextInfo.reset(new MgSpatialContextInfo());

        for (int i = 0; i < spatialContexts->GetCount(); ++i)
        {
            MdfModel::SupplementalSpatialContextInfo* spatialContext = spatialContexts->GetAt(i);
            CHECKNULL(spatialContext, L"MgCacheManager.GetSpatialContextCacheItem");

            // Get Name element.
            STRING name = (STRING)spatialContext->GetName();
            FdoString* propertyName = name.c_str();
            CHECKNULL(propertyName, L"MgCacheManager.GetSpatialContextCacheItem");

            // Get Coordinate System element.
            STRING coordinateSystem = (STRING)spatialContext->GetCoordinateSystem();
            FdoString* propertyCoordinateSystem = coordinateSystem.c_str();
            CHECKNULL(propertyCoordinateSystem, L"MgCacheManager.GetSpatialContextCacheItem");

            spatialContextInfo->insert(MgSpatialContextInfo::value_type(name, coordinateSystem));
        }

        cacheItem = new MgSpatialContextCacheItem(spatialContextInfo.release());
        GetInstance()->SetSpatialContextInfo(resource, cacheItem.p);
    }
    else
    {
        //CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_CATCH_AND_THROW(L"MgCacheManager.GetSpatialContextCacheItem")

    return cacheItem.Detach();
}