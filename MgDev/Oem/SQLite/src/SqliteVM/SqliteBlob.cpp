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

#include "SqliteWrap.h"

SqliteBlob::SqliteBlob(BYTE_ARRAY_IN newBuffer, INT32 lengthBuf):
buffer(NULL),
length(lengthBuf),
lastRead(0)
{
    if (newBuffer != NULL)
    {
        buffer = new char[lengthBuf];
        memcpy(buffer, newBuffer, lengthBuf);
    }
}

SqliteBlob::~SqliteBlob()
{
    delete [] buffer;
}

void SqliteBlob::Dispose()
{
    delete this;
}

INT32 SqliteBlob::Read(BYTE_ARRAY_OUT content, INT32 contentLength)
{
    int numBytes = contentLength;

    if (contentLength+lastRead >= length)
    {
        numBytes = length-lastRead;
    }

    if (numBytes > 0)
    {
        memcpy(content, &(buffer[lastRead]), numBytes);
        lastRead += numBytes;
    }

    return numBytes;
}

INT32 SqliteBlob::GetClassId()
{
    return m_cls_id;
}
