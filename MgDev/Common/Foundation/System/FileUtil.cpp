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

#include "Foundation.h"
#include "FileUtil.h"

ACE_Recursive_Thread_Mutex MgFileUtil::sm_mutex;

const STRING MgFileUtil::sm_reservedCharacters = L"\\/:*?\"<>|";
const STRING MgFileUtil::sm_slash = L"\\/";

static int MgDirEntrySelector(const dirent *d)
{
    return (0 != ACE_OS::strcmp(d->d_name, ACE_TEXT(".")) &&
            0 != ACE_OS::strcmp(d->d_name, ACE_TEXT("..")));
}

static int MgDirEntryComparator(const dirent **d1, const dirent **d2)
{
    return ACE_OS::strcmp((*d1)->d_name, (*d2)->d_name);
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgFileUtil::MgFileUtil()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgFileUtil::~MgFileUtil()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Validates the specified file name.
///
void MgFileUtil::ValidateFileName(CREFSTRING fileName)
{
    MG_TRY()

    if (fileName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(
            L"MgFileUtil.ValidateFileName",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    if (STRING::npos != fileName.find_first_of(sm_reservedCharacters))
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(fileName);

        MgStringCollection whyArguments;
        whyArguments.Add(sm_reservedCharacters);

        throw new MgInvalidArgumentException(
            L"MgFileUtil.ValidateFileName",
            __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", &whyArguments);
    }

    if (L'.' == fileName[0] || L'.' == fileName[fileName.length() - 1])
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(fileName);

        throw new MgInvalidArgumentException(
            L"MgFileUtil.ValidateFileName",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgFileUtil.ValidateFileName")
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks if the specified string (e.g. file extension) begins with a dot.
/// </summary>
///----------------------------------------------------------------------------

bool MgFileUtil::BeginsWithDot(CREFSTRING str)
{
    size_t index = str.find_first_not_of(L'.');

    return (0 != index && STRING::npos != index);
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks if the specified string (e.g. pathname) ends with a forward or
/// backward slash.
/// </summary>
///----------------------------------------------------------------------------

bool MgFileUtil::EndsWithSlash(CREFSTRING str)
{
    size_t index = str.find_last_not_of(sm_slash);

    return ((str.length() - 1) != index);
}

///----------------------------------------------------------------------------
/// <summary>
/// Check to see if a file ends in the desired extension
/// </summary>
/// <param name="pathname">
/// path to file name, e.g. "c:\temp\readme.txt"
/// </param>
/// <param name="extension">
/// file extension, e.g. ".txt" or "txt"
/// </param>
///----------------------------------------------------------------------------
bool MgFileUtil::EndsWithExtension(CREFSTRING pathname, CREFSTRING extension)
{
    CHECKARGUMENTEMPTYSTRING(pathname, L"MgFileUtil.EndsWithExtension");
    CHECKARGUMENTEMPTYSTRING(extension, L"MgFileUtil.EndsWithExtension");

    bool result = false;

    size_t offset = 0;
    if (!BeginsWithDot(extension))
        offset = 1;

    size_t len = extension.length();
    size_t pos = pathname.find_last_of(L'.') + offset;

    if (STRING::npos != pos && (pathname.length() == pos + len))
    {
        STRING temp = pathname.substr(pos, len);

        if (ACE_OS::strcasecmp(temp.c_str(), extension.c_str()) == 0)
        {
            result = true;
        }
    }


    return result;
}

///----------------------------------------------------------------------------
/// <summary>
/// Appends a forward slash to the end of the specified path if it does exist.
/// </summary>
///----------------------------------------------------------------------------

void MgFileUtil::AppendSlashToEndOfPath(REFSTRING path)
{
    if (!EndsWithSlash(path))
    {
        path.append(L"/");
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Removes any forward or backward slash from the end of the specified path.
/// </summary>
///----------------------------------------------------------------------------

void MgFileUtil::RemoveSlashFromEndOfPath(REFSTRING path)
{
    size_t index = path.find_last_not_of(sm_slash);

    path.erase(index + 1);
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieves the status information on a file.
/// </summary>
///----------------------------------------------------------------------------

bool MgFileUtil::GetFileStatus(CREFSTRING pathname, struct _stat64& statInfo,
    bool strict)
{
    bool success = false;

    MG_TRY()

    CHECKARGUMENTEMPTYSTRING(pathname, L"MgFileUtil.GetFileStatus");

    STRING path = pathname;

    if (IsRootUncName(path) || IsRootDrive(path))
        AppendSlashToEndOfPath(path);
    else
        RemoveSlashFromEndOfPath(path);

    { // Begin guarding.
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

#ifdef _WIN32
        success = ::_wstat64(path.c_str(), &statInfo) == 0;
#else
        success = ::stat(MgUtil::WideCharToMultiByte(path).c_str(), &statInfo) == 0;
#endif
    } // End guarding.

    if (!success && strict)
    {
        MgStringCollection arguments;
        arguments.Add(pathname);

        throw new MgFileIoException(L"MgFileUtil.GetFileStatus",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgFileUtil.GetFileStatus")

    return success;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines if the specified file or directory exists on disk.
/// </summary>
///
/// <param name="pathname">
/// Full path name of the file or directory.
/// </param>
///
/// <returns>
/// true if the specified file or directory exists on disk, false otherwise.
/// </returns>
///----------------------------------------------------------------------------

bool MgFileUtil::PathnameExists(CREFSTRING pathname)
{
    struct _stat64 statInfo;

    return GetFileStatus(pathname, statInfo);
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks if the specified pathname is a directory.
/// </summary>
///----------------------------------------------------------------------------

bool MgFileUtil::IsDirectory(CREFSTRING pathname)
{
    bool result = false;
    struct _stat64 statInfo;

    if (GetFileStatus(pathname, statInfo))
    {
        result = (S_IFDIR == (S_IFMT & statInfo.st_mode));
    }

    return result;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks if the specified pathname is a root UNC name.
/// </summary>
///----------------------------------------------------------------------------

bool MgFileUtil::IsRootUncName(CREFSTRING pathname)
{
    bool result = false;

#ifdef _WIN32
    // pathname must be in the form: \\machine\share\ or \\machine\share
    STRING temp = pathname;
    RemoveSlashFromEndOfPath(temp);

    // minimum length is 5
    // e.g. \\a\b
    if (temp.length() >= 5)
    {
        // check for two leading slashes
        size_t pos1 = temp.find_first_not_of(sm_slash);
        if (pos1 == 2)
        {
            // finds the next slash after the machine name
            size_t pos2 = temp.find_first_of(sm_slash, 3);
            if (pos2 >= 3)
            {
                // no more slashes the after third slash
                size_t pos3 = temp.find_first_of(sm_slash, pos2 + 1);
                result = pos3 == string::npos;
            }
        }
    }
#endif

    return result;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks if the specified pathname is a drive letter.
/// </summary>
///----------------------------------------------------------------------------

bool MgFileUtil::IsRootDrive(CREFSTRING pathname)
{
    bool result = false;

#ifdef _WIN32
    // must be in the form: c:\ or c:
    STRING temp = pathname;

    RemoveSlashFromEndOfPath(temp);

    if (temp.length() == 2)
    {
        result = temp[1] == L':' && IsDriveLetter(temp[0]);
    }
#endif

    return result;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks if the specified character is a drive letter.
/// </summary>
///----------------------------------------------------------------------------

bool MgFileUtil::IsDriveLetter(wchar_t letter)
{
    // make it lowercase
    wchar_t c = towlower(letter);

    // check that it's a-z
    return c >= L'a' && c <= L'z';
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a directory and it can create the new directory recursively.
/// Both the backslash(\)  and the forward slash(/) are valid path delimiters in character strings.
/// Restriction: not support UNC pathnames.
/// </summary>
///
/// <exceptions>
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgFileIoException
/// MgDuplicateDirectoryException
/// </exceptions>
///----------------------------------------------------------------------------

void MgFileUtil::CreateDirectory(CREFSTRING path, bool strict, bool recursive)
{
    MG_TRY()

    if (PathnameExists(path))
    {
        if (strict)
        {
            MgStringCollection arguments;
            arguments.Add(path);

            throw new MgDuplicateDirectoryException(L"MgFileUtil.CreateDirectory",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        return;
    }

    if (recursive)
    {
        STRING fullPath = path;
        AppendSlashToEndOfPath(fullPath);

        size_t len = fullPath.length();
        size_t count = len;             // the length of substring of the fullPath
        size_t pos = 0;                 // current slash position in the fullPath
        size_t subpos = 0;              // current slash position in the substring of the fullPath

        // Not support UNC pathnames
        while (STRING::npos != (subpos = fullPath.substr(pos, count).find_first_of(sm_slash)))
        {
            pos += subpos + 1;  // move forward the subpos in order to skip the found slash.

            if (pos > 1)        // skip the slash as the beginning of the path string, in order to support the path like this "/abc/def/gh/"
            {
                CREFSTRING subPath = fullPath.substr(0, pos);

                if (!PathnameExists(subPath))
                {
                    MkDir(subPath);
                }
            }

            count = len - pos;
        }
    }
    else
    {
        MkDir(path);
    }

    MG_CATCH_AND_THROW(L"MgFileUtil.CreateDirectory")
}

/// private helper

void MgFileUtil::MkDir(CREFSTRING path)
{
    MG_TRY()

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    int errCode = ACE_OS::mkdir(MG_WCHAR_TO_TCHAR(path));

    if (0 != errCode)
    {
        errCode = errno; // This is not thread safe - requires guard
        MgStringCollection arguments;
        arguments.Add(path);

        switch (errCode)
        {
            case EEXIST: // PathnameExists is more reliable. It gets called above, so this error should not be detected.
            case ENOENT:
            case EACCES:
            default:
                MG_FILE_IO_EXCEPTION_ADD_ERRNO();
                throw new MgFileIoException(L"MgFileUtil.MkDir",
                    __LINE__, __WFILE__, &arguments, L"MgFileIoErrNo", &errNoArgs);
                break;
        }
    }

    MG_CATCH_AND_THROW(L"MgFileUtil.MkDir")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes a directory.
/// </summary>
///
/// <exceptions>
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgFileIoException
/// </exceptions>
///----------------------------------------------------------------------------

void MgFileUtil::DeleteDirectory(CREFSTRING path, bool recursive, bool strict)
{
    MG_TRY()

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    if (CleanDirectory(path, recursive, strict) == false)
    {
        return;
    }

    int errCode = _rmdir(MgUtil::WideCharToMultiByte(path).c_str());

    if (0 != errCode && strict)
    {
        errCode = errno; // This is not thread safe - requires guard
        MgStringCollection arguments;
        arguments.Add(path);

        switch (errCode)
        {
            case ENOENT: // PathnameExists is more reliable. It gets called above, so this error should not be detected.
            case ENOTEMPTY:
            case EACCES:
            default:
                MG_FILE_IO_EXCEPTION_ADD_ERRNO();
                throw new MgFileIoException(L"MgFileUtil.DeleteDirectory",
                    __LINE__, __WFILE__, &arguments, L"MgFileIoErrNo", &errNoArgs);
                break;
        }
    }

    MG_CATCH_AND_THROW(L"MgFileUtil.DeleteDirectory")
}

///----------------------------------------------------------------------------
/// <summary>
/// Cleans up a directory.
/// </summary>
///
/// <exceptions>
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgFileIoException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgFileUtil::CleanDirectory(CREFSTRING path, bool recursive, bool strict)
{
    struct dirent **dirEntries = NULL;
    int numEntries = 0;

    MG_TRY()

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    if (!PathnameExists(path))
    {
        if (strict)
        {
            MgStringCollection arguments;
            arguments.Add(path);

            throw new MgDirectoryNotFoundException(L"MgFileUtil.CleanDirectory",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        return false;
    }

    numEntries = ACE_OS::scandir(MG_WCHAR_TO_TCHAR(path), &dirEntries,
        MgDirEntrySelector, MgDirEntryComparator);

    if (NULL != dirEntries)
    {
        STRING root = path;

        AppendSlashToEndOfPath(root);

        for (int i = 0; i < numEntries; ++i)
        {
            STRING pathname = root;

            pathname += MG_TCHAR_TO_WCHAR(dirEntries[i]->d_name);

            if (IsFile(pathname))
            {
                DeleteFile(pathname, strict);
            }
            else if (recursive)
            {
                DeleteDirectory(pathname, recursive, strict);
            }
        }
    }

    MG_CATCH(L"MgFileUtil.CleanDirectory")

    if (NULL != dirEntries)
    {
        for (int i = 0; i < numEntries; ++i)
        {
#if defined (ACE_LACKS_STRUCT_DIR)
            // By default, ACE_LACKS_STRUCT_DIR is defined for Windows but not for Linux.
            // When ACE_LACKS_STRUCT_DIR is NOT defined, d_name is initialized via ACE_OS::memcpy,
            // When ACE_LACKS_STRUCT_DIR is defined, d_name is created via ACE_OS::malloc,
            // and therefore it must be freed.

            ACE_OS::free(dirEntries[i]->d_name);
#endif
            ACE_OS::free(dirEntries[i]);
        }

        ACE_OS::free(dirEntries);
    }

    MG_THROW()

    return true;
}

///----------------------------------------------------------------------------
/// <summary>
/// Changes the current working directory to the specified directory.
/// </summary>
///
/// <param name="path">
/// The path to the directory to be made current.
/// </param>
///
/// <returns>
/// The previous working directory.
/// </returns>
///
/// <exceptions>
/// MgDirectoryNotFoundException
/// MgFileIoException
/// </exceptions>
///----------------------------------------------------------------------------

STRING MgFileUtil::ChangeDirectory(CREFSTRING path)
{
    STRING currDir;
    ACE_TCHAR buffer[MAXPATHLEN];

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, L""));

    if (ACE_OS::getcwd(buffer, MAXPATHLEN) == NULL)
    {
        MgStringCollection arguments;
        arguments.Add(path);

        throw new MgFileIoException(L"MgFileUtil.ChangeDirectory",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if (ACE_OS::chdir(MG_WCHAR_TO_TCHAR(path)) != 0)
    {
        MgStringCollection arguments;
        arguments.Add(path);

        throw new MgDirectoryNotFoundException(L"MgFileUtil.ChangeDirectory",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    currDir = MG_TCHAR_TO_WCHAR(buffer);

    return currDir;
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets all files in the specified directory.
/// </summary>
///----------------------------------------------------------------------------

bool MgFileUtil::GetFilesInDirectory(
    MgStringCollection* files,
    CREFSTRING path,
    bool recursive,
    bool fileNameOnly)
{
    CHECKARGUMENTNULL(files, L"MgFileUtil.GetFilesInDirectory");

    struct dirent **dirEntries = NULL;
    int numEntries = 0;

    MG_TRY()

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    if (!PathnameExists(path))
    {
        MgStringCollection arguments;
        arguments.Add(path);

        throw new MgDirectoryNotFoundException(L"MgFileUtil.GetFilesInDirectory",
            __LINE__, __WFILE__, &arguments, L"", NULL);

        return false;
    }

    numEntries = ACE_OS::scandir(MG_WCHAR_TO_TCHAR(path), &dirEntries,
        MgDirEntrySelector, MgDirEntryComparator);

    if (NULL != dirEntries)
    {
        STRING root = path;

        AppendSlashToEndOfPath(root);

        for (int i = 0; i < numEntries; ++i)
        {
            STRING pathname = root;
            STRING fileName = MG_TCHAR_TO_WCHAR(dirEntries[i]->d_name);

            pathname += fileName;
            if (IsFile(pathname))
            {
                if (fileNameOnly)
                    files->Add(fileName);
                else
                    files->Add(pathname);
            }
            else if (recursive)
            {
                GetFilesInDirectory(files, pathname, recursive);
            }
        }
    }

    MG_CATCH(L"MgFileUtil.GetFilesInDirectory")

    if (NULL != dirEntries)
    {
        for (int i = 0; i < numEntries; ++i)
        {
#if defined (ACE_LACKS_STRUCT_DIR)
            // By default, ACE_LACKS_STRUCT_DIR is defined for Windows but not for Linux.
            // When ACE_LACKS_STRUCT_DIR is NOT defined, d_name is initialized via ACE_OS::memcpy,
            // When ACE_LACKS_STRUCT_DIR is defined, d_name is created via ACE_OS::malloc,
            // and therefore it must be freed.

            ACE_OS::free(dirEntries[i]->d_name);
#endif
            ACE_OS::free(dirEntries[i]);
        }

        ACE_OS::free(dirEntries);
    }

    MG_THROW()

    return true;
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks if the specified pathname is a file.
/// </summary>
///----------------------------------------------------------------------------

bool MgFileUtil::IsFile(CREFSTRING pathname)
{
    bool result = false;
    struct _stat64 statInfo;

    if (GetFileStatus(pathname, statInfo))
    {
        result = (S_IFREG == (S_IFMT & statInfo.st_mode));
    }

    return result;
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes a file.
/// </summary>
///
/// <exceptions>
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgFileIoException
/// </exceptions>
///----------------------------------------------------------------------------

void MgFileUtil::DeleteFile(CREFSTRING pathname, bool strict)
{
    MG_TRY()

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    if (!PathnameExists(pathname))
    {
        if (strict)
        {
            MgStringCollection arguments;
            arguments.Add(pathname);

            throw new MgFileNotFoundException(L"MgFileUtil.DeleteFile",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        return;
    }

    int errCode = ACE_OS::unlink(MG_WCHAR_TO_TCHAR(pathname));

    if (0 != errCode && strict)
    {
        errCode = errno; // This is not thread safe - requires guard
        MgStringCollection arguments;
        arguments.Add(pathname);

        switch (errCode)
        {
            case ENOENT: // PathnameExists is more reliable. It gets called above, so this error should not be detected.
            case EACCES:
            default:
                MG_FILE_IO_EXCEPTION_ADD_ERRNO();
                throw new MgFileIoException(L"MgFileUtil.DeleteFile",
                    __LINE__, __WFILE__, &arguments, L"MgFileIoErrNo", &errNoArgs);
                break;
        }
    }

    MG_CATCH_AND_THROW(L"MgFileUtil.DeleteFile")
}

///----------------------------------------------------------------------------
/// <summary>
/// Copies a file from one location to another.
/// </summary>
///
/// <exceptions>
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgFileNotFoundException
/// MgFileIoException
/// </exceptions>
///----------------------------------------------------------------------------

void MgFileUtil::CopyFile(CREFSTRING sourcePathname, CREFSTRING destPathname,
    bool overwrite)
{
    MG_TRY()

    CHECKARGUMENTEMPTYSTRING(sourcePathname, L"MgFileUtil.CopyFile");
    CHECKARGUMENTEMPTYSTRING(destPathname, L"MgFileUtil.CopyFile");

    if (_wcsicmp(sourcePathname.c_str(), destPathname.c_str()) == 0)
    {
        MgStringCollection arguments;
        arguments.Add(destPathname);

        throw new MgDuplicateFileException(L"MgFileUtil.CopyFile",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if (overwrite)
    {
        DeleteFile(destPathname);
    }
    else if (PathnameExists(destPathname))
    {
        MgStringCollection arguments;
        arguments.Add(destPathname);

        throw new MgDuplicateFileException(L"MgFileUtil.CopyFile",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    Ptr<MgByteSource> byteSource = new MgByteSource(sourcePathname);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    MgByteSink byteSink(byteReader);
    byteSink.ToFile(destPathname);

    MG_CATCH_AND_THROW(L"MgFileUtil.CopyFile")
}

///----------------------------------------------------------------------------
/// <summary>
/// Renames a file.
/// </summary>
///
/// <exceptions>
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgFileNotFoundException
/// MgFileIoException
/// </exceptions>
///----------------------------------------------------------------------------

void MgFileUtil::RenameFile(CREFSTRING path, CREFSTRING oldFileName,
    CREFSTRING newFileName, bool overwrite)
{
    MG_TRY()

    CHECKARGUMENTEMPTYSTRING(path, L"MgFileUtil.RenameFile");
    CHECKARGUMENTEMPTYSTRING(oldFileName, L"MgFileUtil.RenameFile");
    CHECKARGUMENTEMPTYSTRING(newFileName, L"MgFileUtil.RenameFile");

    STRING oldPathname = path;
    AppendSlashToEndOfPath(oldPathname);
    STRING newPathname = oldPathname;

    oldPathname += oldFileName;
    newPathname += newFileName;

    RenameFile(oldPathname, newPathname, overwrite);

    MG_CATCH_AND_THROW(L"MgFileUtil.RenameFile")
}

///----------------------------------------------------------------------------
/// <summary>
/// Renames a file.
/// </summary>
///
/// <exceptions>
/// MgNullArgumentException
/// MgInvalidArgumentException
/// MgFileNotFoundException
/// MgFileIoException
/// </exceptions>
///----------------------------------------------------------------------------

void MgFileUtil::RenameFile(CREFSTRING oldPathname, CREFSTRING newPathname,
    bool overwrite)
{
    MG_TRY()

    CHECKARGUMENTEMPTYSTRING(oldPathname, L"MgFileUtil.RenameFile");
    CHECKARGUMENTEMPTYSTRING(newPathname, L"MgFileUtil.RenameFile");

    if (_wcsicmp(oldPathname.c_str(), newPathname.c_str()) == 0)
    {
        MgStringCollection arguments;
        arguments.Add(newPathname);

        throw new MgDuplicateFileException(L"MgFileUtil.RenameFile",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    if (overwrite)
    {
        DeleteFile(newPathname);
    }
    else if (PathnameExists(newPathname))
    {
        MgStringCollection arguments;
        arguments.Add(newPathname);

        throw new MgDuplicateFileException(L"MgFileUtil.RenameFile",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

#ifdef _WIN32
    int errCode = ::_wrename(oldPathname.c_str(), newPathname.c_str());
#else
    int errCode = ACE_OS::rename(MG_WCHAR_TO_TCHAR(oldPathname), MG_WCHAR_TO_TCHAR(newPathname));
#endif

    if (0 != errCode)
    {
        errCode = errno; // This is not thread safe - requires guard
        MgStringCollection arguments;

        switch (errCode)
        {
            case ENOENT: // File or path specified by oldname not found.
                arguments.Add(oldPathname);
                throw new MgFileNotFoundException(L"MgFileUtil.RenameFile",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
                break;

            case EINVAL: // Name contains invalid characters.
            {
                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(oldPathname);
                arguments.Add(L"2");
                arguments.Add(newPathname);

                MgStringCollection whyArguments;
                whyArguments.Add(L"");

                throw new MgInvalidArgumentException(L"MgFileUtil.RenameFile",
                    __LINE__, __WFILE__, &arguments, L"MgStringContainsReservedCharacters", &whyArguments);
            }
                break;

            case EACCES: // File or directory specified by newname already
                         // exists or could not be created (invalid path);
                         // or oldname is a directory and newname specifies a
                         // different path.
            default:
                arguments.Add(newPathname);
                MG_FILE_IO_EXCEPTION_ADD_ERRNO();
                throw new MgFileIoException(L"MgFileUtil.RenameFile",
                    __LINE__, __WFILE__, &arguments, L"MgFileIoErrNo", &errNoArgs);
                break;
        }
    }

    MG_CATCH_AND_THROW(L"MgFileUtil.RenameFile")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the TempPath defined in the configuration file. If the TempPath doesn't
/// exist, then create it.
/// </summary>
///
/// <returns>
/// Returns the TempPath defined in the configuration file.
/// </returns>
///----------------------------------------------------------------------------

STRING MgFileUtil::GetTempPath()
{
    STRING tempPathname;

    MG_TRY()

    // TODO: Server Drawing Service GETDRAWINGLAYER API does not work well
    // if using Mg temporary directory.
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetStringValue(
        MgFoundationConfigProperties::GeneralPropertiesSection,
        MgFoundationConfigProperties::GeneralPropertyTempPath,
        tempPathname,
        MgFoundationConfigProperties::DefaultGeneralPropertyTempPath);

    CreateDirectory(tempPathname);
    AppendSlashToEndOfPath(tempPathname);

    MG_CATCH_AND_THROW(L"MgFileUtil.GetTempPath")

    return tempPathname;
}

///----------------------------------------------------------------------------
/// <summary>
/// Generates a temporary file folder using a UUID in the TempPath defined in
/// the configuration file.
/// </summary>
///
/// <returns>
/// Returns full path name of the temporary path.
/// </returns>
///----------------------------------------------------------------------------

STRING MgFileUtil::GenerateTempPath()
{
    STRING tempPathname;

    MG_TRY()

    tempPathname = GetTempPath();

    STRING uuid;
    MgUtil::GenerateUuid(uuid);
    tempPathname.append(uuid);

    CreateDirectory(tempPathname);

    MG_CATCH_AND_THROW(L"MgFileUtil.GenerateTempPath")

    return tempPathname;
}

///----------------------------------------------------------------------------
/// <summary>
/// Generates a temporary file name. File will reside in the TempPath defined
/// in the configuration file. File name will be a UUID so it should be unique.
/// </summary>
///
/// <returns>
/// Full path name of the temporary file.
/// </returns>
///
/// <exceptions>
/// MgFileIoException
/// </exceptions>
///----------------------------------------------------------------------------

STRING MgFileUtil::GenerateTempFileName(bool useMgTempPath,
    CREFSTRING prefix, CREFSTRING extension)
{
    STRING tempPathname;

    MG_TRY()

    if (useMgTempPath)
    {
        tempPathname = GetTempPath();
        if (!prefix.empty())
        {
            tempPathname.append(prefix);
            tempPathname.append(L"-");
        }

        STRING uuid;

        MgUtil::GenerateUuid(uuid);
        tempPathname.append(uuid);
    }
    else
    {
        // Note that ::tempnam uses the TMP environment variable as the path
        // to the temporary file. Alternately, ACE_OS::mkstemp() could be used.
        // Add current thread id to the mix to ensure that the name is unique.

        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, L""));
        string mbPrefix;
        ACE_thread_t threadId = ACE_OS::thr_self();
        MgUtil::Int32ToString(threadId, mbPrefix);
        mbPrefix.append( MgUtil::WideCharToMultiByte(prefix) );

        char* tempFileName = ::tempnam(NULL, mbPrefix.c_str());

        if (0 == tempFileName)
        {
            throw new MgTemporaryFileNotAvailableException(
                L"MgFileUtil.GenerateTempFileName", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        string tempStr = tempFileName;
        ::free(tempFileName);

        MgUtil::MultiByteToWideChar(tempStr, tempPathname);
    }

    if (!extension.empty())
    {
        if (!BeginsWithDot(extension))
        {
            tempPathname.append(L".");
        }

        tempPathname.append(extension);
    }

    MG_CATCH_AND_THROW(L"MgFileUtil.GenerateTempFileName")

    return tempPathname;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieves the time that the specified file was created.
///
MgDateTime MgFileUtil::GetFileCreationTime(CREFSTRING pathname)
{
    struct _stat64 statInfo;

    if (GetFileStatus(pathname, statInfo, true))
    {
        return MgDateTime(statInfo.st_ctime);
    }

    return MgDateTime();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieves the time that the specified file was last modified.
///
MgDateTime MgFileUtil::GetFileModificationTime(CREFSTRING pathname)
{
    struct _stat64 statInfo;

    if (GetFileStatus(pathname, statInfo, true))
    {
        return MgDateTime(statInfo.st_mtime);
    }

    return MgDateTime();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieves the size (in bytes) of the specified file.
///
INT64 MgFileUtil::GetFileSize(CREFSTRING pathname)
{
    struct _stat64 statInfo;

    if (GetFileStatus(pathname, statInfo, true))
    {
        return (INT64)statInfo.st_size;
    }

    return 0;
}

///----------------------------------------------------------------------------
/// <summary>
/// Determines if the specified file is in use.
/// </summary>
///
/// <param name="pathname">
/// Full path name of the file to check.
/// </param>
///
/// <returns>
/// true if the specified file is in use, false otherwise.
/// </returns>
///----------------------------------------------------------------------------

bool MgFileUtil::IsFileInUse(CREFSTRING pathname)
{
    bool bResult = false;

    MG_TRY()

    CHECKARGUMENTEMPTYSTRING(pathname, L"MgFileUtil.IsFileInUse");

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    FILE* file = NULL;

#ifdef _WIN32
    file = _wfsopen(pathname.c_str(), L"r+", _SH_DENYRW);
#else
    file = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(pathname), ACE_TEXT("r+"));
#endif

    if (file)
    {
#ifndef _WIN32
        // Try to lock the file
        int fd = fileno(file);
        int nResult = flock(fd, LOCK_EX|LOCK_NB);
        if(0 != nResult)
        {
            bResult = true;
        }
#endif
        // Close the file
        fclose(file);
    }
    else
    {
        // If the file doesn't exist then it is not an error
        int errCode = errno; // This is not thread safe - requires guard
        if(ENOENT != errCode)
        {
            bResult = true;
        }
    }

    MG_CATCH_AND_THROW(L"MgFileUtil.IsFileInUse")

    return bResult;
}

///----------------------------------------------------------------------------
/// <summary>
/// Locks the specified file.
/// On Windows, always returns true.
/// On Linux, returns true if the file now has an advisory by the calling
/// process, false otherwise.
// </summary>
///----------------------------------------------------------------------------

bool MgFileUtil::LockFile(CREFSTRING pathname)
{
    bool bResult = true;

#ifndef _WIN32
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    FILE* file = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(pathname), ACE_TEXT("r+"));
    if (file)
    {
        // Try to lock the file
        int fd = fileno(file);
        int nResult = flock(fd, LOCK_EX|LOCK_NB);
        if(0 != nResult)
        {
            bResult = false;
        }
    }
#endif

    return bResult;
}

///----------------------------------------------------------------------------
/// <summary>
/// Verifies if the specified file can be safely opened.
/// </summary>
///
/// <param name="pathname">
/// Full path name of the file to verify.
/// </param>
///
/// <returns>
/// true if the specified file can be safely opened, false otherwise.
/// </returns>
///----------------------------------------------------------------------------

bool MgFileUtil::VerifySafeFileAccess(CREFSTRING pathname)
{
    bool safe = true;

    if (IsFileInUse(pathname))
    {
        safe = false;
    }
#ifndef _WIN32
    else if (!MgFileUtil::LockFile(pathname))
    {
        safe = false;
    }
#endif

    return safe;
}
