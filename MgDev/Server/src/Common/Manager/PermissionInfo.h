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

#ifndef MGPERMISSIONINFO_H_
#define MGPERMISSIONINFO_H_

#include "ServerManager.h"

typedef std::map<STRING, STRING> MgUserPermissionMap;
typedef std::map<STRING, STRING> MgGroupPermissionMap;

class MG_SERVER_MANAGER_API MgPermissionInfo
{
    DECLARE_CLASSNAME(MgPermissionInfo)

/// Enumerations

public:

    enum PermissionStatus
    {
        Denied          = -1,
        Unknown         =  0,
        Permitted       =  1
    };

/// Constructors/Destructor

public:

    explicit MgPermissionInfo(time_t accessedTime);
    explicit MgPermissionInfo(const MgPermissionInfo& permissionInfo);
    virtual ~MgPermissionInfo();

    MgPermissionInfo& operator=(const MgPermissionInfo& permissionInfo);
    bool operator==(const MgPermissionInfo& permissionInfo) const;
    bool operator!=(const MgPermissionInfo& permissionInfo) const;

private:

    // Unimplemented Constructors/Methods

    MgPermissionInfo();

/// Methods

public:

    time_t GetAccessedTime() const;
    STRING GetOwner() const;
    bool IsInherited() const;
    const MgUserPermissionMap& GetUserPermissionMap() const;
    const MgGroupPermissionMap& GetGroupPermissionMap() const;

    void SetAccessedTime(time_t accessedTime);
    void SetOwner(CREFSTRING owner);
    void SetInherited(bool inherited);
    void SetUserPermission(CREFSTRING user, CREFSTRING permission);
    void SetGroupPermission(CREFSTRING group, CREFSTRING permission);

    bool UserIsOwner(CREFSTRING user) const;
    PermissionStatus UserHasPermission(CREFSTRING user,
        CREFSTRING permission) const;
    PermissionStatus GroupHasPermission(CREFSTRING group,
        CREFSTRING permission) const;

/// Data Members

private:

    time_t m_accessedTime;
    STRING m_owner;
    bool m_inherited;
    MgUserPermissionMap m_userPermissionMap;
    MgGroupPermissionMap m_groupPermissionMap;
};

/// Inline Methods

inline time_t MgPermissionInfo::GetAccessedTime() const
{
    return m_accessedTime;
}

inline STRING MgPermissionInfo::GetOwner() const
{
    return m_owner;
}

inline bool MgPermissionInfo::IsInherited() const
{
    return m_inherited;
}

inline const MgUserPermissionMap& MgPermissionInfo::GetUserPermissionMap() const
{
    return m_userPermissionMap;
}

inline const MgGroupPermissionMap& MgPermissionInfo::GetGroupPermissionMap() const
{
    return m_groupPermissionMap;
}

#endif
