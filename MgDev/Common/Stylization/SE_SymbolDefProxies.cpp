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

#include "stdafx.h"
#include "SE_SymbolDefProxies.h"
#include "SE_Renderer.h"
#include "SE_Bounds.h"
#include "SE_SymbolManager.h"
#include "RS_FontEngine.h"


///////////////////////////////////////////////////////////////////////////////
// assumes axis aligned bounds stored in src and dst (with y pointing up),
// and the order of the points is CCW starting at the bottom left
static void BoundsUnion(RS_F_Point* dst, RS_F_Point* src)
{
    dst[0].x = dst[3].x = rs_min(dst[0].x, src[0].x);
    dst[1].x = dst[2].x = rs_max(dst[2].x, src[2].x);
    dst[0].y = dst[1].y = rs_min(dst[0].y, src[0].y);
    dst[2].y = dst[3].y = rs_max(dst[2].y, src[2].y);
}


///////////////////////////////////////////////////////////////////////////////
// Compute a margin that we'll add to the primitive's bounds to account
// for line weight.  This is only an approximation...
static double GetHalfWeightMargin(const SE_LineStroke& lineStroke)
{
    // default half-weight margin
    double margin = 0.5 * lineStroke.weight;
/*
    // We could also take into account the cap and join properties,
    // but for now we leave this out.  The bounds are used with the
    // line wrapping code and labels, but they are not required to be
    // 100% accurate.
    if (lineStroke.weight > 0.0)
    {
        // a mitered line can extend out miter_limit times the weight
        if (lineStroke.join == SE_LineJoin_Miter)
            margin = rs_max(margin, lineStroke.miterLimit * lineStroke.weight);

        // for square caps data can extend out weight / sqrt(2)
        if (lineStroke.cap == SE_LineCap_Square)
            margin = rs_max(margin, 0.707 * lineStroke.weight);
    }
*/
    return margin;
}


///////////////////////////////////////////////////////////////////////////////
SE_Style::~SE_Style()
{
    for (SE_PrimitiveList::iterator iter = symbol.begin(); iter != symbol.end(); ++iter)
        delete *iter;

    delete rstyle;
}


///////////////////////////////////////////////////////////////////////////////
static double CalcTolerance(SE_EvalContext* ctx)
{
    // Desired tolerance is 0.25 pixels at typical screen resolution (96 dpi).
    // This equates to 0.066145833 mm, which can be used to calculate a screen
    // unit tolerance for devices of varying resolutions - specifically
    // printers - which typically have resolutions of 600 dpi and up.
    //
    // The rationale here is that there is little visual benefit to using a
    // tolerance much smaller than the value used for the screen device.  E.g.
    // tesselating symbols to a tolerance of 0.01 mm on a 600 dpi device will
    // not offer any graphical improvements detectable by eye, yet it can slow
    // down rendering significantly.
    return ctx->mm2sud * 0.066145833;
}


///////////////////////////////////////////////////////////////////////////////
SE_RenderPrimitive* SE_Polyline::evaluate(SE_EvalContext* ctx)
{
    // simple check for empty geometry
    if (geometry->Empty())
        return NULL;

    SE_RenderPolyline* ret = new SE_RenderPolyline();

    const wchar_t* sResizeCtrl = resizeControl.evaluate(ctx->exec);
    if (wcscmp(sResizeCtrl, L"AddToResizeBox") == 0)
        ret->resizeControl = SE_ResizeControl_AddToResizeBox;
    else if (wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0)
        ret->resizeControl = SE_ResizeControl_AdjustToResizeBox;
    else // default is ResizeNone
        ret->resizeControl = SE_ResizeControl_ResizeNone;

    ret->geometry = geometry->Clone();

    double wx                  = weightScalable.evaluate(ctx->exec)? fabs(ctx->xform->x0) : ctx->mm2su;
    ret->lineStroke.weight     = weight.evaluate(ctx->exec) * wx;
    ret->lineStroke.color      = color.evaluate(ctx->exec);
    ret->lineStroke.miterLimit = miterLimit.evaluate(ctx->exec);
    if (ret->lineStroke.miterLimit < 0.0)
        ret->lineStroke.miterLimit = 0.0;

    // restrict the weight to something reasonable
    double devWeightInMM = ret->lineStroke.weight / ctx->mm2sud;
    if (devWeightInMM > MAX_LINEWEIGHT_IN_MM)
        ret->lineStroke.weight = MAX_LINEWEIGHT_IN_MM * ctx->mm2sud;
    else if (devWeightInMM < 0.0)
        ret->lineStroke.weight = 0.0;

    const wchar_t* sCap = cap.evaluate(ctx->exec);
    if (wcscmp(sCap, L"Round") == 0)    // check this first since it's the most common
        ret->lineStroke.cap = SE_LineCap_Round;
    else if (wcscmp(sCap, L"None") == 0)
        ret->lineStroke.cap = SE_LineCap_None;
    else if (wcscmp(sCap, L"Square") == 0)
        ret->lineStroke.cap = SE_LineCap_Square;
    else if (wcscmp(sCap, L"Triangle") == 0)
        ret->lineStroke.cap = SE_LineCap_Triangle;
    else // default is Round
        ret->lineStroke.cap = SE_LineCap_Round;

    const wchar_t* sJoin = join.evaluate(ctx->exec);
    if (wcscmp(sJoin, L"Round") == 0)   // check this first since it's the most common
        ret->lineStroke.join = SE_LineJoin_Round;
    else if (wcscmp(sJoin, L"None") == 0)
        ret->lineStroke.join = SE_LineJoin_None;
    else if (wcscmp(sJoin, L"Bevel") == 0)
    {
        ret->lineStroke.join = SE_LineJoin_Bevel;
        ret->lineStroke.miterLimit = 0.0;
    }
    else if (wcscmp(sJoin, L"Miter") == 0)
        ret->lineStroke.join = SE_LineJoin_Miter;
    else // default is Round
        ret->lineStroke.join = SE_LineJoin_Round;

    ret->scaleX = scaleX.evaluate(ctx->exec);
    ret->scaleY = scaleY.evaluate(ctx->exec);

    // populate the line buffer
    if (ret->scaleX != 1.0 || ret->scaleY != 1.0)
    {
        // combine the path scaling with the style transform
        SE_Matrix xform(ret->scaleX, 0.0, 0.0, 0.0, ret->scaleY, 0.0);
        xform.premultiply(*ctx->xform);
        ret->geometry->Transform(xform, CalcTolerance(ctx));
    }
    else
    {
        ret->geometry->Transform(*ctx->xform, CalcTolerance(ctx));
    }
    
    // If the line buffer contains dots (zero-length segments) then replace them
    // with very short horizontal lines.  When the symbol gets applied to the
    // geometry these segments will then have the correct orientation, and this
    // allows them to be nicely renderered.
    LineBuffer* lb = ret->geometry->xf_buffer();
    int numContours = lb->cntr_count();
    for (int i=0; i<numContours; ++i)
    {
        if (lb->cntr_size(i) == 2)
        {
            int j0 = lb->contour_start_point(i);
            int j1 = j0 + 1;

            double& sx = lb->x_coord(j0);
            double& sy = lb->y_coord(j0);
            double& ex = lb->x_coord(j1);
            double& ey = lb->y_coord(j1);

            if (sx == ex && sy == ey)
            {
                sx -= 0.5*LINE_SEGMENT_DOT_SIZE;
                ex += 0.5*LINE_SEGMENT_DOT_SIZE;
            }
        }
    }

    // TODO: here we would implement a rotating calipers algorithm to get a tighter
    //       oriented box, but for now just get the axis-aligned bounds of the path
    SE_Bounds* seb = ret->geometry->xf_bounds();
    if (seb == NULL)
    {
        // we failed for some reason - return NULL
        delete ret;
        return NULL;
    }

    // compute half-weight margin that we'll add to the primitive's bounds
    double margin = GetHalfWeightMargin(ret->lineStroke);

    ret->bounds[0].x = seb->min[0] - margin;
    ret->bounds[0].y = seb->min[1] - margin;
    ret->bounds[1].x = seb->max[0] + margin;
    ret->bounds[1].y = seb->min[1] - margin;
    ret->bounds[2].x = seb->max[0] + margin;
    ret->bounds[2].y = seb->max[1] + margin;
    ret->bounds[3].x = seb->min[0] - margin;
    ret->bounds[3].y = seb->max[1] + margin;

    return ret;
}


///////////////////////////////////////////////////////////////////////////////
SE_RenderPrimitive* SE_Polygon::evaluate(SE_EvalContext* ctx)
{
    // simple check for empty geometry
    if (geometry->Empty())
        return NULL;

    SE_RenderPolygon* ret = new SE_RenderPolygon();

    const wchar_t* sResizeCtrl = resizeControl.evaluate(ctx->exec);
    if (wcscmp(sResizeCtrl, L"AddToResizeBox") == 0)
        ret->resizeControl = SE_ResizeControl_AddToResizeBox;
    else if (wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0)
        ret->resizeControl = SE_ResizeControl_AdjustToResizeBox;
    else // default is ResizeNone
        ret->resizeControl = SE_ResizeControl_ResizeNone;

    ret->geometry = geometry->Clone();
    ret->fill     = fill.evaluate(ctx->exec);

    double wx                  = weightScalable.evaluate(ctx->exec)? fabs(ctx->xform->x0) : ctx->mm2su;
    ret->lineStroke.weight     = weight.evaluate(ctx->exec) * wx;
    ret->lineStroke.color      = color.evaluate(ctx->exec);
    ret->lineStroke.miterLimit = miterLimit.evaluate(ctx->exec);
    if (ret->lineStroke.miterLimit < 0.0)
        ret->lineStroke.miterLimit = 0.0;

    // restrict the weight to something reasonable
    double devWeightInMM = ret->lineStroke.weight / ctx->mm2sud;
    if (devWeightInMM > MAX_LINEWEIGHT_IN_MM)
        ret->lineStroke.weight = MAX_LINEWEIGHT_IN_MM * ctx->mm2sud;
    else if (devWeightInMM < 0.0)
        ret->lineStroke.weight = 0.0;

    const wchar_t* sCap = cap.evaluate(ctx->exec);
    if (wcscmp(sCap, L"Round") == 0)    // check this first since it's the most common
        ret->lineStroke.cap = SE_LineCap_Round;
    else if (wcscmp(sCap, L"None") == 0)
        ret->lineStroke.cap = SE_LineCap_None;
    else if (wcscmp(sCap, L"Square") == 0)
        ret->lineStroke.cap = SE_LineCap_Square;
    else if (wcscmp(sCap, L"Triangle") == 0)
        ret->lineStroke.cap = SE_LineCap_Triangle;
    else // default is Round
        ret->lineStroke.cap = SE_LineCap_Round;

    const wchar_t* sJoin = join.evaluate(ctx->exec);
    if (wcscmp(sJoin, L"Round") == 0)   // check this first since it's the most common
        ret->lineStroke.join = SE_LineJoin_Round;
    else if (wcscmp(sJoin, L"None") == 0)
        ret->lineStroke.join = SE_LineJoin_None;
    else if (wcscmp(sJoin, L"Bevel") == 0)
    {
        ret->lineStroke.join = SE_LineJoin_Bevel;
        ret->lineStroke.miterLimit = 0.0;
    }
    else if (wcscmp(sJoin, L"Miter") == 0)
        ret->lineStroke.join = SE_LineJoin_Miter;
    else // default is Round
        ret->lineStroke.join = SE_LineJoin_Round;

    ret->scaleX = scaleX.evaluate(ctx->exec);
    ret->scaleY = scaleY.evaluate(ctx->exec);

    // populate the line buffer
    if (ret->scaleX != 1.0 || ret->scaleY != 1.0)
    {
        // combine the path scaling with the style transform
        SE_Matrix xform(ret->scaleX, 0.0, 0.0, 0.0, ret->scaleY, 0.0);
        xform.premultiply(*ctx->xform);
        ret->geometry->Transform(xform, CalcTolerance(ctx));
    }
    else
    {
        ret->geometry->Transform(*ctx->xform, CalcTolerance(ctx));
    }

    // TODO: here we would implement a rotating calipers algorithm to get a tighter
    //       oriented box, but for now just get the axis-aligned bounds of the path
    SE_Bounds* seb = ret->geometry->xf_bounds();
    if (seb == NULL)
    {
        // we failed for some reason - return NULL
        delete ret;
        return NULL;
    }

    // compute half-weight margin that we'll add to the primitive's bounds
    double margin = GetHalfWeightMargin(ret->lineStroke);

    ret->bounds[0].x = seb->min[0] - margin;
    ret->bounds[0].y = seb->min[1] - margin;
    ret->bounds[1].x = seb->max[0] + margin;
    ret->bounds[1].y = seb->min[1] - margin;
    ret->bounds[2].x = seb->max[0] + margin;
    ret->bounds[2].y = seb->max[1] + margin;
    ret->bounds[3].x = seb->min[0] - margin;
    ret->bounds[3].y = seb->max[1] + margin;

    return ret;
}


///////////////////////////////////////////////////////////////////////////////
SE_RenderPrimitive* SE_Text::evaluate(SE_EvalContext* ctx)
{
    if (ctx->fonte == NULL)
        return NULL;

    // don't bother creating a primitive if there's no content
    const wchar_t* contentStr = content.evaluate(ctx->exec);
    if (wcslen(contentStr) == 0)
        return NULL;

    SE_RenderText* ret = new SE_RenderText();

    const wchar_t* sResizeCtrl = resizeControl.evaluate(ctx->exec);
    if (wcscmp(sResizeCtrl, L"AddToResizeBox") == 0)
        ret->resizeControl = SE_ResizeControl_AddToResizeBox;
    else if (wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0)
        ret->resizeControl = SE_ResizeControl_AdjustToResizeBox;
    else // default is ResizeNone
        ret->resizeControl = SE_ResizeControl_ResizeNone;
    if(NULL != content.expression)
        ret->expression  = content.expression->ToString();
    ret->content     = contentStr;
    ret->position[0] = position[0].evaluate(ctx->exec);
    ret->position[1] = position[1].evaluate(ctx->exec);

    ctx->xform->transform(ret->position[0], ret->position[1]);

    RS_TextDef& textDef = ret->tdef;
    RS_FontDef& fontDef = textDef.font();

    textDef.rotation() = fmod(angleDeg.evaluate(ctx->exec), 360.0);   // in degrees

    int style = RS_FontStyle_Regular;
    if (bold.evaluate(ctx->exec))
        style |= RS_FontStyle_Bold;
    if (italic.evaluate(ctx->exec))
        style |= RS_FontStyle_Italic;
    if (underlined.evaluate(ctx->exec))
        style |= RS_FontStyle_Underline;
    if (overlined.evaluate(ctx->exec))
        style |= RS_FontStyle_Overline;

    fontDef.style() = (RS_FontStyle_Mask)style;
    fontDef.name()  = fontName.evaluate(ctx->exec);

    // RS_TextDef expects font height to be in meters - convert it from mm
    double wy              = heightScalable.evaluate(ctx->exec)? 0.001 * fabs(ctx->xform->y1) / ctx->mm2sud : 0.001;
    fontDef.height()       = height.evaluate(ctx->exec) * wy;
    textDef.obliqueAngle() = obliqueAngle.evaluate(ctx->exec);
    textDef.trackSpacing() = trackSpacing.evaluate(ctx->exec);
    textDef.linespace()    = lineSpacing.evaluate(ctx->exec);
    textDef.textcolor()    = RS_Color::FromARGB(textColor.evaluate(ctx->exec));
    textDef.markup()       = markup.evaluate(ctx->exec);

    if (!ghostColor.empty())
    {
        textDef.ghostcolor() = RS_Color::FromARGB(ghostColor.evaluate(ctx->exec));
        textDef.textbg() |= RS_TextBackground_Ghosted;
    }
    if (!frameLineColor.empty())
    {
        textDef.framecolor() = RS_Color::FromARGB(frameLineColor.evaluate(ctx->exec));
        textDef.textbg() |= RS_TextBackground_Framed;
    }
    if (!frameFillColor.empty())
    {
        textDef.opaquecolor() = RS_Color::FromARGB(frameFillColor.evaluate(ctx->exec));
        textDef.textbg() |= RS_TextBackground_Opaque;
    }
    textDef.frameoffsetx() = frameOffset[0].evaluate(ctx->exec) * fabs(ctx->xform->x0);
    textDef.frameoffsety() = frameOffset[1].evaluate(ctx->exec) * fabs(ctx->xform->y1);

    const wchar_t* hAlign = hAlignment.evaluate(ctx->exec);
    if (wcscmp(hAlign, L"Center") == 0)     // check this first since it's the most common
        textDef.halign() = RS_HAlignment_Center;
    else if (wcscmp(hAlign, L"Left") == 0)
        textDef.halign() = RS_HAlignment_Left;
    else if (wcscmp(hAlign, L"Right") == 0)
        textDef.halign() = RS_HAlignment_Right;
    else // default is Center
        textDef.halign() = RS_HAlignment_Center;

    const wchar_t* vAlign = vAlignment.evaluate(ctx->exec);
    if (wcscmp(vAlign, L"Halfline") == 0)   // check this first since it's the most common
        textDef.valign() = RS_VAlignment_Half;
    else if (wcscmp(vAlign, L"Bottom") == 0)
        textDef.valign() = RS_VAlignment_Descent;
    else if (wcscmp(vAlign, L"Baseline") == 0)
        textDef.valign() = RS_VAlignment_Base;
    else if (wcscmp(vAlign, L"Capline") == 0)
        textDef.valign() = RS_VAlignment_Cap;
    else if (wcscmp(vAlign, L"Top") == 0)
        textDef.valign() = RS_VAlignment_Ascent;
    else // default is Halfline
        textDef.valign() = RS_VAlignment_Half;

    const wchar_t* justification = this->justification.evaluate(ctx->exec);
    if (wcscmp(justification, L"FromAlignment") == 0)   // check this first since it's the most common
    {
        switch (textDef.halign())
        {
            case RS_HAlignment_Center:
                textDef.justify() = RS_Justify_Center;
                break;
            case RS_HAlignment_Left:
                textDef.justify() = RS_Justify_Left;
                break;
            case RS_HAlignment_Right:
                textDef.justify() = RS_Justify_Right;
                break;
        }
    }
    else if (wcscmp(justification, L"Left") == 0)
        textDef.justify() = RS_Justify_Left;
    else if (wcscmp(justification, L"Right") == 0)
        textDef.justify() = RS_Justify_Right;
    else if (wcscmp(justification, L"Center") == 0)
        textDef.justify() = RS_Justify_Center;
    else if (wcscmp(justification, L"Justified") == 0)
        textDef.justify() = RS_Justify_Justify;

    RS_TextMetrics& tm = ret->tm;
    if (!ctx->fonte->GetTextMetrics(ret->content, textDef, tm, false))  // mark this RS_TextMetrics as invalid
        tm.font = NULL;

    SE_Matrix txf;
    txf.rotate(textDef.rotation() * M_PI180);
    txf.translate(ret->position[0], ret->position[1]);

    // compute axis aligned bounds of the text primitive - these are
    // initialized to the text position to ensure we get valid bounds
    // for the case of an empty text string
    RS_F_Point fpts[4];
    RS_Bounds xformBounds(ret->position[0], ret->position[1], ret->position[0], ret->position[1]);

    for (size_t k=0; k<tm.line_pos.size(); ++k)
    {
        // convert the unrotated measured bounds for the current line to a local point array
        memcpy(fpts, tm.line_pos[k].ext, sizeof(fpts));

        // process the extent points
        for (int j=0; j<4; ++j)
        {
            txf.transform(fpts[j].x, fpts[j].y);

            // update the overall rotated bounds
            xformBounds.add_point(fpts[j]);
        }
    }

    // compute axis aligned bounds of the text primitive, including the frame offsets
    // we do it separately because the offsets could be negative
    double offx = textDef.frameoffsetx();
    double offy = textDef.frameoffsety();
    if (ctx->xform->y1 < 0.0)
        offy = -offy;

    if (offx != 0.0 || offy != 0.0)
    {
        for (size_t k=0; k<tm.line_pos.size(); ++k)
        {
            // convert the unrotated measured bounds for the current line to a local point array
            memcpy(fpts, tm.line_pos[k].ext, sizeof(fpts));

            // factor in the offset
            fpts[0].x -= offx;  fpts[0].y -= offy;
            fpts[1].x += offx;  fpts[1].y -= offy;
            fpts[2].x += offx;  fpts[2].y += offy;
            fpts[3].x -= offx;  fpts[3].y += offy;

            // process the extent points
            for (int j=0; j<4; ++j)
            {
                txf.transform(fpts[j].x, fpts[j].y);

                // update the overall rotated bounds
                xformBounds.add_point(fpts[j]);
            }
        }
    }

    // set the bounds into the returned primitive
    xformBounds.get_points(ret->bounds);

    return ret;
}


///////////////////////////////////////////////////////////////////////////////
SE_RenderPrimitive* SE_Raster::evaluate(SE_EvalContext* ctx)
{
    SE_RenderRaster* ret = new SE_RenderRaster();

    const wchar_t* sResizeCtrl = resizeControl.evaluate(ctx->exec);
    if (wcscmp(sResizeCtrl, L"AddToResizeBox") == 0)
        ret->resizeControl = SE_ResizeControl_AddToResizeBox;
    else if (wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0)
        ret->resizeControl = SE_ResizeControl_AdjustToResizeBox;
    else // default is ResizeNone
        ret->resizeControl = SE_ResizeControl_ResizeNone;

    if (!imageData.data)
    {
        // if we have non-empty resource ID then use it, otherwise use
        // the ID of any parent symbol definition
        const wchar_t* resourceId = pngResourceId.evaluate(ctx->exec);
        if (wcslen(resourceId) == 0)
            resourceId = resId;
        ctx->resources->GetImageData(resourceId, pngResourceName.evaluate(ctx->exec), ret->imageData);
    }
    else
    {
        ret->imageData = imageData;
    }

    ret->position[0] = position[0].evaluate(ctx->exec);
    ret->position[1] = position[1].evaluate(ctx->exec);
    ctx->xform->transform(ret->position[0], ret->position[1]);

    if (sizeScalable.evaluate(ctx->exec))
    {
        ret->extent[0] = fabs(extent[0].evaluate(ctx->exec) * fabs(ctx->xform->x0));
        ret->extent[1] = fabs(extent[1].evaluate(ctx->exec) * fabs(ctx->xform->y1));
    }
    else
    {
        ret->extent[0] = fabs(extent[0].evaluate(ctx->exec) * ctx->mm2sud);
        ret->extent[1] = fabs(extent[1].evaluate(ctx->exec) * ctx->mm2sud);
    }

    ret->angleRad = fmod(angleDeg.evaluate(ctx->exec), 360.0) * M_PI180;

    // this one is just a raw double
    ret->opacity = opacity;

    SE_Matrix rxf;
    rxf.rotate(ret->angleRad);
    rxf.translate(ret->position[0], ret->position[1]);

    double w2 = 0.5*ret->extent[0];
    double h2 = 0.5*ret->extent[1];

    RS_F_Point pts[4];
    rxf.transform(-w2, -h2, pts[0].x, pts[0].y);
    rxf.transform( w2, -h2, pts[1].x, pts[1].y);
    rxf.transform( w2,  h2, pts[2].x, pts[2].y);
    rxf.transform(-w2,  h2, pts[3].x, pts[3].y);

    memcpy(ret->bounds, pts, sizeof(ret->bounds));

    return ret;
}


///////////////////////////////////////////////////////////////////////////////
void SE_Style::reset()
{
    delete rstyle;
    rstyle = NULL;
}


///////////////////////////////////////////////////////////////////////////////
void SE_Style::evaluate(SE_EvalContext* ctx)
{
    // evaluate values that are common to all styles
    rstyle->renderPass = renderPass.evaluate(ctx->exec);

    //
    // evaluation of all primitives and also resize box stuff
    //

    double minx0 = 0.0, maxx0 = 0.0, miny0 = 0.0, maxy0 = 0.0;
    double minx1 = 0.0, maxx1 = 0.0, miny1 = 0.0, maxy1 = 0.0;

    if (useBox)
    {
        // get the initial resize box
        double dx = resizePosition[0].evaluate(ctx->exec);
        double dy = resizePosition[1].evaluate(ctx->exec);
        double sx = 0.5*fabs(resizeSize[0].evaluate(ctx->exec));
        double sy = 0.5*fabs(resizeSize[1].evaluate(ctx->exec));

        double ptAx, ptAy, ptBx, ptBy;
        ctx->xform->transform(dx - sx, dy - sy, ptAx, ptAy);
        ctx->xform->transform(dx + sx, dy + sy, ptBx, ptBy);

        // the transform may invert points (e.g. in y), so therefore
        // we need to compute min & max
        minx0 = minx1 = rs_min(ptAx, ptBx);
        maxx0 = maxx1 = rs_max(ptAx, ptBx);
        miny0 = miny1 = rs_min(ptAy, ptBy);
        maxy0 = maxy1 = rs_max(ptAy, ptBy);
    }

    for (SE_PrimitiveList::const_iterator src = symbol.begin(); src != symbol.end(); ++src)
    {
        SE_Primitive* sym = *src;

        // evaluate the render primitive
        SE_RenderPrimitive* rsym = sym->evaluate(ctx);
        if (!rsym)
            continue;

        rstyle->symbol.push_back(rsym);

        // add the primitive bounds to the overall render style bounds
        if (!useBox || rsym->resizeControl != SE_ResizeControl_AdjustToResizeBox)
            BoundsUnion(rstyle->bounds, rsym->bounds);

        // add the primitive bounds to the resize box, if necessary
        if (useBox)
        {
            if (rsym->resizeControl == SE_ResizeControl_AddToResizeBox)
            {
                // the symbol bounds min/max should be properly set (e.g. max > min)
                minx1 = rs_min(minx1, rsym->bounds[0].x);
                maxx1 = rs_max(maxx1, rsym->bounds[2].x);
                miny1 = rs_min(miny1, rsym->bounds[0].y);
                maxy1 = rs_max(maxy1, rsym->bounds[2].y);
            }
        }
    }

    // update all primitives which need to adjust to the resize box
    if (useBox)
    {
        double sx0 = maxx0 - minx0;
        double sy0 = maxy0 - miny0;
        double scalex = 1.0;
        double scaley = 1.0;
        double transx0 = 0.0;
        double transy0 = 0.0;
        double transx1 = 0.0;
        double transy1 = 0.0;
        const wchar_t* sGrowCtrl = growControl.evaluate(ctx->exec);
        if (wcscmp(sGrowCtrl, L"GrowInX") == 0)
        {
            if (sx0 != 0.0)
            {
                scalex = (maxx1 - minx1) / sx0; // the ratio of their widths
                transx0 = 0.5*(minx0 + maxx0);
                transx1 = 0.5*(minx1 + maxx1);
            }
        }
        else if (wcscmp(sGrowCtrl, L"GrowInY") == 0)
        {
            if (sy0 != 0.0)
            {
                scaley = (maxy1 - miny1) / sy0; // the ratio of their heights
                transy0 = 0.5*(miny0 + maxy0);
                transy1 = 0.5*(miny1 + maxy1);
            }
        }
        else if (wcscmp(sGrowCtrl, L"GrowInXY") == 0)
        {
            if (sx0 != 0.0 && sy0 != 0.0)
            {
                scalex = (maxx1 - minx1) / sx0; // the ratio of their widths
                scaley = (maxy1 - miny1) / sy0; // the ratio of their heights
                transx0 = 0.5*(minx0 + maxx0);
                transy0 = 0.5*(miny0 + maxy0);
                transx1 = 0.5*(minx1 + maxx1);
                transy1 = 0.5*(miny1 + maxy1);
            }
        }
        else // default is GrowInXYMaintainAspect
        {
            if (sx0 != 0.0 && sy0 != 0.0)
            {
                scalex = (maxx1 - minx1) / sx0; // the ratio of their widths
                scaley = (maxy1 - miny1) / sy0; // the ratio of their heights
                if (scalex > scaley)
                    scaley = scalex;
                else
                    scalex = scaley;
                transx0 = 0.5*(minx0 + maxx0);
                transy0 = 0.5*(miny0 + maxy0);
                transx1 = 0.5*(minx1 + maxx1);
                transy1 = 0.5*(miny1 + maxy1);
            }
        }

        // build the grow transform
        SE_Matrix growxf;
        growxf.translate(-transx0, -transy0); // step 1 - translate the resize box to the origin
        growxf.scale(scalex, scaley);         // step 2 - scale the new box so it's the same size as the grown box
        growxf.translate(transx1, transy1);   // step 3 - translate the new box so it has the same center as the grown box

        // build the full transform
        SE_Matrix totalxf(*ctx->xform);
        totalxf.premultiply(growxf);

        for (SE_RenderPrimitiveList::iterator rs = rstyle->symbol.begin(); rs != rstyle->symbol.end(); ++rs)
        {
            SE_RenderPrimitive* rsym = *rs;
            if (rsym->resizeControl == SE_ResizeControl_AdjustToResizeBox)
            {
                switch (rsym->type)
                {
                case SE_RenderPrimitive_Polygon:
                case SE_RenderPrimitive_Polyline:
                    {
                        SE_RenderPolyline* rp = (SE_RenderPolyline*)rsym;

                        // populate the line buffer
                        if (rp->scaleX != 1.0 || rp->scaleY != 1.0)
                        {
                            // add the path scaling to the overall transform
                            SE_Matrix xform(rp->scaleX, 0.0, 0.0, 0.0, rp->scaleY, 0.0);
                            xform.premultiply(totalxf);
                            rp->geometry->Transform(xform, CalcTolerance(ctx));
                        }
                        else
                        {
                            rp->geometry->Transform(totalxf, CalcTolerance(ctx));
                        }

                        SE_Bounds* seb = rp->geometry->xf_bounds();
                        double margin = GetHalfWeightMargin(rp->lineStroke);
                        rp->bounds[0].x = seb->min[0] - margin;
                        rp->bounds[0].y = seb->min[1] - margin;
                        rp->bounds[1].x = seb->max[0] + margin;
                        rp->bounds[1].y = seb->min[1] - margin;
                        rp->bounds[2].x = seb->max[0] + margin;
                        rp->bounds[2].y = seb->max[1] + margin;
                        rp->bounds[3].x = seb->min[0] - margin;
                        rp->bounds[3].y = seb->max[1] + margin;
                        break;
                    }
                case SE_RenderPrimitive_Text:
                    {
                        SE_RenderText* rt = (SE_RenderText*)rsym;
                        growxf.transform(rt->position[0], rt->position[1]);
                        rt->tdef.font().height() *= scaley; // TODO: should this only be done if HeightScalable is true?
                        rt->tdef.frameoffsetx() *= scalex;  // TODO: should this only be done if HeightScalable is true?
                        rt->tdef.frameoffsety() *= scaley;  // TODO: should this only be done if HeightScalable is true?
                        for (int j=0; j<4; ++j)
                            growxf.transform(rt->bounds[j].x, rt->bounds[j].y);
                        break;
                    }
                case SE_RenderPrimitive_Raster:
                    {
                        SE_RenderRaster* rr = (SE_RenderRaster*)rsym;
                        growxf.transform(rr->position[0], rr->position[1]);
                        rr->extent[0] *= scalex;    // TODO: should this only be done if SizeScalable is true?
                        rr->extent[1] *= scaley;
                        for (int j=0; j<4; ++j)
                            growxf.transform(rr->bounds[j].x, rr->bounds[j].y);
                        break;
                    }
                }

                BoundsUnion(rstyle->bounds, rsym->bounds);
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void SE_PointStyle::evaluate(SE_EvalContext* ctx)
{
    // can skip out of evaluation if style is constant and has been evaluated once
    if (cacheable && rstyle)
        return;

    SE_RenderPointStyle* style = new SE_RenderPointStyle();
    delete rstyle;
    rstyle = style;

    const wchar_t* sAngleControl = angleControl.evaluate(ctx->exec);
    if (wcscmp(sAngleControl, L"FromGeometry") == 0)
        style->angleControl = SE_AngleControl_FromGeometry;
    else // default is FromAngle
        style->angleControl = SE_AngleControl_FromAngle;

    style->angleRad = fmod(angleDeg.evaluate(ctx->exec), 360.0) * M_PI180;

    // scale by xform->x0 and xform->y1 instead of mm2su, because these encompass
    // mm2su as well as scaleX and scaleY
    style->offset[0] = originOffset[0].evaluate(ctx->exec) * ctx->xform->x0;
    style->offset[1] = originOffset[1].evaluate(ctx->exec) * ctx->xform->y1;

    // evaluate all the primitives too
    SE_Style::evaluate(ctx);
}


///////////////////////////////////////////////////////////////////////////////
void SE_LineStyle::evaluate(SE_EvalContext* ctx)
{
    // can skip out of evaluation if style is constant and has been evaluated once
    if (cacheable && rstyle)
        return;

    SE_RenderLineStyle* style = new SE_RenderLineStyle();
    delete rstyle;
    rstyle = style;

    const wchar_t* sAngleControl = angleControl.evaluate(ctx->exec);
    if (wcscmp(sAngleControl, L"FromAngle") == 0)
        style->angleControl = SE_AngleControl_FromAngle;
    else // default is FromGeometry
        style->angleControl = SE_AngleControl_FromGeometry;

    const wchar_t* sUnitsControl = unitsControl.evaluate(ctx->exec);
    if (wcscmp(sUnitsControl, L"Parametric") == 0)
        style->unitsControl = SE_UnitsControl_Parametric;
    else // default is Absolute
        style->unitsControl = SE_UnitsControl_Absolute;

    const wchar_t* sVertexControl = vertexControl.evaluate(ctx->exec);
    if (wcscmp(sVertexControl, L"OverlapNone") == 0)
        style->vertexControl = SE_VertexControl_OverlapNone;
    else if (wcscmp(sVertexControl, L"OverlapDirect") == 0)
        style->vertexControl = SE_VertexControl_OverlapDirect;
    else if (wcscmp(sVertexControl, L"OverlapNoWrap") == 0)
        // this deprecated option is treated as OverlapNone
        style->vertexControl = SE_VertexControl_OverlapNone;
    else // default is OverlapWrap
        style->vertexControl = SE_VertexControl_OverlapWrap;

    style->angleRad = fmod(angleDeg.evaluate(ctx->exec), 360.0) * M_PI180;

    style->startOffset = startOffset.evaluate(ctx->exec);
    style->endOffset   = endOffset.evaluate(ctx->exec);
    style->repeat      = repeat.evaluate(ctx->exec);

    // with parametric units control the repeat / offsets will be scaled
    // later on by each contour length, which includes the transform
    double origRepeat = -1.0;
    if (style->unitsControl == SE_UnitsControl_Absolute)
    {
        // scale by xform->x0 and xform->y1 instead of mm2su, because
        // these include mm2su as well as scaleX and scaleY
        style->startOffset *= fabs(ctx->xform->x0);
        style->endOffset   *= fabs(ctx->xform->x0);
        style->repeat      *= fabs(ctx->xform->x0);

        origRepeat = style->repeat;

        // It makes no sense to distribute symbols using a repeat value
        // which is much less than one pixel.  We'll scale up any value
        // less than 0.25 to 0.5.
        if (style->repeat > 0.0 && style->repeat < 0.25*ctx->px2su)
        {
            // just increase it by an integer multiple so the overall
            // distribution isn't affected
            int factor = (int)(0.5*ctx->px2su / style->repeat);
            style->repeat *= factor;
        }
    }

    double angleLimit = vertexAngleLimit.evaluate(ctx->exec);
    if (angleLimit < 0.0)
        angleLimit = 0.0;
    style->vertexAngleLimit = fmod(angleLimit, 360.0) * M_PI180;
    style->vertexMiterLimit = vertexMiterLimit.evaluate(ctx->exec);
    if (style->vertexMiterLimit < 0.0)
        style->vertexMiterLimit = 0.0;

    const wchar_t* sJoin = vertexJoin.evaluate(ctx->exec);
    if (wcscmp(sJoin, L"Round") == 0)       // check this first since it's the most common
        style->vertexJoin = SE_LineJoin_Round;
    else if (wcscmp(sJoin, L"None") == 0)
        style->vertexJoin = SE_LineJoin_None;
    else if (wcscmp(sJoin, L"Bevel") == 0)
    {
        style->vertexJoin = SE_LineJoin_Bevel;
        style->vertexMiterLimit = 0.0;
    }
    else if (wcscmp(sJoin, L"Miter") == 0)
        style->vertexJoin = SE_LineJoin_Miter;
    else // default is Round
        style->vertexJoin = SE_LineJoin_Round;

    double wx                      = dpWeightScalable.evaluate(ctx->exec)? fabs(ctx->xform->x0) : ctx->mm2su;
    style->dpLineStroke.weight     = dpWeight.evaluate(ctx->exec) * wx;
    style->dpLineStroke.color      = dpColor.evaluate(ctx->exec);
    style->dpLineStroke.miterLimit = dpMiterLimit.evaluate(ctx->exec);
    if (style->dpLineStroke.miterLimit < 0.0)
        style->dpLineStroke.miterLimit = 0.0;

    // restrict the weight to something reasonable
    double devWeightInMM = style->dpLineStroke.weight / ctx->mm2sud;
    if (devWeightInMM > MAX_LINEWEIGHT_IN_MM)
        style->dpLineStroke.weight = MAX_LINEWEIGHT_IN_MM * ctx->mm2sud;
    else if (devWeightInMM < 0.0)
        style->dpLineStroke.weight = 0.0;

    const wchar_t* sdpCap = dpCap.evaluate(ctx->exec);
    if (wcscmp(sdpCap, L"Round") == 0)      // check this first since it's the most common
        style->dpLineStroke.cap = SE_LineCap_Round;
    else if (wcscmp(sdpCap, L"None") == 0)
        style->dpLineStroke.cap = SE_LineCap_None;
    else if (wcscmp(sdpCap, L"Square") == 0)
        style->dpLineStroke.cap = SE_LineCap_Square;
    else if (wcscmp(sdpCap, L"Triangle") == 0)
        style->dpLineStroke.cap = SE_LineCap_Triangle;
    else // default is Round
        style->dpLineStroke.cap = SE_LineCap_Round;

    const wchar_t* sdpJoin = dpJoin.evaluate(ctx->exec);
    if (wcscmp(sdpJoin, L"Round") == 0)     // check this first since it's the most common
        style->dpLineStroke.join = SE_LineJoin_Round;
    else if (wcscmp(sdpJoin, L"None") == 0)
        style->dpLineStroke.join = SE_LineJoin_None;
    else if (wcscmp(sdpJoin, L"Bevel") == 0)
    {
        style->dpLineStroke.join = SE_LineJoin_Bevel;
        style->dpLineStroke.miterLimit = 0.0;
    }
    else if (wcscmp(sdpJoin, L"Miter") == 0)
        style->dpLineStroke.join = SE_LineJoin_Miter;
    else // default is Round
        style->dpLineStroke.join = SE_LineJoin_Round;

    // evaluate all the primitives too
    SE_Style::evaluate(ctx);

    // special code to check if we have a simple straight solid line style
    style->solidLine = false;
    if (origRepeat > 0.0)
    {
        SE_RenderPrimitiveList& rplist = style->symbol;

        // check if it is a single symbol that is not a label participant
        if (rplist.size() == 1
            && rplist[0]->type == SE_RenderPrimitive_Polyline
            && !style->drawLast
            && !style->addToExclusionRegion)
        {
            SE_RenderPolyline* rp = (SE_RenderPolyline*)rplist[0];
            LineBuffer* lb = rp->geometry->xf_buffer();

            // check if it is a horizontal line
            if (lb->point_count() == 2
                && lb->y_coord(0) == 0.0
                && lb->y_coord(1) == 0.0)
            {
                // now make sure it is not a dashed line by comparing the
                // single segment to the symbol repeat (the unmodified one)
                double len = lb->x_coord(1) - lb->x_coord(0);

                // repeat must be within 0.1 pixels for us to assume solid line (this is
                // only to avoid FP precision issues, in reality they would be exactly equal)
                if (fabs(len - origRepeat) < 0.1*ctx->px2su)
                    style->solidLine = true;
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void SE_AreaStyle::evaluate(SE_EvalContext* ctx)
{
    // can skip out of evaluation if style is constant and has been evaluated once
    if (cacheable && rstyle)
        return;

    SE_RenderAreaStyle* style = new SE_RenderAreaStyle();
    delete rstyle;
    rstyle = style;

    const wchar_t* sAngleControl = angleControl.evaluate(ctx->exec);
    if (wcscmp(sAngleControl, L"FromGeometry") == 0)
        style->angleControl = SE_AngleControl_FromGeometry;
    else // default is FromAngle
        style->angleControl = SE_AngleControl_FromAngle;

    const wchar_t* sOriginControl = originControl.evaluate(ctx->exec);
    if (wcscmp(sOriginControl, L"Centroid") == 0)
        style->originControl = SE_OriginControl_Centroid;
    else if (wcscmp(sOriginControl, L"Local") == 0)
        style->originControl = SE_OriginControl_Local;
    else // default is Global
        style->originControl = SE_OriginControl_Global;

    const wchar_t* sClippingControl = clippingControl.evaluate(ctx->exec);
    if (wcscmp(sClippingControl, L"Inside") == 0)
        style->clippingControl = SE_ClippingControl_Inside;
    else if (wcscmp(sClippingControl, L"Overlap") == 0)
        style->clippingControl = SE_ClippingControl_Overlap;
    else // default is Clip
        style->clippingControl = SE_ClippingControl_Clip;

    style->angleRad = fmod(angleDeg.evaluate(ctx->exec), 360.0) * M_PI180;

    // scale by xform->x0 and xform->y1 instead of mm2su, because these encompass
    // mm2su as well as scaleX and scaleY
    style->origin[0] = origin[0].evaluate(ctx->exec);
    style->origin[1] = origin[1].evaluate(ctx->exec);
    if (style->originControl != SE_OriginControl_Local)
    {
        // For non-local origin control the origin is in millimeters and must
        // be scaled.
        style->origin[0] *= ctx->xform->x0;
        style->origin[1] *= ctx->xform->x0;
    }
    else
    {
        // For local origin control the origin is a parametric value which
        // shouldn't be scaled.  In the case of the renderer Y pointing down
        // we need to invert the y origin parametric value.
        if (ctx->xform->y1 < 0.0)
            style->origin[1] = 1.0 - style->origin[1];
    }
    style->repeat[0]   = repeat[0].evaluate(ctx->exec)   * fabs(ctx->xform->x0);
    style->repeat[1]   = repeat[1].evaluate(ctx->exec)   * fabs(ctx->xform->y1);
    style->bufferWidth = bufferWidth.evaluate(ctx->exec) * fabs(ctx->xform->x0);
    double origRepeatX = style->repeat[0];
    double origRepeatY = style->repeat[1];

    // It makes no sense to distribute symbols using repeat values which
    // are much less than one pixel.  We'll scale up any values less than
    // 0.25 to 0.5.
    for (int i=0; i<=1; ++i)
    {
        // work with absolute value in case repeat is negative
        double repeat = fabs(style->repeat[i]);
        if (repeat > 0.0 && repeat < 0.25*ctx->px2su)
        {
            // just increase it by an integer multiple so the overall
            // distribution isn't affected
            int factor = (int)(0.5*ctx->px2su / repeat);
            style->repeat[i] *= factor;
        }
    }

    // evaluate all the primitives too
    SE_Style::evaluate(ctx);

    // special code to check if we have a simple solid fill
    style->solidFill = false;
    if (origRepeatX > 0.0 && origRepeatY > 0.0)
    {
        SE_RenderPrimitiveList& rplist = style->symbol;

        // check if it is a single symbol that is not a label participant
        if (rplist.size() == 1
            && rplist[0]->type == SE_RenderPrimitive_Polygon
            && !style->drawLast
            && !style->addToExclusionRegion)
        {
            SE_RenderPolygon* rp = (SE_RenderPolygon*)rplist[0];
            LineBuffer* lb = rp->geometry->xf_buffer();

            // make sure it has no edge style
            if ((rp->lineStroke.color & 0xFF000000) == 0)
            {
                // check if it is a rectangle
                if (lb->cntr_count() == 1 &&
                    lb->point_count() == 5 &&
                    lb->contour_closed(0))
                {
                    // case 1: first segment is horizontal
                    if (lb->y_coord(1) == lb->y_coord(0) &&
                        lb->x_coord(2) == lb->x_coord(1) &&
                        lb->y_coord(3) == lb->y_coord(2) &&
                        lb->x_coord(4) == lb->x_coord(3))
                    {
                        // now make sure the rectangle fills the repeat region
                        double lenX = lb->x_coord(1) - lb->x_coord(0);
                        double lenY = lb->y_coord(2) - lb->y_coord(1);

                        // repeats must be within 0.1 pixels for us to assume solid fill (this is
                        // only to avoid FP precision issues, in reality they would be exactly equal)
                        if (fabs(lenX - origRepeatX) < 0.1*ctx->px2su && fabs(lenY - origRepeatY) < 0.1*ctx->px2su)
                            style->solidFill = true;
                    }
                    // case 2: first segment is vertical
                    else if (lb->x_coord(1) == lb->x_coord(0) &&
                             lb->y_coord(2) == lb->y_coord(1) &&
                             lb->x_coord(3) == lb->x_coord(2) &&
                             lb->y_coord(4) == lb->y_coord(3))
                    {
                        // now make sure the rectangle fills the repeat region
                        double lenY = lb->y_coord(1) - lb->y_coord(0);
                        double lenX = lb->x_coord(2) - lb->x_coord(1);

                        // repeats must be within 0.1 pixels for us to assume solid fill (this is
                        // only to avoid FP precision issues, in reality they would be exactly equal)
                        if (fabs(lenX - origRepeatX) < 0.1*ctx->px2su && fabs(lenY - origRepeatY) < 0.1*ctx->px2su)
                            style->solidFill = true;
                    }
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void SE_PointStyle::apply(SE_ApplyContext* ctx)
{
    ctx->renderer->ProcessPoint(ctx, (SE_RenderPointStyle*)rstyle);
}


///////////////////////////////////////////////////////////////////////////////
void SE_LineStyle::apply(SE_ApplyContext* ctx)
{
    ctx->renderer->ProcessLine(ctx, (SE_RenderLineStyle*)rstyle);
}


///////////////////////////////////////////////////////////////////////////////
void SE_AreaStyle::apply(SE_ApplyContext* ctx)
{
    ctx->renderer->ProcessArea(ctx, (SE_RenderAreaStyle*)rstyle);
}
