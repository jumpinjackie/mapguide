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

#include "PermissionCache.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionCache::MgPermissionCache()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionCache::~MgPermissionCache()
{
    ClearResourcePermissionMap();
}

///----------------------------------------------------------------------------
/// <summary>
/// Disposes this object.
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionCache::Dispose()
{
    delete this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the explicit permission information on the specified resource.
/// </summary>
///----------------------------------------------------------------------------

const MgPermissionInfo* MgPermissionCache::GetPermissionInfo(
    CREFSTRING resource) const
{
    const MgPermissionInfo* permissionInfo = NULL;
    MgPermissionInfoMap::const_iterator i =
        m_permissionInfoMap.find(resource);

    if (m_permissionInfoMap.end() != i)
    {
        permissionInfo = (*i).second;
    }

    return permissionInfo;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the permission information on the specified resource.
/// The permission cache will take the ownership of the allocated
/// MgPermissionInfo object.
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionCache::SetPermissionInfo(CREFSTRING resource,
    const MgPermissionInfo* permissionInfo)
{
    if (resource.empty() || NULL == permissionInfo)
    {
        throw new MgNullArgumentException(
            L"MgPermissionCache.SetPermissionInfo", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgPermissionInfoMap::iterator i =
        m_permissionInfoMap.find(resource);

    if (m_permissionInfoMap.end() != i)
    {
        delete (*i).second;
        m_permissionInfoMap.erase(i);
    }

    m_permissionInfoMap.insert(MgPermissionInfoMap::value_type(
        resource, const_cast<MgPermissionInfo*>(permissionInfo)));
}

///----------------------------------------------------------------------------
/// <summary>
/// Initializes the resource permission map.
/// </summary>
///----------------------------------------------------------------------------
void MgPermissionCache::InitializeResourcePermissionMap(
    const MgPermissionInfoMap& permissionInfoMap, time_t cutoffTime)
{
    ClearResourcePermissionMap();

    for (MgPermissionInfoMap::const_iterator i = permissionInfoMap.begin();
        i != permissionInfoMap.end(); ++i)
    {
        const MgPermissionInfo* permissionInfo = (*i).second;
        assert(NULL != permissionInfo);

        if (NULL != permissionInfo
            && permissionInfo->GetAccessedTime() > cutoffTime)
        {
            m_permissionInfoMap.insert(MgPermissionInfoMap::value_type(
                (*i).first, new MgPermissionInfo(*permissionInfo)));
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the resource permission map.
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionCache::UpdateResourcePermissionMap(
    const MgPermissionInfoMap& permissionInfoMap)
{
    for (MgPermissionInfoMap::const_iterator i = permissionInfoMap.begin();
        i != permissionInfoMap.end(); ++i)
    {
        const MgPermissionInfo* permissionInfo = (*i).second;
        assert(NULL != permissionInfo);

        if (NULL != permissionInfo)
        {
            CREFSTRING resource = (*i).first;
            MgPermissionInfoMap::iterator j =
                m_permissionInfoMap.find(resource);

            if (m_permissionInfoMap.end() != j)
            {
                delete (*j).second;
                m_permissionInfoMap.erase(j);
            }

            if (permissionInfo->GetAccessedTime() > 0)
            {
                m_permissionInfoMap.insert(MgPermissionInfoMap::value_type(
                    resource, new MgPermissionInfo(*permissionInfo)));
            }
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Clears all the elements in the resource permission map.
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionCache::ClearResourcePermissionMap()
{
    for (MgPermissionInfoMap::const_iterator i = m_permissionInfoMap.begin();
        i != m_permissionInfoMap.end(); ++i)
    {
        delete (*i).second;
    }

    m_permissionInfoMap.clear();
}
