//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "SessionCache.h"

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs the object.
///
MgSessionCache::MgSessionCache()
{
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Destructs the object.
///
MgSessionCache::~MgSessionCache()
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgSessionCache::~MgSessionCache()\n")));

    for (MgSessionInfoMap::const_iterator i = m_sessionInfoMap.begin();
        i != m_sessionInfoMap.end(); ++i)
    {
        delete (*i).second;
    }

    m_sessionInfoMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Disposes this object.
///
void MgSessionCache::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Cleans up expired sessions in this cache.
///
void MgSessionCache::CleanUpSessions(INT32 sessionTimeout,
    MgStringCollection* expiredSessions)
{
    ACE_Time_Value currentTime = ACE_High_Res_Timer::gettimeofday();
    ACE_Time_Value sessionLifetime = ACE_Time_Value(sessionTimeout);

    for (MgSessionInfoMap::iterator i = m_sessionInfoMap.begin();
        i != m_sessionInfoMap.end();)
    {
        MgSessionInfo* sessionInfo = (*i).second;
        ACE_Time_Value idleTimeout = currentTime - sessionInfo->GetAccessedTime();

        if (idleTimeout > sessionLifetime) // session has been expired
        {
            expiredSessions->Add((*i).first);
            m_sessionInfoMap.erase(i++);
            delete sessionInfo;
        }
        else
        {
            ++i;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified session to this cache.
///
void MgSessionCache::AddSession(CREFSTRING session, CREFSTRING user)
{
    if (m_sessionInfoMap.end() == m_sessionInfoMap.find(session))
    {
        auto_ptr<MgSessionInfo> sessionInfo;

        sessionInfo.reset(new MgSessionInfo());
        sessionInfo->SetUser(user);
        m_sessionInfoMap.insert(MgSessionInfoMap::value_type(session, sessionInfo.release()));
    }
    else
    {
        throw new MgDuplicateSessionException(L"MgSessionCache.AddSession",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the specified session from this cache.
///
void MgSessionCache::RemoveSession(CREFSTRING session)
{
    MgSessionInfoMap::iterator i = m_sessionInfoMap.find(session);

    if (m_sessionInfoMap.end() == i)
    {
        MgStringCollection args;
        args.Add(session);
        throw new MgSessionExpiredException(L"MgSessionCache.RemoveSession",
            __LINE__, __WFILE__, NULL, L"MgSessionExpired", &args);
    }
    else
    {
        delete (*i).second;
        m_sessionInfoMap.erase(i);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Determines whether or not the specified user is in the specified session.
///
bool MgSessionCache::IsUserInSession(CREFSTRING user,
    CREFSTRING session) const
{
    const MgSessionInfo* sessionInfo = GetSessionInfo(session, false);

    if (NULL != sessionInfo && sessionInfo->GetUser() == user)
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the session information for the specified session ID.
///
MgSessionInfo* MgSessionCache::GetSessionInfo(CREFSTRING session,
    bool strict) const
{
#ifdef _DEBUG
    // Session Affinity: Check that the session was created by this server
    if(!session.empty())
    {
        STRING hexString = session.substr(session.rfind(L"_") + 1); 
        Ptr<MgSiteInfo> sessionSiteInfo = new MgSiteInfo(hexString);
        if(sessionSiteInfo->GetStatus() == MgSiteInfo::Ok)
        {
            MgSiteManager* siteManager = MgSiteManager::GetInstance();
            Ptr<MgSiteInfo> firstSite = siteManager->GetSiteInfo(0);
            assert(sessionSiteInfo->GetTarget().compare(firstSite->GetTarget()) == 0);
            assert(sessionSiteInfo->GetPort(MgSiteInfo::Site) == firstSite->GetPort(MgSiteInfo::Site));
        }
    }
#endif //_DEBUG

    MgSessionInfo* sessionInfo = NULL;
    MgSessionInfoMap::const_iterator i = m_sessionInfoMap.find(session);

    if (m_sessionInfoMap.end() != i)
    {
        sessionInfo = (*i).second;
    }
    else if (strict)
    {
        MgStringCollection args;
        args.Add(session);
        throw new MgSessionExpiredException(L"MgSessionCache.GetSessionInfo",
            __LINE__, __WFILE__, NULL, L"MgSessionExpired", &args);
    }

    return sessionInfo;
}
