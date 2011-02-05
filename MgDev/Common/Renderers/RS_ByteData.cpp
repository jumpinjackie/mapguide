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

#include "stdafx.h"
#include "RS_ByteData.h"
#include <memory.h>


RS_ByteData::RS_ByteData() :
    m_bytes(NULL),
    m_numBytes(0)
{
}


// stores a copy of the supplied data
RS_ByteData::RS_ByteData(unsigned char* bytes, unsigned int numBytes) :
    m_bytes(NULL),
    m_numBytes(numBytes)
{
    _ASSERT(bytes != NULL);
    _ASSERT(numBytes > 0);

    m_bytes = new unsigned char[numBytes];
    if (m_bytes)
        ::memcpy(m_bytes, bytes, numBytes);
}


RS_ByteData::~RS_ByteData()
{
    delete [] m_bytes;
}


void RS_ByteData::Dispose()
{
    delete this;
}


unsigned char* RS_ByteData::GetBytes()
{
    return m_bytes;
}


unsigned int RS_ByteData::GetNumBytes()
{
    return m_numBytes;
}
