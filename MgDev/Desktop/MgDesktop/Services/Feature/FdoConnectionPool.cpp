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
    ScopedLock scc(g_pool.mutex);
    STRING fsIdStr = featureSourceId->ToString();
    ConnPool::iterator it = g_pool.freePool.find(fsIdStr);

    MdfModel::FeatureSource* fs = MgFdoConnectionUtil::GetFeatureSource(featureSourceId);
    STRING providerName = MgFdoConnectionUtil::ParseNonQualifiedProviderName(fs->GetProvider());

    //no connection for this string in the pool -- make one
    if (!g_bPoolingEnabled || it == g_pool.freePool.end() || it->second.size() == 0)
    {
        FdoPtr<FdoIConnection> conn = MgFdoConnectionUtil::CreateConnection(featureSourceId);
    #ifdef DEBUG_FDO_CONNECTION_POOL
        ACE_DEBUG((LM_INFO, ACE_TEXT("[Created]: (%W)\n"), featureSourceId->ToString().c_str()));
    #endif
        return conn.Detach();
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
            return rec._conn;
        }
        else
        {
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("Provider for (%W) is not poolable\n"), featureSourceId->ToString().c_str()));
        #endif
            FdoPtr<FdoIConnection> conn = MgFdoConnectionUtil::CreateConnection(featureSourceId);
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("[Created]: (%W)\n"), featureSourceId->ToString().c_str()));
        #endif
            return conn.Detach();
        }
    }

    return NULL;
}

void MgFdoConnectionPool::ReturnConnection(MgFeatureConnection* conn)
{
    ScopedLock scc(g_pool.mutex);
    FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();
    Ptr<MgResourceIdentifier> fsId = conn->GetFeatureSource();
    STRING fsIdStr = fsId->ToString();
    fdoConn->Close();

    //Only return it to pool if pooling enabled
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
        }
        else
        {
        #ifdef DEBUG_FDO_CONNECTION_POOL
            ACE_DEBUG((LM_INFO, ACE_TEXT("Connection (%W) is not poolable\n"), fsIdStr.c_str()));
        #endif
        }
    }
}

void MgFdoConnectionPool::Initialize(MgConfiguration* pConfiguration)
{
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
}

void MgFdoConnectionPool::Cleanup()
{
    ScopedLock scc(g_pool.mutex);
    
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
}

void MgFdoConnectionPool::PurgeCachedConnections(MgResourceIdentifier* resId)
{
    ScopedLock scc(g_pool.mutex);

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
    }
}

void MgFdoConnectionPool::PurgeCachedConnectionsUnderFolder(MgResourceIdentifier* resId)
{
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