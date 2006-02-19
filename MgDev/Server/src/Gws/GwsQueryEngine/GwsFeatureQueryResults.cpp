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
#include "GwsBinaryFeatureWriter.h"

/////////////////////////////////////////////////////////////////////
//
// class CGwsFeatureIterator
//
/////////////////////////////////////////////////////////////////////
CGwsFeatureIterator::CGwsFeatureIterator ()
{
    m_reader = NULL;
    m_query = NULL;
    m_prepquery = NULL;
    m_mutableFeature = NULL;
    m_bMutableFeatureSet = false;
    m_bExposeFeatureIdAsCacheId = false;
    m_bGeometryConverted = false;
    m_pBinaryWriter = NULL;
}


CGwsFeatureIterator::~CGwsFeatureIterator () throw()
{
#ifdef _CS_DEBUG
    if (m_converter) {
        long nPoints =  m_converter->GetNumPoints ();
        printf ("\n%ld points converted\n", nPoints);
    }
#endif
    if(m_pBinaryWriter)
        delete m_pBinaryWriter;
}

EGwsStatus CGwsFeatureIterator::InitializeReader (
    FdoIFeatureReader               * pReader,
    IGWSQuery                       * fquery,
    CGwsPreparedFeatureQuery        * prepquery
)
{
    assert (pReader != NULL && fquery != NULL);
    m_reader = pReader;
    if (pReader)
        pReader->AddRef ();

    m_query = fquery;
    fquery->AddRef ();
    m_prepquery = prepquery;
    if (m_prepquery->QueryType () == eGwsQueryFeature) {
        // pfquery is actually this
        CGwsPreparedFeatureQuery * pfquery = m_prepquery->GetPrimaryQuery ();
        pfquery->GetCSConverter (& m_converter);
#ifdef _CS_DEBUG
        if (m_converter)
            m_converter->Clear ();
#endif
    }

    m_mutableFeature = NULL;
    m_bMutableFeatureSet = false;
    InitializeMutableFeature ();
    return eGwsOk;
}

const GWSCoordinateSystem & CGwsFeatureIterator::GetCSName ()
{
    static GWSCoordinateSystem csname;
    CGwsPreparedFeatureQuery * pfquery = m_prepquery->GetPrimaryQuery ();
    pfquery->GetCSConverter (& m_converter);
    if (m_converter != NULL && * m_converter->DestinationCS () != 0)
        return m_converter->DestinationCS ();

    WSTR sc = pfquery->ActiveSpatialContext ();
    GisPtr<FdoIConnection> conn = pfquery->GetFdoConnection ();
    GwsSpatialContextDescription scdesc;
    if (! IGWSException::IsError (GwsCommonFdoUtils::DescribeSC (conn, sc.c_str (), scdesc)))
        csname = GWSCoordinateSystem (scdesc.CsNameWkt ());
    return csname;
}


bool CGwsFeatureIterator::InitializeMutableFeature ()
{
    GisPtr<IGWSExtendedFeatureDescription> ifdesc;
    DescribeFeature (& ifdesc);

    CGwsMutableFeature * feat = NULL;
    IGWSMutableFeature::Create (ifdesc, NULL, (IGWSMutableFeature **)& feat);
    feat->InitializePropertyValues ();
    m_mutableFeature = feat;
    m_bMutableFeatureSet = false;

    return true;
}

void CGwsFeatureIterator::SetExposeFeatureIdAsCacheId (bool bFlag)
{
    m_bExposeFeatureIdAsCacheId = bFlag;
}


bool CGwsFeatureIterator::NextFeature (IGWSFeature ** feature)
{
    if (m_reader == NULL)
        return false;
    bool bRet = false;
    try {
        m_bMutableFeatureSet = false;
        bRet = ReadNext ();
        if (feature) {
            * feature = this;
            AddRef ();
        }

    } catch (GisException * gis) {
        PushGisException (eGwsFdoProviderError, gis);
        gis->Release ();
    }
    return bRet;
}

void CGwsFeatureIterator::DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc)
{
    m_prepquery->DescribeResults (ppResDesc);
}

GisInt32 CGwsFeatureIterator::GetCacheId ()
{
    if (m_bExposeFeatureIdAsCacheId) {
        GWSFeatureId id = GetFeatureId();
        if(id.GetCount() != 1)
            GWS_THROW(eGwsInvalidCacheId);

        GisPtr<FdoDataValue> pVal = id.GetItem(0);

        GisInt16 int16Val;
        GisInt32 int32Val;
        switch(pVal->GetDataType())
        {
            case FdoDataType_Int16:
                int16Val = (GisInt16) (* (FdoInt16Value *)  pVal.p);
                return (GisInt32)int16Val;

            case FdoDataType_Int32:
                int32Val = (GisInt32) (* (FdoInt32Value *)  pVal.p);
                return int32Val;
            default: break;
        }
    }
    return 0;   // not feature id in this case
}

GisInt32 CGwsFeatureIterator::GetRevisionNumber ()
{
    if (m_reader == NULL)
        return -1;

    const WSTR & revnumber =
            ((CGwsPreparedFeatureQuery *)m_prepquery)->m_revisionprop;

    if (revnumber.empty())
        return -1;

    return m_reader->GetInt32 (revnumber.c_str ());
}

GWSFeatureId CGwsFeatureIterator::GetFeatureId ()
{
    GisPtr<IGWSExtendedFeatureDescription> extFeatDsc;

    DescribeFeature (&extFeatDsc);

    CGwsQueryResultDescriptors *  fdesc = (CGwsQueryResultDescriptors *) extFeatDsc.p;
    GisPtr<CGwsDataValueCollection> keyvals;

    GisPtr<FdoDataPropertyDefinitionCollection> iddefs =
                                fdesc->GetIdentityProperties ();

    for (int i = 0; iddefs != NULL && i < iddefs->GetCount (); i ++) {
        GisPtr<FdoDataPropertyDefinition> idprop = iddefs->GetItem (i);

        GisPtr<FdoDataValue> val =
            GwsQueryUtils::GetDataPropertyValue (m_reader,
                                               idprop->GetDataType (),
                                               idprop->GetName ());
        if (val == NULL)
             continue;
        if (keyvals == NULL)
            keyvals = (CGwsDataValueCollection *) CGwsDataValueCollection::Create ();
        keyvals->Add (val);

    }

    return GWSFeatureId (keyvals);
}


bool CGwsFeatureIterator::IsNew ()
{
    return false;
}
bool CGwsFeatureIterator::IsModified ()
{
    return false;
}
bool CGwsFeatureIterator::IsDeleted ()
{
    return false;
}

EGwsLockType CGwsFeatureIterator::GetCacheLockType ()
{
    return eGwsLockNolock;
}

IGWSFeatureIterator* CGwsFeatureIterator::GetJoinedFeatures (int i)
{
    i; // For "unreferenced formal parameter" warning
    return NULL;
}


void CGwsFeatureIterator::Close ()
{
    if (m_reader == NULL)
        return;
    try {
        m_reader->Close ();
    } catch (GisException * gis) {
        PushGisException (eGwsFdoProviderError, gis);
        gis->Release ();
    }
    return ;
}


const CGwsPropertyDesc &
    CGwsFeatureIterator::GetPropertyDescriptor (int iProp)
{
    GisPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    return ((CGwsQueryResultDescriptors *) featDsc.p)->GetPropertyDescriptor (iProp);
}

const CGwsPropertyDesc &
    CGwsFeatureIterator::GetPropertyDescriptor (GisString * propname)
{
    GisPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    return ((CGwsQueryResultDescriptors *) featDsc.p)->GetPropertyDescriptor (propname);
}

GisString * CGwsFeatureIterator::GetString (GisString * propname)
{
    CheckReader ();
    return m_reader->GetString (propname);
}

bool CGwsFeatureIterator::GetBoolean(GisString* propname)
{
    CheckReader ();
    return m_reader->GetBoolean (propname);
}

GisByte CGwsFeatureIterator::GetByte(GisString* propname)
{
    CheckReader ();
    return m_reader->GetByte (propname);
}

GisDateTime CGwsFeatureIterator::GetDateTime(GisString* propname)
{
    CheckReader ();
    return m_reader->GetDateTime (propname);
}

double CGwsFeatureIterator::GetDouble(GisString* propname)
{
    CheckReader ();
    return m_reader->GetDouble (propname);
}

GisInt16 CGwsFeatureIterator::GetInt16(GisString* propname)
{
    CheckReader ();
    return m_reader->GetInt16 (propname);
}

GisInt32 CGwsFeatureIterator::GetInt32(GisString* propname)
{
    CheckReader ();
    return m_reader->GetInt32 (propname);
}

GisInt64 CGwsFeatureIterator::GetInt64(GisString* propname)
{
    CheckReader ();
    return m_reader->GetInt64 (propname);
}

float CGwsFeatureIterator::GetSingle(GisString* propname)
{
    CheckReader ();
    return m_reader->GetSingle (propname);
}

FdoLOBValue * CGwsFeatureIterator::GetLOB(GisString* propertyName)
{
    CheckReader ();
    return m_reader->GetLOB (propertyName);
}

GisIStreamReader* CGwsFeatureIterator::GetLOBStreamReader(const wchar_t* propertyName )
{
    CheckReader ();
    return m_reader->GetLOBStreamReader (propertyName);
}

bool CGwsFeatureIterator::IsNull(GisString* propertyName)
{
    CheckReader ();
    return m_reader->IsNull (propertyName);
}
FdoIRaster*  CGwsFeatureIterator::GetRaster(GisString* propertyName)
{
    CheckReader ();
    return m_reader->GetRaster (propertyName);
}

FdoDataValue * CGwsFeatureIterator::GetDataValue (GisString* propertyName)
{
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    if (desc.m_name.empty ())
        return NULL;
    return GetPropertyValue (desc);
}

FdoDataValueCollection * CGwsFeatureIterator::GetDataValues (
    GisStringCollection* propertyNames
)
{
    CGwsDataValueCollection * vals = NULL;
    for (int i = 0; i < propertyNames->GetCount (); i ++) {
        GisPtr<FdoDataValue> val = GetDataValue (propertyNames->GetString (i));
        assert (val != NULL);
        if (vals == NULL)
            vals = (CGwsDataValueCollection *) CGwsDataValueCollection::Create ();
        vals->Add (val);
    }
    return vals;
}


GisInt32 CGwsFeatureIterator::GetDepth()
{
    CheckReader ();
    return m_reader->GetDepth ();

}
const GisByte * CGwsFeatureIterator::GetGeometry(
    GisString * propertyName,
    GisInt32  * count
)
{
    CheckReader ();
    GisByte * gvalue = (GisByte *) m_reader->GetGeometry (propertyName, count);
    if (m_converter != NULL && ! m_bGeometryConverted) {
        EGwsStatus stat = m_converter->ConvertForward (gvalue, * count);
        if (IGWSException::IsError (stat))
            GWS_THROW (stat);
        m_bGeometryConverted = true;
    }
    return gvalue;

}
GisByteArray* CGwsFeatureIterator::GetGeometry(GisString* propertyName)
{
    CheckReader ();
    GisByteArray * gvalue = m_reader->GetGeometry (propertyName);
    if (m_converter != NULL && ! m_bGeometryConverted) {
        EGwsStatus stat = m_converter->ConvertForward (gvalue);
        if (IGWSException::IsError (stat))
            GWS_THROW (stat);
        m_bGeometryConverted = true;
    }
    return gvalue;
}
FdoIFeatureReader* CGwsFeatureIterator::GetFeatureObject(GisString* propertyName)
{
    CheckReader ();
    return m_reader->GetFeatureObject (propertyName);
}

/*
bool CGwsFeatureIterator::IsNull(GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->IsNull (desc.m_name.c_str ());
}
GisString * CGwsFeatureIterator::GetString (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetString (desc.m_name.c_str ());
}
bool CGwsFeatureIterator::GetBoolean  (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetBoolean (desc.m_name.c_str ());
}
GisByte CGwsFeatureIterator::GetByte     (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetByte (desc.m_name.c_str ());
}
GisDateTime CGwsFeatureIterator::GetDateTime (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetDateTime (desc.m_name.c_str ());
}
double CGwsFeatureIterator::GetDouble   (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetDouble (desc.m_name.c_str ());
}
GisInt16 CGwsFeatureIterator::GetInt16    (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetInt16 (desc.m_name.c_str ());
}
GisInt32 CGwsFeatureIterator::GetInt32    (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetInt32 (desc.m_name.c_str ());
}
GisInt64 CGwsFeatureIterator::GetInt64    (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetInt64 (desc.m_name.c_str ());
}
float CGwsFeatureIterator::GetSingle   (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetSingle (desc.m_name.c_str ());
}
FdoLOBValue* CGwsFeatureIterator::GetLOB (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetLOB (desc.m_name.c_str ());
}
GisIStreamReader* CGwsFeatureIterator::GetLOBStreamReader (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetLOBStreamReader (desc.m_name.c_str ());
}
FdoIRaster*     CGwsFeatureIterator::GetRaster   (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetRaster (desc.m_name.c_str ());
}
const GisByte * CGwsFeatureIterator::GetGeometry (GisInt32 iProp, GisInt32 * count)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetGeometry (desc.m_name.c_str (), count);
}
GisByteArray*   CGwsFeatureIterator::GetGeometry (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetGeometry (desc.m_name.c_str ());
}
FdoIFeatureReader* CGwsFeatureIterator::GetFeatureObject (GisInt32 iProp)
{
    CheckReader ();
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    return m_reader->GetFeatureObject (desc.m_name.c_str ());
}

*/

bool CGwsFeatureIterator::ReadNext()
{
    if (m_reader == NULL)
        return false;
    m_bMutableFeatureSet = false;
    m_bGeometryConverted = false;
    return m_reader->ReadNext ();
}

GisGeometryType CGwsFeatureIterator::GetGeometryType(GisByteArray* pArray)
{
    GisPtr<GisAgfGeometryFactory> pAgfFactory = GisAgfGeometryFactory::GetInstance();
    GisPtr<GisIGeometry> pAgfGeometry = pAgfFactory->CreateGeometryFromAgf(pArray);
    return pAgfGeometry->GetDerivedType();
}

GisString* CGwsFeatureIterator::GetPrimaryGeometryName()
{
    static std::wstring name;
    GisPtr<FdoClassDefinition> pClassDef(GetClassDefinition());
    GwsCommonFdoUtils::GetGeometryName(pClassDef, name);
    return name.c_str();
}

FdoClassDefinition* CGwsFeatureIterator::GetClassDefinition()
{
    CheckReader ();
    return m_reader->GetClassDefinition ();
}
/*
void CGwsFeatureIterator::ToString (
    GisInt32    iProp,
    wchar_t *   buff,
    int         len
)
{
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
    GwsQueryUtils::ToString (this, desc, buff, len);
}
*/

void CGwsFeatureIterator::ToString (
    GisString * propname,
    wchar_t *   buff,
    int         len
)
{
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propname);
    GwsQueryUtils::ToString (this, desc, buff, len);
}


FdoDataValue * CGwsFeatureIterator::GetPropertyValue (
    const CGwsPropertyDesc & desc
)
{
    CheckReader ();
    if (desc.m_ptype != FdoPropertyType_DataProperty)
        return NULL;

    return GwsQueryUtils::GetDataPropertyValue (m_reader, desc.m_dataprop, desc.m_name.c_str ());
}

IGWSMutableFeature * CGwsFeatureIterator::GetSimpleFeature ()
{
    if (m_mutableFeature == NULL)
        return NULL;

    if (! m_bMutableFeatureSet) {
        CGwsMutableFeature * feat = (CGwsMutableFeature *) m_mutableFeature.p;
        feat->SetPropertyValues (this);
        feat->SetFeatureId (GetFeatureId ());
        feat->SetRevisionNumber (GetRevisionNumber ());
        m_bMutableFeatureSet = true;

    }
    m_mutableFeature.p->AddRef ();
    return m_mutableFeature;
}

void CGwsFeatureIterator::CheckReader ()
{
  if (m_reader == NULL)
        GWS_THROW(eGwsClosedFeatureIterator);
}

unsigned char* CGwsFeatureIterator::ToBuffer(int& bufLen)
{
    if(m_pBinaryWriter != NULL)
        delete m_pBinaryWriter;

    m_pBinaryWriter = new GwsBinaryFeatureWriter();


    GisPtr<IGWSExtendedFeatureDescription> ifdesc;
    DescribeFeature (& ifdesc);
    wchar_t buf[256];
    int len = ifdesc->ClassName().ToFullyQualifedString(buf, 256);
    assert(len < 256);
    len;

    GisPtr<FdoClassDefinition> pClassDef = ifdesc->ClassDefinition();
    m_pBinaryWriter->WriteFeature(pClassDef, buf, NULL, m_reader);
    return m_pBinaryWriter->ToBuffer(bufLen);
}
