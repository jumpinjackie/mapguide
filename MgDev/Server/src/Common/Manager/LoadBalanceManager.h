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

#ifndef MGLOADBALANCEMANAGER_H_
#define MGLOADBALANCEMANAGER_H_

#include "ServerManager.h"

#include <deque>

typedef std::deque<STRING> MgServerQueue;
typedef std::map<STRING, MgServerInformation*, MgWcsICmpAscending> MgServerMap;

class MG_SERVER_MANAGER_API MgLoadBalanceManager : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgLoadBalanceManager)

/// Constructors/Destructor

public:

    virtual ~MgLoadBalanceManager();

private:

    MgLoadBalanceManager();

/// Methods

public:

    virtual void Dispose();

    static MgLoadBalanceManager* GetInstance();

    void Initialize();
    static void Terminate();

    STRING GetLocalServerAddress() const;

    // Server Methods

    STRING RequestServer(INT32 serviceType);

    MgByteReader* EnumerateServers();
    void AddServer(CREFSTRING name, CREFSTRING description,
        CREFSTRING address, CREFSTRING identifier = L"");
    void UpdateServer(CREFSTRING oldName, CREFSTRING newName,
        CREFSTRING newDescription, CREFSTRING newAddress);
    void RemoveServer(CREFSTRING name);

    // Service Methods

    MgSerializableCollection* RegisterServicesOnServers(
        MgSerializableCollection* serverInfoList);
    void UnregisterServicesOnServers(
        MgSerializableCollection* serverInfoList);
    bool RegisterServices();
    void UnregisterServices();
    void EnableServices(INT32 serviceFlags);

private:

    MgStringCollection* GetServerAddresses(INT32 serviceFlags,
        bool includeLocalServer, bool includeSupportServers) const;
    MgServerInformation* GetServerInfo(CREFSTRING address) const;
    MgServerInformation* GetServerInfo(INT32 index,
        MgSerializableCollection* serverInfoList) const;
    MgSerializableCollection* GetServerInfoList(
        bool includeLocalServer, bool includeSupportServers,
        MgServerInformation* newServerInfo = NULL,
        MgServerInformation* removedServerInfo = NULL) const;
    MgSerializableCollection* CopyServerInfoList(
        bool includeLocalServer, bool includeSupportServers,
        MgServerInformation* removedServerInfo = NULL) const;

    bool FindServerByName(CREFSTRING name) const;
    bool FindServerByAddress(CREFSTRING address) const;

    void UpdateServerConfiguration();
    void WriteServerList(MgServerInformation* serverInfo,
        string& serverList);

    void AddServerToQueue(INT32 serviceType, CREFSTRING serverAddress);
    void UpdateServerInQueue(INT32 serviceType, CREFSTRING oldAddress,
        CREFSTRING newAddress);
    void RemoveServerFromQueue(INT32 serviceType, CREFSTRING serverAddress);
    void UpdateServerQueues(MgServerInformation* serverInfo);

    bool RegisterServices(MgServerInformation* newServerInfo,
        MgServerInformation* removedServerInfo = NULL);
    void UnregisterServices(MgServerInformation* removedServerInfo);
    MgSerializableCollection* RegisterServicesOnServers(CREFSTRING serverAddress,
        MgSerializableCollection* serverInfoList);
    void UnregisterServicesOnServers(CREFSTRING serverAddress,
        MgSerializableCollection* serverInfoList);
    void EnableServicesOnServers(MgSerializableCollection* serverInfoList,
        bool strict);

    void NotifyResourcesChanged(CREFSTRING serverAddress,
        MgSerializableCollection* resources);
    void DispatchResourceChangeNotifications(
        MgStringCollection* serverAddresses,
        MgSerializableCollection* changedResources);

/// Data Members

private:

    friend class MgServiceManager;

    static Ptr<MgLoadBalanceManager> sm_loadBalanceManager;
    static ACE_Recursive_Thread_Mutex sm_mutex;

    MgConfiguration* m_configuration;
    MgServerManager* m_serverManager;

    Ptr<MgServerInformation> m_localServerInfo; // Local server.
    MgServerMap m_supportServerMap;             // Support servers within this site.
    MgServerMap m_externalServerMap;            // Other servers at different sites.

    MgServerQueue m_serverQueues[MgServerInformation::sm_knMaxNumberServices];
};

/// Inline Methods

inline STRING MgLoadBalanceManager::GetLocalServerAddress() const
{
    assert(m_localServerInfo != NULL);
    return m_localServerInfo->GetAddress();
}

#endif
