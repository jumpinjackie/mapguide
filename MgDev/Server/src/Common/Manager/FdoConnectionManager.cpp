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
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include "ServiceManager.h"
#include "LogManager.h"
#include "LongTransactionManager.h"

ACE_Recursive_Thread_Mutex MgFdoConnectionManager::sm_mutex;

// Process-wide MgFdoConnectionManager
Ptr<MgFdoConnectionManager> MgFdoConnectionManager::sm_fdoConnectionManager = (MgFdoConnectionManager*)NULL;

// Uncomment the next line to debug the FDO Connection Manager cache
//#define _DEBUG_FDOCONNECTION_MANAGER

// Constructor
MgFdoConnectionManager::MgFdoConnectionManager(void)
{
    m_connManager = NULL;

    m_bFdoConnectionPoolEnabled = false;
    m_nFdoConnectionPoolSize = 0;
    m_nFdoConnectionTimeout = 0;
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

    ACE_TRACE ("MgFdoConnectionManager::GetInstance");

    if (MgFdoConnectionManager::sm_fdoConnectionManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgFdoConnectionManager::sm_fdoConnectionManager == NULL)
        {
            MgFdoConnectionManager::sm_fdoConnectionManager = new MgFdoConnectionManager;
        }
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.GetInstance")

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

    if((MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize < nFdoConnectionPoolSize) ||
       (1 > nFdoConnectionPoolSize))
    {
        nFdoConnectionPoolSize = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize;
    }

    m_bFdoConnectionPoolEnabled = bFdoConnectionPoolEnabled;
    m_nFdoConnectionPoolSize = nFdoConnectionPoolSize;
    m_nFdoConnectionTimeout = nFdoConnectionTimeout;

    // Parse the comma delimited string into a string collection
    m_excludedProviders = MgStringCollection::ParseCollection(excludedProviders, L",");

    Ptr<MgStringCollection> fdoConnectionPoolSizeCustomCol = MgStringCollection::ParseCollection(fdoConnectionPoolSizeCustom, L",");

    // Update the provider cache size collection
    if (fdoConnectionPoolSizeCustomCol.p)
    {
        for(INT32 i=0;i<fdoConnectionPoolSizeCustomCol->GetCount();i++)
        {
            STRING customPoolSize = fdoConnectionPoolSizeCustomCol->GetItem(i);

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

    MdfModel::FeatureSource* featureSource = GetFeatureSource(resourceIdentifier);
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

    // Connection string should have something.

    // Empty connection string is allowed for ODBC provider to retrieve
    // DataSources
    if (provider.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgFdoConnectionManager.Open",
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
            pFdoConnection = FindFdoConnection(providerNoVersion, connectionString);
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
            if (!connectionString.empty())
            {
                // Set the connection properties
                pFdoConnection->SetConnectionString(connectionString.c_str());

                // Open the connection to the FDO provider
                Open(pFdoConnection);
            }

            // Add this entry to the cache
            STRING ltName = L"";
            CacheFdoConnection(pFdoConnection, 
                               providerNoVersion, 
                               connectionString, 
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

    // Release reference
    FDO_SAFE_RELEASE(pFdoConnection);

    // Update the FDO connections
    UpdateConnections();

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.Close")
}


// TODO: Need to tie in MgServerFeatureReaderIdentifierPool and any other pools when we remove cached connection and close it
void MgFdoConnectionManager::RemoveExpiredConnections()
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

                            STRING resourceId = iter->first;

                            // Remove any feature service cache entries for this resource
                            MgServiceManager* serviceManager = MgServiceManager::GetInstance();
                            assert(NULL != serviceManager);

                            try
                            {
                                Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(resourceId);
                                serviceManager->RemoveFeatureServiceCacheEntry(resource);
                            }
                            catch(MgInvalidRepositoryTypeException* e)
                            {
                                // If this exception is thrown then the key was not a resource identifier string 
                                // and so there will be no entries in the feature service cache to remove.
                                SAFE_RELEASE(e);
                            }

                            // Remove Feature Source cache entry
                            FeatureSourceCache::iterator iterFeatureSource = m_FeatureSourceCache.find(resourceId);
                            if(m_FeatureSourceCache.end() != iterFeatureSource)
                            {
                                MdfModel::FeatureSource* featureSource = iterFeatureSource->second;
                                if(featureSource)
                                {
                                    delete featureSource;
                                    featureSource = NULL;
                                }
                                m_FeatureSourceCache.erase(iterFeatureSource);
                            }

                            fdoConnectionCache->erase(iter++);

                            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager.RemoveExpiredConnections() - Found expired cached FDO connection.\n")));
                        }
                        else
                        {
                            // Check the next cached connection
                            iter++;
                        }
                    }
                    else
                    {
                        // NULL Pointer
                        break;
                    }
                }

                // If there are no cached entries left, reset the # of current connections
                if(fdoConnectionCache->size() == 0)
                {
                    providerInfo->ResetCurrentConnections();
                }
            }
        }

        // Next provider FDO connection cache
        iterProviderInfoCollection++;
    }
    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.RemoveExpiredConnections")
}


FdoIConnection* MgFdoConnectionManager::FindFdoConnection(MgResourceIdentifier* resourceIdentifier)
{
    CHECKNULL(resourceIdentifier, L"MgFdoConnectionManager.FindFdoConnection()");

    FdoIConnection* pFdoConnection = NULL;

    MG_FDOCONNECTION_MANAGER_TRY()

    // Parse feature source XML and get provider
    MdfModel::FeatureSource* featureSource = GetFeatureSource(resourceIdentifier);

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
                FdoConnectionCache::iterator iter = fdoConnectionCache->begin();
                while(fdoConnectionCache->end() != iter)
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
                                if (FdoConnectionState_Closed == pFdoConnectionCacheEntry->pFdoConnection->GetConnectionState())
                                {
                                    pFdoConnectionCacheEntry->pFdoConnection->Open();
                                    #ifdef _DEBUG_FDOCONNECTION_MANAGER
                                    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("SearchFdoConnectionCache - Had to reopen connection!!\n")));
                                    #endif
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

void MgFdoConnectionManager::GetSpatialContextInfoFromXml(MdfModel::FeatureSource* pFeatureSource, MgSpatialContextInfoMap* spatialContextInfoMap)
{
    CHECKNULL(pFeatureSource, L"MgFdoConnectionManager.GetSpatialContextInfoFromXml");

    MG_FDOCONNECTION_MANAGER_TRY()

    MdfModel::SupplementalSpatialContextInfoCollection* spatialContexts = pFeatureSource->GetSupplementalSpatialContextInfo();
    CHECKNULL(spatialContexts, L"MgFdoConnectionManager.GetSpatialContextInfoFromXml");

    for (int i = 0; i < spatialContexts->GetCount(); i++)
    {
        MdfModel::SupplementalSpatialContextInfo* spatialContext = spatialContexts->GetAt(i);
        CHECKNULL(spatialContext, L"MgFdoConnectionManager.GetSpatialContextInfoFromXml");

        // Name element
        STRING name = (STRING)spatialContext->GetName();
        FdoString* propertyName = name.c_str();
        CHECKNULL(propertyName, L"MgFdoConnectionManager.GetSpatialContextInfoFromXml");

        // CoordinateSystem element
        STRING coordinateSystem = (STRING)spatialContext->GetCoordinateSystem();
        FdoString* propertyCoordinateSystem = coordinateSystem.c_str();
        CHECKNULL(propertyCoordinateSystem, L"MgFdoConnectionManager.GetSpatialContextInfoFromXml");

        spatialContextInfoMap->insert( MgSpatialContextInfoPair(name, coordinateSystem) );
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.GetSpatialContextInfoFromXml");    
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
    CHECKNULL((FdoIConnection*)pFdoConnection, L"MgFdoConnectionManager.SupportsConfiguration()");

    FdoPtr<FdoIConnectionCapabilities> ficc = pFdoConnection->GetConnectionCapabilities();
    CHECKNULL((FdoIConnectionCapabilities*)ficc, L"MgFdoConnectionManager.SupportsConfiguration");

    return ficc->SupportsConfiguration();
}

void MgFdoConnectionManager::RetrieveFeatureSource(MgResourceIdentifier* resource, string& resourceContent)
{
    CHECKNULL(resource, L"MgFdoConnectionManager.RetrieveFeatureSource");

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_FDOCONNECTION_MANAGER_TRY()

    resourceContent = "";

    // Check to see if we have a valid resource
    bool bValidResource = false;
    try
    {
        resource->Validate();
        bValidResource = true;
    }
    catch(MgException* e)
    {
        SAFE_RELEASE(e);
    }

    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    // Get the service from service manager
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
    assert(resourceService != NULL);

    // Get the feature source contents
    Ptr<MgByteReader> byteReader = resourceService->GetResourceContent(resource, MgResourcePreProcessingType::Substitution);

    byteReader->ToStringUtf8(resourceContent);

    ValidateFeatureSource(resourceContent);

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.RetrieveFeatureSource")
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

        throw new MgInvalidArgumentException(L"MgFdoConnectionManager.SetConfiguration",
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

    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    // Get the service from service manager
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
    assert(resourceService != NULL);

    // Get the feature source contents
    Ptr<MgByteReader> byteReader = resourceService->GetResourceData(resourceIdentifier, configDataName , MgResourcePreProcessingType::Substitution);
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

void MgFdoConnectionManager::ValidateFeatureSource(string& featureSourceXmlContent)
{
    bool isValidFeatureSource = true;

    // TODO: Should we add XML validation here to ensure the integrity of feature source
    if (featureSourceXmlContent.empty())
    {
        isValidFeatureSource = false;
    }
    else
    {
        int index = (int)featureSourceXmlContent.find("<FeatureSource");
        if (index == -1)
        {
            isValidFeatureSource = false;
        }
    }

    // if invalid FeatureSource, throw exception saying invalid provider specified
    if (!isValidFeatureSource)
    {
        STRING message = MgUtil::GetResourceMessage(MgResources::FeatureService, L"MgInvalidFdoProvider");

        Ptr<MgStringCollection> strCol;
        if (!message.empty())
        {
            strCol = new MgStringCollection();
            strCol->Add(message);
        }

        throw new MgInvalidFeatureSourceException(L"MgFdoConnectionManager.ValidateFeatureSource",
            __LINE__, __WFILE__, (MgStringCollection*)strCol, L"", NULL);
    }
}

void MgFdoConnectionManager::Open(FdoIConnection* pFdoConnection)
{
    try
    {
        // Open the connection to the FDO provider
        if (pFdoConnection != NULL)
        {
            pFdoConnection->Open();
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

MgSpatialContextInfoMap* MgFdoConnectionManager::GetSpatialContextInfo(MgResourceIdentifier* resourceIdentifier)
{
    MgSpatialContextInfoMap* spatialContextInfoMap = NULL;

    MG_FDOCONNECTION_MANAGER_TRY()

    // Get the feature source
    MdfModel::FeatureSource* featureSource = GetFeatureSource(resourceIdentifier);

    // Get the supplementary spatial context information which defines the coordinate system
    // for spatial contexts that are missing this information
    spatialContextInfoMap = new MgSpatialContextInfoMap();
    GetSpatialContextInfoFromXml(featureSource, spatialContextInfoMap);

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.GetSpatialContextInfo")

    return spatialContextInfoMap;
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


bool MgFdoConnectionManager::RemoveCachedFdoConnection(CREFSTRING key)
{
    INT32 connections = 0;
    INT32 connectionsRemoved = 0;

    MG_FDOCONNECTION_MANAGER_TRY()

    // Protect the cache
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

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
                FdoConnectionCache::iterator iter = fdoConnectionCache->find(key);

                // We need to search the entire cache because there could be more then 1 cached 
                // connection to the same FDO provider.
                while(fdoConnectionCache->end() != iter)
                {
                    connections++;

                    // We have a key match
                    FdoConnectionCacheEntry* pFdoConnectionCacheEntry = iter->second;
                    if(pFdoConnectionCacheEntry)
                    {
                        if(pFdoConnectionCacheEntry->pFdoConnection)
                        {
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

                                // Remove any feature service cache entries for this resource
                                MgServiceManager* serviceManager = MgServiceManager::GetInstance();
                                assert(NULL != serviceManager);

                                try
                                {
                                    Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(key);
                                    serviceManager->RemoveFeatureServiceCacheEntry(resource);
                                }
                                catch(MgInvalidRepositoryTypeException* e)
                                {
                                    // If this exception is thrown then the key was not a resource identifier string 
                                    // and so there will be no entries in the feature service cache to remove.
                                    SAFE_RELEASE(e);
                                }

                                fdoConnectionCache->erase(iter++);

                                connectionsRemoved++;

                                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager.RemoveCachedFdoConnection() - Releasing cached FDO connection.\n")));
                            }
                            else
                            {
                                // The resource is still in use and so it cannot be removed
                                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager.RemoveCachedFdoConnection() - FDO connection in use!\n")));

                                // Next cached FDO connection
                                iter++;
                            }
                        }
                    }
                }
            }
        }

        // Next provider FDO connection cache
        iterProviderInfoCollection++;
    }

    // Remove Feature Source cache entry
    FeatureSourceCache::iterator iterFeatureSource = m_FeatureSourceCache.find(key);
    if(m_FeatureSourceCache.end() != iterFeatureSource)
    {
        MdfModel::FeatureSource* featureSource = iterFeatureSource->second;
        if(featureSource)
        {
            delete featureSource;
            featureSource = NULL;
        }
        m_FeatureSourceCache.erase(iterFeatureSource);
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.RemoveCachedFdoConnection")

    return (connections == connectionsRemoved);
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
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

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

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.FdoConnectionCacheFull")

    return providerInfo;
}

void MgFdoConnectionManager::ClearCache()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

#ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ClearCache() - FDO cache BEFORE\n")));
    ShowCache();
#endif

    size_t cacheCollectionSize = m_ProviderInfoCollection.size();
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ClearCache()\n")));
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
                            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("Removed (NULL connection)\n")));

                            // Remove NULL FDO connection entry
                            delete pFdoConnectionCacheEntry;
                            pFdoConnectionCacheEntry = NULL;

                            fdoConnectionCache->erase(iter++);
                        }
                    }
                    else
                    {
                        // NULL pointer
                        break;
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

        // Next provider FDO connection cache
        iterProviderInfoCollection++;
    }

    m_ProviderInfoCollection.clear();

#ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ClearCache() - FDO cache AFTER\n")));
    ShowCache();
#endif

    // Clear the feature source cache
    FeatureSourceCache::iterator iterFeatureSource = m_FeatureSourceCache.begin();
    while(m_FeatureSourceCache.end() != iterFeatureSource)
    {
        MdfModel::FeatureSource* featureSource = iterFeatureSource->second;

        if (NULL != featureSource)
        {
            delete featureSource;
            featureSource = NULL;
        }

        iterFeatureSource++;
    }

    m_FeatureSourceCache.clear();
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

MdfModel::FeatureSource* MgFdoConnectionManager::GetFeatureSource(MgResourceIdentifier* resId)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    MdfModel::FeatureSource* featureSource = NULL;

    if (resId->GetRepositoryType().empty())
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgFdoConnectionManager.GetFeatureSource", __LINE__,  __WFILE__, NULL, L"", NULL);
    }

    STRING resourceId = resId->ToString();

    FeatureSourceCache::iterator iter = m_FeatureSourceCache.find(resourceId);
    if(m_FeatureSourceCache.end() != iter)
    {
        featureSource = iter->second;
    }

    if(NULL == featureSource)
    {
        // Retrieve XML from repository
        string featureSourceXmlContent;
        RetrieveFeatureSource(resId, featureSourceXmlContent);

        MdfParser::FSDSAX2Parser parser;
        parser.ParseString(featureSourceXmlContent.c_str(),
            (unsigned int)(featureSourceXmlContent.length()*sizeof(char)));

        assert(parser.GetSucceeded());

        // detach the feature source from the parser - it's
        // now the caller's responsibility to delete it
        featureSource = parser.DetachFeatureSource();

        // Store this feature source in the feature source cache
        m_FeatureSourceCache.insert(FeatureSourceCacheEntry_Pair(resourceId, featureSource));
    }

    assert(featureSource != NULL);
    return featureSource;
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
                strThreadModel = L"";
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

void MgFdoConnectionManager::UpdateConnections()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_FDOCONNECTION_MANAGER_TRY()

#ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::UpdateConnections() - Provider Info Cache Before\n")));
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
                        INT32 refCount = pFdoConnectionCacheEntry->pFdoConnection->GetRefCount();
                        if(1 == refCount)
                        {
                            #ifdef _DEBUG_FDOCONNECTION_MANAGER
                            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::UpdateConnections() - Decrementing Connection!\n")));
                            #endif

                            // There are no more references to this provider connection
                            providerInfo->DecrementCurrentConnections();

                            // Are we supposed to release this provider from the cache?
                            if(!providerInfo->GetKeepCached())
                            {
                                #ifdef _DEBUG_FDOCONNECTION_MANAGER
                                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::UpdateConnections() - Closing Connection!\n")));
                                #endif

                                // Close the connection
                                pFdoConnectionCacheEntry->pFdoConnection->Close();

                                // Release any resource
                                FDO_SAFE_RELEASE(pFdoConnectionCacheEntry->pFdoConnection);

                                delete pFdoConnectionCacheEntry;
                                pFdoConnectionCacheEntry = NULL;

                                STRING resourceId = iter->first;

                                // Remove any feature service cache entries for this resource
                                MgServiceManager* serviceManager = MgServiceManager::GetInstance();
                                assert(NULL != serviceManager);

                                try
                                {
                                    Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(resourceId);
                                    serviceManager->RemoveFeatureServiceCacheEntry(resource);
                                }
                                catch(MgInvalidRepositoryTypeException* e)
                                {
                                    // If this exception is thrown then the key was not a resource identifier string 
                                    // and so there will be no entries in the feature service cache to remove.
                                    SAFE_RELEASE(e);
                                }

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
                }
            }
        }

        // Next provider FDO connection cache
        iterProviderInfoCollection++;
    }

#ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::UpdateConnections() - Provider Info Cache After\n")));
    ShowProviderInfoCache();
#endif

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.UpdateConnections")
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
