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
#include "Repository.h"

const string MgRepository::LibraryResourceContentContainerName   = "MgLibraryResourceContents.dbxml";
const string MgRepository::LibraryResourceHeaderContainerName    = "MgLibraryResourceHeaders.dbxml";
const string MgRepository::LibraryResourceDataStreamDatabaseName = "MgLibraryResourceData.db";
const string MgRepository::SessionResourceContentContainerName   = "MgSessionResourceContents.dbxml";
const string MgRepository::SessionResourceDataStreamDatabaseName = "MgSessionResourceData.db";
const string MgRepository::SiteResourceContentContainerName      = "MgSiteResourceContents.dbxml";

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgRepository::MgRepository() :
    m_environment(NULL),
    m_resourceContentContainer(NULL)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgRepository::~MgRepository()
{
    delete m_resourceContentContainer;
    delete m_environment;
}

///----------------------------------------------------------------------------
/// <summary>
/// Verifies if the specified database can be safely opened.
/// </summary>
///----------------------------------------------------------------------------

void MgRepository::VerifySafeDatabaseAccess(CREFSTRING dirPath,
    CREFSTRING fileName)
{
    STRING pathname = dirPath;
    MgFileUtil::AppendSlashToEndOfPath(pathname);
    pathname += fileName;

    if (!MgFileUtil::VerifySafeFileAccess(pathname))
    {
        MgStringCollection arguments;
        arguments.Add(pathname);

        throw new MgRepositoryOpenFailedException(
            L"MgRepository.VerifySafeDatabaseAccess",
            __LINE__, __WFILE__, &arguments, L"MgRepositoryAlreadyOpened", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Performs a checkpoint for this repository.
/// </summary>
///----------------------------------------------------------------------------

void MgRepository::PerformCheckpoint(UINT32 flags)
{
    if (NULL != m_environment)
    {
        m_environment->PerformCheckpoint(flags);
    }
}
