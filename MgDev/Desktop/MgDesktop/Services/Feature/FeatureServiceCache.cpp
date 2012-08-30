#include "MgDesktop.h"
#include "FeatureServiceCache.h"

Ptr<MgdFeatureServiceCache> MgdFeatureServiceCache::smInstance = (MgdFeatureServiceCache*)NULL;

MgdFeatureServiceCache::MgdFeatureServiceCache() { }

MgdFeatureServiceCache::~MgdFeatureServiceCache()
{
    MG_TRY()

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgdFeatureServiceCache::~MgdFeatureServiceCache()\n")));

    Clear();

    MG_CATCH_AND_RELEASE()
}

MgdFeatureServiceCache* MgdFeatureServiceCache::GetInstance()
{
    if (NULL == MgdFeatureServiceCache::smInstance)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (NULL == MgdFeatureServiceCache::smInstance)
        {
            MgdFeatureServiceCache::smInstance = new MgdFeatureServiceCache();
        }
    }
    return smInstance;
}

void MgdFeatureServiceCache::Clear()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    for (MgdFeatureServiceCacheEntries::iterator i = m_featureServiceCacheEntries.begin();
        i != m_featureServiceCacheEntries.end(); ++i)
    {
#ifdef _DEBUG
        ACE_ASSERT(NULL != i->second);

        if (NULL != i->second && 1 != i->second->GetRefCount())
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgdFeatureServiceCache::Clear() - Reference Count of '%W': %d\n"),
                i->first.c_str(), i->second->GetRefCount()));
        }
#endif
        SAFE_RELEASE(i->second);
    }

    m_featureServiceCacheEntries.clear();
}

void MgdFeatureServiceCache::RemoveEntry(CREFSTRING resource)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    MgdFeatureServiceCacheEntries::iterator i =
        m_featureServiceCacheEntries.find(resource);

    if (m_featureServiceCacheEntries.end() != i)
    {
        SAFE_RELEASE(i->second);
        m_featureServiceCacheEntries.erase(i);
    }
}

void MgdFeatureServiceCache::RemoveEntry(MgResourceIdentifier* resource)
{
    if (NULL != resource)
    {
        RemoveEntry(resource->ToString());
    }
}

MgdFeatureServiceCacheEntry* MgdFeatureServiceCache::SetEntry(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgdFeatureServiceCacheEntry> entry;

    entry = GetEntry(resource);
    if (NULL == entry.p)
    {
        //Compact();

        entry = new MgdFeatureServiceCacheEntry();
        m_featureServiceCacheEntries.insert(MgdFeatureServiceCacheEntries::value_type(
            resource->ToString(), SAFE_ADDREF(entry.p)));
    }

    return entry.Detach();
}

MgdFeatureServiceCacheEntry* MgdFeatureServiceCache::GetEntry(MgResourceIdentifier* resource)
{
    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgdFeatureServiceCache::GetEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    resource->Validate();

    if (!resource->IsResourceTypeOf(MgResourceType::FeatureSource))
    {
        throw new MgInvalidResourceTypeException(
            L"MgdFeatureServiceCache::GetEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgdFeatureServiceCacheEntry> entry;
    MgdFeatureServiceCacheEntries::iterator i =
        m_featureServiceCacheEntries.find(resource->ToString());

    if (m_featureServiceCacheEntries.end() != i)
    {
        entry = SAFE_ADDREF(i->second);
        //entry->UpdateTimestamp();
    }

    return entry.Detach();
}

void MgdFeatureServiceCache::SetFeatureSource(MgResourceIdentifier* resource, MgdFeatureSourceCacheItem* featureSource)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    Ptr<MgdFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetFeatureSource(featureSource);
}

MgdFeatureSourceCacheItem* MgdFeatureServiceCache::GetFeatureSourceInternal(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgdFeatureSourceCacheItem> data;
    Ptr<MgdFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetFeatureSource();
    }

    return data.Detach();
}
void MgdFeatureServiceCache::SetSpatialContextInfo(MgResourceIdentifier* resource, MgdSpatialContextCacheItem* spatialContextInfo)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    Ptr<MgdFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetSpatialContextInfo(spatialContextInfo);
}

MgdSpatialContextCacheItem* MgdFeatureServiceCache::GetSpatialContextInfoInternal(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgdSpatialContextCacheItem> data;
    Ptr<MgdFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetSpatialContextInfo();
    }

    return data.Detach();
}

void MgdFeatureServiceCache::SetSpatialContextReader(MgResourceIdentifier* resource, MgSpatialContextReader* spatialContextReader)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    Ptr<MgdFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetSpatialContextReader(spatialContextReader);
}

MgSpatialContextReader* MgdFeatureServiceCache::GetSpatialContextReader(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgSpatialContextReader> data;
    Ptr<MgdFeatureServiceCacheEntry> entry = GetEntry(resource);

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

void MgdFeatureServiceCache::SetSchemaNames(MgResourceIdentifier* resource, MgStringCollection* schemaNames)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    Ptr<MgdFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetSchemaNames(schemaNames);
}

MgStringCollection* MgdFeatureServiceCache::GetSchemaNames(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgStringCollection> data;
    Ptr<MgdFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetSchemaNames();
    }

    return data.Detach();
}

void MgdFeatureServiceCache::SetClassNames(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    Ptr<MgdFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetClassNames(schemaName, classNames);
}

MgStringCollection* MgdFeatureServiceCache::GetClassNames(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgStringCollection> data;
    Ptr<MgdFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetClassNames(schemaName);
    }

    return data.Detach();
}

void MgdFeatureServiceCache::SetSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, CREFSTRING schemaXml)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    Ptr<MgdFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetSchemaXml(schemaName, classNames, schemaXml);
}

STRING MgdFeatureServiceCache::GetSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, L""));

    STRING data;
    Ptr<MgdFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetSchemaXml(schemaName, classNames);
    }

    return data;
}

void MgdFeatureServiceCache::SetSchemas(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, bool serialized, MgFeatureSchemaCollection* schemas)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    Ptr<MgdFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetSchemas(schemaName, classNames, serialized, schemas);
}

MgFeatureSchemaCollection* MgdFeatureServiceCache::GetSchemas(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames, bool serialized)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgFeatureSchemaCollection> data;
    Ptr<MgdFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetSchemas(schemaName, classNames, serialized);
    }

    return data.Detach();
}

void MgdFeatureServiceCache::SetClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDef)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    Ptr<MgdFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetClassDefinition(schemaName, className, classDef);
}

MgClassDefinition* MgdFeatureServiceCache::GetClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgClassDefinition> data;
    Ptr<MgdFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetClassDefinition(schemaName, className);
    }

    return data.Detach();
}

void MgdFeatureServiceCache::SetClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* idProperties)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));

    Ptr<MgdFeatureServiceCacheEntry> entry = SetEntry(resource);

    entry->SetClassIdentityProperties(schemaName, className, idProperties);
}

MgPropertyDefinitionCollection* MgdFeatureServiceCache::GetClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, NULL));

    Ptr<MgPropertyDefinitionCollection> data;
    Ptr<MgdFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetClassIdentityProperties(schemaName, className);
    }

    return data.Detach();
}

MgdFeatureSourceCacheItem* MgdFeatureServiceCache::GetFeatureSource(MgResourceIdentifier* resource)
{
    Ptr<MgdFeatureSourceCacheItem> cacheItem;

    MG_TRY()

    cacheItem = GetInstance()->GetFeatureSourceInternal(resource);

    if (NULL == cacheItem.p)
    {
        // Get the Resource Service.
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
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
                throw new MgdInvalidFeatureSourceException(
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

            throw new MgdInvalidFeatureSourceException(
                L"MgCacheManager.GetFeatureSourceCacheItem",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        cacheItem = new MgdFeatureSourceCacheItem(featureSource.release());
        GetInstance()->SetFeatureSource(resource, cacheItem.p);
    }
    else
    {
        //CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_CATCH_AND_THROW(L"MgCacheManager.GetFeatureSourceCacheItem")

    return cacheItem.Detach();
}

MgdSpatialContextCacheItem* MgdFeatureServiceCache::GetSpatialContextInfo(MgResourceIdentifier* resource)
{
    Ptr<MgdSpatialContextCacheItem> cacheItem;

    MG_TRY()

    cacheItem = GetInstance()->GetSpatialContextInfoInternal(resource);

    if (NULL == cacheItem.p)
    {
        Ptr<MgdFeatureSourceCacheItem> featureSourceCacheItem = GetFeatureSource(resource);
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

        cacheItem = new MgdSpatialContextCacheItem(spatialContextInfo.release());
        GetInstance()->SetSpatialContextInfo(resource, cacheItem.p);
    }
    else
    {
        //CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_CATCH_AND_THROW(L"MgCacheManager.GetSpatialContextCacheItem")

    return cacheItem.Detach();
}