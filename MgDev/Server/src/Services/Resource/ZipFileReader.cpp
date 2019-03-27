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
#include "ZipFileReader.h"

//Bulk of this code copied from miniunz.c reference implementation

class MgByteSourceUnzipImpl : public ByteSourceImpl
{
public:
    MgByteSourceUnzipImpl(unz_file_info currentFileInfo, unzFile unzip) 
        : m_fi(currentFileInfo), m_unzip(unzip), m_read(0)
    { 
        int err = unzOpenCurrentFile(m_unzip);
        if (err != UNZ_OK)
        {
            //throw;
        }
    }
    virtual ~MgByteSourceUnzipImpl() 
    { 
        unzCloseCurrentFile(m_unzip);
    }
    INT64 GetLength() { return m_fi.uncompressed_size - m_read; }
    bool IsRewindable() { return false;  }
    void Rewind() { }
    INT32 Read(BYTE_ARRAY_OUT buffer, INT32 length)
    {
        INT32 err = unzReadCurrentFile(m_unzip, buffer, length);
        m_read += err;
        return err;
    }

private:
    unz_file_info m_fi;
    unzFile m_unzip;
    INT32 m_read;

    // Unimplemented Methods

    MgByteSourceUnzipImpl();
    MgByteSourceUnzipImpl(const MgByteSourceUnzipImpl&);
    MgByteSourceUnzipImpl& operator=(
        const MgByteSourceUnzipImpl&);
};

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgZipFileReader::MgZipFileReader(CREFSTRING filePath) 
    : m_unzip(NULL)
{
    std::string mbFilePath = MgUtil::WideCharToMultiByte(filePath);
    m_unzip = unzOpen(mbFilePath.c_str());
    if (!m_unzip)
    {
        MgStringCollection arguments;
        arguments.Add(filePath);
        throw new MgInvalidDwfPackageException(L"MgZipFileReader.MgZipFileReader", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgZipFileReader::~MgZipFileReader()
{
    if (m_unzip)
    {
        int r = unzClose(m_unzip);
#ifdef _DEBUG
        if (r != UNZ_OK)
        {
            throw new MgInvalidDwfPackageException(L"MgZipFileReader.~MgZipFileReader", __LINE__, __WFILE__, NULL, L"", NULL);
        }
#endif
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Extracts an archive from the zip file.
///
MgByteSource* MgZipFileReader::ExtractArchive(CREFSTRING filePath)
{
    ACE_ASSERT(!filePath.empty());
    Ptr<MgByteSource> byteSource;

    char* mbFilePath = MgUtil::WideCharToMultiByte(filePath.c_str());

    MG_RESOURCE_SERVICE_TRY()

    //Set pointer to location of specified file
    if (unzLocateFile(m_unzip, mbFilePath, 0 /* Let platform decide case-sensitivity */) != UNZ_OK)
    {
        MgStringCollection arguments;
        arguments.Add(filePath);
        throw new MgInvalidDwfPackageException(L"MgZipFileReader.ExtractArchive", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    int err = UNZ_OK;
    unz_file_info file_info;
    err = unzGetCurrentFileInfo(m_unzip, &file_info, mbFilePath, strlen(mbFilePath), NULL, 0, NULL, 0);
    if (err != UNZ_OK)
    {
        MgStringCollection arguments;
        arguments.Add(filePath);
        throw new MgInvalidDwfPackageException(L"MgZipFileReader.ExtractArchive", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    MgByteSourceUnzipImpl* impl = new MgByteSourceUnzipImpl(file_info, m_unzip);
    byteSource = new MgByteSource(impl);

    MG_RESOURCE_SERVICE_CATCH(L"MgZipFileReader.ExtractArchive")

        delete[] mbFilePath;

    MG_RESOURCE_SERVICE_THROW()
    return byteSource.Detach();
}
