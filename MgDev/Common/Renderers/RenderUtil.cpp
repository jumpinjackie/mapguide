//
//  Copyright (C) 2010-2011 by Autodesk, Inc.
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
#include "RendererStyles.h"
#include "AGGImageIO.h"
#include "RenderUtil.h"

void RenderUtil::DrawScreenRaster(SE_Renderer* render,
                                 unsigned char* data, int length,
                                 RS_ImageFormat format, int native_width, int native_height,
                                 double x, double y, double w, double h, double angledeg,
                                 double alpha)
{
    if (format == RS_ImageFormat_Unknown)
        return;

    if (alpha <= 0.0)
        return;

    if (alpha >= 1.0)
    {
        render->DrawScreenRaster(data, length, format, native_width, native_height, x, y, w, h, angledeg);
        return;
    }

    // if it's PNG, decode it and come back around
    if (format == RS_ImageFormat_PNG)
    {
        unsigned int* decoded = AGGImageIO::DecodePNG(data, length, native_width, native_height);
        if (decoded)
        {
            DrawScreenRaster(render,
                             (unsigned char*)decoded,
                             native_width * native_height * 4,
                             RS_ImageFormat_ARGB,
                             native_width,
                             native_height,
                             x, y,
                             w, h,
                             angledeg, alpha);
        }

        delete [] decoded;
    }
    else if (format == RS_ImageFormat_ABGR || format == RS_ImageFormat_ARGB)
    {
        // set alpha value for each pixel
        unsigned char* end = data + 4 * native_width * native_height;
        for (unsigned char* pixel = data; pixel < end; pixel += 4)
            pixel[3] = (unsigned char)((float)pixel[3] * (float)alpha);
        render->DrawScreenRaster(data, length, format, native_width, native_height, x, y, w, h, angledeg);
    }
    else
    {
        // TODO: set alpha for other format
        render->DrawScreenRaster(data, length, format, native_width, native_height, x, y, w, h, angledeg);
    }
}
