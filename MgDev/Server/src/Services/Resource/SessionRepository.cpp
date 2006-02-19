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

MgSessionRepository::MgSessionRepository()
{
    MG_RESOURCE_SERVICE_TRY()

    // Get the repositry and data file paths.

    STRING repositoryPath;
    MgConfiguration* configuration = MgConfiguration::GetInstance();

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertySessionRepositoryPath,
        repositoryPath,
        MgConfigProperties::DefaultResourceServicePropertySessionRepositoryPath);

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertySessionResourceDataFilePath,
        m_resourceDataFilePath,
        MgConfigProperties::DefaultResourceServicePropertySessionResourceDataFilePath);

    // Check to see whether or not it is safe to open the database.

    VerifySafeDatabaseAccess(repositoryPath);

    // Open the repository.

    m_environment = new MgDbEnvironment(MgRepositoryType::Session,
        MgUtil::WideCharToMultiByte(repositoryPath));
    m_resourceContentContainer = new MgResourceContainer(*m_environment,
        MgRepository::SessionResourceContentContainerName);
    m_resourceDataStreamDatabase = new MgResourceDatabase(*m_environment,
        MgRepository::SessionResourceDataStreamDatabaseName);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgSessionRepository.MgSessionRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgSessionRepository::~MgSessionRepository()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Verifies if this repository can be safely opened.
/// </summary>
///----------------------------------------------------------------------------

void MgSessionRepository::VerifySafeDatabaseAccess(CREFSTRING repositoryPath)
{
    MgRepository::VerifySafeDatabaseAccess(repositoryPath,
        MgUtil::MultiByteToWideChar(
            MgRepository::SessionResourceContentContainerName));
    MgRepository::VerifySafeDatabaseAccess(repositoryPath,
        MgUtil::MultiByteToWideChar(
            MgRepository::SessionResourceDataStreamDatabaseName));
}
