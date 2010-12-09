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
#include "SessionRepository.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgSessionRepository::MgSessionRepository(CREFSTRING name)
{
    MG_RESOURCE_SERVICE_TRY()

    // Get the repositry and data file paths.

    MgConfiguration* configuration = MgConfiguration::GetInstance();

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertySessionRepositoryPath,
        m_repositoryPath,
        MgConfigProperties::DefaultResourceServicePropertySessionRepositoryPath);

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertySessionResourceDataFilePath,
        m_resourceDataFilePath,
        MgConfigProperties::DefaultResourceServicePropertySessionResourceDataFilePath);

    m_sessionName = name;

    // Check to see whether or not it is safe to open the database.
    m_dbVersion = VerifyAccess(m_repositoryPath, m_resourceDataFilePath);

    // Open the repository.
    m_environment = new MgDbEnvironment(MgRepositoryType::Session,
        MgUtil::WideCharToMultiByte(m_repositoryPath));

    if(m_sessionName.empty())
    {
        m_resourceContentContainer = new MgResourceContainer(*m_environment,
            MgRepository::SessionResourceContentContainerName);
        m_resourceDataStreamDatabase = new MgResourceDatabase(*m_environment,
            MgRepository::SessionResourceDataStreamDatabaseName);
    }
    else
    {
        std::string containerName = MgUtil::WideCharToMultiByte(m_sessionName) + MgRepository::SessionResourceContentContainerExt;
        m_resourceContentContainer = new MgResourceContainer(*m_environment, containerName);

        std::string databaseName = MgUtil::WideCharToMultiByte(m_sessionName) + MgRepository::SessionResourceDataStreamDatabaseExt;
        m_resourceDataStreamDatabase = new MgResourceDatabase(*m_environment, databaseName);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSessionRepository.MgSessionRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSessionRepository::~MgSessionRepository()
{

    MG_RESOURCE_SERVICE_TRY()

    delete m_resourceDataStreamDatabase;
    m_resourceDataStreamDatabase = NULL;

    delete m_resourceContentContainer;
    m_resourceContentContainer = NULL;

    delete m_environment;
    m_environment = NULL;

    // Now that the above have been deleted we can delete the associated files
    STRING containerName = L"";
    STRING databaseName = L"";
    if(m_sessionName.empty())
    {
        containerName = m_repositoryPath + MgUtil::MultiByteToWideChar(MgRepository::SessionResourceContentContainerName);
        databaseName = m_repositoryPath + MgUtil::MultiByteToWideChar(MgRepository::SessionResourceDataStreamDatabaseName);
    }
    else
    {
        containerName = m_repositoryPath + m_sessionName + MgUtil::MultiByteToWideChar(MgRepository::SessionResourceContentContainerExt);
        databaseName = m_repositoryPath + m_sessionName + MgUtil::MultiByteToWideChar(MgRepository::SessionResourceDataStreamDatabaseExt);
    }

    MgFileUtil::DeleteFile(containerName);
    MgFileUtil::DeleteFile(databaseName);

    MG_RESOURCE_SERVICE_CATCH(L"MgSessionRepository.~MgSessionRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Verifies if this repository can be safely opened.
/// </summary>
///----------------------------------------------------------------------------

int MgSessionRepository::VerifyAccess(CREFSTRING repositoryPath, CREFSTRING resourceDataFilePath)
{
    MgFileUtil::CreateDirectory(repositoryPath, false, true);
    MgFileUtil::CreateDirectory(resourceDataFilePath, false, true);

    STRING containerName = m_sessionName + MgUtil::MultiByteToWideChar(MgRepository::SessionResourceContentContainerExt);
    int dbVersion = MgRepository::VerifyAccess(
        repositoryPath,
        containerName,
        true);

    STRING databaseName = m_sessionName + MgUtil::MultiByteToWideChar(MgRepository::SessionResourceDataStreamDatabaseExt);
    MgRepository::VerifyAccess(
        repositoryPath,
        databaseName,
        false);

    return dbVersion;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initialize the repository.
///
void MgSessionRepository::Initialize()
{
    // Set up the repository indices.
    SetupIndices();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set up the indices for the repository.
///
void MgSessionRepository::SetupIndices()
{
    if (0 == m_dbVersion)
    {
        m_dbVersion = MG_DBXML_CURRENT_VERSION;
    }

    m_resourceContentContainer->AddIndex(
        "",
        MgResourceInfo::sm_elementResourceId,
        "node-element-equality-string");
}
