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
#include "SE_ExpressionBase.h"

using namespace MDFMODEL_NAMESPACE;


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//        SE_Primitives
//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

class RS_FilterExecutor;
class RS_FontEngine;
class SE_SymbolManager;
class SE_Renderer;

class SE_EvalContext
{
public:
    RS_FontEngine* fonte;
    SE_Matrix* xform;
    RS_FilterExecutor* exec;
    SE_SymbolManager* resources;
    double mm2pxw;
    double mm2pxs;
    double mm2px;
    SE_LineBufferPool* pool;
    bool useBox;
    LineBuffer* geometry; //only used for SE_PointStyles -- get rid of it if possible
};

struct SE_RenderPrimitive;

struct SE_Primitive
{
    SE_String resizeControl;
    bool cacheable;

    virtual ~SE_Primitive() { }
    virtual SE_RenderPrimitive* evaluate(SE_EvalContext*) = 0;
};

typedef std::vector<SE_Primitive*> SE_PrimitiveList;


struct SE_Polyline : public SE_Primitive
{
    SE_LineBuffer* geometry;
    SE_Double weight;
    SE_Color color;
    SE_Boolean weightScalable;

    SE_INLINE SE_Polyline() : weight(0.0) { }
    ~SE_Polyline() { geometry->Free(); }
    virtual SE_RenderPrimitive* evaluate(SE_EvalContext*);
};


struct SE_Polygon : public SE_Polyline
{
    SE_Color fill;

    virtual SE_RenderPrimitive* evaluate(SE_EvalContext*);
};


// Font/properties caching is left to the implementor of SE_Renderer
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

    virtual SE_RenderPrimitive* evaluate(SE_EvalContext*);
};


struct SE_Raster : public SE_Primitive
{
    SE_String pngPath;
    const wchar_t* resId;
    const unsigned char* pngPtr;
    int pngSize;
    SE_Double position[2];
    SE_Double extent[2];
    SE_Double angle;

    SE_INLINE SE_Raster() {  }
    virtual SE_RenderPrimitive* evaluate(SE_EvalContext*);
};


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//        SE_Styles
//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

struct SE_RenderStyle;

struct SE_Style
{
    SE_RenderStyle* rstyle; // cached evaluated RenderStyle
    bool cacheable;
    SE_PrimitiveList symbol;
    SE_Integer renderPass;

    bool useBox;
    SE_Double resizePosition[2];
    SE_Double resizeSize[2];
    SE_String growControl;

    SE_INLINE SE_Style() : rstyle(NULL), cacheable(false) { }

    virtual ~SE_Style();

    virtual void apply(LineBuffer* geometry, SE_Renderer* renderer) = 0;
    virtual void evaluate(SE_EvalContext*) = 0;
};


struct SE_PointStyle : public SE_Style
{
    SE_String angleControl;
    SE_Double angle;
    SE_Double originOffset[2];

    SE_INLINE SE_PointStyle() { }
    virtual void apply(LineBuffer* geometry, SE_Renderer* renderer);
    virtual void evaluate(SE_EvalContext*);
};


struct SE_LineStyle : public SE_Style
{
    SE_String angleControl;
    SE_String unitsControl;
    SE_String vertexControl;

    SE_Double angle;
    SE_Double startOffset;
    SE_Double endOffset;
    SE_Double repeat;
    SE_Double vertexAngleLimit;
    SE_String vertexJoin;

    SE_INLINE SE_LineStyle() { }
    virtual void apply(LineBuffer* geometry, SE_Renderer* renderer);
    virtual void evaluate(SE_EvalContext*);
};


struct SE_AreaStyle : public SE_Style
{  
    SE_String angleControl;
    SE_String originControl;
    SE_String clippingControl;

    SE_Double angle;
    SE_Double origin[2];
    SE_Double repeat[2];
    SE_Double bufferWidth;

    SE_INLINE SE_AreaStyle() { }
    virtual void apply(LineBuffer* geometry, SE_Renderer* renderer);
    virtual void evaluate(SE_EvalContext*);
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
            delete *iter;

        styles.clear();
    }
};


struct SE_Rule
{
    std::vector<SE_Symbolization*> symbolization;
    RS_String legendLabel;  // no expressions on this guy?
    FdoFilter* filter;

    SE_Rule() : filter(NULL)  {  }

    ~SE_Rule()
    {
        if (filter) filter->Release();

        for (std::vector<SE_Symbolization*>::iterator iter = symbolization.begin(); iter != symbolization.end(); iter++)
            delete *iter;

        symbolization.clear();
    }
};

#endif
