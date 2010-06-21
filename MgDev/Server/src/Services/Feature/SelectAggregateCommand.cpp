//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "MapGuideCommon.h"
#include "Services/FeatureService.h"
#include "FeatureServiceCommand.h"
#include "SelectCommand.h"
#include "SelectAggregateCommand.h"
#include "ServerDataReader.h"
#include "ServerFeatureConnection.h"

MgSelectAggregateCommand::MgSelectAggregateCommand(MgResourceIdentifier* resource)
{
    CHECKNULL((MgResourceIdentifier*)resource, L"MgSelectAggregateCommand.MgSelectAggregateCommand");

    // Connect to provider
    m_connection = new MgServerFeatureConnection(resource);
    if ((NULL != m_connection.p) && ( m_connection->IsConnectionOpen() ))
    {
        m_providerName = m_connection->GetProviderName();
    }
    else
    {
        throw new MgConnectionFailedException(L"MgSelectAggregateCommand.MgSelectAggregateCommand", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    // Create FdoISelectAggregates command
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    m_command = (FdoISelectAggregates*)fdoConn->CreateCommand(FdoCommandType_SelectAggregates);
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.MgSelectAggregateCommand");
}

MgSelectAggregateCommand::~MgSelectAggregateCommand()
{
    m_command = NULL;
    m_filter = NULL;
}

FdoIdentifierCollection* MgSelectAggregateCommand::GetPropertyNames()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetPropertyNames");
    return m_command->GetPropertyNames();
}

void MgSelectAggregateCommand::SetDistinct(bool value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetDistinct");
    m_command->SetDistinct(value);
}

bool MgSelectAggregateCommand::GetDistinct()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetDistinct");
    return m_command->GetDistinct();
}

void MgSelectAggregateCommand::SetFetchSize(FdoInt32 fetchSize)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetFetchSize");
    m_command->SetFetchSize(fetchSize);
}

FdoInt32 MgSelectAggregateCommand::GetFetchSize()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetFetchSize");
    return m_command->GetFetchSize();
}

FdoIdentifierCollection* MgSelectAggregateCommand::GetOrdering()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetOrdering");
    return m_command->GetOrdering();
}

void MgSelectAggregateCommand::SetOrderingOption(FdoOrderingOption option)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetOrderingOption");
    m_command->SetOrderingOption(option);
}

FdoOrderingOption MgSelectAggregateCommand::GetOrderingOption()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetOrderingOption");
    return m_command->GetOrderingOption();
}

FdoIdentifierCollection* MgSelectAggregateCommand::GetGrouping()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetGrouping");
    return m_command->GetGrouping();
}

void MgSelectAggregateCommand::SetGroupingFilter(FdoFilter* filter)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetGroupingFilter");
    m_command->SetGroupingFilter(filter);
}

FdoFilter* MgSelectAggregateCommand::GetGroupingFilter()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetGroupingFilter");
    return m_command->GetGroupingFilter();
}

void MgSelectAggregateCommand::SetFeatureClassName(FdoString* value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetFeatureClassName");
    m_command->SetFeatureClassName(value);
}

void MgSelectAggregateCommand::SetFilter(FdoString* value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetFilter");
    m_command->SetFilter(value);
}

void MgSelectAggregateCommand::SetFilter(FdoFilter* value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetFilter");
    m_command->SetFilter(value);

    m_filter = FDO_SAFE_ADDREF(value);
}

MgReader* MgSelectAggregateCommand::Execute()
{
    // Execute the command
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.Execute");
    FdoPtr<FdoIDataReader> dataReader = m_command->Execute();
    CHECKNULL((FdoIDataReader*)dataReader, L"MgSelectAggregateCommand.Execute");

    // Create a feature reader identifier
    return new MgServerDataReader(m_connection, dataReader, m_providerName);
}

bool MgSelectAggregateCommand::IsSupportedFunction(FdoFunction* fdoFunc)
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return this->IsFdoSupportedFunction(fdoConn, fdoFunc);
}

bool MgSelectAggregateCommand::SupportsSelectGrouping()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgFeatureServiceCommand::SupportsSelectGrouping(fdoConn);
}

bool MgSelectAggregateCommand::SupportsSelectOrdering()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgFeatureServiceCommand::SupportsSelectOrdering(fdoConn);
}

bool MgSelectAggregateCommand::SupportsSelectDistinct()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgFeatureServiceCommand::SupportsSelectDistinct(fdoConn);
}

FdoFilter* MgSelectAggregateCommand::GetFilter()
{
    return FDO_SAFE_ADDREF(m_filter.p);
}
