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

#include "Foundation.h"

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a ByteSourceMemoryImpl object
///</summary>
///<param name="data">Memory data source</param>
///<param name="length">Data length</param>
///
ByteSourceMemoryImpl::ByteSourceMemoryImpl(BYTE_ARRAY_IN data, INT32 length)
{
    //make a copy of the memory
    m_data = new MgByte(data, length);

    if(m_data == NULL)
    {
        throw new MgOutOfMemoryException(L"ByteSourceMemoryImpl.ByteSourceMemoryImpl",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_pos = 0;
}

ByteSourceMemoryImpl::ByteSourceMemoryImpl(MgByte* data)
{
    m_data = SAFE_ADDREF(data);
    m_pos = 0;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a ByteSourceMemoryImpl object
///</summary>
///
ByteSourceMemoryImpl::~ByteSourceMemoryImpl()
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Reads a buffer of data
///</summary>
///<param name="buffer">
/// A buffer receiving the data.
///</param>
///<param name="length">
///Maximum number of bytes to read
///</param>
///<returns>Actual number of bytes put in the buffer. 0 means end of bytes</returns>
///
INT32 ByteSourceMemoryImpl::Read(BYTE_ARRAY_OUT buffer, INT32 length)
{
    if (m_data == NULL)
        return 0;
    INT32 remaining = m_data->GetLength() - m_pos;
    if(remaining == 0)
        return 0;

    INT32 bytesCopied = remaining > length? length: remaining;
    m_data->Read(buffer, m_pos, bytesCopied);
    m_pos += bytesCopied;

    return bytesCopied;
}

INT64 ByteSourceMemoryImpl::GetLength()
{
    return (m_data == NULL)? 0 : m_data->GetLength() - m_pos;
}

bool ByteSourceMemoryImpl::IsRewindable()
{
    return true;
}

void ByteSourceMemoryImpl::Rewind()
{
    m_pos = 0;
}

MgByte* ByteSourceMemoryImpl::Bytes()
{
    return (SAFE_ADDREF((MgByte*)m_data));
}
