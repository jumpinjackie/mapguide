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
//

#include "ServerFeatureServiceDefs.h"
#include "OpGetWfsFeatureTotal.h"
#include "ServerFeatureService.h"
#include "LogManager.h"

MgOpGetWfsFeatureTotal::MgOpGetWfsFeatureTotal() { }

MgOpGetWfsFeatureTotal::~MgOpGetWfsFeatureTotal() { }

void MgOpGetWfsFeatureTotal::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpGetWfsFeatureTotal::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"GetWfsFeatureTotal");

    MG_FEATURE_SERVICE_TRY()

        MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (4 == m_packet.m_NumArguments)
    {
        // Get the feature source
        Ptr<MgResourceIdentifier> featureSourceId = (MgResourceIdentifier*)m_stream->GetObject();

        // Get the schema name
        STRING featureClass;
        m_stream->GetString(featureClass);

        // Get the filter
        STRING filter;
        m_stream->GetString(filter);

        // Get the sort criteria
        INT32 maxFeatures;
        m_stream->GetInt32(maxFeatures);

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(featureSourceId->ToString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(featureClass.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(filter.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(maxFeatures);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        // Execute the operation
        INT32 total = m_service->GetWfsFeatureTotal(featureSourceId, featureClass, filter, maxFeatures);

        // Write the response
        EndExecution(total);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpGetWfsReader.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgOpGetWfsReader.Execute")

        if (mgException != NULL)
        {
            // Failed operation
            MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
}