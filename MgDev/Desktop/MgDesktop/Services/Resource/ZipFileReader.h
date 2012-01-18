#ifndef DESKTOP_ZIP_FILE_READER_H
#define DESKTOP_ZIP_FILE_READER_H

#include "ZipFileHandler.h"

class MgZipFileReader : public MgZipFileHandler
{
/// Constructors/Destructor

public:

    explicit MgZipFileReader(CREFSTRING filePath);
    virtual ~MgZipFileReader();

private:

    // Unimplemented Constructors/Methods

    MgZipFileReader();
    MgZipFileReader(const MgZipFileReader&);
    MgZipFileReader& operator=(const MgZipFileReader&);

/// Methods

public:

    MgByteSource* ExtractArchive(CREFSTRING filePath);

/// Data Members

private:

};

/// Inline Methods

#endif