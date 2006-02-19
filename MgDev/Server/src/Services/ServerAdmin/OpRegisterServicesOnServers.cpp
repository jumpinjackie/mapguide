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

#include "AceCommon.h"
#include "ServerAdminService.h"
#include "OpRegisterServicesOnServers.h"
#include "LogManager.h"
#include "SecurityManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOpRegisterServicesOnServers::MgOpRegisterServicesOnServers()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOpRegisterServicesOnServers::~MgOpRegisterServicesOnServers()
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

void MgOpRegisterServicesOnServers::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpRegisterServicesOnServers::Execute()\n")));
    ACE_ASSERT(NULL != m_data);

    bool operationCompleted = false;
    bool argsRead = false;
    Ptr<MgStream> stream;

    MG_LOG_OPERATION_MESSAGE(L"RegisterServicesOnServers");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    stream = new MgStream(m_data->GetStreamHelper());

    if (1 == m_packet.m_NumArguments)
    {
        Ptr<MgSerializableCollection> serverInfoList =
            (MgSerializableCollection*)stream->GetObject();

        argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgSerializableCollection");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        MgSecurityManager::Authenticate(MgUserInformation::GetCurrentUserInfo());

        Ptr<MgSerializableCollection> feedbackList =
            m_service->RegisterServicesOnServers(serverInfoList);

        operationCompleted = true;
        WriteResponseStream(*stream, feedbackList);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if ( !argsRead )
    {
        throw new MgOperationProcessingException(L"MgOpRegisterServicesOnServers.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgOpRegisterServicesOnServers.Execute")

    if (mgException != NULL && !operationCompleted && stream != NULL)
    {
        WriteResponseStream(*stream, mgException);

        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add admin log entry for operation
    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();

    MG_THROW()
}
