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

#ifndef SE_INCLUDE_H
#define SE_INCLUDE_H

#include <math.h>
#include <map>

#include "FilterExecutor.h"
#include "SE_Matrix.h"
#include "SE_LineBuffer.h"

using namespace MDFMODEL_NAMESPACE;

#define SE_INLINE inline

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

struct SE_Color
{
    unsigned char b, g, r, a; // argb, but little endian
    FdoExpression* expression;

    SE_INLINE SE_Color() : expression(NULL) { *((unsigned int*)this) = 0; }
    ~SE_Color() { if (expression) expression->Release(); }

    // Retrieve argb color
    SE_INLINE unsigned int evaluate(RS_FilterExecutor* processor)
    {
        if (expression)
        {
            // we need to try-catch the expression evaluation
            try
            {
                expression->Process(processor);
                *((unsigned int*)this) = (unsigned int)processor->GetInt64Result();
            }
            catch (FdoException* e)
            {
                e->Release();
                processor->Reset();

                // set a default
                *((unsigned int*)this) = 0;
            }
        }
        
        return *((unsigned int*)this);
    }

    SE_INLINE void operator=(unsigned int val) { *((unsigned int*)this) = val; }
    SE_INLINE bool empty() { return (*(unsigned int*)this) == 0 && expression == NULL; }
    SE_INLINE unsigned int& argb() { return (*(unsigned int*)this); }
};

struct SE_Double
{
    double value;
    FdoExpression* expression;

    SE_INLINE SE_Double() : expression(NULL),value(0.0)  {  }
    SE_INLINE SE_Double(double d) : value(d), expression(NULL) { }
    ~SE_Double() { if (expression) expression->Release(); }

    SE_INLINE double evaluate(RS_FilterExecutor* processor)
    {
        if (expression)
        {
            // we need to try-catch the expression evaluation
            try
            {
                expression->Process(processor);
                value = processor->GetDoubleResult();
            }
            catch (FdoException* e)
            {
                e->Release();
                processor->Reset();

                // set a default
                value = 0.0;
            }
        }

        return value;
    }

    SE_INLINE void operator=(double d)  { value = d; }
};

struct SE_Integer
{
    int value;
    FdoExpression* expression;

    SE_INLINE SE_Integer() : expression(NULL), value(0) { }
    SE_INLINE SE_Integer(int i) : value(i), expression(NULL) { }
    ~SE_Integer() { if (expression) expression->Release(); }

    SE_INLINE int evaluate(RS_FilterExecutor* processor)
    {
        if (expression)
        {
            // we need to try-catch the expression evaluation
            try
            {
                expression->Process(processor);
                value = (int)processor->GetInt64Result();
            }
            catch (FdoException* e)
            {
                e->Release();
                processor->Reset();

                // set a default
                value = 0;
            }
        }

        return value;
    }

    SE_INLINE void SE_Integer::operator=(int i)  { value = i; }
};

struct SE_Boolean
{
    bool value;
    FdoExpression* expression;

    SE_INLINE SE_Boolean() : expression(NULL), value(false) { }
    SE_INLINE SE_Boolean(bool b) : value(b), expression(NULL) { }
    ~SE_Boolean() { if (expression) expression->Release(); }

    SE_INLINE bool evaluate(RS_FilterExecutor* processor)
    {
        if (expression)
        {
            // we need to try-catch the expression evaluation
            try
            {
                expression->Process(processor);
                value = (bool)processor->GetBooleanResult();
            }
            catch (FdoException* e)
            {
                e->Release();
                processor->Reset();

                // set a default
                value = false;
            }
        }

        return value;
    }

    SE_INLINE void operator=(bool b) { value = b; }
};

struct SE_String
{
    const wchar_t* value;
    FdoExpression* expression;

    SE_INLINE SE_String() : expression(NULL), value(NULL) { }
    SE_INLINE SE_String(const wchar_t* s) : value(s), expression(NULL) { }
    ~SE_String() 
    {
        if (value)
            delete[] value;
        if (expression) 
            expression->Release(); 
    }

    SE_INLINE const wchar_t* evaluate(RS_FilterExecutor* processor)
    {
        if (expression)
        {
            if (value)
            {
                delete[] value;
                value = NULL;
            }

            wchar_t* newValue = NULL;

            // we need to try-catch the expression evaluation
            try
            {
                expression->Process(processor);
                newValue = processor->GetStringResult();
            }
            catch (FdoException* e)
            {
                e->Release();
                processor->Reset();

                // just return the stored expression
                FdoString* exprValue = expression->ToString();
                size_t len = wcslen(exprValue) + 1;
                newValue = new wchar_t[len];
                wcscpy(newValue, exprValue);
            }

            value = newValue;
        }

        return value;
    }

    SE_INLINE void operator=(const wchar_t* s) { value = s; }
};

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//        SE_Primitives
//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

struct SE_Primitive
{
    SE_PrimitiveType type;
    GraphicElement::ResizeControl resize;
    bool cacheable;
};

struct SE_Polyline : public SE_Primitive
{
    SE_LineBuffer* geometry;
    SE_Double weight;
    SE_Color color;

    SE_INLINE SE_Polyline() : weight(0.0) { type = SE_PolylinePrimitive; }
};

struct SE_Polygon : public SE_Polyline
{
    SE_Color fill;

    SE_INLINE SE_Polygon() { type = SE_PolygonPrimitive; weight = 0.0; } 
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
    unsigned char* pngPtr;
    int pngSize;
    SE_Double position[2];
    SE_Double extent[2];
    SE_Double angle;

    SE_INLINE SE_Raster() { type = SE_RasterPrimitive; }
};


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//        SE_RenderPrimitives
//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

struct SE_RenderPrimitive
{
    SE_PrimitiveType type;
    bool resize;
    SE_Bounds* bounds;
};

struct SE_RenderPolyline : public SE_RenderPrimitive
{
    SE_LineBuffer* geometry;
    double weight;
    unsigned int color;

    SE_INLINE SE_RenderPolyline() { type = SE_PolylinePrimitive; }
};

struct SE_RenderPolygon : public SE_RenderPolyline
{
    unsigned int fill;

    SE_INLINE SE_RenderPolygon() { type = SE_PolygonPrimitive; }
};

struct SE_RenderText : public SE_RenderPrimitive
{
    std::wstring text;
    double position[2];

    RS_TextDef tdef;

    SE_INLINE SE_RenderText() { type = SE_TextPrimitive; }
};

/* Caching, if any, is left to the implementor of SE_Renderer */
struct SE_RenderRaster : public SE_RenderPrimitive
{
    unsigned char* pngPtr;
    int pngSize;
    double position[2];
    double extent[2];
    double angle;

    SE_INLINE SE_RenderRaster() : pngPtr(0), pngSize(0) { type = SE_RasterPrimitive; }
};

typedef std::vector<SE_Primitive*> SE_Symbol;
typedef std::vector<SE_RenderPrimitive*> SE_RenderSymbol;

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//        SE_Styles
//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

struct SE_Style
{
    SE_StyleType type;
    SE_Symbol symbol;
    SE_Integer renderPass;

    bool useBox;
    SE_Double resizePosition[2];
    SE_Double resizeSize[2];
    ResizeBox::GrowControl resize;

    SE_INLINE SE_Style(SE_StyleType stype) : type(stype) { }

    ~SE_Style()
    {
        for (SE_Symbol::iterator iter = symbol.begin(); iter != symbol.end(); iter++)
            delete *iter;
    }
};

struct SE_PointStyle : public SE_Style
{
    SE_INLINE SE_PointStyle() : SE_Style(SE_PointStyleType) { }

    SE_String orientation;
    SE_Double angle;
    SE_Double offset[2];
};

struct SE_LineStyle : public SE_Style
{
    SE_INLINE SE_LineStyle() : SE_Style(SE_LineStyleType) { }

    SE_String orientation;
    SE_String units;
    SE_String overlap;
    SE_String join;

    SE_Double startOffset;
    SE_Double endOffset;
    SE_Double repeat;
    SE_Double angleLimit;
    SE_Double angle;
};

struct SE_AreaStyle : public SE_Style
{
    SE_INLINE SE_AreaStyle() : SE_Style(SE_AreaStyleType) { }

    SE_String orientation;
    SE_String origincontrol;
    SE_String clipping;

    SE_Double origin[2];
    SE_Double repeat[2];
    SE_Double angle;
    SE_Double bufferWidth;
};

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//
//        SE_RenderStyles
//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

struct SE_RenderStyle
{
    SE_INLINE SE_RenderStyle(SE_StyleType stype) 
        : type(stype),
          drawLast(false),
          checkExclusionRegions(false),
          addToExclusionRegions(false),
          bounds(NULL)
    { }

    ~SE_RenderStyle()
    {
        for (SE_RenderSymbol::iterator iter = symbol.begin(); iter != symbol.end(); iter++)
            delete *iter;
    }

    SE_StyleType type;
    SE_RenderSymbol symbol;
    SE_Bounds* bounds;

    int renderPass;

    bool drawLast;
    bool checkExclusionRegions;
    bool addToExclusionRegions;
};

struct SE_RenderPointStyle : public SE_RenderStyle
{
    SE_INLINE SE_RenderPointStyle() : SE_RenderStyle(SE_PointStyleType) { }
};

struct SE_RenderLineStyle : public SE_RenderStyle
{
    SE_INLINE SE_RenderLineStyle() : SE_RenderStyle(SE_LineStyleType) { }

    const wchar_t* orientation;
    const wchar_t* units;
    const wchar_t* overlap;
    const wchar_t* join;

    double startOffset;
    double endOffset;
    double repeat;
    double angleLimit;
    double angle;
};

struct SE_RenderAreaStyle : public SE_RenderStyle
{
    SE_INLINE SE_RenderAreaStyle() : SE_RenderStyle(SE_AreaStyleType) { }

    const wchar_t* origincontrol;
    const wchar_t* orientation;
    const wchar_t* clipping;

    unsigned int background;
    double origin[2];
    double repeat[2];
    double angle;
    double radius;
};


class SE_LabelInfo
{
public:
    SE_RenderStyle* symbol;
    double x;
    double y;
    double dx;
    double dy;
    double anglerad;
    RS_Units dunits;    
    
    SE_INLINE SE_LabelInfo() :
        x(0.0),
        y(0.0),
        dx(0.0),
        dy(0.0),
        anglerad(0.0),
        symbol(NULL)
    { }

    SE_INLINE SE_LabelInfo (double _x, double _y, double _dx, double _dy, RS_Units _dunits, double _anglerad, SE_RenderStyle* _symbol)
                 : x(_x), y(_y), dx(_dx), dy(_dy), dunits(_dunits), anglerad(_anglerad), symbol(_symbol)
    { }
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
    FdoFilter* filter;
    
    ~SE_Rule()
    {
        if (filter) filter->Release();

        for (std::vector<SE_Symbolization*>::iterator iter = symbolization.begin(); iter != symbolization.end(); iter++)
            delete *iter;

        symbolization.clear();
    }
};


#endif // SE_INCLUDE_H
