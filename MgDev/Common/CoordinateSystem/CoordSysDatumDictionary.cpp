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

#include "CoordSysDatum.h"              //for CCoordinateSystemDatum
#include "CoordSysEnum.h"               //for CCoordinateSystemEnum
#include "CoordSysEnumDatum.h"          //for CCoordinateSystemEnumDatum
#include "CoordSysDatumDictionary.h"    //for CCoordinateSystemDatumDictionary

#include "CoordSysUtil.h"               //for Convert_Wide_To_Ascii

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemDatumDictionary::CCoordinateSystemDatumDictionary(MgCoordinateSystemCatalog *pCatalog)
    : m_pmapSystemNameDescription(NULL) 
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

CCoordinateSystemDatumDictionary::~CCoordinateSystemDatumDictionary()
{
    m_sPath = L"";
    if (NULL != m_pmapSystemNameDescription)
    {
        m_pmapSystemNameDescription->clear();
        delete m_pmapSystemNameDescription; m_pmapSystemNameDescription = NULL;
    }
}

//-------------------------------------------------------------------------------
MgCoordinateSystemDatum* CCoordinateSystemDatumDictionary::NewDatum()
{
    Ptr<CCoordinateSystemDatum> pNewDef;

    MG_TRY()

    pNewDef = new CCoordinateSystemDatum(m_pCatalog);

    if (NULL == pNewDef.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatumDictionary.NewDatum", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatumDictionary.NewDatum")

    return pNewDef.Detach();
}

//-----------------------------------------------------------------------------
STRING CCoordinateSystemDatumDictionary::GetDefaultFileName()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.GetDefaultFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystemDatumDictionary::GetFileName()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.GetFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystemDatumDictionary::SetFileName(CREFSTRING sFileName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.SetFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

//------------------------------------------------------------------------
//Gets the path to the dictionary file, if currently open.
STRING CCoordinateSystemDatumDictionary::GetPath()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.GetPath", __LINE__, __WFILE__, NULL, L"", NULL);
}

//------------------------------------------------------------------------
//Gets the size of the set (number of defs it currently contains).
UINT32 CCoordinateSystemDatumDictionary::GetSize()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.GetSize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Adds the specified def to the set.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
//Adds a new datum definition to the set.  The supplied MgCoordinateSystemDatum
//object will be copied into the set, and can therefore be safely disposed
//of once the function returns.
//Throws an exception Duplicate (if there's already a definition in the set with that name),
//a ReadOnly (if the set isn't allowing write access-- for example, if this
//is an interface to a catalog that doesn't have the datum dictionary
//open for write).
void CCoordinateSystemDatumDictionary::Add(MgGuardDisposable *pDefinition)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.Add", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Removes the datum definition with the specified name from the
//set.
//Throws an exception (if there is
//no definition with that name in the catalog), ReadOnly (if the set
//isn't allowing write access).
void CCoordinateSystemDatumDictionary::Remove(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.Remove", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Modifies a datum definition in the set.  The existing definition
//of the same name will be replaced with a copy of the def pointed to by
//kpDef.
//Throws an exception (if there is no definition with that name
//in the catalog), a ReadOnly (if the set isn't allowing write access).
void CCoordinateSystemDatumDictionary::Modify(MgGuardDisposable *pDefinition)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.Modify", __LINE__, __WFILE__, NULL, L"", NULL);
}

//This function looks for a datum definition in the set with
//the specified name and, if found, creates an MgCoordinateSystemDatum and
//returns it (user is responsible for freeing the def via Release()
//function).
//Throws an exception (if no such definition exists in the catalog).
MgGuardDisposable* CCoordinateSystemDatumDictionary::Get(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.Get", __LINE__, __WFILE__, NULL, L"", NULL);
}

//This function looks for a datum definition in the set with
//the specified name and, if found, creates an MgCoordinateSystemDatum and
//returns it (user is responsible for freeing the def via Release()
//function).  
//Throws an exception (if no such definition exists in the catalog).
MgCoordinateSystemDatum* CCoordinateSystemDatumDictionary::GetDatum(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.Get", __LINE__, __WFILE__, NULL, L"", NULL);
}

//--------------------------------------------------------------
//Returns whether the set contains a def with the specified name.
bool CCoordinateSystemDatumDictionary::Has(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.Has", __LINE__, __WFILE__, NULL, L"", NULL);
}

//--------------------------------------------------------------
//Gets an enumerator for all the defs in the set.
MgCoordinateSystemEnum* CCoordinateSystemDatumDictionary::GetEnum()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatumDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"", NULL);
}

//MgDisposable
void CCoordinateSystemDatumDictionary::Dispose()
{
    delete this;
}

MgCoordinateSystemCatalog* CCoordinateSystemDatumDictionary::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}
