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
#include "SE_RenderProxies.h"
#include "SE_Renderer.h"
#include "RS_FontEngine.h"
#include "SE_ConvexHull.h"
#include "SE_SymbolManager.h"

// Line weight is limited by this value, in mm device units.  Is there any
// use case for rendering using a line weight greater than one meter?
#define MAX_LINEWEIGHT_IN_MM 1000.0


// assumes axis aligned bounds stored in src and dst (with y pointing up),
// and the order of the points is CCW starting at the bottom left
static void BoundsUnion(RS_F_Point* dst, RS_F_Point* src)
{
    dst[0].x = dst[3].x = rs_min(dst[0].x, src[0].x);
    dst[1].x = dst[2].x = rs_max(dst[2].x, src[2].x);
    dst[0].y = dst[1].y = rs_min(dst[0].y, src[0].y);
    dst[2].y = dst[3].y = rs_max(dst[2].y, src[2].y);
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

    double wx       = weightScalable.evaluate(cxt->exec)? fabs(cxt->xform->x0) : cxt->mm2pxs;
    ret->weight     = weight.evaluate(cxt->exec) * wx;
    ret->geometry   = geometry->Clone();
    ret->color      = color.evaluate(cxt->exec);
    ret->miterLimit = miterLimit.evaluate(cxt->exec);

    // restrict the weight to something reasonable
    double weightInMM = ret->weight / cxt->mm2pxs;
    if (weightInMM > MAX_LINEWEIGHT_IN_MM)
        ret->weight = MAX_LINEWEIGHT_IN_MM * cxt->mm2pxs;

    const wchar_t* sCap = cap.evaluate(cxt->exec);
    if (wcscmp(sCap, L"Round") == 0)    // check this first since it's the most common
        ret->cap = SE_LineCap_Round;
    else if (wcscmp(sCap, L"None") == 0)
        ret->cap = SE_LineCap_None;
    else if (wcscmp(sCap, L"Square") == 0)
        ret->cap = SE_LineCap_Square;
    else if (wcscmp(sCap, L"Triangle") == 0)
        ret->cap = SE_LineCap_Triangle;
    else // default is Round
        ret->cap = SE_LineCap_Round;

    const wchar_t* sJoin = join.evaluate(cxt->exec);
    if (wcscmp(sJoin, L"Round") == 0)   // check this first since it's the most common
        ret->join = SE_LineJoin_Round;
    else if (wcscmp(sJoin, L"None") == 0)
        ret->join = SE_LineJoin_None;
    else if (wcscmp(sJoin, L"Bevel") == 0)
        ret->join = SE_LineJoin_Bevel;
    else if (wcscmp(sJoin, L"Miter") == 0)
        ret->join = SE_LineJoin_Miter;
    else // default is Round
        ret->join = SE_LineJoin_Round;

    ret->geometry->Transform(*cxt->xform, cxt->tolerance, ret);

    // TODO: here we would implement a rotating calipers algorithm to get a tighter
    //       oriented box, but for now just get the axis-aligned bounds of the path
    SE_Bounds* seb = ret->geometry->xf_bounds();
    if (seb == NULL)
    {
        // we failed for some reason - return NULL
        delete ret;
        return NULL;
    }

    ret->bounds[0].x = seb->min[0];
    ret->bounds[0].y = seb->min[1];
    ret->bounds[1].x = seb->max[0];
    ret->bounds[1].y = seb->min[1];
    ret->bounds[2].x = seb->max[0];
    ret->bounds[2].y = seb->max[1];
    ret->bounds[3].x = seb->min[0];
    ret->bounds[3].y = seb->max[1];

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

    double wx       = weightScalable.evaluate(cxt->exec)? fabs(cxt->xform->x0) : cxt->mm2pxs;
    ret->weight     = weight.evaluate(cxt->exec) * wx;
    ret->geometry   = geometry->Clone();
    ret->color      = color.evaluate(cxt->exec);
    ret->fill       = fill.evaluate(cxt->exec);
    ret->miterLimit = miterLimit.evaluate(cxt->exec);

    // restrict the weight to something reasonable
    double weightInMM = ret->weight / cxt->mm2pxs;
    if (weightInMM > MAX_LINEWEIGHT_IN_MM)
        ret->weight = MAX_LINEWEIGHT_IN_MM * cxt->mm2pxs;

    const wchar_t* sCap = cap.evaluate(cxt->exec);
    if (wcscmp(sCap, L"Round") == 0)    // check this first since it's the most common
        ret->cap = SE_LineCap_Round;
    else if (wcscmp(sCap, L"None") == 0)
        ret->cap = SE_LineCap_None;
    else if (wcscmp(sCap, L"Square") == 0)
        ret->cap = SE_LineCap_Square;
    else if (wcscmp(sCap, L"Triangle") == 0)
        ret->cap = SE_LineCap_Triangle;
    else // default is Round
        ret->cap = SE_LineCap_Round;

    const wchar_t* sJoin = join.evaluate(cxt->exec);
    if (wcscmp(sJoin, L"Round") == 0)   // check this first since it's the most common
        ret->join = SE_LineJoin_Round;
    else if (wcscmp(sJoin, L"None") == 0)
        ret->join = SE_LineJoin_None;
    else if (wcscmp(sJoin, L"Bevel") == 0)
        ret->join = SE_LineJoin_Bevel;
    else if (wcscmp(sJoin, L"Miter") == 0)
        ret->join = SE_LineJoin_Miter;
    else // default is Round
        ret->join = SE_LineJoin_Round;

    ret->geometry->Transform(*cxt->xform, cxt->tolerance, ret);

    // TODO: here we would implement a rotating calipers algorithm to get a tighter
    //       oriented box, but for now just get the axis-aligned bounds of the path
    SE_Bounds* seb = ret->geometry->xf_bounds();
    if (seb == NULL)
    {
        // we failed for some reason - return NULL
        delete ret;
        return NULL;
    }

    ret->bounds[0].x = seb->min[0];
    ret->bounds[0].y = seb->min[1];
    ret->bounds[1].x = seb->max[0];
    ret->bounds[1].y = seb->min[1];
    ret->bounds[2].x = seb->max[0];
    ret->bounds[2].y = seb->max[1];
    ret->bounds[3].x = seb->min[0];
    ret->bounds[3].y = seb->max[1];

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

    textDef.rotation() = angleDeg.evaluate(cxt->exec);   // in degrees

    int style = RS_FontStyle_Regular;
    if (bold.evaluate(cxt->exec))
        style |= RS_FontStyle_Bold;
    if (italic.evaluate(cxt->exec))
        style |= RS_FontStyle_Italic;
    if (underlined.evaluate(cxt->exec))
        style |= RS_FontStyle_Underline;

    fontDef.style() = (RS_FontStyle_Mask)style;
    fontDef.name()  = fontName.evaluate(cxt->exec);

    // RS_TextDef expects font height to be in meters - convert it from mm
    double wy           = heightScalable.evaluate(cxt->exec)? 0.001 * fabs(cxt->xform->y1) / cxt->mm2pxs : 0.001;
    fontDef.height()    = height.evaluate(cxt->exec) * wy;
    textDef.linespace() = lineSpacing.evaluate(cxt->exec);
    textDef.textcolor() = RS_Color::FromARGB(textColor.evaluate(cxt->exec));
    textDef.markup()    = markup.evaluate(cxt->exec);

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

    ret->angleRad = angleDeg.evaluate(cxt->exec) * M_PI180;

    SE_Matrix rxf;
    rxf.rotate(ret->angleRad);
    rxf.translate(ret->position[0], ret->position[1]);

    double w = 0.5*ret->extent[0];
    double h = 0.5*ret->extent[1];

    RS_F_Point pts[4];
    rxf.transform(-w, -h, pts[0].x, pts[0].y);
    rxf.transform( w, -h, pts[1].x, pts[1].y);
    rxf.transform( w,  h, pts[2].x, pts[2].y);
    rxf.transform(-w,  h, pts[3].x, pts[3].y);

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
                        rp->geometry->Transform(totalxf, cxt->tolerance, rp);
                        SE_Bounds* seb = rp->geometry->xf_bounds();
                        rp->bounds[0].x = seb->min[0];
                        rp->bounds[0].y = seb->min[1];
                        rp->bounds[1].x = seb->max[0];
                        rp->bounds[1].y = seb->min[1];
                        rp->bounds[2].x = seb->max[0];
                        rp->bounds[2].y = seb->max[1];
                        rp->bounds[3].x = seb->min[0];
                        rp->bounds[3].y = seb->max[1];
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

    render->angleRad = angleDeg.evaluate(cxt->exec) * M_PI180;

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

    render->angleRad = angleDeg.evaluate(cxt->exec) * M_PI180;

    // scale by xform->x0 and xform->y1 instead of mm2px, because these encompass
    // mm2px as well as scaleX and scaleY
    render->startOffset = startOffset.evaluate(cxt->exec) * fabs(cxt->xform->x0);
    render->endOffset   = endOffset.evaluate(cxt->exec)   * fabs(cxt->xform->x0);
    render->repeat      = repeat.evaluate(cxt->exec)      * fabs(cxt->xform->x0);

    render->vertexAngleLimit = vertexAngleLimit.evaluate(cxt->exec) * M_PI180;

    const wchar_t* sJoin = vertexJoin.evaluate(cxt->exec);
    if (wcscmp(sJoin, L"Round") == 0)       // check this first since it's the most common
        render->vertexJoin = SE_LineJoin_Round;
    else if (wcscmp(sJoin, L"None") == 0)
        render->vertexJoin = SE_LineJoin_None;
    else if (wcscmp(sJoin, L"Bevel") == 0)
        render->vertexJoin = SE_LineJoin_Bevel;
    else if (wcscmp(sJoin, L"Miter") == 0)
        render->vertexJoin = SE_LineJoin_Miter;
    else // default is Round
        render->vertexJoin = SE_LineJoin_Round;

    double wx            = dpWeightScalable.evaluate(cxt->exec)? fabs(cxt->xform->x0) : cxt->mm2pxs;
    render->dpWeight     = dpWeight.evaluate(cxt->exec) * wx;
    render->dpColor      = dpColor.evaluate(cxt->exec);
    render->dpMiterLimit = dpMiterLimit.evaluate(cxt->exec);

    // restrict the weight to something reasonable
    double weightInMM = render->dpWeight / cxt->mm2pxs;
    if (weightInMM > MAX_LINEWEIGHT_IN_MM)
        render->dpWeight = MAX_LINEWEIGHT_IN_MM * cxt->mm2pxs;

    const wchar_t* sdpCap = dpCap.evaluate(cxt->exec);
    if (wcscmp(sdpCap, L"Round") == 0)      // check this first since it's the most common
        render->dpCap = SE_LineCap_Round;
    else if (wcscmp(sdpCap, L"None") == 0)
        render->dpCap = SE_LineCap_None;
    else if (wcscmp(sdpCap, L"Square") == 0)
        render->dpCap = SE_LineCap_Square;
    else if (wcscmp(sdpCap, L"Triangle") == 0)
        render->dpCap = SE_LineCap_Triangle;
    else // default is Round
        render->dpCap = SE_LineCap_Round;

    const wchar_t* sdpJoin = dpJoin.evaluate(cxt->exec);
    if (wcscmp(sdpJoin, L"Round") == 0)     // check this first since it's the most common
        render->dpJoin = SE_LineJoin_Round;
    else if (wcscmp(sdpJoin, L"None") == 0)
        render->dpJoin = SE_LineJoin_None;
    else if (wcscmp(sdpJoin, L"Bevel") == 0)
        render->dpJoin = SE_LineJoin_Bevel;
    else if (wcscmp(sdpJoin, L"Miter") == 0)
        render->dpJoin = SE_LineJoin_Miter;
    else // default is Round
        render->dpJoin = SE_LineJoin_Round;

    // evaluate all the primitives too
    SE_Style::evaluate(cxt);
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

    render->angleRad = angleDeg.evaluate(cxt->exec) * M_PI180;

    // scale by xform->x0 and xform->y1 instead of mm2px, because these encompass
    // mm2px as well as scaleX and scaleY
    render->origin[0]   = origin[0].evaluate(cxt->exec)   * cxt->xform->x0;
    render->origin[1]   = origin[1].evaluate(cxt->exec)   * cxt->xform->y1;
    render->repeat[0]   = repeat[0].evaluate(cxt->exec)   * fabs(cxt->xform->x0);
    render->repeat[1]   = repeat[1].evaluate(cxt->exec)   * fabs(cxt->xform->y1);
    render->bufferWidth = bufferWidth.evaluate(cxt->exec) * fabs(cxt->xform->x0);

    // evaluate all the primitives too
    SE_Style::evaluate(cxt);
}


void SE_PointStyle::apply(SE_ApplyContext* cxt)
{
    cxt->renderer->ProcessPoint(cxt, (SE_RenderPointStyle*)rstyle);
}


void SE_LineStyle::apply(SE_ApplyContext* cxt)
{
    cxt->renderer->ProcessLine(cxt, (SE_RenderLineStyle*)rstyle);
}


void SE_AreaStyle::apply(SE_ApplyContext* cxt)
{
    cxt->renderer->ProcessArea(cxt, (SE_RenderAreaStyle*)rstyle);
}


void SE_Style::reset()
{
    delete rstyle;
    rstyle = NULL;
}
