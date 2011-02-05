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
#include "OpNotifyResourcesChanged.h"
#include "LogManager.h"
#include "SecurityManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpNotifyResourcesChanged::MgOpNotifyResourcesChanged()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpNotifyResourcesChanged::~MgOpNotifyResourcesChanged()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Gets the role(s) required to perform this operation.
/// </summary>
///----------------------------------------------------------------------------
MgStringCollection* MgOpNotifyResourcesChanged::GetRoles() const
{
    return GetAuthorRole();
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
void MgOpNotifyResourcesChanged::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpNotifyResourcesChanged::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"NotifyResourcesChanged");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (1 == m_packet.m_NumArguments)
    {
        Ptr<MgSerializableCollection> resources = (MgSerializableCollection*)m_stream->GetObject();

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgSerializableCollection");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        // Validate() not called here because this is an internal server-only operation.
        // Instead, MgSecurityManager::Authenticate() should be called to perform authentication.
        Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
        MgSecurityManager::Authenticate(userInfo);

        m_service->NotifyResourcesChanged(resources);

        EndExecution();
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpNotifyResourcesChanged.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgOpNotifyResourcesChanged.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add admin log entry for operation
    MG_LOG_OPERATION_MESSAGE_ADMIN_ENTRY();

    MG_THROW()
}
