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
#include "SE_Renderer.h"
#include "SE_AreaPositioning.h"
#include "RS_FontEngine.h"
#include "SE_SymbolDefProxies.h"

using namespace MDFMODEL_NAMESPACE;


///////////////////////////////////////////////////////////////////////////////
SE_Renderer::SE_Renderer()
: m_pPool(NULL)
, m_bSelectionMode(false)
, m_selFillColor(0)
, m_textForeColor(0)
, m_textBackColor(0)
, m_rasterGridSize(100)
, m_minRasterGridSize(10)
, m_rasterGridSizeOverrideRatio(0.25)
{
}


///////////////////////////////////////////////////////////////////////////////
SE_Renderer::~SE_Renderer()
{
}


///////////////////////////////////////////////////////////////////////////////
int SE_Renderer::GetRasterGridSize()
{
    return m_rasterGridSize;
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::SetRasterGridSize(int size)
{
    m_rasterGridSize = size;
}


///////////////////////////////////////////////////////////////////////////////
int SE_Renderer::GetMinRasterGridSize()
{
    return m_minRasterGridSize;
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::SetMinRasterGridSize(int size)
{
    m_minRasterGridSize = size;
}


///////////////////////////////////////////////////////////////////////////////
double SE_Renderer::GetRasterGridSizeOverrideRatio()
{
    return m_rasterGridSizeOverrideRatio;
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::SetRasterGridSizeOverrideRatio(double ratio)
{
    m_rasterGridSizeOverrideRatio = ratio;
}

///////////////////////////////////////////////////////////////////////////////
double SE_Renderer::GetMaxRasterImageWidth()
{
    return m_maxRasterImageWidth;
}

///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::SetMaxRasterImageWidth(int width)
{
    m_maxRasterImageWidth = width;
}

///////////////////////////////////////////////////////////////////////////////
double SE_Renderer::GetMaxRasterImageHeight()
{
    return m_maxRasterImageHeight;
}

///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::SetMaxRasterImageHeight(int height)
{
    m_maxRasterImageHeight = height;
}

///////////////////////////////////////////////////////////////////////////////
bool SE_Renderer::SupportsTooltips()
{
    // by default, set the renderer to process tooltip expressions
    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool SE_Renderer::SupportsHyperlinks()
{
    // by default, set the renderer to process hyperlink expressions
    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool SE_Renderer::RequiresCompositeLineStyleSeparation()
{
    // by default, set the renderer to separate composite line styles
    return true;
}


///////////////////////////////////////////////////////////////////////////////
SE_BufferPool* SE_Renderer::GetBufferPool()
{
    return m_pPool;
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::SetBufferPool(SE_BufferPool* pool)
{
    m_pPool = pool;
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::SetRenderSelectionMode(bool mode)
{
    SetRenderSelectionMode(mode, 0x0000FFFF);
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::SetRenderSelectionMode(bool mode, int rgba)
{
    m_bSelectionMode = mode;

    if (mode)
    {
        // set the default selection style - 1mm line weight, partially transparent blue
        int rgb0 = rgba & 0xFFFFFF00;

        m_selLineStroke.weight = 3.0;  // should be 1 to give 1mm, but the renderer is way off
        m_selLineStroke.color = RS_Color(rgb0 | 200).argb();

        RS_Color selFillColor(rgba & 0xFFFFFFFF);
        //For backward compatibility, only do the override if we have a fully opaque alpha component, otherwise respect the value defined
        if (selFillColor.alpha() == 255)
            selFillColor = RS_Color((rgba & 0xFFFFFF00) | 160);

        m_selFillColor  = selFillColor.argb();
        m_textForeColor = RS_Color(rgb0 | 200);
        m_textBackColor = RS_Color(rgb0 | 255);
    }
}


///////////////////////////////////////////////////////////////////////////////
// Returns the viewport rotation angle, in radians CCW.
double SE_Renderer::GetWorldToScreenRotation()
{
    // default is no rotation
    return 0.0;
}


///////////////////////////////////////////////////////////////////////////////
// Called when applying a point style on a feature geometry.  Point styles can
// be applied to all feature geometry types.
void SE_Renderer::ProcessPoint(SE_ApplyContext* ctx, SE_RenderPointStyle* style, RS_Bounds* bounds)
{
    // the feature geometry we're applying the style on...
    LineBuffer* featGeom = ctx->geometry;

    double angleRad = 0.0;
    if (style->angleControl == SE_AngleControl_FromGeometry)
    {
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

    angleRad += style->angleRad;

    // also account for any viewport rotation
    angleRad += GetWorldToScreenRotation();

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

        // transform to screen space - feature geometry is in [the original] mapping space
        WorldToScreenPoint(x, y, x, y);

        xform = xformbase;
        xform.translate(x, y);

        if (style->drawLast)
            AddLabel(featGeom, style, xform, angleRad);
        else
            DrawSymbol(style->symbol, xform, angleRad, style->addToExclusionRegion);
    }

    if (bounds)
    {
        // get the symbol bounds after applying the transforms
        bounds->minx = bounds->miny = +DBL_MAX;
        bounds->maxx = bounds->maxy = -DBL_MAX;
        for (int i=0; i<4; ++i)
        {
            RS_F_Point xfpt;
            xformbase.transform(style->bounds[i].x, style->bounds[i].y, xfpt.x, xfpt.y);
            bounds->add_point(xfpt);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Called when applying a line style on a feature geometry.  Line styles can
// only be applied to linestring and polygon feature geometry types.
void SE_Renderer::ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style)
{
    // the feature geometry we're applying the style on...
    LineBuffer* featGeom = ctx->geometry;

    // can't apply a line style to point geometry types
    switch (featGeom->geom_type())
    {
        case FdoGeometryType_Point:
        case FdoGeometryType_MultiPoint:
            return;
    }

    //--------------------------------------------------------------
    // special code to handle simple straight solid line styles
    //--------------------------------------------------------------

    if (style->solidLine)
    {
        // just draw it and bail out of the layout function
        SE_RenderPolyline* rp = (SE_RenderPolyline*)style->symbol[0];

        SE_Matrix w2s;
        GetWorldToScreenTransform(w2s);
        if (m_bSelectionMode)
        {
            m_selLineStroke.cap        = rp->lineStroke.cap;
            m_selLineStroke.join       = rp->lineStroke.join;
            m_selLineStroke.miterLimit = rp->lineStroke.miterLimit;
            DrawScreenPolyline(featGeom, &w2s, m_selLineStroke);
        }
        else
            DrawScreenPolyline(featGeom, &w2s, rp->lineStroke);
        return;
    }

    //--------------------------------------------------------------
    // handle the case repeat <= 0 - here we ignore vertex control
    //--------------------------------------------------------------

    if (style->repeat <= 0.0)
    {
        // this can be handled using the overlap direct algorithm with:
        // - repeat set to larger than each contour length
        // - vertex angle limit set to >180 degrees
        double old_val = style->vertexAngleLimit;
        double old_rep = style->repeat;
        style->vertexAngleLimit = M_PI + 1.0;   // any value greater than M_PI
        style->repeat = DBL_MAX;

        ProcessLineOverlapDirect(featGeom, style);

        style->vertexAngleLimit = old_val;
        style->repeat = old_rep;
        return;
    }

    //--------------------------------------------------------------
    // check the vertex control type and call the appropriate helper
    //--------------------------------------------------------------

    if (style->vertexControl == SE_VertexControl_OverlapNone)
        ProcessLineOverlapNone(featGeom, style);
    else if (style->vertexControl == SE_VertexControl_OverlapDirect)
        ProcessLineOverlapDirect(featGeom, style);
    else
        ProcessLineOverlapWrap(featGeom, style);
}


///////////////////////////////////////////////////////////////////////////////
// Called when applying an area style on a feature geometry.  Area styles can
// can only be applied to polygon feature geometry types.
void SE_Renderer::ProcessArea(SE_ApplyContext* ctx, SE_RenderAreaStyle* style)
{
    // the feature geometry we're applying the style on...
    LineBuffer* featGeom = ctx->geometry;

    // can't apply an area style to point and linestring geometry types
    switch (featGeom->geom_type())
    {
        case FdoGeometryType_Point:
        case FdoGeometryType_MultiPoint:
        case FdoGeometryType_LineString:
        case FdoGeometryType_MultiLineString:
        case FdoGeometryType_CurveString:
        case FdoGeometryType_MultiCurveString:
            return;
    }

    SE_Matrix w2s;
    GetWorldToScreenTransform(w2s);

    //--------------------------------------------------------------
    // special code to handle simple solid fill styles
    //--------------------------------------------------------------

    if (style->solidFill)
    {
        // just draw it and bail out of the layout function
        SE_RenderPolygon* rp = (SE_RenderPolygon*)style->symbol[0];

        if (m_bSelectionMode)
            DrawScreenPolygon(featGeom, &w2s, m_selFillColor);
        else
            DrawScreenPolygon(featGeom, &w2s, rp->fill);
        return;
    }

    // transform the feature geometry to rendering space
    LineBuffer* xfgeom = LineBufferPool::NewLineBuffer(m_pPool, featGeom->point_count());
    std::auto_ptr<LineBuffer> spLB(xfgeom);
    *xfgeom = *featGeom;

    int size = featGeom->point_count();
    for (int i=0; i<size; ++i)
        w2s.transform(xfgeom->x_coord(i), xfgeom->y_coord(i));

    // recompute the bounds
    RS_Bounds& bounds = const_cast<RS_Bounds&>(xfgeom->bounds());
    bounds.minx = bounds.miny = bounds.minz = +DBL_MAX;
    bounds.maxx = bounds.maxy = bounds.maxz = -DBL_MAX;
    xfgeom->ComputeBounds(bounds);

    // account for any viewport rotation
    SE_AreaPositioning ap(xfgeom, style, GetWorldToScreenRotation());
    double baserot = ap.PatternRotation();

    SE_Matrix xform;
    SE_Matrix xformbase = *ctx->xform;
    xformbase.rotate(baserot);

    for (const Point2D* pos = ap.NextLocation(); pos != NULL; pos = ap.NextLocation())
    {
        xform = xformbase;
        xform.translate(pos->x, pos->y);
        DrawSymbol(style->symbol, xform, baserot, style->addToExclusionRegion);
    }

    LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::DrawSymbol(SE_RenderPrimitiveList& symbol,
                             const SE_Matrix& xform,
                             double angleRad,
                             bool excludeRegion)
{
    RS_Bounds extents(DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX);
    unsigned int nprims = symbol.size();

    for (unsigned int i=0; i<nprims; ++i)
    {
        SE_RenderPrimitive* primitive = symbol[i];

        if (primitive->type == SE_RenderPrimitive_Polygon || primitive->type == SE_RenderPrimitive_Polyline)
        {
            SE_RenderPolyline* rp = (SE_RenderPolyline*)primitive;

            LineBuffer* lb = rp->geometry->xf_buffer();

            // update the extents with this primitive
            RS_Bounds lbnds;
            lb->ComputeBounds(lbnds);
            extents.add_bounds(lbnds);

            if (m_bSelectionMode)
            {
                if (primitive->type == SE_RenderPrimitive_Polygon)
                    DrawScreenPolygon(lb, &xform, m_selFillColor);

                m_selLineStroke.cap        = rp->lineStroke.cap;
                m_selLineStroke.join       = rp->lineStroke.join;
                m_selLineStroke.miterLimit = rp->lineStroke.miterLimit;
                DrawScreenPolyline(lb, &xform, m_selLineStroke);
            }
            else
            {
                if (primitive->type == SE_RenderPrimitive_Polygon)
                    DrawScreenPolygon(lb, &xform, ((SE_RenderPolygon*)primitive)->fill);

                DrawScreenPolyline(lb, &xform, rp->lineStroke);
            }
        }
        else if (primitive->type == SE_RenderPrimitive_Text)
        {
            SE_RenderText* tp = (SE_RenderText*)primitive;

            // update the extents with this primitive's bounds
            for (int j=0; j<4; ++j)
                extents.add_point(primitive->bounds[j]);

            // get position and angle to use
            double x, y;
            xform.transform(tp->position[0], tp->position[1], x, y);
            RS_TextDef tdef = tp->tdef;
            tdef.rotation() += angleRad * M_180PI;

            if (m_bSelectionMode)
            {
                tdef.textcolor()   = m_textForeColor;
                tdef.ghostcolor()  = m_textBackColor;
//              tdef.framecolor()  = m_textBackColor;
//              tdef.opaquecolor() = m_textBackColor;
            }

            // Here we cannot use the cached RS_TextMetrics in the SE_RenderText object.
            // We must recalculate the text metrics with the new tdef before we can call DrawScreenText.
            RS_TextMetrics tm;
            if (this->GetRSFontEngine()->GetTextMetrics(tp->content, tdef, tm, false))
                DrawScreenText(tm, tdef, x, y, NULL, 0, 0.0);
        }
        else if (primitive->type == SE_RenderPrimitive_Raster)
        {
            SE_RenderRaster* rp = (SE_RenderRaster*)primitive;

            if (m_bSelectionMode)
            {
                // if the raster symbol is selected, then draw the mask selection polygon only
                LineBuffer *lb = LineBufferPool::NewLineBuffer(m_pPool, 5);
                std::auto_ptr<LineBuffer> spLB(lb);

                lb->MoveTo(rp->bounds[3].x, rp->bounds[3].y);
                for (int i = 0; i < 4; ++i)
                {
                    lb->LineTo(rp->bounds[i].x, rp->bounds[i].y);
                }
                
                DrawScreenPolygon(lb, &xform, m_selFillColor);
                DrawScreenPolyline(lb, &xform, m_selLineStroke);

                LineBufferPool::FreeLineBuffer(m_pPool, spLB.release());
            }
            else 
            {
                ImageData& imgData = rp->imageData;
                if (imgData.data != NULL)
                {
                    // update the extents with this primitive's bounds
                    for (int j=0; j<4; ++j)
                        extents.add_point(primitive->bounds[j]);

                    // get position and angle to use
                    double x, y;
                    xform.transform(rp->position[0], rp->position[1], x, y);
                    double angleDeg = (rp->angleRad + angleRad) * M_180PI;

                    DrawScreenRaster(imgData.data, imgData.size, imgData.format, imgData.width, imgData.height, x, y, rp->extent[0], rp->extent[1], angleDeg, rp->opacity);
                }
            }
        }
    }

    if (nprims > 0)
    {
        // always compute the last symbol extent
        xform.transform(extents.minx, extents.miny, m_lastSymbolExtent[0].x, m_lastSymbolExtent[0].y);
        xform.transform(extents.maxx, extents.miny, m_lastSymbolExtent[1].x, m_lastSymbolExtent[1].y);
        xform.transform(extents.maxx, extents.maxy, m_lastSymbolExtent[2].x, m_lastSymbolExtent[2].y);
        xform.transform(extents.minx, extents.maxy, m_lastSymbolExtent[3].x, m_lastSymbolExtent[3].y);

        if (excludeRegion)
            AddExclusionRegion(m_lastSymbolExtent, 4);
    }
    else
    {
        // symbol contains no primitives - update last symbol extent assuming
        // zero symbol extent, but don't add any exclusion region
        xform.transform(0.0, 0.0, m_lastSymbolExtent[0].x, m_lastSymbolExtent[0].y);
        m_lastSymbolExtent[1].x = m_lastSymbolExtent[2].x = m_lastSymbolExtent[3].x = m_lastSymbolExtent[0].x;
        m_lastSymbolExtent[1].y = m_lastSymbolExtent[2].y = m_lastSymbolExtent[3].y = m_lastSymbolExtent[0].y;
    }
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::AddLabel(LineBuffer* geom, SE_RenderStyle* style, const SE_Matrix& xform, double angleRad)
{
    // clone the SE_RenderStyle so that the label renderer can keep track
    // of it until the end of rendering when it draws all the labels
    // TODO: cloning is bad
    SE_RenderStyle* clonedStyle = CloneRenderStyle(style);

    SE_LabelInfo info(xform.x2, xform.y2, RS_Units_Device, angleRad, clonedStyle);
    RS_OverpostType overpostType = style->checkExclusionRegion? RS_OverpostType_AllFit : RS_OverpostType_All;
    ProcessSELabelGroup(&info, 1, overpostType, style->addToExclusionRegion, geom);
}


///////////////////////////////////////////////////////////////////////////////
const RS_F_Point* SE_Renderer::GetLastSymbolExtent()
{
    return m_lastSymbolExtent;
}


///////////////////////////////////////////////////////////////////////////////
// cloning of render styles - unfortunate but necessary for delay-drawing labels
SE_RenderStyle* SE_Renderer::CloneRenderStyle(SE_RenderStyle* symbol)
{
    SE_RenderStyle* ret = NULL;

    // first determine what kind of style it is and copy all the
    // style specific properties
    switch (symbol->type)
    {
    case SE_RenderStyle_Point:
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

    case SE_RenderStyle_Line:
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
            dls->dpLineStroke     = sls->dpLineStroke;
            dls->solidLine        = sls->solidLine;
        }
        break;

    case SE_RenderStyle_Area:
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
    ret->addToExclusionRegion = symbol->addToExclusionRegion;
    ret->checkExclusionRegion = symbol->checkExclusionRegion;
    ret->drawLast             = symbol->drawLast;
    ret->renderPass           = symbol->renderPass;

    // copy the graphics for the symbol
    for (size_t i=0; i<symbol->symbol.size(); ++i)
    {
        SE_RenderPrimitive* rp = symbol->symbol[i];
        SE_RenderPrimitive* rpc = NULL;

        switch (rp->type)
        {
        case SE_RenderPrimitive_Polygon:
            {
                SE_RenderPolygon* sp = (SE_RenderPolygon*)rp;
                SE_RenderPolygon* dp = new SE_RenderPolygon();
                rpc = dp;

                dp->fill = sp->fill;
            }
            // fall through

        case SE_RenderPrimitive_Polyline:
            {
                SE_RenderPolyline* sp = (SE_RenderPolyline*)rp;
                if (!rpc)
                    rpc = new SE_RenderPolyline();
                SE_RenderPolyline* dp = (SE_RenderPolyline*)rpc;

                // clone the geometry, but do NOT let the clone keep a pointer to
                // the buffer pool since labels are rendered much later and by then
                // the pool may already be destroyed
                dp->geometry   = sp->geometry->Clone(false);
                dp->lineStroke = sp->lineStroke;
            }
            break;

        case SE_RenderPrimitive_Text:
            {
                SE_RenderText* st = (SE_RenderText*)rp;
                SE_RenderText* dt = new SE_RenderText();
                rpc = dt;

                dt->expression  = st->expression;
                dt->content     = st->content;
                dt->position[0] = st->position[0];
                dt->position[1] = st->position[1];
                dt->tdef        = st->tdef;

                // don't copy the text metrics - they are recalculated
            }
            break;

        case SE_RenderPrimitive_Raster:
            {
                SE_RenderRaster* sr = (SE_RenderRaster*)rp;
                SE_RenderRaster* dr = new SE_RenderRaster();
                rpc = dr;

                // image data pointer is managed/cached by the SE_SymbolManager
                dr->imageData   = sr->imageData;
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


//////////////////////////////////////////////////////////////////////////////
// Indicates whether rendering optimization is used by this renderer.  For
// example, if we are rendering text and optimization is turned on, then
// text is rendered as a simple line when it is very small.  By default,
// optimization is turned on.  For other behavior, this method must be
// overridden.
bool SE_Renderer::OptimizeGeometry()
{
    return true;
}
