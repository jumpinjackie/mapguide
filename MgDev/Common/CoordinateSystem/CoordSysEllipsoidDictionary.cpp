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

#include "CoordSysEllipsoid.h"              //for CCoordinateSystemEllipsoid
#include "CoordSysEnum.h"                   //for CCoordinateSystemEnum
#include "CoordSysEnumEllipsoid.h"          //for CCoordinateSystemEnumEllipsoid
#include "CoordSysEllipsoidDictionary.h"    //for CCoordinateSystemEllipsoidDictionary

#include "CoordSysUtil.h"                   //for Convert_Wide_To_Ascii

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemEllipsoidDictionary::CCoordinateSystemEllipsoidDictionary(MgCoordinateSystemCatalog *pCatalog)
    : m_pmapSystemNameDescription(NULL)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

//Destructor.  Closes the dictionary, if open.
CCoordinateSystemEllipsoidDictionary::~CCoordinateSystemEllipsoidDictionary()
{
    m_sPath = L"";
    if (NULL != m_pmapSystemNameDescription)
    {
        m_pmapSystemNameDescription->clear();
        delete m_pmapSystemNameDescription; m_pmapSystemNameDescription = NULL;
    }
}

//-----------------------------------------------------------------------------
MgCoordinateSystemEllipsoid* CCoordinateSystemEllipsoidDictionary::NewEllipsoid()
{
    Ptr<CCoordinateSystemEllipsoid> pNewDef;

    MG_TRY()

    pNewDef = new CCoordinateSystemEllipsoid();

    if (NULL == pNewDef.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEllipsoidDictionary.NewEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoidDictionary.NewEllipsoid")

    return pNewDef.Detach();
}

//-----------------------------------------------------------------------------
STRING CCoordinateSystemEllipsoidDictionary::GetDefaultFileName()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.GetDefaultFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystemEllipsoidDictionary::GetFileName()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.GetFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystemEllipsoidDictionary::SetFileName(CREFSTRING sFileName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.SetFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

//------------------------------------------------------------------------
//Gets the path to the dictionary file, if currently open.
STRING CCoordinateSystemEllipsoidDictionary::GetPath()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.GetPath", __LINE__, __WFILE__, NULL, L"", NULL);
}

//------------------------------------------------------------------------
//Gets the size of the set (number of defs it currently contains).
UINT32 CCoordinateSystemEllipsoidDictionary::GetSize()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.GetSize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Adds the specified def to the set.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
//The supplied MgCoordinateSystemEllipsoid
//object will be copied into the set, and can therefore be safely disposed
//of once the function returns.
//Throws an exception Duplicate (if there's already a definition in the set with that name),
//ReadOnly (if the set isn't allowing write access-- for example, if this
//is an interface to a catalog that doesn't have the ellipsoid dictionary
//open for write).
void CCoordinateSystemEllipsoidDictionary::Add(MgGuardDisposable *pDefinition)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.Add", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Removes the ellipsoid definition with the specified name from the
//set.
//Throws an exception if there is
//no definition with that name in the catalog, ReadOnly if the set
//isn't allowing write access.
void CCoordinateSystemEllipsoidDictionary::Remove(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.Remove", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Modifies the specified def in the set.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.  Otherwise, works like ModifyEllipsoid().
//Modifies an ellipsoid definition in the set.  The existing definition
//of the same name will be replaced with a copy of the def pointed to by
//kpDef.
//Throws an exception if there is no definition with that name
//in the catalog, ReadOnly if the set isn't allowing write access.
void CCoordinateSystemEllipsoidDictionary::Modify(MgGuardDisposable *pDefinition)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.Modify", __LINE__, __WFILE__, NULL, L"", NULL);
}

//This function looks for an ellipsoid definition in the set with
//the specified name and, if found, creates an MgCoordinateSystemEllipsoid and
//returns it (user is responsible for freeing the def via Release()
//function).
//Throws an exception if no such definition exists in the catalog.
MgGuardDisposable* CCoordinateSystemEllipsoidDictionary::Get(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.Get", __LINE__, __WFILE__, NULL, L"", NULL);
}

//This function looks for an ellipsoid definition in the set with
//the specified name and, if found, creates an MgCoordinateSystemEllipsoid and
//returns it (user is responsible for freeing the def via Release()
//function).
//Throws an exception if no such definition exists in the catalog.
MgCoordinateSystemEllipsoid* CCoordinateSystemEllipsoidDictionary::GetEllipsoid(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.Get", __LINE__, __WFILE__, NULL, L"", NULL);
}

//--------------------------------------------------------------
//Returns whether the set contains a def with the specified name.
bool CCoordinateSystemEllipsoidDictionary::Has(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.Has", __LINE__, __WFILE__, NULL, L"", NULL);
}

//--------------------------------------------------------------
//Gets an enumerator for all the defs in the set.
MgCoordinateSystemEnum* CCoordinateSystemEllipsoidDictionary::GetEnum()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoidDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"", NULL);
}

//MgDisposable
void CCoordinateSystemEllipsoidDictionary::Dispose()
{
    delete this;
}

MgCoordinateSystemCatalog* CCoordinateSystemEllipsoidDictionary::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}
