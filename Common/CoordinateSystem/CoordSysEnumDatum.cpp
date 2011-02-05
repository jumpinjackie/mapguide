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
#include "CoordSysEnumDatum.h"      //for CCoordinateSystemEnumDatum
#include "CoordSysUtil.h"           //for CsDictionaryOpenMode
#include "MentorUtil.h"             //for IsLegalMentorName()

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemEnumDatum::CCoordinateSystemEnumDatum()
{
}

CCoordinateSystemEnumDatum::~CCoordinateSystemEnumDatum()
{
}

//Creates a copy of the enumerator, with the same enumeration state.
//The bool returned is true for success; if out of memory,
//false; if some other error, E_UNEXPECTED.
//
MgCoordinateSystemEnum* CCoordinateSystemEnumDatum::CreateClone()
{
    Ptr<CCoordinateSystemEnumDatum> pNew;

    MG_TRY()

    //Make an object to be the copy
    pNew = new CCoordinateSystemEnumDatum;

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

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEnum.CreateClone")

    return pNew.Detach();
}
