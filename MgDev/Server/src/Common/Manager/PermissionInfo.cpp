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

#include "PermissionInfo.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionInfo::MgPermissionInfo(time_t accessedTime) :
    m_accessedTime(accessedTime),
    m_inherited(true)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object via copy constructor.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionInfo::MgPermissionInfo(const MgPermissionInfo& permissionInfo)
{
    *this = permissionInfo;
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionInfo::~MgPermissionInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Performs overloaded assignment operation for this object.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionInfo& MgPermissionInfo::operator=(const MgPermissionInfo& permissionInfo)
{
    if (this != &permissionInfo)
    {
        m_accessedTime = permissionInfo.m_accessedTime;
        m_owner = permissionInfo.m_owner;
        m_inherited = permissionInfo.m_inherited;

        m_userPermissionMap.clear();
        m_groupPermissionMap.clear();

        m_userPermissionMap = permissionInfo.m_userPermissionMap;
        m_groupPermissionMap = permissionInfo.m_groupPermissionMap;
    }

    return *this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks permission information for equality.
/// </summary>
///----------------------------------------------------------------------------

bool MgPermissionInfo::operator==(const MgPermissionInfo& permissionInfo) const
{
    bool equal = false;

    if (this == &permissionInfo)
    {
        equal = true;
    }
    else if (m_accessedTime == permissionInfo.m_accessedTime
          && m_owner        == permissionInfo.m_owner
          && m_inherited    == permissionInfo.m_inherited)
    {
        if (m_inherited)
        {
            equal = true;
        }
        else if (m_userPermissionMap  == permissionInfo.m_userPermissionMap
             &&  m_groupPermissionMap == permissionInfo.m_groupPermissionMap)
        {
            equal = true;
        }
    }

    return equal;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks permission information for inequality.
/// </summary>
///----------------------------------------------------------------------------

bool MgPermissionInfo::operator!=(const MgPermissionInfo& permissionInfo) const
{
    return !(permissionInfo == *this);
}
///----------------------------------------------------------------------------
/// <summary>
/// Sets the accessed time (in elapsed seconds).
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionInfo::SetAccessedTime(time_t accessedTime)
{
    m_accessedTime = accessedTime;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the owner.
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionInfo::SetOwner(CREFSTRING owner)
{
    m_owner = owner;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the inherited flag.
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionInfo::SetInherited(bool inherited)
{
    m_inherited = inherited;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the user permission.
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionInfo::SetUserPermission(CREFSTRING user, CREFSTRING permission)
{
    if (user.empty())
    {
        assert(false);
    }
    else
    {
        assert(!permission.empty());
        m_userPermissionMap[user] = permission;
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the group permission.
/// </summary>
///----------------------------------------------------------------------------

void MgPermissionInfo::SetGroupPermission(CREFSTRING group, CREFSTRING permission)
{
    if (group.empty())
    {
        assert(false);
    }
    else
    {
        assert(!permission.empty());
        m_groupPermissionMap[group] = permission;
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is the resource owner.
/// </summary>
///----------------------------------------------------------------------------

bool MgPermissionInfo::UserIsOwner(CREFSTRING user) const
{
    return (user == m_owner);
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user has the specified permission.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionInfo::PermissionStatus MgPermissionInfo::UserHasPermission(
    CREFSTRING user, CREFSTRING permission) const
{
    PermissionStatus status = MgPermissionInfo::Denied;
    MgUserPermissionMap::const_iterator i = m_userPermissionMap.find(user);

    if (m_userPermissionMap.end() == i)
    {
        status = MgPermissionInfo::Unknown;
    }
    else
    {
        CREFSTRING grantedPermission = (*i).second;

        if (MgResourcePermission::NoAccess != grantedPermission)
        {
            if (permission == grantedPermission)
            {
                status = MgPermissionInfo::Permitted;
            }
            else if (MgResourcePermission::ReadWrite == grantedPermission)
            {
                if (MgResourcePermission::ReadOnly == permission)
                {
                    status = MgPermissionInfo::Permitted;
                }
            }
        }
    }

    return status;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified group has the specified permission.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionInfo::PermissionStatus MgPermissionInfo::GroupHasPermission(
    CREFSTRING group, CREFSTRING permission) const
{
    PermissionStatus status = MgPermissionInfo::Denied;
    MgGroupPermissionMap::const_iterator i = m_groupPermissionMap.find(group);

    if (m_groupPermissionMap.end() == i)
    {
        status = MgPermissionInfo::Unknown;
    }
    else
    {
        CREFSTRING grantedPermission = (*i).second;

        if (MgResourcePermission::NoAccess != grantedPermission)
        {
            if (permission == grantedPermission)
            {
                status = MgPermissionInfo::Permitted;
            }
            else if (MgResourcePermission::ReadWrite == grantedPermission)
            {
                if (MgResourcePermission::ReadOnly == permission)
                {
                    status = MgPermissionInfo::Permitted;
                }
            }
        }
    }

    return status;
}
