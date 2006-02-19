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
// class CGwsRightNestedLoopJoinQueryResults
//
/////////////////////////////////////////////////////////////////////

CGwsRightNestedLoopJoinQueryResults::CGwsRightNestedLoopJoinQueryResults ()
{
    m_bClosed = true;
}


CGwsRightNestedLoopJoinQueryResults::~CGwsRightNestedLoopJoinQueryResults () throw()
{
}


EGwsStatus CGwsRightNestedLoopJoinQueryResults::InitializeReader (
    IGWSQuery                  * query,
    CGwsPreparedQuery          * prepquery,
    GisStringCollection        * joincols
)
{
    m_bClosed = true;
    return CGwsRightJoinQueryResults::InitializeReader (query, prepquery, joincols);
}

bool CGwsRightNestedLoopJoinQueryResults::ReadNext()
{
    return CGwsRightJoinQueryResults::ReadNext ();
}
void CGwsRightNestedLoopJoinQueryResults::Close ()
{
    if (! m_bClosed)
        CGwsRightJoinQueryResults::Close ();
    m_bClosed = true;
}

EGwsStatus CGwsRightNestedLoopJoinQueryResults::SetRelatedValues (
    const GWSFeatureId & vals
 )
{
    EGwsStatus stat = eGwsOk;

    try {
        Close ();

        GisPtr<FdoFilter> pFilter;

        for (int idx = 0; idx < m_joincols->GetCount (); idx ++) {
            GisString          * propname = m_joincols->GetString (idx);
            GisPtr<FdoDataValue> val = vals.GetItem (idx);

            GisPtr<FdoIdentifier> pFeatureNameProperty = FdoIdentifier::Create(propname);
            if (idx == 0) {
                pFilter = FdoComparisonCondition::Create(pFeatureNameProperty,
                                                         FdoComparisonOperations_EqualTo,
                                                         val);
            } else {
                GisPtr<FdoFilter> pRhsFilter = FdoComparisonCondition::Create(pFeatureNameProperty,
                                                                              FdoComparisonOperations_EqualTo,
                                                                              val);
                GisPtr<FdoFilter> pCombinedFilter = FdoFilter::Combine(pFilter,
                                                                       FdoBinaryLogicalOperations_And,
                                                                       pRhsFilter);
                pFilter = pCombinedFilter;
            }
        }

        m_prepquery->SetFilter (pFilter);
        IGWSFeatureIterator * fiter = NULL;
        stat = m_prepquery->Execute (& fiter);
        if (IGWSException::IsError (stat)) {
            PushStatus (stat);
            return stat;
        }
        m_reader = dynamic_cast<FdoIFeatureReader*> (fiter);
        m_bClosed = false;

        stat = CGwsRightJoinQueryResults::SetRelatedValues (vals);

    } catch (GisException *e) {
        PushGisException (eGwsFailed, e);
        e->Release();
        stat = eGwsFailed;
    }
    return stat;
}
