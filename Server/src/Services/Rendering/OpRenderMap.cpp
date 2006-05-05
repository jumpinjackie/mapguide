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

#include "OpRenderMap.h"
#include "LogManager.h"


MgOpRenderMap::MgOpRenderMap()
{
}


MgOpRenderMap::~MgOpRenderMap()
{
}


void MgOpRenderMap::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpRenderMap::Execute()\n")));
    





    MG_LOG_OPERATION_MESSAGE(L"RenderMap");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (3 == m_packet.m_NumArguments)
    {
        Ptr<MgMap> map = (MgMap*)m_stream->GetObject();
        Ptr<MgSelection> selection = (MgSelection*)m_stream->GetObject();
        if(selection)
            selection->SetMap(map);

        STRING format;
        m_stream->GetString(format);

        m_argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgMap");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgSelection");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Authenticate();

        Ptr<MgByteReader> byteReader =
            m_service->RenderMap(map, selection, format);

        m_opCompleted = true;
        WriteResponseStream(byteReader);
    }
    else if (7 == m_packet.m_NumArguments)
    {
        Ptr<MgMap> map = (MgMap*)m_stream->GetObject();
        Ptr<MgSelection> selection = (MgSelection*)m_stream->GetObject();
        if(selection)
            selection->SetMap(map);

        Ptr<MgEnvelope> extents = (MgEnvelope*)m_stream->GetObject();

        INT32 width = 0;
        m_stream->GetInt32(width);

        INT32 height = 0;
        m_stream->GetInt32(height);

        Ptr<MgColor> color = (MgColor*)m_stream->GetObject();

        STRING format;
        m_stream->GetString(format);

        m_argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgMap");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgSelection");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgEnvelope");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"width");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"height");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgColor");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Authenticate();

        Ptr<MgByteReader> byteReader =
            m_service->RenderMap(map, selection, extents, width, height, color, format);

        m_opCompleted = true;
        WriteResponseStream(byteReader);
    }
    else if (8 == m_packet.m_NumArguments)
    {
        Ptr<MgMap> map = (MgMap*)m_stream->GetObject();
        Ptr<MgSelection> selection = (MgSelection*)m_stream->GetObject();
        if(selection)
            selection->SetMap(map);

        Ptr<MgCoordinate> center = (MgCoordinate*)m_stream->GetObject();

        double scale = 0.0;
        m_stream->GetDouble(scale);

        INT32 width = 0;
        m_stream->GetInt32(width);

        INT32 height = 0;
        m_stream->GetInt32(height);

        Ptr<MgColor> color = (MgColor*)m_stream->GetObject();

        STRING format;
        m_stream->GetString(format);

        m_argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgMap");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgSelection");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgCoordinate");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"scale");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"width");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"height");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgColor");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Authenticate();

        Ptr<MgByteReader> byteReader =
            m_service->RenderMap(map, selection, center, scale, width, height, color, format);

        m_opCompleted = true;
        WriteResponseStream(byteReader);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpRenderMap.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgOpRenderMap.Execute")

    if (mgException != NULL)
    {


        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()
}
