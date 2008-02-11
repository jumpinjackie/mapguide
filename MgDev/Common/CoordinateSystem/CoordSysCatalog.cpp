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

#include "ArbitraryCoordsys.h"              //for CCsArbitraryCoordinateSystemUtil
#include "CoordSysCategoryCollection.h"
#include "CoordSysCategory.h"
#include "CoordSysInformation.h"

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


CCoordinateSystemCatalog::CCoordinateSystemCatalog() :
    m_libraryStatus(lsInitializationFailed)
{
    MG_TRY()

    m_pCsDict = new CCoordinateSystemDictionary(this);
    m_pDtDict = new CCoordinateSystemDatumDictionary(this);
    m_pElDict = new CCoordinateSystemEllipsoidDictionary(this);
    m_pCtDict = new CCoordinateSystemCategoryDictionary(this);

    if (!m_pCsDict || !m_pDtDict || !m_pElDict || !m_pCtDict)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.MgCoordinateSystemCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_categories = new CCoordinateSystemCategoryCollection();
    if (m_categories == NULL)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.MgCoordinateSystemCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Add arbitrary XY systems. These are not in the dictionary
    CCoordinateSystemCategory* category = NULL;
    category = new CCoordinateSystemCategory(this);
    if (category == NULL)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.MgCoordinateSystemCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    category->SetName(CCsArbitraryCoordinateSystemUtil::ArbitraryXYCategoryDescription);

    for (int i = 0; i < CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount; i++)
    {
        CCoordinateSystemInformation* coordSysInfo = new CCoordinateSystemInformation();
        if (coordSysInfo == NULL)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemCatalog.MgCoordinateSystemCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING units = CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[i].unitsCode;

        // Determine description
        coordSysInfo->m_code = CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[i].code;
        coordSysInfo->m_description = CCsArbitraryCoordinateSystemUtil::ArbitraryXYDescription + L" (" + units + L")";
        coordSysInfo->m_projection = CCsArbitraryCoordinateSystemUtil::ArbitraryXYProjection;
        coordSysInfo->m_projectionDescription = CCsArbitraryCoordinateSystemUtil::ArbitraryXYProjectionDescription;
        coordSysInfo->m_datum = CCsArbitraryCoordinateSystemUtil::ArbitraryXYDatum;
        coordSysInfo->m_datumDescription = CCsArbitraryCoordinateSystemUtil::ArbitraryXYDatumDescription;
        coordSysInfo->m_ellipsoid = CCsArbitraryCoordinateSystemUtil::ArbitraryXYEllipsoid;
        coordSysInfo->m_ellipsoidDescription = CCsArbitraryCoordinateSystemUtil::ArbitraryXYEllipsoidDescription;
        coordSysInfo->m_proj4Definition = L"";

        category->Add(coordSysInfo);
    }

    m_categories->Add(category);

    STRING path = COORDINATE_SYSTEM_DESCRIPTION_FILENAME;

    // This dictionary is optional, without it only arbitrary XY is supported.
    ReadCategoryDictionary(path);

    // Initialize the category dictionary
    m_pCtDict->Initialize();

    m_libraryStatus = lsInitialized;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemCatalog.MgCoordinateSystemCatalog")
}

CCoordinateSystemCatalog::~CCoordinateSystemCatalog()
{
}

void CCoordinateSystemCatalog::PrepareForDispose()
{
    delete m_categories;
    m_categories = NULL;

    m_pCsDict = NULL;
    m_pDtDict = NULL;
    m_pElDict = NULL;
    m_pCtDict = NULL;
}

CCoordinateSystemCategoryCollection* CCoordinateSystemCatalog::GetCoordinateSystemCategories()
{
    return m_categories;
}

CCoordinateSystemCategory* CCoordinateSystemCatalog::GetCoordinateSystemCategory(CREFSTRING categoryName)
{
    CCoordinateSystemCategory* category = NULL;

    if(categoryName.length() > 0)
    {
        CCoordinateSystemCategory* tmpCategory = NULL;

        size_t numCats = m_categories->GetCount();
        size_t i;
        for (i = 0; i < numCats; i++)
        {
            tmpCategory = m_categories->GetItem(i);
            if(_wcsicmp(categoryName.c_str(), tmpCategory->GetName().c_str()) == 0)
            {
                category = tmpCategory;
                break;
            }
        }
    }

    return category;
}

void CCoordinateSystemCatalog::ReadCategoryDictionary(CREFSTRING fileName)
{
    // Open categories files from current directory or PROJ directory

    // Check to see if the PROJ environment variable is set
    char* szPath = getenv(PROJ_NAD_PATH);
    STRING path;
    if(szPath)
    {
        wchar_t* wszPath = Convert_Ascii_To_Wide(szPath);
        if(wszPath)
        {
            if(::wcslen(wszPath) > 0)
            {
                path = wszPath;
            }

            delete [] wszPath;
            wszPath = NULL;
        }
    }

#ifdef _WIN32

    // If we failed to read the path from an environment variable,
    // see if it is set in the registry
    if(path.empty())
    {
        wchar_t wszPath[MAX_PATH_SIZE] = L"";
        HKEY hKey;
        LONG lResult = ERROR_SUCCESS;

        // Attempt to open the registry key
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, PROJ_NAD_REG_KEY, 0, KEY_QUERY_VALUE, &hKey);
        if (lResult == ERROR_SUCCESS)
        {
            DWORD dwReqSize = MAX_PATH_SIZE;

            // Attempt to read the value
            lResult = RegQueryValueEx(hKey, PROJ_NAD_REG_PATH, NULL, NULL, (LPBYTE) wszPath, &dwReqSize);
            if(lResult == ERROR_SUCCESS && ::wcslen(wszPath) > 0)
            {
                path = wszPath;
            }
            RegCloseKey(hKey);
        }
    }

    // Attempt to open from current directory
    FILE* file = _wfopen(fileName.c_str(), L"rt");

    if (NULL == file && !path.empty())
    {
        // Attempt to open from PROJ directory
        STRING wName = path;
        wName.append(L"\\");
        wName.append(fileName);
        file = _wfopen(wName.c_str(), L"rt");
    }
#else
    // Linux - attempt to open from current directory
    char* szFileName = Convert_Wide_To_Ascii(fileName.c_str());

    FILE* file = fopen(szFileName, "rt");
    if (NULL == file && !path.empty())
    {
        // Attempt to open from PROJ directory
        string fName = szPath;
        fName.append("/");
        fName.append(szFileName);
        file = fopen(fName.c_str(), "rt");
    }

    delete [] szFileName;
    szFileName = NULL;
#endif

    if(file != NULL)
    {
        CCoordinateSystemCategory* category = NULL;

        // Set a default path if we could not set it from the environment
        if(path.empty())
        {
            path = L"./nad";
        }

        AppendSlashToEndOfPath(path);

        do
        {
            STRING buffer = ReadString(file, 1024);
            if (!buffer.empty())
            {
                // Parse the categroy name
                STRING catName = L"";

                // Trim white spaces
                buffer = TrimLeft(buffer);
                buffer = TrimLeft(buffer, L"\t");
                buffer = TrimRight(buffer);
                buffer = TrimRight(buffer, L"\n");
                if(buffer.length() > 0)
                {
                    if(buffer.find(L"[") == 0)
                    {
                        size_t pos = buffer.find(L"]");
                        if(-1 != pos)
                        {
                            catName.assign(buffer, 1, pos-1);
                            category = new CCoordinateSystemCategory(this);
                            if (category)
                            {
                                category->SetName(catName);

                                // Read the coordinate systems for this category
                                buffer = ReadString(file, 1024);
                                if (!buffer.empty())
                                {
                                    // Trim white spaces
                                    buffer = TrimLeft(buffer);
                                    buffer = TrimLeft(buffer, L"\t");
                                    buffer = TrimRight(buffer);
                                    buffer = TrimRight(buffer, L"\n");
                                    if(buffer.length() > 0)
                                    {
                                        if(buffer.find(L"#") == 0)
                                        {
                                            // This is a comment line, so skip it
                                        }
                                        else
                                        {
                                            // Read the filename associated with this category
                                            STRING filename = path;
                                            filename += buffer;
                                            ReadCategoryCoordinateSystems(filename, category);
                                            m_categories->Add(category);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                throw new MgOutOfMemoryException(L"CCoordinateSystemCatalog.ReadCategoryDictionary", __LINE__, __WFILE__, NULL, L"", NULL);
                            }
                        }
                    }
                }
            }
        }
        while(feof(file) == 0);

        // Close the file
        fclose(file);
    }
    else
    {
        #ifdef _DEBUG
        char* szFileName = Convert_Wide_To_Ascii(fileName.c_str());
        if(szFileName)
        {
            printf("Failed to open coordinate system categories file: %s\n", szFileName);
            delete [] szFileName;
            szFileName = NULL;
        }
        #endif
    }
}

void CCoordinateSystemCatalog::ReadCategoryCoordinateSystems(CREFSTRING fileName, CCoordinateSystemCategory* category)
{
    if (NULL == category)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.ReadCategoryCoordinateSystems",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Open the file
#ifdef _WIN32
    FILE* file = _wfopen(fileName.c_str(), L"rt");
#else
    char* szFileName = Convert_Wide_To_Ascii(fileName.c_str());
    FILE* file = fopen(szFileName, "rt");
    delete [] szFileName;
    szFileName = NULL;
#endif
    if(file != NULL)
    {
        STRING code = L"";
        STRING description = category->GetName();
        STRING proj4 = L"";

        // Set EPSG prefix if this is the EPSG category
        STRING codePrefix = L"";
        if(_wcsicmp(category->GetName().c_str(), L"EPSG") == 0)
        {
            codePrefix = L"EPSG:";
        }

        do
        {
            // Read the coordinate systems for this category
            STRING buffer = ReadString(file, 1024);
            if (!buffer.empty())
            {
                // Trim white spaces
                buffer = TrimLeft(buffer);
                buffer = TrimLeft(buffer, L"\t");
                buffer = TrimRight(buffer);
                buffer = TrimRight(buffer, L"\n");
                if(buffer.length() > 0)
                {
                    if(buffer.find(L"#") == 0)
                    {
                        // This is a comment line, so skip it, but use it as the description
                        description.assign(buffer,1,buffer.length());
                        description = TrimLeft(description);
                        description = TrimLeft(description, L"\t");
                    }
                    else
                    {
                        // Read the coordinate system
                        size_t pos = buffer.find(L">");
                        if(-1 != pos)
                        {
                            if(pos > 0)
                            {
                                // Extract code
                                code.assign(buffer,1,pos-1);
                                code = TrimRight(code);

                                // Update buffer
                                buffer.assign(buffer,pos+1,buffer.length());

                                // Trim white spaces
                                buffer = TrimLeft(buffer);
                                if(buffer.find(L"#") == 0)
                                {
                                    // There is a comment on this line so use it as the description
                                    description.assign(buffer,1,buffer.length());
                                    description = TrimLeft(description);
                                    description = TrimLeft(description, L"\t");

                                    buffer = L"";
                                }

                                size_t pos2 = 0;
                                bool bDone = false;
                                while(!bDone)
                                {
                                    pos2 = buffer.find(L"<");
                                    if(-1 == pos2)
                                    {
                                        // Read the next line
                                        STRING temp = ReadString(file, 1024);
                                        temp = TrimLeft(temp);
                                        temp = TrimLeft(temp, L"\t");
                                        temp = TrimRight(temp);
                                        temp = TrimRight(temp, L"\n");

                                        buffer += L" " + temp;
                                    }
                                    else
                                    {
                                        bDone = true;
                                    }
                                }

                                if(pos2 > pos+1)
                                {
                                    // Extract Proj4
                                    proj4.assign(buffer, 0, pos2);
                                    proj4 = TrimLeft(proj4);
                                    proj4 = TrimLeft(proj4, L"\t");
                                    proj4 = TrimRight(proj4);

                                    // Create the new coordinate system
                                    CCoordinateSystemInformation* coordSysInfo = new CCoordinateSystemInformation();
                                    if(coordSysInfo)
                                    {
                                        coordSysInfo->m_code = codePrefix + code;
                                        coordSysInfo->m_description = description;
                                        coordSysInfo->m_proj4Definition = proj4;

                                        category->Add(coordSysInfo);

                                        code = L"";
                                        description = category->GetName();
                                        proj4 = L"";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        while(feof(file) == 0);

        // Close the file
        fclose(file);
    }
}

STRING CCoordinateSystemCatalog::ReadString(FILE* file, int size)
{
    STRING results;

#ifdef _WIN32
    wchar_t* str = new wchar_t[size];
    memset(str, 0, sizeof(wchar_t)*size);
    fgetws(str, size, file);

    results = str;

    delete [] str;
    str = NULL;
#else
    char* str = new char[size];
    memset(str, 0, sizeof(char)*size);
    fgets(str, size, file);

    wchar_t* wszValue = Convert_Ascii_To_Wide(str);
    results = wszValue;
    delete [] wszValue;
    wszValue = NULL;

    delete [] str;
    str = NULL;
#endif

    return results;
}

void CCoordinateSystemCatalog::SetDefaultDictionaryDirAndFileNames()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.SetDefaultDictionaryDirAndFileNames", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystemCatalog::GetDefaultDictionaryDir()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.GetDefaultDictionaryDir", __LINE__, __WFILE__, NULL, L"", NULL);
}

void CCoordinateSystemCatalog::SetDictionaryDir(CREFSTRING sDirPath)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.SetDictionaryDir", __LINE__, __WFILE__, NULL, L"", NULL);
}

STRING CCoordinateSystemCatalog::GetDictionaryDir()
{
    return m_sDir;
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
    return SAFE_ADDREF(m_pCtDict.p);
}

MgCoordinateSystemDictionary* CCoordinateSystemCatalog::GetCoordinateSystemDictionary()
{
    return SAFE_ADDREF(m_pCsDict.p);
}

MgCoordinateSystemDatumDictionary* CCoordinateSystemCatalog::GetDatumDictionary()
{
    return SAFE_ADDREF(m_pDtDict.p);
}

MgCoordinateSystemEllipsoidDictionary* CCoordinateSystemCatalog::GetEllipsoidDictionary()
{
    return SAFE_ADDREF(m_pElDict.p);
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

bool CCoordinateSystemCatalog::AreDictionaryFilesWritable()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemCatalog.AreDictionaryFilesWritable", __LINE__, __WFILE__, NULL, L"", NULL);
}
