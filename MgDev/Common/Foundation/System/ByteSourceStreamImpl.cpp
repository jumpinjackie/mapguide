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
#include "ByteSourceStreamImpl.h"

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a ByteSourceMgStreamImpl object
///</summary>
///<param name="stream">ACE socket stream</param>
///<param name="length">Length of the stream</param>
///
ByteSourceMgStreamImpl::ByteSourceMgStreamImpl(MgStream* stream)
{
    CHECKARGUMENTNULL(stream, L"ByteSourceMgStreamImpl.ByteSourceMgStreamImpl");

    m_stream = SAFE_ADDREF(stream);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a ByteSourceMgStreamImpl object
///</summary>
///<param name="stream">ACE socket stream</param>
///<param name="length">Length of the stream</param>
///
ByteSourceMgStreamImpl::~ByteSourceMgStreamImpl()
{
    SAFE_RELEASE(m_stream);
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
INT32 ByteSourceMgStreamImpl::Read(BYTE_ARRAY_OUT buffer, INT32 length)
{
    // Read data for the specified length
    INT32 bytesReceived = m_stream->GetData(buffer, length);

    if (bytesReceived == -1)
    {
        throw new MgStreamIoException(L"ByteSourceMgStreamImpl.Read",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //nRecv = -errno;
    return bytesReceived;
}


INT64 ByteSourceMgStreamImpl::GetLength()
{
    // Streaming format - we do not know the length;
    return 0;
}

bool ByteSourceMgStreamImpl::IsRewindable()
{
    return false;
}

void ByteSourceMgStreamImpl::Rewind()
{
    throw new MgInvalidOperationException(L"ByteSourceMgStreamImpl.Rewind",
        __LINE__, __WFILE__, NULL, L"", NULL);
}
