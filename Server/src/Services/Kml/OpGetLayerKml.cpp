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

#include "OpGetLayerKml.h"
#include "LogManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpGetLayerKml::MgOpGetLayerKml()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpGetLayerKml::~MgOpGetLayerKml()
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
void MgOpGetLayerKml::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpGetLayerKml::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"GetLayerKml");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (8 == m_packet.m_NumArguments)
    {
        Ptr<MgLayer> layer = (MgLayer*)m_stream->GetObject();
        Ptr<MgResourceIdentifier> resource = layer->GetLayerDefinition();
        Ptr<MgEnvelope> extents = (MgEnvelope*)m_stream->GetObject();
        INT32 width;
        m_stream->GetInt32(width);
        INT32 height;
        m_stream->GetInt32(height);
        double dpi;
        m_stream->GetDouble(dpi);
        INT32 drawOrder;
        m_stream->GetInt32(drawOrder);
        STRING agentUri;
        m_stream->GetString(agentUri);
        STRING format;
        m_stream->GetString(format);
        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgEnvelope");
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(width);
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(height);
        MG_LOG_OPERATION_MESSAGE_ADD_DOUBLE(dpi);
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(drawOrder);
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(agentUri.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        Ptr<MgByteReader> kml =
            m_service->GetLayerKml(layer, extents, width, height, dpi, drawOrder, agentUri, format);

        EndExecution(kml);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpGetLayerKml.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgOpGetLayerKml.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()
}
