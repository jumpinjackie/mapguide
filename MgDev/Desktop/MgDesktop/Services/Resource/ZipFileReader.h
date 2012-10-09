#ifndef DESKTOP_ZIP_FILE_READER_H
#define DESKTOP_ZIP_FILE_READER_H

#include "ZipFileHandler.h"

class MgdZipFileReader : public MgdZipFileHandler
{
/// Constructors/Destructor

public:

    explicit MgdZipFileReader(CREFSTRING filePath);
    virtual ~MgdZipFileReader();

private:

    // Unimplemented Constructors/Methods

    MgdZipFileReader();
    MgdZipFileReader(const MgdZipFileReader&);
    MgdZipFileReader& operator=(const MgdZipFileReader&);

/// Methods

public:

    MgByteSource* ExtractArchive(CREFSTRING filePath);

/// Data Members

private:

};

/// Inline Methods

#endif