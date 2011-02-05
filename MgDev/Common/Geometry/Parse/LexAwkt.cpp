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
#include "ParseAwkt.h"
#include <math.h>   // for pow()
#include <limits.h> // LONG_MAX and LONG_MIN
#ifndef _WIN32
#include <wctype.h>
#endif
#include <time.h>
#include "StringUtility.h"

#define CHR_NULL    '\0'

//////////////////////////////////////////////////////////////////////////////

static GisKeyWord g_aAwktWords[] =
{
  L"ARC",                   GisToken_ARC,
  L"BEZIERSEGMENT",         GisToken_BEZIERSEGMENT,
  L"CIRCLE",                GisToken_CIRCLE,
  L"CIRCULARARCSEGMENT",    GisToken_CIRCULARARCSEGMENT,
  L"CURVEPOLYGON",          GisToken_CURVEPOLYGON,
  L"CURVESTRING",           GisToken_CURVESTRING,
  L"CUSTOM",                GisToken_CUSTOM,
  L"GEOMETRYCOLLECTION",    GisToken_GEOMETRYCOLLECTION,
  L"GRIDTERRAIN",           GisToken_GRIDTERRAIN,
  L"LINESTRING",            GisToken_LINESTRING,
  L"LINESTRINGSEGMENT",     GisToken_LINESTRINGSEGMENT,
  L"MULTICURVEPOLYGON",     GisToken_MULTICURVEPOLYGON,
  L"MULTICURVESTRING",      GisToken_MULTICURVESTRING,
  L"MULTILINESTRING",       GisToken_MULTILINESTRING,
  L"MULTIPOINT",            GisToken_MULTIPOINT,
  L"MULTIPOLYGON",          GisToken_MULTIPOLYGON,
  L"POINT",                 GisToken_POINT,
  L"POLYGON",               GisToken_POLYGON,
  L"RECTANGLE",             GisToken_RECTANGLE,
  L"TINTERRAIN",            GisToken_TINTERRAIN,
  L"XY",                    GisToken_XY,
  L"XYM",                   GisToken_XYM,
  L"XYZ",                   GisToken_XYZ,
  L"XYZM",                  GisToken_XYZM,

(wchar_t*)0,  GisToken_Undefined
};
#define AWKTKEYWORDCOUNT   (sizeof(g_aAwktWords)/sizeof(g_aAwktWords[0])-1)

#ifdef _DEBUG
void GisLexAwkt::PrintToken(INT32 token)
{
    printf(" Awkt::");
    switch (token)
    {
    case GisToken_Undefined:
        printf("Undefined");
        break;
    case GisToken_End:
        printf("End");
        break;
    case GisToken_Start:
        printf("Start");
        break;
    case GisToken_Error:
        printf("!!!!Error!!!! invalid token");
        break;
    case GisToken_Integer:
        printf("%d (INT32)", m_integer);
        break;
    case GisToken_Double:
        printf("%g (double)", m_double);
        break;
    case GisToken_LeftParenthesis:
        printf("LeftParenthesis");
        break;
    case GisToken_RightParenthesis:
        printf("RightParenthesis");
        break;
    case GisToken_Comma:
        printf("Comma");
        break;

    default:
        {
            // Keywords
            INT32 i;
            for (i=0; i<AWKTKEYWORDCOUNT; i++)
            {
                if (g_aAwktWords[i].token == token)
                {
                    printf("%S (keyword)\n", g_aAwktWords[i].word);
                    return;
                }
            }
        }
        printf("UNDEFINED TOKEN IN TEST CODE--check GisTokens\n");
        break;
    }
    printf("\n");
}
#endif

static INT32 CompareInsensitive(wchar_t* pToken, wchar_t* pucString)
{
    if (!pucString && !pToken)
        return 0;

    if (!pucString  && pToken)
        return 1;

    if (pucString && !pToken)
        return -1;

    return GisStringUtility::StringCompareNoCase(pToken, pucString);
}

//  Search for token in the keyword array
static INT32 findtoken
(
wchar_t*            token,              // string token to find
const GisKeyWord    words[],            // array of keyword/token pairs
INT32                   number_of_words     // number of patterns
)
{
    register INT32 ii;             // Indexes are also needed. Sometimes ...
    register INT32 jj;
    register INT32 kk;

    // Split tokens array in two parts and select appropriate part
    ii  = 0 ;
    jj  = number_of_words-1 ;

    kk = (ii + jj) / 2;
    while (ii <= jj)
    {
        if (CompareInsensitive(token, words[kk].word) <= 0)
            jj = kk-1 ;

        if (CompareInsensitive(token, words[kk].word) >= 0)
            ii = kk+1 ;

        kk = (ii + jj) / 2;
    }

    if (CompareInsensitive(token, words[kk].word) == 0)
        return kk;
    else
        return -1;  // Token not found
}

INT32 GisLexAwkt::FindKeyWord(wchar_t* word, GisKeyWord* aKeyWords, INT32 cCount)
{
    INT32 ii;

    if ((ii = findtoken(word, aKeyWords, cCount)) >= 0)
         return aKeyWords[ii].token;

    return GisToken_Undefined;
}

//  return -1 if sign is true and 1 if it is false
inline INT32 ifac(bool sign)
{
   return sign ? (-1) : 1;
}

inline double make_double(GisUInt32 n, GisUInt32 f, INT32 m)
{
    return ((double)n+(double)f*pow(10.0, -(double)m));
}

GisLexAwkt::GisLexAwkt(wchar_t* str)
{
    m_line = str;
    // Cache the parsed string length to avoid poor performance of calls to wcslen in a large loop.
    m_length = GisStringUtility::StringLength(m_line);
    m_prevToken = GisToken_Start;
    m_lastToken = GisToken_Start;
    m_token = GisToken_Start;
    m_cprev = 0;
    m_cc = 0;

    m_double = 0.0;
    m_integer = 0;
    m_ch = if_getch();   // Get first character from input string
}

GisLexAwkt::~GisLexAwkt()
{
}

//  Return next character from the input stream.
//  Next character becomes current
const wchar_t GisLexAwkt::if_getch()
{
    wchar_t c;

    if ((size_t)m_cc >= m_length)
    {
        c = '\0';
    }
    else
    {
        c = m_line[m_cc++];
        if (c == '\n' || c == '\r')
            c = ' ';
    }
    return c;
}

//  Get Word from input stream
void GisLexAwkt::getword(wchar_t* pstr, INT32 maxCharacters)
{
    // TODO: check maximum length
    while (iswalnum(m_ch) || m_ch == '_')
    {
        *pstr = m_ch;
        pstr++;
        m_ch = if_getch();
    }
    *pstr = '\0';
}

// Get digits from input stream.
// This outputs characters to buffer with range checking, but no NULL termination.
// TODO: range checking
wchar_t* GisLexAwkt::getdigits(wchar_t* pstr)
{
    while (iswdigit(m_ch))
    {
        *pstr = m_ch;
        pstr++;
        m_ch = if_getch();
    }
    return pstr;
}

// Get unsigned number
GisUInt32 GisLexAwkt::get_unsigned()
{
    wchar_t     str[maxCharLength];     // Input char storage
    wchar_t*    pStr=str;

    // Get numbers from input stream
    pStr = getdigits(pStr);
    *pStr = CHR_NULL;
    if (GisStringUtility::StringLength(str) == 0)
    {
        return (GisUInt32) -1;      // No digits in input stream
    }
    // TODO: wtoi is NOT ANSI
#ifdef _WIN32
    return (GisUInt32) _wtoi(str);
#else
    return (GisUInt32) wcstol(str, NULL, 10);
#endif
}

//  Get number
bool GisLexAwkt::getnumber(bool sign)
{
// TODO: rewrite with my AutoSketch code
//    static char buffer [64];

    bool        dot;                    // Dot flag
    double      rnum;                   // Real number
    wchar_t     str[maxCharLength];     // Input char storage
    wchar_t*    pstr = str;

    if (sign)
    {
        *pstr = '-';
        pstr++;
    }

    m_token = GisToken_Integer;
    dot = false;

    // Get numbers from input stream
    pstr = getdigits(pstr);
    if (m_ch == '.')
    {
        // Point follows: Real number
        *pstr = m_ch;
        pstr++;
        m_ch = if_getch();
        pstr = getdigits(pstr);
        dot = true;
    }
    else
    {
        dot = false;
    }

    if (towupper(m_ch) == 'E')
    {
        // E follows: Float point
        *pstr = m_ch;
        pstr++;
        m_ch = if_getch();
        if (m_ch == '-' || m_ch =='+')
        {
            *pstr = m_ch;
            pstr++;
            m_ch = if_getch();
        }
        if (!iswdigit(m_ch))
        {
            // Exponent shall contain digits
            throw new MgNotFiniteNumberException(L"GisLexAwkt.getnumber", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        pstr = getdigits(pstr);
        *pstr = CHR_NULL;
#ifdef _WIN32
        rnum = _wtof(str);
#else
        rnum =  wcstod(str, NULL);
#endif

    }
    else if (dot == false)
    {
        // Integer number
        *pstr = CHR_NULL;
#ifdef _WIN32
        rnum = _wtof(str);
#else
        wchar_t *end;
        rnum =  wcstod(str, &end);
#endif
        if (rnum < (double)LONG_MAX && rnum > (double)LONG_MIN)
        {
            try
            {
                m_integer = (INT32)rnum;
                return true;
            }
            catch (...)
            {
            }
        }
    }
    else
    {
        *pstr = CHR_NULL;
#ifdef _WIN32
        rnum = _wtof(str);
#else
        rnum =  wcstod(str, NULL);
#endif
    }
    m_double = rnum;
    m_token = GisToken_Double;
    return true;
}

//  Find first non-blank symbol
const wchar_t GisLexAwkt::find_nonblank()
{
    // skip whitespace
    while (m_ch == ' ' || m_ch == '\t')
    {
        m_ch = if_getch() ;
    }
    return m_ch;
}

//  Return next character after the current one
const wchar_t GisLexAwkt::nextchar() const
{
    return m_line[m_cc];
}

//  Return previous character before the current one
const wchar_t GisLexAwkt::prevchar() const
{
    wchar_t c;
    return (m_cc<2) ? c = '\0': m_line[m_cc-2];
}

INT32 GisLexAwkt::GetToken()
{
    // Save start position of the previous symbol
    m_cprev = m_cc;

    // Save previous token
    m_lastToken = m_token;

    // Get next not blank character
    m_ch = find_nonblank();

    // Stream is over
    if (m_ch == '\0')
    {
        m_token = GisToken_End;
        m_prevToken = m_token;
        return m_token;
    }

    // Is it keyword ?
    if (iswalpha(m_ch))
    {
        wchar_t     word[maxCharLength];

        // Get keyword
        getword(word, sizeof(word)/sizeof(wchar_t));

        // Is it a valid keyword for this grammar?
        INT32   tktmp;
        if ((tktmp = FindKeyWord(word, g_aAwktWords, AWKTKEYWORDCOUNT)) != GisToken_Undefined)
        {
            m_token = tktmp;
        }
        else
        {
            m_token = GisToken_Undefined;
        }
    }

    // Is it a numeric constant?
    else if (iswdigit(m_ch))
    {
        // m_token is real or integer number
        getnumber(false);
    }

    // negative values
    else if (m_ch == '-')
    {
        m_ch = if_getch();
        m_ch = find_nonblank();
        getnumber(true);
    }

    // comma
    else if (m_ch == ',')
    {
        m_token = GisToken_Comma;
        m_ch = if_getch();

    }
    // left parenthesis
    else if (m_ch == '(')
    {
        m_token = GisToken_LeftParenthesis;
        m_ch = if_getch();

    }
    // right parenthesis
    else if (m_ch == ')')
    {
        m_token = GisToken_RightParenthesis;
        m_ch = if_getch();
    }

    // something bad
    else
    {
        m_token = GisToken_Error;
        m_ch = if_getch();
    }

    m_prevToken = m_token;
    return m_token;
}
