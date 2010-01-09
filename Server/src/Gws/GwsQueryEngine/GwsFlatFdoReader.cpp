//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include <algorithm>
#include "GwsQueryEngineImp.h"

#ifndef _WIN32
extern long GetTickCount();
#endif


/////////////////////////////////////////////////////////////////////
//
// class CGwsTimer
//
/////////////////////////////////////////////////////////////////////

class CGwsTimer {
    long           m_start, m_stop;
    std::wstring    m_label;
public:
    CGwsTimer(const wchar_t* label) {
        if(label)
            m_label = label;
        m_start = GetTickCount();
    }
    ~CGwsTimer() {
        wchar_t buffer[256];
        m_stop = GetTickCount();
        long duration = m_stop-m_start;
        swprintf(buffer, 256, /*MSG0*/L"%ls took %d ms.\n",m_label.empty() ? /*MSG0*/L"" : m_label.c_str(), duration);
        //::OutputDebugStringW(buffer);
        #ifdef _DEBUG
        wprintf(L"%ls", buffer);
        #endif
    }
};

#ifdef _DEBUG
#define GWSTIMER(label) CGwsTimer gwstimer(label)
#else
#define GWSTIMER(label)
#endif


/////////////////////////////////////////////////////////////////////
//
// Defines
//
/////////////////////////////////////////////////////////////////////
#define CHECK_GWS_ITERATOR if (m_gwsReader == NULL) GWS_THROW (eGwsFailed)


/////////////////////////////////////////////////////////////////////
//
// class CGwsFlatFdoReader
//
/////////////////////////////////////////////////////////////////////

CGwsFlatFdoReader::CGwsFlatFdoReader (IGWSFeatureIterator * pIterator)
{
    CGwsFeatureIterator* pCGFI = dynamic_cast<CGwsFeatureIterator*>(pIterator);
    if (! pCGFI)
        return;
    m_gwsReader = pCGFI;
    pCGFI->AddRef ();

    FdoPtr<IGWSExtendedFeatureDescription> fdsc;
    pIterator->DescribeFeature (& fdsc);

    m_bJoin = (fdsc->GetCount() > 0);
    m_forceOneToOne = fdsc->ForceOneToOneJoin();

    m_names = FdoStringCollection::Create ();
    std::vector<int>        iPath;
    int                     nItem = 0;

    CollectPropertyNames (m_names, m_PropDefs, fdsc, NULL, nItem, iPath, &m_readerPaths, &m_readerKeys);
    SortResults();

    m_curIndex = -1;
    m_tableSize = -1;
    m_bReadersLoaded = false;
}


CGwsFlatFdoReader::~CGwsFlatFdoReader ()
{
    ReleaseJoinReaders ();
}


void CGwsFlatFdoReader::Dispose()
{
    delete this;
}


IGWSFeatureIterator * CGwsFlatFdoReader::GetOriginalIterator ()
{
    CHECK_GWS_ITERATOR;
    m_gwsReader.p->AddRef ();
    return m_gwsReader;
}


FdoClassDefinition* CGwsFlatFdoReader::GetClassDefinition()
{
    CHECK_GWS_ITERATOR;

    if (! m_bJoin)
        return m_gwsReader->GetClassDefinition ();

    if (m_classDef == NULL) {
        FdoPtr<IGWSExtendedFeatureDescription> fdsc;
        std::wstring                           wstr;

        m_gwsReader->DescribeFeature (& fdsc);
        // use the first one as a class name. But who cares
        wstr = fdsc->ClassName ().Name ();
        wstr.append (/*MSG0*/L"_FlattenJoin");

        FdoFeatureClass * clsDef = FdoFeatureClass::Create (wstr.c_str(), /*MSG0*/L"Flatten_Join");
        FdoPtr<FdoPropertyDefinitionCollection> pdefs = clsDef->GetProperties ();
        for (int i = 0; i < m_names->GetCount (); i ++) {
            PropertyDefItem * pdefi = GetPropertyDefItem (m_names->GetString (i));
            if (pdefi != NULL) {
                FdoPtr<FdoPropertyDefinition> pdef = pdefi->GetFlatPropertyDefinition ();
                pdefs->Add (pdef);
            }
        }
        m_classDef = clsDef;
    }
    assert (m_classDef);
    m_classDef.p->AddRef ();
    return m_classDef;
}


FdoInt32 CGwsFlatFdoReader::GetDepth()
{
    CHECK_GWS_ITERATOR;
    return 0;
}

FdoString* CGwsFlatFdoReader::GetPropertyName(FdoInt32 index)
{
    assert(index >= 0 && index < m_names->GetCount());
    if (index < m_names->GetCount() && index >= 0)
        return m_names->GetString(index);

    return L""; // throw exception? 
}

FdoInt32 CGwsFlatFdoReader::GetPropertyIndex(FdoString* propertyName)
{
    return m_names->IndexOf(propertyName, false);
}

const FdoByte * CGwsFlatFdoReader::GetGeometry(FdoString* propertyName, FdoInt32 * count)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    if (name.empty () || name.length () == 0) {
        GWS_THROW (eGwsFailedToGetProperty);
    }

    return freader->GetGeometry (name.c_str (), count);
}

FdoByteArray* CGwsFlatFdoReader::GetGeometry(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetGeometry (name.c_str ());
}


FdoIFeatureReader*  CGwsFlatFdoReader::GetFeatureObject(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetFeatureObject (name.c_str ());
}


bool CGwsFlatFdoReader::GetBoolean(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);

    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetBoolean (name.c_str ());
}


FdoByte CGwsFlatFdoReader::GetByte(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetByte (name.c_str ());
}


FdoDateTime CGwsFlatFdoReader::GetDateTime(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);

    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetDateTime (name.c_str ());
}


double CGwsFlatFdoReader::GetDouble(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetDouble (name.c_str ());
}


FdoInt16 CGwsFlatFdoReader::GetInt16(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetInt16 (name.c_str ());
}


FdoInt32 CGwsFlatFdoReader::GetInt32(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetInt32 (name.c_str ());
}


FdoInt64 CGwsFlatFdoReader::GetInt64(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetInt64 (name.c_str ());
}


float CGwsFlatFdoReader::GetSingle(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);

    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetSingle (name.c_str ());
}


FdoString* CGwsFlatFdoReader::GetString(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetString (name.c_str ());
}


FdoLOBValue* CGwsFlatFdoReader::GetLOB(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);

    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetLOB (name.c_str ());
}


FdoIStreamReader* CGwsFlatFdoReader::GetLOBStreamReader(const wchar_t* propertyName )
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);

    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetLOBStreamReader (name.c_str ());
}


bool CGwsFlatFdoReader::IsNull(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)
        return true;
    return freader->IsNull (name.c_str ());
}


FdoIRaster* CGwsFlatFdoReader::GetRaster(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;

    WSTR                      name;
    FdoPtr<IGWSFeatureIterator> freader = GetReaderForProperty (propertyName, name);
    if (freader == NULL)    // treated as null
        GWS_THROW (eGwsFailedToGetProperty);

    return freader->GetRaster (name.c_str ());
}

const FdoByte * CGwsFlatFdoReader::GetGeometry(FdoInt32 index, FdoInt32 * count)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetGeometry((FdoString*)propertyName, count);
}

FdoByteArray* CGwsFlatFdoReader::GetGeometry(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetGeometry((FdoString*)propertyName);
}

FdoIFeatureReader*  CGwsFlatFdoReader::GetFeatureObject(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetFeatureObject((FdoString*)propertyName);
}


bool CGwsFlatFdoReader::GetBoolean(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetBoolean((FdoString*)propertyName);
}


FdoByte CGwsFlatFdoReader::GetByte(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetByte((FdoString*)propertyName);
}


FdoDateTime CGwsFlatFdoReader::GetDateTime(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetDateTime((FdoString*)propertyName);
}


double CGwsFlatFdoReader::GetDouble(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetDouble((FdoString*)propertyName);
}


FdoInt16 CGwsFlatFdoReader::GetInt16(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetInt16((FdoString*)propertyName);
}


FdoInt32 CGwsFlatFdoReader::GetInt32(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetInt32((FdoString*)propertyName);
}


FdoInt64 CGwsFlatFdoReader::GetInt64(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetInt64((FdoString*)propertyName);
}


float CGwsFlatFdoReader::GetSingle(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetSingle((FdoString*)propertyName);
}


FdoString* CGwsFlatFdoReader::GetString(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetString((FdoString*)propertyName);
}

FdoLOBValue* CGwsFlatFdoReader::GetLOB(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetLOB((FdoString*)propertyName);
}


FdoIStreamReader* CGwsFlatFdoReader::GetLOBStreamReader(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetLOBStreamReader((FdoString*)propertyName);
}

bool CGwsFlatFdoReader::IsNull(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return IsNull((FdoString*)propertyName);
}

FdoIRaster* CGwsFlatFdoReader::GetRaster(FdoInt32 index)
{
    FdoStringP propertyName = GetPropertyName(index);
    return GetRaster((FdoString*)propertyName);
}

bool CGwsFlatFdoReader::ReadNext()
{
    CHECK_GWS_ITERATOR;
    if(!m_forceOneToOne) {  // one to many case
        if(m_gwsReader->ReadNext())
            return true;
        }
    ReleaseJoinReaders ();
    return m_gwsReader->ReadNext();
}


void CGwsFlatFdoReader::Close()
{
    CHECK_GWS_ITERATOR;

    ReleaseJoinReaders ();
    m_gwsReader->Close ();
}


static void MakeUniqueName(std::wstring& str, FdoStringCollection* names)
{
    wchar_t buffer[16];
    std::wstring baseName(str);
    int i = 1;
    while(-1 != names->IndexOf(str.c_str())) {
        str = baseName + /*MSG0*/L"_";

        #ifdef _WIN32
        str += _itow(i++, buffer, 10);
        #else
        swprintf(buffer, 16, L"%d", i++);
        #endif

    }
}


//*****************************************************************************
void CGwsFlatFdoReader::CollectPropertyNames (
    FdoStringCollection                     * names,
    std::map<std::wstring, PropertyDefItem> & types,
    IGWSExtendedFeatureDescription         *  pDescription,
    FdoStringCollection*                      keyProps,
    int                                       nItem,
    std::vector<int>                          iPath,
    std::vector<std::vector<int> >*           pPaths/* = NULL*/,
    std::vector<WSTR>*                        pKeys/* = NULL*/
)
{
    assert (NULL != pDescription);
    bool bSecondaryFeature = !(iPath.empty());  // secondary feature is everything but the primary
    // readonly props ara all secondary tabe props and primary table join keys
    // and, in the one to many case, all primary props as well
    bool bReadOnly = bSecondaryFeature || !pDescription->ForceOneToOneJoin();
    FdoPtr<FdoStringCollection>  strs  = pDescription->PropertyNames ();
    WSTR iKey = PropertyDefItem::MakeAKey (iPath, (int) iPath.size ());
    for (int j = 0; j < strs->GetCount (); j ++) {
        std::wstring wstr;

        if (bSecondaryFeature) {
            wstr.append(pDescription->ClassName().Name());
            if(NULL != pDescription->JoinDelimiter())
                wstr += pDescription->JoinDelimiter();
            wstr += strs->GetString (j);
        } else {
            wstr = strs->GetString (j);
        }

        MakeUniqueName(wstr,names);
        names->Add (wstr.c_str ());

        {
            bool bKeyProp = keyProps && (-1 != keyProps->IndexOf(wstr.c_str()));
            FdoPtr<FdoPropertyDefinition> pdef = pDescription->FindPropertyDefinition (strs->GetString (j));
            PropertyDefItem pdi(nItem, iPath, iKey, wstr.c_str (), pdef, pDescription, bReadOnly || bKeyProp);
            std::pair<std::wstring, PropertyDefItem> propDef(wstr,pdi);
            types.insert (propDef);
        }
    }
    if(bSecondaryFeature) {
        if(NULL != pPaths)
            pPaths->push_back(iPath);
        if(NULL != pKeys)
            pKeys->push_back(iKey);
    }
    for (int i = 0; i < pDescription->GetCount (); i ++) {
        FdoPtr<IGWSExtendedFeatureDescription> efdsc = pDescription->GetItem (i);
        nItem ++;
        iPath.push_back(i);
        CollectPropertyNames (names, types, efdsc, keyProps, nItem, iPath, pPaths, pKeys);
        iPath.pop_back();
    }
}


PropertyDefItem * CGwsFlatFdoReader::GetPropertyDefItem (FdoString * pname)
{
    std::map<std::wstring, PropertyDefItem>::iterator iter =
                                m_PropDefs.find (pname);
    if (iter != m_PropDefs.end ())
        return & (*iter).second;

    return NULL;
}


CGwsFeatureIterator * CGwsFlatFdoReader::GetReaderForProperty (
    FdoString * propname,
    WSTR      & origname
)
{
    if (! m_bJoin) {    // shortcut not joined features
        m_gwsReader.p->AddRef ();
        origname = propname;
        return m_gwsReader;
    }

    PropertyDefItem * pdefi = GetPropertyDefItem (propname);

    if (pdefi == NULL)
        GWS_THROW (eGwsInvalidPropertyName);

    origname = pdefi->GetOriginalName ();

    const WSTR & aKey = pdefi->GetAccessKey ();
    if (aKey.empty ()) {
        m_gwsReader.p->AddRef ();
        return m_gwsReader;
    }

    FdoPtr<CGwsFeatureIterator> reader = FindReader (aKey, pdefi->GetAccessPath ());
    if (reader != NULL)
        reader.p->AddRef ();
    return reader;
}


void CGwsFlatFdoReader::LoadAllReaders()
{
    if(m_bReadersLoaded)
        return;
    try {
        // load all the secondary readers associated with the join
        for(size_t i = 0; i < m_readerKeys.size(); i++) {
            FdoPtr<IGWSFeatureIterator> reader = FindReader (m_readerKeys[i], m_readerPaths[i]);
        }
        m_bReadersLoaded = true;
    } catch (...) {
        // this try/catch to handle the case where the sdf provider (cache)
        // is being loaded and being read simultaniously and throws exceptions
        ReleaseJoinReaders();
    }
}


FdoInt32 CGwsFlatFdoReader::GetCacheId()
{
    LoadAllReaders();
    std::vector<FdoInt32> cidVec;
    cidVec.push_back(m_gwsReader->GetCacheId());
    std::map<WSTR, CGwsFeatureIterator *>::iterator pIter;
    for(pIter = m_pJoinReaders.begin(); pIter!= m_pJoinReaders.end(); pIter++) {
        cidVec.push_back(pIter->second ? pIter->second->GetCacheId() : 0);
    }
    return m_gwsReader->ResolveJoinIdVector(cidVec);
}


CGwsFeatureIterator * CGwsFlatFdoReader::FindReader (
    const WSTR              & aKey,
    const std::vector<int>  & aPath
)
{
    if (aKey.empty ()) {
        m_gwsReader.p->AddRef ();
        return m_gwsReader;
    }

    std::map<WSTR, CGwsFeatureIterator *>::iterator iter = m_pJoinReaders.find (aKey);
    if (iter == m_pJoinReaders.end ()) {
        FdoPtr<IGWSFeatureIterator> reader;

    WSTR aSubKey = PropertyDefItem::MakeAKey (aPath, (int)aPath.size () - 1);
    std::vector<int> aSubPath;
    for (size_t i = 0; i < aPath.size () - 1; i ++) {
        aSubPath.push_back (aPath [i]);
    }
        reader = FindReader (aSubKey, aSubPath);

    CGwsFeatureIterator * fiter = NULL;
    bool                  bRes  = false;
    if (reader != NULL) {
        IGWSFeatureIterator* pIFIter = reader->GetJoinedFeatures (aPath [aPath.size () - 1]);
        fiter = dynamic_cast<CGwsFeatureIterator*>(pIFIter);
        if(NULL != fiter) {
            bRes = fiter->ReadNext ();
            if (! bRes) {
                fiter->Release ();
                fiter = NULL;
            }
        }
    }
        m_pJoinReaders.insert (std::map<WSTR, CGwsFeatureIterator *>::value_type (aKey, fiter));
        iter = m_pJoinReaders.find (aKey);
    }
    CGwsFeatureIterator * reader = iter->second;
    if (reader != NULL)
        reader->AddRef ();
    return reader;
}


void CGwsFlatFdoReader::ReleaseJoinReaders ()
{
    std::map<WSTR, CGwsFeatureIterator *>::iterator iter;

    for (iter = m_pJoinReaders.begin (); iter != m_pJoinReaders.end (); iter ++) {
        if ((*iter).second != NULL) {
            (*iter).second->Release ();
        }
    }
    m_pJoinReaders.clear ();
    m_bReadersLoaded = false;
}


//IGWSFeatureIterator::SdfIScrollableFeatureReader implementation.
int
CGwsFlatFdoReader::Count()
{
    if(-1 == m_tableSize)
        m_tableSize = m_gwsReader->Count();
    return m_tableSize;
}


bool
CGwsFlatFdoReader::ReadFirst()
{
    return ReadAtIndex(1);
}


bool
CGwsFlatFdoReader::ReadLast()
{
    return ReadAtIndex(Count());
}


bool
CGwsFlatFdoReader::ReadPrevious()
{
    if(m_curIndex > 1)
        return ReadAtIndex(m_curIndex - 1);
    m_curIndex = -1;
    return false;
}


bool
CGwsFlatFdoReader::ReadAt(FdoPropertyValueCollection* key)
{
    unsigned int index = IndexOf(key);
    return ReadAtIndex(index);
}


bool
CGwsFlatFdoReader::ReadAtIndex( unsigned int recordindex )
{
    if(m_curIndex == (int)recordindex)
        return m_curIndexResult;

    if( ((long)recordindex > (long)Count()) || (recordindex < 1)) {
        m_curIndex = -1;
        return false;
    }

    m_curIndex = (int) recordindex;
    ReleaseJoinReaders ();

    if(m_orderedCIDs.empty()) {     // unsorted results
        m_curCacheId = m_curIndex;  // unsorted - cacheId == index
        if( m_forceOneToOne ) {
            // one-to-one case - position primary reader only
            m_curIndexResult = m_gwsReader->ReadAtIndex(m_curIndex);
        } else {
            // one-to-many case get a cache id vector from the cfs and position all readers
            std::vector<FdoInt32>* pIndexVec = m_gwsReader->GetJoinIdVector(m_curCacheId);
            if(NULL == pIndexVec) {
                assert(false);  // something wrong - not finding join id vector
                m_curIndexResult = m_gwsReader->ReadAtIndex(m_curIndex);
            } else {
                // have a cacheId vector
                FdoInt32 cacheid = pIndexVec->at(0);
                m_curIndexResult = PositionPrimaryReaderAtCacheId(cacheid);
                    LoadAllReaders();
                    long x = 1;
                    std::map<WSTR, CGwsFeatureIterator *>::iterator iter;
                    for (iter = m_pJoinReaders.begin (); iter != m_pJoinReaders.end (); iter ++) {
                        if ((*iter).second != NULL)
                            (*iter).second->ReadAtCacheId(pIndexVec->at(x));
                        x++;
                    }
                }
            }
    } else {
        // sorted results - get cache id from map
        m_curCacheId = m_orderedCIDs[m_curIndex-1].joinCacheId;
        std::vector<FdoInt32>* pIndexVec = m_gwsReader->GetJoinIdVector(m_curCacheId);
        if(NULL == pIndexVec) {
            m_curIndex = -1;
            return false;
        }

        FdoInt32 primaryCID = pIndexVec->at(0);
        m_curIndexResult = PositionPrimaryReaderAtCacheId(primaryCID);
        if( !m_forceOneToOne ) {
            LoadAllReaders();
            long x = 1;
            std::map<WSTR, CGwsFeatureIterator *>::iterator iter;
            for (iter = m_pJoinReaders.begin (); iter != m_pJoinReaders.end (); iter ++) {
                if ((*iter).second != NULL)
                    (*iter).second->ReadAtCacheId(pIndexVec->at(x));
                x++;
            }
        }
    }
    return m_curIndexResult;
}


bool CGwsFlatFdoReader::PositionPrimaryReaderAtCacheId(FdoInt32 cacheId)
{
    if(m_gwsReader->Scrollable()) {
        if(m_left_cid_map.empty()) {
            FdoInt32 max = m_gwsReader->Count();
            for(FdoInt32 index = 1; index <= max;index++) {
                m_gwsReader->ReadAtIndex(index);
                m_left_cid_map.insert(std::pair<FdoInt32,FdoInt32>(m_gwsReader->GetCacheId(),index));
            }
        }
        std::map<FdoInt32,FdoInt32>::iterator pIter = m_left_cid_map.find(cacheId);
        if(pIter != m_left_cid_map.end())
            return m_gwsReader->ReadAtIndex(pIter->second);
    }
    return false;
}


unsigned int
CGwsFlatFdoReader::IndexOf(FdoPropertyValueCollection* key)
{
    // TODO: more efficient integer extraction
    if((NULL == key) || (key->GetCount() != 1))
        return false;
    FdoPtr<FdoPropertyValue> pVal = key->GetItem(0);
    if(NULL == pVal)
        return false;
    FdoPtr<FdoValueExpression> pExp = pVal->GetValue();
    if(NULL == pExp)
        return false;
    FdoString* pStr = pExp->ToString();
    if(NULL == pStr)
        return false;
    //int cacheId = _wtoi(pStr);
    int cacheId = wcstol(pStr, NULL, 10);
    std::vector<FdoInt32>::iterator pIter;

    if(m_orderedCIDs.empty()) {
        // unsorted
        return cacheId;
    } else {
        GWSTIMER(L"IndexOf");
        long index = 0;
        while((index < (long)m_orderedCIDs.size()) && (m_orderedCIDs[index].joinCacheId != cacheId))
            index++;
        return index == (long)m_orderedCIDs.size() ? 1 : 1+ index;
    }
}


FdoByteArray*
CGwsFlatFdoReader::GetOriginalGeometry(FdoString* propertyName)
{
    CHECK_GWS_ITERATOR;
    return m_gwsReader->GetOriginalGeometry(propertyName);
}


bool CGwsFlatFdoReader::Scrollable()
{
    return m_gwsReader->Scrollable();
}


static void GatherLeftKeys(IGWSJoinQueryDefinition* pJqd, FdoStringCollection* keyNames)
{
    IGWSJoinQueryDefinition*    pChildJoin;
    FdoPtr<IGWSQueryDefinition> qdef;
    FdoPtr<FdoStringCollection> lCols = pJqd->LeftJoinAttributes ();

    keyNames->Append(*lCols);

    qdef = pJqd->LeftQueryDefinition ();
    pChildJoin = dynamic_cast<IGWSJoinQueryDefinition*>(qdef.p);
    if(NULL != pChildJoin)
        GatherLeftKeys(pChildJoin,keyNames);
    qdef = pJqd->RightQueryDefinition ();
    pChildJoin = dynamic_cast<IGWSJoinQueryDefinition*>(qdef.p);
    if(NULL != pChildJoin)
        GatherLeftKeys(pChildJoin,keyNames);
}


void CGwsFlatFdoReader::BuildFlatDescription(IGWSQueryDefinition* pQryDef,
                                             IGWSExtendedFeatureDescription* pJoinDesc,
                                             IGWSExtendedFeatureDescription** ppResDesc)
{
    FdoPtr<FdoStringCollection>                 names;
    FdoPtr<FdoStringCollection>                 keyNames;
    PropertyDefItem *                           pdefi;
    IGWSJoinQueryDefinition*                    pJQryDef = NULL;
    std::map<WSTR, PropertyDefItem>             propDefs;
    std::map<WSTR, PropertyDefItem>::iterator   pdIter;
    std::vector<int>                            iPath;
    int                                         nItem = 0;

    names = FdoStringCollection::Create ();
    keyNames = FdoStringCollection::Create ();
    pJQryDef = dynamic_cast<IGWSJoinQueryDefinition*>(pQryDef);

    if(NULL != pJQryDef)
        GatherLeftKeys(pJQryDef,keyNames);

    CollectPropertyNames (names, propDefs, pJoinDesc, keyNames, nItem, iPath);

    std::wstring                           wstr;

    // use the first one as a class name. But who cares
    wstr = pJoinDesc->ClassName ().Name ();
    wstr.append (L"_FlattenJoin");

    FdoFeatureClass * clsDef = FdoFeatureClass::Create (wstr.c_str(), L"Flatten_Join");
    FdoPtr<FdoPropertyDefinitionCollection> pdefs = clsDef->GetProperties ();
    for (int i = 0; i < names->GetCount (); i ++) {
        pdIter = propDefs.find (names->GetString (i));
        if (pdIter != propDefs.end ()) {
            pdefi = & (*pdIter).second;
            FdoPtr<FdoPropertyDefinition> pdef = pdefi->GetFlatPropertyDefinition ();
            pdefs->Add (pdef);
        }
    }
    CGwsQueryResultDescriptors * resdsc =
            new CGwsQueryResultDescriptors (clsDef, clsDef->GetName (), NULL, NULL, true, NULL);
    * ppResDesc = resdsc;
    if (* ppResDesc)
        (* ppResDesc)->AddRef ();
}


static bool siCmp(sortKey a, sortKey b)
{
    return a.sortPropId < b.sortPropId;
}


void buildSortIndex(CGwsFeatureIterator* pIterator, std::vector<FdoInt32>& proporder, size_t tix,std::vector<sortKey>& m_sortedIndex)
{
    // build sort index
    std::vector<FdoInt32> *pVec;
    FdoInt32 jCacheId(1);
    sortKey sk;
    {
        GWSTIMER(/*MSG0*/L"BuildSortIndex");
        while(NULL != (pVec = pIterator->GetJoinIdVector(jCacheId))) {
            sk.joinCacheId = jCacheId;
            std::vector<FdoInt32>::iterator iter = std::find(proporder.begin(), proporder.end(),pVec->at(tix));
            size_t sortIx = iter-proporder.begin();
            sk.sortPropId = (FdoInt32)sortIx;
            m_sortedIndex.push_back(sk);
            jCacheId++;
        }
    }
    {
        // sort the index
        GWSTIMER(/*MSG0*/L"SortSortIndex");
        std::sort(m_sortedIndex.begin(),m_sortedIndex.end(),siCmp);
    }
}


class dvless
{
public:
    bool operator()(const FdoPtr<FdoDataValue>& a, const FdoPtr<FdoDataValue>& b)
    {
        return GWSFdoUtilities::CompareDataValues(a,b) < 0;
    }
};


static void SortByDV(CGwsFeatureIterator* pIter, FdoString* propName, FdoOrderingOption order, std::vector<FdoInt32>& propertyOrder)
{
    std::multimap<FdoPtr<FdoDataValue>,FdoInt32,dvless> propmap;
    FdoInt32 i(1);
    while(pIter->ReadAtIndex(i++)) {
        FdoInt32 cacheId = pIter->GetCacheId();
        if(!pIter->IsNull(propName)) {
            std::pair<FdoPtr<FdoDataValue>,FdoInt32>  proppair(pIter->GetDataValue(propName),cacheId);
            propmap.insert(proppair);
        }
    }
    // now we have an ordering of the cache ids
    if(FdoOrderingOption_Ascending==order) {
        std::multimap<FdoPtr<FdoDataValue>,FdoInt32,dvless>::iterator mmIter;
        for(mmIter = propmap.begin(); mmIter!= propmap.end(); mmIter++)
            propertyOrder.push_back(mmIter->second);
    } else {
        std::multimap<FdoPtr<FdoDataValue>,FdoInt32,dvless>::reverse_iterator mmIter;
        for(mmIter = propmap.rbegin(); mmIter!= propmap.rend(); mmIter++)
            propertyOrder.push_back(mmIter->second);
    }
}


template <class T>
static void bpo(std::multimap<T,FdoInt32>& propmap, FdoOrderingOption order, std::vector<FdoInt32>& propertyOrder)
{
    if(FdoOrderingOption_Ascending==order) {
        typename std::multimap<T,FdoInt32>::iterator mmIter;
        for(mmIter = propmap.begin(); mmIter!= propmap.end(); mmIter++)
            propertyOrder.push_back(mmIter->second);
    } else {
        typename std::multimap<T,FdoInt32>::reverse_iterator mmIter;
        for(mmIter = propmap.rbegin(); mmIter!= propmap.rend(); mmIter++)
            propertyOrder.push_back(mmIter->second);
    }
}


static void BuildPropOrder(CGwsFeatureIterator* pIter, FdoString* propName, FdoOrderingOption order, std::vector<FdoInt32>& propertyOrder)
{
    FdoPtr<FdoDataValue> pVal = pIter->GetDataValue(propName);
    FdoDataType dtype = pVal->GetDataType();
    FdoInt32 i(1);
    switch(dtype) {
    case FdoDataType_String:
        {
            std::multimap<std::wstring,FdoInt32> propmap;
            while(pIter->ReadAtIndex(i++))
                if(!pIter->IsNull(propName))
                    propmap.insert(std::pair<std::wstring,FdoInt32>(pIter->GetString(propName),pIter->GetCacheId()));
            bpo<std::wstring>(propmap,order,propertyOrder);
        }
        break;
    case FdoDataType_Single:
        {
            std::multimap<float,FdoInt32> propmap;
            while(pIter->ReadAtIndex(i++))
                if(!pIter->IsNull(propName))
                    propmap.insert(std::pair<float,FdoInt32>(pIter->GetSingle(propName),pIter->GetCacheId()));
            bpo<float>(propmap,order,propertyOrder);
        }
        break;
    case FdoDataType_Decimal:
    case FdoDataType_Double:
        {
            std::multimap<double,FdoInt32> propmap;
            while(pIter->ReadAtIndex(i++))
                if(!pIter->IsNull(propName))
                    propmap.insert(std::pair<double,FdoInt32>(pIter->GetDouble(propName),pIter->GetCacheId()));
            bpo<double>(propmap,order,propertyOrder);
        }
        break;
    case FdoDataType_Boolean:
        {
            std::multimap<FdoBoolean,FdoInt32> propmap;
            while(pIter->ReadAtIndex(i++))
                if(!pIter->IsNull(propName))
                    propmap.insert(std::pair<FdoBoolean,FdoInt32>(pIter->GetBoolean(propName),pIter->GetCacheId()));
            bpo<FdoBoolean>(propmap,order,propertyOrder);
        }
        break;
    case FdoDataType_Byte:
        {
            std::multimap<FdoByte,FdoInt32> propmap;
            while(pIter->ReadAtIndex(i++))
                if(!pIter->IsNull(propName))
                    propmap.insert(std::pair<FdoByte,FdoInt32>(pIter->GetByte(propName),pIter->GetCacheId()));
            bpo<FdoByte>(propmap,order,propertyOrder);
        }
        break;
    case FdoDataType_Int16:
        {
            std::multimap<FdoInt16,FdoInt32> propmap;
            while(pIter->ReadAtIndex(i++))
                if(!pIter->IsNull(propName))
                    propmap.insert(std::pair<FdoInt16,FdoInt32>(pIter->GetInt16(propName),pIter->GetCacheId()));
            bpo<FdoInt16>(propmap,order,propertyOrder);
        }
        break;
    case FdoDataType_Int32:
        {
            std::multimap<FdoInt32,FdoInt32> propmap;
            while(pIter->ReadAtIndex(i++))
                if(!pIter->IsNull(propName))
                    propmap.insert(std::pair<FdoInt32,FdoInt32>(pIter->GetInt32(propName),pIter->GetCacheId()));
            bpo<FdoInt32>(propmap,order,propertyOrder);
        }
        break;
    case FdoDataType_Int64:
        {
            std::multimap<FdoInt64,FdoInt32> propmap;
            while(pIter->ReadAtIndex(i++))
                if(!pIter->IsNull(propName))
                    propmap.insert(std::pair<FdoInt64,FdoInt32>(pIter->GetInt64(propName),pIter->GetCacheId()));
            bpo<FdoInt64>(propmap,order,propertyOrder);
        }
        break;
    case FdoDataType_DateTime:
    case FdoDataType_BLOB:
    case FdoDataType_CLOB:
    default:
        SortByDV(pIter,propName,order,propertyOrder);
        break;
    }
}


void CGwsFlatFdoReader::SortResults()
{
    CGwsPreparedQuery* pPQ = m_gwsReader->PrepQuery();
    if(NULL != pPQ) {
        FdoPtr<IGWSQueryDefinition> qDef;
        pPQ->GetQueryDefinition(&qDef);
        if(NULL != qDef) {
            m_orderBy   = qDef->GetOrderBy ();
            m_orderOpt  = qDef->GetOrderingOption ();
        }
        if( (NULL != m_orderBy) && (m_orderBy->GetCount() > 0) ) {
            GWSTIMER(/*MSG0*/L"SortResults");
            m_curIndex = -1;
            m_tableSize = -1;
            m_bReadersLoaded = false;
            ReadAtIndex(1);
            LoadAllReaders();
            WSTR propName;
            FdoString*  propertyName = m_orderBy->GetString(0);
            FdoPtr<CGwsFeatureIterator> pIter = GetReaderForProperty (propertyName, propName);
            if(pIter != NULL) {
                std::vector<FdoInt32>   propertyOrder;
                BuildPropOrder(pIter,propName.c_str(),m_orderOpt,propertyOrder);
                // build a sort index
                FdoInt32 iterIndex(0);
                if(pIter != m_gwsReader) {
                    iterIndex = 1;
                    std::map<WSTR,CGwsFeatureIterator*>::iterator pMapIter = m_pJoinReaders.begin();
                    while((m_pJoinReaders.end() != pMapIter) && (pMapIter->second != pIter)) {
                        iterIndex++;
                        pMapIter++;
                    }
                }
                buildSortIndex(m_gwsReader,propertyOrder,iterIndex,m_orderedCIDs);
            }
        }
    }
}
