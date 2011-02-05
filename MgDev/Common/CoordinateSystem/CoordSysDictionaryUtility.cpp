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

#include "CoordSysUtil.h"               //for Convert_Wide_To_Ascii, CsDictionaryOpenMode
#include "CoordSysDictionaryUtility.h"  //for CCoordinateSystemDictionaryUtility
#include "CoordSysCategoryDictionary.h" //for klCategoryMagic
#include "cs_map.h"

#ifdef _WIN32
#include <tchar.h>
#endif

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemDictionaryUtility::CCoordinateSystemDictionaryUtility(MgCoordinateSystemCatalog *pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemDictionaryUtility::~CCoordinateSystemDictionaryUtility()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemDictionaryUtility::CreateEllipsoidDictionary(CREFSTRING sDictPathName)
{
    return CreateDictionaryFile(sDictPathName.c_str(), cs_ELDEF_MAGIC);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemDictionaryUtility::CreateDatumDictionary(CREFSTRING sDictPathName)
{
    return CreateDictionaryFile(sDictPathName.c_str(), cs_DTDEF_MAGIC);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemDictionaryUtility::CreateCoordsysDictionary(CREFSTRING sDictPathName)
{
    return CreateDictionaryFile(sDictPathName.c_str(), cs_CSDEF_MAGIC);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemDictionaryUtility::CreateCategoryDictionary(CREFSTRING sDictPathName)
{
    return CreateDictionaryFile(sDictPathName.c_str(), klCategoryMagic);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//MgDisposable
void CCoordinateSystemDictionaryUtility::Dispose()
{
    delete this;
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemDictionaryUtility::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemDictionaryUtility::CreateDictionaryFile(const wchar_t* kpDict, const long klMagic)
{
    MG_TRY()

    char *pDict = Convert_Wide_To_Ascii(kpDict);    //need to delete [] pDict;
    if (NULL == pDict)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDictionaryUtility.CreateDictionaryFile", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make sure the file doesn't already exist
    csFILE *pFile=NULL;
    pFile=CS_fopen(pDict, "rb");
    if (pFile && !ferror(pFile))
    {
        CS_fclose(pFile);
        delete [] pDict;
        //It already exists, but it's not supposed to.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDictionaryUtility.CreateDictionaryFile", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDuplicateException", NULL);
    }
    if (pFile)
    {
        clearerr(pFile);
    }

    //Make the file
    pFile=CS_fopen(pDict, "wb");
    delete [] pDict;
    size_t nWrite=CS_fwrite(reinterpret_cast<const char *>(&klMagic), sizeof(klMagic), 1, pFile);
    if (1!=nWrite || ferror(pFile))
    {
        CS_fclose(pFile);
        throw new MgFileIoException(L"MgCoordinateSystemDictionaryUtility.CreateDictionaryFile", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Success!
    CS_fclose(pFile);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDictionaryUtility.CreateDictionaryFile")
}
