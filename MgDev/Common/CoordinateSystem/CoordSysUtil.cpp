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
// This implementation file provides some useful utility functions and classes.

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CriticalSection.h"

#include <limits.h>    //for INT_MAX
#include <assert.h>
#include <math.h>        //for fabs()
#ifdef _WIN32
#include <io.h>            //for _tfindfirst()
#endif
#include <locale.h>
#ifdef _WIN32
#include <tchar.h>
#endif
#include "CoordSysUtil.h"

//Local utility function used by SetDictionaryFileNames.
//
bool IsValidDictionaryName(CREFSTRING sName)
{
    if (sName.empty())
    {
        //Null or empty strings are okay here
        return true;
    }

#ifdef _WIN32
    //Make sure it's not too long
    if (_tcslen(sName.c_str()) > (_MAX_FNAME + _MAX_EXT))
    {
        return false;
    }

    //Make sure the string contains file & extension only
    //(no path information)
    wchar_t szDrive[_MAX_DRIVE] = {0};
    wchar_t szDir[_MAX_DIR] = {0};
    _tsplitpath(sName.c_str(), szDrive, szDir, NULL, NULL);
    if ((_tcslen(szDrive) > 0) || (_tcslen(szDir) > 0))
    {
        return false;
    }
#endif

    //Okay, everything looks kosher.
    return true;
}

//Quick-and-dirty utility function to convert an EFileValidity
//error code into an appropriate exception.
//
void ThrowFileError(const wchar_t* wszMethodName, CREFSTRING sPathName, EFileValidity reason)
{
    assert(kFileIsValid!=reason);
    switch (reason)
    {
    case kFileIsValid:
        break;
    case kFileInvalidEmptyString:
        throw new MgInvalidArgumentException(wszMethodName, __LINE__, __WFILE__, NULL, L"", NULL);
        break;
    case kFileInvalidNotADir:
    case kFileInvalidIsADir:
        {
        MgStringCollection arguments;
        arguments.Add(sPathName);
        throw new MgFileIoException(wszMethodName, __LINE__, __WFILE__, &arguments, L"MgInvalidArgumentException", NULL);
        }
        break;
    case kFileInvalidPath:
    case kFileInvalidDoesNotExist:
        {
        MgStringCollection arguments;
        arguments.Add(sPathName);
        throw new MgFileIoException(wszMethodName, __LINE__, __WFILE__, &arguments, L"MgFileNotFoundException", NULL);
        }
        break;
    case kFileInvalidCantRead:
    case kFileInvalidCantWrite:
        throw new MgFileIoException(wszMethodName, __LINE__, __WFILE__, NULL, L"MgCoordinateSystemCatalogFileLockedException", NULL);
        break;
    default:
        assert(0);
        throw new MgUnclassifiedException(wszMethodName, __LINE__, __WFILE__, NULL, L"", NULL);
        break;
    }
}

//Tests a string for legality, based on size.  Returns false
//if the string is a null pointer, or if it is greater than
//nMaxSize bytes (including terminating null) when converted
//to a regular character string.
//
bool IsLegalString(const wchar_t* kpStr, UINT32 unMaxSize)
{
    //According to the rules of BSTR, a null BSTR is
    //equivalent to an empty string.  So we'll allow
    //a null pointer there.
    if (NULL == kpStr) return (unMaxSize > 0);

    //How long would it be as an MBCS string?
    size_t nChars = wcstombs(NULL, kpStr, INT_MAX);
    if (nChars < 0) return false;

    return (nChars < unMaxSize);
}

//Tests two floating-point numbers to see if they're "equal"
//to within a very tiny value.  Returns true if so.  This
//function is used for asserts in various places in the
//code to make sure calculated results match up with what
//they're supposed to.
//
bool FloatEqual(double d1, double d2, double dEpsilon)
{
    return (fabs(d1-d2) < dEpsilon);
}

//Gets the mode with which to open a file,
//given a INT32 value.
void GetFileModes(CsDictionaryOpenMode nOpenMode, char* szMode)
{
    assert((Read == nOpenMode) || (Write == nOpenMode));
    if (Write == nOpenMode)
    {
        strcpy(szMode, "r+b");
    }
    else
    {
        strcpy(szMode, "rb");
    }
}

//This function tests whether a specified file is "valid," allowing the
//caller to specify various conditions for validity (must the file exist?
//must it be a directory?  must it be writable?)
//
//The function returns true if valid, false if not.
//
//Validity conditions:
//
//    If the filename includes a path, then it must be a valid path
//    (i.e., the directory where the file or directory is located
//    must exist).
//
//    If bExists is true, the file or directory itself must exist.
//    (This flag is redundant and assumed true if bCanWrite is
//    false; we can't read a nonexistent object!)
//
//    If bIsDir is true, the specified file must be a directory.
//    If bIsDir is false, the specified file must NOT be a directory.
//
//    If bCanWrite is true, then the specified file or directory
//    must be "writable".  That means that the directory it's located
//    in must not be read-only, and if the file or directory
//    already exists, then it must not be read-only either.
//
//    TODO If the pReason pointer is not NULL, then if the function fails,
//    it will store the string ID of the reason for failure there.
//
bool ValidateFile(
    const wchar_t *kpFile, //the file name to validate
    bool bExists,    //if true, function fails if file doesn't exist
    bool bIsDir,    //if true, function fails if file isn't a dir
    bool bCanWrite,    //if true, function fails if file isn't writable
    EFileValidity *pReason)    //if not NULL, stores result code
{
    SmartCriticalClass critical(true);

    //If we are requesting validation for read access, then the
    //file must exist to be valid.  Nonexistent items aren't readable.
    if (!bCanWrite) bExists = true;

#ifdef _WIN32
    //Decide what functions we'll use for testing read/write ability
    typedef bool (*FileTestFunc)(const wchar_t *);
    FileTestFunc CanReadItem, CanWriteItem;
    if (bIsDir)
    {
        CanReadItem = CanReadFromDir;
        CanWriteItem = CanWriteToDir;
    }
    else
    {
        CanReadItem = CanReadFromFile;
        CanWriteItem = CanWriteToFile;
    }
#endif

    if ((NULL == kpFile) || (0 == wcslen(kpFile)))
    {
        //We were given an empty string.  Obviously, this can't
        //be a valid file or directory.
        if (NULL != pReason)
        {
            *pReason = kFileInvalidEmptyString;
        }
        return false;
    }

#ifdef _WIN32
    wchar_t szDrive[_MAX_DRIVE],
          szDir[_MAX_DIR],
          szFname[_MAX_FNAME],
          szExt[_MAX_EXT],
          szDirPath[_MAX_PATH];

    UINT32 dwResult;

    //If kpFile includes drive or path information,
    //make sure it's in an existing directory
    _tsplitpath(kpFile, szDrive, szDir, szFname, szExt);
    if (0 == _tcslen(szDir))
    {
        //No directory was specified, so assume current directory.
        _tcscpy(szDir, L".\\");//NOXLATE
    }
    _tmakepath(szDirPath, szDrive, szDir, NULL, NULL);
    dwResult = GetFileAttributes(szDirPath);
    if (0xFFFFFFFF == dwResult)
    {
        //Nonexistent directory!  Example:  Specified
        //path was C:\foo\blah.txt, and there is no
        //directory C:\foo.
        if (NULL != pReason)
        {
            *pReason = kFileInvalidPath;
        }
        return false;
    }
    if (!(FILE_ATTRIBUTE_DIRECTORY & dwResult))
    {
        //Not a directory!  Example:  Specified path
        //was C:\foo\blah.txt, and it turns out
        //that C:\foo exists but is a file (not a
        //directory).  Kinda weird, but we have to
        //consider the possibility.
        if (NULL != pReason)
        {
            *pReason = kFileInvalidPath;
        }
        return false;
    }

    //See if the file exists.  If bExists is true, fail if
    //it doesn't exist.
    dwResult = GetFileAttributes(kpFile);
    bool bFileExists = (0xFFFFFFFF != dwResult);
    if (bExists && !bFileExists)
    {
        //Doesn't exist!
        if (NULL != pReason)
        {
            *pReason = kFileInvalidDoesNotExist;
        }
        return false;
    }

    //If the file actually exists, make sure that whether it's a
    //directory or a file matches what the user specified.
    if (bFileExists)
    {
        if (bIsDir)
        {
            //Must be a directory.  Fail if it's a file.
            if (!(FILE_ATTRIBUTE_DIRECTORY & dwResult))
            {
                //Not a directory!
                if (NULL != pReason)
                {
                    *pReason = kFileInvalidNotADir;
                }
                return false;
            }
        }
        else
        {
            //Must be a file.  Fail if it's a directory.
            if (FILE_ATTRIBUTE_DIRECTORY & dwResult)
            {
                //Not a file!
                if (NULL != pReason)
                {
                    *pReason = kFileInvalidIsADir;
                }
                return false;
            }
        }
    }

    //If bCanWrite is true, make sure we can write to the
    //file, and fail if we can't.
    bool bOk = true;

    if (bCanWrite)
    {
        if (bFileExists)
        {
            //The file already exists.
            bOk = CanWriteItem(kpFile);
        } //if the file already exists
        else
        {
            //The file doesn't already exist, so just make sure
            //we can write to the directory it's in.
            bOk = CanWriteItem(szDirPath);
        } //if the file doesn't already exist

        if (!bOk)
        {
            //We don't have write access to the item!
            if (NULL != pReason)
            {
                *pReason = kFileInvalidCantWrite;
            }
            return false;
        }
    } //if the file must be writable
    else
    {
        //The file doesn't have to be writable, but it has
        //to be readable.  The file must exist; if it doesn't
        //already exist, it's ipso facto not readable.
        if (!CanReadItem(kpFile))
        {
            //We don't have read access to the item!
            if (NULL != pReason)
            {
                *pReason = kFileInvalidCantRead;
            }
            return false;
        }    //if we can't read from the dir
    } //if the file must be readable, but doesn't have to be writable

#else
    UINT32 dwResult;


    //See if the file exists.  If bExists is true, fail if
    //it doesn't exist.
    bool bFileExists = MgFileUtil::PathnameExists(kpFile);
    if (bExists && !bFileExists)
    {
        if (NULL != pReason)
        {
            *pReason = kFileInvalidPath;
        }
    }


    //If the file actually exists, make sure that whether it's a
    //directory or a file matches what the user specified.
    if (bFileExists)
    {
        if (bIsDir)
        {
            // Must be a directory.  Fail if its a file.
            if (!MgFileUtil::IsDirectory(kpFile))
            {
                // Not a directory!
                if (NULL != pReason)
                {
                    *pReason = kFileInvalidNotADir;
                }
                return false;
            }
        }
        else
        {
            // Must be a file.  Fail if it's a directory.
            if (!MgFileUtil::IsFile(kpFile))
            {
                // Not a file!
                if (NULL != pReason)
                {
                    *pReason = kFileInvalidIsADir;
                }
                return false;
            }
        }
    }

    // If bCanWrite is true, make sure we can write to the
    // file, and fail if we can't.
    bool bOk = true;

    if (bCanWrite)
    {
        if (bFileExists)
        {
            // The file already exists
            struct _stat statInfo;

            MgFileUtil::GetFileStatus(kpFile, statInfo);
            if (S_IWUSR & (statInfo.st_mode))
            {
                bOk = true;
            }
        }
        else
        {
            // File doesn't already exist, so just make sure
            //we can write to the directory it's in.
            try
            {
                MgFileUtil::CreateDirectory(kpFile);
                MgFileUtil::DeleteDirectory(kpFile);
                bOk = true;
            }
            catch (MgException *e)
            {
                SAFE_RELEASE(e);
                bOk = false;
            }
        }

        if (!bOk)
        {
            // We don't have write access to the item!
            if (NULL != pReason)
            {
                *pReason = kFileInvalidCantWrite;
            }
            return false;
        }
    }  // if the file must be writable
    else
    {
        // The file doesn't have to be writable, but it has
        // to be readable.  The file must exist; if it doesn't
        // already exist, it's ipso facto no readable.
        if (bFileExists)
        {
            // The file already exists
            struct _stat statInfo;

            MgFileUtil::GetFileStatus(kpFile, statInfo);
            if (S_IRUSR & (statInfo.st_mode))
            {
                bOk = true;
            }
        }
        if (!bFileExists || !bOk)
        {
            // We don't have read access to the item!
            if (NULL != pReason)
            {
                *pReason = kFileInvalidCantRead;
            }
            return false;
        }  // if we can't read from the dir
    }  // if the file must be readable, but doesn't have to be writeable
#endif

    // If we made it this far, it must be valid!
    if (NULL != pReason)
    {
        *pReason = kFileIsValid;
    }

    return true;
}





//This function tests whether we have write access to the
//contents of a directory.  It does this by attempting to
//create a temporary file therein, and seeing what happens.
//
//(Unfortunately, we can't test a directory's writability
//just by making a GetAttributes() call and checking whether
//its read-only flag is set.  Doing so would only tell us
//wether we have permission to delete/rename the directory
//itself, not whether we have permission to modify its
//contents.  Attempting to create a temporary file therein
//is the only test I know to find out what we need.)
//
bool
CanWriteToDir(const wchar_t *kpDir)
{
#ifdef _WIN32
    // TODO: Check if _WIN32 code should be replaced with MgFileUtil methods
    wchar_t szPath[_MAX_PATH];
    UINT32 unResult;

    unResult = GetTempFileName(kpDir, _T(/*NOXLATE*/"tmp"), 0, szPath);
    if (unResult == 0)
    {
        //Couldn't create a temporary file, which means
        //that we don't have write access to the directory.
        return false;
    }

    //Successfully created the file!  Delete it to clean
    //up after ourselves, then return success.
    DeleteFile(szPath);
#else
    // Used MgFileUtil methods instead
#endif

    return true;
}



//This function tests whether we have read access to the
//contents of a directory.  Note:  This function counts on the
//fact that the path is a *directory* path, terminated in backslash.
//
bool
CanReadFromDir(const wchar_t *kpFile)
{
#ifdef _WIN32
    //Try to use _tfindfirst() to get a handle.
    //If we have read access to the directory, this
    //is always guaranteed to succeed because "."
    //will be present even if it's an empty directory
    wchar_t szPath[_MAX_PATH],
          szDrive[_MAX_DRIVE],
          szDir[_MAX_DIR],
          szFname[_MAX_FNAME],
          szExt[_MAX_EXT];
    _tsplitpath(kpFile, szDrive, szDir, szFname, szExt);
    assert(_tcslen(szFname) == 0);
    assert(_tcslen(szExt) == 0);
    _tmakepath(
        szPath,
        szDrive,
        szDir,
        L"*", //NOXLATE
        L"");//NOXLATE
    _wfinddata_t info;
    intptr_t lHandle = _tfindfirst(szPath, &info);
    if (-1 == lHandle)
    {
        //Nope, no read access.
        return false;
    }

    //Looks like we have read access!
    _findclose(lHandle);
#else
    //
    // Used MgFileUtil methods instead.
    //
#endif
    return true;
}



//This function tests whether we have write access to the
//specified file (which must exist).  It does this by
//attempting to open the file for exclusive write access.
//
//If successful, the function closes the file before returning
//true.  This could conceivably lead to race conditions if
//something happens to remove write access to the file between
//the time that this function is called and the time that
//it's actually attempted to be used.
//
bool
CanWriteToFile(const wchar_t *kpFile)
{
#ifdef _WIN32
    HANDLE hFile;
    hFile = CreateFile(
                kpFile, //the file to open
                GENERIC_READ | GENERIC_WRITE, //read/write access
                0,    //not shared
                NULL,    //security attributes not used in Win95
                OPEN_EXISTING,    //file must exist
                FILE_ATTRIBUTE_NORMAL,
                NULL    //must be NULL in Win95
            );
    if (INVALID_HANDLE_VALUE == hFile)
    {
        //Couldn't open it, so either it doesn't exist
        //or it's not writable.
        return false;
    }
    else
    {
        //It's writable!  Close it and return.
        CloseHandle(hFile);
        return true;
    }
#else
    int implement_code_for_linux_platform;
#endif
    return false;
}


//This function returns whether we have read access
//to the specified file (which must exist).  It does this
//by attempting to open the file for shared read access.
//
//If successful, the function closes the file before returning
//true.  This could conceivably lead to race conditions if
//something happens to remove read access to the file between
//the time that this function is called and the time that
//it's actually attempted to be used.
//
bool
CanReadFromFile(const wchar_t *kpFile)
{
#ifdef _WIN32
    HANDLE hFile;
    hFile = CreateFile(
                kpFile, //the file to open
                GENERIC_READ, //read access
                FILE_SHARE_READ,    //not shared
                NULL,    //security attributes not used in Win95
                OPEN_EXISTING,    //file must exist
                FILE_ATTRIBUTE_NORMAL,
                NULL    //must be NULL in Win95
            );
    if (INVALID_HANDLE_VALUE == hFile)
    {
        //Couldn't open it, so either it doesn't exist
        //or it's not readable.
        return false;
    }
    else
    {
        //It's readable!  Close it and return.
        CloseHandle(hFile);
        return true;
    }
#else
    //
    // Replaced with MgFileUtil methods
    //
#endif
    return false;
}

//*****************************************************************************
// Use _T("C") for ANSI C, numeric separator is a decimal point
//
CCsNumericDotLocale::CCsNumericDotLocale()
{
    // Save the original locale setting.
    m_pszCurLocale = ::strdup(::setlocale(LC_NUMERIC, NULL));
    // Use the "C" locale for floating-point conversions.
    ::setlocale(LC_NUMERIC, "C");
}

//*****************************************************************************
CCsNumericDotLocale::~CCsNumericDotLocale()
{
    if (m_pszCurLocale)
    {
        // Restore the original locale setting.
        ::setlocale(LC_NUMERIC, m_pszCurLocale);

        // Release the resource allocated for the previously saved locale.
        ::free(m_pszCurLocale);
    }
}

//*****************************************************************************
INT64 GetAvailableMemory()
{
#ifdef _WIN32
    // Size of available memory in bytes, with an additional
    // 200Mb subtracted to take into account heap fragmentation,
    // and a 100Mb buffer for smaller allocations that may happen in
    // between checks.
    MEMORYSTATUSEX  statex;
    statex.dwLength = sizeof (statex);
    INT64 maxAvailable = 0L;
    if(GlobalMemoryStatusEx (&statex)) {
        maxAvailable = min(statex.ullAvailPhys + statex.ullAvailPageFile, statex.ullAvailVirtual);
        maxAvailable = (maxAvailable > 300000000L) ? maxAvailable - 300000000L : 0L;
    }
    return maxAvailable;
#else // LINUX
    throw new MgNotImplementedException(L"GetAvailableMemory", __LINE__, __WFILE__, NULL, L"", NULL);
#endif
}
