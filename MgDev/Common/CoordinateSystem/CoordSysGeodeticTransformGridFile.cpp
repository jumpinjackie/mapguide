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

#define CS_MAP_DEF_VARIABLE this->gridFileDesc

#include "CoordSysMacro.h"

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CriticalSection.h"

#include "CoordSysUtil.h"               //for Convert_Wide_To_UTF8
#include "MentorUtil.h"                 //for OpenDictionaryFile()
#include "MentorDictionary.h"

#include "CoordSysGeodeticTransformGridFile.h"

#include <cs_map.h>

using namespace CSLibrary;

CCoordinateSystemGeodeticTransformGridFile::CCoordinateSystemGeodeticTransformGridFile(const csGeodeticXfromParmsFile_* const fileFormatParam, bool isProtected)
    : gridFileDesc(NULL), isProtected(isProtected)
{
    this->gridFileDesc = (csGeodeticXfromParmsFile_*)CS_malc(sizeof(csGeodeticXfromParmsFile_));
    if (NULL == this->gridFileDesc)
        throw new MgOutOfMemoryException(L"CCoordinateSystemGeodeticAnalyticalTransformDefParams.ctor", __LINE__, __WFILE__, NULL, L"", NULL);

    ::memset(this->gridFileDesc, 0, sizeof(csGeodeticXfromParmsFile_));
    if (NULL != fileFormatParam)
        *this->gridFileDesc = *fileFormatParam;
    //else: keep the 0'ed memory
}

CCoordinateSystemGeodeticTransformGridFile::~CCoordinateSystemGeodeticTransformGridFile()
{
    this->ReleaseInstance();
}

void CCoordinateSystemGeodeticTransformGridFile::Dispose()
{
    delete this;
}

void CCoordinateSystemGeodeticTransformGridFile::ReleaseInstance()
{
    if (NULL != this->gridFileDesc)
    {
        CS_free(this->gridFileDesc);
        this->gridFileDesc = NULL;
    }
}

void CCoordinateSystemGeodeticTransformGridFile::CopyTo(csGeodeticXfromParmsFile_* destGridFile) const
{
    ENSURE_NOT_NULL(destGridFile, L"CCoordinateSystemGeodeticTransformGridFile.CopyTo");
    *destGridFile = *this->gridFileDesc;
}

bool CCoordinateSystemGeodeticTransformGridFile::IsProtected()
{
    return this->isProtected;
}

bool CCoordinateSystemGeodeticTransformGridFile::IsValid()
{
    _ASSERT(NULL != this->gridFileDesc);

    size_t fileNameLength = strlen(this->gridFileDesc->fileName);

    _ASSERT(fileNameLength < csGRIDI1_FLNMSZ);
    if (0 == fileNameLength || fileNameLength >= csGRIDI1_FLNMSZ)
        return false;

    switch(this->gridFileDesc->fileFormat)
    {
    case cs_DTCFRMT_CNTv1:
    case cs_DTCFRMT_CNTv2:
    case cs_DTCFRMT_NADCN:
    case cs_DTCFRMT_FRNCH:
    case cs_DTCFRMT_JAPAN:
    case cs_DTCFRMT_ATS77:
    case cs_DTCFRMT_OST97:
    case cs_DTCFRMT_OST02:
    case cs_DTCFRMT_GEOCN:
        break;

    case cs_DTCFRMT_NONE:
        _ASSERT(false);
        break;

    //unsupported file formats
    default:
        return false;
    }

    switch(this->gridFileDesc->direction)
    {
    case 'i':
    case 'I':
    case 'f':
    case 'F':
        break;

    default:
        return false;
    }

    return true;
}


//************************************ Getters / Setters ***************************** /
DEFINE_GET_SET_NUMERIC(CCoordinateSystemGeodeticTransformGridFile,FileFormat,INT32,this->gridFileDesc->fileFormat);

bool CCoordinateSystemGeodeticTransformGridFile::GetIsInverseDirection()
{
    return 'I' == this->gridFileDesc->direction || 'i' == this->gridFileDesc->direction;
}

void CCoordinateSystemGeodeticTransformGridFile::SetIsInverseDirection(bool isInverseDirection)
{
    this->gridFileDesc->direction = isInverseDirection ? 'I' : 'F';
}

DEFINE_GET_SET_STRING(CCoordinateSystemGeodeticTransformGridFile, FileName, this->gridFileDesc->fileName);
