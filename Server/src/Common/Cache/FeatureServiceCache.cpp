//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
MgFeatureServiceCache::MgFeatureServiceCache() :
    m_nDroppedEntries(0)
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
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgFeatureServiceCache::Clear() - Reference Count of '%W': %d\n"),
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
/// Return an existing entry from this cache or a newly created one
/// if it does not exist.
///
MgFeatureServiceCacheEntry* MgFeatureServiceCache::SetEntry(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgFeatureServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        Compact();

        entry = new MgFeatureServiceCacheEntry();
        m_featureServiceCacheEntries.insert(MgFeatureServiceCacheEntries::value_type(
            resource->ToString(), SAFE_ADDREF(entry.p)));
    }

    return entry.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an existing entry from this cache.
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

    if (!resource->IsResourceTypeOf(MgResourceType::FeatureSource))
    {
        throw new MgInvalidResourceTypeException(
            L"MgFeatureServiceCache.GetEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

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
        m_nDroppedEntries++;
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

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

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

    Ptr<MgFeatureServiceCacheEntry> entry = SetEntry(resource);

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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the size of the cache.
///
INT32 MgFeatureServiceCache::GetCacheSize()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, -1));

    INT32 size = (INT32)m_featureServiceCacheEntries.size();
    return size;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the # of dropped cache entries.
///
INT32 MgFeatureServiceCache::GetDroppedEntriesCount()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, -1));

    return m_nDroppedEntries;
}
