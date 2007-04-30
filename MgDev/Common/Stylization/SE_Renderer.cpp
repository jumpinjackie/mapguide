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
#include "SE_IdentityJoin.h"
#include "SE_MiterJoin.h"

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
        // default is FromGeometry
        angleRad = 0.0;
        switch(featGeom->geom_type())
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
        double x = featGeom->points()[2*i];
        double y = featGeom->points()[2*i+1];

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
    RS_Bounds bounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
    bounds.add_point(style->bounds[0]);
    bounds.add_point(style->bounds[1]);
    bounds.add_point(style->bounds[2]);
    bounds.add_point(style->bounds[3]);
    double width = bounds.width();

    double increment = style->repeat;
    /* Avoid infinite looping;
     * Handily, if repeat is omitted from the XML, the symbol width will be used. */
    if (increment <= 0)
        increment = width;

    double* pts = geometry->points();
    RS_F_Point pprev, prev, cur, next, dv;

    for (int j=0; j<geometry->cntr_count(); j++)
    {
        /* Current polyline */
        double* end = pts + 2*geometry->cntrs()[j];

        /* Without two points, you can't have much of a line */
        if (end - pts < 4)
            return;

        /* TODO: test end offset (not implemented below either) */
        //pixel position along the current segment of the polyline
        //double drawpos = style->endOffset;
        //double length = 0.0, seglen = 0.0;

        //next.y = *(--end);
        //next.x = *(--end);

        //for(;;)
        //{
        //    cur.x = end[-2];
        //    cur.y = end[-1];
        //    dv.x = next.x - cur.x;
        //    dv.y = next.y - cur.y;
        //    seglen = sqrt(dv.x*dv.x + dv.y*dv.y);
        //    length += seglen;
        //    if (drawpos < length || pts > end)
        //        break;
        //    next.x = cur.x;
        //    next.y = cur.y;
        //    end -= 2;
        //}

        //if (pts < end)
        //{
        //    double frac = (length - drawpos)/seglen;
        //    last.x = cur.x + dv.x*frac;
        //    last.y = cur.y + dv.y*frac;
        //}

        WorldToScreenPoint(pts[0], pts[1], prev.x, prev.y);
        pts += 2;
        double drawpos = style->startOffset + bounds.minx;
        double pprevlength = 0.0, prevlength = 0.0, length = 0.0, seglength = 0.0, nextseglength = 0.0;

        for(;;)
        {
            WorldToScreenPoint(pts[0], pts[1], cur.x, cur.y);
            dv.x = cur.x - prev.x;
            dv.y = cur.y - prev.y;
            seglength = sqrt(dv.x*dv.x + dv.y*dv.y);
            length += seglength;
            if (drawpos < length || pts > end)
                break;
            prev.x = cur.x;
            prev.y = cur.y;
            prevlength = length;
            pts += 2;
        }

        double frac = 1.0 - (length - drawpos)/seglength;
        prev.x = prev.x + dv.x*frac;
        prev.y = prev.y + dv.y*frac;
        prevlength = drawpos;

        /* Handle single-segment lines */
        if (end - pts == 2)
        {
            while (drawpos < length)
            {
                SE_IdentityJoin idj(bounds, drawpos - prevlength, prev, cur, -DBL_MAX, length - prevlength);
                SE_PiecewiseTransform* ptx = &idj;
                if (style->drawLast)
                    AddLabelJoin(geometry, style, &ptx, 1);
                else
                {
                    DrawSymbolJoin(style->symbol, &ptx, 1);
                    if (style->addToExclusionRegions)
                        AddExclusionRegionJoin(&ptx, 1);
                }
                drawpos += increment;
            }
            return;
        }

        /* TODO: start cap */

        pts += 2;
        WorldToScreenPoint(pts[0], pts[1], next.x, next.y);
        dv.x = next.x - cur.x;
        dv.y = next.y - cur.y;
        nextseglength = sqrt(dv.x*dv.x + dv.y*dv.y);

        SE_PiecewiseTransform** pxforms = (SE_PiecewiseTransform**)alloca(sizeof(SE_PiecewiseTransform*)*PXF_BUF_LEN);
        SE_PiecewiseTransform* curpxf = NULL;
        int npxforms = 0, pxfbuflen = PXF_BUF_LEN;
        double cur_pre_rad, cur_post_rad, prev_pre_rad = 0.0, prev_post_rad = 0.0;

        /* TODO: Refactor. Create control flow that makes sense. */
        for(;;)
        {
            /* TODO: every time you allocate to the heap, god kills a kitten...needs object pooling... */
            double miterLimit = 5.0; /* TODO: Forgotten in MDF model */
            switch(style->vertexJoin)
            {
            case SE_LineJoin_Round: /* TODO */
            case SE_LineJoin_None:
            case SE_LineJoin_Bevel:
                miterLimit = 0.0;
            case SE_LineJoin_Miter:
                {
                    curpxf = new SE_MiterJoin(miterLimit, bounds, drawpos - length, prev, cur, next, npxforms > 0);
                }
                break;
            //case SE_LineJoin_Round: /* TODO */
            //case SE_LineJoin_None:
            default:
                {
                    /* TODO: Unhose (Here, idj needs to not clip, but instead to piecewise transform)
                     * basically, add another constructor with the same args as MiterJoin */
                    curpxf = new SE_IdentityJoin(bounds, drawpos - prevlength, prev, cur);
                }
            }

            double idlength = 0;
            double startnpxforms = npxforms;
            curpxf->GetXRadius(cur_pre_rad, cur_post_rad);

            while (npxforms > 0 && drawpos + width < length - cur_pre_rad)
            {
                if (style->drawLast)
                    AddLabelJoin(geometry, style, pxforms, npxforms);
                else
                {
                    DrawSymbolJoin(style->symbol, pxforms, npxforms);
                    if (style->addToExclusionRegions)
                        AddExclusionRegionJoin(pxforms, npxforms);
                }
                idlength += increment;
                drawpos += increment;

                if (drawpos < prevlength + prev_post_rad)
                {
                    for (int i = 0; i < npxforms - 1; i++)
                        delete pxforms[i];

                    if (npxforms > 1)
                    {   /* Stick in an I transform so the input coordinate systems match */
                        pxforms[0] = new SE_IdentityJoin(bounds, drawpos - pprevlength, pprev, prev);
                        pxforms[1] = pxforms[npxforms-1];
                        npxforms = 2;
                    }
                    else
                    {
                        SE_Matrix updatexf(1.0, 0.0, increment, 0.0, 1.0, 0.0); /* Translate by (increment, 0) */
                        pxforms[0]->ApplyPreTransform(updatexf);
                    }
                }
                else
                {
                    for (int i = 0; i < npxforms; i++)
                        delete pxforms[i];
                    npxforms = 0;
                }
            }

            if (npxforms == 0)
            {
                while (drawpos + width < length - cur_pre_rad)
                {
                    SE_IdentityJoin idj = SE_IdentityJoin(bounds, drawpos - prevlength, prev, cur);
                    SE_PiecewiseTransform* pxform = &idj;
                    /* TODO: This block gets copied and pasted like 8 times */
                    if (style->drawLast)
                        AddLabelJoin(geometry, style, &pxform, 1);
                    else
                    {
                        DrawSymbolJoin(style->symbol, &pxform, 1);
                        if (style->addToExclusionRegions)
                            AddExclusionRegionJoin(&pxform, 1);
                    }
                    idlength += increment;
                    drawpos += increment;
                }
            }

            if (idlength > 0 && npxforms == 0)
            {
                if (startnpxforms > 0)
                {/* curpxf is taking world coordinates, but is going to be the first xform */
                    pxforms[npxforms++] = new SE_IdentityJoin(bounds, drawpos - prevlength, prev, cur);
                }
                else
                {
                    SE_Matrix updatexf(1.0, 0.0, idlength, 0.0, 1.0, 0.0); /* Translate by (idlength, 0) */
                    curpxf->ApplyPreTransform(updatexf);
                }
            }

            if (npxforms >= pxfbuflen)
            {   /* TODO: pooling, etc. */
                pxfbuflen *= 2;
                SE_PiecewiseTransform** newbuf = new SE_PiecewiseTransform*[pxfbuflen];
                memcpy(newbuf, pxforms, npxforms*sizeof(SE_PiecewiseTransform*));
                if (npxforms != PXF_BUF_LEN)
                    delete[] pxforms;
                pxforms = newbuf;
            }
            pxforms[npxforms++] = curpxf;

            pprev.x = prev.x;
            pprev.y = prev.y;
            prev.x = cur.x;
            prev.y = cur.y;
            cur.x = next.x;
            cur.y = next.y;
            prev_pre_rad = cur_pre_rad;
            prev_post_rad = cur_post_rad;

            seglength = nextseglength;
            pprevlength = prevlength;
            prevlength = length;
            length += seglength;

            if (pts + 2 >= end)
                break;

            do
            {
                pts += 2;
                WorldToScreenPoint(pts[0], pts[1], next.x, next.y);
                /* TODO: Do this computation only once (nextlength?) */
                dv.x = next.x - cur.x;
                dv.y = next.y - cur.y;
                nextseglength = sqrt(dv.x*dv.x + dv.y*dv.y);
                /* TODO: do something more (simplify/smooth the line ahead of time?),
                         and handle this case for the first few points. */
            } while (nextseglength < PXF_SEG_DELTA && pts < end);
            /* TODO: if the last points in a line are coincident, suboptimal results will occur */
        }

        if (npxforms > 0 && drawpos < length)
        {
            if (drawpos + width > length)
            {   /* TODO: HACKHACK */
                if (npxforms >= pxfbuflen)
                {
                    pxfbuflen *= 2;
                    SE_PiecewiseTransform** newbuf = new SE_PiecewiseTransform*[pxfbuflen];
                    memcpy(newbuf, pxforms, npxforms*sizeof(SE_PiecewiseTransform*));
                    if (npxforms != PXF_BUF_LEN)
                        delete[] pxforms;
                    pxforms = newbuf;
                }
                for (int i = npxforms; i > 0; i--)
                    pxforms[i] = pxforms[i-1];
                npxforms++;
                RS_F_Point v0(0.0, 0.0), v1(1.0, 0.0);
                pxforms[0] = new SE_IdentityJoin(bounds, bounds.minx, v0, v1, -DBL_MAX, bounds.minx + length - drawpos);
            }

            if (style->drawLast)
                AddLabelJoin(geometry, style, pxforms, npxforms);
            else
            {
                DrawSymbolJoin(style->symbol, pxforms, npxforms);
                if (style->addToExclusionRegions)
                    AddExclusionRegionJoin(pxforms, npxforms);
            }
            drawpos += increment;

            while (drawpos < prevlength + prev_post_rad && drawpos < length)
            {
                /* TODO: handle chopping in this case */
                for (int i = 0; i < npxforms - 1; i++)
                    delete pxforms[i];

                if (npxforms > 1)
                {
                    pxforms[0] = new SE_IdentityJoin(bounds, drawpos - pprevlength, pprev, prev, -DBL_MAX, length - pprevlength);
                    pxforms[1] = curpxf;
                    npxforms = 2;
                }
                else
                {
                    SE_Matrix updatexf(1.0, 0.0, increment, 0.0, 1.0, 0.0);
                    pxforms[1] = curpxf;
                    pxforms[1]->ApplyPreTransform(updatexf);
                    RS_F_Point v0(0.0, 0.0), v1(1.0, 0.0);
                    pxforms[0] = new SE_IdentityJoin(bounds, bounds.minx, v0, v1, -DBL_MAX, bounds.minx + length - drawpos);
                    npxforms = 2;
                }

                if (style->drawLast)
                    AddLabelJoin(geometry, style, pxforms, npxforms);
                else
                {
                    DrawSymbolJoin(style->symbol, pxforms, npxforms);
                    if (style->addToExclusionRegions)
                        AddExclusionRegionJoin(pxforms, npxforms);
                }
                drawpos += increment;
            }
        }

        for (int i = 0; i < npxforms; i++)
            delete pxforms[i];

        while (drawpos < length)
        {
            /* TODO: fix chopping and change next false to true */
            SE_IdentityJoin idj = SE_IdentityJoin(bounds, drawpos - prevlength, prev, cur, -DBL_MAX, seglength);
            SE_PiecewiseTransform* pxform = &idj;
            if (style->drawLast)
                AddLabelJoin(geometry, style, &pxform, 1);
            else
            {
                DrawSymbolJoin(style->symbol, &pxform, 1);
                if (style->addToExclusionRegions)
                    AddExclusionRegionJoin(&pxform, 1);
            }
            drawpos += increment;
        }

        /* TODO: endcap */

        if (pxfbuflen != PXF_BUF_LEN)
            delete[] pxforms;
    }
}


void SE_Renderer::DrawSymbolJoin(SE_RenderPrimitiveList& symbol, SE_PiecewiseTransform** xforms, int nxforms)
{
    SE_Matrix identity;

    for (unsigned i = 0; i < symbol.size(); i++)
    {
        SE_RenderPrimitive* primitive = symbol[i];

        if (primitive->type == SE_RenderPolygonPrimitive || primitive->type == SE_RenderPolylinePrimitive)
        {
            SE_RenderPolyline* pl = (SE_RenderPolyline*)primitive;

            if (m_bSelectionMode)
            {
                if (primitive->type == SE_RenderPolygonPrimitive)
                {
                    SE_LineStorage* inst = pl->geometry->TransformInstance(xforms, nxforms, true);
                    DrawScreenPolygon((LineBuffer*)inst, &identity, m_selFill);
                    inst->Free();
                }

                SE_LineStorage* inst = pl->geometry->TransformInstance(xforms, nxforms, false);
                DrawScreenPolyline((LineBuffer*)inst, &identity, m_selColor, m_selWeight );
                inst->Free();
            }
            else
            {
                if (primitive->type == SE_RenderPolygonPrimitive)
                {
                    SE_LineStorage* inst = pl->geometry->TransformInstance(xforms, nxforms, true);
                    DrawScreenPolygon((LineBuffer*)inst, &identity, ((SE_RenderPolygon*)primitive)->fill);
                    inst->Free();
                }

                SE_LineStorage* inst = pl->geometry->TransformInstance(xforms, nxforms, false);
                DrawScreenPolyline((LineBuffer*)inst, &identity, pl->color, pl->weight);
                inst->Free();
            }
        }
    }
}


void SE_Renderer::AddLabelJoin(LineBuffer* /*geom*/, SE_RenderStyle* /*style*/, SE_PiecewiseTransform** /*xforms*/, int /*nxforms*/)
{
    /* TODO */
}


void SE_Renderer::AddExclusionRegionJoin(SE_PiecewiseTransform** /*xforms*/, int /*nxforms*/)
{
    /* TODO */
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
            && lb->points()[1] == 0.0
            && lb->points()[3] == 0.0)
        {
            //now make sure it is not a dashed line by comparing the
            //single segment to the symbol repeat
            double len = lb->points()[2] - lb->points()[0];

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

    int ptindex = 0;

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
        int ptcount = featGeom->cntrs()[j];
        double* pts = featGeom->points() + 2*ptindex;

        //pixel position along the current segment of the polyline
        double drawpos = style->startOffset;

        int cur_seg = 0;

        while (cur_seg < ptcount - 1)
        {
            symxf.setIdentity();

            //current line segment
            double* seg = pts + cur_seg * 2;
            double seg_screen[4];

            //transform segment from mapping to screen space
            WorldToScreenPoint(seg[0], seg[1], seg_screen[0], seg_screen[1]);
            WorldToScreenPoint(seg[2], seg[3], seg_screen[2], seg_screen[3]);

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
            cur_seg++;
        }

        ptindex += ptcount;
    }
}


void SE_Renderer::ProcessArea(SE_ApplyContext* /*ctx*/, SE_RenderAreaStyle* /*style*/)
{
}


void SE_Renderer::DrawSymbol(SE_RenderPrimitiveList& symbol, const SE_Matrix& posxform, double angleRad)
{
    for (unsigned i = 0; i < symbol.size(); i++)
    {
        SE_RenderPrimitive* primitive = symbol[i];

        if (primitive->type == SE_RenderPolygonPrimitive || primitive->type == SE_RenderPolylinePrimitive)
        {
            SE_RenderPolyline* pl = (SE_RenderPolyline*)primitive;

            LineBuffer* geometry = pl->geometry->xf_buffer();

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

            DrawScreenText(tp->text, tdef, x, y, NULL, 0, 0.0);
        }
        else if (primitive->type == SE_RenderRasterPrimitive)
        {
            SE_RenderRaster* rp = (SE_RenderRaster*)primitive;

            if (rp->pngPtr != NULL)
            {
                double x, y;
                posxform.transform(rp->position[0], rp->position[1], x, y);
                double angleDeg = (rp->angleRad + angleRad) / M_PI180;

                DrawScreenRaster((unsigned char*)rp->pngPtr, rp->pngSize, RS_ImageFormat_PNG, -1, -1, x, y, rp->extent[0], rp->extent[1], angleDeg);
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

                dt->text        = st->text;
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

                dr->pngPtr      = sr->pngPtr; // this pointer is managed/cached by the SE_SymbolManager
                dr->pngSize     = sr->pngSize;
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
