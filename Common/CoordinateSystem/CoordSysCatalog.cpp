//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "CoordSysUtil.h"
#include "CriticalSection.h"

#include "CoordSys.h"                       //for CCoordinateSystem
#include "CoordSysEnum.h"                   //for CCoordinateSystemEnum
#include "CoordSysEnumDatum.h"              //for CCoordinateSystemEnumDatum
#include "CoordSysEnumEllipsoid.h"          //for CCoordinateSystemEnumEllipsoid
#include "CoordSysDictionary.h"             //for CCoordinateSystemDictionary
#include "CoordSysDatumDictionary.h"        //for CCoordinateSystemDatumDictionary
#include "CoordSysEllipsoidDictionary.h"    //for CCoordinateSystemEllipsoidDictionary
#include "CoordSysCategoryDictionary.h"     //for CCoordinateSystemCategoryDictionary
#include "CoordSysCatalog.h"                //for CCoordinateSystemCatalog
#include "CoordSysFormatConverter.h"        //for CCoordinateSystemFormatConverter

#include "ogr_spatialref.h"
#include "proj_api.h"
#include "cpl_csv.h"
#include "cpl_conv.h"
#include "cpl_multiproc.h"


#define COORDINATE_SYSTEM_DESCRIPTION_FILENAME L"categories.txt"
#define PROJ_NAD_PATH "PROJ_LIB"
#define PROJ_NAD_REG_KEY L"SOFTWARE\\MapGuideOpenSource\\Coordsys\\"
#define PROJ_NAD_REG_PATH L"PROJ_LIB"
#define MAX_PATH_SIZE 4095

using namespace CSLibrary;

// Dummy class used to automate initialization/uninitialization of CPL.
class CInitCPL
{
public:
    CInitCPL()
    {
        // Initialize the critical section class
        CriticalClass.Initialize();
    }
    ~CInitCPL()
    {
        {
            // Lock all threads
            AutoCriticalClass acc;

            // free PROJ4 resources
            pj_deallocate_grids();

            // free CPL resources
            CSVDeaccess(NULL);
            CPLFinderClean();
            CPLCleanupTLS();
        }
    }
};

static CInitCPL s_InitCPL;


CCoordinateSystemCatalog::CCoordinateSystemCatalog()
    : m_pCsDict(NULL),
	m_pDtDict(NULL),
	m_pElDict(NULL),
	m_pCtDict(NULL),
    m_libraryStatus(lsInitializationFailed)
{
    MG_TRY()

	m_pCsDict=new CCoordinateSystemDictionary(this);
	m_pDtDict=new CCoordinateSystemDatumDictionary(this);
	m_pElDict=new CCoordinateSystemEllipsoidDictionary(this);
	m_pCtDict=new CCoordinateSystemCategoryDictionary(this);

    if (!m_pCsDict || !m_pDtDict || !m_pElDict || !m_pCtDict)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.MgCoordinateSystemCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Initialize the categroy dictionary
    m_pCtDict->Initialize();

    MG_CATCH(L"MgCoordinateSystemCatalog.MgCoordinateSystemCatalog")
    if (mgException != NULL)
    {
        SAFE_RELEASE(m_pCsDict);
        SAFE_RELEASE(m_pDtDict);
        SAFE_RELEASE(m_pElDict);
        SAFE_RELEASE(m_pCtDict);
    }
    MG_THROW()
}

CCoordinateSystemCatalog::~CCoordinateSystemCatalog()
{
    SAFE_RELEASE(m_pCsDict);
    SAFE_RELEASE(m_pDtDict);
    SAFE_RELEASE(m_pElDict);
    SAFE_RELEASE(m_pCtDict);
}

void CCoordinateSystemCatalog::SetDefaultDictionaryDirAndFileNames()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.SetDefaultDictionaryDirAndFileNames", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystemCatalog::SetDictionaryDir(CREFSTRING sDirPath)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.SetDictionaryDir", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystemCatalog::SetDictionaryFileNames(CREFSTRING sEllipsoidDictFileName, CREFSTRING sDatumDictFileName, CREFSTRING sCoordinateSystemDictFileName, CREFSTRING sCategoryDictFileName)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.SetDictionaryFileNames", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystemCatalog::GetDefaultDictionaryDirAndFileNames(REFSTRING sDir, REFSTRING sEllipsoidDictFileName, REFSTRING sDatumDictFileName, REFSTRING sCoordinateSystemDictFileName, REFSTRING sCategoryDictFileName)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.GetDefaultDictionaryDirAndFileNames", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystemCatalog::GetDictionaryDir()
{
    return m_sDir;
}

STRING CCoordinateSystemCatalog::GetCategoryFileName()
{
    return m_sCoordinateSystemDictFileName;
}

STRING CCoordinateSystemCatalog::GetCoordinateSystemFileName()
{
    return m_sCoordinateSystemDictFileName;;
}

STRING CCoordinateSystemCatalog::GetDatumFileName()
{
    return m_sDatumDictFileName;
}

STRING CCoordinateSystemCatalog::GetEllipsoidFileName()
{
    return m_sEllipsoidDictFileName;
}

void CCoordinateSystemCatalog::SetProtectionMode(INT16 nMode)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.SetProtectionMode", __LINE__, __WFILE__, NULL, L"", NULL);
}

INT16 CCoordinateSystemCatalog::GetProtectionMode()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.GetProtectionMode", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinateSystemCategoryDictionary* CCoordinateSystemCatalog::GetCategoryDictionary()
{
    MgCoordinateSystemCategoryDictionary* pDict=static_cast<MgCoordinateSystemCategoryDictionary*>(m_pCtDict);
    return SAFE_ADDREF(pDict);
}

MgCoordinateSystemDictionary* CCoordinateSystemCatalog::GetCoordinateSystemDictionary()
{
    MgCoordinateSystemDictionary* pDict=static_cast<MgCoordinateSystemDictionary*>(m_pCsDict);
    return SAFE_ADDREF(pDict);
}

MgCoordinateSystemDatumDictionary* CCoordinateSystemCatalog::GetDatumDictionary()
{
    MgCoordinateSystemDatumDictionary* pDict=static_cast<MgCoordinateSystemDatumDictionary*>(m_pDtDict);
    return SAFE_ADDREF(pDict);
}

MgCoordinateSystemEllipsoidDictionary* CCoordinateSystemCatalog::GetEllipsoidDictionary()
{
    MgCoordinateSystemEllipsoidDictionary* pDict=static_cast<MgCoordinateSystemEllipsoidDictionary*>(m_pElDict);
    return SAFE_ADDREF(pDict);
}

MgDisposableCollection* CCoordinateSystemCatalog::GetGeodeticTransformations(MgCoordinateSystemDatum* pSource, MgCoordinateSystemDatum *pTarget)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.GetGeodeticTransformations", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinateSystemMathComparator* CCoordinateSystemCatalog::GetMathComparator()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.GetMathComparator", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinateSystemFormatConverter* CCoordinateSystemCatalog::GetFormatConverter()
{
    CCoordinateSystemFormatConverter *pNew=NULL;
    MG_TRY()
    pNew=new CCoordinateSystemFormatConverter(this);
	if (NULL == pNew) 
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.GetFormatConverter", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.GetFormatConverter")

	//And we're done!  Return success.
	return pNew;
}

MgCoordinateSystemProjectionInformation* CCoordinateSystemCatalog::GetProjectionInformation()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.GetProjectionInformation", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinateSystemUnitInformation* CCoordinateSystemCatalog::GetUnitInformation()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.GetUnitInformation", __LINE__, __WFILE__, NULL, L"", NULL);
}

LibraryStatus CCoordinateSystemCatalog::GetLibraryStatus()
{
    return m_libraryStatus;
}

//MgDisposable
void CCoordinateSystemCatalog::Dispose()
{
    delete this;
}

MgCoordinateSystemDictionaryUtility* CCoordinateSystemCatalog::GetDictionaryUtility()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.GetDictionaryUtility", __LINE__, __WFILE__, NULL, L"", NULL);
}
