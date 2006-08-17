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

#include "ServiceManager.h"
#include "ResourceService.h"
#include "ServerAdminService.h"
#include "ServerResourceService.h"
#include "ServerSiteService.h"
#include "ServerDrawingService.h"
#include "ServerFeatureService.h"
#include "ServerMappingService.h"
#include "ServerRenderingService.h"
#include "ServerTileService.h"
#include "ServerKmlService.h"

// Process-wide MgServiceManager
Ptr<MgServiceManager> MgServiceManager::sm_serviceManager = (MgServiceManager*)NULL;

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgServiceManager::MgServiceManager()
{
    m_serverManager = MgServerManager::GetInstance();
    assert(NULL != m_serverManager);
    m_loadBalanceManager = MgLoadBalanceManager::GetInstance();
    assert(NULL != m_loadBalanceManager);
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgServiceManager::~MgServiceManager()
{
    MG_TRY()

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::~MgServiceManager()\n")));

    // Close the repositories on the Site server.
    if (m_resourceService != NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Releasing Resource Service. Reference Count: %d\n"), m_resourceService->GetRefCount()));

        m_resourceService->CloseRepositories();
        assert(1 == m_resourceService->GetRefCount());
        m_resourceService = NULL;

        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Resource Service released.\n")));
    }

    MG_CATCH(L"MgServiceManager.~MgServiceManager")
}

///----------------------------------------------------------------------------
/// <summary>
/// Disposes the object.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceManager::Dispose()
{
    delete this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the pointer to a process-wide MgServiceManager.
/// </summary>
///----------------------------------------------------------------------------

MgServiceManager* MgServiceManager::GetInstance()
{
    MG_TRY()

    ACE_TRACE("MgServiceManager::GetInstance");

    if (MgServiceManager::sm_serviceManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance(), NULL));

        if (MgServiceManager::sm_serviceManager == NULL)
        {
            MgServiceManager::sm_serviceManager = new MgServiceManager();
        }
    }

    MG_CATCH_AND_THROW(L"MgServiceManager.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgServiceManager::sm_serviceManager;
}

///----------------------------------------------------------------------------
/// <summary>
/// Intializes the service manager.
/// This method must be called once during the server startup time.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceManager::Initialize()
{
    MG_TRY()

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::Initialize()\n")));
    MG_LOG_TRACE_ENTRY(L"MgServiceManager::Initialize()");

    // Open the repositories on the Site server.
    if (m_serverManager->IsSiteServer())
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::Initialize() - Creating Resource Service.\n")));

        Ptr<MgUserInformation> userInfo = new MgUserInformation(
            MgUser::Administrator, L"");

        MgUserInformation::SetCurrentUserInfo(userInfo);

        m_resourceService = dynamic_cast<MgServerResourceService*>(
            RequestLocalService(MgServiceType::ResourceService));
        assert(m_resourceService != NULL);
        m_resourceService->OpenRepositories();

        MgUserInformation::SetCurrentUserInfo(NULL);
    }

    MG_CATCH_AND_THROW(L"MgServiceManager.Initialize")
}

///----------------------------------------------------------------------------
/// <summary>
/// Cleans up the service manager.
/// This method must be called once during the server shutdown time.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceManager::Terminate()
{
    MgUserInformation::SetCurrentUserInfo(NULL);
    MgServiceManager::sm_serviceManager = NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return a local service of the specified type.
/// The local service will be created if it has not been created.
///
MgService* MgServiceManager::RequestLocalService(INT32 serviceType)
{
    MgUserInformation* currUserInfo = MgUserInformation::GetCurrentUserInfo();
    Ptr<MgConnectionProperties> connProp = new MgConnectionProperties(
        currUserInfo, L"", 0);

    return MgSiteConnection::CreateService(serviceType, connProp);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Create a proxy service of the specified type.
///
MgService* MgServiceManager::RequestProxyService(INT32 serviceType)
{
    MgService* proxyService = NULL;
    STRING serverAddress;

    MG_TRY()

    serverAddress = m_loadBalanceManager->RequestServer(serviceType);

    // Use the site port for Site/Support Server communications.
    MgUserInformation* currUserInfo = MgUserInformation::GetCurrentUserInfo();
    Ptr<MgConnectionProperties> connProp = new MgConnectionProperties(
        currUserInfo, serverAddress, m_serverManager->GetSitePort());

    proxyService = MgSiteConnection::CreateService(serviceType, connProp);

    MG_CATCH(L"MgServiceManager.RequestProxyService")

    if (mgException != NULL)
    {
        if (mgException->IsOfClass(Common_Exception_MgConnectionFailedException)
         || mgException->IsOfClass(Common_Exception_MgConnectionNotOpenException))
        {
            // Remove the server from the queues. Server is down?
            for (INT32 i = 0; i < MgServerInformation::sm_knMaxNumberServices; ++i)
            {
                m_loadBalanceManager->RemoveServerFromQueue(i, serverAddress);
            }
        }
        else
        {
            MG_THROW()
        }
    }

    return proxyService;
}

///----------------------------------------------------------------------------
/// <summary>
/// Cleans up expired session repositories. Undeleted ones will be returned
/// so that they will be deleted by the next session cleanup event timer.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgServiceManager::CleanUpRepositories(
    MgStringCollection* expiredSessions)
{
    Ptr<MgStringCollection> undeletedSessions = new MgStringCollection;
    Ptr<MgServerResourceService> resourceService;

    if (m_serverManager->IsSiteServer())
    {
        resourceService = dynamic_cast<MgServerResourceService*>(
            RequestLocalService(MgServiceType::ResourceService));
    }

    if (resourceService != NULL)
    {
        MgResourceIdentifier resource;

        resource.SetRepositoryType(MgRepositoryType::Session);
        resource.SetResourceType(MgResourceType::Folder);

        for (INT32 i = 0; i < expiredSessions->GetCount(); ++i)
        {
            CREFSTRING session = expiredSessions->GetItem(i);
            resource.SetRepositoryName(session);

            MG_TRY()

            resourceService->DeleteRepository(&resource);

            MG_CATCH(L"MgServiceManager.CleanUpRepositories")

            if (mgException != NULL // dead locks?
                && !mgException->IsOfClass(Common_Exception_MgResourceNotFoundException)
                && !mgException->IsOfClass(Common_Exception_MgRepositoryNotFoundException))
            {
                undeletedSessions->Add(session);
            }
        }
    }

    return undeletedSessions.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Performs checkpoints for all the repositories.
/// </summary>
///----------------------------------------------------------------------------

void MgServiceManager::PerformRepositoryCheckpoints()
{
    Ptr<MgServerResourceService> resourceService;

    if (m_serverManager->IsSiteServer())
    {
        resourceService = dynamic_cast<MgServerResourceService*>(
            RequestLocalService(MgServiceType::ResourceService));
    }

    if (resourceService != NULL)
    {
        resourceService->PerformRepositoryCheckpoints();
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Enables and/or disables the specified services on the server.
/// </summary>
///
/// <param name="hostProps">
/// Property collection containing the list of hosting services on the server.
/// </param>
///
/// <returns>
/// Nothing
/// </returns>
///----------------------------------------------------------------------------

void MgServiceManager::EnableServices(MgPropertyCollection* hostProps)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_loadBalanceManager->sm_mutex));

    MG_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServiceManager::EnableServices()");

    INT32 serviceFlags = MgServerInformation::ToServiceFlags(hostProps,
        m_loadBalanceManager->m_localServerInfo->GetServiceFlags());

    // Ensure Site and Resource services are always enabled for the Site server.

    if (m_serverManager->IsSiteServer())
    {
        assert(m_loadBalanceManager->m_localServerInfo->IsSiteServer());

        if (!(serviceFlags & MgServerInformation::ToServiceFlag(
            MgServiceType::SiteService)) ||
            !(serviceFlags & MgServerInformation::ToServiceFlag(
            MgServiceType::ResourceService)))
        {
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(L"MgPropertyCollection");

            throw new MgInvalidArgumentException(L"MgServiceManager.EnableServices",
                __LINE__, __WFILE__, &arguments, L"MgUnsupportedService", NULL);
        }
    }
    else
    {
        assert(!m_loadBalanceManager->m_localServerInfo->IsSiteServer());

        if ((serviceFlags & MgServerInformation::ToServiceFlag(
            MgServiceType::SiteService)) ||
            (serviceFlags & MgServerInformation::ToServiceFlag(
            MgServiceType::ResourceService)))
        {
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(L"MgPropertyCollection");

            throw new MgInvalidArgumentException(L"MgServiceManager.EnableServices",
                __LINE__, __WFILE__, &arguments, L"MgUnsupportedService", NULL);
        }
    }

    m_loadBalanceManager->EnableServices(serviceFlags);

    MG_CATCH_AND_THROW(L"MgServiceManager.EnableServices")
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the next available service.
/// </summary>
///----------------------------------------------------------------------------

MgService* MgServiceManager::RequestService(INT32 serviceType)
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_loadBalanceManager->sm_mutex, NULL));

    MG_CHECK_RANGE(serviceType, 0, MgServerInformation::sm_knMaxNumberServices - 1,
        L"MgServiceManager.RequestService");

    // Return a local service if it is available.
    // Otherwise, return a proxy service from a support server if possible.
    MgService* service;

    if (m_loadBalanceManager->m_localServerInfo->IsServiceEnabled(serviceType))
    {
        service = RequestLocalService(serviceType);
    }
    else
    {
        do
        {
            service = RequestProxyService(serviceType);
        } while (NULL == service); // or until an exception is thrown
    }

    return service;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Notify the tile service of the resources changed.
///
void MgServiceManager::NotifyResourcesChanged(MgSerializableCollection* resources)
{
    if (m_loadBalanceManager->m_localServerInfo->IsServiceEnabled(
        MgServiceType::TileService))
    {
        Ptr<MgServerTileService> tileService = dynamic_cast<MgServerTileService*>(
            RequestLocalService(MgServiceType::TileService));

        if (tileService != NULL)
        {
            tileService->NotifyResourcesChanged(resources);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Dispatch resource change notifications to all the servers that host the
/// tile service.
///
void MgServiceManager::DispatchResourceChangeNotifications()
{
    Ptr<MgSerializableCollection> changedResources;
    Ptr<MgServerResourceService> resourceService;

    MG_TRY()

    if (m_serverManager->IsSiteServer())
    {
        resourceService = dynamic_cast<MgServerResourceService*>(
            RequestLocalService(MgServiceType::ResourceService));
    }

    if (resourceService != NULL)
    {
        changedResources = resourceService->GetChangedResources();

        if (changedResources != NULL)
        {
            Ptr<MgSerializableCollection> changedMaps = 
                resourceService->EnumerateParentMapDefinitions(changedResources);

            m_loadBalanceManager->DispatchResourceChangeNotifications(changedMaps);
        }
    }

    MG_CATCH(L"MgServiceManager.DispatchResourceChangeNotifications")

    // If an error occurred (e.g. due to deadlocks?), then restore the changed
    // resources so that they will be retried in the next Resource Change
    // Notification event.
    if (mgException != NULL && resourceService != NULL)
    {
        resourceService->UpdateChangedResources(changedResources);
    }

    MG_THROW()
}
