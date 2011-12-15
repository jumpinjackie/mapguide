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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"
#include <SDF/IExtendedSelect.h>
#include <SDF/SdfCommandType.h>
#include <SHP/IExtendedSelect.h>
#include <SHP/ShpCommandType.h>
#include "FdoExpressionEngineCopyFilter.h"
#include "FdoExpressionEngineFilterProcessor.h"

//Uncomment to track how many FdoISelect commands are executed for a join
//operation. Each executed command prints [Query] to stdout
//
//#define TRACE_GWS_QUERY

/////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedQuery
//
/////////////////////////////////////////////////////////////////////

CGwsPreparedQuery::CGwsPreparedQuery ()
: m_bExtendedQuerySupported(false)
{
    m_resultDescriptor = NULL;
    m_pQuery = NULL;
    m_pathlength = 0;
}


CGwsPreparedQuery::CGwsPreparedQuery (bool bExtendedQuery)
: m_bExtendedQuerySupported(bExtendedQuery)
{
    m_resultDescriptor = NULL;
    m_pQuery = NULL;
    m_pathlength = 0;
}


CGwsPreparedQuery::~CGwsPreparedQuery ()
{
    if (m_resultDescriptor)
        m_resultDescriptor->Release ();
}


void CGwsPreparedQuery::DescribeResults (IGWSExtendedFeatureDescription ** ppResDesc)
{
    if (ppResDesc == NULL)
        return;
    m_resultDescriptor->AddRef ();
    * ppResDesc = m_resultDescriptor;
}


CGwsFeatureIterator * CGwsPreparedQuery::CreateFeatureIterator (EGwsFeatureIteratorType itype)
{
    switch (itype) {
    case eGwsFeatureIterator:
        return new CGwsFeatureIterator ();
    case eGwsSortMergeJoinIterator:
        return new CGwsSortMergeJoinQueryResults ();
    case eGwsNestedLoopsIterator:
        return new CGwsNestedLoopsJoinQueryResults ();
    case eGwsNestedLoopSortedBlockIterator:
        return new CGwsNestedLoopSortedBlockJoinQueryResults ();
    case eGwsBatchSortedBlockIterator:
        return new CGwsBatchSortedBlockJoinQueryResults ();
    case eGwsRightSortMergeJoinIterator:
        return new CGwsRightSortedJoinQueryResults ();
    case eGwsRightNestedLoopsIterator:
        return new CGwsRightNestedLoopJoinQueryResults ();
    case eGwsRightNestedLoopSortedBlockIterator:
        return new CGwsRightNestedLoopSortedBlockJoinQueryResults ();
    case eGwsRightBatchSortedBlockIterator:
        return new CGwsRightBatchSortedBlockJoinQueryResults ();
    }
    return NULL;
}


EGwsPreparedQueryType CGwsPreparedQuery::GetPreparedQueryType ()
{
    return eGwsPreparedFeatureQuery;
}


void CGwsPreparedQuery::GetQueryDefinition (IGWSQueryDefinition ** ppQdef)
{
    if (m_pQuery != NULL)
    {
        m_pQuery->GetQueryDefinition(ppQdef);
    }
    else
    {
        *ppQdef = NULL;
    }
}


/////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedFeatureQuery
//
/////////////////////////////////////////////////////////////////////

CGwsPreparedFeatureQuery::CGwsPreparedFeatureQuery (
    FdoIConnection         * sess,
    const GWSQualifiedName & classname,
    CGwsFeatureSourceQuery * pQuery
)
: CGwsFdoCommand (sess, classname)
{
    m_pQuery = pQuery;
    m_bIsSelectDistinct = false;
    m_bIsAxisAlignedRectangleFilter = false;
    m_selectList = FdoIdentifierCollection::Create();
}


CGwsPreparedFeatureQuery::~CGwsPreparedFeatureQuery ()
{
}


EGwsStatus CGwsPreparedFeatureQuery::Init (
    FdoIdentifierCollection  * sellist,
    FdoStringCollection      * orderBy,
    FdoOrderingOption          orderingOption,
    FdoFilter                * pFilter
)
{
    EGwsStatus stat = eGwsOk;

    stat = CGwsFdoCommand::Init ();
    if (IGWSException::IsError (stat))
        return stat;

    try {

        // TODO: this code below can be called by the client very often
        // need to optimize it in terms of minimazing calles to the init
        // Client ideally should do init once and repeatedly call SetFilter
        // and execute
        assert(m_classDef != NULL); //set in CGwsFdoCommand::Init ();
        FdoPtr<IGWSQueryDefinition> qDef;
        m_pQuery->GetQueryDefinition(&qDef);
        IGWSJoinQueryDefinition* jqd = dynamic_cast<IGWSJoinQueryDefinition*>(qDef.p);
        if(m_resultDescriptor != NULL)
        {
            m_resultDescriptor->Release();
        }
        if(jqd)
            m_resultDescriptor = new CGwsQueryResultDescriptors (m_classDef, m_classname, jqd->JoinName(), jqd->JoinDelimiter(), jqd->ForceOneToOne(), sellist, qDef->Type() == eGwsQueryLeftOuterJoin);
        else
            m_resultDescriptor = new CGwsQueryResultDescriptors (m_classDef, m_classname, NULL, NULL, true, sellist);
        m_resultDescriptor->AddRef ();

        const CGwsPropertyDesc & propdsc = m_resultDescriptor->GetGeometryPropertyDescriptor ();
        if (propdsc.m_name.length () != 0) {
            GwsSpatialContextDescription   desc;
            GwsCommonFdoUtils::DescribeSC (m_connection,
                                           propdsc.m_spatialcontext.c_str (),
                                           desc);
            m_resultDescriptor->SetCSName (desc.CsName ());
        }

        if (sellist != NULL) {
            for (int i = 0; i < sellist->GetCount (); i ++) {
                m_selectList->Add (FdoPtr<FdoIdentifier>(sellist->GetItem (i)));
            }
        }

        m_revisionprop.clear ();
        m_revisionprop = GwsCommonFdoUtils::GetRevisionProperty (m_classDef);

        //attempt to build an scrollable select command and, if that fails,
        //attempt to build a standard select comand
        FdoISelect * selcmd = NULL;
        FdoInt32 size = 0;
        FdoPtr<FdoICommandCapabilities> ptrCap = m_connection->GetCommandCapabilities();
        FdoInt32* pTypes = ptrCap->GetCommands(size);
        for(int i = 0; i < size; i++ )
        {
            if( pTypes[i] == SdfCommandType_ExtendedSelect)
            {
                m_bExtendedQuerySupported = true;
                selcmd = (SdfIExtendedSelect *) m_connection->CreateCommand (SdfCommandType_ExtendedSelect);
                mExSelProv = eSDF;
                break;
            }
            if( pTypes[i] == ShpCommandType_ExtendedSelect)
            {
                m_bExtendedQuerySupported = true;
                selcmd = (ShpIExtendedSelect *) m_connection->CreateCommand (ShpCommandType_ExtendedSelect);
                mExSelProv = eSHP;
                break;
            }
        }

        if(!m_bExtendedQuerySupported)
            selcmd = (FdoISelect *) m_connection->CreateCommand (FdoCommandType_Select);

        selcmd->SetFeatureClassName(GwsCommonFdoUtils::MakeFdoQualifiedName (m_classname).c_str());
        if (orderBy != NULL && orderBy->GetCount () > 0) {
            FdoPtr<FdoIdentifierCollection> order = selcmd->GetOrdering ();
            for (int i = 0; i < orderBy->GetCount (); i ++) {
                FdoPtr<FdoIdentifier> ident = FdoIdentifier::Create (orderBy->GetString (i));
                order->Add (ident);
            }
            if(m_bExtendedQuerySupported)
            {
                if(mExSelProv == eSDF)
                {
                    SdfIExtendedSelect* pExSelCmd = (SdfIExtendedSelect*)selcmd;
                    pExSelCmd->SetOrderingOption(orderBy->GetString(0), orderingOption);
                }
                else if(mExSelProv == eSHP)
                {
                    ShpIExtendedSelect* pExSelCmd = (ShpIExtendedSelect*)selcmd;
                    pExSelCmd->SetOrderingOption(orderBy->GetString(0), orderingOption);
                }//etc.
            }
            else
            {
                selcmd->SetOrderingOption(orderingOption);
            }
        }
        m_pCommand = selcmd;
        SetFilter (pFilter);

//        selcmd->SetFilter (pFilter);

    } catch (FdoException * fdoEx) {
        PushFdoException (eGwsFdoProviderError, fdoEx);
        fdoEx->Release ();
        stat = eGwsFdoProviderError;
    }

    return stat;
}


CGwsPreparedFeatureQuery * CGwsPreparedFeatureQuery::GetPrimaryQuery ()
{
    return this;
}


FdoDataPropertyDefinitionCollection * CGwsPreparedFeatureQuery::GetIdentityProperties ()
{
    if (m_identity != NULL)
        m_identity.p->AddRef ();
    return m_identity;
}


void CGwsPreparedFeatureQuery::PrepareInternal ()
{
    CGwsFdoCommand::PrepareNonKeyProperties ();
    if(m_selectList->GetCount() > 0)
    {
        //add in the select ilst, along with the identity properties, revision number
        WSTRARRAY strNames;
        bool     bFoundRevision = false;
        for (FdoInt32 idx = 0; m_identity != NULL && idx < m_identity->GetCount(); idx ++)
        {
            FdoPtr<FdoDataPropertyDefinition> pPropdef = m_identity->GetItem(idx);
            FdoString*                        pName = pPropdef->GetName();
            bool                              bFound = false;

            for (int i = 0; i < m_selectList->GetCount(); i++)
            {
                FdoPtr<FdoIdentifier> selProp = m_selectList->GetItem(i);
                FdoString * selectName = selProp->GetName();
                if(wcscmp(pName,selectName) == 0)
                    bFound = true;
                if(idx==0 && wcscmp(m_revisionprop.c_str (), selectName) == 0)
                    bFoundRevision = true;
            }
            if(!bFound)
                strNames.push_back(pName);
        }
        if(!bFoundRevision && !m_revisionprop.empty())
            strNames.push_back(m_revisionprop.c_str());

        FdoPtr<FdoIdentifierCollection> lst = ((FdoIBaseSelect*)m_pCommand.p)->GetPropertyNames();
        if (lst != NULL)
        {
            lst->Clear();
            if(!m_bIsSelectDistinct)
            {
                //add identity properties
                for (unsigned int i = 0; i < strNames.size(); i++)
                {
                    FdoPtr<FdoIdentifier> pIdent = FdoIdentifier::Create (strNames[i].c_str());
                    lst->Add(pIdent);
                }
            }
            for (int i = 0; i < m_selectList->GetCount(); i++)
            {
                FdoPtr<FdoIdentifier> pIdent = m_selectList->GetItem(i);
                lst->Add(pIdent);
            }
        }
    }
}


void CGwsPreparedFeatureQuery::SetFilterInternal (FdoFilter * pFilter)
{
    assert (m_pCommand);
    ((FdoISelect *) m_pCommand.p)->SetFilter (pFilter);

}


EGwsStatus CGwsPreparedFeatureQuery::SetFilter (FdoFilter * pFilter)
{
    if (pFilter== NULL || m_selectList == NULL) {
        SetFeatureCommandFilter (pFilter);
    }
    else {
        // The filter may contain reference to computed properties which may break certain providers.
        // In this case we need to make a new filter where the conputed identifier is replaced by their expressions
        if( ! GwsQueryUtils::FilterHasCalculatedProperties( pFilter, m_selectList ) )
            SetFeatureCommandFilter (pFilter);
        else
            SetFeatureCommandFilter (FdoPtr<FdoFilter>(FdoExpressionEngineCopyFilter::Copy(pFilter, m_selectList)));
    }
    return eGwsOk;
}


FdoFilter * CGwsPreparedFeatureQuery::GetFilter ()
{
    assert (m_pCommand);
    return ((FdoISelect *) m_pCommand.p)->GetFilter ();

}


FdoStringCollection * CGwsPreparedFeatureQuery::GetOrderBy ()
{
    FdoPtr<FdoIdentifierCollection> ordering = ((FdoISelect *) m_pCommand.p)->GetOrdering ();
    FdoStringCollection           * orderBy  = NULL;

    if(NULL == ordering) return NULL;

    for (int i = 0; i < ordering->GetCount (); i ++) {
        FdoPtr<FdoIdentifier> ident = ordering->GetItem (i);
        if (orderBy == NULL) {
            orderBy  = FdoStringCollection::Create ();
        }
        orderBy->Add (ident->GetText ());
    }
    return orderBy;
}


FdoOrderingOption CGwsPreparedFeatureQuery::GetOrderingOption ()
{
    FdoOrderingOption orderOption = ((FdoISelect *) m_pCommand.p)->GetOrderingOption ();
    return orderOption;
}


EGwsStatus CGwsPreparedFeatureQuery::Execute (
    IGWSFeatureIterator    ** results,
    bool                   bScrollable /*false*/
)
{
    EGwsStatus stat = eGwsOk;
    try {
        FdoPtr<FdoFilter> filter = ((FdoISelect *)m_pCommand.p)->GetFilter ();
        PrepareFilter (filter, m_bIsAxisAlignedRectangleFilter);
        #ifdef TRACE_GWS_QUERY
        printf("[Query]");
        #endif
        #ifdef _DEBUG_BATCHSORT_JOIN
        if(filter)
        {
            printf("Filter:%S\n\n", filter->ToString());
        }
        #endif
        //Code for executing an extended query - if it's requested (bScrollable)
        //AND if it's supported (m_bSdfExtendedQuerySupported)
        bool bGotScrollableIterator = false;
        FdoPtr<FdoIFeatureReader> reader;
        if(bScrollable && m_bExtendedQuerySupported)
        {
            if(mExSelProv == eSDF)
                reader = ((SdfIExtendedSelect *)m_pCommand.p)->ExecuteScrollable();
            else if(mExSelProv == eSHP)
                reader = ((ShpIExtendedSelect *)m_pCommand.p)->ExecuteScrollable();
        }
        //If extended query failed or was skipped, do standard query
        if(NULL == reader)
        {
            reader = ((FdoISelect *)m_pCommand.p)->Execute ();
        }
        else
            bGotScrollableIterator = true;

        CGwsFeatureIterator * pResults = CreateFeatureIterator (eGwsFeatureIterator);
        stat = pResults->InitializeReader (reader, m_pQuery, this, bGotScrollableIterator);
        if (! IGWSException::IsError (stat)) {
            pResults->AddRef ();
            * results = pResults;
        }
    } catch (FdoException * fdoEx) {
        PushFdoException (eGwsFailedToExecuteCommand, fdoEx);
        fdoEx->Release ();
        return eGwsFailedToExecuteCommand;
    }

    return stat;
}


EGwsStatus CGwsPreparedFeatureQuery::Execute (
    FdoFilter               * filter,
    IGWSFeatureIterator    ** results,
    bool                      bScrollable
)
{
    SetFilter (filter);
    return Execute (results,bScrollable);
}


EGwsStatus CGwsPreparedFeatureQuery::Execute (
    const GWSFeatureId     & featid,
    IGWSFeatureIterator   ** results
)
{
    FdoPtr<FdoFilter> filter;
    EGwsStatus        stat = eGwsOk;

    try   {

        eGwsOkThrow (BuildFilter (featid, filter.p));
        return Execute (filter, results);

    } catch (EGwsStatus es) {
        PushStatus (es);
        stat = es;
    }
    return stat;
}


EGwsStatus CGwsPreparedFeatureQuery::Execute (
    const GwsFeaturesIdVector    & featids,
    int                            lBound,
    int                            uBound,
    IGWSFeatureIterator         ** results,
    bool                           bScrollable
)
{
    FdoPtr<FdoFilter> filter;
    EGwsStatus        stat = eGwsOk;

    try   {
        eGwsOkThrow (BuildFilter (featids, lBound, uBound, filter.p));
        return Execute (filter, results, bScrollable);

    } catch (EGwsStatus es) {
        PushStatus (es);
        stat = es;
    }
    return stat;
}
