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

#include "CoordSysCommon.h"
#include "CoordSysIncludes.h"
#include "CoordSysUtil.h"

#define COORDINATE_SYSTEM_DESCRIPTION_FILENAME L"categories.txt"
#define PROJ_NAD_PATH "PROJ_LIB"

CCoordinateSystemCatalog::CCoordinateSystemCatalog()
{
    m_categories = new CCoordinateSystemCategoryCollection();
    if (m_categories == NULL)
    {
        throw new COutOfMemoryException(L"CCoordinateSystemCatalog.CCoordinateSystemCatalog", __LINE__, __WFILE__, L"Could not allocate CCoordinateSystemCategoryCollection.");
    }

    // Add arbitrary XY systems. These are not in the dictionary
    CCoordinateSystemCategory* category;
    category = new CCoordinateSystemCategory(CCoordinateSystem::ArbitraryXYCategoryDescription);
    if (category == NULL)
    {
        throw new COutOfMemoryException(L"CCoordinateSystemCatalog.CCoordinateSystemCatalog", __LINE__, __WFILE__, L"Could not allocate CCoordinateSystemCategory.");
    }

    for (int i = 0; i < CCoordinateSystem::ArbitraryXYCoordinateSystemsCount; i++)
    {
        CCoordinateSystemInformation* coordSysInfo = new CCoordinateSystemInformation();
        if (coordSysInfo == NULL)
        {
            throw new COutOfMemoryException(L"CCoordinateSystemCatalog.CCoordinateSystemCatalog", __LINE__, __WFILE__, L"Could not allocate CCoordinateSystemInformation.");
        }

        STRING units = CCoordinateSystem::ArbitraryXYCoordinateSystems[i].unitsCode;

        // Determine description
        coordSysInfo->m_code = CCoordinateSystem::ArbitraryXYCoordinateSystems[i].code;
        coordSysInfo->m_description = CCoordinateSystem::ArbitraryXYDescription + L" (" + units + L")";
        coordSysInfo->m_projection = CCoordinateSystem::ArbitraryXYProjection;
        coordSysInfo->m_projectionDescription = CCoordinateSystem::ArbitraryXYProjectionDescription;
        coordSysInfo->m_datum = CCoordinateSystem::ArbitraryXYDatum;
        coordSysInfo->m_datumDescription = CCoordinateSystem::ArbitraryXYDatumDescription;
        coordSysInfo->m_ellipsoid = CCoordinateSystem::ArbitraryXYEllipsoid;
        coordSysInfo->m_ellipsoidDescription = CCoordinateSystem::ArbitraryXYEllipsoidDescription;
        coordSysInfo->m_proj4Definition = L"";

        category->Add(coordSysInfo);
    }

    m_categories->Add(category);

    STRING path = COORDINATE_SYSTEM_DESCRIPTION_FILENAME;

    // This dictionary is optional, without it only arbitrary XY is supported.
    ReadCategoryDictionary(path);
}

CCoordinateSystemCatalog::~CCoordinateSystemCatalog()
{
    if(m_categories)
    {
        delete m_categories;
        m_categories = NULL;
    }
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
            if (categoryName == tmpCategory->GetCategoryName())
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
    // Open the file
#ifdef WIN32
    FILE* file = _wfopen(fileName.c_str(), L"rt");
#else
    char* szFileName = Convert_Wide_To_Ascii(fileName.c_str());
    FILE* file = fopen(szFileName, "rt");
    delete [] szFileName;
    szFileName = NULL;
#endif
    if(file != NULL)
    {
        CCoordinateSystemCategory* category = NULL;

        #ifdef _WIN32
        // Check to see if the environment variable is set
        char* szPath = getenv(PROJ_NAD_PATH);
        #else
        // Linux initialization
        char szPath[1024] = "";
        strcpy(szPath, getenv(PROJ_NAD_PATH));
        #endif

        STRING path = L"";
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
                            category = new CCoordinateSystemCategory(catName);
                            if (category)
                            {
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
                                throw new COutOfMemoryException(L"CCoordinateSystemCatalog.ReadCategoryDictionary", __LINE__, __WFILE__, L"Could not allocate CCoordinateSystemCategory.");
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
    if(NULL == category)
    {
        throw new CNullArgumentException(L"CCoordinateSystemCatalog.ReadCategoryCoordinateSystems", __LINE__, __WFILE__, L"[2] - CCoordinateSystemCategory pointer.");
    }

    // Open the file
#ifdef WIN32
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
        STRING description = category->GetCategoryName();
        STRING proj4 = L"";

        // Set EPSG prefix if this is the EPSG category
        STRING codePrefix = L"";
        if(_wcsicmp(category->GetCategoryName().c_str(), L"EPSG") == 0)
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
                                        description = category->GetCategoryName();
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

#ifdef WIN32
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
