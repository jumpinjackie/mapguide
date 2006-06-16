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
#include "LibraryRepository.h"
#include "ResourceInfo.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgLibraryRepository::MgLibraryRepository() :
    m_resourceHeaderContainer(NULL)
{
    MG_RESOURCE_SERVICE_TRY()

    // Get the repositry and data file paths.

    STRING repositoryPath;
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    assert(NULL != configuration);

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertyLibraryRepositoryPath,
        repositoryPath,
        MgConfigProperties::DefaultResourceServicePropertyLibraryRepositoryPath);

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath,
        m_resourceDataFilePath,
        MgConfigProperties::DefaultResourceServicePropertyLibraryResourceDataFilePath);

    // Check to see whether or not it is safe to open the database.

    VerifySafeDatabaseAccess(repositoryPath);

    // Open the repository.

    m_environment = new MgDbEnvironment(MgRepositoryType::Library,
        MgUtil::WideCharToMultiByte(repositoryPath));
    m_resourceContentContainer = new MgResourceContainer(*m_environment,
        MgRepository::LibraryResourceContentContainerName);
    m_resourceHeaderContainer = new MgResourceContainer(*m_environment,
        MgRepository::LibraryResourceHeaderContainerName);
    m_resourceDataStreamDatabase = new MgResourceDatabase(*m_environment,
        MgRepository::LibraryResourceDataStreamDatabaseName);

    // Add the index specification.

    // TODO: Index Id/ModifiedDate
    // m_resourceContentContainer->AddIndex(MgResourceInfo::sm_nodeResourceId);
    // m_resourceHeaderContainer->AddIndex(MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate]);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryRepository.MgLibraryRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgLibraryRepository::~MgLibraryRepository()
{
    delete m_resourceHeaderContainer;
}

///----------------------------------------------------------------------------
/// <summary>
/// Verifies if this repository can be safely opened.
/// </summary>
///----------------------------------------------------------------------------

void MgLibraryRepository::VerifySafeDatabaseAccess(CREFSTRING repositoryPath)
{
    MgRepository::VerifySafeDatabaseAccess(repositoryPath,
        MgUtil::MultiByteToWideChar(
            MgRepository::LibraryResourceContentContainerName));
    MgRepository::VerifySafeDatabaseAccess(repositoryPath,
        MgUtil::MultiByteToWideChar(
            MgRepository::LibraryResourceHeaderContainerName));
    MgRepository::VerifySafeDatabaseAccess(repositoryPath,
        MgUtil::MultiByteToWideChar(
            MgRepository::LibraryResourceDataStreamDatabaseName));
}
