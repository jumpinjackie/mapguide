//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef MGSERVERMANAGER_H_
#define MGSERVERMANAGER_H_

// Undefine conflicting macros on Linux only.
#ifndef _WIN32
#undef min
#undef max
#endif

#include "ServerManagerDllExport.h"
#include "MapGuideCommon.h"
#include <fstream>

#ifdef _WIN32
    #include <pdh.h>
    #include <pdhMsg.h>
#endif

class MgWorkerThread;

typedef ACE_Atomic_Op<ACE_Thread_Mutex, INT32>  SAFE_INT32;

class MG_SERVER_MANAGER_API MgServerManager : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgServerManager)

public:
    virtual ~MgServerManager();

    // MgDisposable method
    virtual void Dispose();

    static MgServerManager* GetInstance();

    // This initializes the server manager
    void Initialize(CREFSTRING locale);
    void LoadConfigurationProperties();

    CREFSTRING GetDefaultMessageLocale();

    MgPropertyCollection* GetConfigurationProperties(CREFSTRING propertySection);
    void SetConfigurationProperties(CREFSTRING propertySection, MgPropertyCollection* properties);
    void RemoveConfigurationProperties(CREFSTRING propertySection, MgPropertyCollection* properties);

    MgPropertyCollection* GetInformationProperties();

    MgByteReader* GetDocument(CREFSTRING identifier);
    void SetDocument(CREFSTRING identifier, MgByteReader* data);

    bool IsSiteServer() const;
    CREFSTRING GetServerName() const;
    CREFSTRING GetLocalServerAddress() const;

    // Admin Properties
    INT32 GetAdminPort();
    INT32 GetAdminThreads();
    CREFSTRING GetAdminEmail();

    // Client Properties
    INT32 GetClientPort();
    INT32 GetClientThreads();

    // Site Properties
    CREFSTRING GetSiteServerAddress() const;
    INT32 GetSitePort();
    INT32 GetSiteThreads();

    enum ServerStatus
    {
        ssOffline = 0,
        ssOnline
    };

    // Methods for bringing/taking server online/offline
    void TakeOffline();
    void BringOnline();
    bool IsOnline();

    // Server health monitoring
    ACE_Time_Value GetStartTime();
    INT32 GetTotalReceivedOperations();
    INT32 GetTotalProcessedOperations();
    INT32 GetTotalOperationTime();

    void IncrementOperationTime(INT32 operationTime);
    void IncrementReceivedOperations();
    void IncrementProcessedOperations();

    void SetAdminMessageQueue(ACE_Message_Queue<ACE_MT_SYNCH>* pMessageQueue);
    void SetClientMessageQueue(ACE_Message_Queue<ACE_MT_SYNCH>* pMessageQueue);
    void SetSiteMessageQueue(ACE_Message_Queue<ACE_MT_SYNCH>* pMessageQueue);
    ACE_Message_Queue<ACE_MT_SYNCH>* GetAdminMessageQueue();
    ACE_Message_Queue<ACE_MT_SYNCH>* GetClientMessageQueue();
    ACE_Message_Queue<ACE_MT_SYNCH>* GetSiteMessageQueue();

    INT32 GetAdminOperationsQueueCount();
    INT32 GetClientOperationsQueueCount();
    INT32 GetSiteOperationsQueueCount();

    INT32 GetUptime();
    INT32 GetAverageOperationTime();

    INT64 GetTotalPhysicalMemory();
    INT64 GetAvailablePhysicalMemory();
    INT64 GetTotalVirtualMemory();
    INT64 GetAvailableVirtualMemory();

    INT32 GetTotalConnections();
    INT32 GetTotalActiveConnections();
    void IncrementActiveConnections();
    void DecrementActiveConnections();

    STRING GetOperatingSystemVersion();

    ACE_Unbounded_Set<ACE_HANDLE>* GetClientHandles() { return m_pClientHandles; }

    void AddClientHandle(ACE_HANDLE handle);
    void RemoveClientHandle(ACE_HANDLE handle);

    // Methods for accessing the worker thread pool
    void StartWorkerThread(void (*function)());
    void StopWorkerThreads();

private:

    // Constructor
    MgServerManager();

    INT32 ComputeWIN32CpuLoad();
    INT32 ComputeLinuxCpuLoad();

    bool ParseDocumentIdentifier(CREFSTRING identifier, STRING& pathTag, STRING& fileTag);
    STRING GetDocumentIdentifierFilename(CREFSTRING pathTag, CREFSTRING fileTag);

    static Ptr<MgServerManager> m_serverManager;
    ACE_Unbounded_Set<ACE_HANDLE>* m_pClientHandles;

    ServerStatus m_ssServerStatus;
    bool m_isSiteServer;
    STRING m_localServerAddress;
    STRING m_defaultMessageLocale;
    STRING m_displayName;

    // Admin Properties
    INT32 m_nAdminPort;
    INT32 m_nAdminThreads;
    STRING m_adminEmail;

    // Client Properties
    INT32 m_nClientPort;
    INT32 m_nClientThreads;

    // Site Properties
    STRING m_siteServerAddress;
    INT32 m_nSitePort;
    INT32 m_nSiteThreads;

    ACE_Time_Value m_startTime;
    SAFE_INT32 m_totalOperationTime;        // This value is tracked in seconds
    SAFE_INT32 m_totalReceivedOperations;
    SAFE_INT32 m_totalProcessedOperations;

    SAFE_INT32 m_totalConnections;
    SAFE_INT32 m_totalActiveConnections;

    ACE_Message_Queue<ACE_MT_SYNCH>*    m_pAdminMessageQueue;
    ACE_Message_Queue<ACE_MT_SYNCH>*    m_pClientMessageQueue;
    ACE_Message_Queue<ACE_MT_SYNCH>*    m_pSiteMessageQueue;

    ACE_Thread_Manager m_threadManager;
    MgWorkerThread* m_pWorkerThreads;

    static const STRING DocumentExtension;
    static const STRING DocumentPath;
};

/// Inline Methods

inline bool MgServerManager::IsSiteServer() const
{
    return m_isSiteServer;
}

inline CREFSTRING MgServerManager::GetServerName() const
{
    return m_displayName;
}

inline CREFSTRING MgServerManager::GetLocalServerAddress() const
{
    return m_localServerAddress;
}

inline CREFSTRING MgServerManager::GetSiteServerAddress() const
{
    return m_siteServerAddress;
}

#endif
