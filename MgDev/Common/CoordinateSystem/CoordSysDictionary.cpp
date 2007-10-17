//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
                            : m_pmapSystemNameDescription(NULL),
                            m_lMagic(0),
                            m_pCatalog(pCatalog)
{
    SAFE_ADDREF(m_pCatalog);
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
    m_lMagic = 0;

    SAFE_RELEASE(m_pCatalog);
}

//------------------------------------------------------------------------
MgCoordinateSystem* CCoordinateSystemDictionary::NewCoordinateSystem()
{
    CCoordinateSystem* pNewDef=NULL;
    MgCoordinateSystem* pDefinition=NULL;

    MG_TRY()

    pNewDef = new CCoordinateSystem(m_pCatalog);
    pDefinition=dynamic_cast<MgCoordinateSystem*>(pNewDef);
    if (!pDefinition)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDictionary.NewCoordinateSystem", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH(L"MgCoordinateSystemDictionary.NewCoordinateSystem")
    if (mgException != NULL)
    {
        delete pNewDef;
    }
    MG_THROW()

    return pDefinition;
}

//------------------------------------------------------------------------
void CCoordinateSystemDictionary::SetPath(CREFSTRING sPath)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDictionary.SetPath", __LINE__, __WFILE__, NULL, L"", NULL);
}

//------------------------------------------------------------------------
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
    MgCoordinateSystem *pDefinition=NULL;

    MG_TRY()
    //Get the name to search for
    char *pName = Convert_Wide_To_Ascii(sName.c_str()); //need to delete [] pName
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDictionary.Get", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
    }

    pDefinition=NewCoordinateSystem();
    if (!pDefinition)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDictionary.Get", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
    }
    CCoordinateSystem* pImp=dynamic_cast<CCoordinateSystem*>(pDefinition);
    assert(pImp);
    if (!pImp)
    {
        MgStringCollection arguments;
        arguments.Add(sName);
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemDictionary.Get", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // TODO - init the coordinate system - pDefinition

    MG_CATCH(L"MgCoordinateSystemDictionary.Get")
    if (mgException != NULL)
    {
        SAFE_RELEASE(pDefinition)
    }
    MG_THROW()

    return pDefinition;
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
    return SAFE_ADDREF(m_pCatalog);
}
