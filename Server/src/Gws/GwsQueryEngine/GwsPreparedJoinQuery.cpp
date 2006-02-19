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
// class CGwsPreparedJoinQuery
//
/////////////////////////////////////////////////////////////////////
CGwsPreparedJoinQuery::CGwsPreparedJoinQuery (
    EGwsJoinMethod             joinmethod,
    CGwsPreparedQuery        * lpq,
    CGwsPreparedQuery        * rpq,
    GisStringCollection      * lcols,
    GisStringCollection      * rcols,
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
    GisPtr<IGWSExtendedFeatureDescription> lqdesc;
    GisPtr<IGWSExtendedFeatureDescription> rqdesc;

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
    IGWSFeatureIterator ** results
)
{
    GisPtr<IGWSFeatureIterator> left;
    GisPtr<IGWSFeatureIterator> right;

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
        }

        if (reader != NULL) {
            stat = reader->InitializeReader (m_pQuery, this);
            if (IGWSException::IsError (stat)) {
                CopyStatus (* reader);
                delete  reader;
                return stat;
            }
            reader->AddRef ();
            * results = reader;
        }
        assert (reader);

    } catch (GisException * gis) {
        PushGisException (eGwsFailedToExecuteCommand, gis);
        gis->Release ();
        return eGwsFailedToExecuteCommand;
    }
    return eGwsOk;
}

EGwsStatus CGwsPreparedJoinQuery::Execute (
    FdoFilter            * filter,
    IGWSFeatureIterator ** results
)
{
    filter; // For "unreferenced formal parameter" warning
    results; // For "unreferenced formal parameter" warning
    return eGwsFailed;
}

EGwsStatus CGwsPreparedJoinQuery::Execute (
    const GWSFeatureId   & featid,
    IGWSFeatureIterator ** results
)
{
    featid; // For "unreferenced formal parameter" warning
    results; // For "unreferenced formal parameter" warning
    return eGwsFailed;
}
EGwsStatus CGwsPreparedJoinQuery::Execute (
    const GwsFeaturesIdVector & featids,
    int lbound,
    int ubound,
    IGWSFeatureIterator ** results
)
{
    featids; // For "unreferenced formal parameter" warning
    lbound; // For "unreferenced formal parameter" warning
    ubound; // For "unreferenced formal parameter" warning
    results; // For "unreferenced formal parameter" warning
    return eGwsFailed;
}

EGwsStatus CGwsPreparedJoinQuery::SetFilter (FdoFilter * filter)
{
    // the leftmost query is the one
    if (m_leftquery != NULL)
        return m_leftquery->SetFilter (filter);
    return eGwsFailed;
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
    GisStringCollection      * lcols,
    GisStringCollection      * rcols,
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
    GisStringCollection      * lcols,
    GisStringCollection      * rcols,
    CGwsFeatureSourceQuery   * query
)
: CGwsPreparedJoinQuery (joinmethod, lpq, rpq, lcols, rcols, query)
{
}

CGwsPreparedEqualJoinQuery::~CGwsPreparedEqualJoinQuery ()
{
}
