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

#ifndef RASTERADAPTER_H_
#define RASTERADAPTER_H_

#include "GeometryAdapter.h"

class GridData;
class GridStylizer;

class RasterAdapter : public GeometryAdapter
{
public:
    RasterAdapter(LineBufferPool* lbp);
    virtual ~RasterAdapter();

    virtual void Stylize(Renderer*                   renderer,
                         RS_FeatureReader*           features,
                         bool                        initialPass,
                         FdoExpressionEngine*        exec,
                         RS_Raster*                  raster,
                         MdfModel::GridColorStyle*   style,
                         MdfModel::GridSurfaceStyle* surfStyle = NULL,
                         const MdfModel::MdfString*  tooltip = NULL,
                         const MdfModel::MdfString*  url = NULL,
                         RS_ElevationSettings*       elevSettings = NULL,
                         CSysTransformer*            layer2mapxformer = NULL);

    void DecodeRGBA(RS_InputStream* is, unsigned char* dst, int w, int h);
    void DecodeRGB(RS_InputStream* is, unsigned char* dst, int w, int h);
    void DecodeMapped(RS_InputStream* is, RS_InputStream* pal, unsigned char* dst, int w, int h);
    void DecodeBitonal(RS_InputStream* is, const RS_Color& fg, const RS_Color& bg, unsigned char* dst, int w, int h);
};

#endif
