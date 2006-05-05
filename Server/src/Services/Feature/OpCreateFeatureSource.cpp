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
#include "OpCreateFeatureSource.h"
#include "ServerFeatureService.h"
#include "LogManager.h"

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs the object.
/// </summary>

MgOpCreateFeatureSource::MgOpCreateFeatureSource()
{
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Destructs the object.
/// </summary>

MgOpCreateFeatureSource::~MgOpCreateFeatureSource()
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

void MgOpCreateFeatureSource::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpCreateFeatureSource::Execute()\n")));
    





    MG_LOG_OPERATION_MESSAGE(L"SelectFeatures");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (2 == m_packet.m_NumArguments)
    {
        // Get the resource identifier
        Ptr<MgResourceIdentifier> resource = (MgResourceIdentifier*)m_stream->GetObject();

        // Get the parameters
        Ptr<MgFeatureSourceParams> params = (MgFeatureSourceParams*)m_stream->GetObject();

        m_argsRead = true;

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgResourceIdentifier");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgCreateSdfParams");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Authenticate();

        // Execute the operation
        m_service->CreateFeatureSource(resource, params);

        WriteResponseStream();

        // Mark operation is completed successfully
        m_opCompleted = true;
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpCreateFeatureSource.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgOpCreateFeatureSource.Execute")
    // Exception occured
    if (mgException != NULL)
    {


        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
}
