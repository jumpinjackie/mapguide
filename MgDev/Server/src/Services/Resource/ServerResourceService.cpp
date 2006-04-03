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

#include "ResourceServiceDefs.h"
#include "ServerResourceService.h"
#include "PermissionManager.h"
#include "LoadBalanceManager.h"
#include "LibraryRepository.h"
#include "SessionRepository.h"
#include "SiteRepository.h"
#include "LibraryRepositoryManager.h"
#include "SessionRepositoryManager.h"
#include "SiteRepositoryManager.h"
#include "SiteResourceContentManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgServerResourceService::MgServerResourceService(
    MgConnectionProperties* connection) :
    MgResourceService(connection),
    m_loadBalanceMan(NULL),
    m_siteRepository(NULL),
    m_sessionRepository(NULL),
    m_libraryRepository(NULL)
{
    MG_RESOURCE_SERVICE_TRY()

    m_loadBalanceMan = MgLoadBalanceManager::GetInstance();
    ACE_ASSERT(NULL != m_loadBalanceMan);

    // Clean up the Session repository after creating the Site repository
    // to eliminate unnecessary XMLPlatformUtils::Initialize/Terminate calls
    // made by the BDB XmlManager.

    m_siteRepository = new MgSiteRepository();
    MgRepositoryManager::CleanRepository(MgRepositoryType::Session);
    m_sessionRepository = new MgSessionRepository();
    m_libraryRepository = new MgLibraryRepository();

    // TODO: Remove the following code when the installer is ready to install
    // default repositories?
    // Create a site repository if it does not exists.

    MgSiteRepositoryManager siteRepositoryMan(*m_siteRepository);
    MgResourceIdentifier resource;

    resource.SetRepositoryType(MgRepositoryType::Site);
    resource.SetResourceType(MgResourceType::Folder);

    if (!siteRepositoryMan.FindResource(&resource))
    {
        siteRepositoryMan.CreateRepository(&resource, 0, 0);
        siteRepositoryMan.CommitTransaction();
    }

    // Create a library repository if it does not exists.

    MgLibraryRepositoryManager libraryRepositoryMan(*m_libraryRepository, true);

    resource.SetRepositoryType(MgRepositoryType::Library);

    if (!libraryRepositoryMan.FindResource(&resource))
    {
        libraryRepositoryMan.CreateRepository(&resource, 0, 0);
        libraryRepositoryMan.CommitTransaction();
    }

    // Create the permission cache.

    MgPermissionManager::RefreshPermissionCache(CreatePermissionCache(true));

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.MgServerResourceService")
}

///----------------------------------------------------------------------------
/// <summary>
/// Default constructor. Need this to prevent a GCC 3 compile error.
/// </summary>
///----------------------------------------------------------------------------

MgServerResourceService::MgServerResourceService() : MgResourceService(NULL)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgServerResourceService::~MgServerResourceService()
{
    MG_TRY()

    PerformRepositoryCheckpoints(DB_FORCE);

    MG_CATCH(L"MgServerResourceService.~MgServerResourceService")

    delete m_siteRepository;
    delete m_sessionRepository;
    delete m_libraryRepository;
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates a list of entries in the specified repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateRepositories(
    CREFSTRING repositoryType)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateRepositories()");

    if (MgRepositoryType::Session != repositoryType)
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.EnumerateRepositories",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgSessionRepositoryManager repositoryMan(*m_sessionRepository);

    byteReader = repositoryMan.EnumerateRepositories();

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateRepositories")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a repository with the given content and header.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::CreateRepository(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::CreateRepository()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.CreateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Session))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.CreateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(!resource->GetRepositoryName().empty());
    MgSessionRepositoryManager repositoryMan(*m_sessionRepository);

    repositoryMan.CreateRepository(resource, content, header);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CreateRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes the specified repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::DeleteRepository(MgResourceIdentifier* resource)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::DeleteRepository()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.DeleteRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Session))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.DeleteRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(!resource->GetRepositoryName().empty());
    MgSessionRepositoryManager repositoryMan(*m_sessionRepository);

    repositoryMan.DeleteRepository(resource);

    repositoryMan.CommitTransaction();

    // Dispatch resource change notifications.

    Ptr<MgSerializableCollection> changedResources = repositoryMan.GetChangedResources();

    m_loadBalanceMan->DispatchResourceChangeNotifications(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates an existing repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::UpdateRepository(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::UpdateRepository()");

    if (NULL == resource || (NULL == content && NULL == header))
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.UpdateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    repositoryMan->UpdateRepository(resource, content, header);

    repositoryMan->CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.UpdateRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the content of the specified repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::GetRepositoryContent(
    MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GetRepositoryContent()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetRepositoryContent",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    byteReader = repositoryMan->GetRepositoryContent(resource);

    repositoryMan->CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetRepositoryContent")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the header associated with the specified repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::GetRepositoryHeader(
    MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GetRepositoryHeader()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetRepositoryHeader",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.GetRepositoryHeader",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    MgLibraryRepositoryManager repositoryMan(*m_libraryRepository);

    byteReader = repositoryMan.GetRepositoryHeader(resource);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetRepositoryHeader")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Applies a package of resource changes to the repository.
///
void MgServerResourceService::ApplyResourcePackage(MgByteReader* packageStream)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::ApplyResourcePackage()");

    if (NULL == packageStream)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.ApplyResourcePackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLibraryRepositoryManager repositoryMan(*m_libraryRepository);

    repositoryMan.ApplyResourcePackage(packageStream);

    repositoryMan.CommitTransaction();

    // Dispatch resource change notifications.

    Ptr<MgSerializableCollection> changedResources = repositoryMan.GetChangedResources();

    m_loadBalanceMan->DispatchResourceChangeNotifications(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.ApplyResourcePackage")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Loads the specified resource package into the repository.
///
void MgServerResourceService::LoadResourcePackage(CREFSTRING packagePathname, 
    bool logActivities)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::LoadResourcePackage()");

    if (!MgFileUtil::PathnameExists(packagePathname))
    {
        MgStringCollection arguments;
        arguments.Add(packagePathname);

        throw new MgFileNotFoundException(
            L"MgServerResourceService.LoadResourcePackage", 
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MgLibraryRepositoryManager repositoryMan(*m_libraryRepository);

    repositoryMan.LoadResourcePackage(packagePathname, logActivities);

    repositoryMan.CommitTransaction();

    // Dispatch resource change notifications.

    Ptr<MgSerializableCollection> changedResources = repositoryMan.GetChangedResources();

    m_loadBalanceMan->DispatchResourceChangeNotifications(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.LoadResourcePackage")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a package from the specified resource, and then saves it into 
/// the specified name.
///
void MgServerResourceService::MakeResourcePackage(MgResourceIdentifier* resource, 
    CREFSTRING packagePathname, CREFSTRING packageDescription, bool logActivities)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::MakeResourcePackage()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.ApplyResourcePackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsFolder())
    {
        throw new MgInvalidResourceTypeException(
            L"MgServerResourceService.MakeResourcePackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.MakeResourcePackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    MgLibraryRepositoryManager repositoryMan(*m_libraryRepository);

    repositoryMan.MakeResourcePackage(resource, packagePathname, 
        packageDescription, logActivities);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.MakeResourcePackage")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates the resources in the resource repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateResources(
    MgResourceIdentifier* resource, INT32 depth, CREFSTRING type,
    INT32 properties, CREFSTRING fromDate, CREFSTRING toDate)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateResources()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.EnumerateResources",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.EnumerateResources",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    MgLibraryRepositoryManager repositoryMan(*m_libraryRepository);

    byteReader = repositoryMan.EnumerateResources(resource, depth, type,
        properties, fromDate, toDate);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateResources")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a new resource to the repository or updates an existing resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::SetResource(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::SetResource()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.SetResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    repositoryMan->SetResource(resource, content, header);

    repositoryMan->CommitTransaction();

    // Dispatch resource change notifications.

    Ptr<MgSerializableCollection> changedResources = repositoryMan->GetChangedResources();

    m_loadBalanceMan->DispatchResourceChangeNotifications(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.SetResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes an existing resource from the resource repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::DeleteResource(MgResourceIdentifier* resource)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::DeleteResource()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.DeleteResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    repositoryMan->DeleteResource(resource);

    repositoryMan->CommitTransaction();

    // Dispatch resource change notifications.

    Ptr<MgSerializableCollection> changedResources = repositoryMan->GetChangedResources();

    m_loadBalanceMan->DispatchResourceChangeNotifications(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Renames a resource and/or moves it from one location to another.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::MoveResource(MgResourceIdentifier* sourceResource,
    MgResourceIdentifier* destResource, bool overwrite)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::MoveResource()");

    if (NULL == sourceResource || NULL == destResource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.MoveResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(destResource));

    repositoryMan->MoveResource(sourceResource, destResource, overwrite);

    repositoryMan->CommitTransaction();

    // Dispatch resource change notifications.

    Ptr<MgSerializableCollection> changedResources = repositoryMan->GetChangedResources();

    m_loadBalanceMan->DispatchResourceChangeNotifications(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.MoveResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Copies an existing resource to another location.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::CopyResource(MgResourceIdentifier* sourceResource,
    MgResourceIdentifier* destResource, bool overwrite)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::CopyResource()");

    if (NULL == sourceResource || NULL == destResource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.CopyResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(destResource));

    repositoryMan->CopyResource(sourceResource, destResource, overwrite);

    repositoryMan->CommitTransaction();

    // Dispatch resource change notifications.

    Ptr<MgSerializableCollection> changedResources = repositoryMan->GetChangedResources();

    m_loadBalanceMan->DispatchResourceChangeNotifications(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CopyResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the content of the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::GetResourceContent(
    MgResourceIdentifier* resource, CREFSTRING preProcessTags)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GetResourceContent()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetResourceContent", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    byteReader = repositoryMan->GetResourceContent(resource, preProcessTags);

    repositoryMan->CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetResourceContent")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the header associated with the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::GetResourceHeader(
    MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GetResourceHeader()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetResourceHeader", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.GetResourceHeader", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    MgLibraryRepositoryManager repositoryMan(*m_libraryRepository);

    byteReader = repositoryMan.GetResourceHeader(resource);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetResourceHeader")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the modified date of the specified resource.
/// </summary>
///
/// <exceptions>
/// MgInvalidRepositoryTypeException
/// MgInvalidResourceTypeException
/// </exceptions>
///----------------------------------------------------------------------------

MgDateTime* MgServerResourceService::GetResourceModifiedDate(
    MgResourceIdentifier* resource)
{
    Ptr<MgDateTime> dateTime;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GetResourceModifiedDate()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetResourceModifiedDate",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.GetResourceModifiedDate",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    MgLibraryRepositoryManager repositoryMan(*m_libraryRepository);

    dateTime = repositoryMan.GetResourceModifiedDate(resource);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetResourceModifiedDate")

    return dateTime.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates all resources which reference the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateReferences(
    MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateReferences()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.EnumerateReferences", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    byteReader = repositoryMan->EnumerateReferences(resource);

    repositoryMan->CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateReferences")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerate all the parent Map Definition resources of the specified
/// resources.
///
MgSerializableCollection* MgServerResourceService::EnumerateParentMapDefinitions(
    MgSerializableCollection* resources)
{
    Ptr<MgSerializableCollection> mapDefinitions;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateParentMapDefinitions()");

    if (NULL == resources)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.EnumerateParentMapDefinitions",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Check if all the resources come from the Library or Session repository.

    set<string> childResources;
    INT32 libraryResources = 0;
    INT32 sessionResources = 0;
    INT32 numResources = resources->GetCount();

    for (INT32 i = 0; i < numResources; ++i)
    {
        Ptr<MgSerializable> serializableObj = resources->GetItem(i);
        MgResourceIdentifier* resource = dynamic_cast<MgResourceIdentifier*>(
            serializableObj.p);

        if (NULL == resource)
        {
            throw new MgInvalidCastException(
                L"MgServerResourceService.EnumerateParentMapDefinitions",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (!resource->IsFolder())
        {
            if (resource->IsRepositoryTypeOf(MgRepositoryType::Library))
            {
                childResources.insert(MgUtil::WideCharToMultiByte(
                    resource->ToString()));
                ++libraryResources;
            }
            else if (resource->IsRepositoryTypeOf(MgRepositoryType::Session))
            {
                childResources.insert(MgUtil::WideCharToMultiByte(
                    resource->ToString()));
                ++sessionResources;
            }
            else
            {
                throw new MgInvalidRepositoryTypeException(
                    L"MgServerResourceService.EnumerateParentMapDefinitions",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    }

    // Check if all the resources come from the same repository.

    if (0 != libraryResources && 0 != sessionResources)
    {
        // TODO: Throw a more descriptive exception?
        throw new MgInvalidArgumentException(
            L"MgServerResourceService.EnumerateParentMapDefinitions",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Note that a Session resource may reference a Library resource but not
    // the other way around, therefore:
    //
    // 1. If the resource comes from the Session repository, then we need to
    //    perform this operation on only the Session repository.
    // 2. If the resource comes from the Library repository, then we need to
    //    perform this operation on both the Library and Session repositories.

    set<STRING> parentResources;
    numResources = static_cast<INT32>(childResources.size());

    if (numResources > 0)
    {
        MgSessionRepositoryManager sessionRepositoryMan(*m_sessionRepository);

        if (sessionResources > 0)
        {
            sessionRepositoryMan.EnumerateParentMapDefinitions(childResources,
                parentResources);
            sessionRepositoryMan.CommitTransaction();
        }
        else if (libraryResources > 0)
        {
            MgLibraryRepositoryManager libraryRepositoryMan(*m_libraryRepository);

            libraryRepositoryMan.EnumerateParentMapDefinitions(childResources,
                parentResources);
            libraryRepositoryMan.CommitTransaction();

            sessionRepositoryMan.EnumerateParentMapDefinitions(childResources,
                parentResources);
            sessionRepositoryMan.CommitTransaction();
        }
    }

    mapDefinitions = new MgSerializableCollection();

    for (set<STRING>::const_iterator i = parentResources.begin();
        i != parentResources.end( ); ++i)
    {
        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(*i);

        mapDefinitions->Add(resource.p);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateParentMapDefinitions")

    return mapDefinitions.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Changes the owner of an existing resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::ChangeResourceOwner(
    MgResourceIdentifier* resource, CREFSTRING owner, bool includeDescendants)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::ChangeResourceOwner()");

    if (NULL == resource || owner.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.ChangeResourceOwner",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.ChangeResourceOwner",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    MgLibraryRepositoryManager repositoryMan(*m_libraryRepository);

    repositoryMan.ChangeResourceOwner(resource, owner, includeDescendants);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.ChangeResourceOwner")
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the permissions for all descendants to be inherited from the
/// specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::InheritPermissionsFrom(MgResourceIdentifier* resource)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::InheritPermissionsFrom()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.InheritPermissionsFrom",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.InheritPermissionsFrom",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    MgLibraryRepositoryManager repositoryMan(*m_libraryRepository);

    repositoryMan.InheritPermissionsFrom(resource);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.InheritPermissionsFrom")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates tagged data for the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateResourceData(
    MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateResourceData()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.EnumerateResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    byteReader = repositoryMan->EnumerateResourceData(resource);

    repositoryMan->CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateResourceData")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets tagged data of a specific type to the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::SetResourceData(MgResourceIdentifier* resource,
    CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::SetResourceData()");

    if (NULL == resource || NULL == data || dataName.empty() || dataType.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.SetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    repositoryMan->SetResourceData(resource, dataName, dataType, data);

    repositoryMan->CommitTransaction();

    // Dispatch resource change notifications.

    Ptr<MgSerializableCollection> changedResources = repositoryMan->GetChangedResources();

    m_loadBalanceMan->DispatchResourceChangeNotifications(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.SetResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes tagged data from the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::DeleteResourceData(
    MgResourceIdentifier* resource, CREFSTRING dataName)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::DeleteResourceData()");

    if (NULL == resource || dataName.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.DeleteResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    repositoryMan->DeleteResourceData(resource, dataName);

    repositoryMan->CommitTransaction();

    // Dispatch resource change notifications.

    Ptr<MgSerializableCollection> changedResources = repositoryMan->GetChangedResources();

    m_loadBalanceMan->DispatchResourceChangeNotifications(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Renames tagged data for the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::RenameResourceData(MgResourceIdentifier* resource,
    CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::RenameResourceData()");

    if (NULL == resource || oldDataName.empty() || newDataName.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.RenameResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    repositoryMan->RenameResourceData(resource, oldDataName, newDataName, overwrite);

    repositoryMan->CommitTransaction();

    // Dispatch resource change notifications.

    Ptr<MgSerializableCollection> changedResources = repositoryMan->GetChangedResources();

    m_loadBalanceMan->DispatchResourceChangeNotifications(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.RenameResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets tagged data for the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::GetResourceData(
    MgResourceIdentifier* resource, CREFSTRING dataName,
    CREFSTRING preProcessTags)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GetResourceData()");

    if (NULL == resource || dataName.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    byteReader = repositoryMan->GetResourceData(resource, dataName,
        preProcessTags);

    repositoryMan->CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetResourceData")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates an application repository manager based on the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgApplicationRepositoryManager* MgServerResourceService::CreateApplicationRepositoryManager(
    MgResourceIdentifier* resource)
{
    ACE_ASSERT(NULL != resource);
    auto_ptr<MgApplicationRepositoryManager> repositoryMan;
    STRING repositoryType = resource->GetRepositoryType();

    MG_RESOURCE_SERVICE_TRY()

    if (MgRepositoryType::Library == repositoryType)
    {
        ACE_ASSERT(resource->GetRepositoryName().empty());
        repositoryMan.reset(new MgLibraryRepositoryManager(*m_libraryRepository));
    }
    else if (MgRepositoryType::Session == repositoryType)
    {
        ACE_ASSERT(!resource->GetRepositoryName().empty());
        repositoryMan.reset(new MgSessionRepositoryManager(*m_sessionRepository));
    }
    else
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.CreateApplicationRepositoryManager",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CreateApplicationRepositoryManager")

    return repositoryMan.release();
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates users in the specified group or role. If no group and role are
/// specified, all available users will be returned.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateUsers(CREFSTRING group,
    CREFSTRING role, bool includePassword, bool includeGroups)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateUsers()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    byteReader = repositoryMan.EnumerateUsers(group, role, includePassword,
        includeGroups);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateUsers")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a new user,
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::AddUser(CREFSTRING userId, CREFSTRING username,
    CREFSTRING password, CREFSTRING description)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::AddUser()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.AddUser(userId, username, password, description);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.AddUser")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes the given users in the MgStringCollection.  If the collection is
/// empty, all users are deleted.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgServerResourceService::DeleteUsers(MgStringCollection* users)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::DeleteUsers()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.DeleteUsers(users);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the user using the given parameters.  If a parameter is empty, that
/// user attribute is not changed (except for the newDescription parameter).
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgServerResourceService::UpdateUser(CREFSTRING userId, CREFSTRING newUserId,
    CREFSTRING newUsername, CREFSTRING newPassword, CREFSTRING newDescription)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::UpdateUser()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.UpdateUser(userId, newUserId, newUsername, newPassword, newDescription);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates groups that contain the given user and/or role.  If the user
/// and role parameters are empty, all groups are returned.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateGroups(CREFSTRING user,
    CREFSTRING role)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateGroups()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    byteReader = repositoryMan.EnumerateGroups(user, role);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateGroups")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a group.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgServerResourceService::AddGroup(CREFSTRING group, CREFSTRING description)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::AddGroup()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.AddGroup(group, description);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.AddGroup")
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a group.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgServerResourceService::DeleteGroups(MgStringCollection* groups)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::DeleteGroups()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.DeleteGroups(groups);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteGroups")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the group using the given parameters.  If a parameter is empty, that
/// group attribute is not changed (except for the newDescription parameter).
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgServerResourceService::UpdateGroup(CREFSTRING group, CREFSTRING newGroup,
    CREFSTRING newDescription )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::UpdateGroup()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.UpdateGroup(group, newGroup, newDescription);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.UpdateGroup")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Grants membership in the specified groups to the specified users.
/// </summary>
///
/// <param name="groups">
/// Groups to receive the specified users.
/// </param>
/// <param name="users">
/// IDs of users to gain membership in the specified groups.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgGroupNotFoundException
void MgServerResourceService::GrantGroupMembershipsToUsers( MgStringCollection* groups,
    MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GrantGroupMembershipsToUsers()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.GrantGroupMembershipsToUsers(groups, users);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GrantGroupMembershipsToUsers")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Revokes membership in the specified groups from the specified users.
/// </summary>
///
/// <param name="groups">
/// Groups to lose the specified users.
/// </param>
/// <param name="users">
/// IDs of users to lose membership in the specified groups.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgGroupNotFoundException
void MgServerResourceService::RevokeGroupMembershipsFromUsers( MgStringCollection* groups,
    MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::RevokeGroupMembershipsFromUsers()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.RevokeGroupMembershipsFromUsers(groups, users);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.RevokeGroupMembershipsFromUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets a list of available roles.  Parameters specify contents of the list:
/// - If both user and group are NOT specified, return a list of all available roles.
/// - If group is specified, return a list of roles for that group.
/// - If user is specified, return a list of roles for that user.
/// - If both user and group are specified, throw an exception.
/// </summary>
///
/// <param name="user">
/// ID of user, if returning list of roles for a particular user; empty, otherwise.
/// </param>
/// <param name="group">
/// Name of group, if returning list of roles for a particular group; empty, otherwise.
/// </param>
///
/// <returns>
/// Result containing the appropriate list of roles.
/// </returns>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgStringCollection* MgServerResourceService::EnumerateRoles(CREFSTRING user,
    CREFSTRING group)
{
    Ptr<MgStringCollection> roles;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateRoles()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    roles = repositoryMan.EnumerateRoles(user, group);

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateRoles")

    return roles.Detach();
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Grants the specified roles to the specified users.
/// </summary>
/// <param name="roles">
/// List of roles to grant to the specified users.
/// </param>
/// <param name="users">
/// IDs of users to receive the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgRoleNotFoundException
void MgServerResourceService::GrantRoleMembershipsToUsers(MgStringCollection* roles,
    MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GrantRoleMembershipsToUsers()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.GrantRoleMembershipsToUsers( roles, users );

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GrantRoleMembershipsToUsers")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Revokes the specified roles from the specified users.
/// </summary>
///
/// <param name="roles">
/// List of roles to revoke from the specified users.
/// Repository.
/// <param name="users">
/// IDs of users to lose the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgRoleNotFoundException
void MgServerResourceService::RevokeRoleMembershipsFromUsers(MgStringCollection* roles,
    MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::RevokeRoleMembershipsFromUsers()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.RevokeRoleMembershipsFromUsers( roles, users );

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.RevokeRoleMembershipsFromUsers")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Grants the specified role memberships to the specified groups.
/// </summary>
///
/// <param name="roles">
/// List of roles to be granted to the specified groups.
/// </param>
/// <param name="groups">
/// Groups to receive the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
/// MgRoleNotFoundException
void MgServerResourceService::GrantRoleMembershipsToGroups(MgStringCollection* roles,
    MgStringCollection* groups )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GrantRoleMembershipsToGroups()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.GrantRoleMembershipsToGroups( roles, groups );

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GrantRoleMembershipsToGroups")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Revokes the specified roles from the specified groups.
/// </summary>
///
/// <param name="roles">
/// Roles to be revoked from the groups.
/// </param>
/// <param name="groups">
/// Groups to lose the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
/// MgRoleNotFoundException
void MgServerResourceService::RevokeRoleMembershipsFromGroups(MgStringCollection* roles,
    MgStringCollection* groups )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::RevokeRoleMembershipsFromGroups()");

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);

    repositoryMan.RevokeRoleMembershipsFromGroups( roles, groups );

    repositoryMan.CommitTransaction();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.RevokeRoleMembershipsFromGroups")
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a new security cache.
/// </summary>
///----------------------------------------------------------------------------

MgSecurityCache* MgServerResourceService::CreateSecurityCache()
{
    Ptr<MgSecurityCache> securityCache;

    MG_RESOURCE_SERVICE_TRY()

    MgSiteRepositoryManager repositoryMan(*m_siteRepository);
    MgSiteResourceContentManager* resourceContentMan =
        dynamic_cast<MgSiteResourceContentManager*>(
            repositoryMan.GetResourceContentManager());
    ACE_ASSERT(NULL != resourceContentMan);

    securityCache = resourceContentMan->CreateSecurityCache();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CreateSecurityCache")

    return securityCache.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a new permission cache.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionCache* MgServerResourceService::CreatePermissionCache(bool startup)
{
    Ptr<MgPermissionCache> permissionCache;

    MG_RESOURCE_SERVICE_TRY()

    MgLibraryRepositoryManager repositoryManager(*m_libraryRepository, startup);
    MgResourceHeaderManager* resourceHeaderManager =
        dynamic_cast<MgResourceHeaderManager*>(
            repositoryManager.GetResourceHeaderManager());
    ACE_ASSERT(NULL != resourceHeaderManager);

    permissionCache = resourceHeaderManager->CreatePermissionCache();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CreatePermissionCache")

    return permissionCache.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Performs checkpoints for all the repositories.
/// </summary>
///----------------------------------------------------------------------------

void MgServerResourceService::PerformRepositoryCheckpoints(UINT32 flags)
{
    MG_RESOURCE_SERVICE_TRY()

    if (NULL != m_libraryRepository)
    {
        m_libraryRepository->PerformCheckpoint(flags);
    }

    if (NULL != m_siteRepository)
    {
        m_siteRepository->PerformCheckpoint(flags);
    }

    if (NULL != m_sessionRepository)
    {
        m_sessionRepository->PerformCheckpoint(flags);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.PerformCheckpoint")
}
