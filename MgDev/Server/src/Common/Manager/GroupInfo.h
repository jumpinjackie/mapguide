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

#ifndef MGGROUPINFO_H_
#define MGGROUPINFO_H_

#include "ServerManager.h"

#include <set>

class MG_SERVER_MANAGER_API MgGroupInfo
{
    DECLARE_CLASSNAME(MgGroupInfo)

/// Constructors/Destructor

public:

    MgGroupInfo();
    MgGroupInfo(const MgGroupInfo& groupInfo);
    virtual ~MgGroupInfo();

    MgGroupInfo& operator=(const MgGroupInfo& groupInfo);

/// Methods

public:

    set<STRING>& GetUsers();

    bool IsUserInGroup(CREFSTRING user) const;

/// Data Members

private:

    set<STRING> m_users;
};

/// Inline Methods

inline set<STRING>& MgGroupInfo::GetUsers()
{
    return m_users;
}

#endif
