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


/////////////////////////////////////////////////////////////////////
//
// Static data
//
/////////////////////////////////////////////////////////////////////

static bool supportOrdering (FdoIConnection * conn)
{
    FdoPtr<FdoICommandCapabilities> ptrCap;
    ptrCap = conn->GetCommandCapabilities();
    assert (ptrCap);
    if (ptrCap == NULL)
        return false;
    return ptrCap->SupportsSelectOrdering();
}


/////////////////////////////////////////////////////////////////////
//
// class CGwsFeatureSourceQuery
//
/////////////////////////////////////////////////////////////////////

CGwsFeatureSourceQuery::CGwsFeatureSourceQuery ()
{
    m_pQuery = NULL;
}


CGwsFeatureSourceQuery::~CGwsFeatureSourceQuery () throw()
{
    delete m_pQuery;
}


void CGwsFeatureSourceQuery::Initialize (
    IGWSConnectionPool        * pool,
    IGWSQueryDefinition       * qrydef
)
{
    m_connectionpool = pool;
    if (pool != NULL)
        pool->AddRef ();

    m_qrydef = qrydef;
    if (qrydef != NULL)
        qrydef->AddRef ();
    m_pQuery = NULL;
}


IGWSConnectionPool * CGwsFeatureSourceQuery::GetConnectionPool ()
{
    m_connectionpool.p->AddRef ();
    return m_connectionpool;
}


void CGwsFeatureSourceQuery::GetQueryDefinition (IGWSQueryDefinition ** ppQdef)
{
    if (ppQdef == NULL)
        return;
    * ppQdef = m_qrydef;
    if (m_qrydef != NULL) {
        m_qrydef.p->AddRef ();
    }
}


void CGwsFeatureSourceQuery::DescribeResults (
    IGWSExtendedFeatureDescription ** ppResDesc
)
{
    if (ppResDesc == NULL)
        GWS_THROW (eGwsNullPointer);
    if (m_pQuery == NULL)
        GWS_THROW (eGwsFdoQueryIsNotPrepared);
    m_pQuery->DescribeResults (ppResDesc);
}


void CGwsFeatureSourceQuery::SetDestinationCS (
    const GWSCoordinateSystem &  csname
)
{
    m_csname = csname;
}


const GWSCoordinateSystem & CGwsFeatureSourceQuery::GetDestinationCS ()
{
    return m_csname;
}


void CGwsFeatureSourceQuery::SetSourceCS (const GWSCoordinateSystem & csname)
{
    m_srccsname = csname;
}


const GWSCoordinateSystem &  CGwsFeatureSourceQuery::GetSourceCS ()
{
    return m_srccsname;
}


void CGwsFeatureSourceQuery::SetCSFactory (
    IGWSCoordinateSystemConverterFactory * csfactory
)
{
    m_csfactory = csfactory;
    if (csfactory != NULL)
        csfactory->AddRef ();
}


IGWSCoordinateSystemConverterFactory * CGwsFeatureSourceQuery::GetCSFactory ()
{
    IGWSCoordinateSystemConverterFactory * csfactory = m_csfactory;

    if (csfactory != NULL)
        csfactory->AddRef ();
    return csfactory;
}


void CGwsFeatureSourceQuery::Prepare ()
{
    Clear ();
    assert (m_qrydef && m_connectionpool);

    if (m_pQuery != NULL)
        return;  // query was already preapared. Ignore

    if(m_qrydef == NULL)
        return;

    try {
        m_pQuery = Prepare (m_qrydef, L"");
    } catch (FdoException * ex) {
        GWS_RETHROW (ex, eGwsFailedToPrepareQuery);
    }

    if (m_pQuery == NULL) {
        GWS_THROW_DIAG (eGwsFailedToPrepareQuery, this);
    }
}


void CGwsFeatureSourceQuery::SetClassDefinition(FdoClassDefinition* pClassDef)
{
    FDO_SAFE_ADDREF(pClassDef);
    m_classDef = pClassDef;
}


CGwsPreparedQuery * CGwsFeatureSourceQuery::Prepare (
    IGWSQueryDefinition * qrydef,
    const WSTR          & suffix
)
{
    switch (qrydef->Type ()) {
    case eGwsQueryFeature:
        {
            FdoPtr<FdoStringCollection> orderByList = qrydef->GetOrderBy();
            FdoOrderingOption orderingOption = qrydef->GetOrderingOption();
            return PrepareFeatureQuery (
                (IGWSFeatureQueryDefinition *) qrydef, orderByList, orderingOption, suffix, m_classDef);
        }

    case eGwsQueryLeftOuterJoin:
    case eGwsQueryEqualJoin:
        return PrepareJoinQuery ((IGWSJoinQueryDefinition *) qrydef, suffix);
    }
    return NULL;
}


CGwsPreparedFeatureQuery * CGwsFeatureSourceQuery::PrepareFeatureQuery (
    IGWSFeatureQueryDefinition * pFQuery,
    FdoStringCollection        * orderByCols,
    FdoOrderingOption            orderingOption,
    const WSTR                 & suffix,
    FdoClassDefinition         * pClassDef
)
{
    EGwsStatus                  stat = eGwsOk;

    const GWSQualifiedName      classname = pFQuery->ClassName ();

    // fdo connection for the prepared query from the pool
    FdoPtr<FdoIConnection>      conn = m_connectionpool->GetConnection (
                                                            classname.FeatureSource ());
    CGwsPreparedFeatureQuery *  pQuery = CreatePreparedFeatureQuery (conn, classname);
    FdoPtr<FdoIdentifierCollection> sellist = pFQuery->SelectList ();
    WSTR                        madesuffix;

    if (suffix.empty ())
        madesuffix = L"L";
    else
        madesuffix = suffix;

    FdoPtr<IGWSCoordinateSystemConverter> csconverter;
    if (m_csfactory) {
        // if coordinate system factory and coordinate system name
        // are both set, create conveter
        stat = m_csfactory->Create (& csconverter);

        if (! IGWSException::IsError (stat)) {
            if (! m_csname.IsEmpty ())
                stat = csconverter->SetDestinationCS (m_csname);
        }

        if (! IGWSException::IsError (stat)) {
            if (! m_srccsname.IsEmpty ())
                stat = csconverter->SetSourceCS (m_srccsname);
        }

        if (IGWSException::IsError (stat)) {
            delete pQuery;
            GWS_THROW (stat);
        }
    }
    //Optimization - if the class def is available. This call must be before
    //the Init() method of the command to avoid a describe schema call to the provider.
    if(pClassDef != NULL)
        pQuery->SetClassDefinition(pClassDef);
    stat = pQuery->Init ( sellist, orderByCols, orderingOption, pFQuery->Filter ());

    if (IGWSException::IsError (stat)) {
        delete pQuery;
        GWS_THROW (stat);
    }

    if (csconverter != NULL)
        pQuery->SetCSConverter (csconverter);

    stat = pQuery->Prepare ();

    if (IGWSException::IsError (stat)) {
        delete pQuery;
        GWS_THROW_DIAG (stat, dynamic_cast<CGwsObject*> (pQuery));
    }
    m_revprop = pQuery->m_revisionprop;
    // set suffix for the query descriptor
    pQuery->m_resultDescriptor->SetSuffix (madesuffix);
    return pQuery;
}


CGwsPreparedJoinQuery * CGwsFeatureSourceQuery::PrepareJoinQuery (
    IGWSJoinQueryDefinition     * pFQuery,
    const WSTR                  & suffix
)
{
    CGwsPreparedJoinQuery    *  prepQuery  = NULL;
    CGwsPreparedQuery        *  pLeftQuery = NULL;
    CGwsPreparedQuery        *  pRightQuery = NULL;

    try {
        FdoPtr<IGWSQueryDefinition> lqdef = pFQuery->LeftQueryDefinition ();
        FdoPtr<FdoStringCollection> lCols = pFQuery->LeftJoinAttributes ();
        if (lCols == NULL) {
            PushStatus (eGwsMissingLeftJoinAttributes);
            eGwsOkThrow (eGwsMissingLeftJoinAttributes);
        }

        FdoPtr<IGWSQueryDefinition> rqdef = pFQuery->RightQueryDefinition ();
        FdoPtr<FdoStringCollection> rCols = pFQuery->RightJoinAttributes ();
        if (lCols->GetCount () != rCols->GetCount ()) {
            PushStatus (eGwsJoinPropertiesCardinalityViolation);
            eGwsOkThrow (eGwsJoinPropertiesCardinalityViolation);
        }

        // have the following choices:
        // - nested loops join (both classes do not suppport ordering)
        // - Nested Block Join and Sorted Block (left side supports ordering, right side not)
        //   left side is ordered by the join attributes. For each group of rows with the
        //   equal attributes the right side is scanned only once.
        // - Hash join (build a hash table of a right side if it is small) (no way to define how
        //   right side is small).
        // - sort-merge algorithm

        WSTR  subsuffix;
        subsuffix = suffix;
        subsuffix.append(L"L");

        bool lSupportsOrdering = false;
        bool rSupportsOrdering = false;

        if (lqdef->Type () == eGwsQueryFeature) {
            // left node is a primary query
            IGWSFeatureQueryDefinition     * lfqdef = static_cast<IGWSFeatureQueryDefinition*>(lqdef.p);
            FdoPtr<FdoIConnection>  conn =
                        m_connectionpool->GetConnection (
                                                lfqdef->ClassName ().FeatureSource ());
            lSupportsOrdering = supportOrdering (conn);
            pLeftQuery = PrepareFeatureQuery (
                lfqdef, lSupportsOrdering ? lCols.p : NULL, lfqdef->GetOrderingOption(), subsuffix);

        } else {
            pLeftQuery = Prepare (lqdef, subsuffix);
            CGwsPreparedJoinQuery * lJoin = (CGwsPreparedLeftJoinQuery *) pLeftQuery;
            if (lJoin->JoinMethod () == eGwsNestedLoopSortedBlock ||
                lJoin->JoinMethod () == eGwsSortMerge)
            {
                // if left part is ordered and ordering is identical to
                // the left join attrbites use this orderin
                FdoPtr<FdoStringCollection> l =  lJoin->LeftProperties ();
                if (l->GetCount () == lCols->GetCount ()) {
                    int i;
                    for (i = 0; i < l->GetCount (); i ++) {
                        if (wcscmp (l->GetString (i), lCols->GetString (i)) != 0) {
                            break;
                        }
                    }
                    if (i == l->GetCount ())
                        lSupportsOrdering = true;
                }
            }
        }

        subsuffix = suffix;
        subsuffix.append(L"R");

        if (rqdef->Type () == eGwsQueryFeature) {
            IGWSFeatureQueryDefinition     * rfqdef = static_cast<IGWSFeatureQueryDefinition*>(rqdef.p);
            FdoPtr<FdoIConnection>  conn =
                        m_connectionpool->GetConnection (
                                                rfqdef->ClassName ().FeatureSource ());
            rSupportsOrdering = supportOrdering (conn);

            pRightQuery = PrepareFeatureQuery (rfqdef,
                                            rSupportsOrdering ? rCols.p : NULL,
                                            rfqdef->GetOrderingOption(),
                                            subsuffix);

        } else {
            pRightQuery = Prepare (rqdef, subsuffix);
        }

        if (pLeftQuery == NULL || pRightQuery == NULL) {
            eGwsOkThrow (eGwsFailed);
        }

        // validate join attributes
        // make sure that left and right join columns exist in the
        // left and right primary queries and that they are comparable
        CGwsPreparedFeatureQuery * lpq = pLeftQuery->GetPrimaryQuery ();
        CGwsPreparedFeatureQuery * rpq = pRightQuery->GetPrimaryQuery ();

        // will throw EGwsStatus if invalid
        int i = 0;
        for (i = 0; i < lCols->GetCount (); i ++) {
            FdoString* lProp = lCols->GetString (i);
            FdoString* rProp = rCols->GetString (i);
            FdoPtr<FdoPropertyDefinition> rpdef = rpq->GetPropertyDefinition (rProp);
            FdoPtr<FdoPropertyDefinition> lpdef = lpq->GetPropertyDefinition (lProp);
            if(NULL == lpdef) {
                // check for join query def and check right side
                CGwsPreparedJoinQuery* plj = dynamic_cast<CGwsPreparedJoinQuery*>(pLeftQuery);
                if(NULL != plj) {
                    CGwsPreparedFeatureQuery* prq = plj->RightQuery()->GetPrimaryQuery();
                    std::wstring strname(lProp);
                    size_t delimpos = strname.find(pFQuery->JoinDelimiter());
                    if(std::wstring::npos != delimpos) {
                        std::wstring propname = strname.substr(delimpos+1);
                        lpdef = prq->GetPropertyDefinition (propname.c_str());
                        ValidateJoinAttributes (lpdef, rpdef, propname.c_str(), rProp);
                        continue;
                    }
                }
            }
            ValidateJoinAttributes (lpdef, rpdef, lProp, rProp);
        }

        EGwsJoinMethod joinmethod = eGwsNestedLoops;

        if (lSupportsOrdering) {
            if (rSupportsOrdering) {
                // sort-merge
                joinmethod = eGwsSortMerge;
            } else {
                // nested loop join and sorted block
                joinmethod = eGwsNestedLoopSortedBlock;
            }
        } else {
            if ((rSupportsOrdering) && (lCols->GetCount() == 1)) {
                joinmethod = eGwsBatchSortedBlock;
            }
            else {
                // nested loop join
                joinmethod = eGwsNestedLoops;
            }
        }

        prepQuery = CreatePreparedJoinQuery (pFQuery->Type (),
                                             joinmethod,
                                             pLeftQuery,
                                             pRightQuery,
                                             lCols,
                                             rCols);

        eGwsOkThrow (prepQuery->Init ());

    } catch (EGwsStatus estat) {
        if (prepQuery != NULL)
            delete prepQuery;
        else {
            delete pLeftQuery;
            delete pRightQuery;
        }
        prepQuery = NULL;
        GWS_THROW (estat);
    }

    return prepQuery;
}


void CGwsFeatureSourceQuery::ValidateJoinAttributes (
    FdoPropertyDefinition * lpdef,
    FdoPropertyDefinition * rpdef,
    FdoString             * lcol,
    FdoString             * rcol
)
{
    if (lpdef == NULL) {
        CGwsStatus stat (eGwsInvalidPropertyName);
        stat.SetParameter (L"PropertyName", lcol);
        PushStatus (stat);
        eGwsOkThrow (eGwsInvalidPropertyName);
    }
    if (rpdef == NULL) {
        CGwsStatus stat (eGwsInvalidPropertyName);
        stat.SetParameter (L"PropertyName", rcol);
        PushStatus (stat);
        eGwsOkThrow (eGwsInvalidPropertyName);
    }
    if (lpdef->GetPropertyType () != FdoPropertyType_DataProperty) {
        CGwsStatus stat (eGwsInvalidJoinAttributeType);
        stat.SetParameter (L"PropertyName", lcol);
        PushStatus (stat);
        eGwsOkThrow (eGwsInvalidJoinAttributeType);
    }
    if (rpdef->GetPropertyType () != FdoPropertyType_DataProperty) {
        CGwsStatus stat (eGwsInvalidJoinAttributeType);
        stat.SetParameter (L"PropertyName", rcol);
        PushStatus (stat);
        eGwsOkThrow (eGwsInvalidJoinAttributeType);
    }
    FdoDataPropertyDefinition * lddef = (FdoDataPropertyDefinition *) lpdef;
    if (lddef->GetDataType () == FdoDataType_BLOB ||
        lddef->GetDataType () == FdoDataType_CLOB)
    {
        CGwsStatus stat (eGwsInvalidJoinAttributeType);
        stat.SetParameter (L"PropertyName", lcol);
        PushStatus (stat);
        eGwsOkThrow (eGwsInvalidJoinAttributeType);
    }
    FdoDataPropertyDefinition * rddef = (FdoDataPropertyDefinition *) rpdef;
    if (rddef->GetDataType () == FdoDataType_BLOB ||
        rddef->GetDataType () == FdoDataType_CLOB)
    {
        CGwsStatus stat (eGwsInvalidJoinAttributeType);
        stat.SetParameter (L"PropertyName", rcol);
        PushStatus (stat);
        eGwsOkThrow (eGwsInvalidJoinAttributeType);
    }

    if (! GWSFdoUtilities::IsComparableDataTypes (lddef, rddef)) {
        CGwsStatus stat (eGwsJoinAttributesAreNotComparable);
        stat.SetParameter (L"PropertyName1", lcol);
        stat.SetParameter (L"PropertyName2", rcol);
        PushStatus (stat);
        eGwsOkThrow (eGwsJoinAttributesAreNotComparable);
    }
}


CGwsPreparedFeatureQuery * CGwsFeatureSourceQuery::CreatePreparedFeatureQuery (
//    CGwsFeatureSourceSession * sess,
    FdoIConnection          * sess,
    const GWSQualifiedName  & classname
)
{
    return new CGwsPreparedFeatureQuery (sess, classname, this);
}


CGwsPreparedJoinQuery * CGwsFeatureSourceQuery::CreatePreparedJoinQuery(
    EGwsQueryType              jtype,
    EGwsJoinMethod             joinmethod,
    CGwsPreparedQuery        * lpq,
    CGwsPreparedQuery        * rpq,
    FdoStringCollection      * lcols,
    FdoStringCollection      * rcols
)
{
    if (jtype == eGwsQueryLeftOuterJoin)
        return new CGwsPreparedLeftJoinQuery (joinmethod, lpq, rpq, lcols, rcols, this);
    else if (jtype == eGwsQueryEqualJoin)
        return new CGwsPreparedEqualJoinQuery (joinmethod, lpq, rpq, lcols, rcols, this);

    assert (false);
    return NULL;
}


void CGwsFeatureSourceQuery::SetFilter (FdoFilter * filter)
{
    if (m_pQuery == NULL) {
        Prepare ();
    }

    assert (m_pQuery); // successful preparation must initialize m_pQuery
    m_pQuery->SetFilter (filter);
}


FdoFilter * CGwsFeatureSourceQuery::GetFilter ()
{
    if (m_pQuery == NULL) {
        Prepare ();
    }

    assert (m_pQuery); // successful preparation must initialize m_pQuery
    return m_pQuery->GetFilter ();
}


const WSTR & CGwsFeatureSourceQuery::GetRevisionPropertyName ()
{
    return m_revprop;
}


void CGwsFeatureSourceQuery::Execute (IGWSFeatureIterator ** results, bool bScrollable /*false*/)
{
    if (m_pQuery == NULL) {
        Prepare ();
    }
    assert (m_pQuery); // successful preparation must initialize m_pQuery

    EGwsStatus stat = m_pQuery->Execute (results, bScrollable);
    if (IGWSException::IsError (stat)) {
        GWS_THROW_DIAG (eGwsFailedToExecuteQuery, dynamic_cast<CGwsObject *> (m_pQuery));
    }
}


void CGwsFeatureSourceQuery::Execute (
    FdoFilter            * filter,
    IGWSFeatureIterator ** results
)
{
    if (m_pQuery == NULL) {
        Prepare ();
    }

    assert (m_pQuery); // successful preparation must initialize m_pQuery
    EGwsStatus stat = m_pQuery->Execute (filter, results);
    if (IGWSException::IsError (stat)) {
        GWS_THROW_DIAG (eGwsFailedToExecuteQuery, dynamic_cast<CGwsObject *> (m_pQuery));
    }
}


void CGwsFeatureSourceQuery::Execute (
    const GWSFeatureId   & featid,
    IGWSFeatureIterator ** results
)
{
    if (m_pQuery == NULL) {
        Prepare ();
    }
    assert (m_pQuery); // successful preparation must initialize m_pQuery

    EGwsStatus stat = m_pQuery->Execute (featid, results);
    if (IGWSException::IsError (stat)) {
        GWS_THROW_DIAG (eGwsFailedToExecuteQuery, dynamic_cast<CGwsObject *> (m_pQuery));
    }
}


void CGwsFeatureSourceQuery::Execute (
    const GwsFeaturesIdVector & featids,
    int                         lbound,
    int                         ubound,
    IGWSFeatureIterator     **  results
)
{
    if (m_pQuery == NULL) {
        Prepare ();
    }

    assert (m_pQuery); // successful preparation must initialize m_pQuery

    EGwsStatus stat = m_pQuery->Execute (featids, lbound, ubound, results);
    if (IGWSException::IsError (stat)) {
        GWS_THROW_DIAG (eGwsFailedToExecuteQuery, dynamic_cast<CGwsObject *> (m_pQuery));
    }
}


CGwsPreparedFeatureQuery * CGwsFeatureSourceQuery::GetPrimaryPreparedQuery ()
{
    if (m_pQuery == NULL)
        return NULL;
    return m_pQuery->GetPrimaryQuery ();

}
