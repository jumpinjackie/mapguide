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
#include "SecurityCache.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgSecurityCache::MgSecurityCache()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSecurityCache::~MgSecurityCache()
{
    Clear();
}

///----------------------------------------------------------------------------
/// <summary>
/// Disposes this object.
/// </summary>
///----------------------------------------------------------------------------

void MgSecurityCache::Dispose()
{
    delete this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Initialize this security cache.
/// </summary>
///----------------------------------------------------------------------------
void MgSecurityCache::Initialize(const MgSecurityCache& securityCache)
{
    Clear();

    for (MgUserInfoMap::const_iterator i = securityCache.m_userInfoMap.begin();
        i != securityCache.m_userInfoMap.end(); ++i)
    {
        const MgUserInfo* userInfo = (*i).second;
        assert(NULL != userInfo);

        m_userInfoMap.insert(MgUserInfoMap::value_type(
            (*i).first, new MgUserInfo(*userInfo)));
    }

    for (MgGroupInfoMap::const_iterator i = securityCache.m_groupInfoMap.begin();
        i != securityCache.m_groupInfoMap.end(); ++i)
    {
        const MgGroupInfo* groupInfo = (*i).second;
        assert(NULL != groupInfo);

        m_groupInfoMap.insert(MgGroupInfoMap::value_type(
            (*i).first, new MgGroupInfo(*groupInfo)));
    }

    for (MgRoleInfoMap::const_iterator i = securityCache.m_roleInfoMap.begin();
        i != securityCache.m_roleInfoMap.end(); ++i)
    {
        const MgRoleInfo* roleInfo = (*i).second;
        assert(NULL != roleInfo);

        m_roleInfoMap.insert(MgRoleInfoMap::value_type(
            (*i).first, new MgRoleInfo(*roleInfo)));
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Clear this security cache.
/// </summary>
///----------------------------------------------------------------------------
void MgSecurityCache::Clear()
{
    for (MgUserInfoMap::const_iterator i = m_userInfoMap.begin();
        i != m_userInfoMap.end(); ++i)
    {
        delete (*i).second;
    }

    for (MgGroupInfoMap::const_iterator i = m_groupInfoMap.begin();
        i != m_groupInfoMap.end(); ++i)
    {
        delete (*i).second;
    }

    for (MgRoleInfoMap::const_iterator i = m_roleInfoMap.begin();
        i != m_roleInfoMap.end(); ++i)
    {
        delete (*i).second;
    }

    m_userInfoMap.clear();
    m_groupInfoMap.clear();
    m_roleInfoMap.clear();
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the user information for the specified user ID.
/// </summary>
///----------------------------------------------------------------------------

const MgUserInfo* MgSecurityCache::GetUserInfo(CREFSTRING user,
    bool strict) const
{
    MgUserInfo* userInfo = NULL;
    MgUserInfoMap::const_iterator i = m_userInfoMap.find(user);

    if (m_userInfoMap.end() != i)
    {
        userInfo = (*i).second;
    }
    else if (strict)
    {
        MgStringCollection arguments;
        arguments.Add(user);

        throw new MgUserNotFoundException(L"MgSecurityCache.GetUserInfo",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return userInfo;
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns of a list of roles to which the specified user (or his/her groups)
/// is assigned.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgSecurityCache::EnumerateRoles(CREFSTRING user)
{
    Ptr<MgStringCollection> roles = new MgStringCollection();

    if (IsUserInRole(user, MgRole::Administrator))
    {
        roles->Add(MgRole::Administrator);
    }

    if (IsUserInRole(user, MgRole::Author))
    {
        roles->Add(MgRole::Author);
    }

    if (IsUserInRole(user, MgRole::Viewer))
    {
        roles->Add(MgRole::Viewer);
    }

    return roles.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is in the specified group.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityCache::IsUserInGroup(CREFSTRING user,
    CREFSTRING group) const
{
    if (MgGroup::Everyone == group)
    {
        return (m_userInfoMap.end() != m_userInfoMap.find(user));
    }

    MgGroupInfoMap::const_iterator i = m_groupInfoMap.find(group);

    if (m_groupInfoMap.end() != i)
    {
        MgGroupInfo* groupInfo = (*i).second;

        if (groupInfo->IsUserInGroup(user))
        {
            return true;
        }
    }

    return false;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is in the specified groups.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityCache::IsUserInGroups(CREFSTRING user,
    MgStringCollection* groups) const
{
    if (NULL != groups)
    {
        for (INT32 i = 0; i < groups->GetCount(); ++i)
        {
            if (IsUserInGroup(user, groups->GetItem(i)))
            {
                return true;
            }
        }
    }

    return false;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user (or his/her groups) is in
/// the specified role.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityCache::IsUserInRole(CREFSTRING user,
    CREFSTRING role) const
{
    if (MgRole::Viewer == role)
    {
        return (m_userInfoMap.end() != m_userInfoMap.find(user));
    }
    else if (MgRole::Ogc == role)
    {
        // TODO: Re-visit this implementation when we support the real WMS/WFS
        //       authentication.

        return (MgUser::WmsUser == user || MgUser::WfsUser == user);
    }

    MgRoleInfoMap::const_iterator i = m_roleInfoMap.find(role);

    if (m_roleInfoMap.end() != i)
    {
        MgRoleInfo* roleInfo = (*i).second;

        if (roleInfo->IsUserInRole(user))
        {
            return true;
        }

        const set<STRING>& groups = roleInfo->GetGroups();
        set<STRING>::const_iterator j;

        for (j = groups.begin(); j != groups.end(); ++j)
        {
            if (IsUserInGroup(user, *j))
            {
                return true;
            }
        }
    }

    return false;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is in the specified roles.
/// </summary>
///----------------------------------------------------------------------------

bool MgSecurityCache::IsUserInRoles(CREFSTRING user,
    MgStringCollection* roles) const
{
    if (NULL != roles)
    {
        for (INT32 i = 0; i < roles->GetCount(); ++i)
        {
            if (IsUserInRole(user, roles->GetItem(i)))
            {
                return true;
            }
        }
    }

    return false;
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a new user or updates an existing user.
/// </summary>
///----------------------------------------------------------------------------

void MgSecurityCache::SetUser(CREFSTRING user, CREFSTRING password)
{
    MgUserInfo* userInfo = const_cast<MgUserInfo*>(GetUserInfo(user, false));

    if (NULL == userInfo)
    {
        userInfo = new MgUserInfo;
        m_userInfoMap.insert(MgUserInfoMap::value_type(
            user, userInfo));
    }

    userInfo->SetPassword(password);
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a new group. No group update is supported.
/// </summary>
///----------------------------------------------------------------------------

void MgSecurityCache::SetGroup(CREFSTRING group)
{
    if (m_groupInfoMap.end() == m_groupInfoMap.find(group))
    {
        m_groupInfoMap.insert(MgGroupInfoMap::value_type(
            group, new MgGroupInfo()));
    }
}
