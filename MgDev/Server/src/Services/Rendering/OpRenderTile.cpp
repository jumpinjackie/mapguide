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

#include "OpRenderTile.h"
#include "LogManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpRenderTile::MgOpRenderTile()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpRenderTile::~MgOpRenderTile()
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
void MgOpRenderTile::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpRenderTile::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"RenderTile");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (4 == m_packet.m_NumArguments)
    {
        Ptr<MgMap> map = (MgMap*)m_stream->GetObject();
        Ptr<MgResourceIdentifier> resource = map->GetResourceId();
        map->SetDelayedLoadResourceService(m_resourceService);

        STRING baseMapLayerGroupName;
        m_stream->GetString(baseMapLayerGroupName);

        INT32 tileCol = 0;
        m_stream->GetInt32(tileCol);

        INT32 tileRow = 0;
        m_stream->GetInt32(tileRow);

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(baseMapLayerGroupName.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileCol);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileRow);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        Ptr<MgByteReader> byteReader =
            m_service->RenderTile(map, baseMapLayerGroupName, tileCol, tileRow);

        EndExecution(byteReader);
    }
    else if (8 == m_packet.m_NumArguments)
    {
        Ptr<MgMap> map = (MgMap*)m_stream->GetObject();
        Ptr<MgResourceIdentifier> resource = map->GetResourceId();
        map->SetDelayedLoadResourceService(m_resourceService);

        STRING baseMapLayerGroupName;
        m_stream->GetString(baseMapLayerGroupName);

        INT32 tileCol = 0;
        m_stream->GetInt32(tileCol);

        INT32 tileRow = 0;
        m_stream->GetInt32(tileRow);

        INT32 tileWidth = 0;
        m_stream->GetInt32(tileWidth);

        INT32 tileHeight = 0;
        m_stream->GetInt32(tileHeight);

        INT32 tileDpi = 0;
        m_stream->GetInt32(tileDpi);

        STRING tileImageFormat;
        m_stream->GetString(tileImageFormat);

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(baseMapLayerGroupName.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileCol);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileRow);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileWidth);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileHeight);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileDpi);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(tileImageFormat.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        Ptr<MgByteReader> byteReader =
            m_service->RenderTile(map, baseMapLayerGroupName, tileCol, tileRow, tileWidth, tileHeight, tileDpi, tileImageFormat);

        EndExecution(byteReader);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpRenderTile.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgOpRenderTile.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()
}
