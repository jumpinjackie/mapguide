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

#ifndef MGPERMISSIONCACHE_H_
#define MGPERMISSIONCACHE_H_

#include "PermissionInfo.h"

typedef std::map<STRING, MgPermissionInfo*> MgPermissionInfoMap;

class MG_SERVER_MANAGER_API MgPermissionCache : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgPermissionCache)

/// Constructors/Destructor

public:

    MgPermissionCache();
    virtual ~MgPermissionCache();

private:

    // Unimplemented copy constructor and assignment operator.

    MgPermissionCache(const MgPermissionCache&);
    MgPermissionCache& operator=(const MgPermissionCache&);

/// Methods

public:

    const MgPermissionInfoMap& GetResourcePermissionMap() const;

    const MgPermissionInfo* GetPermissionInfo(CREFSTRING resource) const;
    void SetPermissionInfo(CREFSTRING resource,
        const MgPermissionInfo* permissionInfo);

    void InitializeResourcePermissionMap(
        const MgPermissionInfoMap& permissionInfoMap,
        time_t cutoffTime);
    void UpdateResourcePermissionMap(
        const MgPermissionInfoMap& permissionInfoMap);

protected:

    virtual void Dispose();

private:

    void ClearResourcePermissionMap();

/// Data Members

private:

    MgPermissionInfoMap m_permissionInfoMap;
};

/// Inline Methods

inline const MgPermissionInfoMap& MgPermissionCache::GetResourcePermissionMap() const
{
    return m_permissionInfoMap;
}

#endif
