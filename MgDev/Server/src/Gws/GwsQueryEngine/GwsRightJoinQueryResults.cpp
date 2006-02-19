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
    GisStringCollection * joincols
)
{
    m_reader = reader;
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
        m_prepquery = fiter->m_prepquery;

    m_joincols = joincols;
    if (joincols != NULL)
        joincols->AddRef ();

    GisPtr<IGWSExtendedFeatureDescription> ifdsc;

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
    GisStringCollection      * joincols
)
{
    m_reader = NULL;
    m_query = query;
    if (query != NULL)
        query->AddRef ();
    m_prepquery = prepquery;

    m_joincols = joincols;
    if (joincols != NULL)
        joincols->AddRef ();

    GisPtr<IGWSExtendedFeatureDescription> ifdsc;

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
    throw GisException::Create (L"Feature iterator is closed");
}

GisInt32 CGwsRightJoinQueryResults::GetRevisionNumber ()
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetRevisionNumber ();
    }
    return CGwsFeatureIterator::GetRevisionNumber ();
}

GWSFeatureId CGwsRightJoinQueryResults::GetFeatureId ()
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetFeatureId ();
    }
    return CGwsFeatureIterator::GetFeatureId ();
}


bool CGwsRightJoinQueryResults::IsNull (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->IsNull (propertyName);
    }
    return CGwsFeatureIterator::IsNull (propertyName);
}

GisString * CGwsRightJoinQueryResults::GetString (GisString * propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetString (propertyName);
    }
    return CGwsFeatureIterator::GetString (propertyName);
}

bool CGwsRightJoinQueryResults::GetBoolean  (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetBoolean (propertyName);
    }
    return CGwsFeatureIterator::GetBoolean (propertyName);
}

GisByte  CGwsRightJoinQueryResults::GetByte (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetByte (propertyName);
    }
    return CGwsFeatureIterator::GetByte (propertyName);
}

GisDateTime CGwsRightJoinQueryResults::GetDateTime (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDateTime (propertyName);
    }
    return CGwsFeatureIterator::GetDateTime (propertyName);
}

double CGwsRightJoinQueryResults::GetDouble   (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDouble (propertyName);
    }
    return CGwsFeatureIterator::GetDouble (propertyName);
}

GisInt16 CGwsRightJoinQueryResults::GetInt16 (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt16 (propertyName);
    }
    return CGwsFeatureIterator::GetInt16 (propertyName);
}

GisInt32 CGwsRightJoinQueryResults::GetInt32 (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt32 (propertyName);
    }
    return CGwsFeatureIterator::GetInt32 (propertyName);
}
GisInt64 CGwsRightJoinQueryResults::GetInt64    (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt64 (propertyName);
    }
    return CGwsFeatureIterator::GetInt64 (propertyName);
}

float CGwsRightJoinQueryResults::GetSingle   (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetSingle (propertyName);
    }
    return CGwsFeatureIterator::GetSingle (propertyName);
}

FdoLOBValue* CGwsRightJoinQueryResults::GetLOB (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        // TODO: Not implemented yet
        assert (false);
        return NULL;
    }
    return CGwsFeatureIterator::GetLOB (propertyName);
}

GisIStreamReader* CGwsRightJoinQueryResults::GetLOBStreamReader(const wchar_t* propertyName )
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        // TODO: Not implemented yet
        assert (false);
        return NULL;
    }
    return CGwsFeatureIterator::GetLOBStreamReader (propertyName);
}

FdoIRaster* CGwsRightJoinQueryResults::GetRaster (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        // TODO: Not implemented yet
        assert (false);
        return NULL;
    }
    return CGwsFeatureIterator::GetRaster (propertyName);
}

const GisByte * CGwsRightJoinQueryResults::GetGeometry (GisString* propertyName, GisInt32 * count)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetGeometry (propertyName, count);
    }
    return CGwsFeatureIterator::GetGeometry (propertyName, count);
}

GisByteArray* CGwsRightJoinQueryResults::GetGeometry (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetGeometry (propertyName);
    }
    return CGwsFeatureIterator::GetGeometry (propertyName);
}

FdoIFeatureReader* CGwsRightJoinQueryResults::GetFeatureObject(GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetFeatureObject (propertyName);
    }
    return CGwsFeatureIterator::GetFeatureObject (propertyName);
}

FdoDataValue *  CGwsRightJoinQueryResults::GetDataValue (GisString* propertyName)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDataValue (propertyName);
    }
    return CGwsFeatureIterator::GetDataValue (propertyName);
}

FdoDataValueCollection * CGwsRightJoinQueryResults::GetDataValues (GisStringCollection* propertyNames)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDataValues (propertyNames);
    }
    return CGwsFeatureIterator::GetDataValues (propertyNames);
}

/*

bool CGwsRightJoinQueryResults::IsNull (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->IsNull (iProp);
    }
    return CGwsFeatureIterator::IsNull (iProp);
}

GisString * CGwsRightJoinQueryResults::GetString (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetString (iProp);
    }
    return CGwsFeatureIterator::GetString (iProp);
}
bool CGwsRightJoinQueryResults::GetBoolean  (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetBoolean (iProp);
    }
    return CGwsFeatureIterator::GetBoolean (iProp);
}
GisByte CGwsRightJoinQueryResults::GetByte (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetByte (iProp);
    }
    return CGwsFeatureIterator::GetByte (iProp);
}
GisDateTime CGwsRightJoinQueryResults::GetDateTime (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDateTime (iProp);
    }
    return CGwsFeatureIterator::GetDateTime (iProp);
}
double CGwsRightJoinQueryResults::GetDouble (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetDouble (iProp);
    }
    return CGwsFeatureIterator::GetDouble (iProp);
}
GisInt16 CGwsRightJoinQueryResults::GetInt16 (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt16 (iProp);
    }
    return CGwsFeatureIterator::GetInt16 (iProp);
}
GisInt32 CGwsRightJoinQueryResults::GetInt32(GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt32 (iProp);
    }
    return CGwsFeatureIterator::GetInt32 (iProp);
}
GisInt64 CGwsRightJoinQueryResults::GetInt64 (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetInt64 (iProp);
    }
    return CGwsFeatureIterator::GetInt64 (iProp);
}
float CGwsRightJoinQueryResults::GetSingle (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetSingle (iProp);
    }
    return CGwsFeatureIterator::GetSingle (iProp);
}
FdoLOBValue* CGwsRightJoinQueryResults::GetLOB (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetLOB (iProp);
    }
    return CGwsFeatureIterator::GetLOB (iProp);
}
GisIStreamReader* CGwsRightJoinQueryResults::GetLOBStreamReader (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetLOBStreamReader (iProp);
    }
    return CGwsFeatureIterator::GetLOBStreamReader (iProp);
}
FdoIRaster* CGwsRightJoinQueryResults::GetRaster   (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetRaster (iProp);
    }
    return CGwsFeatureIterator::GetRaster (iProp);
}
const GisByte * CGwsRightJoinQueryResults::GetGeometry (GisInt32 iProp, GisInt32 * count)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetGeometry (iProp, count);
    }
    return CGwsFeatureIterator::GetGeometry (iProp, count);
}
GisByteArray*   CGwsRightJoinQueryResults::GetGeometry (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetGeometry (iProp);
    }
    return CGwsFeatureIterator::GetGeometry (iProp);
}
FdoIFeatureReader* CGwsRightJoinQueryResults::GetFeatureObject (GisInt32 iProp)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->GetFeatureObject (iProp);
    }
    return CGwsFeatureIterator::GetFeatureObject (iProp);
}

void CGwsRightJoinQueryResults::ToString (
    GisInt32    iProp,
    wchar_t *   buff,
    int         len
)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->ToString (iProp, buff, len);
    }
    return CGwsFeatureIterator::ToString (iProp, buff, len);
}

*/

void CGwsRightJoinQueryResults::ToString (
    GisString*  propertyName,
    wchar_t *   buff,
    int         len
)
{
    if (m_usepool) {
        GisPtr<IGWSFeature> f = GetPooledFeature ();
        return f->ToString (propertyName, buff, len);
    }
    return CGwsFeatureIterator::ToString (propertyName, buff, len);
}
