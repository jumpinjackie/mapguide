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
#include "AceCommon.h"
#include "FeatureService.h"
#include "FeatureServiceCommand.h"
#include "SelectCommand.h"
#include "SelectAggregateCommand.h"
#include "ServerDataReader.h"
#include "ServerFeatureConnection.h"

MgSelectAggregateCommand::MgSelectAggregateCommand(MgResourceIdentifier* resource) : m_filter(NULL)
{
    CHECKNULL((MgResourceIdentifier*)resource, L"MgSelectAggregateCommand.MgSelectAggregateCommand");

    // Connect to provider
    MgServerFeatureConnection msfc(resource);
    if ( msfc.IsConnectionOpen() )
    {
        m_fdoConn = msfc.GetConnection();
        m_providerName = msfc.GetProviderName();
    }
    else
    {
        throw new MgConnectionFailedException(L"MgServerSelectFeatures::SelectFeatures()", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    // Create FdoISelectAggregates command

    m_command = (FdoISelectAggregates*)m_fdoConn->CreateCommand(FdoCommandType_SelectAggregates);
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.MgSelectAggregateCommand");
}

MgSelectAggregateCommand::~MgSelectAggregateCommand()
{
    if (NULL != m_fdoConn)
        m_fdoConn->Release();

    if (NULL != m_command)
        m_command->Release();
}

FdoIdentifierCollection* MgSelectAggregateCommand::GetPropertyNames()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetPropertyNames");
    return m_command->GetPropertyNames();
}

void MgSelectAggregateCommand::SetDistinct(bool value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetPropertyNames");
    m_command->SetDistinct(value);
}

bool MgSelectAggregateCommand::GetDistinct()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetDistinct");
    return m_command->GetDistinct();
}

FdoIdentifierCollection* MgSelectAggregateCommand::GetOrdering()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetOrdering");
    return m_command->GetOrdering();
}

void MgSelectAggregateCommand::SetOrderingOption(FdoOrderingOption option)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetOrdering");
    m_command->SetOrderingOption(option);
}

FdoOrderingOption MgSelectAggregateCommand::GetOrderingOption()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetOrdering");
    return m_command->GetOrderingOption();
}

FdoIdentifierCollection* MgSelectAggregateCommand::GetGrouping()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetDistinct");
    return m_command->GetGrouping();
}

void MgSelectAggregateCommand::SetGroupingFilter(FdoFilter* filter)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetDistinct");
    m_command->SetGroupingFilter(filter);
}

FdoFilter* MgSelectAggregateCommand::GetGroupingFilter()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetGroupingFilter");
    return m_command->GetGroupingFilter();
}

void MgSelectAggregateCommand::SetFeatureClassName(GisString* value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetFeatureClassName");
    m_command->SetFeatureClassName(value);
}

void MgSelectAggregateCommand::SetFilter(GisString* value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetFilter");
    m_command->SetFilter(value);
}

void MgSelectAggregateCommand::SetFilter(FdoFilter* value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetFilter");
    m_command->SetFilter(value);
    m_filter = GIS_SAFE_ADDREF((FdoFilter*)value);
}

MgReader* MgSelectAggregateCommand::Execute()
{
    Ptr<MgDataReader> mgDataReader = (MgDataReader*)NULL;

    // Execute the command
    GisPtr<FdoIDataReader> dataReader = m_command->Execute();
    CHECKNULL((FdoIDataReader*)dataReader, L"MgSelectAggregateCommand.Execute");

    // Create a feature reader identifier
    mgDataReader = new MgServerDataReader(dataReader, m_providerName);

    return SAFE_ADDREF((MgDataReader*)mgDataReader);
}

bool MgSelectAggregateCommand::IsSupportedFunction(FdoFunction* fdoFunc)
{
    return this->IsFdoSupportedFunction(m_fdoConn, fdoFunc);
}

bool MgSelectAggregateCommand::SupportsSelectGrouping()
{
    return MgFeatureServiceCommand::SupportsSelectGrouping(m_fdoConn);
}

bool MgSelectAggregateCommand::SupportsSelectOrdering()
{
    return MgFeatureServiceCommand::SupportsSelectOrdering(m_fdoConn);
}

bool MgSelectAggregateCommand::SupportsSelectDistinct()
{
    return MgFeatureServiceCommand::SupportsSelectDistinct(m_fdoConn);
}

FdoFilter* MgSelectAggregateCommand::GetFilter()
{
    return m_filter;
}
