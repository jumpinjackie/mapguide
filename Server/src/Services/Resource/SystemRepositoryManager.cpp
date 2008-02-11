//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "SystemRepositoryManager.h"
#include "ResourceContentManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgSystemRepositoryManager::MgSystemRepositoryManager(
    MgSystemRepository& repository) :
    MgRepositoryManager(repository)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSystemRepositoryManager::~MgSystemRepositoryManager()
{
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

MgByteReader* MgSystemRepositoryManager::GetResourceContent(
    MgResourceIdentifier* resource, CREFSTRING preProcessTags)
{
    assert(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (resource->IsFolder())
    {
        throw new MgInvalidResourceTypeException(
            L"MgSystemRepositoryManager.GetResourceContent",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Perform the operation.

    if (preProcessTags.empty())
    {
        MgResourceContentManager* resourceContentMan = GetResourceContentManager();

        byteReader = resourceContentMan->GetResource(resource);
    }
    else
    {
        throw new MgInvalidResourcePreProcessingTypeException(
            L"MgSystemRepositoryManager.GetResourceContent",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSystemRepositoryManager.GetResourceContent")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}
