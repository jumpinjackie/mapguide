//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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


/////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedJoinQuery
//
/////////////////////////////////////////////////////////////////////

CGwsPreparedJoinQuery::CGwsPreparedJoinQuery (
    EGwsJoinMethod             joinmethod,
    CGwsPreparedQuery        * lpq,
    CGwsPreparedQuery        * rpq,
    FdoStringCollection      * lcols,
    FdoStringCollection      * rcols,
    CGwsFeatureSourceQuery   * query
)
{
    assert (lpq && rpq && lcols && rcols);

    assert (lcols->GetCount () == rcols->GetCount ());

    m_joinmethod = joinmethod;
    m_leftquery  = lpq;
    m_rightquery = rpq;
    m_leftCols = lcols;
    lcols->AddRef ();
    m_rightCols = rcols;
    rcols->AddRef ();
    m_pQuery = query;
}


CGwsPreparedJoinQuery::~CGwsPreparedJoinQuery ()
{
    delete m_leftquery;
    delete m_rightquery;
}


EGwsStatus CGwsPreparedJoinQuery::Init ()
{
    assert (m_leftquery != NULL && m_rightquery != NULL);
    // make sure
    FdoPtr<IGWSExtendedFeatureDescription> lqdesc;
    FdoPtr<IGWSExtendedFeatureDescription> rqdesc;

    m_leftquery->DescribeResults (&lqdesc);
    m_rightquery->DescribeResults (&rqdesc);

    m_pathlength = m_leftquery->GetPathLength () + 1;

    CGwsQueryResultDescriptors * ldesc = dynamic_cast<CGwsQueryResultDescriptors *> (lqdesc.p);
    CGwsQueryResultDescriptors * qres = new CGwsQueryResultDescriptors (* ldesc);
    qres->Add (rqdesc);

    m_resultDescriptor = qres;
    m_resultDescriptor->AddRef ();

    return eGwsOk;
}


CGwsPreparedFeatureQuery * CGwsPreparedJoinQuery::GetPrimaryQuery ()
{
    return m_leftquery->GetPrimaryQuery ();
}


FdoDataPropertyDefinitionCollection * CGwsPreparedJoinQuery::GetIdentityProperties ()
{
    return m_leftquery->GetIdentityProperties ();
}


EGwsStatus CGwsPreparedJoinQuery::Execute (
    IGWSFeatureIterator ** results,
    bool bScrollable /*false*/
)
{
    FdoPtr<IGWSFeatureIterator> left;
    FdoPtr<IGWSFeatureIterator> right;

    if (results == NULL)
        return eGwsFailed;

    * results = NULL;
    EGwsStatus stat = eGwsOk;

    try {
        CGwsJoinQueryResults * reader = NULL;
        if (m_joinmethod == eGwsSortMerge) {
            reader = dynamic_cast<CGwsJoinQueryResults *> (CreateFeatureIterator (eGwsSortMergeJoinIterator));
        } else if (m_joinmethod == eGwsNestedLoops) {
             reader = dynamic_cast<CGwsJoinQueryResults *> (CreateFeatureIterator (eGwsNestedLoopsIterator));
        } else if (m_joinmethod == eGwsNestedLoopSortedBlock) {
            reader = dynamic_cast<CGwsJoinQueryResults *> (CreateFeatureIterator (eGwsNestedLoopSortedBlockIterator));
        } else if (m_joinmethod == eGwsBatchSortedBlock) {
            reader = dynamic_cast<CGwsJoinQueryResults *> (CreateFeatureIterator (eGwsBatchSortedBlockIterator));
        }

        if (reader != NULL) {
            stat = reader->InitializeReader (m_pQuery, this, bScrollable);
            if (IGWSException::IsError (stat)) {
                CopyStatus (* reader);
                delete  reader;
                return stat;
            }
            reader->AddRef ();
            * results = reader;
        }
        assert (reader);

    } catch (FdoException * fdoEx) {
        PushFdoException (eGwsFailedToExecuteCommand, fdoEx);
        fdoEx->Release ();
        return eGwsFailedToExecuteCommand;
    }
    return eGwsOk;
}


EGwsStatus CGwsPreparedJoinQuery::Execute (
    FdoFilter            * filter,
    IGWSFeatureIterator ** results,
    bool                   bScrollable
)
{
    EGwsStatus                 stat = eGwsOk;

    try   {
        eGwsOkThrow (SetFilter (filter));
        return Execute (results,bScrollable);

    } catch (EGwsStatus es) {
        PushStatus (es);
        stat = es;
    }
    return stat;
}


EGwsStatus CGwsPreparedJoinQuery::Execute (
    const GWSFeatureId   & featid,
    IGWSFeatureIterator ** results
)
{
    CGwsPreparedFeatureQuery * pquery = NULL;
    FdoPtr<FdoFilter>          filter;
    EGwsStatus                 stat = eGwsOk;

    try   {
        pquery = GetPrimaryQuery ();

        eGwsOkThrow (pquery->BuildFilter (featid, filter.p));
        return Execute (filter, results);

    } catch (EGwsStatus es) {
        PushStatus (es);
        stat = es;
    }
    return stat;
}


EGwsStatus CGwsPreparedJoinQuery::Execute (
    const GwsFeaturesIdVector & featids,
    int lbound,
    int ubound,
    IGWSFeatureIterator ** results,
    bool bScrollable
)
{
    CGwsPreparedFeatureQuery * pquery = NULL;
    FdoPtr<FdoFilter>          filter;
    EGwsStatus                 stat = eGwsOk;

    try   {
        pquery = GetPrimaryQuery ();

        eGwsOkThrow (pquery->BuildFilter (featids, lbound, ubound, filter.p));
        return Execute (filter, results, bScrollable);

    } catch (EGwsStatus es) {
        PushStatus (es);
        stat = es;
    }
    return stat;
}


EGwsStatus CGwsPreparedJoinQuery::SetFilter (FdoFilter * filter)
{
    // the leftmost query is the one
    if (m_leftquery != NULL)
        return m_leftquery->SetFilter (filter);
    return eGwsFailed;
}


FdoFilter * CGwsPreparedJoinQuery::GetFilter ()
{
    // the leftmost query is the one
    if (m_leftquery != NULL)
        return m_leftquery->GetFilter ();
    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedLeftJoinQuery
//
///////////////////////////////////////////////////////////////////////////////

CGwsPreparedLeftJoinQuery::CGwsPreparedLeftJoinQuery (
    EGwsJoinMethod             joinmethod,
    CGwsPreparedQuery        * lpq,
    CGwsPreparedQuery        * rpq,
    FdoStringCollection      * lcols,
    FdoStringCollection      * rcols,
    CGwsFeatureSourceQuery   * query
)
: CGwsPreparedJoinQuery (joinmethod, lpq, rpq, lcols, rcols, query)
{
}


CGwsPreparedLeftJoinQuery::~CGwsPreparedLeftJoinQuery ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedEqualJoinQuery
//
///////////////////////////////////////////////////////////////////////////////

CGwsPreparedEqualJoinQuery::CGwsPreparedEqualJoinQuery (
    EGwsJoinMethod             joinmethod,
    CGwsPreparedQuery        * lpq,
    CGwsPreparedQuery        * rpq,
    FdoStringCollection      * lcols,
    FdoStringCollection      * rcols,
    CGwsFeatureSourceQuery   * query
)
: CGwsPreparedJoinQuery (joinmethod, lpq, rpq, lcols, rcols, query)
{
}


CGwsPreparedEqualJoinQuery::~CGwsPreparedEqualJoinQuery ()
{
}
