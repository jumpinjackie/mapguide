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

#ifndef MGPERMISSIONMANAGER_H_
#define MGPERMISSIONMANAGER_H_

#include "SecurityManager.h"
#include "PermissionCache.h"

class MG_SERVER_MANAGER_API MgPermissionManager
{
    DECLARE_CLASSNAME(MgPermissionManager)

/// Constructors/Destructor

public:

    explicit MgPermissionManager(const MgSecurityManager& securityMan);
    virtual ~MgPermissionManager();

private:

    // Unimplemented copy constructor and assignment operator.

    MgPermissionManager();
    MgPermissionManager(const MgPermissionManager&);
    MgPermissionManager& operator=(const MgPermissionManager&);

/// Methods

public:

    static void RefreshPermissionCache(MgPermissionCache* permissionCache);
    static void UpdatePermissionCache(MgPermissionCache* permissionCache);

    INT32 GetPermissionInfoCacheSize() const;
    const MgPermissionInfo* GetPermissionInfo(CREFSTRING resource) const;

    bool CheckPermission(CREFSTRING user, CREFSTRING permission,
        const MgPermissionInfo* permissionInfo, bool isParent);

/// Data Members

private:

    static ACE_Recursive_Thread_Mutex sm_mutex;
    static Ptr<MgPermissionCache> sm_permissionCache;
    static INT32 sm_permissionInfoCacheSize;
    static time_t sm_cutoffTime;

    const MgSecurityManager& m_securityMan;
    Ptr<MgPermissionCache> m_permissionCache;
};

/// Inline Methods

inline INT32 MgPermissionManager::GetPermissionInfoCacheSize() const
{
    return sm_permissionInfoCacheSize;
}

#endif
