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

#include "stdafx.h"
#include "GwsQueryEngineImp.h"

CGwsMultiSelectIterator::CGwsMultiSelectIterator ()
    : CGwsFeatureIterator()
    , m_queryIdx(0)
{
}

CGwsMultiSelectIterator::~CGwsMultiSelectIterator() throw()
{
}

EGwsStatus CGwsMultiSelectIterator::InitializeReader (
    FdoIFeatureReader               * pReader,
    IGWSQuery                       * fquery,
    CGwsPreparedFeatureQuery        * prepquery,
    bool                            bScrollable
)
{
    assert(false);
    return eGwsNotSupported;
}

EGwsStatus CGwsMultiSelectIterator::InitializeReader (
                                    IGWSQuery                       * fquery,
                                    FdoFilter                       * pLayerFilter,
                                    const std::vector<FdoPtr<FdoFilter> > & filterSet)
{
    assert(fquery);
    m_query = fquery;
    FDO_SAFE_ADDREF(fquery);
    m_layerFilter = pLayerFilter;
    FDO_SAFE_ADDREF(pLayerFilter);
    m_filterSet = filterSet;
    m_queryIdx = 0;
    FdoPtr<IGWSObject> pOwner = fquery->GetOwner();
    SetOwner(pOwner);

    // Execute the first query
    //
    return Execute();
}

EGwsStatus CGwsMultiSelectIterator::Execute ()
{
    try
    {
        if (m_queryIdx >= m_filterSet.size())
        {
            // No more queries
            //
            return eGwsFailedToExecuteQuery;
        }

        // Filter
        //
        FdoPtr<FdoFilter> pFilter = m_filterSet[m_queryIdx];

        // Combine with layer filter, if any
        //
        if (m_layerFilter)
        {
            pFilter = GwsCommonFdoUtils::CombineFilters (m_layerFilter, pFilter, FdoBinaryLogicalOperations_And);
        }

        // Execute
        //
        m_queryIdx++;
        m_query->SetFilter (pFilter);
        FdoPtr<IGWSFeatureIterator> pResult;
        m_query->Execute(&pResult);

        // Currently all iterators derive from CGwsFeatureIterator
        //
        CGwsFeatureIterator *pIter = dynamic_cast<CGwsFeatureIterator*>(pResult.p);
        assert(pIter);
        if (!pIter)
        {
            return eGwsFailedToExecuteQuery;
        }

        // Store new iterator
        //
        m_iter = pIter;
        pIter->AddRef();

            // Propagate base class members
            //
            if (!m_strLayerSource.empty())
            {
            m_iter->SetLayerSource(m_strLayerSource.c_str());
            }
        if (m_bExposeFeatureIdAsCacheId != m_iter->ExposeFeatureIdAsCacheId())
            {
            m_iter->SetExposeFeatureIdAsCacheId (m_bExposeFeatureIdAsCacheId);
            }

        return eGwsOk;
    }
    catch (FdoException *e)
    {
        GWS_RETHROW (e, eGwsFailedToExecuteQuery);
    }
}

bool CGwsMultiSelectIterator::ReadNext()
{
    CheckIterator ();
    if (m_iter->ReadNext())
    {
        // Something remains in the current reader
        //
        return true;
    }

    // Execute next select
    //
    if (Execute() != eGwsOk)
    {
        return false;
    }

    // Fetch to the first record
    //
    return m_iter->ReadNext();
}

bool CGwsMultiSelectIterator::NextFeature (IGWSFeature ** feature)
{
    CheckIterator ();
    if (m_iter->NextFeature(feature))
    {
        // Something remains in the current reader
        //
        return true;
    }

    // Execute next select
    //
    if (Execute() != eGwsOk)
    {
        return false;
    }

    // Fetch to the first record
    //
    return m_iter->NextFeature(feature);
}

void CGwsMultiSelectIterator::SetExposeFeatureIdAsCacheId (bool bFlag)
{
    m_bExposeFeatureIdAsCacheId = bFlag;
    if (m_iter && m_iter->ExposeFeatureIdAsCacheId() != bFlag)
    {
        m_iter->SetExposeFeatureIdAsCacheId (bFlag);
    }
}

void CGwsMultiSelectIterator::SetLayerSource (FdoString* strLayerSource)
{
    m_strLayerSource = strLayerSource;
    if (m_iter)
    {
        m_iter->SetLayerSource (strLayerSource);
    }
}

const GWSCoordinateSystem & CGwsMultiSelectIterator::GetCSName ()
{
    CheckIterator ();
    return m_iter->GetCSName ();
}


bool CGwsMultiSelectIterator::InitializeMutableFeature ()
{
    CheckIterator ();
    return m_iter->InitializeMutableFeature ();
}

void CGwsMultiSelectIterator::DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc)
{
    CheckIterator ();
    m_iter->DescribeFeature(ppResDesc);
}

FdoInt32 CGwsMultiSelectIterator::GetCacheId ()
{
    CheckIterator ();
    return m_iter->GetCacheId ();
}

FdoInt32 CGwsMultiSelectIterator::GetRevisionNumber ()
{
    CheckIterator ();
    return m_iter->GetRevisionNumber ();
}

GWSFeatureId CGwsMultiSelectIterator::GetFeatureId ()
{
    CheckIterator ();
    return m_iter->GetFeatureId ();
}


bool CGwsMultiSelectIterator::IsNew ()
{
    CheckIterator ();
    return m_iter->IsNew ();
}
bool CGwsMultiSelectIterator::IsModified ()
{
    CheckIterator ();
    return m_iter->IsModified ();
}
bool CGwsMultiSelectIterator::IsDeleted ()
{
    CheckIterator ();
    return m_iter->IsDeleted ();
}

EGwsLockType CGwsMultiSelectIterator::GetCacheLockType ()
{
    CheckIterator ();
    return m_iter->GetCacheLockType ();
}

IGWSFeatureIterator* CGwsMultiSelectIterator::GetJoinedFeatures (int i)
{
    CheckIterator ();
    return m_iter->GetJoinedFeatures (i);
}


void CGwsMultiSelectIterator::Close ()
{
    if (m_iter)
    {
        m_iter->Close();
    }
}


const CGwsPropertyDesc &
    CGwsMultiSelectIterator::GetPropertyDescriptor (int iProp)
{
    CheckIterator ();
    return m_iter->GetPropertyDescriptor (iProp);
}

const CGwsPropertyDesc &
    CGwsMultiSelectIterator::GetPropertyDescriptor (FdoString * propname)
{
    CheckIterator ();
    return m_iter->GetPropertyDescriptor (propname);
}

FdoString * CGwsMultiSelectIterator::GetString (FdoString * propname)
{
    CheckIterator ();
    return m_iter->GetString (propname);
}

bool CGwsMultiSelectIterator::GetBoolean(FdoString* propname)
{
    CheckIterator ();
    return m_iter->GetBoolean(propname);
}

FdoByte CGwsMultiSelectIterator::GetByte(FdoString* propname)
{
    CheckIterator ();
    return m_iter->GetByte(propname);
}

FdoDateTime CGwsMultiSelectIterator::GetDateTime(FdoString* propname)
{
    CheckIterator ();
    return m_iter->GetDateTime(propname);
}

double CGwsMultiSelectIterator::GetDouble(FdoString* propname)
{
    CheckIterator ();
    return m_iter->GetDouble(propname);
}

FdoInt16 CGwsMultiSelectIterator::GetInt16(FdoString* propname)
{
    CheckIterator ();
    return m_iter->GetInt16(propname);
}

FdoInt32 CGwsMultiSelectIterator::GetInt32(FdoString* propname)
{
    CheckIterator ();
    return m_iter->GetInt32(propname);
}

FdoInt64 CGwsMultiSelectIterator::GetInt64(FdoString* propname)
{
    CheckIterator ();
    return m_iter->GetInt64(propname);
}

float CGwsMultiSelectIterator::GetSingle(FdoString* propname)
{
    CheckIterator ();
    return m_iter->GetSingle(propname);
}


FdoLOBValue * CGwsMultiSelectIterator::GetLOB(FdoString* propertyName)
{
    CheckIterator ();
    return m_iter->GetLOB(propertyName);
}

FdoIStreamReader* CGwsMultiSelectIterator::GetLOBStreamReader(const wchar_t* propertyName )
{
    CheckIterator ();
    return m_iter->GetLOBStreamReader(propertyName);
}

bool CGwsMultiSelectIterator::IsNull(FdoString* propertyName)
{
    CheckIterator ();
    return m_iter->IsNull(propertyName);
}

FdoIRaster*  CGwsMultiSelectIterator::GetRaster(FdoString* propertyName)
{
    CheckIterator ();
    return m_iter->GetRaster(propertyName);
}

FdoDataValue * CGwsMultiSelectIterator::GetDataValue (FdoString* propertyName)
{
    CheckIterator ();
    return m_iter->GetDataValue (propertyName);
}

FdoDataValueCollection * CGwsMultiSelectIterator::GetDataValues (
    FdoStringCollection* propertyNames
)
{
    CheckIterator ();
    return m_iter->GetDataValues (propertyNames);
}


FdoInt32 CGwsMultiSelectIterator::GetDepth()
{
    CheckIterator ();
    return m_iter->GetDepth();

}

const FdoByte * CGwsMultiSelectIterator::GetGeometry(
    FdoString * propertyName,
    FdoInt32  * count
)
{
    CheckIterator ();
    return m_iter->GetGeometry(propertyName, count);
}

FdoByteArray* CGwsMultiSelectIterator::GetGeometry(FdoString* propertyName)
{
    CheckIterator ();
    return m_iter->GetGeometry(propertyName);
}

FdoByteArray * CGwsMultiSelectIterator::GetOriginalGeometry (FdoString* propertyName)
{
    CheckIterator ();
    return m_iter->GetOriginalGeometry (propertyName);
}

bool CGwsMultiSelectIterator::ConvertingGeometry ()
{
    CheckIterator ();
    return m_iter->ConvertingGeometry ();
}

FdoIFeatureReader* CGwsMultiSelectIterator::GetFeatureObject(FdoString* propertyName)
{
    CheckIterator ();
    return m_iter->GetFeatureObject(propertyName);
}

FdoGeometryType CGwsMultiSelectIterator::GetGeometryType(FdoByteArray* pArray)
{
    CheckIterator ();
    return m_iter->GetGeometryType(pArray);
}

FdoString* CGwsMultiSelectIterator::GetPrimaryGeometryName()
{
    CheckIterator ();
    return m_iter->GetPrimaryGeometryName();
}

FdoClassDefinition* CGwsMultiSelectIterator::GetClassDefinition()
{
    CheckIterator ();
    return m_iter->GetClassDefinition();
}

void CGwsMultiSelectIterator::ToString (
    FdoString * propname,
    wchar_t *   buff,
    int         len
)
{
    CheckIterator ();
    return m_iter->ToString (propname, buff, len);
}


FdoDataValue * CGwsMultiSelectIterator::GetPropertyValue (
    const CGwsPropertyDesc & desc
)
{
    CheckIterator ();
    return m_iter->GetPropertyValue (desc);
}

IGWSMutableFeature * CGwsMultiSelectIterator::GetSimpleFeature ()
{
    CheckIterator ();
    return m_iter->GetSimpleFeature ();
}

unsigned char* CGwsMultiSelectIterator::ToBuffer(int& bufLen)
{
    CheckIterator ();
    return m_iter->ToBuffer(bufLen);
}

void CGwsMultiSelectIterator::CheckIterator ()
{
    assert(m_iter);
    if (!m_iter)
    {
        GWS_THROW(eGwsClosedFeatureIterator);
    }
}

//IGWSFeatureIterator::SdfIScrollableFeatureReader implementation.
int
CGwsMultiSelectIterator::Count()
{
    CheckIterator ();

    assert(m_iter->Scrollable());
    if(!m_iter->Scrollable())   GWS_THROW(eGwsNotSupported);

    return m_iter->Count ();
}

bool
CGwsMultiSelectIterator::ReadFirst()
{
    CheckIterator ();

    assert(m_iter->Scrollable());
    if(!m_iter->Scrollable())   GWS_THROW(eGwsNotSupported);

    return m_iter->ReadFirst ();
}
bool
CGwsMultiSelectIterator::ReadLast()
{
    CheckIterator ();

    assert(m_iter->Scrollable());
    if(!m_iter->Scrollable())   GWS_THROW(eGwsNotSupported);

    return m_iter->ReadLast ();
}

bool
CGwsMultiSelectIterator::ReadPrevious()
{
    CheckIterator ();

    assert(m_iter->Scrollable());
    if(!m_iter->Scrollable())   GWS_THROW(eGwsNotSupported);

    return m_iter->ReadPrevious ();
}

bool
CGwsMultiSelectIterator::ReadAt(FdoPropertyValueCollection* key)
{
    CheckIterator ();

    assert(m_iter->Scrollable());
    if(!m_iter->Scrollable())   GWS_THROW(eGwsNotSupported);

    return m_iter->ReadAt (key);
}

bool
CGwsMultiSelectIterator::ReadAtIndex( unsigned int recordindex )
{
    CheckIterator ();

    assert(m_iter->Scrollable());
    if(!m_iter->Scrollable())   GWS_THROW(eGwsNotSupported);

    return m_iter->ReadAtIndex (recordindex);
}


unsigned int
CGwsMultiSelectIterator::IndexOf(FdoPropertyValueCollection* key)
{
    CheckIterator ();

    assert(m_iter->Scrollable());
    if(!m_iter->Scrollable())   GWS_THROW(eGwsNotSupported);

    return m_iter->IndexOf (key);
}

bool
CGwsMultiSelectIterator::Scrollable()
{
    CheckIterator ();
    return m_iter->Scrollable ();
}

IGWSFeatureIterator*
CGwsMultiSelectIterator::GetPrimaryIterator()
{
    CheckIterator ();
    return m_iter->GetPrimaryIterator();
}


