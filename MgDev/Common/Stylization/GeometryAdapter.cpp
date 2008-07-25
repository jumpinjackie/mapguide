//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "FilterExecutor.h"
#include "SymbolVisitor.h"
#include "SLDSymbols.h"

extern void ProcessStylizerException(FdoException* exception, int line, wchar_t* file);

GeometryAdapter::GeometryAdapter(LineBufferPool* lbp)
{
    m_exec = NULL;
    m_lbPool = lbp;
}


GeometryAdapter::~GeometryAdapter()
{
    //free up cached fdo filters
    for (std::map<const void*, FdoFilter*>::iterator iter = m_hFilterCache.begin();
        iter != m_hFilterCache.end(); iter++)
    {
        if (iter->second)
            iter->second->Release();
    }

    m_hFilterCache.clear();

    //free up cached fdo expressions
    for (std::map<const void*, FdoExpression*>::iterator iter = m_hExpressionCache.begin();
        iter != m_hExpressionCache.end(); iter++)
    {
        if (iter->second)
            iter->second->Release();
    }

    m_hExpressionCache.clear();
}


bool GeometryAdapter::EvalDouble(const MdfModel::MdfString& exprstr, double& res)
{
    //TODO: needs an expression processor argument to eval expressions

    const wchar_t* sd = exprstr.c_str();

    double d = 0.0;

    //we will look if there is any other stuff behind any numeric data
    //in order to detect if we are dealing with just a number or an expression
    //for example "100 + stuff" would successfully parse a number, yet it is not
    //just a number.
    wchar_t* tmp = (wchar_t*)alloca((wcslen(sd)+1)*sizeof(wchar_t));
    *tmp = L'\0';
    int status = swscanf(sd, L"%lf%s", &d, tmp);

    if (status == 1 || (status && !wcslen(tmp)))
    {
        res = d;
        return true; //value is constant
    }

    //try to evaluate as expression if it was not constant

    if (!m_exec)
    {
        //TODO:
        //hmm... we can't eval as expression, what to do?
        _ASSERT(false);
        return 0.0;
    }

    FdoExpression* expr = ObtainFdoExpression(&exprstr);

    if (!expr)
    {
        _ASSERT(false);
        return false;
    }

    try
    {
        expr->Process(m_exec);
        res = m_exec->GetDoubleResult();
    }
    catch (FdoException* e)
    {
        _ASSERT(false);
        ProcessStylizerException(e, __LINE__, __WFILE__);
        m_exec->Reset();
    }

    //if we are here, the value was not constant so it is not cacheable
    return false;
}


bool GeometryAdapter::EvalBoolean(const MdfModel::MdfString& exprstr, bool & res)
{
    //check for boolean constants first...
    const wchar_t* sb = exprstr.c_str();

    if (_wcsnicmp(sb, L"true", 5) == 0)
    {
        res = true;
        return true; //value was constant, return true
    }

    if (_wcsnicmp(sb, L"false", 6) == 0)
    {
        res = false;
        return true; //value was constant, return true
    }

    //darn, it must be an expression, so evaluate it
    if (!m_exec)
    {
        //hmm... we can't eval as expression, what to do?
        _ASSERT(false);
        return false;
    }

    FdoExpression* expr = ObtainFdoExpression(&exprstr);

    //make sure we have a parsed expression
    if (!expr)
    {
        _ASSERT(false);
        return false;
    }

    //and then hope evaluation succeeds
    try
    {
        expr->Process(m_exec);
        res = m_exec->GetBooleanResult();
    }
    catch (FdoException* e)
    {
        _ASSERT(false);
        ProcessStylizerException(e, __LINE__, __WFILE__);
        m_exec->Reset();
    }

    return false; //value was expression, so not cacheable
}


bool GeometryAdapter::EvalString(const MdfModel::MdfString& exprstr, RS_String& res)
{
    if (!m_exec)
    {
        //no execution engine...
        //spit the string back out
        _ASSERT(false);
        res = exprstr;
        return false;
    }

    //TODO: need a scheme to check if a string is a constant expression
    //so that we can cache it instead of evaluating every time
    FdoExpression* expr = ObtainFdoExpression(&exprstr);

    if (!expr)
    {
        //this should not happen...
        res = exprstr;
        return false;
    }

    wchar_t* rettmp = NULL;

    //try-catch the expression evaluation -- I think we want
    //a silent failure here...
    try
    {
        expr->Process(m_exec);
        rettmp = m_exec->GetStringResult();
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
        m_exec->Reset();
    }

    if (rettmp)
    {
        res = rettmp;
        delete [] rettmp;
    }
    else
        res = L"";

    return false; //not cacheable
}


bool GeometryAdapter::EvalColor(const MdfModel::MdfString& exprstr, RS_Color& rscolor)
{
    //TODO: needs an expression processor argument to eval expressions

    const wchar_t* scolor = exprstr.c_str();

    size_t len = wcslen(scolor);
    //string is in the form "AARRGGBB"

    unsigned int color = 0;

    bool isConst = false;

    //try to check if the expression is constant
    int status = 0;
    if (len == 0)
    {
        //error or a color was not set
        //use transparent black which indicates "not set"
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

        //there was no alpha specified in the constant string, add it
        color |= 0xFF000000;
    }

    if (status != 1)
    {
        //if not constant try to evaluate as expression
        if (!m_exec)
        {
            _ASSERT(false);
            rscolor = RS_Color(0x000000FF);
            return true;
        }

        FdoExpression* expr = ObtainFdoExpression(&exprstr);

        if (!expr)
        {
            _ASSERT(false);
            rscolor = RS_Color(0x000000FF);
            return false;
        }

        try
        {
            expr->Process(m_exec);
            color = (unsigned int)m_exec->GetInt64Result();
        }
        catch (FdoException* e)
        {
            _ASSERT(false);
            ProcessStylizerException(e, __LINE__, __WFILE__);
            m_exec->Reset();
            rscolor = RS_Color(0x000000FF);
            return false;
        }
    }
    else
    {
        isConst = true;
    }

    rscolor.alpha() =  color >> 24;
    rscolor.red() = (color >> 16) & 0xFF;
    rscolor.green() = (color >> 8) & 0xFF;
    rscolor.blue() =  color & 0xFF;

    return isConst;
}


bool GeometryAdapter::ConvertLineThickness(const MdfModel::MdfString& expr, double& thickness)
{
    return EvalDouble(expr, thickness);
}


bool GeometryAdapter::ConvertStroke(MdfModel::Stroke* stroke, RS_LineStroke& rsstroke)
{
    double val;

    if (stroke != NULL)
    {
        bool const1 = ConvertLineThickness(stroke->GetThickness(), val);
        rsstroke.width()= MdfModel::LengthConverter::UnitToMeters(stroke->GetUnit(), val);
        rsstroke.style() = stroke->GetLineStyle();
        rsstroke.units() = (stroke->GetSizeContext() == MdfModel::MappingUnits)? RS_Units_Model : RS_Units_Device;
        bool const2 = EvalColor(stroke->GetColor(), rsstroke.color());

        return const1 && const2; //if all members are constant, the stroke is constant
    }
    else
    {
        //no stroke -> set transparent outline
        rsstroke.color() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
        return true;
    }
}


bool GeometryAdapter::ConvertStroke(MdfModel::LineSymbolization2D* lsym, RS_LineStroke& rsstroke)
{
    if (lsym == NULL)
        return false; //can also return true -- this really is an error condition

    return ConvertStroke(lsym->GetStroke(), rsstroke);
}


bool GeometryAdapter::ConvertFill(MdfModel::AreaSymbolization2D* fill, RS_FillStyle& rsfill)
{
    if (fill == NULL)
        return false;//can also return true -- this really is an error condition

    MdfModel::Fill* mdffill = fill->GetFill();

    bool const1 = ConvertFill(mdffill, rsfill);
    bool const2 = ConvertStroke(fill->GetEdge(), rsfill.outline());

    return const1 && const2;
}


bool GeometryAdapter::ConvertFill(MdfModel::Fill* mdffill, RS_FillStyle&   rsfill)
{
    bool const1 = false, const2 = false;

    if (mdffill != NULL)
    {
        const1 = EvalColor(mdffill->GetForegroundColor(), rsfill.color());
        const2 = EvalColor(mdffill->GetBackgroundColor(), rsfill.background());
        rsfill.pattern() = mdffill->GetFillPattern();
    }
    else
    {
        //no fill -> set transparent fill color
        rsfill.color() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
        rsfill.background() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
        const1 = const2 = true;
    }

    return const1 && const2;
}


bool GeometryAdapter::ConvertTextHAlign(const MdfModel::MdfString& halign, RS_HAlignment& rshalign)
{

    //first check if the expression is a constant.
    //In that case it can be cached
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

    //otherwise we need to evaluate as expression
    //if it expression, the value will come back without quotes
    RS_String str = L"";

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


bool GeometryAdapter::ConvertTextVAlign(const MdfModel::MdfString& valign, RS_VAlignment& rsvalign)
{
    //first check if the expression is a constant.
    //In that case it can be cached
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

    //otherwise we need to evaluate as expression
    //if it expression, the value will come back without quotes
    RS_String str = L"";

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


bool GeometryAdapter::ConvertMarkerDef(MdfModel::Symbol* marker, RS_MarkerDef& mdef)
{
    SymbolVisitor::eSymbolType type = SymbolVisitor::DetermineSymbolType(marker);

    mdef.units() = (marker->GetSizeContext() == MdfModel::MappingUnits)
        ? RS_Units_Model : RS_Units_Device;

    double val;
    bool const1 = EvalDouble(marker->GetSizeX(), val);
    mdef.width() = MdfModel::LengthConverter::UnitToMeters(marker->GetUnit(), val);

    if (mdef.width() < 0.0)
    {
        _ASSERT(false); //should not get negative width and height
        mdef.width() = - mdef.width();
    }

    const1 = EvalDouble(marker->GetSizeY(), val) && const1;
    mdef.height() = MdfModel::LengthConverter::UnitToMeters(marker->GetUnit(), val);
    const1 = EvalDouble(marker->GetRotation(), mdef.rotation()) && const1;

    if (mdef.height() < 0.0)
    {
        _ASSERT(false); //should not get negative width and height
        mdef.height() = - mdef.height();
    }

    const1 = EvalDouble(marker->GetInsertionPointX(), mdef.insx()) && const1;
    const1 = EvalDouble(marker->GetInsertionPointY(), mdef.insy()) && const1;

    if (type == SymbolVisitor::stW2D)
    {
        MdfModel::W2DSymbol* w2dsym = (MdfModel::W2DSymbol*)marker;

        mdef.name() = w2dsym->GetSymbolName(); //!!! this sets a reference to the string
        mdef.library() = w2dsym->GetSymbolLibrary(); //!!! this sets a reference to the string

        //override colors
        const1 = EvalColor(w2dsym->GetAreaColor(), mdef.style().color()) && const1;
        const1 = EvalColor(w2dsym->GetLineColor(), mdef.style().outline().color()) && const1;

        //HACK: well, a little bit of a hack -- store the text override color
        //int the bg color of the fill style, which is unused in this case anyway
        const1 = EvalColor(w2dsym->GetTextColor(), mdef.style().background()) && const1;

    }
    else if (type == SymbolVisitor::stMark)
    {
        MdfModel::MarkSymbol* marksym = (MdfModel::MarkSymbol*)marker;

        MdfModel::MarkSymbol::Shape shape = marksym->GetShape();

        switch (shape)
        {
            case MdfModel::MarkSymbol::Square :   mdef.name() = SLD_SQUARE_NAME; break;
            case MdfModel::MarkSymbol::Circle :   mdef.name() = SLD_CIRCLE_NAME; break;
            case MdfModel::MarkSymbol::Cross :    mdef.name() = SLD_CROSS_NAME;  break;
            case MdfModel::MarkSymbol::Star :     mdef.name() = SLD_STAR_NAME;   break;
            case MdfModel::MarkSymbol::Triangle : mdef.name() = SLD_TRIANGLE_NAME;break;
            case MdfModel::MarkSymbol::X :        mdef.name() = SLD_X_NAME; break;
            default: break;
        }

        const1 = ConvertFill(marksym->GetFill(), mdef.style()) && const1;
        const1 = ConvertStroke(marksym->GetEdge(), mdef.style().outline()) && const1;
    }
    else if (type == SymbolVisitor::stFont)
    {
        MdfModel::FontSymbol* fontSym = (MdfModel::FontSymbol*)marker;

        //store the font name as the library string
        mdef.library() = fontSym->GetFontName();

        //store the marker character as the symbol name
        mdef.name() = (wchar_t)fontSym->GetCharacter();

        RS_FontStyle_Mask style = RS_FontStyle_Regular;

        bool setting = false;
        const1 = EvalBoolean(fontSym->GetBold(), setting) && const1;
        if (setting)
            style = (RS_FontStyle_Mask)(style | RS_FontStyle_Bold);

        const1 = EvalBoolean(fontSym->GetItalic(), setting) && const1;
        if (setting)
            style = (RS_FontStyle_Mask)(style | RS_FontStyle_Italic);

        const1 = EvalBoolean(fontSym->GetUnderlined(), setting) && const1;
        if (setting)
            style = (RS_FontStyle_Mask)(style | RS_FontStyle_Underline);

        mdef.fontstyle() = style;

        //color
        const1 = EvalColor(fontSym->GetForegroundColor(), mdef.style().color()) && const1;
    }

    return const1;
}


bool GeometryAdapter::ConvertTextDef(MdfModel::TextSymbol* text, RS_TextDef& tdef)
{
    bool const1 = EvalColor(text->GetForegroundColor(), tdef.textcolor());

    bool const2 = true;
    switch (text->GetBackgroundStyle())
    {
        case MdfModel::TextSymbol::Transparent :
            tdef.textbg() = RS_TextBackground_None;
            break;
        case MdfModel::TextSymbol::Ghosted :
            tdef.textbg() = RS_TextBackground_Ghosted;
            const2 = EvalColor(text->GetBackgroundColor(), tdef.ghostcolor());
            break;
        case MdfModel::TextSymbol::Opaque :
            tdef.textbg() = RS_TextBackground_Opaque;
            const2 = EvalColor(text->GetBackgroundColor(), tdef.opaquecolor());
            break;
    }

    RS_FontStyle_Mask style = RS_FontStyle_Regular;

    bool setting = false;
    bool const3 = EvalBoolean(text->GetBold(), setting);
    if (setting)
        style = (RS_FontStyle_Mask)(style | RS_FontStyle_Bold);

    bool const4 = EvalBoolean(text->GetItalic(), setting);
    if (setting)
        style = (RS_FontStyle_Mask)(style | RS_FontStyle_Italic);

    bool const5 = EvalBoolean(text->GetUnderlined(), setting);
    if (setting)
        style = (RS_FontStyle_Mask)(style | RS_FontStyle_Underline);

    tdef.font().style() = style;

    tdef.font().name() = text->GetFontName();

    double val;
    bool const6 = EvalDouble(text->GetSizeY(), val);
    tdef.font().height() = MdfModel::LengthConverter::UnitToMeters(text->GetUnit(), val);

    bool const7 = EvalDouble(text->GetRotation(), tdef.rotation());

    tdef.font().units() = (text->GetSizeContext() == MdfModel::MappingUnits)? RS_Units_Model : RS_Units_Device;

    bool const8 = ConvertTextHAlign(text->GetHorizontalAlignment(), tdef.halign());

    bool const9 = ConvertTextVAlign(text->GetVerticalAlignment(), tdef.valign());

    return const1 && const2 && const3 && const4 && const5 && const6 && const7 && const8 && const9;
}


void GeometryAdapter::AddLabel(double x, double y,
                               double slope_rad, bool useSlope,
                               MdfModel::Label* label,
                               RS_OverpostType type, bool exclude,
                               Renderer* renderer,
                               LineBuffer* lb)
{
    MdfModel::TextSymbol* text = label->GetSymbol();

    RS_TextDef def;
    ConvertTextDef(text, def);

    if (useSlope)
        def.rotation() = slope_rad / M_PI180;

    std::wstring txt;
    /*bool const1 =*/ EvalString(text->GetText(), txt);

    if (!txt.empty())
    {
        RS_LabelInfo info(x, y, 0.0, 0.0, RS_Units_Model, def, text->IsAdvancedPlacement());
        renderer->ProcessLabelGroup(&info, 1, txt, type, exclude, lb);
    }
}


//---------------------------------------------------------------
//parses and caches an FDO filter from a pointer to an MDF string.
//Once cached, the filter will be reused next time the same expression
//is asked for
//Also executes the filter and returns failure or success
bool GeometryAdapter::ExecFdoFilter(const MdfModel::MdfString* pExpression)
{
    //BOGUS:
    //TODO: we use pointer to the MDF strings for caching --
    //this may be risky but all caching in the stylizer is
    //based on the MDF pointer staying the same throughout the
    //stylization process

    //empty expression -- no filter
    //pass trivially
    if (pExpression->empty())
        return true;

    //get from cache
    FdoFilter* filter = m_hFilterCache[pExpression];

    //if in cache, return existing
    //NOTE: do not addref, it is not needed
    if (!filter)
    {
        try
        {
            //otherwise parse and cache it
            filter = FdoFilter::Parse(pExpression->c_str());
            m_hFilterCache[pExpression] = filter;
        }
        catch (FdoException* e)
        {
            ProcessStylizerException(e, __LINE__, __WFILE__);
            filter = NULL;
        }
    }

    //no filter means pass in this case
    if (!filter)
        return true;

    //m_exec should be set in the Stylize call
    //of the inheriting geometry adapter
    _ASSERT(m_exec);

    try
    {
        filter->Process(m_exec);
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
        return false;
    }

    return m_exec->GetResult();
}


//---------------------------------------------------------------
//parses and caches an FDO filter from a pointer to an MDF string.
//Once cached, the filter will be reused next time the same expression
//is asked for
FdoExpression* GeometryAdapter::ObtainFdoExpression(const MdfModel::MdfString* pExpression)
{
    //BOGUS:
    //TODO: we use pointer to the MDF strings for caching --
    //this may be risky but all caching in the stylizer is
    //based on the MDF pointer staying the same throughout the
    //stylization process

    //empty expression -- no filter
    if (pExpression->empty())
        return NULL;

    //get from cache
    FdoExpression* expr = m_hExpressionCache[pExpression];

    //if in cache, return existing
    //NOTE: do not addref, it is not needed
    if (expr)
        return expr;

    try
    {
        //otherwise parse and cache it
        expr = FdoExpression::Parse(pExpression->c_str());
        m_hExpressionCache[pExpression] = expr;
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
        expr = NULL;
    }

    return expr;
}

bool GeometryAdapter::GetElevationParams(RS_ElevationSettings* elevSettings, 
    double& zOffset, double& zExtrusion, RS_ElevationType& elevType)
{
    // Elevation Settings
    elevType = RS_ElevationType_RelativeToGround;
    zOffset = 0;
    zExtrusion = 0;
    if(elevSettings != NULL)
    {
        RS_String zExtrusionExpression = elevSettings->zExtrusionExpression();;
        RS_String zOffsetExpression = elevSettings->zOffsetExpression();
        elevType = elevSettings->elevType();
        double metersPerUnit = elevSettings->metersPerUnit();
        if (!zOffsetExpression.empty())
        {
            EvalDouble(zOffsetExpression, zOffset);
            zOffset *= metersPerUnit;
        }
        if(!zExtrusionExpression.empty())
        {
            EvalDouble(zExtrusionExpression, zExtrusion);
            zExtrusion *= metersPerUnit;
        }
    }
    return true;
}
