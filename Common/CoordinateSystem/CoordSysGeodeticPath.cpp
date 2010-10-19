//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "CriticalSection.h"

#include "CoordSysGeodeticPathElement.h"
#include "CoordSysGeodeticPath.h"

#include "CoordSysTransform.h"          //for CCoordinateSystemTransform
#include "CoordSysUtil.h"               //for CsDictionaryOpenMode
#include "MentorDictionary.h"

using namespace CSLibrary;

#define CS_MAP_DEF_VARIABLE this->pathDefinition //needed by CoordSysMacro

#include "CoordSysMacro.h" //for DEFINE_GET_SET_STRING and DEFINE_GET_SET_NUMERIC

CCoordinateSystemGeodeticPath::CCoordinateSystemGeodeticPath(MgCoordinateSystemCatalog* pCatalog)
    : pathDefinition(NULL), catalog(SAFE_ADDREF(pCatalog))
{
    if (NULL == pCatalog)
        throw new MgNullArgumentException(L"CCoordinateSystemGeodeticPath.ctor", __LINE__, __WFILE__, NULL, L"", NULL);
}

CCoordinateSystemGeodeticPath::~CCoordinateSystemGeodeticPath()
{
    this->CleanupInstanceVariables();
}

void CCoordinateSystemGeodeticPath::Dispose()
{
    delete this;
}

void CCoordinateSystemGeodeticPath::CleanupInstanceVariables()
{
    if (NULL != this->pathDefinition)
    {
        CS_free(this->pathDefinition);
        this->pathDefinition = NULL;
    }
}

void CCoordinateSystemGeodeticPath::Reset()
{
    this->CleanupInstanceVariables();
    this->pathDefinition = (cs_GeodeticPath_*)CS_malc(sizeof(cs_GeodeticPath_));

    if (NULL == this->pathDefinition)
        throw new MgOutOfMemoryException(L"CCoordinateSystemGeodeticPath.Initialize", __LINE__, __WFILE__, NULL, L"", NULL);

    memset ((void*)this->pathDefinition, 0, sizeof(cs_GeodeticPath_));
}

void CCoordinateSystemGeodeticPath::Initialize(const cs_GeodeticPath_& pathDef)
{
    this->Reset();

    *this->pathDefinition = pathDef;
}

MgCoordinateSystemGeodeticPath* CCoordinateSystemGeodeticPath::CreateClone()
{
    VERIFY_INITIALIZED(L"CCoordinateSystemGeodeticPath.CreateClone");

    Ptr<CCoordinateSystemGeodeticPath> clonedPath = new CCoordinateSystemGeodeticPath(this->catalog);
    clonedPath->Initialize(*this->pathDefinition);

    //unset the EPSG code; we've an editable instance now where we neither can guarantee the correctness of the EPSG code
    //nor is it currently supported by CsMap's NameMapper anyway
    clonedPath->pathDefinition->epsgCode = 0;
    clonedPath->pathDefinition->protect = 0; //unset the protection flag; otherwise the caller wouldn't be able to change any values

    return clonedPath.Detach();
}

MgCoordinateSystemGeodeticPathElement* CCoordinateSystemGeodeticPath::NewPathElement()
{
    return new CCoordinateSystemGeodeticPathElement(NULL, false);
}

void CCoordinateSystemGeodeticPath::CopyTo(cs_GeodeticPath_& pathDef) const
{
    VERIFY_INITIALIZED(L"CCoordinateSystemGeodeticPath.CopyTo");
    pathDef = *this->pathDefinition;
}

//helper - don't delete; will be called by MentorUtil / DictionaryUtil
bool CCoordinateSystemGeodeticPath::IsEncrypted()
{
    return false;
}

bool CCoordinateSystemGeodeticPath::IsProtected()
{
    VERIFY_INITIALIZED(L"CCoordinateSystemGeodeticPath.IsProtected");
    return DICTIONARY_SYS_DEF == this->pathDefinition->protect;
}


bool CCoordinateSystemGeodeticPath::IsValid()
{
    if (NULL == this->pathDefinition)
        return false;

    //make sure, the names for this path and the datums are set
    size_t pathNameLength = strlen(this->pathDefinition->pathName);
    if (0 == pathNameLength || pathNameLength >= sizeof(this->pathDefinition->pathName))
        return false;

    size_t srcDatumNameLength = strlen(this->pathDefinition->srcDatum);
    if (0 == srcDatumNameLength || srcDatumNameLength >= sizeof(this->pathDefinition->srcDatum))
        return false;

    size_t trgDatumNameLength = strlen(this->pathDefinition->trgDatum);
    if (0 == trgDatumNameLength || trgDatumNameLength >= sizeof(this->pathDefinition->trgDatum))
        return false;

    //check, whether there are only valid path elements according to MgCoordinateSystemGeodeticPathElement::IsValid()
    Ptr<MgDisposableCollection> pathElements = this->GetPathElements();
    INT32 pathElementCount = pathElements->GetCount();
    if (0 == pathElementCount || pathElementCount > csPATH_MAXXFRM)
        return false;

    if (pathElementCount != this->pathDefinition->elementCount)
        return false;

    for(INT32 i = 0; i < pathElementCount; i++)
    {
        Ptr<MgDisposable> mgDisposable = pathElements->GetItem(i);
        MgCoordinateSystemGeodeticPathElement* pathElement = dynamic_cast<MgCoordinateSystemGeodeticPathElement*>(mgDisposable.p);
        if (NULL == pathElement)
            throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticPath.IsValid", __LINE__, __WFILE__, NULL, L"", NULL);

        if (!pathElement->IsValid())
            return false;
    }

    //TODO: add gp_gpchk in cs_map.h
    return true; //for any other stuff, like [accuracy] etc. rely on cs_gpcheck
}

//all getters and setters

MgDisposableCollection* CCoordinateSystemGeodeticPath::GetPathElements()
{
    VERIFY_INITIALIZED(L"CCoordinateSystemGeodeticPath.GetPathElements");

    Ptr<MgDisposableCollection> pathElements = new MgDisposableCollection();

    const INT32 pathElementCount = this->pathDefinition->elementCount;
    for(int i = 0; (i < pathElementCount && i <= csPATH_MAXXFRM); i++)
    {
        const cs_GeodeticPathElement_ pathElement = this->pathDefinition->geodeticPathElements[i];

        Ptr<CCoordinateSystemGeodeticPathElement> mgPathElement =
            new CCoordinateSystemGeodeticPathElement(&pathElement, this->IsProtected());

        //the elements will be released when the collection is released
        pathElements->Add(mgPathElement);
    }

    return pathElements.Detach();
}

void CCoordinateSystemGeodeticPath::SetPathElements(MgDisposableCollection* pathElements)
{
    VERIFY_INITIALIZED(L"CCoordinateSystemGeodeticPath.SetPathElements");
    VERIFY_NOT_PROTECTED(L"CCoordinateSystemGeodeticPath.SetPathElements");

    ENSURE_NOT_NULL(pathElements, L"CCoordinateSystemGeodeticPath.SetPathElements");

    const INT32 pathElementCount = pathElements->GetCount();
    if (0 == pathElementCount || pathElementCount > csPATH_MAXXFRM)
        throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticPath.SetPathElements", __LINE__, __WFILE__, NULL, L"", NULL);

    cs_GeodeticPathElement_* validElements = new cs_GeodeticPathElement_[pathElementCount];

    MG_TRY()

    memset(validElements, 0, pathElementCount * sizeof(cs_GeodeticPathElement_));

    //we don't take ownership over the elements
    for(int i = 0; i < pathElementCount; i++)
    {
        Ptr<MgDisposable> collectionItem = pathElements->GetItem(i);
        MgCoordinateSystemGeodeticPathElement* pathElement = dynamic_cast<MgCoordinateSystemGeodeticPathElement*>(collectionItem.p);
        if (NULL == pathElement)
            throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticPath.SetPathElements", __LINE__, __WFILE__, NULL, L"", NULL);

        cs_GeodeticPathElement_& pathElementDef = validElements[i];

        //set the name of the transformation on the temporary [pathElementDef] we'll later copy into our own [this->pathDefinition]
        MentorSetString(pathElement->GetTransformName(),
            pathElementDef.geodeticXformName /* target buffer */, sizeof(pathElementDef.geodeticXformName));

        pathElementDef.direction = pathElement->GetIsInversed() ? cs_PATHDIR_INV : cs_PATHDIR_FWD;
    }

    //wipe out the current path definition settings...
    memset(this->pathDefinition->geodeticPathElements, 0, csPATH_MAXXFRM * sizeof(cs_GeodeticPathElement_));
    //...copy the new values over...
    memcpy(this->pathDefinition->geodeticPathElements, validElements, pathElementCount * sizeof(cs_GeodeticPathElement_));
    //...and set the number of elements contained in the struct
    this->pathDefinition->elementCount = pathElementCount;

    MG_CATCH(L"CCoordinateSystemGeodeticPath.SetPathElements")

    delete[] validElements;

    MG_THROW();
}

DEFINE_GET_SET_STRING(CCoordinateSystemGeodeticPath,PathName,this->pathDefinition->pathName)
DEFINE_GET_SET_STRING(CCoordinateSystemGeodeticPath,Description,this->pathDefinition->description)
DEFINE_GET_SET_STRING(CCoordinateSystemGeodeticPath,Group,this->pathDefinition->group)
DEFINE_GET_SET_STRING(CCoordinateSystemGeodeticPath,Source,this->pathDefinition->source)
DEFINE_GET_SET_STRING(CCoordinateSystemGeodeticPath,SourceDatum,this->pathDefinition->srcDatum)
DEFINE_GET_SET_STRING(CCoordinateSystemGeodeticPath,TargetDatum,this->pathDefinition->trgDatum)

DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticPath,Accuracy,double,this->pathDefinition->accuracy)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticPath,IsReversible,bool,this->pathDefinition->reversible)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticPath,EpsgCode,INT32,this->pathDefinition->epsgCode)
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticPath,EpsgVariant,INT32,this->pathDefinition->variant)
