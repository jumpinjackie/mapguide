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

#include "ResourceServiceDefs.h"
#include "SessionRepositoryManager.h"
#include "ResourceDataStreamManager.h"
#include "ResourceDataFileManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgSessionRepositoryManager::MgSessionRepositoryManager(
    MgSessionRepository& repository) :
    MgApplicationRepositoryManager(repository),
    m_resourceContentMan(NULL)
{
    m_resourceContentMan = new MgSessionResourceContentManager(*this);
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSessionRepositoryManager::~MgSessionRepositoryManager()
{
    delete m_resourceContentMan;
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates a list of entries in the repository.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgSessionRepositoryManager::EnumerateRepositories()
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Allows only the administrator browses the repositories.

    if (!m_currUserIsAdmin)
    {
        MG_LOG_AUTHENTICATION_ENTRY(MgResources::UnauthorizedAccess.c_str());

        throw new MgUnauthorizedAccessException(
            L"MgSessionRepositoryManager.EnumerateRepositories",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Perform the operation.

    byteReader = m_resourceContentMan->EnumerateRepositories();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSessionRepositoryManager.EnumerateRepositories")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes a repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgSessionRepositoryManager::DeleteRepository(MgResourceIdentifier* resource)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (!resource->IsRoot())
    {
        throw new MgInvalidResourceTypeException(
            L"MgSessionRepositoryManager.DeleteRepository",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Delete the document content.

    m_resourceContentMan->DeleteRepository(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSessionRepositoryManager.DeleteRepository")
}
