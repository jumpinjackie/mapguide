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

const STRING MgUnmanagedDataManager::Folders              = L"FOLDERS";
const STRING MgUnmanagedDataManager::Files                = L"FILES";
const STRING MgUnmanagedDataManager::Both                 = L"BOTH";
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
/// Parses the path into mappingName and subfolder
/// The path must be in the form of:
///     "[mappingName]subfolder1/subfolder2"
///
bool MgUnmanagedDataManager::ParsePath(CREFSTRING path, REFSTRING mappingName, REFSTRING subfolder)
{
    bool result = false;

    if (path.empty())
    {
        result = true;
    }
    else
    {
        // the first character must be open square bracket '['
        if (path.at(0) == L'[')
        {
            // find the first closed square bracket ']'
            size_t pos = path.find_first_of(L']');
            size_t pathlen = path.length();

            if (pos > 1 && pos < pathlen)
            {
                // extract the mapping name
                mappingName = path.substr(1, pos - 1);

                if (pos + 1 < pathlen)
                {
                    subfolder = path.substr(pos + 1, pathlen - pos + 1);
                }

                result = true;
            }
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Parses the filter into an MgStringCollection
///
void MgUnmanagedDataManager::ParseFilter(CREFSTRING filter, MgStringCollection* filters)
{
    if (!filter.empty())
    {
        wchar_t* token = const_cast<wchar_t*>(filter.c_str());
        const wchar_t* delimit = L"; \t\r\n\v\f";
        wchar_t* state = NULL;

        for (token = _wcstok(token, delimit, &state);
             token != NULL;
             token = _wcstok(NULL, delimit, &state))
        {
            filters->Add(token);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns true is file ends in one of the extensions in filters
///
bool MgUnmanagedDataManager::FilterFile(CREFSTRING file, const MgStringCollection* filters)
{
    bool result = false;

    INT32 numFilters = filters->GetCount();
    if (numFilters == 0)
    {
        // no filters, return all files 
        result = true;
    }
    else
    {
        for (int i = 0; i < numFilters; i++)
        {
            if (MgFileUtil::EndsWithExtension(file, filters->GetItem(i)))
            {
                result = true;
                break;
            }
        }
    }
    return result;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Recursive method that returns all files and/or folders for a given mapping 
/// name
///
void MgUnmanagedDataManager::GetFilesAndFolders(string& list, CREFSTRING mappingName, CREFSTRING rootdir, CREFSTRING subdir, const MgStringCollection* filters, bool storeFolders, bool storeFiles, bool recursive)
{
    STRING fulldir = rootdir;
    if (!MgFileUtil::EndsWithSlash(fulldir))
        MgFileUtil::AppendSlashToEndOfPath(fulldir);

    if (!subdir.empty())
    {
        fulldir += subdir;
        if (!MgFileUtil::EndsWithSlash(fulldir))
            MgFileUtil::AppendSlashToEndOfPath(fulldir);
    }

    // Open the directory
    ACE_DIR* directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(fulldir.c_str()));

    if (directory != NULL)
    {
        dirent* direntry = NULL;

        // Go through the directory entries
        while ((direntry = ACE_OS::readdir(directory)) != NULL)
        {
            STRING entryName = MG_TCHAR_TO_WCHAR(direntry->d_name);
            STRING fullDataPathname = fulldir + entryName;
            MgDateTime createdDate = MgFileUtil::GetFileCreationTime(fullDataPathname);
            MgDateTime modifiedDate = MgFileUtil::GetFileModificationTime(fullDataPathname);

            if (MgFileUtil::IsFile(fullDataPathname)
                && storeFiles
                && FilterFile(entryName, filters))
            {
                INT64 fileSize = MgFileUtil::GetFileSize(fullDataPathname);
  
                AddFile(list, mappingName, subdir, entryName, fileSize, createdDate, modifiedDate);
            }
            else if (MgFileUtil::IsDirectory(fullDataPathname) 
                && entryName.compare(L"..") != 0 // skip ..
                && entryName.compare(L".") != 0) // skip .
            {
                if (storeFolders)
                {
                    // Add folders
                    INT32 numFolders = 0;
                    INT32 numFiles = 0;
                    GetNumberOfFilesAndSubfolders(fullDataPathname, numFolders, numFiles);

                    AddFolder(list, mappingName, subdir, entryName, numFolders, numFiles, createdDate, modifiedDate);
                }

                // recursive call to get files in subfolders
                if (recursive)
                    GetFilesAndFolders(list, mappingName, rootdir, FormatSubdir(subdir) + entryName, filters, storeFolders, storeFiles, recursive);
            }
        }

        ACE_OS::closedir(directory);
    }
}
 
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Appends folder details to xml string
///
void MgUnmanagedDataManager::AddFolder(string& list, CREFSTRING mappingName, CREFSTRING subdir, CREFSTRING entryName, INT32 numFolders, INT32 numFiles, MgDateTime createdDate, MgDateTime modifiedDate)
{
    list += "\t<UnmanagedDataFolder>\n";

    STRING id = FormatMappingName(mappingName) + FormatSubdir(subdir) + entryName;
    // append a slash to the folder if is NOT just the [alias]
    if (!entryName.empty() && !MgFileUtil::EndsWithSlash(id))
        MgFileUtil::AppendSlashToEndOfPath(id);
    string unmanagedDataId = MgUtil::WideCharToMultiByte(id);
    list += "\t\t<UnmanagedDataId>";
    list += unmanagedDataId;
    list += "</UnmanagedDataId>\n";

    list += "\t\t<CreatedDate>";
    list += createdDate.ToXmlStringUtf8();
    list += "</CreatedDate>\n";

    list += "\t\t<ModifiedDate>";
    list += modifiedDate.ToXmlStringUtf8();
    list += "</ModifiedDate>\n";

    STRING numFoldersStr;
    MgUtil::Int32ToString(numFolders, numFoldersStr);
    list += "\t\t<NumberOfFolders>";
    list += MgUtil::WideCharToMultiByte(numFoldersStr);
    list += "</NumberOfFolders>\n";

    string numFilesStr;
    MgUtil::Int32ToString(numFiles, numFilesStr);
    list += "\t\t<NumberOfFiles>";
    list += numFilesStr;
    list += "</NumberOfFiles>\n";

    list += "\t</UnmanagedDataFolder>\n";
}
 
void MgUnmanagedDataManager::AddFile(string& list, CREFSTRING mappingName, CREFSTRING subdir, CREFSTRING entryName, INT64 fileSize, MgDateTime createdDate, MgDateTime modifiedDate)
{
    list += "\t<UnmanagedDataFile>\n";

    STRING id = FormatMappingName(mappingName) + FormatSubdir(subdir) + entryName;
    string unmanagedDataId = MgUtil::WideCharToMultiByte(id);
    list += "\t\t<UnmanagedDataId>";
    list += unmanagedDataId;
    list += "</UnmanagedDataId>\n";

    list += "\t\t<CreatedDate>";
    list += createdDate.ToXmlStringUtf8();
    list += "</CreatedDate>\n";

    list += "\t\t<ModifiedDate>";
    list += modifiedDate.ToXmlStringUtf8();
    list += "</ModifiedDate>\n";

    string fileSizeStr;
    MgUtil::Int64ToString(fileSize, fileSizeStr);
    list += "\t\t<Size>";
    list += fileSizeStr;
    list += "</Size>\n";

    list += "\t</UnmanagedDataFile>\n";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieves number of folders and files in the specified folder path
/// pre-condition: assumes dirpath is a valid folder
///
void MgUnmanagedDataManager::GetNumberOfFilesAndSubfolders(CREFSTRING dirpath, INT32& numFolders, INT32& numFiles)
{
    ACE_DIR* directory = ACE_OS::opendir(ACE_TEXT_WCHAR_TO_TCHAR(dirpath.c_str()));
    if (directory != NULL)
    {
        dirent* direntry = NULL;

        // Go through the directory entries
        while ((direntry = ACE_OS::readdir(directory)) != NULL)
        {
            STRING entryName = MG_TCHAR_TO_WCHAR(direntry->d_name);

            STRING fullDataPathname = dirpath;
            if (!MgFileUtil::EndsWithSlash(fullDataPathname))
                MgFileUtil::AppendSlashToEndOfPath(fullDataPathname);

            fullDataPathname += entryName;

            if (MgFileUtil::IsFile(fullDataPathname))
            {
                ++numFiles;
            }
            else if (MgFileUtil::IsDirectory(fullDataPathname) 
                && entryName.compare(L"..") != 0 // skip ..
                && entryName.compare(L".") != 0) // skip .
            {
                ++numFolders;
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
/// Returns unmanaged data 
///
MgByteReader* MgUnmanagedDataManager::EnumerateUnmanagedData(CREFSTRING path, bool recursive, CREFSTRING select, CREFSTRING filter)
{
    Ptr<MgByteReader> byteReader;

    MG_TRY()

    ACE_TRACE("MgUnmanagedDataManager::EnumerateUnmanagedData");

    MgConfiguration* config = MgConfiguration::GetInstance();
    Ptr<MgPropertyCollection> properties = config->GetProperties(MgConfigProperties::UnmanagedDataMappingsSection);
   
    if (properties != NULL)
    {
        // this XML follows the ResourceList-1.0.0.xsd schema
        string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        list += "<ResourceList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ResourceList-1.0.0.xsd\">\n";

        // check arguments...

        // pull out the mapping name from the path
        // path must be in the form of:
        //      ""
        //      "[alias1]"
        //      "[alias1]subfolder1"
        //      "[alias1]subfolder1/"
        //      "[alias1]subfolder1/subfolder2/"

        STRING mappingName = L"", subfolder = L"";
        if (!ParsePath(path, mappingName, subfolder))
        {
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(path);

            throw new MgInvalidArgumentException(L"MgUnmanagedDataManager::EnumerateUnmanagedData",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // select must be: 
        //      "FOLDERS"
        //      "FILES"
        //      "BOTH"

        bool storeFolders = ACE_OS::strcasecmp(select.c_str(), MgUnmanagedDataManager::Folders.c_str()) == 0 
            || ACE_OS::strcasecmp(select.c_str(), MgUnmanagedDataManager::Both.c_str()) == 0;
        
        bool storeFiles = ACE_OS::strcasecmp(select.c_str(), MgUnmanagedDataManager::Files.c_str()) == 0 
            || ACE_OS::strcasecmp(select.c_str(), MgUnmanagedDataManager::Both.c_str()) == 0;

        ACE_ASSERT(storeFolders || storeFiles);


        // filter is ignored if select = "FOLDERS"
        // filter can be:
        //      ""
        //      ".sdf"
        //      ".sdf;.pdf;.shp"
        //      "sdf"
        //      "sdf;dwf;png"

        MgStringCollection filters;
        if (storeFiles)
            ParseFilter(filter, &filters);

        // are we looking in a specific path?
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
                arguments.Add(path);

                throw new MgInvalidArgumentException(L"MgUnmanagedDataManager::EnumerateUnmanagedData",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
            else
            {
                // get the files and/or folders from that folder and subfolder (recursive)
                GetFilesAndFolders(list, mappingName, mappingDir, subfolder, &filters, storeFolders, storeFiles, recursive);
            }
        }
        else
        {
            // getting files starting from virtual root (all mappings)
            // iterate thru mappings
            for (int i = 0; i < properties->GetCount(); i++)
            {
                Ptr<MgStringProperty> stringProp = (MgStringProperty*)properties->GetItem(i);

                STRING mapName = stringProp->GetName();
                STRING mapDir = stringProp->GetValue();

                if (MgFileUtil::IsDirectory(mapDir))
                {
                    if (storeFolders)
                    {
                        MgDateTime createdDate = MgFileUtil::GetFileCreationTime(mapDir);
                        MgDateTime modifiedDate = MgFileUtil::GetFileModificationTime(mapDir);

                        INT32 numFolders = 0;
                        INT32 numFiles = 0;

                        GetNumberOfFilesAndSubfolders(mapDir, numFolders, numFiles);

                        // add top-level mappings
                        AddFolder(list, mapName, L"", L"", numFolders, numFiles, createdDate, modifiedDate);
                    }

                    if (recursive)
                        GetFilesAndFolders(list, mapName, mapDir, L"", &filters, storeFolders, storeFiles, recursive);
                }
            }
        }

        list += "</ResourceList>";

        Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

        byteSource->SetMimeType(MgMimeType::Xml);
        byteReader = byteSource->GetReader();
    }
    
    MG_CATCH_AND_THROW(L"MgUnmanagedDataManager.EnumerateUnmanagedData")

    return byteReader.Detach();
}
