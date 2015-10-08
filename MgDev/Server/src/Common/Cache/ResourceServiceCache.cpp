//
//  Copyright (C) 2004-2015 by Autodesk, Inc.
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
#include "ResourceServiceCache.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgResourceServiceCache::MgResourceServiceCache() :
    m_nDroppedEntries(0)
{
    Initialize(MgConfigProperties::DefaultResourceServicePropertyCacheSize,
        MgConfigProperties::DefaultResourceServicePropertyCacheTimeLimit);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgResourceServiceCache::~MgResourceServiceCache()
{
    MG_TRY()

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgResourceServiceCache::~MgResourceServiceCache()\n")));

    Clear();

    MG_CATCH_AND_RELEASE()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Clear the cache.
///
void MgResourceServiceCache::Clear()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    for (MgResourceServiceCacheEntries::iterator i = m_resourceServiceCacheEntries.begin();
        i != m_resourceServiceCacheEntries.end(); ++i)
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

    m_resourceServiceCacheEntries.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Compact the cache.
///
void MgResourceServiceCache::Compact()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    INT32 size = (INT32)m_resourceServiceCacheEntries.size();

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
MgResourceServiceCacheEntry* MgResourceServiceCache::SetEntry(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgResourceServiceCacheEntry> entry = GetEntry(resource);

    if (NULL == entry.p)
    {
        Compact();

        entry = new MgResourceServiceCacheEntry();
        m_resourceServiceCacheEntries.insert(MgResourceServiceCacheEntries::value_type(
            resource->ToString(), SAFE_ADDREF(entry.p)));
    }

    return entry.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return an existing entry from this cache.
///
MgResourceServiceCacheEntry* MgResourceServiceCache::GetEntry(MgResourceIdentifier* resource)
{
    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgResourceServiceCache.GetEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    resource->Validate();

    if (!resource->IsResourceTypeOf(MgResourceType::LayerDefinition))
    {
        throw new MgInvalidResourceTypeException(
            L"MgResourceServiceCache.GetEntry",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgResourceServiceCacheEntry> entry;
    MgResourceServiceCacheEntries::iterator i =
        m_resourceServiceCacheEntries.find(resource->ToString());

    if (m_resourceServiceCacheEntries.end() != i)
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
void MgResourceServiceCache::RemoveEntry(CREFSTRING resource)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    MgResourceServiceCacheEntries::iterator i =
        m_resourceServiceCacheEntries.find(resource);

    if (m_resourceServiceCacheEntries.end() != i)
    {
        SAFE_RELEASE(i->second);
        m_resourceServiceCacheEntries.erase(i);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Remove an entry from the cache.
///
void MgResourceServiceCache::RemoveEntry(MgResourceIdentifier* resource)
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
void MgResourceServiceCache::RemoveOldEntry()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    MgResourceServiceCacheEntries::iterator currIter, prevIter, oldEntry;

    currIter = prevIter = oldEntry = m_resourceServiceCacheEntries.begin();

    while (m_resourceServiceCacheEntries.end() != currIter)
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

    if (m_resourceServiceCacheEntries.end() != oldEntry)
    {
        SAFE_RELEASE(oldEntry->second);
        m_resourceServiceCacheEntries.erase(oldEntry);
        m_nDroppedEntries++;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Remove expired entries from the cache.
///
void MgResourceServiceCache::RemoveExpiredEntries()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    ACE_Time_Value currTime = ACE_High_Res_Timer::gettimeofday();
    MgResourceServiceCacheEntries::iterator i = m_resourceServiceCacheEntries.begin();

    while (m_resourceServiceCacheEntries.end() != i)
    {
        if (NULL == i->second)
        {
            m_resourceServiceCacheEntries.erase(i++);
        }
        else
        {
            ACE_Time_Value idleTimeout = currTime - i->second->GetTimestamp();

            if (idleTimeout > m_timeLimit) // entry has been expired
            {
                SAFE_RELEASE(i->second);
                m_resourceServiceCacheEntries.erase(i++);
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
void MgResourceServiceCache::SetLayerDefinition(MgResourceIdentifier* resource, MgResourceLayerDefinitionCacheItem* layerDefinition)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    Ptr<MgResourceServiceCacheEntry> entry = SetEntry(resource);

    entry->SetLayerDefinition(layerDefinition);
}

MgResourceLayerDefinitionCacheItem* MgResourceServiceCache::GetLayerDefinition(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgResourceLayerDefinitionCacheItem> data;
    Ptr<MgResourceServiceCacheEntry> entry = GetEntry(resource);

    if (NULL != entry.p)
    {
        data = entry->GetLayerDefinition();

        // Make sure this cached data is only used by one thread at a time.
        if (NULL != data.p && data->GetRefCount() > 2)
        {
            data = NULL;
        }
    }

    return data.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the size of the cache.
///
INT32 MgResourceServiceCache::GetCacheSize()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, -1));

    INT32 size = (INT32)m_resourceServiceCacheEntries.size();
    return size;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the # of dropped cache entries.
///
INT32 MgResourceServiceCache::GetDroppedEntriesCount()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, -1));

    return m_nDroppedEntries;
}

