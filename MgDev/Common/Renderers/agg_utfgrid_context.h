//
//  Copyright (C) 2007-2017 by Autodesk, Inc.
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


#ifndef _AGGUTFGRIDCONTEXT_H_
#define _AGGUTFGRIDCONTEXT_H_

#pragma warning(push)
#pragma warning(disable: 4100 4244 4267 4512)

#include <map>
#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_scanline_bin.h"
#include "agg_renderer_scanline.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"

#include "MapUTFGrid.h"

#pragma warning(pop)

typedef agg::int32u utfgrid_band_type;
typedef agg::row_ptr_cache<utfgrid_band_type> utfgrid_rendering_buffer;
typedef pixfmt_utf<utfpix32, utfgrid_rendering_buffer> pixfmt_utf32;
typedef agg::renderer_base<pixfmt_utf32> utfgrid_renderer_base;
typedef agg::rasterizer_scanline_aa<> utfgrid_rasterizer_scanline;
typedef agg::renderer_scanline_bin_solid<utfgrid_renderer_base> utfgrid_renderer_scanline;

static utfpix32 UTF_WATER = utfpix32(32);

// encapsulates our rendering context
class agg_utfgrid_context
{
public:
    agg_utfgrid_context(int width, int height, unsigned int resolution)
    {
        m_resolution = resolution;
        int bufWidth = width / m_resolution;
        int bufHeight = height / m_resolution;

        size_t len = bufWidth * bufHeight * sizeof(utfgrid_band_type);
        m_rows = new utfgrid_band_type[len];
        int stride = bufWidth;
        rendering_buffer.attach(m_rows, bufWidth, bufHeight, stride);
        pixel_format.attach(rendering_buffer);
        renderer_base.attach(pixel_format);
        renderer_scanline.attach(renderer_base);
        renderer_base.clear(UTF_WATER);
        rasterizer.gamma(agg::gamma_none());
    }

    ~agg_utfgrid_context()
    {
        delete[] m_rows;
    }

    // rendering buffer
    unsigned int                m_resolution;
    utfgrid_band_type*          m_rows; //Not owned by this
    bool                        ownrows;
    utfgrid_rendering_buffer    rendering_buffer;
    pixfmt_utf32                pixel_format;
    agg::path_storage           ps;
    utfgrid_renderer_base       renderer_base;
    agg::scanline_bin           scanline_utf;
    utfgrid_rasterizer_scanline rasterizer;
    utfgrid_renderer_scanline   renderer_scanline;
};

#endif