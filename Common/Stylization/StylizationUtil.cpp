//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "StylizationUtil.h"
#include "SE_Renderer.h"
#include "FeatureTypeStyleVisitor.h"
#include "SymbolVisitor.h"
#include "SLDSymbols.h"
#include "SE_StyleVisitor.h"
#include "SE_BufferPool.h"
#include "Foundation.h"


void StylizationUtil::ParseColor(const MdfString& colorstr, RS_Color& rscol)
{
    // string is in the form "AARRGGBB"
    const wchar_t* scolor = colorstr.c_str();

    size_t len = wcslen(scolor);

    // is color string empty?
    if (len == 0)
    {
        // set to undefined color
        rscol = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
        return;
    }

    // default is black
    unsigned int color = 0xFF000000;

    if (len == 8)
    {
        swscanf(scolor, L"%8X", &color);
    }
    else if (len == 6)
    {
        swscanf(scolor, L"%6X", &color);

        // there was no alpha specified in the constant string, add it
        color |= 0xFF000000;
    }

    rscol.alpha() = (color >> 24) & 0xFF;
    rscol.red()   = (color >> 16) & 0xFF;
    rscol.green() = (color >>  8) & 0xFF;
    rscol.blue()  =  color        & 0xFF;
}


// Returns true if the expression evaluates to a constant value.
bool StylizationUtil::ParseDouble(const MdfString& valstr, double& res)
{
    const wchar_t* sd = valstr.c_str();

    double d = 0.0;
    int status = swscanf(sd, L"%lf", &d);

    if (status == 1)
    {
        res = d;
        return true;
    }

    return false;
}


// draws a preview of a given feature type style into an image
void StylizationUtil::DrawStylePreview(int imgWidth,
                                       int imgHeight,
                                       int themeCategory,
                                       FeatureTypeStyle* fts,
                                       SE_Renderer* pSERenderer,
                                       SE_SymbolManager* sman)
{
    if (!fts)
        return;

    // make sure we have rules
    RuleCollection* rules = fts->GetRules();
    if (!rules)
        return;

    // if there's one rule then always use it
    if (rules->GetCount() == 1)
        themeCategory = 0;

    // validate category index
    if (themeCategory < 0 || themeCategory >= rules->GetCount())
        return;

    // set the renderer's mapping space bounds to the image bounds
    RS_Bounds bounds(0.0, 0.0, imgWidth, imgHeight);

    RS_MapUIInfo info(L"", L"name", L"guid", L"", L"", RS_Color(255, 255, 255, 0));

    double pixelsPerInch = STANDARD_DISPLAY_DPI;
    double metersPerPixel = METERS_PER_INCH / pixelsPerInch;

    pSERenderer->StartMap(&info, bounds, 1.0, pixelsPerInch, metersPerPixel, NULL);

    // overwrite the drawing scale so linestyles look good
//  pSERenderer->SetDrawingScale(1.0);

    pSERenderer->StartLayer(NULL, NULL);

    try
    {
        int type = FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts);
        switch (type)
        {
            case FeatureTypeStyleVisitor::ftsComposite:
            {
                // get correct theme rule
                CompositeRule* rule = (CompositeRule*)rules->GetAt(themeCategory);

                // render the symbolization
                CompositeSymbolization* csym = rule->GetSymbolization();
                StylizationUtil::RenderCompositeSymbolization(csym, pSERenderer, sman, 0.0, 0.0, imgWidth, imgHeight);

                break;
            }

            case FeatureTypeStyleVisitor::ftsArea:
            {
                // get correct theme rule
                AreaRule* rule = (AreaRule*)rules->GetAt(themeCategory);

                // render the symbolization
                AreaSymbolization2D* asym = rule->GetSymbolization();
                StylizationUtil::RenderAreaSymbolization(asym, pSERenderer, 0.0, 0.0, imgWidth, imgHeight);

                break;
            }

            case FeatureTypeStyleVisitor::ftsLine:
            {
                // determine the maximum line width used in this category
                double maxLineWidth = StylizationUtil::GetMaxMappingSpaceLineWidth(fts, themeCategory);

                // get correct theme rule
                LineRule* rule = (LineRule*)rules->GetAt(themeCategory);

                // render the symbolizations
                LineSymbolizationCollection* lsc = rule->GetSymbolizations();
                for (int j=0; j<lsc->GetCount(); ++j)
                {
                    LineSymbolization2D* lsym = lsc->GetAt(j);
                    StylizationUtil::RenderLineSymbolization(lsym, pSERenderer, 0.0, 0.0, imgWidth, imgHeight, maxLineWidth);
                }

                break;
            }

            case FeatureTypeStyleVisitor::ftsPoint:
            {
                // get correct theme rule
                PointRule* rule = (PointRule*)rules->GetAt(themeCategory);

                // render the symbolization
                PointSymbolization2D* psym = rule->GetSymbolization();
                StylizationUtil::RenderPointSymbolization(psym, pSERenderer, 0.0, 0.0, imgWidth, imgHeight);

                break;
            }

            default:
                break;
        }
    }
    catch (FdoException* e)
    {
        e->Release();
    }
    catch (...)
    {
    }

    pSERenderer->EndLayer();
    pSERenderer->EndMap();
}


// Draws a preview of the supplied point symbolization.  The preview is sized to
// fill the renderer image.  Calls to this method should be wrapped by the standard
// calls to StartMap / StartLayer and EndMap / EndLayer.
void StylizationUtil::RenderPointSymbolization(PointSymbolization2D* psym,
                                               SE_Renderer* pSERenderer,
                                               double x, double y,
                                               double width, double height)
{
    // sanity (note - point symbolization can be NULL)
    _ASSERT(NULL != pSERenderer);
    if (NULL == psym || NULL == pSERenderer)
        return;

    double metersPerPixel = METERS_PER_INCH / pSERenderer->GetDpi();

    RS_MarkerDef mdef;

    // just pick a symbol size in meters that
    // will mostly fit the whole image
    double sz = (rs_min(width, height) - 2.0) * metersPerPixel;
    mdef.width() = sz;
    mdef.height() = sz;
    mdef.units() = RS_Units_Device;

    Symbol* symbol = psym->GetSymbol();

    if (symbol)
    {
        SymbolVisitor::eSymbolType type = SymbolVisitor::DetermineSymbolType(symbol);

        switch (type)
        {
        case SymbolVisitor::stMark:
            {
                MarkSymbol* marksym = (MarkSymbol*)symbol;

                mdef.type() = RS_MarkerType_Marker;

                // shape
                MarkSymbol::Shape shape = marksym->GetShape();
                switch (shape)
                {
                    case MarkSymbol::Square:   mdef.markernum() = SLDType_Square;   break;
                    case MarkSymbol::Circle:   mdef.markernum() = SLDType_Circle;   break;
                    case MarkSymbol::Triangle: mdef.markernum() = SLDType_Triangle; break;
                    case MarkSymbol::Star:     mdef.markernum() = SLDType_Star;     break;
                    case MarkSymbol::Cross:    mdef.markernum() = SLDType_Cross;    break;
                    case MarkSymbol::X:        mdef.markernum() = SLDType_X;        break;
                    default: break;
                }

                // fill and edge colors
                if (marksym->GetFill())
                    StylizationUtil::ParseColor(marksym->GetFill()->GetForegroundColor(), mdef.style().color());
                else
                    mdef.style().color() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);

                if (marksym->GetEdge())
                    StylizationUtil::ParseColor(marksym->GetEdge()->GetColor(), mdef.style().outline().color());
                else
                    mdef.style().outline().color() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
            }
            break;

        case SymbolVisitor::stW2D:
            {
                W2DSymbol* w2dsym = (W2DSymbol*)symbol;

                mdef.type()    = RS_MarkerType_W2D;
                mdef.library() = w2dsym->GetSymbolLibrary();
                mdef.name()    = w2dsym->GetSymbolName();

                // fill, line, and text override colors
                StylizationUtil::ParseColor(w2dsym->GetFillColor(), mdef.style().color());
                StylizationUtil::ParseColor(w2dsym->GetLineColor(), mdef.style().outline().color());
                StylizationUtil::ParseColor(w2dsym->GetTextColor(), mdef.style().background());
            }
            break;

        case SymbolVisitor::stFont:
            {
                FontSymbol* fontSym = (FontSymbol*)symbol;

                mdef.type() = RS_MarkerType_Font;

                // store the font name as the library string
                mdef.library() = fontSym->GetFontName();

                // store the marker character as the symbol name
                mdef.name() = (wchar_t)fontSym->GetCharacter();

                // font style
                RS_FontStyle_Mask style = RS_FontStyle_Regular;

                if (_wcsnicmp(L"true", fontSym->GetBold().c_str(), 4) == 0)
                    style = (RS_FontStyle_Mask)(style | RS_FontStyle_Bold);

                if (_wcsnicmp(L"true", fontSym->GetItalic().c_str(), 4) == 0)
                    style = (RS_FontStyle_Mask)(style | RS_FontStyle_Italic);

                if (_wcsnicmp(L"true", fontSym->GetUnderlined().c_str(), 4) == 0)
                    style = (RS_FontStyle_Mask)(style | RS_FontStyle_Underline);

                mdef.fontstyle() = style;

                // foreground color
                StylizationUtil::ParseColor(fontSym->GetForegroundColor(), mdef.style().color());
            }
            break;

        case SymbolVisitor::stBlock:
        case SymbolVisitor::stImage:
            // TODO: not currently supported
            break;

        default:
            break;
        }
    }

    // generate a geometry to draw
    LineBuffer lb(2);
    lb.MoveTo(x + 0.5*width, y + 0.5*height);

    pSERenderer->ProcessMarker(&lb, mdef, true);
}


// Draws a preview of the supplied line symbolization.  The preview is sized to
// fill the renderer image.  Calls to this method should be wrapped by the standard
// calls to StartMap / StartLayer and EndMap / EndLayer.
void StylizationUtil::RenderLineSymbolization(LineSymbolization2D* lsym,
                                              SE_Renderer* pSERenderer,
                                              double x, double y,
                                              double width, double height,
                                              double maxLineWidth)
{
    // sanity checks
    _ASSERT(NULL != lsym && NULL != pSERenderer);
    if (NULL == lsym || NULL == pSERenderer)
        return;

    double metersPerPixel = METERS_PER_INCH / pSERenderer->GetDpi();

    RS_LineStroke ls;

    Stroke* stroke = lsym->GetStroke();
    if (stroke)
    {
        StylizationUtil::ParseColor(stroke->GetColor(), ls.color());

        ls.style() = stroke->GetLineStyle();

        double edgeWidth = 0.0;
        StylizationUtil::ParseDouble(stroke->GetThickness(), edgeWidth);
        edgeWidth = LengthConverter::UnitToMeters(stroke->GetUnit(), edgeWidth);
        if (edgeWidth > 0.0)
        {
            if (stroke->GetSizeContext() == MappingUnits)
            {
                // For line widths in mapping units, scale so the widest line
                // is half the height of the legend image.
                edgeWidth = edgeWidth / maxLineWidth * 0.5 * height * metersPerPixel;
            }
            else if (edgeWidth > height * metersPerPixel)
            {
                // For lines in device coords, ensure that the line is not wider
                // than the height of the legend image.  This is a performance
                // optimization and does not affect the appearance of the image
                edgeWidth = height * metersPerPixel;
            }
        }

        ls.width() = edgeWidth;
        ls.units() = (stroke->GetSizeContext() == DeviceUnits)? RS_Units_Device : RS_Units_Model;
    }

    // generate a geometry to draw - make it slightly smaller than
    // the map extent to avoid having missing pixels at the edges
    LineBuffer lb(2);
    lb.MoveTo(x +       0.000001, y + 0.5*height);
    lb.LineTo(x + width-0.000001, y + 0.5*height);

    pSERenderer->ProcessPolyline(&lb, ls);
}


// Draws a preview of the supplied area symbolization.  The preview is sized to
// fill the renderer image.  Calls to this method should be wrapped by the standard
// calls to StartMap / StartLayer and EndMap / EndLayer.
void StylizationUtil::RenderAreaSymbolization(AreaSymbolization2D* asym,
                                              SE_Renderer* pSERenderer,
                                              double x, double y,
                                              double width, double height)
{
    // sanity checks
    _ASSERT(NULL != asym && NULL != pSERenderer);
    if (NULL == asym || NULL == pSERenderer)
        return;

    double metersPerPixel = METERS_PER_INCH / pSERenderer->GetDpi();

    // convert fill style to RS_FillStyle
    RS_FillStyle fs;
    int linePixelWidth = 0;

    Fill* fill = asym->GetFill();
    if (fill)
    {
        StylizationUtil::ParseColor(fill->GetBackgroundColor(), fs.background());
        StylizationUtil::ParseColor(fill->GetForegroundColor(), fs.color());
        fs.pattern() = fill->GetFillPattern();
    }
    else
    {
        fs.background() = RS_Color(0,0,0,0);
        fs.color() = RS_Color(0,0,0,0);
    }

    Stroke* edge = asym->GetEdge();
    if (edge)
    {
        StylizationUtil::ParseColor(edge->GetColor(), fs.outline().color());
        fs.outline().style() = edge->GetLineStyle();

        double edgeWidth = 0.0;
        StylizationUtil::ParseDouble(edge->GetThickness(), edgeWidth);
        edgeWidth = LengthConverter::UnitToMeters(edge->GetUnit(), edgeWidth);
        if (edgeWidth > 0.0)
        {
            if (edge->GetSizeContext() == MappingUnits)
            {
                // for mapping space edges with non-zero width, always use a
                // width of two pixels
                edgeWidth = 2.0 * metersPerPixel;
            }
            else if (edgeWidth > (0.5*rs_min(width, height) - 2.0) * metersPerPixel)
            {
                // for lines in device coords, ensure that the line width
                // still allows a 4 pixel square of fill color to be displayed
                edgeWidth = (0.5*rs_min(width, height) - 2.0) * metersPerPixel;
            }
        }
        linePixelWidth = (int)(edgeWidth / metersPerPixel);

        fs.outline().width() = edgeWidth;
        fs.outline().units() = (edge->GetSizeContext() == DeviceUnits)? RS_Units_Device : RS_Units_Model;
    }
    else
        fs.outline().color() = RS_Color(0,0,0,0);

    // lines with zero width are rendered one pixel wide
    if (linePixelWidth == 0)
        linePixelWidth = 1;

    // create a rectangle that allows the line width to be
    // included within the legend image, and also make it
    // slightly smaller than the map extent to avoid having
    // missing pixels at the edges
    double offset = linePixelWidth / 2 + 0.000001;

    LineBuffer lb(5);
    lb.MoveTo(x +       offset, y +        offset);
    lb.LineTo(x + width-offset, y +        offset);
    lb.LineTo(x + width-offset, y + height-offset);
    lb.LineTo(x +       offset, y + height-offset);
    lb.Close();

    pSERenderer->ProcessPolygon(&lb, fs);
}


// Draws a preview of the supplied composite symbolization.  The preview is sized to
// fill the renderer image.  Calls to this method should be wrapped by the standard
// calls to StartMap / StartLayer and EndMap / EndLayer.
//
// TODO: issues with the current implementation:
// - The symbol is currently drawn as a point symbol.  We'll need to change that
//   to draw the symbol on a sample geometry matching the usage type.  E.g. in the
//   case where the symbol specifies a LineUsage, we'll draw the symbol along an
//   imaginary line that crosses the preview image.  Once again though the problem
//   will be how to draw a meaningful preview in such a small image.
void StylizationUtil::RenderCompositeSymbolization(CompositeSymbolization* csym,
                                                   SE_Renderer* pSERenderer,
                                                   SE_SymbolManager* sman,
                                                   double x, double y,
                                                   double width, double height)
{
    SE_BufferPool* pool = pSERenderer->GetBufferPool();
    SE_StyleVisitor visitor(sman, pool);

    std::vector<SE_Symbolization*> styles;
    visitor.Convert(styles, csym);

    // create an expression engine with our custom functions
    FdoPtr<FdoExpressionEngine> exec = ExpressionHelper::GetExpressionEngine(pSERenderer, NULL);

    //-------------------------------------------------------
    // step 1 - get the overall bounds for the symbolization
    //-------------------------------------------------------

    RS_Bounds symBounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
    GetCompositeSymbolizationBoundsInternal(styles, pSERenderer, sman, exec, symBounds);

    //-------------------------------------------------------
    // step 2 - bounds processing
    //-------------------------------------------------------

    if (!symBounds.IsValid())
        return;

    double mm2sud = pSERenderer->GetScreenUnitsPerMillimeterDevice();
    double mm2suw = pSERenderer->GetScreenUnitsPerMillimeterWorld();
    double drawingScale = pSERenderer->GetDrawingScale();
    bool yUp = pSERenderer->YPointsUp();

    // check for degenerate bounds
    if (symBounds.width() == 0.0 && symBounds.height() == 0.0)
    {
        // make the bounds have the same width and height as the image
        symBounds.minx -= 0.5 * width / drawingScale;
        symBounds.maxx += 0.5 * width / drawingScale;
        symBounds.miny -= 0.5 * height / drawingScale;
        symBounds.maxy += 0.5 * height / drawingScale;
    }

    // make the aspect ratio of the symbol bounds match that of the supplied
    // rectangle - this is needed to properly center the symbol in the rectangle
    double arDisplay = width / height;
    if (symBounds.width() > symBounds.height() * arDisplay)
    {
        double dHeight = symBounds.width() / arDisplay - symBounds.height();
        symBounds.miny -= 0.5*dHeight;
        symBounds.maxy += 0.5*dHeight;
    }
    else
    {
        double dWidth = symBounds.height() * arDisplay - symBounds.width();
        symBounds.minx -= 0.5*dWidth;
        symBounds.maxx += 0.5*dWidth;
    }

    // make the bounds slightly larger to avoid having missing pixels at the edges
    double w = symBounds.width();
    double h = symBounds.height();
    symBounds.minx -= 0.00001*w;
    symBounds.miny -= 0.00001*h;
    symBounds.maxx += 0.00001*w;
    symBounds.maxy += 0.00001*h;

    //-------------------------------------------------------
    // step 3 - pre-draw preparation
    //-------------------------------------------------------

    // From the bounds we can compute how much to scale and translate the symbols so
    // that they fill the supplied rectangle.  We could then apply the scaling and
    // translation directly to the current world-to-screen transform, and if we drew
    // the currently evaluated symbols using it they would fill the supplied rectangle.

    // The problem is that any text heights, line weights, and image sizes in the
    // currently evaluated symbols are not adjusted when we draw using this updated
    // transform.  To fix this we need to re-evaluate the symbols using a transform
    // which includes the scale factor from the updated world-to-screen transform.
    //
    // Borrowing the notation from StylizationEngine::Stylize we have:
    //
    //   [T_fe] [S_mm] [T_si] [R_pu] [S_si] [T_pu] {Geom}
    //
    // We'll add an additional scale factor [S_a] and translation [T_a] as follows:
    //
    //   [T_a] [T_fe] [S_a] [S_mm] [T_si] [R_pu] [S_si] [T_pu] {Geom}
    //
    // Reworking this then gives:
    //
    //   [T_a] [T_fe] [T_si*] [R_pu*] [T_pu*] [S_a] [S_mm] [S_si] {Geom}
    //
    // where:
    //   T_si* = symbol instance insertion offset, using offsets scaled by S_a and S_mm
    //   R_pu* = point usage rotation, with angle accounting for y-up or y-down
    //   T_pu* = point usage origin offset, using offsets scaled by S_a, S_mm, and S_si

    // compute the scale factor we need to apply to the symbol to size it correctly
    double scale = width / (drawingScale * symBounds.width());

    // get the center of the scaled symbol
    double symCtrX = 0.5*(symBounds.minx + symBounds.maxx) * scale;
    double symCtrY = 0.5*(symBounds.miny + symBounds.maxy) * scale;

    // get the center of the rectangle center
    double mapCtrX = x + 0.5*width;
    double mapCtrY = y + 0.5*height;
    pSERenderer->WorldToScreenPoint(mapCtrX, mapCtrY, mapCtrX, mapCtrY);

    // get the translation matrix which centers the symbol in the specified rectangle
    SE_Matrix xformTrans;
    xformTrans.translate(mapCtrX, mapCtrY);
    xformTrans.translate(-symCtrX, yUp? -symCtrY : symCtrY);

    //-------------------------------------------------------
    // step 4 - re-evaluate and draw the symbolization
    //-------------------------------------------------------

    for (std::vector<SE_Symbolization*>::const_iterator iter = styles.begin(); iter != styles.end(); iter++)
    {
        // one per symbol instance
        SE_Symbolization* sym = *iter;

        // skip labels
        if (sym->drawLast.evaluate(exec))
            continue;

        double mm2suX = (sym->sizeContext == MappingUnits)? mm2suw : mm2sud;
        double mm2suY = yUp? mm2suX : -mm2suX;

        // this time we scale by [S_si], [S_mm], and [S_a]
        SE_Matrix xformScale;
        xformScale.scale(sym->scale[0].evaluate(exec),
                         sym->scale[1].evaluate(exec));
        xformScale.scale(mm2suX, mm2suY);
        xformScale.scale(scale, scale);

        // initialize the style evaluation context
        // NOTE: do not adjust the mm2su values by the scale factor
        SE_EvalContext evalCxt;
        evalCxt.exec = exec;
        evalCxt.mm2su = mm2suX;
        evalCxt.mm2sud = mm2sud;
        evalCxt.mm2suw = mm2suw;
        evalCxt.px2su = pSERenderer->GetScreenUnitsPerPixel();
        evalCxt.pool = pool;
        evalCxt.fonte = pSERenderer->GetRSFontEngine();
        evalCxt.xform = &xformScale;
        evalCxt.resources = sman;

        for (std::vector<SE_Style*>::const_iterator siter = sym->styles.begin(); siter != sym->styles.end(); siter++)
        {
            // have one style per simple symbol definition
            SE_Style* style = *siter;

            // since the render styles are cached we need to reset these before
            // re-evaluating the style
            style->reset();
            style->evaluate(&evalCxt);

            // Each style type has additional transformations associated with it.  See
            // StylizationEngine::Stylize for a detailed explanation of these transforms.
            SE_Matrix xformStyle;
            double angleRad = 0.0;

            SE_RenderStyle* rStyle = style->rstyle;
            switch (rStyle->type)
            {
                case SE_RenderStyle_Point:
                {
                    SE_RenderPointStyle* ptStyle = (SE_RenderPointStyle*)(rStyle);

                    // point usage offset (already scaled by [S_si], [S_mm], and [S_a])
                    xformStyle.translate(ptStyle->offset[0], ptStyle->offset[1]);

                    // point usage rotation - assume geometry angle is zero
                    angleRad = ptStyle->angleRad;
                    xformStyle.rotate(angleRad);

                    // symbol instance offset - must scale this by [S_mm], and [S_a]
                    xformStyle.translate(sym->absOffset[0].evaluate(exec) * mm2suX * scale,
                                         sym->absOffset[1].evaluate(exec) * mm2suY * scale);

                    break;
                }

                case SE_RenderStyle_Line:
                {
                    SE_RenderLineStyle* lnStyle = (SE_RenderLineStyle*)(rStyle);

                    // line usage rotation - assume geometry angle is zero
                    angleRad = lnStyle->angleRad;
                    xformStyle.rotate(angleRad);

                    break;
                }

                case SE_RenderStyle_Area:
                default:
                    break;
            }

            // factor the translation into the transform and draw the symbol
            xformStyle.premultiply(xformTrans);
            pSERenderer->DrawSymbol(style->rstyle->symbol, xformStyle, angleRad);
        }
    }

    //-------------------------------------------------------
    // step 5 - final clean up
    //-------------------------------------------------------

    for (std::vector<SE_Symbolization*>::iterator iter = styles.begin(); iter != styles.end(); iter++)
        delete *iter;

    styles.clear();
}


// Returns the bounds of the supplied composite symbolization in the context
// of the supplied renderer.  The platform must be properly initialized via a
// call to MgFoundation::LoadConfiguration (including specifying the path to
// the resource file) in order for this method to work.
RS_Bounds StylizationUtil::GetCompositeSymbolizationBounds(CompositeSymbolization* csym,
                                                           SE_Renderer* pSERenderer,
                                                           SE_SymbolManager* sman)
{
    SE_BufferPool* pool = pSERenderer->GetBufferPool();
    SE_StyleVisitor visitor(sman, pool);

    std::vector<SE_Symbolization*> styles;
    visitor.Convert(styles, csym);

    // create an expression engine with our custom functions
    FdoPtr<FdoExpressionEngine> exec = ExpressionHelper::GetExpressionEngine(pSERenderer, NULL);

    // calculate bounds - symbol geometries cannot use expressions, so no expression engine is needed
    RS_Bounds symBounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
    GetCompositeSymbolizationBoundsInternal(styles, pSERenderer, sman, exec, symBounds);

    // clean up
    for (std::vector<SE_Symbolization*>::iterator iter = styles.begin(); iter != styles.end(); iter++)
        delete *iter;

    styles.clear();

    return symBounds;
}


// Computes the bounds of the supplied composite symbolization.
void StylizationUtil::GetCompositeSymbolizationBoundsInternal(std::vector<SE_Symbolization*> styles,
                                                              SE_Renderer* pSERenderer,
                                                              SE_SymbolManager* sman,
                                                              FdoExpressionEngine* exec,
                                                              RS_Bounds& bounds)
{
    // make sure we have symbols
    if (styles.size() == 0)
        return;

    SE_BufferPool* pool = pSERenderer->GetBufferPool();

    double mm2sud = pSERenderer->GetScreenUnitsPerMillimeterDevice();
    double mm2suw = pSERenderer->GetScreenUnitsPerMillimeterWorld();

    for (std::vector<SE_Symbolization*>::const_iterator iter = styles.begin(); iter != styles.end(); iter++)
    {
        // one per symbol instance
        SE_Symbolization* sym = *iter;

        // skip labels
        if (sym->drawLast.evaluate(exec))
            continue;

        // keep y pointing up while we compute the bounds
        double mm2suX = (sym->sizeContext == MappingUnits)? mm2suw : mm2sud;
        double mm2suY = mm2suX;

        SE_Matrix xformScale;
        xformScale.scale(sym->scale[0].evaluate(exec),
                         sym->scale[1].evaluate(exec));
        xformScale.scale(mm2suX, mm2suY);

        // initialize the style evaluation context
        SE_EvalContext evalCxt;
        evalCxt.exec = exec;
        evalCxt.mm2su = mm2suX;
        evalCxt.mm2sud = mm2sud;
        evalCxt.mm2suw = mm2suw;
        evalCxt.px2su = pSERenderer->GetScreenUnitsPerPixel();
        evalCxt.pool = pool;
        evalCxt.fonte = pSERenderer->GetRSFontEngine();
        evalCxt.xform = &xformScale;
        evalCxt.resources = sman;

        for (std::vector<SE_Style*>::const_iterator siter = sym->styles.begin(); siter != sym->styles.end(); siter++)
        {
            // have one style per simple symbol definition
            SE_Style* style = *siter;
            style->evaluate(&evalCxt);

            // Each style type has additional transformations associated with it.  See
            // StylizationEngine::Stylize for a detailed explanation of these transforms.
            SE_Matrix xformStyle;

            SE_RenderStyle* rStyle = style->rstyle;
            switch (rStyle->type)
            {
                case SE_RenderStyle_Point:
                {
                    SE_RenderPointStyle* ptStyle = (SE_RenderPointStyle*)(rStyle);

                    // point usage offset (already scaled)
                    xformStyle.translate(ptStyle->offset[0], ptStyle->offset[1]);

                    // point usage rotation - assume geometry angle is zero
                    xformStyle.rotate(ptStyle->angleRad);

                    // symbol instance offset
                    xformStyle.translate(sym->absOffset[0].evaluate(exec) * mm2suX,
                                         sym->absOffset[1].evaluate(exec) * mm2suY);

                    break;
                }

                case SE_RenderStyle_Line:
                {
                    SE_RenderLineStyle* lnStyle = (SE_RenderLineStyle*)(rStyle);

                    // line usage rotation - assume geometry angle is zero
                    xformStyle.rotate(lnStyle->angleRad);

                    break;
                }

                case SE_RenderStyle_Area:
                default:
                    break;
            }

            // if the symbol def has graphic elements then we can add their bounds
            if (style->rstyle->symbol.size() > 0)
            {
                for (int i=0; i<4; ++i)
                {
                    // account for any style-specific transform
                    RS_F_Point pt = style->rstyle->bounds[i];
                    xformStyle.transform(pt.x, pt.y);
                    bounds.add_point(pt);
                }
            }
        }
    }
}


// Determine the maximum line width contained in the specified feature type style
double StylizationUtil::GetMaxMappingSpaceLineWidth(FeatureTypeStyle* fts, int themeCategory)
{
    double maxLineWidth = 0.0;

    if (fts)
    {
        switch (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts))
        {
            case FeatureTypeStyleVisitor::ftsArea:
            {
                AreaTypeStyle* ats = (AreaTypeStyle*)fts;
                RuleCollection* arc = ats->GetRules();
                if (arc)
                {
                    // case caller asked for one and only category
                    // or category index is bad
                    if ((themeCategory < 0 || themeCategory >= arc->GetCount()) && arc->GetCount() == 1)
                        themeCategory = 0;

                    if (themeCategory >= 0 && themeCategory <= arc->GetCount())
                    {
                        // get correct theme rule
                        AreaRule* rule = (AreaRule*)arc->GetAt(themeCategory);

                        // convert fill style to RS_FillStyle
                        AreaSymbolization2D* asym = rule->GetSymbolization();
                        RS_FillStyle fs;

                        if (asym)
                        {
                            Stroke* edgeStroke = asym->GetEdge();
                            if (edgeStroke && edgeStroke->GetSizeContext() == MappingUnits)
                            {
                                double edgeWidth = 0.0;
                                StylizationUtil::ParseDouble(edgeStroke->GetThickness(), edgeWidth);
                                edgeWidth = LengthConverter::UnitToMeters(edgeStroke->GetUnit(), edgeWidth);
                                if (edgeWidth > maxLineWidth)
                                    maxLineWidth = edgeWidth;
                            }
                        }
                    }
                }
                break;
            }
            case FeatureTypeStyleVisitor::ftsLine:
            {
                LineTypeStyle* lts = (LineTypeStyle*) fts;
                RuleCollection* lrc = lts->GetRules();

                if (lrc)
                {
                    // case caller asked for one and only category
                    // or category index is bad
                    if ((themeCategory < 0 || themeCategory >= lrc->GetCount()) && lrc->GetCount() == 1)
                        themeCategory = 0;

                    if (themeCategory >= 0 && themeCategory <= lrc->GetCount())
                    {
                        LineRule* lr = (LineRule*)lrc->GetAt(themeCategory);
                        LineSymbolizationCollection* lsc = lr->GetSymbolizations();

                        for (int j=0; j<lsc->GetCount(); ++j)
                        {
                            LineSymbolization2D* lsym = lsc->GetAt(j);
                            Stroke* stroke = lsym->GetStroke();
                            if (stroke && stroke->GetSizeContext() == MappingUnits)
                            {
                                double edgeWidth = 0.0;
                                StylizationUtil::ParseDouble(stroke->GetThickness(), edgeWidth);
                                edgeWidth = LengthConverter::UnitToMeters(stroke->GetUnit(), edgeWidth);
                                if (edgeWidth > maxLineWidth)
                                    maxLineWidth = edgeWidth;
                            }
                        }
                    }
                }
                break;
            }
        }
    }

    return maxLineWidth;
}
