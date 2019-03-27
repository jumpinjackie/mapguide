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

#include "ResourceServiceDefs.h"
#include "ZipFileWriter.h"

//Bulk of this code copied from minizip.c reference implementation

#ifdef _WIN32
uLong filetime(const wchar_t* f /* name of file to get info on */, tm_zip* tmzip /* return value: access, modific. and creation times */, uLong* dt /* dostime */)
{
    int ret = 0;
    {
        FILETIME ftLocal;
        HANDLE hFind;
        WIN32_FIND_DATA  ff32;

        hFind = FindFirstFile(f, &ff32);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            FileTimeToLocalFileTime(&(ff32.ftLastWriteTime), &ftLocal);
            FileTimeToDosDateTime(&ftLocal, ((LPWORD)dt) + 1, ((LPWORD)dt) + 0);
            FindClose(hFind);
            ret = 1;
        }
    }
    return ret;
}
#else
uLong filetime(char* f /* name of file to get info on */, tm_zip* tmzip /* return value: access, modific. and creation times */, uLong* dt /* dostime */)
{
    int ret = 0;
    struct stat s;        /* results of stat() */
    struct tm* filedate;
    time_t tm_t = 0;

    if (strcmp(f, "-") != 0)
    {
        char name[PATH_MAX + 1];
        int len = strlen(f);
        if (len > PATH_MAX)
            len = PATH_MAX;

        strncpy(name, f, PATH_MAX - 1);
        /* strncpy doesnt append the trailing NULL, of the string is too long. */
        name[PATH_MAX] = '\0';

        if (name[len - 1] == '/')
            name[len - 1] = '\0';
        /* not all systems allow stat'ing a file with / appended */
        if (stat(name, &s) == 0)
        {
            tm_t = s.st_mtime;
            ret = 1;
        }
    }
    filedate = localtime(&tm_t);

    tmzip->tm_sec = filedate->tm_sec;
    tmzip->tm_min = filedate->tm_min;
    tmzip->tm_hour = filedate->tm_hour;
    tmzip->tm_mday = filedate->tm_mday;
    tmzip->tm_mon = filedate->tm_mon;
    tmzip->tm_year = filedate->tm_year;

    return ret;
}
#endif

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgZipFileWriter::MgZipFileWriter(CREFSTRING filePath)
    : m_zip(NULL)
{
    std::string mbFilePath = MgUtil::WideCharToMultiByte(filePath);
    m_zip = zipOpen(mbFilePath.c_str(), 0);
    if (!m_zip)
    {
        throw new MgInvalidDwfPackageException(L"MgZipFileWriter.MgZipFileWriter", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgZipFileWriter::~MgZipFileWriter()
{
    if (m_zip)
    {
        int r = zipClose(m_zip, "Created by MapGuide Server"); //NOXLATE
#ifdef _DEBUG
        if (r != ZIP_OK)
        {
            throw new MgInvalidDwfPackageException(L"MgZipFileWriter.~MgZipFileWriter", __LINE__, __WFILE__, NULL, L"", NULL);
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Adds a byte buffer to the zip file.
///
void MgZipFileWriter::AddArchive(CREFSTRING filePath, MgByteReader* byteReader)
{
    ACE_ASSERT(!filePath.empty());

    char* mbFilePath = MgUtil::WideCharToMultiByte(filePath.c_str());
    MG_RESOURCE_SERVICE_TRY()

    CHECKARGUMENTNULL(byteReader, L"MgZipFileWriter.AddArchive");

    int err = UNZ_OK;

    zip_fileinfo zi;
    unsigned long crcFile = 0;
    const char* password = NULL;
    int opt_compress_level = Z_DEFAULT_COMPRESSION;

    zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
        zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;

#ifdef _WIN32
    filetime(filePath.c_str(), &zi.tmz_date, &zi.dosDate);
#else
    filetime(mbFilePath, &zi.tmz_date, &zi.dosDate);
#endif

    err = zipOpenNewFileInZip3(m_zip, mbFilePath, &zi,
        NULL, 0, NULL, 0, NULL /* comment*/,
        (opt_compress_level != 0) ? Z_DEFLATED : 0,
        opt_compress_level, 0,
        /* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
        -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
        password, crcFile);

    if (err != ZIP_OK)
    {
        throw new MgInvalidDwfPackageException(L"MgZipFileWriter.AddArchive", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    const INT32 length = 4096;
    BYTE buffer[length] = { 0 };
    INT32 numBytes = 0;
    do
    {
        numBytes = byteReader->Read(buffer, length);
        err = zipWriteInFileInZip(m_zip, buffer, numBytes);
        if (err < 0)
        {
            throw new MgInvalidDwfPackageException(L"MgZipFileWriter.AddArchive", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    } while ((err == ZIP_OK) && (numBytes > 0));

    err = zipCloseFileInZip(m_zip);
    if (err != ZIP_OK)
    {
        throw new MgInvalidDwfPackageException(L"MgZipFileWriter.AddArchive", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH(L"MgZipFileWriter.AddArchive")

    delete[] mbFilePath;

    MG_RESOURCE_SERVICE_THROW()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Adds an XML document to the zip file.
///
void MgZipFileWriter::AddArchive(CREFSTRING filePath, const XmlDocument& xmlDoc)
{
    string document;
    xmlDoc.getContent(document);

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (BYTE_ARRAY_IN)document.c_str(), (INT32)document.length());
    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    AddArchive(filePath, byteReader);
}
