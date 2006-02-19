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
#include "ServerFeatureReader.h"
#include "ServerFeatureReaderIdentifier.h"
#include "ServerFeatureConnection.h"

MgSelectCommand::MgSelectCommand(MgResourceIdentifier* resource) : m_filter(NULL)
{
    CHECKNULL((MgResourceIdentifier*)resource, L"MgSelectCommand.MgSelectCommand");

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
    // Create FdoISelectAggregate command

    m_command = (FdoISelect*)m_fdoConn->CreateCommand(FdoCommandType_Select);
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.MgSelectCommand");
}

MgSelectCommand::~MgSelectCommand()
{
    if (NULL != m_fdoConn)
        m_fdoConn->Release();

    if (NULL != m_command)
        m_command->Release();
}

FdoIdentifierCollection* MgSelectCommand::GetPropertyNames()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetPropertyNames");
    return m_command->GetPropertyNames();
}

void MgSelectCommand::SetDistinct(bool value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetPropertyNames");
    // This operation is not supported by FdoISelect
    // m_command->SetDistinct(value);

    // throw new MgInvalidOperationException(L"MgSelectCommand.SetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool MgSelectCommand::GetDistinct()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetDistinct");
    // This operation is not supported by FdoISelect
    // return m_command->GetDistinct();

    // throw new MgInvalidOperationException(L"MgSelectCommand.SetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);

    return false;
}

FdoIdentifierCollection* MgSelectCommand::GetOrdering()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetOrdering");
    return m_command->GetOrdering();
}

void MgSelectCommand::SetOrderingOption(FdoOrderingOption option)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetOrdering");
    m_command->SetOrderingOption(option);
}

FdoOrderingOption MgSelectCommand::GetOrderingOption()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetOrdering");
    return m_command->GetOrderingOption();
}

FdoIdentifierCollection* MgSelectCommand::GetGrouping()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetDistinct");
    // This operation is not supported by FdoISelect
    // return m_command->GetGrouping();

    // throw new MgInvalidOperationException(L"MgSelectCommand.SetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgSelectCommand::SetGroupingFilter(FdoFilter* filter)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetDistinct");
    // This operation is not supported by FdoISelect
    // m_command->SetGroupingFilter(filter);

    // throw new MgInvalidOperationException(L"MgSelectCommand.SetGroupingFilter", __LINE__, __WFILE__, NULL, L"", NULL);
}

FdoFilter* MgSelectCommand::GetGroupingFilter()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetGroupingFilter");
    // This operation is not supported by FdoISelect
    // return m_command->GetGroupingFilter(filter);

    // throw new MgInvalidOperationException(L"MgSelectCommand.GetGroupingFilter", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgSelectCommand::SetFeatureClassName(GisString* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetFeatureClassName");
    m_command->SetFeatureClassName(value);
}

void MgSelectCommand::SetFilter(GisString* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetFilter");
    m_command->SetFilter(value);
}

void MgSelectCommand::SetFilter(FdoFilter* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetFilter");
    m_command->SetFilter(value);
    m_filter = GIS_SAFE_ADDREF((FdoFilter*)value);
}

MgReader* MgSelectCommand::Execute()
{
    Ptr<MgFeatureReader> mgFeatureReader = (MgFeatureReader*)NULL;

    // Execute the command
    GisPtr<FdoIFeatureReader> featureReader = m_command->Execute();
    CHECKNULL((FdoIFeatureReader*)featureReader, L"MgSelectCommand.Execute");

    // Create a feature reader identifier
    Ptr<MgServerFeatureReaderIdentifier> featReaderId =
            new MgServerFeatureReaderIdentifier(featureReader);

    // TODO: This needs to be tied back to the original FDO connection that is cached so that if the original
    //       FDO connection is ever removed from the cache then the associated MgServerFeatureReaderIdentifier
    //       is also removed from it's cache.


    // TODO: Add this identifier to FdoConnectionPool along with connection for next fetch, if required.
    // TODO: MgServerGetFeatures will tell us that whether we need to cache this or not based on if there
    // TODO: can be possible more record available. Need to look into FdoConnectionPoolManager to find out
    // TODO: how can we pool the connections and MgServerFeatureReaderIdentifier

    mgFeatureReader = new MgServerFeatureReader(featReaderId);

    return SAFE_ADDREF((MgFeatureReader*)mgFeatureReader);
}

bool MgSelectCommand::IsSupportedFunction(FdoFunction* fdoFunc)
{
    return this->IsFdoSupportedFunction(m_fdoConn, fdoFunc);
}

bool MgSelectCommand::SupportsSelectGrouping()
{
    return MgFeatureServiceCommand::SupportsSelectGrouping(m_fdoConn);
}

bool MgSelectCommand::SupportsSelectOrdering()
{
    return MgFeatureServiceCommand::SupportsSelectOrdering(m_fdoConn);
}

bool MgSelectCommand::SupportsSelectDistinct()
{
    return MgFeatureServiceCommand::SupportsSelectDistinct(m_fdoConn);
}

FdoFilter* MgSelectCommand::GetFilter()
{
    return m_filter;
}
