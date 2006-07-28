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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include <stdafx.h>

#include "GwsQueryEngineImp.h"

/////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedQuery
//
/////////////////////////////////////////////////////////////////////
CGwsPreparedQuery::CGwsPreparedQuery ()
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
    case eGwsRightSortMergeJoinIterator:
        return new CGwsRightSortedJoinQueryResults ();
    case eGwsRightNestedLoopsIterator:
        return new CGwsRightNestedLoopJoinQueryResults ();
    case eGwsRightNestedLoopSortedBlockIterator:
        return new CGwsRightNestedLoopSortedBlockJoinQueryResults ();
    }
    return NULL;
}

EGwsPreparedQueryType CGwsPreparedQuery::GetPreparedQueryType ()
{
    return eGwsPreparedFeatureQuery;
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
}

CGwsPreparedFeatureQuery::~CGwsPreparedFeatureQuery ()
{
}

EGwsStatus CGwsPreparedFeatureQuery::Init (
    FdoStringCollection      * sellist,
    FdoStringCollection      * orderBy,
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

        m_resultDescriptor = new CGwsQueryResultDescriptors (m_classDef, m_classname, sellist);
        m_resultDescriptor->AddRef ();

        const CGwsPropertyDesc & propdsc = m_resultDescriptor->GetGeometryPropertyDescriptor ();
        if (propdsc.m_name.length () != 0) {
            GwsSpatialContextDescription   desc;
            GwsCommonFdoUtils::DescribeSC (m_connection,
                                           propdsc.m_spatialcontext.c_str (),
                                           desc);
            m_resultDescriptor->SetCSName (desc.CsNameWkt ());
        }

        if (sellist != NULL) {
            for (int i = 0; i < sellist->GetCount (); i ++) {
                m_selectList.push_back (sellist->GetString (i));
            }
        }

        m_revisionprop.clear ();

        m_revisionprop = GwsCommonFdoUtils::GetRevisionProperty (m_classDef);

        FdoISelect * selcmd =
                 (FdoISelect *) m_connection->CreateCommand (FdoCommandType_Select);
        selcmd->SetFeatureClassName(GwsCommonFdoUtils::MakeFdoQualifiedName (m_classname).c_str());
        if (orderBy != NULL && orderBy->GetCount () > 0) {
            FdoPtr<FdoIdentifierCollection> order = selcmd->GetOrdering ();
            for (int i = 0; i < orderBy->GetCount (); i ++) {
                FdoPtr<FdoIdentifier> ident = FdoIdentifier::Create (orderBy->GetString (i));
                order->Add (ident);
            }
        }
        m_pCommand = selcmd;
        SetFilter (pFilter);

//        selcmd->SetFilter (pFilter);

    } catch (FdoException * gis) {
        PushFdoException (eGwsFdoProviderError, gis);
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
    if(m_selectList.size() > 0)
    {
        //add in the select ilst, along with the identity properties, revision number
        assert(m_identity != NULL);
        WSTRARRAY strNames;
        bool     bFoundRevision = false;
        for (FdoInt32 idx = 0; idx < m_identity->GetCount(); idx ++)
        {
            FdoPtr<FdoDataPropertyDefinition> pPropdef = m_identity->GetItem(idx);
            FdoString*                        pName = pPropdef->GetName();
            bool                              bFound = false;

            for (unsigned int i = 0; i < m_selectList.size(); i++)
            {
                const wchar_t * selectName = m_selectList[i].c_str ();
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
            for (unsigned int i = 0; i < m_selectList.size(); i++)
            {
                FdoPtr<FdoIdentifier> pIdent = FdoIdentifier::Create (m_selectList[i].c_str());
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
    SetFeatureCommandFilter (pFilter);
//    ((FdoISelect *) m_pCommand.p)->SetFilter (pFilter);
    return eGwsOk;
}

FdoStringCollection * CGwsPreparedFeatureQuery::GetOrderBy ()
{
    FdoPtr<FdoIdentifierCollection> ordering = ((FdoISelect *) m_pCommand.p)->GetOrdering ();
    FdoStringCollection           * orderBy  = NULL;

    for (int i = 0; i < ordering->GetCount (); i ++) {
        FdoPtr<FdoIdentifier> ident = ordering->GetItem (i);
        if (orderBy == NULL) {
            orderBy  = FdoStringCollection::Create ();
        }
        orderBy->Add (ident->GetText ());
    }
    return orderBy;
}


EGwsStatus CGwsPreparedFeatureQuery::Execute (
    IGWSFeatureIterator    ** results
)
{
    EGwsStatus stat = eGwsOk;
    try {
        FdoPtr<FdoFilter> filter = ((FdoISelect *)m_pCommand.p)->GetFilter ();
        PrepareFilter (filter);
        // do I need to set filter after converting it?

        FdoPtr<FdoIFeatureReader> reader = ((FdoISelect *)m_pCommand.p)->Execute ();

        CGwsFeatureIterator * pResults = CreateFeatureIterator (eGwsFeatureIterator);
        stat = pResults->InitializeReader (reader, m_pQuery, this);
        if (! IGWSException::IsError (stat)) {
            pResults->AddRef ();
            * results = pResults;
        }
    } catch (FdoException * gis) {
        PushFdoException (eGwsFailedToExecuteCommand, gis);
        gis->Release ();
        return eGwsFailedToExecuteCommand;
    }

    return stat;
}

EGwsStatus CGwsPreparedFeatureQuery::Execute (
    FdoFilter               * filter,
    IGWSFeatureIterator    ** results
)
{
    SetFilter (filter);
    return Execute (results);
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
    IGWSFeatureIterator         ** results
)
{
    FdoPtr<FdoFilter> filter;
    EGwsStatus        stat = eGwsOk;

    try   {
        eGwsOkThrow (BuildFilter (featids, lBound, uBound, filter.p));
        return Execute (filter, results);

    } catch (EGwsStatus es) {
        PushStatus (es);
        stat = es;
    }
    return stat;
}
