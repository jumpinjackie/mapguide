#include "ResourceDefs.h"
#include "ZipFileHandler.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgZipFileHandler::MgZipFileHandler(CREFSTRING filePath,
    DWFZipFileDescriptor::teFileMode fileMode)
{
    ACE_ASSERT(!filePath.empty());

    MG_RESOURCE_SERVICE_TRY()

    DWFFile zipFile(filePath.c_str());

    m_zipFileDescriptor.reset(new DWFZipFileDescriptor(zipFile, fileMode));
    m_zipFileDescriptor->open();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgZipFileHandler.MgZipFileHandler")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgZipFileHandler::~MgZipFileHandler()
{
}
