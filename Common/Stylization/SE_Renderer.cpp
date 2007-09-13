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
#include "SE_Renderer.h"
#include "SE_LineBuffer.h"
#include "SE_LineStorage.h"
#include "RS_FontEngine.h"
#include "SE_Bounds.h"

#include "SE_JoinProcessor.h"
#include "SE_Join_Miter.h"
#include "SE_Join_Bevel.h"
#include "SE_Join_Round.h"
#include "SE_Join_Identity.h"
#include "SE_Cap_Butt.h"
/* TODO: test, etc. */
//#include "SE_Cap_Triangle.h"
//#include "SE_Cap_Square.h"
//#include "SE_Cap_Round.h"

using namespace MDFMODEL_NAMESPACE;

#define PXF_BUF_LEN 500
#define PXF_SEG_DELTA 1e-2

SE_Renderer::SE_Renderer()
: m_bp(NULL)
, m_bSelectionMode(false)
, m_selWeight(0.0)
, m_selColor(0)
, m_selFill(0)
, m_textForeColor(0)
, m_textBackColor(0)
{
}


SE_Renderer::~SE_Renderer()
{
}


void SE_Renderer::SetRenderSelectionMode(bool mode)
{
    m_bSelectionMode = mode;

    if (mode)
    {
        // set the default selection style - 1mm line weight, partially transparent blue
        m_selWeight = 3.0;  // should be 1 to give 1mm, but the renderer is way off
        m_selColor = RS_Color(0, 0, 255, 200).argb();
        m_selFill = RS_Color(0, 0, 255, 160).argb();
        m_textForeColor = RS_Color(0, 0, 255, 200);
        m_textBackColor = RS_Color(0, 0, 255, 255);
    }
}


void SE_Renderer::ProcessPoint(SE_ApplyContext* ctx, SE_RenderPointStyle* style)
{
    // the feature geometry we're apply the style on...
    LineBuffer* featGeom = ctx->geometry;

    double angleRad;
    if (wcscmp(L"FromAngle", style->angleControl) == 0)
    {
        angleRad = style->angleRad;
    }
    else
    {
        angleRad = 0.0;
        switch (featGeom->geom_type())
        {
            case FdoGeometryType_LineString:
            case FdoGeometryType_MultiLineString:
            case FdoGeometryType_Polygon:
            case FdoGeometryType_MultiPolygon:
            {
                double x0, y0;
                featGeom->Centroid(LineBuffer::ctLine, &x0, &y0, &angleRad);
                break;
            }
        }
    }

    SE_Matrix xform;
    bool yUp = YPointsUp();

    // see StylizationEngine::Stylize for a detailed explanation of these transforms
    SE_Matrix xformbase;
    xformbase.translate(style->offset[0], style->offset[1]);
    xformbase.rotate(yUp? angleRad : -angleRad);
    xformbase.premultiply(*ctx->xform);

    // render the points
    for (int i=0; i<featGeom->point_count(); ++i)
    {
        double x, y;
        featGeom->get_point(i, x, y);

        // transform to screen space -- feature geometry is in [the original] mapping space
        WorldToScreenPoint(x, y, x, y);

        xform = xformbase;
        xform.translate(x, y);

        if (style->drawLast)
            AddLabel(featGeom, style, xform, angleRad);
        else
        {
            DrawSymbol(style->symbol, xform, angleRad);

            if (style->addToExclusionRegions)
                AddExclusionRegion(style, xform, angleRad);
        }
    }
}


void SE_Renderer::ProcessLineJoin(LineBuffer* geometry, SE_RenderLineStyle* style)
{
    SE_Join<NullData>* pJoin;
    SE_Cap<NullData>*  pCap;

    switch(style->vertexJoin)
    {
    case SE_LineJoin_Bevel:
        pJoin = (SE_Join<NullData>*)new SE_Join_Bevel<NullData>( style );
        break;
    case SE_LineJoin_Round:
        pJoin = (SE_Join<NullData>*)new SE_Join_Round<NullData>( style );
        break;
    case SE_LineJoin_Miter:
        pJoin = (SE_Join<NullData>*)new SE_Join_Miter<NullData>( style );
        break;
    default:
    case SE_LineJoin_None:
        pJoin = (SE_Join<NullData>*)new SE_Join_Identity<NullData>( style );
        break;
    }
    /* TODO: caps in mdf model? */
    pCap = (SE_Cap<NullData>*)new SE_Cap_Butt<NullData>( style );

    /* TODO: :( */
    SE_Matrix w2s;
    GetWorldToScreenTransform(w2s);

    SE_LineStorage* xfgeom = m_bp->NewLineStorage(geometry->point_count());
    xfgeom->SetToTransform(w2s, geometry);

    for (int i = 0; i < xfgeom->cntr_count(); ++i)
    {
        /* TODO: options for other processors */
        NullProcessor processor(pJoin, pCap, xfgeom, i, style, m_bp);
        double position = style->startOffset;

        /* TODO: additional calls at beginning/end to account for offset action? */
        while (position < processor.ContourLength())
        {
            processor.UpdateLinePosition(position);
            DrawSymbol(style->symbol, SE_Matrix::Identity, 0.0, &processor);
            position += style->repeat;
        }
    }

    xfgeom->Free();
    delete pJoin;
    delete pCap;
}


void SE_Renderer::DrawScreenRaster(unsigned char* /*data*/, int /*length*/,
                                   RS_ImageFormat /*format*/, int /*native_width*/,
                                   int /*native_height*/, SE_Tuple* /*uv_quads*/,
                                   SE_Tuple* /*xy_quads*/, int /*txlength*/)
{

}


void SE_Renderer::SetBufferPool(SE_BufferPool* pool)
{
    m_bp = pool;
}


void SE_Renderer::ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style)
{
    // the feature geometry we're apply the style on...
    LineBuffer* featGeom = ctx->geometry;

    //determine if the style is a simple straight solid line
    SE_RenderPrimitiveList& rs = style->symbol;

    //check if it is a single symbol that is not a label participant
    if (rs.size() == 1
        && rs[0]->type == SE_RenderPolylinePrimitive
        && !style->drawLast
        && !style->addToExclusionRegions)
    {
        SE_RenderPolyline* rp = (SE_RenderPolyline*)rs[0];
        LineBuffer* lb = rp->geometry->xf_buffer();

        //check if it is a horizontal line
        if (lb->point_count() == 2
            && lb->y_coord(0) == 0.0
            && lb->y_coord(1) == 0.0)
        {
            //now make sure it is not a dashed line by comparing the
            //single segment to the symbol repeat
            double len = lb->x_coord(1) - lb->x_coord(0);

            //repeat must be within 1/1000 of a pixel for us to assume solid line
            //this is only to avoid FP precision issues, in reality they would be exactly equal
            if (fabs(len - style->repeat) < 0.001)
            {
                //ok, it's only a solid line, just draw it and bail out of the
                //layout function
                SE_Matrix m;
                GetWorldToScreenTransform(m);
                DrawScreenPolyline(featGeom, &m, rp->color, rp->weight);
                return;
            }
        }
    }

    // TODO: remove/integrate when joins work with rasters, text
    bool vectorOnly = true;
    for (SE_RenderPrimitiveList::const_iterator iter = rs.begin(); iter != rs.end(); iter++)
    {
        if ((*iter)->type != SE_RenderPolylinePrimitive && (*iter)->type != SE_RenderPolygonPrimitive)
        {
            vectorOnly = false;
            break;
        }
    }

    if (vectorOnly && wcscmp(style->vertexControl, L"OverlapWrap") == 0)
    {
        ProcessLineJoin(featGeom, style);
        return;
    }

    bool yUp = YPointsUp();

    SE_Matrix symxf;


    //get the increment (the render style already stores this in screen units)
    //TODO - handle case where increment is 0
    double increment = style->repeat;

    bool fromAngle = (wcscmp(L"FromAngle", style->angleControl) == 0);
    double angleRad = style->angleRad;

    // precompute these - these are in renderer space, hence the check for yUp with the sine
    double angleCos = cos(angleRad);
    double angleSin = sin(yUp? angleRad : -angleRad);

    for (int j=0; j<featGeom->cntr_count(); ++j)
    {
        //current polyline
        int last = featGeom->contour_end_point(j);
        //pixel position along the current segment of the polyline
        double drawpos = style->startOffset;

        int cur_seg = featGeom->contour_start_point(j);

        while (cur_seg < last)
        {
            symxf.setIdentity();

            //current line segment
            double seg_screen[4];

            //transform segment from mapping to screen space
            WorldToScreenPoint(featGeom->x_coord(cur_seg), featGeom->y_coord(cur_seg), seg_screen[0], seg_screen[1]);
            cur_seg++;
            WorldToScreenPoint(featGeom->x_coord(cur_seg), featGeom->y_coord(cur_seg), seg_screen[2], seg_screen[3]);

            //get length
            double dx = seg_screen[2] - seg_screen[0];
            double dy = seg_screen[3] - seg_screen[1];
            double len = sqrt(dx*dx + dy*dy);

            //check if completely skipping current segment since it is smaller than
            //the increment
            if (drawpos < len && len > 0.0)
            {
                //compute linear deltas for x and y directions
                // -- we will use these to quickly move along the line
                //without having to do too much math
                double invlen = 1.0 / len;
                double dx_incr = dx * invlen;
                double dy_incr = dy * invlen;

                if (!fromAngle)
                {
                    angleCos = dx_incr;
                    angleSin = dy_incr;
                    angleRad = atan2(dy_incr, dx_incr);

                    // since dy_incr and dx_incr are in renderer space we need to
                    // negate the angle if y points down
                    if (!yUp)
                        angleRad = -angleRad;
                }
                double tx = seg_screen[0] + dx_incr * drawpos;
                double ty = seg_screen[1] + dy_incr * drawpos;

                symxf.rotate(angleSin, angleCos);
                symxf.translate(tx, ty);
                dx_incr *= increment;
                dy_incr *= increment;

                //loop-draw the symbol along the current segment,
                //moving along by increment pixels
                while (drawpos < len)
                {
                    if (style->drawLast)
                        AddLabel(featGeom, style, symxf, angleRad);
                    else
                    {
                        DrawSymbol(style->symbol, symxf, angleRad);

                        if (style->addToExclusionRegions)
                            AddExclusionRegion(style, symxf, angleRad);
                    }

                    symxf.translate(dx_incr, dy_incr);
                    drawpos += increment;
                }
            }

            drawpos -= len;
        }
    }
}


void SE_Renderer::ProcessArea(SE_ApplyContext* /*ctx*/, SE_RenderAreaStyle* /*style*/)
{
}


void SE_Renderer::DrawSymbol(SE_RenderPrimitiveList& symbol,
                             const SE_Matrix& posxform,
                             double angleRad,
                             SE_IJoinProcessor* processor)
{
    for (unsigned i = 0; i < symbol.size(); i++)
    {
        SE_RenderPrimitive* primitive = symbol[i];

        if (primitive->type == SE_RenderPolygonPrimitive || primitive->type == SE_RenderPolylinePrimitive)
        {
            SE_RenderPolyline* pl = (SE_RenderPolyline*)primitive;

            LineBuffer* geometry = pl->geometry->xf_buffer();
            if (processor)
                geometry = processor->Transform(geometry);

            if (m_bSelectionMode)
            {
                if (primitive->type == SE_RenderPolygonPrimitive)
                    DrawScreenPolygon(geometry, &posxform, m_selFill);

                DrawScreenPolyline(geometry, &posxform, m_selColor, m_selWeight);
            }
            else
            {
                if (primitive->type == SE_RenderPolygonPrimitive)
                    DrawScreenPolygon(geometry, &posxform, ((SE_RenderPolygon*)primitive)->fill);

                DrawScreenPolyline(geometry, &posxform, pl->color, pl->weight);
            }
            if (processor)
                ((SE_LineStorage*)geometry)->Free();
        }
        else if (primitive->type == SE_RenderTextPrimitive)
        {
            SE_RenderText* tp = (SE_RenderText*)primitive;

            // TODO take into account rotation if drawing along a line and
            // the angle control is "from geometry"
            double x, y;
            posxform.transform(tp->position[0], tp->position[1], x, y);

            RS_TextDef tdef = tp->tdef;
            tdef.rotation() += angleRad / M_PI180;

            if (m_bSelectionMode)
            {
                tdef.textcolor() = m_textForeColor;
                tdef.ghostcolor() = m_textBackColor;
//              tdef.framecolor() = m_textBackColor;
//              tdef.opaquecolor() = m_textBackColor;
            }

            DrawScreenText(tp->content, tdef, x, y, NULL, 0, 0.0);
        }
        else if (primitive->type == SE_RenderRasterPrimitive)
        {
            SE_RenderRaster* rp = (SE_RenderRaster*)primitive;
            ImageData& imgData = rp->imageData;

            if (imgData.data != NULL)
            {
                double x, y;
                posxform.transform(rp->position[0], rp->position[1], x, y);
                double angleDeg = (rp->angleRad + angleRad) / M_PI180;

                DrawScreenRaster(imgData.data, imgData.size, imgData.format, imgData.width, imgData.height, x, y, rp->extent[0], rp->extent[1], angleDeg);
            }
        }
    }
}


void SE_Renderer::AddLabel(LineBuffer* geom, SE_RenderStyle* style, SE_Matrix& xform, double angleRad)
{
    //clone the SE_RenderStyle so that the label renderer can keep track of it until
    //the end of rendering when it draws all the labels
    //TODO: cloning is bad.
    SE_RenderStyle* copied_style = CloneRenderStyle(style);

    SE_LabelInfo info(xform.x2, xform.y2, RS_Units_Device, angleRad, copied_style);

    ProcessLabelGroup(&info, 1, RS_OverpostType_AllFit, style->addToExclusionRegions, geom);
}


void SE_Renderer::AddExclusionRegion(SE_RenderStyle* rstyle, SE_Matrix& xform, double /*angleRad*/)
{
    RS_F_Point* fpts = m_lastExclusionRegion;
    memcpy(fpts, rstyle->bounds, 4 * sizeof(RS_F_Point));

    for (int i=0; i<4; i++)
        xform.transform(fpts[i].x, fpts[i].y);

    AddExclusionRegion(fpts, 4);
}


const RS_F_Point* SE_Renderer::GetLastExclusionRegion()
{
    return m_lastExclusionRegion;
}


// cloning of render styles - unfortunate but necessary for delay-drawing labels
SE_RenderStyle* SE_Renderer::CloneRenderStyle(SE_RenderStyle* symbol)
{
    SE_RenderStyle* ret = NULL;

    // first determine what kind of style it is and copy all the
    // style specific properties
    switch (symbol->type)
    {
    case SE_RenderPointStyleType:
        {
            SE_RenderPointStyle* sps = (SE_RenderPointStyle*)symbol;
            SE_RenderPointStyle* dps = new SE_RenderPointStyle();
            ret = dps;

            dps->angleControl = sps->angleControl;
            dps->angleRad     = sps->angleRad;
            dps->offset[0]    = sps->offset[0];
            dps->offset[1]    = sps->offset[1];
        }
        break;

    case SE_RenderLineStyleType:
        {
            SE_RenderLineStyle* sls = (SE_RenderLineStyle*)symbol;
            SE_RenderLineStyle* dls = new SE_RenderLineStyle();
            ret = dls;

            dls->angleControl     = sls->angleControl;
            dls->unitsControl     = sls->unitsControl;
            dls->vertexControl    = sls->vertexControl;
            dls->angleRad         = sls->angleRad;
            dls->startOffset      = sls->startOffset;
            dls->endOffset        = sls->endOffset;
            dls->repeat           = sls->repeat;
            dls->vertexAngleLimit = sls->vertexAngleLimit;
            dls->vertexJoin       = sls->vertexJoin;
            dls->vertexMiterLimit = sls->vertexMiterLimit;
        }
        break;

    case SE_RenderAreaStyleType:
        {
            SE_RenderAreaStyle* sas = (SE_RenderAreaStyle*)symbol;
            SE_RenderAreaStyle* das = new SE_RenderAreaStyle();
            ret = das;

            das->angleControl    = sas->angleControl;
            das->originControl   = sas->originControl;
            das->clippingControl = sas->clippingControl;
            das->angleRad        = sas->angleRad;
            das->origin[0]       = sas->origin[0];
            das->origin[1]       = sas->origin[1];
            das->repeat[0]       = sas->repeat[0];
            das->repeat[1]       = sas->repeat[1];
            das->bufferWidth     = sas->bufferWidth;
        }

    default:
        break;
    }

    // copy all the common properties
    memcpy(ret->bounds, symbol->bounds, sizeof(ret->bounds));
    ret->addToExclusionRegions = symbol->addToExclusionRegions;
    ret->checkExclusionRegions = symbol->checkExclusionRegions;
    ret->drawLast              = symbol->drawLast;
    ret->renderPass            = symbol->renderPass;

    // copy the graphics for the symbol
    for (size_t i=0; i<symbol->symbol.size(); i++)
    {
        SE_RenderPrimitive* rp = symbol->symbol[i];
        SE_RenderPrimitive* rpc = NULL;

        switch (rp->type)
        {
        case SE_RenderPolygonPrimitive:
            {
                SE_RenderPolygon* sp = (SE_RenderPolygon*)rp;
                SE_RenderPolygon* dp = new SE_RenderPolygon();
                rpc = dp;

                dp->fill = sp->fill;
            }
            // fall through

        case SE_RenderPolylinePrimitive:
            {
                SE_RenderPolyline* sp = (SE_RenderPolyline*)rp;
                if (!rpc) rpc = new SE_RenderPolyline();
                SE_RenderPolyline* dp = (SE_RenderPolyline*)rpc;

                dp->geometry   = sp->geometry->Clone();
                dp->weight     = sp->weight;
                dp->color      = sp->color;
                dp->join       = sp->join;
                dp->cap        = sp->cap;
                dp->miterLimit = sp->miterLimit;
            }
            break;

        case SE_RenderTextPrimitive:
            {
                SE_RenderText* st = (SE_RenderText*)rp;
                SE_RenderText* dt = new SE_RenderText();
                rpc = dt;

                dt->content     = st->content;
                dt->position[0] = st->position[0];
                dt->position[1] = st->position[1];
                dt->tdef        = st->tdef;
            }
            break;

        case SE_RenderRasterPrimitive:
            {
                SE_RenderRaster* sr = (SE_RenderRaster*)rp;
                SE_RenderRaster* dr = new SE_RenderRaster();
                rpc = dr;

                dr->imageData   = sr->imageData;    // image data pointer is managed/cached by the SE_SymbolManager
                dr->position[0] = sr->position[0];
                dr->position[1] = sr->position[1];
                dr->extent[0]   = sr->extent[0];
                dr->extent[1]   = sr->extent[1];
                dr->angleRad    = sr->angleRad;
            }
        }

        if (rpc)
        {
            // copy common properties
            rpc->resizeControl = rp->resizeControl;
            memcpy(rpc->bounds, rp->bounds, sizeof(rp->bounds));

            // add to list
            ret->symbol.push_back(rpc);
        }
    }

    return ret;
}
