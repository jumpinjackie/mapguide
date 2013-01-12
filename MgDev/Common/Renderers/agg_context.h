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

#ifndef AGG_CONTEXT_H
#define AGG_CONTEXT_H

#pragma warning(push)
#pragma warning(disable: 4100 4244 4267 4512)

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
#include "agg_font_freetype.h"
#include "agg_pixfmt_gray.h"
#include "agg_pixfmt_amask_adaptor.h"
#include "agg_alpha_mask_u8.h"

#include "agg_mg_overloads.h"

#pragma warning(pop)


// MG specific AGG template instatiations
enum flip_y_e
{
    flip_y = true
};

typedef agg::row_ptr_cache<agg::int8u> mg_rendering_buffer;

typedef pixfmt_alpha_blend_rgba_mg<agg::blender_bgra32    , mg_rendering_buffer, agg::pixel32_type>     mg_pixfmt_type_argb;
typedef pixfmt_alpha_blend_rgba_mg<agg::blender_bgra32_pre, mg_rendering_buffer, agg::pixel32_type>     mg_pixfmt_type_argb_pre;
typedef pixfmt_alpha_blend_rgba_mg<agg::blender_rgba32    , mg_rendering_buffer, agg::pixel32_type>     mg_pixfmt_type_abgr;
typedef pixfmt_alpha_blend_rgba_mg<agg::blender_rgba32_pre, mg_rendering_buffer, agg::pixel32_type>     mg_pixfmt_type_abgr_pre;

#ifdef MG_ORDER_ARGB
typedef mg_pixfmt_type_argb         mg_pixfmt_type;
typedef mg_pixfmt_type_argb_pre     mg_pixfmt_type_pre;
#else
typedef mg_pixfmt_type_abgr         mg_pixfmt_type;
typedef mg_pixfmt_type_abgr_pre     mg_pixfmt_type_pre;
#endif

// regular renderers
typedef agg::renderer_base<mg_pixfmt_type>                              mg_ren_base;
typedef agg::renderer_base<mg_pixfmt_type_pre>                          mg_ren_base_pre;
typedef agg::renderer_scanline_aa_solid<mg_ren_base>                    mg_ren_aa_solid;

// rasterizer
typedef agg::rasterizer_scanline_aa<>                                   mg_ras_aa;

// font types
typedef agg::font_engine_freetype_int32                                 font_engine_type;
typedef agg::font_cache_manager<font_engine_type>                       font_manager_type;
typedef agg::conv_curve<font_manager_type::path_adaptor_type>           font_curve_type;
typedef agg::conv_contour<font_curve_type>                              font_contour_type;

// alpha mask pixel format
typedef agg::renderer_base<agg::pixfmt_gray8>                           mg_alpha_mask_rb_type;
typedef agg::renderer_scanline_aa_solid<mg_alpha_mask_rb_type>          mg_alpha_mask_ren_type;

// renderers that make use of an alpha clip mask
typedef agg::amask_no_clip_gray8                                        mg_alpha_mask_type;
typedef agg::pixfmt_amask_adaptor<mg_pixfmt_type, mg_alpha_mask_type>   mg_pixfmt_clip_mask_type;
typedef agg::renderer_base<mg_pixfmt_clip_mask_type>                    mg_clip_mask_ren_base;
typedef agg::renderer_scanline_aa_solid<mg_clip_mask_ren_base>          mg_clip_mask_ren_aa_solid;


struct RS_Font;

// encapsulates our rendering context
class agg_context
{
public:
    agg_context(unsigned int* rows, int width, int height)
        : fman(feng)
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
        rb.attach((agg::int8u*)m_rows, width, height, flip_y? -stride : stride);
        pf.attach(rb);
        pf_pre.attach(rb);
        ren.attach(pf);
        ren_pre.attach(pf_pre);
        ras.gamma(agg::gamma_power(1.0));
        ras.filling_rule(agg::fill_even_odd);
        ras.clip_box(0, 0, width, height);

        feng.flip_y(!flip_y);
        feng.hinting(true);
        feng.transform(agg::trans_affine());
        last_font_height = 0.0;
        last_font = NULL;
        last_glyph_type = agg::glyph_ren_agg_gray8;

        // polygon clip buffer
        bPolyClip = false;
        mask_buf = new unsigned char[width * height];
        mask_rbuf.attach((agg::int8u*)mask_buf, width, height, width);
        mask_mask.attach(mask_rbuf);
        mask_pixf = new agg::pixfmt_gray8(mask_rbuf);
        mask_rb.attach(*mask_pixf);
        mask_ren.attach(mask_rb);

        clip_pixf.attach(rb);
        clip_mask = new mg_pixfmt_clip_mask_type(clip_pixf, mask_mask);
        clip_rb.attach(*clip_mask);
        clip_ren.attach(clip_rb);
    }

    ~agg_context()
    {
        if (ownrows)
            delete [] m_rows;

        delete clip_mask;
        delete mask_pixf;
        delete [] mask_buf;
    }

    // rendering buffer
    unsigned int*               m_rows;
    bool                        ownrows;
    mg_rendering_buffer         rb;
    mg_pixfmt_type              pf;
    mg_pixfmt_type_pre          pf_pre;
    agg::path_storage           ps;
    mg_ren_base                 ren;
    mg_ren_base_pre             ren_pre;
    mg_ras_aa                   ras;
    agg::scanline_u8            sl;

    // polyclip mask rendering buffer
    bool                        bPolyClip;
    unsigned char*              mask_buf;
    agg::rendering_buffer       mask_rbuf;
    mg_alpha_mask_type          mask_mask;
    mg_alpha_mask_rb_type       mask_rb;
    mg_alpha_mask_ren_type      mask_ren;
    agg::pixfmt_gray8*          mask_pixf;

    // renderers that do alpha masking
    mg_clip_mask_ren_base       clip_rb;
    mg_clip_mask_ren_aa_solid   clip_ren;
    mg_pixfmt_type              clip_pixf;
    mg_pixfmt_clip_mask_type*   clip_mask;

    // font engine state caching
    font_engine_type feng;
    font_manager_type fman;

    double last_font_height;
    const RS_Font* last_font;
    agg::trans_affine last_font_transform;
    agg::glyph_rendering last_glyph_type;
};

#endif
