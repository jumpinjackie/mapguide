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
#include "ExtendedSelectCommand.h"
#include "SelectAggregateCommand.h"
#include "ServerFeatureReader.h"
#include "ServerFeatureConnection.h"
#include "FdoFeatureReader.h"
#include "FdoFilterCollection.h"
#include "FdoReaderCollection.h"
#include "Util/FdoExpressionEngineUtilDataReader.h"
#include "FdoForcedOneToOneFeatureReader.h"

// The maximum size of the subfilter for a selection query.  Tune this value for optimal selection perfomance.
#define MG_MAX_SUBFILTER_SIZE  250

MgExtendedSelectCommand::MgExtendedSelectCommand(MgResourceIdentifier* resource)
{
    CHECKNULL((MgResourceIdentifier*)resource, L"MgExtendedSelectCommand.MgSelectCommand");

    // Connect to provider
    m_connection = new MgServerFeatureConnection(resource);
    if ((NULL != m_connection.p) && ( m_connection->IsConnectionOpen() ))
    {
        m_providerName = m_connection->GetProviderName();
    }
    else
    {
        throw new MgConnectionFailedException(L"MgExtendedSelectCommand.MgSelectCommand", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    // Create FdoISelect command
    FdoPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    m_command = (FdoIExtendedSelect*)fdoConn->CreateCommand(FdoCommandType_ExtendedSelect);
    CHECKNULL((FdoIExtendedSelect*)m_command, L"MgExtendedSelectCommand.MgSelectCommand");
}

MgExtendedSelectCommand::~MgExtendedSelectCommand()
{
    m_command = NULL;
    m_filter = NULL;
}

FdoIdentifierCollection* MgExtendedSelectCommand::GetPropertyNames()
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.GetPropertyNames");
    return m_command->GetPropertyNames();
}

FdoJoinCriteriaCollection* MgExtendedSelectCommand::GetJoinCriteria()
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.GetJoinCriteria");
    return m_command->GetJoinCriteria();
}

void MgExtendedSelectCommand::SetAlias(FdoString* alias)
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.SetDistinct");
    m_command->SetAlias(alias);
}

void MgExtendedSelectCommand::SetDistinct(bool value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.SetDistinct");
    // This operation is not supported by FdoISelect
    // m_command->SetDistinct(value);

    // throw new MgInvalidOperationException(L"MgExtendedSelectCommand.SetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool MgExtendedSelectCommand::GetDistinct()
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.GetDistinct");
    // This operation is not supported by FdoISelect
    // return m_command->GetDistinct();

    // throw new MgInvalidOperationException(L"MgExtendedSelectCommand.GetDistinct", __LINE__, __WFILE__, NULL, L"", NULL);

    return false;
}

void MgExtendedSelectCommand::SetFetchSize(FdoInt32 fetchSize)
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.SetFetchSize");
    m_command->SetFetchSize(fetchSize);
}

FdoInt32 MgExtendedSelectCommand::GetFetchSize()
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.GetFetchSize");
    return m_command->GetFetchSize();
}

FdoIdentifierCollection* MgExtendedSelectCommand::GetOrdering()
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.GetOrdering");
    return m_command->GetOrdering();
}

void MgExtendedSelectCommand::SetOrderingOption(FdoOrderingOption option)
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.SetOrderingOption");
    ((FdoIBaseSelect*)m_command)->SetOrderingOption(option);
}

FdoOrderingOption MgExtendedSelectCommand::GetOrderingOption()
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.GetOrderingOption");
    return ((FdoIBaseSelect*)m_command)->GetOrderingOption();
}

FdoIdentifierCollection* MgExtendedSelectCommand::GetGrouping()
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.GetGrouping");
    // This operation is not supported by FdoISelect
    // return m_command->GetGrouping();

    // throw new MgInvalidOperationException(L"MgExtendedSelectCommand.GetGrouping", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgExtendedSelectCommand::SetGroupingFilter(FdoFilter* filter)
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.SetGroupingFilter");
    // This operation is not supported by FdoISelect
    // m_command->SetGroupingFilter(filter);

    // throw new MgInvalidOperationException(L"MgExtendedSelectCommand.SetGroupingFilter", __LINE__, __WFILE__, NULL, L"", NULL);
}

FdoFilter* MgExtendedSelectCommand::GetGroupingFilter()
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.GetGroupingFilter");
    // This operation is not supported by FdoISelect
    // return m_command->GetGroupingFilter(filter);

    // throw new MgInvalidOperationException(L"MgExtendedSelectCommand.GetGroupingFilter", __LINE__, __WFILE__, NULL, L"", NULL);
    return NULL;
}

void MgExtendedSelectCommand::SetFeatureClassName(FdoString* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.SetFeatureClassName");
    m_command->SetFeatureClassName(value);
}

void MgExtendedSelectCommand::SetFilter(FdoString* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.SetFilter");
    m_command->SetFilter(value);
#ifdef DEBUG_FDO_JOIN
    ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join] Set Filter: %W"), value));
#endif
}

void MgExtendedSelectCommand::SetFilter(FdoFilter* value)
{
    CHECKNULL((FdoISelect*)m_command, L"MgExtendedSelectCommand.SetFilter");
    m_command->SetFilter(value);
#ifdef DEBUG_FDO_JOIN
    ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join] Set Filter: %W"), value->ToString()));
#endif
    m_filter = FDO_SAFE_ADDREF(value);
}

MgReader* MgExtendedSelectCommand::Execute()
{
    throw new MgNotImplementedException(L"MgExtendedSelectCommand.Execute", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgReader* MgExtendedSelectCommand::ExecuteJoined(MgStringCollection* idPropNames, bool bForceOneToOne)
{
    Ptr<MgReader> ret;

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoIFeatureReader> fdoReader = m_command->Execute();
    if (bForceOneToOne)
    {
        FdoPtr<FdoStringCollection> names = MgServerFeatureUtil::MgToFdoStringCollection(idPropNames, false);
        FdoPtr<FdoIFeatureReader> forcedReader = new MgFdoForcedOneToOneFeatureReader(fdoReader, names); 
        ret = new MgServerFeatureReader(m_connection, forcedReader);
    }
    else
    {
        ret = new MgServerFeatureReader(m_connection, fdoReader);
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgExtendedSelectCommand.ExecuteJoined")

    return ret.Detach();
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

MgFdoFilterCollection* MgExtendedSelectCommand::GetSubFilters()
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
