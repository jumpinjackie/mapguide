//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "ByteSourceRasterStreamImpl.h"

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a ByteSourceRasterStreamImpl object
///</summary>
///<param name="stream">ACE socket stream</param>
///<param name="length">Length of the stream</param>
///
ByteSourceRasterStreamImpl::ByteSourceRasterStreamImpl(FdoIStreamReaderTmpl<FdoByte>* stream)
{
    if (stream == NULL)
    {
        throw new MgNullReferenceException(L"ByteSourceRasterStreamImpl", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    m_stream = FDO_SAFE_ADDREF(stream);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a ByteSourceRasterStreamImpl object
///</summary>
///<param name="stream">ACE socket stream</param>
///<param name="length">Length of the stream</param>
///
ByteSourceRasterStreamImpl::~ByteSourceRasterStreamImpl()
{
    FDO_SAFE_RELEASE(m_stream);
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
INT32 ByteSourceRasterStreamImpl::Read(BYTE_ARRAY_OUT buffer, INT32 length)
{
    if (buffer == NULL)
    {
        throw new MgNullArgumentException(L"ByteSourceRasterStreamImpl.Read", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (length < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(length, buffer);

        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"ByteSourceRasterStreamImpl.Read",
            __LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanZero", NULL);
    }

    // Read data for the specified length from current index position
    INT32 bytesRead = m_stream->ReadNext(buffer, 0, (FdoInt32)length);

    return bytesRead;
}


INT64 ByteSourceRasterStreamImpl::GetLength()
{
    // Streaming format - we do not know the length;
    return m_stream->GetLength();
}

bool ByteSourceRasterStreamImpl::IsRewindable()
{
    return true;
}

void ByteSourceRasterStreamImpl::Rewind()
{
    m_stream->Reset();
}
