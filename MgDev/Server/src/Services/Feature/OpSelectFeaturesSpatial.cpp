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

#include "ServerFeatureServiceDefs.h"
#include "BaseService.h"
#include "ResourceIdentifier.h"
#include "FeatureService.h"
#include "OpSelectFeaturesSpatial.h"
#include "ServerFeatureService.h"
#include "LogManager.h"

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs the object.
/// </summary>

MgOpSelectFeaturesSpatial::MgOpSelectFeaturesSpatial()
{
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Destructs the object.
/// </summary>

MgOpSelectFeaturesSpatial::~MgOpSelectFeaturesSpatial()
{
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Executes the operation.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>

void MgOpSelectFeaturesSpatial::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpSelectFeaturesSpatial::Execute()\n")));
    ACE_ASSERT(0 != m_data);

    bool operationCompleted = false;
    bool argsRead = false;
    Ptr<MgStream> stream;

    MG_LOG_OPERATION_MESSAGE(L"SelectFeaturesSpatial");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    stream = new MgStream(m_data->GetStreamHelper());

    if (3 == m_packet.m_NumArguments)
    {
        // Get the feature source
        Ptr<MgResourceIdentifier> resource = (MgResourceIdentifier*)stream->GetObject();
        // Get the schema name
        STRING className;
        stream->GetString(className);

        // Get properties collection
        Ptr<MgFeatureAggregateOptions> qryOptions = (MgFeatureAggregateOptions*)stream->GetObject();

        argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgResourceIdentifier");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureAggregateOptions");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        // Execute the operation
        Ptr<MgDataReader> dataReader = m_service->SelectAggregate(resource, className, qryOptions);
        // Write the response
        WriteResponseStream(*stream, (MgDataReader*)dataReader);

        operationCompleted = true;
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if ( !argsRead )
    {
        throw new MgOperationProcessingException(L"MgOpSelectFeaturesSpatial.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgOpSelectFeaturesSpatial.Execute")
    // Exception occured
    if (mgException != 0 && !operationCompleted && stream != 0)
    {
        WriteResponseStream(*stream, mgException);

        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
}
