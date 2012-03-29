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
#include "UnicodeString.h"
#include "ConvertUTF.h"

#define UTFCONVERT(FROMCHARSET,TOCHARSET) \
    if (!str.empty())\
        str.clear();\
\
    if (uniChar == NULL)\
        return;\
\
    size_t size = 0;\
\
    const FROMCHARSET* sourceStart = (FROMCHARSET*) uniChar;\
    FROMCHARSET* sourceEnd = (FROMCHARSET*) sourceStart;\
    while (*sourceEnd != (FROMCHARSET)0)\
    {\
        ++sourceEnd;\
    }\
    ++sourceEnd; /* include terminating null in conversion */\
    TOCHARSET* targetStart = 0;\
    TOCHARSET* targetEnd = 0;\
    ConversionResult convResult = MgConvert##FROMCHARSET##to##TOCHARSET (&sourceStart, sourceEnd,\
        &targetStart, targetEnd, getConvertedSize);\
\
    if (conversionOK == convResult)\
    {\
        size = ((size_t)targetStart)/sizeof(TOCHARSET);\
    }\
    else\
    {\
        throw UnicodeString::ConversionError;\
    }\
\
    if (size > 0)\
    {\
        if (str.capacity() < size)\
        {\
            str.reserve(size);\
        }\
        str.resize(size-1);\
        sourceStart = (FROMCHARSET*) uniChar;\
        targetStart = (TOCHARSET*) str.c_str();\
        targetEnd = (TOCHARSET*) (targetStart+size);\
        convResult = MgConvert##FROMCHARSET##to##TOCHARSET (&sourceStart, sourceEnd,\
            &targetStart, targetEnd, lenientConversion);\
\
        if (conversionOK != convResult)\
        {\
            throw UnicodeString::ConversionError;\
        }\
    }

void UnicodeString::WideCharToMultiByte(const wchar_t* uniChar, string& str)
{
#ifdef _WIN32
    UTFCONVERT(UTF16,UTF8)
#else
    UTFCONVERT(UTF32,UTF8)
#endif
}

void UnicodeString::MultiByteToWideChar(const char* uniChar, wstring& str)
{
#ifdef _WIN32
    UTFCONVERT(UTF8,UTF16)
#else
    UTFCONVERT(UTF8,UTF32)
#endif
}


void UnicodeString::UTF16toUTF8(const XMLCh* uniChar, string& str)
{
    UTFCONVERT(UTF16,UTF8)
}

void UnicodeString::UTF8toUTF16(const char* uniChar, xstring& str)
{
    UTFCONVERT(UTF8,UTF16)
}

xstring UnicodeString::UTF8toUTF16(const char* mbStr)
{
    xstring str;
    UTF8toUTF16(mbStr, str);
    return str;
}

void UnicodeString::UTF32toUTF8(const LCh* uniChar, string& str)
{
    UTFCONVERT(UTF32,UTF8)
}

void UnicodeString::UTF8toUTF32(const char* uniChar, lstring& str)
{
    UTFCONVERT(UTF8,UTF32)
}

void UnicodeString::UTF32toUTF16(const LCh* uniChar, xstring& str)
{
    UTFCONVERT(UTF32,UTF16)
}

xstring UnicodeString::UTF32toUTF16(const LCh* wideStr)
{
    xstring utf16;
    try
    {
        UTF32toUTF16(wideStr, utf16);
    }
    catch (int)
    {
    }

    return utf16;
}

void UnicodeString::UTF16toUTF32(const XMLCh* uniChar, lstring& str)
{
    UTFCONVERT(UTF16,UTF32)
}

lstring UnicodeString::UTF16toUTF32(const XMLCh* uniChar)
{
    lstring utf32;
    try
    {
        UTF16toUTF32(uniChar, utf32);
    }
    catch (int)
    {
    }

    return utf32;
}

#ifndef _WIN32
template class basic_string<XMLCh, unicode_traits<XMLCh> >;
template class basic_string<LCh, unicode_traits<LCh> >;
#endif
