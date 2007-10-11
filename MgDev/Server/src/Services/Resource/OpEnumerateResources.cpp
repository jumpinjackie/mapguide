//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "OpEnumerateResources.h"
#include "ServerResourceService.h"
#include "LogManager.h"
#include "System/XmlJsonConvert.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOpEnumerateResources::MgOpEnumerateResources()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOpEnumerateResources::~MgOpEnumerateResources()
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

void MgOpEnumerateResources::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpEnumerateResources::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"EnumerateResources");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (8 == m_packet.m_NumArguments || 7 == m_packet.m_NumArguments)
    {
        bool computeChildren = true;
        INT32 depth, properties;
        STRING type, format, fromDate, toDate;
        Ptr<MgResourceIdentifier> resource = (MgResourceIdentifier*)m_stream->GetObject();
        m_stream->GetInt32(depth);
        m_stream->GetString(type);
        m_stream->GetInt32(properties);
        m_stream->GetString(fromDate);
        m_stream->GetString(toDate);
        if (8 == m_packet.m_NumArguments)
        {
            m_stream->GetBoolean(computeChildren);
        }
        m_stream->GetString(format);


        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgResourceIdentifier");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(depth);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(type.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(properties);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(fromDate.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(toDate.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(computeChildren);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        Ptr<MgByteReader> byteReader =
            m_service->EnumerateResources(resource, depth, type,
                properties, fromDate, toDate, computeChildren, format);

        EndExecution(byteReader);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpEnumerateResources.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgOpEnumerateResources.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}
