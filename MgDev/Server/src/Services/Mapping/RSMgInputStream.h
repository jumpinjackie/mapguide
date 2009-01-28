//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef RSMGINPUTSTREAM_N
#define RSMGINPUTSTREAM_N

#include "RS_InputStream.h"

//simple stylization input stream wrapper around MgByteReader
//TODO: for now MgByteReader cannot seek, so we read it fully
//and then use the underlying byte array
class RSMgInputStream : public RS_InputStream
{
public:

    RSMgInputStream(MgByteReader* reader)
    {
        MgByteSink sink(reader);
        m_bytes = sink.ToBuffer();
        m_pos = 0;
    }

    RSMgInputStream(MgByte* bytes)
    {
        m_bytes = SAFE_ADDREF(bytes);
        m_pos = 0;
    }

    virtual ~RSMgInputStream()
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
