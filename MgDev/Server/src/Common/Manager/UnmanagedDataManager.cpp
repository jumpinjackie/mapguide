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

#include "MapGuideCommon.h"
#include "ServerManager.h"
#include "ServerResourceService.h"
#include "UnmanagedDataManager.h"


// Process-wide MgUnmanagedDataManager
Ptr<MgUnmanagedDataManager> MgUnmanagedDataManager::sm_unmanagedDataManager = (MgUnmanagedDataManager*)NULL;

const STRING MgUnmanagedDataManager::Sdf                  = L"SDF";
const STRING MgUnmanagedDataManager::Shp                  = L"SHP";
const STRING MgUnmanagedDataManager::Dwf                  = L"DWF";
const STRING MgUnmanagedDataManager::Raster               = L"RASTER";
const STRING MgUnmanagedDataManager::Odbc                 = L"ODBC";
const STRING MgUnmanagedDataManager::Folder               = L"FOLDER";
const STRING MgUnmanagedDataManager::Jpg                  = L"JPG";
const STRING MgUnmanagedDataManager::Jpeg                 = L"JPEG";
const STRING MgUnmanagedDataManager::Jpe                  = L"JPE";
const STRING MgUnmanagedDataManager::Png                  = L"PNG";
const STRING MgUnmanagedDataManager::Bmp                  = L"BMP";
const STRING MgUnmanagedDataManager::Cal                  = L"CAL";
const STRING MgUnmanagedDataManager::Tga                  = L"TGA";
const STRING MgUnmanagedDataManager::Tif                  = L"TIF";
const STRING MgUnmanagedDataManager::Tiff                 = L"TIFF";
const STRING MgUnmanagedDataManager::Sid                  = L"SID";
const STRING MgUnmanagedDataManager::Ecw                  = L"ECW";
const STRING MgUnmanagedDataManager::Bil                  = L"BIL";
const STRING MgUnmanagedDataManager::Mdb                  = L"MDB";
const STRING MgUnmanagedDataManager::OpenSquareBracket    = L"[";
const STRING MgUnmanagedDataManager::ClosedSquareBracket  = L"]";


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgUnmanagedDataManager::MgUnmanagedDataManager(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgUnmanagedDataManager::~MgUnmanagedDataManager(void)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) MgUnmanagedDataManager::~MgUnmanagedDataManager()\n")));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes the object.
///
void MgUnmanagedDataManager::Dispose(void)
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Recursive method that returns all files for a given mapping name
///
void MgUnmanagedDataManager::GetFiles(MgStringCollection* files, CREFSTRING mappingName, CREFSTRING rootdir, CREFSTRING subdir, CREFSTRING dataTypeFilter, INT32 depth)
{
    STRING fulldir = rootdir;
    MgFileUtil::AppendSlashToEndOfPath(fulldir);
    if (!subdir.empty())
    {
        fulldir += subdir;
        MgFileUtil::AppendSlashToEndOfPath(fulldir);
    }

    // Open the directory
    ACE_DIR* directory = directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(fulldir.c_str()));

    if (directory != NULL)
    {
        dirent* direntry = NULL;

        // Go through the directory entries
        while ((direntry = ACE_OS::readdir(directory)) != NULL)
        {
            STRING entryName = MG_TCHAR_TO_WCHAR(direntry->d_name);
            STRING fullDataPathname = fulldir + entryName;

            if (MgFileUtil::IsFile(fullDataPathname)
                && IsValidFileType(entryName, dataTypeFilter))
            {
                // Add to list of files
                files->Add(FormatMappingName(mappingName) + FormatSubdir(subdir) + entryName);
            }
            else if (MgFileUtil::IsDirectory(fullDataPathname) 
                && entryName.compare(L"..") != 0 // skip ..
                && entryName.compare(L".") != 0 // skip .
                && (depth == -1 || depth > 0))
            {
                if (depth > 0)
                    depth--;

                // recursive call to get files in subdirecories
                GetFiles(files, mappingName, rootdir, FormatSubdir(subdir) + entryName, dataTypeFilter, depth);
            }
        }

        ACE_OS::closedir(directory);
    }
}
 
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Recursive method that returns all subdirectories for a given mapping name
///
void MgUnmanagedDataManager::GetDirectories(MgStringCollection* dirs, CREFSTRING mappingName, CREFSTRING rootdir, CREFSTRING subdir, INT32 depth)
{
    STRING fulldir = rootdir;
    MgFileUtil::AppendSlashToEndOfPath(fulldir);
    if (!subdir.empty())
    {
        fulldir += subdir;
        MgFileUtil::AppendSlashToEndOfPath(fulldir);
    }

    // Open the directory
    ACE_DIR* directory = directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(fulldir.c_str()));

    if (directory != NULL)
    {
        dirent* direntry = NULL;

        // Go through the directory entries
        while ((direntry = ACE_OS::readdir(directory)) != NULL)
        {
            STRING entryName = MG_TCHAR_TO_WCHAR(direntry->d_name);
            STRING fullDataPathname = fulldir + entryName;

            if (MgFileUtil::IsDirectory(fullDataPathname) 
                && entryName.compare(L"..") != 0 // skip ..
                && entryName.compare(L".") != 0) // skip .
            {
                // Add to list of directories
                dirs->Add(FormatMappingName(mappingName) + FormatSubdir(subdir) + entryName);

                if (depth == -1 || depth > 0)
                {
                    if (depth > 0)
                        depth--;

                    // recursive call to get directories in subdirecories
                    GetDirectories(dirs, mappingName, rootdir, FormatSubdir(subdir) + entryName, depth);
                }
            }
        }

        ACE_OS::closedir(directory);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Is valid data type filter
///
bool MgUnmanagedDataManager::IsValidDataTypeFilter(CREFSTRING dataTypeFilter)
{
    // TODO: other file types?
    return (dataTypeFilter.empty()
        || ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Sdf.c_str()) == 0
        || ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Shp.c_str()) == 0
        || ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Dwf.c_str()) == 0
        || ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Raster.c_str()) == 0
        || ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Odbc.c_str()) == 0
        || ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Folder.c_str()) == 0);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// checks that a given file is of a specified type
///
bool MgUnmanagedDataManager::IsValidFileType(CREFSTRING file, CREFSTRING dataTypeFilter)
{
    bool result = false;

    if (ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Sdf.c_str()) == 0)
        result = IsSdfFile(file);
    else if (ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Shp.c_str()) == 0)
        result = IsShpFile(file);
    else if (ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Dwf.c_str()) == 0)
        result = IsDwfFile(file);
    else if (ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Raster.c_str()) == 0)
        result = IsRasterFile(file);
    else if (ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Odbc.c_str()) == 0)
        result = IsOdbcFile(file);
    else if (dataTypeFilter.empty())
        result = IsAnyValidFile(file);

    return result;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Is valid sdf data file
///
bool MgUnmanagedDataManager::IsSdfFile(CREFSTRING file)
{
    return MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Sdf);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Is valid shp data file
///
bool MgUnmanagedDataManager::IsShpFile(CREFSTRING file)
{
    return MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Shp);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Is valid dwf data file
///
bool MgUnmanagedDataManager::IsDwfFile(CREFSTRING file)
{
    return MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Dwf);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Is valid raster data file
///
bool MgUnmanagedDataManager::IsRasterFile(CREFSTRING file)
{
    return MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Jpg)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Jpeg)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Jpe)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Png)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Bmp)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Cal)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Tga)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Tif)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Tiff)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Sid)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Ecw)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Bil);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Is valid odbc data file
///
bool MgUnmanagedDataManager::IsOdbcFile(CREFSTRING file)
{
    // TODO: other file types?
    return MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Mdb);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Is valid unmanaged data file
///
bool MgUnmanagedDataManager::IsAnyValidFile(CREFSTRING file)
{
    return MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Sdf)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Shp)
        || MgFileUtil::EndsWithExtension(file, MgUnmanagedDataManager::Dwf)
        || IsRasterFile(file)
        || IsOdbcFile(file);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Surrounds name with square brackets
///
STRING MgUnmanagedDataManager::FormatMappingName(CREFSTRING name)
{
    return MgUnmanagedDataManager::OpenSquareBracket + name + MgUnmanagedDataManager::ClosedSquareBracket;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Appends a slash to non-empty string
///
STRING MgUnmanagedDataManager::FormatSubdir(CREFSTRING subdir)
{
    STRING result = subdir;
    if (!result.empty())
    {
        MgFileUtil::AppendSlashToEndOfPath(result);
    }
    
    return result;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the pointer to a process-wide MgUnmanagedDataManager.
///
MgUnmanagedDataManager* MgUnmanagedDataManager::GetInstance()
{
    MG_TRY()

    ACE_TRACE("MgUnmanagedDataManager::GetInstance");

    if (MgUnmanagedDataManager::sm_unmanagedDataManager == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance(), NULL));

        if (MgUnmanagedDataManager::sm_unmanagedDataManager == NULL)
        {
            MgUnmanagedDataManager::sm_unmanagedDataManager = new MgUnmanagedDataManager;
        }
    }

    MG_CATCH_AND_THROW(L"MgUnmanagedDataManager.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgUnmanagedDataManager::sm_unmanagedDataManager;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns string collection containing unmanaged data 
///
MgStringCollection* MgUnmanagedDataManager::EnumerateUnmanagedData(CREFSTRING mappingName, CREFSTRING dataTypeFilter, INT32 depth)
{
    Ptr<MgStringCollection> dataPaths = new MgStringCollection();

    MG_TRY()

    ACE_TRACE("MgUnmanagedDataManager::EnumerateUnmanagedData");

    MgConfiguration* config = MgConfiguration::GetInstance();
    Ptr<MgPropertyCollection> properties = config->GetProperties(MgConfigProperties::UnmanagedDataMappingsSection);

    if (properties != NULL)
    {
        if (!IsValidDataTypeFilter(dataTypeFilter))
        {
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(dataTypeFilter);

            throw new MgInvalidArgumentException(L"MgUnmanagedDataManager::EnumerateUnmanagedData",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // are we looking in a specific mapping?
        if (!mappingName.empty())
        {
            STRING mappingDir = L"";

            // check to make sure it's a valid mapping name
            if (properties != NULL)
            {
                // cycle thru mappings until we have a match
                for (int i = 0; i < properties->GetCount(); i++)
                {
                    Ptr<MgStringProperty> stringProp = (MgStringProperty*)properties->GetItem(i);
                    if (mappingName.compare(stringProp->GetName()) == 0)
                    {
                        // we have a match!
                        mappingDir = stringProp->GetValue();
                        break;
                    }
                }
            }

            if (mappingDir.empty())
            {
                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(mappingName);

                throw new MgInvalidArgumentException(L"MgUnmanagedDataManager::EnumerateUnmanagedData",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
            else
            {
                // get the files from that directory (recursive) --> use another method
                dataPaths = new MgStringCollection();

                if (ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Folder.c_str()) == 0)
                    GetDirectories(dataPaths, mappingName, mappingDir, L"", depth);
                else
                    GetFiles(dataPaths, mappingName, mappingDir, L"", dataTypeFilter, depth);
            }
        }
        else
        {
            // getting files from all available mappings

            // iterate thru mappings
            for (int i = 0; i < properties->GetCount(); i++)
            {
                Ptr<MgStringProperty> stringProp = (MgStringProperty*)properties->GetItem(i);

                STRING mapName = stringProp->GetName();
                STRING mapDir = stringProp->GetValue();

                if (ACE_OS::strcasecmp(dataTypeFilter.c_str(), MgUnmanagedDataManager::Folder.c_str()) == 0)
                    GetDirectories(dataPaths, mapName, mapDir, L"", depth);
                else
                    GetFiles(dataPaths, mapName, mapDir, L"", dataTypeFilter, depth);
            }
        }
    }
    MG_CATCH_AND_THROW(L"MgUnmanagedDataManager.EnumerateUnmanagedData")

    return dataPaths.Detach();
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns string collection containing unmanaged data mappings
///
MgStringCollection* MgUnmanagedDataManager::EnumerateUnmanagedDataMappings()
{
    Ptr<MgStringCollection> mappings = new MgStringCollection();

    MgConfiguration* config = MgConfiguration::GetInstance();
        
    Ptr<MgPropertyCollection> properties = config->GetProperties(MgConfigProperties::UnmanagedDataMappingsSection);
    if (properties != NULL)
    {
        for (int i = 0; i < properties->GetCount(); i++)
        {
            Ptr<MgStringProperty> stringProp = (MgStringProperty*)properties->GetItem(i);
            STRING name = stringProp->GetName();
            mappings->Add(name);
        }
    }

    return mappings.Detach();
}


