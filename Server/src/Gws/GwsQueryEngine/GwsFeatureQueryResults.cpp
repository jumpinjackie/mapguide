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
#include "GwsBinaryFeatureWriter.h"
#include "SDF/IScrollableFeatureReader.h"
#include "SHP/IScrollableFeatureReader.h"


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
    m_cacheIdType = FdoDataType_String;
    m_bScrollableReader = false;
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
    CGwsPreparedFeatureQuery        * prepquery,
    bool                            bIsScrollable
)
{
    assert (pReader != NULL && fquery != NULL);
    m_reader = pReader;

    m_bScrollableReader = bIsScrollable;

    if(dynamic_cast<SdfIScrollableFeatureReader*>(pReader) != NULL)
        m_extProviderType = eSDF;
    else if(dynamic_cast<ShpIScrollableFeatureReader*>(pReader) != NULL)
        m_extProviderType = eSHP;
    else if(dynamic_cast<CGwsFeatureIterator*>(pReader) != NULL)
        m_extProviderType = eCGFI;
   else
        m_bScrollableReader = false;

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
    //deferred to GetSimpleFeature()
    //InitializeMutableFeature ();
    return eGwsOk;
}


const GWSCoordinateSystem & CGwsFeatureIterator::GetCSName ()
{
    static GWSCoordinateSystem csname;
    CGwsPreparedFeatureQuery * pfquery = m_prepquery->GetPrimaryQuery ();
    pfquery->GetCSConverter (& m_converter);
    if (m_converter != NULL &&
        m_converter->IsInitialized ())
        return m_converter->DestinationCS ();
    if (m_converter != NULL &&
        ! m_converter->SourceCS().IsEmpty ())
        return m_converter->SourceCS ();

    WSTR sc = pfquery->ActiveSpatialContext ();
    FdoPtr<FdoIConnection> conn = pfquery->GetFdoConnection ();
    GwsSpatialContextDescription scdesc;
    if (! IGWSException::IsError (GwsCommonFdoUtils::DescribeSC (conn, sc.c_str (), scdesc)))
        csname = GWSCoordinateSystem (scdesc.CsName ());
    return csname;
}


bool CGwsFeatureIterator::InitializeMutableFeature ()
{
    FdoPtr<IGWSExtendedFeatureDescription> ifdesc;
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
    if (m_bExposeFeatureIdAsCacheId) {
        // do some prepartions ...

         FdoPtr<IGWSExtendedFeatureDescription> extFeatDsc;

        DescribeFeature (&extFeatDsc);

        CGwsQueryResultDescriptors *  fdesc = (CGwsQueryResultDescriptors *) extFeatDsc.p;

        FdoPtr<FdoDataPropertyDefinitionCollection> iddefs =
                                fdesc->GetIdentityProperties ();
        if (fdesc->GetCount () > 1) {
            m_bExposeFeatureIdAsCacheId = false;
            return;
        }

        FdoPtr<FdoDataPropertyDefinition> idprop = iddefs->GetItem (0);
        m_cacheIdType = idprop->GetDataType ();
        m_idname = idprop->GetName ();
    }
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

    } catch (FdoException * fdoEx) {
        PushFdoException (eGwsFdoProviderError, fdoEx);
        fdoEx->Release ();
    }
    return bRet;
}


void CGwsFeatureIterator::DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc)
{
    m_prepquery->DescribeResults (ppResDesc);
}


FdoInt32 CGwsFeatureIterator::GetCacheId ()
{
    if (m_bExposeFeatureIdAsCacheId) {
        if (m_cacheIdType == FdoDataType_Int16)
            return (FdoInt32) m_reader->GetInt16 (m_idname.c_str ());
        else if (m_cacheIdType == FdoDataType_Int32)
            return m_reader->GetInt32 (m_idname.c_str ());
    }
    return 0;   // not feature id in this case
}


FdoInt32 CGwsFeatureIterator::GetRevisionNumber ()
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
    FdoPtr<IGWSExtendedFeatureDescription> extFeatDsc;

    DescribeFeature (&extFeatDsc);

    CGwsQueryResultDescriptors *  fdesc = (CGwsQueryResultDescriptors *) extFeatDsc.p;
    FdoPtr<CGwsDataValueCollection> keyvals;

    FdoPtr<FdoDataPropertyDefinitionCollection> iddefs =
                                fdesc->GetIdentityProperties ();

    for (int i = 0; iddefs != NULL && i < iddefs->GetCount (); i ++) {
        FdoPtr<FdoDataPropertyDefinition> idprop = iddefs->GetItem (i);

        FdoPtr<FdoDataValue> val =
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
    return NULL;
}


void CGwsFeatureIterator::Close ()
{
    if (m_reader == NULL)
        return;
    try {
        m_reader->Close ();
    } catch (FdoException * fdoEx) {
        PushFdoException (eGwsFdoProviderError, fdoEx);
        fdoEx->Release ();
    }
    return ;
}


const CGwsPropertyDesc &
    CGwsFeatureIterator::GetPropertyDescriptor (int iProp)
{
    FdoPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    return ((CGwsQueryResultDescriptors *) featDsc.p)->GetPropertyDescriptor (iProp);
}


const CGwsPropertyDesc &
    CGwsFeatureIterator::GetPropertyDescriptor (FdoString * propname)
{
    FdoPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    return ((CGwsQueryResultDescriptors *) featDsc.p)->GetPropertyDescriptor (propname);
}


FdoString * CGwsFeatureIterator::GetString (FdoString * propname)
{
    CheckReader ();
    return m_reader->GetString (propname);
}


bool CGwsFeatureIterator::GetBoolean(FdoString* propname)
{
    CheckReader ();
    return m_reader->GetBoolean (propname);
}


FdoByte CGwsFeatureIterator::GetByte(FdoString* propname)
{
    CheckReader ();
    return m_reader->GetByte (propname);
}


FdoDateTime CGwsFeatureIterator::GetDateTime(FdoString* propname)
{
    CheckReader ();
    return m_reader->GetDateTime (propname);
}


double CGwsFeatureIterator::GetDouble(FdoString* propname)
{
    CheckReader ();
    return m_reader->GetDouble (propname);
}


FdoInt16 CGwsFeatureIterator::GetInt16(FdoString* propname)
{
    CheckReader ();
    return m_reader->GetInt16 (propname);
}


FdoInt32 CGwsFeatureIterator::GetInt32(FdoString* propname)
{
    CheckReader ();
    return m_reader->GetInt32 (propname);
}


FdoInt64 CGwsFeatureIterator::GetInt64(FdoString* propname)
{
    CheckReader ();
    return m_reader->GetInt64 (propname);
}


float CGwsFeatureIterator::GetSingle(FdoString* propname)
{
    CheckReader ();
    return m_reader->GetSingle (propname);
}


FdoLOBValue * CGwsFeatureIterator::GetLOB(FdoString* propertyName)
{
    CheckReader ();
    return m_reader->GetLOB (propertyName);
}


FdoIStreamReader* CGwsFeatureIterator::GetLOBStreamReader(const wchar_t* propertyName )
{
    CheckReader ();
    return m_reader->GetLOBStreamReader (propertyName);
}


bool CGwsFeatureIterator::IsNull(FdoString* propertyName)
{
    CheckReader ();
    return m_reader->IsNull (propertyName);
}


FdoIRaster*  CGwsFeatureIterator::GetRaster(FdoString* propertyName)
{
    CheckReader ();
    return m_reader->GetRaster (propertyName);
}


FdoDataValue * CGwsFeatureIterator::GetDataValue (FdoString* propertyName)
{
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    if (desc.m_name.empty ())
        return NULL;
    return GetPropertyValue (desc);
}


FdoDataValueCollection * CGwsFeatureIterator::GetDataValues (
    FdoStringCollection* propertyNames
)
{
    CGwsDataValueCollection * vals = NULL;
    for (int i = 0; i < propertyNames->GetCount (); i ++) {
        FdoPtr<FdoDataValue> val = GetDataValue (propertyNames->GetString (i));
        assert (val != NULL);
        if (vals == NULL)
            vals = (CGwsDataValueCollection *) CGwsDataValueCollection::Create ();
        vals->Add (val);
    }
    return vals;
}


FdoInt32 CGwsFeatureIterator::GetDepth()
{
    CheckReader ();
    return m_reader->GetDepth ();
}


const FdoByte * CGwsFeatureIterator::GetGeometry(
    FdoString * propertyName,
    FdoInt32  * count
)
{
    CheckReader ();
    FdoByte * gvalue = (FdoByte *) m_reader->GetGeometry (propertyName, count);
    if (m_converter != NULL && ! m_bGeometryConverted && gvalue) {
        EGwsStatus stat = m_converter->ConvertForward (gvalue, * count);
        if (IGWSException::IsError (stat))
            GWS_THROW (stat);
        m_bGeometryConverted = true;
    }
    return gvalue;
}


FdoByteArray* CGwsFeatureIterator::GetGeometry(FdoString* propertyName)
{
    CheckReader ();
    FdoByteArray * gvalue = m_reader->GetGeometry (propertyName);
    if (m_converter != NULL && ! m_bGeometryConverted && gvalue) {
        EGwsStatus stat = m_converter->ConvertForward (gvalue);
        if (IGWSException::IsError (stat))
            GWS_THROW (stat);
        m_bGeometryConverted = true;
    }
    return gvalue;
}


FdoByteArray * CGwsFeatureIterator::GetOriginalGeometry (FdoString* propertyName)
{
    CheckReader ();
    return m_reader->GetGeometry (propertyName);
}


bool CGwsFeatureIterator::ConvertingGeometry ()
{
    if (m_converter != NULL && m_converter->IsInitialized ()) {
        return true;
    }
    return false;
}


FdoIFeatureReader* CGwsFeatureIterator::GetFeatureObject(FdoString* propertyName)
{
    CheckReader ();
    return m_reader->GetFeatureObject (propertyName);
}


bool CGwsFeatureIterator::ReadNext()
{
    if (m_reader == NULL)
        return false;
    m_bMutableFeatureSet = false;
    m_bGeometryConverted = false;
    return m_reader->ReadNext ();
}


FdoGeometryType CGwsFeatureIterator::GetGeometryType(FdoByteArray* pArray)
{
    FdoPtr<FdoFgfGeometryFactory> pFgfFactory = FdoFgfGeometryFactory::GetInstance();
    FdoPtr<FdoIGeometry> pFgfGeometry = pFgfFactory->CreateGeometryFromFgf(pArray);
    return pFgfGeometry->GetDerivedType();
}


FdoString* CGwsFeatureIterator::GetPrimaryGeometryName()
{
    static std::wstring name;
    FdoPtr<FdoClassDefinition> pClassDef(GetClassDefinition());
    GwsCommonFdoUtils::GetGeometryName(pClassDef, name);
    return name.c_str();
}


FdoClassDefinition* CGwsFeatureIterator::GetClassDefinition()
{
    CheckReader ();
    return m_reader->GetClassDefinition ();
}


void CGwsFeatureIterator::ToString (
    FdoString * propname,
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
        InitializeMutableFeature();

    assert(m_mutableFeature != NULL);
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


//this method throws an exception if the reader is NULL;
void CGwsFeatureIterator::CheckReader ()
{
    assert(m_reader != NULL);
    if(m_reader == NULL)
        GWS_THROW(eGwsClosedFeatureIterator);
}


unsigned char* CGwsFeatureIterator::ToBuffer(int& bufLen)
{
    if(m_pBinaryWriter != NULL)
        delete m_pBinaryWriter;

    m_pBinaryWriter = new GwsBinaryFeatureWriter();

    FdoPtr<IGWSExtendedFeatureDescription> ifdesc;
    DescribeFeature (& ifdesc);
    wchar_t buf[256];
    int len = ifdesc->ClassName().ToFullyQualifedString(buf, 256);
    assert(len < 256);

    FdoPtr<FdoClassDefinition> pClassDef = ifdesc->ClassDefinition();
    m_pBinaryWriter->WriteFeature(pClassDef, buf, NULL, m_reader);
    return m_pBinaryWriter->ToBuffer(bufLen);
}


//SdfIScrollableFeatureReader implementation.
int
CGwsFeatureIterator::Count()
{
    CheckReader();
    if(!m_bScrollableReader) GWS_THROW(eGwsNotSupported);

    if(dynamic_cast<SdfIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((SdfIScrollableFeatureReader*)m_reader.p)->Count();
    if(dynamic_cast<ShpIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((ShpIScrollableFeatureReader*)m_reader.p)->Count();
    if(dynamic_cast<CGwsFeatureIterator*>(m_reader.p) != NULL)
        return ((CGwsFeatureIterator*)m_reader.p)->Count();
    assert(false);
    return 0;
}


bool
CGwsFeatureIterator::ReadFirst()
{
    CheckReader();
    if(!m_bScrollableReader) GWS_THROW(eGwsNotSupported);

    m_bMutableFeatureSet = false;
    m_bGeometryConverted = false;

    if(dynamic_cast<SdfIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((SdfIScrollableFeatureReader*)m_reader.p)->ReadFirst();
    if(dynamic_cast<ShpIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((ShpIScrollableFeatureReader*)m_reader.p)->ReadFirst();
    if(dynamic_cast<CGwsFeatureIterator*>(m_reader.p) != NULL)
        return ((CGwsFeatureIterator*)m_reader.p)->ReadFirst();
    assert(false);
    return false;
}


bool
CGwsFeatureIterator::ReadLast()
{
    CheckReader();
    if(!m_bScrollableReader) GWS_THROW(eGwsNotSupported);

    m_bMutableFeatureSet = false;
    m_bGeometryConverted = false;

    if(dynamic_cast<SdfIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((SdfIScrollableFeatureReader*)m_reader.p)->ReadLast();
    if(dynamic_cast<ShpIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((ShpIScrollableFeatureReader*)m_reader.p)->ReadLast();
    if(dynamic_cast<CGwsFeatureIterator*>(m_reader.p) != NULL)
        return ((CGwsFeatureIterator*)m_reader.p)->ReadLast();
    assert(false);
    return false;
}


bool
CGwsFeatureIterator::ReadPrevious()
{
    CheckReader();
    if(!m_bScrollableReader) GWS_THROW(eGwsNotSupported);

    m_bMutableFeatureSet = false;
    m_bGeometryConverted = false;

    if(dynamic_cast<SdfIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((SdfIScrollableFeatureReader*)m_reader.p)->ReadPrevious();
    if(dynamic_cast<ShpIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((ShpIScrollableFeatureReader*)m_reader.p)->ReadPrevious();
    if(dynamic_cast<CGwsFeatureIterator*>(m_reader.p) != NULL)
        return ((CGwsFeatureIterator*)m_reader.p)->ReadPrevious();
    assert(false);
    return false;
}


bool
CGwsFeatureIterator::ReadAt(FdoPropertyValueCollection* key)
{
    CheckReader();
    if(!m_bScrollableReader) GWS_THROW(eGwsNotSupported);

    m_bMutableFeatureSet = false;
    m_bGeometryConverted = false;

    if(dynamic_cast<SdfIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((SdfIScrollableFeatureReader*)m_reader.p)->ReadAt(key);
    if(dynamic_cast<ShpIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((ShpIScrollableFeatureReader*)m_reader.p)->ReadAt(key);
    if(dynamic_cast<CGwsFeatureIterator*>(m_reader.p) != NULL)
        return ((CGwsFeatureIterator*)m_reader.p)->ReadAt(key);
    assert(false);
    return false;
}


bool
CGwsFeatureIterator::ReadAtIndex( unsigned int recordindex )
{
    CheckReader();
    if(!m_bScrollableReader) GWS_THROW(eGwsNotSupported);

    m_bMutableFeatureSet = false;
    m_bGeometryConverted = false;

    if(dynamic_cast<SdfIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((SdfIScrollableFeatureReader*)m_reader.p)->ReadAtIndex(recordindex);
    if(dynamic_cast<ShpIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((ShpIScrollableFeatureReader*)m_reader.p)->ReadAtIndex(recordindex);
    if(dynamic_cast<CGwsFeatureIterator*>(m_reader.p) != NULL)
        return ((CGwsFeatureIterator*)m_reader.p)->ReadAtIndex(recordindex);
    assert(false);
    return false;
}


unsigned int
CGwsFeatureIterator::IndexOf(FdoPropertyValueCollection* key)
{
    CheckReader();
    if(!m_bScrollableReader) GWS_THROW(eGwsNotSupported);

    if(dynamic_cast<SdfIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((SdfIScrollableFeatureReader*)m_reader.p)->IndexOf(key);
    if(dynamic_cast<ShpIScrollableFeatureReader*>(m_reader.p) != NULL)
        return ((ShpIScrollableFeatureReader*)m_reader.p)->IndexOf(key);
    if(dynamic_cast<CGwsFeatureIterator*>(m_reader.p) != NULL)
        return ((CGwsFeatureIterator*)m_reader.p)->IndexOf(key);
    assert(false);
    return 0;
}


bool
CGwsFeatureIterator::Scrollable()
{
    CheckReader();
    if(NULL == m_reader || !m_bScrollableReader)
        return false;
    if( (NULL == dynamic_cast<SdfIScrollableFeatureReader*>(m_reader.p)) &&
        (NULL == dynamic_cast<ShpIScrollableFeatureReader*>(m_reader.p)) &&
        (NULL == dynamic_cast<CGwsFeatureIterator*>(m_reader.p)))
        return false;

    return true;
}


IGWSFeatureIterator*
CGwsFeatureIterator::GetPrimaryIterator()
{
    AddRef();
    return this;
}
