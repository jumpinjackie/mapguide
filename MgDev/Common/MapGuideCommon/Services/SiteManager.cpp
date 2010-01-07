//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "MapGuideCommon.h"

// Process-wide MgSiteManager
Ptr<MgSiteManager> MgSiteManager::sm_siteManager = (MgSiteManager*)NULL;

// Background thread function
ACE_THR_FUNC_RETURN CheckServers(void* param)
{
    SMThreadData* threadData = (SMThreadData*)param;
    // Store our unique thread ID
    threadData->threadId = ACE_Thread::self();

    MgSiteManager* siteManager = MgSiteManager::GetInstance();

    bool bDone = false;
    while(!bDone)
    {
        INT32 sleepTime = threadData->failoverRetryTime;

        MgSiteVector* sites = siteManager->GetSites();
        if(sites)
        {
            for(size_t i=0;i<sites->size();i++)
            {
                MgSiteInfo* siteInfo = sites->at(i);

                // Check the server status
                if (MgSiteInfo::Ok != siteInfo->GetStatus())
                {
                    // This is an unavailable server so we need to check if it is now available
                    MG_TRY()

                    // Send a simple request to the server to see if it can respond
                    Ptr<MgUserInformation> userInformation = new MgUserInformation(L"", L"");
                    Ptr<MgConnectionProperties> connProp = siteManager->GetConnectionProperties(userInformation, siteInfo, MgSiteInfo::Admin);

                    // Use GetSiteStatus because it doesn't require authentication
                    MgCommand cmd;
                    cmd.ExecuteCommand(connProp,                               // Connection
                                       MgCommand::knObject,                    // Return type expected
                                       MgServerAdminServiceOpId::GetSiteStatus, // Command Code
                                       0,                                      // No of arguments
                                       ServerAdmin_Service,                    // Service Id
                                       BUILD_VERSION(2,2,0),                   // Operation version
                                       MgCommand::knNone);

                    // The server responded so update the status if the server is online
                    Ptr<MgPropertyCollection> properties = (MgPropertyCollection*)cmd.GetReturnValue().val.m_obj;
                    if(properties.p)
                    {
                        // Check the status of the server
                        Ptr<MgBooleanProperty> boolProp = (MgBooleanProperty*)properties->GetItem(MgServerInformationProperties::Status);
                        if(boolProp->GetValue())
                        {
                            // Server is there and "Online"
                            ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, siteManager->m_mutex, NULL));
                            siteInfo->SetStatus(MgSiteInfo::Ok);
                        }
                    }

                    MG_CATCH_AND_RELEASE()
                    if(NULL != mgException.p)
                    {
                        // Exception was thrown
                        STRING message = mgException->GetMessage();
                    }
                }
            }

            // Does this site only contain 1 server?
            if(sites->size() == 1)
            {
                MgSiteInfo* siteInfo = sites->at(0);

                // Check the server status in case it was just updated
                if (MgSiteInfo::Ok != siteInfo->GetStatus())
                {
                    // There is only 1 server in the site and it is not in a "Good" state yet so reduce the sleep time
                    sleepTime = 1;
                }
            }
        }

        // Sleep the thread as thread resume/suspend is not supported on all platforms
        for(size_t i=0;i<sleepTime;i++)
        {
            ACE_OS::sleep(1);

            // Update whether we are done or not
            bDone = siteManager->GetCheckServersThreadDone();
            if(bDone)
            {
                // We don't want to sleep anymore as we need to shutdown this thread ASAP
                break;
            }
        }
    }

    return 0;
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSiteManager::MgSiteManager() :
    m_index(0)
{
    Initialize();
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSiteManager::~MgSiteManager()
{
    MG_TRY()

    ClearSiteInfo();

    // Block any threads as we are updating the thread done
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
    m_bCheckServersThreadDone = true;

    MG_CATCH(L"MgSiteManager.~MgSiteManager")
}

///----------------------------------------------------------------------------
/// <summary>
/// Disposes the object.
/// </summary>
///----------------------------------------------------------------------------

void MgSiteManager::Dispose()
{
    delete this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Releases the stored site information
/// </summary>
///----------------------------------------------------------------------------

void MgSiteManager::ClearSiteInfo()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    for (MgSiteVector::iterator iter = m_sites.begin();
        iter != m_sites.end(); iter++)
    {
        SAFE_RELEASE(*iter);
    }

    m_sites.clear();
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the pointer to a process-wide MgSiteManager.
/// </summary>
///----------------------------------------------------------------------------

MgSiteManager* MgSiteManager::GetInstance()
{
    MG_TRY()

    ACE_TRACE("MgSiteManager::GetInstance");

    if (MgSiteManager::sm_siteManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance(), NULL));

        if (MgSiteManager::sm_siteManager == NULL)
        {
            MgSiteManager::sm_siteManager = new MgSiteManager();
        }
    }

    MG_CATCH_AND_THROW(L"MgSiteManager.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgSiteManager::sm_siteManager;
}

///----------------------------------------------------------------------------
/// <summary>
/// Intializes the site manager.
/// </summary>
///----------------------------------------------------------------------------

void MgSiteManager::Initialize()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    MG_TRY()

    ClearSiteInfo();

    MgConfiguration* configuration = MgConfiguration::GetInstance();
    assert(NULL != configuration);

    // Get the IP address and port for the site server.

    STRING targets;
    STRING sitePorts;
    STRING clientPorts;
    STRING adminPorts;

    configuration->GetStringValue(
        MgConfigProperties::SiteConnectionPropertiesSection,
        MgConfigProperties::SiteConnectionPropertyIpAddress,
        targets,
        MgConfigProperties::DefaultSiteConnectionPropertyIpAddress);

    configuration->GetStringValue(
        MgConfigProperties::SiteConnectionPropertiesSection,
        MgConfigProperties::SiteConnectionPropertyPort,
        sitePorts,
        L"");

    configuration->GetStringValue(
        MgConfigProperties::ClientConnectionPropertiesSection,
        MgConfigProperties::ClientConnectionPropertyPort,
        clientPorts,
        L"");

    configuration->GetStringValue(
        MgConfigProperties::AdministrativeConnectionPropertiesSection,
        MgConfigProperties::AdministrativeConnectionPropertyPort,
        adminPorts,
        L"");

    Ptr<MgStringCollection> targetValues = MgStringCollection::ParseCollection(targets, L",");
    Ptr<MgStringCollection> sitePortValues = MgStringCollection::ParseCollection(sitePorts, L",");
    Ptr<MgStringCollection> clientPortValues = MgStringCollection::ParseCollection(clientPorts, L",");
    Ptr<MgStringCollection> adminPortValues = MgStringCollection::ParseCollection(adminPorts, L",");

    INT32 defaultSitePort = sitePortValues->GetCount() > 0 ? MgUtil::StringToInt32(sitePortValues->GetItem(0)) : MgConfigProperties::DefaultSiteConnectionPropertyPort;
    INT32 defaultClientPort = clientPortValues->GetCount() > 0 ? MgUtil::StringToInt32(clientPortValues->GetItem(0)) : MgConfigProperties::DefaultClientConnectionPropertyPort;
    INT32 defaultAdminPort = adminPortValues->GetCount() > 0 ? MgUtil::StringToInt32(adminPortValues->GetItem(0)) : MgConfigProperties::DefaultAdministrativeConnectionPropertyPort;

    INT32 numTargets = targetValues->GetCount();

    for (INT32 i = 0; i < numTargets; i++)
    {
        INT32 sitePort = sitePortValues->GetCount() > i ? MgUtil::StringToInt32(sitePortValues->GetItem(i)) : defaultSitePort;
        INT32 clientPort = clientPortValues->GetCount() > i ? MgUtil::StringToInt32(clientPortValues->GetItem(i)) : defaultClientPort;
        INT32 adminPort = adminPortValues->GetCount() > i ? MgUtil::StringToInt32(adminPortValues->GetItem(i)) : defaultAdminPort;

        STRING targetIp;
        MgIpUtil::HostNameToAddress(targetValues->GetItem(i), targetIp);

        Ptr<MgSiteInfo> siteInfo = new MgSiteInfo(targetIp,
            sitePort, clientPort, adminPort);
        m_sites.push_back(siteInfo.Detach());
    }

    // Create background thread that will check any unavailable servers to see if their status has changed
    INT32 failoverRetryTime = MgConfigProperties::DefaultGeneralPropertyFailoverRetryTime;
    configuration->GetIntValue(
        MgConfigProperties::GeneralPropertiesSection,
        MgConfigProperties::GeneralPropertyFailoverRetryTime,
        failoverRetryTime,
        MgConfigProperties::DefaultGeneralPropertyFailoverRetryTime);

    m_threadData.failoverRetryTime = failoverRetryTime;

    m_bCheckServersThreadDone = false;

    MG_CATCH_AND_THROW(L"MgSiteManager.Initialize")
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves connection properties for a site server. If useSessionIp is true
/// and the userInfo contains a session ID, the connection properties will be
/// generated for the site that contains that session.
/// </summary>
///----------------------------------------------------------------------------

MgConnectionProperties* MgSiteManager::GetConnectionProperties(
    MgUserInformation* userInfo, MgSiteInfo::MgPortType portType, bool useSessionIp)
{
    if (NULL == userInfo)
    {
        throw new MgNullArgumentException(
            L"MgSiteManager.GetConnectionProperties",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgConnectionProperties> connProps;

    // Determine if the user info contains a session ID.
    STRING sessionId = userInfo->GetMgSessionId();

    if (useSessionIp && !sessionId.empty())
    {
        size_t length = sessionId.length();

        if (length > MgSiteInfo::HexStringLength)
        {
            STRING siteHexString = sessionId.substr(
                length - MgSiteInfo::HexStringLength, MgSiteInfo::HexStringLength);
            Ptr<MgSiteInfo> siteInfo = GetSiteInfo(siteHexString);

			if ((NULL != siteInfo.p) && (MgSiteInfo::Ok == siteInfo->GetStatus()))
            {
                connProps = GetConnectionProperties(userInfo, siteInfo, portType);
            }
            else
            {
                // This site is not currently working

                // We have a session, but it will not exist on any other machine so we force the session exception
                throw new MgSessionExpiredException(L"MgSiteManager.GetConnectionProperties",__LINE__,__WFILE__, NULL, L"", NULL);
            }
        }
    }
    else
    {
        Ptr<MgSiteInfo> nextSite = GetNextSite();

        if (NULL != nextSite.p)
        {
            connProps = GetConnectionProperties(userInfo, nextSite, portType);
        }
    }

    if (NULL == connProps.p)
    {
        // There might not be any MapGuide servers running for the site
        throw new MgConnectionFailedException(
            L"MgSiteManager.GetConnectionProperties",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return connProps.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves connection properties for the site specified in the siteInfo object
/// </summary>
///----------------------------------------------------------------------------
MgConnectionProperties* MgSiteManager::GetConnectionProperties(
    MgUserInformation* userInfo, MgSiteInfo* siteInfo, MgSiteInfo::MgPortType portType)
{
    if (NULL == userInfo || NULL == siteInfo)
    {
        throw new MgNullArgumentException(
            L"MgSiteManager.GetConnectionProperties",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgConnectionProperties> connProps = new MgConnectionProperties(
        userInfo, siteInfo->GetTarget(), siteInfo->GetPort(portType));

    return connProps.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves connection properties for a support server. If useSessionIp is true
/// and the userInfo contains a session ID, the connection properties will be
/// generated for the site that contains that session.
/// </summary>
///----------------------------------------------------------------------------
MgConnectionProperties* MgSiteManager::GetSupportServerConnectionProperties(
    CREFSTRING supportServer, MgUserInformation* userInfo, MgSiteInfo::MgPortType portType)
{
    Ptr<MgConnectionProperties> supportConnProps;

    // Session Affinity: Retrieve the site server corresponding to this user info
    Ptr<MgConnectionProperties> siteConnProps = GetConnectionProperties(userInfo, portType, true);

    if (NULL != siteConnProps.p)
    {
        // Create support server connection props using the specified IP target and the same admin port
        // used by the site server
        supportConnProps = new MgConnectionProperties(userInfo, supportServer,
            siteConnProps->GetPort());
    }

    return supportConnProps.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves site info for the next site server that has an Ok status.
/// </summary>
///----------------------------------------------------------------------------
MgSiteInfo* MgSiteManager::GetNextSite()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    MgSiteInfo* nextSite = NULL;
    INT32 numSites = (INT32)m_sites.size();

    for (INT32 siteIndex = 0; siteIndex < numSites; siteIndex++)
    {
        INT32 siteToTry = (m_index + siteIndex) % numSites;
        MgSiteInfo* siteInfo = m_sites.at(siteToTry);

        if (MgSiteInfo::Ok == siteInfo->GetStatus())
        {
            nextSite = siteInfo;
            m_index = siteToTry + 1;
            break;
        }
    }

    return SAFE_ADDREF(nextSite);
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves the number of configured site servers
/// </summary>
///----------------------------------------------------------------------------
INT32 MgSiteManager::GetSiteCount()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, 0));

    return (INT32)m_sites.size();
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves site info for the site server corresponding to the specified index
/// </summary>
///----------------------------------------------------------------------------
MgSiteInfo* MgSiteManager::GetSiteInfo(INT32 index)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    MG_CHECK_RANGE(index, 0, INT32(m_sites.size() - 1), L"MgSiteManager.GetSiteInfo");

    MgSiteInfo* siteInfo = m_sites.at(index);

    return SAFE_ADDREF(siteInfo);
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves site info for the site server matching the specified target and port
/// </summary>
///----------------------------------------------------------------------------
MgSiteInfo* MgSiteManager::GetSiteInfo(CREFSTRING target, INT32 port)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    Ptr<MgSiteInfo> matchingSiteInfo;

    for (INT32 i = 0; i < (INT32)m_sites.size(); i++)
    {
        MgSiteInfo* siteInfo = m_sites.at(i);

        if (siteInfo->GetTarget() == target
            && (siteInfo->GetPort(MgSiteInfo::Site)   == port ||
                siteInfo->GetPort(MgSiteInfo::Client) == port ||
                siteInfo->GetPort(MgSiteInfo::Admin)  == port))
        {
            matchingSiteInfo = SAFE_ADDREF(siteInfo);
            break;
        }
    }

    return matchingSiteInfo.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves site info for the site server matching the specified target and port
/// contained within the specifed hex string.
/// </summary>
///----------------------------------------------------------------------------
MgSiteInfo* MgSiteManager::GetSiteInfo(CREFSTRING hexString)
{
    Ptr<MgSiteInfo> matchingSiteInfo;

    if(hexString.length() >= MgSiteInfo::HexStringLength)
    {
        UINT32 n1, n2, n3, n4;
        INT32 sitePort, clientPort, adminPort;
        STRING target;

        // Read the IP parts into their variables
        if(::swscanf(hexString.c_str(), L"%2X%2X%2X%2X%4X%4X%4X", &n1, &n2, &n3, &n4, &sitePort, &clientPort, &adminPort) == 7)
        {
            // Write the 4 'n' values into an IP string
            wchar_t buffer[20];
            swprintf(buffer, 20, L"%u.%u.%u.%u", n1, n2, n3, n4);
            target = buffer;
        }

        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

        for (INT32 i = 0; i < (INT32)m_sites.size(); i++)
        {
            MgSiteInfo* siteInfo = m_sites.at(i);

            if (siteInfo->GetTarget() == target
                && (siteInfo->GetPort(MgSiteInfo::Site)   == sitePort ||
                    siteInfo->GetPort(MgSiteInfo::Client) == clientPort ||
                    siteInfo->GetPort(MgSiteInfo::Admin)  == adminPort))
            {
                matchingSiteInfo = SAFE_ADDREF(siteInfo);
                break;
            }
        }
    }

    if (matchingSiteInfo == NULL)
        matchingSiteInfo = new MgSiteInfo(hexString);

    return matchingSiteInfo.Detach();
}

MgSiteVector* MgSiteManager::GetSites()
{
    return &m_sites;
}

bool MgSiteManager::GetCheckServersThreadDone()
{
    return m_bCheckServersThreadDone;
}

void MgSiteManager::SetCheckServersThreadDone(bool bDone)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
    m_bCheckServersThreadDone = bDone;
}

void MgSiteManager::StartCheckServersThread()
{
    // Need a thread manager
    ACE_Thread_Manager* manager = ACE_Thread_Manager::instance();
    
    // Create the background thread
    manager->spawn(ACE_THR_FUNC(CheckServers), &m_threadData);
}

void MgSiteManager::StopCheckServersThread()
{
    SetCheckServersThreadDone(true);
}
