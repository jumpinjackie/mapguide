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
#include "MapGuideCommon.h"
#include "Services/FeatureService.h"
#include "FeatureServiceCommand.h"
#include "SelectCommand.h"
#include "SelectAggregateCommand.h"
#include "ServerDataReader.h"
#include "ServerFeatureConnection.h"
#include "FdoForcedOneToOneFeatureReader.h"
#include "ServerFeatureUtil.h"
#include "FdoExpressionEngine.h"
#include "Util/FdoExpressionEngineUtilDataReader.h"


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

FdoJoinCriteriaCollection* MgSelectAggregateCommand::GetJoinCriteria()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.GetJoinCriteria");
    return m_command->GetJoinCriteria();
}

void MgSelectAggregateCommand::SetAlias(FdoString* alias)
{
    CHECKARGUMENTNULL(alias, L"MgSelectAggregateCommand.SetAlias");
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgSelectAggregateCommand.SetAlias");
    m_command->SetAlias(alias);
}

MgReader* MgSelectAggregateCommand::ExecuteJoined(MgStringCollection* idPropNames, bool bForceOneToOne)
{
    Ptr<MgReader> ret;

    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    FdoPtr<FdoIExtendedSelect> select = static_cast<FdoIExtendedSelect*>(fdoConn->CreateCommand(FdoCommandType_ExtendedSelect));

    FdoPtr<FdoIdentifier> clsName = m_command->GetFeatureClassName();
    select->SetFeatureClassName(clsName);

    
    STRING clsNameStr = clsName->GetText();
    STRING schemaName;
    STRING className;
    MgUtil::ParseQualifiedClassName(clsNameStr, schemaName, className);

    FdoPtr<FdoIdentifierCollection> selectedIds = GetPropertyNames();
    FdoPtr<FdoIdentifierCollection> computedIds = FdoIdentifierCollection::Create();
    for (FdoInt32 i = 0; i < selectedIds->GetCount(); i++)
    {
        FdoPtr<FdoIdentifier> identifier = selectedIds->GetItem(i);
        if (identifier->GetExpressionType() == FdoExpressionItemType_ComputedIdentifier)
        {
            FdoComputedIdentifier* comp = static_cast<FdoComputedIdentifier*>(identifier.p);
            FdoPtr<FdoExpression> expr = comp->GetExpression();
            if (expr->GetExpressionType() == FdoExpressionItemType_Function)
                computedIds->Add(identifier);
        }
    }
    FdoPtr<FdoClassDefinition> originalClassDef;

    FdoPtr<FdoIDescribeSchema> descSchema = static_cast<FdoIDescribeSchema*>(fdoConn->CreateCommand(FdoCommandType_DescribeSchema));
    if (!schemaName.empty())
    {
        descSchema->SetSchemaName(schemaName.c_str());
    }
    if (!className.empty())
    {
        FdoPtr<FdoStringCollection> classNames = FdoStringCollection::Create();
        classNames->Add(className.c_str());
        descSchema->SetClassNames(classNames);
    }

    FdoPtr<FdoClassDefinition> classDef;
    FdoPtr<FdoFeatureSchemaCollection> schemas = descSchema->Execute();
    for (FdoInt32 i = 0; i < schemas->GetCount(); i++)
    {
        FdoPtr<FdoFeatureSchema> schema = schemas->GetItem(i);
        if (wcscmp(schema->GetName(), schemaName.c_str()) == 0) 
        {
            FdoPtr<FdoClassCollection> classes = schema->GetClasses();
            for (FdoInt32 j = 0; j < classes->GetCount(); j++) 
            {
                FdoPtr<FdoClassDefinition> klassDef = classes->GetItem(j);
                if (wcscmp(klassDef->GetName(), className.c_str()) == 0)
                {
                    originalClassDef = SAFE_ADDREF(klassDef.p);
                    break;
                }
            }
        }
    }
    

    if (NULL != (FdoFilter*)m_filter)
        select->SetFilter(m_filter);

    FdoCommonExpressionType exprType;
    FdoPtr<FdoFunctionDefinitionCollection> functions = FdoExpressionEngine::GetStandardFunctions();
    FdoPtr<FdoArray<FdoFunction*> > aggrIdents = FdoExpressionEngineUtilDataReader::GetAggregateFunctions(functions, computedIds, exprType);

    FdoPtr<FdoIFeatureReader> reader;
    FdoPtr<FdoIdentifierCollection> ids;
    FdoPtr<FdoIdentifierCollection> orderBy = m_command->GetOrdering();
    FdoOrderingOption orderOpt = m_command->GetOrderingOption();

    //n00bism: String goes in, garbage comes out. Anyway, we know the alias we want to set
    select->SetAlias(L"primary");
    //select->SetAlias(m_command->GetAlias());

    FdoPtr<FdoJoinCriteriaCollection> srcCriteria = GetJoinCriteria();
    FdoPtr<FdoJoinCriteriaCollection> dstCriteria = select->GetJoinCriteria();
    for (FdoInt32 i = 0; i < srcCriteria->GetCount(); i++)
    {
        FdoPtr<FdoJoinCriteria> criteria = srcCriteria->GetItem(i);
        dstCriteria->Add(criteria);
    }

    if ((aggrIdents != NULL) && (aggrIdents->GetCount() > 0))
    {
        reader = select->Execute();
    }
    else
    {
        // transfer over the identifiers to the basic select command:
        ids = select->GetPropertyNames();
        ids->Clear();
        if (0 == selectedIds->GetCount())
        {
            FdoPtr<FdoPropertyDefinitionCollection> propDefs = originalClassDef->GetProperties();
            for (int i=0; i<propDefs->GetCount(); i++)
            {
                FdoPtr<FdoPropertyDefinition> propDef = propDefs->GetItem(i);
                FdoPtr<FdoIdentifier> localId = FdoIdentifier::Create(propDef->GetName());
                ids->Add(localId);
            }
            FdoPtr<FdoReadOnlyPropertyDefinitionCollection> basePropDefs = originalClassDef->GetBaseProperties();
            for (int i=0; i<basePropDefs->GetCount(); i++)
            {
                FdoPtr<FdoPropertyDefinition> basePropDef = basePropDefs->GetItem(i);
                FdoPtr<FdoIdentifier> localId = FdoIdentifier::Create(basePropDef->GetName());
                ids->Add(localId);
            }
        }
        else
        {
            for (int i=0; i<selectedIds->GetCount(); i++)
            {
                FdoPtr<FdoIdentifier> localId = selectedIds->GetItem(i);
                ids->Add(localId);
            }
        }

        // Execute the basic select command:
        reader = select->Execute();
    }

    if (bForceOneToOne)
    {
        FdoPtr<FdoStringCollection> names = MgServerFeatureUtil::MgToFdoStringCollection(idPropNames, false);
        FdoPtr<FdoIFeatureReader> forcedReader = new MgFdoForcedOneToOneFeatureReader(reader, names);
        FdoPtr<FdoIDataReader> dataReader = new FdoExpressionEngineUtilDataReader(functions, forcedReader, originalClassDef, computedIds, GetDistinct(), orderBy, orderOpt, ids, aggrIdents);

        ret = new MgServerDataReader(m_connection, dataReader, m_providerName);
    }
    else
    {
        FdoPtr<FdoIDataReader> dataReader = new FdoExpressionEngineUtilDataReader(functions, reader, originalClassDef, computedIds, GetDistinct(), orderBy, orderOpt, ids, aggrIdents);

        ret = new MgServerDataReader(m_connection, dataReader, m_providerName);
    }

    return ret.Detach();
}