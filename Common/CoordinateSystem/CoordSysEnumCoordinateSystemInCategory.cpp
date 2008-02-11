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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysEnumCoordinateSystemInCategory.h"     //for CCoordinateSystemEnumCoordinateSystemInCategory
#include "CoordSysUtil.h"                               //for Convert_Ascii_To_Wide

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemEnumCoordinateSystemInCategory::CCoordinateSystemEnumCoordinateSystemInCategory(MgCoordinateSystemCatalog *pCatalog)
    : m_kpListCoordinateSystemNames(NULL)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

CCoordinateSystemEnumCoordinateSystemInCategory::~CCoordinateSystemEnumCoordinateSystemInCategory()
{
    ClearFilter();
}

//MgDisposable
void CCoordinateSystemEnumCoordinateSystemInCategory::Dispose()
{
    delete this;
}

//----------------------------------------------------------
//Set the filter.
void CCoordinateSystemEnumCoordinateSystemInCategory::AddFilter(MgCoordinateSystemFilter *pFilter)
{
    MG_TRY()
    ClearFilter();
    m_vectFilter.push_back(pFilter);
    SAFE_ADDREF(pFilter);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.SetFilter")
}

//----------------------------------------------------------
//This function clears the Filter, if one is set.
//
void CCoordinateSystemEnumCoordinateSystemInCategory::ClearFilter()
{
    for (size_t i=0; i<m_vectFilter.size(); i++)
    {
        SAFE_RELEASE(m_vectFilter[i]);
    }
    m_vectFilter.clear();
}

//----------------------------------------------------------
//This function must be called on a freshly-constructed CCoordinateSystemEnumCoordinateSystemInCategory
//object before anything else can be done with it.
//
void CCoordinateSystemEnumCoordinateSystemInCategory::Initialize(
    const CSystemNameList *kpListCoordinateSystemNames)

{
    assert(NULL != kpListCoordinateSystemNames);

    m_kpListCoordinateSystemNames = kpListCoordinateSystemNames;
    m_iter = m_kpListCoordinateSystemNames->begin();
}

//----------------------------------------------------------
MgDisposableCollection* CCoordinateSystemEnumCoordinateSystemInCategory::Next(UINT32 ulCount)
{
    Ptr<MgDisposableCollection> pOutput;

    MG_TRY()
    pOutput=new MgDisposableCollection;
    if (!pOutput)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.Next", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemEnum.Next", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemDictionary> pCsDict=m_pCatalog->GetCoordinateSystemDictionary();
    if (!pCsDict)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.Next", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    wchar_t* pstr;
    for (; m_iter != m_kpListCoordinateSystemNames->end(); m_iter++)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }

        //get the coordinate system name from the category
        const char *kpName = (*m_iter).name;

        pstr = Convert_Ascii_To_Wide(kpName);
        if (NULL == pstr)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.Next", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        STRING strCsName(pstr);
        delete[] pstr;

        //now get the definition from the dictionary
        Ptr<MgGuardDisposable> pCs=pCsDict->Get(strCsName);
        if (!pCs)
        {
            MgStringCollection arguments;
            arguments.Add(strCsName);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemEnum.Next", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        //is it filtered out?
        if (IsFilteredOut(pCs))
        {
            continue;
        }
        pOutput->Add(pCs);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.Next")
    return pOutput.Detach();
}

//----------------------------------------------------------
//Fetches the names of the next ulCount items.  The caller
//is responsible for freeing the returned strings via delete[]
//
MgStringCollection* CCoordinateSystemEnumCoordinateSystemInCategory::NextName(UINT32 ulCount)
{
    Ptr<MgStringCollection> pOutput;
    MG_TRY()
    pOutput=new MgStringCollection;
    if (!pOutput)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.NextName", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
    }
    wchar_t* pstr;
    for (; m_iter != m_kpListCoordinateSystemNames->end(); m_iter++)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }
        const char *kpName = (*m_iter).name;

        if (IsFilteredOut(kpName))
        {
            continue;
        }

        pstr = Convert_Ascii_To_Wide(kpName);
        if (NULL == pstr)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.NextName", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
        }
        pOutput->Add(pstr);
        delete[] pstr;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.NextName")
    return pOutput.Detach();
}

//----------------------------------------------------------
MgStringCollection* CCoordinateSystemEnumCoordinateSystemInCategory::NextDescription(UINT32 ulCount)
{
    Ptr<MgStringCollection> pOutput;
    MG_TRY()
    pOutput=new MgStringCollection;
    if (!pOutput)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.NextDescription", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemEnum.NextDescription", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemDictionary> pCsDict=m_pCatalog->GetCoordinateSystemDictionary();
    if (!pCsDict)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.NextDescription", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    wchar_t* pstr;
    for (; m_iter != m_kpListCoordinateSystemNames->end(); m_iter++)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }
        //get the coordinate system name from the category
        const char *kpName = (*m_iter).name;

        pstr = Convert_Ascii_To_Wide(kpName);
        if (NULL == pstr)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.NextDescription", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        STRING strCsName(pstr);
        delete[] pstr;

        //now get the definition from the dictionary
        Ptr<MgGuardDisposable> pCs=pCsDict->Get(strCsName);
        if (!pCs)
        {
            MgStringCollection arguments;
            arguments.Add(strCsName);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemEnum.NextDescription", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        //is it filtered out?
        if (IsFilteredOut(pCs))
        {
            continue;
        }
        MgCoordinateSystem* pCsDef=dynamic_cast<MgCoordinateSystem*>(pCs.p);
        assert(pCsDef);
        STRING strDescription=pCsDef->GetDescription();

        pOutput->Add(strDescription);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.NextDescription")
    return pOutput.Detach();
}

//Skips the next ulSkipCount names.
//Throws an exception if
//ulSkipCount items were not skipped
//
void CCoordinateSystemEnumCoordinateSystemInCategory::Skip(UINT32 ulSkipCount)
{
    MG_TRY()
    UINT32 ulSkipped;
    for (ulSkipped=0; m_iter != m_kpListCoordinateSystemNames->end(); m_iter++)
    {
        if (ulSkipped == ulSkipCount)
        {
            //success
            return;
        }
        //get the coordinate system name from the category
        const char *kpName = (*m_iter).name;
        if (IsFilteredOut(kpName))
        {
            continue;
        }

        ulSkipped++;
    }

    if (ulSkipCount != ulSkipped)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEnum.Skip", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.Skip")
}

//Resets the enumerator to the start of the collection.
//
void CCoordinateSystemEnumCoordinateSystemInCategory::Reset()
{
    m_iter = m_kpListCoordinateSystemNames->begin();
}

//This function evaluates the provided def using the current
//Filter.  If there is no Filter, the result will always
//be true.
//
bool CCoordinateSystemEnumCoordinateSystemInCategory::IsFilteredOut(const char *kpName)
{
    bool bIsFiltered=false;

    MG_TRY()
    assert(NULL != kpName);
    if (!kpName)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemEnum.IsFilteredOut", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Get a def from the set for the Filter to work with
    wchar_t* pStr = Convert_Ascii_To_Wide(kpName);
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.IsFilteredOut", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
    }
    bool bIsFiltered=IsFilteredOut(pStr);
    delete[] pStr;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.IsFilteredOut")
    return bIsFiltered;
}

//This function evaluates the provided def using the current
//Filter.  If there is no Filter, the result will always
//be true.
//
bool CCoordinateSystemEnumCoordinateSystemInCategory::IsFilteredOut(const wchar_t *kpName)
{
    bool bIsFiltered=false;

    MG_TRY()
    assert(NULL != kpName);
    if (!kpName)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemEnum.IsFilteredOut", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //If a Filter hasn't been specified, just return true.
    if (m_vectFilter.empty())
    {
        //No Filter was specified.
        return false;
    }

    if (!m_pCatalog)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemEnum.IsFilteredOut", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgCoordinateSystemDictionary> pCsDict=m_pCatalog->GetCoordinateSystemDictionary();
    if (!pCsDict)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.IsFilteredOut", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Get a def from the set for the Filter to work with
    STRING str(kpName);
    Ptr<MgGuardDisposable> pDef = pCsDict->Get(str);
    assert(pDef);
    if (!pDef)
    {
        MgStringCollection arguments;
        arguments.Add(str);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemEnum.IsFilteredOut", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    //Evaluate it
    bIsFiltered=IsFilteredOut(pDef);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.IsFilteredOut")
    return bIsFiltered;
}

//This function evaluates the provided def using the current
//Filter.  If there is no Filter, the result will always
//be true.
//
bool CCoordinateSystemEnumCoordinateSystemInCategory::IsFilteredOut(MgGuardDisposable *pDef)
{
    MG_TRY()
    assert(NULL != pDef);
    if (!pDef)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemEnum.IsFilteredOut", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    for (size_t i=0; i<m_vectFilter.size(); i++)
    {
        if (m_vectFilter[i]->IsFilteredOut(pDef))
        {
            return true;
        }
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.IsFilteredOut")

    //No Filter has been set.
    return false;
}


//Creates a copy of the enumerator, with the same enumeration state.
//The bool returned is true for success; if out of memory,
//false; if some other error, E_UNEXPECTED.
//
MgCoordinateSystemEnum* CCoordinateSystemEnumCoordinateSystemInCategory::CreateClone()
{
    Ptr<CCoordinateSystemEnumCoordinateSystemInCategory> pNew;

    MG_TRY()

    //Make a new object
    pNew = new CCoordinateSystemEnumCoordinateSystemInCategory(m_pCatalog);

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy it from this one
    pNew->m_iter = m_iter;
    pNew->m_kpListCoordinateSystemNames = m_kpListCoordinateSystemNames;
    for (size_t i=0; i<m_vectFilter.size(); i++)
    {
        pNew->m_vectFilter.push_back(m_vectFilter[i]);
        SAFE_ADDREF(m_vectFilter[i]);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.CreateClone")

    //And we're done!  Return success.
    return pNew.Detach();
}


//End of file.
