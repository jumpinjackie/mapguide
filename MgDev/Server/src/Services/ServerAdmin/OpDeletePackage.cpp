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

#include "MapGuideCommon.h"
#include "ServerAdminService.h"
#include "OpDeletePackage.h"
#include "LogManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpDeletePackage::MgOpDeletePackage()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpDeletePackage::~MgOpDeletePackage()
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
void MgOpDeletePackage::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpDeletePackage::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"DeletePackage");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (1 == m_packet.m_NumArguments)
    {
        STRING package;
        m_stream->GetString(package);

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(package.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        // Validate operation
        Validate();

        m_service->DeletePackage(package);

        EndExecution();
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpDeletePackage.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgOpDeletePackage.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add admin log entry for operation
    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();

    MG_THROW()
}
