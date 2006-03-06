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

#include "OpRenderTile.h"
#include "LogManager.h"


MgOpRenderTile::MgOpRenderTile()
{
}


MgOpRenderTile::~MgOpRenderTile()
{
}


void MgOpRenderTile::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpRenderTile::Execute()\n")));
    ACE_ASSERT(0 != m_data);

    bool operationCompleted = false;
    bool argsRead = false;
    Ptr<MgStream> stream;

    MG_LOG_OPERATION_MESSAGE(L"RenderTile");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    stream = new MgStream(m_data->GetStreamHelper());

    if (4 == m_packet.m_NumArguments)
    {
        Ptr<MgMap> map = (MgMap*)stream->GetObject();

        STRING baseMapLayerGroupName;
        stream->GetString(baseMapLayerGroupName);

        INT32 tileCol = 0;
        stream->GetInt32(tileCol);

        INT32 tileRow = 0;
        stream->GetInt32(tileRow);

        argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgMap");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(baseMapLayerGroupName.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"tileCol");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"tileRow");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Ptr<MgByteReader> byteReader =
            m_service->RenderTile(map, baseMapLayerGroupName, tileCol, tileRow);

        operationCompleted = true;
        WriteResponseStream(*stream, byteReader);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if ( !argsRead )
    {
        throw new MgOperationProcessingException(L"MgOpRenderTile.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgOpRenderTile.Execute")

    if (mgException != 0 && !operationCompleted && stream != 0)
    {
        WriteResponseStream(*stream, mgException);

        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()
}
