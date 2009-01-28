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

#include "stdafx.h"
#include "GwsBinaryWriter.h"
#include "utf8_.h"


BinaryWriter::BinaryWriter(int initialLen)
{
    m_len = initialLen;

    _ASSERT(m_len > 0);

    m_data = new unsigned char[m_len];
    m_pos = 0;

    m_strCacheLen = 0;
    m_strCache = NULL;
}


BinaryWriter::~BinaryWriter()
{
    delete [] m_data;

    if (m_strCache)
        delete [] m_strCache;
}


void BinaryWriter::Reset()
{
    m_pos = 0;
}


unsigned char* BinaryWriter::GetData()
{
    return m_data;
}


int BinaryWriter::GetDataLen()
{
    return m_pos;
}


int BinaryWriter::GetPosition()
{
    return m_pos;
}


void BinaryWriter::WriteDouble(double d)
{
    CheckResize(sizeof(double));
    //TODO: endian
    *(double*)(m_data+m_pos) = d;
    m_pos += sizeof(double);
}


void BinaryWriter::WriteSingle(float f)
{
    CheckResize(sizeof(float));
    //TODO: endian
    *(float*)(m_data+m_pos) = f;
    m_pos += sizeof(float);
}


void BinaryWriter::WriteInt32(int i)
{
    CheckResize(sizeof(int));
    //TODO: endian
    *(int*)(m_data+m_pos) = i;
    m_pos += sizeof(int);
}


void BinaryWriter::WriteUInt32(unsigned i)
{
    CheckResize(sizeof(unsigned));
    //TODO: endian
    *(unsigned*)(m_data+m_pos) = i;
    m_pos += sizeof(unsigned);
}


void BinaryWriter::WriteInt16(short s)
{
    CheckResize(sizeof(short));
    //TODO: endian
    *(short*)(m_data+m_pos) = s;
    m_pos += sizeof(short);
}


void BinaryWriter::WriteUInt16(unsigned short us)
{
    CheckResize(sizeof(unsigned short));
    //TODO: endian
    *(unsigned short*)(m_data+m_pos) = us;
    m_pos += sizeof(unsigned short);
}


void BinaryWriter::WriteInt64(FdoInt64 ll)
{
    CheckResize(sizeof(FdoInt64));
    //TODO: endian
    *(FdoInt64*)(m_data+m_pos) = ll;
    m_pos += sizeof(FdoInt64);
}


void BinaryWriter::WriteByte(unsigned char b)
{
    CheckResize(sizeof(unsigned char));
    *(m_data+m_pos) = b;
    m_pos += sizeof(unsigned char);
}


void BinaryWriter::WriteChar(char c)
{
    CheckResize(sizeof(char));
    *(m_data+m_pos) = c;
    m_pos += sizeof(char);
}


void BinaryWriter::WriteString(const wchar_t* src)
{
    unsigned srcLen = 0;

    //handle empty string
    if (src == NULL || (srcLen = (unsigned)wcslen(src)) == 0 )
    {
        WriteInt32(0);
        return;
    }

    unsigned maxmbslen = srcLen * 4 + 1;

    if (m_strCacheLen < maxmbslen)
    {
        if (m_strCache)
            delete [] m_strCache;

        m_strCacheLen = maxmbslen;
        m_strCache = new char[maxmbslen];
    }

    int actualLen = ut_utf8_from_unicode(src, srcLen, m_strCache, m_strCacheLen);

    _ASSERT(actualLen >= 0);

    actualLen += 1; //add 1 for null character

    CheckResize(actualLen + sizeof(unsigned));

    //write string length (number of bytes, not characters!!!)
    WriteUInt32(actualLen);

    //write actual string content to the output
    memcpy(m_data + m_pos, m_strCache, actualLen);
    m_pos += actualLen;
}


void BinaryWriter::WriteRawString(const wchar_t* src)
{
    int srcLen = 0;

    //handle null string
    if (src == NULL)
    {
        return;
    }

    //handle empty string -> write trailing 0
    //NOTE: NOT the same as NULL.
    if ((srcLen = (unsigned)wcslen(src)) == 0)
    {
        WriteByte(0);
        return;
    }

    unsigned maxmbslen = srcLen * 4 + 1;

    if (m_strCacheLen < maxmbslen)
    {
        if (m_strCache)
            delete [] m_strCache;

        m_strCacheLen = maxmbslen;
        m_strCache = new char[maxmbslen];
    }

    int actualLen = ut_utf8_from_unicode(src, srcLen, m_strCache, m_strCacheLen);

    _ASSERT(actualLen >= 0);

    actualLen += 1; //add 1 for null character

    CheckResize(actualLen + sizeof(int));

    //RAW WRITE - do not write length, we know how to compute it from the
    //property offsets at the beginning of the data record
    /*
    //write string length (number of bytes, not characters!!!)
    WriteInt32(actualLen);
    */

    //write actual string content to the output
    memcpy(m_data + m_pos, m_strCache, actualLen);
    m_pos += actualLen;
}


//checks if the data write buffer has enough space and
//resizes it if needed
void BinaryWriter::CheckResize(unsigned len)
{
    if (m_pos + len < m_len)
        return;

    int newlen = 0;
    if (len > m_len)
        newlen = m_len + len;
    else
        newlen = m_len*2;

    unsigned char* ndata = new unsigned char[newlen];
    memcpy(ndata, m_data, m_len);
    m_len = newlen;
    delete [] m_data;
    m_data = ndata;
}


//writes a byte array
void BinaryWriter::WriteBytes(unsigned char* buf, int len)
{
    CheckResize(len);

    memcpy(m_data + m_pos, buf, len);
    m_pos += len;
}


//serializes a FdoDateTime
void BinaryWriter::WriteDateTime(FdoDateTime dt)
{
    WriteInt16(dt.year);
    WriteChar(dt.month);
    WriteChar(dt.day);
    WriteChar(dt.hour);
    WriteChar(dt.minute);
    WriteSingle(dt.seconds);
}
