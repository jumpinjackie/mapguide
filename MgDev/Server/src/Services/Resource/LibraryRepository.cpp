//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

    VerifyAccess(repositoryPath);

    // Open the repository.

    m_environment = new MgDbEnvironment(MgRepositoryType::Library,
        MgUtil::WideCharToMultiByte(repositoryPath));
    m_resourceContentContainer = new MgResourceContainer(*m_environment,
        MgRepository::LibraryResourceContentContainerName);
    m_resourceHeaderContainer = new MgResourceContainer(*m_environment,
        MgRepository::LibraryResourceHeaderContainerName);
    m_resourceDataStreamDatabase = new MgResourceDatabase(*m_environment,
        MgRepository::LibraryResourceDataStreamDatabaseName);

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

void MgLibraryRepository::VerifyAccess(CREFSTRING repositoryPath)
{
    MgRepository::VerifyAccess(
        repositoryPath,
        MgUtil::MultiByteToWideChar(MgRepository::LibraryResourceContentContainerName),
        true);
    MgRepository::VerifyAccess(
        repositoryPath,
        MgUtil::MultiByteToWideChar(MgRepository::LibraryResourceHeaderContainerName),
        true);
    MgRepository::VerifyAccess(
        repositoryPath,
        MgUtil::MultiByteToWideChar(MgRepository::LibraryResourceDataStreamDatabaseName),
        false);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initialize the repository.
///
void MgLibraryRepository::Initialize()
{
    bool freshlyNew = false;
    MgResourceIdentifier resource;

    resource.SetRepositoryType(MgRepositoryType::Library);
    resource.SetResourceType(MgResourceType::Folder);

    MgLibraryRepositoryManager repositoryMan(*this);

    repositoryMan.Initialize(true);

    if (!repositoryMan.FindResource(&resource))
    {
        repositoryMan.CreateRepository(&resource, NULL, NULL);
        freshlyNew = true;
    }

    repositoryMan.Terminate();

    if (freshlyNew)
    {
        SetupIndices();
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set up the indices for the repository.
///
void MgLibraryRepository::SetupIndices()
{
    m_resourceContentContainer->AddIndex(
        "",
        MgResourceInfo::sm_elementResourceId,
        "node-element-equality-string");

    m_resourceHeaderContainer->AddIndex(
        DbXml::metaDataNamespace_uri,
        DbXml::metaDataName_name,
        "node-metadata-substring-string");
    m_resourceHeaderContainer->AddIndex(
        MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::Depth],
        "node-metadata-equality-double");
    m_resourceHeaderContainer->AddIndex(
        MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::Owner],
        "node-metadata-equality-string");
}
