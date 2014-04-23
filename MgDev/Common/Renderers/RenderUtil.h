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

#ifndef RENDERUTIL_H
#define RENDERUTIL_H

#include <vector>

#include "SE_Renderer.h"

class RenderUtil
{
public:
    // Draw screen raster with alpha
    // Alpha is a value between 0 and 1.
    // 0 means completely transparent, while 1 means completely opaque.
    static void DrawScreenRaster(SE_Renderer* render,
                                 unsigned char* data, int length,
                                 RS_ImageFormat format, int native_width, int native_height,
                                 double x, double y, double w, double h, double angledeg,
                                 double alpha);
};

#endif      //RENDERUTIL_H
