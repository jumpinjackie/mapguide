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
#include "KeyEncode.h"
#include "RS_FeatureReader.h"
#include "UnicodeString.h"

// macros to swap from Big Endian to Little Endian
#ifdef _WIN32
#define RS_BIGENDIAN 0
#else
#define RS_BIGENDIAN (__BYTE_ORDER == __BIG_ENDIAN)
#endif
#define SWAPWORD(x) MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x) MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))
#if RS_BIGENDIAN
#   define LE_WORD(x) SWAPWORD(x)
#   define LE_LONG(x) SWAPLONG(x)
#else
#   define LE_WORD(x) ((unsigned short)x)
#   define LE_LONG(x) ((unsigned long)x)
#endif


KeyEncode::KeyEncode()
: m_stream(8)
{
    m_base64Len = 0;
    m_base64Data = NULL;
}

KeyEncode::~KeyEncode()
{
    delete [] m_base64Data;
}

const char* KeyEncode::EncodeKey(RS_FeatureReader* reader)
{
    m_stream.reset();

    int count = 0;
    const wchar_t* const* idprops = reader->GetIdentPropNames(count);

    for (int i=0; i<count; ++i)
    {
        const wchar_t* name = idprops[i];
        int dataType = reader->GetPropertyType(name);

        switch (dataType)
        {
            case FdoDataType_Boolean :
                    WriteByte(reader->GetBoolean(name)? 1 : 0);
                    break;
            case FdoDataType_Byte :
                    WriteByte(reader->GetByte(name));
                    break;
            case FdoDataType_DateTime :
                    WriteDateTime(reader->GetDateTime(name));
                    break;
            case FdoDataType_Decimal :
                    WriteDouble(reader->GetDouble(name));
                    break;
            case FdoDataType_Double :
                    WriteDouble(reader->GetDouble(name));
                    break;
            case FdoDataType_Int16 :
                    WriteInt16(reader->GetInt16(name));
                    break;
            case FdoDataType_Int32 :
                    WriteInt32(reader->GetInt32(name));
                    break;
            case FdoDataType_Int64 :
                    WriteInt64(reader->GetInt64(name));
                    break;
            case FdoDataType_Single :
                    WriteSingle(reader->GetSingle(name));
                    break;
            case FdoDataType_String :
                    WriteString(reader->GetString(name));
                    break;
            case FdoDataType_BLOB :
                {
                    RS_InputStream* is = reader->GetBLOB(name);
                    WriteUInt32((unsigned int)is->available());
                    WriteStream(is);
                    delete is;
                }
                break;
            case FdoDataType_CLOB :
                {
                    RS_InputStream* is = reader->GetBLOB(name);
                    WriteUInt32((unsigned int)is->available());
                    WriteStream(is);
                    delete is;
                }
                break;
            default:
                    throw FdoException::Create();
        }
    }

    unsigned long estLen = Base64::GetEncodedLength((unsigned long)m_stream.length());
    if (estLen == 0)
        return NULL;

    // m_base64Len stores the buffer size not including the null terminator
    if (m_base64Len < estLen)
    {
        delete [] m_base64Data;
        m_base64Len = rs_max(estLen, 2*m_base64Len);
        m_base64Data = new char[m_base64Len+1];
    }

    size_t base64Len = 0;

    // encode key -> empty string if there were no id properties
    if (count > 0)
    {
        base64Len = Base64::Encode(m_base64Data, m_stream.data(), (unsigned long)m_stream.length());
        _ASSERT(base64Len <= estLen);
    }

    // null terminate so that we can treat as string
    m_base64Data[base64Len] = 0;

    return m_base64Data;
}

void KeyEncode::WriteDouble(double d)
{
    FdoInt64 swap = *(FdoInt64*)&d;
    WriteInt64(swap);
}

void KeyEncode::WriteSingle(float f)
{
    unsigned int swap = *(unsigned int*)&f;
    WriteUInt32(swap);
}

void KeyEncode::WriteInt32(int i)
{
    unsigned int swap = LE_LONG(i);
    m_stream.write(&swap, sizeof(int));
}

void KeyEncode::WriteUInt32(unsigned i)
{
    unsigned int swap = LE_LONG(i);
    m_stream.write(&swap, sizeof(unsigned));
}

void KeyEncode::WriteInt16(short s)
{
    unsigned short swap = LE_WORD(s);
    m_stream.write(&swap, sizeof(unsigned short));
}

void KeyEncode::WriteUInt16(unsigned short us)
{
    unsigned short swap = LE_WORD(us);
    m_stream.write(&swap, sizeof(unsigned short));
}

void KeyEncode::WriteInt64(FdoInt64 ll)
{
    unsigned int word = (unsigned int) (ll & 0xFFFFFFFF);
    WriteUInt32(word);

    word = (unsigned int)((ll >> 32) & 0xFFFFFFFF);
    WriteUInt32(word);
}

void KeyEncode::WriteByte(unsigned char b)
{
    m_stream.write(&b, sizeof(unsigned char));
}

void KeyEncode::WriteChar(char c)
{
    m_stream.write(&c, sizeof(char));
}

void KeyEncode::WriteString(const wchar_t* src)
{
    std::string sutf8;
    const XMLCh* srcX = W2X(src);
    UnicodeString::UTF16toUTF8(srcX, sutf8);
    size_t nUsed = sutf8.length();
    WriteBytes((unsigned char*)sutf8.c_str(), nUsed+1);
}

void KeyEncode::WriteBytes(unsigned char* buf, size_t len)
{
    m_stream.write(buf, len);
}

void KeyEncode::WriteDateTime(FdoDateTime dt)
{
    // serialize date time in MapGuide HTTP protocol format
    // where seconds and microseconds are separate

    WriteInt16(dt.year);
    WriteChar(dt.month);
    WriteChar(dt.day);
    WriteChar(dt.hour);
    WriteChar(dt.minute);

    double secs;
    double fraction;

    fraction = modf(dt.seconds, &secs);

    WriteByte((unsigned char)secs);
    WriteInt32((int)(fraction * 1000000.0));
}

void KeyEncode::WriteStream(RS_InputStream* is)
{
    unsigned char buf[256];

    int read = 0;

    do
    {
        size_t read = is->read(buf, 256);

        if (read > 0)
            WriteBytes(buf, read);
    }
    while (read > 0);
}
