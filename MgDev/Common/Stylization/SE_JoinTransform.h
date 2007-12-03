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

#ifndef SE_JOINTRANSFORM_H
#define SE_JOINTRANSFORM_H

#include "SE_BufferPool.h"
#include "SE_LinearClip.h"
#include "SE_GeometryOperations.h"
#include "SE_Deque.h"
#include "SE_Tuple.h"
#include <utility>
#include <vector>

#define DEBUG_TOLERANCE 0.001

struct SE_SegmentInfo
{
    SE_Tuple* vertex;
    SE_Tuple next;
    double vertpos;
    double nextlen;
};

/* The class JOIN_DATA is required to define a type TX_INFO, which
 * will be associated with every TxData struct, and a type GLOBAL_INFO,
 * which will be a member of the join transform that can be used for
 * storing persistent data (over all transforms using the object).
 * 
 * Additionally, the class JOIN_DATA must define overloads of operator()
 * that take SE_Join<JOIN_DATA>& and SE_Cap<JOIN_DATA>& arguments:
 *      const JOIN_DATA& operator() (const SE_Join<JOIN_DATA>&) 
 *      const JOIN_DATA& operator() (const SE_Cap<JOIN_DATA>&)
 */
template<class JOIN_DATA> class SE_JoinTransform
{
public:
    struct TxCache;

private:
    struct TxData
    {
        double   pos;
        SE_Tuple out;
        SE_Tuple ctr;

        /* The info for the transform region for which the TxData is the leading edge */
        typename JOIN_DATA::TX_INFO info;

        TxData(const SE_Tuple& pout, const SE_Tuple& pctr, double lpos) :
        pos(lpos),
            out(pout),
            ctr(pctr)
        {
        }
    };

    void ProcessSegment(double in_len,
                        double out_len,
                        double end_pos,
                        const SE_Tuple end_vert);

    void ProcessSegmentInfo(const JOIN_DATA& data,
                            int in_start,
                            int in_stop,
                            int out_start,
                            int out_stop);

    void PopulateTransformerData();

    SE_INLINE void LineToNoRepeat(LineBuffer* buf, const SE_Tuple& pt);

#ifdef _DEBUG
    int        m_vtx_cnt;
#endif
    int        m_cur_in_cnt;
    int        m_prev_in_cnt;
    int        m_cur_out_cnt;
    int        m_prev_out_cnt;
    JOIN_DATA  m_cur_data;
    JOIN_DATA  m_prev_data;
    SE_Tuple   m_prev_vtx;
    double     m_prev_pos;
    
    typename JOIN_DATA::GLOBAL_INFO        m_global_info;

    SE_Deque<std::pair<SE_Tuple, double> > m_out_pts;
    SE_Deque<std::pair<SE_Tuple, double> > m_in_pts;
    std::vector<TxData>                    m_in_tx;
    SE_Deque<TxCache>                      m_in_cache;
    std::vector<TxData>                    m_out_tx;
    SE_Deque<TxCache>                      m_out_cache;
    std::vector<double>                    m_breaks;

    SE_Deque<std::pair<SE_Tuple, double> >* m_inside;
    SE_Deque<std::pair<SE_Tuple, double> >* m_outside;

public:

    /*    (0,vmax)    (1,vmax)
    *       ________             A ___________ B
    *      |(A)  (B)|              \          |
    *  /|  |        |               \     ____|
    *   |  |        |                \___/   D
    *   |  |________|                  C
    *  v| (C)      (D)
    *   |(0,0)    (1,0)
    *   |_________
    *     u      /
    *
    * Each pair of segments in the buffer are the mapping of a source rectangle in symbol space
    * to a destination quadrilateral in screen space, where (A,C) is the (out,center)
    * line of the left segment, and (B,D) is the (out, center) line of the right segment.
    * The symbol space is the rectangle from (left line_pos, 0) to
    * (right line_pos, vmax), normalized to (0,0) to (1,1) in u-v space.
    *
    * The transform for a point from u-v to screen space is as follows:
    *
    *      T(u,v) = (1-v)*(uD + (1-u)C) + v((1-u)A + uB)
    *
    * with:
    *
    *      (dT/du)(u,v)     = (1-v)(D-C) + v(B-A)
    *                       = (D-C) + v(B + C - A - D)
    *
    *      (dT/dv)(u,v)     = (1-u)(A-C) + u(B-D)
    *                       = (A-C) + u(B + C - A - D)
    *
    *      (d2T/du/dv)(u,v) = B + C - A - D
    */
    struct TxCache
    {
        SE_Tuple d_m_c;       /* D - C */
        SE_Tuple a_m_c;       /* A - C */
        SE_Tuple bc_m_ad;     /* B + C - A - D */
        double bcad_len;      /* ||B + C - A - D|| */
        double inv_width;     /* 1/width (Symbol space) */
#ifdef _DEBUG
        const TxData* low_data;
#endif // _DEBUG
    };

    class Transformer
    {
        friend class SE_JoinTransform<JOIN_DATA>;
        typedef void (Transformer::*update_fxn)(void);

        int                  m_in_idx;
        int                  m_out_idx;
        int                  m_break_idx;
        bool                 m_in_active;

        double               m_inv_height;
        double               m_tolerance;
        double               m_clip_ext[2];

        SE_Tuple             m_last_uv;
        SE_Tuple             m_last_scrn;

        LineBuffer*          m_cur_dst;
        TxCache*             m_cur_cache;
        TxData*              m_cur_low_data;

        SE_LinearClip<VerticalClip> m_clip;
        std::vector<LineBuffer*> m_tx_stack;

        SE_JoinTransform* m_buffer;

        SE_Deque<std::pair<SE_Tuple, update_fxn> > m_next_pts;

        Transformer(SE_JoinTransform& buffer, double height, double clip_min, double clip_max);

        LineBuffer* ApplyBreaks(LineBuffer* src, double position, LineBufferPool* pool);

        double& CurrentTolerance();
        void AddPoint(const SE_Tuple& point);
        void BeginContour(const SE_Tuple& point);
        void EndSegment();
        void EndLine();
        void EndContour();

        void LineToUV(const SE_Tuple& point, SE_Tuple& uv);
        void MapPoint(const SE_Tuple& uv, SE_Tuple& world);
        double MapSegment(const SE_Tuple& target_uv, const double& tolerance);

        void Find(double x, double dx);
        void EvaluateCache();

        void Move(const SE_Tuple& xy, const SE_Tuple& direction);
        void Forward();
        void Backward();
        void Vertical();

    public:
        Transformer();
        LineBuffer* TransformLine(LineBuffer* src, double position, LineBufferPool* lbp);
        void TransformArea(double position, const SE_Tuple outline[4],
            std::vector<SE_Tuple>& uvquads,
            std::vector<SE_Tuple>& txquads);
    };

    SE_INLINE SE_JoinTransform(int initsize = 10);
    SE_INLINE ~SE_JoinTransform();

    SE_INLINE void StartJoin(bool clockwise, const JOIN_DATA& data);
    SE_INLINE void AddVertex(const SE_Tuple& outer,
        const SE_Tuple& vertex,
        const SE_Tuple& inner,
        double line_position);
    SE_INLINE void AddOutsidePoint(const SE_Tuple& outer);
    SE_INLINE void AddInsidePoint(const SE_Tuple& inner);

    SE_INLINE const double& LastPosition();

    SE_INLINE void SetGlobalInfo(const typename JOIN_DATA::GLOBAL_INFO& info);

    SE_INLINE void Close();
    SE_INLINE void Reset();
    SE_INLINE Transformer* GetTransformer(double clip_min, double clip_max, double height);
    void GetTransformOutline(LineBuffer* outline);
};


/******************************************************************************
* SE_JoinTransform::Transformer function definitions                         *
******************************************************************************/

template<class JOIN_DATA>
SE_JoinTransform<JOIN_DATA>::Transformer::Transformer
    (SE_JoinTransform& buffer, double height, double clip_min, double clip_max) :
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


/* Uninitialized use of the iterator will be detectable by a caller when the caller
* finds itself dereferencing double-digit pointers */
template<class JOIN_DATA>
    SE_JoinTransform<JOIN_DATA>::Transformer::Transformer() :
m_buffer(NULL),
m_cur_cache(NULL),
m_in_idx(0),
m_out_idx(0),
m_in_active(false),
m_next_pts(0)
{
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::Move(const SE_Tuple& xy, const SE_Tuple& direction)
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


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::Vertical()
{
    m_in_active = !m_in_active;

    _ASSERT(m_last_uv.y < DEBUG_TOLERANCE && m_last_uv.y > -DEBUG_TOLERANCE);

    double x = m_cur_low_data[0].pos + m_last_uv.x * (m_cur_low_data[1].pos - m_cur_low_data[0].pos);
    /* For a vertical movement, choose dx to keep us on the same side of a discontinuity */
    Find(x, m_last_uv.x < 0.5 ? 1.0 : -1.0);
    EvaluateCache();

    m_last_uv.x = (x - m_cur_low_data->pos) * (m_cur_cache->inv_width);
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::Backward()
{
    _ASSERT(m_last_uv.x < DEBUG_TOLERANCE && m_last_uv.x > -DEBUG_TOLERANCE);

    /* The contours should have been broken over discontinuities */
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


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::Forward()
{
    _ASSERT(m_last_uv.x > (1.0 - DEBUG_TOLERANCE) && m_last_uv.x < (1.0 + DEBUG_TOLERANCE));

    /* The contours should have been broken over discontinuities */
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


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::Find(double x, double dx)
{
    /* For now, naive linear search */
    std::vector<TxData>* tx;
    const SE_Deque<TxCache>* cache;
    int*  index;

    _ASSERT(!m_cur_cache || m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);


    if (m_in_active)
    {
        tx = &m_buffer->m_in_tx;
        cache = &m_buffer->m_in_cache;
        index = &m_in_idx;
    }
    else
    {
        tx = &m_buffer->m_out_tx;
        cache = &m_buffer->m_out_cache;
        index = &m_out_idx;
    }

    _ASSERT(*index >= 0 && *index < (int)tx->size() - 1);

    /* Sanity check for current index.  Is our state invalid? */
    if (*index < 0)
        *index = 0;
    if (*index >= (int)tx->size() - 1)
        *index = (int)tx->size() - 2;

    /* Sanity check, return closest values for out-of-bounds points */
    _ASSERT(x >= m_clip_ext[0] - DEBUG_TOLERANCE && x <= m_clip_ext[1] + DEBUG_TOLERANCE);
    if (x < (*tx)[0].pos)
    {
        m_cur_cache = &(*cache)[0];
        m_cur_low_data = &(*tx)[0];
        *index = 0;
        return;
    } else if (x > tx->back().pos)
    {
        m_cur_low_data = &tx->back() - 1;
        m_cur_cache = &cache->tail();
        *index = (int)tx->size() - 2;
        return;
    }

    if (x >= (*tx)[*index].pos)
//        for (;x > (*tx)[(*index)+1].pos;++(*index));
        /* TODO: remove. additional check to abort on aberrant transform data */
        for (;(*tx)[(*index)+1].pos >= (*tx)[(*index)].pos && x > (*tx)[(*index)+1].pos;++(*index));
    else
        while(x < (*tx)[--(*index)].pos);

    _ASSERT(m_in_idx < (int)m_buffer->m_in_tx.size() - 1 && m_in_idx >= 0 &&
        m_out_idx < (int)m_buffer->m_out_tx.size() - 1 && m_out_idx >= 0);

    /* Position correctly with respect to discontinuities */
    if ((*tx)[*index].pos == (*tx)[*index + 1].pos)
        *index += dx > 0.0 ? 1 : -1;
    else if ((*tx)[*index].pos == x && dx < 0.0)
        *index -= 2;
    else if ((*tx)[*index+1].pos == x && dx > 0.0)
        *index += 2;

    m_cur_cache = &(*cache)[*index];
    m_cur_low_data = &(*tx)[*index];
    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::EvaluateCache()
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
#endif // _DEBUG
    }
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::AddPoint(const SE_Tuple& point)
{
    if (m_cur_dst->cntr_size(m_cur_dst->cntr_count() - 1) > 1)
    {
        SE_Tuple *ult, *penult;
        SE_Tuple dv;

        ult = (SE_Tuple*)&m_cur_dst->x_coord(m_cur_dst->point_count() - 1);
        penult = ult - 1;
        dv = *ult - *penult;

        /* TODO: use current tolerance */
        /* if last seg is < .25 long, combine segs */
        if (dv.lengthSquared() < .25)
        {
            *ult = point;
            return;
        }
    }
    m_cur_dst->UnsafeLineTo(point.x, point.y);
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::BeginContour(const SE_Tuple& point)
{
    m_cur_dst->UnsafeMoveTo(point.x, point.y);
}


template<class JOIN_DATA>
double& SE_JoinTransform<JOIN_DATA>::Transformer::CurrentTolerance()
{
    m_tolerance = 0.25;
    return m_tolerance;
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::EndSegment()
{
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::EndContour()
{
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::EndLine()
{
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::LineToUV(const SE_Tuple& point, SE_Tuple& uv)
{
    uv.x = (point.x - m_cur_low_data[0].pos) * m_cur_cache->inv_width;
    uv.y = fabs(point.y) * m_inv_height;
    _ASSERT(uv.x >= 0.0 - DEBUG_TOLERANCE && uv.x <= 1.0 + DEBUG_TOLERANCE &&
            uv.y >= 0.0 - DEBUG_TOLERANCE && uv.y <= 1.0 + DEBUG_TOLERANCE);
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::MapPoint(const SE_Tuple& uv, SE_Tuple& world)
{
    world = (m_cur_low_data[1].ctr * ((1.0 - uv.y) * uv.x)) +
        (m_cur_low_data[0].ctr * ((1.0 - uv.y) * (1.0 - uv.x))) +
        (m_cur_low_data[0].out * ((1.0 - uv.x) * uv.y)) +
        (m_cur_low_data[1].out * (uv.x * uv.y));
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
* (i*j*0.25)*(A+C-B-D) from T.  Consequently, if the destination quadrilateral is a rectangle
* (A+C==B+D), or if du == 0 (vertical line), or dv == 0 (horizontal line), the approximation
* error will be zero.
*
* Regardless of the error incurred in approximating the line, the endpoint will be exact, so the
* error in mapping a segment over a transform does not affect the error over subsequent transforms 
*/
template<class JOIN_DATA>
double SE_JoinTransform<JOIN_DATA>::Transformer::MapSegment(const SE_Tuple& target_uv, const double& tolerance)
{
    SE_Tuple dp = target_uv - m_last_uv;

    SE_Tuple dPt, ddPt, d2Tdudv;
    double err = fabs(dp.x * dp.y) * 0.25 * m_cur_cache->bcad_len;
    int segs = (int)(sqrt(err / tolerance)) + 1;
    double invsegs = 1.0 / (double)segs;
    dp *= invsegs;

    /* Is the number of segments absurdly high? */
    _ASSERT(segs < 100000);

    dPt = m_cur_cache->d_m_c * dp.x + m_cur_cache->a_m_c * dp.y +
        m_cur_cache->bc_m_ad * (m_last_uv.y * dp.x + m_last_uv.x * dp.y);
    d2Tdudv = m_cur_cache->bc_m_ad * (dp.x * dp.y);
    dPt += d2Tdudv;
    ddPt = d2Tdudv * 2.0;

    SE_Tuple cur_pt_scrn = m_last_scrn;

    m_cur_dst->EnsurePoints(segs);
    for (int i = 0;;)
    {
        cur_pt_scrn += dPt;
        AddPoint(cur_pt_scrn);
        if (++i >= segs)
            break;
        dPt += ddPt;
    }

    m_last_scrn = cur_pt_scrn;
    m_last_uv = target_uv;

    /* Return the actual error */
    return err * invsegs * invsegs;
}


template<class JOIN_DATA>
LineBuffer* SE_JoinTransform<JOIN_DATA>::Transformer::ApplyBreaks
(LineBuffer* src, double position, LineBufferPool* pool)
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


template<class JOIN_DATA>
LineBuffer* SE_JoinTransform<JOIN_DATA>::Transformer::TransformLine
(LineBuffer* geom, double position, LineBufferPool* lbp)
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
            /* We must look for a nonzero dx so that we can select the correct
            * transform in the case of discontinuities */
            SE_Tuple dc(src->x_coord(curidx+1) + position - curpt.x,
                src->y_coord(curidx+1) - curpt.y);
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

                m_next_pts.push_head(std::pair<SE_Tuple, update_fxn>(curpt, NULL));

                while (m_next_pts.size())
                {
                    /* Sanity check, abort if we are going to have an infinite loop */
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
                            &SE_JoinTransform<JOIN_DATA>::Transformer::Vertical) );
                        continue;
                    }
                    else if (m_next_pts.head().first.x < low_edge)
                    {
                        m_next_pts.push_head( std::pair<SE_Tuple, update_fxn>( SE_Tuple(low_edge,
                            lastpt.y + (m_next_pts.head().first.y - lastpt.y) *
                            ((low_edge - lastpt.x) / (m_next_pts.head().first.x - lastpt.x))),
                            &SE_JoinTransform<JOIN_DATA>::Transformer::Backward) );
                        continue;
                    }
                    else if (m_next_pts.head().first.x > high_edge)
                    {
                        m_next_pts.push_head( std::pair<SE_Tuple, update_fxn>( SE_Tuple(high_edge,
                            lastpt.y + (m_next_pts.head().first.y - lastpt.y) *
                            ((high_edge - lastpt.x) / (m_next_pts.head().first.x - lastpt.x))),
                            &SE_JoinTransform<JOIN_DATA>::Transformer::Forward) );
                        continue;
                    }

                    _ASSERT(m_cur_cache->inv_width == 0.0 || m_cur_cache->low_data == m_cur_low_data);

                    SE_Tuple target_uv;
                    LineToUV(m_next_pts.head().first, target_uv);
                    MapSegment(target_uv, CurrentTolerance());

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
                EndSegment();
            }
            EndContour();
        }
        EndLine();
        cntrs += src->geom_size(j);
    }

    if (src != geom)
        LineBufferPool::FreeLineBuffer(lbp, src);

    return m_cur_dst;
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Transformer::TransformArea(
    double /* position */, const SE_Tuple /* outline */ [4],
    std::vector<SE_Tuple>& /* uvquads */,
    std::vector<SE_Tuple>& /* txquads */)
{
    /* TODO */
}


/******************************************************************************
* SE_JoinTransform function definitions                                      *
******************************************************************************/

template<class JOIN_DATA>
SE_JoinTransform<JOIN_DATA>::SE_JoinTransform(int initsize) :
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


template<class JOIN_DATA>
SE_JoinTransform<JOIN_DATA>::~SE_JoinTransform()
{
}


/* If this is not called before every join, parts of the transform may be inverted */
template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::StartJoin(bool clockwise, const JOIN_DATA& data)
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


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::ProcessSegmentInfo(const JOIN_DATA& /*data*/,
                                                     int /*in_start*/,
                                                     int /*in_stop*/,
                                                     int /*out_start*/,
                                                     int /*out_stop*/)
{
}

template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::ProcessSegment(double in_len,
                                                 double out_len,
                                                 double end_pos,
                                                 const SE_Tuple end_vert)
{
    SE_Tuple dv = end_vert - m_prev_vtx;
    double dp = end_pos - m_prev_pos;
    double ilen = 1.0 / (in_len + m_in_pts.tail().second);

    while (m_in_pts.size() > 0)
    {
        const std::pair<SE_Tuple, double>& point = m_in_pts.head();

        _ASSERT(dp * point.second * ilen >= 0.0);
        m_in_tx.push_back(TxData(point.first,
            m_prev_vtx + (dv * (point.second * ilen)),
            m_prev_pos + (dp * point.second * ilen)));

        m_in_pts.pop_head();
    }

    ilen = 1.0 / (out_len + m_out_pts.tail().second);

    while (m_out_pts.size() > 0)
    {
        const std::pair<SE_Tuple, double>& point = m_out_pts.head();

        _ASSERT(dp * point.second * ilen >= 0.0);

        m_out_tx.push_back(TxData(point.first,
            m_prev_vtx + (dv * (point.second * ilen)),
            m_prev_pos + (dp * point.second * ilen)));

        m_out_pts.pop_head();
    }
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::AddVertex(const SE_Tuple& outer,
                                            const SE_Tuple& vertex,
                                            const SE_Tuple& inner,
                                            double pos)
{
    /* The first vertex must be preceeded by zero points */
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


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Close()
{
    _ASSERT(m_in_pts.size() == 1 && m_out_pts.size() == 1);

    m_in_tx.push_back(TxData(m_in_pts.head().first, m_prev_vtx, m_prev_pos));
    m_in_pts.pop_head();

    m_out_tx.push_back(TxData(m_out_pts.head().first, m_prev_vtx, m_prev_pos));
    m_out_pts.pop_head();

    ProcessSegmentInfo(m_cur_data, m_cur_in_cnt, (int)m_in_tx.size(), m_cur_out_cnt, (int)m_out_tx.size());

    PopulateTransformerData();
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::PopulateTransformerData()
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

    /* Every zero-width transform segment is considered a continuity break. */
    typename std::vector<TxData>::const_iterator iiter = m_in_tx.begin(), oiter = m_out_tx.begin();
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


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::AddOutsidePoint(const SE_Tuple& outer)
{
    /* Adding points in the case where m_outside->tail() is undefined will cause an
    * assertion to fail in the AddVertex call */
    double d = (outer - m_outside->tail().first).length();
    m_outside->push_tail(std::pair<SE_Tuple, double>(outer, m_outside->tail().second + d));
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::AddInsidePoint(const SE_Tuple& inner)
{
    double d = (inner - m_inside->tail().first).length();
    m_inside->push_tail(std::pair<SE_Tuple, double>(inner, m_inside->tail().second + d));
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::Reset()
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
    m_vtx_cnt(-1);
#endif
}


template<class JOIN_DATA>
const double& SE_JoinTransform<JOIN_DATA>::LastPosition()
{
    return m_prev_pos;
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::SetGlobalInfo(const typename JOIN_DATA::GLOBAL_INFO& info)
{
    m_global_info = info;
}


template<class JOIN_DATA>
typename SE_JoinTransform<JOIN_DATA>::Transformer* SE_JoinTransform<JOIN_DATA>::GetTransformer
(double clip_min, double clip_max, double height)
{
    return new Transformer(*this, height, clip_min, clip_max);
}

template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::LineToNoRepeat(LineBuffer* buf, const SE_Tuple& pt)
{
    double lx, ly;
    buf->get_point(buf->point_count() - 1, lx, ly);
    if (lx != pt.x || ly != pt.y)
        buf->UnsafeLineTo(pt.x, pt.y);
}


template<class JOIN_DATA>
void SE_JoinTransform<JOIN_DATA>::GetTransformOutline(LineBuffer* outline)
{
    // avoid exception which occurs when m_in_tx's / m_out_tx's size equals to 0
    // because back() returns *(end() -1) (and there is no geometry, obviously)
    if (0 == m_in_tx.size() || 0 == m_out_tx.size())
        return;

    outline->SetGeometryType(outline->geom_count() ? FdoGeometryType_MultiPolygon : FdoGeometryType_Polygon);
    outline->NewGeometry();

    if (m_out_tx.front().ctr == m_out_tx.back().ctr ||
        m_in_tx.front().ctr == m_in_tx.back().ctr)
    {
        /* Really, if one is closed, they should both be closed */
        _ASSERT(m_out_tx.front().ctr == m_out_tx.back().ctr &&
            m_in_tx.front().ctr == m_in_tx.back().ctr);

        outline->EnsureContours(2);
        outline->EnsurePoints((int)(m_in_tx.size() + m_out_tx.size()));

        outline->UnsafeMoveTo(m_out_tx[0].out.x, m_out_tx[0].out.y);
        for (size_t i = 1; i < m_out_tx.size(); ++i)
            LineToNoRepeat(outline, m_out_tx[i].out);

        outline->UnsafeMoveTo(m_in_tx[0].out.x, m_in_tx[0].out.y);
        for (size_t i = 1; i < m_in_tx.size(); ++i)
            LineToNoRepeat(outline, m_in_tx[i].out);
    }
    else
    {
        outline->EnsureContours(1);
        outline->EnsurePoints((int)(m_in_tx.size() + m_out_tx.size()) + 3);

        /* While building the polygon, we check for adjecent coincident points that can
        * throw off rendering. */
        outline->UnsafeMoveTo(m_out_tx[0].ctr.x, m_out_tx[0].ctr.y);
        for (size_t i = 0; i < m_out_tx.size(); ++i)
            LineToNoRepeat(outline, m_out_tx[i].out);

        LineToNoRepeat(outline, m_out_tx[m_out_tx.size()-1].ctr);

        for (size_t i = m_in_tx.size(); i-- > 0;)
            LineToNoRepeat(outline, m_in_tx[i].out);
        outline->UnsafeClose();
    }
}

#endif // SE_JOINTRANSFORM_H
