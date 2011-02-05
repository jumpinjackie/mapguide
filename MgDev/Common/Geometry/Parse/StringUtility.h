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

#ifndef _STRINGUTILITY_H_
#define _STRINGUTILITY_H_

#ifdef _WIN32
#pragma once
#endif


#define GIS_API

/// \cond INTERNAL
class GisStringUtility
{
public:
    GIS_API static void ClearString(wchar_t*& psz);
    GIS_API static wchar_t* MakeString(wchar_t* psz1, wchar_t* psz2 = NULL, wchar_t* psz3 = NULL, wchar_t* psz4 = NULL, wchar_t* psz5 = NULL);
    GIS_API static wchar_t* MakeString(INT32 numStrings, wchar_t* * strings, wchar_t* separator = NULL);
    GIS_API static wchar_t* MakeString(unsigned char* data, INT32 iDataSize);
    GIS_API static void     FormatDouble(double d, wchar_t* pszBuffer, INT32 iLen);
    GIS_API static void     FormatSingle(float f, wchar_t* pszBuffer, INT32 iLen);
    GIS_API static void     FormatNumber(double d, INT32 precision, wchar_t* pszBuffer, INT32 iLen);
    GIS_API static wchar_t* QuoteString(wchar_t* psz, wchar_t chQuote);

    GIS_API static size_t StringLength(wchar_t* string);
    GIS_API static INT32 StringCompare(wchar_t* string1, wchar_t* string2);
    GIS_API static INT32 StringCompareNoCase(wchar_t* string1, wchar_t* string2);
    GIS_API static void StringCopy(wchar_t* string1, wchar_t* string2);
    GIS_API static void SubstringCopy(wchar_t* string1, wchar_t* string2, size_t length);
    GIS_API static void StringConcatenate(wchar_t* string1, wchar_t* string2);
    GIS_API static wchar_t* FindCharacter(wchar_t* string, wchar_t character);

    GIS_API static wchar_t*   NullString;
};
/// \endcond

#endif
