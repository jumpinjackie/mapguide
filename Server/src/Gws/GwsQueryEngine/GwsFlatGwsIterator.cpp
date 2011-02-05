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


CGwsFlatGwsIterator::CGwsFlatGwsIterator (IGWSFeatureIterator * pFrom)
{
    m_pFlatFdoReader = new CGwsFlatFdoReader (pFrom);
}


CGwsFlatGwsIterator::~CGwsFlatGwsIterator () throw()
{
    delete m_pFlatFdoReader;
}


FdoClassDefinition* CGwsFlatGwsIterator::GetClassDefinition()
{
    return m_pFlatFdoReader->GetClassDefinition ();
}


FdoInt32 CGwsFlatGwsIterator::GetDepth()
{
    return m_pFlatFdoReader->GetDepth();

}


const FdoByte * CGwsFlatGwsIterator::GetGeometry(FdoString* propertyName, FdoInt32 * count)
{
    return m_pFlatFdoReader->GetGeometry (propertyName, count);
}


FdoByteArray* CGwsFlatGwsIterator::GetGeometry(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetGeometry (propertyName);
}


FdoIFeatureReader*  CGwsFlatGwsIterator::GetFeatureObject(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetFeatureObject (propertyName);
}


bool CGwsFlatGwsIterator::GetBoolean(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetBoolean (propertyName);
}


FdoByte  CGwsFlatGwsIterator::GetByte(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetByte (propertyName);
}


FdoDateTime CGwsFlatGwsIterator::GetDateTime(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetDateTime (propertyName);
}


double CGwsFlatGwsIterator::GetDouble(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetDouble (propertyName);
}


FdoInt16 CGwsFlatGwsIterator::GetInt16(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetInt16 (propertyName);
}


FdoInt32 CGwsFlatGwsIterator::GetInt32(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetInt32 (propertyName);
}


FdoInt64 CGwsFlatGwsIterator::GetInt64(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetInt64 (propertyName);
}


float CGwsFlatGwsIterator::GetSingle(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetSingle (propertyName);
}


FdoString*  CGwsFlatGwsIterator::GetString(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetString (propertyName);
}


FdoLOBValue* CGwsFlatGwsIterator::GetLOB(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetLOB (propertyName);
}


FdoIStreamReader*   CGwsFlatGwsIterator::GetLOBStreamReader(const wchar_t* propertyName )
{
    return m_pFlatFdoReader->GetLOBStreamReader (propertyName);
}


bool  CGwsFlatGwsIterator::IsNull(FdoString* propertyName)
{
    return m_pFlatFdoReader->IsNull (propertyName);
}


FdoIRaster*  CGwsFlatGwsIterator::GetRaster(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetRaster (propertyName);
}


bool CGwsFlatGwsIterator::ReadNext()
{
    return m_pFlatFdoReader->ReadNext ();
}


void  CGwsFlatGwsIterator::Close()
{
    m_pFlatFdoReader->Close ();
}


bool CGwsFlatGwsIterator::NextFeature (IGWSFeature ** feature)
{
    if (feature != NULL) {
        if (ReadNext ()) {
            * feature = this;
            AddRef ();
            return true;
        }
    }
    return false;
}


IGWSFeatureIterator* CGwsFlatGwsIterator::GetJoinedFeatures (int i)
{
    return NULL;
}


void CGwsFlatGwsIterator::DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc)
{
    if (ppResDesc == NULL)
        GWS_THROW (eGwsNullPointer);
    if (m_fdsc == NULL) {

        FdoPtr<FdoClassDefinition> classDef = GetClassDefinition ();

        CGwsQueryResultDescriptors * resdsc =
                new CGwsQueryResultDescriptors (classDef, classDef->GetName (), NULL, NULL, true, NULL);
        resdsc->AddRef ();
        m_fdsc = resdsc;
    }
    * ppResDesc = m_fdsc;
    if (* ppResDesc)
        (* ppResDesc)->AddRef ();
}


FdoInt32 CGwsFlatGwsIterator::GetCacheId ()
{
    return m_pFlatFdoReader->GetCacheId ();
}


FdoInt32 CGwsFlatGwsIterator::ResolveJoinIdVector (std::vector<FdoInt32>& idVec)
{
    return m_pFlatFdoReader->GetCacheId();
}


std::vector<FdoInt32>* CGwsFlatGwsIterator::GetJoinIdVector(FdoInt32 jCacheId)
{
    return NULL;
}


bool CGwsFlatGwsIterator::ReadAtCacheId(FdoInt32 cacheId) {
    return false;
}


FdoString * CGwsFlatGwsIterator::GetLayerSource ()
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->GetLayerSource ();
}


bool CGwsFlatGwsIterator::IsNew ()
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->IsNew ();
}


bool CGwsFlatGwsIterator::IsModified ()
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->IsModified ();
}


bool CGwsFlatGwsIterator::IsDeleted ()
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->IsDeleted ();
}


EGwsLockType CGwsFlatGwsIterator::GetCacheLockType ()
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->GetCacheLockType();
}


FdoInt32 CGwsFlatGwsIterator::GetRevisionNumber ()
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->GetRevisionNumber ();
}


GWSFeatureId CGwsFlatGwsIterator::GetFeatureId ()
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->GetFeatureId ();
}


FdoGeometryType CGwsFlatGwsIterator::GetGeometryType(FdoByteArray* pArray)
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->GetGeometryType (pArray);
}


FdoString* CGwsFlatGwsIterator::GetPrimaryGeometryName()
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->GetPrimaryGeometryName ();
}


const GWSCoordinateSystem &  CGwsFlatGwsIterator::GetCSName ()
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->GetCSName ();
}


FdoDataValue *  CGwsFlatGwsIterator::GetDataValue (FdoString* propertyName)
{
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    if (desc.m_name.empty ())
        return NULL;
    return GetPropertyValue (desc);
}


FdoDataValueCollection *   CGwsFlatGwsIterator::GetDataValues (
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


void  CGwsFlatGwsIterator::ToString  (FdoString * propName, wchar_t * buff, int len)
{
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propName);
    GwsQueryUtils::ToString (this, desc, buff, len);
}


unsigned char*  CGwsFlatGwsIterator::ToBuffer    (int& bufLen)
{
    FdoPtr<IGWSFeatureIterator> fiter = m_pFlatFdoReader->GetOriginalIterator ();
    return fiter->ToBuffer (bufLen);    // only primary feature
}


FdoDataValue * CGwsFlatGwsIterator::GetPropertyValue (
    const CGwsPropertyDesc & desc
)
{
    if (desc.m_ptype != FdoPropertyType_DataProperty)
        return NULL;

    return GwsQueryUtils::GetDataPropertyValue (this->m_pFlatFdoReader, desc.m_dataprop, desc.m_name.c_str ());
}


const CGwsPropertyDesc & CGwsFlatGwsIterator::GetPropertyDescriptor (
    FdoString * propname
)
{
    FdoPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    return ((CGwsQueryResultDescriptors *) featDsc.p)->GetPropertyDescriptor (propname);
}


//IGWSFeatureIterator::SdfIScrollableFeatureReader implementation.
int
CGwsFlatGwsIterator::Count()
{
    return m_pFlatFdoReader->Count();
}


bool
CGwsFlatGwsIterator::ReadFirst()
{
    return m_pFlatFdoReader->ReadFirst ();
}


bool
CGwsFlatGwsIterator::ReadLast()
{
    return m_pFlatFdoReader->ReadLast ();
}


bool
CGwsFlatGwsIterator::ReadPrevious()
{
    return m_pFlatFdoReader->ReadPrevious ();
}


bool
CGwsFlatGwsIterator::ReadAt(FdoPropertyValueCollection* key)
{
    return m_pFlatFdoReader->ReadAt (key);
}


bool
CGwsFlatGwsIterator::ReadAtIndex( unsigned int recordindex )
{
    return m_pFlatFdoReader->ReadAtIndex(recordindex);
}


unsigned int
CGwsFlatGwsIterator::IndexOf(FdoPropertyValueCollection* key)
{
    return m_pFlatFdoReader->IndexOf (key);
}


bool
CGwsFlatGwsIterator::Scrollable()
{
    return m_pFlatFdoReader->Scrollable();
}


FdoByteArray*
CGwsFlatGwsIterator::GetOriginalGeometry(FdoString* propertyName)
{
    return m_pFlatFdoReader->GetOriginalGeometry(propertyName);
}
