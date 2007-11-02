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

#include "FdoConnectionManager.h"
#include "System/ConfigProperties.h"
#include "ServiceManager.h"
#include "LogManager.h"
#include "LongTransactionManager.h"
#include "CacheManager.h"

ACE_Recursive_Thread_Mutex MgFdoConnectionManager::sm_mutex;

// Process-wide MgFdoConnectionManager
Ptr<MgFdoConnectionManager> MgFdoConnectionManager::sm_fdoConnectionManager = (MgFdoConnectionManager*)NULL;

// Uncomment the next line to debug the FDO Connection Manager cache
//#define _DEBUG_FDOCONNECTION_MANAGER

// Constructor
MgFdoConnectionManager::MgFdoConnectionManager() :
    m_connManager(NULL),
    m_bFdoConnectionPoolEnabled(false),
    m_nFdoConnectionPoolSize(0),
    m_nFdoConnectionTimeout(0)
{
}

// Destructor
MgFdoConnectionManager::~MgFdoConnectionManager(void)
{
    MG_FDOCONNECTION_MANAGER_TRY()

    ClearCache();

    FDO_SAFE_RELEASE(m_connManager);

    MG_FDOCONNECTION_MANAGER_CATCH(L"MgFdoConnectionManager.~MgFdoConnectionManager")
}


void MgFdoConnectionManager::Dispose(void)
{
    delete this;
}

// Get pointer to a process-wide MgFdoConnectionManager.
MgFdoConnectionManager* MgFdoConnectionManager::GetInstance()
{
    MG_FDOCONNECTION_MANAGER_TRY()

    ACE_TRACE("MgFdoConnectionManager::GetInstance");

    if (NULL == MgFdoConnectionManager::sm_fdoConnectionManager.p)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), NULL));

        if (NULL == MgFdoConnectionManager::sm_fdoConnectionManager.p)
        {
            MgFdoConnectionManager::sm_fdoConnectionManager = new MgFdoConnectionManager();
        }
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.GetInstance")

    ACE_ASSERT(NULL != MgFdoConnectionManager::sm_fdoConnectionManager.p);

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgFdoConnectionManager::sm_fdoConnectionManager;
}


///----------------------------------------------------------------------------
/// <summary>
/// Intializes the FDO connection manager.
/// This method must be called once during the server startup time.
/// </summary>
///----------------------------------------------------------------------------

void MgFdoConnectionManager::Initialize(bool bFdoConnectionPoolEnabled,
                                        INT32 nFdoConnectionPoolSize,
                                        INT32 nFdoConnectionTimeout,
                                        STRING excludedProviders,
                                        STRING fdoConnectionPoolSizeCustom)
{
    MG_FDOCONNECTION_MANAGER_TRY()

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::Initialize()\n")));
    MG_LOG_TRACE_ENTRY(L"MgFdoConnectionManager::Initialize()");

    m_connManager = FdoFeatureAccessManager::GetConnectionManager();
    CHECKNULL(m_connManager, L"MgFdoConnectionManager.Initialize()");

    if(1 > nFdoConnectionPoolSize)
    {
        nFdoConnectionPoolSize = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize;
    }

    m_bFdoConnectionPoolEnabled = bFdoConnectionPoolEnabled;
    m_nFdoConnectionPoolSize = nFdoConnectionPoolSize;
    m_nFdoConnectionTimeout = nFdoConnectionTimeout;

    // Parse the comma delimited string into a string collection
    m_excludedProviders = MgStringCollection::ParseCollection(excludedProviders, L",");

    m_fdoConnectionPoolSizeCustomCol = MgStringCollection::ParseCollection(fdoConnectionPoolSizeCustom, L",");

    // Update the provider cache size collection
    if (m_fdoConnectionPoolSizeCustomCol.p)
    {
        for(INT32 i=0;i<m_fdoConnectionPoolSizeCustomCol->GetCount();i++)
        {
            STRING customPoolSize = m_fdoConnectionPoolSizeCustomCol->GetItem(i);

            STRING provider = customPoolSize;
            INT32 nCustomPoolSize = nFdoConnectionPoolSize;

            // Parse the format: provider:size
            // Example: OSGeo.SDF:10

            size_t position = customPoolSize.find(L":", 0); // NOXLATE
            if(position != string::npos)
            {
                provider = customPoolSize.substr(0, position);
                STRING value = customPoolSize.substr(position+1, customPoolSize.size());
                nCustomPoolSize = MgUtil::StringToInt32(value);
            }

            ProviderInfo* providerInfo = new ProviderInfo(provider, nCustomPoolSize, (FdoThreadCapability)-1, (m_bFdoConnectionPoolEnabled & !IsExcludedProvider(provider)));
            if(providerInfo)
            {
                m_ProviderInfoCollection.insert(ProviderInfoCacheEntry_Pair(provider, providerInfo));
            }
        }
    }

#ifdef _DEBUG
    ShowProviderInfoCache();
#endif

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.Initialize")
}


///----------------------------------------------------------------------------
/// <summary>
/// Cleans up the FDO connection manager.
/// This method must be called once during the server shutdown time.
/// </summary>
///----------------------------------------------------------------------------

void MgFdoConnectionManager::Terminate()
{
    MgFdoConnectionManager::sm_fdoConnectionManager = NULL;
}


FdoIConnection* MgFdoConnectionManager::Open(MgResourceIdentifier* resourceIdentifier)
{
    FdoPtr<FdoIConnection> pFdoConnection;
    ProviderInfo* providerInfo = NULL;

    MG_FDOCONNECTION_MANAGER_TRY()

    ACE_TRACE ("MgFdoConnectionManager::Open");

    MgCacheManager* cacheManager = MgCacheManager::GetInstance();
    Ptr<MgFeatureSourceCacheItem> cacheItem = cacheManager->GetFeatureSourceCacheItem(resourceIdentifier);
    MdfModel::FeatureSource* featureSource = cacheItem->Get();
    STRING provider = (STRING)featureSource->GetProvider();
    provider = UpdateProviderName(provider);

    // Try to acquire a connection. We will either get a connection or exhaust the re-try logic
    providerInfo = TryAcquireFdoConnection(provider);
    if(providerInfo)
    {
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

        if(m_bFdoConnectionPoolEnabled)
        {
            // Search the cache for an FDO connection matching this resourceIdentifier
            // The content and long transaction name must also match, as the information may change
            pFdoConnection = FindFdoConnection(resourceIdentifier);
        }

        if(NULL == pFdoConnection)
        {
            // Parse XML and get properties
            STRING configDocumentName = (STRING)featureSource->GetConfigurationDocument();
            STRING longTransactionName = (STRING)featureSource->GetLongTransaction();

            // Update the long transaction name to any active one for the current request
            MgLongTransactionManager::GetLongTransactionName(resourceIdentifier, longTransactionName);

            // Create a new connection
            pFdoConnection = m_connManager->CreateConnection(provider.c_str());

            // Check if we have thread model
            if((FdoThreadCapability)-1 == providerInfo->GetThreadModel())
            {
                // Get the connection capabilities
                FdoPtr<FdoIConnectionCapabilities> ficc = pFdoConnection->GetConnectionCapabilities();

                // Get the thread model
                FdoThreadCapability ftc = ficc->GetThreadCapability();

                // Update the provider information collection
                providerInfo->SetThreadModel(ftc);
            }

            // Retrieve the properties and open the connection
            SetConnectionProperties(pFdoConnection, featureSource);

            SetConfiguration(provider, pFdoConnection, resourceIdentifier, configDocumentName);

            Open(pFdoConnection);

            // The following are only applicable when we are creating from a resource identifier
            ActivateLongTransaction(pFdoConnection, longTransactionName);

            // Add this entry to the cache
            CacheFdoConnection(pFdoConnection,
                               provider,
                               resourceIdentifier->ToString(),
                               longTransactionName);
        }

        #ifdef _DEBUG_FDOCONNECTION_MANAGER
        ShowProviderInfoCache();
        #endif
    }
    else
    {
        // Could not establish another connection to the provider!
        #ifdef _DEBUG_FDOCONNECTION_MANAGER
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("********** (%P|%t) Unable to create connection\n")));
        #endif

        MgStringCollection arguments;
        arguments.Add(provider);

        throw new MgAllProviderConnectionsUsedException(L"MgFdoConnectionManager.Open",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_FDOCONNECTION_MANAGER_CATCH(L"MgFdoConnectionManager.Open")
    if (mgException != NULL)
    {
        // There was an exception so remove any connection
        // A connection was available
        if(providerInfo)
        {
            // Update the # of current connections
            providerInfo->DecrementCurrentConnections();
        }
    }
    MG_FDOCONNECTION_MANAGER_THROW()

    return pFdoConnection.Detach();
}

FdoIConnection* MgFdoConnectionManager::Open(CREFSTRING provider, CREFSTRING connectionString)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    FdoPtr<FdoIConnection> pFdoConnection;
    ProviderInfo* providerInfo = NULL;

    MG_FDOCONNECTION_MANAGER_TRY()

    ACE_TRACE ("MgFdoConnectionManager::Open");

    // The connection string may contain substitution tags that need updating
    STRING updatedConnectionString = connectionString;
    MgUserInformation* userInfo =  MgUserInformation::GetCurrentUserInfo();
    if(userInfo)
    {
        SubstituteTag(MgResourceTag::LoginUsername, userInfo->GetUserName(), updatedConnectionString);
        SubstituteTag(MgResourceTag::LoginPassword, userInfo->GetPassword(), updatedConnectionString);
    }

    // Connection string should have something.

    // Empty connection string is allowed for ODBC provider to retrieve
    // DataSources
    if (provider.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(
            L"MgFdoConnectionManager.Open",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    STRING providerNoVersion = UpdateProviderName(provider);

    // Try to acquire a connection. We will either get a connection or exhaust the re-try logic
    providerInfo = TryAcquireFdoConnection(providerNoVersion);
    if(providerInfo)
    {
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

        if(m_bFdoConnectionPoolEnabled)
        {
            // Search the cache for an FDO connection matching this provider/connection string
            pFdoConnection = FindFdoConnection(providerNoVersion, updatedConnectionString);
        }

        if(NULL == pFdoConnection)
        {
            // Create a new connection and add it to the cache
            pFdoConnection = m_connManager->CreateConnection(providerNoVersion.c_str());

            // Check if we have thread model
            if((FdoThreadCapability)-1 == providerInfo->GetThreadModel())
            {
                // Get the connection capabilities
                FdoPtr<FdoIConnectionCapabilities> ficc = pFdoConnection->GetConnectionCapabilities();

                // Get the thread model
                FdoThreadCapability ftc = ficc->GetThreadCapability();

                // Update the provider information collection
                providerInfo->SetThreadModel(ftc);
            }

            // No connection string. So connection will remain in closed state
            if (!updatedConnectionString.empty())
            {
                // Set the connection properties
                pFdoConnection->SetConnectionString(updatedConnectionString.c_str());

                // Open the connection to the FDO provider
                Open(pFdoConnection);
            }

            // Add this entry to the cache
            STRING ltName = L"";
            CacheFdoConnection(pFdoConnection,
                               providerNoVersion,
                               updatedConnectionString,
                               ltName);
        }

        #ifdef _DEBUG_FDOCONNECTION_MANAGER
        ShowProviderInfoCache();
        #endif
    }
    else
    {
        // Could not establish another connection to the provider!
        #ifdef _DEBUG_FDOCONNECTION_MANAGER
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("********** (%P|%t) Unable to create connection\n")));
        #endif

        MgStringCollection arguments;
        arguments.Add(providerNoVersion);

        throw new MgAllProviderConnectionsUsedException(L"MgFdoConnectionManager.Open",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_FDOCONNECTION_MANAGER_CATCH(L"MgFdoConnectionManager.Open")
    if (mgException != NULL)
    {
        // A connection was available
        if(providerInfo)
        {
            // Update the # of current connections
            providerInfo->DecrementCurrentConnections();
        }
    }
    MG_FDOCONNECTION_MANAGER_THROW()

    return pFdoConnection.Detach();
}


void MgFdoConnectionManager::Close(FdoIConnection* pFdoConnection)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    CHECKNULL((FdoIConnection*)pFdoConnection, L"MgFdoConnectionManager.Close()");

    MG_FDOCONNECTION_MANAGER_TRY()

    ACE_TRACE ("MgFdoConnectionManager::Close");

    FDO_SAFE_RELEASE(pFdoConnection);

    RemoveUnusedFdoConnections();

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.Close")
}


// TODO: Need to tie in MgServerFeatureReaderIdentifierPool and any other pools when we remove cached connection and close it
void MgFdoConnectionManager::RemoveExpiredFdoConnections()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_FDOCONNECTION_MANAGER_TRY()

    ACE_Time_Value now = ACE_OS::gettimeofday();

    // Loop all of the providers to get the FDO connection caches
    ProviderInfoCollection::iterator iterProviderInfoCollection = m_ProviderInfoCollection.begin();
    while(m_ProviderInfoCollection.end() != iterProviderInfoCollection)
    {
        ProviderInfo* providerInfo = iterProviderInfoCollection->second;
        if(providerInfo)
        {
            FdoConnectionCache* fdoConnectionCache = providerInfo->GetFdoConnectionCache();
            if(fdoConnectionCache)
            {
                FdoConnectionCache::iterator iter = fdoConnectionCache->begin();

                while(fdoConnectionCache->end() != iter)
                {
                    FdoConnectionCacheEntry* pFdoConnectionCacheEntry = iter->second;
                    if(pFdoConnectionCacheEntry)
                    {
                        INT32 time = now.sec() - pFdoConnectionCacheEntry->lastUsed.sec();
                        if(time > m_nFdoConnectionTimeout)
                        {
                            // Connection has expired so close it and remove it
                            pFdoConnectionCacheEntry->pFdoConnection->Close();

                            // Release any resource
                            FDO_SAFE_RELEASE(pFdoConnectionCacheEntry->pFdoConnection);

                            delete pFdoConnectionCacheEntry;
                            pFdoConnectionCacheEntry = NULL;

                            fdoConnectionCache->erase(iter++);

                            #ifdef _DEBUG_FDOCONNECTION_MANAGER
                            ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::RemoveExpiredFdoConnections - Found expired cached FDO connection.\n")));
                            #endif
                        }
                        else
                        {
                            // Check the next cached connection
                            iter++;
                        }
                    }
                    else
                    {
                        ACE_ASSERT(false);
                        iter++;
                    }
                }

                // If there are no cached entries left, reset the # of current connections
                if(fdoConnectionCache->size() == 0)
                {
                    providerInfo->ResetCurrentConnections();
                }
            }
        }
        else
        {
            ACE_ASSERT(false);
        }

        // Next provider FDO connection cache
        iterProviderInfoCollection++;
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.RemoveExpiredFdoConnections")
}


FdoIConnection* MgFdoConnectionManager::FindFdoConnection(MgResourceIdentifier* resourceIdentifier)
{
    CHECKNULL(resourceIdentifier, L"MgFdoConnectionManager.FindFdoConnection()");

    FdoIConnection* pFdoConnection = NULL;

    MG_FDOCONNECTION_MANAGER_TRY()

    // Parse feature source XML and get provider
    MgCacheManager* cacheManager = MgCacheManager::GetInstance();
    Ptr<MgFeatureSourceCacheItem> cacheItem = cacheManager->GetFeatureSourceCacheItem(resourceIdentifier);
    MdfModel::FeatureSource* featureSource = cacheItem->Get();

    STRING provider = (STRING)featureSource->GetProvider();
    provider = UpdateProviderName(provider);

    // Get the active long transaction name for the current request
    STRING ltName = L"";

    if(!MgLongTransactionManager::GetLongTransactionName(resourceIdentifier, ltName))
    {
        // No long transaction name cached for the current session or no current session
        // In this case we want to use the requested long transaction of the feature source

        // Get long transaction

        STRING longTransactionName = (STRING)featureSource->GetLongTransaction();

        ltName = longTransactionName;
    }

    pFdoConnection = SearchFdoConnectionCache(provider,
                                              resourceIdentifier->ToString(),
                                              ltName);

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.FindFdoConnection")

    return pFdoConnection;
}


FdoIConnection* MgFdoConnectionManager::FindFdoConnection(CREFSTRING provider, CREFSTRING connectionString)
{
    FdoIConnection* pFdoConnection = NULL;

    MG_FDOCONNECTION_MANAGER_TRY()

    STRING providerNoVersion = UpdateProviderName(provider);
    STRING ltName = L"";

    pFdoConnection = SearchFdoConnectionCache(providerNoVersion, connectionString, ltName);

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.FindFdoConnection")

    return pFdoConnection;
}


FdoIConnection* MgFdoConnectionManager::SearchFdoConnectionCache(CREFSTRING provider, CREFSTRING key, CREFSTRING ltName)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    FdoIConnection* pFdoConnection = NULL;

    MG_FDOCONNECTION_MANAGER_TRY()

    // Loop all of the providers to get the FDO connection caches
    ProviderInfoCollection::iterator iterProviderInfoCollection = m_ProviderInfoCollection.find(provider);
    if(m_ProviderInfoCollection.end() != iterProviderInfoCollection)
    {
        ProviderInfo* providerInfo = iterProviderInfoCollection->second;
        if(providerInfo)
        {
            FdoConnectionCache* fdoConnectionCache = providerInfo->GetFdoConnectionCache();
            if(fdoConnectionCache)
            {
                FdoConnectionCache::iterator iter = fdoConnectionCache->find(key);
                while(fdoConnectionCache->end() != iter && key == iter->first)
                {
                    // We have a key match
                    FdoConnectionCacheEntry* pFdoConnectionCacheEntry = iter->second;
                    if(pFdoConnectionCacheEntry)
                    {
                        if(pFdoConnectionCacheEntry->ltName == ltName)
                        {
                            // We have a long transaction name match
                            if(pFdoConnectionCacheEntry->pFdoConnection->GetRefCount() == 1)
                            {
                                // It is not in use so claim it
                                pFdoConnectionCacheEntry->lastUsed = ACE_OS::gettimeofday();

                                // Check to see if the key is blank which indicates a blank connection string was cached
                                if(0 < key.size())
                                {
                                    // The key was not blank so check if we need to open it
                                    if (FdoConnectionState_Closed == pFdoConnectionCacheEntry->pFdoConnection->GetConnectionState())
                                    {
                                        pFdoConnectionCacheEntry->pFdoConnection->Open();
                                        #ifdef _DEBUG_FDOCONNECTION_MANAGER
                                        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("SearchFdoConnectionCache() - Had to reopen connection!!\n")));
                                        #endif
                                    }
                                }
                                pFdoConnection = FDO_SAFE_ADDREF(pFdoConnectionCacheEntry->pFdoConnection);
                                break;
                            }
                        }
                    }

                    // Next match
                    iter++;
                }
            }
        }
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.SearchFdoConnectionCache")

    #ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("SearchFdoConnectionCache:\nConnection: %@\nKey = %W\nVersion(LT) = %W\n\n"), (void*)pFdoConnection, key.c_str(), ltName.empty() ? L"(empty)" : ltName.c_str()));
    #endif

    return pFdoConnection;
}

void MgFdoConnectionManager::SetConnectionProperties(FdoIConnection *pFdoConnection, MdfModel::FeatureSource *pFeatureSource)
{
        CHECKNULL(pFeatureSource, L"MgFdoConnectionManager.SetConnectionProperties");
        CHECKNULL((FdoIConnection*)pFdoConnection, L"MgFdoConnectionManager.SetConnectionProperties");

        // Get FdoIConnectionInfo
        FdoPtr<FdoIConnectionInfo> fdoConnInfo = pFdoConnection->GetConnectionInfo();
        CHECKNULL((FdoIConnectionInfo*)fdoConnInfo, L"MgFdoConnectionManager.SetConnectionProperties");

        // GetFdoIConnectionPropertyDictionary
        FdoPtr<FdoIConnectionPropertyDictionary> fdoConnPropertyDict = fdoConnInfo->GetConnectionProperties();
        CHECKNULL((FdoIConnectionPropertyDictionary*)fdoConnPropertyDict, L"MgFdoConnectionManager.SetConnectionProperties");

        // Get all all connection properties
        MdfModel::NameStringPairCollection* parameters = pFeatureSource->GetParameters();
        CHECKNULL(parameters, L"MgFdoConnectionManager.SetConnectionProperties");

        for (int i = 0; i < parameters->GetCount(); i++)
        {
            // Get the Name and Value elements
            MdfModel::NameStringPair* pair = parameters->GetAt(i);
            STRING name = (STRING)pair->GetName();
            STRING value = (STRING)pair->GetValue();

            // If name is null, means invalid xml
            if (name.empty())
            {
                STRING message = MgUtil::GetResourceMessage(MgResources::FeatureService, L"MgInvalidPropertyName");

                Ptr<MgStringCollection> strCol;
                if (!message.empty())
                {
                    strCol = new MgStringCollection();
                    strCol->Add(message);
                }

                throw new MgInvalidFeatureSourceException(L"MgFdoConnectionManager.SetConnectionProperties",
                    __LINE__, __WFILE__, (MgStringCollection*)strCol, L"", NULL);
            }

            FdoString* propertyName = name.c_str();
            CHECKNULL(propertyName, L"MgFdoConnectionManager.SetConnectionProperties");

            // Property value can be null ( optional properties may not have values )
            if (!value.empty())
            {
                FdoString* propertyValue = value.c_str();
                if (propertyValue != NULL)
                {
                    fdoConnPropertyDict->SetProperty(propertyName, propertyValue);
                }
            }
        }
}

void MgFdoConnectionManager::ActivateSpatialContext(FdoIConnection* pFdoConnection, STRING& spatialContextName)
{
    CHECKNULL((FdoIConnection*)pFdoConnection, L"MgFdoConnectionManager.ActivateSpatialContext()");
    // If command is not supported we simply return and ignore element from xml
    if (!SupportsCommand(pFdoConnection, FdoCommandType_ActivateSpatialContext))
    {
        return;
    }

    // No spatial context specified
    if (spatialContextName.length() <=0)
    {
        return;
    }

    // Spatial Context can be executed when connection is already open
    if (pFdoConnection->GetConnectionState() == FdoConnectionState_Open)
    {
        FdoPtr<FdoIActivateSpatialContext> fdoCommand = (FdoIActivateSpatialContext*)pFdoConnection->CreateCommand(FdoCommandType_ActivateSpatialContext);
        CHECKNULL((FdoIActivateSpatialContext*)fdoCommand, L"MgFdoConnectionManager.ActivateSpatialContext");
        // Set the spatial context from the feature source
        fdoCommand->SetName(spatialContextName.c_str());
        // Execute the command to activate
        fdoCommand->Execute();
    }
}


void MgFdoConnectionManager::ActivateLongTransaction(FdoIConnection* pFdoConnection, STRING& longTransactionName)
{
    CHECKNULL((FdoIConnection*)pFdoConnection, L"MgFdoConnectionManager.ActivateLongTransaction()");

    // If command is not supported we simply return and ignore element from xml
    if (!SupportsCommand(pFdoConnection, FdoCommandType_ActivateLongTransaction))
    {
        return;
    }

    // No long transaction specified
    if (longTransactionName.length() <=0)
    {
        return;
    }

    // Long transaction can be executed when connection is already open
    if (pFdoConnection->GetConnectionState() == FdoConnectionState_Open)
    {
        FdoPtr<FdoIActivateLongTransaction> fdoCommand = (FdoIActivateLongTransaction*)pFdoConnection->CreateCommand(FdoCommandType_ActivateLongTransaction);
        CHECKNULL((FdoIActivateLongTransaction*)fdoCommand, L"MgFdoConnectionManager.ActivateLongTransaction");
        // Set the spatial context from the feature source
        fdoCommand->SetName(longTransactionName.c_str());
        // Execute the command to activate
        fdoCommand->Execute();
    }
}


bool MgFdoConnectionManager::SupportsConfiguration(FdoIConnection* pFdoConnection)
{
    CHECKNULL((FdoIConnection*)pFdoConnection, L"MgFdoConnectionManager.SupportsConfiguration");

    FdoPtr<FdoIConnectionCapabilities> ficc = pFdoConnection->GetConnectionCapabilities();
    CHECKNULL((FdoIConnectionCapabilities*)ficc, L"MgFdoConnectionManager.SupportsConfiguration");

    return ficc->SupportsConfiguration();
}

void MgFdoConnectionManager::SetConfiguration(CREFSTRING provider, FdoIConnection* pFdoConnection, MgResourceIdentifier* resourceIdentifier, STRING& configDataName)
{
    CHECKNULL(resourceIdentifier, L"MgFdoConnectionManager.SetConfiguration");
    CHECKNULL(pFdoConnection, L"MgFdoConnectionManager.SetConfiguration");

    if (provider.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(
            L"MgFdoConnectionManager.SetConfiguration",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    if (!SupportsConfiguration(pFdoConnection))
    {
        // Configuration is not supported
        return;
    }

    // No configration specified. Defaults to provider configuration
    if (configDataName.length() <= 0)
    {
        return;
    }

    // Get the service from service manager
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
        serviceManager->RequestService(MgServiceType::ResourceService));
    ACE_ASSERT(NULL != resourceService.p);

    // Get the feature source data
    Ptr<MgByteReader> byteReader = resourceService->GetResourceData(
        resourceIdentifier, configDataName,
        MgResourcePreProcessingType::Substitution);

    if (byteReader == NULL)
    {
        STRING message = MgUtil::GetResourceMessage(MgResources::FeatureService, L"MgMissingConfiguration");

        Ptr<MgStringCollection> strCol;
        if (!message.empty())
        {
            strCol = new MgStringCollection();
            strCol->Add(message);
        }
        throw new MgInvalidFeatureSourceException(L"MgFdoConnectionManager.SetConfiguration",
            __LINE__, __WFILE__, (MgStringCollection*)strCol, L"", NULL);
    }

    Ptr<MgByte> bytes;
    bytes = NULL;

    MgByteSink byteSink(byteReader);
    bytes = byteSink.ToBuffer();

    if(bytes)
    {
        FdoIoMemoryStreamP stream = FdoIoMemoryStream::Create();
        stream->Write((FdoByte*)bytes->Bytes(), (FdoSize)bytes->GetLength());
        pFdoConnection->SetConfiguration(stream);
    }
}


bool MgFdoConnectionManager::SupportsCommand(FdoIConnection* pFdoConnection, INT32 commandType)
{
    CHECKNULL((FdoIConnection*)pFdoConnection, L"MgFdoConnectionManager.SupportsCommand()");

    FdoPtr<FdoICommandCapabilities> fcc = pFdoConnection->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)fcc, L"MgFdoConnectionManager::SupportsCommand");

    bool supports = false;

    // Find all supported command types
    FdoInt32 cnt = 0;
    FdoInt32* fcmd = fcc->GetCommands(cnt);
    if (cnt > 0 && fcmd != NULL)
    {
        for (FdoInt32 i=0; i < cnt; i++)
        {
            if (fcmd[i] == (FdoInt32)commandType)
            {
                supports = true;
            }
        }
    }

    return supports;
}

void MgFdoConnectionManager::Open(FdoIConnection* pFdoConnection)
{
    try
    {
        // Open the connection to the FDO provider
        if (pFdoConnection != NULL)
        {
            // If the connection is closed then open it
            if (FdoConnectionState_Closed == pFdoConnection->GetConnectionState())
            {
                pFdoConnection->Open();
            }
        }
    }
    catch (FdoException* e)
    {
        STRING messageId;
        MgStringCollection arguments;
        wchar_t* buf = (wchar_t*)e->GetExceptionMessage();

        if (NULL != buf)
        {
            messageId = L"MgFormatInnerExceptionMessage";
            arguments.Add(buf);
        }

        FDO_SAFE_RELEASE(e);

        throw new MgFdoException(L"MgFdoConnectionManager.Open",
            __LINE__, __WFILE__, NULL, messageId, &arguments);
    }
    catch (...)
    {
        throw new MgConnectionFailedException(L"MgFdoConnectionManager.Open",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

STRING MgFdoConnectionManager::UpdateProviderName(CREFSTRING provider)
{
    STRING providerNoVersion = provider;

    // Remove the version from the provider name if it is found
    // ie: OSGeo.SDF.3.0 = OSGeo.SDF
    STRING::size_type index = providerNoVersion.find(L".");
    if(STRING::npos != index)
    {
        index++;

        // Found 1st ".", keep looking for second one
        index = providerNoVersion.find(L".", index);
        if(STRING::npos != index)
        {
            // Found 2nd "."
            // Update provider name to not include version
            providerNoVersion = providerNoVersion.substr(0, index);
        }
    }

    return providerNoVersion;
}

bool MgFdoConnectionManager::RemoveCachedFdoConnection(CREFSTRING resource, bool strict)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    bool success = false;
    INT32 connections = 0;
    INT32 connectionsRemoved = 0;

    MG_FDOCONNECTION_MANAGER_TRY()

    if (resource.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(
            L"MgFdoConnectionManager.RemoveCachedFdoConnection",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    // Loop all of the providers to get the FDO connection caches
    ProviderInfoCollection::iterator iterProviderInfoCollection = m_ProviderInfoCollection.begin();
    while(m_ProviderInfoCollection.end() != iterProviderInfoCollection)
    {
        ProviderInfo* providerInfo = iterProviderInfoCollection->second;
        if(providerInfo)
        {
            FdoConnectionCache* fdoConnectionCache = providerInfo->GetFdoConnectionCache();
            if(fdoConnectionCache)
            {
                FdoConnectionCache::iterator iter = fdoConnectionCache->find(resource);

                // We need to search the entire cache because there could be more then 1 cached
                // connection to the same FDO provider.
                while(fdoConnectionCache->end() != iter && resource == iter->first)
                {
                    // We have a key match
                    FdoConnectionCacheEntry* pFdoConnectionCacheEntry = iter->second;
                    if (pFdoConnectionCacheEntry)
                    {
                        if(pFdoConnectionCacheEntry->pFdoConnection)
                        {
                            connections++;

                            INT32 refCount = pFdoConnectionCacheEntry->pFdoConnection->GetRefCount();

                            // We have a match, is it in use?
                            if(1 == refCount)
                            {
                                // Close the connection
                                pFdoConnectionCacheEntry->pFdoConnection->Close();

                                // Release any resource
                                FDO_SAFE_RELEASE(pFdoConnectionCacheEntry->pFdoConnection);

                                delete pFdoConnectionCacheEntry;
                                pFdoConnectionCacheEntry = NULL;

                                fdoConnectionCache->erase(iter++);

                                connectionsRemoved++;

                                #ifdef _DEBUG_FDOCONNECTION_MANAGER
                                ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::RemoveCachedFdoConnection - Releasing cached FDO connection.\n")));
                                #endif
                            }
                            else
                            {
                                // The resource is still in use and so it cannot be removed
                                #ifdef _DEBUG_FDOCONNECTION_MANAGER
                                ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::RemoveCachedFdoConnection - FDO connection in use!\n")));
                                #endif

                                // Next cached FDO connection
                                iter++;
                            }
                        }
                        else
                        {
                            ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::RemoveCachedFdoConnection - Removed NULL connection\n")));

                            delete pFdoConnectionCacheEntry;
                            pFdoConnectionCacheEntry = NULL;

                            fdoConnectionCache->erase(iter++);
                        }
                    }
                    else
                    {
                        ACE_ASSERT(false);
                        iter++;
                    }
                }
            }
        }
        else
        {
            ACE_ASSERT(false);
        }

        // Next provider FDO connection cache
        iterProviderInfoCollection++;
    }

    success = (connections == connectionsRemoved);

    if (!success)
    {
        // Could not remove the cached FDO connection because it is in use.
        MgStringCollection arguments;
        arguments.Add(resource);

        throw new MgResourceBusyException(
            L"MgFdoConnectionManager.RemoveCachedFdoConnection",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_FDOCONNECTION_MANAGER_CATCH(L"MgFdoConnectionManager.RemoveCachedFdoConnection")

    if (NULL != mgException.p && strict)
    {
        MG_FDOCONNECTION_MANAGER_THROW();
    }

    return success;
}

bool MgFdoConnectionManager::RemoveCachedFdoConnection(MgResourceIdentifier* resource, bool strict)
{
    STRING resId;

    if (NULL != resource)
    {
        resId = resource->ToString();
    }

    return RemoveCachedFdoConnection(resId, strict);
}

void MgFdoConnectionManager::CacheFdoConnection(FdoIConnection* pFdoConnection, CREFSTRING provider, CREFSTRING key, CREFSTRING ltName)
{
    MG_FDOCONNECTION_MANAGER_TRY()

    // Protect the cache
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, ));

    if(!UpdateFdoConnectionCache(provider))
    {
        // Add this entry to the cache
        FdoConnectionCacheEntry* pFdoConnectionCacheEntry = new FdoConnectionCacheEntry;
        if(pFdoConnectionCacheEntry)
        {
            pFdoConnectionCacheEntry->ltName = ltName;
            pFdoConnectionCacheEntry->pFdoConnection = pFdoConnection;
            pFdoConnectionCacheEntry->lastUsed = ACE_OS::gettimeofday();

            #ifdef _DEBUG_FDOCONNECTION_MANAGER
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("CacheFdoConnection:\nConnection: %@\nProvider = %W\nKey = %W\nVersion(LT) = %W\n\n"), (void*)pFdoConnection, provider.c_str(), key.c_str(), ltName.empty() ? L"(empty)" : ltName.c_str()));
            #endif

            // Get the appropriate provider cache
            ProviderInfoCollection::iterator iterProviderInfoCollection = m_ProviderInfoCollection.find(provider);
            if(m_ProviderInfoCollection.end() != iterProviderInfoCollection)
            {
                ProviderInfo* providerInfo = iterProviderInfoCollection->second;
                if(providerInfo)
                {
                    FdoConnectionCache* fdoConnectionCache = providerInfo->GetFdoConnectionCache();
                    if(fdoConnectionCache)
                    {
                        fdoConnectionCache->insert(FdoConnectionCacheEntry_Pair(key, pFdoConnectionCacheEntry));
                    }
                }
            }

            // Increase the reference count before returning it because this entry has been cached
            FDO_SAFE_ADDREF(pFdoConnection);
        }
    }
    else
    {
        // Failed to update the cache to allow for a new cache entry
        #ifdef _DEBUG_FDOCONNECTION_MANAGER
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("********** (%P|%t) Cache Full - Unable to create connection\n")));
        #endif

        MgStringCollection arguments;
        arguments.Add(provider);

        throw new MgAllProviderConnectionsUsedException(L"MgFdoConnectionManager.CacheFdoConnection",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.CacheFdoConnection")
}


bool MgFdoConnectionManager::UpdateFdoConnectionCache(CREFSTRING provider)
{
    bool bCacheFull = false;

    MG_FDOCONNECTION_MANAGER_TRY()

    // Protect the cache
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    ProviderInfoCollection::iterator iterProviderInfoCollection = m_ProviderInfoCollection.find(provider);
    if(m_ProviderInfoCollection.end() != iterProviderInfoCollection)
    {
        ProviderInfo* providerInfo = iterProviderInfoCollection->second;
        if(providerInfo)
        {
            FdoConnectionCache* fdoConnectionCache = providerInfo->GetFdoConnectionCache();
            if(fdoConnectionCache)
            {
                if(fdoConnectionCache->size() == providerInfo->GetPoolSize())
                {
                    bCacheFull = true;

                    // We are full, so we will need to remove the oldest entry
                    FdoConnectionCache::iterator iter = fdoConnectionCache->begin();

                    while(fdoConnectionCache->end() != iter)
                    {
                        FdoConnectionCacheEntry* pFdoConnectionCacheEntry = iter->second;
                        if(pFdoConnectionCacheEntry)
                        {
                            // Is it in use?
                            if(pFdoConnectionCacheEntry->pFdoConnection)
                            {
                                INT32 refCount = pFdoConnectionCacheEntry->pFdoConnection->GetRefCount();

                                // Is it in use?
                                if(1 == refCount)
                                {
                                    // Close the connection
                                    pFdoConnectionCacheEntry->pFdoConnection->Close();

                                    // Release any resource
                                    FDO_SAFE_RELEASE(pFdoConnectionCacheEntry->pFdoConnection);

                                    delete pFdoConnectionCacheEntry;
                                    pFdoConnectionCacheEntry = NULL;

                                    fdoConnectionCache->erase(iter++);

                                    // We were able to free up a cache entry spot
                                    bCacheFull = false;
                                    break;
                                }
                                else
                                {
                                    // Next cached connection
                                    iter++;
                                }
                            }
                            else
                            {
                                ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::UpdateFdoConnectionCache - Removed NULL connection\n")));

                                delete pFdoConnectionCacheEntry;
                                pFdoConnectionCacheEntry = NULL;

                                fdoConnectionCache->erase(iter++);
                            }
                        }
                        else
                        {
                            ACE_ASSERT(false);
                            iter++;
                        }
                    }
                }
            }
        }
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.UpdateFdoConnectionCache")

    return bCacheFull;
}


ProviderInfo* MgFdoConnectionManager::AcquireFdoConnection(CREFSTRING provider)
{
    ProviderInfo* providerInfo = NULL;

    MG_FDOCONNECTION_MANAGER_TRY()

    // Protect the cache
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    ProviderInfoCollection::iterator iterProviderInfoCollection = m_ProviderInfoCollection.find(provider);
    if(m_ProviderInfoCollection.end() != iterProviderInfoCollection)
    {
        providerInfo = iterProviderInfoCollection->second;
        if(providerInfo)
        {
            // Check to see if all connections are in use
            if(providerInfo->GetCurrentConnections() == providerInfo->GetPoolSize())
            {
                // All connections are in use
                providerInfo = NULL;
            }
        }
    }
    else
    {
        // Provider information has not been cached yet so a connection will be available.
        providerInfo = new ProviderInfo(provider, m_nFdoConnectionPoolSize, (FdoThreadCapability)-1, (m_bFdoConnectionPoolEnabled & !IsExcludedProvider(provider)));
        if(providerInfo)
        {
            m_ProviderInfoCollection.insert(ProviderInfoCacheEntry_Pair(provider, providerInfo));
        }
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.AcquireFdoConnection")

    return providerInfo;
}

void MgFdoConnectionManager::ClearCache()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

#ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ClearCache - FDO cache BEFORE\n")));
    ShowCache();
#endif

    size_t cacheCollectionSize = m_ProviderInfoCollection.size();
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ClearCache\n")));
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("Provider Information caches: %d.\n"), cacheCollectionSize));

    // Loop all of the providers
    ProviderInfoCollection::iterator iterProviderInfoCollection = m_ProviderInfoCollection.begin();
    while(m_ProviderInfoCollection.end() != iterProviderInfoCollection)
    {
        // Cleanup the FDO connection cache
        STRING provider = iterProviderInfoCollection->first;
        ProviderInfo* providerInfo = iterProviderInfoCollection->second;
        if(providerInfo)
        {
            FdoConnectionCache* fdoConnectionCache = providerInfo->GetFdoConnectionCache();
            if(fdoConnectionCache)
            {
                size_t cacheSize = fdoConnectionCache->size();
                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("Releasing %d cached %W FDO connections.\n"), cacheSize, provider.c_str()));

                FdoConnectionCache::iterator iter = fdoConnectionCache->begin();
                while(fdoConnectionCache->end() != iter)
                {
                    STRING key = iter->first;
                    FdoConnectionCacheEntry* pFdoConnectionCacheEntry = iter->second;
                    if(pFdoConnectionCacheEntry)
                    {
                        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    %W - "), key.c_str()));

                        if(pFdoConnectionCacheEntry->pFdoConnection)
                        {
                            INT32 refCount = pFdoConnectionCacheEntry->pFdoConnection->GetRefCount();

                            if(1 == refCount)
                            {
                                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("Removed\n")));

                                // The FDO cache is the only one with a reference
                                // Close the connection
                                pFdoConnectionCacheEntry->pFdoConnection->Close();

                                // Release any resource
                                FDO_SAFE_RELEASE(pFdoConnectionCacheEntry->pFdoConnection);

                                delete pFdoConnectionCacheEntry;
                                pFdoConnectionCacheEntry = NULL;

                                fdoConnectionCache->erase(iter++);
                            }
                            else
                            {
                                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("Still in use!!\n")));
                                // Next cached connection
                                iter++;
                            }
                        }
                        else
                        {
                            ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ClearCache - Removed NULL connection\n")));

                            // Remove NULL FDO connection entry
                            delete pFdoConnectionCacheEntry;
                            pFdoConnectionCacheEntry = NULL;

                            fdoConnectionCache->erase(iter++);
                        }
                    }
                    else
                    {
                        ACE_ASSERT(false);
                        iter++;
                    }
                }

#ifdef _DEBUG_FDOCONNECTION_MANAGER
                cacheSize = fdoConnectionCache->size();
                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("%W FDO cached connections still in use %d.\n"), provider.c_str(), cacheSize));
#endif
            }

            delete providerInfo;
            providerInfo = NULL;
        }
        else
        {
            ACE_ASSERT(false);
        }

        // Next provider FDO connection cache
        iterProviderInfoCollection++;
    }

    m_ProviderInfoCollection.clear();

#ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ClearCache - FDO cache AFTER\n")));
    ShowCache();
#endif
}


void MgFdoConnectionManager::ShowCache(void)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_FDOCONNECTION_MANAGER_TRY()

    size_t cacheCollectionSize = m_ProviderInfoCollection.size();
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ShowCache()\n")));
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("Provider Information caches: %d\n"), cacheCollectionSize));

    // Show the contents of the provider information cache collection
    INT32 nIndex = 1;
    for (ProviderInfoCollection::iterator iterCol = m_ProviderInfoCollection.begin();iterCol != m_ProviderInfoCollection.end(); iterCol++)
    {
        STRING provider = iterCol->first;
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("%2d) %W\n"), nIndex++, provider.c_str()));

        ProviderInfo* providerInfo = iterCol->second;
        if(providerInfo)
        {
            FdoConnectionCache* fdoConnectionCache = providerInfo->GetFdoConnectionCache();
            if(fdoConnectionCache)
            {
                INT32 entry = 1;
                if(fdoConnectionCache->size() > 0)
                {
                    for (FdoConnectionCache::iterator iter = fdoConnectionCache->begin();iter != fdoConnectionCache->end(); iter++)
                    {
                        STRING key = iter->first;
                        FdoConnectionCacheEntry* pFdoConnectionCacheEntry = iter->second;
                        if(pFdoConnectionCacheEntry)
                        {
                            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  %2d) %W\n"), entry++, key.c_str()));
                        }
                    }
                }
                else
                {
                    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    No cached entries.\n")));
                }
            }
        }
    }

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n")));

    MG_FDOCONNECTION_MANAGER_CATCH(L"MgFdoConnectionManager.ShowCache")
}

bool MgFdoConnectionManager::IsExcludedProvider(CREFSTRING provider)
{
    bool bResult = false;

    if(m_excludedProviders.p)
    {
        bResult = m_excludedProviders->Contains(provider);
    }

    return bResult;
}

ProviderInfo* MgFdoConnectionManager::GetProviderInformation(CREFSTRING provider)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    ProviderInfo* providerInfo = NULL;

    ProviderInfoCollection::iterator iter = m_ProviderInfoCollection.find(provider);
    if(iter != m_ProviderInfoCollection.end())
    {
        providerInfo = iter->second;
    }

    if(NULL == providerInfo)
    {
        // Create new entry
        providerInfo = new ProviderInfo(provider, m_nFdoConnectionPoolSize, (FdoThreadCapability)-1, (m_bFdoConnectionPoolEnabled & !IsExcludedProvider(provider)));
        if(providerInfo)
        {
            m_ProviderInfoCollection.insert(ProviderInfoCacheEntry_Pair(provider, providerInfo));
        }
    }

    return providerInfo;
}

void MgFdoConnectionManager::ShowProviderInfoCache(void)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_FDOCONNECTION_MANAGER_TRY()

    size_t cacheCollectionSize = m_ProviderInfoCollection.size();
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ShowProviderInfoCache()\n")));
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("Provider Information cache size: %d\n"), cacheCollectionSize));

    // Show the contents of the provider info cache collection
    INT32 nIndex = 1;
    for (ProviderInfoCollection::iterator iterCol = m_ProviderInfoCollection.begin();iterCol != m_ProviderInfoCollection.end(); iterCol++)
    {
        STRING provider = iterCol->first;
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("%2d) %W\n"), nIndex++, provider.c_str()));

        ProviderInfo* providerInfo = iterCol->second;
        if(providerInfo)
        {
            STRING strThreadModel = L"";
            switch(providerInfo->GetThreadModel())
            {
            case FdoThreadCapability_SingleThreaded:
                strThreadModel = L"FdoThreadCapability_SingleThreaded";
                break;
            case FdoThreadCapability_PerConnectionThreaded:
                strThreadModel = L"FdoThreadCapability_PerConnectionThreaded";
                break;
            case FdoThreadCapability_PerCommandThreaded:
                strThreadModel = L"FdoThreadCapability_PerCommandThreaded";
                break;
            case FdoThreadCapability_MultiThreaded:
                strThreadModel = L"FdoThreadCapability_MultiThreaded";
                break;
            default:
                strThreadModel = L"Not initialized.";
                break;
            }

            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  Maximum Pool Size  : %d\n"), providerInfo->GetPoolSize()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  Current Pool Size  : %d\n"), providerInfo->GetFdoConnectionCache()->size()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  Current Connections: %d\n"), providerInfo->GetCurrentConnections()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  Thread Model       : %W\n"), strThreadModel.c_str()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  Keep Cached        : %W\n"), providerInfo->GetKeepCached() ? L"True" : L"False"));

#ifdef _DEBUG
            if((INT32)providerInfo->GetFdoConnectionCache()->size() < providerInfo->GetCurrentConnections())
            {
                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("********** Connection Pool/Current Connections mismatch!!\n")));
                assert(0);
            }
#endif
        }
    }

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n")));

    MG_FDOCONNECTION_MANAGER_CATCH(L"MgFdoConnectionManager.ShowProviderInfoCache")
}

void MgFdoConnectionManager::RemoveUnusedFdoConnections()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_FDOCONNECTION_MANAGER_TRY()

#ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::RemoveUnusedFdoConnections - Provider Info Cache Before\n")));
    ShowProviderInfoCache();
#endif

    // Loop all of the providers to get the FDO connection caches
    ProviderInfoCollection::iterator iterProviderInfoCollection = m_ProviderInfoCollection.begin();
    while(m_ProviderInfoCollection.end() != iterProviderInfoCollection)
    {
        ProviderInfo* providerInfo = iterProviderInfoCollection->second;
        if(providerInfo)
        {
            FdoConnectionCache* fdoConnectionCache = providerInfo->GetFdoConnectionCache();
            if(fdoConnectionCache)
            {
                FdoConnectionCache::iterator iter = fdoConnectionCache->begin();

                while(fdoConnectionCache->end() != iter)
                {
                    FdoConnectionCacheEntry* pFdoConnectionCacheEntry = iter->second;
                    if(pFdoConnectionCacheEntry)
                    {
                        if(pFdoConnectionCacheEntry->pFdoConnection)
                        {
                            INT32 refCount = pFdoConnectionCacheEntry->pFdoConnection->GetRefCount();
                            if(1 == refCount)
                            {
                                #ifdef _DEBUG_FDOCONNECTION_MANAGER
                                ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::RemoveUnusedFdoConnections - Decrementing Connection!\n")));
                                #endif

                                // There are no more references to this provider connection
                                providerInfo->DecrementCurrentConnections();

                                // Are we supposed to release this provider from the cache?
                                if(!providerInfo->GetKeepCached())
                                {
                                    #ifdef _DEBUG_FDOCONNECTION_MANAGER
                                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::RemoveUnusedFdoConnections - Closing Connection!\n")));
                                    #endif

                                    // Close the connection
                                    pFdoConnectionCacheEntry->pFdoConnection->Close();

                                    // Release any resource
                                    FDO_SAFE_RELEASE(pFdoConnectionCacheEntry->pFdoConnection);

                                    delete pFdoConnectionCacheEntry;
                                    pFdoConnectionCacheEntry = NULL;

                                    fdoConnectionCache->erase(iter++);
                                }
                                else
                                {
                                    // Check the next cached connection
                                    iter++;
                                }
                            }
                            else
                            {
                                // Check the next cached connection
                                iter++;
                            }
                        }
                        else
                        {
                            ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::RemoveUnusedFdoConnections - Removed NULL connection\n")));

                            delete pFdoConnectionCacheEntry;
                            pFdoConnectionCacheEntry = NULL;

                            fdoConnectionCache->erase(iter++);
                        }
                    }
                    else
                    {
                        ACE_ASSERT(false);
                        iter++;
                    }
                }
            }
        }
        else
        {
            ACE_ASSERT(false);
        }

        // Next provider FDO connection cache
        iterProviderInfoCollection++;
    }

#ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::RemoveUnusedFdoConnections - Provider Info Cache After\n")));
    ShowProviderInfoCache();
#endif

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.RemoveUnusedFdoConnections")
}

ProviderInfo* MgFdoConnectionManager::TryAcquireFdoConnection(CREFSTRING provider)
{
    ProviderInfo* providerInfo = NULL;
    bool bConnectionAvailable = false;

    // The code segment below needs to be guarded
    {
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

        #ifdef _DEBUG_FDOCONNECTION_MANAGER
        ShowProviderInfoCache();
        #endif

        providerInfo = AcquireFdoConnection(provider);
        if(providerInfo)
        {
            // Increment the # of connections
            providerInfo->IncrementCurrentConnections();

            // We have not reached the limit so proceed with connection
            bConnectionAvailable = true;
        }
    }

    // If no connection is available we need to at least have some re-try logic .
    if(!bConnectionAvailable)
    {
        bool bDone = false;
        bool bRetry = true;
        int nRetries = 0;

        while(!bDone)
        {
            bRetry = true;

            {
                ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

                #ifdef _DEBUG_FDOCONNECTION_MANAGER
                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("********** (%P|%t) Retrying FDO connection\n")));
                #endif

                // Try to acquire a connection?
                providerInfo = AcquireFdoConnection(provider);
                if(providerInfo)
                {
                    // Increment the # of connections
                    providerInfo->IncrementCurrentConnections();

                    // We have not reached the limit so proceed with connection
                    bConnectionAvailable = true;

                    // No retry needed
                    bRetry = false;

                    bDone = true;
                }
            }

            if(bRetry)
            {
                // Pause briefly (100ms) before checking again
                ACE_Time_Value t(0, 100000);
                ACE_OS::sleep(t);
                nRetries++;
                if(nRetries >= 600) // 1 minute worth of retries
                {
                    bDone = true;
                }
            }
        }
    }

    // If we are here it is because we either have a connection available or have exhausted the re-try logic
    if(!bConnectionAvailable)
    {
        // No connection available
        providerInfo = NULL;
    }

    return providerInfo;
}

STRING MgFdoConnectionManager::GetFdoCacheInfo(void)
{
    STRING info = L"";
    wchar_t buffer[255];

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, L""));

    // Add the header
    info = L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n";
    info += L"<FdoCacheInformation>";

    // Add the current time
    MgDateTime now;

    info += L"<TimeStamp>";
    info += now.ToXmlString(false);
    info += L"</TimeStamp>\n";

    // Server configuration settings
    info += L"<ConfigurationSettings>";

    // FDO connection pool enabled
    info += L"<DataConnectionPoolEnabled>";
    info += m_bFdoConnectionPoolEnabled ? L"True" : L"False";
    info += L"</DataConnectionPoolEnabled>\n";

    info += L"<DataConnectionPoolExcludedProviders>";
    if(m_excludedProviders.p)
    {
        for(int i=0;i<m_excludedProviders->GetCount();i++)
        {
            info += m_excludedProviders->GetItem(i);

            if(i+1 < m_excludedProviders->GetCount())
            {
                info += L",";
            }
        }
    }
    info += L"</DataConnectionPoolExcludedProviders>\n";

    info += L"<DataConnectionPoolSize>";
    ACE_OS::itoa(m_nFdoConnectionPoolSize, buffer, 10);
    info += buffer;
    info += L"</DataConnectionPoolSize>\n";

    info += L"<DataConnectionPoolSizeCustom>";
    if(m_fdoConnectionPoolSizeCustomCol.p)
    {
        for(int i=0;i<m_fdoConnectionPoolSizeCustomCol->GetCount();i++)
        {
            info += m_fdoConnectionPoolSizeCustomCol->GetItem(i);

            if(i+1 < m_fdoConnectionPoolSizeCustomCol->GetCount())
            {
                info += L",";
            }
        }
    }
    info += L"</DataConnectionPoolSizeCustom>\n";

    info += L"<DataConnectionTimeout>";
    ACE_OS::itoa(m_nFdoConnectionTimeout, buffer, 10);
    info += buffer;
    info += L"</DataConnectionTimeout>\n";

    info += L"</ConfigurationSettings>";

    MG_FDOCONNECTION_MANAGER_TRY()

    // Show the contents of the provider info cache collection
    INT32 nIndex = 1;
    for (ProviderInfoCollection::iterator iterCol = m_ProviderInfoCollection.begin();iterCol != m_ProviderInfoCollection.end(); iterCol++)
    {
        info += L"<Provider>";

        // Add the name of the provider
        STRING provider = iterCol->first;
        info += L"<Name>";
        info += provider;
        info += L"</Name>\n";

        ProviderInfo* providerInfo = iterCol->second;
        if(providerInfo)
        {
            STRING strThreadModel = L"";
            switch(providerInfo->GetThreadModel())
            {
            case FdoThreadCapability_SingleThreaded:
                strThreadModel = L"FdoThreadCapability_SingleThreaded";
                break;
            case FdoThreadCapability_PerConnectionThreaded:
                strThreadModel = L"FdoThreadCapability_PerConnectionThreaded";
                break;
            case FdoThreadCapability_PerCommandThreaded:
                strThreadModel = L"FdoThreadCapability_PerCommandThreaded";
                break;
            case FdoThreadCapability_MultiThreaded:
                strThreadModel = L"FdoThreadCapability_MultiThreaded";
                break;
            default:
                strThreadModel = L"Not initialized.";
                break;
            }

            // Add the provider information
            info += L"<MaximumDataConnectionPoolSize>";
            ACE_OS::itoa(providerInfo->GetPoolSize(), buffer, 10);
            info += buffer;
            info += L"</MaximumDataConnectionPoolSize>\n";

            info += L"<CurrentDataConnectionPoolSize>";
            ACE_OS::itoa((int)(providerInfo->GetFdoConnectionCache()->size()), buffer, 10);
            info += buffer;
            info += L"</CurrentDataConnectionPoolSize>\n";

            info += L"<CurrentDataConnections>";
            ACE_OS::itoa(providerInfo->GetCurrentConnections(), buffer, 10);
            info += buffer;
            info += L"</CurrentDataConnections>\n";

            info += L"<ThreadModel>";
            info += strThreadModel;
            info += L"</ThreadModel>\n";

            info += L"<KeepDataConnectionsCached>";
            info += providerInfo->GetKeepCached() ? L"True" : L"False";
            info += L"</KeepDataConnectionsCached>\n";

            // Get the cache FDO connection information
            FdoConnectionCache* fdoConnectionCache = providerInfo->GetFdoConnectionCache();
            if(fdoConnectionCache)
            {
                INT32 entry = 1;
                if(fdoConnectionCache->size() > 0)
                {
                    for (FdoConnectionCache::iterator iter = fdoConnectionCache->begin();iter != fdoConnectionCache->end(); iter++)
                    {
                        info += L"<CachedFdoConnection>";

                        STRING key = iter->first;
                        info += L"<Name>";
                        info += key;
                        info += L"</Name>\n";

                        FdoConnectionCacheEntry* pFdoConnectionCacheEntry = iter->second;
                        if(pFdoConnectionCacheEntry)
                        {
                            STRING state = L"";

                            // Status
                            switch(pFdoConnectionCacheEntry->pFdoConnection->GetConnectionState())
                            {
                            case FdoConnectionState_Busy:
                                state = L"Busy";
                                break;
                            case FdoConnectionState_Closed:
                                state = L"Closed";
                                break;
                            case FdoConnectionState_Open:
                                state = L"Open";
                                break;
                            case FdoConnectionState_Pending:
                                state = L"Pending";
                                break;
                            default:
                                state = L"Unknown";
                                break;
                            }

                            info += L"<ConnectionState>";
                            info += state;
                            info += L"</ConnectionState>\n";

                            // Set in use
                            info += L"<InUse>";
                            info += (pFdoConnectionCacheEntry->pFdoConnection->GetRefCount() > 1) ? L"True" : L"False";
                            info += L"</InUse>\n";

                            info += L"<LongTransaction>";
                            info += pFdoConnectionCacheEntry->ltName;
                            info += L"</LongTransaction>\n";

                            MgDateTime lastUsed((time_t)pFdoConnectionCacheEntry->lastUsed.sec());
                            info += L"<LastUsed>";
                            info += lastUsed.ToXmlString(false);
                            info += L"</LastUsed>\n";
                        }

                        info += L"</CachedFdoConnection>\n";
                    }
                }
            }
        }

        info += L"</Provider>\n";
    }

    MG_FDOCONNECTION_MANAGER_CATCH(L"MgFdoConnectionManager.GetFdoCacheInfo")

    info += L"</FdoCacheInformation>\n";

    return info;
}

int MgFdoConnectionManager::SubstituteTag(CREFSTRING name, CREFSTRING value, REFSTRING doc)
{
    int count = 0;
    size_t index;
    size_t pos1 = name.length();
    size_t pos2 = value.length();

    while (wstring::npos != (index = doc.find(name)))
    {
        doc.replace(index, pos1, value, 0, pos2);
        ++count;
    }

    return count;
}
