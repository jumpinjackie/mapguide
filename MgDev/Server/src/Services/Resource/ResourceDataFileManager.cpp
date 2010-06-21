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
#include "ResourceDataFileManager.h"
#include "ApplicationRepository.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceDataFileManager::MgResourceDataFileManager(
    MgApplicationRepositoryManager& repositoryMan) :
    MgResourceDataManager(repositoryMan),
    m_resourceDataFilePath(((MgApplicationRepository&)
        repositoryMan.GetRepository()).GetResourceDataFilePath())
{
    MG_RESOURCE_SERVICE_TRY()

    if (m_resourceDataFilePath.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgResourceDataFileManager.MgResourceDataFileManager",
            __LINE__, __WFILE__, &arguments, L"MgResourceDataFilePathEmpty", NULL);
    }

    MgFileUtil::AppendSlashToEndOfPath(m_resourceDataFilePath);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceDataFileManager.MgResourceDataFileManager")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceDataFileManager::~MgResourceDataFileManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds resource data to disk.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDataFileManager::SetResourceData(CREFSTRING pathname,
    MgByteReader* byteReader)
{
    assert(NULL != byteReader);

    MG_RESOURCE_SERVICE_TRY()

    // Ensure the current operation is transactionally protected.
    m_repositoryMan.ValidateTransaction();

    // TODO: Optimization - Back up the file if it exists?
    MgByteSink byteSink(byteReader);

    byteSink.ToFile(pathname);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceDataFileManager.SetResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes resource data from disk.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDataFileManager::DeleteResourceData(CREFSTRING pathname,
    bool strict)
{
    MG_RESOURCE_SERVICE_TRY()

    // Ensure the current operation is transactionally protected.
    m_repositoryMan.ValidateTransaction();

    MgFileUtil::DeleteFile(pathname, strict);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceDataFileManager.DeleteResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Renames resource data on disk.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDataFileManager::RenameResourceData(CREFSTRING path,
    CREFSTRING oldFileName, CREFSTRING newFileName, bool overwrite)
{
    MG_RESOURCE_SERVICE_TRY()

    // Ensure the current operation is transactionally protected.
    m_repositoryMan.ValidateTransaction();

    // TODO: Optimization - Back up the file if it exists?
    MgFileUtil::RenameFile(path, oldFileName, newFileName, overwrite);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceDataFileManager.RenameResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Copies resource data on disk from one location to another.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceDataFileManager::CopyResourceData(CREFSTRING sourcePathname,
    CREFSTRING destPathname, bool overwrite)
{
    MG_RESOURCE_SERVICE_TRY()

    // Ensure the current operation is transactionally protected.
    m_repositoryMan.ValidateTransaction();

    // TODO: Optimization - Back up the file if it exists?
    MgFileUtil::CopyFile(sourcePathname, destPathname, overwrite);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceDataFileManager.CopyResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets resource data from disk.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgResourceDataFileManager::GetResourceData(CREFSTRING pathname,
    CREFSTRING mimeType)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgByteSource> byteSource = new MgByteSource(pathname);

    byteSource->SetMimeType(mimeType);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceDataFileManager.GetResourceData")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}
