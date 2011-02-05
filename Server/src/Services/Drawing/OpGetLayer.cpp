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
#include "ServerDrawingServiceDefs.h"
#include "OpGetLayer.h"
#include "LogManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpGetLayer::MgOpGetLayer()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpGetLayer::~MgOpGetLayer()
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
void MgOpGetLayer::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpGetLayer::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"GetLayer");

    MG_SERVER_DRAWING_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (3 == m_packet.m_NumArguments)
    {
        Ptr<MgResourceIdentifier> identifier = (MgResourceIdentifier*)m_stream->GetObject();

        STRING sectionName;
        m_stream->GetString(sectionName);

        STRING layerName;
        m_stream->GetString(layerName);

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == identifier) ? L"MgResourceIdentifier" : identifier->ToString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(sectionName.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(layerName.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        Ptr<MgByteReader> byteReader = m_service->GetLayer(identifier, sectionName, layerName);

        EndExecution(byteReader);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpGetLayer.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_SERVER_DRAWING_SERVICE_CATCH(L"MgOpGetLayer.Execute");

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_SERVER_DRAWING_SERVICE_THROW()
}
