#include "ResourceDefs.h"
#include "ZipFileReader.h"
#include "ByteSourceDwfInputStreamImpl.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgdZipFileReader::MgdZipFileReader(CREFSTRING filePath) :
    MgdZipFileHandler(filePath, DWFZipFileDescriptor::eUnzip)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgdZipFileReader::~MgdZipFileReader()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Extracts an archive from the zip file.
///
MgByteSource* MgdZipFileReader::ExtractArchive(CREFSTRING filePath)
{
    ACE_ASSERT(!filePath.empty());
    Ptr<MgByteSource> byteSource;

    MG_RESOURCE_SERVICE_TRY()

    // Note that the byteSource object owns the byteSourceImpl object whose
    // destructor will de-allocate the inputStream object using the macro
    // DWFCORE_FREE_OBJECT.

    DWFString archivedFile(filePath.c_str());
    DWFInputStream* inputStream = m_zipFileDescriptor->unzip(archivedFile);

    MgdByteSourceDwfInputStreamImpl* byteSourceImpl =
        new MgdByteSourceDwfInputStreamImpl(inputStream);
    byteSource = new MgByteSource(byteSourceImpl);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdZipFileReader.ExtractArchive")

    return byteSource.Detach();
}
