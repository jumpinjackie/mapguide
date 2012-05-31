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

#ifndef MGFDOCONNECTIONMANAGER_H_
#define MGFDOCONNECTIONMANAGER_H_

// Undefine conflicting macros on Linux only.
#ifndef _WIN32
#undef min
#undef max
#endif

#include "ServerManagerDllExport.h"
#include "MapGuideCommon.h"
#include "Fdo.h"
#include "ServerFeatureServiceExceptionDef.h"
#include "FeatureSource.h"

#define MG_FDOCONNECTION_MANAGER_TRY()                         MG_FEATURE_SERVICE_TRY()
#define MG_FDOCONNECTION_MANAGER_CATCH(methodName)             MG_FEATURE_SERVICE_CATCH(methodName)
#define MG_FDOCONNECTION_MANAGER_THROW()                       MG_FEATURE_SERVICE_THROW()
#define MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(methodName)   MG_FEATURE_SERVICE_CATCH_AND_THROW(methodName)

#define MG_FDOCONNECTION_MANAGER_CATCH_WITH_FEATURE_SOURCE(methodName, fsId)             MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(methodName, fsId)
#define MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW_WITH_FEATURE_SOURCE(methodName, fsId)   MG_FEATURE_SERVICE_CATCH_AND_THROW_WITH_FEATURE_SOURCE(methodName, fsId)

typedef struct {
    STRING ltName;                              // Current long transaction name for this connection
    FdoIConnection* pFdoConnection;
    ACE_Time_Value lastUsed;
    bool bValid;
    bool bInUse;
    INT32 nUseCount;    // Used by PerCommandThreaded/MultiThreaded providers
    INT32 nUseTotal;    // Total number of times this connection has been used
} FdoConnectionCacheEntry;

// FDO Connection Cache
typedef std::multimap<STRING, FdoConnectionCacheEntry*> FdoConnectionCache;
typedef std::pair<STRING, FdoConnectionCacheEntry*> FdoConnectionCacheEntry_Pair;

// Provider Information class
class ProviderInfo
{
public:
    ProviderInfo(STRING provider,
                 INT32 poolSize,
                 FdoThreadCapability threadModel,
                 bool bKeepCached,
                 INT32 useLimit)
    {
        m_provider = provider;
        m_poolSize = poolSize;
        m_threadModel = threadModel;
        m_bKeepCached = bKeepCached;
        m_useLimit = useLimit;

        m_currentConnections = 0;
    }

    ~ProviderInfo()                                {}

    INT32 GetPoolSize()
    {
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, -1));
        return m_poolSize;
    }

    void SetPoolSize(INT32 ps)
    {
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));
        m_poolSize = ps;

        // If this is a single threaded provider we need to enforce
        // the maximum # of concurrent connections to 1
        if(FdoThreadCapability_SingleThreaded == m_threadModel)
        {
            m_poolSize = 1;
        }
    }

    INT32 GetCurrentConnections()
    {
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, -1));
        return m_currentConnections;
    }

    void IncrementCurrentConnections()
    {
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));
        m_currentConnections++;
    }

    void DecrementCurrentConnections()
    {
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));
        m_currentConnections--;

        if(m_currentConnections < 0)
        {
            // We should not be here
            #ifdef _DEBUG
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("DecrementCurrentConnections - Negative Value!!!\n")));
            #endif
            m_currentConnections = 0;
        }
    }

    void ResetCurrentConnections()
    {
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));
        m_currentConnections = 0;
    }

    FdoThreadCapability GetThreadModel()
    {
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, (FdoThreadCapability)-1));
        return m_threadModel;
    }

    void SetThreadModel(FdoThreadCapability tm)
    {
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));
        m_threadModel = tm;

        // If this is a single threaded provider we need to enforce
        // the maximum # of concurrent connections to 1
        if(FdoThreadCapability_SingleThreaded == m_threadModel)
        {
            m_poolSize = 1;
        }
    }

    FdoConnectionCache* GetFdoConnectionCache()     { return &m_fdoConnectionCache; }

    bool GetKeepCached()
    {
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));
        return m_bKeepCached;
    }

    void SetKeepCached(bool bKeepCached)
    {
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));
        m_bKeepCached = bKeepCached;
    }

    STRING GetProviderName()                        { return m_provider; }

    INT32 GetUseLimit()
    {
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, -1));
        return m_useLimit;
    }

    void SetUseLimit(INT32 useLimit)
    {
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));
        m_useLimit = useLimit;
    }

private:
    // The name of the provider
    STRING m_provider;

    // The # of FDO connections to allow
    INT32 m_poolSize;

    // The # of current FDO connections to this provider
    INT32 m_currentConnections;

    // The thread model of the provider
    FdoThreadCapability m_threadModel;

    // The collection of FDO connections
    FdoConnectionCache m_fdoConnectionCache;

    // The flag indicating if the FDO connections for this provider should be cached
    bool m_bKeepCached;

    // The # of times this FDO connections can be used before it is released
    INT32 m_useLimit;

    static ACE_Recursive_Thread_Mutex sm_mutex;
};

// Provider Information Collection
typedef std::map<STRING, ProviderInfo*> ProviderInfoCollection;
typedef std::pair<STRING, ProviderInfo*> ProviderInfoCacheEntry_Pair;

class MG_SERVER_MANAGER_API MgFdoConnectionManager : public MgGuardDisposable
{
    friend class MgCacheManager;

    DECLARE_CLASSNAME(MgFdoConnectionManager)

public:
    virtual ~MgFdoConnectionManager();

    // MgDisposable method
    virtual void Dispose();

    static MgFdoConnectionManager* GetInstance();

    // This initializes the FDO connection manager
    void Initialize(bool bFdoConnectionPoolEnabled,
                    INT32 nFdoConnectionPoolSize,
                    INT32 nFdoConnectionTimeout,
                    STRING excludedProviders,
                    STRING fdoConnectionPoolSizeCustom,
                    STRING fdoConnectionUseLimit);

    static void Terminate();
    void ClearCache();

    FdoIConnection* Open(MgResourceIdentifier* resourceIdentifier);
    FdoIConnection* Open(CREFSTRING provider, CREFSTRING connectionString);
    void Close(FdoIConnection* pFdoConnection);

    STRING UpdateProviderName(CREFSTRING provider);

    void RemoveExpiredFdoConnections();
    bool RemoveCachedFdoConnection(CREFSTRING resource, bool strict = true);
    bool RemoveCachedFdoConnection(MgResourceIdentifier* resource, bool strict = true);
    void MakeFdoConnectionAvailable(FdoIConnection* connection);
    bool SetCachedFdoConnectionAsInvalid(MgResourceIdentifier* resource);

    void ShowCache();
    void ShowProviderInfoCache();

    STRING GetFdoCacheInfo();

private:
    // Constructor
    MgFdoConnectionManager();

    FdoIConnection* FindFdoConnection(MgResourceIdentifier* resourceIdentifier, bool reuseOnly = false);
    FdoIConnection* FindFdoConnection(CREFSTRING provider, CREFSTRING connectionString, bool reuseOnly = false);
    FdoIConnection* SearchFdoConnectionCache(CREFSTRING provider, CREFSTRING key, CREFSTRING ltName, bool reuseOnly = false);
    void CacheFdoConnection(FdoIConnection* pFdoConnection, CREFSTRING provider, CREFSTRING key, CREFSTRING ltName);
    bool UpdateFdoConnectionCache(CREFSTRING provider);

    void SetConfiguration(CREFSTRING provider, FdoIConnection* pFdoConnection, MgResourceIdentifier* resourceIdentifier, STRING& configDataName);
    void SetConnectionProperties(FdoIConnection* pFdoConnection, MdfModel::FeatureSource* pFeatureSource);

    void SetConnectionTimeout(FdoIConnection* pFdoConnection, STRING providerName);

    void ActivateSpatialContext(FdoIConnection* pFdoConnection, STRING& spatialContextName);
    void ActivateLongTransaction(FdoIConnection* pFdoConnection, STRING& longTransactionName);
    bool SupportsConfiguration(FdoIConnection* pFdoConnection);

    bool SupportsCommand(FdoIConnection* pFdoConnection, INT32 commandType);
    void Open(FdoIConnection* pFdoConnection);

    bool IsExcludedProvider(CREFSTRING provider);
    ProviderInfo* GetProviderInformation(CREFSTRING provider);
    ProviderInfo* TryAcquireFdoConnection(CREFSTRING provider);
    ProviderInfo* AcquireFdoConnection(CREFSTRING provider);

    void SubstituteConnectionTags(REFSTRING connectionStr);
    void ScrambleConnectionTags(REFSTRING connectionStr);

    static Ptr<MgFdoConnectionManager> sm_fdoConnectionManager;
    static ACE_Recursive_Thread_Mutex  sm_mutex;

    IConnectionManager* m_connManager;
    ProviderInfoCollection m_ProviderInfoCollection;

    bool m_bFdoConnectionPoolEnabled;
    INT32 m_nFdoConnectionPoolSize;
    INT32 m_nFdoConnectionTimeout;
    Ptr<MgStringCollection> m_excludedProviders;
    Ptr<MgStringCollection> m_fdoConnectionPoolSizeCustomCol;
    Ptr<MgStringCollection> m_fdoConnectionUseLimitCol;
};

#endif
