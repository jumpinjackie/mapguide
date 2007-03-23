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

#include <algorithm>
#include <functional>


//assumes axis aligned
static void SetUndefinedBounds(RS_F_Point* pts)
{
    pts[0].x = pts[3].x = DBL_MAX;
    pts[1].x = pts[2].x = -DBL_MAX;
    pts[0].y = pts[1].y = DBL_MAX;
    pts[2].y = pts[3].y = -DBL_MAX;
}

//assumes axis aligned bounds stored in src and dst
static void BoundsUnion(RS_F_Point* dst, RS_F_Point* src)
{
    dst[0].x = dst[3].x = rs_min(dst[0].x, src[0].x);
    dst[1].x = dst[2].x = rs_max(dst[1].x, src[1].x);
    dst[0].y = dst[1].y = rs_min(dst[0].y, src[0].y);
    dst[2].y = dst[3].y = rs_max(dst[2].y, src[2].y);
}

static void ComputeGrowAmount(RS_F_Point* bounds, double minx, double miny, double maxx, double maxy, double &growx, double &growy)
{
    double sx, sy;
    double cx = (minx + maxx)/2.0;
    double cy = (miny + maxy)/2.0;
    minx -= cx;
    maxx -= cx;
    miny -= cy;
    maxy -= cy;
    double xfminx, xfminy, xfmaxx, xfmaxy;
    xfminx = bounds[0].x - cx;
    xfminy = bounds[0].y - cy;
    xfmaxx = bounds[2].x - cx;
    xfmaxy = bounds[2].y - cy;

    if (xfminx < minx) // minx always negative
    {
        sx = xfminx/minx - 1.0;
        growx = (growx > sx) ? growx : sx;
    }
    if (xfmaxx > maxx) // maxx always positive
    {
        sx = xfmaxx/maxx - 1.0;
        growx = (growx > sx) ? growx : sx;
    }
    if (xfminy < miny)
    {
        sy = xfminy/miny - 1.0;
        growy = (growy > sy) ? growy : sy;
    }
    if (xfmaxy > maxy)
    {
        sy = xfmaxy/maxy - 1.0;
        growy = (growy > sy) ? growy : sy;
    }
}


SE_Style::~SE_Style()
{
    for (SE_PrimitiveList::iterator iter = symbol.begin(); iter != symbol.end(); iter++)
        delete *iter;

    delete rstyle;
}


SE_RenderPrimitive* SE_Polyline::evaluate(SE_EvalContext* cxt)
{
    SE_RenderPolyline* ret = new SE_RenderPolyline();
    const wchar_t* sResizeCtrl = resizeControl.evaluate(cxt->exec);
    ret->resize = (sResizeCtrl && wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0);

    double wx =     weightScalable.evaluate(cxt->exec)? cxt->mm2pxw : cxt->mm2pxs;
    ret->weight =   weight.evaluate(cxt->exec) * wx;
    ret->geometry = geometry->Clone();
    ret->color =    color.evaluate(cxt->exec);

    if (!cxt->useBox)
    {
        ret->geometry->Transform(*cxt->xform, ret->weight);
        
        SE_Bounds* seb = ret->geometry->xf_bounds();

        //TODO: here we would implement rotating calipers algorithm to get
        //a tighter oriented box, but for now we just get the axis aligned bounds of the path
        ret->bounds[0].x = seb->min[0];
        ret->bounds[0].y = seb->min[1];
        ret->bounds[1].x = seb->max[0];
        ret->bounds[1].y = seb->min[1];
        ret->bounds[2].x = seb->max[0];
        ret->bounds[2].y = seb->max[1];
        ret->bounds[3].x = seb->min[0];
        ret->bounds[3].y = seb->max[1];
    }
    else
        SetUndefinedBounds(ret->bounds);

    return ret;
}

SE_RenderPrimitive* SE_Polygon::evaluate(SE_EvalContext* cxt)
{
    SE_RenderPolygon* ret = new SE_RenderPolygon();
    const wchar_t* sResizeCtrl = resizeControl.evaluate(cxt->exec);
    ret->resize = (sResizeCtrl && wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0);

    ret->fill = fill.evaluate(cxt->exec);

    double wx =     weightScalable.evaluate(cxt->exec)? cxt->mm2pxw : cxt->mm2pxs;
    ret->weight =   weight.evaluate(cxt->exec) * wx;
    ret->geometry = geometry->Clone();
    ret->color =    color.evaluate(cxt->exec);

    if (!cxt->useBox)
    {
        ret->geometry->Transform(*cxt->xform, ret->weight);

        SE_Bounds* seb = ret->geometry->xf_bounds();

        //TODO: here we would implement rotating calipers algorithm to get
        //a tighter oriented box, but for now we just get the axis aligned bounds of the path
        ret->bounds[0].x = seb->min[0];
        ret->bounds[0].y = seb->min[1];
        ret->bounds[1].x = seb->max[0];
        ret->bounds[1].y = seb->min[1];
        ret->bounds[2].x = seb->max[0];
        ret->bounds[2].y = seb->max[1];
        ret->bounds[3].x = seb->min[0];
        ret->bounds[3].y = seb->max[1];
    }
    else
        SetUndefinedBounds(ret->bounds);

    return ret;
}

SE_RenderPrimitive* SE_Text::evaluate(SE_EvalContext* cxt)
{
    if (cxt->fonte == NULL)
        return NULL;

    SE_RenderText* ret = new SE_RenderText();
    const wchar_t* sResizeCtrl = resizeControl.evaluate(cxt->exec);
    ret->resize = (sResizeCtrl && wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0);

    ret->text = textExpr.evaluate(cxt->exec);
    ret->position[0] = position[0].evaluate(cxt->exec);
    ret->position[1] = position[1].evaluate(cxt->exec);

    cxt->xform->transform(ret->position[0], ret->position[1]);

    ret->tdef.rotation() = angle.evaluate(cxt->exec);

    int style = RS_FontStyle_Regular;
    if (underlined.evaluate(cxt->exec)) style |= (int)RS_FontStyle_Underline;
    if (italic.evaluate(cxt->exec)) style |= (int)RS_FontStyle_Italic;
    if (bold.evaluate(cxt->exec)) style |= (int)RS_FontStyle_Bold;

    ret->tdef.font().style() = (RS_FontStyle_Mask)style;
    ret->tdef.font().name() = fontExpr.evaluate(cxt->exec);
    
    //TODO: SizeScaleable -- remove the true
    if (true || sizeScaleable.evaluate(cxt->exec))
        ret->tdef.font().height() = size.evaluate(cxt->exec)*0.001*fabs(cxt->xform->y1)/cxt->mm2px; //convert mm to meters which is what RS_TextDef expects
    else
        ret->tdef.font().height() = size.evaluate(cxt->exec) * 0.001; //size is not scaleable -- only convert from mm to meters.

    ret->tdef.linespace() = lineSpacing.evaluate(cxt->exec);

    ret->tdef.color() = RS_Color::FromARGB(textColor.evaluate(cxt->exec));
    ret->tdef.bgcolor() = RS_Color::FromARGB(ghostColor.evaluate(cxt->exec));

    const wchar_t* hAlign = hAlignment.evaluate(cxt->exec);
    if (hAlign)
    {
        if (wcscmp(hAlign, L"Left") == 0)
            ret->tdef.halign() = RS_HAlignment_Left;
        else if (wcscmp(hAlign, L"Center") == 0)
            ret->tdef.halign() = RS_HAlignment_Center;
        else if (wcscmp(hAlign, L"Right") == 0)
            ret->tdef.halign() = RS_HAlignment_Right;
    }

    const wchar_t* vAlign = vAlignment.evaluate(cxt->exec);
    if (vAlign)
    {
        if (wcscmp(vAlign, L"Bottom") == 0)
            ret->tdef.valign() = RS_VAlignment_Descent;
        else if (wcscmp(vAlign, L"Baseline") == 0)
            ret->tdef.valign() = RS_VAlignment_Base;
        else if (wcscmp(vAlign, L"Halfline") == 0)
            ret->tdef.valign() = RS_VAlignment_Half;
        else if (wcscmp(vAlign, L"Capline") == 0)
            ret->tdef.valign() = RS_VAlignment_Cap;
        else if (wcscmp(vAlign, L"Top") == 0)
            ret->tdef.valign() = RS_VAlignment_Ascent;
    }

    RS_TextMetrics tm;
    SE_Matrix txf;
    cxt->fonte->GetTextMetrics(ret->text, ret->tdef, tm, false);
    txf.rotate(ret->tdef.rotation() * M_PI180);
    txf.translate(ret->position[0], ret->position[1]);

    //compute axis aligned boudns of the text primitive
    RS_F_Point fpts[4];
    RS_Bounds xformBounds(+DBL_MAX, +DBL_MAX, -DBL_MAX, -DBL_MAX);

    for (size_t k=0; k<tm.line_pos.size(); ++k)
    {
        //convert the unrotated measured bounds for the current line to a local point array
        memcpy(fpts, tm.line_pos[k].ext, sizeof(fpts));

        // process the extent points
        for (int j=0; j<4; ++j)
        {
            txf.transform(fpts[j].x, fpts[j].y);

            // update the overall rotated bounds
            xformBounds.add_point(fpts[j]);
        }
    }

    //set the bounds into the returned primitive
    xformBounds.get_points(ret->bounds);

    return ret;
}

SE_RenderPrimitive* SE_Raster::evaluate(SE_EvalContext* cxt)
{
    SE_RenderRaster* ret = new SE_RenderRaster();
    const wchar_t* sResizeCtrl = resizeControl.evaluate(cxt->exec);
    ret->resize = (sResizeCtrl && wcscmp(sResizeCtrl, L"AdjustToResizeBox") == 0);

    if (!pngPtr)
    {
        ret->pngPtr = cxt->resources->GetImageData(resId, pngPath.evaluate(cxt->exec), ret->pngSize);
    }
    else
    {
        ret->pngPtr = pngPtr;
        ret->pngSize = pngSize;
    }

    ret->position[0] = position[0].evaluate(cxt->exec);
    ret->position[1] = position[1].evaluate(cxt->exec);
    cxt->xform->transform(ret->position[0], ret->position[1]);

    //TODO: SizeScaleable -- remove the true
    if (true || extentScaleable.evaluate(cxt->exec))
    {
        ret->extent[0] = extent[0].evaluate(cxt->exec)*cxt->xform->x0;
        ret->extent[1] = extent[1].evaluate(cxt->exec)*cxt->xform->y1;
    }
    else
    {
        ret->extent[0] = extent[0].evaluate(cxt->exec)*cxt->mm2pxw;
        ret->extent[1] = extent[1].evaluate(cxt->exec)*cxt->mm2pxw;
    }

    ret->angle = angle.evaluate(cxt->exec) * M_PI180;

    SE_Matrix rxf;
    rxf.rotate(ret->angle);
    rxf.translate(ret->position[0], ret->position[1]);

    double w = 0.5 * ret->extent[0];
    double h = 0.5 * ret->extent[1];

    RS_F_Point pts[4];
    rxf.transform( w,  h, pts[0].x, pts[0].y);
    rxf.transform( w, -h, pts[1].x, pts[1].y);
    rxf.transform(-w, -h, pts[2].x, pts[2].y);
    rxf.transform(-w,  h, pts[3].x, pts[3].y);

    memcpy(ret->bounds, pts, sizeof(ret->bounds));

    return ret;
}

void SE_Style::evaluate(SE_EvalContext* cxt)
{
    //evaluate values that are common to all styles
    rstyle->renderPass = renderPass.evaluate(cxt->exec);

    //
    //evaluation of all primitives and also resize box stuff
    //

    double dx, dy, sx, sy;
    double minx, maxx, miny, maxy;
    double growx, growy;
    SE_Matrix growxform;

    if (useBox)
    {
        dx = resizePosition[0].evaluate(cxt->exec);
        dy = resizePosition[1].evaluate(cxt->exec);
        sx = resizeSize[0].evaluate(cxt->exec)/2.0;
        sy = resizeSize[1].evaluate(cxt->exec)/2.0;

        cxt->xform->transform(dx - sx, dy - sy, minx, miny);
        cxt->xform->transform(dx + sx, dy + sy, maxx, maxy);
        cxt->xform->transform(dx, dy);

        growx = 0.0;
        growy = 0.0;

        //TODO: find a way to reorg the size box code to get rid of this flag
        cxt->useBox = true;
    }
    else
        cxt->useBox = false;

    for (SE_PrimitiveList::const_iterator src = symbol.begin(); src != symbol.end(); src++)
    {
        SE_Primitive* sym = *src;

        //evaluate the render primitive
        SE_RenderPrimitive* rsym = sym->evaluate(cxt);

        //add the primitive bounds to the overall render style bounds
        if (rsym)
        {
            if (!rsym->resize)
            {
                BoundsUnion(rstyle->bounds, rsym->bounds);
            }

            rstyle->symbol.push_back(rsym);

            if (useBox)
            {
                const wchar_t* sResizeCtrl = sym->resizeControl.evaluate(cxt->exec);

                if (wcscmp(sResizeCtrl, L"AddToResizeBox") == 0)
                    ComputeGrowAmount(rstyle->bounds, minx, miny, maxx, maxy, growx, growy);
            }
        }
    }

    if (useBox)
    {
        const wchar_t* sGrowCtrl = growControl.evaluate(cxt->exec);
        // TODO - if the string is empty we need to use the default
        if (sGrowCtrl)
        {
            if (wcscmp(sGrowCtrl, L"GrowInX") == 0)
            {
                growy = 0.0;
            }
            else if (wcscmp(sGrowCtrl, L"GrowInY") == 0)
            {
                growx = 0.0;
            }
            else if (wcscmp(sGrowCtrl, L"GrowInXY") == 0)
            {
                // TODO
            }
            else if (wcscmp(sGrowCtrl, L"GrowInXYMaintainAspect") == 0)
            {
                if (growy > growx)
                    growx = growy;
                else if (growx > growy)
                    growy = growx;
            }
        }

        SE_Matrix totalxf(*cxt->xform);
        SE_Matrix growxf;
        growxf.translate(-dx, -dy);
        growxf.scale(1.0 + growx, 1.0 + growy);
        growxf.translate(dx, dy);
        totalxf.premultiply(growxf);

        for (SE_RenderPrimitiveList::iterator rs = rstyle->symbol.begin(); rs != rstyle->symbol.end(); rs++)
        {
            SE_RenderPrimitive* rsym = *rs;
            if (rsym->resize)
            {
                switch(rsym->type)
                {
                case SE_RenderPolygonPrimitive:
                case SE_RenderPolylinePrimitive:
                    {
                        SE_RenderPolyline* rp = (SE_RenderPolyline*)rsym;
                        rp->geometry->Transform(totalxf, rp->weight);
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
                        rt->tdef.font().height() *= growxf.y1;
                        for (int j=0; j<4; j++) 
                            growxf.transform(rt->bounds[j].x, rt->bounds[j].y);
                        break;
                    }
                case SE_RenderRasterPrimitive:
                    {
                        SE_RenderRaster* rr = (SE_RenderRaster*)rsym;
                        growxf.transform(rr->position[0], rr->position[1]);
                        rr->extent[0] *= growxf.x0;
                        rr->extent[1] *= growxf.y1;
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
    SE_RenderPointStyle* render;

    if (cacheable && rstyle)
    {
        //style is constant and has been evluated once -- we can skip out of evaluation
        return;
    }
    else
    {
        render = new SE_RenderPointStyle();
        delete rstyle;
        rstyle = render;
    }

    LineBuffer::GeomOperationType type;
    switch(cxt->geometry->geom_type())
    {
    case FdoGeometryType_LineString:
    case FdoGeometryType_MultiLineString:
        type = LineBuffer::ctLine;
        break;
    case FdoGeometryType_Polygon:
    case FdoGeometryType_MultiPolygon:
        type = LineBuffer::ctArea;
        break;
    case FdoGeometryType_Point:
    case FdoGeometryType_MultiPoint:
        type = LineBuffer::ctPoint;
        break;
    default:
        type = LineBuffer::ctNone;
        break;
    }

    double angle = 0.0;
    const wchar_t* angleControl = this->angleControl.evaluate(cxt->exec);
    if (wcscmp(L"FromGeometry", angleControl) == 0)
    {
        if (type == LineBuffer::ctLine || type == LineBuffer::ctArea)
        {
            double x0, y0;
            double slope_rad = 0.0;
            cxt->geometry->Centroid(LineBuffer::ctLine, &x0, &y0, &slope_rad);

            angle = slope_rad;

            //TODO: do we really need to invert this in case of y-down?
            if (cxt->xform->y1 < 0)
                angle = -angle;
        }
    }
    else
        angle = this->angle.evaluate(cxt->exec) * M_PI180;

    double originOffsetX = originOffset[0].evaluate(cxt->exec)*cxt->mm2px;
    double originOffsetY = originOffset[1].evaluate(cxt->exec)*cxt->mm2px;

    SE_Matrix sxform;
    sxform.translate(originOffsetX, originOffsetY);
    sxform.rotate(angle);
    sxform.premultiply(*cxt->xform);
    *cxt->xform = sxform; //BAD here we modify the passed in transform -- figure out a way to avoid this

    //evaluate all the primitives too
    SE_Style::evaluate(cxt);
}

void SE_LineStyle::evaluate(SE_EvalContext* cxt)
{
    SE_RenderLineStyle* render;

    if (cacheable && rstyle)
    {
        //style is constant and has been evluated once -- we can skip out of evaluation
        return;
    }
    else
    {
        render = new SE_RenderLineStyle();
        delete rstyle;
        rstyle = render;
    }

    render->angleControl = angleControl.evaluate(cxt->exec);
    render->unitsControl = unitsControl.evaluate(cxt->exec);
    render->vertexControl = vertexControl.evaluate(cxt->exec);

    render->angle = angle.evaluate(cxt->exec) * M_PI180;
    render->startOffset = startOffset.evaluate(cxt->exec)*cxt->mm2px;
    render->endOffset = endOffset.evaluate(cxt->exec)*cxt->mm2px;
    render->repeat = repeat.evaluate(cxt->exec)*cxt->mm2px;
    render->vertexAngleLimit = vertexAngleLimit.evaluate(cxt->exec) * M_PI180;
    render->vertexJoin = vertexJoin.evaluate(cxt->exec);

    //evaluate all the primitives too
    SE_Style::evaluate(cxt);
}

void SE_AreaStyle::evaluate(SE_EvalContext* cxt)
{
    SE_RenderAreaStyle* render;
    if (cacheable && rstyle)
    {
        //style is constant and has been evluated once -- we can skip out of evaluation
        return;
    }
    else
    {
        render = new SE_RenderAreaStyle();
        delete rstyle;
        rstyle = render;
    }

    render->angleControl = angleControl.evaluate(cxt->exec);
    render->originControl = originControl.evaluate(cxt->exec);
    render->clippingControl = clippingControl.evaluate(cxt->exec);

    render->angle = angle.evaluate(cxt->exec) * M_PI180;
    render->origin[0] = origin[0].evaluate(cxt->exec);
    render->origin[1] = origin[1].evaluate(cxt->exec);
    render->repeat[0] = repeat[0].evaluate(cxt->exec);
    render->repeat[1] = repeat[1].evaluate(cxt->exec);
    render->bufferWidth = bufferWidth.evaluate(cxt->exec);

    //evaluate all the primitives too
    SE_Style::evaluate(cxt);
}

void SE_PointStyle::apply(LineBuffer* geometry, SE_Renderer* renderer)
{
    renderer->ProcessPoint(geometry, (SE_RenderPointStyle*)rstyle);
}

void SE_LineStyle::apply(LineBuffer* geometry, SE_Renderer* renderer)
{
    renderer->ProcessLine(geometry, (SE_RenderLineStyle*)rstyle);
}

void SE_AreaStyle::apply(LineBuffer* geometry, SE_Renderer* renderer)
{
    renderer->ProcessArea(geometry, (SE_RenderAreaStyle*)rstyle);
}
