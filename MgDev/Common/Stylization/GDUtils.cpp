//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include <memory>
#include <math.h>
#include "stdafx.h"
#include "GDUtils.h"


int ConvertColor(gdImagePtr i, RS_Color& c)
{
    return gdImageColorAllocateAlpha(i, c.red(), c.green(), c.blue(), 127 - c.alpha()/2 );
}

//this is adapted from gdImageFilledPolygon to draw a polygon with holes
//contours point counts are given by the cntrs array
//TODO: this code is written as if it belongs to gd.c, except that
//gdMalloc and gdRealloc are replaced by malloc/realloc -- which should
//not matter in our case since gd.dll is compiled to use the same CRT
//as Stylization.dll
void rs_gdImageMultiPolygon(gdImagePtr im, int* cntrs, int nCntrs, gdPointPtr p, int nPts, int c)
{
    int i;
    int j;
    int index;
    int y;
    int miny, maxy;
    int x1, y1;
    int x2, y2;
    int ind1, ind2;
    int ints;
    int fill_color;
    int offset;

    if (!nPts)
    {
        return;
    }

    if (!im->polyAllocated)
    {
        //if (overflow2(sizeof (int), nPts)) {
        //    return;
        //}
        im->polyInts = (int *) malloc (sizeof (int) * nPts);
        im->polyAllocated = nPts;
    }
    if (im->polyAllocated < nPts)
    {
        while (im->polyAllocated < nPts)
        {
            im->polyAllocated *= 2;
        }
        im->polyInts = (int *) realloc (im->polyInts,
            sizeof (int) * im->polyAllocated);
    }
    miny = p[0].y;
    maxy = p[0].y;
    for (i = 1; (i < nPts); i++)
    {
        if (p[i].y < miny)
        {
            miny = p[i].y;
        }
        if (p[i].y > maxy)
        {
            maxy = p[i].y;
        }
    }
    /* 2.0.16: Optimization by Ilia Chipitsine -- don't waste time offscreen */
    /* 2.0.26: clipping rectangle is even better */
    if (miny < im->cy1)
    {
        miny = im->cy1;
    }
    if (maxy > im->cy2)
    {
        maxy = im->cy2;
    }
    /* Fix in 1.3: count a vertex only once */
    for (y = miny; (y <= maxy); y++)
    {
        /*1.4           int interLast = 0; */
        /*              int dirLast = 0; */
        /*              int interFirst = 1; */
        /* 2.0.26+      int yshift = 0; */
        if (c == gdAntiAliased) {
            fill_color = im->AA_color;
        } else {
            fill_color = c;
        }

        ints = 0;
        offset = 0;

        /*go over each contour and treat it as a polygon of its own*/
        /*the sort of x intercepts later on will automatically take care of the even-odd fill*/
        for (j = 0; j < nCntrs; j++)
        {
            for (i = 0; (i < cntrs[j]); i++)
            {
                if (!i)
                {
                    ind1 = cntrs[j] - 1 + offset;
                    ind2 = 0            + offset;
                }
                else
                {
                    ind1 = i - 1 + offset;
                    ind2 = i     + offset;
                }
                y1 = p[ind1].y;
                y2 = p[ind2].y;
                if (y1 < y2)
                {
                    x1 = p[ind1].x;
                    x2 = p[ind2].x;
                }
                else if (y1 > y2)
                {
                    y2 = p[ind1].y;
                    y1 = p[ind2].y;
                    x2 = p[ind1].x;
                    x1 = p[ind2].x;
                }
                else
                {
                    continue;
                }

                /* Do the following math as float intermediately, and round to ensure
                * that Polygon and FilledPolygon for the same set of points have the
                * same footprint. */

                if ((y >= y1) && (y < y2))
                {
                    im->polyInts[ints++] = (int) ((float) ((y - y1) * (x2 - x1)) /
                        (float) (y2 - y1) + 0.5 + x1);
                }
//    WCW - don't know why the last scanline is treated differently than the
//          others - it definitely causes problems with tiles; commenting
//          out this line until we know why this was added
//              else if ((y == maxy) && (y > y1) && (y <= y2))
//              {
//                  im->polyInts[ints++] = (int) ((float) ((y - y1) * (x2 - x1)) /
//                      (float) (y2 - y1) + 0.5 + x1);
//              }
            }

            offset += cntrs[j];
        }
        /*
        2.0.26: polygons pretty much always have less than 100 points,
        and most of the time they have considerably less. For such trivial
        cases, insertion sort is a good choice. Also a good choice for
        future implementations that may wish to indirect through a table.
        */
        for (i = 1; (i < ints); i++) {
            index = im->polyInts[i];
            j = i;
            while ((j > 0) && (im->polyInts[j - 1] > index)) {
                im->polyInts[j] = im->polyInts[j - 1];
                j--;
            }
            im->polyInts[j] = index;
        }
        for (i = 0; (i < (ints)); i += 2)
        {
#if 0
            int minx = im->polyInts[i];
            int maxx = im->polyInts[i + 1];
#endif
            /* 2.0.29: back to gdImageLine to prevent segfaults when
            performing a pattern fill */
            gdImageLine (im, im->polyInts[i], y, im->polyInts[i + 1], y,
                fill_color);
        }
    }
    /* If we are drawing this AA, then redraw the border with AA lines. */
    /* This doesn't work as well as I'd like, but it doesn't clash either. */
    if (c == gdAntiAliased) {

        offset = 0;
        for (i = 0; (i < nCntrs); i++)
        {
            gdImagePolygon (im, p + offset, cntrs[i], c);
            offset += cntrs[i];
        }
    }
}

//axis aligned circle for setting up brushes for thick lines
void rs_gdImageCircleForBrush(gdImagePtr im, int x, int y, int rad, RS_Color& color)
{
    int gdc = ConvertColor(im, color);
    float rad2 = (float)rad*rad;

    for (int j = -rad, k = y+j; j <= rad; j++, k++)
    {
        float j_offset = j + 0.5f;

        float hlen = sqrt(rad2 - j_offset*j_offset);

        int solidwid = (int)hlen;

        if (solidwid)
            gdImageLine(im, x-solidwid, k, x+solidwid-1, k, gdc);

        float aalpha = hlen - floor(hlen);
        RS_Color ac = color;
        ac.alpha() = (int)(ac.alpha() * aalpha);
        int gdc2 = ConvertColor(im, ac);

        gdImageSetPixel(im, x-solidwid-1, k, gdc2);
        gdImageSetPixel(im, x+solidwid, k, gdc2);
    }
}


gdImagePtr rs_gdImageThickLineBrush(int line_weight, RS_Color& color)
{
    gdImagePtr brush1 = NULL;

    if (line_weight % 2 == 1)
        line_weight += 1;

    int sx = line_weight;
    int sy = line_weight;

    brush1 = gdImageCreateTrueColor(sx, sy);
    int transparent = gdImageColorAllocateAlpha(brush1, 0, 0, 0, 127);

    gdImageAlphaBlending(brush1, 0);
    gdImageFilledRectangle(brush1, 0, 0, sx, sy, transparent);

    //compute fractional alpha value for antialiasing effect
    //each pixel should be hit by line_weight / 2 number of circles
    //so the alpha is computed as 255 / line_weight * 2
    RS_Color falpha = color;
    falpha.alpha() = falpha.alpha() / line_weight * 2 + 1;
    falpha.alpha() = rs_min(255, falpha.alpha());

    //outer transparent circle -- for antialiased effect
    rs_gdImageCircleForBrush(brush1, sx/2, sy/2, line_weight / 2,  falpha);

    gdImageAlphaBlending(brush1, 1);

    //inner non-transparent circle
    rs_gdImageCircleForBrush(brush1, sx/2, sy/2, (line_weight - 2) / 2, color);

    /////////////////////////////////////////
    //test
    /*
    FILE* out = fopen("c:\\dude.png", "wb");

    int size = 0;
    char* data = NULL;

    data = (char*)gdImagePngPtr((gdImagePtr)brush1, &size);

    if (data)
    {
        if (fwrite(data, 1, size, out) != (size_t)size)
        {
            // Error
            _ASSERT(false);
        }
    }

    fclose(out);
    */
    //////////////////////////////////////////

    return brush1;
}
