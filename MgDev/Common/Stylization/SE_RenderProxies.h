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

#ifndef SE_RENDERPROXIES_H
#define SE_RENDERPROXIES_H

#include "SE_Bounds.h"
#include "SE_LineBuffer.h"


enum SE_RenderPrimitiveType
{
    SE_RenderPolylinePrimitive,
    SE_RenderPolygonPrimitive,
    SE_RenderTextPrimitive,
    SE_RenderRasterPrimitive
};


enum SE_RenderStyleType
{
    SE_RenderPointStyleType,
    SE_RenderLineStyleType,
    SE_RenderAreaStyleType
};


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//        SE_RenderPrimitives
//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


struct SE_RenderPrimitive
{
    SE_RenderPrimitiveType type;
    bool resize;
    RS_F_Point bounds[4];
};


struct SE_RenderPolyline : public SE_RenderPrimitive
{
    SE_INLINE SE_RenderPolyline() { type = SE_RenderPolylinePrimitive; }
    ~SE_RenderPolyline() { if (geometry) geometry->Free(); }

    SE_LineBuffer* geometry;
    double weight;
    unsigned int color;
};


struct SE_RenderPolygon : public SE_RenderPolyline
{
    SE_INLINE SE_RenderPolygon() { type = SE_RenderPolygonPrimitive; }
    ~SE_RenderPolygon() { }

    unsigned int fill;
};


struct SE_RenderText : public SE_RenderPrimitive
{
    SE_INLINE SE_RenderText() { type = SE_RenderTextPrimitive; }

    std::wstring text;
    double position[2];
    RS_TextDef tdef;
};


// Caching, if any, is left to the implementor of SE_Renderer
struct SE_RenderRaster : public SE_RenderPrimitive
{
    SE_INLINE SE_RenderRaster() : pngPtr(0), pngSize(0) { type = SE_RenderRasterPrimitive; }

    const unsigned char* pngPtr;
    int pngSize;
    double position[2];
    double extent[2];
    double angle;
};


typedef std::vector<SE_RenderPrimitive*> SE_RenderPrimitiveList;


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//        SE_RenderStyles
//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


struct SE_RenderStyle
{
    SE_INLINE SE_RenderStyle(SE_RenderStyleType stype)
        : type(stype),
          drawLast(false),
          checkExclusionRegions(false),
          addToExclusionRegions(false)
    {
        bounds[0].x = bounds[3].x = +DBL_MAX;
        bounds[1].x = bounds[2].x = -DBL_MAX;
        bounds[0].y = bounds[1].y = +DBL_MAX;
        bounds[2].y = bounds[3].y = -DBL_MAX;
    }

    ~SE_RenderStyle()
    {
        for (SE_RenderPrimitiveList::iterator iter = symbol.begin(); iter != symbol.end(); iter++)
        {
            //necessary since destructor of SE_RenderPrimitive is not virtual
            //we may want to figure out a better scheme to manage render primitives
            switch ((*iter)->type)
            {
            case SE_RenderPolylinePrimitive: delete (SE_RenderPolyline*)(*iter);break;
            case SE_RenderPolygonPrimitive: delete (SE_RenderPolygon*)(*iter);break;
            case SE_RenderRasterPrimitive: delete (SE_RenderRaster*)(*iter);break;
            case SE_RenderTextPrimitive: delete (SE_RenderText*)(*iter);break;
            default: throw; //means there is a bug
            }
        }
    }

    SE_RenderStyleType type;
    SE_RenderPrimitiveList symbol;
    RS_F_Point bounds[4];

    int renderPass;

    bool drawLast;
    bool checkExclusionRegions;
    bool addToExclusionRegions;
};


struct SE_RenderPointStyle : public SE_RenderStyle
{
    SE_INLINE SE_RenderPointStyle() : SE_RenderStyle(SE_RenderPointStyleType) { }

    const wchar_t* angleControl;

    double angle;
    double offset[2];
};


struct SE_RenderLineStyle : public SE_RenderStyle
{
    SE_INLINE SE_RenderLineStyle() : SE_RenderStyle(SE_RenderLineStyleType) { }

    const wchar_t* angleControl;
    const wchar_t* unitsControl;
    const wchar_t* vertexControl;

    double angle;
    double startOffset;
    double endOffset;
    double repeat;
    double vertexAngleLimit;
    const wchar_t* vertexJoin;
};


struct SE_RenderAreaStyle : public SE_RenderStyle
{
    SE_INLINE SE_RenderAreaStyle() : SE_RenderStyle(SE_RenderAreaStyleType) { }

    const wchar_t* angleControl;
    const wchar_t* originControl;
    const wchar_t* clippingControl;

    double angle;
    double origin[2];
    double repeat[2];
    double bufferWidth;
};


class SE_LabelInfo
{
public:
    SE_INLINE SE_LabelInfo()
        : x(0.0), y(0.0), dx(0.0), dy(0.0), dunits(RS_Units_Device), anglerad(0.0), symbol(NULL)
    { }

    SE_INLINE SE_LabelInfo (double _x, double _y, double _dx, double _dy, RS_Units _dunits, double _anglerad, SE_RenderStyle* _symbol)
        : x(_x), y(_y), dx(_dx), dy(_dy), dunits(_dunits), anglerad(_anglerad), symbol(_symbol)
    { }

    SE_RenderStyle* symbol;
    double x;
    double y;
    double dx;
    double dy;
    double anglerad;
    RS_Units dunits;
};

#endif
