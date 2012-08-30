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

#include "Services/Feature/FeatureDefs.h"
#include "MgDesktop.h"
#include "Services/FeatureService.h"
#include "FeatureServiceCommand.h"
#include "ExtendedSelectCommand.h"
#include "SelectAggregateCommand.h"
#include "Services/FeatureReader.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FdoFeatureReader.h"
#include "Services/Feature/FdoFilterCollection.h"
#include "Services/Feature/FdoReaderCollection.h"

// The maximum size of the subfilter for a selection query.  Tune this value for optimal selection perfomance.
#define MG_MAX_SUBFILTER_SIZE  250

MgdExtendedSelectCommand::MgdExtendedSelectCommand(MgResourceIdentifier* resource)
{
    CHECKNULL((MgResourceIdentifier*)resource, L"MgdExtendedSelectCommand.MgdExtendedSelectCommand");

    // Connect to provider
    m_connection = new MgdFeatureConnection(resource);
    if ((NULL != m_connection.p) && ( m_connection->IsConnectionOpen() ))
    {
        m_providerName = m_connection->GetProviderName();
    }
    else
    {
        throw new MgdConnectionFailedException(L"MgdExtendedSelectCommand.MgdExtendedSelectCommand", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    // Create FdoIExtendedSelect command
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    m_command = (FdoIExtendedSelect*)fdoConn->CreateCommand(FdoCommandType_ExtendedSelect);
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.MgdExtendedSelectCommand");
}

MgdExtendedSelectCommand::~MgdExtendedSelectCommand()
{
    m_command = NULL;
    m_filter = NULL;
}

FdoIdentifierCollection* MgdExtendedSelectCommand::GetPropertyNames()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.GetPropertyNames");
    return m_command->GetPropertyNames();
}

void MgdExtendedSelectCommand::SetDistinct(bool value)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.SetDistinct");
    // This operation is not supported by FdoIExtendedSelect
    // m_command->SetDistinct(value);

    // throw new MgInvalidOperationException(L"MgdExtendedSelectCommand.SetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool MgdExtendedSelectCommand::GetDistinct()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.GetDistinct");
    // This operation is not supported by FdoIExtendedSelect
    // return m_command->GetDistinct();

    // throw new MgInvalidOperationException(L"MgdExtendedSelectCommand.GetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);

    return false;
}

void MgdExtendedSelectCommand::SetFetchSize(FdoInt32 fetchSize)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.SetFetchSize");
    m_command->SetFetchSize(fetchSize);
}

FdoInt32 MgdExtendedSelectCommand::GetFetchSize()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.GetFetchSize");
    return m_command->GetFetchSize();
}

FdoIdentifierCollection* MgdExtendedSelectCommand::GetOrdering()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.GetOrdering");
    return m_command->GetOrdering();
}

void MgdExtendedSelectCommand::SetOrderingOption(FdoOrderingOption option)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.SetOrderingOption");
    ((FdoISelect*)m_command)->SetOrderingOption(option);
}

FdoOrderingOption MgdExtendedSelectCommand::GetOrderingOption()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.GetOrderingOption");
    return ((FdoISelect*)m_command)->GetOrderingOption();
}

void MgdExtendedSelectCommand::SetOrderingOption(FdoString* name, FdoOrderingOption option)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.SetOrderingOption");
    m_command->SetOrderingOption(name, option);
}

FdoOrderingOption MgdExtendedSelectCommand::GetOrderingOption(FdoString* name)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.GetOrderingOption");
    return m_command->GetOrderingOption(name);
}

FdoIdentifierCollection* MgdExtendedSelectCommand::GetGrouping()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.GetGrouping");
    // This operation is not supported by FdoIExtendedSelect
    // return m_command->GetGrouping();

    // throw new MgInvalidOperationException(L"MgdExtendedSelectCommand.GetGrouping", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgdExtendedSelectCommand::SetGroupingFilter(FdoFilter* filter)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.SetGroupingFilter");
    // This operation is not supported by FdoIExtendedSelect
    // m_command->SetGroupingFilter(filter);

    // throw new MgInvalidOperationException(L"MgdExtendedSelectCommand.SetGroupingFilter", __LINE__, __WFILE__, NULL, L"", NULL);
}

FdoFilter* MgdExtendedSelectCommand::GetGroupingFilter()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.GetGroupingFilter");
    // This operation is not supported by FdoIExtendedSelect
    // return m_command->GetGroupingFilter(filter);

    // throw new MgInvalidOperationException(L"MgdExtendedSelectCommand.GetGroupingFilter", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgdExtendedSelectCommand::SetFeatureClassName(FdoString* value)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.SetFeatureClassName");
    m_command->SetFeatureClassName(value);
}

void MgdExtendedSelectCommand::SetFilter(FdoString* value)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.SetFilter");
    m_command->SetFilter(value);
}

void MgdExtendedSelectCommand::SetFilter(FdoFilter* value)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.SetFilter");
    m_command->SetFilter(value);

    m_filter = FDO_SAFE_ADDREF(value);
}

MgReader* MgdExtendedSelectCommand::Execute()
{
    FdoPtr<FdoIScrollableFeatureReader> reader;
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.Execute");

    reader = m_command->ExecuteScrollable();
    return new MgdScrollableFeatureReader(m_connection, reader);
}

MgReader* MgdExtendedSelectCommand::ExecuteWithLock()
{
    FdoPtr<FdoIFeatureReader> reader;
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.Execute");

    reader = m_command->ExecuteWithLock();
    return new MgdFeatureReader(m_connection, reader);
}

bool MgdExtendedSelectCommand::IsSupportedFunction(FdoFunction* fdoFunc)
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return this->IsFdoSupportedFunction(fdoConn, fdoFunc);
}

bool MgdExtendedSelectCommand::SupportsSelectGrouping()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgdFeatureServiceCommand::SupportsSelectGrouping(fdoConn);
}

bool MgdExtendedSelectCommand::SupportsSelectOrdering()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgdFeatureServiceCommand::SupportsSelectOrdering(fdoConn);
}

bool MgdExtendedSelectCommand::SupportsSelectDistinct()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgdFeatureServiceCommand::SupportsSelectDistinct(fdoConn);
}

FdoFilter* MgdExtendedSelectCommand::GetFilter()
{
    return FDO_SAFE_ADDREF(m_filter.p);
}

FdoJoinCriteriaCollection* MgdExtendedSelectCommand::GetJoinCriteria()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.GetJoinCriteria");
    return m_command->GetJoinCriteria();
}

void MgdExtendedSelectCommand::SetAlias(FdoString* alias)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgdExtendedSelectCommand.SetDistinct");
    m_command->SetAlias(alias);
}

MgReader* MgdExtendedSelectCommand::ExecuteJoined(MgStringCollection* idPropNames, bool bForceOneToOne)
{
    Ptr<MgReader> ret;

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoIFeatureReader> fdoReader = m_command->Execute();
    if (bForceOneToOne)
    {
        FdoPtr<FdoStringCollection> names = MgdFeatureUtil::MgToFdoStringCollection(idPropNames, false);
        FdoPtr<FdoIFeatureReader> forcedReader = new MgdFdoForcedOneToOneFeatureReader(fdoReader, names); 
        ret = new MgdFeatureReader(m_connection, forcedReader);
    }
    else
    {
        ret = new MgdFeatureReader(m_connection, fdoReader);
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdExtendedSelectCommand.ExecuteJoined")

    return ret.Detach();
}
