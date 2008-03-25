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

#include "CoordSys.h"           //for CCoordinateSystem
#include "CoordSysEnum.h"       //for CCoordinateSystemEnum
#include "CoordSysUtil.h"       //for Convert_Wide_To_Ascii, CsDictionaryOpenMode
#include "CoordSysDictionary.h" //for CCoordinateSystemDictionary

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemDictionary::CCoordinateSystemDictionary(MgCoordinateSystemCatalog *pCatalog)
                            : m_pmapSystemNameDescription(NULL)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemDictionary::~CCoordinateSystemDictionary()
{
    m_sPath = L"";
    if (NULL != m_pmapSystemNameDescription)
    {
        m_pmapSystemNameDescription->clear();
        delete m_pmapSystemNameDescription;
        m_pmapSystemNameDescription = NULL;
    }
}

//------------------------------------------------------------------------
MgCoordinateSystem* CCoordinateSystemDictionary::NewCoordinateSystem()
{
    Ptr<CCoordinateSystem> pNewDef;

    MG_TRY()

    pNewDef = new CCoordinateSystem(m_pCatalog);

    if (NULL == pNewDef.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDictionary.NewCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.NewCoordinateSystem")

    return pNewDef.Detach();
}

//------------------------------------------------------------------------
STRING CCoordinateSystemDictionary::GetDefaultFileName()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.GetDefaultFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystemDictionary::GetFileName()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.GetFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystemDictionary::SetFileName(CREFSTRING sFileName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.SetFileName", __LINE__, __WFILE__, NULL, L"", NULL);
}

//-----------------------------------------------------------------------------
//Gets the path to the dictionary file, if currently open.
STRING CCoordinateSystemDictionary::GetPath()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.GetPath", __LINE__, __WFILE__, NULL, L"", NULL);
}

//------------------------------------------------------------------------
UINT32 CCoordinateSystemDictionary::GetSize()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.GetSize", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Adds the specified def.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
//The supplied MgGuardDisposable object will be copied, and can therefore be safely disposed
//of once the function returns.
//Throws an exception Duplicate (if there's already a definition in the set with that name),
//a ReadOnly (if the set isn't allowing write access-- for example, if this
//is an interface to a catalog that doesn't have the coordsys dictionary
//open for write).
void CCoordinateSystemDictionary::Add(MgGuardDisposable *pDefinition)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.Add", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Removes the coordsys definition with the specified name from the
//set.
//Throws an exception if there is
//no definition with that name in the catalog, a ReadOnly if the set
//isn't allowing write access).
//
void CCoordinateSystemDictionary::Remove(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.Remove", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Modifies the specified def in the set.
//Throws an exception MgCoordinateSystemMismatchException if the
//def is not the right kind.
void CCoordinateSystemDictionary::Modify(MgGuardDisposable *pDefinition)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.Modify", __LINE__, __WFILE__, NULL, L"", NULL);
}

//This function looks for a coordsys definition in the set with
//the specified name and, if found, creates an MgCoordinateSystem and
//returns it (user is responsible for freeing the def via Release()
//function).
//Throws an exception if no such definition exists in the catalog.
MgGuardDisposable* CCoordinateSystemDictionary::Get(CREFSTRING sName)
{
    return GetCoordinateSystem(sName);
}

//This function looks for a coordsys definition in the set with
//the specified name and, if found, creates an MgCoordinateSystem and
//returns it (user is responsible for freeing the def via Release()
//function).
//Throws an exception if no such definition exists in the catalog.
MgCoordinateSystem* CCoordinateSystemDictionary::GetCoordinateSystem(CREFSTRING sName)
{
    Ptr<MgCoordinateSystem> pDefinition;

    MG_TRY()

    Ptr<MgCoordinateSystemFormatConverter> pConverter=m_pCatalog->GetFormatConverter();
    pDefinition=pConverter->CodeToDefinition(MgCoordinateSystemCodeFormat::Mentor, sName);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionary.GetCoordinateSystem")

    return pDefinition.Detach();
}

//Checks whether the named coordsys exists in the set.
bool CCoordinateSystemDictionary::Has(CREFSTRING sName)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.Has", __LINE__, __WFILE__, NULL, L"", NULL);
}

//--------------------------------------------------------------
//Gets an enumerator for all the defs
MgCoordinateSystemEnum* CCoordinateSystemDictionary::GetEnum()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.GetEnum", __LINE__, __WFILE__, NULL, L"", NULL);
}

//MgDisposable
void CCoordinateSystemDictionary::Dispose()
{
    delete this;
}

MgCoordinateSystemCatalog* CCoordinateSystemDictionary::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}
