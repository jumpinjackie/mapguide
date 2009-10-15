//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

const STRING MgUnmanagedDataManager::SquareBracketBegin      = L"[";
const wchar_t MgUnmanagedDataManager::SquareBracketCharBegin = L'[';
const STRING MgUnmanagedDataManager::SquareBracketEnd        = L"]";
const wchar_t MgUnmanagedDataManager::SquareBracketCharEnd   = L']';

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgUnmanagedDataManager::MgUnmanagedDataManager()
{
    Initialize();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgUnmanagedDataManager::~MgUnmanagedDataManager()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgUnmanagedDataManager::~MgUnmanagedDataManager()\n")));
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes the object.
///
void MgUnmanagedDataManager::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initialize the unmanaged data manager.
///
void MgUnmanagedDataManager::Initialize()
{
    RefreshUnmanagedDataMappings();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Parses the path into mappingName and subfolder
/// The path must be in the form of:
///     "[mappingName]subfolder1/subfolder2"
///
bool MgUnmanagedDataManager::ParsePath(CREFSTRING path, REFSTRING mappingName, REFSTRING subpath)
{
    bool result = false;

    if (path.empty())
    {
        result = true;
    }
    else
    {
        // the first character must be open square bracket '['
        if (path.at(0) == SquareBracketCharBegin)
        {
            // find the first closed square bracket ']'
            size_t pos = path.find_first_of(SquareBracketCharEnd);
            size_t pathlen = path.length();

            if (pos > 1 && pos < pathlen)
            {
                // extract the mapping name
                mappingName = path.substr(1, pos - 1);

                if (pos + 1 < pathlen)
                {
                    subpath = path.substr(pos + 1, pathlen - pos + 1);
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
        const wchar_t* delimit = L";, \t\r\n\v\f";
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

            if (MgFileUtil::IsFile(fullDataPathname)
                && storeFiles
                && FilterFile(entryName, filters))
            {
                INT64 fileSize = MgFileUtil::GetFileSize(fullDataPathname);
                MgDateTime createdDate = MgFileUtil::GetFileCreationTime(fullDataPathname);
                MgDateTime modifiedDate = MgFileUtil::GetFileModificationTime(fullDataPathname);
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
                    MgDateTime createdDate = MgFileUtil::GetFileCreationTime(fullDataPathname);
                    MgDateTime modifiedDate = MgFileUtil::GetFileModificationTime(fullDataPathname);
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
    string unmanagedDataId = MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(id));
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
    string unmanagedDataId = MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(id));
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
/// Surrounds name with square brackets
///
STRING MgUnmanagedDataManager::FormatMappingName(CREFSTRING name)
{
    return MgUnmanagedDataManager::SquareBracketBegin + name + MgUnmanagedDataManager::SquareBracketEnd;
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
/// Refresh unmanaged data mappings.
///
void MgUnmanagedDataManager::RefreshUnmanagedDataMappings()
{
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    m_unmanagedDataMappings = configuration->GetProperties(
        MgConfigProperties::UnmanagedDataMappingsSection);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return unmanaged data mappings.
///
MgPropertyCollection* MgUnmanagedDataManager::GetUnmanagedDataMappings()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, NULL));

    return SAFE_ADDREF(m_unmanagedDataMappings.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Substitutes unmanaged data mappings
///
int MgUnmanagedDataManager::SubstituteDataPathAliases(REFSTRING doc)
{
    string temp = MgUtil::WideCharToMultiByte(doc);
    int result = SubstituteDataPathAliases(temp);

    doc = MgUtil::MultiByteToWideChar(temp);
    return result;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Substitutes unmanaged data aliases
///
int MgUnmanagedDataManager::SubstituteDataPathAliases(string& doc)
{
    int count = 0;
    size_t startPos = 0, endPos = 0, currStartPos = 0;
    string dataPathAliasBegin = MgUtil::WideCharToMultiByte(MgResourceTag::DataPathAliasBegin);
    string dataPathAliasEnd = MgUtil::WideCharToMultiByte(MgResourceTag::DataPathAliasEnd);
    size_t len1 = dataPathAliasBegin.length();
    size_t len2 = dataPathAliasEnd.length();

    while (string::npos != (startPos = doc.find(dataPathAliasBegin, currStartPos)))
    {
        bool aliasSubstituted = false;
        // beginTag found, now look for endTag
        while (string::npos != (endPos = doc.find(dataPathAliasEnd, startPos + len1)))
        {
            // extract out the mapping name
            string mappingName = doc.substr(startPos + len1, endPos - startPos - len1);
            size_t nameLen = mappingName.length();

            // find the mapping name in the map, and then replace it
            Ptr<MgPropertyCollection> unmanagedDataMappings = MgUnmanagedDataManager::GetInstance()->GetUnmanagedDataMappings();

            if (NULL != unmanagedDataMappings.p)
            {
                Ptr<MgStringProperty> stringProp = dynamic_cast<MgStringProperty*>(unmanagedDataMappings->FindItem(MgUtil::MultiByteToWideChar(mappingName)));
                if (stringProp != NULL)
                {
                    STRING mappingDir = stringProp->GetValue();

                    // replace the mappingName with the actual directory
                    if (!MgFileUtil::EndsWithSlash(mappingDir))
                        MgFileUtil::AppendSlashToEndOfPath(mappingDir);

                    string mappingDirStr = MgUtil::WideCharToMultiByte(mappingDir);
                    size_t dirLen = mappingDirStr.length();

                    doc.replace(startPos, len1 + nameLen + len2, mappingDirStr, 0, dirLen);
                    currStartPos = startPos + dirLen;
                    aliasSubstituted = true;
                    ++count;
                }
            }
            break;
        }

        if (!aliasSubstituted)
            currStartPos = endPos + len2;
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns unmanaged data
///
MgByteReader* MgUnmanagedDataManager::EnumerateUnmanagedData(CREFSTRING path, bool recursive, CREFSTRING type, CREFSTRING filter)
{
    Ptr<MgByteReader> byteReader;

    MG_TRY()

    ACE_TRACE("MgUnmanagedDataManager::EnumerateUnmanagedData");

    Ptr<MgPropertyCollection> unmanagedDataMappings = GetUnmanagedDataMappings();

    if (NULL != unmanagedDataMappings.p)
    {
        // this XML follows the ResourceList-1.0.0.xsd schema
        string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        list += "<UnmanagedDataList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"UnmanagedDataList-1.0.0.xsd\">\n";

        // check arguments...

        // pull out the mapping name from the path
        // path must be in the form of:
        //      ""
        //      "[alias1]"
        //      "[alias1]subfolder1"
        //      "[alias1]subfolder1/"
        //      "[alias1]subfolder1/subfolder2/"

        STRING mappingName = L"", subfolder = L"";
        if (!MgUnmanagedDataManager::ParsePath(path, mappingName, subfolder))
        {
            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(path);

            throw new MgInvalidArgumentException(L"MgUnmanagedDataManager.EnumerateUnmanagedData",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // type must be:
        //      "FOLDERS"
        //      "FILES"
        //      "BOTH"

        bool storeFolders = ACE_OS::strcasecmp(type.c_str(), MgResourceUnmanagedDataType::Folders.c_str()) == 0
            || ACE_OS::strcasecmp(type.c_str(), MgResourceUnmanagedDataType::Both.c_str()) == 0;

        bool storeFiles = ACE_OS::strcasecmp(type.c_str(), MgResourceUnmanagedDataType::Files.c_str()) == 0
            || ACE_OS::strcasecmp(type.c_str(), MgResourceUnmanagedDataType::Both.c_str()) == 0;

        ACE_ASSERT(storeFolders || storeFiles);

        // filter is ignored if type = "FOLDERS"
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
            Ptr<MgStringProperty> stringProp = dynamic_cast<MgStringProperty*>(unmanagedDataMappings->FindItem(mappingName));
            if (stringProp != NULL)
            {
                // we have a match!
                STRING mappingDir = stringProp->GetValue();

                // get the files and/or folders from that folder and subfolder (recursive)
                GetFilesAndFolders(list, mappingName, mappingDir, subfolder, &filters, storeFolders, storeFiles, recursive);
            }
            else
            {
                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(path);

                throw new MgInvalidArgumentException(L"MgUnmanagedDataManager.EnumerateUnmanagedData",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }
        else
        {
            // getting files starting from virtual root (all mappings)
            // iterate thru mappings
            for (int i = 0; i < unmanagedDataMappings->GetCount(); i++)
            {
                Ptr<MgStringProperty> stringProp = dynamic_cast<MgStringProperty*>(unmanagedDataMappings->GetItem(i));

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

        list += "</UnmanagedDataList>";

        Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

        byteSource->SetMimeType(MgMimeType::Xml);
        byteReader = byteSource->GetReader();
    }

    MG_CATCH_AND_THROW(L"MgUnmanagedDataManager.EnumerateUnmanagedData")

    return byteReader.Detach();
}
