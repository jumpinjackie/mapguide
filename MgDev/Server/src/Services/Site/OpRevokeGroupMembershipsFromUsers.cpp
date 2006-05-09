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

#include "SiteServiceDefs.h"
#include "SiteServiceUtil.h"
#include "OpRevokeGroupMembershipsFromUsers.h"
#include "LogManager.h"

MgOpRevokeGroupMembershipsFromUsers::MgOpRevokeGroupMembershipsFromUsers()
{
}

MgOpRevokeGroupMembershipsFromUsers::~MgOpRevokeGroupMembershipsFromUsers()
{
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Executes the operation and writes out the results. Implements
/// IOperation::Execute().
/// </summary>
///
/// <exceptions>
/// An MgException is thrown on failure.
/// </exceptions>

void MgOpRevokeGroupMembershipsFromUsers::Execute()
{
    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) MgOpRevokeGroupMembershipsFromUsers.Execute\n")));
    ACE_ASSERT( 0 != m_data );





    MG_LOG_OPERATION_MESSAGE(L"RevokeGroupMembershipsFromUsers");

    MG_SITE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    //  Get Arguments
    if ( 2 == m_packet.m_NumArguments )
    {
        Ptr<MgStringCollection> groups =
            (MgStringCollection*) m_stream->GetObject();

        Ptr<MgStringCollection> users =
            (MgStringCollection*) m_stream->GetObject();

        m_argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgStringCollection");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgStringCollection");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        // Validate operation
        Validate();

        m_service->RevokeGroupMembershipsFromUsers( groups, users );

        m_opCompleted = true;
        WriteResponseStream();
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpRevokeGroupMembershipsFromUsers.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SITE_SERVICE_CATCH( L"MgOpRevokeGroupMembershipsFromUsers.Execute")

    if (mgException != NULL)
    {


        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add admin log entry for operation
    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();

    MG_SITE_SERVICE_THROW()
}
