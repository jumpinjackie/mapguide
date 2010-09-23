//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "ServiceManager.h"
#include "ServerDrawingService.h"
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

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgServiceManager::~MgServiceManager()\n")));

    // Close the repositories on the Site server.
    if (m_resourceService != NULL)
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Releasing Resource Service. Reference Count: %d\n"), m_resourceService->GetRefCount()));

        m_resourceService->CloseRepositories();
        assert(1 == m_resourceService->GetRefCount());
        m_resourceService = NULL;

        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Resource Service released.\n")));
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

    if (NULL == MgServiceManager::sm_serviceManager.p)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance(), NULL));

        if (NULL == MgServiceManager::sm_serviceManager.p)
        {
            MgServiceManager::sm_serviceManager = new MgServiceManager();
        }
    }

    MG_CATCH_AND_THROW(L"MgServiceManager.GetInstance")

    ACE_ASSERT(NULL != MgServiceManager::sm_serviceManager.p);

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

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgServiceManager::Initialize()\n")));
    MG_LOG_TRACE_ENTRY(L"MgServiceManager::Initialize()");

    // Open the repositories on the Site server.
    if (m_serverManager->IsSiteServer())
    {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgServiceManager::Initialize() - Creating Resource Service.\n")));

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
    Ptr<MgUserInformation> currUserInfo = MgUserInformation::GetCurrentUserInfo();
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
    Ptr<MgUserInformation> currUserInfo = MgUserInformation::GetCurrentUserInfo();
    Ptr<MgConnectionProperties> connProp = new MgConnectionProperties(
        currUserInfo, serverAddress, m_serverManager->GetSitePort());

    proxyService = MgSiteConnection::CreateService(serviceType, connProp);

    MG_CATCH(L"MgServiceManager.RequestProxyService")

    if (mgException != NULL)
    {
        if (mgException->IsOfClass(MapGuide_Exception_MgConnectionFailedException)
         || mgException->IsOfClass(MapGuide_Exception_MgConnectionNotOpenException))
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
                && !mgException->IsOfClass(PlatformBase_Exception_MgResourceNotFoundException)
                && !mgException->IsOfClass(MapGuide_Exception_MgRepositoryNotFoundException))
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

        if (!(serviceFlags & MgServiceFlag::SiteService)
         || !(serviceFlags & MgServiceFlag::ResourceService))
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

        if ((serviceFlags & MgServiceFlag::SiteService)
         || (serviceFlags & MgServiceFlag::ResourceService))
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
/// Notify the feature service of the resources changed.
///
bool MgServiceManager::NotifyFeatureServiceOnResourcesChanged(
    const set<STRING>& resources, bool strict)
{
    bool success = true;

    if (!resources.empty())
    {
        Ptr<MgService> service = RequestLocalService(MgServiceType::FeatureService);
        MgServerFeatureService* featureService = dynamic_cast<MgServerFeatureService*>(service.p);
        ACE_ASSERT(NULL != featureService);

        if (NULL != featureService)
        {
            success = featureService->NotifyResourcesChanged(resources, strict);
        }
    }

    return success;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Notify the feature service of the resources changed.
///
bool MgServiceManager::NotifyFeatureServiceOnResourcesChanged(
    MgSerializableCollection* resources, bool strict)
{
    bool success = true;

    if (NULL != resources && resources->GetCount() > 0)
    {
        Ptr<MgService> service = RequestLocalService(MgServiceType::FeatureService);
        MgServerFeatureService* featureService = dynamic_cast<MgServerFeatureService*>(service.p);
        ACE_ASSERT(NULL != featureService);

        if (NULL != featureService)
        {
            success = featureService->NotifyResourcesChanged(resources, strict);
        }
    }

    return success;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Notify the tile service of the resources changed.
///
bool MgServiceManager::NotifyTileServiceOnResourcesChanged(
    MgSerializableCollection* resources, bool strict)
{
    bool success = true;

    if (NULL != resources && resources->GetCount() > 0)
    {
        Ptr<MgService> service = RequestLocalService(MgServiceType::TileService);
        MgServerTileService* tileService = dynamic_cast<MgServerTileService*>(service.p);
        ACE_ASSERT(NULL != tileService);

        if (NULL != tileService)
        {
            success = tileService->NotifyResourcesChanged(resources, strict);
        }
    }

    return success;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Notify the applicable services of the resources changed.
///
void MgServiceManager::NotifyResourcesChanged(MgSerializableCollection* resources)
{
    NotifyFeatureServiceOnResourcesChanged(resources, false);
    NotifyTileServiceOnResourcesChanged(resources, false);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Dispatch resource change notifications to applicable servers.
///
void MgServiceManager::DispatchResourceChangeNotifications()
{
    // For Site server, only changed map definitions will be notified.
    // For Support servers, both changed map definitions and feature
    // sources will be notified.

    Ptr<MgSerializableCollection> changedResources;
    Ptr<MgServerResourceService> resourceService;

    MG_TRY()

    if (m_serverManager->IsSiteServer())
    {
        resourceService = dynamic_cast<MgServerResourceService*>(
            RequestLocalService(MgServiceType::ResourceService));
        ACE_ASSERT(NULL != resourceService.p);
    }

    if (NULL != resourceService.p)
    {
        changedResources = resourceService->GetChangedResources();

        if (NULL != changedResources.p && changedResources->GetCount() > 0)
        {
            // Notify the tile service of the resources changed.

            Ptr<MgSerializableCollection> affectedResources;
            Ptr<MgService> service = RequestLocalService(MgServiceType::TileService);
            MgServerTileService* tileService = dynamic_cast<MgServerTileService*>(service.p);
            ACE_ASSERT(NULL != tileService);

            if (NULL != tileService && !tileService->IsTileCacheEmpty())
            {
                affectedResources = resourceService->EnumerateParentMapDefinitions(changedResources);
                tileService->NotifyResourcesChanged(affectedResources, false);
            }

            // Dispatch resource change notifications to support servers.

            INT32 serviceFlags = (MgServiceFlag::FeatureService | MgServiceFlag::TileService);
            Ptr<MgStringCollection> serverAddresses =
                m_loadBalanceManager->GetServerAddresses(serviceFlags, false, true);

            if (NULL != serverAddresses.p && serverAddresses->GetCount() > 0)
            {
                if (NULL == affectedResources.p)
                {
                    affectedResources = new MgSerializableCollection();
                }

                INT32 numResources = changedResources->GetCount();

                for (INT32 i = 0; i < numResources; ++i)
                {
                    Ptr<MgSerializable> serializableObj = changedResources->GetItem(i);
                    MgResourceIdentifier* resource = dynamic_cast<MgResourceIdentifier*>(
                        serializableObj.p);
                    ACE_ASSERT(NULL != resource);

                    if (NULL != resource && resource->IsResourceTypeOf(MgResourceType::FeatureSource))
                    {
                        affectedResources->Add(resource);
                    }
                }

                m_loadBalanceManager->DispatchResourceChangeNotifications(
                    serverAddresses, affectedResources);
            }
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
