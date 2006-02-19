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
// class CGwsJoinQueryResults
//
/////////////////////////////////////////////////////////////////////

CGwsJoinQueryResults::CGwsJoinQueryResults()
{
    m_reader = NULL;
    m_query = NULL;
    m_prepquery = NULL;
    m_leftcols = NULL;
    m_bLeftJoinValuesSet = false;
}

EGwsStatus CGwsJoinQueryResults::InitializeReader (
    GisStringCollection             * leftjoincols,
    IGWSQuery                       * query,
    CGwsPreparedQuery               * prepquery
)
{
    EGwsStatus stat = eGwsOk;

    IGWSFeatureIterator * left = NULL;
    stat = prepquery->Execute (& left);
    if (IGWSException::IsError (stat)) {
        PushStatus  (stat);
        return stat;
    }

    m_reader = left;

    m_query = query;
    if (query != NULL)
        query->AddRef ();
    m_prepquery = prepquery;

    m_leftcols = leftjoincols;
    if (leftjoincols != NULL)
        leftjoincols->AddRef ();

    return stat;
}

CGwsJoinQueryResults::~CGwsJoinQueryResults  () throw()
{
    // this is FdoIDisposable not virtual AddRef
    IGWSFeatureIterator * fiter = dynamic_cast<IGWSFeatureIterator *> (m_reader.p);
    if (fiter != NULL) {
        fiter->Release  ();
        m_reader.p = NULL;
    }
    if (m_right != NULL)
        m_right->Release ();
}

bool CGwsJoinQueryResults::ReadNext ()
{
    m_bLeftJoinValuesSet = false;
    bool bRes = CGwsFeatureIterator::ReadNext ();

    if (m_prepquery->QueryType () == eGwsQueryLeftOuterJoin) {
        return bRes;

    } else {
        while (bRes) {
            GisPtr<IGWSFeatureIterator> riter = GetJoinedFeatures ();

            if (riter->ReadNext ()) {
                break;
            }
            m_bLeftJoinValuesSet = false;
            bRes = CGwsFeatureIterator::ReadNext ();
        }
    }
    return bRes;
}


FdoDataValueCollection * CGwsJoinQueryResults::GetJoinValues ()
{
    if (! m_bLeftJoinValuesSet) {
        IGWSFeatureIterator * fiter = dynamic_cast<IGWSFeatureIterator *> (m_reader.p);
        GisPtr<FdoDataValueCollection> coll = fiter->GetDataValues (m_leftcols);
        m_leftJoinVals = coll;
        m_bLeftJoinValuesSet = true;
    }
    return NULL; // m_leftJoinVals;
}

IGWSFeatureIterator * CGwsJoinQueryResults::GetJoinedFeatures (int iJoin)
{
    // check join feature index
    GisPtr<IGWSExtendedFeatureDescription> fdsc;
    DescribeFeature (& fdsc);
    if (iJoin >= fdsc->GetCount ())
        GWS_THROW (eGwsIndexOutOfBounds);

    if (iJoin < m_prepquery->GetPathLength () - 1) {
        if (m_prepquery->QueryType () == eGwsQueryLeftOuterJoin ||
            m_prepquery->QueryType () == eGwsQueryEqualJoin)
        {
            CGwsJoinQueryResults * jqr = dynamic_cast<CGwsJoinQueryResults*> (m_reader.p);
            assert (jqr);
            return jqr->GetJoinedFeatures (iJoin);
        }
    }

    return GetJoinedFeatures ();
}

IGWSFeatureIterator * CGwsJoinQueryResults::GetJoinedFeatures ()
{
    GetJoinValues ();
    assert (m_bLeftJoinValuesSet);

    EGwsStatus stat = m_right->SetRelatedValues (m_leftJoinVals);
    if (IGWSException::IsError (stat)) {
        CopyStatus (* m_right); // TODO: candidate for exception throwing
        return NULL;
    }
    m_right->AddRef ();
    return m_right;
}


GisInt32 CGwsJoinQueryResults::GetRevisionNumber ()
{
    // get it from the left side
    IGWSFeatureIterator * fiter = dynamic_cast<IGWSFeatureIterator *> (m_reader.p);
    return fiter->GetRevisionNumber ();
}

GWSFeatureId CGwsJoinQueryResults::GetFeatureId ()
{
    IGWSFeatureIterator * fiter = dynamic_cast<IGWSFeatureIterator *> (m_reader.p);
    return fiter->GetFeatureId ();
}

void CGwsJoinQueryResults::Close ()
{
    assert (m_reader && m_right);
    try {
        CGwsFeatureIterator::Close ();
        m_right->CGwsFeatureIterator::Close ();
    } catch (GisException * gis) {
        PushGisException (eGwsFdoProviderError, gis);
        gis->Release ();
    }
    return ;
}
