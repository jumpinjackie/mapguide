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
#include "RS_FontEngine.h"
#include "SE_Bounds.h"

using namespace MDFMODEL_NAMESPACE;

void SE_Renderer::SetLineBufferPool(SE_LineBufferPool* pool)
{
    m_lbp = pool;
}

void SE_Renderer::ProcessPoint(SE_LineBuffer* geometry, SE_RenderPointStyle* style)
{
    SE_Matrix xform;

    SE_Geometry geom;
    geom.n_pts = 1;
    geom.n_cntrs = 0;
    geom.geom_type = geometry->xf_geom_type();

    /* Render the points */
    for (int i = 0; i < geometry->xf_point_cnt(); i++)
    {
        xform.setIdentity();
        xform.translate(geometry->xf_points()[2*i], geometry->xf_points()[2*i+1]);
        double pt[2] = {geometry->xf_points()[2*i], geometry->xf_points()[2*i+1]};
        geom.points = pt;

        double angle = 0;//TODO: angle needs to be added to the RenderPointStyle
        if (style->drawLast)
            AddLabel(&geom, style, xform, angle); 
        else
        {
            DrawSymbol(style->symbol, xform, angle);

            if (style->addToExclusionRegions)
                AddExclusionRegion(style, xform, angle);
        }
    }
}

void SE_Renderer::ProcessLine(SE_LineBuffer* geometry, SE_RenderLineStyle* style)
{
    SE_Matrix symxf;
    SE_Geometry geom;
    geom.points = geometry->xf_points();
    geom.n_pts = geometry->xf_point_cnt();
    geom.contours = geometry->xf_cntrs();
    geom.n_cntrs = geometry->xf_cntr_cnt();
    geom.geom_type = geometry->xf_geom_type();

    int ptindex = 0;

    //convert increment to pixels
    double increment = style->repeat; //TODO: is this already scaled by the mm to pixel scale?

    for (int j=0; j<geom.n_cntrs; j++)
    {
        //current polyline
        int ptcount = geom.contours[j];
        double* pts = geom.points + 2*ptindex;

        //pixel position along the current segment
        //of the polyline
        double drawpos = style->startOffset; //position of symbol along current segment of the polyline 
                                             //TODO: is this already scaled by the mm to pixel scale?

        int cur_seg = 0;

        while (cur_seg < ptcount - 1)
        {
            symxf.setIdentity();

            //current line segment
            double* seg = pts + cur_seg * 2;
            
            //get length
            double dx = seg[2] - seg[0];
            double dy = seg[3] - seg[1];
            double len = sqrt(dx*dx + dy*dy);

            //check if completely skipping current segment since it is smaller than
            //the increment
            if (drawpos < len)
            {
                //compute linear deltas for x and y directions
                // -- we will use these to quickly move along the line
                //without having to do too much math
                double dx_incr, dy_incr;
                double slope;

                slope = atan2(dy, dx);
                dx_incr = cos(slope);
                dy_incr = sin(slope);

                double symrot = wcscmp(L"FromAngle", style->orientation) == 0 ? style->angle : slope;
                symxf.rotate(symrot);
                double tx = seg[0] + dx_incr * drawpos;
                double ty = seg[1] + dy_incr * drawpos;
                symxf.translate(tx, ty);
                dx_incr *= increment;
                dy_incr *= increment;

                //loop-draw the symbol along the current segment,
                //moving along by increment pixels
                while (drawpos < len)
                {                    
                    if (style->drawLast)
                        AddLabel(&geom, style, symxf, symrot);
                    else
                    {
                        DrawSymbol(style->symbol, symxf, symrot);

                        if (style->addToExclusionRegions)
                            AddExclusionRegion(style, symxf, symrot);
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

void SE_Renderer::ProcessArea(SE_LineBuffer* /*geometry*/, SE_RenderAreaStyle* /*style*/)
{
}

void SE_Renderer::DrawSymbol(SE_RenderSymbol& symbol, const SE_Matrix& posxform, double anglerad)
{
    for (unsigned i = 0; i < symbol.size(); i++)
    {
        SE_RenderPrimitive* primitive = symbol[i];

        if (primitive->type == SE_PolygonPrimitive || primitive->type == SE_PolylinePrimitive)
        {
            SE_RenderPolyline* pl = (SE_RenderPolyline*)primitive;

            SE_Geometry geometry;
            pl->geometry->InstanceTransform(posxform, geometry, NULL);

            if (primitive->type == SE_PolygonPrimitive)
                DrawScreenPolygon( geometry, ((SE_RenderPolygon*)primitive)->fill );

                DrawScreenPolyline( geometry, pl->color, pl->weight );
        }
        else if (primitive->type == SE_TextPrimitive)
        {
            SE_RenderText* tp = (SE_RenderText*)primitive;

            //TODO take into account rotation if drawing along a line and
            //the angle control is "from geometry"
            double x, y;
            posxform.transform(tp->position[0], tp->position[1], x, y);

            if (anglerad != 0)
            {
                RS_TextDef tdef2 = tp->tdef;
                tdef2.rotation() = anglerad * (180 / M_PI);
                DrawScreenText(tp->text, tdef2, x, y, NULL, 0, 0.0);
            }
            else
            {
                DrawScreenText(tp->text, tp->tdef, x, y, NULL, 0, 0.0);
            }
        }
        else if (primitive->type == SE_RasterPrimitive)
        {
            SE_RenderRaster* rp = (SE_RenderRaster*)primitive;

            double x, y;
            posxform.transform(rp->position[0], rp->position[1], x, y);

            DrawScreenRaster(rp->pngPtr, rp->pngSize, RS_ImageFormat_PNG, -1, -1, x, y, rp->extent[0], rp->extent[1], anglerad * (180 / M_PI));
        }
     }
}


void SE_Renderer::AddLabel(SE_Geometry* geom, SE_RenderStyle* style, SE_Matrix& xform, double angle)
{
    SE_LabelInfo info(xform.x2, xform.y2, 0.0, 0.0, RS_Units_Device, angle, style);
    
    RS_OverpostType type = RS_OverpostType_AllFit;

    ProcessLabelGroup(&info, 1, type, style->addToExclusionRegions, geom);
}

void SE_Renderer::AddExclusionRegion(SE_RenderStyle* rstyle, SE_Matrix& xform, double angle)
{
    SE_Matrix xform2;
    xform2.setIdentity();
    xform2.rotate(angle);
    xform2.translate(xform.x2, xform.y2);

    RS_F_Point* fpts = m_lastExclusionRegion;

    fpts[0].x = rstyle->bounds->min[0];
    fpts[0].y = rstyle->bounds->min[1];
    fpts[1].x = rstyle->bounds->max[0];
    fpts[1].y = rstyle->bounds->min[1];
    fpts[2].x = rstyle->bounds->max[0];
    fpts[2].y = rstyle->bounds->max[1];
    fpts[3].x = rstyle->bounds->min[0];
    fpts[3].y = rstyle->bounds->max[1];

    for (int i=0; i<4; i++)
        xform2.transform(fpts[i].x, fpts[i].y);

    AddExclusionRegion(fpts, 4);
}
