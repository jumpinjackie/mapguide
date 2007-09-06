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
#include "StylizationUtil.h"
#include "Renderer.h"
#include "SE_Renderer.h"
#include "FeatureTypeStyleVisitor.h"
#include "SymbolVisitor.h"
#include "SLDSymbols.h"
#include "SE_StyleVisitor.h"
#include "SE_BufferPool.h"
#include "RS_ByteData.h"


// draws a given feature type style into an image
RS_ByteData* StylizationUtil::DrawStylePreview(const RS_String& format,
                                               INT32 imgWidth,
                                               INT32 imgHeight,
                                               INT32 themeCategory,
                                               FeatureTypeStyle* fts,
                                               Renderer* renderer,
                                               SE_Renderer* se_renderer,
                                               SE_SymbolManager* sman)
{
    if (!fts)
        return NULL;

    double pixelW = imgWidth;
    double pixelH = imgHeight;

    RS_Bounds b(0.0, 0.0, pixelW, pixelH);

    RS_MapUIInfo info(L"", L"name", L"guid", L"", L"", RS_Color(255, 255, 255, 0));

    double pixelsPerInch = 96.0;
    double metersPerPixel = 0.0254 / pixelsPerInch;

    renderer->StartMap(&info, b, 1.0, pixelsPerInch, metersPerPixel, NULL);

    renderer->StartLayer(NULL, NULL);

    try                                                                       \
    {                                                                         \
        int type = FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts);
        switch (type)
        {
        case FeatureTypeStyleVisitor::ftsComposite:
            {
                CompositeTypeStyle* cts = (CompositeTypeStyle*)fts;
                RuleCollection* crc = cts->GetRules();

                if (crc)
                {
                    // case caller asked for one and only category
                    // or category index is bad
                    if ((themeCategory < 0 || themeCategory >= crc->GetCount()) && crc->GetCount() == 1)
                        themeCategory = 0;

                    if (themeCategory >= 0 && themeCategory <= crc->GetCount())
                    {
                        //get correct theme rule
                        CompositeRule* rule = (CompositeRule*)crc->GetAt(themeCategory);
                        CompositeSymbolization* csym = rule->GetSymbolization();

                        DrawCTSPreview(csym, renderer, se_renderer, sman);
                    }
                    else
                    {
                        // Error
                        // TODO: throw?
                    }
                }
            }
            break;

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
                        int linePixelWidth = 0;
                        if (asym)
                        {
                            if (asym->GetFill())
                            {
                                StylizationUtil::ParseColor(asym->GetFill()->GetBackgroundColor(), fs.background());
                                StylizationUtil::ParseColor(asym->GetFill()->GetForegroundColor(), fs.color());
                                fs.pattern() = asym->GetFill()->GetFillPattern();
                            }
                            else
                            {
                                fs.color() = RS_Color(0,0,0,0);
                                fs.background() = RS_Color(0,0,0,0);
                            }

                            Stroke* edgeStroke = asym->GetEdge();
                            if (edgeStroke)
                            {
                                StylizationUtil::ParseColor(edgeStroke->GetColor(), fs.outline().color());
                                fs.outline().style() = edgeStroke->GetLineStyle();

                                double width = 0.0;
                                StylizationUtil::ParseDouble(edgeStroke->GetThickness(), width);
                                width = LengthConverter::UnitToMeters(edgeStroke->GetUnit(), width);
                                if (width > 0.0)
                                {
                                    if (edgeStroke->GetSizeContext() == MappingUnits)
                                    {
                                        // for mapping space edges with non-zero width, always use a
                                        // width of two pixels
                                        width = 2.0 * metersPerPixel;
                                    }
                                    else if (width > (0.5*rs_min(pixelH, pixelW) - 2.0) * metersPerPixel)
                                    {
                                        // for lines in device coords, ensure that the line width
                                        // still allows a 4 pixel square of fill color to be displayed
                                        width = (0.5*rs_min(pixelH, pixelW) - 2.0) * metersPerPixel;
                                    }
                                }
                                linePixelWidth = (int)(width / metersPerPixel);
                                fs.outline().width() = width;
                                fs.outline().units() = (edgeStroke->GetSizeContext() == DeviceUnits)? RS_Units_Device : RS_Units_Model;
                            }
                            else
                                fs.outline().color() = RS_Color(0,0,0,0);
                        }

                        // lines with zero width are rendered one pixel wide
                        if (linePixelWidth == 0)
                            linePixelWidth = 1;

                        // create a rectangle that allows the line width to be
                        // included within the legend image, and also make it
                        // slightly smaller than the map extent to avoid having
                        // missing pixels at the edges
                        double offset = linePixelWidth / 2 + 0.000001;

                        LineBuffer lb(5);
                        lb.MoveTo(       offset,        offset);
                        lb.LineTo(pixelW-offset,        offset);
                        lb.LineTo(pixelW-offset, pixelH-offset);
                        lb.LineTo(       offset, pixelH-offset);
                        lb.Close();

                        renderer->ProcessPolygon(&lb, fs);
                    }
                    else
                    {
                        // Error
                        // TODO: throw?
                    }
                }
            }
            break;

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

                        // determine the maximum line width used by this FTS
                        double maxLineWidth = GetMaxMappingSpaceLineWidth(fts, themeCategory);
                        for (int j=0; j<lsc->GetCount(); j++)
                        {
                            LineSymbolization2D* lsym = lsc->GetAt(j);
                            Stroke* stroke = lsym->GetStroke();

                            RS_LineStroke ls;

                            if (stroke)
                            {
                                StylizationUtil::ParseColor(stroke->GetColor(), ls.color());

                                ls.style() = stroke->GetLineStyle();

                                double width = 0.0;
                                StylizationUtil::ParseDouble(stroke->GetThickness(), width);
                                width = LengthConverter::UnitToMeters(stroke->GetUnit(), width);
                                if (width > 0.0)
                                {
                                    if (stroke->GetSizeContext() == MappingUnits)
                                    {
                                        // for line widths in mapping units, scale so the widest line
                                        // is half the height of the legend image
                                        width = width / maxLineWidth * 0.5 * pixelH * metersPerPixel;
                                    }
                                    else if (width > pixelH * metersPerPixel)
                                    {
                                        // for lines in device coords, ensure that the line is not wider
                                        // than the height of the legend image. This is a performance
                                        // optimization and does not affect the appearance of the image
                                        width = pixelH * metersPerPixel;
                                    }
                                }
                                ls.width() = width;
                                ls.units() = (stroke->GetSizeContext() == DeviceUnits)? RS_Units_Device : RS_Units_Model;
                            }

                            // generate a geometry to draw - make it slightly smaller than
                            // the map extent to avoid having missing pixels at the edges
                            LineBuffer lb(2);
                            lb.MoveTo(       0.000001, 0.5*pixelH);
                            lb.LineTo(pixelW-0.000001, 0.5*pixelH);

                            renderer->ProcessPolyline(&lb, ls);
                        }
                    }
                    else
                    {
                        // Error
                        // TODO: throw?
                    }
                }
            }
            break;

        case FeatureTypeStyleVisitor::ftsPoint:
            {
                PointTypeStyle* pts = (PointTypeStyle*) fts;
                RuleCollection* prc = pts->GetRules();

                if (prc)
                {
                    // case caller asked for one and only category
                    // or category index is bad
                    if ((themeCategory < 0 || themeCategory >= prc->GetCount()) && prc->GetCount() == 1)
                        themeCategory = 0;

                    if (themeCategory >= 0 && themeCategory < prc->GetCount())
                    {
                        PointRule* pr = (PointRule*)prc->GetAt(themeCategory);

                        PointSymbolization2D* ps = pr->GetSymbolization();

                        RS_MarkerDef mdef;

                        // just pick a symbol size in meters that
                        // will mostly fit the whole image
                        double sz = (rs_min(pixelW, pixelH) - 2.0) * metersPerPixel;
                        mdef.width() = sz;
                        mdef.height() = sz;
                        mdef.units() = RS_Units_Model;

                        if (ps)
                        {
                            Symbol* symbol = ps->GetSymbol();

                            if (symbol)
                            {
                                SymbolVisitor::eSymbolType type = SymbolVisitor::DetermineSymbolType(ps->GetSymbol());

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
                        }

                        // generate a geometry to draw
                        LineBuffer lb(2);
                        lb.MoveTo(0.5*pixelW, 0.5*pixelH);

                        renderer->ProcessMarker(&lb, mdef, true);
                    }
                    else
                    {
                        // Error
                        // TODO: throw?
                    }
                }
            }
            break;
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

    renderer->EndLayer();
    renderer->EndMap();

    return renderer->SaveAsImage(format, imgWidth, imgHeight);
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
void StylizationUtil::DrawCTSPreview(CompositeSymbolization* csym,
                                     Renderer* renderer, SE_Renderer* se_renderer, SE_SymbolManager* sman)
{
    double mm2pxs = se_renderer->GetPixelsPerMillimeterScreen();
    double mm2pxw = se_renderer->GetPixelsPerMillimeterWorld();

    SE_BufferPool pool;
    SE_StyleVisitor visitor(sman, &pool);
    RS_FilterExecutor* exec = RS_FilterExecutor::Create(NULL);

    std::vector<SE_Symbolization*> styles;

    //-------------------------------------------------------
    // step 1 - get the overall bounds for the symbolization
    //-------------------------------------------------------

    visitor.Convert(styles, csym);

    RS_Bounds symBounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
    for (std::vector<SE_Symbolization*>::const_iterator iter = styles.begin(); iter != styles.end(); iter++)
    {
        // one per symbol instance
        SE_Symbolization* sym = *iter;

        // keep y pointing up while we compute the bounds
        double mm2px = (sym->context == MappingUnits)? mm2pxw : mm2pxs;

        SE_Matrix xformScale;
        xformScale.scale(sym->scale[0].evaluate(exec),
                         sym->scale[1].evaluate(exec));
        xformScale.scale(mm2px, mm2px);

        // initialize the style evaluation context
        SE_EvalContext cxt;
        cxt.exec = exec;
        cxt.mm2px = mm2px;
        cxt.mm2pxs = mm2pxs;
        cxt.mm2pxw = mm2pxw;
        cxt.pool = &pool;
        cxt.fonte = se_renderer->GetRSFontEngine();
        cxt.xform = &xformScale;
        cxt.resources = sman;

        for (std::vector<SE_Style*>::const_iterator siter = sym->styles.begin(); siter != sym->styles.end(); siter++)
        {
            // have one style per simple symbol definition
            SE_Style* style = *siter;
            style->evaluate(&cxt);

            // Each style type has additional transformations associated with it.  See
            // StylizationEngine::Stylize for a detailed explanation of these transforms.
            SE_Matrix xformStyle;

            SE_RenderStyle* rStyle = style->rstyle;
            switch (rStyle->type)
            {
                case SE_RenderPointStyleType:
                {
                    SE_RenderPointStyle* ptStyle = (SE_RenderPointStyle*)(rStyle);

                    // point usage offset (already scaled)
                    xformStyle.translate(ptStyle->offset[0], ptStyle->offset[1]);

                    // point usage rotation
                    if (wcscmp(L"FromAngle", ptStyle->angleControl) == 0)
                        xformStyle.rotate(ptStyle->angleRad);

                    // symbol instance offset
                    xformStyle.translate(sym->absOffset[0].evaluate(exec) * mm2px,
                                         sym->absOffset[1].evaluate(exec) * mm2px);

                    break;
                }

                case SE_RenderLineStyleType:
                {
                    SE_RenderLineStyle* lnStyle = (SE_RenderLineStyle*)(rStyle);

                    // line usage rotation
                    if (wcscmp(L"FromAngle", lnStyle->angleControl) == 0)
                        xformStyle.rotate(lnStyle->angleRad);

                    break;
                }

                case SE_RenderAreaStyleType:
                default:
                    break;
            }

            // if the symbol def has graphic elements then we can add its bounds
            if (style->rstyle->symbol.size() > 0)
            {
                for (int i=0; i<4; ++i)
                {
                    // account for any style-specific transform
                    RS_F_Point pt = style->rstyle->bounds[i];
                    xformStyle.transform(pt.x, pt.y);
                    symBounds.add_point(pt);
                }
            }
        }
    }

    // make the aspect ratio of the bounds match that of the image - this
    // is needed to properly center the symbol in the image
    RS_Bounds& imgBounds = renderer->GetBounds();
    double arDisplay = imgBounds.width() / imgBounds.height();
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
    // step 2 - pre-draw preparation
    //-------------------------------------------------------

    // The easiest way to make the symbolization fit into the image is to set the
    // renderer extent to the symbol's.  We could then get the updated world-to-screen
    // transform, and if we drew the currently evaluated symbols using it they would
    // fill the image.
    renderer->SetBounds(symBounds);

    // The problem is that any text heights, line weights, and image sizes in the
    // currently evaluated symbols are not adjusted when we draw using this updated
    // transform.  To fix this we need to re-evaluate the symbols using a transform
    // which includes the scale factor from the updated world-to-screen transform.
    //
    // Borrowing the notation from StylizationEngine::Stylize we have:
    //
    //   [M_w2s] [S_mm] [T_si] [R_pu] [S_si] [T_pu] {Geom}
    //
    // where:
    //   M_w2s = world-to-screen transform
    //
    // We factor out the scale component [S_a] from [M_w2s] as follows:
    //
    //   [M_w2s] [S_a]^(-1) [S_a] [S_mm] [T_si] [R_pu] [S_si] [T_pu] {Geom}
    //
    // Reworking this then gives:
    //
    //   [M_w2s] [S_a]^(-1) [T_si*] [R_pu*] [T_pu*] [S_a] [S_mm] [S_si] {Geom}
    //
    // where:
    //   T_si* = symbol instance insertion offset, using offsets scaled by S_a and S_mm
    //   R_pu* = point usage rotation, with angle accounting for y-up or y-down
    //   T_pu* = point usage origin offset, using offsets scaled by S_a, S_mm, and S_si

    SE_Matrix xformW2S;
    se_renderer->GetWorldToScreenTransform(xformW2S);

    // compute the inverse scale matrix - [S_a]^(-1)
    SE_Matrix xformInvScale;
    xformInvScale.scale(1.0/xformW2S.x0, 1.0/xformW2S.x0);

    // include this in xformW2S - this gives us [M_w2s] [S_a]^(-1)
    xformW2S.postmultiply(xformInvScale);

    //-------------------------------------------------------
    // step 3 - draw the symbolization
    //-------------------------------------------------------

    for (std::vector<SE_Symbolization*>::const_iterator iter = styles.begin(); iter != styles.end(); iter++)
    {
        // one per symbol instance
        SE_Symbolization* sym = *iter;

        // keep y pointing up while we evaluate the symbols - drawXform includes
        // the y-down factor
        double mm2px = (sym->context == MappingUnits)? mm2pxw : mm2pxs;

        // this time we scale by [S_si], [S_mm], and [S_a]
        SE_Matrix xformScale;
        xformScale.scale(sym->scale[0].evaluate(exec),
                         sym->scale[1].evaluate(exec));
        xformScale.scale(mm2px, mm2px);
        xformScale.scale(xformW2S.x0, xformW2S.x0);

        // initialize the style evaluation context
        // NOTE: do not adjust the mm2px values by the scale factor
        SE_EvalContext cxt;
        cxt.exec = exec;
        cxt.mm2px = mm2px;
        cxt.mm2pxs = mm2pxs;
        cxt.mm2pxw = mm2pxw;
        cxt.pool = &pool;
        cxt.fonte = se_renderer->GetRSFontEngine();
        cxt.xform = &xformScale;
        cxt.resources = sman;

        for (std::vector<SE_Style*>::const_iterator siter = sym->styles.begin(); siter != sym->styles.end(); siter++)
        {
            // have one style per simple symbol definition
            SE_Style* style = *siter;

            // since the render styles are cached we need to reset these before
            // re-evaluating the style
            style->reset();
            style->evaluate(&cxt);

            // Each style type has additional transformations associated with it.  See
            // StylizationEngine::Stylize for a detailed explanation of these transforms.
            SE_Matrix xformStyle;
            double angleRad = 0.0;

            SE_RenderStyle* rStyle = style->rstyle;
            switch (rStyle->type)
            {
                case SE_RenderPointStyleType:
                {
                    SE_RenderPointStyle* ptStyle = (SE_RenderPointStyle*)(rStyle);

                    // point usage offset (already scaled by [S_si], [S_mm], and [S_a])
                    xformStyle.translate(ptStyle->offset[0], ptStyle->offset[1]);

                    // point usage rotation
                    if (wcscmp(L"FromAngle", ptStyle->angleControl) == 0)
                    {
                        xformStyle.rotate(ptStyle->angleRad);
                        angleRad = ptStyle->angleRad;
                    }

                    // symbol instance offset - must scale this by [S_mm], and [S_a]
                    xformStyle.translate(sym->absOffset[0].evaluate(exec) * mm2px * xformW2S.x0,
                                         sym->absOffset[1].evaluate(exec) * mm2px * xformW2S.x0);

                    break;
                }

                case SE_RenderLineStyleType:
                {
                    SE_RenderLineStyle* lnStyle = (SE_RenderLineStyle*)(rStyle);

                    // line usage rotation
                    if (wcscmp(L"FromAngle", lnStyle->angleControl) == 0)
                    {
                        xformStyle.rotate(lnStyle->angleRad);
                        angleRad = lnStyle->angleRad;
                    }

                    break;
                }

                case SE_RenderAreaStyleType:
                default:
                    break;
            }

            // assemble the final matrix and draw the style
            xformStyle.premultiply(xformW2S);
            se_renderer->DrawSymbol(style->rstyle->symbol, xformStyle, angleRad);
        }
    }

    //-------------------------------------------------------
    // step 4 - final clean up
    //-------------------------------------------------------

    for (std::vector<SE_Symbolization*>::iterator iter = styles.begin(); iter != styles.end(); iter++)
        delete *iter;

    styles.clear();
}


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


// Determine the maximum line width contained in the specified feature type style
double StylizationUtil::GetMaxMappingSpaceLineWidth(FeatureTypeStyle* fts, INT32 themeCategory)
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
                                double width = 0.0;
                                StylizationUtil::ParseDouble(edgeStroke->GetThickness(), width);
                                width = LengthConverter::UnitToMeters(edgeStroke->GetUnit(), width);
                                if (width > maxLineWidth)
                                    maxLineWidth = width;
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

                        for (int j=0; j<lsc->GetCount(); j++)
                        {
                            LineSymbolization2D* lsym = lsc->GetAt(j);
                            Stroke* stroke = lsym->GetStroke();
                            if (stroke && stroke->GetSizeContext() == MappingUnits)
                            {
                                double width = 0.0;
                                StylizationUtil::ParseDouble(stroke->GetThickness(), width);
                                width = LengthConverter::UnitToMeters(stroke->GetUnit(), width);
                                if (width > maxLineWidth)
                                    maxLineWidth = width;
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
