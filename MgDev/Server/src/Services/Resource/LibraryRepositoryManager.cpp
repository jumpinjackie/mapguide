//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "LibraryRepositoryManager.h"
#include "ResourceHeaderManager.h"
#include "ResourceDataStreamManager.h"
#include "ResourceDataFileManager.h"
#include "ResourcePackageLoader.h"
#include "ResourcePackageMaker.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgLibraryRepositoryManager::MgLibraryRepositoryManager(
    MgLibraryRepository& repository) :
    MgApplicationRepositoryManager(repository),
    m_resourceContentMan(NULL),
    m_resourceHeaderMan(NULL)
{
    MG_RESOURCE_SERVICE_TRY()

    m_resourceContentMan = new MgLibraryResourceContentManager(*this);
    m_resourceHeaderMan = new MgResourceHeaderManager(*this);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.MgLibraryRepositoryManager")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgLibraryRepositoryManager::~MgLibraryRepositoryManager()
{
    delete m_resourceContentMan;
    delete m_resourceHeaderMan;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Insert the specified resource into the date-modified resource set.
///
void MgLibraryRepositoryManager::UpdateDateModifiedResourceSet(
    CREFSTRING resource)
{
    ACE_ASSERT(!resource.empty());
    m_dateModifiedResources.insert(resource);
}

///----------------------------------------------------------------------------
/// <summary>
/// Commits the transaction. If needed, this method should be used before
/// the destructor gets called.
/// </summary>
///----------------------------------------------------------------------------

void MgLibraryRepositoryManager::CommitTransaction()
{
    MG_RESOURCE_SERVICE_TRY()

    // TODO: Updating the modified dates and doing the actual commit can cause DB_BUSY errors.
    //       Ensure that only the Library updates the modified dates as the session does not need them. Check that this is true?
    //       The root issue might be that 2 transactions are done here 1)Modified dates and 2)Repository Changes - can these be combined into a single transaction?
    m_resourceHeaderMan->UpdateResourceModifiedDates(m_dateModifiedResources);

    MgApplicationRepositoryManager::CommitTransaction();

    m_resourceHeaderMan->UpdatePermissionCache();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.CommitTransaction")
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

MgByteReader* MgLibraryRepositoryManager::GetRepositoryHeader(MgResourceIdentifier* resource)
{
    ACE_ASSERT(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (!resource->IsRoot())
    {
        throw new MgInvalidResourceTypeException(
            L"MgLibraryRepositoryManager.GetRepositoryHeader",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Perform the operation.

    byteReader = m_resourceHeaderMan->GetRepository(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.GetRepositoryHeader")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Applies a package of resource changes to the repository.
///
void MgLibraryRepositoryManager::ApplyResourcePackage(MgByteReader* packageStream)
{
    ACE_ASSERT(NULL != packageStream);
    STRING packagePathname;

    MG_RESOURCE_SERVICE_TRY()

    packagePathname = MgFileUtil::GenerateTempFileName();
    MgByteSink byteSink(packageStream);

    byteSink.ToFile(packagePathname);
    LoadResourcePackage(packagePathname, false);

    MG_RESOURCE_SERVICE_CATCH(L"MgLibraryRepositoryManager.ApplyResourcePackage")

    if (!packagePathname.empty())
    {
        try
        {
            MgFileUtil::DeleteFile(packagePathname);
        }
        catch (MgException* e)
        {
            SAFE_RELEASE(e);
        }
        catch (...)
        {
        }
    }

    MG_RESOURCE_SERVICE_THROW()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Loads the specified resource package into the repository.
///
void MgLibraryRepositoryManager::LoadResourcePackage(CREFSTRING packagePathname,
    bool logActivities)
{
    ACE_ASSERT(!packagePathname.empty());
    auto_ptr<MgResourcePackageLoader> packageLoader;

    MG_RESOURCE_SERVICE_TRY()

    packageLoader.reset(new MgResourcePackageLoader(*this));
    packageLoader->Start(packagePathname, logActivities);

    MG_RESOURCE_SERVICE_CATCH(L"MgLibraryRepositoryManager.LoadResourcePackage")

    if (NULL != packageLoader.get())
    {
        packageLoader->End(mgException);
    }

    MG_RESOURCE_SERVICE_THROW()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a package from the specified resource, and then saves it into
/// the specified name.
///
void MgLibraryRepositoryManager::MakeResourcePackage(MgResourceIdentifier* resource,
    CREFSTRING packagePathname, CREFSTRING packageDescription, bool logActivities)
{
    ACE_ASSERT(NULL != resource && !packagePathname.empty());

    auto_ptr<MgResourcePackageMaker> packageMaker;

    MG_RESOURCE_SERVICE_TRY()

    packageMaker.reset(new MgResourcePackageMaker(*this));
    packageMaker->Start(*resource, packagePathname, packageDescription,
        logActivities);

    m_resourceHeaderMan->PackageResource(*resource, *packageMaker.get());
    m_resourceContentMan->PackageResource(*resource, *packageMaker.get());

    MG_RESOURCE_SERVICE_CATCH(L"MgLibraryRepositoryManager.MakeResourcePackage")

    if (NULL != packageMaker.get())
    {
        packageMaker->End(mgException);
    }

    MG_RESOURCE_SERVICE_THROW()
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates the resources in the opened resource repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgLibraryRepositoryManager::EnumerateResources(
    MgResourceIdentifier* resource, INT32 depth, CREFSTRING type,
    INT32 properties, CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren)
{
    ACE_ASSERT(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (!type.empty())
    {
        MgResourceIdentifier::CheckType(resource->GetRepositoryType(), type);
    }

    if (!resource->IsFolder())
    {
        if (0 != depth)
        {
            throw new MgArgumentOutOfRangeException(
                L"MgLibraryRepositoryManager.EnumerateResources",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (!type.empty() && !resource->IsResourceTypeOf(type))
        {
            throw new MgInvalidResourceTypeException(
                L"MgLibraryRepositoryManager.EnumerateResources",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    // Check the date range, the XML query parser will do the date validation.

    if (!toDate.empty() && fromDate > toDate)
    {
        throw new MgArgumentOutOfRangeException(
            L"MgLibraryRepositoryManager.EnumerateResources",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Perform the operation.

    byteReader = m_resourceHeaderMan->EnumerateResources(
        resource, depth, type, properties, fromDate, toDate, computeChildren);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.EnumerateResources")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerate the resource documents in the specified repository.
///
STRING MgLibraryRepositoryManager::EnumerateResourceDocuments(
    MgStringCollection* resources, CREFSTRING type, INT32 properties)
{
    STRING resourceList;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (type.empty())
    {
        if (NULL == resources || resources->GetCount() <= 0)
        {
            throw new MgNullArgumentException(
                L"MgLibraryRepositoryManager.EnumerateResourceDocuments",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        MgResourceIdentifier::CheckType(MgRepositoryType::Library, type);

        if (MgResourceType::Folder == type)
        {
            throw new MgInvalidResourceTypeException(
                L"MgLibraryRepositoryManager.EnumerateResourceDocuments",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

    }

    // Perform the operation.

    resourceList = m_resourceHeaderMan->EnumerateResourceDocuments(
        resources, type, properties);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.EnumerateResourceDocuments")

    return resourceList;
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

MgByteReader* MgLibraryRepositoryManager::GetResourceHeader(MgResourceIdentifier* resource)
{
    ACE_ASSERT(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (resource->IsRoot())
    {
        throw new MgInvalidResourceTypeException(
            L"MgLibraryRepositoryManager.GetResourceHeader",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Perform the operation.

    byteReader = m_resourceHeaderMan->GetResource(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.GetResourceHeader")

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

MgDateTime* MgLibraryRepositoryManager::GetResourceModifiedDate(
    MgResourceIdentifier* resource)
{
    ACE_ASSERT(NULL != resource);
    Ptr<MgDateTime> dateTime;

    MG_RESOURCE_SERVICE_TRY()

    dateTime = m_resourceHeaderMan->GetResourceModifiedDate(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.GetResourceModifiedDate")

    return dateTime.Detach();
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

void MgLibraryRepositoryManager::ChangeResourceOwner(
    MgResourceIdentifier* resource, CREFSTRING owner, bool includeDescendants)
{
    ACE_ASSERT(NULL != resource && !owner.empty());

    MG_RESOURCE_SERVICE_TRY()

    // Perform the operation.

    m_resourceHeaderMan->ChangeResourceOwner(resource, owner,
        includeDescendants);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.ChangeResourceOwner")
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

void MgLibraryRepositoryManager::InheritPermissionsFrom(
    MgResourceIdentifier* resource)
{
    ACE_ASSERT(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (!resource->IsFolder())
    {
        throw new MgInvalidResourceTypeException(
            L"MgLibraryRepositoryManager.InheritPermissionsFrom",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Perform the operation.

    m_resourceHeaderMan->InheritPermissionsFrom(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.InheritPermissionsFrom")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Checks to see if the specified resource exists.
///
bool MgLibraryRepositoryManager::ResourceExists(MgResourceIdentifier* resource)
{
    ACE_ASSERT(NULL != resource);
    ACE_ASSERT(NULL != m_resourceHeaderMan);

    return m_resourceHeaderMan->ResourceExists(resource->ToString());
}
