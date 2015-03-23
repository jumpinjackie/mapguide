#ifndef DESKTOP_RS_INPUT_STREAM_H
#define DESKTOP_RS_INPUT_STREAM_H

#include "RS_InputStream.h"

//simple stylization input stream wrapper around MgByteReader
//TODO: for now MgByteReader cannot seek, so we read it fully
//and then use the underlying byte array
class RSMgdInputStream : public RS_InputStream
{
public:

    RSMgdInputStream(MgByteReader* reader)
    {
        MgByteSink sink(reader);
        m_bytes = sink.ToBuffer();
        m_pos = 0;
    }

    RSMgdInputStream(MgByte* bytes)
    {
        m_bytes = SAFE_ADDREF(bytes);
        m_pos = 0;
    }

    virtual ~RSMgdInputStream()
    {
        SAFE_RELEASE(m_bytes);
    }

    //
    // Returns the number of bytes available to read from stream
    //
    virtual size_t available() const
    {
        return m_bytes->GetLength();
    }

    //
    // Reads at most nBytesToRead from the stream into the buffer
    // Returns the number of actual bytes read
    //
    virtual size_t read(void* pBuffer, size_t nBytesToRead)
    {
        memcpy(pBuffer, (unsigned char*)m_bytes->Bytes() + m_pos, nBytesToRead);
        m_pos += nBytesToRead;
        return nBytesToRead;
    }

    //
    // Repositions the stream pointer
    // Returns the previous offset before the seek
    //
    virtual off_t seek(int eOrigin, off_t  nOffset)
    {
        size_t oldpos = m_pos;

        switch (eOrigin)
        {
        case SEEK_CUR: m_pos += nOffset; break;
        case SEEK_SET: m_pos = nOffset; break;
        case SEEK_END: m_pos = m_bytes->GetLength() - nOffset; break;
        default: break;
        }

        return (off_t)oldpos;
    }


private:

    MgByte* m_bytes;
    size_t m_pos;
};

#endif