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

#include "LoadBalanceManager.h"
#include "ServiceManager.h"
#include "SecurityManager.h"
#include "LogManager.h"
#include "LogDetail.h"

// Process-wide MgLoadBalanceManager
Ptr<MgLoadBalanceManager> MgLoadBalanceManager::sm_loadBalanceManager = (MgLoadBalanceManager*)NULL;
ACE_Recursive_Thread_Mutex MgLoadBalanceManager::sm_mutex;

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgLoadBalanceManager::MgLoadBalanceManager()
{
    m_configuration = MgConfiguration::GetInstance();
    assert(NULL != m_configuration);
    m_serverManager = MgServerManager::GetInstance();
    assert(NULL != m_serverManager);

    m_localServerInfo = new MgServerInformation();
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgLoadBalanceManager::~MgLoadBalanceManager()
{
    MG_TRY()

    m_localServerInfo = NULL;

    for (MgServerMap::iterator i = m_supportServerMap.begin();
        i != m_supportServerMap.end(); ++i)
    {
        SAFE_RELEASE((*i).second);
    }

    for (MgServerMap::iterator i = m_externalServerMap.begin();
        i != m_externalServerMap.end(); ++i)
    {
        SAFE_RELEASE((*i).second);
    }

    MG_CATCH(L"MgLoadBalanceManager.~MgLoadBalanceManager")
}

///----------------------------------------------------------------------------
/// <summary>
/// Disposes the object.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::Dispose()
{
    delete this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the pointer to a process-wide MgLoadBalanceManager.
/// </summary>
///----------------------------------------------------------------------------

MgLoadBalanceManager* MgLoadBalanceManager::GetInstance()
{
    MG_TRY()

    ACE_TRACE("MgLoadBalanceManager::GetInstance");

    if (MgLoadBalanceManager::sm_loadBalanceManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance(), NULL));

        if (MgLoadBalanceManager::sm_loadBalanceManager == NULL)
        {
            MgLoadBalanceManager::sm_loadBalanceManager = new MgLoadBalanceManager();
        }
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgLoadBalanceManager::sm_loadBalanceManager;
}

///----------------------------------------------------------------------------
/// <summary>
/// Intializes the load balance manager.
/// This method must be called once during the server startup time.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::Initialize()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_TRY()

    ACE_DEBUG((LM_DEBUG, ACE_TEXT( "(%P|%t) MgLoadBalanceManager::Initialize()\n")));
    MG_LOG_TRACE_ENTRY(L"MgLoadBalanceManager::Initialize()");

    // Initialize local server information.

    Ptr<MgPropertyCollection> hostProps = m_configuration->GetProperties(
        MgConfigProperties::HostPropertiesSection);
    STRING localName;
    MgUtil::GenerateUuid(localName);
    STRING localAddress = m_serverManager->GetLocalServerAddress();

    m_localServerInfo->SetName(localName);
    m_localServerInfo->SetAddress(localAddress);
    m_localServerInfo->SetServiceFlags(MgServerInformation::ToServiceFlags(
        hostProps, m_localServerInfo->GetServiceFlags()));

    if (m_serverManager->IsSiteServer() != m_localServerInfo->IsSiteServer())
    {
        throw new MgLogicException(
            L"MgLoadBalanceManager.Initialize",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    for (INT32 i = 0; i < MgServerInformation::sm_knMaxNumberServices; ++i)
    {
        if (m_localServerInfo->IsServiceEnabled(i))
        {
            m_serverQueues[i].push_back(localAddress);
        }
    }

    // Initialize support server information if this is the site server.

    if (m_serverManager->IsSiteServer())
    {
        STRING serverIds;

        m_configuration->GetStringValue(
            MgConfigProperties::SupportServersSection,
            MgConfigProperties::SupportServerIdentifiersProperty,
            serverIds,
            L"");

        wchar_t* token = const_cast<wchar_t*>(serverIds.c_str());
        const wchar_t* delimit = L", \t\r\n\v\f";
        wchar_t* state = NULL;

        for (token = _wcstok(token, delimit, &state);
             token != NULL;
             token = _wcstok(NULL, delimit, &state))
        {
            STRING identifier = token;
            STRING name, description, address;
            STRING supportServerSection = MgConfigProperties::SupportServerSection;

            supportServerSection += identifier;

            m_configuration->GetStringValue(
                supportServerSection,
                MgConfigProperties::SupportServerNameProperty,
                name, L"");

            m_configuration->GetStringValue(
                supportServerSection,
                MgConfigProperties::SupportServerDescriptionProperty,
                description, L"");

            m_configuration->GetStringValue(
                supportServerSection,
                MgConfigProperties::SupportServerAddressProperty,
                address, L"");

            AddServer(name, description, address, identifier);
        }
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.Initialize")
}

///----------------------------------------------------------------------------
/// <summary>
/// Cleans up the load balance manager.
/// This method must be called once during the server shutdown time.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::Terminate()
{
    MgLoadBalanceManager::sm_loadBalanceManager = NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return a list of addresses of all the servers that host the specified
/// services.
///
MgStringCollection* MgLoadBalanceManager::GetServerAddresses(INT32 serviceFlags,
    bool includeLocalServer, bool includeSupportServers) const
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    Ptr<MgStringCollection> addresses;

    MG_TRY()

    addresses = new MgStringCollection();

    if (includeLocalServer)
    {
        if (m_localServerInfo->GetServiceFlags() & serviceFlags)
        {
            addresses->Add(m_localServerInfo->GetAddress());
        }
    }

    if (includeSupportServers)
    {
        for (MgServerMap::const_iterator i = m_supportServerMap.begin();
            i != m_supportServerMap.end(); ++i)
        {
            MgServerInformation* supportServerInfo = (*i).second;
            assert(NULL != supportServerInfo);

            if (supportServerInfo->GetServiceFlags() & serviceFlags)
            {
                addresses->Add(supportServerInfo->GetAddress());
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.GetServerAddresses")

    return addresses.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves an MgServerInformation object based on the specified server
/// address.
/// </summary>
///----------------------------------------------------------------------------

MgServerInformation* MgLoadBalanceManager::GetServerInfo(
    CREFSTRING address) const
{
    if (address.empty())
    {
        throw new MgNullArgumentException(
            L"MgLoadBalanceManager.GetServerInfo",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgServerInformation> serverInfo;

    if (0 == MgIpUtil::CompareAddresses(m_localServerInfo->GetAddress(), address))
    {
        serverInfo = SAFE_ADDREF(m_localServerInfo.p);
    }
    else
    {
        for (MgServerMap::const_iterator i = m_supportServerMap.begin();
            i != m_supportServerMap.end(); ++i)
        {
            MgServerInformation* supportServerInfo = (*i).second;

            if (0 == MgIpUtil::CompareAddresses(supportServerInfo->GetAddress(), address))
            {
                serverInfo = SAFE_ADDREF(supportServerInfo);
                break;
            }
        }
    }

    return serverInfo.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves an MgServerInformation object from the specified server
/// information list.
/// </summary>
///----------------------------------------------------------------------------

MgServerInformation* MgLoadBalanceManager::GetServerInfo(INT32 index,
    MgSerializableCollection* serverInfoList) const
{
    assert(NULL != serverInfoList);
    assert(index >= 0 && index < serverInfoList->GetCount());

    Ptr<MgSerializable> serializableObj = serverInfoList->GetItem(index);
    MgServerInformation* serverInfo = dynamic_cast<MgServerInformation*>(
        serializableObj.p);

    if (NULL == serverInfo)
    {
        throw new MgInvalidCastException(
            L"MgLoadBalanceManager.GetServerInfo",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return SAFE_ADDREF(serverInfo);
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the list of local/support server information.
/// </summary>
///----------------------------------------------------------------------------

MgSerializableCollection* MgLoadBalanceManager::GetServerInfoList(
    bool includeLocalServer, bool includeSupportServers,
    MgServerInformation* newServerInfo, MgServerInformation* removedServerInfo) const
{
    Ptr<MgSerializableCollection> serverInfoList =
        new MgSerializableCollection();

    if (includeLocalServer)
    {
        serverInfoList->Add(m_localServerInfo.p);
    }

    if (includeSupportServers)
    {
        for (MgServerMap::const_iterator i = m_supportServerMap.begin();
            i != m_supportServerMap.end(); ++i)
        {
            MgServerInformation* supportServerInfo = (*i).second;

            if (supportServerInfo != removedServerInfo)
            {
                serverInfoList->Add(supportServerInfo);
            }
        }
    }

    if (NULL != newServerInfo)
    {
        serverInfoList->Add(newServerInfo);
    }

    return serverInfoList.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Makes a copy of the list of local/support server information.
/// </summary>
///----------------------------------------------------------------------------

MgSerializableCollection* MgLoadBalanceManager::CopyServerInfoList(
    bool includeLocalServer, bool includeSupportServers,
    MgServerInformation* removedServerInfo) const
{
    Ptr<MgSerializableCollection> serverInfoList =
        new MgSerializableCollection();
    Ptr<MgServerInformation> copiedServerInfo;

    if (includeLocalServer)
    {
        copiedServerInfo = new MgServerInformation(*m_localServerInfo.p);
        serverInfoList->Add(copiedServerInfo.p);
    }

    if (includeSupportServers)
    {
        for (MgServerMap::const_iterator i = m_supportServerMap.begin();
            i != m_supportServerMap.end(); ++i)
        {
            MgServerInformation* supportServerInfo = (*i).second;

            if (supportServerInfo != removedServerInfo)
            {
                copiedServerInfo = new MgServerInformation(*supportServerInfo);
                serverInfoList->Add(copiedServerInfo.p);
            }
        }
    }

    return serverInfoList.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Finds a server by its name.
/// </summary>
///----------------------------------------------------------------------------

bool MgLoadBalanceManager::FindServerByName(CREFSTRING name) const
{
    if (name.empty())
    {
        throw new MgNullArgumentException(
            L"MgLoadBalanceManager.FindServerByName",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return (m_supportServerMap.end() != m_supportServerMap.find(name));
}
///----------------------------------------------------------------------------
/// <summary>
/// Finds a server by its address.
/// </summary>
///----------------------------------------------------------------------------

bool MgLoadBalanceManager::FindServerByAddress(CREFSTRING address) const
{
    Ptr<MgServerInformation> serverInfo = GetServerInfo(address);

    return (serverInfo != NULL);
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the Support Servers section in the configuration file.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::UpdateServerConfiguration()
{
    assert(m_serverManager->IsSiteServer());

    STRING serverIds;
    int count = 0;

    for (MgServerMap::const_iterator i = m_supportServerMap.begin();
        i != m_supportServerMap.end(); ++i)
    {
        if (count > 0)
        {
            serverIds += L",";
        }

        MgServerInformation* supportServerInfo = (*i).second;

        serverIds += supportServerInfo->GetIdentifier();
        ++count;
    }

    // Update the Support Servers section.

    Ptr<MgPropertyCollection> properties = new MgPropertyCollection();
    Ptr<MgStringProperty> property = new MgStringProperty(
        MgConfigProperties::SupportServerIdentifiersProperty, serverIds);
    properties->Add(property);

    m_configuration->SetProperties(MgConfigProperties::SupportServersSection,
        properties);
}

///----------------------------------------------------------------------------
/// <summary>
/// Appends a given server information to the specified server list.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::WriteServerList(MgServerInformation* serverInfo,
    string& serverList)
{
    // TODO: Need an XML writer.
    assert(NULL != serverInfo);

    serverList += "\t<Server>\n";

    serverList += "\t\t<Name>";
    serverList += MgUtil::WideCharToMultiByte(serverInfo->GetName());
    serverList += "</Name>\n";

    serverList += "\t\t<Description>";
    serverList += MgUtil::WideCharToMultiByte(serverInfo->GetDescription());
    serverList += "</Description>\n";

    serverList += "\t\t<IpAddress>";
    serverList += MgUtil::WideCharToMultiByte(serverInfo->GetAddress());
    serverList += "</IpAddress>\n";

    serverList += "\t</Server>\n";
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds the specified server to the queue.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::AddServerToQueue(INT32 serviceType,
    CREFSTRING serverAddress)
{
    assert(serviceType >= 0 && serviceType < MgServerInformation::sm_knMaxNumberServices);
    assert(!serverAddress.empty());

    // Every server must host the Server Admin service internally, so exclude
    // it from the server queue.
    if (MgServiceType::ServerAdminService != serviceType)
    {
        MgServerQueue& serverQueue = m_serverQueues[serviceType];

        for (MgServerQueue::iterator i = serverQueue.begin();
            i != serverQueue.end(); ++i)
        {
            if (serverAddress == *i)
            {
                return;
            }
        }

        serverQueue.push_front(serverAddress);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the server in the queue with the new IP address.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::UpdateServerInQueue(INT32 serviceType,
    CREFSTRING oldAddress, CREFSTRING newAddress)
{
    assert(serviceType >= 0 && serviceType < MgServerInformation::sm_knMaxNumberServices);
    assert(!oldAddress.empty() && !newAddress.empty());

    MgServerQueue& serverQueue = m_serverQueues[serviceType];

    for (MgServerQueue::iterator i = serverQueue.begin();
        i != serverQueue.end(); ++i)
    {
        if (oldAddress == *i)
        {
            *i = newAddress;

            return;
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Removes the specified server from the queue.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::RemoveServerFromQueue(INT32 serviceType,
    CREFSTRING serverAddress)
{
    assert(serviceType >= 0 && serviceType < MgServerInformation::sm_knMaxNumberServices);
    assert(!serverAddress.empty());

    MgServerQueue& serverQueue = m_serverQueues[serviceType];

    for (MgServerQueue::iterator i = serverQueue.begin();
        i != serverQueue.end(); ++i)
    {
        if (serverAddress == *i)
        {
            serverQueue.erase(i);

            return;
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Add/remove the specified server to/from all the server queues.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::UpdateServerQueues(MgServerInformation* serverInfo)
{
    assert(NULL != serverInfo);
    STRING address = serverInfo->GetAddress();

    for (INT32 i = 0; i < MgServerInformation::sm_knMaxNumberServices; ++i)
    {
        if (serverInfo->IsServiceEnabled(i))
        {
            AddServerToQueue(i, address);
        }
        else
        {
            RemoveServerFromQueue(i, address);
        }
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Registers services on the specified server that was just added to the queue.
/// </summary>
///----------------------------------------------------------------------------

bool MgLoadBalanceManager::RegisterServices(MgServerInformation* newServerInfo,
    MgServerInformation* removedServerInfo)
{
    bool success = false;

    MG_TRY()

    if (NULL == newServerInfo)
    {
        throw new MgNullArgumentException(
            L"MgLoadBalanceManager.RegisterServices",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Register the site server and other support servers' services with the new server.

    Ptr<MgSerializableCollection> newList = new MgSerializableCollection();
    newList->Add(newServerInfo);
    Ptr<MgSerializableCollection> fullList = GetServerInfoList(true, true, newServerInfo, removedServerInfo);
    Ptr<MgSerializableCollection> feedbackList = RegisterServicesOnServers(
        newServerInfo->GetAddress(), fullList);

    if (feedbackList == NULL || 1 != feedbackList->GetCount())
    {
        throw new MgLogicException(
            L"MgLoadBalanceManager.RegisterServices",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgServerInformation> serverInfo = GetServerInfo(0, feedbackList);

    newServerInfo->CopyFrom(*serverInfo, false);

    // Register the new server's services with other support servers.

    for (MgServerMap::const_iterator i = m_supportServerMap.begin();
        i != m_supportServerMap.end(); ++i)
    {
        MgServerInformation* supportServerInfo = (*i).second;

        // If an error occurs, catch the exception but do not throw it
        // so that the site server can continue registering
        // the new server's services with other support servers.

        try
        {
            if (supportServerInfo != newServerInfo &&
                supportServerInfo != removedServerInfo)
            {
                feedbackList = RegisterServicesOnServers(
                    supportServerInfo->GetAddress(), newList);
                assert(feedbackList != NULL && 1 == feedbackList->GetCount());
            }
        }
        catch (MgException* e)
        {
            // Server is down or request gets timed out? Log the message.
            STRING locale = m_serverManager->GetDefaultMessageLocale();

            ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(locale).c_str()));
            MG_LOG_SYSTEM_ENTRY(LM_ERROR, e->GetDetails(locale).c_str());
            MG_LOG_EXCEPTION_ENTRY(e->GetMessage(locale).c_str(), e->GetStackTrace(locale).c_str());

            SAFE_RELEASE(e);
        }
        catch (...)
        {
        }
    }

    // If the server connection could not be established, silently ignore it.
    // When the server is online, it will automaticaly register its services.

    MG_CATCH(L"MgLoadBalanceManager.RegisterServices")

    if (mgException == NULL)
    {
        success = true;
    }
    else if (!mgException->IsOfClass(MapGuide_Exception_MgConnectionFailedException)
          && !mgException->IsOfClass(MapGuide_Exception_MgConnectionNotOpenException))
    {
        MG_THROW()
    }

    return success;
}

///----------------------------------------------------------------------------
/// <summary>
/// Un-registers services on the specified server that was just removed from the queue.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::UnregisterServices(MgServerInformation* removedServerInfo)
{
    MG_TRY()

    if (NULL == removedServerInfo)
    {
        throw new MgNullArgumentException(
            L"MgLoadBalanceManager.UnregisterServices",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Un-register the site server and other support servers' services with the removed server.

    Ptr<MgSerializableCollection> fullList = CopyServerInfoList(true, true, removedServerInfo);
    assert(fullList != NULL && fullList->GetCount() > 0);

    for (INT32 i = 0; i < fullList->GetCount(); ++i)
    {
        Ptr<MgServerInformation> serverInfo = GetServerInfo(
            i, fullList);

        serverInfo->SetServiceFlags(0);
    }

    UnregisterServicesOnServers(removedServerInfo->GetAddress(), fullList);

    // Un-register the removed server's services with other support servers.

    Ptr<MgSerializableCollection> removedList = new MgSerializableCollection();
    Ptr<MgServerInformation> copiedServerInfo = new MgServerInformation(*removedServerInfo);

    copiedServerInfo->SetServiceFlags(0);
    removedList->Add(copiedServerInfo.p);

    for (MgServerMap::const_iterator i = m_supportServerMap.begin();
        i != m_supportServerMap.end(); ++i)
    {
        MgServerInformation* supportServerInfo = (*i).second;

        // If an error occurs, catch the exception but do not throw it
        // so that the site server can continue un-registering
        // the removed server's services with other support servers.

        try
        {
            if (supportServerInfo != removedServerInfo)
            {
                UnregisterServicesOnServers(supportServerInfo->GetAddress(), removedList);
            }
        }
        catch (MgException* e)
        {
            // Server is down or request gets timed out? Log the message.
            STRING locale = m_serverManager->GetDefaultMessageLocale();

            ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(locale).c_str()));
            MG_LOG_SYSTEM_ENTRY(LM_ERROR, e->GetDetails(locale).c_str());
            MG_LOG_EXCEPTION_ENTRY(e->GetMessage(locale).c_str(), e->GetStackTrace(locale).c_str());

            SAFE_RELEASE(e);
        }
        catch (...)
        {
        }
    }

    // If the server connection could not be established, silently ignore it.
    // When the server is offline, it will automaticaly un-register its services.

    MG_CATCH(L"MgLoadBalanceManager.UnregisterServices")
}

///----------------------------------------------------------------------------
/// <summary>
/// Registers services on the specified servers.
/// </summary>
///----------------------------------------------------------------------------

MgSerializableCollection* MgLoadBalanceManager::RegisterServicesOnServers(
    CREFSTRING serverAddress, MgSerializableCollection* serverInfoList)
{
    Ptr<MgSerializableCollection> feedbackList;

    MG_TRY()

    STRING ipAddress;
    MgIpUtil::HostNameToAddress(serverAddress, ipAddress);
    Ptr<MgUserInformation> userInformation = MgSecurityManager::CreateSystemCredentials();
    MgServerAdmin serverAdmin;

    serverAdmin.Open(ipAddress, userInformation);
    feedbackList = serverAdmin.RegisterServicesOnServers(serverInfoList);
    serverAdmin.Close();

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.RegisterServicesOnServers")

    return feedbackList.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Un-registers services on the specified servers.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::UnregisterServicesOnServers(
    CREFSTRING serverAddress, MgSerializableCollection* serverInfoList)
{
    MG_TRY()

    STRING ipAddress;
    MgIpUtil::HostNameToAddress(serverAddress, ipAddress);
    Ptr<MgUserInformation> userInformation = MgSecurityManager::CreateSystemCredentials();
    MgServerAdmin serverAdmin;

    serverAdmin.Open(ipAddress, userInformation);
    serverAdmin.UnregisterServicesOnServers(serverInfoList);
    serverAdmin.Close();

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.UnregisterServicesOnServers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enables and/or disables the specified services on the specified servers.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::EnableServicesOnServers(
    MgSerializableCollection* serverInfoList, bool strict)
{
    assert(NULL != serverInfoList);

    MG_TRY()

    bool sourceFromSiteServer = !m_serverManager->IsSiteServer();

    for (INT32 i = 0; i < serverInfoList->GetCount(); ++i)
    {
        Ptr<MgServerInformation> serverInfo = GetServerInfo(
            i, serverInfoList);

        if (serverInfo->IsSiteServer() &&
            0 != MgIpUtil::CompareAddresses(m_serverManager->GetSiteServerAddress(), serverInfo->GetAddress()))
        {
            // There are more than one Site servers within a site?
            throw new MgLogicException(
                L"MgLoadBalanceManager.EnableServicesOnServers",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerInformation> supportServerInfo = GetServerInfo(
            serverInfo->GetAddress());

        if (supportServerInfo != NULL)
        {
            // The server information have been changed, update the server queue
            // based on the changes.
            STRING oldAddress = supportServerInfo->GetAddress();
            INT32 oldServiceFlags = supportServerInfo->GetServiceFlags();

            supportServerInfo->CopyFrom(*serverInfo, sourceFromSiteServer);

            if (supportServerInfo->GetAddress() != oldAddress)
            {
                for (INT32 j = 0; j < MgServerInformation::sm_knMaxNumberServices; ++j)
                {
                    UpdateServerInQueue(j, oldAddress, supportServerInfo->GetAddress());
                }
            }

            if (supportServerInfo->GetServiceFlags() != oldServiceFlags)
            {
                UpdateServerQueues(supportServerInfo);
            }
        }
        else if (sourceFromSiteServer && !strict)
        {
            // The server does not exist, add it to the server map and update
            // the server queue.
            Ptr<MgServerInformation> newServerInfo = new MgServerInformation(
                *serverInfo);
            UpdateServerQueues(newServerInfo);
            std::pair<MgServerMap::iterator, bool> j = m_supportServerMap.insert(
                MgServerMap::value_type(serverInfo->GetName(), newServerInfo.Detach()));
            assert(j.second);
        }
        else
        {
            // The Support server has not been added to the site?
            MgStringCollection arguments;
            arguments.Add(serverInfo->GetAddress());

            throw new MgServerNotFoundException(
                L"MgLoadBalanceManager.EnableServicesOnServers",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.EnableServicesOnServers")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Notify the specified server of the resource changes.
///
void MgLoadBalanceManager::NotifyResourcesChanged(CREFSTRING serverAddress,
    MgSerializableCollection* resources)
{
    MG_TRY()

    assert(!serverAddress.empty());

    if (NULL != resources && resources->GetCount() > 0)
    {
        STRING ipAddress;
        MgIpUtil::HostNameToAddress(serverAddress, ipAddress);
        Ptr<MgUserInformation> userInformation = MgSecurityManager::CreateSystemCredentials();
        MgServerAdmin serverAdmin;

        serverAdmin.Open(ipAddress, userInformation);
        serverAdmin.NotifyResourcesChanged(resources);
        serverAdmin.Close();
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.NotifyResourcesChanged")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Dispatch resource change notifications to the specified servers.
///
void MgLoadBalanceManager::DispatchResourceChangeNotifications(
    MgStringCollection* serverAddresses,
    MgSerializableCollection* changedResources)
{
    MG_TRY()

    if (NULL != serverAddresses  &&  serverAddresses->GetCount() > 0 &&
        NULL != changedResources && changedResources->GetCount() > 0)
    {
        MG_LOG_TRACE_ENTRY(L"MgLoadBalanceManager::DispatchResourceChangeNotifications()");

        INT32 numServers = serverAddresses->GetCount();

        for (INT32 i = 0; i < numServers; ++i)
        {
            // If an error occurs, catch the exception but do not throw it
            // so that the site server can continue dispatching
            // resource change notifications to other support servers.

            try
            {
                NotifyResourcesChanged(serverAddresses->GetItem(i), changedResources);
            }
            catch (MgException* e)
            {
                // Server is down or request gets timed out? Log the message.
                STRING locale = m_serverManager->GetDefaultMessageLocale();

                ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(locale).c_str()));
                MG_LOG_SYSTEM_ENTRY(LM_ERROR, e->GetDetails(locale).c_str());
                MG_LOG_EXCEPTION_ENTRY(e->GetMessage(locale).c_str(), e->GetStackTrace(locale).c_str());

                SAFE_RELEASE(e);
            }
            catch (...)
            {
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.DispatchResourceChangeNotifications")
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the IP address of the next server available for the specified
/// service.
/// </summary>
///
/// <param name="serviceType">
/// Type of the service.
/// </param>
///
/// <returns>
/// IP address of the server.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgInvalidArgumentException
/// MgServiceNotAvailableException
///----------------------------------------------------------------------------

STRING MgLoadBalanceManager::RequestServer(INT32 serviceType)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, STRING(L"")));

    STRING serverAddress;

    MG_TRY()

    MgLogDetail logDetail(MgServiceType::SiteService,MgLogDetail::Trace,L"MgLoadBalanceManager.RequestServer",mgStackParams);
    logDetail.Create();

    MG_CHECK_RANGE(serviceType, 0, MgServerInformation::sm_knMaxNumberServices - 1,
        L"MgLoadBalanceManager.RequestServer");

    MgServerQueue& serverQueue = m_serverQueues[serviceType];
    size_t numServers = serverQueue.size();

    if (numServers == 1)
    {
        return serverQueue[0];
    }
    else if (numServers == 0)
    {
        throw new MgServiceNotAvailableException(
            L"MgLoadBalanceManager.RequestServer",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    serverAddress = serverQueue[0];
    serverQueue.pop_front();
    serverQueue.push_back(serverAddress);

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.RequestServer")

    return serverAddress;
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates all the support servers on the site.
/// </summary>
///----------------------------------------------------------------------------

MgByteReader* MgLoadBalanceManager::EnumerateServers()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    Ptr<MgByteReader> byteReader;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgLoadBalanceManager::EnumerateServers()");

    if (!m_serverManager->IsSiteServer())
    {
        throw new MgInvalidOperationException(
            L"MgLoadBalanceManager.EnumerateServers",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // TODO: Need an XML writer.

    // this XML follows the ServerList-1.0.0.xsd schema
    string serverList = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    serverList += "<ServerList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ServerList-1.0.0.xsd\">\n";

    for (MgServerMap::const_iterator i = m_supportServerMap.begin();
        i != m_supportServerMap.end(); ++i)
    {
        WriteServerList((*i).second, serverList);
    }

    serverList += "</ServerList>";

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)serverList.c_str(), (INT32)serverList.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.EnumerateServers")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a server to the site.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::AddServer(CREFSTRING name, CREFSTRING description,
    CREFSTRING address, CREFSTRING identifier)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_TRY()

    if (!m_serverManager->IsSiteServer())
    {
        throw new MgInvalidOperationException(
            L"MgLoadBalanceManager.AddServer",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (FindServerByName(name))
    {
        MgStringCollection arguments;
        arguments.Add(name);

        throw new MgDuplicateServerException(
            L"MgLoadBalanceManager.AddServer",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if (0 == MgIpUtil::CompareAddresses(m_serverManager->GetSiteServerAddress(), address))
    {
        throw new MgLogicException(
            L"MgLoadBalanceManager.AddServer",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (FindServerByAddress(address))
    {
        MgStringCollection arguments;
        arguments.Add(address);

        throw new MgDuplicateServerException(
            L"MgLoadBalanceManager.AddServer",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Add the server.

    STRING serverId = identifier;

    if (serverId.empty())
    {
        MgUtil::GenerateUuid(serverId);
    }

    Ptr<MgServerInformation> newServerInfo = new MgServerInformation(
        serverId, name, description, address);

    if (identifier.empty())
    {
        // Register services on the new server.
        if (RegisterServices(newServerInfo))
        {
            // Update the server in the queues.
            UpdateServerQueues(newServerInfo);
        }

        // Add the server to the map.
        std::pair<MgServerMap::iterator, bool> i = m_supportServerMap.insert(
            MgServerMap::value_type(name, SAFE_ADDREF(newServerInfo.p)));
        assert(i.second);

        // Update the Support Server configuration.
        STRING supportServerSection = MgConfigProperties::SupportServerSection;

        supportServerSection += serverId;

        Ptr<MgPropertyCollection> properties = new MgPropertyCollection();
        Ptr<MgStringProperty> property;

        property = new MgStringProperty(
            MgConfigProperties::SupportServerNameProperty, name);
        properties->Add(property);

        property = new MgStringProperty(
            MgConfigProperties::SupportServerDescriptionProperty, description);
        properties->Add(property);

        property = new MgStringProperty(
            MgConfigProperties::SupportServerAddressProperty, address);
        properties->Add(property);

        m_configuration->SetProperties(supportServerSection, properties);
        UpdateServerConfiguration();
    }
    else
    {
        // Add the server to the map.
        std::pair<MgServerMap::iterator, bool> i = m_supportServerMap.insert(
            MgServerMap::value_type(name, SAFE_ADDREF(newServerInfo.p)));
        assert(i.second);
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.AddServer");
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates an existing server on the site.
/// </summary>
///----------------------------------------------------------------------------

void MgLoadBalanceManager::UpdateServer(CREFSTRING oldName, CREFSTRING newName,
    CREFSTRING newDescription, CREFSTRING newAddress)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_TRY()

    if (!m_serverManager->IsSiteServer())
    {
        throw new MgInvalidOperationException(
            L"MgLoadBalanceManager.UpdateServer",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!FindServerByName(oldName))
    {
        MgStringCollection arguments;
        arguments.Add(oldName);

        throw new MgServerNotFoundException(
            L"MgLoadBalanceManager.UpdateServer",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if (newName.empty() && newDescription.empty() && newAddress.empty())
    {
        throw new MgNullArgumentException(
            L"MgLoadBalanceManager.UpdateServer",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgServerInformation* existingServerInfo = m_supportServerMap[oldName];
    assert(NULL != existingServerInfo);
    Ptr<MgServerInformation> newServerInfo = new MgServerInformation(
        *existingServerInfo);
    Ptr<MgPropertyCollection> properties = new MgPropertyCollection();
    Ptr<MgStringProperty> property;

    if (!newName.empty())
    {
        if (0 != _wcsicmp(oldName.c_str(), newName.c_str()) &&
            FindServerByName(newName))
        {
            MgStringCollection arguments;
            arguments.Add(newName);

            throw new MgDuplicateServerException(
                L"MgLoadBalanceManager.UpdateServer",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        newServerInfo->SetName(newName);

        property = new MgStringProperty(
            MgConfigProperties::SupportServerNameProperty, newName);
        properties->Add(property);
    }

    if (!newDescription.empty())
    {
        newServerInfo->SetDescription(newDescription);

        property = new MgStringProperty(
            MgConfigProperties::SupportServerDescriptionProperty, newDescription);
        properties->Add(property);
    }

    if (!newAddress.empty())
    {
        if (0 == MgIpUtil::CompareAddresses(m_serverManager->GetSiteServerAddress(), newAddress))
        {
            throw new MgLogicException(
                L"MgLoadBalanceManager.UpdateServer",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (0 != MgIpUtil::CompareAddresses(existingServerInfo->GetAddress(), newAddress) &&
            FindServerByAddress(newAddress))
        {
            MgStringCollection arguments;
            arguments.Add(newAddress);

            throw new MgDuplicateServerException(
                L"MgLoadBalanceManager.UpdateServer",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        newServerInfo->SetAddress(newAddress);

        property = new MgStringProperty(
            MgConfigProperties::SupportServerAddressProperty, newAddress);
        properties->Add(property);
    }

    // Do nothing if no server information has been changed.

    newServerInfo->Validate();

    if (*existingServerInfo == *newServerInfo.p)
    {
        // Let's be forgiving by not throwing an exception here.
        return;
    }

    // Update the server.

    if (existingServerInfo->GetAddress() == newServerInfo->GetAddress())
    {
        // It is OK to update the server name referenced by other support
        // servers because these servers are mapped by their addresses.
        *existingServerInfo = *newServerInfo.p;
    }
    else
    {
        // Un-register services on the existing server.
        UnregisterServices(existingServerInfo);

        // Register services on the new server.
        if (RegisterServices(newServerInfo, existingServerInfo))
        {
            // Replace the server in the queues.
            for (INT32 i = 0; i < MgServerInformation::sm_knMaxNumberServices; ++i)
            {
                UpdateServerInQueue(i, existingServerInfo->GetAddress(),
                    newServerInfo->GetAddress());
            }
        }
        else
        {
            // Remove the server from the queues.
            for (INT32 i = 0; i < MgServerInformation::sm_knMaxNumberServices; ++i)
            {
                RemoveServerFromQueue(i, existingServerInfo->GetAddress());
            }
        }

        // Replace the server in the map.
        Ptr<MgServerInformation> removedServerInfo = existingServerInfo;

        m_supportServerMap.erase(oldName);

        std::pair<MgServerMap::iterator, bool> j = m_supportServerMap.insert(
            MgServerMap::value_type(newServerInfo->GetName(), SAFE_ADDREF(newServerInfo.p)));
        assert(j.second);
    }

    // Update the Support Server configuration.
    STRING supportServerSection = MgConfigProperties::SupportServerSection;

    supportServerSection += newServerInfo->GetIdentifier();
    m_configuration->SetProperties(supportServerSection, properties);

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.UpdateServer");
}

///----------------------------------------------------------------------------
/// <summary>
/// Remove server from the site.
/// </summary>
/// <param name="name">
/// Name of server to remove from site.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgServerNotFoundException
///----------------------------------------------------------------------------

void MgLoadBalanceManager::RemoveServer(CREFSTRING name)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_TRY()

    if (!m_serverManager->IsSiteServer())
    {
        throw new MgInvalidOperationException(
            L"MgLoadBalanceManager.RemoveServer",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!FindServerByName(name))
    {
        MgStringCollection arguments;
        arguments.Add(name);

        throw new MgServerNotFoundException(
            L"MgLoadBalanceManager.RemoveServer",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Un-register services on the removed server.
    MgServerInformation* existingServerInfo = m_supportServerMap[name];

    UnregisterServices(existingServerInfo);

    // Remove the server from the queues.
    for (INT32 i = 0; i < MgServerInformation::sm_knMaxNumberServices; ++i)
    {
        RemoveServerFromQueue(i, existingServerInfo->GetAddress());
    }

    // Remove the server from the map.
    Ptr<MgServerInformation> removedServerInfo = existingServerInfo;

    m_supportServerMap.erase(name);

    // Update the Support Server configuration.
    STRING supportServerSection = MgConfigProperties::SupportServerSection;

    supportServerSection += removedServerInfo->GetIdentifier();
    m_configuration->RemoveProperties(supportServerSection, NULL);
    UpdateServerConfiguration();

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.RemoveServer");
}

///----------------------------------------------------------------------------
/// <summary>
/// Registers services on the specified servers.
/// </summary>
///
/// <param name="serverInfoList">
/// List of server information such as name, description, IP address, and
/// available services.
/// </param>
///
/// <returns>
/// List of server information from all other servers within a site.
/// The returned list may be NULL.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgNullArgumentException
/// MgInvalidArgumentException
///----------------------------------------------------------------------------

MgSerializableCollection* MgLoadBalanceManager::RegisterServicesOnServers(
    MgSerializableCollection* serverInfoList)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    Ptr<MgSerializableCollection> feedbackList;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgLoadBalanceManager::RegisterServicesOnServers()");

    if (NULL == serverInfoList || serverInfoList->GetCount() <= 0)
    {
        throw new MgNullArgumentException(
            L"MgLoadBalanceManager.RegisterServicesOnServers",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    EnableServicesOnServers(serverInfoList, false);

    if (m_serverManager->IsSiteServer())
    {
        if (1 != serverInfoList->GetCount())
        {
            STRING buffer;
            MgUtil::Int32ToString(serverInfoList->GetCount(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(
                L"MgLoadBalanceManager.RegisterServicesOnServers",
                __LINE__, __WFILE__, &arguments, L"MgInvalidCollectionSize", NULL);
        }

        feedbackList = CopyServerInfoList(true, true);
        assert(feedbackList != NULL && feedbackList->GetCount() > 0);
    }
    else
    {
        feedbackList = CopyServerInfoList(true, false);
        assert(feedbackList != NULL && 1 == feedbackList->GetCount());
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.RegisterServicesOnServers")

    return feedbackList.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Un-registers services on the specified servers.
/// </summary>
///
/// <param name="serverInfoList">
/// List of server information such as name, description, IP address, and
/// available services.
/// </param>
///
/// <returns>
/// Nothing.
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgNullArgumentException
/// MgInvalidArgumentException
///----------------------------------------------------------------------------

void MgLoadBalanceManager::UnregisterServicesOnServers(
    MgSerializableCollection* serverInfoList)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgLoadBalanceManager::UnregisterServicesOnServers()");

    if (NULL == serverInfoList || serverInfoList->GetCount() <= 0)
    {
        throw new MgNullArgumentException(
            L"MgLoadBalanceManager.UnregisterServicesOnServers",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    EnableServicesOnServers(serverInfoList, true);

    if (m_serverManager->IsSiteServer() && !m_supportServerMap.empty())
    {
        if (1 != serverInfoList->GetCount())
        {
            STRING buffer;
            MgUtil::Int32ToString(serverInfoList->GetCount(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(
                L"MgLoadBalanceManager.UnregisterServicesOnServers",
                __LINE__, __WFILE__, &arguments, L"MgInvalidCollectionSize", NULL);
        }

        Ptr<MgServerInformation> serverInfo = GetServerInfo(0, serverInfoList);
        STRING serverAddress = serverInfo->GetAddress();

        for (MgServerMap::const_iterator i = m_supportServerMap.begin();
            i != m_supportServerMap.end(); ++i)
        {
            MgServerInformation* supportServerInfo = (*i).second;

            // If an error occurs, catch the exception but do not throw it
            // so that the site server can continue un-registering
            // the specified server's services with other support servers.

            try
            {
                if (0 != MgIpUtil::CompareAddresses(supportServerInfo->GetAddress(), serverAddress))
                {
                    UnregisterServicesOnServers(supportServerInfo->GetAddress(),
                        serverInfoList);
                }
            }
            catch (MgException* e)
            {
                // Server is down or request gets timed out? Log the message.
                STRING locale = m_serverManager->GetDefaultMessageLocale();

                ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(locale).c_str()));
                MG_LOG_SYSTEM_ENTRY(LM_ERROR, e->GetDetails(locale).c_str());
                MG_LOG_EXCEPTION_ENTRY(e->GetMessage(locale).c_str(), e->GetStackTrace(locale).c_str());

                SAFE_RELEASE(e);
            }
            catch (...)
            {
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.UnregisterServicesOnServers")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Register services on the local server and other servers within the site.
///
bool MgLoadBalanceManager::RegisterServices()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    bool success = false;

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgLoadBalanceManager::RegisterServices()");

    if (m_serverManager->IsSiteServer())
    {
        if (!m_supportServerMap.empty())
        {
            // Register services on this site server with any currently online support server.

            Ptr<MgSerializableCollection> onlineList = new MgSerializableCollection();
            Ptr<MgSerializableCollection> localList = GetServerInfoList(true, false);
            assert(localList != NULL && 1 == localList->GetCount());

            for (MgServerMap::const_iterator i = m_supportServerMap.begin();
                i != m_supportServerMap.end(); ++i)
            {
                MgServerInformation* supportServerInfo = (*i).second;

                // If an error occurs, catch the exception but do not throw it
                // so that the site server can continue registering
                // its services with other support servers.

                try
                {
                    Ptr<MgSerializableCollection> feedbackList = RegisterServicesOnServers(
                        supportServerInfo->GetAddress(), localList);

                    if (feedbackList == NULL || 1 != feedbackList->GetCount())
                    {
                        throw new MgLogicException(
                            L"MgLoadBalanceManager.RegisterServices",
                            __LINE__, __WFILE__, NULL, L"", NULL);
                    }

                    Ptr<MgServerInformation> serverInfo = GetServerInfo(0, feedbackList);

                    supportServerInfo->CopyFrom(*serverInfo, false);
                    onlineList->Add(supportServerInfo);
                }
                catch (MgException* e)
                {
                    // Server is down or request gets timed out? Log the message.
                    STRING locale = m_serverManager->GetDefaultMessageLocale();

                    ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(locale).c_str()));
                    MG_LOG_SYSTEM_ENTRY(LM_ERROR, e->GetDetails(locale).c_str());
                    MG_LOG_EXCEPTION_ENTRY(e->GetMessage(locale).c_str(), e->GetStackTrace(locale).c_str());

                    SAFE_RELEASE(e);
                }
                catch (...)
                {
                }
            }

            // Register services on currently online support servers with each other (excluding the site server).

            for (INT32 i = 0; i < onlineList->GetCount(); ++i)
            {
                Ptr<MgServerInformation> serverInfo = GetServerInfo(i, onlineList);
                Ptr<MgSerializableCollection> feedbackList = RegisterServicesOnServers(
                    serverInfo->GetAddress(), onlineList);

                if (feedbackList == NULL || 1 != feedbackList->GetCount()) // dummy feedback
                {
                    throw new MgLogicException(
                        L"MgLoadBalanceManager.RegisterServices",
                        __LINE__, __WFILE__, NULL, L"", NULL);
                }

                UpdateServerQueues(serverInfo);
            }
        }
    }
    else
    {
        // Register services on this support server with the site server.

        Ptr<MgSerializableCollection> localList = GetServerInfoList(true, false);
        assert(localList != NULL && 1 == localList->GetCount());
        Ptr<MgSerializableCollection> feedbackList = RegisterServicesOnServers(
            m_serverManager->GetSiteServerAddress(), localList);

        if (feedbackList == NULL || feedbackList->GetCount() <= 0)
        {
            throw new MgLogicException(
                L"MgLoadBalanceManager.RegisterServices",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Enables and/or disables services on other servers (including the site server).

        EnableServicesOnServers(feedbackList, false);
    }

    success = true;

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.RegisterServices")

    return success;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Unregister services on the local server and other servers within the site.
///
void MgLoadBalanceManager::UnregisterServices()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgLoadBalanceManager::UnregisterServices()");

    Ptr<MgSerializableCollection> localList = CopyServerInfoList(true, false);
    assert(localList != NULL && 1 == localList->GetCount());

    for (INT32 i = 0; i < localList->GetCount(); ++i)
    {
        Ptr<MgServerInformation> serverInfo = GetServerInfo(
            i, localList);

        serverInfo->SetServiceFlags(0);
    }

    if (m_serverManager->IsSiteServer())
    {
        for (MgServerMap::const_iterator i = m_supportServerMap.begin();
            i != m_supportServerMap.end(); ++i)
        {
            MgServerInformation* supportServerInfo = (*i).second;

            // If an error occurs, catch the exception but do not throw it
            // so that the site server can continue un-registering
            // its services with other support servers.

            try
            {
                UnregisterServicesOnServers(supportServerInfo->GetAddress(),
                    localList);
            }
            catch (MgException* e)
            {
                // Server is down or request gets timed out? Log the message.
                STRING locale = m_serverManager->GetDefaultMessageLocale();

                ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(locale).c_str()));
                MG_LOG_SYSTEM_ENTRY(LM_ERROR, e->GetDetails(locale).c_str());
                MG_LOG_EXCEPTION_ENTRY(e->GetMessage(locale).c_str(), e->GetStackTrace(locale).c_str());

                SAFE_RELEASE(e);
            }
            catch (...)
            {
            }
        }
    }
    else
    {
        // If an error occurs, catch the exception but do not throw it
        // so that the support server can continue un-registering
        // its services with other servers.

        try
        {
            UnregisterServicesOnServers(m_serverManager->GetSiteServerAddress(),
                localList);
        }
        catch (MgException* e)
        {
            // Server is down or request gets timed out? Log the message.
            STRING locale = m_serverManager->GetDefaultMessageLocale();

            ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(locale).c_str()));
            MG_LOG_SYSTEM_ENTRY(LM_ERROR, e->GetDetails(locale).c_str());
            MG_LOG_EXCEPTION_ENTRY(e->GetMessage(locale).c_str(), e->GetStackTrace(locale).c_str());

            SAFE_RELEASE(e);
        }
        catch (...)
        {
        }
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.UnregisterServices")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enable/disable specified services on the local server and other servers
/// within the site.
///
void MgLoadBalanceManager::EnableServices(INT32 serviceFlags)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgLoadBalanceManager::EnableServices()");

    m_localServerInfo->SetServiceFlags(serviceFlags);
    UpdateServerQueues(m_localServerInfo);

    Ptr<MgSerializableCollection> localList = CopyServerInfoList(true, false);
    assert(localList != NULL && 1 == localList->GetCount());

    // If the server is offline, then disable its services on other servers.
    if (!m_serverManager->IsOnline())
    {
        for (INT32 i = 0; i < localList->GetCount(); ++i)
        {
            Ptr<MgServerInformation> serverInfo = GetServerInfo(
                i, localList);

            serverInfo->SetServiceFlags(0);
        }
    }

    if (m_serverManager->IsSiteServer())
    {
        for (MgServerMap::const_iterator i = m_supportServerMap.begin();
            i != m_supportServerMap.end(); ++i)
        {
            MgServerInformation* supportServerInfo = (*i).second;

            // If an error occurs, catch the exception but do not throw it
            // so that the site server can continue enabling
            // its services on other support servers.

            try
            {
                Ptr<MgSerializableCollection> feedbackList = RegisterServicesOnServers(
                    supportServerInfo->GetAddress(), localList);
                assert(feedbackList != NULL && 1 == feedbackList->GetCount());
            }
            catch (MgException* e)
            {
                // Server is down or request gets timed out? Log the message.
                STRING locale = m_serverManager->GetDefaultMessageLocale();

                ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(locale).c_str()));
                MG_LOG_SYSTEM_ENTRY(LM_ERROR, e->GetDetails(locale).c_str());
                MG_LOG_EXCEPTION_ENTRY(e->GetMessage(locale).c_str(), e->GetStackTrace(locale).c_str());

                SAFE_RELEASE(e);
            }
            catch (...)
            {
            }
        }
    }
    else
    {
        // If an error occurs, catch the exception but do not throw it
        // so that the support server can continue enabling
        // its services on other servers.

        try
        {
            Ptr<MgSerializableCollection> feedbackList = RegisterServicesOnServers(
                m_serverManager->GetSiteServerAddress(), localList);
            assert(feedbackList != NULL && feedbackList->GetCount() > 0);
        }
        catch (MgException* e)
        {
            // Server is down or request gets timed out? Log the message.
            STRING locale = m_serverManager->GetDefaultMessageLocale();

            ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(locale).c_str()));
            MG_LOG_SYSTEM_ENTRY(LM_ERROR, e->GetDetails(locale).c_str());
            MG_LOG_EXCEPTION_ENTRY(e->GetMessage(locale).c_str(), e->GetStackTrace(locale).c_str());

            SAFE_RELEASE(e);
        }
        catch (...)
        {
        }
    }

    MG_CATCH_AND_THROW(L"MgLoadBalanceManager.EnableServices")
}
