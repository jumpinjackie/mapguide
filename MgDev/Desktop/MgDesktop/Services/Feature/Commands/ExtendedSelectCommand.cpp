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

MgExtendedSelectCommand::MgExtendedSelectCommand(MgResourceIdentifier* resource)
{
    CHECKNULL((MgResourceIdentifier*)resource, L"MgExtendedSelectCommand.MgExtendedSelectCommand");

    // Connect to provider
    m_connection = new MgFeatureConnection(resource);
    if ((NULL != m_connection.p) && ( m_connection->IsConnectionOpen() ))
    {
        m_providerName = m_connection->GetProviderName();
    }
    else
    {
        throw new MgdConnectionFailedException(L"MgExtendedSelectCommand.MgExtendedSelectCommand", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    // Create FdoIExtendedSelect command
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    m_command = (FdoIExtendedSelect*)fdoConn->CreateCommand(FdoCommandType_ExtendedSelect);
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.MgExtendedSelectCommand");
}

MgExtendedSelectCommand::~MgExtendedSelectCommand()
{
    m_command = NULL;
    m_filter = NULL;
}

FdoIdentifierCollection* MgExtendedSelectCommand::GetPropertyNames()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.GetPropertyNames");
    return m_command->GetPropertyNames();
}

void MgExtendedSelectCommand::SetDistinct(bool value)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.SetDistinct");
    // This operation is not supported by FdoIExtendedSelect
    // m_command->SetDistinct(value);

    // throw new MgInvalidOperationException(L"MgExtendedSelectCommand.SetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool MgExtendedSelectCommand::GetDistinct()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.GetDistinct");
    // This operation is not supported by FdoIExtendedSelect
    // return m_command->GetDistinct();

    // throw new MgInvalidOperationException(L"MgExtendedSelectCommand.GetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);

    return false;
}

void MgExtendedSelectCommand::SetFetchSize(FdoInt32 fetchSize)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.SetFetchSize");
    m_command->SetFetchSize(fetchSize);
}

FdoInt32 MgExtendedSelectCommand::GetFetchSize()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.GetFetchSize");
    return m_command->GetFetchSize();
}

FdoIdentifierCollection* MgExtendedSelectCommand::GetOrdering()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.GetOrdering");
    return m_command->GetOrdering();
}

void MgExtendedSelectCommand::SetOrderingOption(FdoOrderingOption option)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.SetOrderingOption");
    ((FdoISelect*)m_command)->SetOrderingOption(option);
}

FdoOrderingOption MgExtendedSelectCommand::GetOrderingOption()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.GetOrderingOption");
    return ((FdoISelect*)m_command)->GetOrderingOption();
}

void MgExtendedSelectCommand::SetOrderingOption(FdoString* name, FdoOrderingOption option)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.SetOrderingOption");
    m_command->SetOrderingOption(name, option);
}

FdoOrderingOption MgExtendedSelectCommand::GetOrderingOption(FdoString* name)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.GetOrderingOption");
    return m_command->GetOrderingOption(name);
}

FdoIdentifierCollection* MgExtendedSelectCommand::GetGrouping()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.GetGrouping");
    // This operation is not supported by FdoIExtendedSelect
    // return m_command->GetGrouping();

    // throw new MgInvalidOperationException(L"MgExtendedSelectCommand.GetGrouping", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgExtendedSelectCommand::SetGroupingFilter(FdoFilter* filter)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.SetGroupingFilter");
    // This operation is not supported by FdoIExtendedSelect
    // m_command->SetGroupingFilter(filter);

    // throw new MgInvalidOperationException(L"MgExtendedSelectCommand.SetGroupingFilter", __LINE__, __WFILE__, NULL, L"", NULL);
}

FdoFilter* MgExtendedSelectCommand::GetGroupingFilter()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.GetGroupingFilter");
    // This operation is not supported by FdoIExtendedSelect
    // return m_command->GetGroupingFilter(filter);

    // throw new MgInvalidOperationException(L"MgExtendedSelectCommand.GetGroupingFilter", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgExtendedSelectCommand::SetFeatureClassName(FdoString* value)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.SetFeatureClassName");
    m_command->SetFeatureClassName(value);
}

void MgExtendedSelectCommand::SetFilter(FdoString* value)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.SetFilter");
    m_command->SetFilter(value);
}

void MgExtendedSelectCommand::SetFilter(FdoFilter* value)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.SetFilter");
    m_command->SetFilter(value);

    m_filter = FDO_SAFE_ADDREF(value);
}

MgReader* MgExtendedSelectCommand::Execute()
{
    FdoPtr<FdoIScrollableFeatureReader> reader;
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.Execute");

    reader = m_command->ExecuteScrollable();
    return new MgdScrollableFeatureReader(m_connection, reader);
}

MgReader* MgExtendedSelectCommand::ExecuteWithLock()
{
    FdoPtr<FdoIFeatureReader> reader;
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.Execute");

    reader = m_command->ExecuteWithLock();
    return new MgdFeatureReader(m_connection, reader);
}

bool MgExtendedSelectCommand::IsSupportedFunction(FdoFunction* fdoFunc)
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return this->IsFdoSupportedFunction(fdoConn, fdoFunc);
}

bool MgExtendedSelectCommand::SupportsSelectGrouping()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgFeatureServiceCommand::SupportsSelectGrouping(fdoConn);
}

bool MgExtendedSelectCommand::SupportsSelectOrdering()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgFeatureServiceCommand::SupportsSelectOrdering(fdoConn);
}

bool MgExtendedSelectCommand::SupportsSelectDistinct()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgFeatureServiceCommand::SupportsSelectDistinct(fdoConn);
}

FdoFilter* MgExtendedSelectCommand::GetFilter()
{
    return FDO_SAFE_ADDREF(m_filter.p);
}

FdoJoinCriteriaCollection* MgExtendedSelectCommand::GetJoinCriteria()
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.GetJoinCriteria");
    return m_command->GetJoinCriteria();
}

void MgExtendedSelectCommand::SetAlias(FdoString* alias)
{
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.SetDistinct");
    m_command->SetAlias(alias);
}

MgReader* MgExtendedSelectCommand::ExecuteJoined(MgStringCollection* idPropNames, bool bForceOneToOne)
{
    Ptr<MgReader> ret;

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoIFeatureReader> fdoReader = m_command->Execute();
    if (bForceOneToOne)
    {
        FdoPtr<FdoStringCollection> names = MgFeatureUtil::MgToFdoStringCollection(idPropNames, false);
        FdoPtr<FdoIFeatureReader> forcedReader = new MgFdoForcedOneToOneFeatureReader(fdoReader, names); 
        ret = new MgdFeatureReader(m_connection, forcedReader);
    }
    else
    {
        ret = new MgdFeatureReader(m_connection, fdoReader);
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgExtendedSelectCommand.ExecuteJoined")

    return ret.Detach();
}
