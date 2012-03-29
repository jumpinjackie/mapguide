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

#ifndef SE_RENDERPROXIES_H_
#define SE_RENDERPROXIES_H_

#include "SE_RendererStyles.h"
#include "SE_LineBuffer.h"
#include "SE_SymbolManager.h"
#include "RS_TextMetrics.h"


enum SE_RenderPrimitiveType
{
    SE_RenderPrimitive_Polyline,
    SE_RenderPrimitive_Polygon,
    SE_RenderPrimitive_Text,
    SE_RenderPrimitive_Raster
};


enum SE_RenderStyleType
{
    SE_RenderStyle_Point,
    SE_RenderStyle_Line,
    SE_RenderStyle_Area
};


enum SE_ResizeControlType
{
    SE_ResizeControl_ResizeNone,
    SE_ResizeControl_AddToResizeBox,
    SE_ResizeControl_AdjustToResizeBox
};


enum SE_AngleControlType
{
    SE_AngleControl_FromAngle,
    SE_AngleControl_FromGeometry
};


enum SE_UnitsControlType
{
    SE_UnitsControl_Absolute,
    SE_UnitsControl_Parametric
};


enum SE_VertexControlType
{
    SE_VertexControl_OverlapNone,
    SE_VertexControl_OverlapDirect,
    SE_VertexControl_OverlapWrap
};


enum SE_OriginControlType
{
    SE_OriginControl_Global,
    SE_OriginControl_Local,
    SE_OriginControl_Centroid
};


enum SE_ClippingControlType
{
    SE_ClippingControl_Clip,
    SE_ClippingControl_Inside,
    SE_ClippingControl_Overlap
};


//----------------------------------------------------------------------------
// SE_RenderPrimitives
//----------------------------------------------------------------------------

struct SE_RenderPrimitive
{
    SE_RenderPrimitiveType type;
    SE_ResizeControlType resizeControl;
    RS_F_Point bounds[4];
};

typedef std::vector<SE_RenderPrimitive*> SE_RenderPrimitiveList;


//////////////////////////////////////////////////////////////////////////////
struct SE_RenderPolyline : public SE_RenderPrimitive
{
    SE_INLINE SE_RenderPolyline()
    {
        type = SE_RenderPrimitive_Polyline;
    }
    ~SE_RenderPolyline()
    {
        if (geometry)
            geometry->Free();
    }

    SE_LineBuffer* geometry;
    SE_LineStroke lineStroke;
};


//////////////////////////////////////////////////////////////////////////////
struct SE_RenderPolygon : public SE_RenderPolyline
{
    SE_INLINE SE_RenderPolygon()
    {
        type = SE_RenderPrimitive_Polygon;
    }
    ~SE_RenderPolygon()
    {}

    unsigned int fill;
};


//////////////////////////////////////////////////////////////////////////////
struct SE_RenderText : public SE_RenderPrimitive
{
    SE_INLINE SE_RenderText()
    {
        type = SE_RenderPrimitive_Text;
    }

    std::wstring content;
    std::wstring expression;
    double position[2];
    RS_TextDef tdef;
    RS_TextMetrics tm;
};


//////////////////////////////////////////////////////////////////////////////
struct SE_RenderRaster : public SE_RenderPrimitive
{
    SE_INLINE SE_RenderRaster()
    {
        type = SE_RenderPrimitive_Raster;
    }

    ImageData imageData;
    double position[2];
    double extent[2];
    double angleRad; // radians CCW
    double opacity;
};


//----------------------------------------------------------------------------
// SE_RenderStyles
//----------------------------------------------------------------------------

struct SE_RenderStyle
{
    SE_INLINE SE_RenderStyle(SE_RenderStyleType stype)
        : type(stype),
          renderPass(0),
          drawLast(false),
          checkExclusionRegion(false),
          addToExclusionRegion(false)
    {
        bounds[0].x = bounds[3].x = +DBL_MAX;
        bounds[1].x = bounds[2].x = -DBL_MAX;
        bounds[0].y = bounds[1].y = +DBL_MAX;
        bounds[2].y = bounds[3].y = -DBL_MAX;
    }

    ~SE_RenderStyle()
    {
        for (SE_RenderPrimitiveList::iterator iter = symbol.begin(); iter != symbol.end(); ++iter)
        {
            // necessary since destructor of SE_RenderPrimitive is not virtual
            switch ((*iter)->type)
            {
                case SE_RenderPrimitive_Polyline:
                    delete (SE_RenderPolyline*)(*iter);
                    break;

                case SE_RenderPrimitive_Polygon:
                    delete (SE_RenderPolygon*)(*iter);
                    break;

                case SE_RenderPrimitive_Raster:
                    delete (SE_RenderRaster*)(*iter);
                    break;

                case SE_RenderPrimitive_Text:
                    delete (SE_RenderText*)(*iter);
                    break;

                default:
                    _ASSERT(false); // means there is a bug
                    break;
            }
        }
    }

    SE_RenderStyleType type;
    SE_RenderPrimitiveList symbol;
    RS_F_Point bounds[4];

    int renderPass;

    bool drawLast;
    bool checkExclusionRegion;
    bool addToExclusionRegion;
};


//////////////////////////////////////////////////////////////////////////////
struct SE_RenderPointStyle : public SE_RenderStyle
{
    SE_INLINE SE_RenderPointStyle() : SE_RenderStyle(SE_RenderStyle_Point)
    {}

    SE_AngleControlType angleControl;

    double angleRad; // radians CCW
    double offset[2];
};


//////////////////////////////////////////////////////////////////////////////
struct SE_RenderLineStyle : public SE_RenderStyle
{
    SE_INLINE SE_RenderLineStyle() : SE_RenderStyle(SE_RenderStyle_Line)
    {}

    SE_AngleControlType angleControl;
    SE_UnitsControlType unitsControl;
    SE_VertexControlType vertexControl;

    double angleRad; // radians CCW
    double startOffset;
    double endOffset;
    double repeat;
    double vertexAngleLimit; // radians
    SE_LineJoin vertexJoin;
    double vertexMiterLimit;

    // default path attributes
    SE_LineStroke dpLineStroke;

    bool solidLine;
};


//////////////////////////////////////////////////////////////////////////////
struct SE_RenderAreaStyle : public SE_RenderStyle
{
    SE_INLINE SE_RenderAreaStyle() : SE_RenderStyle(SE_RenderStyle_Area)
    {}

    SE_AngleControlType angleControl;
    SE_OriginControlType originControl;
    SE_ClippingControlType clippingControl;

    double angleRad; // radians CCW
    double origin[2];
    double repeat[2];
    double bufferWidth;

    bool solidFill;
};


//////////////////////////////////////////////////////////////////////////////
class SE_LabelInfo
{
public:
    SE_INLINE SE_LabelInfo()
        : x(0.0),
          y(0.0),
          anglerad(0.0),
          dunits(RS_Units_Device),
          style(NULL)
    {}

    SE_INLINE SE_LabelInfo(double _x, double _y, RS_Units _dunits, double _anglerad, SE_RenderStyle* _style)
        : x(_x),
          y(_y),
          anglerad(_anglerad),
          dunits(_dunits),
          style(_style)
    {}

    SE_INLINE void Set(double _x, double _y, RS_Units _dunits, double _anglerad, SE_RenderStyle* _style)
    {
        x = _x;
        y = _y;
        anglerad = _anglerad;
        dunits = _dunits;
        style = _style;
    }

    ~SE_LabelInfo()
    {
        delete style;
    }

    double x;
    double y;
    double anglerad; // radians CCW
    RS_Units dunits;
    SE_RenderStyle* style;
};

#endif
