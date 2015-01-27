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
#include "StringUtility.h"
#include <float.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>

wchar_t*  GisStringUtility::NullString = L"NULL";

void GisStringUtility::ClearString(wchar_t*& psz)
{
    delete [] psz;
    psz = NULL;
}

wchar_t* GisStringUtility::MakeString(wchar_t* psz1, wchar_t* psz2, wchar_t* psz3, wchar_t* psz4, wchar_t* psz5)
{
    if (!psz1 && !psz2 && !psz3 && !psz4 && !psz5)
        return NULL;

    size_t  iLen = 1;   // 1 for EOS
    if (psz1 != NULL)
        iLen += GisStringUtility::StringLength(psz1);
    if (psz2 != NULL)
        iLen += GisStringUtility::StringLength(psz2);
    if (psz3 != NULL)
        iLen += GisStringUtility::StringLength(psz3);
    if (psz4 != NULL)
        iLen += GisStringUtility::StringLength(psz4);
    if (psz5 != NULL)
        iLen += GisStringUtility::StringLength(psz5);

    wchar_t*    pszRet = new wchar_t[iLen];
    *pszRet = L'\0';

    if (psz1 != NULL)
        GisStringUtility::StringConcatenate(pszRet, psz1);
    if (psz2 != NULL)
        GisStringUtility::StringConcatenate(pszRet, psz2);
    if (psz3 != NULL)
        GisStringUtility::StringConcatenate(pszRet, psz3);
    if (psz4 != NULL)
        GisStringUtility::StringConcatenate(pszRet, psz4);
    if (psz5 != NULL)
        GisStringUtility::StringConcatenate(pszRet, psz5);

    return pszRet;
}

wchar_t* GisStringUtility::MakeString(INT32 numStrings, wchar_t* * strings, wchar_t* separator)
{
    size_t  iLen = 1;   // 1 for EOS
    INT32     i;          // index

    for (i = 0;  i < numStrings;  i++)
    {
        if (NULL != separator && i > 0)
            iLen += GisStringUtility::StringLength(separator);
        if (strings[i] != NULL)
            iLen += GisStringUtility::StringLength(strings[i]);
    }

    wchar_t*    pszRet = new wchar_t[iLen];
    *pszRet = L'\0';

    for (i = 0;  i < numStrings;  i++)
    {
        if (NULL != separator && i > 0)
            GisStringUtility::StringConcatenate(pszRet, separator);
        if (strings[i] != NULL)
            GisStringUtility::StringConcatenate(pszRet, strings[i]);
    }

    return pszRet;
}

wchar_t* GisStringUtility::MakeString(unsigned char* pData, INT32 iDataSize)
{
    if (pData == NULL || iDataSize == 0)
    {
        return GisStringUtility::MakeString(GisStringUtility::NullString);
    }

    size_t  iLen = 3;       // 1 for eos, 2 for braces {}
    iLen += 5 * iDataSize;  // "\x12 " for each byte
    wchar_t*    szGeom = new wchar_t[iLen];

    INT32 iByte;
    GisStringUtility::StringCopy(szGeom, L"{");
    for (iByte = 0; iByte < iDataSize; iByte++)
    {
        wchar_t szByte[6];  // " \x12"
        INT32     iChar;

        iChar = 0;
        if (iByte != 0)
            szByte[iChar++] = L' ';

        szByte[iChar++] = L'\\';
        szByte[iChar++] = L'x';

        unsigned char     bNibble = (pData[iByte] >> 4) & 0x0F;
        if (bNibble > 0x09)
            szByte[iChar++] = L'A' + (bNibble - 10);
        else
            szByte[iChar++] = L'0' + bNibble;

        bNibble = pData[iByte] & 0x0F;
        if (bNibble > 0x09)
            szByte[iChar++] = L'A' + (bNibble - 10);
        else
            szByte[iChar++] = L'0' + bNibble;

        szByte[iChar++] = L'\0';
        GisStringUtility::StringConcatenate(szGeom, szByte);
    }
    GisStringUtility::StringConcatenate(szGeom, L"}");
    return szGeom;
}

void GisStringUtility::FormatDouble(double d, wchar_t* pszBuffer, INT32 iLen)
{
    FormatNumber(d, DBL_DIG, pszBuffer, iLen);
}

void GisStringUtility::FormatSingle(float f, wchar_t* pszBuffer, INT32 iLen)
{
    FormatNumber(f, FLT_DIG, pszBuffer, iLen);
}

void GisStringUtility::FormatNumber(double d, INT32 precision, wchar_t* pszBuffer, INT32 iLen)
{
    int             left;               // digits to left of decimal
    int             right;              // digits to right of decimal
    wchar_t*        end;                // end of formatted string
    wchar_t         radix = L'\0';      // radix character
    struct lconv    *nls;               // NLS info


    // Get NLS Info and extract the decimal separator character (if available)
    nls = localeconv();
    if (nls)
        radix = nls->decimal_point[0];
    if (radix == '\0')
        radix = '.';    // if not there, default to '.'

    // determine digits to left of decimal point.  note: add 1.0 before int cnv
    // so that: 1.n=>1, 0.n=>0 not 0.n=>1
    if (d > 0)
        left = (int)(log10(d) + 1.0);
    else if (d < 0)
        left = (int)(log10(-d) + 1.0);
    else
        left = 0;
    //  treat 0 like .nnnn ...
    if (left < 0)
        left = 0;

    // determine digits allowed on right, within precision
    right = precision - left;

    //  format with appropriate decimals
    if (right < 0)      // go to exponential format
    {
        right = 0;
#ifdef _WIN32
        _snwprintf(pszBuffer, iLen, L"%.*g", precision, d);
#else
        swprintf(pszBuffer, iLen, L"%.*g", precision, d);
#endif
        return;
    }
    else
#ifdef _WIN32
        _snwprintf(pszBuffer, iLen, L"%.*f", right, d);
#else
        swprintf(pszBuffer, iLen, L"%.*f", right, d);
#endif
    end = pszBuffer + GisStringUtility::StringLength(pszBuffer) - 1;

    //  note: sprintf rounds dec remove trailing '0's if there is a decimal
    if (right > 0)
    {
        while (*end == '0')
            end--;
    }

    //  remove radix if no decimal
    if (*end == radix)
        *end = L'\0';
    else
        *(end+1) = L'\0';   //  keep last non-zero

    // special case for "-0".  Change to "0"
    if (GisStringUtility::StringCompare(pszBuffer, L"-0") == 0)
        GisStringUtility::StringCopy(pszBuffer, L"0");
}

wchar_t* GisStringUtility::QuoteString(wchar_t* pszIn, wchar_t chQuote)
{
    // string functions below fail when pszIn == NULL
    if (pszIn == NULL || *pszIn == L'\0')
    {
        wchar_t*    pszRet = new wchar_t[3];
        pszRet[0] = chQuote;
        pszRet[1] = chQuote;
        pszRet[2] = L'\0';
        return pszRet;
    }

    size_t  iLen = GisStringUtility::StringLength(pszIn) + 3; // 1 for EOS and 2 for quotes.

    wchar_t*  psz = GisStringUtility::FindCharacter(pszIn, chQuote);
    while (psz != NULL)
    {
        iLen++;     // add 1 more for double quote

        psz++;
        psz = GisStringUtility::FindCharacter(psz, chQuote);
    }

    size_t      iPos = 0;
    wchar_t*    pszOut = new wchar_t[iLen];
    pszOut[iPos++] = chQuote;
    pszOut[iPos] = L'\0';

    wchar_t*  pszStart = pszIn;
    wchar_t*  pszEnd = GisStringUtility::FindCharacter(pszStart, chQuote);
    while (pszEnd != NULL)
    {
        // copy up to (but not including) quote char.
        GisStringUtility::SubstringCopy(&pszOut[iPos], pszStart, (pszEnd - pszStart));
        iPos += (pszEnd - pszStart);

        // copy quote char 2x
        pszOut[iPos++] = chQuote;
        pszOut[iPos++] = chQuote;
        pszOut[iPos] = L'\0';

        // skip over quote char and go again
        pszStart = pszEnd + 1;
        pszEnd = GisStringUtility::FindCharacter(pszStart, chQuote);
    }

    // no more quote chars, copy rest of string
    GisStringUtility::StringConcatenate(pszOut, pszStart);

    iPos = GisStringUtility::StringLength(pszOut);
    pszOut[iPos++] = chQuote;
    pszOut[iPos] = L'\0';
    return pszOut;
}

size_t GisStringUtility::StringLength(wchar_t* string)
{
    CHECKARGUMENTNULL(string, L"GisStringUtility.StringLength");

    return wcslen(string);
}

INT32 GisStringUtility::StringCompare(wchar_t* string1, wchar_t* string2)
{
    CHECKARGUMENTNULL(string1, L"GisStringUtility.StringCompare");
    CHECKARGUMENTNULL(string2, L"GisStringUtility.StringCompare");

    return wcscmp(string1, string2);
}

INT32 GisStringUtility::StringCompareNoCase(wchar_t* string1, wchar_t* string2)
{
    CHECKARGUMENTNULL(string1, L"GisStringUtility.StringCompareNoCase");
    CHECKARGUMENTNULL(string2, L"GisStringUtility.StringCompareNoCase");

#ifdef _WIN32
    return wcsicmp(string1, string2);
#else
    return wcscasecmp(string1, string2);
#endif
}

void GisStringUtility::StringCopy(wchar_t* string1, wchar_t* string2)
{
    CHECKARGUMENTNULL(string1, L"GisStringUtility.StringCopy");
    CHECKARGUMENTNULL(string2, L"GisStringUtility.StringCopy");

    wcscpy(string1, string2);
}

void GisStringUtility::SubstringCopy(wchar_t* string1, wchar_t* string2, size_t length)
{
    CHECKARGUMENTNULL(string1, L"GisStringUtility.SubstringCopy");
    CHECKARGUMENTNULL(string2, L"GisStringUtility.SubstringCopy");

    wcsncpy(string1, string2, length);
}

void GisStringUtility::StringConcatenate(wchar_t* string1, wchar_t* string2)
{
    CHECKARGUMENTNULL(string1, L"GisStringUtility.StringConcatenate");

    if (string2 == NULL)
        return;

    wcscat(string1, string2);
}

wchar_t* GisStringUtility::FindCharacter(wchar_t* string, wchar_t character)
{
    CHECKARGUMENTNULL(string, L"GisStringUtility.FindCharacter");

    return wcschr(string, character);
}
