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

#define CS_MAP_DEF_VARIABLE this->gridFileParams

#include "CoordSysMacro.h"

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysUtil.h"
#include "MentorUtil.h"

#include "CoordSysGeodeticTransformGridFile.h"
#include "CoordSysGeodeticTransformDefParams.h"
#include "CoordSysGeodeticInterpolationTransformDefParams.h"

#include <cs_map.h>

using namespace CSLibrary;

CCoordinateSystemGeodeticInterpolationTransformDefParams::CCoordinateSystemGeodeticInterpolationTransformDefParams(
    const csGridFileXformParams& params, bool isProtected)
    : CCoordinateSystemGeodeticTransformDefParams(isProtected), gridFileParams(NULL)
{
    this->Reset();
    *this->gridFileParams = params;
}

CCoordinateSystemGeodeticInterpolationTransformDefParams::~CCoordinateSystemGeodeticInterpolationTransformDefParams()
{
    this->ReleaseInstance();
}

void CCoordinateSystemGeodeticInterpolationTransformDefParams::Dispose()
{
    delete this;
}

void CCoordinateSystemGeodeticInterpolationTransformDefParams::Reset()
{
    this->ReleaseInstance();

    this->gridFileParams = (csGridFileXformParams*) CS_malc(sizeof(csGridFileXformParams));
    if (NULL == this->gridFileParams)
        throw new MgOutOfMemoryException(L"CCoordinateSystemGeodeticInterpolationTransformDefParams.Reset", __LINE__, __WFILE__, NULL, L"", NULL);

    memset(this->gridFileParams, 0, sizeof(csGridFileXformParams));
}

void CCoordinateSystemGeodeticInterpolationTransformDefParams::ReleaseInstance()
{
    if (NULL != this->gridFileParams)
    {
        CS_free(this->gridFileParams);
        this->gridFileParams = NULL;
    }
}

void CCoordinateSystemGeodeticInterpolationTransformDefParams::SetGridFiles(MgDisposableCollection* gridFiles)
{
    VERIFY_INITIALIZED(L"CCoordinateSystemGeodeticInterpolationTransformDefParams.SetGridFiles");
    VERIFY_NOT_PROTECTED(L"CCoordinateSystemGeodeticInterpolationTransformDefParams.SetGridFiles");

    //we don't take ownership over fileNames
    ENSURE_NOT_NULL(gridFiles, L"CCoordinateSystemGeodeticInterpolationTransformDefParams.SetGridFileNames");
    const INT32 fileNamesCount = gridFiles->GetCount();
    if (fileNamesCount > csGRIDI1_FILEMAX)
        throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticInterpolationTransformDefParams.SetGridFiles", __LINE__, __WFILE__, NULL, L"", NULL);

    bool emptyFileCollection = (0 == fileNamesCount);
    csGeodeticXfromParmsFile_* allFiles = emptyFileCollection ? NULL : new csGeodeticXfromParmsFile_[fileNamesCount];

    MG_TRY()

    if (!emptyFileCollection)
        memset(allFiles, 0, fileNamesCount * sizeof(csGeodeticXfromParmsFile_));

    for(INT32 i = 0; i < fileNamesCount; i++)
    {
        Ptr<MgDisposable> gridFile = gridFiles->GetItem(i); //calls [AddRef] on the item being returned
        CCoordinateSystemGeodeticTransformGridFile* gridFileSetting = dynamic_cast<CCoordinateSystemGeodeticTransformGridFile*>(gridFile.p);
        if (NULL == gridFileSetting)
            throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticInterpolationTransformDefParams.SetGridFiles", __LINE__, __WFILE__, NULL, L"", NULL);

        //we only allow for setting valid grid files
        if (!gridFileSetting->IsValid())
            throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticInterpolationTransformDefParams.SetGridFiles", __LINE__, __WFILE__, NULL, L"", NULL);

        //take the values from the settings we have been passed here and set our internal
        //[csGridFileXFormat] instance at index [i] to those values
        gridFileSetting->CopyTo(&allFiles[i]);
    }

    this->gridFileParams->fileReferenceCount = fileNamesCount;

    //wipe out all memory where the filenames are stored...
    memset(this->gridFileParams->fileNames, 0, csGRIDI1_FILEMAX * sizeof(csGeodeticXfromParmsFile_));
    //...and copy our values in case we've been given a non-empty list
    if (!emptyFileCollection)
        memcpy(this->gridFileParams->fileNames, allFiles, fileNamesCount * sizeof(csGeodeticXfromParmsFile_));

    MG_CATCH(L"CCoordinateSystemGeodeticInterpolationTransformDefParams.SetGridFileNames");

    delete[] allFiles; //safe to call on [null]

    MG_THROW();
}

MgDisposableCollection* CCoordinateSystemGeodeticInterpolationTransformDefParams::GetGridFiles()
{
    Ptr<MgDisposableCollection> gridFileNames = new MgDisposableCollection();

    INT32 fileCount = this->gridFileParams->fileReferenceCount;
    if (0 == fileCount)
        return gridFileNames.Detach();

    for(INT32 i = 0; i < fileCount && i < csGRIDI1_FILEMAX; i++)
    {
        csGeodeticXfromParmsFile_& singleGridFile = this->gridFileParams->fileNames[i];

        Ptr<CCoordinateSystemGeodeticTransformGridFile> gridFileSetting =
            new CCoordinateSystemGeodeticTransformGridFile(&singleGridFile, this->IsProtected());

        //the collection will take ownership over [gridFileSetting]
        gridFileNames->Add(gridFileSetting);
    }

    return gridFileNames.Detach();
}

MgCoordinateSystemGeodeticTransformGridFile* CCoordinateSystemGeodeticInterpolationTransformDefParams::NewGridFile()
{
    return new CCoordinateSystemGeodeticTransformGridFile(NULL, false);
}

bool CCoordinateSystemGeodeticInterpolationTransformDefParams::IsValid()
{
    if (NULL == this->gridFileParams)
        return false;

    Ptr<MgDisposableCollection> gridFiles = this->GetGridFiles();
    INT32 gridFileCount = gridFiles->GetCount();
    if (gridFileCount > csGRIDI1_FILEMAX)
        return false;

    for (INT32 i = 0; i < gridFileCount; i++)
    {
        Ptr<MgDisposable> gridFilePtr = gridFiles->GetItem(i);
        MgCoordinateSystemGeodeticTransformGridFile* gridFile = dynamic_cast<MgCoordinateSystemGeodeticTransformGridFile*>(gridFilePtr.p);

        if (NULL == gridFile || !gridFile->IsValid())
        {
            _ASSERT(false); //how comes? This can only have happened when we initialized this item here
            return false;
        }
    }

    return true; //0 items count is ok
}

bool CCoordinateSystemGeodeticInterpolationTransformDefParams::IsProtected()
{
    return CCoordinateSystemGeodeticTransformDefParams::IsProtected();
}

DEFINE_GET_SET_STRING(CCoordinateSystemGeodeticInterpolationTransformDefParams,Fallback,this->gridFileParams->fallback)

void CCoordinateSystemGeodeticInterpolationTransformDefParams::CopyTo(void* target) const
{
    ENSURE_NOT_NULL(target, L"CCoordinateSystemGeodeticInterpolationTransformDefParams.CopyTo");

    memcpy(target, this->gridFileParams, sizeof(csGridFileXformParams));
}
