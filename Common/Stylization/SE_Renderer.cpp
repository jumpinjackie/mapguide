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

SE_Renderer::SE_Renderer()
: m_bSelectionMode(false)
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

void SE_Renderer::SetLineBufferPool(SE_LineBufferPool* pool)
{
    m_lbp = pool;
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

void SE_Renderer::ProcessPoint(LineBuffer* geometry, SE_RenderPointStyle* style)
{
    SE_Matrix xform;

    /* Render the points */
    for (int i = 0; i < geometry->point_count(); i++)
    {
        xform.setIdentity();
        xform.translate(geometry->points()[2*i], geometry->points()[2*i+1]);
        double angle = 0;//TODO: angle needs to be added to the RenderPointStyle
        if (style->drawLast)
            AddLabel(geometry, style, xform, 0);
        else
        {
            DrawSymbol(style->symbol, xform, angle);

            double angle = 0;
            if (style->addToExclusionRegions)
                AddExclusionRegion(style, xform, angle);
        }
    }
}

void SE_Renderer::ProcessLine(LineBuffer* geometry, SE_RenderLineStyle* style)
{
    //determine if the style is a simple straight solid line
    SE_RenderSymbol& rs = style->symbol;

    //check if it is a single symbol that is not a label participant
    if (rs.size() == 1 
        && rs[0]->type == SE_PolylinePrimitive 
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
            
            if (fabs(len - style->repeat) < 0.001) //repeat must be within 1/1000 of a pixel for us to assume solid line
                                                   //this is only to avoid FP precision issues, in reality they would be exactly equal
            {
                //ok, it's only a solid line, just draw it and bail out of the
                //layout function
                DrawScreenPolyline(geometry, rp->color, rp->weight);
                return;
            }
        }
    }
    
    SE_Matrix symxf;
    
    int ptindex = 0;

    //convert increment to pixels
    double increment = style->repeat; //TODO: is this already scaled by the mm to pixel scale?

    bool fromAngle = (wcscmp(L"FromAngle", style->angleControl) == 0);

    for (int j=0; j<geometry->cntr_count(); j++)
    {
        //current polyline
        int ptcount = geometry->cntrs()[j];
        double* pts = geometry->points() + 2*ptindex;

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
                double slope = atan2(dy, dx);
                double dx_incr = cos(slope);
                double dy_incr = sin(slope);

                double symrot = fromAngle? style->angle : slope;
                double tx = seg[0] + dx_incr * drawpos;
                double ty = seg[1] + dy_incr * drawpos;

                symxf.rotate(symrot);
                symxf.translate(tx, ty);
                dx_incr *= increment;
                dy_incr *= increment;

                //loop-draw the symbol along the current segment,
                //moving along by increment pixels
                while (drawpos < len)
                {                    
                    if (style->drawLast)
                        AddLabel(geometry, style, symxf, symrot);
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

void SE_Renderer::ProcessArea(LineBuffer* /*geometry*/, SE_RenderAreaStyle* /*style*/)
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

            LineBuffer* geometry = pl->geometry->TransformInstance(posxform);

            if (m_bSelectionMode)
            {
                if (primitive->type == SE_PolygonPrimitive)
                    DrawScreenPolygon( geometry, m_selFill);

                DrawScreenPolyline( geometry, m_selColor, m_selWeight );
            }
            else
            {
                if (primitive->type == SE_PolygonPrimitive)
                    DrawScreenPolygon( geometry, ((SE_RenderPolygon*)primitive)->fill );

                DrawScreenPolyline( geometry, pl->color, pl->weight );
            }
        }
        else if (primitive->type == SE_TextPrimitive)
        {
            SE_RenderText* tp = (SE_RenderText*)primitive;

            //TODO take into account rotation if drawing along a line and
            //the angle control is "from geometry"
            double x, y;
            posxform.transform(tp->position[0], tp->position[1], x, y);

            if (m_bSelectionMode)
            {
                RS_TextDef tdef = tp->tdef;
                tdef.color() = m_textForeColor;
                tdef.bgcolor() = m_textBackColor;
                if (anglerad != 0)
                    tdef.rotation() = anglerad / M_PI180;
                DrawScreenText(tp->text, tdef, x, y, NULL, 0, 0.0);
            }
            else if (anglerad != 0.0)
            {
                RS_TextDef tdef = tp->tdef;
                tdef.rotation() = anglerad / M_PI180;
                DrawScreenText(tp->text, tdef, x, y, NULL, 0, 0.0);
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

            DrawScreenRaster(rp->pngPtr, rp->pngSize, RS_ImageFormat_PNG, -1, -1, x, y, rp->extent[0], rp->extent[1], anglerad / M_PI180);
        }
    }
}


void SE_Renderer::AddLabel(LineBuffer* geom, SE_RenderStyle* style, SE_Matrix& xform, double angle)
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
