//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#ifndef WIN32
#undef min
#undef max
#endif

#include "ServerManagerDllExport.h"
#include "MapGuideCommon.h"
#include "Fdo.h"
#include "ServerFeatureServiceExceptionDef.h"
#include "FSDSAX2Parser.h"

class MgXmlUtil;

#define MG_FDOCONNECTION_MANAGER_TRY()                         MG_FEATURE_SERVICE_TRY()
#define MG_FDOCONNECTION_MANAGER_CATCH(methodName)             MG_FEATURE_SERVICE_CATCH(methodName)
#define MG_FDOCONNECTION_MANAGER_THROW()                       MG_FEATURE_SERVICE_THROW()
#define MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(methodName)   MG_FEATURE_SERVICE_CATCH_AND_THROW(methodName)

typedef struct {
    STRING ltName;                              // Current long transaction name for this connection
    FdoIConnection* pFdoConnection;
    ACE_Time_Value lastUsed;

} FdoConnectionCacheEntry;

// Feature Source Cache
typedef std::map<STRING, MdfModel::FeatureSource*> FeatureSourceCache;
typedef std::pair<STRING, MdfModel::FeatureSource*> FeatureSourceCacheEntry_Pair;

// FDO Connection Cache
typedef std::multimap<STRING, FdoConnectionCacheEntry*> FdoConnectionCache;
typedef std::pair<STRING, FdoConnectionCacheEntry*> FdoConnectionCacheEntry_Pair;

// FDO Connection Cache Collection
typedef std::map<STRING, FdoConnectionCache*> FdoConnectionCacheCollection;
typedef std::pair<STRING, FdoConnectionCache*> FdoConnectionCache_Pair;

// Spatial Context
typedef std::map<STRING, STRING> MgSpatialContextInfoMap;
typedef std::pair<STRING, STRING> MgSpatialContextInfoPair;


typedef struct {
    INT32 cacheSize;
    FdoThreadCapability threadModel;
} ProviderInfo;

// Provider Information Collection
typedef std::map<STRING, ProviderInfo*> ProviderInfoCollection;
typedef std::pair<STRING, ProviderInfo*> ProviderInfoCacheEntry_Pair;

class MG_SERVER_MANAGER_API MgFdoConnectionManager : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgFdoConnectionManager)

public:
    virtual ~MgFdoConnectionManager(void);

    // MgDisposable method
    virtual void Dispose();

    static MgFdoConnectionManager* GetInstance(void);

    // This initializes the FDO connection manager
    void Initialize(bool bFdoConnectionPoolEnabled, INT32 nFdoConnectionPoolSize, INT32 nFdoConnectionTimeout, STRING excludedProviders, STRING fdoConnectionPoolSizeCustom);
    static void Terminate();
    void ClearCache();

    FdoIConnection* Open(MgResourceIdentifier* resourceIdentifier);
    FdoIConnection* Open(CREFSTRING provider, CREFSTRING connectionString);
    void Close(FdoIConnection* pFdoConnection);

    MgSpatialContextInfoMap* GetSpatialContextInfo(MgResourceIdentifier* resourceIdentifier);
    STRING UpdateProviderName(CREFSTRING provider);

    void RetrieveFeatureSource(MgResourceIdentifier* resource, string& resourceContent);

    MdfModel::FeatureSource* GetFeatureSource(MgResourceIdentifier* resId);

    void RemoveExpiredConnections();
    bool RemoveCachedFdoConnection(CREFSTRING key);

#ifdef _DEBUG
    void ShowCache(void);
#endif

private:
    // Constructor
    MgFdoConnectionManager(void);

    FdoIConnection* FindFdoConnection(MgResourceIdentifier* resourceIdentifier);
    FdoIConnection* FindFdoConnection(CREFSTRING provider, CREFSTRING connectionString);
    FdoIConnection* SearchFdoConnectionCache(CREFSTRING provider, CREFSTRING key, CREFSTRING ltName);
    void CacheFdoConnection(FdoIConnection* pFdoConnection, CREFSTRING provider, CREFSTRING key, CREFSTRING ltName);
    bool FdoConnectionCacheFull(CREFSTRING provider);

    void GetSpatialContextInfoFromXml(MdfModel::FeatureSource* pFeatureSource, MgSpatialContextInfoMap* spatialContextInfoMap);

    void SetConfiguration(CREFSTRING provider, FdoIConnection* pFdoConnection, MgResourceIdentifier* resourceIdentifier, STRING& configDataName);
    void SetConnectionProperties(FdoIConnection* pFdoConnection, MgXmlUtil* pXmlUtil);
    void SetConnectionProperties(FdoIConnection* pFdoConnection, MdfModel::FeatureSource* pFeatureSource);

    void ActivateSpatialContext(FdoIConnection* pFdoConnection, STRING& spatialContextName);
    void ActivateLongTransaction(FdoIConnection* pFdoConnection, STRING& longTransactionName);
    bool SupportsConfiguration(FdoIConnection* pFdoConnection);

    bool SupportsCommand(FdoIConnection* pFdoConnection, INT32 commandType);
    void ValidateFeatureSource(string& featureSourceXmlContent);
    void Open(FdoIConnection* pFdoConnection);

    bool IsExcludedProvider(CREFSTRING provider);
    ProviderInfo* GetProviderInformation(CREFSTRING provider);

    static Ptr<MgFdoConnectionManager> sm_fdoConnectionManager;
    static ACE_Recursive_Thread_Mutex  sm_mutex;
    IConnectionManager*                m_connManager;

    FdoConnectionCacheCollection m_FdoConnectionCacheCollection;
    FeatureSourceCache m_FeatureSourceCache;
    ProviderInfoCollection m_ProviderInfoCollection;

    bool m_bFdoConnectionPoolEnabled;
    INT32 m_nFdoConnectionPoolSize;
    INT32 m_nFdoConnectionTimeout;
    Ptr<MgStringCollection> m_excludedProviders;
};

#endif
