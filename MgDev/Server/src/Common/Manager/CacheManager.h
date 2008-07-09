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

#ifndef MG_CACHE_MANAGER_H_
#define MG_CACHE_MANAGER_H_

#include "ServerManager.h"
#include "FeatureServiceCache.h"

class MgServiceManager;
class MgFdoConnectionManager;

class MG_SERVER_MANAGER_API MgCacheManager : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCacheManager)

/// Constructors/Destructor

public:

    MgCacheManager();
    virtual ~MgCacheManager();

private:

    // Unimplemented copy constructor and assignment operator.
    MgCacheManager(const MgCacheManager&);
    MgCacheManager& operator=(const MgCacheManager&);

/// Methods

public:

    virtual void Dispose();

    static MgCacheManager* GetInstance();

    void Initialize();
    void ClearCaches();

    void NotifyResourceChanged(CREFSTRING resource);
    void NotifyResourceChanged(MgResourceIdentifier* resource);

    MgFeatureServiceCache* GetFeatureServiceCache();

    MgFeatureSourceCacheItem* GetFeatureSourceCacheItem(MgResourceIdentifier* resource);
    MgSpatialContextCacheItem* GetSpatialContextCacheItem(MgResourceIdentifier* resource);

/// Data Members

private:

    static Ptr<MgCacheManager> sm_cacheManager;

    MgServiceManager* m_serviceManager;
    MgFdoConnectionManager* m_fdoConnectionManager;

    MgFeatureServiceCache m_featureServiceCache;
};

/// Inline Methods

inline MgFeatureServiceCache* MgCacheManager::GetFeatureServiceCache()
{
    return &m_featureServiceCache;
}

#endif
