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

#include "ResourceServiceDefs.h"
#include "OpEnumerateParentMapDefinitions.h"
#include "ServerResourceService.h"
#include "LogManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgOpEnumerateParentMapDefinitions::MgOpEnumerateParentMapDefinitions()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgOpEnumerateParentMapDefinitions::~MgOpEnumerateParentMapDefinitions()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Execute the operation.
///
void MgOpEnumerateParentMapDefinitions::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpEnumerateParentMapDefinitions::Execute()\n")));
    ACE_ASSERT(0 != m_data);

    bool operationCompleted = false;
    bool argsRead = false;
    Ptr<MgStream> stream;

    MG_LOG_OPERATION_MESSAGE(L"EnumerateParentMapDefinitions");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    stream = new MgStream(m_data->GetStreamHelper());

    if (1 == m_packet.m_NumArguments)
    {
        Ptr<MgSerializableCollection> resources =
            (MgSerializableCollection*)stream->GetObject();

        argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgSerializableCollection");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Authenticate();

        Ptr<MgSerializableCollection> mapDefinitions =
            m_service->EnumerateParentMapDefinitions(resources);

        operationCompleted = true;
        WriteResponseStream(*stream, mapDefinitions);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpEnumerateParentMapDefinitions.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgOpEnumerateParentMapDefinitions.Execute")

    if (mgException != 0 && !operationCompleted && stream != 0)
    {
        WriteResponseStream(*stream, mgException);

        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}
