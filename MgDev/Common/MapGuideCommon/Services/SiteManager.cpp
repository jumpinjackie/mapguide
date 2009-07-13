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
            Ptr<MgSiteInfo> siteInfo = new MgSiteInfo(siteHexString);

            if (MgSiteInfo::Ok == siteInfo->GetStatus())
            {
                connProps = GetConnectionProperties(userInfo, siteInfo, portType);
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
        throw new MgLogicException(
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

    MgSiteInfo* matchingSiteInfo = NULL;

    for (INT32 i = 0; i < (INT32)m_sites.size(); i++)
    {
        MgSiteInfo* siteInfo = m_sites.at(i);

        if (siteInfo->GetTarget() == target
            && (siteInfo->GetPort(MgSiteInfo::Site)   == port ||
                siteInfo->GetPort(MgSiteInfo::Client) == port ||
                siteInfo->GetPort(MgSiteInfo::Admin)  == port))
        {
            matchingSiteInfo = siteInfo;
            break;
        }
    }

    return SAFE_ADDREF(matchingSiteInfo);
}
