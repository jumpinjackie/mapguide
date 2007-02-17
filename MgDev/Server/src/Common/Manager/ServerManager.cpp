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

#include "ServerManager.h"
#include "ProductVersion.h"
#include "ServiceManager.h"
#include "ServerFeatureService.h"
#include "WorkerThread.h"
#include "WorkerThreadData.h"

const STRING MgServerManager::DocumentExtension = L".awd";
const STRING MgServerManager::DocumentPath      = L"DocumentPath";

// Process-wide MgServerManager
Ptr<MgServerManager> MgServerManager::m_serverManager = (MgServerManager*)NULL;

// Constructor
MgServerManager::MgServerManager(void) :
    m_pClientHandles(NULL),
    m_ssServerStatus(MgServerManager::ssOffline),
    m_isSiteServer(true),
    m_totalOperationTime(0),
    m_totalReceivedOperations(0),
    m_totalProcessedOperations(0),
    m_totalConnections(0),
    m_totalActiveConnections(0),
    m_pAdminMessageQueue(NULL),
    m_pClientMessageQueue(NULL),
    m_pSiteMessageQueue(NULL),
    m_pWorkerThreads(NULL)
{
    m_localServerAddress = MgConfigProperties::DefaultGeneralPropertyMachineIp;
    m_defaultLocale = MgConfigProperties::DefaultGeneralPropertyDefaultLocale;
    m_displayName = MgConfigProperties::DefaultGeneralPropertyDisplayName;

    // Admin
    m_nAdminPort    = MgConfigProperties::DefaultAdministrativeConnectionPropertyPort;
    m_adminEmail    = MgConfigProperties::DefaultAdministrativeConnectionPropertyEmail;
    m_nAdminThreads = MgConfigProperties::DefaultAdministrativeConnectionPropertyThreadPoolSize;

    // Client
    m_nClientPort    = MgConfigProperties::DefaultClientConnectionPropertyPort;
    m_nClientThreads = MgConfigProperties::DefaultClientConnectionPropertyThreadPoolSize;

    // Site
    m_siteServerAddress = MgConfigProperties::DefaultSiteConnectionPropertyIpAddress;
    m_nSitePort    = MgConfigProperties::DefaultSiteConnectionPropertyPort;
    m_nSiteThreads = MgConfigProperties::DefaultSiteConnectionPropertyThreadPoolSize;

    m_startTime = ACE_OS::gettimeofday();
}

// Destructor
MgServerManager::~MgServerManager(void)
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%P|%t) MgServerManager::~MgServerManager()\n")));

    // Remove client handlers
    if(m_pClientHandles)
    {
        ACE_Reactor_Mask mask = ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL;
        ACE_Unbounded_Set_Iterator<ACE_HANDLE> handleIter(*m_pClientHandles);
        for (handleIter = m_pClientHandles->begin(); handleIter != m_pClientHandles->end(); handleIter++)
        {
            ACE_HANDLE handle = (*handleIter);
            if ( ACE_Reactor::instance()->remove_handler(handle, mask) == 0 )
            {
                m_pClientHandles->remove(handle);
            }
        }

        delete m_pClientHandles;
        m_pClientHandles = NULL;
    }

    delete m_pWorkerThreads;
    m_pWorkerThreads = NULL;
}

void MgServerManager::Dispose(void)
{
    delete this;
}

// Get pointer to a process-wide MgServerManager.
MgServerManager* MgServerManager::GetInstance()
{
    MG_TRY()

    ACE_TRACE ("MgServerManager::GetInstance");

    if (MgServerManager::m_serverManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgServerManager::m_serverManager == NULL)
        {
            MgServerManager::m_serverManager = new MgServerManager;
        }
    }

    MG_CATCH_AND_THROW(L"MgServerManager.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgServerManager::m_serverManager;
}

// Initialization the server manager
void MgServerManager::Initialize(CREFSTRING locale)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerManager::Initialize()");

    // Set the locale
    m_defaultLocale = locale;

    m_pClientHandles = new ACE_Unbounded_Set<ACE_HANDLE>;

    // Load the configuration properties
    LoadConfigurationProperties();

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();

    // Determine if this server is a site or support server.
    pConfiguration->GetBoolValue(
        MgConfigProperties::HostPropertiesSection,
        MgConfigProperties::HostPropertySiteService,
        m_isSiteServer,
        MgConfigProperties::DefaultHostPropertySiteService);

    // Admin Connection
    pConfiguration->GetIntValue(MgConfigProperties::AdministrativeConnectionPropertiesSection, MgConfigProperties::AdministrativeConnectionPropertyPort, m_nAdminPort, MgConfigProperties::DefaultAdministrativeConnectionPropertyPort);
    pConfiguration->GetIntValue(MgConfigProperties::AdministrativeConnectionPropertiesSection, MgConfigProperties::AdministrativeConnectionPropertyThreadPoolSize, m_nAdminThreads, MgConfigProperties::DefaultAdministrativeConnectionPropertyThreadPoolSize);

    // Client Connection
    pConfiguration->GetIntValue(MgConfigProperties::ClientConnectionPropertiesSection, MgConfigProperties::ClientConnectionPropertyPort, m_nClientPort, MgConfigProperties::DefaultClientConnectionPropertyPort);
    pConfiguration->GetIntValue(MgConfigProperties::ClientConnectionPropertiesSection, MgConfigProperties::ClientConnectionPropertyThreadPoolSize, m_nClientThreads, MgConfigProperties::DefaultClientConnectionPropertyThreadPoolSize);

    // Site Connection
    STRING siteServerAddress;

    pConfiguration->GetStringValue(
        MgConfigProperties::SiteConnectionPropertiesSection,
        MgConfigProperties::SiteConnectionPropertyIpAddress,
        siteServerAddress,
        MgConfigProperties::DefaultSiteConnectionPropertyIpAddress);
    pConfiguration->GetIntValue(MgConfigProperties::SiteConnectionPropertiesSection, MgConfigProperties::SiteConnectionPropertyPort, m_nSitePort, MgConfigProperties::DefaultSiteConnectionPropertyPort);
    pConfiguration->GetIntValue(MgConfigProperties::SiteConnectionPropertiesSection, MgConfigProperties::SiteConnectionPropertyThreadPoolSize, m_nSiteThreads, MgConfigProperties::DefaultSiteConnectionPropertyThreadPoolSize);

    MgIpUtil::HostNameToAddress(siteServerAddress, m_siteServerAddress);

    // Validate IP configurations for this server.
    STRING localServerAddress, hostAddress;

    pConfiguration->GetStringValue(
        MgConfigProperties::GeneralPropertiesSection, 
        MgConfigProperties::GeneralPropertyMachineIp, 
        localServerAddress, 
        MgConfigProperties::DefaultGeneralPropertyMachineIp);

    if (m_isSiteServer && MgIpUtil::IsLocalHost(localServerAddress, false))
    {
        localServerAddress = siteServerAddress;
    }

    MgIpUtil::HostNameToAddress(localServerAddress, m_localServerAddress);

    if (MgIpUtil::HostNameToAddress(L"localhost", hostAddress, false)
        && (MgIpUtil::IsIpAddress(hostAddress, false) == MgIpUtil::IsIpAddress(m_localServerAddress, false))
        && (0 != MgIpUtil::CompareAddresses(hostAddress, m_localServerAddress)))
    {
        MgStringCollection arguments;
        arguments.Add(localServerAddress);

        throw new MgInvalidIpAddressException(
            L"MgServerManager.Initialize", __LINE__, __WFILE__, &arguments,
            L"MgInvalidIpConfigurationForLocalMachine", NULL);
    }
    
    if (m_isSiteServer)
    {
        if (0 != MgIpUtil::CompareAddresses(m_siteServerAddress, m_localServerAddress))
        {
            MgStringCollection arguments;
            arguments.Add(localServerAddress);
            arguments.Add(siteServerAddress);

            throw new MgLogicException(
                L"MgServerManager.Initialize", __LINE__, __WFILE__, NULL,
                L"MgInvalidIpConfigurationForSiteServer", &arguments);
        }
    }
    else
    {
        if (0 == MgIpUtil::CompareAddresses(m_siteServerAddress, m_localServerAddress))
        {
            MgStringCollection arguments;
            arguments.Add(localServerAddress);
            arguments.Add(siteServerAddress);

            throw new MgLogicException(
                L"MgServerManager.Initialize", __LINE__, __WFILE__, NULL,
                L"MgInvalidIpConfigurationForSupportServer", &arguments);
        }
    }

    // Create the worker thread pool
    INT32 workerThreadPoolSize = 0;
    pConfiguration->GetIntValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyWorkerThreadPoolSize, workerThreadPoolSize, MgConfigProperties::DefaultGeneralPropertyWorkerThreadPoolSize);
    m_pWorkerThreads = new MgWorkerThread(m_threadManager, workerThreadPoolSize);
    m_pWorkerThreads->Activate();

    MG_LOG_TRACE_ENTRY(L"MgServerManager::Initialize() - Site  server's IP address: " + m_siteServerAddress);
    MG_LOG_TRACE_ENTRY(L"MgServerManager::Initialize() - Local server's IP address: " + m_localServerAddress);

    m_ssServerStatus = MgServerManager::ssOnline;

    MG_CATCH_AND_THROW(L"MgServerManager.Initialize")
}

void MgServerManager::LoadConfigurationProperties()
{
    MG_LOGMANAGER_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerManager::LoadConfigurationProperties()");

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();

    // General
    pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyDisplayName, m_displayName, MgConfigProperties::DefaultGeneralPropertyDisplayName);

    // Admin Connection
    pConfiguration->GetStringValue(MgConfigProperties::AdministrativeConnectionPropertiesSection, MgConfigProperties::AdministrativeConnectionPropertyEmail, m_adminEmail, MgConfigProperties::DefaultAdministrativeConnectionPropertyEmail);

    MG_CATCH_AND_THROW(L"MgServerManager.LoadConfigurationProperties")
}

CREFSTRING MgServerManager::GetDefaultLocale()
{
    return m_defaultLocale;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the configuration properties for the specified property section.
/// </summary>
MgPropertyCollection* MgServerManager::GetConfigurationProperties(CREFSTRING propertySection)
{
    Ptr<MgPropertyCollection> pProperties;
    pProperties = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerManager::GetConfigurationProperties()");

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();
    if (NULL == pConfiguration)
    {
        throw new MgNullReferenceException(L"MgServerManager::GetConfigurationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Retrieve the properties
    pProperties = pConfiguration->GetProperties(propertySection);

    MG_CATCH_AND_THROW(L"MgServerManager.GetConfigurationProperties")

    return pProperties.Detach();
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the configuration properties for the specified property section.
/// </summary>
void MgServerManager::SetConfigurationProperties(CREFSTRING propertySection, 
    MgPropertyCollection* properties)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerManager::SetConfigurationProperties()");

    if (NULL == properties)
    {
        throw new MgNullArgumentException(
            L"MgServerManager::SetConfigurationProperties", 
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();

    if (NULL == pConfiguration)
    {
        throw new MgNullReferenceException(
            L"MgServerManager::SetConfigurationProperties", 
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Set the properties
    pConfiguration->SetProperties(propertySection, properties);

    // Enable/disable specified services for this local server if applicable.
    if (MgConfigProperties::HostPropertiesSection == propertySection)
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        assert(NULL != serviceManager);

        serviceManager->EnableServices(properties);
    }

    // Certain properties can be updated without requiring a server restart
    LoadConfigurationProperties();

    MgLogManager* logManager = MgLogManager::GetInstance();
    assert(NULL != logManager);
    logManager->LoadConfigurationProperties();

    MG_CATCH_AND_THROW(L"MgServerManager.SetConfigurationProperties")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Removes the configuration properties for the specified property section.
/// If the properties are not specified, then the entire section will be removed.
/// </summary>
void MgServerManager::RemoveConfigurationProperties(CREFSTRING propertySection, 
    MgPropertyCollection* properties)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerManager::SetConfigurationProperties()");

    if (NULL == properties)
    {
        throw new MgNullArgumentException(
            L"MgServerManager::SetConfigurationProperties", 
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();

    if (NULL == pConfiguration)
    {
        throw new MgNullReferenceException(
            L"MgServerManager::SetConfigurationProperties", 
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Set the properties
    pConfiguration->RemoveProperties(propertySection, properties);

    // Enable/disable specified services for this local server if applicable.
    if (MgConfigProperties::HostPropertiesSection == propertySection)
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        assert(NULL != serviceManager);

        serviceManager->EnableServices(properties);
    }

    // Certain properties can be updated without requiring a server restart
    LoadConfigurationProperties();

    MgLogManager* logManager = MgLogManager::GetInstance();
    assert(NULL != logManager);
    logManager->LoadConfigurationProperties();

    MG_CATCH_AND_THROW(L"MgServerManager.SetConfigurationProperties")
}


///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the information properties for the server.
/// </summary>
MgPropertyCollection* MgServerManager::GetInformationProperties()
{
    Ptr<MgPropertyCollection> pProperties;
    pProperties = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerManager::GetInformationProperties()");

    pProperties = new MgPropertyCollection();

    // Add the information properties
    Ptr<MgProperty> pProperty;

    // Add the AdminOperationsQueueCount
    int nOperationQueueCount = GetAdminOperationsQueueCount();
    pProperty = new MgInt32Property(MgServerInformationProperties::AdminOperationsQueueCount, nOperationQueueCount);
    pProperties->Add(pProperty);

    // Add the ClientOperationsQueueCount
    nOperationQueueCount = GetClientOperationsQueueCount();
    pProperty = new MgInt32Property(MgServerInformationProperties::ClientOperationsQueueCount, nOperationQueueCount);
    pProperties->Add(pProperty);

    // Add the SiteOperationsQueueCount
    nOperationQueueCount = GetSiteOperationsQueueCount();
    pProperty = new MgInt32Property(MgServerInformationProperties::SiteOperationsQueueCount, nOperationQueueCount);
    pProperties->Add(pProperty);

    // Add the CpuUtilization
#ifdef WIN32
    INT32 nCpuLoad = ComputeWIN32CpuLoad();
#else
    INT32 nCpuLoad = ComputeLinuxCpuLoad();
#endif // WIN32
    pProperty = new MgInt32Property(MgServerInformationProperties::CpuUtilization, nCpuLoad);
    pProperties->Add(pProperty);

    // Add the Status
    bool bOnline = IsOnline();
    pProperty = new MgBooleanProperty(MgServerInformationProperties::Status, bOnline);
    pProperties->Add(pProperty);

    // Add the Uptime
    INT32 nUptime = GetUptime();
    pProperty = new MgInt32Property(MgServerInformationProperties::Uptime, nUptime);
    pProperties->Add(pProperty);

    // Add the TotalPhysicalMemory
    INT64 nMemory = GetTotalPhysicalMemory();
    pProperty = new MgInt64Property(MgServerInformationProperties::TotalPhysicalMemory, nMemory);
    pProperties->Add(pProperty);

    // Add the AvailablePhysicalMemory
    nMemory = GetAvailablePhysicalMemory();
    pProperty = new MgInt64Property(MgServerInformationProperties::AvailablePhysicalMemory, nMemory);
    pProperties->Add(pProperty);

    // Add the TotalVirtualMemory
    nMemory = GetTotalVirtualMemory();
    pProperty = new MgInt64Property(MgServerInformationProperties::TotalVirtualMemory, nMemory);
    pProperties->Add(pProperty);

    // Add the AvailableVirtualMemory
    nMemory = GetAvailableVirtualMemory();
    pProperty = new MgInt64Property(MgServerInformationProperties::AvailableVirtualMemory, nMemory);
    pProperties->Add(pProperty);

    // Add the TotalOperationTime
    INT32 nTotalOperationTime = GetTotalOperationTime();
    pProperty = new MgInt32Property(MgServerInformationProperties::TotalOperationTime, nTotalOperationTime);
    pProperties->Add(pProperty);

    // Add the AverageOperationTime
    INT32 nAverageOperationTime = GetAverageOperationTime();
    pProperty = new MgInt32Property(MgServerInformationProperties::AverageOperationTime, nAverageOperationTime);
    pProperties->Add(pProperty);

    // Add the server Version
    pProperty = new MgStringProperty(MgServerInformationProperties::ServerVersion, ProductVersion);
    pProperties->Add(pProperty);

    // Add the DisplayName
    pProperty = new MgStringProperty(MgServerInformationProperties::DisplayName, m_displayName);
    pProperties->Add(pProperty);

     // Add the Operations info
    INT32 nTotalReceivedOperations = GetTotalReceivedOperations();
    pProperty = new MgInt32Property(MgServerInformationProperties::TotalReceivedOperations, nTotalReceivedOperations);
    pProperties->Add(pProperty);

    INT32 nTotalProcessedOperations = GetTotalProcessedOperations();
    pProperty = new MgInt32Property(MgServerInformationProperties::TotalProcessedOperations, nTotalProcessedOperations);
    pProperties->Add(pProperty);

    // Add the Connection info
    INT32 nTotalConnections = GetTotalConnections();
    pProperty = new MgInt32Property(MgServerInformationProperties::TotalConnections, nTotalConnections);
    pProperties->Add(pProperty);

    INT32 nTotalActiveConnections = GetTotalActiveConnections();
    pProperty = new MgInt32Property(MgServerInformationProperties::TotalActiveConnections, nTotalActiveConnections);
    pProperties->Add(pProperty);

    // Add the Operating System Version
    STRING osVersion = GetOperatingSystemVersion();
    pProperty = new MgStringProperty(MgServerInformationProperties::OperatingSystemVersion, osVersion);
    pProperties->Add(pProperty);

    MG_CATCH_AND_THROW(L"MgServerManager.GetInformationProperties")

    return pProperties.Detach();
}

MgByteReader* MgServerManager::GetDocument(CREFSTRING identifier)
{
    Ptr<MgByteReader> pByteReader;
    pByteReader = NULL;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerManager::GetDocument()");

    // Need to seperate the identifier into the path tag and filename
    STRING pathTag;
    STRING fileTag;

    bool bValid = ParseDocumentIdentifier(identifier, pathTag, fileTag);
    if(bValid)
    {
        STRING filename = GetDocumentIdentifierFilename(pathTag, fileTag);
        if(!filename.empty())
        {
            Ptr<MgByteSource> byteSource = new MgByteSource(filename);
            pByteReader = byteSource->GetReader();
        }
        else
        {
            // Invalid document identifier
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(identifier);

            throw new MgInvalidArgumentException(L"MgServerManager.GetDocument",
                __LINE__, __WFILE__, &arguments, L"MgDocumentIdentifierFilenameFailed", NULL);
        }
    }
    else
    {
        // Invalid document identifier
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(identifier);

        MgStringCollection whyArguments;
        whyArguments.Add(L"");

        throw new MgInvalidArgumentException(L"MgServerManager.GetDocument",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", &whyArguments);
    }

    MG_CATCH_AND_THROW(L"MgServerManager.GetDocument")

    return pByteReader.Detach();
}
void MgServerManager::SetDocument(CREFSTRING identifier, MgByteReader* data)
{
    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerManager::SetDocument()");

    // Need to seperate the identifier into the path tag and filename
    STRING pathTag;
    STRING fileTag;

    bool bValid = ParseDocumentIdentifier(identifier, pathTag, fileTag);
    if(bValid)
    {
        STRING filename = GetDocumentIdentifierFilename(pathTag, fileTag);
        if(!filename.empty())
        {
            // Create document directory if it does not exist
            STRING path;

            // Lookup path tag to determine actually path
            STRING configurationPropertyName = pathTag + MgServerManager::DocumentPath;

            MgConfiguration* pConfiguration = MgConfiguration::GetInstance();
            pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, configurationPropertyName, path, L"");
            if(!path.empty())
            {
                MgFileUtil::AppendSlashToEndOfPath(path);

                // Create the directory if it doesn't exist
                MgFileUtil::CreateDirectory(path, false);
            }

            MgByteSink byteSink(data);
            byteSink.ToFile(filename);
        }
        else
        {
            // Invalid document identifier
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(identifier);

            throw new MgInvalidArgumentException(L"MgServerManager.SetDocument",
                __LINE__, __WFILE__, &arguments, L"MgDocumentIdentifierFilenameFailed", NULL);
        }
    }
    else
    {
        // Invalid document identifier
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(identifier);

        MgStringCollection whyArguments;
        whyArguments.Add(L"");

        throw new MgInvalidArgumentException(L"MgServerManager.SetDocument",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", &whyArguments);
    }

    MG_CATCH_AND_THROW(L"MgServerManager.SetDocument")
}

INT32 MgServerManager::GetAdminPort()
{
    return m_nAdminPort;
}

INT32 MgServerManager::GetAdminThreads()
{
    return m_nAdminThreads;
}

CREFSTRING MgServerManager::GetAdminEmail()
{
    return m_adminEmail;
}

INT32 MgServerManager::GetClientPort()
{
    return m_nClientPort;
}

INT32 MgServerManager::GetClientThreads()
{
    return m_nClientThreads;
}

INT32 MgServerManager::GetSitePort()
{
    return m_nSitePort;
}

INT32 MgServerManager::GetSiteThreads()
{
    return m_nSiteThreads;
}

void MgServerManager::TakeOffline()
{
    if (MgServerManager::ssOffline != m_ssServerStatus)
    {
        MgLoadBalanceManager* loadBalanceManager = MgLoadBalanceManager::GetInstance();
        ACE_ASSERT(NULL != loadBalanceManager);

        if (NULL != loadBalanceManager)
        {
            loadBalanceManager->UnregisterServices();
        }

        // TODO: Currently, Feature Service will always be there, but in the future Feature Service 
        // may or not be present on this server and so the code below will need to be modified.

        // Clear the Feature Service cache
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        assert(NULL != serviceManager);

        Ptr<MgServerFeatureService> featureService = dynamic_cast<MgServerFeatureService*>(
            serviceManager->RequestService(MgServiceType::FeatureService));
        assert(featureService != NULL);

        if (featureService != NULL)
        {
            featureService->ClearFeatureServiceCache();
        }

        // Clear the FDO connection cache
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(NULL != pFdoConnectionManager)
        {
            pFdoConnectionManager->ClearCache();
        }

        m_ssServerStatus = MgServerManager::ssOffline;
    }
}

void MgServerManager::BringOnline()
{
    if (MgServerManager::ssOnline != m_ssServerStatus)
    {
        MgLoadBalanceManager* loadBalanceManager = MgLoadBalanceManager::GetInstance();
        ACE_ASSERT(NULL != loadBalanceManager);

        if (NULL != loadBalanceManager && loadBalanceManager->RegisterServices())
        {
            m_ssServerStatus = MgServerManager::ssOnline;
        }
    }
}

bool MgServerManager::IsOnline()
{
    return MgServerManager::ssOnline == m_ssServerStatus ? true : false;
}

ACE_Time_Value MgServerManager::GetStartTime()
{
    return m_startTime;
}

INT32 MgServerManager::GetTotalReceivedOperations()
{
    return m_totalReceivedOperations.value();
}

INT32 MgServerManager::GetTotalProcessedOperations()
{
    return m_totalProcessedOperations.value();
}

INT32 MgServerManager::GetTotalOperationTime()
{
    return m_totalOperationTime.value();
}

void MgServerManager::IncrementOperationTime(INT32 operationTime)
{
    m_totalOperationTime += operationTime;
}

void MgServerManager::IncrementReceivedOperations()
{
    m_totalReceivedOperations++;
}

void MgServerManager::IncrementProcessedOperations()
{
    m_totalProcessedOperations++;
}

void MgServerManager::SetAdminMessageQueue(ACE_Message_Queue<ACE_MT_SYNCH>* pMessageQueue)
{
    m_pAdminMessageQueue = pMessageQueue;
}

void MgServerManager::SetClientMessageQueue(ACE_Message_Queue<ACE_MT_SYNCH>* pMessageQueue)
{
    m_pClientMessageQueue = pMessageQueue;
}

void MgServerManager::SetSiteMessageQueue(ACE_Message_Queue<ACE_MT_SYNCH>* pMessageQueue)
{
    m_pSiteMessageQueue = pMessageQueue;
}

ACE_Message_Queue<ACE_MT_SYNCH>* MgServerManager::GetAdminMessageQueue()
{
    return m_pAdminMessageQueue;
}

ACE_Message_Queue<ACE_MT_SYNCH>* MgServerManager::GetClientMessageQueue()
{
    return m_pClientMessageQueue;
}

ACE_Message_Queue<ACE_MT_SYNCH>* MgServerManager::GetSiteMessageQueue()
{
    return m_pSiteMessageQueue;
}

INT32 MgServerManager::GetAdminOperationsQueueCount()
{
    int nOperationQueueCount = -1;

    if(m_pAdminMessageQueue)
    {
        nOperationQueueCount = m_pAdminMessageQueue->message_count();
    }

    return nOperationQueueCount;
}

INT32 MgServerManager::GetClientOperationsQueueCount()
{
    int nOperationQueueCount = -1;

    if(m_pAdminMessageQueue)
    {
        nOperationQueueCount = m_pClientMessageQueue->message_count();
    }

    return nOperationQueueCount;
}

INT32 MgServerManager::GetSiteOperationsQueueCount()
{
    int nOperationQueueCount = -1;

    if(m_pAdminMessageQueue)
    {
        nOperationQueueCount = m_pSiteMessageQueue->message_count();
    }

    return nOperationQueueCount;
}

INT32 MgServerManager::GetTotalConnections()
{
    return m_totalConnections.value();
}

INT32 MgServerManager::GetTotalActiveConnections()
{
    return m_totalActiveConnections.value();
}

void MgServerManager::IncrementActiveConnections()
{
    m_totalActiveConnections++;

    // Increment the total connections
    m_totalConnections++;
}

void MgServerManager::DecrementActiveConnections()
{
    m_totalActiveConnections--;

    // This should not go below 0
    if(m_totalActiveConnections < 0)
    {
        m_totalActiveConnections = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the total physical memory in bytes.
/// </summary>
/// <returns>
/// The total physical memory in bytes.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
INT64 MgServerManager::GetTotalPhysicalMemory()
{
    INT64 nMemory = -1;

    MG_TRY()

    #ifdef WIN32
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof (statex);

        if (GlobalMemoryStatusEx(&statex))
        {
            nMemory = (INT64)statex.ullTotalPhys;
        }

    #else
        MgLinuxMemoryStatus linuxMemoryStatus;

        if (MgUtil::GetLinuxMemoryStatus(&linuxMemoryStatus))
        {
            nMemory = linuxMemoryStatus.m_memTotal;
        }

    #endif // WIN32


    MG_CATCH_AND_THROW(L"MgServerManager.GetTotalPhysicalMemory")

    return nMemory;

}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the available physical memory in bytes.
/// </summary>
/// <returns>
/// The available physical memory in bytes.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
INT64 MgServerManager::GetAvailablePhysicalMemory()
{
    INT64 nMemory = -1;

    MG_TRY()

    #ifdef WIN32
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof (statex);

        if (GlobalMemoryStatusEx(&statex))
        {
            nMemory = (INT64)statex.ullAvailPhys;
        }

    #else
        MgLinuxMemoryStatus linuxMemoryStatus;

        if (MgUtil::GetLinuxMemoryStatus(&linuxMemoryStatus))
        {
            nMemory = linuxMemoryStatus.m_memFree;
        }

    #endif // WIN32


    MG_CATCH_AND_THROW(L"MgServerManager.GetAvailablePhysicalMemory")

    return nMemory;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the total virtual memory in bytes.
/// </summary>
/// <returns>
/// The total virtual memory in bytes.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
INT64 MgServerManager::GetTotalVirtualMemory()
{
    INT64 nMemory = -1;

    MG_TRY()

    #ifdef WIN32
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof (statex);

        if (GlobalMemoryStatusEx(&statex))
        {
            nMemory = (INT64)statex.ullTotalVirtual;
        }

    #else
        MgLinuxMemoryStatus linuxMemoryStatus;

        if (MgUtil::GetLinuxMemoryStatus(&linuxMemoryStatus))
        {
            nMemory = linuxMemoryStatus.m_swapTotal;
        }


    #endif // WIN32


    MG_CATCH_AND_THROW(L"MgServerManager.GetTotalVirtualMemory");

    return nMemory;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the available virtual memory in bytes.
/// </summary>
/// <returns>
/// The available virtual memory in bytes.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
INT64 MgServerManager::GetAvailableVirtualMemory()
{
    INT64 nMemory = -1;

    MG_TRY()

    #ifdef WIN32
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof (statex);

        if (GlobalMemoryStatusEx(&statex))
        {
            nMemory = (INT64)statex.ullAvailVirtual;
        }

    #else
        MgLinuxMemoryStatus linuxMemoryStatus;

        if (MgUtil::GetLinuxMemoryStatus(&linuxMemoryStatus))
        {
            nMemory = linuxMemoryStatus.m_swapFree;
        }

    #endif


    MG_CATCH_AND_THROW(L"MgServerManager.GetAvailableVirtualMemory");

    return nMemory;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the amount of time that the server has been up and running.
/// </summary>
/// <returns>
/// The server up time in seconds.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
INT32 MgServerManager::GetUptime()
{
    ACE_Time_Value upTime(0);

    MG_TRY()

    upTime = ACE_OS::gettimeofday() - GetStartTime();

    MG_CATCH_AND_THROW(L"MgServerManager.GetUptime");

    return (INT32)upTime.sec();
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the average time to process an operation.
/// </summary>
/// <returns>
/// The average time to process an operation in milliseconds.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
INT32 MgServerManager::GetAverageOperationTime()
{
    double avgTime = 0.0;

    MG_TRY()

    if (GetTotalProcessedOperations() > 0)
    {
        avgTime = ((double)(GetTotalOperationTime()) * 1000.0) / (double)(GetTotalProcessedOperations());
    }


    MG_CATCH_AND_THROW(L"MgServerManager.GetAverageOperationTime")

    return (INT32)avgTime;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Helper method to compute the current CPU load on WIN32.
/// </summary>
/// <returns>
/// The total the current CPU load in percent.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
INT32 MgServerManager::ComputeWIN32CpuLoad()
{
    INT32 nCpuLoad = -1;

    #ifdef WIN32

        HQUERY          hQuery;
        HGLOBAL         hAlloc;
        HCOUNTER        *pCounterHandle;
        PDH_STATUS      pdhStatus;
        PDH_FMT_COUNTERVALUE   fmtValue;
        DWORD           ctrType;
        wchar_t         szPathBuffer[MAX_PATH];

        // Open the query object.
        pdhStatus = PdhOpenQuery (0, 0, &hQuery);

        // Allocate the counter handle array.
        hAlloc = ::GlobalAlloc(GPTR, sizeof(HCOUNTER));

        wcscpy(szPathBuffer, L"\\Process(Idle)\\% Processor Time");    // NOXLATE

        pCounterHandle = (HCOUNTER*)hAlloc;
        pdhStatus = PdhAddCounter (hQuery,
                                    szPathBuffer,
                                    0,
                                    pCounterHandle);

        // Prime the counter for computing the CPU load over the sample interval.
        pdhStatus = PdhCollectQueryData (hQuery);

        // Wait one interval.
        ACE_OS::sleep(1);

        // Get the current data values.
        pdhStatus = PdhCollectQueryData (hQuery);

        // Get the current value of this counter.
        pdhStatus = PdhGetFormattedCounterValue (*pCounterHandle,
                                                PDH_FMT_LONG,
                                                &ctrType,
                                                &fmtValue);

        if (pdhStatus == ERROR_SUCCESS)
        {
            nCpuLoad = 100 - (INT32)fmtValue.longValue;
        }

        // Close the query.
        pdhStatus = PdhCloseQuery (hQuery);

        // Free memory
        ::GlobalFree(hAlloc);

    #else

        throw new MgPlatformNotSupportedException(L"MgServerManager.ComputeWIN32CpuLoad", __LINE__, __WFILE__, NULL, L"", NULL);

    #endif  // WIN32

    return nCpuLoad;
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Helper method to compute the current CPU load on Linux.
/// </summary>
/// <returns>
/// The total the current CPU load in percent.
/// </returns>
INT32 MgServerManager::ComputeLinuxCpuLoad()
{
    INT32 nCpuLoad = -1;

    #ifdef WIN32

        throw new MgPlatformNotSupportedException(L"MgServerManager.ComputeLinuxCpuLoad", __LINE__, __WFILE__, NULL, L"", NULL);

    #else

        FILE* stat;
        char line[80];
        int user;
        int nice;
        int sys;
        int idle;
        int ouser;
        int onice;
        int osys;
        int oidle;
        double load;

        // Prime initial values for computing the CPU load over the sample interval
        stat = fopen( ( MgUtil::WideCharToMultiByte(MgUtil::LinuxFilesystemStat) ).c_str(), "r" );
        if (NULL == stat)
        {
            MgStringCollection arguments;
            arguments.Add(MgUtil::LinuxFilesystemStat);
            throw new MgFileNotFoundException(L"MgServerManager.ComputeLinuxCpuLoad", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        fgets(line, 80, stat);
        sscanf(line, "%*3s %i %i %i %i", &ouser, &onice, &osys, &oidle);
        fclose(stat);

        // Wait one interval.
        ACE_OS::sleep(1);

        // Get the current data values
        stat = fopen( ( MgUtil::WideCharToMultiByte(MgUtil::LinuxFilesystemStat) ).c_str(), "r");
        if(NULL == stat)
        {
            MgStringCollection arguments;
            arguments.Add(MgUtil::LinuxFilesystemStat);
            throw new MgFileNotFoundException(L"MgServerManager.ComputeLinuxCpuLoad", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        fgets(line, 80, stat);
        sscanf(line, "%*3s %i %i %i %i", &user, &nice, &sys, &idle);
        fclose(stat);

        load = (double)(idle-oidle) / (double)( (user-ouser) + (nice-onice) + (sys-osys) + (idle-oidle) ) * 100;

        nCpuLoad = 100 - (INT32)load;

    #endif  // WIN32

    return nCpuLoad;
}

#ifdef WIN32
// Note: This code is a modified version taken from the Microsoft SDK
STRING MgServerManager::GetOperatingSystemVersion()
{
    STRING osVersion;
    OSVERSIONINFOEX osvi;
    bool bOsVersionInfoEx = false;
    bool bHaveVersion = false;

    // Set to default
    osVersion = L"Microsoft Windows";

    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if((bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
    {
        bHaveVersion = true;
    }
    else
    {
        // Try using standard version infor structure
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (GetVersionEx((OSVERSIONINFO *)&osvi))
        {
            bHaveVersion = true;
        }
    }

    if(bHaveVersion)
    {
        switch (osvi.dwPlatformId)
        {
            // Test for the Windows NT product family.
            case VER_PLATFORM_WIN32_NT:

            // Test for the specific product family.
            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
                osVersion = L"Microsoft Windows Server 2003 family, ";

            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
                osVersion = L"Microsoft Windows XP ";

            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
                osVersion = L"Microsoft Windows 2000 ";

            if ( osvi.dwMajorVersion <= 4 )
                osVersion = L"Microsoft Windows NT ";

            // Test for specific product on Windows NT 4.0 SP6 and later.
            if( bOsVersionInfoEx )
            {
                // Test for the workstation type.
                if ( osvi.wProductType == VER_NT_WORKSTATION )
                {
                    if( osvi.dwMajorVersion == 4 )
                        osVersion += L"Workstation 4.0 ";
                    else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
                        osVersion += L"Home Edition ";
                    else
                        osVersion += L"Professional ";
                }

                // Test for the server type.
                else if ( osvi.wProductType == VER_NT_SERVER )
                {
                    if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
                    {
                        if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                            osVersion += L"Datacenter Edition ";
                        else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                            osVersion += L"Enterprise Edition ";
                        else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
                            osVersion += L"Web Edition ";
                        else
                            osVersion += L"Standard Edition ";
                    }
                    else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
                    {
                        if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                            osVersion += L"Datacenter Server ";
                        else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                            osVersion += L"Advanced Server ";
                        else
                            osVersion += L"Server ";
                    }
                    else  // Windows NT 4.0
                    {
                        if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                            osVersion += L"Server 4.0, Enterprise Edition ";
                        else
                            osVersion += L"Server 4.0 ";
                    }
                }
            }
            else  // Test for specific product on Windows NT 4.0 SP5 and earlier
            {
                HKEY hKey;
                wchar_t wszProductType[255];
                DWORD dwBufLen = 255;
                LONG lRet;

                lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\ProductOptions", 0, KEY_QUERY_VALUE, &hKey );
                if( lRet == ERROR_SUCCESS )
                {
                    lRet = RegQueryValueEx( hKey, L"ProductType", NULL, NULL, (LPBYTE) wszProductType, &dwBufLen);
                    if( (lRet == ERROR_SUCCESS) && (dwBufLen < 255) )
                    {
                        if ( ACE_OS::strcmp(L"WINNT", wszProductType) == 0 )
                            osVersion += L"Workstation ";
                        if ( ACE_OS::strcmp(L"LANMANNT", wszProductType) == 0 )
                            osVersion += L"Server ";
                        if ( ACE_OS::strcmp(L"SERVERNT", wszProductType) == 0 )
                            osVersion += L"Advanced Server ";
                    }

                    RegCloseKey( hKey );
                }
            }

            // Display service pack (if any) and build number.
            wchar_t wszTemp[255];

            if( osvi.dwMajorVersion == 4 && ACE_OS::strcmp( osvi.szCSDVersion, L"Service Pack 6" ) == 0 )
            {
                HKEY hKey;
                LONG lRet;

                // Test for SP6 versus SP6a.
                lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009", 0, KEY_QUERY_VALUE, &hKey );
                if( lRet == ERROR_SUCCESS )
                {
                    swprintf(wszTemp, L"Service Pack 6a (Build %d)", osvi.dwBuildNumber & 0xFFFF);
                    osVersion += wszTemp;
                }
                else // Windows NT 4.0 prior to SP6a
                {
                    swprintf(wszTemp, L"%s (Build %d)", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
                    osVersion += wszTemp;
                }

                RegCloseKey( hKey );
            }
            else // Windows NT 3.51 and earlier or Windows 2000 and later
            {
                swprintf(wszTemp, L"%s (Build %d)", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
                osVersion += wszTemp;
            }

            break;

            // Test for the Windows 95 product family.
            case VER_PLATFORM_WIN32_WINDOWS:

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
            {
                osVersion = L"Microsoft Windows 95 ";
                if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
                    osVersion += L"OSR2 ";
            }

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
            {
                osVersion = L"Microsoft Windows 98 ";
                if ( osvi.szCSDVersion[1] == 'A' )
                    osVersion += L"SE ";
            }

            if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
            {
                osVersion = L"Microsoft Windows Millennium Edition";
            }
            break;

            case VER_PLATFORM_WIN32s:

            osVersion = L"Microsoft Win32s";
            break;
        }
    }

    return osVersion;
}
#else
#include <sys/utsname.h>
// Linux Operating System Version
STRING MgServerManager::GetOperatingSystemVersion()
{
    STRING osVersion;

    //SetDefault
    osVersion=L"Linux";
    char temp[100];
    struct utsname osInfo;

    if (-1 !=uname(&osInfo))
    {
        snprintf(temp, 99, "%s, Kernel Version: %s", osInfo.sysname, osInfo.release);
        osVersion = MgUtil::MultiByteToWideChar(std::string(temp));
    }

    return osVersion;
}
#endif

void MgServerManager::AddClientHandle(ACE_HANDLE handle)
{
    if (m_pClientHandles)
    {
        // Mutex is shared with MgIdleTimeoutHandler::handle_timeout
        ACE_MT (ACE_GUARD_ACTION(ACE_Lock, ace_mon, ACE_Reactor::instance()->lock(), m_pClientHandles->insert(handle);, ;));
    }
}

void MgServerManager::RemoveClientHandle(ACE_HANDLE handle)
{
    if (m_pClientHandles)
    {
        // Mutex is shared with MgIdleTimeoutHandler::handle_timeout
        ACE_MT (ACE_GUARD_ACTION(ACE_Lock, ace_mon, ACE_Reactor::instance()->lock(), m_pClientHandles->remove(handle);, ;));
    }
}

bool MgServerManager::ParseDocumentIdentifier(CREFSTRING identifier, STRING& pathTag, STRING& fileTag)
{
    bool bResult = false;

    // Initialize
    pathTag = L"";
    fileTag = L"";

    size_t pos = identifier.find(L":");
    if(string::npos != pos)
    {
        // Is this a valid path tag?
        if(pos > 0)
        {
            // Valid path tag
            pathTag.assign(identifier, 0, pos);

            // Is this a valid file tag?
            // Valid characters are: A-Za-Z0-9_.-+[]
            fileTag.assign(identifier, pos+1, identifier.length());
            if((string::npos == fileTag.find_first_not_of(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_.-+[]")) &&
               (string::npos == fileTag.find(L"..")))
            {
                // Valid file tag
                // Therefore, identifier is valid
                bResult = true;
            }
        }
    }

    return bResult;
}

STRING MgServerManager::GetDocumentIdentifierFilename(CREFSTRING pathTag, CREFSTRING fileTag)
{
    STRING filename;
    filename = L"";

    // We have a valid identifier
    STRING path;

    // Lookup path tag to determine actually path
    STRING configurationPropertyName = pathTag + MgServerManager::DocumentPath;

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();
    pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, configurationPropertyName, path, L"");
    if(!path.empty())
    {
        MgFileUtil::AppendSlashToEndOfPath(path);

        // Append the path to the filename
        filename = path + fileTag;

        // Add extension
        filename += MgServerManager::DocumentExtension;
    }

    return filename;
}

void MgServerManager::StartWorkerThread(void (*function)())
{
    // We want the worker thread pool to do some work for us
    MgWorkerThreadData* wtd;
    ACE_NEW_NORETURN( wtd, MgWorkerThreadData( function ) );

    ACE_Message_Block* mb;
    ACE_NEW_NORETURN( mb, ACE_Message_Block( wtd ) );
    if(mb)
    {
        mb->msg_type(ACE_Message_Block::MB_DATA);
        int nResult = m_pWorkerThreads->putq(mb);
        if(nResult == -1)
        {
            // Failed to queue the message
            STRING messageId;
            MgStringCollection arguments;

            arguments.Add(L"Failed to queue ACE_Message_Block.");
            messageId = L"MgFormatInnerExceptionMessage";

            MgException* mgException = new MgRuntimeException(L"MgServerManager.StartWorkerThread", __LINE__, __WFILE__, NULL, messageId, &arguments);
            throw mgException;
        }
    }
}

void MgServerManager::StopWorkerThreads()
{
    // Tell the worker threads to stop
    ACE_Message_Block* mb = new ACE_Message_Block(4);
    if(mb)
    {
        mb->msg_type(ACE_Message_Block::MB_STOP);
        if (m_pWorkerThreads)
        {
            m_pWorkerThreads->putq(mb);
        }
    }

    // Wait for threads to process STOP
    if (m_pWorkerThreads)
    {
        m_pWorkerThreads->wait();
    }

    m_threadManager.wait(0,1);
    m_threadManager.close();
}
