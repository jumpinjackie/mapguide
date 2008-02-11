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

#ifndef SE_LINEARCLIP_H
#define SE_LINEARCLIP_H

#include "StylizationDefs.h"
#include "LineBuffer.h"
#include "SE_Tuple.h"
#include <vector>
#include <list>

struct ClipNode
{
    SE_Tuple intersection;
    int      point_low;
    int      point_high;
    int      contour;
    int      cntr_node_cnt;
    bool     is_cw;
    bool     is_even;

    std::list<ClipNode*>::iterator back_ptr;

    ClipNode(const SE_Tuple& isection, int low, int cntr, bool cw) :
        intersection(isection),
        point_low(low),
        contour(cntr),
        is_cw(cw)
    {
    }

    ClipNode(const SE_Tuple& isection, int low, int high, int ctr, bool cw) :
        intersection(isection),
        point_low(low),
        point_high(high),
        contour(ctr),
        is_cw(cw)
    {
    }
};

template<class T>
    void swapp(T& left, T& right)
{
    const T tmp = left;
    left = right;
    right = tmp;
}

/*
 * CLIP_INFO provides the functions that determine the clipping behavior of
 * SE_LinearClip.  SE_LinearClip will work as long as CLIP_INFO clips to a
 * straight line (for a polyline or curve clip, potentially a line segment
 * could intersect the clip line multiple times, which would require a more
 * complex algorithm).  Further, CLIP_INFO must perturb points that are exactly
 * on the clip line, so that no clip line intersection coincides exactly with an
 * endpoint, with the constraint that this perturbation must be consistent for
 * a point (i.e. it should be consistent over multiple calls to intersect,
 * clockwise, operator(), etc.).
 *
 * CLIP_INFO is required to implement:
 *
 * // returns true if (start, end) intersects *linear* clip line, and stores
 * // intersection point in isect.
 * bool intersect(const SE_Tuple& start,
 *                const SE_Tuple& end,
 *                SE_Tuple& isect);
 *
 * // returns true if point is clockwise of the clip line
 * bool clockwise(const SE_Tuple& point);
 *
 * // orders the clip nodes by their position along the clip line
 * bool operator()(const ClipNode* _Left, const ClipNode* _Right);
 */
template<class CLIP_INFO> struct SE_LinearClip
{
public:
    /*
     * Clips the geometry src according to the behavior of info.  If dst_cw (resp. dst_ccw)
     * is not null, the portion of the geometry that is clockwise (resp. counter-clockwise)
     * of the clip line is stored in dst_cw (resp. dst_ccw).  It is possible for dst_cw and
     * dst_ccw to point to the same line buffer.  The algorithm (which, admittedly, I just
     * made up, and thus comes with no guarantee of optimality--although I doubt that there
     * is a linear time algorithm that works with holes) works for general polylines or
     * polygons (convex, self-intersecting, etc.) with any number of contours. The worst-
     * case running time is O(n log n).  Well, really, it is O(n + m log m) where m < n, n
     * being the number of points in the source geometry, and m being the number of segments
     * that intersect the clipline (which are merge-sorted by std::list).
     * TODO: Add support for multiple geometries in input buffer.
     */
    void Clip(CLIP_INFO& info,
              LineBufferPool* pool,
              LineBuffer* src,
              LineBuffer* dst_cw,
              LineBuffer* dst_ccw);

private:
    void ClipPolyline(CLIP_INFO& info, LineBuffer* src, LineBuffer* dst_cw, LineBuffer* dst_ccw);
    void ClipPolygon(CLIP_INFO& info, LineBuffer* src, LineBuffer* dst_cw, LineBuffer* dst_ccw);
    void ConsumeContour(std::list<ClipNode*>& segs, LineBuffer* src, LineBuffer* dst, bool cw);

    SE_INLINE ClipNode* Next(ClipNode* node);
    SE_INLINE ClipNode* Previous(ClipNode* node);
    SE_INLINE int NextPoint(ClipNode* node, LineBuffer* geom, int point);
    SE_INLINE int PreviousPoint(ClipNode* node, LineBuffer* geom, int point);
    SE_INLINE int NumPoints(ClipNode* node, LineBuffer* geom);

    std::vector<ClipNode>  m_nodes;
    std::list<ClipNode*>   m_cw_order;
    std::list<ClipNode*>   m_ccw_order;
};


template<class CLIP_INFO>
    void SE_LinearClip<CLIP_INFO>::Clip( CLIP_INFO& info,
                                         LineBufferPool* pool,
                                         LineBuffer* src,
                                         LineBuffer* dst_cw,
                                         LineBuffer* dst_ccw )
{
    if ((dst_cw || dst_ccw) && src->cntr_count())
    {
        _ASSERT(src->geom_type() == FdoGeometryType_LineString ||
                src->geom_type() == FdoGeometryType_MultiLineString ||
                src->geom_type() == FdoGeometryType_Polygon ||
                src->geom_type() == FdoGeometryType_MultiPolygon);

        if (src->geom_type() == FdoGeometryType_LineString ||
            src->geom_type() == FdoGeometryType_MultiLineString)
        {
            /*
             * The ClipPolygon function calculates the cw and ccw portions sepearately,
             * so there is no problem when there is only one destination buffer.
             * ClipPolyline, however, is one-pass, so we create a temporary buffer
             * to accept the ccw portion, and then combine the buffers afterward
             * (this should still be faster than making two passes).
             */
            if (dst_cw == dst_ccw)
            {
                LineBuffer* tmp = LineBufferPool::NewLineBuffer(pool, src->point_count());
                ClipPolyline(info, src, tmp, dst_cw);
                *dst_cw += *tmp;
                LineBufferPool::FreeLineBuffer(pool, tmp);
            }
            else
                ClipPolyline(info, src, dst_cw, dst_ccw);
        }
        else
            ClipPolygon(info, src, dst_cw, dst_ccw);
    }
}


template<class CLIP_INFO>
    void SE_LinearClip<CLIP_INFO>::ClipPolygon( CLIP_INFO& info,
                                                LineBuffer* src,
                                                LineBuffer* dst_cw,
                                                LineBuffer* dst_ccw )
{
    int ncntrs = src->cntr_count();
    for (int i = 0; i < ncntrs; ++i)
    {
        int start = src->contour_start_point(i);
        int end = src->contour_end_point(i);

        const SE_Tuple* prev;
        const SE_Tuple* cur = (const SE_Tuple*)&(src->x_coord(start));
        bool isCW = info.clockwise(*cur);
        int cntr_nodes = 0;
        SE_Tuple isect;

        for (int j = start + 1; j <= end; ++j)
        {
            prev = cur;
            cur = (const SE_Tuple*)&(src->x_coord(j));
            if (info.intersect(*prev, *cur, isect))
            {
                if (cntr_nodes++)
                    m_nodes.back().point_high = j - 1;
                m_nodes.push_back(ClipNode(isect, j, i, isCW = !isCW));
            }
        }
        if (cntr_nodes)
        {
            m_nodes.back().point_high = m_nodes[m_nodes.size() - cntr_nodes].point_low - 1;
            for (int j = 1; j <= cntr_nodes; ++j)
                m_nodes[m_nodes.size() - j].cntr_node_cnt = cntr_nodes;
        }
        else
        {
            LineBuffer* dst = isCW ? dst_cw : dst_ccw;
            if (dst)
            {
                dst->EnsureContours(1);
                dst->EnsurePoints(end - start + 1);
                dst->UnsafeMoveTo(src->x_coord(start), src->y_coord(start));
                for (int j = start + 1; j <= end; ++j)
                    dst->UnsafeLineTo(src->x_coord(j), src->y_coord(j));
            }
        }

        /* If there are an odd # of crossings, the contour was not actually closed... */
        _ASSERT((cntr_nodes & 1) == 0);
    }

    /* Sort nodes in order by position along the clipping line */
    std::list<ClipNode*>& fl = dst_cw ? m_cw_order : m_ccw_order;
    for (unsigned int i = 0; i < m_nodes.size(); ++i)
        fl.push_back(&m_nodes[i]);
    fl.sort<CLIP_INFO>(info);
    if (dst_cw && dst_ccw)
        m_ccw_order = m_cw_order;

    /*
     * Traverse all the CW and CCW contours. We are using even-odd filling, so an
     * even-numbered node will connect to the next node in the vector, and an odd-
     * numbered node will connect to the previous node in the vector. Save the
     * evenness, since it would be expensive to calculate later (it would require
     * traversal to an endpoint of the list).
     */
    if (m_cw_order.size())
    {
        bool even = true;
        for (std::list<ClipNode*>::iterator iter = m_cw_order.begin();
             iter != m_cw_order.end(); ++iter)
        {
            (*iter)->back_ptr = iter;
            (*iter)->is_even = even;
            even = !even;
        }
        do
        {
            ConsumeContour(m_cw_order, src, dst_cw, true);
        } while (m_cw_order.size());
    }
    if (m_ccw_order.size())
    {
        bool even = true;
        for (std::list<ClipNode*>::iterator iter = m_ccw_order.begin();
             iter != m_ccw_order.end(); ++iter)
        {
            (*iter)->back_ptr = iter;
            (*iter)->is_even = even;
            even = !even;
        }
        do
        {
            ConsumeContour(m_ccw_order, src, dst_ccw, false);
        } while (m_ccw_order.size());
    }

    m_nodes.clear();
    m_cw_order.clear();
    m_ccw_order.clear();
}


/*
 * In this case (which works for all polylines), we can simply check
 * each segment for an intersection, and in each such case, start
 * adding subsequent points to the other line buffer.
 */
template<class CLIP_INFO>
    void SE_LinearClip<CLIP_INFO>::ClipPolyline( CLIP_INFO& info,
                                                 LineBuffer* src,
                                                 LineBuffer* dst_cw,
                                                 LineBuffer* dst_ccw )
{
    int ncntrs = src->cntr_count();
    for (int i = 0; i < ncntrs; ++i)
    {
        int start = src->contour_start_point(i);
        int end = src->contour_end_point(i);

        const SE_Tuple* prev;
        const SE_Tuple* cur = (const SE_Tuple*)&(src->x_coord(start++));
        SE_Tuple isect;
        LineBuffer *dst = dst_ccw, *other = dst_cw;

        if (info.clockwise(*cur))
            swapp<LineBuffer*>(dst, other);

        if (dst)
        {
            dst->EnsureContours(1);
            dst->EnsurePoints(end - start + 1);
            dst->UnsafeMoveTo(cur->x, cur->y);
        }

        for (int j = start; j <= end; ++j)
        {
            prev = cur;
            cur = (const SE_Tuple*)&(src->x_coord(j));
            if (info.intersect(*prev, *cur, isect))
            {
                if (dst)
                    dst->UnsafeLineTo(isect.x, isect.y);
                if (other)
                {
                    other->EnsureContours(1);
                    other->EnsurePoints(end - j + 2);
                    other->UnsafeMoveTo(isect.x, isect.y);
                    other->UnsafeLineTo(cur->x, cur->y);
                }
                swapp<LineBuffer*>(dst, other);
            }
            else if (dst)
                dst->UnsafeLineTo(cur->x, cur->y);
        }
    }
}


template<class CLIP_INFO>
    void SE_LinearClip<CLIP_INFO>::ConsumeContour( std::list<ClipNode*>& segs,
                                                   LineBuffer* src,
                                                   LineBuffer* dst,
                                                   bool cw)
{
    ClipNode* start = segs.front();

    dst->EnsureContours(1);
    dst->EnsurePoints(1);
    dst->UnsafeMoveTo(start->intersection.x, start->intersection.y);

    ClipNode* current = start;
    do
    {
        ClipNode* endpt;
        if (current->is_cw == cw)
        {
            int npts = NumPoints(current, src);
            dst->EnsurePoints(npts + 2);
            for (int i = current->point_low, j = 0; j < npts; i = NextPoint(current, src, i), ++j)
                dst->UnsafeLineTo(src->x_coord(i), src->y_coord(i));
            endpt = Next(current);
            dst->UnsafeLineTo(endpt->intersection.x, endpt->intersection.y);
        }
        else
        {
            endpt = Previous(current);
            int npts = NumPoints(endpt, src);
            dst->EnsurePoints(npts + 2);
            for (int i = endpt->point_high, j = 0; j < npts; i = PreviousPoint(endpt, src, i), ++j)
                dst->UnsafeLineTo(src->x_coord(i), src->y_coord(i));
            dst->UnsafeLineTo(endpt->intersection.x, endpt->intersection.y);
        }

        /* Intersection zero is connected to one, two to three, etc,
         * as a result of even-odd filling rules. */
        std::list<ClipNode*>::const_iterator iter = endpt->back_ptr;
        current = (*iter)->is_even ? *(++iter) : *(--iter);
        dst->UnsafeLineTo(current->intersection.x, current->intersection.y);

        segs.erase(endpt->back_ptr);
        segs.erase(current->back_ptr);

    } while (current != start);
}


template<class CLIP_INFO>
    ClipNode* SE_LinearClip<CLIP_INFO>::Next(ClipNode* node)
{
    ClipNode* next = node == &m_nodes.back() ? &m_nodes[0] : node + 1;
    return next->contour != node->contour ? node - node->cntr_node_cnt + 1 :
        next;
}


template<class CLIP_INFO>
    ClipNode* SE_LinearClip<CLIP_INFO>::Previous(ClipNode* node)
{
    ClipNode* prev = node == &m_nodes[0] ? &m_nodes.back() : node - 1;
    return prev->contour != node->contour ? node + node->cntr_node_cnt - 1 :
        prev;
}


template<class CLIP_INFO>
    int SE_LinearClip<CLIP_INFO>::NextPoint(ClipNode* node, LineBuffer* geom, int point)
{
    /* Skip the first point, since it is identical to the last point */
    return point == geom->contour_end_point(node->contour) ?
        geom->contour_start_point(node->contour) + 1: point + 1;
}


template<class CLIP_INFO>
    int SE_LinearClip<CLIP_INFO>::PreviousPoint(ClipNode* node, LineBuffer* geom, int point)
{
    /* Skip the last point, since it is identical to the first point */
    return point == geom->contour_start_point(node->contour) ?
        geom->contour_end_point(node->contour) - 1: point - 1;
}


template<class CLIP_INFO>
    int SE_LinearClip<CLIP_INFO>::NumPoints(ClipNode* node, LineBuffer* geom)
{
    return node->point_high >= node->point_low ? node->point_high - node->point_low + 1 :
        geom->contour_end_point(node->contour) - node->point_low +
        node->point_high - geom->contour_start_point(node->contour) + 1;
}


/**********************************************************************
 * VERTICAL CLIP
 */

/*
 * Points exactly on the clip line will all be perturbed away from zero...
 * perturbing all points in the same direction has the advantage of moving
 * entire columns of points that would otherwise form a number of intersecting
 * segments entirely off of the clip line.
 */
struct VerticalClip
{
    VerticalClip(double clipx);

    bool intersect(const SE_Tuple& start,
                   const SE_Tuple& end,
                   SE_Tuple& isect) const;
    bool clockwise(const SE_Tuple& point) const;
    bool operator()(const ClipNode* left, const ClipNode* right) const;

private:
    double m_clip;
    double m_perturbation;
};


/**********************************************************************
 * GENERAL CLIP
 */

/* General clip works for all simple cliplines */
struct GeneralClip
{
    /* Clips to the line defined by point clipbase and vector clipvector.  Clipbase
     * must be outside of the possible bounds of input geometry--preferably barely
     * outside so we don't lose precision (e.g. 1e100 units outside would not be a
     * very good idea). */
    GeneralClip(const SE_Tuple& clipbase, const SE_Tuple& clipvector);

    bool intersect(const SE_Tuple& start,
                   const SE_Tuple& end,
                   SE_Tuple& isect) const;
    bool clockwise(const SE_Tuple& point) const;
    bool operator()(const ClipNode* left, const ClipNode* right) const;

private:
    SE_Tuple m_clippt;
    SE_Tuple m_clipln;
    SE_Tuple m_perturbation;
};

#endif // SE_LINEARCLIP_H
