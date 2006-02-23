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

#ifndef MGFILEUTIL_H_
#define MGFILEUTIL_H_

/// \ingroup NOT_EXPOSED_module

// This allows to use the same function name for both Windows and Linux.

#ifdef WIN32
    #include <share.h>
#else
    #include <sys/file.h>

    #define _rmdir rmdir
    #define _stat stat
#endif

class MG_SERVICE_API ACE_Recursive_Thread_Mutex;

class MG_SERVICE_API MgFileUtil
{
    DECLARE_CLASSNAME(MgFileUtil)

    /// Constructors/Destructor

    public:

    MgFileUtil();
    virtual ~MgFileUtil();

    private:

    /// Unimplemented Methods

    MgFileUtil(const MgFileUtil& util);
    MgFileUtil& operator=(const MgFileUtil& util);

/// Methods

    public:

    /// Helper Methods

    static bool BeginsWithDot(CREFSTRING str);
    static bool EndsWithSlash(CREFSTRING str);
    static bool EndsWithExtension(CREFSTRING pathname,
        CREFSTRING extension);

    static void AppendSlashToEndOfPath(REFSTRING path);
    static void RemoveSlashFromEndOfPath(REFSTRING path);

    static bool GetFileStatus(CREFSTRING pathname, struct _stat& statInfo,
        bool strict = false);
    static bool PathnameExists(CREFSTRING pathname);

    /// Directory related methods

    static bool IsDirectory(CREFSTRING pathname);
    static void CreateDirectory(CREFSTRING path, bool strict = false);
    static void DeleteDirectory(CREFSTRING path, bool recursive = true,
        bool strict = false);
        static bool CleanDirectory(CREFSTRING path, bool recursive = true,
        bool strict = false);
    static STRING ChangeDirectory(CREFSTRING path);

    /// File related methods

    static bool IsFile(CREFSTRING pathname);
    static void DeleteFile(CREFSTRING pathname, bool strict = false);
    static void CopyFile(CREFSTRING sourcePathname,
        CREFSTRING destPathname, bool overwrite = false);
    static void RenameFile(CREFSTRING path, CREFSTRING oldFileName,
        CREFSTRING newFileName, bool overwrite = false);
    static void RenameFile(CREFSTRING oldPathName, CREFSTRING newPathName,
        bool overwrite = false);

    static STRING GenerateTempFileName(bool useMgTempPath = true,
        CREFSTRING prefix = L"", CREFSTRING extension = L"");

    static STRING GetLastModifiedDate(CREFSTRING pathname);
    static INT32 GetFileSizeInBytes(CREFSTRING pathname);

    static bool IsFileInUse(CREFSTRING pathname);
    static bool LockFile(CREFSTRING pathname);
    static bool VerifySafeFileAccess(CREFSTRING pathname);

/// Data Members

    private:

    static ACE_Recursive_Thread_Mutex sm_mutex;
};

/// Inline Methods

#endif
