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
#include "ByteSourceDwfInputStreamImpl.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgZipFileReader::MgZipFileReader(CREFSTRING filePath) :
    MgZipFileHandler(filePath, DWFZipFileDescriptor::eUnzip)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgZipFileReader::~MgZipFileReader()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Extracts an archive from the zip file.
///
MgByteSource* MgZipFileReader::ExtractArchive(CREFSTRING filePath)
{
    ACE_ASSERT(!filePath.empty());
    Ptr<MgByteSource> byteSource;

    MG_RESOURCE_SERVICE_TRY()

    // Note that the byteSource object owns the byteSourceImpl object whose
    // destructor will de-allocate the inputStream object using the macro
    // DWFCORE_FREE_OBJECT.

    DWFString archivedFile(filePath.c_str());
    DWFInputStream* inputStream = m_zipFileDescriptor->unzip(archivedFile);

    MgByteSourceDwfInputStreamImpl* byteSourceImpl =
        new MgByteSourceDwfInputStreamImpl(inputStream);
    byteSource = new MgByteSource(byteSourceImpl);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgZipFileReader.ExtractArchive")

    return byteSource.Detach();
}
