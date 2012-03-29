//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef AGGIMAGEIO_H
#define AGGIMAGEIO_H

class RS_ByteData;

class AGGImageIO
{
public:
    static bool Save(const RS_String& filename, const RS_String& format,
                     unsigned int* src, int src_width, int src_height,
                     int dst_width, int dst_height, RS_Color& bgColor);

    static RS_ByteData* Save(const RS_String& format,
                      unsigned int* src, int src_width, int src_height,
                      int dst_width, int dst_height, RS_Color& bgColor,
                      RS_ColorVector* baseColorPalette = NULL);

    static void Combine(const RS_String& src1, const RS_String& src2, const RS_String& dst);

    // NOTE: the returned buffer contains the *inverted* image
    static unsigned int* DecodePNG(const unsigned char* src, size_t len, int& width, int& height);

    // NOTE: the returned buffer contains the *non-inverted* image
    static unsigned int* DecodeJPEG(const unsigned char* src, size_t len, int& width, int& height);

private:
    static unsigned char* ReadFile(const RS_String& fname, size_t& len);
    static void UnmultiplyAlpha(unsigned int* argb, int len);
};

#endif      //AGGIMAGEIO_H
