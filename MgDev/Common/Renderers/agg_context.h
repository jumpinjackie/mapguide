//
//  Copyright (C) 2007 by Autodesk, Inc.
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
//#include "platform/agg_platform_support.h"
#include "agg_font_freetype.h"
#include "agg_pixfmt_gray.h"
#include "agg_pixfmt_amask_adaptor.h"
#include "agg_alpha_mask_u8.h"

#pragma warning(pop)

#if USE_SSE

static void __fastcall set_row_sse(int count, unsigned int* dst, unsigned int color)
{
    //going in, ecx contains count and dst is in edx, according to the __fastcall
    //calling convention
    __asm
    {
        mov eax, [color]
        movd xmm0, eax
        pshufd xmm0, xmm0, 0 ;replicate the color 4 time in the SSE register
        mov edi, edx;
        and edx, 0xF ;do some gymnastics to align the starting address on a 16 byte boundary
        jz do0       ;it is already aligned, go directly to the SSE loop
        sub edx, 8   ;bummer, we need to do 1, 2 or 3 pixels using regular movs
        jz do2
        jg do1
/*do3:*/mov [edi], eax
        add edi, 4
        dec ecx
do2:    mov [edi], eax
        add edi, 4
        dec ecx
do1:    mov [edi], eax
        add edi, 4
        dec ecx
do0:    ;SSE loop starts here -- set 32 pixels each time we loop
        sub ecx, 32
loop1:  movaps xmmword ptr [edi], xmm0;
        movaps xmmword ptr [edi+10h], xmm0;
        movaps xmmword ptr [edi+20h], xmm0;
        movaps xmmword ptr [edi+30h], xmm0;
        movaps xmmword ptr [edi+40h], xmm0;
        movaps xmmword ptr [edi+50h], xmm0;
        movaps xmmword ptr [edi+60h], xmm0;
        movaps xmmword ptr [edi+70h], xmm0;
        add edi, 80h
        sub ecx, 32
        jg loop1

        ;finish setting remaining <32 pixels using a non-SSE loop
        cld
        add ecx, 32
        rep stosd 
    }
}

#endif

template<class Blender, class RenBuf, class PixelT = int32u> 
class pixfmt_alpha_blend_rgba_mg : public agg::pixfmt_alpha_blend_rgba<Blender, RenBuf, PixelT>
{
public:
    //--------------------------------------------------------------------
    void blend_solid_hspan(int x, int y,
                           unsigned len, 
                           const color_type& c,
                           const agg::int8u*__restrict covers)
    {
        if (c.a == base_mask)
        {
            value_type* p = (value_type*)row_ptr(y) + (x << 2);
            agg::int32u color = *(agg::int32u*)&c;

            while (len > 3)
            {
                const agg::int8u* covers0 = covers;
                const agg::int8u* covers1 = covers + (len & 0xfffffffc);

                //count how many opaque pixels there are in a row
                do
                {
                    if (*(agg::int32u*)covers != 0xffffffff)
                        break;

                    covers += 4;
                }
                while (covers < covers1);
                    
                int count = (int)(covers - covers0);

                //if there are opaque pixels in a row, set them all at once
                if (count)
                {
                    agg::int32u* pdst = (agg::int32u*)p;

#if USE_SSE
                    if (count >= 64)
                    {  
                        set_row_sse(count, pdst, color);
                        pdst += count;
                    }
                    else
                    {
#endif
                        for (int i=0; i<count; i++)
                            *pdst++ = color;
#if USE_SSE
                    }
#endif
                    p = (agg::int8u*)pdst;
                    len -= count;
                }
                //otherwise blend four pixels and loop again
                else
                {
                    for (int i=0; i<4; i++)
                    {
                        calc_type alpha = *covers++;
                        if(alpha == 255) 
                            *(agg::int32*)p = color;
                        else 
                            blender_type::blend_pix(p, c.r, c.g, c.b, alpha);
                        p += 4;
                    }
                   
                    //blend4(color, (unsigned int*)p, covers);
                    //covers += 4;
                    //p += 16;
                    
                    len -= 4;
                }
            }

            //this repeats at most 3 times
            while (len) 
            {
                calc_type cover = *covers++;
                if(cover == 255)
                    *(agg::int32*)p = color;
                else
                    blender_type::blend_pix(p, c.r, c.g, c.b, cover, cover);

                p += 4;
                len--;
            }
            
        }
        else if (c.a)
        {
            value_type* p = (value_type*)row_ptr(y) + (x << 2);
            do 
            {
                calc_type alpha = (calc_type(c.a) * (calc_type(*covers) + 1)) >> 8;

                blender_type::blend_pix(p, c.r, c.g, c.b, alpha, *covers);

                p += 4;
                ++covers;
            }
            while(--len);
        }
    }
};


//MG specific AGG template instatiations
enum flip_y_e { flip_y = true };

//#define pix_format agg::pix_format_bgra32
typedef agg::row_ptr_cache<agg::int8u> mg_rendering_buffer;

typedef pixfmt_alpha_blend_rgba_mg<agg::blender_rgba32, mg_rendering_buffer, agg::pixel32_type> mg_pixfmt_type; //----pixfmt_rgba32
typedef pixfmt_alpha_blend_rgba_mg<agg::blender_rgba32_pre, mg_rendering_buffer, agg::pixel32_type> mg_pixfmt_type_pre; //----pixfmt_rgba32
typedef pixfmt_alpha_blend_rgba_mg<agg::blender_bgra32, mg_rendering_buffer, agg::pixel32_type> mg_pixfmt_type_argb; //----pixfmt_bgra32

typedef agg::renderer_base<mg_pixfmt_type> mg_ren_base;
typedef agg::renderer_base<mg_pixfmt_type_pre> mg_ren_base_pre;
typedef agg::renderer_outline_aa<mg_ren_base> mg_ren_base_o;
typedef agg::renderer_scanline_aa_solid<mg_ren_base> mg_ren_solid;

typedef agg::font_engine_freetype_int32 font_engine_type;
typedef agg::font_cache_manager<font_engine_type> font_manager_type;

// polyclip masking
typedef agg::amask_no_clip_gray8                                        mg_alpha_mask_type;
typedef agg::pixfmt_amask_adaptor<mg_pixfmt_type, mg_alpha_mask_type>   mg_pixfmt_clip_mask_type;
typedef agg::renderer_base<mg_pixfmt_clip_mask_type>                    mg_clip_mask_rb_type;
typedef agg::renderer_base<agg::pixfmt_gray8>                           mg_alpha_mask_rb_type;
typedef agg::renderer_scanline_aa_solid<mg_alpha_mask_rb_type>          mg_alpha_mask_ren_type;
typedef agg::renderer_scanline_aa_solid<mg_clip_mask_rb_type>           mg_clip_mask_ren_type;

struct RS_Font;

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
        feng.hinting(true);
        feng.transform(agg::trans_affine());
        last_font_height = 0;
        last_font = NULL;

        // polygon clip buffer
        bPolyClip = false;
        polyClip_buf = new unsigned char[width * height];
        polyClip_rbuf.attach((agg::int8u*)polyClip_buf,width,height,width);
        polyClip_mask.attach(polyClip_rbuf);
        polyClip_mask_pixf = new agg::pixfmt_gray8(polyClip_rbuf);
        polyClip_mask_rb.attach(*polyClip_mask_pixf);
        polyClip_mask_ren.attach(polyClip_mask_rb);
        polyClip_clip_pixf.attach(rb);
        polyClip_clip_mask = new mg_pixfmt_clip_mask_type(polyClip_clip_pixf, polyClip_mask);
        polyClip_clip_rb.attach(*polyClip_clip_mask);
        polyClip_clip_ren.attach(polyClip_clip_rb);
    }

    ~agg_context()
    {
        for (std::map<double, agg::line_profile_aa*>::iterator iter = h_lprof.begin();
            iter != h_lprof.end(); iter++)
            delete iter->second;

        if (ownrows)
            delete [] m_rows;

        delete polyClip_clip_mask;
        delete polyClip_mask_pixf;
        delete [] polyClip_buf;
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

    double last_font_height;
    const RS_Font* last_font;
    agg::trans_affine last_font_transform;

    // polyclip mask
    bool                        bPolyClip;
    unsigned char*              polyClip_buf;
    agg::rendering_buffer       polyClip_rbuf;
    mg_alpha_mask_type          polyClip_mask;
    mg_alpha_mask_rb_type       polyClip_mask_rb;
    mg_alpha_mask_ren_type      polyClip_mask_ren;
    agg::pixfmt_gray8*          polyClip_mask_pixf;
    mg_clip_mask_rb_type        polyClip_clip_rb;
    mg_clip_mask_ren_type       polyClip_clip_ren;
    mg_pixfmt_type              polyClip_clip_pixf;
    mg_pixfmt_clip_mask_type*   polyClip_clip_mask;
};

#endif
