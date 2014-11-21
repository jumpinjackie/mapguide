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

#include "CoordSysMacro.h"

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CriticalSection.h"

#include "CoordSysUtil.h"               //for Convert_Wide_To_UTF8
#include "MentorUtil.h"                 //for OpenDictionaryFile()
#include "MentorDictionary.h"

#include <cs_map.h>

#include "CoordSysEnum.h"
#include "CoordSysDictionaryBase.h"

#include "CoordSysGeodeticPath.h"
#include "CoordSysGeodeticPathDictionary.h"

using namespace CSLibrary;

DEFINE_DICTIONARY_FILE_NAME(GeodeticPath, cs_GP_NAME);

extern DefinitionAccess<CCoordinateSystemGeodeticPath, cs_GeodeticPath_> definitionAccess(
    CS_gpdef,
    CCoordinateSystemGeodeticPathDictionary::UpdateGeodeticPath, /* CS_gpupd has a different signature than the other CS 'update' methods */
    CS_gpdel,

    &CCoordinateSystemGeodeticPath::IsValid,
    CCoordinateSystemGeodeticPathDictionary::SetupCsGeodeticPathStruct,
    CCoordinateSystemGeodeticPathDictionary::FullInitialize,

    CCoordinateSystemGeodeticPathDictionary::ReadName,
    CCoordinateSystemGeodeticPathDictionary::ReadDescription,

    CS_gpdefAll,

    CS_gpfnm,

    CCoordinateSystemGeodeticPathDictionary::GetFileOpenMode);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemGeodeticPathDictionary::CCoordinateSystemGeodeticPathDictionary(MgCoordinateSystemCatalog *pCatalog)
    : m_pDictionary(NULL)
{
    this->m_pDictionary = new CCoordinateSystemDictionaryBase<CCoordinateSystemGeodeticPath, cs_GeodeticPath_,
        &definitionAccess, true, true, cs_GPDEF_MAGIC, DICTIONARY_FILE_NAME(GeodeticPath)>(pCatalog);
}

CCoordinateSystemGeodeticPathDictionary::~CCoordinateSystemGeodeticPathDictionary()
{
    SAFE_RELEASE(this->m_pDictionary)
}

const char* CCoordinateSystemGeodeticPathDictionary::ReadName(const cs_GeodeticPath_& definition)
{
    return definition.pathName;
}

const char* CCoordinateSystemGeodeticPathDictionary::ReadDescription(const cs_GeodeticPath_& definition)
{
    return definition.description;
}

CsDictionaryOpenMode CCoordinateSystemGeodeticPathDictionary::GetFileOpenMode(long magicNumberFromFile)
{
    if (cs_GPDEF_MAGIC == magicNumberFromFile)
        return Write;

    _ASSERT(false);
    return Closed;
}

int CCoordinateSystemGeodeticPathDictionary::UpdateGeodeticPath(cs_GeodeticPath_* csPath, int /*unused*/)
{
    return CS_gpupd(csPath);
}

bool CCoordinateSystemGeodeticPathDictionary::SetupCsGeodeticPathStruct(CCoordinateSystemGeodeticPath* mgGeodeticPath, cs_GeodeticPath_& csPath)
{
    _ASSERT(NULL != mgGeodeticPath);

    //copy the values from [mgGeodeticPath] to [csPath]; this will throw an exception in case [mgGeodeticPath] hasn't been initialized yet
    mgGeodeticPath->CopyTo(csPath);

    return true;
}

void CCoordinateSystemGeodeticPathDictionary::FullInitialize(CCoordinateSystemGeodeticPath* mgGeodeticPath, cs_GeodeticPath_* csPath, MgCoordinateSystemCatalog* catalog)
{
    ENSURE_NOT_NULL(mgGeodeticPath, L"CCoordinateSystemGeodeticPathDictionary.FullInitialize");
    ENSURE_NOT_NULL(csPath, L"CCoordinateSystemGeodeticPathDictionary.FullInitialize");

    _ASSERT(NULL != catalog);

    mgGeodeticPath->Initialize(*csPath);
}

//-------------------------------------------------------------------------------
MgCoordinateSystemGeodeticPath* CCoordinateSystemGeodeticPathDictionary::NewGeodeticPath()
{
    Ptr<CCoordinateSystemGeodeticPath> newPathItem = this->m_pDictionary->NewItem();
    newPathItem->Reset(); //sets up the [cs_geodeticpath_] struct; it can thus be used by

    return newPathItem.Detach();
}

//-----------------------------------------------------------------------------
STRING CCoordinateSystemGeodeticPathDictionary::GetDefaultFileName()
{
    return this->m_pDictionary->GetDefaultFileName();
}

//-----------------------------------------------------------------------------
//Gets the file name of the dictionary file.
STRING CCoordinateSystemGeodeticPathDictionary::GetFileName()
{
    return this->m_pDictionary->GetFileName();
}

//-----------------------------------------------------------------------------
void CCoordinateSystemGeodeticPathDictionary::SetFileName(CREFSTRING sFileName)
{
    this->m_pDictionary->SetFileName(sFileName);
}

//------------------------------------------------------------------------
//Gets the path to the dictionary file, if currently open.
STRING CCoordinateSystemGeodeticPathDictionary::GetPath()
{
    return this->m_pDictionary->GetPath();
}

//------------------------------------------------------------------------
//Gets the size of the set (number of defs it currently contains).
UINT32 CCoordinateSystemGeodeticPathDictionary::GetSize()
{
    return this->m_pDictionary->GetSize();
}

void CCoordinateSystemGeodeticPathDictionary::Add(MgGuardDisposable *pDefinition)
{
    this->m_pDictionary->Add(pDefinition);
}

void CCoordinateSystemGeodeticPathDictionary::Remove(CREFSTRING sName)
{
    this->m_pDictionary->Remove(sName);
}

void CCoordinateSystemGeodeticPathDictionary::Modify(MgGuardDisposable *pDefinition)
{
    this->m_pDictionary->Modify(pDefinition);
}

MgGuardDisposable* CCoordinateSystemGeodeticPathDictionary::Get(CREFSTRING sName)
{
    return this->m_pDictionary->Get(sName);
}

MgCoordinateSystemGeodeticPath* CCoordinateSystemGeodeticPathDictionary::GetGeodeticPath(CREFSTRING sName)
{
    try
    {
        return static_cast<MgCoordinateSystemGeodeticPath*>(this->m_pDictionary->Get(sName));
    }
    catch(MgCoordinateSystemLoadFailedException* loadFailedException)
    {
        //catch only the [MgCoordinateSystemLoadFailedException] and release it right away
        loadFailedException->Release();
    }

    return NULL;
}

//--------------------------------------------------------------
//Returns whether the set contains a def with the specified name.
bool CCoordinateSystemGeodeticPathDictionary::Has(CREFSTRING sName)
{
    return this->m_pDictionary->Has(sName);
}

//--------------------------------------------------------------
//Gets an enumerator for all the defs in the set.
MgCoordinateSystemEnum* CCoordinateSystemGeodeticPathDictionary::GetEnum()
{
    return this->m_pDictionary->GetEnum();
}

//MgDisposable
void CCoordinateSystemGeodeticPathDictionary::Dispose()
{
    delete this;
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemGeodeticPathDictionary::GetCatalog()
{
    return this->m_pDictionary->GetCatalog();
}

#include "CoordSysDictionaryBase.cpp"
