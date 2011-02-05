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
#include "Repository.h"
#include "LibraryRepository.h"
#include "SessionRepository.h"
#include "SiteRepository.h"

const string MgRepository::LibraryResourceContentContainerName   = "MgLibraryResourceContents.dbxml";
const string MgRepository::LibraryResourceHeaderContainerName    = "MgLibraryResourceHeaders.dbxml";
const string MgRepository::LibraryResourceDataStreamDatabaseName = "MgLibraryResourceData.db";
const string MgRepository::SessionResourceContentContainerName   = "MgSessionResourceContents.dbxml";
const string MgRepository::SessionResourceDataStreamDatabaseName = "MgSessionResourceData.db";
const string MgRepository::SiteResourceContentContainerName      = "MgSiteResourceContents.dbxml";
const string MgRepository::SessionResourceContentContainerExt    = ".dbxml";
const string MgRepository::SessionResourceDataStreamDatabaseExt  = ".db";

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
    m_dbVersion(0),
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
    if(NULL != m_resourceContentContainer)
        delete m_resourceContentContainer;

    if(NULL != m_environment)
        delete m_environment;
}

///----------------------------------------------------------------------------
/// <summary>
/// Verifies if the specified database can be safely opened.
/// </summary>
///----------------------------------------------------------------------------

int MgRepository::VerifyAccess(CREFSTRING dirPath,
    CREFSTRING fileName, bool checkVersion)
{
    int dbVersion = 0;
    STRING pathname = dirPath;
    MgFileUtil::AppendSlashToEndOfPath(pathname);
    pathname += fileName;

    // Check if the database is currently opened by any other application.
    if (!MgFileUtil::VerifySafeFileAccess(pathname))
    {
        MgStringCollection arguments;
        arguments.Add(pathname);

        throw new MgRepositoryOpenFailedException(
            L"MgRepository.VerifyAccess",
            __LINE__, __WFILE__, &arguments, L"MgRepositoryAlreadyOpened", NULL);
    }

    if (checkVersion)
    {
        // Note that this very first instance of XmlManager will eliminate
        // unnecessary XMLPlatformUtils::Initialize/Terminate calls.
        XmlManager xmlMan;

        dbVersion = xmlMan.existsContainer(MgUtil::WideCharToMultiByte(pathname));

        // Check if the database version is compatible.
        if (0 != dbVersion && MG_DBXML_CURRENT_VERSION != dbVersion)
        {
            MgStringCollection whatArguments;
            whatArguments.Add(pathname);
            MgStringCollection whyArguments;
            STRING version;
            MgUtil::Int32ToString(dbVersion, version);
            whyArguments.Add(version);
            MgUtil::Int32ToString(MG_DBXML_CURRENT_VERSION, version);
            whyArguments.Add(version);

            throw new MgRepositoryOpenFailedException(
                L"MgRepository.VerifyAccess",
                __LINE__, __WFILE__, &whatArguments, L"MgRepositoryVersionMismatch", &whyArguments);
        }
    }

    return dbVersion;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determines if the repository is of the specified type.
///
bool MgRepository::IsTypeOf(CREFSTRING type) const
{
    bool result = false;

    if (MgRepositoryType::Library == type)
    {
        result = (NULL != dynamic_cast<const MgLibraryRepository*>(this));
    }
    else if (MgRepositoryType::Session == type)
    {
        result = (NULL != dynamic_cast<const MgSessionRepository*>(this));
    }
    else if (MgRepositoryType::Site == type)
    {
        result = (NULL != dynamic_cast<const MgSiteRepository*>(this));
    }
    else
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgRepository.IsTypeOf",
            __LINE__,  __WFILE__, NULL, L"", NULL);
    }

    return result;
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
