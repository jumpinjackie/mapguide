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

#include "ServerFeatureServiceDefs.h"
#include "OpUpdateFeatures.h"
#include "ServerFeatureService.h"
#include "LogManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpUpdateFeatures::MgOpUpdateFeatures()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpUpdateFeatures::~MgOpUpdateFeatures()
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
void MgOpUpdateFeatures::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpUpdateFeatures::Execute()\n")));

    STRING partialFailureMsg;
    Ptr<MgPropertyCollection> rowsAffected;
    MG_LOG_OPERATION_MESSAGE(L"UpdateFeatures");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (3 == m_packet.m_NumArguments)
    {
        // Get the feature source
        Ptr<MgResourceIdentifier> resource = (MgResourceIdentifier*)m_stream->GetObject();

        // Get properties collection
        Ptr<MgFeatureCommandCollection> commands = (MgFeatureCommandCollection*)m_stream->GetObject();

        // Get the filter text
        bool useTransaction;
        m_stream->GetBoolean(useTransaction);

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureCommandCollection");
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(useTransaction);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        // Execute the operation
        rowsAffected = m_service->UpdateFeatures(resource, commands, useTransaction);

        // #649: Exceptions are only thrown in transactional cases (which will never reach here). For non-transactional cases, 
        // we check for any MgStringProperty instances. These are "serialized" FDO exception messages indicating failure for that 
        // particular command. We can't throw for non-transactional cases, instead we put the onus on the consuming caller to do 
        // the same check we are doing here.
        for (INT32 i = 0; i < rowsAffected->GetCount(); i++) 
        {
            Ptr<MgProperty> prop = rowsAffected->GetItem(i);
            if (prop->GetPropertyType() == MgPropertyType::String)
            {
                MgStringProperty* sprop = static_cast<MgStringProperty*>(prop.p);
                //One is enough to flag partial failure (should we go through all of them?)
                partialFailureMsg = sprop->GetValue();
                break;
            }
        }

        if (mgException == NULL && partialFailureMsg.empty())
        {
            // Only Write the response if there no Fdo Exception
            // if there is an exception, Response will be written by MgFeatureServiceHandler::ProcessOperation catching it
            EndExecution(rowsAffected);
        }
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpUpdateFeatures.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (partialFailureMsg.empty())
    {
        // Successful operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());
    }

    MG_FEATURE_SERVICE_CATCH(L"MgOpUpdateFeatures.Execute")

    if (mgException != NULL || !partialFailureMsg.empty())
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        // Only thrown exceptions are logged, so for this partial failure to be logged into Error.log
        // we have to log it here
        if (mgException == NULL && !partialFailureMsg.empty())
        {
            //NOTE: Serialized MgFdoException already has the stack trace, so stub an empty string to
            //satisfy the logging macro
            STRING sTrace = L"";
            MG_LOG_EXCEPTION_ENTRY(partialFailureMsg, sTrace);

            //DO NOT THROW A NEW EXCEPTION! We must respect the original contract of the API as perceived by the
            //calling web tier code. That is: Write out the original MgPropertyCollection
            EndExecution(rowsAffected);
        }
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

}
