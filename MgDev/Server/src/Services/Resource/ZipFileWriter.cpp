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

#include "ResourceServiceDefs.h"
#include "ZipFileWriter.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgZipFileWriter::MgZipFileWriter(CREFSTRING filePath) :
    MgZipFileHandler(filePath, DWFZipFileDescriptor::eZip)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgZipFileWriter::~MgZipFileWriter()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Adds a byte buffer to the zip file.
///
void MgZipFileWriter::AddArchive(CREFSTRING filePath, MgByteReader* byteReader)
{
    ACE_ASSERT(!filePath.empty());
    DWFOutputStream* outputStream = NULL;

    MG_RESOURCE_SERVICE_TRY()

    if (NULL == byteReader)
    {
        throw new MgNullArgumentException(L"MgZipFileWriter.AddArchive",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    DWFString archivedFile(filePath.c_str());
    outputStream = m_zipFileDescriptor->zip(archivedFile);

    const INT32 length = 4096;
    BYTE buffer[length] = {0};
    INT32 numBytes = 0;

    do
    {
        numBytes = byteReader->Read(buffer, length);
        outputStream->write((const void*)buffer, (size_t)numBytes);
    } while (numBytes > 0);

    MG_RESOURCE_SERVICE_CATCH(L"MgZipFileWriter.AddArchive")

    DWFCORE_FREE_OBJECT(outputStream);

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
