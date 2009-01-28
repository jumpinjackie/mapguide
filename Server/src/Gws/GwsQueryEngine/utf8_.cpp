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
#include "utf8_.h"
#ifdef _WIN32
#include <mbstring.h>
#else
#include <inttypes.h>
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Fast and portable UTF-8 conversion
// Taken from DWF Toolkit string class
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

#define _WCHAR_MAX      SHRT_MAX
#define _SHRT_MAX       SHRT_MAX
#define _INT_MAX        INT_MAX

typedef char                    ASCII_char_t;
typedef wchar_t                 UCS2_char_t;
typedef unsigned __int32        UCS4_char_t;

#else

#define _WCHAR_MAX      __WCHAR_MAX
#define _SHRT_MAX       __SHRT_MAX__
#define _INT_MAX        __INT_MAX__

#if (_WCHAR_MAX == _SHRT_MAX)

typedef char         ASCII_char_t;
typedef wchar_t      UCS2_char_t;
typedef uin32_t UCS4_char_t;

#else

typedef char        ASCII_char_t;
typedef uint16_t      UCS2_char_t;
typedef wchar_t     UCS4_char_t;

#endif

#endif

//SDF only works on little endian currently
#define LITTLE_ENDIAN_SYSTEM


///////////////////////////////////////////////////////////////////////////////
int EncodeUTF8(     const UCS2_char_t*   zUCS2String,
                    size_t               nUCS2StringBytes,
                    ASCII_char_t*        pUTF8Buffer,
                    size_t               nUTF8BufferBytes )
{
    unsigned char cHi = 0,
                  cLo = 0;
    char* pIn = (char*)zUCS2String;
    char* pOut = (char*)pUTF8Buffer;

    size_t nUsed = 0;
    size_t nAllowed = nUTF8BufferBytes - sizeof(ASCII_char_t);
    size_t nLen = nUCS2StringBytes >> 1;

    for (register size_t i = 0; i < nLen; i++, pIn+=2)
    {
        //check for null terminated string
        if (zUCS2String[i] == 0)
            break;

#ifdef LITTLE_ENDIAN_SYSTEM
        cHi = *pIn;
        cLo = *(pIn+1);
#else
        cLo = *pIn;
        cHi = *(pIn+1);
#endif

        if ((cLo == 0) && (cHi < 0x80))
        {
            if ((++nUsed > nAllowed) && pOut)
            {
                *pOut = 0;
                return -1;
                //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small" );
            }

            if (pOut)
            {
                *(pOut++) = cHi;
            }
        }
        else if (cLo < 0x08)
        {
            if (((nUsed+=2) > nAllowed) && pOut)
            {
                *pOut = 0;
                return -1;
                //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small" );
            }

            if (pOut)
            {
                *(pOut++) = (0xc0 | (cLo << 2) | (cHi >> 6));           // 110lllhh
                *(pOut++) = (0x80 | (cHi & 0x3f));                      // 10hhhhhh
            }
        }
        else
        {
            if (((nUsed+=3) > nAllowed) && pOut)
            {
                *pOut = 0;
                return -1;
                //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small" );
            }

            if (pOut)
            {
                *(pOut++) = (0xe0 | (cLo >> 4));                        // 1110llll
                *(pOut++) = (0x80 | ((cLo & 0x0f) << 2) | cHi >> 6);    // 10llllhh
                *(pOut++) = (0x80 | (cHi & 0x3f));                      // 10hhhhhh
            }
        }
    }

    if (pOut)
    {
        *(pOut++) = 0;
    }

    return (int)nUsed;
}


///////////////////////////////////////////////////////////////////////////////
int EncodeUTF8( const UCS4_char_t*   zUCS4String,
                       size_t               nUCS4StringBytes,
                       ASCII_char_t*        pUTF8Buffer,
                       size_t               nUTF8BufferBytes )
{
    unsigned char a = 0,
                  b = 0,
                  c = 0,
                  d = 0;
    char* pIn = (char*)zUCS4String;
    char* pOut = (char*)pUTF8Buffer;

    size_t nUsed = 0;
    size_t nAllowed = nUTF8BufferBytes - sizeof(ASCII_char_t);
    size_t nLen = nUCS4StringBytes >> 2;
    register size_t i;

    for (i = 0; i < nLen; i++, pIn+=4)
    {
        //check for null terminated string
        if (zUCS4String[i] == 0)
            break;

#ifdef  LITTLE_ENDIAN_SYSTEM
        d = *pIn;
        c = *(pIn+1);
        b = *(pIn+2);
        a = *(pIn+3);
#else
        a = *pIn;
        b = *(pIn+1);
        c = *(pIn+2);
        d = *(pIn+3);
#endif

        if (a >= 0xd8)
        {
            return -1;
            //_DWFCORE_THROW( DWFNotImplementedException, L"Unicode surrogate pairs not yet supported" );
        }
        else if (a >= 0x80)
        {
            return -1;
            //_DWFCORE_THROW( DWFInvalidArgumentException, L"Not a valid Unicode string" );
        }
        else if (a >= 0x04)
        {
            if (((nUsed+=6) > nAllowed) && pOut)
            {
                *pOut = 0;
                return -1;
                //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small" );
            }

            if (pOut)
            {
                *(pOut++) = (0xfc | (a >> 6));                          // 1111110a
                *(pOut++) = (0x80 | (a & 0x3f));                        // 10aaaaaa
                *(pOut++) = (0x80 | (b >> 2));                          // 10bbbbbb
                *(pOut++) = (0x80 | ((b & 0x03) << 4) | (c >> 4));      // 10bbcccc
                *(pOut++) = (0x80 | ((c & 0x0f) << 2) | (d >> 6));      // 10ccccdd
                *(pOut++) = (0x80 | (d & 0x3f));                        // 10dddddd
            }
        }
        else if ((a > 0x0) || (b >= 0x20))
        {
            if (((nUsed+=5) > nAllowed) && pOut)
            {
                *pOut = 0;
                return -1;
                //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small" );
            }

            if (pOut)
            {
                *(pOut++) = (0xf8 | a);                                 // 111110aa
                *(pOut++) = (0x80 | (b >> 2));                          // 10bbbbbb
                *(pOut++) = (0x80 | ((b & 0x03) << 4) | (c >> 4));      // 10bbcccc
                *(pOut++) = (0x80 | ((c & 0x0f) << 2) | (d >> 6));      // 10ccccdd
                *(pOut++) = (0x80 | (d & 0x3f));                        // 10dddddd
            }
        }
        else if (b >= 0x01)
        {
            if (((nUsed+=4) > nAllowed) && pOut)
            {
                *pOut = 0;
                return -1;
                //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small" );
            }

            if (pOut)
            {
                *(pOut++) = (0xf0 | (b >> 2));                          // 11110bbb
                *(pOut++) = (0x80 | ((b & 0x03) << 4) | (c >> 4));      // 10bbcccc
                *(pOut++) = (0x80 | ((c & 0x0f) << 2) | (d >> 6));      // 10ccccdd
                *(pOut++) = (0x80 | (d & 0x3f));                        // 10dddddd
            }
        }
        else if (c >= 0x08)
        {
            if (((nUsed+=3) > nAllowed) && pOut)
            {
                *pOut = 0;
                return -1;
                //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small" );
            }

            if (pOut)
            {
                *(pOut++) = (0xe0 | (c >> 4));                          // 1110cccc
                *(pOut++) = (0x80 | ((c & 0x0f) << 2) | d >> 6);        // 10ccccdd
                *(pOut++) = (0x80 | (d & 0x3f));                        // 10dddddd
            }
        }
        else if ((c > 0x0) || (d >= 0x80))
        {
            if (((nUsed+=2) > nAllowed) && pOut)
            {
                *pOut = 0;
                return -1;
                //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small" );
            }

            if (pOut)
            {
                *(pOut++) = (0xc0 | (c << 2) | (d >> 6));               // 110cccdd
                *(pOut++) = (0x80 | (d & 0x3f));                        // 10dddddd
            }
        }
        else
        {
            if ((++nUsed > nAllowed) && pOut)
            {
                *pOut = 0;
                return -1;
                //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small" );
            }

            if (pOut)
            {
                *(pOut++) = d;
            }
        }
    }

    if (pOut)
    {
        *(pOut++) = 0;
    }

    return (int)nUsed;
}


///////////////////////////////////////////////////////////////////////////////
int DecodeUTF8(     const ASCII_char_t*  zUTF8String,
                       size_t               nUTF8StringBytes,
                       UCS2_char_t*         pUCS2Buffer,
                       size_t               nUCS2BufferBytes )
{
    unsigned char  c1 = 0,
                   c2 = 0,
                   c3 = 0;
    char* pIn = (char*)zUTF8String;
    char* pOut = (char*)pUCS2Buffer;
    size_t nUsed = 0;
    size_t nAllowed = nUCS2BufferBytes - sizeof(UCS2_char_t);

    for (register size_t i = 0; i < nUTF8StringBytes; i++, pIn++)
    {
        if (*pIn == 0)
            break;

        if (((nUsed+=2) > nAllowed) && pOut)
        {
            *pOut = 0;
            return -1;
            //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small." );
        }

        c1 = *pIn;

            //
            // one byte
            //
        if (c1 < 0x80)
        {

#ifdef  LITTLE_ENDIAN_SYSTEM
            *(pOut++) = c1;
            *(pOut++) = 0x00;
#else
            *(pOut++) = 0x00;
            *(pOut++) = c1;
#endif

        }
            //
            // three bytes
            //
        else if ((c1 & 0xf0) == 0xe0)
        {
            c2 = *(pIn+1);
            c3 = *(pIn+2);

#ifdef  LITTLE_ENDIAN_SYSTEM
            *(pOut++) = (((c2 & 0x03) << 6) |  (c3 & 0x3f));
            *(pOut++) = (((c1 & 0x0f) << 4) | ((c2 & 0x3c) >> 2));
#else
            *(pOut++) = (((c1 & 0x0f) << 4) | ((c2 & 0x3c) >> 2));
            *(pOut++) = (((c2 & 0x03) << 6) |  (c3 & 0x3f));
#endif

            i += 2;
            pIn += 2;
        }
            //
            // two bytes
            //
        else
        {
            c2 = *(pIn+1);

#ifdef  LITTLE_ENDIAN_SYSTEM
            *(pOut++) = (((c1 & 0x03) << 6) | (c2 & 0x3f));
            *(pOut++) = ((c1 & 0x1c) >> 2);
#else
            *(pOut++) = ((c1 & 0x1c) >> 2);
            *(pOut++) = (((c1 & 0x03) << 6) | (c2 & 0x3f));
#endif

            i++;
            pIn++;
        }
    }

    *(pOut++) = 0;
    *(pOut++) = 0;

    return (int)nUsed;
}


///////////////////////////////////////////////////////////////////////////////
int DecodeUTF8(     const ASCII_char_t*  zUTF8String,
                       size_t               nUTF8StringBytes,
                       UCS4_char_t*         pUCS4Buffer,
                       size_t               nUCS4BufferBytes )
{
    unsigned char  a = 0,
                   b = 0,
                   c = 0,
                   d = 0,
                   e = 0,
                   f = 0;

    char* pIn = (char*)zUTF8String;
    char* pOut = (char*)pUCS4Buffer;
    size_t nUsed = 0;
    size_t nAllowed = nUCS4BufferBytes - sizeof(UCS4_char_t);
    register size_t i;

    for (i = 0; i < nUTF8StringBytes; i++, pIn++)
    {
        if (*pIn == 0)
            break;

        if ((nUsed+=4) > nAllowed)
        {
            *pOut = 0;
            return -1;
            //_DWFCORE_THROW( DWFOverflowException, L"Buffer too small." );
        }

        a = *pIn;

            //
            // six bytes
            //
        if ((a & 0xfc) == 0xfc)
        {
            b = *(pIn+1);
            c = *(pIn+2);
            d = *(pIn+3);
            e = *(pIn+4);
            f = *(pIn+5);

#ifdef  LITTLE_ENDIAN_SYSTEM
            *(pOut++) = ((e << 6) | (f & 0x3f));
            *(pOut++) = ((d << 4) | ((e & 0x3c) >> 2));
            *(pOut++) = ((c << 2) | ((d & 0x30) >> 4));
            *(pOut++) = ((a << 6) | (b & 0x3f));
#else
            *(pOut++) = ((a << 6) | (b & 0x3f));
            *(pOut++) = ((c << 2) | ((d & 0x30) >> 4));
            *(pOut++) = ((d << 4) | ((e & 0x3c) >> 2));
            *(pOut++) = ((e << 6) | (f & 0x3f));
#endif

            pIn += 5;
        }
            //
            // five bytes
            //
        else if ((a & 0xf8) == 0xf8)
        {
            b = *(pIn+1);
            c = *(pIn+2);
            d = *(pIn+3);
            e = *(pIn+4);

#ifdef  LITTLE_ENDIAN_SYSTEM
            *(pOut++) = ((d << 6) | (e & 0x3f));
            *(pOut++) = ((c << 4) | ((d & 0x3c) >> 2));
            *(pOut++) = ((b << 2) | ((c & 0x30) >> 4));
            *(pOut++) = (a & 0x03);
#else
            *(pOut++) = (a & 0x03);
            *(pOut++) = ((b << 2) | ((c & 0x30) >> 4));
            *(pOut++) = ((c << 4) | ((d & 0x3c) >> 2));
            *(pOut++) = ((d << 6) | (e & 0x3f));
#endif

            pIn += 4;
        }
            //
            // four bytes
            //
        else if ((a & 0xf0) == 0xf0)
        {
            b = *(pIn+1);
            c = *(pIn+2);
            d = *(pIn+3);

#ifdef  LITTLE_ENDIAN_SYSTEM
            *(pOut++) = ((c << 6) | (d & 0x3f));
            *(pOut++) = ((b << 4) | ((c & 0x3c) >> 2));
            *(pOut++) = (((a & 0x07) << 2) | ((b & 0x30) >> 4));
            *(pOut++) = 0x00;
#else
            *(pOut++) = 0x00;
            *(pOut++) = (((a & 0x07) << 2) | ((b & 0x30) >> 4));
            *(pOut++) = ((b << 4) | ((c & 0x3c) >> 2));
            *(pOut++) = ((c << 6) | (d & 0x3f));
#endif

            pIn += 3;
        }

            //
            // one byte
            //
        else if (a < 0x80)
        {

#ifdef  LITTLE_ENDIAN_SYSTEM
            *(pOut++) = a;
            *(pOut++) = 0x00;
            *(pOut++) = 0x00;
            *(pOut++) = 0x00;
#else
            *(pOut++) = 0x00;
            *(pOut++) = 0x00;
            *(pOut++) = 0x00;
            *(pOut++) = a;
#endif

        }
            //
            // three bytes
            //
        else if ((a & 0xf0) == 0xe0)
        {
            b = *(pIn+1);
            c = *(pIn+2);

#ifdef  LITTLE_ENDIAN_SYSTEM
            *(pOut++) = (((b & 0x03) << 6) |  (c & 0x3f));
            *(pOut++) = (((a & 0x0f) << 4) | ((b & 0x3c) >> 2));
            *(pOut++) = 0x00;
            *(pOut++) = 0x00;
#else
            *(pOut++) = 0x00;
            *(pOut++) = 0x00;
            *(pOut++) = (((a & 0x0f) << 4) | ((b & 0x3c) >> 2));
            *(pOut++) = (((b & 0x03) << 6) |  (c & 0x3f));
#endif

            i += 2;
            pIn += 2;
        }
            //
            // two bytes
            //
        else
        {
            b = *(pIn+1);

#ifdef  LITTLE_ENDIAN_SYSTEM
            *(pOut++) = (((a & 0x03) << 6) | (b & 0x3f));
            *(pOut++) = ((a & 0x1c) >> 2);
            *(pOut++) = 0x00;
            *(pOut++) = 0x00;
#else
            *(pOut++) = 0x00;
            *(pOut++) = 0x00;
            *(pOut++) = ((a & 0x1c) >> 2);
            *(pOut++) = (((a & 0x03) << 6) | (b & 0x3f));
#endif

            i++;
            pIn++;
        }
    }

    *((UCS4_char_t*)pOut++) = 0;

    return (int)nUsed;
}


///////////////////////////////////////////////////////////////////////////////
//
// The interface used by SDF
//
///////////////////////////////////////////////////////////////////////////////

int ut_utf8_to_unicode(const char* str_in, size_t in_size, wchar_t* Wtext, size_t out_size)
{
    int ret = DecodeUTF8(str_in, in_size, Wtext, out_size * sizeof(wchar_t));
    return ret / sizeof(wchar_t);
}


int ut_utf8_from_unicode(const wchar_t* Wtext, size_t in_size, char* str_out, int out_size)
{
    return EncodeUTF8(Wtext, in_size * sizeof(wchar_t), str_out, out_size);
}


char* gws_mbscat(char *strDestination, const char *strSource)
{
#ifdef _WIN32
    return (char*)_mbscat((unsigned char *) strDestination, (const unsigned char*) strSource);
#else
    return strcat(strDestination, strSource);
#endif
}
