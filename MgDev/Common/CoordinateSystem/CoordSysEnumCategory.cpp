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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysEnumCategory.h"           //for CCoordinateSystemEnumCategory
#include "CoordSysUtil.h"                   //for Convert_UTF8_To_Wide
#include "MentorUtil.h"                        //for IsLegalMentorName()

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemEnumCategory::CCoordinateSystemEnumCategory()
    : m_pCategoryNameList(NULL)
{
}

CCoordinateSystemEnumCategory::~CCoordinateSystemEnumCategory()
{
    ClearFilter();
}

//MgDisposable
void CCoordinateSystemEnumCategory::Dispose()
{
    delete this;
}

//This function clears the filter, if one is set.
//
void CCoordinateSystemEnumCategory::ClearFilter()
{
    for (size_t i=0; i<m_vectFilter.size(); i++)
    {
        SAFE_RELEASE(m_vectFilter[i]);
    }
    m_vectFilter.clear();
}

//Evaluates the specified def, according to the currently
//set filter.  If no filter has been set, always returns true.
//
bool CCoordinateSystemEnumCategory::IsFilteredOut(MgGuardDisposable *pDef)
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

    //No filter has been set.
    return false;
}

//----------------------------------------------------------
//Evaluates the def with the specified name, according to the
//currently set selector.  If no selector has been set,
//always returns true.
//
bool CCoordinateSystemEnumCategory::IsFilteredOut(const char *kpName)
{
    bool bIsFiltered=false;

    MG_TRY()

    assert(NULL != kpName);
    CHECKARGUMENTNULL(kpName, L"MgCoordinateSystemEnum.IsFilteredOut");

    //If a filter hasn't been specified, just return false.
    if (m_vectFilter.empty())
    {
        //No filter was specified.
        return false;
    }

    //Get a def from the set for the filter to work with
    wchar_t* pStr = Convert_UTF8_To_Wide(kpName);
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.IsFilteredOut", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING str(pStr);
    delete[] pStr;
    Ptr<MgGuardDisposable> pDef = m_pDict->Get(str);
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

//----------------------------------------------------------
//This function must be called on a freshly-constructed CCoordinateSystemEnumCategory
//object before anything else can be done with it.
//
void CCoordinateSystemEnumCategory::Initialize(
    MgCoordinateSystemCategoryDictionary* pDict,
    CCategoryNameList *kpCategoryNameList)
{
    assert(NULL != kpCategoryNameList);

    m_pCategoryNameList = kpCategoryNameList;
    m_iter = m_pCategoryNameList->begin();
    m_pDict=pDict;
}

//----------------------------------------------------------
//Set the filter.
void CCoordinateSystemEnumCategory::AddFilter(MgCoordinateSystemFilter *pFilter)
{
    MG_TRY()
    ClearFilter();
    m_vectFilter.push_back(pFilter);
    SAFE_ADDREF(pFilter);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.SetFilter")
}

//----------------------------------------------------------
MgDisposableCollection* CCoordinateSystemEnumCategory::Next(UINT32 ulCount)
{
    Ptr<MgDisposableCollection> pOutput;

    MG_TRY()
    pOutput=new MgDisposableCollection;
    if (!pOutput)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.Next", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    wchar_t* pStr;

    for (; m_iter != m_pCategoryNameList->end(); ++m_iter)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }

        //get the category definition for the next name in the list
        const char *kpName = (*(m_iter)).Name();
        pStr = Convert_UTF8_To_Wide(kpName);
        if (NULL == pStr)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.Next", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        STRING str(pStr);
        delete[] pStr;
        Ptr<MgGuardDisposable> pDef = m_pDict->Get(str);
        assert(pDef);
        if (!pDef)
        {
            MgStringCollection arguments;
            arguments.Add(str);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemEnum.Next", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        //check if this category is filtered out or not
        if (IsFilteredOut(pDef))
        {
            continue;
        }

        //add the category defintion to the output list
        pOutput->Add(pDef);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.Next")
    return pOutput.Detach();
}

//----------------------------------------------------------
//Fetches the names of the next ulCount items.  The caller
//is responsible for freeing the returned strings via delete[]
//
MgStringCollection* CCoordinateSystemEnumCategory::NextName(UINT32 ulCount)
{
    Ptr<MgStringCollection> pOutput;
    MG_TRY()
    pOutput=new MgStringCollection;
    if (!pOutput)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.NextName", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    for (; m_iter != m_pCategoryNameList->end(); ++m_iter)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }

        const char *kpName = (*m_iter).Name();
        if (IsFilteredOut(kpName))
        {
            continue;
        }

        wchar_t *pwName = Convert_UTF8_To_Wide(kpName);
        if (NULL == pwName)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.NextName", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        pOutput->Add(pwName);
        delete[] pwName;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.NextName")
    return pOutput.Detach();
}

//----------------------------------------------------------
MgStringCollection* CCoordinateSystemEnumCategory::NextDescription(UINT32 ulCount)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemEnum.NextDescription", __LINE__, __WFILE__, NULL, L"", NULL);
}

//----------------------------------------------------------
//Skips the next ulSkipCount names.
//Throws an exception if ulSkipCount items were not skipped
//
void CCoordinateSystemEnumCategory::Skip(UINT32 ulSkipCount)
{
    MG_TRY()

    UINT32 ulSkipped;
    for (ulSkipped=0; m_iter != m_pCategoryNameList->end(); ++m_iter)
    {
        if (ulSkipped == ulSkipCount)
        {
            //success
            return;
        }
        const char *kpName = (*m_iter).Name();
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

//----------------------------------------------------------
//Resets the enumerator to the start of the collection.
//
void CCoordinateSystemEnumCategory::Reset()
{
    m_iter = m_pCategoryNameList->begin();
}

//----------------------------------------------------------
//Creates a copy of the enumerator, with the same enumeration state.
//The bool returned is true for success; if out of memory,
//false; if some other error, E_UNEXPECTED.
//
MgCoordinateSystemEnum* CCoordinateSystemEnumCategory::CreateClone()
{
    Ptr<CCoordinateSystemEnumCategory> pNew;

    MG_TRY()

    //Make a new object
    pNew = new CCoordinateSystemEnumCategory;

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy it from this one
    pNew->m_pDict = m_pDict;
    pNew->m_pCategoryNameList = m_pCategoryNameList;
    pNew->m_iter = m_iter;
    for (size_t i=0; i<m_vectFilter.size(); i++)
    {
        pNew->m_vectFilter.push_back(m_vectFilter[i]);
        SAFE_ADDREF(m_vectFilter[i]);
    }

    //And we're done!  Return success.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.CreateClone")

    return pNew.Detach();
}
