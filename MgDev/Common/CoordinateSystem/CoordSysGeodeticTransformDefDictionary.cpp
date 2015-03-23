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

#include "CoordSysGeodeticTransformDef.h"
#include "CoordSysGeodeticTransformDefDictionary.h"

using namespace CSLibrary;

DEFINE_DICTIONARY_FILE_NAME(GeodeticTransformation, cs_GX_NAME);

extern DefinitionAccess<CCoordinateSystemGeodeticTransformDef, cs_GeodeticTransform_> transformDefinitionAccess(
    CS_gxdef,
    CCoordinateSystemGeodeticTransformDefDictionary::UpdateGeodeticTransformDef, /* CS_gpupd has a different signature than the other CS 'update' methods */
    CS_gxdel,

    &CCoordinateSystemGeodeticTransformDef::IsValid,
    CCoordinateSystemGeodeticTransformDefDictionary::SetupCsStructFromMgTransformDef,
    CCoordinateSystemGeodeticTransformDefDictionary::FullInitialize,

    CCoordinateSystemGeodeticTransformDefDictionary::ReadName,
    CCoordinateSystemGeodeticTransformDefDictionary::ReadDescription,
    CS_gxdefAll,

    CS_gxfnm,

    CCoordinateSystemGeodeticTransformDefDictionary::GetFileOpenMode);

CCoordinateSystemGeodeticTransformDefDictionary::CCoordinateSystemGeodeticTransformDefDictionary(MgCoordinateSystemCatalog *pCatalog)
    : m_pDictionary(NULL)
{
    this->m_pDictionary = new CCoordinateSystemDictionaryBase<CCoordinateSystemGeodeticTransformDef, cs_GeodeticTransform_,
        &transformDefinitionAccess, true, false, cs_GXDEF_MAGIC, DICTIONARY_FILE_NAME(GeodeticTransformation)>(pCatalog);
}

CCoordinateSystemGeodeticTransformDefDictionary::~CCoordinateSystemGeodeticTransformDefDictionary()
{
    SAFE_RELEASE(this->m_pDictionary)
}


void CCoordinateSystemGeodeticTransformDefDictionary::Dispose()
{
    delete this;
}

MgCoordinateSystemGeodeticTransformDef* CCoordinateSystemGeodeticTransformDefDictionary::NewGeodeticTransformationDef(INT32 transformationDefType)
{
    Ptr<CCoordinateSystemGeodeticTransformDef> newTransformDef = this->m_pDictionary->NewItem();
    newTransformDef->Reset(transformationDefType); //sets up the [cs_geodeticTransfrom_] struct and initializes it to [transformationDefType]

    return newTransformDef.Detach(); //this instance is not yet initialized
}

MgCoordinateSystemGeodeticTransformDef* CCoordinateSystemGeodeticTransformDefDictionary::GetGeodeticTransformationDef(CREFSTRING transformationName)
{
    try
    {
        return static_cast<MgCoordinateSystemGeodeticTransformDef*>(this->m_pDictionary->Get(transformationName));
    }
    catch(MgCoordinateSystemLoadFailedException* loadFailedException)
    {
        //catch only the [MgCoordinateSystemLoadFailedException] and release it right away
        loadFailedException->Release();
    }

    return NULL;
}

/******************************************************************************************************/
/* BEGIN -------------------- MgCoordinateSystemDicationary implementation -------------------- BEGIN */
/******************************************************************************************************/
MgGuardDisposable* CCoordinateSystemGeodeticTransformDefDictionary::Get(CREFSTRING sName)
{
    return this->m_pDictionary->Get(sName);
}

MgCoordinateSystemEnum* CCoordinateSystemGeodeticTransformDefDictionary::GetEnum()
{
    return this->m_pDictionary->GetEnum();
}

bool CCoordinateSystemGeodeticTransformDefDictionary::Has(CREFSTRING sName)
{
    return this->m_pDictionary->Has(sName);
}

STRING CCoordinateSystemGeodeticTransformDefDictionary::GetPath()
{
    return this->m_pDictionary->GetPath();
}

UINT32 CCoordinateSystemGeodeticTransformDefDictionary::GetSize()
{
    return this->m_pDictionary->GetSize();
}

MgCoordinateSystemCatalog* CCoordinateSystemGeodeticTransformDefDictionary::GetCatalog()
{
    return this->m_pDictionary->GetCatalog();
}

STRING CCoordinateSystemGeodeticTransformDefDictionary::GetDefaultFileName()
{
    return this->m_pDictionary->GetDefaultFileName();
}

STRING CCoordinateSystemGeodeticTransformDefDictionary::GetFileName()
{
    return this->m_pDictionary->GetFileName();
}

void CCoordinateSystemGeodeticTransformDefDictionary::SetFileName(CREFSTRING sFileName)
{
    this->m_pDictionary->SetFileName(sFileName);
}

void CCoordinateSystemGeodeticTransformDefDictionary::Add(MgGuardDisposable *pDefinition)
{
    this->m_pDictionary->Add(pDefinition);
}

void CCoordinateSystemGeodeticTransformDefDictionary::Remove(CREFSTRING sName)
{
    this->m_pDictionary->Remove(sName);
}

void CCoordinateSystemGeodeticTransformDefDictionary::Modify(MgGuardDisposable *pDefinition)
{
    this->m_pDictionary->Modify(pDefinition);
}

/******************************************************************************************************/
/* END -------------------- MgCoordinateSystemDicationary implementation -------------------- END */
/******************************************************************************************************/

/*********************************************************************************************************/
/* BEGIN -------------------- CCoordinateSystemDictionaryBase support methods -------------------- BEGIN */
int CCoordinateSystemGeodeticTransformDefDictionary::UpdateGeodeticTransformDef(cs_GeodeticTransform_* csTransformDef, int /*unused*/)
{
    return CS_gxupd(csTransformDef);
}

bool CCoordinateSystemGeodeticTransformDefDictionary::SetupCsStructFromMgTransformDef(CCoordinateSystemGeodeticTransformDef* mgGeodeticTransformDef, cs_GeodeticTransform_& csTransformDef)
{
    ENSURE_NOT_NULL(mgGeodeticTransformDef, L"CCoordinateSystemGeodeticTransformDefDictionary.SetupCsStructFromMgTransformDef");

    //copy the struct values from [mgGeodeticTransformDef] to [csTransformDef]; will throw an exception, if [mgGeodeticTransformDef] hasn't been yet initialized
    mgGeodeticTransformDef->CopyTo(csTransformDef);
    return true;
}

void CCoordinateSystemGeodeticTransformDefDictionary::FullInitialize(CCoordinateSystemGeodeticTransformDef* mgGeodeticTransformDef,
                                                                     cs_GeodeticTransform_* csTransformDef,
                                                                     MgCoordinateSystemCatalog* catalog)
{
    ENSURE_NOT_NULL(mgGeodeticTransformDef, L"CCoordinateSystemGeodeticTransformDefDictionary.FullInitialize");
    ENSURE_NOT_NULL(csTransformDef, L"CCoordinateSystemGeodeticTransformDefDictionary.FullInitialize");

    mgGeodeticTransformDef->Initialize(*csTransformDef);

    return;
}

const char* CCoordinateSystemGeodeticTransformDefDictionary::ReadName(const cs_GeodeticTransform_& definition)
{
    return definition.xfrmName;
}

const char* CCoordinateSystemGeodeticTransformDefDictionary::ReadDescription(const cs_GeodeticTransform_& definition)
{
    return definition.description;
}

CsDictionaryOpenMode CCoordinateSystemGeodeticTransformDefDictionary::GetFileOpenMode(long magicNumberFromFile)
{
    if (cs_GXDEF_MAGIC == magicNumberFromFile)
        return Write;

    _ASSERT(false);
    return Closed;
}

/* END -------------------- CCoordinateSystemDictionaryBase support methods -------------------- END */
/*****************************************************************************************************/


#include "CoordSysDictionaryBase.cpp"
