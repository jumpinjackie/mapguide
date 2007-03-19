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

//cloning of RenderSymbols. Unfortunate but nexessary for delay-drawing labels
static SE_RenderStyle* CloneRenderStyle(SE_RenderStyle* symbol)
{
    SE_RenderStyle* ret = NULL;

    //first determine what kind of style it is and copy all the 
    //style specific properties
    switch (symbol->type)
    {
    case SE_RenderPointStyleType:
        {
        SE_RenderPointStyle* dps = new SE_RenderPointStyle();
        ret = dps;
        }
        break;
    case SE_RenderLineStyleType:
        {
            SE_RenderLineStyle* dls = new SE_RenderLineStyle();
            SE_RenderLineStyle* sls = (SE_RenderLineStyle*)symbol;
            ret = dls;
            dls->angle = sls->angle;
            dls->angleControl = sls->angleControl;
            dls->endOffset = sls->endOffset;
            dls->repeat = sls->repeat;
            dls->startOffset = sls->startOffset;
            dls->unitsControl = sls->unitsControl;
            dls->vertexAngleLimit = sls->vertexAngleLimit;
            dls->vertexControl = sls->vertexControl;
        }
        break;
    case SE_RenderAreaStyleType:
        {
            SE_RenderAreaStyle* das = new SE_RenderAreaStyle();
            SE_RenderAreaStyle* sas = (SE_RenderAreaStyle*)symbol;
            ret = das;
            das->angle = sas->angle;
            das->angleControl = sas->angleControl;
            das->bufferWidth = sas->bufferWidth;
            das->clippingControl = sas->clippingControl;
            das->origin[0] = sas->origin[0];
            das->origin[1] = sas->origin[1];
            das->originControl = sas->originControl;
            das->repeat[0] = sas->repeat[0];
            das->repeat[1] = sas->repeat[1];
        }
    default:
        break;
    }

    //copy all the common properties
    ret->addToExclusionRegions = symbol->addToExclusionRegions;
    ret->bounds = symbol->bounds->Clone();
    ret->checkExclusionRegions = symbol->checkExclusionRegions;
    ret->drawLast = symbol->drawLast;
    ret->renderPass = symbol->renderPass;

    //copy the graphics for the symbol
    for (size_t i=0; i<symbol->symbol.size(); i++)
    {
        SE_RenderPrimitive* rp = symbol->symbol[i];
        SE_RenderPrimitive* rpc = NULL;

        switch (rp->type)
        {
        case SE_RenderPolygonPrimitive:
            rpc = new SE_RenderPolygon();
            ((SE_RenderPolygon*)rpc)->fill = ((SE_RenderPolygon*)rp)->fill;
        case SE_RenderPolylinePrimitive:
            {
                if (!rpc) rpc = new SE_RenderPolyline();
                SE_RenderPolyline* drp = (SE_RenderPolyline*)rpc;
                SE_RenderPolyline* srp = (SE_RenderPolyline*)rp;
                drp->bounds = srp->bounds->Clone();
                drp->color = srp->color;
                drp->resize = srp->resize;
                drp->weight = srp->weight;
                drp->geometry = srp->geometry->Clone();
            }
            break;
        case SE_RenderTextPrimitive:
            {
                rpc = new SE_RenderText();
                SE_RenderText* st = (SE_RenderText*)rp;
                SE_RenderText* dt = (SE_RenderText*)rpc;

                dt->bounds = st->bounds->Clone();
                dt->position[0] = st->position[0];
                dt->position[1] = st->position[1];
                dt->resize = st->resize;
                dt->tdef = st->tdef;
                dt->text = st->text;
            }
            break;
        case SE_RenderRasterPrimitive:
            {
                rpc = new SE_RenderRaster();
                SE_RenderRaster* sr = (SE_RenderRaster*)rp;
                SE_RenderRaster* dr = (SE_RenderRaster*)rpc;

                dr->angle = sr->angle;
                dr->bounds = sr->bounds->Clone();
                dr->extent[0] = sr->extent[0];
                dr->extent[1] = sr->extent[1];
                dr->pngSize = sr->pngSize;
                dr->pngPtr = sr->pngPtr; //this pointer is managed/cached by the SE_SymbolManager
                dr->position[0] = sr->position[0];
                dr->position[1] = sr->position[1];
                dr->resize = sr->resize;
            }
        }

        if (rpc)
        {
            ret->symbol.push_back(rpc);
        }
    }

    return ret;
}


SE_Renderer::SE_Renderer()
: m_lbp(NULL)
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
        double x = geometry->points()[2*i];
        double y= geometry->points()[2*i+1];

        //transform to screen space -- geometry is in [the original] mapping space
        WorldToScreenPoint(x, y, x, y);
        
        xform.setIdentity();
        xform.translate(x, y);
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
            
            if (fabs(len - style->repeat) < 0.001) //repeat must be within 1/1000 of a pixel for us to assume solid line
                                                   //this is only to avoid FP precision issues, in reality they would be exactly equal
            {
                //ok, it's only a solid line, just draw it and bail out of the
                //layout function
                SE_Matrix m;
                GetWorldToScreenTransform(m);
                DrawScreenPolyline(geometry, &m, rp->color, rp->weight);
                return;
            }
        }
    }
    
    SE_Matrix symxf;
    
    int ptindex = 0;

    //get the increment (the render style already stores this in screen units)
    //TODO - handle case where increment is 0
    double increment = style->repeat;

    bool fromAngle = (wcscmp(L"FromAngle", style->angleControl) == 0);

    for (int j=0; j<geometry->cntr_count(); j++)
    {
        //current polyline
        int ptcount = geometry->cntrs()[j];
        double* pts = geometry->points() + 2*ptindex;

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

                double symrot = fromAngle? style->angle : atan2(dy, dx);
                double tx = seg_screen[0] + dx_incr * drawpos;
                double ty = seg_screen[1] + dy_incr * drawpos;

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

void SE_Renderer::DrawSymbol(SE_RenderPrimitiveList& symbol, const SE_Matrix& posxform, double anglerad)
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
                    DrawScreenPolygon( geometry, &posxform, m_selFill);

                DrawScreenPolyline( geometry, &posxform, m_selColor, m_selWeight );
            }
            else
            {
                if (primitive->type == SE_RenderPolygonPrimitive)
                    DrawScreenPolygon( geometry, &posxform, ((SE_RenderPolygon*)primitive)->fill );

                DrawScreenPolyline( geometry, &posxform, pl->color, pl->weight );
            }
        }
        else if (primitive->type == SE_RenderTextPrimitive)
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
        else if (primitive->type == SE_RenderRasterPrimitive)
        {
            SE_RenderRaster* rp = (SE_RenderRaster*)primitive;

            double x, y;
            posxform.transform(rp->position[0], rp->position[1], x, y);

            DrawScreenRaster((unsigned char*)rp->pngPtr, rp->pngSize, RS_ImageFormat_PNG, -1, -1, x, y, rp->extent[0], rp->extent[1], anglerad / M_PI180);
        }
    }
}


void SE_Renderer::AddLabel(LineBuffer* geom, SE_RenderStyle* style, SE_Matrix& xform, double angle)
{
    //clone the SE_RenderStyle so that the label renderer can keep track of it until 
    //the end of rendering when it draws all the labels
    //TODO: cloning is bad.
    SE_RenderStyle* copied_style = CloneRenderStyle(style);

    SE_LabelInfo info(xform.x2, xform.y2, 0.0, 0.0, RS_Units_Device, angle, copied_style);
    
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
