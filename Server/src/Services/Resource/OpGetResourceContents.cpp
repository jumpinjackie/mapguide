//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "ResourceServiceDefs.h"
#include "OpGetResourceContents.h"
#include "ServerResourceService.h"
#include "LogManager.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpGetResourceContents::MgOpGetResourceContents()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpGetResourceContents::~MgOpGetResourceContents()
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
void MgOpGetResourceContents::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpGetResourceContents::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"GetResourceContents");

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (2 == m_packet.m_NumArguments)
    {        
        Ptr<MgStringCollection> resources = (MgStringCollection*)m_stream->GetObject();
        Ptr<MgStringCollection> preProcessTags = (MgStringCollection*)m_stream->GetObject();

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resources) ? L"MgStringCollection" : resources->GetLogString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == preProcessTags) ? L"MgStringCollection" : preProcessTags->GetLogString().c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        Ptr<MgStringCollection> resourceContents = m_service->GetResourceContents(resources, preProcessTags);

        // Encrypt the document if substitution pre-processing is required.
        if(preProcessTags != NULL && resourceContents != NULL && preProcessTags->GetCount() == resourceContents->GetCount())
        {
            for(INT32 i = 0; i < resourceContents->GetCount(); i ++)
            {
                STRING tag = preProcessTags->GetItem(i);

                if (MgResourcePreProcessingType::Substitution == tag)
                {
                    STRING plainText = resourceContents->GetItem(i);

                    MgCryptographyManager cryptoManager;
                    STRING cipherText = cryptoManager.EncryptString(plainText);
                    
                    resourceContents->SetItem(i, cipherText);
                }
            }
        }

        EndExecution(resourceContents);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpGetResourceContents.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_RESOURCE_SERVICE_CATCH(L"MgOpGetResourceContents.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_RESOURCE_SERVICE_THROW()
}
