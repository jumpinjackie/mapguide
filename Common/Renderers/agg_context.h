//
//  Copyright (C) 2007  Autodesk, Inc.
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

#ifndef AGG_CONTEXT_H
#define AGG_CONTEXT_H

#include <map>
#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_scanline_u.h"
#include "agg_scanline_bin.h"
#include "agg_renderer_scanline.h"
#include "agg_renderer_outline_aa.h"
#include "agg_renderer_outline_image.h"
#include "agg_renderer_primitives.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_rasterizer_outline_aa.h"
#include "agg_rasterizer_outline.h"
#include "agg_conv_curve.h"
#include "agg_conv_contour.h"
#include "agg_pixfmt_rgba.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"
#include "agg_font_freetype.h"
#include "agg_span_interpolator_linear.h"
#include "agg_image_accessors.h"
#include "agg_span_pattern_rgba.h"
#include "agg_trans_affine.h"
#include "agg_conv_transform.h"
#include "agg_span_image_filter_rgba.h"
#include "agg_span_image_filter_rgb.h"
#include "agg_span_allocator.h"
#include "agg_pattern_filters_rgba.h"
//#include "platform/agg_platform_support.h"
#include "agg_font_freetype.h"


//MG specific AGG template instatiations
enum flip_y_e { flip_y = true };

//#define pix_format agg::pix_format_bgra32
typedef agg::row_ptr_cache<agg::int8u> mg_rendering_buffer;

typedef agg::pixfmt_alpha_blend_rgba<agg::blender_bgra32, mg_rendering_buffer, agg::pixel32_type> mg_pixfmt_type; //----pixfmt_bgra32
typedef agg::pixfmt_alpha_blend_rgba<agg::blender_bgra32_pre, mg_rendering_buffer, agg::pixel32_type> mg_pixfmt_type_pre; //----pixfmt_bgra32
typedef agg::pixfmt_alpha_blend_rgba<agg::blender_rgba32, mg_rendering_buffer, agg::pixel32_type> mg_pixfmt_type_rgba; //----pixfmt_bgra32

typedef agg::renderer_base<mg_pixfmt_type> mg_ren_base;
typedef agg::renderer_base<mg_pixfmt_type_pre> mg_ren_base_pre;
typedef agg::renderer_outline_aa<mg_ren_base> mg_ren_base_o;
typedef agg::renderer_scanline_aa_solid<mg_ren_base> mg_ren_solid;

typedef agg::font_engine_freetype_int32 font_engine_type;
typedef agg::font_cache_manager<font_engine_type> font_manager_type;


//using this in contructor
#pragma warning(disable:4355)

//encapsulates our rendering context
class agg_context
{
public:
    agg_context(unsigned int* rows, int width, int height)
        : 
    lprof(1.0, agg::gamma_power(1.0)),
    ren_o(ren, lprof),
    ras_o(ren_o),
    fman(feng)
    {
        if (!rows)
        {
            ownrows = true;
            m_rows = new unsigned int[width*height];
        }
        else
        {
            ownrows = false;
            m_rows = rows;
        }

        int stride = width * sizeof(unsigned int);
        rb.attach((agg::int8u*)m_rows, width, height, (flip_y) ? -stride : stride);
        pf.attach(rb);
        pf_pre.attach(rb);
        ren.attach(pf);
        ren_pre.attach(pf_pre);
        ras.gamma(agg::gamma_power(1.0));
        ras.filling_rule(agg::fill_even_odd);
        feng.flip_y(!flip_y);
    }

    ~agg_context()
    {
        for (std::map<double, agg::line_profile_aa*>::iterator iter = h_lprof.begin();
            iter != h_lprof.end(); iter++)
            delete iter->second;

        if (ownrows)
            delete [] m_rows;
    }

    //rendering buffer
    unsigned int* m_rows;
    bool ownrows;
    mg_rendering_buffer rb;
    mg_pixfmt_type pf;
    mg_pixfmt_type_pre pf_pre;
    agg::path_storage ps;
    mg_ren_base ren;
    mg_ren_base_pre ren_pre;
    mg_ren_base_o ren_o;
    agg::rasterizer_scanline_aa<> ras;
    agg::rasterizer_outline_aa<mg_ren_base_o> ras_o;
    agg::scanline_u8 sl;
    
    agg::line_profile_aa lprof;
    std::map<double, agg::line_profile_aa*> h_lprof;

    font_engine_type feng;
    font_manager_type fman;
};


#endif
