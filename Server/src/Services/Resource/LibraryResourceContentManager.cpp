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
#include "LibraryResourceContentManager.h"
#include "LibraryRepositoryManager.h"
#include "LibraryRepository.h"
#include "ResourceHeaderManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgLibraryResourceContentManager::MgLibraryResourceContentManager(
    MgLibraryRepositoryManager& repositoryMan) :
    MgApplicationResourceContentManager(repositoryMan)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgLibraryResourceContentManager::~MgLibraryResourceContentManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// specified resource.
/// </summary>
///
/// <exceptions>
/// MgUnauthorizedAccessException
/// MgPermissionDeniedException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgLibraryResourceContentManager::CheckPermission(
    MgResourceIdentifier& resource, CREFSTRING permission, bool strict)
{
    return m_repositoryMan.GetResourceHeaderManager()->CheckPermission(
        resource, permission, strict);
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// parent of the specified resource.
/// </summary>
///
/// <exceptions>
/// MgUnauthorizedAccessException
/// MgPermissionDeniedException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgLibraryResourceContentManager::CheckParentPermission(
    MgResourceIdentifier& resource, CREFSTRING permission, bool strict)
{
    return m_repositoryMan.GetResourceHeaderManager()->CheckParentPermission(
        resource, permission, strict);
}
