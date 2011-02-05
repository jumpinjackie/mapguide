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

#ifndef MGSECURITYCACHE_H_
#define MGSECURITYCACHE_H_

#include "UserInfo.h"
#include "GroupInfo.h"
#include "RoleInfo.h"

typedef std::map<STRING, MgUserInfo*> MgUserInfoMap;
typedef std::map<STRING, MgGroupInfo*> MgGroupInfoMap;
typedef std::map<STRING, MgRoleInfo*> MgRoleInfoMap;

class MG_SERVER_MANAGER_API MgSecurityCache : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgSecurityCache)

/// Constructors/Destructor

public:

    MgSecurityCache();
    virtual ~MgSecurityCache();

private:

    // Unimplemented copy constructor and assignment operator.

    MgSecurityCache(const MgSecurityCache&);
    MgSecurityCache& operator=(const MgSecurityCache&);

/// Methods

public:

    void Initialize(const MgSecurityCache& securityCache);

    MgUserInfoMap& GetUserInfoMap();
    MgGroupInfoMap& GetGroupInfoMap();
    MgRoleInfoMap& GetRoleInfoMap();

    const MgUserInfo* GetUserInfo(CREFSTRING user,
        bool strict = true) const;

    MgStringCollection* EnumerateRoles(CREFSTRING user);

    bool IsUserInGroup(CREFSTRING user, CREFSTRING group) const;
    bool IsUserInGroups(CREFSTRING user, MgStringCollection* groups) const;

    bool IsUserInRole(CREFSTRING user, CREFSTRING role) const;
    bool IsUserInRoles(CREFSTRING user, MgStringCollection* roles) const;

    void SetUser(CREFSTRING user, CREFSTRING password);
    void SetGroup(CREFSTRING group);

protected:

    virtual void Dispose();

private:

    void Clear();

/// Data Members

private:

    MgUserInfoMap m_userInfoMap;
    MgGroupInfoMap m_groupInfoMap;
    MgRoleInfoMap m_roleInfoMap;
};

/// Inline Methods

inline MgUserInfoMap& MgSecurityCache::GetUserInfoMap()
{
    return m_userInfoMap;
}

inline MgGroupInfoMap& MgSecurityCache::GetGroupInfoMap()
{
    return m_groupInfoMap;
}

inline MgRoleInfoMap& MgSecurityCache::GetRoleInfoMap()
{
    return m_roleInfoMap;
}

#endif
