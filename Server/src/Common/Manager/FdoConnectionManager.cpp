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

    // Clear the provider information collection
    ProviderInfoCollection::iterator iter = m_ProviderInfoCollection.begin();
    while(m_ProviderInfoCollection.end() != iter)
    {
        ProviderInfo* providerInfo = iter->second;
        if(providerInfo)
        {
            delete providerInfo;
            providerInfo = NULL;
        }

        m_ProviderInfoCollection.erase(iter++);
    }

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

void MgFdoConnectionManager::Initialize(bool bFdoConnectionPoolEnabled, INT32 nFdoConnectionPoolSize, INT32 nFdoConnectionTimeout, STRING excludedProviders, STRING fdoConnectionPoolSizeCustom)
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
    if (fdoConnectionPoolSizeCustomCol)
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

            ProviderInfo* providerInfo = new ProviderInfo();
            if(providerInfo)
            {
                providerInfo->cacheSize = nCustomPoolSize;
                providerInfo->threadModel = (FdoThreadCapability)-1; // Not set yet

                m_ProviderInfoCollection.insert(ProviderInfoCacheEntry_Pair(provider, providerInfo));
            }
        }
    }

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
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    FdoPtr<FdoIConnection> pFdoConnection;

    MG_FDOCONNECTION_MANAGER_TRY()

    ACE_TRACE ("MgFdoConnectionManager::Open");

    if(m_bFdoConnectionPoolEnabled)
    {
        // Search the cache for an FDO connection matching this resourceIdentifier
        // The content and long transaction name must also match, as the information may change
        pFdoConnection = FindFdoConnection(resourceIdentifier);
    }

    if(NULL == pFdoConnection)
    {
        // Parse XML and get properties

        MdfModel::FeatureSource* featureSource = GetFeatureSource(resourceIdentifier);

        STRING provider = (STRING)featureSource->GetProvider();
        STRING configDocumentName = (STRING)featureSource->GetConfigurationDocument();
        STRING longTransactionName = (STRING)featureSource->GetLongTransaction();

        provider = UpdateProviderName(provider);

        // Update the long transaction name to any active one for the current request
        MgLongTransactionManager::GetLongTransactionName(resourceIdentifier, longTransactionName);

        // Create a new connection and add it to the cache
        pFdoConnection = m_connManager->CreateConnection(provider.c_str());

        // Check to see if we already have the capabilities for this provider
        ProviderInfo* providerInfo = GetProviderInformation(provider);
        if(providerInfo)
        {
            // Check if we have thread model
            if((FdoThreadCapability)-1 == providerInfo->threadModel)
            {
                // Get the connection capabilities
                FdoPtr<FdoIConnectionCapabilities> ficc = pFdoConnection->GetConnectionCapabilities();

                // Get the thread model
                FdoThreadCapability ftc = ficc->GetThreadCapability();

                // Update the provider information collection
                providerInfo->threadModel = ftc;
            }

            // The server only support FdoThreadCapability_PerConnectionThreaded or better connections
           if(FdoThreadCapability_SingleThreaded == providerInfo->threadModel)
            {
                throw new MgUnsupportedProviderThreadModelException(L"MgFdoConnectionManager.Open",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }

        // Retrieve the properties and open the connection
        SetConnectionProperties(pFdoConnection, featureSource);

        SetConfiguration(provider, pFdoConnection, resourceIdentifier, configDocumentName);

        Open(pFdoConnection);

        // The following are only applicable when we are creating from a resource identifier
        ActivateLongTransaction(pFdoConnection, longTransactionName);

        if(m_bFdoConnectionPoolEnabled)
        {
            // Check to see if this provider has been excluded from caching
            if(!IsExcludedProvider(provider))
            {
                if(!FdoConnectionCacheFull(provider))
                {
                    // Add this entry to the cache
                    CacheFdoConnection(pFdoConnection, 
                                       provider, 
                                       resourceIdentifier->ToString(),
                                       longTransactionName);
                }
            }
        }
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.Open")

    return pFdoConnection.Detach();
}


FdoIConnection* MgFdoConnectionManager::Open(CREFSTRING provider, CREFSTRING connectionString)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    FdoPtr<FdoIConnection> pFdoConnection;

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

    if(m_bFdoConnectionPoolEnabled)
    {
        // Search the cache for an FDO connection matching this provider/connection string
        pFdoConnection = FindFdoConnection(providerNoVersion, connectionString);
    }

    if(NULL == pFdoConnection)
    {
        // Create a new connection and add it to the cache
        pFdoConnection = m_connManager->CreateConnection(providerNoVersion.c_str());

        // Check to see if we already have the capabilities for this provider
        ProviderInfo* providerInfo = GetProviderInformation(providerNoVersion);
        if(providerInfo)
        {
            // Check if we have thread model
            if((FdoThreadCapability)-1 == providerInfo->threadModel)
            {
                // Get the connection capabilities
                FdoPtr<FdoIConnectionCapabilities> ficc = pFdoConnection->GetConnectionCapabilities();

                // Get the thread model
                FdoThreadCapability ftc = ficc->GetThreadCapability();

                // Update the provider information collection
                providerInfo->threadModel = ftc;
            }

            // The server only support FdoThreadCapability_PerConnectionThreaded or better connections
           if(FdoThreadCapability_SingleThreaded == providerInfo->threadModel)
            {
                throw new MgUnsupportedProviderThreadModelException(L"MgFdoConnectionManager.Open",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }

        // No connection string, no pooling and connection will remain in closed state
        if (!connectionString.empty())
        {
            // Set the connection properties
            pFdoConnection->SetConnectionString(connectionString.c_str());

            // Open the connection to the FDO provider
            Open(pFdoConnection);

            if(m_bFdoConnectionPoolEnabled)
            {
                // Check to see if this provider has been excluded from caching
                if(!IsExcludedProvider(providerNoVersion))
                {
                    if(!FdoConnectionCacheFull(providerNoVersion))
                    {
                        // Add this entry to the cache
                        STRING ltName = L"";
                        CacheFdoConnection(pFdoConnection, providerNoVersion, connectionString, ltName);
                    }
                }
            }
        }
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.Open")

    return pFdoConnection.Detach();
}


void MgFdoConnectionManager::Close(FdoIConnection* pFdoConnection)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    CHECKNULL((FdoIConnection*)pFdoConnection, L"MgFdoConnectionManager.Close()");

    MG_FDOCONNECTION_MANAGER_TRY()

    // Release reference
    FDO_SAFE_RELEASE(pFdoConnection);

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.Close")
}


// TODO: Need to tie in MgServerFeatureReaderIdentifierPool and any other pools when we remove cached connection and close it
void MgFdoConnectionManager::RemoveExpiredConnections()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_FDOCONNECTION_MANAGER_TRY()

    ACE_Time_Value now = ACE_OS::gettimeofday();

    // Loop all of the FDO connection caches
    FdoConnectionCacheCollection::iterator iterFdoConnectionCacheCollection = m_FdoConnectionCacheCollection.begin();
    while(m_FdoConnectionCacheCollection.end() != iterFdoConnectionCacheCollection)
    {
        FdoConnectionCache* fdoConnectionCache = iterFdoConnectionCacheCollection->second;
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

                        STRING cacheKey = iter->first;

                        // Remove any feature service cache entries for this resource
                        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
                        assert(NULL != serviceManager);

                        try
                        {
                            Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(cacheKey);
                            serviceManager->RemoveFeatureServiceCacheEntry(resource);
                        }
                        catch(MgInvalidRepositoryTypeException* e)
                        {
                            // If this exception is thrown then the key was not a resource identifier string 
                            // and so there will be no entries in the feature service cache to remove.
                            SAFE_RELEASE(e);
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
        }

        // Next FDO connection cache
        iterFdoConnectionCacheCollection++;
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

    // Loop all of the FDO connection caches
    FdoConnectionCacheCollection::iterator iterFdoConnectionCacheCollection = m_FdoConnectionCacheCollection.find(provider);
    if(m_FdoConnectionCacheCollection.end() != iterFdoConnectionCacheCollection)
    {
        FdoConnectionCache* fdoConnectionCache = iterFdoConnectionCacheCollection->second;
        if(fdoConnectionCache)
        {
            FdoConnectionCache::iterator iter = fdoConnectionCache->find(key);
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

    // Loop all of the FDO connection caches
    FdoConnectionCacheCollection::iterator iterFdoConnectionCacheCollection = m_FdoConnectionCacheCollection.begin();
    while(m_FdoConnectionCacheCollection.end() != iterFdoConnectionCacheCollection)
    {
        FdoConnectionCache* fdoConnectionCache = iterFdoConnectionCacheCollection->second;
        if(fdoConnectionCache)
        {
            FdoConnectionCache::iterator iter = fdoConnectionCache->find(key);

            // We need to search the entire cache because FDO only supports a thread per connection. 
            // Therefore, there could be more then 1 cached connection to the same FDO provider.
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
                    else
                    {
                        // NULL pointer
                        break;
                    }
                }
                else
                {
                    // NULL pointer
                    break;
                }
            }
        }

        // Next FDO connection cache
        iterFdoConnectionCacheCollection++;
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.RemoveCachedFdoConnection")

    return (connections == connectionsRemoved);
}

void MgFdoConnectionManager::CacheFdoConnection(FdoIConnection* pFdoConnection, CREFSTRING provider, CREFSTRING key, CREFSTRING ltName)
{
    MG_FDOCONNECTION_MANAGER_TRY()

    // Protect the cache
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, ));

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

        bool bConnectionCached = false;

        // Get the appropriate provider cache
        // Loop all of the FDO connection caches
        FdoConnectionCacheCollection::iterator iterFdoConnectionCacheCollection = m_FdoConnectionCacheCollection.find(provider);
        if(m_FdoConnectionCacheCollection.end() != iterFdoConnectionCacheCollection)
        {
            FdoConnectionCache* fdoConnectionCache = iterFdoConnectionCacheCollection->second;
            if(fdoConnectionCache)
            {
                fdoConnectionCache->insert(FdoConnectionCacheEntry_Pair(key, pFdoConnectionCacheEntry));
                bConnectionCached = true;
            }
        }

        // Check to see if the entry was cached. An entry will not be cached above if
        // the cache doesn't exist for the specified provider
        if(!bConnectionCached)
        {
            // Create the FDO connection cache for the specified provider
            FdoConnectionCache* fdoConnectionCache = new FdoConnectionCache();
            if(fdoConnectionCache)
            {
                fdoConnectionCache->insert(FdoConnectionCacheEntry_Pair(key, pFdoConnectionCacheEntry));
                m_FdoConnectionCacheCollection.insert(FdoConnectionCache_Pair(provider, fdoConnectionCache));
            }
        }

        // Increase the reference count before returning it because this entry has been cached
        FDO_SAFE_ADDREF(pFdoConnection);
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.CacheFdoConnection")
}


bool MgFdoConnectionManager::FdoConnectionCacheFull(CREFSTRING provider)
{
    bool bCacheFull = false;

    MG_FDOCONNECTION_MANAGER_TRY()

    // Protect the cache
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, true));

    FdoConnectionCache* fdoConnectionCache = NULL;
    INT32 fdoConnectionCacheSize = m_nFdoConnectionPoolSize; // Set to default

    // Loop all of the FDO connection caches
    FdoConnectionCacheCollection::iterator iterFdoConnectionCacheCollection = m_FdoConnectionCacheCollection.find(provider);
    if(m_FdoConnectionCacheCollection.end() != iterFdoConnectionCacheCollection)
    {
        // Found the cache we are interested in
        fdoConnectionCache = iterFdoConnectionCacheCollection->second;
    }

    // Loop all of the FDO connection caches size
    ProviderInfoCollection::iterator iterProviderInfo = m_ProviderInfoCollection.find(provider);
    if(m_ProviderInfoCollection.end() != iterProviderInfo)
    {
        ProviderInfo* providerInfo = iterProviderInfo->second;
        if(providerInfo)
        {
            fdoConnectionCacheSize = providerInfo->cacheSize;
        }
    }

    if(fdoConnectionCache)
    {
        if((INT32)fdoConnectionCache->size() >= fdoConnectionCacheSize)
        {
            // We are full, but are all entries in use?
            bCacheFull = true;
        }

        // See if we can make room in the FDO connection cache by removing an unused connection
        if(bCacheFull)
        {
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
                        // NULL pointer
                        break;
                    }
                }
                else
                {
                    // NULL pointer
                    break;
                }
            }
        }
    }

    MG_FDOCONNECTION_MANAGER_CATCH_AND_THROW(L"MgFdoConnectionManager.FdoConnectionCacheFull")

    return bCacheFull;
}


void MgFdoConnectionManager::ClearCache()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

#ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ClearCache() - FDO cache BEFORE\n")));
    ShowCache();
#endif

    size_t cacheCollectionSize = m_FdoConnectionCacheCollection.size();
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ClearCache()\n")));
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("FDO connection caches: %d.\n"), cacheCollectionSize));

    // Loop all of the FDO connection caches
    FdoConnectionCacheCollection::iterator iterFdoConnectionCacheCollection = m_FdoConnectionCacheCollection.begin();
    while(m_FdoConnectionCacheCollection.end() != iterFdoConnectionCacheCollection)
    {
        // Cleanup the FDO connection cache
        STRING provider = iterFdoConnectionCacheCollection->first;
        FdoConnectionCache* fdoConnectionCache = iterFdoConnectionCacheCollection->second;
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

        // Next FDO connection cache
        iterFdoConnectionCacheCollection++;
    }

#ifdef _DEBUG_FDOCONNECTION_MANAGER
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ClearCache() - FDO cache AFTER\n")));
    ShowCache();
#endif

    // Clear the feature source cache
    FeatureSourceCache::iterator iterFeatureSource = m_FeatureSourceCache.begin();
    while(m_FeatureSourceCache.end() != iterFeatureSource)
    {
        MdfModel::FeatureSource* featureSource = iterFeatureSource->second;
        if(featureSource)
        {
            delete featureSource;
        }

        m_FeatureSourceCache.erase(iterFeatureSource++);
    }
}


#ifdef _DEBUG
void MgFdoConnectionManager::ShowCache(void)
{
    MG_FDOCONNECTION_MANAGER_TRY()

    size_t cacheCollectionSize = m_FdoConnectionCacheCollection.size();
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("MgFdoConnectionManager::ShowCache()\n")));
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("FDO connection caches: %d.\n"), cacheCollectionSize));

    // Show the contents of the FDO connection cache collection
    INT32 nIndex = 1;
    for (FdoConnectionCacheCollection::iterator iterCol = m_FdoConnectionCacheCollection.begin();iterCol != m_FdoConnectionCacheCollection.end(); iterCol++)
    {
        STRING provider = iterCol->first;
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("%2d) %W\n"), nIndex++, provider.c_str()));

        FdoConnectionCache* fdoConnectionCache = iterCol->second;
        if(fdoConnectionCache)
        {
            INT32 entry = 1;
            size_t cacheSize = fdoConnectionCache->size();
            if(cacheSize > 0)
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

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n")));

    MG_FDOCONNECTION_MANAGER_CATCH(L"MgFdoConnectionManager.ShowCache")
}
#endif


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
    ProviderInfo* providerInfo = NULL;

    ProviderInfoCollection::iterator iter = m_ProviderInfoCollection.find(provider);
    if(iter != m_ProviderInfoCollection.end())
    {
        providerInfo = iter->second;
    }

    if(NULL == providerInfo)
    {
        // Create new entry
        providerInfo = new ProviderInfo();
        if(providerInfo)
        {
            providerInfo->cacheSize = m_nFdoConnectionPoolSize;
            providerInfo->threadModel = (FdoThreadCapability)-1; // Not set yet

            m_ProviderInfoCollection.insert(ProviderInfoCacheEntry_Pair(provider, providerInfo));
        }
    }

    return providerInfo;
}
