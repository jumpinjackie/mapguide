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

#include "SiteServiceDefs.h"
#include "SiteServiceUtil.h"
#include "OpRevokeRoleMembershipsFromUsers.h"
#include "LogManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpRevokeRoleMembershipsFromUsers::MgOpRevokeRoleMembershipsFromUsers()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpRevokeRoleMembershipsFromUsers::~MgOpRevokeRoleMembershipsFromUsers()
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
void MgOpRevokeRoleMembershipsFromUsers::Execute()
{
    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) MgOpRevokeRoleMembershipsFromUsers.Execute\n" )));
    ACE_ASSERT( 0 != m_data );

    MG_LOG_OPERATION_MESSAGE(L"RevokeRoleMembershipsFromUsers");

    MG_SITE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    //  Get Arguments
    if ( 2 == m_packet.m_NumArguments )
    {
        Ptr<MgStringCollection> roles =
            (MgStringCollection*) m_stream->GetObject();

        Ptr<MgStringCollection> users =
            (MgStringCollection*) m_stream->GetObject();

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgStringCollection");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgStringCollection");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        // Validate operation
        Validate();

        m_service->RevokeRoleMembershipsFromUsers( roles, users );

        EndExecution();
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpRevokeRoleMembershipsFromUsers.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SITE_SERVICE_CATCH( L"MgOpRevokeRoleMembershipsFromUsers.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add admin log entry for operation
    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();

    MG_SITE_SERVICE_THROW()
}
