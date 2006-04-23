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
#include "OpUpdateServer.h"
#include "LogManager.h"


MgOpUpdateServer::MgOpUpdateServer(void)
{
}

MgOpUpdateServer::~MgOpUpdateServer(void)
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

void MgOpUpdateServer::Execute()
{
    ACE_DEBUG( (LM_DEBUG, ACE_TEXT( "  (%t) MgOpUpdateServer::Execute()\n" )) );
    ACE_ASSERT( 0 != m_data );





    MG_LOG_OPERATION_MESSAGE(L"UpdateServer");

    MG_SITE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    //  Get Arguments
    if ( 4 == m_packet.m_NumArguments )
    {
        STRING oldName;
        m_stream->GetString( oldName );

        STRING newName;
        m_stream->GetString( newName );

        STRING newDescription;
        m_stream->GetString( newDescription );

        STRING newAddress;
        m_stream->GetString( newAddress );

        m_argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(oldName.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(newName.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(newDescription.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(newAddress.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        // Authenticate operation
        Authenticate();

        m_service->UpdateServer( oldName, newName, newDescription, newAddress );

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
        throw new MgOperationProcessingException(L"MgOpUpdateServer.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SITE_SERVICE_CATCH( L"MgOpUpdateServer.Execute")

    if (mgException != NULL)
    {


        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add admin log entry for operation
    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();

    MG_SITE_SERVICE_THROW()
}
