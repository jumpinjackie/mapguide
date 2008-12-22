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
// class CGwsSortMergeJoinQueryResults
//
/////////////////////////////////////////////////////////////////////

CGwsSortMergeJoinQueryResults::CGwsSortMergeJoinQueryResults (
)
{
    m_right = NULL;
}


EGwsStatus CGwsSortMergeJoinQueryResults::InitializeReader (
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

    FdoPtr<IGWSFeatureIterator> right;
    stat = rightquery->Execute (& right, bScrollable);
    if (IGWSException::IsError (stat)) {
        PushStatus  (stat);
        return stat;
    }

    CGwsRightJoinQueryResults * results = (CGwsRightJoinQueryResults *) rightquery->CreateFeatureIterator (eGwsRightSortMergeJoinIterator);

    stat = results->InitializeReader (query, right, rightcols, bScrollable);

    if (IGWSException::IsError (stat)) {
        delete results;
    } else {
        m_right = results;
        m_right->AddRef ();
    }

    return stat;
}


CGwsSortMergeJoinQueryResults::~CGwsSortMergeJoinQueryResults () throw()
{
}
