//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
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
#include "SE_LineRenderer.h"
#include "SE_AreaPositioning.h"
#include "RS_FontEngine.h"

using namespace MDFMODEL_NAMESPACE;


///////////////////////////////////////////////////////////////////////////////
SE_Renderer::SE_Renderer()
: m_bp(NULL)
, m_bSelectionMode(false)
, m_selFillColor(0)
, m_textForeColor(0)
, m_textBackColor(0)
, m_rasterGridSize(100)
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
void SE_Renderer::SetRenderSelectionMode(bool mode)
{
    SetRenderSelectionMode(mode, 0x0000FF00);
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

        m_selFillColor  = RS_Color(rgb0 | 160).argb();
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
            DrawSymbol(style->symbol, xform, angleRad, style->addToExclusionRegions);
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
        SE_LineRenderer::ProcessLineOverlapWrap(this, featGeom, style);
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
    LineBuffer* xfgeom = LineBufferPool::NewLineBuffer(m_bp, featGeom->point_count());
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
        DrawSymbol(style->symbol, xform, baserot, style->addToExclusionRegions);
    }

    LineBufferPool::FreeLineBuffer(m_bp, xfgeom);
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::DrawSymbol(SE_RenderPrimitiveList& symbol,
                             const SE_Matrix& xform,
                             double angleRad,
                             bool excludeRegion)
{
    RS_Bounds extents(DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX);

    for (unsigned i=0; i<symbol.size(); ++i)
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
            if ( this->GetRSFontEngine()->GetTextMetrics( tp->content, tdef, tm, false ))
                DrawScreenText(tm, tdef, x, y, NULL, 0, 0.0);
        }
        else if (primitive->type == SE_RenderPrimitive_Raster)
        {
            SE_RenderRaster* rp = (SE_RenderRaster*)primitive;
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

                DrawScreenRaster(imgData.data, imgData.size, imgData.format, imgData.width, imgData.height, x, y, rp->extent[0], rp->extent[1], angleDeg);
            }
        }
    }

    // always compute the last symbol extent
    xform.transform(extents.minx, extents.miny, m_lastSymbolExtent[0].x, m_lastSymbolExtent[0].y);
    xform.transform(extents.maxx, extents.miny, m_lastSymbolExtent[1].x, m_lastSymbolExtent[1].y);
    xform.transform(extents.maxx, extents.maxy, m_lastSymbolExtent[2].x, m_lastSymbolExtent[2].y);
    xform.transform(extents.minx, extents.maxy, m_lastSymbolExtent[3].x, m_lastSymbolExtent[3].y);

    if (excludeRegion)
        AddExclusionRegion(m_lastSymbolExtent, 4);
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::AddLabel(LineBuffer* geom, SE_RenderStyle* style, const SE_Matrix& xform, double angleRad)
{
    // clone the SE_RenderStyle so that the label renderer can keep track
    // of it until the end of rendering when it draws all the labels
    // TODO: cloning is bad
    SE_RenderStyle* clonedStyle = CloneRenderStyle(style);

    SE_LabelInfo info(xform.x2, xform.y2, RS_Units_Device, angleRad, clonedStyle);
    ProcessSELabelGroup(&info, 1, RS_OverpostType_AllFit, style->addToExclusionRegions, geom);
}


///////////////////////////////////////////////////////////////////////////////
SE_BufferPool* SE_Renderer::GetBufferPool()
{
    return m_bp;
}


///////////////////////////////////////////////////////////////////////////////
void SE_Renderer::SetBufferPool(SE_BufferPool* pool)
{
    m_bp = pool;
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
    ret->addToExclusionRegions = symbol->addToExclusionRegions;
    ret->checkExclusionRegions = symbol->checkExclusionRegions;
    ret->drawLast              = symbol->drawLast;
    ret->renderPass            = symbol->renderPass;

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

                dt->content     = st->content;
                dt->position[0] = st->position[0];
                dt->position[1] = st->position[1];
                dt->tdef        = st->tdef;
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


///////////////////////////////////////////////////////////////////////////////
// This method computes the segment lengths for the geometry.  For a given
// contour with N points starting at index M, the length for the entire
// contour is stored at location M, while the segment lengths are stored at
// locations M+n, n=[1, N-1].
void SE_Renderer::ComputeSegmentLengths(LineBuffer* geometry, double* segLens)
{
    // screen coordinates of current line segment
    double segX0, segY0, segX1, segY1;

    // iterate over the contours
    for (int j=0; j<geometry->cntr_count(); ++j)
    {
        // get segment range for current contour
        int start_seg = geometry->contour_start_point(j);
        int end_seg = geometry->contour_end_point(j);
        int cur_seg = start_seg;

        // compute lengths for the contour and all its segments
        segLens[start_seg] = 0.0;

        // get start point of first segment in screen space
        WorldToScreenPoint(geometry->x_coord(cur_seg), geometry->y_coord(cur_seg), segX0, segY0);

        while (cur_seg < end_seg)
        {
            ++cur_seg;

            // get end point of current segment in screen space
            WorldToScreenPoint(geometry->x_coord(cur_seg), geometry->y_coord(cur_seg), segX1, segY1);

            // get segment length
            double dx = segX1 - segX0;
            double dy = segY1 - segY0;
            double len = sqrt(dx*dx + dy*dy);

            segLens[cur_seg] = len;
            segLens[start_seg] += len;

            // start point for next segment is current end point
            segX0 = segX1;
            segY0 = segY1;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// This method computes group lengths.  The segGroups array is the one
// obtained from ComputeSegmentGroups.
void SE_Renderer::ComputeGroupLengths(double* segLens, int numGroups, int* segGroups, double* groupLens)
{
    for (int j=0; j<numGroups; ++j)
    {
        // get segment range for group
        int seg0 = segGroups[2*j];
        int seg1 = segGroups[2*j+1];

        // get the length of the group
        groupLens[j] = 0.0;
        for (int i=seg0+1; i<=seg1; ++i)
            groupLens[j] += segLens[i];
    }
}


///////////////////////////////////////////////////////////////////////////////
// This method groups together segments for the specified contour based on
// the supplied vertex angle limit.  Any pair of segments are part of the
// same group if their relative angle is less then the vertex angle limit.
// The integer array will contain the indices delineating the segments, i.e.
// group 0 goes from index segGroups[0] to segGroups[1], group 1 goes from
// segGroups[2] to segGroups[3], etc.  Each group is guranteed to start and
// end with a non-generate segment.  The method returns the number of groups.
int SE_Renderer::ComputeSegmentGroups(LineBuffer* geometry, int contour, double vertexAngleLimit, double* segLens, int* segGroups)
{
    // get segment range for specified contour
    int start_seg = geometry->contour_start_point(contour);
    int end_seg = geometry->contour_end_point(contour);

    // skip zero-length contours
    if (segLens[start_seg] == 0.0)
        return 0;

    // we have a non-degenerate contour - we'll get at least one group

    // make sure vertex angle limit is positive and in the range [0, 180]
    vertexAngleLimit = fabs(vertexAngleLimit);
    vertexAngleLimit = rs_min(vertexAngleLimit, M_PI);
    double cosLimit = cos(vertexAngleLimit);

    // screen coordinates of current line segment
    double segX0, segY0, segX1, segY1;

    // keep track of number of groups
    int numGroups = 0;

    // find the initial group's starting segment (the first non-degenerate segment)
    int cur_seg = start_seg + 1;
    while (cur_seg <= end_seg && segLens[cur_seg] == 0.0)
        ++cur_seg;

    int group_min = cur_seg - 1;
    int group_max = cur_seg;

    // get the normalized vector for the segment
    WorldToScreenPoint(geometry->x_coord(cur_seg-1), geometry->y_coord(cur_seg-1), segX0, segY0);
    WorldToScreenPoint(geometry->x_coord(cur_seg  ), geometry->y_coord(cur_seg  ), segX1, segY1);
    double dx0 = (segX1 - segX0) / segLens[cur_seg];
    double dy0 = (segY1 - segY0) / segLens[cur_seg];

    // iterate over the rest of the contour, adding groups as we find them
    while (cur_seg < end_seg)
    {
        ++cur_seg;

        // find next non-degenerate segment
        while (cur_seg <= end_seg && segLens[cur_seg] == 0.0)
            ++cur_seg;

        // no more non-degenerate segments left - done processing the contour
        if (cur_seg > end_seg)
            break;

        // get the normalized vector for the segment
        WorldToScreenPoint(geometry->x_coord(cur_seg-1), geometry->y_coord(cur_seg-1), segX0, segY0);
        WorldToScreenPoint(geometry->x_coord(cur_seg  ), geometry->y_coord(cur_seg  ), segX1, segY1);
        double dx1 = (segX1 - segX0) / segLens[cur_seg];
        double dy1 = (segY1 - segY0) / segLens[cur_seg];

        // compare relative angles between current and previous segments
        double cosAngle = dx0*dx1 + dy0*dy1;
        if (cosAngle < cosLimit)
        {
            // vertex limit exceeded - record the existing group
            segGroups[2*numGroups  ] = group_min;
            segGroups[2*numGroups+1] = group_max;
            ++numGroups;

            // initialize the next group
            group_min = cur_seg - 1;
            group_max = cur_seg;
        }
        else
        {
            // vertex limit not exceeded - extend current group to this segment
            group_max = cur_seg;
        }

        // current normalized vector becomes the old one
        dx0 = dx1;
        dy0 = dy1;
    }

    // record the final group
    segGroups[2*numGroups  ] = group_min;
    segGroups[2*numGroups+1] = group_max;
    ++numGroups;

    return numGroups;
}


///////////////////////////////////////////////////////////////////////////////
// Computes the point distribution for a group, given its start offset, end
// end offset, and repeat.
//
// The following rules apply:
//   - If only StartOffset is specified (>=0) then the first point is at the
//     start offset location, and the distribution repeats until the end of
//     the group.
//   - If only EndOffset is specified (>=0) then the last point is at the end
//     offset location, and the distribution repeats until the start of the
//     group.
//   - If StartOffset and EndOffset are both specified then the first and last
//     points are at the start and end offset locations.  Points are then
//     distributed at the repeat value within the group.  The interior dist-
//     ribution is centered within the start / end offset locations, leaving a
//     gap on either side.  The number of interior points is chosen so that
//     repeat/2 < gap < repeat.  The symbol width is also taken into account
//     when computing the interior distribution.
//   - If StartOffset and EndOffset are both unspecified (< 0) then points
//     are not included at the start and end offset locations, but the interior
//     distribution is the same as in the previous case assuming zero offsets.
void SE_Renderer::ComputeGroupDistribution(double groupLen, double startOffset, double endOffset, double repeat,
                                           double symWidth, double& startPos, double& gap, int& numSymbols)
{
    _ASSERT(repeat > 0.0);
    _ASSERT(startOffset <= groupLen);
    _ASSERT(endOffset <= groupLen);

    // Use a slightly smaller group length to avoid round-off issues when
    // computing the distribution.  See comments below.
    groupLen *= 0.999999999999;

    if (startOffset >= 0.0)
    {
        if (endOffset < 0.0)
        {
            // only the start offset is specified

            // starting symbol
            startPos = startOffset;

            // no gap in this case
            gap = 0.0;

            // interior symbols
            double remainder = groupLen - startOffset;
            int numInterior = (int)(remainder / repeat);

            // if the interior distribution goes exactly until the end then reduce
            // the interior count by one so we don't draw a symbol at the end
            // NOTE: using a slightly smaller group length addresses this
//          if (remainder <= numInterior*repeat && numInterior > 0)
//              --numInterior;

            numSymbols = 1 + numInterior;
        }
        else
        {
            // both start and end offsets are specified

            // starting symbol
            startPos = startOffset;

            // interior symbols - in the case where the repeat is set to the symbol
            // width, the minimum gap is 0.5*factor*repeat
            double factor = 0.15;
            double remainder = rs_max(groupLen - startOffset - endOffset - factor*symWidth, 0.0);
            int numInterior = (int)(remainder / repeat);

            // if the interior distribution fits exactly then reduce the
            // interior count by one so we don't draw a symbol at the end
            // NOTE: using a slightly smaller group length addresses this
//          if (remainder <= numInterior*repeat && numInterior > 0)
//              --numInterior;

            if (numInterior == 0)
                gap = groupLen - startOffset - endOffset;   // no room for any internal symbols
            else
                gap = 0.5*(remainder - (numInterior - 1)*repeat - (1.0-factor)*symWidth);

            numSymbols = 2 + numInterior;
        }
    }
    else
    {
        if (endOffset < 0.0)
        {
            // both start and end offsets are unspecified

            // no gap in this case
            gap = 0.0;

            // interior symbols
            double remainder = groupLen;
            int numInterior = (int)(remainder / repeat);

            // if the interior distribution fits exactly then reduce the
            // interior count by one so we don't draw a symbol at the end
            // NOTE: using a slightly smaller group length addresses this
//          if (remainder <= numInterior*repeat && numInterior > 0)
//              --numInterior;

            // starting symbol
            if (numInterior == 0)
                startPos = remainder;   // no room for any internal symbols
            else
                startPos = 0.5*(remainder - (numInterior - 1)*repeat);

            numSymbols = numInterior;
        }
        else
        {
            // only the end offset is specified

            // no gap in this case
            gap = 0.0;

            // interior symbols
            double remainder = groupLen - endOffset;
            int numInterior = (int)(remainder / repeat);

            // if the interior distribution goes exactly until the start then reduce
            // the interior count by one so we don't draw a symbol at the start
            // NOTE: using a slightly smaller group length addresses this
//          if (remainder <= numInterior*repeat && numInterior > 0)
//              --numInterior;

            // starting symbol
            startPos = groupLen - endOffset - numInterior*repeat;

            numSymbols = 1 + numInterior;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Distributes symbols along a polyline using the OverlapNone vertex control option.
void SE_Renderer::ProcessLineOverlapNone(LineBuffer* geometry, SE_RenderLineStyle* style)
{
    _ASSERT(style->repeat > 0.0);

    SE_Matrix symxf;
    bool yUp = YPointsUp();

    double baseAngleRad = style->angleRad;

    // precompute these - these are in renderer space, hence the check for yUp with the sine
    double baseAngleCos = cos(baseAngleRad);
    double baseAngleSin = sin(yUp? baseAngleRad : -baseAngleRad);

    // account for any viewport rotation
    double w2sAngleRad = GetWorldToScreenRotation();

    double angleRad, angleCos, angleSin;
    if (w2sAngleRad == 0.0)
    {
        angleRad = baseAngleRad;
        angleCos = baseAngleCos;
        angleSin = baseAngleSin;
    }
    else
    {
        angleRad = baseAngleRad + w2sAngleRad;
        angleCos = cos(angleRad);
        angleSin = sin(yUp? angleRad : -angleRad);
    }

    // screen coordinates of current line segment
    double segX0, segY0, segX1, segY1;

    // this is the same for all contours / groups
    double repeat = style->repeat;
    double leftEdge = style->bounds[0].x;
    double rightEdge = style->bounds[1].x;

    // get segment lengths
    double* segLens = (double*)alloca(sizeof(double)*geometry->point_count());
    ComputeSegmentLengths(geometry, segLens);

    // used for segment group calculations
    int* segGroups = (int*)alloca(2*sizeof(int)*geometry->point_count());
    double* groupLens = (double*)alloca(sizeof(double)*geometry->point_count());

    // configure the default path line stroke to use
    SE_LineStroke dpLineStroke = style->dpLineStroke;
    if (m_bSelectionMode)
    {
        dpLineStroke.color  = m_selLineStroke.color;
        dpLineStroke.weight = m_selLineStroke.weight;
    }

    // Used for drawing the centerline paths at vertices.  In the case of
    // a single contour the start/end points will need a MoveTo/LineTo,
    // while the interior points will need a MoveTo/LineTo/LineTo.
    LineBuffer vertexLines(3*geometry->point_count()-2);

    // iterate over the contours
    for (int j=0; j<geometry->cntr_count(); ++j)
    {
        // get starting segment for current contour
        int start_seg_contour = geometry->contour_start_point(j);

        // skip zero-length contours
        if (segLens[start_seg_contour] == 0.0)
            continue;

        // check if:
        // - the start offset goes beyond the end of the contour
        // - the end offset goes beyond the beginning of the contour
        // - the start offset goes beyond the end offset
        double offsetSum = rs_max(style->startOffset, 0.0) + rs_max(style->endOffset, 0.0);
        if (offsetSum > segLens[start_seg_contour])
            continue;

        // compute the segment groups for this contour based on the vertex angle limit
        int numGroups = ComputeSegmentGroups(geometry, j, style->vertexAngleLimit, segLens, segGroups);
        if (numGroups == 0)
            continue;

        // compute the group lengths
        ComputeGroupLengths(segLens, numGroups, segGroups, groupLens);

        // for this vertex control option we set the offsets to zero if they're unspecified
        double startOffset = rs_max(style->startOffset, 0.0);
        double endOffset = rs_max(style->endOffset, 0.0);

        // compute the starting group based on the style's start offset
        int start_group = 0;
        if (startOffset > 0.0)
        {
            for (int k=0; k<numGroups; ++k)
            {
                if (startOffset < groupLens[k])
                {
                    start_group = k;
                    break;
                }

                // adjust the start offset so it's relative to the starting group
                startOffset -= groupLens[k];
            }
        }

        // compute the ending group based on the style's end offset
        int end_group = numGroups-1;
        if (endOffset > 0.0)
        {
            for (int k=numGroups-1; k>=0; --k)
            {
                if (endOffset < groupLens[k])
                {
                    end_group = k;
                    break;
                }

                // adjust the end offset so it's relative to the ending group
                endOffset -= groupLens[k];
            }
        }

        // iterate over the relevant groups
        for (int k=start_group; k<=end_group; ++k)
        {
            // get segment range for current group
            int start_seg = segGroups[2*k];
            int end_seg = segGroups[2*k+1];
            int cur_seg = start_seg;

            // get the actual start / end offsets for the current group
            // - for the first group its start offset is the specified value, while
            //   for subsequent groups it's zero (we draw a symbol directly at the
            //   start of the group)
            // - for the last group the end offset is the specified value, while for
            //   prior groups it's zero (we draw a symbol directly at the end of the
            //   group)
            double startOffsetGroup = (k == start_group)? startOffset : 0.0;
            double   endOffsetGroup = (k ==   end_group)?   endOffset : 0.0;

            // Compute the symbol distribution for the group.  The drawpos variable
            // is the position of the first symbol.  If gap is > 0 then the next symbol
            // is offset by that amount.  Subsequent symbols are then offset by the
            // repeat, except for the last one which is again offset by the gap.
            //
            // Here's a graphical depiction:
            //
            // |-----+-----+---+---+---+-----+--------|
            // s     d  |     \__|__/     |           e
            // t     r  g        r        g           n
            // a     a  a        e        a           d
            // r     w  p        p        p
            // t     p           e
            //       o           a
            //       s           t
            int numSymbols = 0;
            double drawpos = startOffsetGroup;
            double gap = 0.0;
            ComputeGroupDistribution(groupLens[k], startOffsetGroup, endOffsetGroup, repeat,
                                     rightEdge - leftEdge, drawpos, gap, numSymbols);
            if (numSymbols == 0)
                continue;

            //-------------------------------------------------------
            // draw symbols along the group
            //-------------------------------------------------------

            int numDrawn = 0;
            double increment;

            // get start point of first segment in screen space
            WorldToScreenPoint(geometry->x_coord(cur_seg), geometry->y_coord(cur_seg), segX0, segY0);

            while (cur_seg < end_seg)
            {
                ++cur_seg;

                // skip zero-length segments - no need to update the start/end points
                double len = segLens[cur_seg];
                if (len == 0.0)
                    continue;

                // get end point of current segment in screen space
                WorldToScreenPoint(geometry->x_coord(cur_seg), geometry->y_coord(cur_seg), segX1, segY1);

                // if our draw position falls within this segment then process
                if (drawpos <= len)
                {
                    // compute linear deltas for x and y directions - we will use these
                    // to quickly move along the line without having to do too much math
                    double invlen = 1.0 / len;
                    double dx_incr = (segX1 - segX0) * invlen;
                    double dy_incr = (segY1 - segY0) * invlen;

                    if (style->angleControl == SE_AngleControl_FromGeometry)
                    {
                        angleCos = dx_incr*baseAngleCos - dy_incr*baseAngleSin;
                        angleSin = dy_incr*baseAngleCos + dx_incr*baseAngleSin;
                        angleRad = atan2(dy_incr, dx_incr);

                        // since dy_incr and dx_incr are in renderer space we need to
                        // negate the angle if y points down
                        if (!yUp)
                            angleRad = -angleRad;

                        angleRad += baseAngleRad;
                    }
                    double tx = segX0 + dx_incr * drawpos;
                    double ty = segY0 + dy_incr * drawpos;

                    symxf.setIdentity();
                    symxf.rotate(angleSin, angleCos);
                    symxf.translate(tx, ty);

                    // loop-draw the symbol along the current segment, incrementing
                    // the draw position by the appropriate amount
                    while (drawpos <= len && numDrawn < numSymbols)
                    {
                        // in the case of labels we only draw them at the interior points
                        if (style->drawLast)
                        {
                            if (numDrawn > 0 && numDrawn < numSymbols-1)
                                AddLabel(geometry, style, symxf, angleRad);
                        }
                        else
                        {
                            // handle the centerline path at the group's start
                            if (numDrawn == 0)
                            {
                                // This is the first time drawing anything for this group.  If
                                // this is the starting group, then initialize the centerline
                                // path at the group's start.  If it's not the starting group
                                // then we'll add LineTo segments (see further down) to the
                                // existing centerline path at the previous group's end.
                                if (k == start_group)
                                    vertexLines.MoveTo(symxf.x2, symxf.y2);
                            }
                            else if (numDrawn == 1 && numSymbols > 2)
                            {
                                // finish and draw the centerline path at the group's start,
                                // aligning it with the left edge of the symbol
                                // TODO: account for symbol rotation
                                vertexLines.LineTo(symxf.x2 + dx_incr*leftEdge, symxf.y2 + dy_incr*leftEdge);
                                DrawScreenPolyline(&vertexLines, NULL, dpLineStroke);
                                vertexLines.Reset();
                            }

                            // only draw symbols at the interior points
                            if (numDrawn > 0 && numDrawn < numSymbols-1)
                                DrawSymbol(style->symbol, symxf, angleRad, style->addToExclusionRegions);

                            // handle the centerline path at the group's end - only
                            // need to do this if we have at least one interior symbol
                            if (numDrawn == numSymbols-2 && numSymbols > 2)
                            {
                                // initialize the centerline path at the group's end,
                                // aligning it with the right edge of the symbol
                                // TODO: account for symbol rotation
                                vertexLines.MoveTo(symxf.x2 + dx_incr*rightEdge, symxf.y2 + dy_incr*rightEdge);
                            }
                            else if (numDrawn == numSymbols-1)
                            {
                                // This is the last time drawing anything for this group, so
                                // finish the centerline path at the group's end.  If this is
                                // the ending group, then also draw it.  If it's not the ending
                                // group then we'll draw it up above when we finish the centerline
                                // path at the next group's start.
                                vertexLines.LineTo(symxf.x2, symxf.y2);
                                if (k == end_group)
                                {
                                    DrawScreenPolyline(&vertexLines, NULL, dpLineStroke);
                                    vertexLines.Reset();
                                }
                            }
                        }

                        ++numDrawn;

                        // move forward
                        increment = repeat;
                        if (gap != 0.0)
                        {
                            // if we have a gap, then use it after drawing the first
                            // symbol and before drawing the last symbol
                            if (numSymbols == 2)
                            {
                                // in this case the gap takes us until the end
                                increment = gap;
                            }
                            else
                            {
                                if (numDrawn == 1)
                                    increment = gap - leftEdge;
                                else if (numDrawn == numSymbols - 1)
                                    increment = gap + rightEdge;
                            }
                        }

                        symxf.translate(dx_incr*increment, dy_incr*increment);
                        drawpos += increment;
                    }
                }

                drawpos -= len;

                // start point for next segment is current end point
                segX0 = segX1;
                segY0 = segY1;
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Distributes feature labels along a polyline.
void SE_Renderer::ProcessLineLabels(LineBuffer* geometry, SE_RenderLineStyle* style)
{
    SE_Matrix symxf;
    bool yUp = YPointsUp();

    double baseAngleRad = style->angleRad;

    // precompute these - these are in renderer space, hence the check for yUp with the sine
    double baseAngleCos = cos(baseAngleRad);
    double baseAngleSin = sin(yUp? baseAngleRad : -baseAngleRad);

    // account for any viewport rotation
    double w2sAngleRad = GetWorldToScreenRotation();

    double angleRad, angleCos, angleSin;
    if (w2sAngleRad == 0.0)
    {
        angleRad = baseAngleRad;
        angleCos = baseAngleCos;
        angleSin = baseAngleSin;
    }
    else
    {
        angleRad = baseAngleRad + w2sAngleRad;
        angleCos = cos(angleRad);
        angleSin = sin(yUp? angleRad : -angleRad);
    }

    // screen coordinates of current line segment
    double segX0, segY0, segX1, segY1;

    // this is the same for all contours
    double repeat = PATH_LABEL_SEPARATION_INCHES * MILLIMETERS_PER_INCH * GetScreenUnitsPerMillimeterDevice();
    double leftEdge = style->bounds[0].x;
    double rightEdge = style->bounds[1].x;
    double symWidth = rightEdge - leftEdge;

    // repeat needs to be the separation (end of one label to start of the
    // next) plus the symbol width
    repeat += symWidth;

    // get the segment lengths
    double* segLens = (double*)alloca(sizeof(double)*geometry->point_count());
    ComputeSegmentLengths(geometry, segLens);

    // iterate over the contours
    for (int j=0; j<geometry->cntr_count(); ++j)
    {
        // get segment range for current contour
        int start_seg = geometry->contour_start_point(j);
        int   end_seg = geometry->contour_end_point(j);

        // skip contours shorter than the symbol width
        double contourLen = segLens[start_seg];
        if (contourLen <= symWidth)
            continue;

        // how many times should we repeat the symbol along the path?
        // TODO: fine tune this formula
        int numSymbols = 1 + (int)((contourLen - symWidth) / repeat);
        double startOffset = 0.5*(contourLen - (numSymbols - 1) * repeat);

        // account for the symbol's extent to properly center it
        startOffset -= 0.5*(leftEdge + rightEdge);

        //-------------------------------------------------------
        // draw symbols along the contour
        //-------------------------------------------------------

        int numDrawn = 0;
        int cur_seg = start_seg;
        double drawpos = startOffset;

        // get start point of first segment in screen space
        WorldToScreenPoint(geometry->x_coord(cur_seg), geometry->y_coord(cur_seg), segX0, segY0);

        while (cur_seg < end_seg)
        {
            ++cur_seg;

            // skip zero-length segments - no need to update the start/end points
            double len = segLens[cur_seg];
            if (len == 0.0)
                continue;

            // get end point of current segment in screen space
            WorldToScreenPoint(geometry->x_coord(cur_seg), geometry->y_coord(cur_seg), segX1, segY1);

            // if our draw position falls within this segment then process
            if (drawpos <= len)
            {
                // compute linear deltas for x and y directions - we will use these
                // to quickly move along the line without having to do too much math
                double invlen = 1.0 / len;
                double dx_incr = (segX1 - segX0) * invlen;
                double dy_incr = (segY1 - segY0) * invlen;

                if (style->angleControl == SE_AngleControl_FromGeometry)
                {
                    angleCos = dx_incr*baseAngleCos - dy_incr*baseAngleSin;
                    angleSin = dy_incr*baseAngleCos + dx_incr*baseAngleSin;
                    angleRad = atan2(dy_incr, dx_incr);

                    // since dy_incr and dx_incr are in renderer space we need to
                    // negate the angle if y points down
                    if (!yUp)
                        angleRad = -angleRad;

                    angleRad += baseAngleRad;
                }
                double tx = segX0 + dx_incr * drawpos;
                double ty = segY0 + dy_incr * drawpos;

                symxf.setIdentity();
                symxf.rotate(angleSin, angleCos);
                symxf.translate(tx, ty);

                // loop-draw the symbol along the current segment, incrementing
                // the draw position by the appropriate amount
                while (drawpos <= len && numDrawn < numSymbols)
                {
                    AddLabel(geometry, style, symxf, angleRad);
                    ++numDrawn;

                    // move forward
                    symxf.translate(dx_incr*repeat, dy_incr*repeat);
                    drawpos += repeat;
                }
            }

            drawpos -= len;

            // start point for next segment is current end point
            segX0 = segX1;
            segY0 = segY1;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Distributes symbols along a polyline using the OverlapDirect vertex control option.
// This setting is intended to only be used in the context of point symbols.  These
// symbols are simply drawn without any wrapping, truncation, or other modification.
//
// The following rules apply to the StartOffset and EndOffset parameters:
//   - if StartOffset is specified (>=0) then a symbol is drawn at the start
//     offset location
//   - if EndOffset is specified (>=0) then a symbol is drawn at the end
//     offset location
//   - if StartOffset and EndOffset are both specified but their sum is greater
//     than the contour length (EndOffset offset comes before StartOffset)
//     then no symbols are drawn
//   - if StartOffset and EndOffset are both unspecified (< 0) then no symbols
//     are drawn
void SE_Renderer::ProcessLineOverlapDirect(LineBuffer* geometry, SE_RenderLineStyle* style)
{
    _ASSERT(style->repeat > 0.0);

    SE_Matrix symxf;
    bool yUp = YPointsUp();

    double baseAngleRad = style->angleRad;

    // precompute these - these are in renderer space, hence the check for yUp with the sine
    double baseAngleCos = cos(baseAngleRad);
    double baseAngleSin = sin(yUp? baseAngleRad : -baseAngleRad);

    // account for any viewport rotation
    double w2sAngleRad = GetWorldToScreenRotation();

    double angleRad, angleCos, angleSin;
    if (w2sAngleRad == 0.0)
    {
        angleRad = baseAngleRad;
        angleCos = baseAngleCos;
        angleSin = baseAngleSin;
    }
    else
    {
        angleRad = baseAngleRad + w2sAngleRad;
        angleCos = cos(angleRad);
        angleSin = sin(yUp? angleRad : -angleRad);
    }

    // screen coordinates of current line segment
    double segX0, segY0, segX1, segY1;

    // this is the same for all contours / groups
    double repeat = style->repeat;

    // get segment lengths
    double* segLens = (double*)alloca(sizeof(double)*geometry->point_count());
    ComputeSegmentLengths(geometry, segLens);

    // used for segment group calculations
    int* segGroups = (int*)alloca(2*sizeof(int)*geometry->point_count());
    double* groupLens = (double*)alloca(sizeof(double)*geometry->point_count());

    // iterate over the contours
    for (int j=0; j<geometry->cntr_count(); ++j)
    {
        // get starting segment for current contour
        int start_seg_contour = geometry->contour_start_point(j);

        // skip zero-length contours
        if (segLens[start_seg_contour] == 0.0)
            continue;

        // check if:
        // - the start offset goes beyond the end of the contour
        // - the end offset goes beyond the beginning of the contour
        // - the start offset goes beyond the end offset
        double offsetSum = rs_max(style->startOffset, 0.0) + rs_max(style->endOffset, 0.0);
        if (offsetSum > segLens[start_seg_contour])
            continue;

        // compute the segment groups for this contour based on the vertex angle limit
        int numGroups = ComputeSegmentGroups(geometry, j, style->vertexAngleLimit, segLens, segGroups);
        if (numGroups == 0)
            continue;

        // compute the group lengths
        ComputeGroupLengths(segLens, numGroups, segGroups, groupLens);

        // compute the starting group based on the style's start offset
        int start_group = 0;
        double startOffset = style->startOffset;
        if (startOffset > 0.0)
        {
            for (int k=0; k<numGroups; ++k)
            {
                if (startOffset < groupLens[k])
                {
                    start_group = k;
                    break;
                }

                // adjust the start offset so it's relative to the starting group
                startOffset -= groupLens[k];
            }
        }

        // compute the ending group based on the style's end offset
        int end_group = numGroups-1;
        double endOffset = style->endOffset;
        if (endOffset > 0.0)
        {
            for (int k=numGroups-1; k>=0; --k)
            {
                if (endOffset < groupLens[k])
                {
                    end_group = k;
                    break;
                }

                // adjust the end offset so it's relative to the ending group
                endOffset -= groupLens[k];
            }
        }

        // iterate over the relevant groups
        for (int k=start_group; k<=end_group; ++k)
        {
            // get segment range for current group
            int start_seg = segGroups[2*k];
            int end_seg = segGroups[2*k+1];
            int cur_seg = start_seg;

            // get the actual start / end offsets for the current group
            // - for the first group its start offset is the specified value, while
            //   for subsequent groups it's zero (we draw a symbol directly at the
            //   start of the group)
            // - for the last group the end offset is the specified value, while for
            //   prior groups it's zero (we draw a symbol directly at the end of the
            //   group)
            double startOffsetGroup = (k == start_group)? startOffset : 0.0;
            double   endOffsetGroup = (k ==   end_group)?   endOffset : 0.0;

            // Compute the symbol distribution for the group.  The drawpos variable
            // is the position of the first symbol.  If gap is > 0 then the next symbol
            // is offset by that amount.  Subsequent symbols are then offset by the
            // repeat, except for the last one which is again offset by the gap.
            //
            // Here's a graphical depiction:
            //
            // |-----+-----+---+---+---+-----+--------|
            // s     d  |     \__|__/     |           e
            // t     r  g        r        g           n
            // a     a  a        e        a           d
            // r     w  p        p        p
            // t     p           e
            //       o           a
            //       s           t
            int numSymbols = 0;
            double drawpos = startOffsetGroup;
            double gap = 0.0;
            ComputeGroupDistribution(groupLens[k], startOffsetGroup, endOffsetGroup, repeat, 0.0,
                                     drawpos, gap, numSymbols);
            if (numSymbols == 0)
                continue;

            //-------------------------------------------------------
            // draw symbols along the group
            //-------------------------------------------------------

            int numDrawn = 0;
            double increment;

            // get start point of first segment in screen space
            WorldToScreenPoint(geometry->x_coord(cur_seg), geometry->y_coord(cur_seg), segX0, segY0);

            while (cur_seg < end_seg)
            {
                ++cur_seg;

                // skip zero-length segments - no need to update the start/end points
                double len = segLens[cur_seg];
                if (len == 0.0)
                    continue;

                // get end point of current segment in screen space
                WorldToScreenPoint(geometry->x_coord(cur_seg), geometry->y_coord(cur_seg), segX1, segY1);

                // if our draw position falls within this segment then process
                if (drawpos <= len)
                {
                    // compute linear deltas for x and y directions - we will use these
                    // to quickly move along the line without having to do too much math
                    double invlen = 1.0 / len;
                    double dx_incr = (segX1 - segX0) * invlen;
                    double dy_incr = (segY1 - segY0) * invlen;

                    if (style->angleControl == SE_AngleControl_FromGeometry)
                    {
                        angleCos = dx_incr*baseAngleCos - dy_incr*baseAngleSin;
                        angleSin = dy_incr*baseAngleCos + dx_incr*baseAngleSin;
                        angleRad = atan2(dy_incr, dx_incr);

                        // since dy_incr and dx_incr are in renderer space we need to
                        // negate the angle if y points down
                        if (!yUp)
                            angleRad = -angleRad;

                        angleRad += baseAngleRad;
                    }
                    double tx = segX0 + dx_incr * drawpos;
                    double ty = segY0 + dy_incr * drawpos;

                    symxf.setIdentity();
                    symxf.rotate(angleSin, angleCos);
                    symxf.translate(tx, ty);

                    // loop-draw the symbol along the current segment,
                    // incrementing the draw position by the appropriate amount
                    while (drawpos <= len && numDrawn < numSymbols)
                    {
                        // don't draw the same symbol once at the end of a group
                        // and again at the start of the next group
                        if (k == start_group || numDrawn > 0)
                        {
                            // draw the symbol at the current position
                            if (style->drawLast)
                                AddLabel(geometry, style, symxf, angleRad);
                            else
                                DrawSymbol(style->symbol, symxf, angleRad, style->addToExclusionRegions);
                        }

                        ++numDrawn;

                        // move forward
                        increment = repeat;
                        if (gap != 0.0)
                        {
                            // if we have a gap, then use it after drawing the first
                            // symbol and before drawing the last symbol
                            if (numDrawn == 1 || numDrawn == numSymbols - 1)
                                increment = gap;
                        }

                        symxf.translate(dx_incr*increment, dy_incr*increment);
                        drawpos += increment;
                    }
                }

                drawpos -= len;

                // start point for next segment is current end point
                segX0 = segX1;
                segY0 = segY1;
            }
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// Indicates whether rendering optimization is used by this renderer.  For example, if we are rendering text and
// optimization is turned on, then text is rendered as a simple line when it is very small.
// By default, optimization is turned on.  For other behavior, this method must be overridden.
bool SE_Renderer::OptimizeGeometry()
{
    return true;
}
