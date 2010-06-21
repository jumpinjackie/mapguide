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

#include "LongTransactionManager.h"

ACE_Recursive_Thread_Mutex MgLongTransactionManager::sm_mutex;
LongTransactionNameCache MgLongTransactionManager::s_LongTransactionNameCache;


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgLongTransactionManager::MgLongTransactionManager()
{
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgLongTransactionManager::~MgLongTransactionManager()
{
    MgLongTransactionManager::ClearCache();
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns any cached entry for the given session / resource.
///
LongTransactionNameCacheEntry* MgLongTransactionManager::FindEntry(CREFSTRING sessionId,
                                                                   CREFSTRING resource)
{
    LongTransactionNameCache::iterator iter = s_LongTransactionNameCache.find(sessionId);
    while (iter != s_LongTransactionNameCache.end() && sessionId == iter->first)
    {
        LongTransactionNameCacheEntry* pLongTransactionNameCacheEntry = iter->second;
        if (pLongTransactionNameCacheEntry)
        {
            STRING cacheResource = pLongTransactionNameCacheEntry->featureSource;
            if (ACE_OS::strcasecmp(cacheResource.c_str(), resource.c_str()) == 0)
                return pLongTransactionNameCacheEntry;
        }

        iter++;
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Provides the long transaction name associated with the specified
/// resource.  Returns false if no session was active for the current
/// request, or if no long transaction name was found.
///
bool MgLongTransactionManager::GetLongTransactionName(MgResourceIdentifier* featureSourceId, REFSTRING longTransactionName)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    STRING sessionId;
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
    if (userInfo != NULL)
        sessionId = userInfo->GetMgSessionId();

    if (sessionId.empty())
        return false;

    return MgLongTransactionManager::GetLongTransactionName(sessionId, featureSourceId, longTransactionName);
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the long transaction name associated with the specified
/// session / resource.
///
bool MgLongTransactionManager::GetLongTransactionName(CREFSTRING sessionId,
                                                      MgResourceIdentifier* featureSourceId,
                                                      REFSTRING longTransactionName)
{
    if (sessionId.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);
        throw new MgInvalidArgumentException(L"MgLongTransactionManager.GetLongTransactionName",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    CHECKARGUMENTNULL(featureSourceId, L"MgLongTransactionManager.GetLongTransactionName");

    MG_FEATURE_SERVICE_TRY()

    STRING resource = featureSourceId->ToString();

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    LongTransactionNameCacheEntry* pLongTransactionNameCacheEntry = MgLongTransactionManager::FindEntry(sessionId, resource);
    if (pLongTransactionNameCacheEntry)
    {
        // found
        longTransactionName = pLongTransactionNameCacheEntry->ltName;
        return true;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgLongTransactionManager.GetLongTransactionName")

    // not found
    return false;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Associates a long transaction name with the specified session / resource.
///
void MgLongTransactionManager::SetLongTransactionName(CREFSTRING sessionId,
                                                      MgResourceIdentifier* featureSourceId,
                                                      CREFSTRING longTransactionName)
{
    if (sessionId.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);
        throw new MgInvalidArgumentException(L"MgLongTransactionManager.AddLongTransactionName",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    CHECKARGUMENTNULL(featureSourceId, L"MgLongTransactionManager.AddLongTransactionName");

    MG_FEATURE_SERVICE_TRY()

    STRING resource = featureSourceId->ToString();

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    // First look for an existing entry
    LongTransactionNameCacheEntry* pLongTransactionNameCacheEntry = MgLongTransactionManager::FindEntry(sessionId, resource);
    if (pLongTransactionNameCacheEntry)
    {
        // We have a match - just update the name
        pLongTransactionNameCacheEntry->ltName = longTransactionName;
        return;
    }

    // No match found - add a new entry to the cache
    pLongTransactionNameCacheEntry = new LongTransactionNameCacheEntry;
    pLongTransactionNameCacheEntry->featureSource = resource;
    pLongTransactionNameCacheEntry->ltName = longTransactionName;

    s_LongTransactionNameCache.insert(LongTransactionNameCache_Pair(sessionId, pLongTransactionNameCacheEntry));

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgLongTransactionManager.AddLongTransactionName")
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Removes all long transaction name entries for the specified session.
///
void MgLongTransactionManager::RemoveLongTransactionNames(CREFSTRING sessionId)
{
    MG_FEATURE_SERVICE_TRY()

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    LongTransactionNameCache::iterator iter = s_LongTransactionNameCache.find(sessionId);
    while (iter != s_LongTransactionNameCache.end() && sessionId == iter->first)
    {
        STRING cacheKey = iter->first;
        if (ACE_OS::strcasecmp(cacheKey.c_str(), sessionId.c_str()) == 0)
        {
            // We have a key match
            delete iter->second;
            s_LongTransactionNameCache.erase(iter++);
        }
        else
        {
            iter++;
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgLongTransactionManager.RemoveLongTransactionNames")
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Removes all long transaction name entries for the specified expired
/// sessions.
///
void MgLongTransactionManager::RemoveLongTransactionNames(MgStringCollection* expiredSessions)
{
    if (expiredSessions != NULL)
    {
        // The mutex usage is to ensure this operation is completed for all expired sessions at once.
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

        for (INT32 i=0; i<expiredSessions->GetCount(); ++i)
            MgLongTransactionManager::RemoveLongTransactionNames(expiredSessions->GetItem(i));
    }
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Removes all long transaction name entries in the cache.
///
void MgLongTransactionManager::ClearCache()
{
    MG_FEATURE_SERVICE_TRY()

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    LongTransactionNameCache::iterator iter = s_LongTransactionNameCache.begin();
    while (iter != s_LongTransactionNameCache.end())
    {
        delete iter->second;
        s_LongTransactionNameCache.erase(iter++);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgLongTransactionManager.ClearCache")
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Cleans up the long transaction manager.  This method must be called
/// once during the server shutdown time.
///
void MgLongTransactionManager::Terminate()
{
    MgLongTransactionManager::ClearCache();
}
