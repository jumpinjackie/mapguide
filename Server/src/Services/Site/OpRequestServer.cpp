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
#include "OpRequestServer.h"
#include "SiteServiceUtil.h"
#include "LogManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOpRequestServer::MgOpRequestServer()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOpRequestServer::~MgOpRequestServer()
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

void MgOpRequestServer::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpRequestServer::Execute()\n")));
    ACE_ASSERT(0 != m_data);

    bool operationCompleted = false;
    bool argsRead = false;
    Ptr<MgStream> stream;

    // Disabled logging this operation because it is called very often
//    MG_LOG_OPERATION_MESSAGE(L"RequestServer");

    MG_SITE_SERVICE_TRY()

//    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    stream = new MgStream(m_data->GetStreamHelper());

    if (1 == m_packet.m_NumArguments)
    {
        INT16 serviceType;
        stream->GetInt16(serviceType);

        argsRead = true;

//        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
//        MG_LOG_OPERATION_MESSAGE_ADD_INT32(serviceType);
//        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Authenticate();

        STRING serverAddress = m_service->RequestServer(serviceType);

        operationCompleted = true;
        WriteResponseStream(*stream, serverAddress);
    }
    else
    {
//        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
//        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if ( !argsRead )
    {
        throw new MgOperationProcessingException(L"MgOpRequestServer.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
//    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SITE_SERVICE_CATCH(L"MgOpRequestServer.Execute")

    if (mgException != 0 && !operationCompleted && stream != 0)
    {
        WriteResponseStream(*stream, mgException);

        // Failed operation
//        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add admin log entry for operation
//    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();

    MG_SITE_SERVICE_THROW()
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the role(s) required to perform this operation.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgOpRequestServer::GetRoles() const
{
    return NULL;
}
