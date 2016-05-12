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
#include "ServerFeatureReader.h"
#include "ServerFeatureConnection.h"
#include "FdoFeatureReader.h"
#include "FdoFilterCollection.h"
#include "FdoReaderCollection.h"

// The maximum size of the subfilter for a selection query.  Tune this value for optimal selection perfomance.
#define MG_MAX_SUBFILTER_SIZE  250

MgSelectCommand::MgSelectCommand(MgResourceIdentifier* resource, MgFeatureQueryOptions* options)
{
    CHECKARGUMENTNULL((MgResourceIdentifier*)resource, L"MgSelectCommand.MgSelectCommand");

    // Connect to provider
    m_connection = new MgServerFeatureConnection(resource);
    if ((NULL != m_connection.p) && ( m_connection->IsConnectionOpen() ))
    {
        m_providerName = m_connection->GetProviderName();
    }
    else
    {
        throw new MgConnectionFailedException(L"MgSelectCommand.MgSelectCommand", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();

    // For SDF/SHP providers, they do not support ordering. But, they can support ordering through the FdoIExtendedSelect
    // command, provided that only a single property is being ordered on. So if it is the case that normal ordering is
    // not supported, we should try for extended select if the conditions are met.
    m_bUseExtendedSelect = false;
    if (NULL != options)
    {
        Ptr<MgStringCollection> orderProps = options->GetOrderingProperties();
        if (NULL != orderProps.p)
        {
            FdoPtr<FdoICommandCapabilities> cmdCaps = fdoConn->GetCommandCapabilities();
            if (!cmdCaps->SupportsSelectOrdering())
            {
                FdoInt32 cmdCount;
                FdoInt32* cmds = cmdCaps->GetCommands(cmdCount);
                for (FdoInt32 i = 0; i < cmdCount; i++)
                {
                    if (FdoCommandType_ExtendedSelect == cmds[i] && orderProps->GetCount() == 1)
                    {
                        m_bUseExtendedSelect = true;
                        break;
                    }
                }
            }
        }
    }

    // Create FdoISelect command
    if (m_bUseExtendedSelect)
        m_command = (FdoIExtendedSelect*)fdoConn->CreateCommand(FdoCommandType_ExtendedSelect);
    else
        m_command = (FdoISelect*)fdoConn->CreateCommand(FdoCommandType_Select);
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.MgSelectCommand");
}

MgSelectCommand::~MgSelectCommand()
{
    m_command = NULL;
    m_filter = NULL;
}

FdoIdentifierCollection* MgSelectCommand::GetPropertyNames()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetPropertyNames");
    return m_command->GetPropertyNames();
}

void MgSelectCommand::SetDistinct(bool value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetDistinct");
    // This operation is not supported by FdoISelect
    // m_command->SetDistinct(value);

    // throw new MgInvalidOperationException(L"MgSelectCommand.SetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool MgSelectCommand::GetDistinct()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetDistinct");
    // This operation is not supported by FdoISelect
    // return m_command->GetDistinct();

    // throw new MgInvalidOperationException(L"MgSelectCommand.GetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);

    return false;
}

void MgSelectCommand::SetFetchSize(FdoInt32 fetchSize)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetFetchSize");
    m_command->SetFetchSize(fetchSize);
}

FdoInt32 MgSelectCommand::GetFetchSize()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetFetchSize");
    return m_command->GetFetchSize();
}

FdoIdentifierCollection* MgSelectCommand::GetOrdering()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetOrdering");
    return m_command->GetOrdering();
}

void MgSelectCommand::SetOrderingOption(FdoOrderingOption option)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetOrderingOption");
    m_command->SetOrderingOption(option);
}

bool MgSelectCommand::IsExtended()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetExtendedOrderingOption");
    FdoIExtendedSelect* extSelect = dynamic_cast<FdoIExtendedSelect*>(m_command.p);
    if (NULL != extSelect)
    {
        return true;
    }
    return false;
}

void MgSelectCommand::SetExtendedOrderingOption(FdoString* orderBy, FdoOrderingOption option)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetExtendedOrderingOption");
    FdoIExtendedSelect* extSelect = dynamic_cast<FdoIExtendedSelect*>(m_command.p);
    if (NULL != extSelect)
    {
        extSelect->SetOrderingOption(orderBy, option);
    }
}

FdoOrderingOption MgSelectCommand::GetOrderingOption()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetOrderingOption");
    return m_command->GetOrderingOption();
}

FdoIdentifierCollection* MgSelectCommand::GetGrouping()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetGrouping");
    // This operation is not supported by FdoISelect
    // return m_command->GetGrouping();

    // throw new MgInvalidOperationException(L"MgSelectCommand.GetGrouping", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgSelectCommand::SetGroupingFilter(FdoFilter* filter)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetGroupingFilter");
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

void MgSelectCommand::SetFeatureClassName(FdoString* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetFeatureClassName");
    m_command->SetFeatureClassName(value);
}

void MgSelectCommand::SetFilter(FdoString* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetFilter");
    m_command->SetFilter(value);
}

void MgSelectCommand::SetFilter(FdoFilter* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetFilter");
    m_command->SetFilter(value);

    m_filter = FDO_SAFE_ADDREF(value);
}

MgReader* MgSelectCommand::Execute()
{
    FdoPtr<FdoIFeatureReader> reader;

    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.Execute");
    FdoIExtendedSelect* extSelect = dynamic_cast<FdoIExtendedSelect*>(m_command.p);
    if (m_bUseExtendedSelect && NULL != extSelect)
    {
        FdoPtr<FdoIScrollableFeatureReader> scReader = extSelect->ExecuteScrollable();
        CHECKNULL((FdoIScrollableFeatureReader*)scReader, L"MgSelectCommand.Execute");

        return new MgServerFeatureReader(m_connection, scReader);
    }
    else
    {
        // Break up the filter into smaller chunks
        FdoPtr<MgFdoFilterCollection> subFilters = this->GetSubFilters();

        // Execute queries using the smaller filters and collect the results of the queries into a reader collection.
        FdoPtr<MgFdoReaderCollection> frc = MgFdoReaderCollection::Create();

        for (FdoInt32 filterIndex = 0; filterIndex < subFilters->GetCount(); filterIndex++)
        {
            FdoPtr<FdoFilter> filter = subFilters->GetItem(filterIndex);
            m_command->SetFilter(filter);
            reader = m_command->Execute();

            frc->Add(reader);
        }

        FdoPtr<MgFdoFeatureReader> featureReader = new MgFdoFeatureReader(frc);
        CHECKNULL((FdoIFeatureReader*)featureReader, L"MgSelectCommand.Execute");

        return new MgServerFeatureReader(m_connection, featureReader);
    }
}

bool MgSelectCommand::IsSupportedFunction(FdoFunction* fdoFunc)
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return this->IsFdoSupportedFunction(fdoConn, fdoFunc);
}

bool MgSelectCommand::SupportsSelectGrouping()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgFeatureServiceCommand::SupportsSelectGrouping(fdoConn);
}

bool MgSelectCommand::SupportsSelectOrdering()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgFeatureServiceCommand::SupportsSelectOrdering(fdoConn);
}

bool MgSelectCommand::SupportsSelectDistinct()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgFeatureServiceCommand::SupportsSelectDistinct(fdoConn);
}

FdoFilter* MgSelectCommand::GetFilter()
{
    return FDO_SAFE_ADDREF(m_filter.p);
}

MgFdoFilterCollection* MgSelectCommand::GetSubFilters()
{
    // Break up a filter into a bunch of smaller filters

    // For now we just reduce a simple case with datastore limitations in handling the number of OR conditions.
    // This is the case where a filter has only OR spatial conditions that can be broken up into a collection
    // of smaller OR filters.

    class FdoCommonFilterFragmenter :  public virtual FdoIFilterProcessor
    {
    private:
        FdoPtr<FdoFilter>    m_newFilter;
        FdoPtr<FdoIGeometry> m_geomRight;
        FdoPtr<FdoIGeometry> m_geomLeft;

        int m_OrCount;
        std::vector<FdoFilter*> m_filters;
        bool m_isFragmented;

    protected:
        void HandleFilter( FdoFilter *filter )
        {
            filter->Process( this );
        }
    public:

        FdoCommonFilterFragmenter( ):m_isFragmented(true)
        {
            m_OrCount = 0;
            m_filters.clear();
        }

        int GetOrCount() { return m_OrCount; }
        std::vector<FdoFilter*>& GetFilters() { return m_filters; }
        bool IsFragmented() { return m_isFragmented; }
        FdoFilter* GetNewFilter() { return m_newFilter ? FDO_SAFE_ADDREF(m_newFilter.p) : NULL; }

        virtual void Dispose()
        {
            delete this;
        }

        virtual void ProcessBinaryLogicalOperator(FdoBinaryLogicalOperator& filter)
        {
            if( filter.GetOperation() != FdoBinaryLogicalOperations_Or )
            {
                m_isFragmented = false;
                return;
            }

            HandleFilter( FdoPtr<FdoFilter>(filter.GetRightOperand()) );
            m_newFilter = filter.GetLeftOperand();

            m_OrCount++;
        }
        virtual void ProcessComparisonCondition(FdoComparisonCondition& filter)
        {
            // Add filter to collection
            m_filters.push_back(&filter);
            return;
        }
        virtual void ProcessDistanceCondition(FdoDistanceCondition& filter)
        {
            m_isFragmented = false;
            return;
        }

        virtual void ProcessInCondition(FdoInCondition& filter)
        {
            m_isFragmented = false;
            return;
        }
        virtual void ProcessNullCondition(FdoNullCondition& filter)
        {
            m_isFragmented = false;
            return;
        }
        virtual void ProcessSpatialCondition(FdoSpatialCondition& filter)
        {
            m_isFragmented = false;
            return;
        }

        virtual void ProcessUnaryLogicalOperator(FdoUnaryLogicalOperator& filter)
        {
            m_isFragmented = false;
            return;
        }
    };


    FdoCommonFilterFragmenter  fragmenter;
    if (m_filter)
        m_filter->Process( &fragmenter );

    FdoPtr<FdoFilter> newFilter = fragmenter.GetNewFilter();
    while (newFilter != NULL)
    {
        newFilter->Process( &fragmenter );
        FdoPtr<FdoFilter> tempFilter = fragmenter.GetNewFilter();
        if (tempFilter != newFilter)
        {
            newFilter = tempFilter;
        }
        else
        {
            newFilter = NULL;
        }
    }

#ifdef _DEBUG
    int nCount = fragmenter.GetOrCount();
    if ( nCount > 0)
    {
        char temp[1024];
        sprintf(temp, "Or_Count = %d", nCount);  // NOXLATE
        printf("%s\n", temp);
    }
#endif

    FdoPtr<MgFdoFilterCollection> filters = MgFdoFilterCollection::Create();

    if (fragmenter.IsFragmented() && fragmenter.GetOrCount() > 0)
    {
        int nSelectionCount = 0;

        std::vector<FdoFilter*>::iterator filterIter;
        bool bFirst = true;

        FdoStringP filterString;
        std::vector<FdoFilter*>& fragmentedFilters = fragmenter.GetFilters();


        bool bIsAddedToCollection = false;

        for (filterIter = fragmentedFilters.begin(); filterIter != fragmentedFilters.end(); filterIter++)
        {
            FdoStringP tempString = (*filterIter)->ToString();
            FdoStringP orString = L" OR ";  // NOXLATE
            if (bFirst)
            {
                filterString = tempString;
                bFirst = false;
            }
            else
            {
                filterString = filterString + orString + tempString;
                nSelectionCount++;
            }

            if (nSelectionCount >= MG_MAX_SUBFILTER_SIZE)
            {
                FdoPtr<FdoFilter> filter = FdoFilter::Parse(filterString);
                filters->Add(filter);
                bFirst = true;
                filterString = L"";
                nSelectionCount = 0;
                bIsAddedToCollection = true;
            }
            else
            {
                bIsAddedToCollection = false;
            }
        }

        if ( !bIsAddedToCollection )
        {
            FdoPtr<FdoFilter> filter = FdoFilter::Parse(filterString);
            filters->Add(filter);
        }

    }
    else
    {
        filters->Add(m_filter);
    }

    return filters.Detach();
}

FdoJoinCriteriaCollection* MgSelectCommand::GetJoinCriteria()
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.GetJoinCriteria");
    return m_command->GetJoinCriteria();
}

void MgSelectCommand::SetAlias(FdoString* alias)
{
    CHECKNULL((FdoISelect*)m_command, L"MgSelectCommand.SetDistinct");
    m_command->SetAlias(alias);
}

MgReader* MgSelectCommand::ExecuteJoined(MgStringCollection* idPropNames, bool bForceOneToOne)
{
    Ptr<MgReader> ret;

    MG_FEATURE_SERVICE_TRY()

#ifdef DEBUG_FDO_JOIN
    FdoPtr<FdoIdentifierCollection> cmdPropNames = m_command->GetPropertyNames();
    for (FdoInt32 i = 0; i < cmdPropNames->GetCount(); i++)
    {
        FdoPtr<FdoIdentifier> ident = cmdPropNames->GetItem(i);
        STRING idStr = ident->ToString();
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n(%t) [FdoISelect]: (%W)"), idStr.c_str()));
    }
#endif

    FdoPtr<FdoIFeatureReader> fdoReader = m_command->Execute();
    if (bForceOneToOne)
    {
        FdoPtr<FdoStringCollection> names = MgServerFeatureUtil::MgToFdoStringCollection(idPropNames, false);
        FdoPtr<FdoIFeatureReader> forcedReader = new MgFdoForcedOneToOneFeatureReader(fdoReader, names); 
        ret = new MgServerFeatureReader(m_connection, forcedReader, idPropNames);
    }
    else
    {
        ret = new MgServerFeatureReader(m_connection, fdoReader, idPropNames);
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgSelectCommand.ExecuteJoined")

    return ret.Detach();
}
