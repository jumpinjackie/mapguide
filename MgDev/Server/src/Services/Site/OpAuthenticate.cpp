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
#include "OpAuthenticate.h"
#include "SiteServiceUtil.h"
#include "LogManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpAuthenticate::MgOpAuthenticate()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpAuthenticate::~MgOpAuthenticate()
{
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
void MgOpAuthenticate::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpAuthenticate::Execute()\n")));

    // Only log this operation for DEBUG builds because it is called very often
#ifdef _DEBUG
    MG_LOG_OPERATION_MESSAGE(L"Authenticate");
#endif

    MG_SITE_SERVICE_TRY()

#ifdef _DEBUG
    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);
#endif

    ACE_ASSERT(m_stream != NULL);

    if (3 == m_packet.m_NumArguments)
    {
        Ptr<MgUserInformation> userInfo =
            (MgUserInformation*)m_stream->GetObject();
        Ptr<MgStringCollection> requiredRoles =
            (MgStringCollection*)m_stream->GetObject();
        bool returnAssignedRoles;
        m_stream->GetBoolean(returnAssignedRoles);

        BeginExecution();

#ifdef _DEBUG
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgUserInformation");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgStringCollection");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(returnAssignedRoles);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
#endif
        // Validate() not called here since this is the operation that performs authentication.

        Ptr<MgStringCollection> assignedRoles = m_service->Authenticate(userInfo,
            requiredRoles, returnAssignedRoles);

        EndExecution(assignedRoles);
    }
    else
    {
#ifdef _DEBUG
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
#endif
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpAuthenticate.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
#ifdef _DEBUG
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());
#endif

    MG_SITE_SERVICE_CATCH(L"MgOpAuthenticate.Execute")

    if (mgException != NULL)
    {
        // Failed operation
#ifdef _DEBUG
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
#endif
    }

    // Add admin log entry for operation
#ifdef _DEBUG
    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();
#endif

    MG_SITE_SERVICE_THROW()
}
