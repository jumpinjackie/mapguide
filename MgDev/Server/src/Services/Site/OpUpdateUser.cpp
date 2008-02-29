//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "OpUpdateUser.h"
#include "LogManager.h"
#include "CryptographyUtil.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpUpdateUser::MgOpUpdateUser()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpUpdateUser::~MgOpUpdateUser()
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
void MgOpUpdateUser::Execute()
{
    ACE_DEBUG( (LM_DEBUG, ACE_TEXT( "  (%t) MgOpUpdateUser::Execute()\n" )) );
    ACE_ASSERT( 0 != m_data );

    MG_LOG_OPERATION_MESSAGE(L"UpdateUser");

    MG_SITE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    //  Get Arguments
    if ( 5 == m_packet.m_NumArguments )
    {
        STRING userID;
        m_stream->GetString( userID );

        STRING newUserID;
        m_stream->GetString( newUserID );

        STRING newUserName;
        m_stream->GetString( newUserName );

        STRING encryptedPassword, newPassword;
        m_stream->GetString(encryptedPassword);

        if (!encryptedPassword.empty())
        {
            MG_CRYPTOGRAPHY_TRY()

            MgCryptographyUtil cryptoUtil;
            string decryptedPassword;

            cryptoUtil.DecryptPassword(MgUtil::WideCharToMultiByte(encryptedPassword),
                decryptedPassword);
            MgUtil::MultiByteToWideChar(decryptedPassword, newPassword);

            MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgOpUpdateUser.Execute")
        }

        STRING newDesc;
        m_stream->GetString( newDesc );

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(userID.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(newUserID.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(newUserName.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"********");   // Password - we don't want to log this.
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(newDesc.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        // Validate operation
        Validate();

        m_service->UpdateUser( userID, newUserID, newUserName, newPassword, newDesc );

        EndExecution();
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpUpdateUser.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SITE_SERVICE_CATCH( L"MgOpUpdateUser.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add admin log entry for operation
    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();

    MG_SITE_SERVICE_THROW()
}
