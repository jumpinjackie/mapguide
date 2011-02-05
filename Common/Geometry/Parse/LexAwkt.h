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

// Note: used SED substitution to fix getenv() prototype
// if you want tracing enabled you need to set environment
// variables of these names with a value of 1 to 9
//#define gis_awkt_YYDEBUG  1


#ifndef NOTOKEN_H
#include "yyAwkt.h"
#endif

#ifdef _DEBUG
//#define DEBUG_TRACETOKEN  1
//#define DEBUG_TRACEACTIONS    1
//#define DEBUG_TRACETREE   1
#endif

typedef enum GisToken
{
    /// bad stuff/processing...
  GisToken_Undefined = -3,     // Token is undefined
  GisToken_End = -2,           // End of string
  GisToken_Start = -1,         // Start of line
  GisToken_Error = 0,          // Lexical analyzer error or erroneous token

    /// data values (literals)
  GisToken_Double = 1,
  GisToken_Integer

} GisToken;

// FDO keywords
typedef struct _GisKeyWord
{
    wchar_t*    word;
    INT32   token;
} GisKeyWord;

// The GisLexAwkt class provides lexical analysis support for AWKT parsing,
// i.e. accepts data from the input stream, recognizes tokens
// in it and outputs their type together with appropriate attributes.
class GisLexAwkt
{
static const INT32 maxCharLength = 256; // only used for keywords or numbers

  public:
    /// must construct class with string to parse.
    explicit    GisLexAwkt(wchar_t*);
    virtual     ~GisLexAwkt();

    /// Read next token and its attributes
    INT32       GetToken();

    /// find keyword in table
    INT32       FindKeyWord(wchar_t*, GisKeyWord*, INT32);

    ///  Public data
    INT32       m_token;            // Token type
    INT32       m_lastToken;        // Predecessor
    double          m_double;           // token double value
    INT32       m_integer;          // token integer value
    INT32       m_cprev;            // Start of the token
#if _DEBUG
    void            PrintToken(INT32 token);
#endif

private:
    const wchar_t   nextchar        () const;       // Next character
    const wchar_t   prevchar        () const;       // Previous character
    const wchar_t   if_getch        ();             // Read next character
    const wchar_t   find_nonblank   ();             // Get next not blank symbol

    void            getword         (wchar_t*, INT32);   // Get keyword or identifier from input

    wchar_t*        getdigits       (wchar_t*);     // Get digits
    GisUInt32       get_unsigned    ();             // Get unsigned integer number
    bool            getnumber       (bool);         // Get number

    /// Lexical analyzer state
    wchar_t*        m_line;         // Points to parsed string
    size_t          m_length;       // Length of parsed string
    INT32       m_cc;           // Current position in the input string
    wchar_t         m_ch;           // Current character in the input string
    INT32       m_prevToken;    // Previous token type for internal use
};
