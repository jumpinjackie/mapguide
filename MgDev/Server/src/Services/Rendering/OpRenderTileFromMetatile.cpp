//
//  Copyright (C) 2004-2019 by Autodesk, Inc.
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

#include "OpRenderTileFromMetatile.h"

MgOpRenderTileFromMetatile::MgOpRenderTileFromMetatile()
{
}

MgOpRenderTileFromMetatile::~MgOpRenderTileFromMetatile()
{
}

void MgOpRenderTileFromMetatile::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpRenderTileFromMetatile::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"MgOpRenderTileFromMetatile");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (4 == m_packet.m_NumArguments)
    {
        Ptr<MgMap> map = (MgMap*)m_stream->GetObject();
        Ptr<MgResourceIdentifier> resource = map->GetResourceId();
        map->SetDelayedLoadResourceService(m_resourceService);

        Ptr<MgMetatile> metaTile = (MgMetatile*)m_stream->GetObject();

        INT32 subTileX = 0;
        m_stream->GetInt32(subTileX);

        INT32 subTileY = 0;
        m_stream->GetInt32(subTileY);

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();

        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgMetatile[");
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(metaTile->GetMetaTilingFactor());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(metaTile->GetMetaTilingFactor());
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"]");

        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(subTileX);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(subTileY);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        Ptr<MgByteReader> tile = m_service->RenderTileFromMetaTile(map, metaTile, subTileX, subTileY);

        EndExecution(tile);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpRenderTileFromMetatile.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgOpRenderTileFromMetatile.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()
}