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

#include "CoordSysEnumInteger32.h"  //for CCoordinateSystemEnumInteger32
#include "CoordSysUtil.h"           //for CsDictionaryOpenMode
#include "MentorUtil.h"             //for IsLegalMentorName()

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemEnumInteger32::CCoordinateSystemEnumInteger32() :
    m_pnValues(NULL),
    m_ulSize(0),
    m_ulPos(0)
{
}

CCoordinateSystemEnumInteger32::~CCoordinateSystemEnumInteger32()
{
    this->ClearFilter();

    delete[] this->m_pnValues;
    this->m_pnValues = NULL;

    this->m_ulSize = 0;
    this->m_ulPos = 0;
}

//MgDisposable
void CCoordinateSystemEnumInteger32::Dispose()
{
    delete this;
}

//Sets the internally stored list of unsigned longs to equal the provided array.
//Since this is only a class method, not an interface method, it will be
//called only internally to this module, so only assert-level checking
//is done on the input paramters.  The function returns true for success
//or false for failure.  (It will fail only if it runs out of memory).
//
void CCoordinateSystemEnumInteger32::SetList(INT32 *pnValues, UINT32 ulSize)
{
    MG_TRY()
    assert((NULL != pnValues) || (ulSize == 0));
    CHECKARGUMENTNULL(pnValues, L"MgCoordinateSystemEnumInteger32.SetList");

    delete [] m_pnValues;
    m_pnValues = NULL;

    m_ulPos = 0;
    m_ulSize = 0;

    //Allocate an array to hold the values
    m_pnValues = new INT32[ulSize];
    if (NULL == m_pnValues)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnumInteger32.SetList", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy them in and return success
    memcpy(m_pnValues, pnValues, ulSize * sizeof(INT32));
    m_ulSize = ulSize;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnumInteger32.SetList")
}

//----------------------------------------------------------
//Set the filter.
void CCoordinateSystemEnumInteger32::AddFilter(MgCoordinateSystemFilterInteger32 *pFilter)
{
    MG_TRY()
    ClearFilter();
    m_vectFilter.push_back(pFilter);
    SAFE_ADDREF(pFilter);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnumInteger32.SetFilter")
}

//This function clears the Filter, if one is set.
//
void CCoordinateSystemEnumInteger32::ClearFilter()
{
    for (size_t i=0; i<m_vectFilter.size(); i++)
    {
        SAFE_RELEASE(m_vectFilter[i]);
    }
    m_vectFilter.clear();
}

//----------------------------------------------------------
/*
MgIntCollection* CCoordinateSystemEnumInteger32::Next(UINT32 ulCount)
{
    Ptr<MgIntCollection> pOutput;

    MG_TRY()
    pOutput=new MgIntCollection;
    if (!pOutput)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnumInteger32.Next", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    for (; m_ulPos < m_ulSize; m_ulPos++)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }

        //is it filtered out?
        if (IsFilteredOut(m_pnValues[m_ulPos]))
        {
            continue;
        }
        pOutput->Add(m_pnValues[m_ulPos]);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnumInteger32.Next")
    return pOutput.Detach();
}
*/

MgStringCollection* CCoordinateSystemEnumInteger32::Next(UINT32 ulCount)
{
    Ptr<MgStringCollection> pOutput;

    MG_TRY()
    pOutput=new MgStringCollection;
    if (!pOutput)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnumInteger32.Next", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    for (; m_ulPos < m_ulSize; m_ulPos++)
    {
        if (pOutput->GetCount() == ulCount)
        {
            //success
            return pOutput.Detach();
        }

        //is it filtered out?
        if (IsFilteredOut(m_pnValues[m_ulPos]))
        {
            continue;
        }
        wchar_t wszINT32[255];
        #ifdef _WIN32
        _itow(m_pnValues[m_ulPos], wszINT32, 10);
        #else
        swprintf(wszINT32, 255, L"%d", m_pnValues[m_ulPos]);
        #endif

        pOutput->Add(wszINT32);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnumInteger32.Next")
    return pOutput.Detach();
}

//Skips the next ulSkipCount names.
//Throws an exception if
//ulSkipCount items were not skipped
//
void CCoordinateSystemEnumInteger32::Skip(UINT32 ulSkipCount)
{
    MG_TRY()
    UINT32 ulSkipped;
    for (ulSkipped=0; m_ulPos < m_ulSize; m_ulPos++)
    {
        if (ulSkipped == ulSkipCount)
        {
            //success
            return;
        }
        if (IsFilteredOut(m_pnValues[m_ulPos]))
        {
            continue;
        }
        ulSkipped++;
    }
    if (ulSkipCount != ulSkipped)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEnumInteger32.Skip", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnumInteger32.Skip")
}

//Resets the enumerator to the start of the collection.
//
void CCoordinateSystemEnumInteger32::Reset()
{
    m_ulPos = 0;
}

//This function evaluates the provided def using the current
//Filter.  If there is no Filter, the result will always
//be true.
//
bool CCoordinateSystemEnumInteger32::IsFilteredOut(INT32 nValue)
{
    MG_TRY()
    //If a Filter hasn't been specified, just return true.
    if (m_vectFilter.empty())
    {
        //No Filter was specified.
        return false;
    }

    for (size_t i=0; i<m_vectFilter.size(); i++)
    {
        if (m_vectFilter[i]->IsFilteredOut(nValue))
        {
            return true;
        }
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnumInteger32.IsFilteredOut")
    //No Filter has been set.
    return false;
}

//Creates a copy of the enumerator, with the same enumeration state.
//The bool returned is true for success; if out of memory,
//false; if some other error, E_UNEXPECTED.
//
MgCoordinateSystemEnumInteger32* CCoordinateSystemEnumInteger32::CreateClone()
{
    Ptr<CCoordinateSystemEnumInteger32> pNew;

    MG_TRY()

    //Make an object to be the copy
    pNew = new CCoordinateSystemEnumInteger32;

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEnumInteger32.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy data members into the new object.  NOTE:  if new
    //data members are added this will
    //need to be updated.
    pNew->SetList(m_pnValues, m_ulSize);
    pNew->m_ulPos = m_ulPos;
    for (size_t i=0; i<m_vectFilter.size(); i++)
    {
        pNew->m_vectFilter.push_back(m_vectFilter[i]);
        SAFE_ADDREF(m_vectFilter[i]);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnumInteger32.CreateClone")

    return pNew.Detach();
}
