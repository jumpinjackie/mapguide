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
// class CGwsRightJoinQueryResults
//
/////////////////////////////////////////////////////////////////////

CGwsRightJoinQueryResults::CGwsRightJoinQueryResults   ()
{
    m_prepquery = NULL;
    m_pool = NULL;
    m_usepool = false;
    m_poolpos = -1;
    m_neverusepooling = false;
}


CGwsRightJoinQueryResults::~CGwsRightJoinQueryResults  () throw()
{
    // not virtual FdoIDisposable AddRef ...
    IGWSFeatureIterator * fiter = dynamic_cast<IGWSFeatureIterator *> (m_reader.p);
    if (fiter != NULL) {
        fiter->Release  ();
        m_reader.p = NULL;
    }
    delete m_pool;
}


EGwsStatus CGwsRightJoinQueryResults::InitializeReader (
    IGWSQuery           * query,
    FdoIFeatureReader   * reader,
    FdoStringCollection * joincols,
    bool                bScrollable
)
{
    m_reader = reader;
    m_bScrollableReader = bScrollable;

    CGwsFeatureIterator * fiter = dynamic_cast <CGwsFeatureIterator *> (reader);
    if (fiter != NULL) {
        fiter->AddRef ();
    } else if (reader != NULL) {
        reader->AddRef ();
    }
    m_query = query;
    if (query != NULL)
        query->AddRef ();
    if (fiter != NULL)
        m_prepquery = fiter->PrepQuery();

    m_joincols = joincols;
    if (joincols != NULL)
        joincols->AddRef ();

    FdoPtr<IGWSExtendedFeatureDescription> ifdsc;

    DescribeFeature (& ifdsc);
    m_pool = new CGwsFeaturesPool (ifdsc);
    m_usepool = false;
    m_poolpos = -1;
    m_neverusepooling = false;
    return eGwsOk;
}


EGwsStatus CGwsRightJoinQueryResults::InitializeReader (
    IGWSQuery                * query,
    CGwsPreparedQuery        * prepquery,
    FdoStringCollection      * joincols,
    bool                    bScrollable
)
{
    m_reader = NULL;
    m_bScrollableReader = bScrollable;

    m_query = query;
    if (query != NULL)
        query->AddRef ();

    m_prepquery = prepquery;
    m_joincols = joincols;
    if (joincols != NULL)
        joincols->AddRef ();

    FdoPtr<IGWSExtendedFeatureDescription> ifdsc;

    DescribeFeature (& ifdsc);
    m_pool = new CGwsFeaturesPool (ifdsc);
    m_usepool = false;
    m_poolpos = -1;
    m_neverusepooling = false;
    return eGwsOk;
}


void CGwsRightJoinQueryResults::SetNeverUsePooling ()
{
    if (m_prepquery->QueryType () == eGwsQueryLeftOuterJoin)
        m_neverusepooling = true;
}


void CGwsRightJoinQueryResults::DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc)
{
    IGWSFeatureIterator * fiter = dynamic_cast<IGWSFeatureIterator *> (m_reader.p);
    assert (fiter != NULL || m_prepquery != NULL);
    if (fiter != NULL)
        fiter->DescribeFeature (ppResDesc);
    else
        m_prepquery->DescribeResults (ppResDesc);
}


EGwsStatus CGwsRightJoinQueryResults::SetRelatedValues (const GWSFeatureId & vals)
{
    m_joinkeys = vals;
    return eGwsOk;
}


IGWSFeatureIterator * CGwsRightJoinQueryResults::GetJoinedFeatures (int i)
{
    CGwsFeatureIterator * fiter = dynamic_cast <CGwsFeatureIterator *> (m_reader.p);
    if (fiter != NULL) {
        return fiter->GetJoinedFeatures (i);
    }
    return NULL;
}


IGWSFeature * CGwsRightJoinQueryResults::GetPooledFeature ()
{
    if (m_poolpos < m_pool->GetCount ()) {
        return m_pool->GetFeature (m_poolpos);
    }
    throw FdoException::Create (L"Feature iterator is closed");
}


FdoInt32 CGwsRightJoinQueryResults::GetRevisionNumber ()
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetRevisionNumber ();
    }
    return CGwsFeatureIterator::GetRevisionNumber ();
}


GWSFeatureId CGwsRightJoinQueryResults::GetFeatureId ()
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetFeatureId ();
    }
    return CGwsFeatureIterator::GetFeatureId ();
}


bool CGwsRightJoinQueryResults::IsNull (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->IsNull (propertyName);
    }
    return CGwsFeatureIterator::IsNull (propertyName);
}


FdoString * CGwsRightJoinQueryResults::GetString (FdoString * propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetString (propertyName);
    }
    return CGwsFeatureIterator::GetString (propertyName);
}


bool CGwsRightJoinQueryResults::GetBoolean  (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetBoolean (propertyName);
    }
    return CGwsFeatureIterator::GetBoolean (propertyName);
}


FdoByte  CGwsRightJoinQueryResults::GetByte (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetByte (propertyName);
    }
    return CGwsFeatureIterator::GetByte (propertyName);
}


FdoDateTime CGwsRightJoinQueryResults::GetDateTime (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDateTime (propertyName);
    }
    return CGwsFeatureIterator::GetDateTime (propertyName);
}


double CGwsRightJoinQueryResults::GetDouble   (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDouble (propertyName);
    }
    return CGwsFeatureIterator::GetDouble (propertyName);
}


FdoInt16 CGwsRightJoinQueryResults::GetInt16 (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt16 (propertyName);
    }
    return CGwsFeatureIterator::GetInt16 (propertyName);
}


FdoInt32 CGwsRightJoinQueryResults::GetInt32 (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt32 (propertyName);
    }
    return CGwsFeatureIterator::GetInt32 (propertyName);
}


FdoInt64 CGwsRightJoinQueryResults::GetInt64    (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt64 (propertyName);
    }
    return CGwsFeatureIterator::GetInt64 (propertyName);
}


float CGwsRightJoinQueryResults::GetSingle   (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetSingle (propertyName);
    }
    return CGwsFeatureIterator::GetSingle (propertyName);
}


FdoLOBValue* CGwsRightJoinQueryResults::GetLOB (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        // Not implemented yet
        assert (false);
        return NULL;
    }
    return CGwsFeatureIterator::GetLOB (propertyName);
}


FdoIStreamReader* CGwsRightJoinQueryResults::GetLOBStreamReader(const wchar_t* propertyName )
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        // Not implemented yet
        assert (false);
        return NULL;
    }
    return CGwsFeatureIterator::GetLOBStreamReader (propertyName);
}


FdoIRaster* CGwsRightJoinQueryResults::GetRaster (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        // Not implemented yet
        assert (false);
        return NULL;
    }
    return CGwsFeatureIterator::GetRaster (propertyName);
}


const FdoByte * CGwsRightJoinQueryResults::GetGeometry (FdoString* propertyName, FdoInt32 * count)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetGeometry (propertyName, count);
    }
    return CGwsFeatureIterator::GetGeometry (propertyName, count);
}


FdoByteArray* CGwsRightJoinQueryResults::GetGeometry (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetGeometry (propertyName);
    }
    return CGwsFeatureIterator::GetGeometry (propertyName);
}


FdoByteArray * CGwsRightJoinQueryResults::GetOriginalGeometry (FdoString* propertyName)
{
    // TODO: check convesion.
    // This is not actual for now since the right side has to be data table in
    // map, but ...
    return GetGeometry (propertyName);
}


bool CGwsRightJoinQueryResults::ConvertingGeometry ()
{
    return CGwsFeatureIterator::ConvertingGeometry ();
}


FdoIFeatureReader* CGwsRightJoinQueryResults::GetFeatureObject(FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetFeatureObject (propertyName);
        return NULL;
    }
    return CGwsFeatureIterator::GetFeatureObject (propertyName);
}


FdoDataValue *  CGwsRightJoinQueryResults::GetDataValue (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDataValue (propertyName);
    }
    return CGwsFeatureIterator::GetDataValue (propertyName);
}


FdoDataValueCollection * CGwsRightJoinQueryResults::GetDataValues (FdoStringCollection* propertyNames)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDataValues (propertyNames);
    }
    return CGwsFeatureIterator::GetDataValues (propertyNames);
}


/*
bool CGwsRightJoinQueryResults::IsNull (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->IsNull (iProp);
    }
    return CGwsFeatureIterator::IsNull (iProp);
}


FdoString * CGwsRightJoinQueryResults::GetString (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetString (iProp);
    }
    return CGwsFeatureIterator::GetString (iProp);
}


bool CGwsRightJoinQueryResults::GetBoolean  (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetBoolean (iProp);
    }
    return CGwsFeatureIterator::GetBoolean (iProp);
}


FdoByte CGwsRightJoinQueryResults::GetByte (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetByte (iProp);
    }
    return CGwsFeatureIterator::GetByte (iProp);
}


FdoDateTime CGwsRightJoinQueryResults::GetDateTime (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDateTime (iProp);
    }
    return CGwsFeatureIterator::GetDateTime (iProp);
}


double CGwsRightJoinQueryResults::GetDouble (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDouble (iProp);
    }
    return CGwsFeatureIterator::GetDouble (iProp);
}


FdoInt16 CGwsRightJoinQueryResults::GetInt16 (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt16 (iProp);
    }
    return CGwsFeatureIterator::GetInt16 (iProp);
}


FdoInt32 CGwsRightJoinQueryResults::GetInt32(FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt32 (iProp);
    }
    return CGwsFeatureIterator::GetInt32 (iProp);
}


FdoInt64 CGwsRightJoinQueryResults::GetInt64 (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt64 (iProp);
    }
    return CGwsFeatureIterator::GetInt64 (iProp);
}


float CGwsRightJoinQueryResults::GetSingle (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetSingle (iProp);
    }
    return CGwsFeatureIterator::GetSingle (iProp);
}


FdoLOBValue* CGwsRightJoinQueryResults::GetLOB (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetLOB (iProp);
    }
    return CGwsFeatureIterator::GetLOB (iProp);
}


FdoIStreamReader* CGwsRightJoinQueryResults::GetLOBStreamReader (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetLOBStreamReader (iProp);
    }
    return CGwsFeatureIterator::GetLOBStreamReader (iProp);
}


FdoIRaster* CGwsRightJoinQueryResults::GetRaster   (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetRaster (iProp);
    }
    return CGwsFeatureIterator::GetRaster (iProp);
}


const FdoByte * CGwsRightJoinQueryResults::GetGeometry (FdoInt32 iProp, FdoInt32 * count)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetGeometry (iProp, count);
    }
    return CGwsFeatureIterator::GetGeometry (iProp, count);
}


FdoByteArray*   CGwsRightJoinQueryResults::GetGeometry (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetGeometry (iProp);
    }
    return CGwsFeatureIterator::GetGeometry (iProp);
}


FdoIFeatureReader* CGwsRightJoinQueryResults::GetFeatureObject (FdoInt32 iProp)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetFeatureObject (iProp);
    }
    return CGwsFeatureIterator::GetFeatureObject (iProp);
}


void CGwsRightJoinQueryResults::ToString (
    FdoInt32    iProp,
    wchar_t *   buff,
    int         len
)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->ToString (iProp, buff, len);
    }
    return CGwsFeatureIterator::ToString (iProp, buff, len);
}
*/


void CGwsRightJoinQueryResults::ToString (
    FdoString*  propertyName,
    wchar_t *   buff,
    int         len
)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->ToString (propertyName, buff, len);
    }
    return CGwsFeatureIterator::ToString (propertyName, buff, len);
}


FdoStringCollection* CGwsRightJoinQueryResults::GetJoinColumns()
{
    m_joincols.p->AddRef();
    return m_joincols;
}
