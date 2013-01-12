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
// class CGwsNestedLoopSortedBlockJoinQueryResults
//
/////////////////////////////////////////////////////////////////////

CGwsNestedLoopSortedBlockJoinQueryResults::CGwsNestedLoopSortedBlockJoinQueryResults (
)
{
    m_right = NULL;
}


CGwsNestedLoopSortedBlockJoinQueryResults::~CGwsNestedLoopSortedBlockJoinQueryResults (
) throw()
{
}


EGwsStatus CGwsNestedLoopSortedBlockJoinQueryResults::InitializeReader (
    IGWSQuery                       * query,
    CGwsPreparedJoinQuery           * prepquery,
    bool                            bScrollable
)
{
    EGwsStatus                  stat = eGwsOk;
    CGwsPreparedQuery         * leftquery = prepquery->LeftQuery ();
    CGwsPreparedQuery         * rightquery = prepquery->RightQuery ();
    FdoPtr<FdoStringCollection> leftcols = prepquery->LeftProperties ();
    FdoPtr<FdoStringCollection> rightcols = prepquery->RightProperties ();

    stat = CGwsJoinQueryResults::InitializeReader (leftcols, query, leftquery, bScrollable);
    if (IGWSException::IsError (stat)) {
        PushStatus  (stat);
        return stat;
    }
    m_prepquery = prepquery;
    CGwsRightJoinQueryResults * results =
            (CGwsRightJoinQueryResults *) rightquery->CreateFeatureIterator (eGwsRightNestedLoopSortedBlockIterator);
    stat = results->InitializeReader (query, rightquery, rightcols, bScrollable);

    if (IGWSException::IsError (stat)) {
        delete results;
    } else {
        m_right = results;
        m_right->AddRef ();
    }

    return stat;
}
