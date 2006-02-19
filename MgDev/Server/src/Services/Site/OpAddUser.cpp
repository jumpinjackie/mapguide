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
#include "OpAddUser.h"
#include "LogManager.h"
#include "CryptographyUtil.h"

MgOpAddUser::MgOpAddUser()
{
}

MgOpAddUser::~MgOpAddUser()
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

void MgOpAddUser::Execute()
{
    ACE_DEBUG( (LM_DEBUG, ACE_TEXT( "  (%t) MgOpAddUser::Execute()\n" )) );
    ACE_ASSERT( 0 != m_data );

    bool operationCompleted = false;
    bool argsRead = false;
    Ptr<MgStream> stream;

    MG_LOG_OPERATION_MESSAGE(L"AddUser");

    MG_SITE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    stream = new MgStream( m_data->GetStreamHelper() );

    //  Get Arguments
    if ( 4 == m_packet.m_NumArguments )
    {
        STRING userid;
        stream->GetString( userid );

        STRING username;
        stream->GetString( username );

        STRING encryptedPassword, password;
        stream->GetString(encryptedPassword);

        if (!encryptedPassword.empty())
        {
            MG_CRYPTOGRAPHY_TRY()

            MgCryptographyUtil cryptoUtil;
            cryptoUtil.DecryptPassword(encryptedPassword, password);

            MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgOpAddUser.Execute")
        }

        STRING desc;
        stream->GetString( desc );

        argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(userid.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(username.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"********");   // Password - we don't want to log this.
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(desc.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        // Authenticate operation
        Authenticate();

        m_service->AddUser( userid, username, password, desc );

        operationCompleted = true;
        WriteResponseStream( *stream );
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if ( !argsRead )
    {
        throw new MgOperationProcessingException(L"MgOpAddUser.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SITE_SERVICE_CATCH( L"MgOpAddUser.Execute")

    if (mgException != 0 && !operationCompleted && stream != 0)
    {
        WriteResponseStream( *stream, mgException );

        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add admin log entry for operation
    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();

    MG_SITE_SERVICE_THROW()
}
