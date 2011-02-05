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
#include "Renderer.h"
#include "GeometryAdapter.h"
#include "SymbolVisitor.h"
#include "SLDSymbols.h"
#include "ExpressionHelper.h"


extern void ProcessStylizerException(FdoException* exception, int line, wchar_t* file);

//////////////////////////////////////////////////////////////////////////////
GeometryAdapter::GeometryAdapter(LineBufferPool* lbp)
{
    m_exec = NULL;
    m_lbPool = lbp;
}


//////////////////////////////////////////////////////////////////////////////
GeometryAdapter::~GeometryAdapter()
{
    // free up cached fdo filters
    for (std::map<const void*, FdoFilter*>::iterator iter = m_hFilterCache.begin();
        iter != m_hFilterCache.end(); ++iter)
    {
        if (iter->second)
            iter->second->Release();
    }

    m_hFilterCache.clear();

    // free up cached fdo expressions
    for (std::map<const void*, FdoExpression*>::iterator iter = m_hExpressionCache.begin();
        iter != m_hExpressionCache.end(); ++iter)
    {
        if (iter->second)
            iter->second->Release();
    }

    m_hExpressionCache.clear();
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::EvalBoolean(const MdfModel::MdfString& exprstr, bool& res)
{
    // check for boolean constants first...
    const wchar_t* sb = exprstr.c_str();

    if (_wcsnicmp(sb, L"true", 5) == 0)
    {
        // value was constant, return true
        res = true;
        return true;
    }

    if (_wcsnicmp(sb, L"false", 6) == 0)
    {
        // value was constant, return true
        res = false;
        return true;
    }

    // darn, it must be an expression, so evaluate it
    if (!m_exec)
    {
        // hmmm... we can't eval as expression, what to do?
        _ASSERT(false);
        return false;
    }

    FdoExpression* expr = ObtainFdoExpression(&exprstr);

    // make sure we have a parsed expression
    if (!expr)
    {
        _ASSERT(false);
        return false;
    }

    // and then hope evaluation succeeds
    try
    {
        FdoPtr<FdoLiteralValue> lval = m_exec->Evaluate(expr);
        res = ExpressionHelper::GetAsBoolean(lval.p);
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }
    catch (...)
    {
    }

    // value was expression, so not cacheable
    return false;
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::EvalDouble(const MdfModel::MdfString& exprstr, double& res)
{
    // TODO: needs an expression processor argument to eval expressions

    const wchar_t* sd = exprstr.c_str();

    double d = 0.0;

    // We will look if there is any other stuff behind any numeric data in
    // order to detect if we are dealing with just a number or an expression.
    // For example "100 + stuff" would successfully parse a number, yet it
    // is not just a number.
    wchar_t* tmp = (wchar_t*)alloca((wcslen(sd)+1)*sizeof(wchar_t));
    *tmp = L'\0';
    int status = swscanf(sd, L"%lf%s", &d, tmp);

    if (status == 1 || (status && !wcslen(tmp)))
    {
        // value is constant
        res = d;
        return true;
    }

    // try to evaluate as expression if it was not constant
    if (!m_exec)
    {
        // hmmm... we can't eval as expression, what to do?
        _ASSERT(false);
        return false;
    }

    FdoExpression* expr = ObtainFdoExpression(&exprstr);

    // make sure we have a parsed expression
    if (!expr)
    {
        _ASSERT(false);
        return false;
    }

    // and then hope evaluation succeeds
    try
    {
        FdoPtr<FdoLiteralValue> lval = m_exec->Evaluate(expr);
        res = ExpressionHelper::GetAsDouble(lval.p);
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }
    catch (...)
    {
    }

    // if we are here, the value was not constant so it is not cacheable
    return false;
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::EvalString(const MdfModel::MdfString& exprstr, RS_String& res)
{
    if (!m_exec)
    {
        // no execution engine... spit the string back out
        _ASSERT(false);
        res = exprstr;
        return false;
    }

    // TODO: need a scheme to check if a string is a constant expression
    //       so that we can cache it instead of evaluating every time
    FdoExpression* expr = ObtainFdoExpression(&exprstr);

    if (!expr)
    {
        // this should not happen...
        res = exprstr;
        return false;
    }

    // try-catch the expression evaluation - I think we want
    // a silent failure here...
    try
    {
        FdoPtr<FdoLiteralValue> lval = m_exec->Evaluate(expr);
        const wchar_t* str = ExpressionHelper::GetAsString(lval.p);
        res = str? str : L"";
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }
    catch (...)
    {
    }

    // not cacheable
    return false;
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::EvalColor(const MdfModel::MdfString& exprstr, RS_Color& rscolor)
{
    // TODO: needs an expression processor argument to eval expressions

    // string is in the form "AARRGGBB"
    const wchar_t* scolor = exprstr.c_str();

    size_t len = wcslen(scolor);
    unsigned int color = 0;
    bool isConst = false;

    // try to check if the expression is constant
    int status = 0;
    if (len == 0)
    {
        // error or a color was not set
        // use transparent black which indicates "not set"
        rscolor = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
        return true;
    }
    else if (len == 8)
    {
        status = swscanf(scolor, L"%8X", &color);
    }
    else if (len == 6)
    {
        status = swscanf(scolor, L"%6X", &color);

        // there was no alpha specified in the constant string, add it
        color |= 0xFF000000;
    }

    if (status != 1)
    {
        // if not constant try to evaluate as expression
        if (!m_exec)
        {
            _ASSERT(false);
            rscolor = RS_Color(0x000000FF);
            return true;
        }

        FdoExpression* expr = ObtainFdoExpression(&exprstr);

        // make sure we have a parsed expression
        if (!expr)
        {
            _ASSERT(false);
            rscolor = RS_Color(0x000000FF);
            return false;
        }

        try
        {
            FdoPtr<FdoLiteralValue> lval = m_exec->Evaluate(expr);
            color = (unsigned int)ExpressionHelper::GetAsInt32(lval.p);
        }
        catch (FdoException* e)
        {
            ProcessStylizerException(e, __LINE__, __WFILE__);
            rscolor = RS_Color(0x000000FF);
            return false;
        }
        catch (...)
        {
            return false;
        }
    }
    else
    {
        isConst = true;
    }

    rscolor.alpha() =  color >> 24;
    rscolor.red()   = (color >> 16) & 0xFF;
    rscolor.green() = (color >>  8) & 0xFF;
    rscolor.blue()  =  color        & 0xFF;

    return isConst;
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::ConvertStroke(MdfModel::Stroke* stroke, RS_LineStroke& rsstroke)
{
    double val;

    if (stroke != NULL)
    {
        bool const1 = EvalDouble(stroke->GetThickness(), val);
        rsstroke.width() = MdfModel::LengthConverter::UnitToMeters(stroke->GetUnit(), val);
        rsstroke.style() = stroke->GetLineStyle();
        rsstroke.units() = (stroke->GetSizeContext() == MdfModel::MappingUnits)? RS_Units_Model : RS_Units_Device;
        bool const2 = EvalColor(stroke->GetColor(), rsstroke.color());

        // if all members are constant, the stroke is constant
        return const1 && const2;
    }
    else
    {
        // no stroke => set transparent outline
        rsstroke.color() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
        return true;
    }
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::ConvertStroke(MdfModel::LineSymbolization2D* lsym, RS_LineStroke& rsstroke)
{
    if (lsym == NULL)
        return false; // can also return true - this really is an error condition

    return ConvertStroke(lsym->GetStroke(), rsstroke);
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::ConvertFill(MdfModel::Fill* mdffill, RS_FillStyle& rsfill)
{
    bool const1 = true, const2 = true;

    if (mdffill != NULL)
    {
        const1 = EvalColor(mdffill->GetForegroundColor(), rsfill.color());
        const2 = EvalColor(mdffill->GetBackgroundColor(), rsfill.background());
        rsfill.pattern() = mdffill->GetFillPattern();
    }
    else
    {
        // no fill => set transparent fill color
        rsfill.color() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
        rsfill.background() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
    }

    return const1 && const2;
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::ConvertFill(MdfModel::AreaSymbolization2D* fill, RS_FillStyle& rsfill)
{
    if (fill == NULL)
        return false; // can also return true - this really is an error condition

    MdfModel::Fill* mdffill = fill->GetFill();

    bool const1 = ConvertFill(mdffill, rsfill);
    bool const2 = ConvertStroke(fill->GetEdge(), rsfill.outline());

    return const1 && const2;
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::ConvertTextHAlign(const MdfModel::MdfString& halign, RS_HAlignment& rshalign)
{
    // first check if the expression is a constant - in that case it can be cached
    if (halign == L"'Center'")
    {
        rshalign = RS_HAlignment_Center;
        return true;
    }
    else if (halign == L"'Left'")
    {
        rshalign = RS_HAlignment_Left;
        return true;
    }
    else if (halign == L"'Right'")
    {
        rshalign = RS_HAlignment_Right;
        return true;
    }
    else if (halign == L"'Path'")
    {
        rshalign = RS_HAlignment_Path;
        return true;
    }

    // Otherwise we need to evaluate as expression.  If it is an expression,
    // the value will come back without quotes.
    RS_String str;
    /*bool dummy =*/ EvalString(halign, str);

    if (str == L"Center")
    {
        rshalign = RS_HAlignment_Center;
    }
    else if (str == L"Left")
    {
        rshalign = RS_HAlignment_Left;
    }
    else if (str == L"Right")
    {
        rshalign = RS_HAlignment_Right;
    }
    else if (halign == L"Path")
    {
        rshalign = RS_HAlignment_Path;
    }

    // not cacheable
    return false;
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::ConvertTextVAlign(const MdfModel::MdfString& valign, RS_VAlignment& rsvalign)
{
    // first check if the expression is a constant - in that case it can be cached
    if (valign == L"'Bottom'")
    {
        rsvalign = RS_VAlignment_Descent;
        return true;
    }
    else if (valign == L"'Baseline'")
    {
        rsvalign = RS_VAlignment_Base;
        return true;
    }
    else if (valign == L"'Halfline'")
    {
        rsvalign = RS_VAlignment_Half;
        return true;
    }
    else if (valign == L"'Capline'")
    {
        rsvalign = RS_VAlignment_Cap;
        return true;
    }
    else if (valign == L"'Top'")
    {
        rsvalign = RS_VAlignment_Ascent;
        return true;
    }

    // Otherwise we need to evaluate as expression.  If it is an expression,
    // the value will come back without quotes.
    RS_String str;
    /*bool dummy =*/ EvalString(valign, str);

    if (str == L"Bottom")
    {
        rsvalign = RS_VAlignment_Descent;
    }
    else if (str == L"Baseline")
    {
        rsvalign = RS_VAlignment_Base;
    }
    else if (str == L"Halfline")
    {
        rsvalign = RS_VAlignment_Half;
    }
    else if (str == L"Capline")
    {
        rsvalign = RS_VAlignment_Cap;
    }
    else if (str == L"Top")
    {
        rsvalign = RS_VAlignment_Ascent;
    }

    // not cacheable
    return false;
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::ConvertSymbol(MdfModel::Symbol* symbol, RS_MarkerDef& mdef)
{
    SymbolVisitor::eSymbolType type = SymbolVisitor::DetermineSymbolType(symbol);

    mdef.units() = (symbol->GetSizeContext() == MdfModel::MappingUnits)? RS_Units_Model : RS_Units_Device;

    double val;
    bool cacheable = true;

    // width
    cacheable = EvalDouble(symbol->GetSizeX(), val) && cacheable;
    mdef.width() = MdfModel::LengthConverter::UnitToMeters(symbol->GetUnit(), val);
    if (mdef.width() < 0.0)
    {
        // should not get negative width and height
        _ASSERT(false);
        mdef.width() = -mdef.width();
    }

    // height
    cacheable = EvalDouble(symbol->GetSizeY(), val) && cacheable;
    mdef.height() = MdfModel::LengthConverter::UnitToMeters(symbol->GetUnit(), val);
    if (mdef.height() < 0.0)
    {
        // should not get negative width and height
        _ASSERT(false);
        mdef.height() = -mdef.height();
    }

    // rotation + insertion point
    cacheable = EvalDouble(symbol->GetRotation(), mdef.rotation()) && cacheable;
    cacheable = EvalDouble(symbol->GetInsertionPointX(), mdef.insx()) && cacheable;
    cacheable = EvalDouble(symbol->GetInsertionPointY(), mdef.insy()) && cacheable;
    mdef.rotation() = fmod(mdef.rotation(), 360.0);

    if (type == SymbolVisitor::stMark)
    {
        MdfModel::MarkSymbol* marksym = (MdfModel::MarkSymbol*)symbol;

        mdef.type() = RS_MarkerType_Marker;

        // shape
        MdfModel::MarkSymbol::Shape shape = marksym->GetShape();
        switch (shape)
        {
            case MdfModel::MarkSymbol::Square:   mdef.markernum() = SLDType_Square;   break;
            case MdfModel::MarkSymbol::Circle:   mdef.markernum() = SLDType_Circle;   break;
            case MdfModel::MarkSymbol::Triangle: mdef.markernum() = SLDType_Triangle; break;
            case MdfModel::MarkSymbol::Star:     mdef.markernum() = SLDType_Star;     break;
            case MdfModel::MarkSymbol::Cross:    mdef.markernum() = SLDType_Cross;    break;
            case MdfModel::MarkSymbol::X:        mdef.markernum() = SLDType_X;        break;
            default: break;
        }

        // fill and edge colors
        cacheable = ConvertFill(marksym->GetFill(), mdef.style()) && cacheable;
        cacheable = ConvertStroke(marksym->GetEdge(), mdef.style().outline()) && cacheable;
    }
    else if (type == SymbolVisitor::stBlock)
    {
        MdfModel::BlockSymbol* blocksym = (MdfModel::BlockSymbol*)symbol;

        mdef.type()    = RS_MarkerType_Block;
        mdef.library() = blocksym->GetDrawingName();
        mdef.name()    = blocksym->GetBlockName();

        // block and layer override colors
        cacheable = EvalColor(blocksym->GetBlockColor(), mdef.style().color()) && cacheable;
        cacheable = EvalColor(blocksym->GetLayerColor(), mdef.style().outline().color()) && cacheable;
    }
    else if (type == SymbolVisitor::stW2D)
    {
        MdfModel::W2DSymbol* w2dsym = (MdfModel::W2DSymbol*)symbol;

        mdef.type()    = RS_MarkerType_W2D;
        mdef.library() = w2dsym->GetSymbolLibrary();
        mdef.name()    = w2dsym->GetSymbolName();

        // fill, line, and text override colors
        cacheable = EvalColor(w2dsym->GetFillColor(), mdef.style().color()) && cacheable;
        cacheable = EvalColor(w2dsym->GetLineColor(), mdef.style().outline().color()) && cacheable;
        cacheable = EvalColor(w2dsym->GetTextColor(), mdef.style().background()) && cacheable;
    }
    else if (type == SymbolVisitor::stFont)
    {
        MdfModel::FontSymbol* fontSym = (MdfModel::FontSymbol*)symbol;

        mdef.type() = RS_MarkerType_Font;

        // store the font name as the library string
        mdef.library() = fontSym->GetFontName();

        // store the marker character as the symbol name
        mdef.name() = (wchar_t)fontSym->GetCharacter();

        // font style
        RS_FontStyle_Mask style = RS_FontStyle_Regular;

        bool setting = false;
        cacheable = EvalBoolean(fontSym->GetBold(), setting) && cacheable;
        if (setting)
            style = (RS_FontStyle_Mask)(style | RS_FontStyle_Bold);

        cacheable = EvalBoolean(fontSym->GetItalic(), setting) && cacheable;
        if (setting)
            style = (RS_FontStyle_Mask)(style | RS_FontStyle_Italic);

        cacheable = EvalBoolean(fontSym->GetUnderlined(), setting) && cacheable;
        if (setting)
            style = (RS_FontStyle_Mask)(style | RS_FontStyle_Underline);

        mdef.fontstyle() = style;

        // foreground color
        cacheable = EvalColor(fontSym->GetForegroundColor(), mdef.style().color()) && cacheable;
    }
    else if (type == SymbolVisitor::stImage)
    {
        // TODO: not currently supported
        _ASSERT(false);
    }

    return cacheable;
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::ConvertTextDef(MdfModel::TextSymbol* text, RS_TextDef& tdef)
{
    // foreground color
    bool cacheable = EvalColor(text->GetForegroundColor(), tdef.textcolor());

    // background style and color
    switch (text->GetBackgroundStyle())
    {
        case MdfModel::TextSymbol::Transparent :
            tdef.textbg() = RS_TextBackground_None;
            break;
        case MdfModel::TextSymbol::Ghosted :
            tdef.textbg() = RS_TextBackground_Ghosted;
            cacheable = EvalColor(text->GetBackgroundColor(), tdef.ghostcolor()) && cacheable;
            break;
        case MdfModel::TextSymbol::Opaque :
            tdef.textbg() = RS_TextBackground_Opaque;
            cacheable = EvalColor(text->GetBackgroundColor(), tdef.opaquecolor()) && cacheable;
            break;
    }

    // font style
    RS_FontStyle_Mask style = RS_FontStyle_Regular;

    bool setting = false;
    cacheable = EvalBoolean(text->GetBold(), setting) && cacheable;
    if (setting)
        style = (RS_FontStyle_Mask)(style | RS_FontStyle_Bold);

    cacheable = EvalBoolean(text->GetItalic(), setting) && cacheable;
    if (setting)
        style = (RS_FontStyle_Mask)(style | RS_FontStyle_Italic);

    cacheable = EvalBoolean(text->GetUnderlined(), setting) && cacheable;
    if (setting)
        style = (RS_FontStyle_Mask)(style | RS_FontStyle_Underline);

    tdef.font().style() = style;

    // font name
    tdef.font().name() = text->GetFontName();

    // font height
    double val;
    cacheable = EvalDouble(text->GetSizeY(), val) && cacheable;
    tdef.font().height() = MdfModel::LengthConverter::UnitToMeters(text->GetUnit(), val);

    // rotation
    cacheable = EvalDouble(text->GetRotation(), tdef.rotation()) && cacheable;
    tdef.rotation() = fmod(tdef.rotation(), 360.0);

    // units
    tdef.font().units() = (text->GetSizeContext() == MdfModel::MappingUnits)? RS_Units_Model : RS_Units_Device;

    // alignment
    cacheable = ConvertTextHAlign(text->GetHorizontalAlignment(), tdef.halign()) && cacheable;
    cacheable = ConvertTextVAlign(text->GetVerticalAlignment(), tdef.valign()) && cacheable;

    return cacheable;
}


//////////////////////////////////////////////////////////////////////////////
void GeometryAdapter::Stylize(Renderer*                   /*renderer*/,
                              RS_FeatureReader*           /*features*/,
                              bool                        /*initialPass*/,
                              FdoExpressionEngine*        /*exec*/,
                              LineBuffer*                 /*lb*/,
                              MdfModel::FeatureTypeStyle* /*style*/,
                              const MdfModel::MdfString*  /*tooltip*/,
                              const MdfModel::MdfString*  /*url*/,
                              RS_ElevationSettings*       /*elevSettings*/,
                              CSysTransformer*            /*layer2mapxformer*/)
{
}


//////////////////////////////////////////////////////////////////////////////
void GeometryAdapter::AddLabel(double x, double y,
                               double slope_rad, bool useSlope,
                               MdfModel::Label* label,
                               RS_OverpostType type, bool exclude,
                               Renderer* renderer,
                               LineBuffer* lb)
{
    MdfModel::TextSymbol* text = label->GetSymbol();

    RS_String txt;
    EvalString(text->GetText(), txt);

    if (!txt.empty())
    {
        RS_TextDef def;
        ConvertTextDef(text, def);

        if (useSlope)
            def.rotation() = slope_rad * M_180PI;

        RS_LabelInfo info(x, y, 0.0, 0.0, RS_Units_Model, def);
        renderer->ProcessLabelGroup(&info, 1, txt, type, exclude, lb, text->GetScaleLimit());
    }
}


//////////////////////////////////////////////////////////////////////////////
// Parses and caches an FDO filter from a pointer to an MDF string.
// Once cached, the filter will be reused next time the same expression
// is asked for.  Also executes the filter and returns failure or success.
bool GeometryAdapter::ExecFdoFilter(const MdfModel::MdfString* pExprstr)
{
    // TODO: we use pointers to the MDF strings for caching -
    //       this may be risky but all caching in the stylizer is
    //       based on the MDF pointer staying the same throughout the
    //       stylization process

    // empty expression - no filter
    // pass trivially
    if (pExprstr->empty())
        return true;

    // get from cache
    FdoFilter* filter = m_hFilterCache[pExprstr];

    // if in cache, return existing
    // NOTE: do not addref, it is not needed
    if (!filter)
    {
        try
        {
            // otherwise parse and cache it
            filter = FdoFilter::Parse(pExprstr->c_str());
            m_hFilterCache[pExprstr] = filter;
        }
        catch (FdoException* e)
        {
            ProcessStylizerException(e, __LINE__, __WFILE__);
            filter = NULL;
        }
    }

    // no filter means pass in this case
    if (!filter)
        return true;

    // m_exec should be set in the Stylize call
    // of the inheriting geometry adapter
    _ASSERT(m_exec);

    bool res = false;
    try
    {
        res = m_exec->ProcessFilter(filter);
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }

    return res;
}


//////////////////////////////////////////////////////////////////////////////
// parses and caches an FDO filter from a pointer to an MDF string.
// Once cached, the filter will be reused next time the same expression
// is asked for.
FdoExpression* GeometryAdapter::ObtainFdoExpression(const MdfModel::MdfString* pExprstr)
{
    // TODO: we use pointers to the MDF strings for caching -
    //       this may be risky but all caching in the stylizer is
    //       based on the MDF pointer staying the same throughout the
    //       stylization process

    // empty expression - no filter
    if (pExprstr->empty())
        return NULL;

    // get from cache
    FdoExpression* expr = m_hExpressionCache[pExprstr];

    // if in cache, return existing
    // NOTE: do not addref, it is not needed
    if (expr)
        return expr;

    try
    {
        // otherwise parse and cache it
        expr = FdoExpression::Parse(pExprstr->c_str());
        m_hExpressionCache[pExprstr] = expr;
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
        expr = NULL;
    }

    return expr;
}


//////////////////////////////////////////////////////////////////////////////
bool GeometryAdapter::GetElevationParams(RS_ElevationSettings* elevSettings,
                                         double& zOffset, double& zExtrusion,
                                         RS_ElevationType& elevType)
{
    // elevation settings
    elevType = RS_ElevationType_RelativeToGround;
    zOffset = 0.0;
    zExtrusion = 0.0;
    if (elevSettings != NULL)
    {
        double metersPerUnit = elevSettings->metersPerUnit();

        elevType = elevSettings->elevType();

        RS_String& zOffsetExpression = elevSettings->zOffsetExpression();
        if (!zOffsetExpression.empty())
        {
            EvalDouble(zOffsetExpression, zOffset);
            zOffset *= metersPerUnit;
        }

        RS_String& zExtrusionExpression = elevSettings->zExtrusionExpression();
        if (!zExtrusionExpression.empty())
        {
            EvalDouble(zExtrusionExpression, zExtrusion);
            zExtrusion *= metersPerUnit;
        }
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
// This method returns the amount that the clip region needs to be expanded
// to ensure that the clipped geometry renders correctly.  For example, if
// we clip a polygon directly to the edge of the screen then the polygon's
// edge style will be visible along any clipped edges.  But we don't want
// any style to display on these clipped edges since they are not part of
// the actual feature geometry.  To fix this we expand the clip region so
// that all clipped edges are sufficiently offscreen and any style applied
// to them does not display in the map region.  This method computes the
// required clip offset for the supplied line stroke.  The value returned
// is in device units, in meters.
double GeometryAdapter::GetClipOffset(RS_LineStroke& lineStroke, double mapScale)
{
    // get the width in device units, in meters
    double width = fabs(lineStroke.width());
    if (lineStroke.units() == RS_Units_Model)
        width /= mapScale;

    const double ptsToMeters = METERS_PER_INCH / 72.0;

    // Account for any decoration in the style.  A decoration has a size
    // (the full width) specified in points.  When rendered, this size is
    // first increased by 1 and then increased by the line weight.  See
    // LineStyleDef::SetStyleDef for details.
    double decorSize = 0.0;
    const wchar_t* lineStyleName = lineStroke.style().c_str();
    if (wcscmp(lineStyleName, L"Solid") != 0)
    {
        if (wcscmp(lineStyleName, L"Rail") == 0)
        {
            // the decoration size is 5 points
            decorSize += 6.0 * ptsToMeters;
            decorSize += width;
        }
        else if (wcscmp(lineStyleName, L"FENCELINE1") == 0)
        {
            // the decoration size is 9 points
            decorSize += 10.0 * ptsToMeters;
            decorSize += width;
        }
        else if (wcscmp(lineStyleName, L"FENCELINE2") == 0)
        {
            // the decoration size is 9 points
            decorSize += 10.0 * ptsToMeters;
            decorSize += width;
        }
        else if (wcscmp(lineStyleName, L"TRACKS") == 0)
        {
            // the decoration size is 25 points
            decorSize += 26.0 * ptsToMeters;
            decorSize += width;
        }
    }

    // return the half-width
    return 0.5*(width + decorSize);
}
