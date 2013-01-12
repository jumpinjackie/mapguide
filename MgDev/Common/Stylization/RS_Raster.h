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

#ifndef RS_RASTER_H_
#define RS_RASTER_H_

#include "RS_InputStream.h"
#include "Bounds.h"
#include "RendererStyles.h"

class RS_Raster
{
public:
    RS_Raster() {}
    virtual ~RS_Raster() {}

    virtual RS_Bounds       GetExtent()         = 0;

    virtual int             GetOriginalWidth()  = 0;
    virtual int             GetOriginalHeight() = 0;

    virtual int             GetBitsPerPixel()   = 0;
    virtual int             GetDataModelType()  = 0;

    virtual RS_InputStream* GetPalette()        = 0;

    virtual RS_InputStream* GetStream(RS_ImageFormat format, int width, int height) = 0;
    virtual int             GetDataType()       = 0;

    virtual RS_String       GetVerticalUnits()  = 0;

    virtual int             GetNullValueType()  = 0;
    virtual long long       GetNullValueData()  = 0;
};

#endif
