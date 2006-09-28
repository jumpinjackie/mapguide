//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "AceCommon.h"
#include "ResourceService.h"
#include "Fdo.h"
#include "ServerFeatureServiceExceptionDef.h"

class MgXmlUtil;

#define MG_FDOCONNECTION_MANAGER_TRY()                         MG_FEATURE_SERVICE_TRY()
#define MG_FDOCONNECTION_MANAGER_CATCH(methodName)             MG_FEATURE_SERVICE_CATCH(methodName)
#define MG_FDOCONNECTION_MANAGER_THROW()                       MG_FEATURE_SERVICE_THROW()
#define MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(methodName)   MG_FEATURE_SERVICE_CATCH_AND_THROW(methodName)

typedef struct {
    STRING data; // XML content or connection string
    FdoIConnection* pFdoConnection;
    ACE_Time_Value lastUsed;

} FdoConnectionCacheEntry;

typedef std::multimap<STRING, FdoConnectionCacheEntry*> FdoConnectionCache;
typedef std::pair<STRING, FdoConnectionCacheEntry*> FdoConnectionCache_Pair;
typedef std::map<STRING, STRING> MgSpatialContextInfoMap;
typedef std::pair<STRING, STRING> MgSpatialContextInfoPair;

class MG_SERVER_MANAGER_API MgFdoConnectionManager : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgFdoConnectionManager)

public:
    virtual ~MgFdoConnectionManager(void);

    // MgDisposable method
    virtual void Dispose();

    static MgFdoConnectionManager* GetInstance(void);

    // This initializes the FDO connection manager
    void Initialize(bool bFdoConnectionPoolEnabled, INT32 nFdoConnectionPoolSize, INT32 nFdoConnectionTimeout, STRING excludedProviders);
    static void Terminate();
    void ClearCache();

    FdoIConnection* Open(MgResourceIdentifier* resourceIdentifier);
    FdoIConnection* Open(CREFSTRING providerName, CREFSTRING connectionString);
    void Close(FdoIConnection* pFdoConnection);

    MgSpatialContextInfoMap* GetSpatialContextInfo(MgResourceIdentifier* resourceIdentifier);
    STRING UpdateProviderName(CREFSTRING providerName);

    void RemoveExpiredConnections();
    bool RemoveCachedFdoConnection(CREFSTRING key);

#ifdef _DEBUG
    void ShowCache(void);
#endif

private:
    // Constructor
    MgFdoConnectionManager(void);

    FdoIConnection* FindFdoConnection(MgResourceIdentifier* resourceIdentifier);
    FdoIConnection* FindFdoConnection(CREFSTRING providerName, CREFSTRING connectionString);
    FdoIConnection* SearchFdoConnectionCache(CREFSTRING key, CREFSTRING data);
    void CacheFdoConnection(FdoIConnection* pFdoConnection, CREFSTRING key, CREFSTRING data);
    bool FdoConnectionCacheFull(void);

    void RetrieveFeatureSource(MgResourceIdentifier* resource, string& resourceContent);
    void GetConnectionPropertiesFromXml(MgXmlUtil* pXmlUtil, STRING& providerName, STRING& configDocumentName, STRING& longTransactionName);
    void GetSpatialContextInfoFromXml(MgXmlUtil* pXmlUtil, MgSpatialContextInfoMap* spatialContextInfoMap);

    void SetConfiguration(CREFSTRING providerName, FdoIConnection* pFdoConnection, MgResourceIdentifier* resourceIdentifier, STRING& configDataName);
    void SetConnectionProperties(FdoIConnection* pFdoConnection, MgXmlUtil* pXmlUtil);

    void ActivateSpatialContext(FdoIConnection* pFdoConnection, STRING& spatialContextName);
    void ActivateLongTransaction(FdoIConnection* pFdoConnection, STRING& longTransactionName);
    bool SupportsConfiguration(FdoIConnection* pFdoConnection);

    bool SupportsCommand(FdoIConnection* pFdoConnection, INT32 commandType);
    void ValidateFeatureSource(string& featureSourceXmlContent);
    void Open(FdoIConnection* pFdoConnection);

    bool IsExcludedProvider(CREFSTRING providerName);

    static Ptr<MgFdoConnectionManager> sm_fdoConnectionManager;
    static ACE_Recursive_Thread_Mutex  sm_mutex;
    IConnectionManager*                m_connManager;

    FdoConnectionCache m_FdoConnectionCache;

    bool m_bFdoConnectionPoolEnabled;
    INT32 m_nFdoConnectionPoolSize;
    INT32 m_nFdoConnectionTimeout;
    Ptr<MgStringCollection> m_excludedProviders;

};

#endif
