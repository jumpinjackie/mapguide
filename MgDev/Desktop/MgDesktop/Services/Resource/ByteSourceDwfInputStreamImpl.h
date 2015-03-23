#ifndef DESKTOP_BYTE_SOURCE_DWF_INPUT_STREAM_IMPL_H
#define DESKTOP_BYTE_SOURCE_DWF_INPUT_STREAM_IMPL_H

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// MgdByteSourceDwfInputStreamImpl provides access to the bytes on a DWF input
/// stream source.
/// </summary>
///
class MG_DESKTOP_API MgdByteSourceDwfInputStreamImpl : public ByteSourceImpl
{
public:

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructs an MgdByteSourceDwfInputStreamImpl object.
    /// </summary>
    ///
    /// <param name="inputStream">
    /// DWFInputStream object to use.
    /// </param>
    ///
    MgdByteSourceDwfInputStreamImpl(DWFInputStream* inputStream);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructs an MgdByteSourceDwfInputStreamImpl object.
    /// </summary>
    ///
    virtual ~MgdByteSourceDwfInputStreamImpl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the remaining length of the byte source.  This length is
    /// adjusted for previous reads.  If the returned length is zero
    /// then the underlying source may be a streaming format and the length
    /// is not known.
    /// </summary>
    ///
    /// <returns>
    /// Remaining length of the byte source.
    /// </returns>
    ///
    INT64 GetLength();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the source is rewindable.
    ///
    /// \return
    /// true if the source is rewindable, false otherwise.
    ///
    bool IsRewindable();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Rewinds to the start of memory.  Length is reset.
    /// </summary>
    ///
    /// <returns>
    /// Nothing.
    /// </returns>
    ///
    void Rewind();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Reads a buffer.
    /// </summary>
    ///
    /// <param name="buffer">
    /// A buffer receiving the data.
    /// </param>
    /// <param name="length">
    /// Maximum number of bytes to read.
    /// </param>
    ///
    /// <returns>
    /// Actual number of bytes put in the buffer. 0 means end of bytes.
    /// </returns>
    ///
    INT32 Read(BYTE_ARRAY_OUT buffer, INT32 length);

private:

    // Unimplemented Methods

    MgdByteSourceDwfInputStreamImpl();
    MgdByteSourceDwfInputStreamImpl(const MgdByteSourceDwfInputStreamImpl&);
    MgdByteSourceDwfInputStreamImpl& operator=(
        const MgdByteSourceDwfInputStreamImpl&);

protected:

    DWFInputStream* m_inputStream;
};

#endif