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
#include "SE_JoinTransform.h"

//--------------------------------------------------------------
// SE_JoinTransform::Transformer function definitions
//--------------------------------------------------------------


SE_JoinTransform::Transformer::Transformer(SE_JoinTransform& buffer, double height, double clip_min, double clip_max) :
m_buffer(&buffer),
m_cur_cache(NULL),
m_inv_height(1.0 / height),
m_in_idx(0),
m_out_idx(0),
m_break_idx(0),
m_in_active(false),
m_next_pts(10)
{
    m_clip_ext[0] = clip_min;
    m_clip_ext[1] = clip_max;
}


// Uninitialized use of the iterator will be detectable by a caller when the
// caller finds itself dereferencing double-digit pointers.
SE_JoinTransform::Transformer::Transformer() :
m_buffer(NULL),
m_cur_cache(NULL),
m_in_idx(0),
m_out_idx(0),
m_in_active(false),
m_next_pts(0)
{
}


void SE_JoinTransform::Transformer::Move(const SE_Tuple& xy, const SE_Tuple& direction)
{
    if (m_in_active && xy.y > 0.0)
        m_in_active = false;
    else if (!m_in_active && xy.y < 0.0)
        m_in_active = true;
    else if (xy.y == 0.0 && direction.y != 0.0)
        m_in_active = direction.y < 0.0;

    Find(xy.x, direction.x);
    EvaluateCache();

    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);
}


void SE_JoinTransform::Transformer::Vertical()
{
    m_in_active = !m_in_active;

    _ASSERT(m_last_uv.y < DEBUG_TOLERANCE && m_last_uv.y > -DEBUG_TOLERANCE);

    double x = m_cur_low_data[0].pos + m_last_uv.x * (m_cur_low_data[1].pos - m_cur_low_data[0].pos);

    // for a vertical movement, choose dx to keep us on the same side of a discontinuity
    Find(x, m_last_uv.x < 0.5 ? 1.0 : -1.0);
    EvaluateCache();

    m_last_uv.x = (x - m_cur_low_data->pos) * (m_cur_cache->inv_width);
}


void SE_JoinTransform::Transformer::Backward()
{
    _ASSERT(m_last_uv.x < DEBUG_TOLERANCE && m_last_uv.x > -DEBUG_TOLERANCE);

    // the contours should have been broken over discontinuities
    _ASSERT(m_cur_low_data[0].pos != m_cur_low_data[-1].pos);

    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);

    --m_cur_low_data;

    if (m_in_active)
        m_cur_cache = &m_buffer->m_in_cache[--m_in_idx];
    else
        m_cur_cache = &m_buffer->m_out_cache[--m_out_idx];

    m_last_uv.x = 1.0;

    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);

    _ASSERT(m_in_idx < (int)m_buffer->m_in_tx.size() - 1 && m_in_idx >= 0 &&
            m_out_idx < (int)m_buffer->m_out_tx.size() - 1 && m_out_idx >= 0);

    EvaluateCache();

    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);
}


void SE_JoinTransform::Transformer::Forward()
{
    _ASSERT(m_last_uv.x > (1.0 - DEBUG_TOLERANCE) && m_last_uv.x < (1.0 + DEBUG_TOLERANCE));

    // the contours should have been broken over discontinuities
    _ASSERT(m_cur_low_data[1].pos != m_cur_low_data[2].pos);

    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);

    ++m_cur_low_data;

    if (m_in_active)
        m_cur_cache = &m_buffer->m_in_cache[++m_in_idx];
    else
        m_cur_cache = &m_buffer->m_out_cache[++m_out_idx];

    m_last_uv.x = 0.0;

    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);

    _ASSERT(m_in_idx < (int)m_buffer->m_in_tx.size() - 1 && m_in_idx >= 0 &&
            m_out_idx < (int)m_buffer->m_out_tx.size() - 1 && m_out_idx >= 0);

    EvaluateCache();

    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);
}


void SE_JoinTransform::Transformer::Find(double x, double dx)
{
    // for now, naive linear search
    std::vector<TxData>* pTx;
    const SE_Deque<TxCache>* pCache;
    int* pIndex;

    _ASSERT(!m_cur_cache || m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);

    if (m_in_active)
    {
        pTx = &m_buffer->m_in_tx;
        pCache = &m_buffer->m_in_cache;
        pIndex = &m_in_idx;
    }
    else
    {
        pTx = &m_buffer->m_out_tx;
        pCache = &m_buffer->m_out_cache;
        pIndex = &m_out_idx;
    }

    std::vector<TxData>& tx = *pTx;
    const SE_Deque<TxCache>& cache = *pCache;
    int& index = *pIndex;

    _ASSERT(index >= 0 && index < (int)tx.size() - 1);

    // Sanity check for current index.  Is our state invalid?
    if (index < 0)
        index = 0;
    if (index >= (int)tx.size() - 1)
        index = (int)tx.size() - 2;

    // Sanity check, return closest values for out-of-bounds points
    _ASSERT(x >= m_clip_ext[0] - DEBUG_TOLERANCE && x <= m_clip_ext[1] + DEBUG_TOLERANCE);
    if (x < tx[0].pos)
    {
        m_cur_cache = &cache[0];
        m_cur_low_data = &tx[0];
        index = 0;
        return;
    }
    else if (x > tx.back().pos)
    {
        m_cur_low_data = &tx.back() - 1;
        m_cur_cache = &cache.tail();
        index = (int)tx.size() - 2;
        return;
    }

    if (x >= tx[index].pos)
        // TODO: remove. additional check to abort on aberrant transform data
        for (;index < (int)tx.size()-1 && tx[index+1].pos >= tx[index].pos && x > tx[index+1].pos; ++index);
    else
        // TODO: remove. additional check to abort on aberrant transform data
        for (;index >                0 && tx[index-1].pos <= tx[index].pos && x < tx[index-1].pos; --index);

    _ASSERT(m_in_idx < (int)m_buffer->m_in_tx.size() - 1 && m_in_idx >= 0 &&
            m_out_idx < (int)m_buffer->m_out_tx.size() - 1 && m_out_idx >= 0);

    // position correctly with respect to discontinuities
    if (tx[index].pos == tx[index+1].pos)
        index += (dx > 0.0)? 1 : -1;
    else if (tx[index  ].pos == x && dx < 0.0)
        index -= 2;
    else if (tx[index+1].pos == x && dx > 0.0)
        index += 2;

    m_cur_cache = &cache[index];
    m_cur_low_data = &tx[index];
    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);
}


void SE_JoinTransform::Transformer::EvaluateCache()
{
    if (m_cur_cache->inv_width == 0.0)
    {
        const TxData& low = m_cur_low_data[0];
        const TxData& high = m_cur_low_data[1];

        m_cur_cache->d_m_c = high.ctr - low.ctr;
        m_cur_cache->a_m_c = low.out - low.ctr;
        m_cur_cache->bc_m_ad = high.out + low.ctr - low.out - high.ctr;

        m_cur_cache->bcad_len = m_cur_cache->bc_m_ad.length();
        m_cur_cache->inv_width = 1.0 / (high.pos - low.pos);
#ifdef _DEBUG
        m_cur_cache->low_data = m_cur_low_data;
#endif
    }
}


void SE_JoinTransform::Transformer::AddPoint(const SE_Tuple& point)
{
    if (m_cur_dst->cntr_size(m_cur_dst->cntr_count() - 1) > 1)
    {
        SE_Tuple *ult, *penult;
        SE_Tuple dv;

        ult = (SE_Tuple*)&m_cur_dst->x_coord(m_cur_dst->point_count() - 1);
        penult = ult - 1;
        dv = *ult - *penult;

        // TODO: use current tolerance
        // if last seg is < 0.25 long, combine segs
        if (dv.lengthSquared() < 0.25)
        {
            *ult = point;
            return;
        }
    }
    m_cur_dst->UnsafeLineTo(point.x, point.y);
}


void SE_JoinTransform::Transformer::BeginContour(const SE_Tuple& point)
{
    m_cur_dst->UnsafeMoveTo(point.x, point.y);
}


double& SE_JoinTransform::Transformer::CurrentInvTolerance()
{
    return m_cur_low_data->inv_tol;
}


void SE_JoinTransform::Transformer::LineToUV(const SE_Tuple& point, SE_Tuple& uv)
{
    uv.x = (point.x - m_cur_low_data[0].pos) * m_cur_cache->inv_width;
    uv.y = fabs(point.y) * m_inv_height;
    _ASSERT(uv.x >= 0.0 - DEBUG_TOLERANCE && uv.x <= 1.0 + DEBUG_TOLERANCE &&
            uv.y >= 0.0 - DEBUG_TOLERANCE && uv.y <= 1.0 + DEBUG_TOLERANCE);
}


void SE_JoinTransform::Transformer::MapPoint(const SE_Tuple& uv, SE_Tuple& world)
{
    world = (m_cur_low_data[1].ctr * ((1.0 - uv.y) *        uv.x )) +
            (m_cur_low_data[0].ctr * ((1.0 - uv.y) * (1.0 - uv.x))) +
            (m_cur_low_data[0].out * ((1.0 - uv.x) *        uv.y )) +
            (m_cur_low_data[1].out * (       uv.x  *        uv.y ));
}


/*
 * T(a+i,b+j) = T(a,b) + i*(dT/du)(a,b) + j*(dT/dv)(a,b) + i*j*(d2T/du/dv)(a,b)
 *
 * Let e be [0,1], yielding a point between (a,b) and (a+i, b+j)
 *
 * T(a+ei,b+ej) = T(a,b) + ie*(dT/du)(a,b) + je*(dT/dv)(a,b) + i*j*e^2*(d2T/du/dv)(a,b)
 *
 * Let T' be the linear approximation function for T between (a,b) and (a+i,b+j).
 *
 * T'(a+ei,b+ej) = T(a,b) + e*(T(a+i,b+j) - T(a,b))
 *               = T(a,b) + e*(i*(dT/du)(a,b) + j*(dT/dv)(a,b) + i*j*(d2T/du/dv)(a,b))
 *               = T(a,b) + ie*(dT/du)(a,b) + je*(dT/dv)(a,b) + i*j*e*(d2T/du/dv)(a,b)
 *
 * The E be the absolute error of the approximation:
 *
 * E(e,i,j) = abs(T' - T)
 *          = abs(i*j*e*(d2T/du/dv)(a,b) - i*j*e^2*(d2T/du/dv)(a,b))
 *          = abs((e-e^2)*i*j(d2T/du/dv)(a,b))
 *          = abs((e-e^2)*i*j*(B+C-A-D)
 *
 * Max(0<=e<=1, abs(E(e)) = abs(i*j*(B+C-A-D)/4)
 *
 * Thus, no point on the direct line between (a,b) and (a+i,b+j) differs by more than
 * (i*j*0.25)*(A+C-B-D) from T.  Consequently, if the destination quadrilateral is a
 * rectangle (A+C==B+D), or if du == 0 (vertical line), or dv == 0 (horizontal line),
 * the approximation error will be zero.
 *
 * Regardless of the error incurred in approximating the line, the endpoint will be
 * exact, so the error in mapping a segment over a transform does not affect the error
 * over subsequent transforms.
 */
void SE_JoinTransform::Transformer::MapSegment(const SE_Tuple& target_uv, const double& invtolerance)
{
    SE_Tuple dp = target_uv - m_last_uv;

    SE_Tuple dPt, ddPt, d2Tdudv;
    double dxdy = dp.x * dp.y;
    double err = fabs(dxdy) * 0.25 * m_cur_cache->bcad_len;

    int segs = (int)(sqrt(err * invtolerance)) + 1;
    double invsegs = 1.0 / (double)segs;
    dp *= invsegs;

    // Is the number of segments absurdly high?
    _ASSERT(segs < 100000);

    dPt = m_cur_cache->d_m_c * dp.x + m_cur_cache->a_m_c * dp.y +
          m_cur_cache->bc_m_ad * (m_last_uv.y * dp.x + m_last_uv.x * dp.y);
    d2Tdudv = m_cur_cache->bc_m_ad * dxdy;
    dPt += d2Tdudv;
    ddPt = d2Tdudv * 2.0;

    SE_Tuple cur_pt_scrn = m_last_scrn;

    m_cur_dst->EnsurePoints(segs);
    for (int i = 0; i<segs; ++i, dPt += ddPt)
    {
        cur_pt_scrn += dPt;
        AddPoint(cur_pt_scrn);
    }

    m_last_scrn = cur_pt_scrn;
    m_last_uv = target_uv;

    // return the actual error
//  return err * invsegs * invsegs;
}


LineBuffer* SE_JoinTransform::Transformer::ApplyBreaks(LineBuffer* src, double position, LineBufferPool* pool)
{
    LineBuffer* clipbuf = src;
    if (src->bounds().minx + position < m_clip_ext[0])
    {
        clipbuf = LineBufferPool::NewLineBuffer(pool, src->point_count());
        clipbuf->SetGeometryType(src->geom_type());
        VerticalClip vc(m_clip_ext[0] - position);
        m_clip.Clip(vc, pool, src, clipbuf, NULL);
    }
    if (src->bounds().maxx + position > m_clip_ext[1])
    {
        LineBuffer* cbuf = LineBufferPool::NewLineBuffer(pool, src->point_count());
        cbuf->SetGeometryType(src->geom_type());
        VerticalClip vc(m_clip_ext[1] - position);
        m_clip.Clip(vc, pool, clipbuf, NULL, cbuf);
        if (clipbuf != src)
            LineBufferPool::FreeLineBuffer(pool, clipbuf);
        clipbuf = cbuf;
    }

    if (m_buffer->m_breaks.size() && clipbuf->point_count())
    {
        double minext = src->bounds().minx + position,
            maxext = src->bounds().maxx + position;

        int maxidx = (int)m_buffer->m_breaks.size() - 1;

        while (m_break_idx < maxidx && m_buffer->m_breaks[m_break_idx] < minext)
            ++m_break_idx;

        while (m_break_idx > 0 && m_buffer->m_breaks[m_break_idx-1] > minext)
            --m_break_idx;

        m_tx_stack.push_back(clipbuf);
        while (m_buffer->m_breaks[m_break_idx] < maxext)
        {
            LineBuffer* ccw = LineBufferPool::NewLineBuffer(pool, m_tx_stack.back()->point_count() * 2);
            LineBuffer* cw =  LineBufferPool::NewLineBuffer(pool, m_tx_stack.back()->point_count() * 2);
            ccw->SetGeometryType(src->geom_type());
            cw->SetGeometryType(src->geom_type());

            VerticalClip vc(m_buffer->m_breaks[m_break_idx] - position);
            m_clip.Clip(vc, pool, m_tx_stack.back(), cw, ccw);

            if (m_tx_stack.back() != src)
                LineBufferPool::FreeLineBuffer(pool, m_tx_stack.back());
            m_tx_stack.pop_back();
            m_tx_stack.push_back(ccw);
            m_tx_stack.push_back(cw);

            if (m_break_idx == maxidx)
                break;
            ++m_break_idx;
        }

        int sections = (int)m_tx_stack.size();
        if (sections > 1)
        {
            for (int i = 1; i < sections; ++i)
            {
                *m_tx_stack[0] += *m_tx_stack[i];
                LineBufferPool::FreeLineBuffer(pool, m_tx_stack[i]);
            }
            clipbuf = m_tx_stack[0];
        }
        m_tx_stack.clear();
    }

    return clipbuf;
}


LineBuffer* SE_JoinTransform::Transformer::TransformLine(LineBuffer* geom, double position, LineBufferPool* lbp)
{
    _ASSERT(geom->point_count() > 1);

    LineBuffer* src = ApplyBreaks(geom, position, lbp);
    m_cur_dst = LineBufferPool::NewLineBuffer(lbp, src->point_count() * 2);
    m_cur_dst->SetGeometryType(geom->geom_type());

    int cntrs = 0;

    for (int j = 0; j < src->geom_count(); ++j)
    {
        m_cur_dst->NewGeometry();
        for(int i = cntrs; i < cntrs + src->geom_size(j); ++i)
        {
            int curidx = src->contour_start_point(i);
            int endidx = src->contour_end_point(i);

            SE_Tuple lastpt;
            SE_Tuple curpt;

            src->get_point(curidx, curpt.x, curpt.y);
            curpt.x += position;

            // we must look for a nonzero dx so that we can select the correct
            // transform in the case of discontinuities
            SE_Tuple dc(src->x_coord(curidx+1) + position - curpt.x,
                        src->y_coord(curidx+1)            - curpt.y);
            for (int k = curidx + 2; dc.x == 0.0 && k <= endidx; ++k)
                dc.x = src->x_coord(k) + position - curpt.x;
            Move(curpt, dc);
            LineToUV(curpt, m_last_uv);
            MapPoint(m_last_uv, m_last_scrn);
            m_cur_dst->EnsureContours(1);
            m_cur_dst->EnsurePoints(1);
            BeginContour(m_last_scrn);
            ++curidx;

            for (;curidx <= endidx; ++curidx)
            {
                lastpt = curpt;
                src->get_point(curidx, curpt.x, curpt.y);
                curpt.x += position;
                double low_edge = m_cur_low_data[0].pos;
                double high_edge = m_cur_low_data[1].pos;

                _ASSERT(high_edge > low_edge);

                // this could create a contour that is just a point, but the rendering
                // code should be able to handle that
                if (high_edge <= low_edge)
                    break;

                m_next_pts.push_head(std::pair<SE_Tuple, update_fxn>(curpt, NULL));

                while (m_next_pts.size())
                {
                    // sanity check - abort if we are going to have an infinite loop
                    if (m_next_pts.size() > (m_buffer->m_in_tx.size() + m_buffer->m_out_tx.size() + 1))
                    {
                        _ASSERT(false);
                        curidx = endidx + 1;
                        break;
                    }

                    if ((m_in_active && m_next_pts.head().first.y > 0) ||
                        (!m_in_active && m_next_pts.head().first.y < 0))
                    {
                        m_next_pts.push_head( std::pair<SE_Tuple, update_fxn>( SE_Tuple(lastpt.x +
                            (m_next_pts.head().first.x - lastpt.x) *
                            (- lastpt.y / (m_next_pts.head().first.y - lastpt.y)), 0.0),
                            &SE_JoinTransform::Transformer::Vertical) );
                        continue;
                    }
                    else if (m_next_pts.head().first.x < low_edge)
                    {
                        m_next_pts.push_head( std::pair<SE_Tuple, update_fxn>( SE_Tuple(low_edge,
                            lastpt.y + (m_next_pts.head().first.y - lastpt.y) *
                            ((low_edge - lastpt.x) / (m_next_pts.head().first.x - lastpt.x))),
                            &SE_JoinTransform::Transformer::Backward) );
                        continue;
                    }
                    else if (m_next_pts.head().first.x > high_edge)
                    {
                        m_next_pts.push_head( std::pair<SE_Tuple, update_fxn>( SE_Tuple(high_edge,
                            lastpt.y + (m_next_pts.head().first.y - lastpt.y) *
                            ((high_edge - lastpt.x) / (m_next_pts.head().first.x - lastpt.x))),
                            &SE_JoinTransform::Transformer::Forward) );
                        continue;
                    }

                    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);

                    SE_Tuple target_uv;
                    LineToUV(m_next_pts.head().first, target_uv);
                    _ASSERT(CurrentInvTolerance() > DEBUG_TOLERANCE);
                    MapSegment(target_uv, CurrentInvTolerance());

                    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);

                    if (m_next_pts.head().second)
                    {
                        (this->*m_next_pts.head().second)();

                        if (m_in_active)
                            _ASSERT(m_in_idx == (m_cur_low_data - &m_buffer->m_in_tx[0]) &&
                                    m_in_idx == (m_cur_cache - &m_buffer->m_in_cache[0]));
                        else
                            _ASSERT(m_out_idx == (m_cur_low_data - &m_buffer->m_out_tx[0]) &&
                                    m_out_idx == (m_cur_cache - &m_buffer->m_out_cache[0]));

                        low_edge = m_cur_low_data[0].pos;
                        high_edge = m_cur_low_data[1].pos;

                        _ASSERT(high_edge > low_edge);
                    }

                    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);

                    m_next_pts.pop_head();
                }
            }
        }
        cntrs += src->geom_size(j);
    }

    if (src != geom)
        LineBufferPool::FreeLineBuffer(lbp, src);

    return m_cur_dst;
}


void SE_JoinTransform::Transformer::TransformArea(double /* position */,
                                                  const SE_Tuple /* outline */ [4],
                                                  std::vector<SE_Tuple>& /* uvquads */,
                                                  std::vector<SE_Tuple>& /* txquads */)
{
    // TODO
}


//--------------------------------------------------------------
// SE_JoinTransform function definitions
//--------------------------------------------------------------


SE_JoinTransform::SE_JoinTransform(int initsize) :
m_out_pts(initsize),
m_in_pts(initsize),
m_inside(&m_in_pts),
m_outside(&m_out_pts),
#ifdef _DEBUG
m_vtx_cnt(-1),
#endif
m_cur_in_cnt(0),
m_prev_in_cnt(0),
m_cur_out_cnt(0),
m_prev_out_cnt(0),
m_cur_data(),
m_prev_pos(DBL_MAX)
{
}


SE_JoinTransform::~SE_JoinTransform()
{
}


// If this is not called before every join, parts of the transform may be inverted.
void SE_JoinTransform::StartJoin(bool clockwise, const LocalJoinInfo& data)
{
    m_prev_in_cnt = m_cur_in_cnt;
    m_cur_in_cnt = (int)(m_in_tx.size() + m_in_pts.size());
    m_prev_out_cnt = m_cur_out_cnt;
    m_cur_out_cnt = (int)(m_out_tx.size() + m_out_pts.size());

    if (clockwise)
    {
        m_inside = &m_in_pts;
        m_outside = &m_out_pts;
    }
    else
    {
        m_inside = &m_out_pts;
        m_outside = &m_in_pts;
    }

#ifdef _DEBUG
    ++m_vtx_cnt;
#endif
    m_prev_data = m_cur_data;
    m_cur_data = data;
}


void SE_JoinTransform::ProcessSegmentInfo(const LocalJoinInfo& data,
                                          int in_start,
                                          int in_stop,
                                          int out_start,
                                          int out_stop)
{
    double invtol = 1.0 / data.join_error;

    // TODO: make a distinction between inside & outside error
    for (int i = in_start; i < in_stop; ++i)
        m_in_tx[i].inv_tol = invtol;

    for (int i = out_start; i < out_stop; ++i)
        m_out_tx[i].inv_tol = invtol;
}


void SE_JoinTransform::ProcessSegmentSide(SE_Deque<std::pair<SE_Tuple, double> >& ptvec,
                                          std::vector<TxData>& txvec,
                                          const SE_Tuple& startpt,
                                          const SE_Tuple& endpt,
                                          const SE_Tuple& lctr,
                                          const SE_Tuple& rctr,
                                          const SE_Tuple& nml,
                                          const SE_Tuple& dv,
                                          int offset,
                                          double startpos,
                                          double endpos,
                                          double lpos,
                                          double rpos,
                                          double len,
                                          double dp)
{
#ifdef _DEBUG
    size_t startsize = ptvec.size();
#endif

    double ilenl, ilenr, lenl, lenr, dr;

    bool l_del = lpos > 0.0 && offset > 0;
    bool r_del = rpos > 0.0 && offset > 0;

    SE_Tuple dvl, dvr;

    if (l_del)
    {
        // Parametric position of the last point of the first join + the distance from the
        // point to the first join delimiter is the parametric distance from the start point
        // to the first delimiter.
        lenl = (ptvec[offset-1].second + (ptvec[offset-1].first - (lctr + nml)).length());
        dvl = dv * (lpos / dp);

        // The parametric distance along the outer contour from the second join delimiter
        // (or first, if there is no second) to the endpoint.
        if (rpos > 0.0)
        {
            lenr = len - lenl - dp + lpos + rpos;
            dvr = dv * (rpos / dp);
#ifdef _DEBUG
            startsize += 2;
#endif
        }
        else
        {
            lenr = len - lenl;
            rpos = dp - lpos;
            dvr = dv * (rpos / dp);
#ifdef _DEBUG
            ++startsize;
#endif
        }
    }
    else if (r_del)
    {
        lpos = dp - rpos;
        dvl = dv * (lpos / dp);
        dvr = dv * (rpos / dp);

        // The parametric distance along the outer contour from the end point to the only
        // join delmiter is the total distance along the contour minus the distance to the
        // last point of the first join minus the distance from that point to the join
        // delimiter.
        lenl = (ptvec[offset-1].second + (ptvec[offset-1].first - (rctr + nml)).length());
        lenr = len - lenl;
#ifdef _DEBUG
        ++startsize;
#endif
    }
    else
    {
        dvl = dvr = dv;
        lenr = lenl = len;
        lpos = rpos = dp;
    }

    ilenl = 1.0 / lenl;
    ilenr = 1.0 / lenr;
    dr = len - lenr;

    txvec.reserve(offset+ptvec.size()+1);

    for (int i = 0; i < offset; ++i)
    {
        const std::pair<SE_Tuple, double>& point = ptvec.head();
        _ASSERT(point.second <= lenl);
        txvec.push_back(TxData(point.first,
            startpt + (dvl * (point.second * ilenl)),
            startpos + (lpos * point.second * ilenl)));
        ptvec.pop_head();
    }

    if (l_del)
        txvec.push_back(TxData(lctr + nml, lctr, startpos + lpos));
    if (r_del)
        txvec.push_back(TxData(rctr + nml, rctr, endpos - rpos));

    while (ptvec.size())
    {
        const std::pair<SE_Tuple, double>& point = ptvec.head();
        _ASSERT(point.second >= dr && point.second - dr < lenr);
        txvec.push_back(TxData(point.first,
            endpt - (dvr * ((lenr - (point.second - dr)) * ilenr)),
            endpos - rpos + (rpos * (point.second - dr) * ilenr)));
        ptvec.pop_head();
    }

#ifdef _DEBUG
    if (txvec.size() == startsize)
        --startsize;
    SE_Tuple t, ix;
    for (size_t i = 1; i <= startsize; ++i)
    {
        // something has gone awry if the outer edge of the transform is crossing the
        // centerline
        _ASSERT(!Intersects(txvec[txvec.size()-i].ctr, txvec[txvec.size()-i-1].ctr,
                            txvec[txvec.size()-i].out, txvec[txvec.size()-i-1].out,
                            t, ix) &&
                txvec[txvec.size()-i].pos >= txvec[txvec.size()-i-1].pos);
    }
#endif
}


void SE_JoinTransform::ProcessSegment(double in_len,
                                      double out_len,
                                      double end_pos,
                                      const SE_Tuple end_vert)
{
    SE_Tuple dv = end_vert - m_prev_vtx;
    double dp = end_pos - m_prev_pos;

    double lpos = m_prev_data.join_width * m_global_info.join_dilation;
    double rpos = m_cur_data.join_width * m_global_info.join_dilation;

    int in_off = -1, out_off = -1;
    SE_Tuple innml, lctr, rctr;

    if (lpos + rpos < dp && (lpos > 0.0 || rpos > 0.0))
    {
        in_off = m_cur_in_cnt - (int)m_in_tx.size();
        out_off = m_cur_out_cnt - (int)m_out_tx.size();
        double idp = 1.0 / dp;

        innml = SE_Tuple(-dv.y * idp, dv.x * idp) * m_global_info.join_height;
        if (lpos > 0.0)
        {
            lctr = m_prev_vtx + (dv * (lpos * idp));
            ++m_cur_in_cnt;
            ++m_cur_out_cnt;
        }
        if (rpos > 0.0)
            rctr = end_vert - (dv * (rpos * idp));
    }

    _ASSERT(in_off != 0 && in_off <= (int)m_in_pts.size());
    if (in_off == 0 || in_off > (int)m_in_pts.size())
        return;

    ProcessSegmentSide(m_in_pts, m_in_tx, m_prev_vtx, end_vert, lctr, rctr, innml, dv,
        in_off, m_prev_pos, end_pos, lpos, rpos, in_len + m_in_pts.tail().second, dp);

    _ASSERT(out_off != 0 && out_off <= (int)m_out_pts.size());
    if (out_off == 0 || out_off > (int)m_out_pts.size())
        return;

    ProcessSegmentSide(m_out_pts, m_out_tx, m_prev_vtx, end_vert, lctr, rctr, -innml, dv,
        out_off, m_prev_pos, end_pos, lpos, rpos, out_len + m_out_pts.tail().second, dp);
}


void SE_JoinTransform::AddVertex(const SE_Tuple& outer,
                                 const SE_Tuple& vertex,
                                 const SE_Tuple& inner,
                                 double pos)
{
    // the first vertex must be preceeded by zero points
    _ASSERT(m_vtx_cnt || (m_in_pts.size() == 0 && m_out_pts.size() == 0));

    if (m_in_pts.size())
    {
        double id = (inner - m_inside->tail().first).length();
        double od = (outer - m_outside->tail().first).length();
        if (m_inside != &m_in_pts)
            std::swap(id, od);
        ProcessSegment(id, od, pos, vertex);
    }

    ProcessSegmentInfo(m_prev_data, m_prev_in_cnt, m_cur_in_cnt, m_prev_out_cnt, m_cur_out_cnt);

    m_inside->push_tail(std::pair<SE_Tuple, double>(inner, 0.0));
    m_outside->push_tail(std::pair<SE_Tuple, double>(outer, 0.0));

    _ASSERT(pos > m_prev_pos || m_prev_pos >= DBL_MAX);

    m_prev_vtx = vertex;
    m_prev_pos = pos;
#ifdef _DEBUG
    ++m_vtx_cnt;
#endif
}


void SE_JoinTransform::Close()
{
    _ASSERT(m_in_pts.size() == 1 && m_out_pts.size() == 1);

    m_in_tx.push_back(TxData(m_in_pts.head().first, m_prev_vtx, m_prev_pos));
    m_in_pts.pop_head();

    m_out_tx.push_back(TxData(m_out_pts.head().first, m_prev_vtx, m_prev_pos));
    m_out_pts.pop_head();

    ProcessSegmentInfo(m_cur_data, m_cur_in_cnt, (int)m_in_tx.size(), m_cur_out_cnt, (int)m_out_tx.size());

    PopulateTransformerData();
}


void SE_JoinTransform::PopulateTransformerData()
{
    m_in_cache.clear();
    m_in_cache.post_enlarge((int)m_in_tx.size() - 1);
    int size = (int)m_in_cache.size();
    for (int i = 0; i < size; ++i)
        m_in_cache[i].inv_width = 0.0;

    m_out_cache.clear();
    m_out_cache.post_enlarge((int)m_out_tx.size() - 1);
    size = (int)m_out_cache.size();
    for (int i = 0; i < size; ++i)
        m_out_cache[i].inv_width = 0.0;

    // every zero-width transform segment is considered a continuity break
    std::vector<TxData>::const_iterator iiter = m_in_tx.begin(), oiter = m_out_tx.begin();
    double ipos = (*iiter++).pos, opos = (*oiter++).pos;
    bool iend = iiter == m_in_tx.end(), oend = oiter == m_out_tx.end();

    while (!(iend && oend))
    {
        if (oend || (!iend && (*iiter).pos < (*oiter).pos))
        {
            if (ipos == (*iiter).pos && (m_breaks.empty() || m_breaks.back() != ipos))
                m_breaks.push_back(ipos);
            ipos = (*iiter++).pos;
            iend = iiter == m_in_tx.end();
        }
        else
        {
            if (opos == (*oiter).pos && (m_breaks.empty() || m_breaks.back() != opos))
                m_breaks.push_back(opos);
            opos = (*oiter++).pos;
            oend = oiter == m_out_tx.end();
        }
    }
}


void SE_JoinTransform::AddOutsidePoint(const SE_Tuple& outer)
{
    // adding points in the case where m_outside->tail() is undefined will cause
    // an assertion to fail in the AddVertex call
    double d = (outer - m_outside->tail().first).length();
    m_outside->push_tail(std::pair<SE_Tuple, double>(outer, m_outside->tail().second + d));
}



void SE_JoinTransform::AddInsidePoint(const SE_Tuple& inner)
{
    double d = (inner - m_inside->tail().first).length();
    m_inside->push_tail(std::pair<SE_Tuple, double>(inner, m_inside->tail().second + d));
}


void SE_JoinTransform::Reset()
{
    m_out_pts.clear();
    m_in_pts.clear();
    m_in_tx.clear();
    m_in_cache.clear();
    m_out_tx.clear();
    m_out_cache.clear();
    m_breaks.clear();

    m_inside = &m_in_pts;
    m_outside = &m_out_pts;
    m_cur_in_cnt = 0;
    m_cur_out_cnt = 0;
    m_prev_in_cnt = 0;
    m_prev_out_cnt = 0;
    m_prev_pos = DBL_MAX;
#ifdef _DEBUG
    m_vtx_cnt = -1;
#endif
}


SE_JoinTransform::Transformer* SE_JoinTransform::GetTransformer
(double clip_min, double clip_max, double height)
{
    return new Transformer(*this, height, clip_min, clip_max);
}
