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
#include "CriticalSection.h"

#include "CoordSysGeodeticPathElement.h"


#include "CoordSysTransform.h"          //for CCoordinateSystemTransform
#include "CoordSysUtil.h"               //for CsDictionaryOpenMode
#include "MentorDictionary.h"

using namespace CSLibrary;

#define CS_MAP_DEF_VARIABLE this->pathElement //needed by CoordSysMacro

#include "CoordSysMacro.h" //for DEFINE_GET_SET_STRING and DEFINE_GET_SET_NUMERIC

CCoordinateSystemGeodeticPathElement::CCoordinateSystemGeodeticPathElement(const cs_GeodeticPathElement_* const pathElementArg, bool isProtected)
    : pathElement(NULL), isProtected(isProtected)
{
    this->Reset(pathElementArg);
}

CCoordinateSystemGeodeticPathElement::~CCoordinateSystemGeodeticPathElement()
{
    this->CleanupInstanceVariables();
}

void CCoordinateSystemGeodeticPathElement::Dispose()
{
    delete this;
}

void CCoordinateSystemGeodeticPathElement::CleanupInstanceVariables()
{
    if (NULL != this->pathElement)
    {
        CS_free(this->pathElement);
        this->pathElement = NULL;
    }
}

void CCoordinateSystemGeodeticPathElement::Reset(const cs_GeodeticPathElement_* const pathElementArg)
{
    cs_GeodeticPathElement_* tempPathElement = (cs_GeodeticPathElement_*) CS_malc(sizeof(cs_GeodeticPathElement_));
    if (NULL == tempPathElement)
        throw new MgOutOfMemoryException(L"CCoordinateSystemGeodeticPathElement.Reset", __LINE__, __WFILE__, NULL, L"", NULL);

    MG_TRY()

    this->CleanupInstanceVariables();
    if (NULL == pathElementArg) //initialize to 0 memory
        memset(tempPathElement, 0, sizeof(cs_GeodeticPathElement_));
    else
        *tempPathElement = *pathElementArg; //otherwise, copy the values from the arg over

    this->pathElement = tempPathElement; //make us hold the allocated memory...
    tempPathElement = NULL; //...and make sure, we don't free it below

    MG_CATCH(L"CCoordinateSystemGeodeticPathElement.Reset")

    if (NULL != tempPathElement)
        CS_free(tempPathElement);

    MG_THROW()
}

bool CCoordinateSystemGeodeticPathElement::IsValid()
{
    if (NULL == this->pathElement)
        return false;

    size_t transformNameLength = strlen(this->pathElement->geodeticXformName);
    _ASSERT(transformNameLength < sizeof(this->pathElement->geodeticXformName)); //how comes?

    //TODO: put transformNameLength 64 in cs_map.h
    if (0 == transformNameLength || transformNameLength >= sizeof(this->pathElement->geodeticXformName))
        return false;

    return (cs_DTCDIR_FWD == this->pathElement->direction) || (cs_DTCDIR_INV == this->pathElement->direction);
}

bool CCoordinateSystemGeodeticPathElement::IsProtected()
{
    return this->isProtected;
}
bool CCoordinateSystemGeodeticPathElement::GetIsInversed ()
{
    return (pathElement->direction == cs_DTCDIR_INV);  
}
void CCoordinateSystemGeodeticPathElement::SetIsInversed (bool inversed)
{
    pathElement->direction = (inversed) ? cs_DTCDIR_INV : cs_DTCDIR_FWD;
}
DEFINE_GET_SET_STRING(CCoordinateSystemGeodeticPathElement,TransformName,this->pathElement->geodeticXformName)
