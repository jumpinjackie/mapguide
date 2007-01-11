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
#include "MapGuideCommon.h"

// Process-wide MgSiteManager
Ptr<MgSiteManager> MgSiteManager::sm_siteManager = (MgSiteManager*)NULL;
ACE_Recursive_Thread_Mutex MgSiteManager::sm_siteManagerMutex;

#define MIN(x,y) ((x) < (y) ? (x) : (y))

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSiteManager::MgSiteManager()
{
    m_configuration = MgConfiguration::GetInstance();
    assert(NULL != m_configuration);

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

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgSiteManager::~MgSiteManager()\n")));

    ClearSiteInfo();

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
    for(MgSiteVector::iterator iter = m_sites.begin(); iter != m_sites.end(); iter++)
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
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_siteManagerMutex));

    MG_TRY()

    ACE_DEBUG((LM_DEBUG, ACE_TEXT( "(%P|%t) MgSiteManager::Initialize()\n")));

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

    int numTargets = targetValues->GetCount();
    for(int i = 0; i < numTargets; i++)
    {
        INT32 sitePort = sitePortValues->GetCount() > i ? MgUtil::StringToInt32(sitePortValues->GetItem(i)) : defaultSitePort;
        INT32 clientPort = clientPortValues->GetCount() > i ? MgUtil::StringToInt32(clientPortValues->GetItem(i)) : defaultClientPort;
        INT32 adminPort = adminPortValues->GetCount() > i ? MgUtil::StringToInt32(adminPortValues->GetItem(i)) : defaultAdminPort;

        STRING targetIP;
        MgIpUtil::HostNameToAddress(targetValues->GetItem(i), targetIP);
        Ptr<MgSiteInfo> siteInfo = new MgSiteInfo(targetIP,
            sitePort, clientPort, adminPort);
        m_sites.push_back(SAFE_ADDREF((MgSiteInfo*)siteInfo));
    }

    MG_CATCH_AND_THROW(L"MgSiteManager.Initialize")
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves connection properties for a site server. If useSessionIP is true
/// and the userInfo contains a session ID, the connection properties will be
/// generated for the site that contains that session.
/// </summary>
///----------------------------------------------------------------------------

MgConnectionProperties* MgSiteManager::GetConnectionProperties(MgUserInformation* userInfo, 
                                                               MgSiteInfo::MgPortType type,
                                                               bool useSessionIP)
{
    MgConnectionProperties* connProps = NULL;
    
    // Determine if the user info contains a session ID
    STRING sessionId = userInfo->GetMgSessionId();
    if(useSessionIP && !sessionId.empty())
    {
        size_t length = sessionId.length();
        if(length > MgSiteInfo::HexStringLength)
        {
            STRING siteHexString = sessionId.substr(length - MgSiteInfo::HexStringLength, MgSiteInfo::HexStringLength);
            Ptr<MgSiteInfo> siteInfo = new MgSiteInfo(siteHexString);
            if(siteInfo->GetStatus() == MgSiteInfo::Ok)
            {
                connProps = GetConnectionProperties(userInfo, siteInfo, type);
            }
        }
    }
    else
    {
        Ptr<MgSiteInfo> nextSite = GetNextSite();
        if(nextSite != NULL)
        {
            connProps = GetConnectionProperties(userInfo, nextSite, type);
        }
    }

    return connProps;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves connection properties for the site specified in the siteInfo object
/// </summary>
///----------------------------------------------------------------------------
MgConnectionProperties* MgSiteManager::GetConnectionProperties(MgUserInformation* userInfo, 
        MgSiteInfo* siteInfo,
        MgSiteInfo::MgPortType type)
{
    MgConnectionProperties* connProps = NULL;
    if(siteInfo != NULL)
    {
        connProps = new MgConnectionProperties(userInfo, siteInfo->GetTarget(), siteInfo->GetPort(type));
    }
    return connProps;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves connection properties for a support server. If useSessionIP is true
/// and the userInfo contains a session ID, the connection properties will be
/// generated for the site that contains that session.
/// </summary>
///----------------------------------------------------------------------------
MgConnectionProperties* MgSiteManager::GetSupportServerConnectionProperties(CREFSTRING supportServer,
                                                               MgUserInformation* userInfo, 
                                                               MgSiteInfo::MgPortType type)
{
    MgConnectionProperties* supportConnProps = NULL; 
    
    // Session Affinity: Retrieve the site server corresponding to this user info
    MgConnectionProperties* siteConnProps = GetConnectionProperties(userInfo, type, true);
    if(siteConnProps != NULL)
    {
        // Create support server connection props using the specified IP target and the same admin port
        // used by the site server
        supportConnProps = new MgConnectionProperties(userInfo, supportServer, siteConnProps->GetPort());
    }
    return supportConnProps;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves site info for the next site server that has an Ok status.
/// </summary>
///----------------------------------------------------------------------------
MgSiteInfo* MgSiteManager::GetNextSite()
{
    MgSiteInfo* nextSite = NULL;
    
    int numSites = (int)m_sites.size();
    for(int siteIndex = 0; siteIndex < numSites; siteIndex++)
    {
        int siteToTry = (m_index + siteIndex) % numSites;
        MgSiteInfo* siteInfo = m_sites.at(siteToTry);
        if(siteInfo->GetStatus() == MgSiteInfo::Ok)
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
    return (INT32)m_sites.size();
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves site info for the site server corresponding to the specified index
/// </summary>
///----------------------------------------------------------------------------
MgSiteInfo* MgSiteManager::GetSiteInfo(INT32 index)
{
    MgSiteInfo* siteInfo = NULL;
    if((UINT32)index < m_sites.size())
    {
        siteInfo = m_sites.at(index);
    }
    return SAFE_ADDREF(siteInfo);
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves site info for the site server matching the specified target and port
/// </summary>
///----------------------------------------------------------------------------
MgSiteInfo* MgSiteManager::GetSiteInfo(STRING target, INT32 port)
{
    Ptr<MgSiteInfo> matchingSiteInfo;
    matchingSiteInfo = NULL;

    for(int i = 0; i < GetSiteCount(); i++)
    {
        Ptr<MgSiteInfo> siteInfo = GetSiteInfo(i);
        if(siteInfo->GetTarget() == target)
        {
            INT32 sitePort = siteInfo->GetPort(MgSiteInfo::Site);
            INT32 clientPort = siteInfo->GetPort(MgSiteInfo::Client);
            INT32 adminPort = siteInfo->GetPort(MgSiteInfo::Admin);
            if(port == sitePort || port == clientPort || port == adminPort)
            {
                matchingSiteInfo = siteInfo;
                break;
            }
        }
    }

    return SAFE_ADDREF((MgSiteInfo*)matchingSiteInfo);
}
