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

#ifndef __Base64_H
#define __Base64_H

class Base64
{
public:
    static unsigned long Encode(char *dest, const unsigned char *src, unsigned long length);
    static unsigned long Decode(unsigned char *dest, const char *src, unsigned long length);
    static unsigned long GetEncodedLength(unsigned long length);
    static unsigned long GetDecodedLength(unsigned long length);
    static bool IsBase64(char c);

private:
    static char Encode(unsigned char uc);
    static unsigned char Decode(char c);
};


inline unsigned long Base64::Encode(char *dest, const unsigned char *src, unsigned long length)
{
    unsigned long k = 0;
    if (length > 0)
    {
        for (unsigned long i=0; i<length; i+=3)
        {
            // obtain the first 3 bytes
            unsigned char by1 = src[i];
            unsigned char by2 = 0;
            unsigned char by3 = 0;
            if (i+1 < length)
            {
                by2 = src[i+1];
            }

            if (i+2 < length)
            {
                by3 = src[i+2];
            }

            // shift the bits from three 8-bit values to four 6-bit values
            unsigned char by4 = by1>>2;
            unsigned char by5 = ((by1&0x3)<<4)|(by2>>4);
            unsigned char by6 = ((by2&0xf)<<2)|(by3>>6);
            unsigned char by7 = by3&0x3f;

            // encode the data, add it to the string
            dest[k++] = Encode(by4);
            dest[k++] = Encode(by5);

            if (i+1 < length)
            {
                dest[k++] = Encode(by6);
            }
            else
            {
                dest[k++] = '=';
            }

            if (i+2 < length)
            {
                dest[k++] = Encode(by7);
            }
            else
            {
                dest[k++] = '=';
            }
        }
    }

    return k;
}


inline unsigned long Base64::Decode(unsigned char *dest, const char *src, unsigned long encoded_len)
{
    unsigned long k = 0;
    unsigned long decoded_len = GetDecodedLength(encoded_len);
    if (encoded_len > 0 && decoded_len > 0)
    {
        for (unsigned long j=0; j<encoded_len; j+=4)
        {
            // obtain the 4 bytes of encoded data
            char c1 = src[j];
            char c2 = 'A';
            char c3 = 'A';
            char c4 = 'A';

            if (j+1 < encoded_len)
            {
                c2 = src[j+1];
            }
            if (j+2 < encoded_len)
            {
                c3 = src[j+2];
            }
            if (j+3 < encoded_len)
            {
                c4 = src[j+3];
            }

            // decode the data
            unsigned char by1 = Decode(c1);
            unsigned char by2 = Decode(c2);
            unsigned char by3 = Decode(c3);
            unsigned char by4 = Decode(c4);

            // shift the bits, add to the byte array
            dest[k++] = (by1<<2)|(by2>>4);

            if (c3 != '=')
            {
                dest[k++] = (by2&0xf)<<4|(by3>>2);
            }

            if (c4 != '=')
            {
                dest[k++] = (by3&0x3)<<6|by4;
            }
        }
    }

    return k;
}


inline char Base64::Encode(unsigned char uc)
{
    char result = '/';

    if (uc < 26)
    {
        result = 'A' + uc;
    }
    else if (uc < 52)
    {
        result = 'a' + (uc - 26);
    }
    else if (uc < 62)
    {
        result = '0' + (uc - 52);
    }
    else if (uc == 62)
    {
        result = '+';
    }

    return result;
}


inline unsigned char Base64::Decode(char c)
{
    unsigned char retval = 63;

    if (c >= 'A' && c <= 'Z')
    {
        retval = c - 'A';
    }
    else if (c >= 'a' && c <= 'z')
    {
        retval = c - 'a' + 26;
    }
    else if (c >= '0' && c <= '9')
    {
        retval = c - '0' + 52;
    }
    else if (c == '+')
    {
        retval = 62;
    }

    return retval;
}


inline bool Base64::IsBase64(char c)
{
    bool retval = false;

    if (c >= 'A' && c <= 'Z')
    {
        retval = true;
    }
    else if (c >= 'a' && c <= 'z')
    {
        retval = true;
    }
    else if (c >= '0' && c <= '9')
    {
        retval = true;
    }
    else if (c == '+')
    {
        retval = true;
    }
    else if (c == '/')
    {
        retval = true;
    }
    else if (c == '=')
    {
        retval = true;
    }

    return retval;
}


inline unsigned long Base64::GetEncodedLength(unsigned long length)
{
    // returns 4 bytes for every 3
    return (((length + 3 - 1) / 3) * 4);
}


inline unsigned long Base64::GetDecodedLength(unsigned long length)
{
    // returns 3 bytes for every 4
    // must be a multiple of 4

    unsigned long result = 0;
    if (length % 4 == 0)
    {
        result = ((length + 4 - 1) / 4) * 3;
    }

    return result;
}

#endif //__Base64_H
