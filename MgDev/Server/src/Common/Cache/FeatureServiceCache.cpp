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
#include "FeatureServiceCache.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgFeatureServiceCache::MgFeatureServiceCache()
{
    Initialize(MgConfigProperties::DefaultFeatureServicePropertyCacheSize,
        MgConfigProperties::DefaultFeatureServicePropertyCacheTimeLimit);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgFeatureServiceCache::~MgFeatureServiceCache()
{
    MG_TRY()

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgFeatureServiceCache::~MgFeatureServiceCache()\n")));

    Clear();

    MG_CATCH_AND_RELEASE()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Clear the cache.
///
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
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgFeatureServiceCache::Clear - Reference Count of '%W': %d\n"),
                i->first.c_str(), i->second->GetRefCount()));
        }
#endif
        SAFE_RELEASE(i->second);
    }

    m_featureServiceCacheEntries.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Compact the cache.
///
void MgFeatureServiceCache::Compact()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    INT32 size = (INT32)m_featureServiceCacheEntries.size();

    if (size >= m_size)
    {
        RemoveOldEntry();
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Create a cache entry for a given resource.
///
MgFeatureServiceCacheEntry* MgFeatureServiceCache::CreateEntry(MgResourceIdentifier* resource)
{
    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgFeatureServiceCache.CreateEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsResourceTypeOf(MgResourceType::FeatureSource))
    {
        throw new MgInvalidResourceTypeException(
            L"MgFeatureServiceCache.CreateEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Compact();

    Ptr<MgFeatureServiceCacheEntry> entry = new MgFeatureServiceCacheEntry();

    m_featureServiceCacheEntries.insert(MgFeatureServiceCacheEntries::value_type(
        resource->ToString(), SAFE_ADDREF(entry.p)));

    return entry.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Get an entry in the cache.
///
MgFeatureServiceCacheEntry* MgFeatureServiceCache::GetEntry(MgResourceIdentifier* resource)
{
    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgFeatureServiceCache.GetEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    resource->Validate();

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgFeatureServiceCacheEntry> entry;
    MgFeatureServiceCacheEntries::iterator i =
        m_featureServiceCacheEntries.find(resource->ToString());

    if (m_featureServiceCacheEntries.end() != i)
    {
        entry = SAFE_ADDREF(i->second);
        entry->UpdateTimestamp();
    }

    return entry.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Remove an entry from the cache.
///
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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Remove an entry from the cache.
///
void MgFeatureServiceCache::RemoveEntry(MgResourceIdentifier* resource)
{
    if (NULL != resource)
    {
        RemoveEntry(resource->ToString());
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Remove the first oldest entry from the cache.
///
void MgFeatureServiceCache::RemoveOldEntry()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    MgFeatureServiceCacheEntries::iterator currIter, prevIter, oldEntry;

    currIter = prevIter = oldEntry = m_featureServiceCacheEntries.begin();

    while (m_featureServiceCacheEntries.end() != currIter)
    {
        if (NULL == currIter->second)
        {
            oldEntry = currIter;
            break;
        }

        if (currIter->second->GetTimestamp() < oldEntry->second->GetTimestamp())
        {
            oldEntry = currIter;
        }

        if (currIter->second->GetTimestamp() < prevIter->second->GetTimestamp())
        {
            break;
        }
        else
        {
            prevIter = currIter;
        }

        ++currIter;
    }

    if (m_featureServiceCacheEntries.end() != oldEntry)
    {
        SAFE_RELEASE(oldEntry->second);
        m_featureServiceCacheEntries.erase(oldEntry);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Remove expired entries from the cache.
///
void MgFeatureServiceCache::RemoveExpiredEntries()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    ACE_Time_Value currTime = ACE_High_Res_Timer::gettimeofday();
    MgFeatureServiceCacheEntries::iterator i = m_featureServiceCacheEntries.begin();

    while (m_featureServiceCacheEntries.end() != i)
    {
        if (NULL == i->second)
        {
            m_featureServiceCacheEntries.erase(i++);
        }
        else
        {
            ACE_Time_Value idleTimeout = currTime - i->second->GetTimestamp();

            if (idleTimeout > m_timeLimit) // entry has been expired
            {
                SAFE_RELEASE(i->second);
                m_featureServiceCacheEntries.erase(i++);
            }
            else
            {
                ++i;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Methods to manage cache entries.
///
void MgFeatureServiceCache::SetFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceCacheItem* featureSource)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        entry = CreateEntry(resource);
    }

    entry->SetFeatureSource(featureSource);
}

MgFeatureSourceCacheItem* MgFeatureServiceCache::GetFeatureSource(MgResourceIdentifier* resource)
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

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        entry = CreateEntry(resource);
    }

    entry->SetSpatialContextInfo(spatialContextInfo);
}

MgSpatialContextCacheItem* MgFeatureServiceCache::GetSpatialContextInfo(MgResourceIdentifier* resource)
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

void MgFeatureServiceCache::SetSpatialContextReader(MgResourceIdentifier* resource, bool active, MgSpatialContextReader* spatialContextReader)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        entry = CreateEntry(resource);
    }

    entry->SetActiveFlag(active);
    entry->SetSpatialContextReader(spatialContextReader);
}

MgSpatialContextReader* MgFeatureServiceCache::GetSpatialContextReader(MgResourceIdentifier* resource, bool active)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgSpatialContextReader> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetSpatialContextReader();

        // Make sure this cached data is only used by one thread at a time.
        if (NULL != data.p
            && (data->GetRefCount() > 2
                || entry->GetActiveFlag() != active))
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

void MgFeatureServiceCache::SetFeatureSchemaNames(MgResourceIdentifier* resource, MgStringCollection* featureSchemaNames)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        entry = CreateEntry(resource);
    }

    entry->SetFeatureSchemaNames(featureSchemaNames);
}

MgStringCollection* MgFeatureServiceCache::GetFeatureSchemaNames(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgStringCollection> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetFeatureSchemaNames();
    }

    return data.Detach();
}

void MgFeatureServiceCache::SetFeatureSchemaCollection(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, bool serialized, MgFeatureSchemaCollection* featureSchemaCollection)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        entry = CreateEntry(resource);
    }

    entry->SetFeatureSchemaName(featureSchemaName);
    entry->SetFeatureSchemaCollectionSerialized(serialized);
    entry->SetFeatureSchemaCollection(featureSchemaCollection);
}

MgFeatureSchemaCollection* MgFeatureServiceCache::GetFeatureSchemaCollection(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, bool serialized)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgFeatureSchemaCollection> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetFeatureSchemaCollection();

        if (NULL != data.p
            && (entry->GetFeatureSchemaName() != featureSchemaName
                || entry->GetFeatureSchemaCollectionSerialized() != serialized))

        {
            data = NULL;
        }
    }

    return data.Detach();
}

void MgFeatureServiceCache::SetFeatureSchemaXml(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, CREFSTRING featureSchemaXml)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        entry = CreateEntry(resource);
    }

    entry->SetFeatureSchemaName(featureSchemaName);
    entry->SetFeatureSchemaXml(featureSchemaXml);
}

STRING MgFeatureServiceCache::GetFeatureSchemaXml(MgResourceIdentifier* resource, CREFSTRING featureSchemaName)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    STRING data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetFeatureSchemaXml();

        if (!data.empty()
            && entry->GetFeatureSchemaName() != featureSchemaName)
        {
            data.clear();
        }
    }

    return data;
}

void MgFeatureServiceCache::SetFeatureClassNames(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, MgStringCollection* featureClassNames)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        entry = CreateEntry(resource);
    }

    entry->SetFeatureSchemaName(featureSchemaName);
    entry->SetFeatureClassNames(featureClassNames);
}

MgStringCollection* MgFeatureServiceCache::GetFeatureClassNames(MgResourceIdentifier* resource, CREFSTRING featureSchemaName)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgStringCollection> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetFeatureClassNames();

        if (NULL != data.p
            && entry->GetFeatureSchemaName() != featureSchemaName)
        {
            data = NULL;
        }
    }

    return data.Detach();
}

void MgFeatureServiceCache::SetFeatureClassDefinition(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, CREFSTRING featureClassName, MgClassDefinition* featureClassDefinition)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        entry = CreateEntry(resource);
    }

    entry->SetFeatureSchemaName(featureSchemaName);
    entry->SetFeatureClassName(featureClassName);
    entry->SetFeatureClassDefinition(featureClassDefinition);
}

MgClassDefinition* MgFeatureServiceCache::GetFeatureClassDefinition(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, CREFSTRING featureClassName)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgClassDefinition> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetFeatureClassDefinition();

        if (NULL != data.p
            && (entry->GetFeatureSchemaName() != featureSchemaName
                || entry->GetFeatureClassName() != featureClassName))
        {
            data = NULL;
        }
    }

    return data.Detach();
}

void MgFeatureServiceCache::SetFeatureClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, CREFSTRING featureClassName, MgPropertyDefinitionCollection* featureClassIdentityProperties)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        entry = CreateEntry(resource);
    }

    entry->SetFeatureSchemaName(featureSchemaName);
    entry->SetFeatureClassName(featureClassName);
    entry->SetFeatureClassIdentityProperties(featureClassIdentityProperties);
}

MgPropertyDefinitionCollection* MgFeatureServiceCache::GetFeatureClassIdentityProperties(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, CREFSTRING featureClassName)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgPropertyDefinitionCollection> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetFeatureClassIdentityProperties();

        if (NULL != data.p
            && (entry->GetFeatureSchemaName() != featureSchemaName
                || entry->GetFeatureClassName() != featureClassName))
        {
            data = NULL;
        }
    }

    return data.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Caches an FDO Feature Schema Collection
/// The cache takes ownership of Feature Schema collection so the caller must not release it.
/// This prevents a race condition that would occur if the caller does a release at the
/// same time another thread tries to retrieve the schemas from this cache.
/// The FDO Feature Schemas are not thread-safe.
/// featureSchemaCollection will be NULL when this function exits.
///
void MgFeatureServiceCache::SetFdoFeatureSchemaCollection(MgResourceIdentifier* resource, CREFSTRING featureSchemaName, FdoPtr<FdoFeatureSchemaCollection>& featureSchemaCollection)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        entry = CreateEntry(resource);
    }

    entry->SetFeatureSchemaName(featureSchemaName);
    entry->SetFdoFeatureSchemaCollection(featureSchemaCollection.p);
    featureSchemaCollection = NULL;
}

FdoFeatureSchemaCollection* MgFeatureServiceCache::GetFdoFeatureSchemaCollection(MgResourceIdentifier* resource, CREFSTRING featureSchemaName)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    FdoPtr<FdoFeatureSchemaCollection> data;
    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetFdoFeatureSchemaCollection();

        if (NULL != data.p
            && entry->GetFeatureSchemaName() != featureSchemaName)
        {
            data = NULL;
        }

        // Make sure this cached data is only used by one thread at a time.
        if (NULL != data.p)
        {
            // FDO Feature Schemas are not thread-safe so caller must take ownership.
            // Remove the FDO schemas from the cache so that they won't be given to
            // any other thread that calls this function.
            // Caller is responsible for putting this schemas back in the cache when 
            // done with them.
            entry->SetFdoFeatureSchemaCollection((FdoFeatureSchemaCollection*) NULL);
        }
    }

    return data.Detach();
}
