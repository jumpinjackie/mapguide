//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "SecurityManager.h"
#include "SessionManager.h"
#include "LogManager.h"

static const STRING MG_SYSTEM_USERNAME = L"2ACd2Gh2N2OqU";
static const STRING MG_SYSTEM_PASSWORD = L"DD@(.ngQu0^'cH0@`ng";

ACE_Recursive_Thread_Mutex MgSecurityManager::sm_mutex;
Ptr<MgSecurityCache> MgSecurityManager::sm_securityCache = new MgSecurityCache;

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSecurityManager::MgSecurityManager()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    m_securityCache = sm_securityCache;
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSecurityManager::~MgSecurityManager()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    m_securityCache = NULL;
}

///----------------------------------------------------------------------------
/// <summary>
/// Refresh the global security cache.
/// </summary>
///----------------------------------------------------------------------------

void MgSecurityManager::RefreshSecurityCache(MgSecurityCache* securityCache)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    sm_securityCache = securityCache;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the user information in the global security cache.
/// </summary>
///----------------------------------------------------------------------------

void MgSecurityManager::SetUser(CREFSTRING user, CREFSTRING password)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    // Make a copy of the global cache if it is concurrently accessed.
    // Otherwise, just update it directly.

    if (sm_securityCache->GetRefCount() > 2)
    {
        Ptr<MgSecurityCache> newSecurityCache = new MgSecurityCache();

        newSecurityCache->Initialize(*sm_securityCache);
        newSecurityCache->SetUser(user, password);

        sm_securityCache = newSecurityCache;
    }
    else
    {
        sm_securityCache->SetUser(user, password);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the group information in the global security cache.
/// </summary>
///----------------------------------------------------------------------------

void MgSecurityManager::SetGroup(CREFSTRING group)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    // Make a copy of the global cache if it is concurrently accessed.
    // Otherwise, just update it directly.

    if (sm_securityCache->GetRefCount() > 2)
    {
        Ptr<MgSecurityCache> newSecurityCache = new MgSecurityCache();

        newSecurityCache->Initialize(*sm_securityCache);
        newSecurityCache->SetGroup(group);

        sm_securityCache = newSecurityCache;
    }
    else
    {
        sm_securityCache->SetGroup(group);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates system credentials (for internal use only).
/// </summary>
///----------------------------------------------------------------------------

MgUserInformation* MgSecurityManager::CreateSystemCredentials()
{
    return new MgUserInformation(MG_SYSTEM_USERNAME, MG_SYSTEM_PASSWORD);
}

///----------------------------------------------------------------------------
/// <summary>
/// Authenticates the specified system credentials (for internal use only).
/// </summary>
///
/// <exceptions>
/// MgAuthenticationFailedException
/// </exceptions>
///----------------------------------------------------------------------------

void MgSecurityManager::Authenticate(MgUserInformation* userInformation)
{
    if (NULL == userInformation ||
        userInformation->GetUserName() != MG_SYSTEM_USERNAME ||
        userInformation->GetPassword() != MG_SYSTEM_PASSWORD)
    {
        throw new MgAuthenticationFailedException(
            L"MgSecurityManager.Authenticate", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Authenticates a user and validates his/her roles.
/// </summary>
///
/// <exceptions>
/// MgAuthenticationFailedException
/// MgUnauthorizedAccessException
/// </exceptions>
///----------------------------------------------------------------------------

MgStringCollection* MgSecurityManager::Authenticate(
    MgUserInformation* userInformation, MgStringCollection* requiredRoles,
    bool returnAssignedRoles)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    Ptr<MgStringCollection> assignedRoles;

    MG_TRY()

    if (NULL == userInformation)
    {
        throw new MgAuthenticationFailedException(
            L"MgSecurityManager.Authenticate", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    STRING user = userInformation->GetUserName();
    STRING session = userInformation->GetMgSessionId();

    if (session.empty())
    {
        if (user.empty())
        {
            throw new MgAuthenticationFailedException(
                L"MgSecurityManager.Authenticate", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const MgUserInfo* userInfo = sm_securityCache->GetUserInfo(user);
        assert(NULL != userInfo);

        if (userInformation->GetPassword() != userInfo->GetPassword())
        {
            throw new MgAuthenticationFailedException(
                L"MgSecurityManager.Authenticate", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        user = MgSessionManager::UpdateLastAccessedTime(session);
    }

    if (NULL != requiredRoles &&
        !sm_securityCache->IsUserInRoles(user, requiredRoles))
    {
        MG_LOG_AUTHENTICATION_ENTRY(MgResources::UnauthorizedAccess.c_str());

        throw new MgUnauthorizedAccessException(
            L"MgSecurityManager.Authenticate", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (returnAssignedRoles)
    {
        assignedRoles = sm_securityCache->EnumerateRoles(user);
    }

    // Commented out logging of successful authentication because it creates lots of entries in the Authentication.log
//    MG_LOG_AUTHENTICATION_ENTRY(MgResources::Success.c_str());

    MG_CATCH(L"MgSecurityManager.Authenticate")

    if (mgException != NULL)
    {
        MG_LOG_AUTHENTICATION_ENTRY(MgResources::Failure.c_str());
    }

    MG_THROW()

    return assignedRoles.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is a system user.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityManager::IsSystemUser(CREFSTRING user)
{
    return (MgUser::Administrator == user
         || MgUser::Anonymous     == user
         || MgUser::Author        == user
         || MgUser::WfsUser       == user
         || MgUser::WmsUser       == user);
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is a Mg administrator.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityManager::IsUserAnAdministrator(CREFSTRING user) const
{
    if (MgUser::Administrator == user)
    {
        return true;
    }

    return m_securityCache->IsUserInRole(user, MgRole::Administrator);
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is a Mg author.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityManager::IsUserAnAuthor(CREFSTRING user) const
{
    if (MgUser::Author == user)
    {
        return true;
    }

    return m_securityCache->IsUserInRole(user, MgRole::Author);
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is in the specified group.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityManager::IsUserInGroup(CREFSTRING user, CREFSTRING group) const
{
    return m_securityCache->IsUserInGroup(user, group);
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is in the specified groups.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityManager::IsUserInGroups(CREFSTRING user,
    MgStringCollection* groups) const
{
    return m_securityCache->IsUserInGroups(user, groups);
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user (or his/her groups) is in
/// the specified role.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityManager::IsUserInRole(CREFSTRING user, CREFSTRING role) const
{
    return m_securityCache->IsUserInRole(user, role);
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is in the specified roles.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityManager::IsUserInRoles(CREFSTRING user,
    MgStringCollection* roles) const
{
    return m_securityCache->IsUserInRoles(user, roles);
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the name of a user in the specified session.
/// </summary>
///----------------------------------------------------------------------------

STRING MgSecurityManager::GetUserName(CREFSTRING session) const
{
    return MgSessionManager::GetUserName(session);
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the password of the specified user.
/// </summary>
///----------------------------------------------------------------------------

STRING MgSecurityManager::GetPassword(CREFSTRING user) const
{
    const MgUserInfo* userInfo = m_securityCache->GetUserInfo(user);
    assert(NULL != userInfo);

    return userInfo->GetPassword();
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user exists.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityManager::FindUser(CREFSTRING user) const
{
    const MgUserInfoMap& userInfoMap = m_securityCache->GetUserInfoMap();

    return (userInfoMap.end() != userInfoMap.find(user));
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified group exists.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityManager::FindGroup(CREFSTRING group) const
{
    const MgGroupInfoMap& groupInfoMap = m_securityCache->GetGroupInfoMap();

    return (groupInfoMap.end() != groupInfoMap.find(group));
}
