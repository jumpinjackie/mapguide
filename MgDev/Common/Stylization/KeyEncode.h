//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef KEY_ENCODE_H
#define KEY_ENCODE_H

#include "RSBufferOutputStream.h"

//class RSBufferOutputStream;
class RS_FeatureReader;
class RS_InputStream;

class KeyEncode
{
public:

    STYLIZATION_API KeyEncode();
    STYLIZATION_API virtual ~KeyEncode();

    STYLIZATION_API virtual const unsigned char* EncodeKey(RS_FeatureReader* reader);

private:

    void WriteDouble(double d);
    void WriteSingle(float f);
    void WriteInt32(int i);
    void WriteUInt32(unsigned i);
    void WriteInt16(short s);
    void WriteUInt16(unsigned short us);
    void WriteInt64(GisInt64 ll);
    void WriteByte(unsigned char b);
    void WriteChar(char c);
    void WriteString(const wchar_t* src);
    void WriteBytes(unsigned char* buf, size_t len);
    void WriteDateTime(GisDateTime dt);
    void WriteStream(RS_InputStream* is);

    RSBufferOutputStream m_stream;

    unsigned char* m_base64Data;
    size_t m_base64Len;

    //
    // Unimplemented methods
    //

    KeyEncode( const KeyEncode& );
    KeyEncode& operator=( const KeyEncode& );
};

#endif
