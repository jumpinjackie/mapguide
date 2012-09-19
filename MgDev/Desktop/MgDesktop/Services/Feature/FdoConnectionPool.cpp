#include "MgDesktop.h"
#include "System/Mutex.h"
#include "FdoConnectionPool.h"
#include "FdoConnectionUtil.h"

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
        MgdFdoConnectionPool::Cleanup();
    }

    ConnPool freePool;
    //ConnPool busyPool;
    MgdMutex    MgdMutex;
};

//The One instance of the connection pool
PoolHolder g_pool;
Ptr<MgStringCollection> g_excludedProviders;
bool g_bPoolingEnabled;

FdoIConnection* MgdFdoConnectionPool::GetConnection(MgResourceIdentifier* featureSourceId)
{
    FdoPtr<FdoIConnection> conn;
    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(featureSourceId, L"MgdFdoConnectionPool::GetConnection");

    ScopedLock scc(g_pool.MgdMutex);
    STRING fsIdStr = featureSourceId->ToString();
    ConnPool::iterator it = g_pool.freePool.find(fsIdStr);

    MdfModel::FeatureSource* fs = MgdFdoConnectionUtil::GetFeatureSource(featureSourceId);
    STRING providerName = MgdFdoConnectionUtil::ParseNonQualifiedProviderName(fs->GetProvider());

    bool bNewInstance = false;

    //no connection for this string in the pool -- make one
    if (!g_bPoolingEnabled || it == g_pool.freePool.end() || it->second.size() == 0)
    {
        conn = MgdFdoConnectionUtil::CreateConnection(featureSourceId);
        conn->Open();
    #ifdef DEBUG_FDO_CONNECTION_POOL
        ACE_DEBUG((LM_INFO, ACE_TEXT("[Created]: (%W) (refcount: %d)\n"), featureSourceId->ToString().c_str(), conn->GetRefCount()));
    #endif
        bNewInstance = true;
    }
    else
    {
        if (!g_excludedProviders->Contains(providerName))
        {
            PoolRec rec = it->second.back();
            it->second.pop_back();
            if (FdoConnectionState_Closed == rec._conn->GetConnectionState())
                rec._conn->Open();
            conn = rec._conn;
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Re-used]: (%W) %d in cache (refcount: %d)\n"), featureSourceId->ToString().c_str(), it->second.size(), conn->GetRefCount()));
        #endif
            bNewInstance = false;
        }
        else
        {
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("Provider for (%W) is not poolable\n"), featureSourceId->ToString().c_str()));
        #endif
            conn = MgdFdoConnectionUtil::CreateConnection(featureSourceId);
            conn->Open();
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Created]: (%W) (refcount: %d)\n"), featureSourceId->ToString().c_str(), conn->GetRefCount()));
        #endif
            bNewInstance = true;
        }
    }

    MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::InternalTrace, L"MgdFdoConnectionPool::GetConnection", mgStackParams);
    logDetail.AddResourceIdentifier(L"featureSourceId", featureSourceId);
    logDetail.AddBool(L"IsNewInstance", bNewInstance);
    logDetail.Create();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFdoConnectionPool::GetConnection")

    return conn.Detach();
}

FdoIConnection* MgdFdoConnectionPool::GetConnection(CREFSTRING providerName, CREFSTRING connectionString)
{
    return MgdFdoConnectionUtil::CreateConnection(providerName, connectionString);
}

void MgdFdoConnectionPool::ReturnConnection(MgdFeatureConnection* conn)
{
    MG_FEATURE_SERVICE_TRY()

    ScopedLock scc(g_pool.MgdMutex);
    STRING providerName = conn->GetProviderName();
    FdoPtr<FdoIConnection> fdoConn = conn->m_fdoConn; //conn->GetConnection();
    
    //If feature source is empty, it means the connection was not created via its feature source id
    //meaning it's not poolable and can't be returned.
    STRING fsIdStr;
    Ptr<MgResourceIdentifier> fsId = conn->GetFeatureSource();
    if (NULL != fsId.p)
        fsIdStr = fsId->ToString();

    bool bReturned = false;
    bool bProviderExcluded = false;

    //Only return it to pool if pooling enabled. Connections returned to the pool stay open otherwise close them
    if (g_bPoolingEnabled)
    {
        STRING providerName = MgdFdoConnectionUtil::ParseNonQualifiedProviderName(conn->GetProviderName());
        if (!g_excludedProviders->Contains(providerName) && !fsIdStr.empty())
        {
            std::vector<PoolRec>& vec = g_pool.freePool[fsIdStr];
            vec.push_back(PoolRec(fdoConn, time(NULL), fsIdStr));
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Returned] (%W) %d in cache (refcount: %d)\n"), fsIdStr.c_str(), vec.size(), fdoConn->GetRefCount()));
        #endif
            bReturned = true;
        }
        else
        {
            MgdFdoConnectionUtil::CloseConnection(fdoConn);
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Closed] (%W) - Provider excluded from pooling (refcount: %d)\n"), fsIdStr.c_str(), fdoConn->GetRefCount()));
        #endif
            bProviderExcluded = true;
        }
    }
    else
    {
        MgdFdoConnectionUtil::CloseConnection(fdoConn);
        #ifdef DEBUG_FDO_CONNECTION_POOL
        ACE_DEBUG((LM_INFO, ACE_TEXT("[Closed] (%W) - Connection Pooling disabled (refcount: %d)\n"), fsIdStr.c_str(), fdoConn->GetRefCount()));
        #endif
    }

    MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::InternalTrace, L"MgdFdoConnectionPool::ReturnConnection", mgStackParams);
    logDetail.AddString(L"FeatureSource", fsIdStr.empty() ? L"<No Feature Source>" : fsIdStr);
    logDetail.AddBool(L"ReturnedToPool", bReturned);
    logDetail.AddBool(L"ProviderExcluded", bProviderExcluded);
    logDetail.Create();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFdoConnectionPool::ReturnConnection")   
}

void MgdFdoConnectionPool::Initialize(MgConfiguration* pConfiguration)
{
    MG_FEATURE_SERVICE_TRY()

    bool bDataConnectionPoolEnabled = MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolEnabled;
    INT32 nDataConnectionPoolSize = MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize;
    STRING excludedProviders = MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolExcludedProviders;
    STRING fdoConnectionPoolSizeCustom = MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSizeCustom;
    STRING fdoConnectionUseLimit = MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionUseLimit;

    pConfiguration->GetBoolValue(MgdConfigProperties::FeatureServicePropertiesSection, 
                                 MgdConfigProperties::FeatureServicePropertyDataConnectionPoolEnabled, 
                                 bDataConnectionPoolEnabled, 
                                 MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolEnabled);

    pConfiguration->GetIntValue(MgdConfigProperties::FeatureServicePropertiesSection, 
                                MgdConfigProperties::FeatureServicePropertyDataConnectionPoolSize, 
                                nDataConnectionPoolSize, 
                                MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize);

    pConfiguration->GetStringValue(MgdConfigProperties::FeatureServicePropertiesSection, 
                                   MgdConfigProperties::FeatureServicePropertyDataConnectionPoolExcludedProviders, 
                                   excludedProviders, 
                                   MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolExcludedProviders);

    pConfiguration->GetStringValue(MgdConfigProperties::FeatureServicePropertiesSection, 
                                   MgdConfigProperties::FeatureServicePropertyDataConnectionPoolSizeCustom, 
                                   fdoConnectionPoolSizeCustom, 
                                   MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSizeCustom);

    pConfiguration->GetStringValue(MgdConfigProperties::FeatureServicePropertiesSection, 
                                   MgdConfigProperties::FeatureServicePropertyDataConnectionUseLimit, 
                                   fdoConnectionUseLimit, 
                                   MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionUseLimit);

    //TODO: Add support for all of the data pooling configuration options
    g_bPoolingEnabled = bDataConnectionPoolEnabled;
    g_excludedProviders = MgStringCollection::ParseCollection(excludedProviders, L",");
    if (NULL == (MgStringCollection*)g_excludedProviders)
    {
        g_excludedProviders = new MgStringCollection();
    }

    MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::InternalTrace, L"MgdFdoConnectionPool::Initialize", mgStackParams);
    logDetail.AddBool(L"PoolingEnabled", g_bPoolingEnabled);
    logDetail.AddString(L"ExcludedProviders", excludedProviders);
    logDetail.Create();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFdoConnectionPool::Initialize")
}

void MgdFdoConnectionPool::Cleanup()
{
    MG_FEATURE_SERVICE_TRY()

    ScopedLock scc(g_pool.MgdMutex);

    for (ConnPool::iterator it = g_pool.freePool.begin(); it != g_pool.freePool.end(); ++it)
    {
        while (it->second.size())
        {
            MgdFdoConnectionUtil::CloseConnection(it->second.back()._conn);
            it->second.back()._conn->Release();
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Cleanup]: (%W) %d in cache\n"), it->second.back()._fsId.c_str(), it->second.size()));
        #endif
            it->second.pop_back();
        }
    }

#ifdef DEBUG_FDO_CONNECTION_POOL
    MgdFdoConnectionUtil::CheckCallStats();
#endif

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFdoConnectionPool::Cleanup")
}

void MgdFdoConnectionPool::PurgeCachedConnections(MgResourceIdentifier* resId)
{
    if (NULL == resId)
        return;

    if (MgResourceType::FeatureSource != resId->GetResourceType())
        return;

    ScopedLock scc(g_pool.MgdMutex);

    MG_FEATURE_SERVICE_TRY()

    STRING fsIdStr = resId->ToString();
    ConnPool::iterator it = g_pool.freePool.find(fsIdStr);
    if (it != g_pool.freePool.end() && it->second.size() > 0)
    {
        INT32 purged = 0;
        while (it->second.size())
        {
            MgdFdoConnectionUtil::CloseConnection(it->second.back()._conn);
            it->second.back()._conn->Release();
            it->second.pop_back();
            purged++;
        }
    #ifdef DEBUG_FDO_CONNECTION_POOL
        ACE_DEBUG((LM_INFO, ACE_TEXT("[Purge]: (%W) %d purged\n"), fsIdStr.c_str(), purged));
    #endif
        MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::InternalTrace, L"MgdFdoConnectionPool::PurgeCachedConnections", mgStackParams);
        logDetail.AddResourceIdentifier(L"resId", resId);
        logDetail.AddInt32(L"purgedConnections", purged);
        logDetail.Create();
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFdoConnectionPool::PurgeCachedConnections")
}

void MgdFdoConnectionPool::PurgeCachedConnectionsUnderFolder(MgResourceIdentifier* resId)
{
    if (NULL == resId)
        return;

    if (MgResourceType::Folder != resId->GetResourceType())
        return;

    MG_FEATURE_SERVICE_TRY()

    ScopedLock scc(g_pool.MgdMutex);

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
                MgdFdoConnectionUtil::CloseConnection(it->second.back()._conn);
                it->second.back()._conn->Release();
                it->second.pop_back();
                purged++;
            }
        }
    }
#ifdef DEBUG_FDO_CONNECTION_POOL
    ACE_DEBUG((LM_INFO, ACE_TEXT("[Purge]: (%W) %d purged\n"), fsIdStr.c_str(), purged));
#endif

    MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::InternalTrace, L"MgdFdoConnectionPool::PurgeCachedConnectionsUnderFolder", mgStackParams);
    logDetail.AddResourceIdentifier(L"resId", resId);
    logDetail.AddInt32(L"purgedConnections", purged);
    logDetail.Create();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFdoConnectionPool::PurgeCachedConnectionsUnderFolder")
}

bool MgdFdoConnectionPool::StringStartsWith(CREFSTRING haystack, CREFSTRING needle)
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

void MgdFdoConnectionPool::GetCacheInfo(std::vector<PoolCacheEntry*>& entries)
{
    ScopedLock scc(g_pool.MgdMutex);

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