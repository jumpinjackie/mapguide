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
#include "Services/FeatureReader.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FdoFeatureReader.h"
#include "Services/Feature/FdoFilterCollection.h"
#include "Services/Feature/FdoReaderCollection.h"

// The maximum size of the subfilter for a selection query.  Tune this value for optimal selection perfomance.
#define MG_MAX_SUBFILTER_SIZE  250

MgdSelectCommand::MgdSelectCommand(MgResourceIdentifier* resource)
{
    CHECKNULL((MgResourceIdentifier*)resource, L"MgdSelectCommand.MgdSelectCommand");

    // Connect to provider
    m_connection = new MgdFeatureConnection(resource);
    if ((NULL != m_connection.p) && ( m_connection->IsConnectionOpen() ))
    {
        m_providerName = m_connection->GetProviderName();
    }
    else
    {
        throw new MgdConnectionFailedException(L"MgdSelectCommand.MgdSelectCommand", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    // Create FdoISelect command
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    m_command = (FdoISelect*)fdoConn->CreateCommand(FdoCommandType_Select);
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.MgdSelectCommand");
}

MgdSelectCommand::~MgdSelectCommand()
{
    m_command = NULL;
    m_filter = NULL;
}

FdoIdentifierCollection* MgdSelectCommand::GetPropertyNames()
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.GetPropertyNames");
    return m_command->GetPropertyNames();
}

void MgdSelectCommand::SetDistinct(bool value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.SetDistinct");
    // This operation is not supported by FdoISelect
    // m_command->SetDistinct(value);

    // throw new MgInvalidOperationException(L"MgdSelectCommand.SetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool MgdSelectCommand::GetDistinct()
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.GetDistinct");
    // This operation is not supported by FdoISelect
    // return m_command->GetDistinct();

    // throw new MgInvalidOperationException(L"MgdSelectCommand.GetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);

    return false;
}

void MgdSelectCommand::SetFetchSize(FdoInt32 fetchSize)
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.SetFetchSize");
    m_command->SetFetchSize(fetchSize);
}

FdoInt32 MgdSelectCommand::GetFetchSize()
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.GetFetchSize");
    return m_command->GetFetchSize();
}

FdoIdentifierCollection* MgdSelectCommand::GetOrdering()
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.GetOrdering");
    return m_command->GetOrdering();
}

void MgdSelectCommand::SetOrderingOption(FdoOrderingOption option)
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.SetOrderingOption");
    m_command->SetOrderingOption(option);
}

FdoOrderingOption MgdSelectCommand::GetOrderingOption()
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.GetOrderingOption");
    return m_command->GetOrderingOption();
}

FdoIdentifierCollection* MgdSelectCommand::GetGrouping()
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.GetGrouping");
    // This operation is not supported by FdoISelect
    // return m_command->GetGrouping();

    // throw new MgInvalidOperationException(L"MgdSelectCommand.GetGrouping", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgdSelectCommand::SetGroupingFilter(FdoFilter* filter)
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.SetGroupingFilter");
    // This operation is not supported by FdoISelect
    // m_command->SetGroupingFilter(filter);

    // throw new MgInvalidOperationException(L"MgdSelectCommand.SetGroupingFilter", __LINE__, __WFILE__, NULL, L"", NULL);
}

FdoFilter* MgdSelectCommand::GetGroupingFilter()
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.GetGroupingFilter");
    // This operation is not supported by FdoISelect
    // return m_command->GetGroupingFilter(filter);

    // throw new MgInvalidOperationException(L"MgdSelectCommand.GetGroupingFilter", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgdSelectCommand::SetFeatureClassName(FdoString* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.SetFeatureClassName");
    m_command->SetFeatureClassName(value);
}

void MgdSelectCommand::SetFilter(FdoString* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.SetFilter");
    m_command->SetFilter(value);
}

void MgdSelectCommand::SetFilter(FdoFilter* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.SetFilter");
    m_command->SetFilter(value);

    m_filter = FDO_SAFE_ADDREF(value);
}

MgReader* MgdSelectCommand::Execute()
{
    FdoPtr<FdoIFeatureReader> reader;

    // Break up the filter into smaller chunks
    FdoPtr<MgdFdoFilterCollection> subFilters = this->GetSubFilters();

    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.Execute");

    // Execute queries using the smaller filters and collect the results of the queries into a reader collection.
    FdoPtr<MgdFdoReaderCollection> frc = MgdFdoReaderCollection::Create();

    for (FdoInt32 filterIndex = 0; filterIndex < subFilters->GetCount(); filterIndex++)
    {
        FdoPtr<FdoFilter> filter = subFilters->GetItem(filterIndex);
        m_command->SetFilter(filter);
        reader = m_command->Execute();

        frc->Add(reader);
    }

    FdoPtr<MgdFdoFeatureReader> featureReader = new MgdFdoFeatureReader(frc);
    CHECKNULL((FdoIFeatureReader*)featureReader, L"MgdSelectCommand.Execute");

    return new MgdFeatureReader(m_connection, featureReader);
}

MgReader* MgdSelectCommand::ExecuteWithLock()
{
    FdoPtr<FdoIFeatureReader> reader;
    CHECKNULL((FdoISelect*)m_command, L"MgdExtendedSelectCommand.Execute");

    reader = m_command->ExecuteWithLock();
    return new MgdFeatureReader(m_connection, reader);
}

bool MgdSelectCommand::IsSupportedFunction(FdoFunction* fdoFunc)
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return this->IsFdoSupportedFunction(fdoConn, fdoFunc);
}

bool MgdSelectCommand::SupportsSelectGrouping()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgdFeatureServiceCommand::SupportsSelectGrouping(fdoConn);
}

bool MgdSelectCommand::SupportsSelectOrdering()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgdFeatureServiceCommand::SupportsSelectOrdering(fdoConn);
}

bool MgdSelectCommand::SupportsSelectDistinct()
{
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    return MgdFeatureServiceCommand::SupportsSelectDistinct(fdoConn);
}

FdoFilter* MgdSelectCommand::GetFilter()
{
    return FDO_SAFE_ADDREF(m_filter.p);
}

MgdFdoFilterCollection* MgdSelectCommand::GetSubFilters()
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

    FdoPtr<MgdFdoFilterCollection> filters = MgdFdoFilterCollection::Create();

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

FdoJoinCriteriaCollection* MgdSelectCommand::GetJoinCriteria()
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.GetJoinCriteria");
    return m_command->GetJoinCriteria();
}

void MgdSelectCommand::SetAlias(FdoString* alias)
{
    CHECKNULL((FdoISelect*)m_command, L"MgdSelectCommand.SetDistinct");
    m_command->SetAlias(alias);
}

MgReader* MgdSelectCommand::ExecuteJoined(MgStringCollection* idPropNames, bool bForceOneToOne)
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
        FdoPtr<FdoStringCollection> names = MgdFeatureUtil::MgToFdoStringCollection(idPropNames, false);
        FdoPtr<FdoIFeatureReader> forcedReader = new MgdFdoForcedOneToOneFeatureReader(fdoReader, names); 
        ret = new MgdFeatureReader(m_connection, forcedReader, idPropNames);
    }
    else
    {
        ret = new MgdFeatureReader(m_connection, fdoReader, idPropNames);
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSelectCommand.ExecuteJoined")

    return ret.Detach();
}
