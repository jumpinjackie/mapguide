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

#include "RoleInfo.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgRoleInfo::MgRoleInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object via copy constructor.
/// </summary>
///----------------------------------------------------------------------------

MgRoleInfo::MgRoleInfo(const MgRoleInfo& roleInfo)
{
    *this = roleInfo;
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgRoleInfo::~MgRoleInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Performs overloaded assignment operation for this object.
/// </summary>
///----------------------------------------------------------------------------

MgRoleInfo& MgRoleInfo::operator=(const MgRoleInfo& roleInfo)
{
    if (this != &roleInfo)
    {
        m_users.clear();
        m_groups.clear();

        m_users = roleInfo.m_users;
        m_groups = roleInfo.m_groups;
    }

    return *this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines whether or not the specified user is in this role.
/// </summary>
///----------------------------------------------------------------------------

bool MgRoleInfo::IsUserInRole(CREFSTRING user) const
{
    return (m_users.end() != m_users.find(user));
}
