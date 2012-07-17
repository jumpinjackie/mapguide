#include "MgDesktop.h"
#include "System/Mutex.h"
#include "FdoConnectionPool.h"
#include "FdoConnectionUtil.h"

INT64 MgFdoConnectionPool::sm_nConnectionsRequested = 0L;
INT64 MgFdoConnectionPool::sm_nConnectionsReturned = 0L;

//connection pool record -- contains database pointer
//plus extra timing information/status
struct PoolRec
{
    PoolRec(FdoIConnection* conn, time_t tm, CREFSTRING fsId)
    {
        _conn = FDO_SAFE_ADDREF(conn);
        _stamp = tm;
        _fsId = fsId;
    }

    FdoIConnection* _conn;
    time_t _stamp;
    STRING _fsId;
};

typedef std::map<STRING, std::vector<PoolRec> > ConnPool;

//makes sure connections are closed on exit
struct PoolHolder
{
    ~PoolHolder()
    {
        MgFdoConnectionPool::Cleanup();
    }

    ConnPool freePool;
    //ConnPool busyPool;
    Mutex    mutex;
};

//The One instance of the connection pool
PoolHolder g_pool;
Ptr<MgStringCollection> g_excludedProviders;
bool g_bPoolingEnabled;

FdoIConnection* MgFdoConnectionPool::GetConnection(MgResourceIdentifier* featureSourceId)
{
    FdoPtr<FdoIConnection> conn;
    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(featureSourceId, L"MgFdoConnectionPool::GetConnection");

    ScopedLock scc(g_pool.mutex);
    STRING fsIdStr = featureSourceId->ToString();
    ConnPool::iterator it = g_pool.freePool.find(fsIdStr);

    MdfModel::FeatureSource* fs = MgFdoConnectionUtil::GetFeatureSource(featureSourceId);
    STRING providerName = MgFdoConnectionUtil::ParseNonQualifiedProviderName(fs->GetProvider());

    bool bNewInstance = false;

    //no connection for this string in the pool -- make one
    if (!g_bPoolingEnabled || it == g_pool.freePool.end() || it->second.size() == 0)
    {
        conn = MgFdoConnectionUtil::CreateConnection(featureSourceId);
    #ifdef DEBUG_FDO_CONNECTION_POOL
        ACE_DEBUG((LM_INFO, ACE_TEXT("[Created]: (%W)\n"), featureSourceId->ToString().c_str()));
    #endif
        conn->Open();
        bNewInstance = true;
    }
    else
    {
        if (!g_excludedProviders->Contains(providerName))
        {
            PoolRec rec = it->second.back();
            it->second.pop_back();
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Re-used]: (%W) %d in cache\n"), featureSourceId->ToString().c_str(), it->second.size()));
        #endif
            if (FdoConnectionState_Closed == rec._conn->GetConnectionState())
                rec._conn->Open();
            conn = rec._conn;
            bNewInstance = false;
        }
        else
        {
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("Provider for (%W) is not poolable\n"), featureSourceId->ToString().c_str()));
        #endif
            conn = MgFdoConnectionUtil::CreateConnection(featureSourceId);
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Created]: (%W)\n"), featureSourceId->ToString().c_str()));
        #endif
            conn->Open();
            bNewInstance = true;
        }
    }

    MgLogDetail logDetail(MgServiceType::FeatureService, MgLogDetail::InternalTrace, L"MgFdoConnectionPool::GetConnection", mgStackParams);
    logDetail.AddResourceIdentifier(L"featureSourceId", featureSourceId);
    logDetail.AddBool(L"IsNewInstance", bNewInstance);
    logDetail.Create();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgFdoConnectionPool::GetConnection")

    sm_nConnectionsRequested++;

    return conn.Detach();
}

void MgFdoConnectionPool::ReturnConnection(MgFeatureConnection* conn)
{
    MG_FEATURE_SERVICE_TRY()

    ScopedLock scc(g_pool.mutex);
    STRING providerName = conn->GetProviderName();
    FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();
    
    STRING fsIdStr;
    Ptr<MgResourceIdentifier> fsId = conn->GetFeatureSource();
    if (NULL != fsId.p)
        fsIdStr = fsId->ToString();

    bool bReturned = false;
    bool bProviderExcluded = false;

    //Only return it to pool if pooling enabled. Connections returned to the pool stay open otherwise close them
    if (g_bPoolingEnabled)
    {
        STRING providerName = MgFdoConnectionUtil::ParseNonQualifiedProviderName(conn->GetProviderName());
        if (!g_excludedProviders->Contains(providerName))
        {
            std::vector<PoolRec>& vec = g_pool.freePool[fsIdStr];
            vec.push_back(PoolRec(fdoConn, time(NULL), fsIdStr));
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Returned] (%W) %d in cache\n"), fsIdStr.c_str(), vec.size()));
        #endif
            bReturned = true;
        }
        else
        {
            fdoConn->Close();
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Closed] (%W) - Provider excluded from pooling\n"), fsIdStr.c_str()));
        #endif
            bProviderExcluded = true;
        }
    }
    else
    {
        fdoConn->Close();
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Closed] (%W) - Connection Pooling disabled\n"), fsIdStr.c_str()));
        #endif
    }

    MgLogDetail logDetail(MgServiceType::FeatureService, MgLogDetail::InternalTrace, L"MgFdoConnectionPool::ReturnConnection", mgStackParams);
    logDetail.AddResourceIdentifier(L"FeatureSource", fsId);
    logDetail.AddBool(L"ReturnedToPool", bReturned);
    logDetail.AddBool(L"ProviderExcluded", bProviderExcluded);
    logDetail.Create();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgFdoConnectionPool::ReturnConnection")

    sm_nConnectionsReturned++;
}

void MgFdoConnectionPool::Initialize(MgConfiguration* pConfiguration)
{
    sm_nConnectionsRequested = 0L;
    sm_nConnectionsReturned = 0L;

    MG_FEATURE_SERVICE_TRY()

    bool bDataConnectionPoolEnabled = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolEnabled;
    INT32 nDataConnectionPoolSize = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize;
    STRING excludedProviders = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolExcludedProviders;
    STRING fdoConnectionPoolSizeCustom = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSizeCustom;
    STRING fdoConnectionUseLimit = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionUseLimit;

    pConfiguration->GetBoolValue(MgConfigProperties::FeatureServicePropertiesSection, 
                                 MgConfigProperties::FeatureServicePropertyDataConnectionPoolEnabled, 
                                 bDataConnectionPoolEnabled, 
                                 MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolEnabled);

    pConfiguration->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection, 
                                MgConfigProperties::FeatureServicePropertyDataConnectionPoolSize, 
                                nDataConnectionPoolSize, 
                                MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize);

    pConfiguration->GetStringValue(MgConfigProperties::FeatureServicePropertiesSection, 
                                   MgConfigProperties::FeatureServicePropertyDataConnectionPoolExcludedProviders, 
                                   excludedProviders, 
                                   MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolExcludedProviders);

    pConfiguration->GetStringValue(MgConfigProperties::FeatureServicePropertiesSection, 
                                   MgConfigProperties::FeatureServicePropertyDataConnectionPoolSizeCustom, 
                                   fdoConnectionPoolSizeCustom, 
                                   MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSizeCustom);

    pConfiguration->GetStringValue(MgConfigProperties::FeatureServicePropertiesSection, 
                                   MgConfigProperties::FeatureServicePropertyDataConnectionUseLimit, 
                                   fdoConnectionUseLimit, 
                                   MgConfigProperties::DefaultFeatureServicePropertyDataConnectionUseLimit);

    //TODO: Add support for all of the data pooling configuration options
    g_bPoolingEnabled = bDataConnectionPoolEnabled;
    g_excludedProviders = MgStringCollection::ParseCollection(excludedProviders, L",");
    if (NULL == (MgStringCollection*)g_excludedProviders)
    {
        g_excludedProviders = new MgStringCollection();
    }

    MgLogDetail logDetail(MgServiceType::FeatureService, MgLogDetail::InternalTrace, L"MgFdoConnectionPool::Initialize", mgStackParams);
    logDetail.AddBool(L"PoolingEnabled", g_bPoolingEnabled);
    logDetail.AddString(L"ExcludedProviders", excludedProviders);
    logDetail.Create();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgFdoConnectionPool::Initialize")
}

void MgFdoConnectionPool::Cleanup()
{
    MG_FEATURE_SERVICE_TRY()

    ScopedLock scc(g_pool.mutex);

    MgLogDetail logDetail(MgServiceType::FeatureService, MgLogDetail::InternalTrace, L"MgFdoConnectionPool::Cleanup", mgStackParams);
    logDetail.AddInt64(L"ConnectionsRequested", sm_nConnectionsRequested);
    logDetail.AddInt64(L"ConnectionsReturned", sm_nConnectionsReturned);
    logDetail.Create();

    if (sm_nConnectionsRequested != sm_nConnectionsReturned)
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("[WARNING] %d connections have leaked for this session (ie. not returned)\n"), (sm_nConnectionsRequested - sm_nConnectionsReturned)));
    }
    
    for (ConnPool::iterator it = g_pool.freePool.begin(); it != g_pool.freePool.end(); ++it)
    {
        while (it->second.size())
        {
            it->second.back()._conn->Close();
            it->second.back()._conn->Release();
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Cleanup]: (%W) %d in cache\n"), it->second.back()._fsId.c_str(), it->second.size()));
        #endif
            it->second.pop_back();
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgFdoConnectionPool::Cleanup")
}

void MgFdoConnectionPool::PurgeCachedConnections(MgResourceIdentifier* resId)
{
    if (NULL == resId)
        return;

    if (MgResourceType::FeatureSource != resId->GetResourceType())
        return;

    ScopedLock scc(g_pool.mutex);

    MG_FEATURE_SERVICE_TRY()

    STRING fsIdStr = resId->ToString();
    ConnPool::iterator it = g_pool.freePool.find(fsIdStr);
    if (it != g_pool.freePool.end() && it->second.size() > 0)
    {
        INT32 purged = 0;
        while (it->second.size())
        {
            it->second.back()._conn->Close();
            it->second.back()._conn->Release();
            it->second.pop_back();
            purged++;
        }
    #ifdef DEBUG_FDO_CONNECTION_POOL
        ACE_DEBUG((LM_INFO, ACE_TEXT("[Purge]: (%W) %d purged\n"), fsIdStr.c_str(), purged));
    #endif
        MgLogDetail logDetail(MgServiceType::FeatureService, MgLogDetail::InternalTrace, L"MgFdoConnectionPool::PurgeCachedConnections", mgStackParams);
        logDetail.AddResourceIdentifier(L"resId", resId);
        logDetail.AddInt32(L"purgedConnections", purged);
        logDetail.Create();
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgFdoConnectionPool::PurgeCachedConnections")
}

void MgFdoConnectionPool::PurgeCachedConnectionsUnderFolder(MgResourceIdentifier* resId)
{
    if (NULL == resId)
        return;

    if (MgResourceType::Folder != resId->GetResourceType())
        return;

    MG_FEATURE_SERVICE_TRY()

    ScopedLock scc(g_pool.mutex);

    STRING fsIdStr = resId->ToString();
    INT32 purged = 0;
    for (ConnPool::iterator it = g_pool.freePool.begin(); it != g_pool.freePool.end(); it++)
    {
        STRING fsId = it->first;
        bool match = StringStartsWith(fsId, fsIdStr);
    #ifdef DEBUG_FDO_CONNECTION_POOL
        ACE_DEBUG((LM_INFO, ACE_TEXT("[Purge]: (%W) starts with (%W) = %W\n"), fsId.c_str(), fsIdStr.c_str(), match ? L"true" : L"false"));
    #endif
        if (match)
        {
            while (it->second.size())
            {
                it->second.back()._conn->Close();
                it->second.back()._conn->Release();
                it->second.pop_back();
                purged++;
            }
        }
    }
#ifdef DEBUG_FDO_CONNECTION_POOL
    ACE_DEBUG((LM_INFO, ACE_TEXT("[Purge]: (%W) %d purged\n"), fsIdStr.c_str(), purged));
#endif

    MgLogDetail logDetail(MgServiceType::FeatureService, MgLogDetail::InternalTrace, L"MgFdoConnectionPool::PurgeCachedConnectionsUnderFolder", mgStackParams);
    logDetail.AddResourceIdentifier(L"resId", resId);
    logDetail.AddInt32(L"purgedConnections", purged);
    logDetail.Create();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgFdoConnectionPool::PurgeCachedConnectionsUnderFolder")
}

bool MgFdoConnectionPool::StringStartsWith(CREFSTRING haystack, CREFSTRING needle)
{
    if (haystack.empty() || needle.empty())
        return false;

    if (haystack.length() < needle.length())
        return false;

    for (INT32 i = 0; i < needle.length(); i++)
    {
        if (needle[i] != haystack[i])
            return false;
    }
    return true;
}

void MgFdoConnectionPool::GetCacheInfo(std::vector<PoolCacheEntry*>& entries)
{
    ScopedLock scc(g_pool.mutex);

    for (ConnPool::iterator it = g_pool.freePool.begin(); it != g_pool.freePool.end(); ++it)
    {
        INT32 open = 0;
        INT32 closed = 0;
        STRING resId = it->first;
        STRING providerName;

        INT32 counted = 0;

        for (std::vector<PoolRec>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            if (it2->_conn->GetConnectionState() == FdoConnectionState_Closed)
                closed++;
            else
                open++;

            if (providerName.empty())
            {
                FdoPtr<FdoIConnectionInfo> connInfo = it2->_conn->GetConnectionInfo();
                providerName = connInfo->GetProviderName();
            }

            counted++;
        }

        //Don't care about zero-size entries
        if (counted > 0)
        {
            PoolCacheEntry* ent = new PoolCacheEntry(resId, providerName, open, closed);
            entries.push_back(ent);
        }
    }
}