#include "ResourceDefs.h"
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
