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
    m_bScrollableReader = true;
    m_forceOneToOne = true;
}


EGwsStatus CGwsJoinQueryResults::InitializeReader (
    FdoStringCollection             * leftjoincols,
    IGWSQuery                       * query,
    CGwsPreparedQuery               * prepquery,
    bool                            bScrollable
)
{
    EGwsStatus stat = eGwsOk;
    m_started = false;

    m_bScrollableReader = bScrollable;

    IGWSFeatureIterator * left = NULL;
    stat = prepquery->Execute (& left, m_bScrollableReader);
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

    FdoPtr<IGWSQueryDefinition> qDef;
    m_query->GetQueryDefinition(&qDef);
    IGWSJoinQueryDefinition* jqDef = dynamic_cast<IGWSJoinQueryDefinition*>(qDef.p);
    m_forceOneToOne = (NULL != jqDef) ? jqDef->ForceOneToOne() : true;

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
    return SetupRightSide(bRes);
}


bool CGwsJoinQueryResults::CacheReadNext ()
{
    if(!m_forceOneToOne) {
        if(m_started) {
            // read right hand side first
            if(false == m_bLeftJoinValuesSet)
                GetJoinedFeatures ();
            if(m_right != NULL) {
                if(m_right->ReadNext())
                    return true;
                else {
                    m_started = false;
                    return false;
                }
            }
        }
    }
    m_bLeftJoinValuesSet = false;
    m_started = CGwsFeatureIterator::ReadNext ();
    return SetupRightSide(m_started);
}


bool CGwsJoinQueryResults::SetupRightSide(bool bRes) {

    if (m_prepquery->QueryType () == eGwsQueryLeftOuterJoin) {
        return bRes;

    } else {
        while (bRes) {
            FdoPtr<IGWSFeatureIterator> riter = GetJoinedFeatures ();

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
        FdoPtr<FdoDataValueCollection> coll = fiter->GetDataValues (m_leftcols);
        m_leftJoinVals = coll;
        m_bLeftJoinValuesSet = true;
    }
    return NULL; // m_leftJoinVals;
}


FdoDataValueCollection * CGwsJoinQueryResults::GetDataValues (
    FdoStringCollection* propertyNames
)
{
    CGwsDataValueCollection * vals = NULL;
    for (int i = 0; i < propertyNames->GetCount (); i ++) {
        FdoString* name = propertyNames->GetString (i);
        FdoPtr<FdoDataValue> val = GetDataValue (name);
        if(NULL == val) {
            // try the right side
            FdoPtr<IGWSExtendedFeatureDescription> pFeatDesc;
            DescribeFeature(&pFeatDesc);
            if(NULL != pFeatDesc) {
                std::wstring strname(name);
                size_t delimpos = strname.find(pFeatDesc->JoinDelimiter());
                if(std::wstring::npos != delimpos) {
                    std::wstring propname = strname.substr(delimpos+1);
                    val = m_right->GetDataValue(propname.c_str());
                }
            }
        }
        if (vals == NULL)
            vals = (CGwsDataValueCollection *) CGwsDataValueCollection::Create ();
        vals->Add (val);
    }
    return vals;
}


IGWSFeatureIterator * CGwsJoinQueryResults::GetJoinedFeatures (int iJoin)
{
    // check join feature index
    FdoPtr<IGWSExtendedFeatureDescription> fdsc;
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


FdoInt32 CGwsJoinQueryResults::GetRevisionNumber ()
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
    } catch (FdoException * fdoEx) {
        PushFdoException (eGwsFdoProviderError, fdoEx);
        fdoEx->Release ();
    }
    return ;
}


int
CGwsJoinQueryResults::Count()
{
    // call the base class for left outer one-to-one case
    return CGwsFeatureIterator::Count();
}


bool
CGwsJoinQueryResults::ReadAtIndex( unsigned int recordindex )
{
    bool bRes = CGwsFeatureIterator::ReadAtIndex(recordindex);
    m_bLeftJoinValuesSet = false;
    return SetupRightSide(bRes);
}


bool CGwsJoinQueryResults::Scrollable() {
    return true;
}
