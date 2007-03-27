//
//  Copyright (C) 2004-2007  Autodesk, Inc.
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
// class CGwsRightBatchSortedBlockJoinQueryResults
//
/////////////////////////////////////////////////////////////////////
CGwsRightBatchSortedBlockJoinQueryResults::CGwsRightBatchSortedBlockJoinQueryResults ()
{
    m_pos = eBeforeFirstRow;
}

CGwsRightBatchSortedBlockJoinQueryResults::~CGwsRightBatchSortedBlockJoinQueryResults () throw()
{
}

EGwsStatus CGwsRightBatchSortedBlockJoinQueryResults::InitializeReader  (
    IGWSQuery                  * query,
    CGwsPreparedQuery          * prepquery,
    FdoStringCollection        * joincols,
    bool                       bScrollable
)
{
    return CGwsRightNestedLoopJoinQueryResults::InitializeReader (query, prepquery, joincols, bScrollable);
}

bool CGwsRightBatchSortedBlockJoinQueryResults::ReadNext()
{
    if (m_usepool) {
        // reading from the pool
        if (m_poolpos + 1 < m_pool->GetCount ()) {
            m_poolpos ++;
            return true;
        }
        return false;
    }

    bool bRet = false;
    if(m_pos == eBeforeFirstRow)
    {
        bRet = CGwsRightJoinQueryResults::ReadNext ();
    }
    else
    {
        // Next read will be for the next primary key so we don't want to advance the secondary
        m_pos = eBeforeFirstRow;
    }

    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext() - JoinColumns:%S\n", m_joincols->ToString());
    #endif

    if (bRet)
    {
        #ifdef _DEBUG_BATCHSORT_JOIN
        printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext()\n");
        #endif
        m_pool->AddFeature (this);
        m_pos = eOnJoinRow;
    }
    return bRet;
}

void CGwsRightBatchSortedBlockJoinQueryResults::Close ()
{
    CGwsRightNestedLoopJoinQueryResults::Close ();
}

EGwsStatus CGwsRightBatchSortedBlockJoinQueryResults::SetRelatedValues (
    const GWSFeatureId & vals
)
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  CGwsRightBatchSortedBlockJoinQueryResults::SetRelatedValues()\n");
    #endif
        if (m_joinkeys == vals) {
            if (! m_neverusepooling) {
                // this completes features pool
                if (! m_bClosed) {
                    while (ReadNext ())
                        ;
                    Close ();
                }
            }
            m_usepool = true;
            m_poolpos = -1;
            return eGwsOk;
        }
        m_pool->Reset ();
        m_usepool = false;
        m_poolpos = -1;

    // The code below is similar to the base class, but we want to use an IN clause in order to
    // do a batch comparison instead of the typical = and LIKE operators.
    EGwsStatus stat = eGwsOk;

    try {
        Close ();

        FdoString* propname = m_joincols->GetString (0);
        WSTR filter = propname;
        filter += L" IN (";

        FdoPtr<FdoFilter> pFilter;

        for (int idx = 0; idx < vals.GetCount (); idx ++)
        {
            if(idx > 0)
            {
                filter += L",";
            }

            FdoPtr<FdoDataValue> val = vals.GetItem (idx);
            FdoStringP valStr = val->ToString();
            filter += valStr;
        }

        filter += L")";
        pFilter = FdoFilter::Parse(filter.c_str());
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

    } catch (FdoException *e) {
        PushFdoException (eGwsFailed, e);
        e->Release();
        stat = eGwsFailed;
    }
    return stat;
}
