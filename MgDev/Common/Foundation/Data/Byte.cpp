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
#include "Byte.h"

MgByte::MgByte()
{
    m_bytes = NULL;
    m_len = 0;
    m_allocLen = 0;
    m_allocType = MgByte::Internal;
}

MgByte::MgByte(BYTE_ARRAY_IN bytes, INT32 len, AllocatorType allocator)
: m_allocType(allocator)
{
    // Basic initialization
    m_bytes = NULL;
    m_len = 0;
    m_allocLen = len;

    // If len was specified then we need to (preallocate) the internal buffer
    if (len > 0)
    {
        if (MgByte::Internal == m_allocType)
        {
            m_bytes = Allocate(len);
        }
    }

    // If bytes were passed in, we need to assign them directly if not allocating,
    // or copy them if we are allocating
    if (bytes != NULL && len > 0)
    {
        if (MgByte::Internal == m_allocType)
        {
            memcpy(m_bytes, bytes, len);
        }
        else
        {
            m_bytes = bytes;
        }
        m_len = len;
    }
}

MgByte::~MgByte()
{
    if (m_bytes != NULL)
    {
        switch (m_allocType)
        {
        case MgByte::Internal:
            CheckEnd();
            delete[] m_bytes;
            break;
        case MgByte::New:
            delete[] m_bytes;
            break;
        case MgByte::Malloc:
            free(m_bytes);
            break;
        default:
            break;
        }
        m_bytes = NULL;
    }

    m_allocType = MgByte::None;
    m_len = 0;
    m_allocLen = 0;
}

void MgByte::Append(BYTE_ARRAY_IN bytes, INT32 len)
{
    CheckEnd();
    if (MgByte::Internal != m_allocType)
    {
        throw new MgInvalidOperationException(L"MgByte.Append", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (len+m_len > m_allocLen)
    {
        // Use geometric allocation
        if (m_allocLen*2 > (len+m_len))
        {
            m_allocLen *= 2;
        }
        else
        {
            m_allocLen = (len+m_len);
        }

        unsigned char* temp = Allocate(m_allocLen);
        if (m_bytes != NULL)
        {
            memcpy(temp, m_bytes, m_len);
            delete [] m_bytes;
        }
        m_bytes = temp;

    }

    memcpy(m_bytes+m_len, bytes, len);
    m_len += len;
}

INT32 MgByte::Read(BYTE_ARRAY_OUT buffer, INT32 pos, INT32 length)
{
    CheckEnd();
    INT32 numRead = 0;
    if (pos+length <= m_len)
    {
        numRead = length;
    }
    else
    {
        numRead = m_len-pos;
    }


    if (numRead > 0)
    {
        memcpy(buffer, &m_bytes[pos], length);
    }

    return (numRead >= 0) ? numRead : 0;
}

BYTE_ARRAY_OUT MgByte::Bytes()
{
    CheckEnd();
    return m_bytes;
}

INT32 MgByte::GetLength()
{
    CheckEnd();
    return m_len;
}

void MgByte::SetLength(INT32 len)
{
    CheckEnd();
    if (len > m_allocLen)
    {
        throw new MgArgumentOutOfRangeException(L"MgByte.SetLength",__LINE__,__WFILE__, NULL, L"", NULL);
    }
    m_len = len;
}

void MgByte::Dispose()
{
    delete this;
}


void MgByte::CheckEnd()
{
    if (MgByte::Internal == m_allocType && m_bytes != NULL && m_allocLen > 0)
    {
        if ( *((UINT64*)&m_bytes[m_allocLen]) != EndBytesMarker)
        {
            throw new MgOverflowException(L"MgByte.CheckEnd",__LINE__,__WFILE__, NULL, L"", NULL);
        }
    }
}

unsigned char* MgByte::Allocate(INT32 len)
{
    unsigned char* buf = new unsigned char[len+sizeof(UINT64)];
    *((UINT64*)&buf[len]) = EndBytesMarker;
    return buf;
}
