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
#include "LibraryRepositoryManager.h"
#include "ResourceHeaderManager.h"
#include "ResourceDataStreamManager.h"
#include "ResourceDataFileManager.h"
#include "ResourcePackageManager.h"

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
    MgLibraryRepository& repository, bool startup) :
    MgApplicationRepositoryManager(repository, startup),
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
    assert(!resource.empty());
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
    m_resourceHeaderMan->UpdateResourceModifiedDates(m_dateModifiedResources);

    MgApplicationRepositoryManager::CommitTransaction();

    m_resourceHeaderMan->UpdatePermissionCache();
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
    assert(NULL != resource);
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
/// Loads the specified resource package into the repository.
///
void MgLibraryRepositoryManager::LoadResourcePackage(CREFSTRING pathname, 
    bool logActivities)
{
    assert(!pathname.empty());

    MG_RESOURCE_SERVICE_TRY()

    MgResourcePackageManager packageMan(*this, pathname, logActivities);

    packageMan.PerformOperations();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.LoadResourcePackage")
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
    INT32 properties, CREFSTRING fromDate, CREFSTRING toDate)
{
    assert(NULL != resource);
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
        resource, depth, type, properties, fromDate, toDate);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepositoryManager.EnumerateResources")

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

MgByteReader* MgLibraryRepositoryManager::GetResourceHeader(MgResourceIdentifier* resource)
{
    assert(NULL != resource);
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
    assert(NULL != resource);
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
    assert(NULL != resource && !owner.empty());

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
    assert(NULL != resource);

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

///----------------------------------------------------------------------------
/// <summary>
/// Determines if the specified resource exists.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgLibraryRepositoryManager::FindResource(MgResourceIdentifier* resource)
{
    assert(NULL != resource);
    assert(NULL != m_resourceHeaderMan);

    return m_resourceHeaderMan->FindResource(resource->ToString());
}
