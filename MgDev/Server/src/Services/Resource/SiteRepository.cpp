//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "SiteRepository.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgSiteRepository::MgSiteRepository()
{
    MG_RESOURCE_SERVICE_TRY()

    // Get the repositry path.

    STRING repositoryPath;
    MgConfiguration* configuration = MgConfiguration::GetInstance();

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertySiteRepositoryPath,
        repositoryPath,
        MgConfigProperties::DefaultResourceServicePropertySiteRepositoryPath);

    // Check to see whether or not it is safe to open the database.

    m_dbVersion = VerifyAccess(repositoryPath);

    // Open the repository.

    m_environment = new MgDbEnvironment(MgRepositoryType::Site,
        MgUtil::WideCharToMultiByte(repositoryPath));
    m_resourceContentContainer = new MgResourceContainer(*m_environment,
        MgRepository::SiteResourceContentContainerName);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSiteRepository.MgSiteRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSiteRepository::~MgSiteRepository()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Verifies if this repository can be safely opened.
/// </summary>
///----------------------------------------------------------------------------

int MgSiteRepository::VerifyAccess(CREFSTRING repositoryPath)
{
    return MgRepository::VerifyAccess(
        repositoryPath,
        MgUtil::MultiByteToWideChar(MgRepository::SiteResourceContentContainerName),
        true);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initialize the repository.
///
void MgSiteRepository::Initialize()
{
    // Set up the repository indices.
    SetupIndices();

    // Create the repository if it does not exist.
    MgResourceIdentifier resource;

    resource.SetRepositoryType(MgRepositoryType::Site);
    resource.SetResourceType(MgResourceType::Folder);

    MgSiteRepositoryManager repositoryMan(*this);

    repositoryMan.Initialize(true);

    if (!repositoryMan.ResourceExists(&resource))
    {
        repositoryMan.CreateRepository(&resource, NULL, NULL);
    }

    repositoryMan.Terminate();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set up the indices for the repository.
///
void MgSiteRepository::SetupIndices()
{
    if (0 == m_dbVersion)
    {
        m_dbVersion = MG_DBXML_CURRENT_VERSION;
    }

    m_resourceContentContainer->AddIndex(
        "",
        MgResourceInfo::sm_elementName,
        "edge-element-equality-string");
}
