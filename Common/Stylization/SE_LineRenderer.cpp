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
#include "SE_LineRenderer.h"
#include "SE_BufferPool.h"


///////////////////////////////////////////////////////////////////////////////
// Used with ProcessLineOverlapWrap
struct HotSpot
{
    // the position of the hotspot vertex point, in rendering space
    double x, y;

    // the segment angle in the entry region in the range (-M_PI,M_PI]
    double angle_start;
    double cos_angle_start;
    double sin_angle_start;

    // the segment angle in the exit region in the range (-M_PI,M_PI]
    double angle_end;
    double cos_angle_end;
    double sin_angle_end;

    // the parametric position of the hotspot entry point, in rendering
    // units, relative to the start of the contour
    double start;

    // the parametric position of the hotspot vertex point, in rendering
    // units, relative to the start of the contour
    double mid;

    // the parametric position of the hotspot exit point, in rendering
    // units, relative to the start of the contour
    double end;

    // Shear factors define the additional amount that a point is shifted
    // along the polyline due to warping.  The shift amount is just the
    // shear factor times the vertical position of the point relative to
    // the polyline.  We allow the shear distribution to be different on
    // the upper (y>=0) and lower (y<0) sides of the polyline.

    // the shear factors at the hotspot entry point
    double shear_start_upper;
    double shear_start_lower;

    // the shear factors at the entry side of the hotspot vertex point
    // (shear factors at exit side are just negative this amount)
    double shear_mid_upper;
    double shear_mid_lower;

    // the shear factors at the hotspot exit point
    double shear_end_upper;
    double shear_end_lower;

    // flags if this hotspot's exit region abuts the next hotspot's
    // entry region
    bool abuts_next_hotspot;

    // the parametric position in the entry region where the miter line begins
    double miter_start;

    // the parametric position in the exit region where the miter line ends
    double miter_end;
};


///////////////////////////////////////////////////////////////////////////////
// Segments whose half-join-angle is smaller than this amount are treated
// as overlapping.
const double HALF_JOIN_ANGLE_LIMIT = 0.0001;


///////////////////////////////////////////////////////////////////////////////
// Segments whose angles differ by less than this amount are treated as
// colinear.
const double COLINEAR_ANGLE_LIMIT = 0.0001;


///////////////////////////////////////////////////////////////////////////////
// Additional factor by which to expand the affected width of each join:
// - a value of 1 makes the join fit tightly, but doesn't look good
// - a value of 2 gives smooth warping over not too large of a width
const double WIDTH_FACTOR = 2.0;


///////////////////////////////////////////////////////////////////////////////
// When doing point reduction, points will be dropped if the distance
// between them squared is more than 1.96 (i.e. 1.4 rendering units).
const double OPTIMIZE_DISTANCE_SQ = 1.96;


///////////////////////////////////////////////////////////////////////////////
// This method implements the OverlapWrap VertexControl option for rendering
// of polyline features.  Symbols are drawn along the polyline, and are warped
// around corners to provide a continuous distribution.
//
// Warping is done by applying shear to the symbol points.  Here's a coarse
// representation:
//
// y=H -+--------+----------+
//      |        |         / \
//      |       |  ---->  /   \
//      |       |  -->   /     \
//      |      |        /   ^   \
// y=0 -+------+-------+  ^  \   \
//      x0     x'     x1\  \  \   \
//      s0     s'     s1 \    __---\
//                        \---
//                         \
//
// A piecewise-linear shear distribution is computed over the entire polyline,
// with the upper and lower side each having their own distribution.  We can
// therefore compute a shear factor s' at each parametric position x' on the
// polyline.  The shear line at x' is obtained by applying the shear factor to
// the set of all points (x', y).  A given point (x', y) is simply mapped to
// (x' + y*s', y).
//
// We also apply the vertex join style - bevel, miter, or round - at each vertex.
// Bevel is just a special case of miter.  Join styles are applied by scaling
// the shear lines so that they end at the desired location (the miter bevel
// line for miter joins, or the circle centered at the vertex for round joins).
//
// Note that the AngleControl option is ignored for wrapped line styles.  It
// doesn't make sense to have the symbol angles not be aligned with the feature
// geometry.
//
// ============================================================================
//
// Following are some mathematical formulas used in the method.
//
//
// Shear Lines
// -----------
//
// The equation for a shear line passing through the parametric position x' is:
//    y = (x - x') / s'
//
// where:
//    s' = s0 + (s1 - s0) * (x' - x0) / (x1 - x0)   (linear shear distribution)
//
//
// Vertex Joins
// ------------
//
// To apply the vertex join to a given shear line we need to:
// - intersect the shear line with the desired join geometry (miter bevel line
//   or round join circle)
// - compute the length L of the full shear line (from y=0 to y=H)
// - compute the length L' of the shear line up to the intersection point
// - divide L' by L to get the scale factor
//
// Note that the length L has a simple formula.  We have:
//    dy = H
//    dx = dy * s'
//
//  => L = H * sqrt(1 + s'^2)
//
//
// Miter Vertex Joins
// ------------------
//
// For a miter vertex join, the equation of the miter bevel line is:
//    y - H = -s1 * (x - xB),
//
// where xB is the x intersection of the bevel line with the line y=H.  The
// value of xB depends on both the miter limit m and the half-join-angle at
// the vertex (alpha/2).  The derivation yields:
//
//                  m - sin(alpha/2)
//   xB - x1 = H * ------------------
//                    cos(alpha/2)
//
// Note that for a bevel join m is set to sin(alpha/2), which gives xB = x1.
//
// The scale factor can then be derived to be:
//
//   L'    1 + |s1| * (xB - x')/H 
//  --- = ------------------------, all s1, s'
//   L           1 + s1*s'
//
//
// Round Vertex Joins
// ------------------
//
// The scale factor formula has a square root since it involves computing the
// intersection of a line with a circle.  The equation is:
//
//   L'    |s'| * xn + sqrt[(1 + s'^2) - xn^2]
//  --- = -------------------------------------, all s'
//   L                  1 + s'^2
//
// where xn = (x1 - x')/H.
//
// ============================================================================

void SE_LineRenderer::ProcessLineOverlapWrap(SE_Renderer* renderer, LineBuffer* geometry, SE_RenderLineStyle* style)
{
    SE_BufferPool* lbp = renderer->GetBufferPool();

    SE_RenderPrimitiveList& rs = style->symbol;

    RS_Bounds styleBounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
    styleBounds.add_point(style->bounds[0]);
    styleBounds.add_point(style->bounds[1]);
    styleBounds.add_point(style->bounds[2]);
    styleBounds.add_point(style->bounds[3]);

    double affected_height = rs_max(fabs(styleBounds.miny), fabs(styleBounds.maxy));
    double affected_height_inv = (affected_height > 0.0)? 1.0 / affected_height : 0.0;

    double increment = style->repeat;
    _ASSERT(increment > 0.0);

    double startOffsetMax = rs_max(style->startOffset, 0.0);
    double endOffsetMax   = rs_max(style->endOffset, 0.0);

    bool roundJoin = (style->vertexJoin == SE_LineJoin_Round);

    // get the size of the largest contour so we can pre-allocate hotspots for it
    int maxPoints = 0;
    for (int cur_contour=0; cur_contour<geometry->cntr_count(); ++cur_contour)
    {
        int numPoints = geometry->cntr_size(cur_contour);
        if (numPoints > maxPoints)
            maxPoints = numPoints;
    }

    LineBuffer** choppedBuffers = NULL;

    // this try-catch is used to catch possible out-of-memory exceptions with the
    // chopped buffers
    try
    {
        // allocate the hotspot array on the stack for performance
        HotSpot* hotspots = (HotSpot*)alloca(maxPoints * sizeof(HotSpot));

        // Create a "chopped up" LineBuffer for each polyline / polygon primitive
        // in the symbol.  We need this because straight lines become curved when
        // the warping is applied.
        int maxChoppedSize = 0;
        choppedBuffers = (LineBuffer**)alloca(rs.size() * sizeof(LineBuffer*));
        memset(choppedBuffers, 0, rs.size() * sizeof(LineBuffer*));
        for (unsigned cur_prim=0; cur_prim<rs.size(); ++cur_prim)
        {
            SE_RenderPrimitive* primitive = rs[cur_prim];

            if (primitive->type == SE_RenderPrimitive_Polygon || primitive->type == SE_RenderPrimitive_Polyline)
            {
                SE_RenderPolyline* pl = (SE_RenderPolyline*)primitive;

                // create the chopped up LineBuffer for this primitive
                choppedBuffers[cur_prim] = LineBufferPool::NewLineBuffer(lbp, 128, FdoDimensionality_XY, true);
                SE_LineRenderer::ChopLineBuffer(renderer, pl->geometry->xf_buffer(), choppedBuffers[cur_prim]);

                // update rendering attributes to account for the selection mode - it's
                // ok to update these attributes and not revert them
                if (renderer->m_bSelectionMode)
                {
                    if (primitive->type == SE_RenderPrimitive_Polygon)
                    {
                        SE_RenderPolygon* pl = (SE_RenderPolygon*)primitive;
                        pl->fill = renderer->m_selFillColor;
                    }

                    pl->lineStroke.color  = renderer->m_selLineStroke.color;
                    pl->lineStroke.weight = renderer->m_selLineStroke.weight;
                }
            }
            else if (primitive->type == SE_RenderPrimitive_Text)
            {
                SE_RenderText* tp = (SE_RenderText*)primitive;

                // create the chopped up LineBuffer for this primitive
                choppedBuffers[cur_prim] = LineBufferPool::NewLineBuffer(lbp, 1, FdoDimensionality_XY, true);
                choppedBuffers[cur_prim]->MoveTo(tp->position[0], tp->position[1]);

                // update rendering attributes to account for the selection mode - it's
                // ok to update these attributes and not revert them
                if (renderer->m_bSelectionMode)
                {
                    tp->tdef.textcolor()   = renderer->m_textForeColor;
                    tp->tdef.ghostcolor()  = renderer->m_textBackColor;
//                  tp->tdef.framecolor()  = renderer->m_textBackColor;
//                  tp->tdef.opaquecolor() = renderer->m_textBackColor;
                }
            }
            else if (primitive->type == SE_RenderPrimitive_Raster)
            {
                SE_RenderRaster* rp = (SE_RenderRaster*)primitive;

                // create the chopped up LineBuffer for this primitive
                choppedBuffers[cur_prim] = LineBufferPool::NewLineBuffer(lbp, 1, FdoDimensionality_XY, true);
                choppedBuffers[cur_prim]->MoveTo(rp->position[0], rp->position[1]);

                // raster primitive don't have any rendering attributes that need to be
                // updated to account for the selection mode
            }

            // keep track of the largest chopped buffer
            int numPoints = choppedBuffers[cur_prim]->point_count();
            if (numPoints > maxChoppedSize)
                maxChoppedSize = numPoints;
        }

        // this will be our work buffer
        LineBuffer geom(maxChoppedSize, FdoDimensionality_XY, true);

        // for each contour
        for (int cur_contour=0; cur_contour<geometry->cntr_count(); ++cur_contour)
        {
            // must have at least one segment
            int numPoints = geometry->cntr_size(cur_contour);
            if (numPoints < 2)
                continue;

            // make a list of hotspots where we go from one join to another
            // or from join to straight line or from straight line to join
            int ptCount = SE_LineRenderer::ConfigureHotSpots(renderer, geometry, cur_contour, style, styleBounds, hotspots);
            double total_length = hotspots[ptCount-1].mid;

            // check if:
            // - the start offset goes beyond the end of the contour
            // - the end offset goes beyond the beginning of the contour
            // - the start offset goes beyond the end offset
            double offsetSum = startOffsetMax + endOffsetMax;
            if (offsetSum > total_length)
                continue;

            // set the distribution's start and end parametric values
            // (the distribution will be clipped against these values)
            double startpos = startOffsetMax;
            double endpos   = total_length - endOffsetMax;

            // calculate the initial draw position
            double drawpos  = startpos;

            if (style->startOffset < 0.0 && style->endOffset >= 0.0)
            {
                // only end offset is specified - adjust the draw position so we
                // have: endpos = drawpos + i*increment
                drawpos = fmod(endpos, increment);

                // reduce the draw position by one increment if the initial style would
                // still display
                if (drawpos - increment + styleBounds.maxx >= 0.0)
                    drawpos -= increment;

                // Roundoff error can lead to the final symbol position being slightly
                // greater than endpos, which could lead to additional undesireable
                // clipping of the final symbol.  Prevent this by reducing drawpos
                // slightly (a fraction of the increment).
                drawpos -= 1.0e-10 * increment;
            }

            // cache these for later use
            HotSpot* first_hotspot = &hotspots[0];
            HotSpot* last_hotspot = &hotspots[ptCount-1];

            // find the first active hotspot for the distribution
            double sym_minx = drawpos + styleBounds.minx;
            double sym_maxx = drawpos + styleBounds.maxx;

            int next_hotspot_index = 0;
            HotSpot* next_hotspot = &hotspots[next_hotspot_index];
            while (next_hotspot != last_hotspot && sym_minx >= next_hotspot->end)
            {
                next_hotspot_index++;
                next_hotspot = &hotspots[next_hotspot_index];
            }

            // now draw symbols over the distribution
            while (sym_minx <= endpos)
            {
                // compute x and y of draw position (remember - drawpos is in rendering
                // units relative to the start of the polyline)

                // get the distance to drawpos along the segment on the hotspot's entry side
                // - a negative value means we're in the hotspot's entry region
                // - a positive value means we're in the hotspot's exit region
                double len_along_cur_seg = drawpos - next_hotspot->mid;

                // just linear interpolation along the entry side of the hotspot
                double xpos = next_hotspot->x + next_hotspot->cos_angle_start * len_along_cur_seg;
                double ypos = next_hotspot->y + next_hotspot->sin_angle_start * len_along_cur_seg;

                // we explicitly ignore any angleControl and angle settings
                SE_Matrix xformStart;
                xformStart.rotate(next_hotspot->sin_angle_start, next_hotspot->cos_angle_start);
                xformStart.translate(xpos, ypos);

                if (sym_maxx < next_hotspot->start && sym_minx >= startpos && sym_maxx <= endpos)
                {
                    // We're not yet in danger of a corner (or the start/end of the distribution).
                    // Just put the pedal to the metal and draw an unwarped symbol.
                    renderer->DrawSymbol(rs, xformStart, next_hotspot->angle_start);

                    drawpos += increment;
                    sym_minx = drawpos + styleBounds.minx;
                    sym_maxx = drawpos + styleBounds.maxx;
                }
                else
                {
                    // we are approaching a join - slam on the brakes and think about turning

                    // needed for text/raster primtives
                    double last_angleRad = 0.0;

                    // loop over the symbol's primitive elements - we will handle them one by one
                    for (unsigned cur_prim=0; cur_prim<rs.size(); ++cur_prim)
                    {
                        SE_RenderPrimitive* primitive = rs[cur_prim];

                        // initialize our work buffer to the chopped line buffer
                        geom = *choppedBuffers[cur_prim];

                        //-------------------------------------------------------
                        // apply the join warp to the chopped line buffer
                        //-------------------------------------------------------

                        // go over the vertices and warp them according to where in the
                        // hotspot they fall
                        for (int i=0; i<geom.point_count(); ++i)
                        {
                            double x = geom.x_coord(i);
                            double y = geom.y_coord(i);

                            // get the parametric location of this point along the line,
                            // and store it in the buffer's z coordinate
                            double xParam = drawpos + x;
                            geom.z_coord(i) = xParam;

                            // compute how far into the join this point is
                            double remaining_len = xParam - next_hotspot->start;

                            if (remaining_len <= 0.0)
                            {
                                // vertex not affected - it comes before the warp area

                                // just transform the point to the starting segment space
                                xformStart.transform(x, y);
                                geom.x_coord(i) = x;
                                geom.y_coord(i) = y;
                                last_angleRad = next_hotspot->angle_start;
                            }
                            else
                            {
                                // We are in the warp area and could potentially have to traverse
                                // several hotspots since the symbol can be bigger than a segment
                                // in the feature and overlap several line joins.

                                // keep track of hotspots that we pass through
                                HotSpot* hs = next_hotspot;

                                // now make our way along the original polyline
                                // TODO: optimize this loop by keeping track of incremental transforms
                                //       rather than snake along from the beginning hotspot for each
                                //       point.  We can also precompute a lot of stuff and store it in
                                //       the hotspot structures.
                                while (remaining_len > 0.0 && hs <= last_hotspot)
                                {
                                    double join_entry_length = hs->mid - hs->start;
                                    double join_length       = hs->end - hs->start;
                                    double join_exit_length  = hs->end - hs->mid;

                                    double shear_mid, shear_start, shear_end;
                                    if (y >= 0.0)
                                    {
                                        shear_mid   = hs->shear_mid_upper;
                                        shear_start = hs->shear_start_upper;
                                        shear_end   = hs->shear_end_upper;
                                    }
                                    else
                                    {
                                        shear_mid   = hs->shear_mid_lower;
                                        shear_start = hs->shear_start_lower;
                                        shear_end   = hs->shear_end_lower;
                                    }

                                    // are we on the outside or inside edge of the join?
                                    bool outside = (y*shear_mid >= 0.0);

                                    if (remaining_len <= join_entry_length)
                                    {
                                        // remaining distance ends in entry area of current hotspot
                                        double dist_after_entry = remaining_len;

                                        // compute warped position in entry area of current hotspot
                                        // for non-centerline points
                                        double shear = 0.0;
                                        double scale = 1.0;
                                        if (y != 0.0)
                                        {
                                            // must be true if y != 0
                                            _ASSERT(affected_height > 0.0);

                                            // get the shear at the point
                                            double fact  = dist_after_entry / join_entry_length;
                                            shear = shear_start + fact * (shear_mid - shear_start);

                                            // compute the scale factor due to any miter or round join
                                            if (outside)
                                            {
                                                if (roundJoin)
                                                {
                                                    // only scale if the shear line at this parametric location
                                                    // extends past the hotspot vertex point
                                                    double dd = (hs->mid - xParam) * affected_height_inv;
                                                    if (fabs(shear) > dd)
                                                    {
                                                        double as2 = 1.0 + shear*shear;

                                                        double rad = as2 - dd*dd;
                                                        _ASSERT(rad >= 0.0);    // guaranteed due to previous if check

                                                        double numer = fabs(shear) * dd + sqrt(rad);
                                                        double denom = as2;

                                                        // only scale factors less than one should be applied
                                                        if (numer < denom)
                                                            scale = numer / denom;
                                                    }
                                                }
                                                else
                                                {
                                                    // miter / bevel join
                                                    double dd = (hs->miter_start - xParam) * affected_height_inv;
                                                    double numer = 1.0 + dd * fabs(shear_mid);
                                                    double denom = 1.0 + shear_mid * shear;

                                                    // only scale factors less than one should be applied
                                                    if (numer < denom)
                                                        scale = numer / denom;
                                                }
                                            }

                                            // if the previous hotspot abuts ours, then also compute the scale factor
                                            // for its exit region since this can "bleed" into the current hotspot
                                            if (hs != first_hotspot)
                                            {
                                                // In the case of a closed polyline we won't get in here for the
                                                // starting hotspot, and so the computed points can be incorrect.
                                                // That's ok though since this is for the entry side, and points on
                                                // that side of the first hotspot will get clipped away.
                                                HotSpot* hs1 = hs-1;
                                                if (hs1->abuts_next_hotspot)
                                                {
                                                    double shear_mid1 = (y > 0.0)? hs1->shear_mid_upper : hs1->shear_mid_lower;

                                                    bool outside1 = (y*shear_mid1 >= 0.0);
                                                    if (outside1)
                                                    {
                                                        if (roundJoin)
                                                        {
                                                            // only scale if the shear line at this parametric location
                                                            // extends past the hotspot vertex point
                                                            double dd1 = (xParam - hs1->mid) * affected_height_inv;
                                                            if (fabs(shear) > dd1)
                                                            {
                                                                double as2 = 1.0 + shear*shear;

                                                                double rad1 = as2 - dd1*dd1;
                                                                _ASSERT(rad1 >= 0.0);   // guaranteed due to previous if check

                                                                double numer1 = fabs(shear) * dd1 + sqrt(rad1);
                                                                double denom1 = as2;

                                                                // use this scale factor if it's smaller
                                                                if (numer1 < scale * denom1)
                                                                    scale = numer1 / denom1;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            // miter / bevel join
                                                            double dd1 = (xParam - hs1->miter_end) * affected_height_inv;
                                                            double numer1 = 1.0 + dd1 * fabs(shear_mid1);
                                                            double denom1 = 1.0 - shear_mid1 * shear;

                                                            // use this scale factor if it's smaller
                                                            if (numer1 < scale * denom1)
                                                                scale = numer1 / denom1;
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        // get the position of the start of the join
                                        double warp_x = hs->x - hs->cos_angle_start * join_entry_length;
                                        double warp_y = hs->y - hs->sin_angle_start * join_entry_length;

                                        // compute the warped point relative to the end of the join
                                        double txx = dist_after_entry + scale*y*shear;
                                        double txy = scale*y;

                                        SE_Matrix xform;
                                        xform.rotate(hs->sin_angle_start, hs->cos_angle_start);
                                        xform.transform(txx, txy);

                                        // add this offset to the join's starting point
                                        warp_x += txx;
                                        warp_y += txy;

                                        // update the geometry with the warped point
                                        geom.x_coord(i) = warp_x;
                                        geom.y_coord(i) = warp_y;
                                        last_angleRad = hs->angle_start;

                                        // done with this point
                                        remaining_len = 0.0;
                                        break;
                                    }
                                    else if (remaining_len <= join_length)
                                    {
                                        // remaining distance ends in exit area of current hotspot
                                        double dist_before_exit = join_length - remaining_len;

                                        // compute warped position in exit area of current hotspot
                                        // for non-centerline points
                                        double shear = 0.0;
                                        double scale = 1.0;
                                        if (y != 0.0)
                                        {
                                            // must be true if y != 0
                                            _ASSERT(affected_height > 0.0);

                                            // get the shear at the point (the shear factor at the exit
                                            // side of the hotspot vertex point is negative shear_mid,
                                            // hence -fact)
                                            double fact  = dist_before_exit / join_exit_length;
                                            shear = shear_end - fact * (shear_mid + shear_end);

                                            // compute the scale factor due to any miter or round join
                                            if (outside)
                                            {
                                                if (roundJoin)
                                                {
                                                    // only scale if the shear line at this parametric location
                                                    // extends past the hotspot vertex point
                                                    double dd = (xParam - hs->mid) * affected_height_inv;
                                                    if (fabs(shear) > dd)
                                                    {
                                                        double as2 = 1.0 + shear*shear;

                                                        double rad = as2 - dd*dd;
                                                        _ASSERT(rad >= 0.0);    // guaranteed due to previous if check

                                                        double numer = fabs(shear) * dd + sqrt(rad);
                                                        double denom = as2;

                                                        // only scale factors less than one should be applied
                                                        if (numer < denom)
                                                            scale = numer / denom;
                                                    }
                                                }
                                                else
                                                {
                                                    // miter / bevel join
                                                    double dd = (xParam - hs->miter_end) * affected_height_inv;
                                                    double numer = 1.0 + dd * fabs(shear_mid);
                                                    double denom = 1.0 - shear_mid * shear;

                                                    // only scale factors less than one should be applied
                                                    if (numer < denom)
                                                        scale = numer / denom;
                                                }
                                            }

                                            // if we abut the next hotspot, then also compute the scale factor
                                            // for its entry region since this can "bleed" into the current hotspot
                                            if (hs != last_hotspot)
                                            {
                                                // In the case of a closed polyline we won't get in here for the
                                                // last hotspot, and so the computed points can be incorrect.
                                                // That's ok though since this is for the exit side, and points on
                                                // that side of the last hotspot will get clipped away.
                                                HotSpot* hs1 = hs+1;
                                                if (hs->abuts_next_hotspot)
                                                {
                                                    double shear_mid1 = (y >= 0.0)? hs1->shear_mid_upper : hs1->shear_mid_lower;

                                                    bool outside1 = (y*shear_mid1 >= 0.0);
                                                    if (outside1)
                                                    {
                                                        if (roundJoin)
                                                        {
                                                            // only scale if the shear line at this parametric location
                                                            // extends past the hotspot vertex point
                                                            double dd1 = (hs1->mid - xParam) * affected_height_inv;
                                                            if (fabs(shear) > dd1)
                                                            {
                                                                double as2 = 1.0 + shear*shear;

                                                                double rad1 = as2 - dd1*dd1;
                                                                _ASSERT(rad1 >= 0.0);   // guaranteed due to previous if check

                                                                double numer1 = fabs(shear) * dd1 + sqrt(rad1);
                                                                double denom1 = as2;

                                                                // use this scale factor if it's smaller
                                                                if (numer1 < scale * denom1)
                                                                    scale = numer1 / denom1;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            // miter / bevel join
                                                            double dd1 = (hs1->miter_start - xParam) * affected_height_inv;
                                                            double numer1 = 1.0 + dd1 * fabs(shear_mid1);
                                                            double denom1 = 1.0 + shear_mid1 * shear;

                                                            // use this scale factor if it's smaller
                                                            if (numer1 < scale * denom1)
                                                                scale = numer1 / denom1;
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        // get the position of the end of the join
                                        double warp_x = hs->x + hs->cos_angle_end * join_exit_length;
                                        double warp_y = hs->y + hs->sin_angle_end * join_exit_length;

                                        // compute the warped point relative to the end of the join
                                        double txx = -dist_before_exit + scale*y*shear;
                                        double txy = scale*y;

                                        SE_Matrix xform;
                                        xform.rotate(hs->sin_angle_end, hs->cos_angle_end);
                                        xform.transform(txx, txy);

                                        // add this offset to the join's ending point
                                        warp_x += txx;
                                        warp_y += txy;

                                        // update the geometry with the warped point
                                        geom.x_coord(i) = warp_x;
                                        geom.y_coord(i) = warp_y;
                                        last_angleRad = hs->angle_end;

                                        // done with this point
                                        remaining_len = 0.0;
                                        break;
                                    }
                                    else
                                    {
                                        // remaining distance passes the join completely and exits the join
                                        remaining_len -= join_length;
                                    }

                                    if (!hs->abuts_next_hotspot && remaining_len > 0.0)
                                    {
                                        // if we are exiting the join and not immediately entering the
                                        // next join, move along the straight line section that follows
                                        // the join - here we do not need to do a miter warp since we are
                                        // in a straight section (the start / end shear factors are zero)

                                        // this is the total length of the straight section - that's the
                                        // maximum length we can move without being affected by the next join
                                        double dist_along_segment = (hs == last_hotspot)? DBL_MAX : (hs+1)->start - hs->end;

                                        if (remaining_len <= dist_along_segment)
                                        {
                                            // compute position in straight line section past current hotspot

                                            // get the distance from the exit segment's start point to the end of the join
                                            double len_along_seg = join_exit_length + remaining_len;

                                            // just linear interpolation from the start of the segment
                                            double warp_x = hs->x + hs->cos_angle_end * len_along_seg;
                                            double warp_y = hs->y + hs->sin_angle_end * len_along_seg;

                                            // compute the rotated contribution due to y
                                            double txx = 0.0;
                                            double txy = y;

                                            SE_Matrix xform;
                                            xform.rotate(hs->sin_angle_end, hs->cos_angle_end);
                                            xform.transform(txx, txy);

                                            // add this offset to the position
                                            warp_x += txx;
                                            warp_y += txy;

                                            // update the geometry with the warped point
                                            geom.x_coord(i) = warp_x;
                                            geom.y_coord(i) = warp_y;
                                            last_angleRad = hs->angle_end;

                                            // done with this point
                                            remaining_len = 0.0;
                                            break;
                                        }
                                        else
                                        {
                                            remaining_len -= dist_along_segment;
                                        }
                                    }

                                    // go to the next hotspot
                                    hs++;
                                }
                            }
                        }

                        switch (primitive->type)
                        {
                            case SE_RenderPrimitive_Polygon:
                            {
                                // do any necessary polygon clip of the buffer against
                                // the start/end, and then draw the warped buffer
                                SE_RenderPolygon* pl = (SE_RenderPolygon*)primitive;
                                if ((pl->fill & 0xFF000000) != 0)
                                {
                                    // use symbol bounds to quickly check whether we need to clip
                                    // TODO: use the primitive's bounds and not the symbol bounds
                                    LineBuffer* geomToDraw = &geom;
                                    if (sym_minx < startpos || sym_maxx > endpos)
                                    {
                                        // check if symbol is completely outside clip region
                                        if (sym_maxx < startpos || sym_minx > endpos)
                                            geomToDraw = NULL;
                                        else
                                            geomToDraw = SE_LineRenderer::ClipPolygon(lbp, geom, startpos, endpos);
                                    }

                                    if (geomToDraw)
                                    {
                                        renderer->DrawScreenPolygon(geomToDraw, NULL, pl->fill);
                                        if (geomToDraw != &geom)
                                            LineBufferPool::FreeLineBuffer(lbp, geomToDraw);
                                    }
                                }

                                // fall through to the polyline case
                            }

                            case SE_RenderPrimitive_Polyline:
                            {
                                // do any necessary polyline clip of the buffer against
                                // the start/end, and then draw the warped buffer
                                SE_RenderPolyline* pl = (SE_RenderPolyline*)primitive;
                                if ((pl->lineStroke.color & 0xFF000000) != 0)
                                {
                                    // use symbol bounds to quickly check whether we need to clip
                                    // TODO: use the primitive's bounds and not the symbol bounds
                                    LineBuffer* geomToDraw = &geom;
                                    if (sym_minx < startpos || sym_maxx > endpos)
                                    {
                                        // check if symbol is completely outside clip region
                                        if (sym_minx > endpos || sym_maxx < startpos)
                                            geomToDraw = NULL;
                                        else
                                            geomToDraw = SE_LineRenderer::ClipPolyline(lbp, geom, startpos, endpos);
                                    }

                                    if (geomToDraw)
                                    {
                                        renderer->DrawScreenPolyline(geomToDraw, NULL, pl->lineStroke);
                                        if (geomToDraw != &geom)
                                            LineBufferPool::FreeLineBuffer(lbp, geomToDraw);
                                    }
                                }
                            }
                            break;

                            case SE_RenderPrimitive_Text:
                            {
                                SE_RenderText* tp = (SE_RenderText*)primitive;

                                // don't draw primitive if it's completely outside clip region
                                double prim_minx = drawpos + tp->bounds[0].x;
                                double prim_maxx = drawpos + tp->bounds[1].x;
                                if (prim_minx <= endpos && prim_maxx >= startpos)
                                {
                                    // get position and angle to use
                                    double x, y;
                                    geom.get_point(0, x, y);
                                    RS_TextDef tdef = tp->tdef;
                                    tdef.rotation() += last_angleRad * M_180PI;

                                    renderer->DrawScreenText(tp->content, tdef, x, y, NULL, 0, 0.0);
                                }
                            }
                            break;

                            case SE_RenderPrimitive_Raster:
                            {
                                SE_RenderRaster* rp = (SE_RenderRaster*)primitive;
                                ImageData& imgData = rp->imageData;

                                if (imgData.data != NULL)
                                {
                                    // don't draw primitive if it's completely outside clip region
                                    double prim_minx = drawpos + rp->bounds[0].x;
                                    double prim_maxx = drawpos + rp->bounds[1].x;
                                    if (prim_minx <= endpos && prim_maxx >= startpos)
                                    {
                                        // get position and angle to use
                                        double x, y;
                                        geom.get_point(0, x, y);
                                        double angleDeg = (rp->angleRad + last_angleRad) * M_180PI;

                                        renderer->DrawScreenRaster(imgData.data, imgData.size, imgData.format, imgData.width, imgData.height, x, y, rp->extent[0], rp->extent[1], angleDeg);
                                    }
                                }
                            }
                            break;
                        }
                    }

                    // we're finally done with the symbol draw, so move forward by the repeat amount
                    drawpos += increment;
                    sym_minx = drawpos + styleBounds.minx;
                    sym_maxx = drawpos + styleBounds.maxx;

                    // also move on to the next segment if we've already cleared the corner
                    while (next_hotspot != last_hotspot && sym_minx >= next_hotspot->end)
                    {
                        next_hotspot_index++;
                        next_hotspot = &hotspots[next_hotspot_index];
                    }
                }
            }
        }
    }
    catch (...)
    {
    }

    // free the chopped line buffers
    if (choppedBuffers)
    {
        for (unsigned cur_prim=0; cur_prim<rs.size(); ++cur_prim)
        {
            if (choppedBuffers[cur_prim] != NULL)
                LineBufferPool::FreeLineBuffer(lbp, choppedBuffers[cur_prim]);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// This method configures hotspots for the supplied feature geometry.
// Hotspots contain all the information needed to render the warped
// symbol distribution for the polyline.
int SE_LineRenderer::ConfigureHotSpots(SE_Renderer* renderer, LineBuffer* geometry, int cur_contour,
                                       SE_RenderLineStyle* style, RS_Bounds& styleBounds, HotSpot* hotspots)
{
    // This value is used when computing the miter warping.  The maximum
    // warp occurs at + or - affected_height from the centerline and all
    // intermediate warp values are just a fraction of that maximum warp.
    double affected_height = rs_max(fabs(styleBounds.miny), fabs(styleBounds.maxy));

    bool is_closed = geometry->contour_closed(cur_contour);

    // initialize hotspot vertices to the set of reduced points
    int ptCount = SE_LineRenderer::ComputePoints(renderer, geometry, cur_contour, hotspots);
    _ASSERT(ptCount >= 2);

    // compute the parametric positions and entry/exit angles for the hotspots
    hotspots[0].mid = 0.0;
    for (int i=1; i<ptCount; ++i)
    {
        double dx  = hotspots[i].x - hotspots[i-1].x;
        double dy  = hotspots[i].y - hotspots[i-1].y;
        double len = sqrt(dx*dx + dy*dy);
        _ASSERT(len != 0.0);

        hotspots[i].mid = hotspots[i-1].mid + len;

        hotspots[i].angle_start = atan2(dy, dx);
        hotspots[i].cos_angle_start = dx / len;
        hotspots[i].sin_angle_start = dy / len;

        hotspots[i-1].angle_end = hotspots[i].angle_start;
        hotspots[i-1].cos_angle_end = hotspots[i].cos_angle_start;
        hotspots[i-1].sin_angle_end = hotspots[i].sin_angle_start;
    }

    if (is_closed)
    {
        hotspots[        0].angle_start     = hotspots[ptCount-1].angle_start;
        hotspots[        0].cos_angle_start = hotspots[ptCount-1].cos_angle_start;
        hotspots[        0].sin_angle_start = hotspots[ptCount-1].sin_angle_start;

        hotspots[ptCount-1].angle_end       = hotspots[        0].angle_end;
        hotspots[ptCount-1].cos_angle_end   = hotspots[        0].cos_angle_end;
        hotspots[ptCount-1].sin_angle_end   = hotspots[        0].sin_angle_end;
    }
    else
    {
        hotspots[        0].angle_start     = hotspots[        0].angle_end;
        hotspots[        0].cos_angle_start = hotspots[        0].cos_angle_end;
        hotspots[        0].sin_angle_start = hotspots[        0].sin_angle_end;

        hotspots[ptCount-1].angle_end       = hotspots[ptCount-1].angle_start;
        hotspots[ptCount-1].cos_angle_end   = hotspots[ptCount-1].cos_angle_start;
        hotspots[ptCount-1].sin_angle_end   = hotspots[ptCount-1].sin_angle_start;
    }

    double total_length = hotspots[ptCount-1].mid;

    // Configure the start/end parametric positions for the interior
    // hotspots, the shear factors (just upper side - lower side will
    // be done later), and the miter base point locations.
    for (int i=1; i<ptCount-1; ++i)
    {
        // compute the turn angle and ensure it's in the range (-M_PI,M_PI]
        double turn_angle = hotspots[i].angle_end - hotspots[i].angle_start;
        if (turn_angle > M_PI)
            turn_angle -= 2.0*M_PI;
        else if (turn_angle <= -M_PI)
            turn_angle += 2.0*M_PI;

        // half join angle between the two neighboring segments - it's half
        // the supplement of the turn angle
        double half_join_angle = 0.5 * (M_PI - fabs(turn_angle));
        _ASSERT(half_join_angle >= 0.0);
        double cos_ha = cos(half_join_angle);
        double sin_ha = sin(half_join_angle);

        // this is the distance on both sides of the join inside which geometry
        // will need to warp - it is a function of the transverse size of the
        // symbol and the join angle
        double affected_width = 0.0;
        if (half_join_angle < HALF_JOIN_ANGLE_LIMIT)
        {
            // Overlapping segments cannot be handled using warping since they
            // require infinite shear.  Instead we choose "nice" values (zero
            // affected width and zero shear) which result in a bevel join.
            turn_angle = 0.0;
        }
        else if (fabs(turn_angle) > COLINEAR_ANGLE_LIMIT)
            affected_width = WIDTH_FACTOR * affected_height * cos_ha / sin_ha;
//      else
//      {
//          // for small turn angles this is the expansion
//          affected_width = WIDTH_FACTOR * affected_height * 0.5 * fabs(turn_angle);
//      }

        hotspots[i].start = hotspots[i].mid - affected_width;
        hotspots[i].end   = hotspots[i].mid + affected_width;
        hotspots[i].shear_mid_upper   = -tan(0.5*turn_angle);
        hotspots[i].shear_start_upper = 0.0;
        hotspots[i].shear_end_upper   = 0.0;

        // set the parametric positions where each miter line begins and ends
        if (style->vertexJoin == SE_LineJoin_Miter)
        {
            if (style->vertexMiterLimit < sin_ha)
            {
                // past the limit, so treat as bevel
                hotspots[i].miter_start = hotspots[i].mid;
                hotspots[i].miter_end   = hotspots[i].mid;
            }
            else
            {
                if (fabs(turn_angle) > COLINEAR_ANGLE_LIMIT)
                {
                    double offset = affected_height * (style->vertexMiterLimit - sin_ha) / cos_ha;
                    hotspots[i].miter_start = hotspots[i].mid + offset;
                    hotspots[i].miter_end   = hotspots[i].mid - offset;
                }
                else
                {
                    // colinear segments have no miter
                    hotspots[i].miter_start = hotspots[i].mid;
                    hotspots[i].miter_end   = hotspots[i].mid;
                }
            }
        }
        else
        {
            // For bevel and round vertex joins the parametric position of the miter
            // line begins and ends at the hotspot vertex point.  A vertex join of
            // none is treated as bevel.
            hotspots[i].miter_start = hotspots[i].mid;
            hotspots[i].miter_end   = hotspots[i].mid;
        }
    }

    // Configure hotspots for the start and end points.
    if (is_closed)
    {
        // compute the turn angle and ensure it's in the range (-M_PI,M_PI]
        double turn_angle = hotspots[0].angle_end - hotspots[0].angle_start;
        if (turn_angle > M_PI)
            turn_angle -= 2.0*M_PI;
        else if (turn_angle <= -M_PI)
            turn_angle += 2.0*M_PI;

        // half join angle between the two neighboring segments - it's half
        // the supplement of the turn angle
        double half_join_angle = 0.5 * (M_PI - fabs(turn_angle));
        _ASSERT(half_join_angle >= 0.0);
        double cos_ha = cos(half_join_angle);
        double sin_ha = sin(half_join_angle);

        double affected_width = 0.0;
        if (half_join_angle < HALF_JOIN_ANGLE_LIMIT)
        {
            // Overlapping segments cannot be handled using warping since they
            // require infinite shear.  Instead we choose "nice" values (zero
            // affected width and zero shear) which result in a bevel join.
            turn_angle = 0.0;
        }
        else if (fabs(turn_angle) > COLINEAR_ANGLE_LIMIT)
            affected_width = WIDTH_FACTOR * affected_height * cos_ha / sin_ha;
//      else
//      {
//          // for small turn angles this is the expansion
//          affected_width = WIDTH_FACTOR * affected_height * 0.5 * fabs(turn_angle);
//      }

        hotspots[0].start = -affected_width;
        hotspots[0].end   =  affected_width;
        hotspots[0].shear_mid_upper   = -tan(0.5*turn_angle);
        hotspots[0].shear_start_upper = 0.0;
        hotspots[0].shear_end_upper   = 0.0;

        // set the parametric positions where each miter line begins and ends
        if (style->vertexJoin == SE_LineJoin_Miter)
        {
            if (style->vertexMiterLimit < sin_ha)
            {
                // past the limit, so treat as bevel
                hotspots[0].miter_start = hotspots[0].mid;
                hotspots[0].miter_end   = hotspots[0].mid;
            }
            else
            {
                if (fabs(turn_angle) > COLINEAR_ANGLE_LIMIT)
                {
                    double offset = affected_height * (style->vertexMiterLimit - sin_ha) / cos_ha;
                    hotspots[0].miter_start = hotspots[0].mid + offset;
                    hotspots[0].miter_end   = hotspots[0].mid - offset;
                }
                else
                {
                    // colinear segments have no miter
                    hotspots[0].miter_start = hotspots[0].mid;
                    hotspots[0].miter_end   = hotspots[0].mid;
                }
            }
        }
        else
        {
            // For bevel and round vertex joins the parametric position of the miter
            // line begins and ends at the hotspot vertex point.  A vertex join of
            // none is treated as bevel.
            hotspots[0].miter_start = hotspots[0].mid;
            hotspots[0].miter_end   = hotspots[0].mid;
        }

        hotspots[ptCount-1].start = total_length - affected_width;
        hotspots[ptCount-1].end   = total_length + affected_width;
        hotspots[ptCount-1].shear_mid_upper   = -tan(0.5*turn_angle);
        hotspots[ptCount-1].shear_start_upper = 0.0;
        hotspots[ptCount-1].shear_end_upper   = 0.0;

        // set the parametric positions where each miter line begins and ends
        if (style->vertexJoin == SE_LineJoin_Miter)
        {
            if (style->vertexMiterLimit < sin_ha)
            {
                // past the limit, so treat as bevel
                hotspots[ptCount-1].miter_start = hotspots[ptCount-1].mid;
                hotspots[ptCount-1].miter_end   = hotspots[ptCount-1].mid;
            }
            else
            {
                if (fabs(turn_angle) > COLINEAR_ANGLE_LIMIT)
                {
                    double offset = affected_height * (style->vertexMiterLimit - sin_ha) / cos_ha;
                    hotspots[ptCount-1].miter_start = hotspots[ptCount-1].mid + offset;
                    hotspots[ptCount-1].miter_end   = hotspots[ptCount-1].mid - offset;
                }
                else
                {
                    // colinear segments have no miter
                    hotspots[ptCount-1].miter_start = hotspots[ptCount-1].mid;
                    hotspots[ptCount-1].miter_end   = hotspots[ptCount-1].mid;
                }
            }
        }
        else
        {
            // For bevel and round vertex joins the parametric position of the miter
            // line begins and ends at the hotspot vertex point.  A vertex join of
            // none is treated as bevel.
            hotspots[ptCount-1].miter_start = hotspots[ptCount-1].mid;
            hotspots[ptCount-1].miter_end   = hotspots[ptCount-1].mid;
        }
    }
    else
    {
        // give the initial join a finite entry size
        hotspots[0].start             = styleBounds.minx - 1.0;
        hotspots[0].end               = 0.0;
        hotspots[0].shear_mid_upper   = 0.0;
        hotspots[0].shear_start_upper = 0.0;
        hotspots[0].shear_end_upper   = 0.0;
        hotspots[0].miter_start       = hotspots[0].mid;
        hotspots[0].miter_end         = hotspots[0].mid;

        // give the final join a finite exit size
        hotspots[ptCount-1].start             = total_length;
        hotspots[ptCount-1].end               = total_length + styleBounds.maxx + 1.0;
        hotspots[ptCount-1].shear_mid_upper   = 0.0;
        hotspots[ptCount-1].shear_start_upper = 0.0;
        hotspots[ptCount-1].shear_end_upper   = 0.0;
        hotspots[ptCount-1].miter_start       = hotspots[ptCount-1].mid;
        hotspots[ptCount-1].miter_end         = hotspots[ptCount-1].mid;
    }

    // Adjust hotspots so that they don't overlap each other.  If they
    // overlap, move their start and end points so they don't.
    // TODO: consider handling the extended case where a hotspot's
    //       entry/exit region overlaps more than just an adjacent
    //       hotspot's entry/exit region.  In that case we'll want
    //       to adjust the affected height in each region so that
    //       we limit the overlap.
    for (int i=0; i<ptCount-1; ++i)
    {
        HotSpot* h0 = &hotspots[i  ];
        HotSpot* h1 = &hotspots[i+1];

        // if they overlap just move their common point to the middle
        // of the segment that's in between them
        if (h0->end > h1->start)
        {
            h0->end = h1->start = 0.5*(h0->mid + h1->mid);
            h0->abuts_next_hotspot = true;
        }
        else
            h0->abuts_next_hotspot = false;
    }

    if (is_closed)
    {
        hotspots[        0].start = hotspots[ptCount-1].start - total_length;
        hotspots[ptCount-1].end   = hotspots[        0].end   + total_length;
    }

    hotspots[ptCount-1].abuts_next_hotspot = false;

    // The upper side shear factors have been set to nominal values, and we make
    // those the default values for the lower side.  Although these are nominal
    // values, they can lead to undesireable graphical artifacts (long slivers)
    // on the inside portions of highly acute joins due to high levels of warping.
    // To prevent this we'll limit the shear factors on the inside portion of each
    // hotspot so that no points are warped past the hotspot's start / end points.
    for (int i=0; i<ptCount; ++i)
    {
        HotSpot& hs = hotspots[i];

        // set defaults
        hs.shear_mid_lower   = hs.shear_mid_upper;
        hs.shear_start_lower = hs.shear_start_upper;
        hs.shear_end_lower   = hs.shear_end_upper;

        // compute allowable lengths on each side of the hotspot point
        double minX = (i > 0)? hotspots[i-1].mid : hs.start;
        double entryLength = hs.mid - minX;

        double maxX = (i < ptCount-1)? hotspots[i+1].mid : hs.end;
        double exitLength = maxX - hs.mid;

        // the inside shear at the join is limited by the minimum of these
        double max_shear = rs_min(entryLength, exitLength) / affected_height;

        // for negative shear the upper side is towards the inside of the join
        if (hs.shear_mid_upper < 0.0)
        {
            // upper side is towards inside
            if (hs.shear_mid_upper < -max_shear)
                hs.shear_mid_upper = -max_shear;
        }
        else
        {
            // lower side is towards inside
            if (hs.shear_mid_lower > max_shear)
                hs.shear_mid_lower = max_shear;
        }
    }

    // Recompute shear values at common points for abutting hotspots.  The common point
    // for abutting hotspots is the middle of the segment that's in between them (see
    // earlier code), and so the shear at this point is the average of selected shears
    // at the hotspot vertices.
    for (int i=0; i<ptCount-1; ++i)
    {
        HotSpot* h0 = &hotspots[i  ];
        HotSpot* h1 = &hotspots[i+1];

        if (!h0->abuts_next_hotspot)
            continue;

        if (h0->shear_mid_upper < 0.0)
        {
            if (h1->shear_mid_upper < 0.0)
            {
                //
                // \            /
                //  \          /      upper side of h0 towards inside of join
                //   *---)(---*       upper side of h1 towards inside of join
                //  h0        h1
                //
                // allow the shear to vary linearly across the entire upper side of the
                // segment (remember - the shear at the exit side of the hotspot vertex
                // point is negative shear_mid, hence -h0->shear_mid_upper)
                h0->shear_end_upper = h1->shear_start_upper = 0.5*(h1->shear_mid_upper - h0->shear_mid_upper);
            }
            else
            {
                //
                // \
                //  \         h1      upper side of h0 towards inside of join
                //   *---)(---*       lower side of h1 towards inside of join
                //  h0         \
                //              \
                //
                // allow the shear to vary piecewise linearly across the upper side of the
                // segment (remember - the shear at the exit side of the hotspot vertex
                // point is negative shear_mid, hence -h0->shear_mid_upper)
                h0->shear_end_upper = h1->shear_start_upper = 0.5*(h1->shear_mid_lower - h0->shear_mid_upper);
            }

            // Match the lower-side shear to the upper side at the common point.  This
            // gives a piecewise linear shear distribution across the lower side.  If
            // we don't this then if the upper and lower shear values at either hotspot
            // vertex are different then we end up with discontinuous upper and lower
            // shear values along the entire length of the segment.
            h0->shear_end_lower = h1->shear_start_lower = h0->shear_end_upper;
        }
        else
        {
            if (h1->shear_mid_upper < 0.0)
            {
                //
                //              /
                //  h0         /      lower side of h0 towards inside of join
                //   *---)(---*       upper side of h1 towards inside of join
                //  /         h1
                // /
                //
                // allow the shear to vary piecewise linearly across the lower side of the
                // segment (remember - the shear at the exit side of the hotspot vertex
                // point is negative shear_mid, hence -h0->shear_mid_lower)
                h0->shear_end_lower = h1->shear_start_lower = 0.5*(h1->shear_mid_upper - h0->shear_mid_lower);
            }
            else
            {
                //
                //  h0        h1      lower side of h0 towards inside of join
                //   *---)(---*       lower side of h1 towards inside of join
                //  /          \
                // /            \
                //
                // allow the shear to vary linearly across the entire lower side of the
                // segment (remember - the shear at the exit side of the hotspot vertex
                // point is negative shear_mid, hence -h0->shear_mid_lower)
                h0->shear_end_lower = h1->shear_start_lower = 0.5*(h1->shear_mid_lower - h0->shear_mid_lower);
            }

            // Match the upper-side shear to the lower side at the common point.  This
            // gives a piecewise linear shear distribution across the upper side.  If
            // we don't this then if the upper and lower shear values at either hotspot
            // vertex are different then we end up with discontinuous upper and lower
            // shear values along the entire length of the segment.
            h0->shear_end_upper = h1->shear_start_upper = h0->shear_end_lower;
        }
    }

    if (is_closed)
    {
        hotspots[        0].shear_start_upper = hotspots[ptCount-1].shear_start_upper;
        hotspots[        0].shear_start_lower = hotspots[ptCount-1].shear_start_lower;
        hotspots[ptCount-1].shear_end_upper   = hotspots[        0].shear_end_upper;
        hotspots[ptCount-1].shear_end_lower   = hotspots[        0].shear_end_lower;
    }

    return ptCount;
}


///////////////////////////////////////////////////////////////////////////////
// This method does point reduction on the input geometry, and initializes
// the hotspot vertices to the reduced set of points.
// TODO: WCW - enhance the point reduction to replace each blob of
//             closely spaced points with its average position
int SE_LineRenderer::ComputePoints(SE_Renderer* renderer, LineBuffer* geometry, int cur_contour, HotSpot* hotspots)
{
    double ds = renderer->GetDrawingScale();
    double d2min = ds*ds*OPTIMIZE_DISTANCE_SQ;

    // start index and number of points in the point array for current contour
    int start_index = geometry->contour_start_point(cur_contour);
    int numPoints = geometry->cntr_size(cur_contour);
    _ASSERT(numPoints >= 2);    // we should only be called if this is true

    // compute each hotspot position in renderer space
    int ptCount = 0;
    bool skipped = false;
    double x=0.0, y=0.0, dx=0.0, dy=0.0, lastx=0.0, lasty=0.0;
    for (int i=0; i<numPoints; ++i)
    {
        geometry->get_point(start_index+i, x, y);

        // skip points which are too close together
        if (i > 0)
        {
            dx = x - lastx;
            dy = y - lasty;
            if (dx*dx + dy*dy < d2min)
            {
                skipped = true;
                continue;
            }
        }

        skipped = false;

        renderer->WorldToScreenPoint(x, y, hotspots[ptCount].x, hotspots[ptCount].y);
        ++ptCount;

        lastx = x;
        lasty = y;
    }

    // If all the original points are bunched together we can end up with just
    // one hotspot position.
    if (ptCount == 1)
    {
        // get the first point - the last point is still stored in x,y
        geometry->get_point(start_index, lastx, lasty);

        // check if the first and last points are identical
        if (x == lastx && y == lasty)
        {
            // use an offset of 0.1 pixels
            double offset = 0.1*renderer->GetScreenUnitsPerPixel();

            // they're identical - add a second hotspot and adjust both so we
            // have a short (yet valid) segment centered about the original point
            hotspots[1].x = hotspots[0].x;
            hotspots[1].y = hotspots[0].y;
            hotspots[0].x -= offset;
            hotspots[1].x += offset;
            ++ptCount;

            // set this so we don't enter the if block further down
            skipped = false;
        }
        else
        {
            _ASSERT(skipped);

            // they're different - simply bump the hotspot count and the code below
            // will set this hotspot's position to the last point
            ++ptCount;
        }
    }

    // We want the final hotspot position to be the last point in the contour, but
    // we also want to avoid having a tiny segment at the end.  So if we skipped the
    // last point, then simply reset the final hotspot position to the last point.
    if (skipped)
    {
        // the last point is still stored in x,y
        renderer->WorldToScreenPoint(x, y, hotspots[ptCount-1].x, hotspots[ptCount-1].y);
    }

    return ptCount;
}


///////////////////////////////////////////////////////////////////////////////
void SE_LineRenderer::ChopLineBuffer(SE_Renderer* renderer, LineBuffer* inBuffer, LineBuffer* outBuffer)
{
    double pixelsPerScreenUnit = 1.0 / renderer->GetScreenUnitsPerPixel();

    for (int j=0; j<inBuffer->cntr_count(); ++j)
    {
        int start = inBuffer->contour_start_point(j);
        int npts  = inBuffer->cntr_size(j);

        double sx = inBuffer->x_coord(start);
        double sy = inBuffer->y_coord(start);
        outBuffer->MoveTo(sx, sy);

        for (int i=1; i<npts; ++i)
        {
            double ex = inBuffer->x_coord(start+i);
            double ey = inBuffer->y_coord(start+i);

            double dx = ex - sx;
            double dy = ey - sy;

            // get the length in pixels
            double lenpx = sqrt(dx*dx + dy*dy) * pixelsPerScreenUnit;

            // segments shorter than 4 pixels will not be chopped
            // TODO: is 4 the correct amount?
            if (lenpx < 4.0)
            {
                outBuffer->LineTo(ex, ey);
            }
            else if (dx == 0.0)
            {
                // vertical segments do not need to be chopped, except if they cross the
                // x-axis (since the shear on the upper and lower sides can be different)
                if (sy*ey >= 0.0)
                {
                    // line doesn't cross the x-axis
                    outBuffer->LineTo(ex, ey);
                }
                else
                {
                    // line crosses the x-axis
                    outBuffer->LineTo(ex, 0.0);
                    outBuffer->LineTo(ex, ey);
                }
            }
            else
            {
                // segment is long enough to chop up
                //
                // Note that while chopping in more pieces results in more accurate
                // joins, if the underlying rendering engine does not do subpixel
                // positioning then the resulting lines may look too nervous.
                // TODO: ideally we would account for the level of warping
                int num_segs = ((int)lenpx/2);
                double x_incr = dx / num_segs;
                double y_incr = dy / num_segs;

                double cur_x = sx;
                double cur_y = sy;

                for (int ii=0; ii<num_segs; ++ii)
                {
                    cur_x += x_incr;
                    cur_y += y_incr;
                    outBuffer->LineTo(cur_x, cur_y);
                }
            }

            sx = ex;
            sy = ey;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Clips a polyline against the clip region.  A new contour is created
// each time the polyline passes a clip boundary.  The algorithm is just
// Cohen-Sutherland modified for 1D clipping (using the parameter Z).
LineBuffer* SE_LineRenderer::ClipPolyline(LineBufferPool* lbp, LineBuffer& geometry, double zMin, double zMax)
{
    // need at least two points
    if (geometry.point_count() < 2)
        return NULL;

    // first point must be a MoveTo
    if (geometry.point_type(0) != (unsigned char)LineBuffer::stMoveTo)
        return NULL;

    LineBuffer* ret = LineBufferPool::NewLineBuffer(lbp, geometry.point_count(), FdoDimensionality_XY, true);
    ret->SetGeometryType(geometry.geom_type());

    // expand clip region a little so that we don't throw
    // out points which lie on the edge of the clip region
    double zSize = (zMax - zMin) * 1.0e-12;
    zMin -= zSize;
    zMax += zSize;

    double aline[6];
    double bline[4];

    bool move = true;

    for (int i=1; i<geometry.point_count(); ++i)
    {
        if (geometry.point_type(i) == (unsigned char)LineBuffer::stMoveTo)
            move = true;
        else if (geometry.point_type(i) == (unsigned char)LineBuffer::stLineTo)
        {
            geometry.get_point(i-1, aline[0], aline[1], aline[4]);
            geometry.get_point(i  , aline[2], aline[3], aline[5]);
            int res = SE_LineRenderer::ClipLine(zMin, zMax, aline, bline);

            if (res == 0)
            {
                // both points clipped to same side
                continue;
            }
            else if (res == 1)
            {
                // second point was not clipped
                if (move)
                    ret->MoveTo(bline[0], bline[1]);

                ret->LineTo(bline[2], bline[3]);

                move = false;
            }
            else if (res == 2)
            {
                // second point was clipped
                if (move)
                    ret->MoveTo(bline[0], bline[1]);

                ret->LineTo(bline[2], bline[3]);

                move = true;
            }
        }
    }

    return ret;
}


///////////////////////////////////////////////////////////////////////////////
// Clips a polygon against the clip region.  The algorithm is just
// Sutherland-Hodgman modified for 1D clipping (using the parameter Z).
LineBuffer* SE_LineRenderer::ClipPolygon(LineBufferPool* lbp, LineBuffer& geometry, double zMin, double zMax)
{
    // need at least two points
    if (geometry.point_count() < 3)
        return NULL;

    // first point must be a MoveTo
    if (geometry.point_type(0) != (unsigned char)LineBuffer::stMoveTo)
        return NULL;

    LineBuffer* ret = LineBufferPool::NewLineBuffer(lbp, geometry.point_count(), FdoDimensionality_XY, true);
    ret->SetGeometryType(geometry.geom_type());

    // expand clip region a little so that we don't throw
    // out points which lie on the edge of the clip region
    double zSize = (zMax - zMin) * 1.0e-12;
    zMin -= zSize;
    zMax += zSize;

    double aline[6];
    double bline[4];

    bool move = true;
    bool line = false;

    for (int i=1; i<geometry.point_count(); ++i)
    {
        if (geometry.point_type(i) == (unsigned char)LineBuffer::stMoveTo)
        {
            // close any previous segment
            ret->Close();

            move = true;
            line = false;
        }
        else if (geometry.point_type(i) == (unsigned char)LineBuffer::stLineTo)
        {
            geometry.get_point(i-1, aline[0], aline[1], aline[4]);
            geometry.get_point(i  , aline[2], aline[3], aline[5]);
            int res = SE_LineRenderer::ClipLine(zMin, zMax, aline, bline);

            if (res == 0)
            {
                // both points clipped to same side
                continue;
            }
            else if (res == 1)
            {
                // second point was not clipped
                if (move)
                    ret->MoveTo(bline[0], bline[1]);
                else if (line)
                    ret->LineTo(bline[0], bline[1]);

                ret->LineTo(bline[2], bline[3]);

                move = false;
                line = false;
            }
            else if (res == 2)
            {
                // second point was clipped
                if (move)
                    ret->MoveTo(bline[0], bline[1]);
                else if (line)
                    ret->LineTo(bline[0], bline[1]);

                ret->LineTo(bline[2], bline[3]);

                move = false;
                line = true;
            }
        }
    }

    // close any final segment
    ret->Close();

    return ret;
}


///////////////////////////////////////////////////////////////////////////////
// Clips a line against the clip region.
#define INSIDE 0x00
#define LEFT   0x01
#define RIGHT  0x02

int SE_LineRenderer::ClipLine(double zMin, double zMax, double* line, double* ret)
{
    ret[0] = line[0];
    ret[1] = line[1];
    ret[2] = line[2];
    ret[3] = line[3];

    // compute the initial clip codes for the endpoints
    int clipCode1 = SE_LineRenderer::ClipCode(zMin, zMax, line[4]);
    int clipCode2 = SE_LineRenderer::ClipCode(zMin, zMax, line[5]);

    // trivially reject or accept the line segment
    if ((clipCode1 & clipCode2) != 0)   // both points left or both points right
        return 0;
    if (clipCode1 == clipCode2)         // both points inside
        return 1;

    // compute the difference between the x and y values of the endpoints
    double deltaX = line[2] - line[0];
    double deltaY = line[3] - line[1];
    double deltaZ = line[5] - line[4];
    _ASSERT(deltaZ != 0.0);

    // check the first point
    if (clipCode1 == LEFT)
    {
        ret[0] = line[0] + (zMin - line[4]) * deltaX / deltaZ;
        ret[1] = line[1] + (zMin - line[4]) * deltaY / deltaZ;
    }
    else if (clipCode1 == RIGHT)
    {
        ret[0] = line[0] + (zMax - line[4]) * deltaX / deltaZ;
        ret[1] = line[1] + (zMax - line[4]) * deltaY / deltaZ;
    }

    // check the second point
    int retcode = 1;
    if (clipCode2 == LEFT)
    {
        ret[2] = line[2] + (zMin - line[5]) * deltaX / deltaZ;
        ret[3] = line[3] + (zMin - line[5]) * deltaY / deltaZ;
        retcode = 2;
    }
    else if (clipCode2 == RIGHT)
    {
        ret[2] = line[2] + (zMax - line[5]) * deltaX / deltaZ;
        ret[3] = line[3] + (zMax - line[5]) * deltaY / deltaZ;
        retcode = 2;
    }

    return retcode;
}


///////////////////////////////////////////////////////////////////////////////
// Returns whether a point is inside the clip region, or to the left or right.
int SE_LineRenderer::ClipCode(double zMin, double zMax, double z)
{
    if (z < zMin)
        return LEFT;

    if (z > zMax)
        return RIGHT;

    return INSIDE;
}
