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
#include "SelectCommand.h"
#include "SelectAggregateCommand.h"
#include "Services/DataReader.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FdoForcedOneToOneFeatureReader.h"
#include "Services/Feature/FeatureUtil.h"
#include "FdoExpressionEngine.h"
#include "Util/FdoExpressionEngineUtilDataReader.h"


MgdSelectAggregateCommand::MgdSelectAggregateCommand(MgResourceIdentifier* resource)
{
    CHECKNULL((MgResourceIdentifier*)resource, L"MgdSelectAggregateCommand.MgdSelectAggregateCommand");

    // Connect to provider
    m_connection = new MgdFeatureConnection(resource);
    if ((NULL != m_connection.p) && ( m_connection->IsConnectionOpen() ))
    {
        m_providerName = m_connection->GetProviderName();
    }
    else
    {
        throw new MgdConnectionFailedException(L"MgdSelectAggregateCommand.MgdSelectAggregateCommand", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    // Create FdoISelectAggregates command
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    m_command = (FdoISelectAggregates*)fdoConn->CreateCommand(FdoCommandType_SelectAggregates);
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.MgdSelectAggregateCommand");
}

MgdSelectAggregateCommand::~MgdSelectAggregateCommand()
{
    m_command = NULL;
    m_filter = NULL;
}

FdoIdentifierCollection* MgdSelectAggregateCommand::GetPropertyNames()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.GetPropertyNames");
    return m_command->GetPropertyNames();
}

void MgdSelectAggregateCommand::SetDistinct(bool value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.SetDistinct");
    m_command->SetDistinct(value);
}

bool MgdSelectAggregateCommand::GetDistinct()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.GetDistinct");
    return m_command->GetDistinct();
}

void MgdSelectAggregateCommand::SetFetchSize(FdoInt32 fetchSize)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.SetFetchSize");
    m_command->SetFetchSize(fetchSize);
}

FdoInt32 MgdSelectAggregateCommand::GetFetchSize()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.GetFetchSize");
    return m_command->GetFetchSize();
}

FdoIdentifierCollection* MgdSelectAggregateCommand::GetOrdering()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.GetOrdering");
    return m_command->GetOrdering();
}

void MgdSelectAggregateCommand::SetOrderingOption(FdoOrderingOption option)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.SetOrderingOption");
    m_command->SetOrderingOption(option);
}

FdoOrderingOption MgdSelectAggregateCommand::GetOrderingOption()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.GetOrderingOption");
    return m_command->GetOrderingOption();
}

FdoIdentifierCollection* MgdSelectAggregateCommand::GetGrouping()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.GetGrouping");
    return m_command->GetGrouping();
}

void MgdSelectAggregateCommand::SetGroupingFilter(FdoFilter* filter)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.SetGroupingFilter");
    m_command->SetGroupingFilter(filter);
}

FdoFilter* MgdSelectAggregateCommand::GetGroupingFilter()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.GetGroupingFilter");
    return m_command->GetGroupingFilter();
}

void MgdSelectAggregateCommand::SetFeatureClassName(FdoString* value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.SetFeatureClassName");
    m_command->SetFeatureClassName(value);
}

void MgdSelectAggregateCommand::SetFilter(FdoString* value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.SetFilter");
    m_command->SetFilter(value);
}

void MgdSelectAggregateCommand::SetFilter(FdoFilter* value)
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.SetFilter");
    m_command->SetFilter(value);

    m_filter = FDO_SAFE_ADDREF(value);
}

MgReader* MgdSelectAggregateCommand::Execute()
{
    // Execute the command
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.Execute");
    FdoPtr<FdoIDataReader> dataReader = m_command->Execute();
    CHECKNULL((FdoIDataReader*)dataReader, L"MgdSelectAggregateCommand.Execute");

    // Create a feature reader identifier
    return new MgdDataReader(m_connection, dataReader); //, m_providerName);
}

bool MgdSelectAggregateCommand::IsSupportedFunction(FdoFunction* fdoFunc)
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return this->IsFdoSupportedFunction(fdoConn, fdoFunc);
}

bool MgdSelectAggregateCommand::SupportsSelectGrouping()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgdFeatureServiceCommand::SupportsSelectGrouping(fdoConn);
}

bool MgdSelectAggregateCommand::SupportsSelectOrdering()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgdFeatureServiceCommand::SupportsSelectOrdering(fdoConn);
}

bool MgdSelectAggregateCommand::SupportsSelectDistinct()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgdFeatureServiceCommand::SupportsSelectDistinct(fdoConn);
}

FdoFilter* MgdSelectAggregateCommand::GetFilter()
{
    return FDO_SAFE_ADDREF(m_filter.p);
}

FdoJoinCriteriaCollection* MgdSelectAggregateCommand::GetJoinCriteria()
{
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.GetJoinCriteria");
    return m_command->GetJoinCriteria();
}

void MgdSelectAggregateCommand::SetAlias(FdoString* alias)
{
    CHECKARGUMENTNULL(alias, L"MgdSelectAggregateCommand.SetAlias");
    CHECKNULL((FdoISelectAggregates*)m_command, L"MgdSelectAggregateCommand.SetAlias");
    m_command->SetAlias(alias);
}

MgReader* MgdSelectAggregateCommand::ExecuteJoined(MgStringCollection* idPropNames, bool bForceOneToOne)
{
    Ptr<MgReader> ret;

    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    FdoPtr<FdoISelect> select = static_cast<FdoISelect*>(fdoConn->CreateCommand(FdoCommandType_Select));

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
    select->SetAlias(PRIMARY_ALIAS);
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
        FdoPtr<FdoStringCollection> names = MgdFeatureUtil::MgToFdoStringCollection(idPropNames, false);
        FdoPtr<FdoIFeatureReader> forcedReader = new MgdFdoForcedOneToOneFeatureReader(reader, names);
        FdoPtr<FdoIDataReader> dataReader = new FdoExpressionEngineUtilDataReader(functions, forcedReader, originalClassDef, computedIds, GetDistinct(), orderBy, orderOpt, ids, aggrIdents);

        ret = new MgdDataReader(m_connection, dataReader); //, m_providerName);
    }
    else
    {
        FdoPtr<FdoIDataReader> dataReader = new FdoExpressionEngineUtilDataReader(functions, reader, originalClassDef, computedIds, GetDistinct(), orderBy, orderOpt, ids, aggrIdents);

        ret = new MgdDataReader(m_connection, dataReader); //, m_providerName);
    }

    return ret.Detach();
}
