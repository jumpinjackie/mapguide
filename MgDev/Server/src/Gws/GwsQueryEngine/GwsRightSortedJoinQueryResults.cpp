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
// class CGwsRightSortedJoinQueryResults
//
/////////////////////////////////////////////////////////////////////

CGwsRightSortedJoinQueryResults::CGwsRightSortedJoinQueryResults (
)
{
    m_pos    = eBeforeFirstRow;
}


CGwsRightSortedJoinQueryResults::~CGwsRightSortedJoinQueryResults () throw()
{
}


EGwsStatus CGwsRightSortedJoinQueryResults::InitializeReader (
    IGWSQuery           * query,
    FdoIFeatureReader   * reader,
    FdoStringCollection * joincols,
    bool                bScrollable
)
{
    return CGwsRightJoinQueryResults::InitializeReader (query, reader, joincols, bScrollable);
}


EGwsStatus CGwsRightSortedJoinQueryResults::SetRelatedValues (const GWSFeatureId & vals)
{
    if (! m_neverusepooling) {
        if (m_joinkeys == vals) {
            Close (); // need to fill the cache
            m_usepool = true;
            m_poolpos = -1;
        } else {
            m_pool->Reset ();
            m_usepool = false;
            m_poolpos = -1;
        }
    }
    return CGwsRightJoinQueryResults::SetRelatedValues (vals);
}


void CGwsRightSortedJoinQueryResults::Close ()
{
    // fill the pool
    if (m_pos == eOnJoinRow) {
        while (ReadNext ())
            ;
    }
}


bool CGwsRightSortedJoinQueryResults::ReadNext()
{
    bool bRes = false;
    if ( m_pos == eAfterLastRow && ! m_usepool)
        return false;

    if (m_pos == eAfterJoinRow || m_pos == eAfterLastRow) {
        if (m_usepool) {
            // reading from the pool
            if (m_poolpos + 1 < m_pool->GetCount ()) {
                m_poolpos ++;
                return true;
            }
            return false;
        }
    }

    if (m_pos != eAfterJoinRow)
        bRes = ((FdoIFeatureReader *)m_reader)->ReadNext ();
    else {
        bRes = true;
    }

    if (! bRes)
        m_pos = eAfterLastRow;

    IGWSFeatureIterator * fiter = dynamic_cast<IGWSFeatureIterator *> (m_reader.p);

    while (bRes) {
        FdoPtr<FdoDataValueCollection> joinvals =
                                            fiter->GetDataValues (m_joincols);
        m_joinvals = joinvals;
        int res = m_joinkeys.Compare (m_joinvals);

        if (res == 1) {
            bRes = m_reader->ReadNext ();
            if (! bRes)
                m_pos = eAfterLastRow;

        } else if (res == 0) {
            if (m_joinvals.IsNull ()) {
                // nulls are reported by compare as equal. In the join
                // case this is not true
                bRes = false;

            } else {
                if (! m_neverusepooling)
                    m_pool->AddFeature (fiter);
                m_pos = eOnJoinRow;
            }
            break;

        } else if (res == -1) {
            m_pos = eAfterJoinRow;
            bRes = false;

        } else if (res == 2) {
            // right is null, continue
            bRes = m_reader->ReadNext ();
            if (! bRes)
                m_pos = eAfterLastRow;

        } else if (res == -2) {
            // left is null
            bRes = false;

        }
    }
    return bRes;
}
