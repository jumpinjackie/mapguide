#ifndef DESKTOP_ZIP_FILE_HANDLER_H
#define DESKTOP_ZIP_FILE_HANDLER_H

class MgdZipFileHandler
{
/// Constructors/Destructor

public:

    MgdZipFileHandler(CREFSTRING filePath,
        DWFZipFileDescriptor::teFileMode fileMode);
    virtual ~MgdZipFileHandler();

private:

    // Unimplemented Constructors/Methods

    MgdZipFileHandler();
    MgdZipFileHandler(const MgdZipFileHandler&);
    MgdZipFileHandler& operator=(const MgdZipFileHandler&);

/// Methods

public:

/// Data Members

protected:

    auto_ptr<DWFZipFileDescriptor> m_zipFileDescriptor;
};

/// Inline Methods

#endif