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

#include "stdafx.h"
#include "GDUtils.h"
#include <memory.h>

int ConvertColor(gdImagePtr i, RS_Color& c)
{
    return gdImageColorAllocateAlpha(i, c.red(), c.green(), c.blue(), 127 - c.alpha()/2 );
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

        float aalpha = hlen - solidwid;
        RS_Color ac = color;
        ac.alpha() = (int)(ac.alpha() * aalpha);
        int gdc2 = ConvertColor(im, ac);

        gdImageSetPixel(im, x-solidwid-1, k, gdc2);
        gdImageSetPixel(im, x+solidwid, k, gdc2);
    }
}


gdImagePtr rs_gdImageThickLineBrush(int line_weight, RS_Color& color)
{
    if (line_weight % 2 == 1)
        line_weight += 1;

    int sx = line_weight;
    int sy = line_weight;

    gdImagePtr brush = gdImageCreateTrueColor(sx, sy);
    int transparent = gdImageColorAllocateAlpha(brush, 0, 0, 0, 127);

    gdImageAlphaBlending(brush, 0);
    gdImageFilledRectangle(brush, 0, 0, sx, sy, transparent);

    //compute fractional alpha value for antialiasing effect
    //each pixel should be hit by line_weight / 2 number of circles
    //so the alpha is computed as 255 / line_weight * 2
    RS_Color falpha = color;
    falpha.alpha() = falpha.alpha() / line_weight * 2 + 1;
    falpha.alpha() = (falpha.alpha() < 255)? falpha.alpha() : 255;

    //outer transparent circle -- for antialiased effect
    rs_gdImageCircleForBrush(brush, sx/2, sy/2, line_weight / 2,  falpha);

    gdImageAlphaBlending(brush, 1);

    //inner non-transparent circle
    rs_gdImageCircleForBrush(brush, sx/2, sy/2, (line_weight - 2) / 2, color);

    return brush;
}
