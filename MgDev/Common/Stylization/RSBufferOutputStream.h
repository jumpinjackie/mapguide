//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef RSBUFFEROUTPUTSTREAM_H
#define RSBUFFEROUTPUTSTREAM_H

#include "RS_OutputStream.h"

class RSBufferOutputStream : public RS_OutputStream
{
public:
    RSBufferOutputStream(int size)
    {
        if (size == 0)
            size = 8;

        m_pos = 0;
        m_len = size;
        m_data = new unsigned char[m_len];
    }

    //
    // Destructor
    //
    virtual ~RSBufferOutputStream()
    {
        delete [] m_data;
        m_data = NULL;
        m_len = 0;
    }

    //
    // Flushes any remaining bytes from the stream
    //
    virtual void flush() {};

    //
    // Writes at most nBytesToWrite to the stream into the buffer
    // Returns the number of actual bytes written
    //
    virtual size_t write(const void* pBuffer, size_t nBytesToWrite)
    {
        ensure_capacity(nBytesToWrite);
        memcpy(m_data + m_pos, pBuffer, nBytesToWrite);
        m_pos += nBytesToWrite;
        return nBytesToWrite;
    }

    inline unsigned char* data()   { return m_data; }
    inline size_t         length() { return m_pos; }

    void reset()
    {
        m_pos = 0;
    }

protected:
    void ensure_capacity(size_t sz)
    {
        if (m_pos + sz <= m_len)
            return;

        m_len = (m_len*2 > sz + m_pos)? m_len*2 : sz + m_pos;
        unsigned char* ndata = new unsigned char[m_len];
        memcpy(ndata, m_data, m_pos);

        delete [] m_data;
        m_data = ndata;
    }

    //
    // Default constructor
    //
    RSBufferOutputStream() {}

private:
    unsigned char* m_data;
    size_t m_pos;
    size_t m_len;
};

#endif
