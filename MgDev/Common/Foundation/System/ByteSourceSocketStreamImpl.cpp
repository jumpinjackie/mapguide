//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

/// INTERNAL_CLASS - NOT EXPOSED

#include "Foundation.h"
#include "ServerConnectionBase.h"
#include "ByteSourceStreamImpl.h"
#include "ByteSourceSocketStreamImpl.h"

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a ByteSourceSocketStreamImpl object
///</summary>
///<param name="length">Length of the stream</param>
///<param name="conn">Map server connection</param>
///
ByteSourceSocketStreamImpl::ByteSourceSocketStreamImpl(INT64 length, MgServerConnectionBase* conn)
:
m_len(length),
m_pos(0),
m_blockSize(0),
m_blockRead(0)
{
    m_conn = conn;
}

//////////////////////////////////////////////////////////////////
///<summary>
///Destroy ByteSourceSocketStreamImpl object
///</summary>
///
ByteSourceSocketStreamImpl::~ByteSourceSocketStreamImpl()
{
    MG_TRY()

    Close();

    MG_CATCH_AND_RELEASE()
}

void ByteSourceSocketStreamImpl::Close()
{
    if (m_conn != NULL)
    {
        // Read everything out of the stream.  Developers
        // may not pull the entire stream before dumping
        // the MgByteReader.
        INT32 nBytes = 0;
        unsigned char buf[1024];
        do
        {
            nBytes = Read(buf, 1024);
        }
        while (nBytes > 0);

        m_conn = NULL;
    }
    m_len = 0;
    m_pos = 0;
    m_blockSize = 0;
    m_blockRead = 0;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Reads a buffer
///</summary>
///<param name="buffer">
/// A buffer receiving the data.
///</param>
///<param name="length">
///Maximum number of bytes to read
///</param>
///<returns>Actual number of bytes put in the buffer. 0 means end of bytes</returns>
INT32 ByteSourceSocketStreamImpl::Read(BYTE_ARRAY_OUT buffer, INT32 length)
{
    if (m_conn == NULL)
    {
        return 0;
    }

    INT32 bytesRead = 0;

    MG_TRY()

    INT64 remaining = m_len - m_pos;
    INT32 toReceive = remaining > (INT64) length ? length : (INT32) remaining;

    Ptr<MgStream> stream = m_conn->GetStream();
    Ptr<MgStreamHelper> helper = stream->GetStreamHelper();

    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssError;

    // Read block size if required
    if (m_blockSize == 0)
    {
        UINT32 len = 0;
        helper->GetUINT32( len, true, false );
        m_blockSize = (INT32) len;
        m_blockRead = 0;
    }

    if (m_blockSize == 0)
    {
        helper = NULL;
        stream->GetStreamEnd();
        stream = NULL;
        m_conn = NULL;
        return 0;
    }

    // We cannot read through input stream block boundaries
    if (toReceive <= m_blockSize - m_blockRead)
    {
        // We are ok.  Just read what's there and increment counts
        stat = helper->GetData(buffer, toReceive);

        if (stat != MgStreamHelper::mssDone)
        {
            throw new MgStreamIoException(L"ByteSourceSocketStreamImpl.Read",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        m_blockRead += toReceive;
        bytesRead = toReceive;

        if (m_blockSize == m_blockRead)
        {
            m_blockSize = 0;
            m_blockRead = 0;
        }
    }
    else
    {
        // Fill with the remainder of the block
        INT32 blockRemain = m_blockSize - m_blockRead;
        if (blockRemain > 0)
        {
            stat = helper->GetData(buffer, blockRemain);

            if (stat != MgStreamHelper::mssDone)
            {
                throw new MgStreamIoException(L"ByteSourceSocketStreamImpl.Read",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }

            bytesRead = blockRemain;
        }

        m_blockSize = 0;
        m_blockRead = 0;
    }

    m_pos += bytesRead;

    // When reaching the end of the stream, release the currently acquired
    // connection and return it to the pool. This will prevent the connection
    // pool from getting exhausted if the .NET or Java's garbage collector does
    // not do its job fast enough.
    if (m_pos == m_len)
    {
        helper = NULL;
        stream = NULL;
        Close();
    }

    MG_CATCH_AND_THROW(L"ByteSourceSocketStreamImpl.Read")

    return bytesRead;
}

INT64 ByteSourceSocketStreamImpl::GetLength()
{
    return m_len - m_pos;
}

bool ByteSourceSocketStreamImpl::IsRewindable()
{
    return false;
}

void ByteSourceSocketStreamImpl::Rewind()
{
    throw new MgInvalidOperationException(L"ByteSourceSocketStreamImpl.Rewind",
        __LINE__, __WFILE__, NULL, L"", NULL);
}
