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

#ifdef MG_USE_SSE

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
loop1:  movdqa xmmword ptr [edi], xmm0;
        movdqa xmmword ptr 16[edi], xmm0;
        movdqa xmmword ptr 32[edi], xmm0;
        movdqa xmmword ptr 48[edi], xmm0;
        movdqa xmmword ptr 64[edi], xmm0;
        movdqa xmmword ptr 80[edi], xmm0;
        movdqa xmmword ptr 96[edi], xmm0;
        movdqa xmmword ptr 112[edi], xmm0;
        add edi, 128
        sub ecx, 32
        jg loop1

        ;finish setting remaining <32 pixels using a non-SSE loop
        cld
        add ecx, 32
        rep stosd
    }
}

#endif


template<class Blender, class RenBuf, class PixelT = agg::int32u>
class pixfmt_alpha_blend_rgba_mg : public agg::pixfmt_alpha_blend_rgba<Blender, RenBuf, PixelT>
{
public:
    typedef RenBuf rbuf_type;
    typedef Blender blender_type;
    typedef typename blender_type::order_type order_type;
    typedef typename blender_type::color_type color_type;
    typedef typename color_type::value_type value_type;
    typedef typename color_type::calc_type calc_type;

    enum base_scale_e
    {
        base_mask = color_type::base_mask
    };

    explicit pixfmt_alpha_blend_rgba_mg()
        : agg::pixfmt_alpha_blend_rgba<Blender, RenBuf, PixelT>()
    {
    }

    explicit pixfmt_alpha_blend_rgba_mg(rbuf_type& rb)
        : agg::pixfmt_alpha_blend_rgba<Blender, RenBuf, PixelT>(rb)
    {
    }

    //--------------------------------------------------------------------
    void blend_solid_hspan(int x, int y,
                           unsigned len,
                           const color_type& c,
                           const agg::int8u* covers)
    {
        if (c.a == base_mask)
        {
            value_type* p = (value_type*)agg::pixfmt_alpha_blend_rgba<Blender, RenBuf, PixelT>::row_ptr(y) + (x << 2);
            agg::int32u color;// = *(agg::int32u*)&c;
            ((agg::int8u*)&color)[order_type::R] = c.r;
            ((agg::int8u*)&color)[order_type::G] = c.g;
            ((agg::int8u*)&color)[order_type::B] = c.b;
            ((agg::int8u*)&color)[order_type::A] = c.a;

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
                   /*
                    blend4(color, (unsigned int*)p, covers);
                    covers += 4;
                    p += 16;
                   */
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
            value_type* p = (value_type*)agg::pixfmt_alpha_blend_rgba<Blender, RenBuf, PixelT>::row_ptr(y) + (x << 2);
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

//============================================================blender_gray_invert
template<class ColorT> struct blender_gray_invert
{
    typedef ColorT color_type;
    typedef typename color_type::value_type value_type;
    typedef typename color_type::calc_type calc_type;
    enum base_scale_e {
        base_shift = color_type::base_shift,
    };

    static AGG_INLINE void blend_pix(value_type* p, unsigned cv,
                                     unsigned alpha, unsigned cover=0)
    {
        cv;
        alpha;
        cover;
        *p = (value_type)(color_type::base_mask - *p);
    }
};
