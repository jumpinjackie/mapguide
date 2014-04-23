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

#ifndef MGSERVICEMANAGER_H_
#define MGSERVICEMANAGER_H_

#include <set>
#include "LoadBalanceManager.h"

class MgServerResourceService;

class MG_SERVER_MANAGER_API MgServiceManager : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgServiceManager)

/// Constructors/Destructor

public:

    virtual ~MgServiceManager();

private:

    MgServiceManager();

/// Methods

public:

    virtual void Dispose();

    static MgServiceManager* GetInstance();

    void Initialize();
    static void Terminate();

    MgStringCollection* CleanUpRepositories(
        MgStringCollection* expiredSessions);
    void PerformRepositoryCheckpoints();

    void EnableServices(MgPropertyCollection* hostProps);
    MgService* RequestService(INT32 serviceType);

    bool NotifyFeatureServiceOnResourcesChanged(const set<STRING>& resources,
        bool strict = true);
    bool NotifyFeatureServiceOnResourcesChanged(MgSerializableCollection* resources,
        bool strict = true);
    bool NotifyTileServiceOnResourcesChanged(MgSerializableCollection* resources,
        bool strict = true);

    void NotifyResourcesChanged(MgSerializableCollection* resources);
    void DispatchResourceChangeNotifications();

private:

    MgService* RequestLocalService(INT32 serviceType);
    MgService* RequestProxyService(INT32 serviceType);

/// Data Members

private:

    static Ptr<MgServiceManager> sm_serviceManager;

    MgServerManager* m_serverManager;
    MgLoadBalanceManager* m_loadBalanceManager;

    Ptr<MgServerResourceService> m_resourceService;
};

#endif
