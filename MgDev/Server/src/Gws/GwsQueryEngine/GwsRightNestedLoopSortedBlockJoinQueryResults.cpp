//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
// class CGwsRightNestedLoopSortedBlockJoinQueryResults
//
/////////////////////////////////////////////////////////////////////

CGwsRightNestedLoopSortedBlockJoinQueryResults::CGwsRightNestedLoopSortedBlockJoinQueryResults ()
{
}


CGwsRightNestedLoopSortedBlockJoinQueryResults::~CGwsRightNestedLoopSortedBlockJoinQueryResults () throw()
{
}


EGwsStatus CGwsRightNestedLoopSortedBlockJoinQueryResults::InitializeReader  (
    IGWSQuery                  * query,
    CGwsPreparedQuery          * prepquery,
    FdoStringCollection        * joincols,
    bool                       bScrollable
)
{
    return CGwsRightNestedLoopJoinQueryResults::InitializeReader (query, prepquery, joincols, bScrollable);
}


bool CGwsRightNestedLoopSortedBlockJoinQueryResults::ReadNext()
{
    if (m_usepool) {
        // reading from the pool
        if (m_poolpos + 1 < m_pool->GetCount ()) {
            m_poolpos ++;
            return true;
        }
        return false;
    }

    bool bRet = CGwsRightNestedLoopJoinQueryResults::ReadNext ();
    if (bRet)
        m_pool->AddFeature (this);
    return bRet;
}


void CGwsRightNestedLoopSortedBlockJoinQueryResults::Close ()
{
    CGwsRightNestedLoopJoinQueryResults::Close ();
}


EGwsStatus CGwsRightNestedLoopSortedBlockJoinQueryResults::SetRelatedValues (
    const GWSFeatureId & vals
)
{
    if (m_joinkeys == vals) {
        m_usepool = true;
        m_poolpos = -1;
        return eGwsOk;
    }
    m_pool->Reset ();
    m_usepool = false;
    m_poolpos = -1;

    return CGwsRightNestedLoopJoinQueryResults::SetRelatedValues (vals);
}
