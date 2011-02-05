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

#include "SiteServiceDefs.h"
#include "OpCreateSession.h"
#include "SiteServiceUtil.h"
#include "LogManager.h"
#include "LicenseManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpCreateSession::MgOpCreateSession()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpCreateSession::~MgOpCreateSession()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Gets the role(s) required to perform this operation.
/// </summary>
///----------------------------------------------------------------------------
MgStringCollection* MgOpCreateSession::GetRoles() const
{
    return NULL;
}


///----------------------------------------------------------------------------
/// <summary>
/// Executes the operation.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgOpCreateSession::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpCreateSession::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"CreateSession");

    MG_SITE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (0 == m_packet.m_NumArguments)
    {
        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        // Validate operation
        Validate();

        STRING sessionId = m_service->CreateSession();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"Session:");
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(sessionId.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L" ");

        EndExecution(sessionId);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpCreateSession.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SITE_SERVICE_CATCH(L"MgOpCreateSession.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add admin log entry for operation
    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SITE_SERVICE_THROW()
}

void MgOpCreateSession::CheckLicense()
{
    // Perform the license validation.
    MgLicenseManager* licenseManager = MgLicenseManager::GetInstance();
    if (NULL != licenseManager)
    {
        licenseManager->CheckLicense();
    }
}
