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
#include "Services/FeatureService.h"
#include "ServerSqlCommand.h"
#include "ServerFeatureConnection.h"
#include "ServerSqlDataReader.h"
#include "ServerFeatureUtil.h"
#include "ServerFeatureTransaction.h"

MgServerSqlCommand::MgServerSqlCommand()
{
}

MgServerSqlCommand::~MgServerSqlCommand()
{
    MG_TRY()

    CloseConnection();

    MG_CATCH_AND_RELEASE()
}

void MgServerSqlCommand::CloseConnection()
{
    // The FDO connection must be released before the parent object is released
    m_fdoConn = NULL;
    m_featureConnection = NULL;
}

// Executes the describe schema command and serializes the schema to XML
MgSqlDataReader* MgServerSqlCommand::ExecuteQuery(
    MgResourceIdentifier* resource,
    CREFSTRING sqlStatement,
    MgParameterCollection* params,
    MgTransaction* transaction,
    INT32 fetchSize)
{
    Ptr<MgSqlDataReader> mgSqlDataReader;

    MG_FEATURE_SERVICE_TRY()

    // If validation does succeeds, it will throw exception
    Validate(resource, sqlStatement, FdoCommandType_SQLCommand, transaction);

    // Create the SQL command
    FdoPtr<FdoISQLCommand> fdoCommand = (FdoISQLCommand*)m_fdoConn->CreateCommand(FdoCommandType_SQLCommand);
    CHECKNULL((FdoISQLCommand*)fdoCommand, L"MgServerSqlCommand.ExecuteQuery");

    // Set SQL statement
    fdoCommand->SetSQLStatement((FdoString*)sqlStatement.c_str());

    // Set fetch size
    fdoCommand->SetFetchSize(fetchSize);

    // Set parameters
    FdoPtr<FdoParameterValueCollection> fdoParams = NULL;
    if (NULL != params && params->GetCount() > 0)
    {
        fdoParams = fdoCommand->GetParameterValues();
        MgServerFeatureUtil::FillFdoParameterCollection(params, fdoParams);
    }

    // Execute the command
    FdoPtr<FdoISQLDataReader> sqlReader = fdoCommand->ExecuteReader();
    CHECKNULL((FdoISQLDataReader*)sqlReader, L"MgServerSqlCommand.ExecuteQuery");

    // Update parameter whose direction is InputOutput, Output, or Return.
    if (NULL != params && params->GetCount() > 0)
        MgServerFeatureUtil::UpdateParameterCollection(fdoParams, params);

    mgSqlDataReader = new MgServerSqlDataReader(m_featureConnection, sqlReader, m_providerName);
    CHECKNULL((MgSqlDataReader*)mgSqlDataReader, L"MgServerSqlCommand.ExecuteQuery");

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgServerSqlCommand.ExecuteQuery")

    return mgSqlDataReader.Detach();
}

// Executes the describe schema command and serializes the schema to XML
INT32 MgServerSqlCommand::ExecuteNonQuery(
    MgResourceIdentifier* resource,
    CREFSTRING sqlStatement,
    MgParameterCollection* params,
    MgTransaction* transaction)
{
    INT32 rowsAffected = 0;

    MG_FEATURE_SERVICE_TRY()

    // If validation does succeeds, it will throw exception
    Validate(resource, sqlStatement, FdoCommandType_SQLCommand, transaction);

    // Create the SQL command
    FdoPtr<FdoISQLCommand> fdoCommand = (FdoISQLCommand*)m_fdoConn->CreateCommand(FdoCommandType_SQLCommand);
    CHECKNULL((FdoISQLCommand*)fdoCommand, L"MgServerSqlCommand.ExecuteQuery");

    // Set SQL statement
    fdoCommand->SetSQLStatement((FdoString*)sqlStatement.c_str());

    // Set parameters
    FdoPtr<FdoParameterValueCollection> fdoParams = NULL;
    if (NULL != params && params->GetCount() > 0)
    {
        fdoParams = fdoCommand->GetParameterValues();
        MgServerFeatureUtil::FillFdoParameterCollection(params, fdoParams);
    }

    // Execute the command
    rowsAffected = fdoCommand->ExecuteNonQuery();

    // Update parameter whose direction is InputOutput, Output, or Return.
    if (NULL != params && params->GetCount() > 0)
        MgServerFeatureUtil::UpdateParameterCollection(fdoParams, params);

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgServerSqlCommand.ExecuteQuery")

    return rowsAffected;
}


void MgServerSqlCommand::Validate(MgResourceIdentifier* resource, CREFSTRING sqlStatement, INT32 commandType, MgTransaction* transaction)
{
    // SQL statement can not be empty
    if (resource == NULL)
    {
        throw new MgNullArgumentException(L"MgServerSqlCommand.Validate", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (sqlStatement.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgServerSqlCommand.Validate",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    // Close any previously opened connection.
    CloseConnection();

    // Connect to provider
    if (NULL != transaction)
    {
        // grab the connection used to start the transaction.
        MgServerFeatureTransaction* featureTransaction = static_cast<MgServerFeatureTransaction*>(transaction);
        m_featureConnection = featureTransaction->GetServerFeatureConnection();
    }
    else
    {
        // No transaction, grab the connection as usual.
        m_featureConnection = new MgServerFeatureConnection(resource);
    }

    if (m_featureConnection->IsConnectionOpen() )
    {
        m_fdoConn = m_featureConnection->GetConnection();
        m_providerName = m_featureConnection->GetProviderName();

        // Check whether command is supported by provider
        if (!m_featureConnection->SupportsCommand(commandType))
        {
            // TODO: specify which argument and message, once we have the mechanism
            STRING message = MgServerFeatureUtil::GetMessage(L"MgCommandNotSupported");
            throw new MgInvalidOperationException(L"MgServerSqlCommand.Validate", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        throw new MgConnectionFailedException(L"MgServerSqlCommand::Validate", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}
