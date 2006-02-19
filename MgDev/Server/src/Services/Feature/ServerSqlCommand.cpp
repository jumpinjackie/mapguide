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
#include "FeatureService.h"
#include "ServerSqlCommand.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureReaderIdentifier.h"
#include "ServerSqlProcessor.h"
#include "ServerSqlDataReader.h"
#include "ServerFeatureUtil.h"

#define CHECKOPEN(methodName) \
    if (!IsConnectionOpen())  \
        throw new MgConnectionNotOpenException(methodName, __LINE__, __WFILE__, NULL, L"", NULL); \

MgServerSqlCommand::MgServerSqlCommand()
{
    m_fdoConn = NULL;
}

MgServerSqlCommand::~MgServerSqlCommand()
{
    if (NULL != m_fdoConn)
        m_fdoConn->Release();
}

// Executes the describe schema command and serializes the schema to XML
MgSqlDataReader* MgServerSqlCommand::ExecuteQuery(MgResourceIdentifier* resource, CREFSTRING sqlStatement)
{
    Ptr<MgSqlDataReader> mgSqlDataReader = (MgSqlDataReader*)NULL;

    MG_FEATURE_SERVICE_TRY()

    // If validation does succeeds, it will throw exception
    Validate(resource, sqlStatement, FdoCommandType_SQLCommand);

    // Create the SQL command
    GisPtr<FdoISQLCommand> fdoCommand = (FdoISQLCommand*)m_fdoConn->CreateCommand(FdoCommandType_SQLCommand);
    CHECKNULL((FdoISQLCommand*)fdoCommand, L"MgServerSqlCommand.ExecuteQuery");

    fdoCommand->SetSQLStatement((GisString*)sqlStatement.c_str());

    // Execute the command
    GisPtr<FdoISQLDataReader> sqlReader = fdoCommand->ExecuteReader();
    CHECKNULL((FdoISQLDataReader*)sqlReader, L"MgServerSqlCommand.ExecuteQuery");

    mgSqlDataReader = new MgServerSqlDataReader((FdoISQLDataReader*)sqlReader, m_providerName);
    CHECKNULL((MgSqlDataReader*)mgSqlDataReader, L"MgServerSqlCommand.ExecuteQuery");

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlCommand.ExecuteQuery")

    return SAFE_ADDREF((MgSqlDataReader*)mgSqlDataReader);
}

// Executes the describe schema command and serializes the schema to XML
INT32 MgServerSqlCommand::ExecuteNonQuery(MgResourceIdentifier* resource, CREFSTRING sqlStatement)
{
    Ptr<MgSqlDataReader> mgSqlDataReader = (MgSqlDataReader*)NULL;
    INT32 rowsAffected = 0;

    MG_FEATURE_SERVICE_TRY()

    // If validation does succeeds, it will throw exception
    Validate(resource, sqlStatement, FdoCommandType_SQLCommand);

    // Create the SQL command
    GisPtr<FdoISQLCommand> fdoCommand = (FdoISQLCommand*)m_fdoConn->CreateCommand(FdoCommandType_SQLCommand);
    CHECKNULL((FdoISQLCommand*)fdoCommand, L"MgServerSqlCommand.ExecuteQuery");

    fdoCommand->SetSQLStatement((GisString*)sqlStatement.c_str());

    // Execute the command
    rowsAffected = fdoCommand->ExecuteNonQuery();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlCommand.ExecuteQuery")

    return rowsAffected;
}


void MgServerSqlCommand::Validate(MgResourceIdentifier* resource, CREFSTRING sqlStatement, INT32 commandType)
{
    // SQL statement can not be empty
    if (resource == NULL)
    {
        throw new MgNullArgumentException(L"MgServerSqlCommand.MgServerSqlCommand()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (sqlStatement.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgServerSqlCommand.MgServerSqlCommand()",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    // Connect to provider
    MgServerFeatureConnection msfc(resource);
    if ( msfc.IsConnectionOpen() )
    {
        m_fdoConn = msfc.GetConnection();
        m_providerName = msfc.GetProviderName();
    }
    else
    {
        throw new MgConnectionFailedException(L"MgServerSqlCommand::SelectFeatures()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Check whether command is supported by provider
    if (!msfc.SupportsCommand(commandType))
    {
        // TODO: specify which argument and message, once we have the mechanism
        STRING message = MgServerFeatureUtil::GetMessage(L"MgCommandNotSupported");
        throw new MgInvalidOperationException(L"MgServerSqlCommand.Validate", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}
