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

#ifndef MGROLEINFO_H_
#define MGROLEINFO_H_

#include "ServerManager.h"

#include <set>

class MG_SERVER_MANAGER_API MgRoleInfo
{
    DECLARE_CLASSNAME(MgRoleInfo)

/// Constructors/Destructor

public:

    MgRoleInfo();
    MgRoleInfo(const MgRoleInfo& roleInfo);
    virtual ~MgRoleInfo();

    MgRoleInfo& operator=(const MgRoleInfo& roleInfo);

/// Methods

public:

    set<STRING>& GetUsers();
    set<STRING>& GetGroups();

    bool IsUserInRole(CREFSTRING user) const;

/// Data Members

private:

    set<STRING> m_users;
    set<STRING> m_groups;
};

/// Inline Methods

inline set<STRING>& MgRoleInfo::GetUsers()
{
    return m_users;
}

inline set<STRING>& MgRoleInfo::GetGroups()
{
    return m_groups;
}

#endif
