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


#ifndef SE_SYMBOLDEFPROXIES_H
#define SE_SYMBOLDEFPROXIES_H

#include "SE_LineBuffer.h"

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//        SE_Primitives
//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
enum SE_PrimitiveType
{
    SE_PolylinePrimitive,
    SE_PolygonPrimitive,
    SE_TextPrimitive,
    SE_RasterPrimitive
};

enum SE_StyleType
{
    SE_PointStyleType,
    SE_LineStyleType,
    SE_AreaStyleType
};


struct SE_Primitive
{
    SE_PrimitiveType type;
    GraphicElement::ResizeControl resize;
    bool cacheable;
};

typedef std::vector<SE_Primitive*> SE_PrimitiveList;


struct SE_Polyline : public SE_Primitive
{
    SE_LineBuffer* geometry;
    SE_Double weight;
    SE_Color color;
    SE_Boolean weightScalable;

    SE_INLINE SE_Polyline() : weight(0.0) { type = SE_PolylinePrimitive; }
    ~SE_Polyline() { geometry->Free(); }
};

struct SE_Polygon : public SE_Polyline
{
    SE_Color fill;

    SE_INLINE SE_Polygon() { type = SE_PolygonPrimitive; weight = 0.0; } 
    ~SE_Polygon() { geometry->Free(); }
};

/* Font/properties caching is left to the implementor of SE_Renderer */
struct SE_Text : public SE_Primitive
{
    SE_String textExpr;
    SE_String fontExpr;
    SE_Double position[2];
    SE_Double size; // pt
    SE_Double angle;
    SE_Boolean underlined;
    SE_Boolean italic;
    SE_Boolean bold;
    SE_Double lineSpacing;
    SE_String hAlignment;
    SE_String vAlignment;
    SE_String justification;
    SE_Color textColor;
    SE_Color ghostColor;

    SE_INLINE SE_Text() { type = SE_TextPrimitive;  }
};

struct SE_Raster : public SE_Primitive
{
    SE_String pngPath;
    const unsigned char* pngPtr;
    int pngSize;
    SE_Double position[2];
    SE_Double extent[2];
    SE_Double angle;

    SE_INLINE SE_Raster() { type = SE_RasterPrimitive; }
};


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//        SE_Styles
//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

struct SE_Style
{
    SE_StyleType type;
    SE_PrimitiveList symbol;
    SE_Integer renderPass;

    bool useBox;
    SE_Double resizePosition[2];
    SE_Double resizeSize[2];
    ResizeBox::GrowControl resize;

    SE_INLINE SE_Style(SE_StyleType stype) : type(stype) { }

    ~SE_Style()
    {
        for (SE_PrimitiveList::iterator iter = symbol.begin(); iter != symbol.end(); iter++)
            delete *iter;
    }
};

struct SE_PointStyle : public SE_Style
{
    SE_INLINE SE_PointStyle() : SE_Style(SE_PointStyleType) { }

    SE_String angleControl;
    SE_Double angle;
    SE_Double originOffset[2];
};

struct SE_LineStyle : public SE_Style
{
    SE_INLINE SE_LineStyle() : SE_Style(SE_LineStyleType) { }

    SE_String angleControl;
    SE_String unitsControl;
    SE_String vertexControl;
//  SE_String join;

    SE_Double angle;
    SE_Double startOffset;
    SE_Double endOffset;
    SE_Double repeat;
    SE_Double vertexAngleLimit;
};

struct SE_AreaStyle : public SE_Style
{
    SE_INLINE SE_AreaStyle() : SE_Style(SE_AreaStyleType) { }

    SE_String angleControl;
    SE_String originControl;
    SE_String clippingControl;

    SE_Double angle;
    SE_Double origin[2];
    SE_Double repeat[2];
    SE_Double bufferWidth;
};


struct SE_Symbolization
{
    std::vector<SE_Style*> styles;

    SE_Double scale[2];
    SE_Double absOffset[2];
    MdfModel::SizeContext context;
    SE_Boolean drawLast;
    SE_Boolean checkExclusionRegions;
    SE_Boolean addToExclusionRegions;
    std::wstring positioningAlgorithm;

    ~SE_Symbolization()
    {
        for (std::vector<SE_Style*>::iterator iter = styles.begin(); iter != styles.end(); iter++)
            //TODO: SE_Styles have no virtual destructor but they don't seem to need
            //to free stuff specific to the derived classes
            delete *iter;

        styles.clear();
    }
};

struct SE_Rule
{
    std::vector<SE_Symbolization*> symbolization;
    RS_String legendLabel;  // no expressions on this guy
    FdoFilter* filter;
    
    ~SE_Rule()
    {
        if (filter) filter->Release();

        for (std::vector<SE_Symbolization*>::iterator iter = symbolization.begin(); iter != symbolization.end(); iter++)
            delete *iter;

        symbolization.clear();
    }
};


#endif
