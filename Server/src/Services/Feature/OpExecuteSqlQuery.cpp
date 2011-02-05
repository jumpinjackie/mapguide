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
#include "OpExecuteSqlQuery.h"
#include "ServerFeatureService.h"
#include "LogManager.h"
#include "Services/SqlResult.h"


///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpExecuteSqlQuery::MgOpExecuteSqlQuery()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------
MgOpExecuteSqlQuery::~MgOpExecuteSqlQuery()
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
void MgOpExecuteSqlQuery::Execute()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("  (%t) MgOpExecuteSqlQuery::Execute()\n")));

    MG_LOG_OPERATION_MESSAGE(L"ExecuteSqlQuery");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(m_packet.m_OperationVersion, m_packet.m_NumArguments);

    ACE_ASSERT(m_stream != NULL);

    if (2 == m_packet.m_NumArguments)
    {
        // Get the feature source
        Ptr<MgResourceIdentifier> resource = (MgResourceIdentifier*)m_stream->GetObject();

        // Get the SQL statement
        STRING sqlStatement;
        m_stream->GetString(sqlStatement);

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(sqlStatement.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        // Execute the operation
        Ptr<MgSqlDataReader> sqlReader = m_service->ExecuteSqlQuery(resource, sqlStatement);

        // Write the response
        EndExecution((MgSqlDataReader*)sqlReader);
    }
    else if (4 == m_packet.m_NumArguments)
    {
        // Get the feature source
        Ptr<MgResourceIdentifier> resource = (MgResourceIdentifier*)m_stream->GetObject();

        // Get the SQL statement
        STRING sqlStatement;
        m_stream->GetString(sqlStatement);

        // Get parameters binded to the SQL statement
        Ptr<MgParameterCollection> parameters = (MgParameterCollection*)m_stream->GetObject();

        // Get the transaction id
        STRING transactionId;
        m_stream->GetString(transactionId);

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(sqlStatement.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(transactionId.c_str());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        MgServerFeatureTransactionPool* transactionPool = MgServerFeatureTransactionPool::GetInstance();
        CHECKNULL(transactionPool, L"MgOpExecuteSqlQuery.Execute")

        transactionPool->ValidateTimeout(transactionId);

        // Get the MgTransaction instance from the pool if one has been started for this resource.
        Ptr<MgServerFeatureTransaction> transaction = transactionPool->GetTransaction(transactionId);

        // Execute the operation
        Ptr<MgSqlDataReader> sqlReader = m_service->ExecuteSqlQuery(resource, sqlStatement, parameters, (MgTransaction*)transaction.p);

        // Write the response
        EndExecution((MgSqlDataReader*)sqlReader);
    }
    else if (5 == m_packet.m_NumArguments)
    {
        // Get the feature source
        Ptr<MgResourceIdentifier> resource = (MgResourceIdentifier*)m_stream->GetObject();

        // Get the SQL statement
        STRING sqlStatement;
        m_stream->GetString(sqlStatement);

        // Get parameters binded to the SQL statement
        Ptr<MgParameterCollection> parameters = (MgParameterCollection*)m_stream->GetObject();

        // Get the transaction id
        STRING transactionId;
        m_stream->GetString(transactionId);

        // Get query fetch size
        INT32 fetchSize;
        m_stream->GetInt32(fetchSize);

        BeginExecution();

        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(sqlStatement.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(transactionId.c_str());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(fetchSize);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        Validate();

        MgServerFeatureTransactionPool* transactionPool = MgServerFeatureTransactionPool::GetInstance();
        CHECKNULL(transactionPool, L"MgOpExecuteSqlQuery.Execute")

        transactionPool->ValidateTimeout(transactionId);

        // Get the MgTransaction instance from the pool if one has been started for this resource.
        Ptr<MgServerFeatureTransaction> transaction = transactionPool->GetTransaction(transactionId);

        // Execute the operation
        Ptr<MgSqlDataReader> sqlReader = m_service->ExecuteSqlQuery(resource, sqlStatement, parameters, (MgTransaction*)transaction.p, fetchSize);

        MgSqlResult result(0, parameters, sqlReader);

        // Write the response
        EndExecution(&result);
    }
    else
    {
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
    }

    if (!m_argsRead)
    {
        throw new MgOperationProcessingException(L"MgOpExecuteSqlQuery.Execute",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgOpExecuteSqlQuery.Execute")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
}
