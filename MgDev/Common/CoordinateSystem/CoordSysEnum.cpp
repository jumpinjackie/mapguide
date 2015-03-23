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

#include "CoordSysEnum.h"           //for CCoordinateSystemEnum
#include "CoordSysUtil.h"           //for Convert_UTF8_To_Wide
#include "MentorUtil.h"                //for IsLegalMentorName()

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemEnum::CCoordinateSystemEnum() :
    m_pmapSystemNameDescription(NULL), m_readAllDefCallback(NULL)
{
}

CCoordinateSystemEnum::~CCoordinateSystemEnum()
{
    Uninitialize();
}

//MgDisposable
void CCoordinateSystemEnum::Dispose()
{
    delete this;
}

//----------------------------------------------------------
//This function must be called on a freshly-constructed CCoordinateSystemEnum
//object before anything else can be done with it.
//
void CCoordinateSystemEnum::Initialize(
    MgCoordinateSystemDictionaryBase* pDict,
    CSystemNameDescriptionMap *pmapSystemNameDescription)
{
    assert(NULL != pmapSystemNameDescription);
    assert(NULL != pDict);

    Uninitialize();

    m_pmapSystemNameDescription = pmapSystemNameDescription;
    m_pDict = SAFE_ADDREF(pDict);
    m_iter = m_pmapSystemNameDescription->begin();
}

//----------------------------------------------------------
//Uninitializer.
//
void CCoordinateSystemEnum::Uninitialize()
{
    m_pmapSystemNameDescription = NULL;
    m_pDict = NULL;
    m_readAllDefCallback = NULL;

    ClearFilter();
}

//----------------------------------------------------------
//Set the filter.
void CCoordinateSystemEnum::AddFilter(MgCoordinateSystemFilter *pFilter)
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
void CCoordinateSystemEnum::ClearFilter()
{
    for (size_t i=0; i<m_vectFilter.size(); i++)
    {
        SAFE_RELEASE(m_vectFilter[i]);
    }
    m_vectFilter.clear();
}

//----------------------------------------------------------
//Reads the entries from the parent dictionary
MgDisposableCollection* CCoordinateSystemEnum::Next(UINT32 ulCount)
{
    Ptr<MgDisposableCollection> pOutput;

    MG_TRY()
    pOutput=new MgDisposableCollection;
    if (!pOutput)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.Next", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //optimization:
    //if the caller intends to read all information at once, that is...
    // - if our iterator points to the map's beginning and
    // - the number of entries to read is at least the size of the entries we know of
    //
    //use the the callback (if there's any) into our parent dictionary to read all entries
    if (ulCount >= this->m_pmapSystemNameDescription->size() && this->m_iter == this->m_pmapSystemNameDescription->begin())
    {
        if (NULL != this->m_readAllDefCallback)
        {
            pOutput = this->m_readAllDefCallback(this->m_pDict, &this->m_vectFilter);
            this->m_iter = this->m_pmapSystemNameDescription->end(); //set our iterator to end() as we've read all entries
            
            return pOutput.Detach();
        }
    }

    //default behavior - read each entry one by one and add it to the list to be returned
    for (; m_iter != m_pmapSystemNameDescription->end(); ++m_iter)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }
        const char *kpName = (*m_iter).first.Name();
        wchar_t* pStr = Convert_UTF8_To_Wide(kpName);
        if (NULL == pStr)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.Next", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING str(pStr);
        delete[] pStr;
        Ptr<MgGuardDisposable> pDef=m_pDict->Get(str);
        assert(pDef);
        if (!pDef)
        {
            MgStringCollection arguments;
            arguments.Add(str);
            throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemEnum.Next", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        //is it filtered out?
        if (IsFilteredOut(pDef))
        {
            continue;
        }
        pOutput->Add(pDef);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.Next")
    return pOutput.Detach();
}

//----------------------------------------------------------
//Fetches the names of the next ulCount items.  The caller
//is responsible for freeing the returned strings via delete[]
//
MgStringCollection* CCoordinateSystemEnum::NextName(UINT32 ulCount)
{
    Ptr<MgStringCollection> pOutput;
    MG_TRY()
    pOutput=new MgStringCollection;
    if (!pOutput)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.NextName", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    for (; m_iter != m_pmapSystemNameDescription->end(); ++m_iter)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }
        const char *kpName = (*m_iter).first.Name();
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
MgStringCollection* CCoordinateSystemEnum::NextDescription(UINT32 ulCount)
{
    Ptr<MgStringCollection> pOutput;
    MG_TRY()
    pOutput=new MgStringCollection;
    if (!pOutput)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.NextDescription", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    for (; m_iter != m_pmapSystemNameDescription->end(); ++m_iter)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }
        if (IsFilteredOut((*m_iter).first.Name()))
        {
            continue;
        }
        const char *kpDecsription = (*m_iter).second.Name();
        wchar_t *pwDecsription = Convert_UTF8_To_Wide(kpDecsription);
        if (NULL == pwDecsription)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.NextDescription", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        pOutput->Add(pwDecsription);
        delete[] pwDecsription;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.NextDescription")
    return pOutput.Detach();
}

//----------------------------------------------------------
//Skips the next ulSkipCount names.
//Throws an exception if ulSkipCount items were not skipped
//
void CCoordinateSystemEnum::Skip(UINT32 ulSkipCount)
{
    MG_TRY()

    UINT32 ulSkipped;
    for (ulSkipped=0; m_iter != m_pmapSystemNameDescription->end(); ++m_iter)
    {
        if (ulSkipped == ulSkipCount)
        {
            //success
            return;
        }
        const char *kpName = (*m_iter).first.Name();
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
void CCoordinateSystemEnum::Reset()
{
    m_iter = m_pmapSystemNameDescription->begin();
}

//----------------------------------------------------------
//This function evaluates the provided def using the current
//Filter.  If there is no Filter, the result will always
//be true.
//
bool CCoordinateSystemEnum::IsFilteredOut(const char *kpName)
{
    bool bIsFiltered=false;

    MG_TRY()

    assert(NULL != kpName);
    CHECKARGUMENTNULL(kpName, L"MgCoordinateSystemEnum.IsFilteredOut");

    //If a Filter hasn't been specified, just return false.
    if (m_vectFilter.empty())
    {
        //No Filter was specified.
        return false;
    }

    //Get a def from the set for the Filter to work with
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
//This function evaluates the provided def using the current
//Filter.  If there is no Filter, the result will always
//be true.
//
bool CCoordinateSystemEnum::IsFilteredOut(MgGuardDisposable *pDef)
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

//----------------------------------------------------------
//Creates a copy of the enumerator, with the same enumeration state.
//The bool returned is true for success; if out of memory,
//false; if some other error, E_UNEXPECTED.
//
MgCoordinateSystemEnum* CCoordinateSystemEnum::CreateClone()
{
    Ptr<CCoordinateSystemEnum> pNew;

    MG_TRY()

    //Make an object to be the copy
    pNew = new CCoordinateSystemEnum;

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnum.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy data members into the new object.  NOTE:  if new
    //data members are added this will
    //need to be updated.
    pNew->Initialize(m_pDict, m_pmapSystemNameDescription);
    pNew->m_iter = m_iter;
    for (size_t i=0; i<m_vectFilter.size(); i++)
    {
        pNew->m_vectFilter.push_back(m_vectFilter[i]);
        SAFE_ADDREF(m_vectFilter[i]);
    }

    pNew->m_readAllDefCallback = this->m_readAllDefCallback;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.CreateClone")

    return pNew.Detach();
}

//----------------------------------------------------------
//Sets the [m_readAllDefCallback] member; if set, the method
//will be invoked by [::Next(UINT32)]
void CCoordinateSystemEnum::SetReadAllDefinitionCallback(ReadAllDictionaryDefinitionsCallback callback)
{
    this->m_readAllDefCallback = callback;
}
