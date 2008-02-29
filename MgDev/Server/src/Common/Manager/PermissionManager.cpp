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

#include "PermissionManager.h"
#include "LogManager.h"

ACE_Recursive_Thread_Mutex MgPermissionManager::sm_mutex;
Ptr<MgPermissionCache> MgPermissionManager::sm_permissionCache = new MgPermissionCache;
INT32 MgPermissionManager::sm_permissionInfoCacheSize = 0;
time_t MgPermissionManager::sm_cutoffTime = 0;

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionManager::MgPermissionManager(const MgSecurityManager& securityMan) :
    m_securityMan(securityMan)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    m_permissionCache = sm_permissionCache;

    if (sm_permissionInfoCacheSize <= 0)
    {
        MgConfiguration* configuration = MgConfiguration::GetInstance();
        assert(NULL != configuration);

        configuration->GetIntValue(
            MgConfigProperties::ResourceServicePropertiesSection,
            MgConfigProperties::ResourceServicePropertyResourcePermissionCacheSize,
            sm_permissionInfoCacheSize,
            MgConfigProperties::DefaultResourceServicePropertyResourcePermissionCacheSize);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionManager::~MgPermissionManager()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    m_permissionCache = NULL;
}

///----------------------------------------------------------------------------
/// <summary>
/// Refresh the global permission cache.
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionManager::RefreshPermissionCache(MgPermissionCache* permissionCache)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    sm_permissionCache = permissionCache;

    ::time(&sm_cutoffTime);
}

///----------------------------------------------------------------------------
/// <summary>
/// Update the global permission cache.
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionManager::UpdatePermissionCache(MgPermissionCache* permissionCache)
{
    if (NULL == permissionCache)
    {
        throw new MgNullArgumentException(
            L"MgPermissionManager.UpdatePermissionCache", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (permissionCache->GetResourcePermissionMap().empty())
    {
        return;
    }

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    time_t cutoffTime = 0;
    INT32 size = (INT32)sm_permissionCache->GetResourcePermissionMap().size();

    if (size >= sm_permissionInfoCacheSize)
    {
        cutoffTime = sm_cutoffTime;
        ::time(&sm_cutoffTime);
    }
    else if (size <= sm_permissionInfoCacheSize / 2)
    {
        ::time(&sm_cutoffTime);
    }

    // Make a copy of the global cache if it is concurrently accessed.
    // Otherwise, just update it directly.

    if (sm_permissionCache->GetRefCount() > 2 || cutoffTime > 0)
    {
        Ptr<MgPermissionCache> newPermissionCache = new MgPermissionCache;

        newPermissionCache->InitializeResourcePermissionMap(
            sm_permissionCache->GetResourcePermissionMap(), cutoffTime);
        newPermissionCache->UpdateResourcePermissionMap(
            permissionCache->GetResourcePermissionMap());

        sm_permissionCache = newPermissionCache;
    }
    else
    {
        sm_permissionCache->UpdateResourcePermissionMap(
            permissionCache->GetResourcePermissionMap());
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the explicit permission information on the specified resource.
/// </summary>
///----------------------------------------------------------------------------

const MgPermissionInfo* MgPermissionManager::GetPermissionInfo(
    CREFSTRING resource) const
{
    return m_permissionCache->GetPermissionInfo(resource);
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the specified user has an adequate permission to
/// perform the current operation.
/// </summary>
///----------------------------------------------------------------------------

bool MgPermissionManager::CheckPermission(
    CREFSTRING user, CREFSTRING permission,
    const MgPermissionInfo* permissionInfo, bool isParent)
{
    MgPermissionInfo::PermissionStatus status = MgPermissionInfo::Denied;

    MG_TRY()

    if (NULL == permissionInfo)
    {
        throw new MgNullArgumentException(
            L"MgPermissionManager.CheckPermission", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Check the inherite flag.

    if (permissionInfo->IsInherited())
    {
        return true;
    }

    // Check the ownership.

    if (!isParent && permissionInfo->UserIsOwner(user))
    {
        return true;
    }

    // Check the user permission.

    status = permissionInfo->UserHasPermission(user, permission);

    // Check the group permission.

    if (MgPermissionInfo::Unknown == status)
    {
        const MgGroupPermissionMap& groupPermissionMap =
            permissionInfo->GetGroupPermissionMap();

        for (MgGroupPermissionMap::const_iterator i = groupPermissionMap.begin();
            i != groupPermissionMap.end(); ++i)
        {
            CREFSTRING group = (*i).first;

            if (m_securityMan.IsUserInGroup(user, group))
            {
                status = permissionInfo->GroupHasPermission(group, permission);

                if (MgPermissionInfo::Permitted == status)
                {
                    break;
                }
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgPermissionManager.CheckPermission")

    return (MgPermissionInfo::Permitted == status);
}
