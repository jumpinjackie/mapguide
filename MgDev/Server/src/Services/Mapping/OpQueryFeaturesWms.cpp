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

#include "ServerMappingServiceDefs.h"
#include "OpQueryFeaturesWms.h"
#include "ServerMappingService.h"
#include "LogManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOpQueryFeaturesWms::MgOpQueryFeaturesWms()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOpQueryFeaturesWms::~MgOpQueryFeaturesWms()
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

void MgOpQueryFeaturesWms::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpQueryFeaturesWms::Execute()\n")));
    ACE_ASSERT(0 != m_data);

    bool operationCompleted = false;
    bool argsRead = false;
    Ptr<MgStream> stream;

    MG_LOG_OPERATION_MESSAGE(L"QueryFeaturesWms");

    MG_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    stream = new MgStream(m_data->GetStreamHelper());

    if (6 == m_packet.m_NumArguments)
    {
        INT32 x, y, maxFeatures;
        STRING coordinateSpace;
        Ptr<MgMap> map = (MgMap*)stream->GetObject();
        Ptr<MgStringCollection> layerNames = (MgStringCollection*)stream->GetObject();
        stream->GetInt32(x);
        stream->GetInt32(y);
        stream->GetInt32(maxFeatures);
        stream->GetString(coordinateSpace);

        argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgMap");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgStringCollection");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(x);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(y);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(maxFeatures);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(coordinateSpace.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Ptr<MgByteReader> byteReader =
            m_service->QueryFeatures(map, layerNames, x, y, maxFeatures,
            coordinateSpace);

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
        throw new MgOperationProcessingException(L"MgOpQueryFeaturesWms.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgOpQueryFeaturesWms.Execute")

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
