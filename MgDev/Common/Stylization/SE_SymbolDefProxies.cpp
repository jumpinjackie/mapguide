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

SE_Style::~SE_Style()
{
    for (SE_PrimitiveList::iterator iter = symbol.begin(); iter != symbol.end(); iter++)
        delete *iter;

    delete rstyle;
}


SE_RenderPrimitive* SE_Polyline::evaluate(SE_EvalContext* cxt)
{
    SE_RenderPolyline* ret = new SE_RenderPolyline();
    ret->resize = (resize == GraphicElement::AdjustToResizeBox);

    double wx =     weightScalable.evaluate(cxt->exec)? cxt->mm2pxw : cxt->mm2pxs;
    ret->weight =   weight.evaluate(cxt->exec) * wx;
    ret->geometry = geometry->Clone();
    ret->color =    color.evaluate(cxt->exec);

    if (!cxt->useBox)
    {
        ret->geometry->Transform(*cxt->xform, ret->weight);
        ret->bounds = ret->geometry->xf_bounds()->Clone();
    }
    else
        ret->bounds = NULL;

    return ret;
}

SE_RenderPrimitive* SE_Polygon::evaluate(SE_EvalContext* cxt)
{
    SE_RenderPolygon* ret = new SE_RenderPolygon();
    ret->resize = (resize == GraphicElement::AdjustToResizeBox);

    ret->fill = fill.evaluate(cxt->exec);

    double wx =     weightScalable.evaluate(cxt->exec)? cxt->mm2pxw : cxt->mm2pxs;
    ret->weight =   weight.evaluate(cxt->exec) * wx;
    ret->geometry = geometry->Clone();
    ret->color =    color.evaluate(cxt->exec);

    if (!cxt->useBox)
    {
        ret->geometry->Transform(*cxt->xform, ret->weight);
        ret->bounds = ret->geometry->xf_bounds()->Clone();
    }
    else
        ret->bounds = NULL;

    return ret;
}

SE_RenderPrimitive* SE_Text::evaluate(SE_EvalContext* cxt)
{
    if (cxt->fonte == NULL)
        return NULL;

    SE_RenderText* ret = new SE_RenderText();
    ret->resize = (resize == GraphicElement::AdjustToResizeBox);

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
    ret->tdef.font().height() = size.evaluate(cxt->exec)*0.001*cxt->xform->y1/cxt->mm2px; //convert mm to meters which is what RS_TextDef expects
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

    double* buffer = (double*)alloca(tm.line_pos.size()*sizeof(double)*8);
    double* ptr = buffer;
    for(size_t k = 0; k < tm.line_pos.size(); k++)
    {
        const RS_F_Point* pts = tm.line_pos[k].ext;
        txf.transform(pts[0].x, pts[0].y, ptr[0], ptr[1]);
        txf.transform(pts[1].x, pts[1].y, ptr[2], ptr[3]);
        txf.transform(pts[2].x, pts[2].y, ptr[4], ptr[5]);
        txf.transform(pts[3].x, pts[3].y, ptr[6], ptr[7]);
        ptr += 8;
    }

    std::sort((std::pair<double,double>*)buffer, (std::pair<double,double>*)(ptr - 2), PointLess( ));
    ret->bounds = AndrewHull<std::pair<double,double>*,SimplePOINT>
        ((std::pair<double,double>*)buffer,
        ((std::pair<double,double>*)ptr) - 1,
        (int)tm.line_pos.size()*4, cxt->pool);

    return ret;
}

SE_RenderPrimitive* SE_Raster::evaluate(SE_EvalContext* cxt)
{
    SE_RenderRaster* ret = new SE_RenderRaster();
    ret->resize = (resize == GraphicElement::AdjustToResizeBox);

    if (!pngPtr)
    {
        ret->pngPtr = cxt->resources->GetImageData(pngPath.evaluate(cxt->exec), ret->pngSize);
    }
    else
    {
        ret->pngPtr = pngPtr;
        ret->pngSize = pngSize;
    }

    ret->position[0] = position[0].evaluate(cxt->exec);
    ret->position[1] = position[1].evaluate(cxt->exec);
    cxt->xform->transform(ret->position[0], ret->position[1]);
    ret->extent[0] = extent[0].evaluate(cxt->exec)*cxt->xform->x0;
    ret->extent[1] = extent[1].evaluate(cxt->exec)*cxt->xform->y1;
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

    std::sort((std::pair<double,double>*)pts, (std::pair<double,double>*)(pts + 3), PointLess( ));
    ret->bounds = AndrewHull<std::pair<double,double>*,SimplePOINT>
        ((std::pair<double,double>*)pts,
        ((std::pair<double,double>*)(pts+3)) - 1,
        4, cxt->pool);

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
                if (rstyle->bounds)
                {
                    SE_Bounds* bounds = rstyle->bounds;
                    rstyle->bounds = bounds->Union(rsym->bounds);
                    bounds->Free();
                }
                else
                    rstyle->bounds = rsym->bounds->Clone();
            }

            rstyle->symbol.push_back(rsym);

            if (useBox && sym->resize == GraphicElement::AddToResizeBox)
                rsym->bounds->Contained(minx, miny, maxx, maxy, growx, growy);
        }
    }

    if (useBox)
    {
        switch(resize)
        {
        case ResizeBox::GrowInX:
            growy = 0.0;
            break;

        case ResizeBox::GrowInY:
            growx = 0.0;
            break;

        case ResizeBox::GrowInXYMaintainAspect:
            if (growy > growx)
                growx = growy;
            else if (growx > growy)
                growy = growx;
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
                        rp->bounds = rp->geometry->xf_bounds()->Clone();
                        break;
                    }
                case SE_RenderTextPrimitive:
                    {
                        SE_RenderText* rt = (SE_RenderText*)rsym;
                        growxf.transform(rt->position[0], rt->position[1]);
                        rt->tdef.font().height() *= growxf.y1;
                        rt->bounds->Transform(growxf);
                        break;
                    }
                case SE_RenderRasterPrimitive:
                    {
                        SE_RenderRaster* rr = (SE_RenderRaster*)rsym;
                        growxf.transform(rr->position[0], rr->position[1]);
                        rr->extent[0] *= growxf.x0;
                        rr->extent[1] *= growxf.y1;
                        rr->bounds->Transform(growxf);
                        break;
                    }
                }

                if (rstyle->bounds)
                {
                    SE_Bounds* bounds = rstyle->bounds;
                    rstyle->bounds = bounds->Union(rsym->bounds);
                    bounds->Free();
                }
                else
                    rstyle->bounds = rsym->bounds->Clone();
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
//  render->join = style->join.evaluate(m_exec);

    render->angle = angle.evaluate(cxt->exec) * M_PI180;
    render->startOffset = startOffset.evaluate(cxt->exec)*cxt->mm2px;
    render->endOffset = endOffset.evaluate(cxt->exec)*cxt->mm2px;
    render->repeat = repeat.evaluate(cxt->exec)*cxt->mm2px;
    render->vertexAngleLimit = vertexAngleLimit.evaluate(cxt->exec) * M_PI180;

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
