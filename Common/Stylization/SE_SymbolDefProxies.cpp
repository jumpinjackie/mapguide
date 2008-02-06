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

#include "stdafx.h"
#include "SE_SymbolDefProxies.h"
#include "SE_Renderer.h"
#include "SE_Bounds.h"
#include "SE_SymbolManager.h"
#include "RS_FontEngine.h"


// assumes axis aligned bounds stored in src and dst (with y pointing up),
// and the order of the points is CCW starting at the bottom left
static void BoundsUnion(RS_F_Point* dst, RS_F_Point* src)
{
    dst[0].x = dst[3].x = rs_min(dst[0].x, src[0].x);
    dst[1].x = dst[2].x = rs_max(dst[2].x, src[2].x);
    dst[0].y = dst[1].y = rs_min(dst[0].y, src[0].y);
    dst[2].y = dst[3].y = rs_max(dst[2].y, src[2].y);
}


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



SE_Style::~SE_Style()
{
    for (SE_PrimitiveList::iterator iter = symbol.begin(); iter != symbol.end(); iter++)
        delete *iter;

    delete rstyle;
}


SE_RenderPrimitive* SE_Polyline::evaluate(SE_EvalContext* cxt)
{
    // simple check for empty geometry
    if (geometry->Empty())
        return NULL;

    SE_RenderPolyline* ret = new SE_RenderPolyline();

    const wchar_t* sResizeCtrl = resizeControl.evaluate(cxt->exec);
    if (wcscmp(sResizeCtrl, L"AddToResizeBox") == 0)
        ret->resizeControl = SE_RenderAddToResizeBox;
    else if (wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0)
        ret->resizeControl = SE_RenderAdjustToResizeBox;
    else // default is ResizeNone
        ret->resizeControl = SE_RenderResizeNone;

    ret->geometry = geometry->Clone();

    double wx                  = weightScalable.evaluate(cxt->exec)? fabs(cxt->xform->x0) : cxt->mm2pxs;
    ret->lineStroke.weight     = weight.evaluate(cxt->exec) * wx;
    ret->lineStroke.color      = color.evaluate(cxt->exec);
    ret->lineStroke.miterLimit = miterLimit.evaluate(cxt->exec);
    if (ret->lineStroke.miterLimit < 0.0)
        ret->lineStroke.miterLimit = 0.0;

    // restrict the weight to something reasonable
    double weightInMM = ret->lineStroke.weight / cxt->mm2pxs;
    if (weightInMM > MAX_LINEWEIGHT_IN_MM)
        ret->lineStroke.weight = MAX_LINEWEIGHT_IN_MM * cxt->mm2pxs;
    else if (weightInMM < 0.0)
        ret->lineStroke.weight = 0.0;

    const wchar_t* sCap = cap.evaluate(cxt->exec);
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

    const wchar_t* sJoin = join.evaluate(cxt->exec);
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

    ret->geometry->Transform(*cxt->xform, cxt->tolerance);

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


SE_RenderPrimitive* SE_Polygon::evaluate(SE_EvalContext* cxt)
{
    // simple check for empty geometry
    if (geometry->Empty())
        return NULL;

    SE_RenderPolygon* ret = new SE_RenderPolygon();

    const wchar_t* sResizeCtrl = resizeControl.evaluate(cxt->exec);
    if (wcscmp(sResizeCtrl, L"AddToResizeBox") == 0)
        ret->resizeControl = SE_RenderAddToResizeBox;
    else if (wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0)
        ret->resizeControl = SE_RenderAdjustToResizeBox;
    else // default is ResizeNone
        ret->resizeControl = SE_RenderResizeNone;

    ret->geometry   = geometry->Clone();
    ret->fill       = fill.evaluate(cxt->exec);

    double wx                  = weightScalable.evaluate(cxt->exec)? fabs(cxt->xform->x0) : cxt->mm2pxs;
    ret->lineStroke.weight     = weight.evaluate(cxt->exec) * wx;
    ret->lineStroke.color      = color.evaluate(cxt->exec);
    ret->lineStroke.miterLimit = miterLimit.evaluate(cxt->exec);
    if (ret->lineStroke.miterLimit < 0.0)
        ret->lineStroke.miterLimit = 0.0;

    // restrict the weight to something reasonable
    double weightInMM = ret->lineStroke.weight / cxt->mm2pxs;
    if (weightInMM > MAX_LINEWEIGHT_IN_MM)
        ret->lineStroke.weight = MAX_LINEWEIGHT_IN_MM * cxt->mm2pxs;
    else if (weightInMM < 0.0)
        ret->lineStroke.weight = 0.0;

    const wchar_t* sCap = cap.evaluate(cxt->exec);
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

    const wchar_t* sJoin = join.evaluate(cxt->exec);
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

    ret->geometry->Transform(*cxt->xform, cxt->tolerance);

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


SE_RenderPrimitive* SE_Text::evaluate(SE_EvalContext* cxt)
{
    if (cxt->fonte == NULL)
        return NULL;

    SE_RenderText* ret = new SE_RenderText();

    const wchar_t* sResizeCtrl = resizeControl.evaluate(cxt->exec);
    if (wcscmp(sResizeCtrl, L"AddToResizeBox") == 0)
        ret->resizeControl = SE_RenderAddToResizeBox;
    else if (wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0)
        ret->resizeControl = SE_RenderAdjustToResizeBox;
    else // default is ResizeNone
        ret->resizeControl = SE_RenderResizeNone;

    ret->content     = content.evaluate(cxt->exec);
    ret->position[0] = position[0].evaluate(cxt->exec);
    ret->position[1] = position[1].evaluate(cxt->exec);

    cxt->xform->transform(ret->position[0], ret->position[1]);

    RS_TextDef& textDef = ret->tdef;
    RS_FontDef& fontDef = textDef.font();

    textDef.rotation() = fmod(angleDeg.evaluate(cxt->exec), 360.0);   // in degrees

    int style = RS_FontStyle_Regular;
    if (bold.evaluate(cxt->exec))
        style |= RS_FontStyle_Bold;
    if (italic.evaluate(cxt->exec))
        style |= RS_FontStyle_Italic;
    if (underlined.evaluate(cxt->exec))
        style |= RS_FontStyle_Underline;
    if (overlined.evaluate(cxt->exec))
        style |= RS_FontStyle_Overline;

    fontDef.style() = (RS_FontStyle_Mask)style;
    fontDef.name()  = fontName.evaluate(cxt->exec);

    // RS_TextDef expects font height to be in meters - convert it from mm
    double wy              = heightScalable.evaluate(cxt->exec)? 0.001 * fabs(cxt->xform->y1) / cxt->mm2pxs : 0.001;
    fontDef.height()       = height.evaluate(cxt->exec) * wy;
    textDef.obliqueAngle() = obliqueAngle.evaluate(cxt->exec);
    textDef.trackSpacing() = trackSpacing.evaluate(cxt->exec);
    textDef.linespace()    = lineSpacing.evaluate(cxt->exec);
    textDef.textcolor()    = RS_Color::FromARGB(textColor.evaluate(cxt->exec));
    textDef.markup()       = markup.evaluate(cxt->exec);

    if (!ghostColor.empty())
    {
        textDef.ghostcolor() = RS_Color::FromARGB(ghostColor.evaluate(cxt->exec));
        textDef.textbg() |= RS_TextBackground_Ghosted;
    }
    if (!frameLineColor.empty())
    {
        textDef.framecolor() = RS_Color::FromARGB(frameLineColor.evaluate(cxt->exec));
        textDef.textbg() |= RS_TextBackground_Framed;
    }
    if (!frameFillColor.empty())
    {
        textDef.opaquecolor() = RS_Color::FromARGB(frameFillColor.evaluate(cxt->exec));
        textDef.textbg() |= RS_TextBackground_Opaque;
    }
    textDef.frameoffsetx() = frameOffset[0].evaluate(cxt->exec) * fabs(cxt->xform->x0);
    textDef.frameoffsety() = frameOffset[1].evaluate(cxt->exec) * fabs(cxt->xform->y1);

    const wchar_t* hAlign = hAlignment.evaluate(cxt->exec);
    if (wcscmp(hAlign, L"Center") == 0)     // check this first since it's the most common
        textDef.halign() = RS_HAlignment_Center;
    else if (wcscmp(hAlign, L"Left") == 0)
        textDef.halign() = RS_HAlignment_Left;
    else if (wcscmp(hAlign, L"Right") == 0)
        textDef.halign() = RS_HAlignment_Right;
    else // default is Center
        textDef.halign() = RS_HAlignment_Center;

    const wchar_t* vAlign = vAlignment.evaluate(cxt->exec);
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

    RS_TextMetrics tm;
    cxt->fonte->GetTextMetrics(ret->content, textDef, tm, false);

    SE_Matrix txf;
    txf.rotate(textDef.rotation() * M_PI180);
    txf.translate(ret->position[0], ret->position[1]);

    // compute axis aligned bounds of the text primitive
    RS_F_Point fpts[4];
    RS_Bounds xformBounds(+DBL_MAX, +DBL_MAX, -DBL_MAX, -DBL_MAX);

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
    if (cxt->xform->y1 < 0.0)
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


SE_RenderPrimitive* SE_Raster::evaluate(SE_EvalContext* cxt)
{
    SE_RenderRaster* ret = new SE_RenderRaster();

    const wchar_t* sResizeCtrl = resizeControl.evaluate(cxt->exec);
    if (wcscmp(sResizeCtrl, L"AddToResizeBox") == 0)
        ret->resizeControl = SE_RenderAddToResizeBox;
    else if (wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0)
        ret->resizeControl = SE_RenderAdjustToResizeBox;
    else // default is ResizeNone
        ret->resizeControl = SE_RenderResizeNone;

    if (!imageData.data)
    {
        // if we have non-empty resource ID then use it, otherwise use
        // the ID of any parent symbol definition
        const wchar_t* resourceId = pngResourceId.evaluate(cxt->exec);
        if (wcslen(resourceId) == 0)
            resourceId = resId;
        cxt->resources->GetImageData(resourceId, pngResourceName.evaluate(cxt->exec), ret->imageData);
    }
    else
    {
        ret->imageData = imageData;
    }

    ret->position[0] = position[0].evaluate(cxt->exec);
    ret->position[1] = position[1].evaluate(cxt->exec);
    cxt->xform->transform(ret->position[0], ret->position[1]);

    if (extentScalable.evaluate(cxt->exec))
    {
        ret->extent[0] = fabs(extent[0].evaluate(cxt->exec) * fabs(cxt->xform->x0));
        ret->extent[1] = fabs(extent[1].evaluate(cxt->exec) * fabs(cxt->xform->y1));
    }
    else
    {
        ret->extent[0] = fabs(extent[0].evaluate(cxt->exec) * cxt->mm2pxs);
        ret->extent[1] = fabs(extent[1].evaluate(cxt->exec) * cxt->mm2pxs);
    }

    ret->angleRad = fmod(angleDeg.evaluate(cxt->exec), 360.0) * M_PI180;

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


void SE_Style::evaluate(SE_EvalContext* cxt)
{
    // evaluate values that are common to all styles
    rstyle->renderPass = renderPass.evaluate(cxt->exec);

    //
    // evaluation of all primitives and also resize box stuff
    //

    double minx0 = 0.0, maxx0 = 0.0, miny0 = 0.0, maxy0 = 0.0;
    double minx1 = 0.0, maxx1 = 0.0, miny1 = 0.0, maxy1 = 0.0;

    if (useBox)
    {
        // get the initial resize box
        double dx = resizePosition[0].evaluate(cxt->exec);
        double dy = resizePosition[1].evaluate(cxt->exec);
        double sx = 0.5*fabs(resizeSize[0].evaluate(cxt->exec));
        double sy = 0.5*fabs(resizeSize[1].evaluate(cxt->exec));

        double ptAx, ptAy, ptBx, ptBy;
        cxt->xform->transform(dx - sx, dy - sy, ptAx, ptAy);
        cxt->xform->transform(dx + sx, dy + sy, ptBx, ptBy);

        // the transform may invert points (e.g. in y), so therefore
        // we need to compute min & max
        minx0 = minx1 = rs_min(ptAx, ptBx);
        maxx0 = maxx1 = rs_max(ptAx, ptBx);
        miny0 = miny1 = rs_min(ptAy, ptBy);
        maxy0 = maxy1 = rs_max(ptAy, ptBy);
    }

    for (SE_PrimitiveList::const_iterator src = symbol.begin(); src != symbol.end(); src++)
    {
        SE_Primitive* sym = *src;

        // evaluate the render primitive
        SE_RenderPrimitive* rsym = sym->evaluate(cxt);
        if (!rsym)
            continue;

        rstyle->symbol.push_back(rsym);

        // add the primitive bounds to the overall render style bounds
        if (!useBox || rsym->resizeControl != SE_RenderAdjustToResizeBox)
            BoundsUnion(rstyle->bounds, rsym->bounds);

        // add the primitive bounds to the resize box, if necessary
        if (useBox)
        {
            if (rsym->resizeControl == SE_RenderAddToResizeBox)
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
        const wchar_t* sGrowCtrl = growControl.evaluate(cxt->exec);
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
        SE_Matrix totalxf(*cxt->xform);
        totalxf.premultiply(growxf);

        for (SE_RenderPrimitiveList::iterator rs = rstyle->symbol.begin(); rs != rstyle->symbol.end(); rs++)
        {
            SE_RenderPrimitive* rsym = *rs;
            if (rsym->resizeControl == SE_RenderAdjustToResizeBox)
            {
                switch (rsym->type)
                {
                case SE_RenderPolygonPrimitive:
                case SE_RenderPolylinePrimitive:
                    {
                        SE_RenderPolyline* rp = (SE_RenderPolyline*)rsym;
                        rp->geometry->Transform(totalxf, cxt->tolerance);
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
                case SE_RenderTextPrimitive:
                    {
                        SE_RenderText* rt = (SE_RenderText*)rsym;
                        growxf.transform(rt->position[0], rt->position[1]);
                        rt->tdef.font().height() *= scaley; // TODO: should this only be done if HeightScalable is true?
                        rt->tdef.frameoffsetx() *= scalex;  // TODO: should this only be done if HeightScalable is true?
                        rt->tdef.frameoffsety() *= scaley;  // TODO: should this only be done if HeightScalable is true?
                        for (int j=0; j<4; j++)
                            growxf.transform(rt->bounds[j].x, rt->bounds[j].y);
                        break;
                    }
                case SE_RenderRasterPrimitive:
                    {
                        SE_RenderRaster* rr = (SE_RenderRaster*)rsym;
                        growxf.transform(rr->position[0], rr->position[1]);
                        rr->extent[0] *= scalex;    // TODO: should this only be done if ExtentScalable is true?
                        rr->extent[1] *= scaley;
                        for (int j=0; j<4; j++)
                            growxf.transform(rr->bounds[j].x, rr->bounds[j].y);
                        break;
                    }
                }

                BoundsUnion(rstyle->bounds, rsym->bounds);
            }
        }
    }
}


void SE_PointStyle::evaluate(SE_EvalContext* cxt)
{
    // can skip out of evaluation if style is constant and has been evaluated once
    if (cacheable && rstyle)
        return;

    SE_RenderPointStyle* render = new SE_RenderPointStyle();
    delete rstyle;
    rstyle = render;

    render->angleControl = angleControl.evaluate(cxt->exec);

    render->angleRad = fmod(angleDeg.evaluate(cxt->exec), 360.0) * M_PI180;

    // scale by xform->x0 and xform->y1 instead of mm2px, because these encompass
    // mm2px as well as scaleX and scaleY
    render->offset[0] = originOffset[0].evaluate(cxt->exec) * cxt->xform->x0;
    render->offset[1] = originOffset[1].evaluate(cxt->exec) * cxt->xform->y1;

    // evaluate all the primitives too
    SE_Style::evaluate(cxt);
}


void SE_LineStyle::evaluate(SE_EvalContext* cxt)
{
    // can skip out of evaluation if style is constant and has been evaluated once
    if (cacheable && rstyle)
        return;

    SE_RenderLineStyle* render = new SE_RenderLineStyle();
    delete rstyle;
    rstyle = render;

    render->angleControl  = angleControl.evaluate(cxt->exec);
    render->unitsControl  = unitsControl.evaluate(cxt->exec);
    render->vertexControl = vertexControl.evaluate(cxt->exec);

    render->angleRad = fmod(angleDeg.evaluate(cxt->exec), 360.0) * M_PI180;

    // scale by xform->x0 and xform->y1 instead of mm2px, because these encompass
    // mm2px as well as scaleX and scaleY
    render->startOffset = startOffset.evaluate(cxt->exec) * fabs(cxt->xform->x0);
    render->endOffset   = endOffset.evaluate(cxt->exec)   * fabs(cxt->xform->x0);
    render->repeat      = repeat.evaluate(cxt->exec)      * fabs(cxt->xform->x0);
    double origRepeat   = render->repeat;

    // It makes no sense to distribute symbols using a repeat value which is much
    // less than one pixel.  We'll scale up any value less than 0.25 to 0.5.
    if (render->repeat > 0.0 && render->repeat < 0.25)
    {
        // just increase it by an integer multiple so the overall distribution
        // isn't affected
        int factor = (int)(0.5 / render->repeat);
        render->repeat *= factor;
    }

    double angleLimit = vertexAngleLimit.evaluate(cxt->exec);
    if (angleLimit < 0.0)
        angleLimit = 0.0;
    render->vertexAngleLimit = fmod(angleLimit, 360.0) * M_PI180;
    render->vertexMiterLimit = vertexMiterLimit.evaluate(cxt->exec);
    if (render->vertexMiterLimit < 0.0)
        render->vertexMiterLimit = 0.0;

    const wchar_t* sJoin = vertexJoin.evaluate(cxt->exec);
    if (wcscmp(sJoin, L"Round") == 0)       // check this first since it's the most common
        render->vertexJoin = SE_LineJoin_Round;
    else if (wcscmp(sJoin, L"None") == 0)
        render->vertexJoin = SE_LineJoin_None;
    else if (wcscmp(sJoin, L"Bevel") == 0)
    {
        render->vertexJoin = SE_LineJoin_Bevel;
        render->vertexMiterLimit = 0.0;
    }
    else if (wcscmp(sJoin, L"Miter") == 0)
        render->vertexJoin = SE_LineJoin_Miter;
    else // default is Round
        render->vertexJoin = SE_LineJoin_Round;

    double wx                       = dpWeightScalable.evaluate(cxt->exec)? fabs(cxt->xform->x0) : cxt->mm2pxs;
    render->dpLineStroke.weight     = dpWeight.evaluate(cxt->exec) * wx;
    render->dpLineStroke.color      = dpColor.evaluate(cxt->exec);
    render->dpLineStroke.miterLimit = dpMiterLimit.evaluate(cxt->exec);
    if (render->dpLineStroke.miterLimit < 0.0)
        render->dpLineStroke.miterLimit = 0.0;

    // restrict the weight to something reasonable
    double weightInMM = render->dpLineStroke.weight / cxt->mm2pxs;
    if (weightInMM > MAX_LINEWEIGHT_IN_MM)
        render->dpLineStroke.weight = MAX_LINEWEIGHT_IN_MM * cxt->mm2pxs;
    else if (weightInMM < 0.0)
        render->dpLineStroke.weight = 0.0;

    const wchar_t* sdpCap = dpCap.evaluate(cxt->exec);
    if (wcscmp(sdpCap, L"Round") == 0)      // check this first since it's the most common
        render->dpLineStroke.cap = SE_LineCap_Round;
    else if (wcscmp(sdpCap, L"None") == 0)
        render->dpLineStroke.cap = SE_LineCap_None;
    else if (wcscmp(sdpCap, L"Square") == 0)
        render->dpLineStroke.cap = SE_LineCap_Square;
    else if (wcscmp(sdpCap, L"Triangle") == 0)
        render->dpLineStroke.cap = SE_LineCap_Triangle;
    else // default is Round
        render->dpLineStroke.cap = SE_LineCap_Round;

    const wchar_t* sdpJoin = dpJoin.evaluate(cxt->exec);
    if (wcscmp(sdpJoin, L"Round") == 0)     // check this first since it's the most common
        render->dpLineStroke.join = SE_LineJoin_Round;
    else if (wcscmp(sdpJoin, L"None") == 0)
        render->dpLineStroke.join = SE_LineJoin_None;
    else if (wcscmp(sdpJoin, L"Bevel") == 0)
    {
        render->dpLineStroke.join = SE_LineJoin_Bevel;
        render->dpLineStroke.miterLimit = 0.0;
    }
    else if (wcscmp(sdpJoin, L"Miter") == 0)
        render->dpLineStroke.join = SE_LineJoin_Miter;
    else // default is Round
        render->dpLineStroke.join = SE_LineJoin_Round;

    // evaluate all the primitives too
    SE_Style::evaluate(cxt);

    // special code to check if we have a simple straight solid line style
    render->solidLine = false;
    if (origRepeat > 0.0)
    {
        SE_RenderPrimitiveList& rs = render->symbol;

        // check if it is a single symbol that is not a label participant
        if (rs.size() == 1
            && rs[0]->type == SE_RenderPolylinePrimitive
            && !render->drawLast
            && !render->addToExclusionRegions)
        {
            SE_RenderPolyline* rp = (SE_RenderPolyline*)rs[0];
            LineBuffer* lb = rp->geometry->xf_buffer();

            // check if it is a horizontal line
            if (lb->point_count() == 2
                && lb->y_coord(0) == 0.0
                && lb->y_coord(1) == 0.0)
            {
                // now make sure it is not a dashed line by comparing the
                // single segment to the symbol repeat (the unmodified one)
                double len = lb->x_coord(1) - lb->x_coord(0);

                // repeat must be within 1/10 of a pixel for us to assume solid line (this is
                // only to avoid FP precision issues, in reality they would be exactly equal)
                if (fabs(len - origRepeat) < 0.1)
                    render->solidLine = true;
            }
        }
    }
}


void SE_AreaStyle::evaluate(SE_EvalContext* cxt)
{
    // can skip out of evaluation if style is constant and has been evaluated once
    if (cacheable && rstyle)
        return;

    SE_RenderAreaStyle* render = new SE_RenderAreaStyle();
    delete rstyle;
    rstyle = render;

    render->angleControl    = angleControl.evaluate(cxt->exec);
    render->originControl   = originControl.evaluate(cxt->exec);
    render->clippingControl = clippingControl.evaluate(cxt->exec);

    render->angleRad = fmod(angleDeg.evaluate(cxt->exec), 360.0) * M_PI180;

    // scale by xform->x0 and xform->y1 instead of mm2px, because these encompass
    // mm2px as well as scaleX and scaleY
    render->origin[0]   = origin[0].evaluate(cxt->exec)   * cxt->xform->x0;
    render->origin[1]   = origin[1].evaluate(cxt->exec)   * cxt->xform->y1;
    render->repeat[0]   = repeat[0].evaluate(cxt->exec)   * fabs(cxt->xform->x0);
    render->repeat[1]   = repeat[1].evaluate(cxt->exec)   * fabs(cxt->xform->y1);
    render->bufferWidth = bufferWidth.evaluate(cxt->exec) * fabs(cxt->xform->x0);
    double origRepeatX  = render->repeat[0];
    double origRepeatY  = render->repeat[1];

    // It makes no sense to distribute symbols using repeat values which are much
    // less than one pixel.  We'll scale up any values less than 0.25 to 0.5.
    for (int i=0; i<=1; ++i)
    {
        if (render->repeat[i] > 0.0 && render->repeat[i] < 0.25)
        {
            // just increase it by an integer multiple so the overall distribution
            // isn't affected
            int factor = (int)(0.5 / render->repeat[i]);
            render->repeat[i] *= factor;
        }
    }

    // evaluate all the primitives too
    SE_Style::evaluate(cxt);

    // special code to check if we have a simple solid fill
    render->solidFill = false;
    if (origRepeatX > 0.0 && origRepeatY > 0.0)
    {
        SE_RenderPrimitiveList& rs = render->symbol;

        // check if it is a single symbol that is not a label participant
        if (rs.size() == 1
            && rs[0]->type == SE_RenderPolygonPrimitive
            && !render->drawLast
            && !render->addToExclusionRegions)
        {
            SE_RenderPolygon* rp = (SE_RenderPolygon*)rs[0];
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

                        // repeats must be within 1/10 of a pixel for us to assume solid fill (this is
                        // only to avoid FP precision issues, in reality they would be exactly equal)
                        if (fabs(lenX - origRepeatX) < 0.1 && fabs(lenY - origRepeatY) < 0.1)
                            render->solidFill = true;
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

                        // repeats must be within 1/10 of a pixel for us to assume solid fill (this is
                        // only to avoid FP precision issues, in reality they would be exactly equal)
                        if (fabs(lenX - origRepeatX) < 0.1 && fabs(lenY - origRepeatY) < 0.1)
                            render->solidFill = true;
                    }
                }
            }
        }
    }
}


void SE_PointStyle::apply(SE_ApplyContext* ctx)
{
    ctx->renderer->ProcessPoint(ctx, (SE_RenderPointStyle*)rstyle);
}


void SE_LineStyle::apply(SE_ApplyContext* ctx)
{
    ctx->renderer->ProcessLine(ctx, (SE_RenderLineStyle*)rstyle);
}


void SE_AreaStyle::apply(SE_ApplyContext* ctx)
{
    ctx->renderer->ProcessArea(ctx, (SE_RenderAreaStyle*)rstyle);
}


void SE_Style::reset()
{
    delete rstyle;
    rstyle = NULL;
}
