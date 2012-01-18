#ifndef DESKTOP_ZIP_FILE_HANDLER_H
#define DESKTOP_ZIP_FILE_HANDLER_H

class MgZipFileHandler
{
/// Constructors/Destructor

public:

    MgZipFileHandler(CREFSTRING filePath,
        DWFZipFileDescriptor::teFileMode fileMode);
    virtual ~MgZipFileHandler();

private:

    // Unimplemented Constructors/Methods

    MgZipFileHandler();
    MgZipFileHandler(const MgZipFileHandler&);
    MgZipFileHandler& operator=(const MgZipFileHandler&);

/// Methods

public:

/// Data Members

protected:

    auto_ptr<DWFZipFileDescriptor> m_zipFileDescriptor;
};

/// Inline Methods

#endif