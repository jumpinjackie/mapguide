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

#ifndef SE_SYMBOLDEFPROXIES_H_
#define SE_SYMBOLDEFPROXIES_H_

#include "SE_LineBuffer.h"
#include "SE_BufferPool.h"
#include "SE_ExpressionBase.h"
#include "SE_SymbolManager.h"

using namespace MDFMODEL_NAMESPACE;

class RS_FontEngine;
class SE_Renderer;
struct SE_RenderPrimitive;
struct SE_RenderStyle;


//////////////////////////////////////////////////////////////////////////////
class SE_EvalContext
{
public:
    RS_FontEngine* fonte;
    SE_Matrix* xform;
    FdoExpressionEngine* exec;
    SE_SymbolManager* resources;
    double mm2su;
    double mm2sud;      // number of screen units per mm device
    double mm2suw;      // number of screen units per mm world
    double px2su;       // number of screen units per pixel
    SE_BufferPool* pool;
};


//////////////////////////////////////////////////////////////////////////////
class SE_ApplyContext
{
public:
    LineBuffer* geometry;
    SE_Renderer* renderer;
    SE_Matrix* xform;
    MdfModel::SizeContext sizeContext;
};


//----------------------------------------------------------------------------
// SE_Primitives
//----------------------------------------------------------------------------

struct SE_Primitive
{
    SE_String resizeControl;
    bool cacheable;

    virtual ~SE_Primitive()
    {}

    virtual SE_RenderPrimitive* evaluate(SE_EvalContext*) = 0;
};

typedef std::vector<SE_Primitive*> SE_PrimitiveList;


//////////////////////////////////////////////////////////////////////////////
struct SE_Polyline : public SE_Primitive
{
    SE_LineBuffer* geometry;
    SE_Double weight;
    SE_Color color;
    SE_Boolean weightScalable;
    SE_String join;
    SE_String cap;
    SE_Double miterLimit;
    SE_Double scaleX;
    SE_Double scaleY;

    SE_INLINE SE_Polyline()
    {}

    ~SE_Polyline()
    {
        geometry->Free();
    }

    virtual SE_RenderPrimitive* evaluate(SE_EvalContext*);
};


//////////////////////////////////////////////////////////////////////////////
struct SE_Polygon : public SE_Polyline
{
    SE_Color fill;

    virtual SE_RenderPrimitive* evaluate(SE_EvalContext*);
};


//////////////////////////////////////////////////////////////////////////////
// Font/properties caching is left to the implementor of SE_Renderer
struct SE_Text : public SE_Primitive
{
    SE_String content;
    SE_String fontName;
    SE_Double position[2];
    SE_Double height;
    SE_Boolean heightScalable;
    SE_Double angleDeg; // degrees CCW
    SE_Boolean bold;
    SE_Boolean italic;
    SE_Boolean underlined;
    SE_Boolean overlined;
    SE_Double obliqueAngle;
    SE_Double trackSpacing;
    SE_Double lineSpacing;
    SE_String hAlignment;
    SE_String vAlignment;
    SE_String justification;
    SE_Color textColor;
    SE_Color ghostColor;
    SE_Color frameLineColor;
    SE_Color frameFillColor;
    SE_Double frameOffset[2];
    SE_String markup;

    virtual SE_RenderPrimitive* evaluate(SE_EvalContext*);
};


//////////////////////////////////////////////////////////////////////////////
struct SE_Raster : public SE_Primitive
{
    SE_String pngResourceId;
    SE_String pngResourceName;
    ImageData imageData;
    const wchar_t* resId;
    bool ownPtr;
    SE_Double position[2];
    SE_Double extent[2];
    SE_Boolean sizeScalable;
    SE_Double angleDeg; // degrees CCW
    double opacity;

    SE_INLINE SE_Raster() : ownPtr(false), opacity(1.0)
    {}

    ~SE_Raster()
    {
        if (ownPtr)
            delete[] imageData.data;
    }

    virtual SE_RenderPrimitive* evaluate(SE_EvalContext*);
};


//----------------------------------------------------------------------------
// SE_Styles
//----------------------------------------------------------------------------

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

    SE_INLINE SE_Style() : rstyle(NULL), cacheable(false)
    {}
    virtual ~SE_Style();
    virtual void evaluate(SE_EvalContext*) = 0;
    virtual void apply(SE_ApplyContext*) = 0;
    virtual void reset();
};


//////////////////////////////////////////////////////////////////////////////
struct SE_PointStyle : public SE_Style
{
    SE_String angleControl;
    SE_Double angleDeg; // degrees CCW
    SE_Double originOffset[2];

    SE_INLINE SE_PointStyle()
    {}
    virtual void evaluate(SE_EvalContext*);
    virtual void apply(SE_ApplyContext*);
};


//////////////////////////////////////////////////////////////////////////////
struct SE_LineStyle : public SE_Style
{
    SE_String angleControl;
    SE_String unitsControl;
    SE_String vertexControl;

    SE_Double angleDeg; // degrees CCW
    SE_Double startOffset;
    SE_Double endOffset;
    SE_Double repeat;
    SE_Double vertexAngleLimit; // degrees
    SE_String vertexJoin;
    SE_Double vertexMiterLimit;

    // default path
    SE_Double dpWeight;
    SE_Color dpColor;
    SE_Boolean dpWeightScalable;
    SE_String dpJoin;
    SE_String dpCap;
    SE_Double dpMiterLimit;

    SE_INLINE SE_LineStyle()
    {}
    virtual void evaluate(SE_EvalContext*);
    virtual void apply(SE_ApplyContext*);
};


//////////////////////////////////////////////////////////////////////////////
struct SE_AreaStyle : public SE_Style
{
    SE_String angleControl;
    SE_String originControl;
    SE_String clippingControl;

    SE_Double angleDeg; // degrees CCW
    SE_Double origin[2];
    SE_Double repeat[2];
    SE_Double bufferWidth;

    SE_INLINE SE_AreaStyle()
    {}
    virtual void evaluate(SE_EvalContext*);
    virtual void apply(SE_ApplyContext*);
};


//////////////////////////////////////////////////////////////////////////////
struct SE_SymbolInstance
{
    std::vector<SE_Style*> styles;

    SE_Double scale[2];
    SE_Double absOffset[2];
    MdfModel::SizeContext sizeContext;
    MdfModel::SymbolInstance::GeometryContext geomContext;
    SE_Boolean drawLast;
    SE_Boolean checkExclusionRegion;
    SE_Boolean addToExclusionRegion;
    SE_String positioningAlgorithm;
    SE_Integer renderPass;

    ~SE_SymbolInstance()
    {
        for (std::vector<SE_Style*>::iterator iter = styles.begin(); iter != styles.end(); ++iter)
            delete *iter;

        styles.clear();
    }
};


//////////////////////////////////////////////////////////////////////////////
struct SE_Rule
{
    std::vector<SE_SymbolInstance*> symbolInstances;
    RS_String legendLabel;  // no expressions on this guy?
    FdoFilter* filter;

    SE_Rule() : filter(NULL)
    {}

    ~SE_Rule()
    {
        if (filter)
            filter->Release();

        for (std::vector<SE_SymbolInstance*>::iterator iter = symbolInstances.begin(); iter != symbolInstances.end(); ++iter)
            delete *iter;

        symbolInstances.clear();
    }
};

#endif
