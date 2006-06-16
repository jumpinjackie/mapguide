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
    m_localConnProp = new MgConnectionProperties(L"", 0);
    assert(m_localConnProp != NULL);
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

    // The tear down order is the reverse of the initialization order.

    if (m_tileService != NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Releasing Tile Service. Reference Count: %d\n"), m_tileService->GetRefCount()));
        assert(1 == m_tileService->GetRefCount());
        m_tileService = NULL;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Tile Service released.\n")));
    }

    if (m_renderingService != NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Releasing Rendering Service. Reference Count: %d\n"), m_renderingService->GetRefCount()));
        assert(1 == m_renderingService->GetRefCount());
        m_renderingService = NULL;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Rendering Service released.\n")));
    }

    if (m_mappingService != NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Releasing Mapping Service. Reference Count: %d\n"), m_mappingService->GetRefCount()));
        assert(1 == m_mappingService->GetRefCount());
        m_mappingService = NULL;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Mapping Service released.\n")));
    }

    if (m_featureService != NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Releasing Feature Service. Reference Count: %d\n"), m_featureService->GetRefCount()));
        assert(1 == m_featureService->GetRefCount());
        m_featureService = NULL;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Feature Service released.\n")));
    }

    if (m_drawingService != NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Releasing Drawing Service. Reference Count: %d\n"), m_drawingService->GetRefCount()));
        assert(1 == m_drawingService->GetRefCount());
        m_drawingService = NULL;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Drawing Service released.\n")));
    }

    if (m_siteService != NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Releasing Site Service. Reference Count: %d\n"), m_siteService->GetRefCount()));
        assert(1 == m_siteService->GetRefCount());
        m_siteService = NULL;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Site Service released.\n")));
    }

    if (m_resourceService != NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Releasing Resource Service. Reference Count: %d\n"), m_resourceService->GetRefCount()));
        assert(1 == m_resourceService->GetRefCount());
        m_resourceService = NULL;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Resource Service released.\n")));
    }

    if (m_adminService != NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Releasing Server Admin Service. Reference Count: %d\n"), m_adminService->GetRefCount()));
        assert(1 == m_adminService->GetRefCount());
        m_adminService = NULL;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Server Admin Service released.\n")));
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

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::Initialize() - Creating services.\n")));
    MG_LOG_TRACE_ENTRY(L"MgServiceManager::Initialize()");

    // Use a dummy credentials to create local services.
    Ptr<MgUserInformation> userInfo = new MgUserInformation();
    MgUserInformation::SetCurrentUserInfo(userInfo);

    // Every server must host the internal Server Admin Service.
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::Initialize() - Creating Server Admin Service.\n")));
    m_adminService = dynamic_cast<MgServerAdminService*>(
        MgSiteConnection::CreateService(MgServiceType::ServerAdminService, m_localConnProp));
    assert(m_adminService != NULL);

    // Resource Service and Site Service must be created by the site server.
    // Other services should not be created at startup because some support
    // server may not be currently running. Therefore, these can only be
    // created/cached at runtime.
    if (m_serverManager->IsSiteServer())
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::Initialize() - Creating Resource Service.\n")));
        m_resourceService = dynamic_cast<MgServerResourceService*>(
            MgSiteConnection::CreateService(MgServiceType::ResourceService, m_localConnProp));
        assert(m_resourceService != NULL);

        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::Initialize() - Creating Site Service.\n")));
        m_siteService = dynamic_cast<MgServerSiteService*>(
            MgSiteConnection::CreateService(MgServiceType::SiteService, m_localConnProp));
        assert(m_siteService != NULL);
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
    MgService* localService = NULL;

    if (MgServiceType::ResourceService == serviceType)
    {
        if (m_resourceService == NULL)
        {
            // Suppport server is hosting Resource Service? This shouldn't happen.
            throw new MgLogicException(
                L"MgServiceManager.RequestLocalService",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        localService = m_resourceService.p;
    }
    else if (MgServiceType::SiteService == serviceType)
    {
        if (m_siteService == NULL)
        {
            // Suppport server is hosting Site Service? This shouldn't happen.
            throw new MgLogicException(
                L"MgServiceManager.RequestLocalService",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        localService = m_siteService.p;
    }
    else if (MgServiceType::DrawingService == serviceType)
    {
        if (m_drawingService == NULL)
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::RequestLocalService() - Creating Drawing Service.\n")));
            m_drawingService = dynamic_cast<MgServerDrawingService*>(
                MgSiteConnection::CreateService(MgServiceType::DrawingService, m_localConnProp));
        }

        localService = m_drawingService.p;
    }
    else if (MgServiceType::FeatureService == serviceType)
    {
        if (m_featureService == NULL)
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::RequestLocalService() - Creating Feature Service.\n")));
            m_featureService = dynamic_cast<MgServerFeatureService*>(
                MgSiteConnection::CreateService(MgServiceType::FeatureService, m_localConnProp));
        }

        localService = m_featureService.p;
    }
    else if (MgServiceType::MappingService == serviceType)
    {
        if (m_mappingService == NULL)
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::RequestLocalService() - Creating Mapping Service.\n")));
            m_mappingService = dynamic_cast<MgServerMappingService*>(
                MgSiteConnection::CreateService(MgServiceType::MappingService, m_localConnProp));
        }

        localService = m_mappingService.p;
    }
    else if (MgServiceType::RenderingService == serviceType)
    {
        if (m_renderingService == NULL)
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::RequestLocalService() - Creating Rendering Service.\n")));
            m_renderingService = dynamic_cast<MgServerRenderingService*>(
                MgSiteConnection::CreateService(MgServiceType::RenderingService, m_localConnProp));
        }

        localService = m_renderingService.p;
    }
    else if (MgServiceType::TileService == serviceType)
    {
        if (m_tileService == NULL)
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgServiceManager::RequestLocalService() - Creating Tile Service.\n")));
            m_tileService = dynamic_cast<MgServerTileService*>(
                MgSiteConnection::CreateService(MgServiceType::TileService, m_localConnProp));
        }

        localService = m_tileService.p;
    }
    else if (MgServiceType::ServerAdminService == serviceType)
    {
        if (m_adminService == NULL)
        {
            // Admin Service was already created at startup. This shouldn't happen.
            throw new MgNullReferenceException(
                L"MgServiceManager.RequestLocalService",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        localService = m_adminService.p;
    }

    if (NULL == localService)
    {
        throw new MgServiceNotSupportedException(
            L"MgServiceManager.RequestLocalService",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return SAFE_ADDREF(localService);
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
    Ptr<MgConnectionProperties> connProp = new MgConnectionProperties(
        serverAddress, m_serverManager->GetSitePort());

    proxyService = MgSiteConnection::CreateService(serviceType, connProp);

    MG_CATCH(L"MgServiceManager.RequestProxyService")

    if (mgException != NULL)
    {
        assert(NULL == proxyService);

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
    assert(m_resourceService != NULL);
    Ptr<MgStringCollection> undeletedSessions = new MgStringCollection;

    if (m_resourceService != NULL)
    {
        MgResourceIdentifier resource;

        resource.SetRepositoryType(MgRepositoryType::Session);
        resource.SetResourceType(MgResourceType::Folder);

        for (INT32 i = 0; i < expiredSessions->GetCount(); ++i)
        {
            CREFSTRING session = expiredSessions->GetItem(i);
            resource.SetRepositoryName(session);

            MG_TRY()

            m_resourceService->DeleteRepository(&resource);

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
    assert(m_resourceService != NULL);

    if (m_resourceService != NULL)
    {
        m_resourceService->PerformRepositoryCheckpoints();
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
    if (m_tileService == NULL && 
        m_loadBalanceManager->m_localServerInfo->IsServiceEnabled(
            MgServiceType::TileService))
    {
        m_tileService = dynamic_cast<MgServerTileService*>(
            MgSiteConnection::CreateService(MgServiceType::TileService, m_localConnProp));
    }

    if (m_tileService != NULL)
    {
        m_tileService->NotifyResourcesChanged(resources);
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

    MG_TRY()

    ACE_ASSERT(m_serverManager->IsSiteServer());

    if (m_resourceService == NULL)
    {
        throw new MgServiceNotAvailableException(
            L"MgServiceManager.DispatchResourceChangeNotifications",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    changedResources = m_resourceService->GetChangedResources();

    if (changedResources != NULL)
    {
        // Use the Administrator account to collect all the map definitions
        // whose child resources have been changed.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(
            MgUser::Administrator, L"");

        MgUserInformation::SetCurrentUserInfo(adminUserInfo);
        Ptr<MgSerializableCollection> changedMaps = 
            m_resourceService->EnumerateParentMapDefinitions(changedResources);
        MgUserInformation::SetCurrentUserInfo(NULL);

        m_loadBalanceManager->DispatchResourceChangeNotifications(changedMaps);
    }

    MG_CATCH(L"MgServiceManager.DispatchResourceChangeNotifications")

    // If an error occurred (e.g. due to deadlocks?), then restore the changed
    // resources so that they will be retried in the next Resource Change
    // Notification event.
    if (mgException != NULL)
    {
        m_resourceService->UpdateChangedResources(changedResources);
    }

    MG_THROW()
}
