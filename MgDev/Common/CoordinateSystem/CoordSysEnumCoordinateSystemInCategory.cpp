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

#include <vector>

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysEnumCoordinateSystemInCategory.h"     //for CCoordinateSystemEnumCoordinateSystemInCategory
#include "CoordSysUtil.h"                               //for Convert_UTF8_To_Wide
#include "MentorUtil.h"                                 //for IsLegalMentorName()

using namespace CSLibrary;
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemEnumCoordinateSystemInCategory::CCoordinateSystemEnumCoordinateSystemInCategory(MgCoordinateSystemCatalog *pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

CCoordinateSystemEnumCoordinateSystemInCategory::CCoordinateSystemEnumCoordinateSystemInCategory(CCoordinateSystemEnumCoordinateSystemInCategory const& copyFrom)
{
    this->m_pCatalog = SAFE_ADDREF(copyFrom.m_pCatalog.p);
    this->Initialize(copyFrom.m_csNames);

    for (size_t i=0; i < copyFrom.m_vectFilter.size(); i++)
    {
        this->m_vectFilter.push_back(copyFrom.m_vectFilter[i]);
        SAFE_ADDREF(this->m_vectFilter[i]);
    }
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
void CCoordinateSystemEnumCoordinateSystemInCategory::Initialize(std::vector<STRING> const& allCsNames)
{
    this->m_csNames = allCsNames;
    this->m_iter = this->m_csNames.begin();
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

    Ptr<MgCoordinateSystemDictionary> pCsDict = m_pCatalog->GetCoordinateSystemDictionary();
    if (!pCsDict)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.Next", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoDictionaryException", NULL);
    }

    for (; m_iter != this->m_csNames.end(); ++m_iter)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }

        //now get the definition from the dictionary
        Ptr<MgGuardDisposable> pCs = pCsDict->Get(m_iter->c_str());
        _ASSERT(NULL != pCs);

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
        Ptr<MgDisposableCollection> pNextCsObjects = this->Next(ulCount);
        for(INT32 i = 0; i < pNextCsObjects->GetCount(); ++i)
        {
            pOutput->Add(Ptr<MgCoordinateSystem>(dynamic_cast<MgCoordinateSystem*>(pNextCsObjects->GetItem(i)))->GetCsCode());
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
        Ptr<MgDisposableCollection> pNextCsObjects = this->Next(ulCount);
        for(INT32 i = 0; i < pNextCsObjects->GetCount(); ++i)
        {
            pOutput->Add(Ptr<MgCoordinateSystem>(dynamic_cast<MgCoordinateSystem*>(pNextCsObjects->GetItem(i)))->GetDescription());
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
    for (ulSkipped=0; m_iter != m_csNames.end(); ++m_iter)
    {
        if (ulSkipped == ulSkipCount)
        {
            //success
            return;
        }

        //get the coordinate system name from the category
        if (IsFilteredOut(m_iter->c_str()))
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
    m_iter = this->m_csNames.begin();
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
    CHECKARGUMENTNULL(kpName, L"MgCoordinateSystemEnum.IsFilteredOut");

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
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystem.IsFilteredOut", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoDictionaryException", NULL);
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
    CHECKARGUMENTNULL(pDef, L"MgCoordinateSystemEnum.IsFilteredOut");

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
    MG_TRY()

        //Make a new object and return it
        return new CCoordinateSystemEnumCoordinateSystemInCategory(*this);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.CreateClone")

    _ASSERT(false);
    return NULL;
}
