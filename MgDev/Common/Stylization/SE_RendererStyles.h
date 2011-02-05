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

#ifndef SE_RENDERERSTYLES_H_
#define SE_RENDERERSTYLES_H_

enum SE_LineCap
{
    SE_LineCap_None,
    SE_LineCap_Round,
    SE_LineCap_Triangle,
    SE_LineCap_Square
};

enum SE_LineJoin
{
    SE_LineJoin_None,
    SE_LineJoin_Bevel,
    SE_LineJoin_Round,
    SE_LineJoin_Miter
};

// attributes used when rendering polylines
struct SE_LineStroke
{
    SE_LineStroke() :
        color(0),
        weight(0.0),
        cap(SE_LineCap_Round),
        join(SE_LineJoin_Round),
        miterLimit(5.0)
    {}

    SE_LineStroke(unsigned int _color, double _weight) :
        color(_color),
        weight(_weight),
        cap(SE_LineCap_Round),
        join(SE_LineJoin_Round),
        miterLimit(5.0)
    {}

    SE_LineStroke(unsigned int _color, double _weight, SE_LineCap _cap, SE_LineJoin _join, double _miterLimit) :
        color(_color),
        weight(_weight),
        cap(_cap),
        join(_join),
        miterLimit(_miterLimit)
    {}

    unsigned int color;
    double       weight;
    SE_LineCap   cap;
    SE_LineJoin  join;
    double       miterLimit;
};

#endif
