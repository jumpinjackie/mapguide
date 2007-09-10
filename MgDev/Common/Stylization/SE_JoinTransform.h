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

template<class USER_DATA> class SE_JoinTransform
{
public:
    struct TxCache;

private:
    struct TxData
    {
        double   pos;
        SE_Tuple out;
        SE_Tuple ctr;
        int      data;

        TxData(const SE_Tuple& pout, const SE_Tuple& pctr, double lpos) :
            pos(lpos),
            out(pout),
            ctr(pctr),
            data(-1)
        {
        }
    };
  
    void ProcessSegment(double in_len, 
                        double out_len, 
                        double end_pos, 
                        const SE_Tuple end_vert);

    int      m_vtx_cnt;
    int      m_cur_in_cnt;
    int      m_prev_in_cnt;
    int      m_cur_out_cnt;
    int      m_prev_out_cnt;
    int      m_cur_data;
    int      m_prev_data;
    SE_Tuple m_prev_vtx;
    double   m_prev_pos;
    double   m_in_len;
    double   m_out_len;
    double   m_height;

    SE_BufferPool*                         m_pool;
    SE_Deque<std::pair<SE_Tuple, double>>  m_out_pts;
    SE_Deque<std::pair<SE_Tuple, double>>  m_in_pts;
    std::vector<USER_DATA>                 m_user_data;
    std::vector<TxData>                    m_in_tx;
    SE_Deque<TxCache>                      m_in_cache;
    std::vector<TxData>                    m_out_tx;
    SE_Deque<TxCache>                      m_out_cache;

    SE_Deque<std::pair<SE_Tuple, double>>* m_inside;
    SE_Deque<std::pair<SE_Tuple, double>>* m_outside;

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
    };

    class Transformer
    {
        friend class SE_JoinTransform<USER_DATA>;
        typedef void (Transformer::*update_fxn)(void);

        int                  m_in_idx;
        int                  m_out_idx;
        bool                 m_in_active;

        double               m_inv_height;
        double               m_position;
        double               m_tolerance;
        
        SE_Tuple             m_last_uv;
        SE_Tuple             m_last_scrn;

        SE_LineStorage*      m_cur_dst;
        TxCache*             m_cur_cache;
        const TxData*        m_cur_low_data;

        const SE_JoinTransform* m_buffer;

        SE_Deque<std::pair<SE_Tuple, update_fxn>> m_next_pts;

        Transformer(const SE_JoinTransform& buffer, double height);
        
        double& CurrentTolerance();
        void AddPoint(const SE_Tuple& point);
        void BeginContour(const SE_Tuple& point);
        void EndSegment();
        void EndLine();
        void EndContour();

        void LineToUV(const SE_Tuple& point, SE_Tuple& uv);
        void MapPoint(const SE_Tuple& uv, SE_Tuple& world);
        void MapSegment(const SE_Tuple& target_uv, 
                        double& tolerance);
        bool Intersects(const SE_Tuple& A0, const SE_Tuple& A1,
                        const SE_Tuple& B0, const SE_Tuple& B1,
                        /*out*/ SE_Tuple& T, /*out*/ SE_Tuple& isection);
                        
        void Find(double x);
        void EvaluateCache();
        
        void Move(const SE_Tuple& xy);
        void Forward();
        void Backward();
        void Vertical();

    public:
        Transformer();
        SE_LineStorage* TransformLine(LineBuffer* src, double position);
        void TransformArea(double position, const SE_Tuple outline[4], 
                           std::vector<SE_Tuple>& uvquads, 
                           std::vector<SE_Tuple>& txquads);
    };

    SE_INLINE SE_JoinTransform(SE_BufferPool* pool, double height, int initsize = 10);
    SE_INLINE ~SE_JoinTransform();

    SE_INLINE void StartJoin(bool clockwise);
    SE_INLINE void AddVertex( const SE_Tuple& outer, 
                              const SE_Tuple& vertex, 
                              const SE_Tuple& inner, 
                              double line_position );
    SE_INLINE void AddUserData(const USER_DATA& user_data);
    SE_INLINE void AddOutsidePoint(SE_Tuple& outer);
    SE_INLINE void AddInsidePoint(SE_Tuple& inner);
    
    SE_INLINE void Close();
    SE_INLINE void Reset();
    SE_INLINE Transformer GetTransformer();
};

/******************************************************************************
 * SE_JoinTransform::Transformer function definitions                             *
 ******************************************************************************/

 template<class USER_DATA> 
    SE_JoinTransform<USER_DATA>::Transformer::Transformer(const SE_JoinTransform& buffer, double height) :
    m_buffer(&buffer),
    m_inv_height(1.0 / height),
    m_in_idx(0),
    m_out_idx(0),
    m_in_active(false),
    m_next_pts(10)
 {
 }

/* Uninitialized use of the iterator will be detectable by a caller when the caller
 * finds itself dereferencing double-digit pointers */
 template<class USER_DATA> 
    SE_JoinTransform<USER_DATA>::Transformer::Transformer() :
    m_buffer(NULL),
    m_in_idx(0),
    m_out_idx(0),
    m_in_active(false),
    m_next_pts(0)
 {
 }


 template<class USER_DATA> void
    SE_JoinTransform<USER_DATA>::Transformer::Move(const SE_Tuple& xy)
 {
    m_in_active = xy.y < 0.0;
    Find(xy.x);
    EvaluateCache();
    LineToUV(xy, m_last_uv);
 }


  template<class USER_DATA> void SE_JoinTransform<USER_DATA>::Transformer::Vertical()
 {
    m_in_active = !m_in_active;

    _ASSERT(m_last_uv.y < DEBUG_TOLERANCE && m_last_uv.y > -DEBUG_TOLERANCE);

    double x = m_cur_low_data[0].pos + m_last_uv.x * (m_cur_low_data[1].pos - m_cur_low_data[0].pos);
    Find(x);
    EvaluateCache();

    m_last_uv.x = (x - m_cur_low_data->pos) / (m_cur_cache->inv_width);
 }
  

 template<class USER_DATA> void SE_JoinTransform<USER_DATA>::Transformer::Backward()
 {
    _ASSERT(m_last_uv.x < DEBUG_TOLERANCE && m_last_uv.x > -DEBUG_TOLERANCE);

    if (m_in_active)
        m_cur_cache = &m_buffer->m_in_cache[--m_in_idx];
    else
        m_cur_cache = &m_buffer->m_out_cache[--m_out_idx];

    m_last_uv.x = 1.0;

    --m_cur_low_data;
    EvaluateCache();
 }


 template<class USER_DATA> void SE_JoinTransform<USER_DATA>::Transformer::Forward()
 {
    _ASSERT(m_last_uv.x > (1.0 - DEBUG_TOLERANCE) && m_last_uv.x < (1.0 + DEBUG_TOLERANCE)); 
     
    if (m_in_active)
        m_cur_cache = &m_buffer->m_in_cache[++m_in_idx];
    else
        m_cur_cache = &m_buffer->m_out_cache[++m_out_idx];

    m_last_uv.x = 0.0;

    ++m_cur_low_data;
    EvaluateCache();
 }


 template<class USER_DATA> void
    SE_JoinTransform<USER_DATA>::Transformer::Find(double x)
 {
    /* For now, naive linear search */
    /* TODO: which side is in/out? */
     const std::vector<TxData>* tx;
     const SE_Deque<TxCache>* cache;
     int*  index;

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

    if (x >= (*tx)[*index].pos)
        for (;x > (*tx)[(*index)+1].pos;++index);
    else
        while(x < (*tx)[--(*index)].pos);

    m_cur_cache = &(*cache)[*index];
    m_cur_low_data = &(*tx)[*index];
 }


 template<class USER_DATA> void SE_JoinTransform<USER_DATA>::Transformer::EvaluateCache()
 {
    if (m_cur_cache->inv_width == 0.0)
    {
        const TxData& low = m_cur_low_data[0];
        const TxData& high = m_cur_low_data[1];

        m_cur_cache->d_m_c = high.ctr - low.ctr;
        m_cur_cache->a_m_c = high.out - low.ctr;
        m_cur_cache->bc_m_ad = high.out + low.ctr - high.ctr - low.out;

        m_cur_cache->bcad_len = m_cur_cache->bc_m_ad.length();
        m_cur_cache->inv_width = 1.0 / (high.pos - low.pos);
    }
 }

 template<class USER_DATA> bool
    SE_JoinTransform<USER_DATA>::Transformer::Intersects(const SE_Tuple& A0, const SE_Tuple& A1,
                                                         const SE_Tuple& B0, const SE_Tuple& B1,
                                                         SE_Tuple& T, SE_Tuple& isection)
 {
     SE_Tuple Va  = A1 - A0;
     SE_Tuple Vb  = B1 - B0;
     SE_Tuple Vab = B0 - A0;
     double iaxb = 1.0 / Va.cross(Vb);

     T.x = Vab.cross(Vb) * iaxb;
     T.y = Vab.cross(Va) * iaxb;

     isection = A0 + (Va * T.x);
     
     return T.x >= 0.0 && T.x <= 1.0 && T.y >= 0.0 && T.y <= 0.0;
 }


 template<class USER_DATA> void 
    SE_JoinTransform<USER_DATA>::Transformer::AddPoint(const SE_Tuple& point)
 {
    m_cur_dst->_LineTo(point.x, point.y);
 }


 template<class USER_DATA> void 
    SE_JoinTransform<USER_DATA>::Transformer::BeginContour(const SE_Tuple& point)
 {
    m_cur_dst->_MoveTo(point.x, point.y);
 }

 
 template<class USER_DATA> double&
    SE_JoinTransform<USER_DATA>::Transformer::CurrentTolerance()
 {
    m_tolerance = 0.25;
    return m_tolerance;
 }
 
 
 template<class USER_DATA> void 
    SE_JoinTransform<USER_DATA>::Transformer::EndSegment()
 {
 }
  

 template<class USER_DATA> void 
    SE_JoinTransform<USER_DATA>::Transformer::EndContour()
 {
 }


 template<class USER_DATA> void 
    SE_JoinTransform<USER_DATA>::Transformer::EndLine()
 {
 }


 template<class USER_DATA> void 
     SE_JoinTransform<USER_DATA>::Transformer::LineToUV(const SE_Tuple& point, SE_Tuple& uv)
 {
    uv.x = (point.x + m_position - m_cur_low_data[0].pos) * m_cur_cache->inv_width;
    uv.y = fabs(point.y) * m_inv_height;
 }


 template<class USER_DATA> void 
    SE_JoinTransform<USER_DATA>::Transformer::MapPoint(const SE_Tuple& uv, SE_Tuple& world)
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
 */
 template<class USER_DATA> void 
    SE_JoinTransform<USER_DATA>::Transformer::MapSegment(const SE_Tuple& target_uv, 
                                                         double& tolerance)
 {
    SE_Tuple dp = target_uv - m_last_uv;

    SE_Tuple dPt, ddPt, d2Tdudv;
    double err = fabs(dp.x * dp.y) * 0.25 * m_cur_cache->bcad_len;
    int segs = (int)(sqrt(err / tolerance)) + 1;
    double invsegs = 1.0 / (double)segs;
    dp *= invsegs;
    tolerance -= err * invsegs * invsegs;

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
 }


 template<class USER_DATA> SE_LineStorage*
    SE_JoinTransform<USER_DATA>::Transformer::TransformLine(LineBuffer* src, double position)
 {
     _ASSERT(src->point_count() > 1);

     m_position = position;
     
     m_cur_dst = m_buffer->m_pool->NewLineStorage(src->point_count() * 2);

     for(int i = 0; i < src->cntr_count(); ++i)
     {
         /* I really love what getting rid of actual buffer access has done for client code... */
         int curidx = src->contour_start_point(i);
         int endidx = src->contour_end_point(i);

         SE_Tuple lastpt;
         SE_Tuple curpt;

         Move(SE_Tuple(src->x_coord(curidx) + m_position, src->y_coord(curidx)));
         MapPoint(m_last_uv, m_last_scrn);
         BeginContour(m_last_scrn);
         ++curidx;

         for (;curidx <= endidx; ++curidx)
         {
             lastpt = curpt;
             src->get_point(curidx, curpt.x, curpt.y);
             double low_edge = m_cur_low_data[0].pos - m_position;
             double high_edge = m_cur_low_data[1].pos - m_position;         
             m_next_pts.push_head(std::pair<SE_Tuple, update_fxn>(curpt, NULL));
             m_next_pts.head().first.x += m_position;
            
             while (m_next_pts.size())
             {
                 if (m_next_pts.head().first.x < low_edge)
                 {
                     m_next_pts.push_head( std::pair<SE_Tuple, update_fxn>(lastpt + 
                         (m_next_pts.head().first - lastpt) * 
                         ((low_edge - lastpt.x) / (m_next_pts.head().first.x - lastpt.x)),
                         &SE_JoinTransform<USER_DATA>::Transformer::Backward) );
                     continue;
                 }
                 else if (m_next_pts.head().first.x > high_edge)
                 {
                     m_next_pts.push_head( std::pair<SE_Tuple, update_fxn>(lastpt + 
                         (m_next_pts.head().first - lastpt) * 
                         ((high_edge - lastpt.x) / (m_next_pts.head().first.x - lastpt.x)),
                         &SE_JoinTransform<USER_DATA>::Transformer::Forward) );
                     continue;
                 }
                 if ((m_in_active && m_next_pts.head().first.y > 0) || 
                     (!m_in_active && m_next_pts.head().first.y < 0))
                 {
                     m_next_pts.push_head( std::pair<SE_Tuple, update_fxn>(lastpt + 
                         (m_next_pts.head().first - lastpt) * 
                         (- lastpt.y / (m_next_pts.head().first.y - lastpt.y)),
                         &SE_JoinTransform<USER_DATA>::Transformer::Vertical) );
                     continue;
                 }
                 
                 m_next_pts.head().first.x += m_position;

                 SE_Tuple target_uv;
                 LineToUV(m_next_pts.head().first, target_uv);
                 MapSegment(target_uv, CurrentTolerance());

                 if (m_next_pts.head().second)
                    (this->*m_next_pts.head().second)();
                 m_next_pts.pop_head();
                 EndSegment();
             }
         }
         EndContour();
     }
     EndLine();

     return m_cur_dst;
 }

template<class USER_DATA> void 
    SE_JoinTransform<USER_DATA>::Transformer::TransformArea
    ( double /* position */, const SE_Tuple /* outline */ [4], 
      std::vector<SE_Tuple>& /* uvquads */, 
      std::vector<SE_Tuple>& /* txquads */)
{
    /* TODO */
}

/******************************************************************************
 * SE_JoinTransform function definitions                                         *
 ******************************************************************************/

template<class USER_DATA> SE_JoinTransform<USER_DATA>::SE_JoinTransform
    (SE_BufferPool* pool, double height, int initsize) :
        m_out_pts(initsize),
        m_in_pts(initsize),
        m_inside(&m_in_pts),
        m_outside(&m_out_pts),
        m_vtx_cnt(-1),
        m_height(height),
        m_cur_in_cnt(0),
        m_cur_out_cnt(0),
        m_cur_data(-1),
        m_pool(pool),
        m_prev_in_cnt(0),
        m_prev_out_cnt(0)
{
}


template<class USER_DATA> SE_JoinTransform<USER_DATA>::~SE_JoinTransform()
{
}


/* If this is not called before every join, parts of the transform may be inverted */
template<class USER_DATA> void SE_JoinTransform<USER_DATA>::StartJoin(bool clockwise)
{
    m_prev_in_cnt = (int)m_in_tx.size() + m_cur_in_cnt;
    m_cur_in_cnt = -(int)m_in_tx.size();
    m_prev_out_cnt = (int)m_out_tx.size() + m_cur_out_cnt;
    m_cur_out_cnt = -(int)m_out_tx.size();
    m_prev_data = m_cur_data;
    m_cur_data = -1;

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
    ++m_vtx_cnt;
}


template<class USER_DATA> void SE_JoinTransform<USER_DATA>::ProcessSegment
    (double in_len, double out_len, double end_pos, const SE_Tuple end_vert)
{
    SE_Tuple dv = end_vert - m_prev_vtx;
    double dp = end_pos - m_prev_pos;
    double ilen = 1.0 / in_len;
    
    m_cur_in_cnt += m_in_pts.size();
    m_cur_out_cnt += m_out_pts.size();

    if (m_prev_data >= 0)
    {
        int start = (int)m_in_tx.size() - m_cur_in_cnt - 1;
        for (int i = start; i > start - m_prev_in_cnt; --i)
            m_in_tx[i].data = m_prev_data;
        start = (int)m_out_tx.size() - m_cur_out_cnt - 1;
        for (int i = start; i > start - m_prev_out_cnt; --i)
            m_out_tx[i].data = m_prev_data;
    }

    while (m_in_pts.size() > 0)
    {
        const std::pair<SE_Tuple, double>& point = m_in_pts.head();
        m_in_tx.push_back(TxData(point.first, 
                                 m_prev_vtx + (dv * (point.second * ilen)), 
                                 m_prev_pos + (dp * point.second * ilen)));
        m_in_pts.pop_head();
    }

    ilen = 1.0 / out_len;
    
    while (m_out_pts.size() > 0)
    {
        const std::pair<SE_Tuple, double>& point = m_out_pts.head();
        m_out_tx.push_back(TxData(point.first, 
                                  m_prev_vtx + (dv * (point.second * ilen)), 
                                  m_prev_pos + (dp * point.second * ilen)));
        m_out_pts.pop_head();
    }
}


template<class USER_DATA> void SE_JoinTransform<USER_DATA>::AddVertex
    (const SE_Tuple& outer, const SE_Tuple& vertex, const SE_Tuple& inner, double pos)
{
    /* The first vertex must be preceeded by zero points */
    _ASSERT(m_in_tx.size() || m_out_tx.size() || (m_in_pts.size() == m_out_pts.size() == 0));

    if (m_in_pts.size())
    {
        double id = (inner - m_inside->tail().first).length();
        double od = (outer - m_outside->tail().first).length();
        ProcessSegment(m_in_len + id, m_out_len + od, pos, vertex);
    }

    m_inside->push_tail(std::pair<SE_Tuple, double>(inner, 0.0));
    m_outside->push_tail(std::pair<SE_Tuple, double>(outer, 0.0));

    m_in_len = 0.0;
    m_out_len = 0.0;
    m_prev_vtx = vertex;
    m_prev_pos = pos;
}


template<class USER_DATA> void SE_JoinTransform<USER_DATA>::Close()
{
    _ASSERT(m_in_pts.size() == 1 && m_out_pts.size() == 1);

    m_in_tx.push_back(TxData(m_in_pts.head().first, m_prev_vtx, m_prev_pos));
    m_in_tx[m_in_tx.size()-1].data = m_cur_data;
    m_in_pts.pop_head();
    
    m_out_tx.push_back(TxData(m_out_pts.head().first, m_prev_vtx, m_prev_pos));
    m_out_tx[m_out_tx.size()-1].data = m_cur_data;
    m_out_pts.pop_head();

    m_in_cache.clear();
    m_in_cache.post_enlarge((int)m_in_tx.size() - 1);
    int size = (int)m_in_cache.size();
    for (int i = 0; 0 < size; ++i)
        m_in_cache[i].inv_width = 0.0;

    m_out_cache.clear();
    m_out_cache.post_enlarge((int)m_out_tx.size() - 1);
    size = (int)m_out_tx.size();
    for (int i = 0; 0 < size; ++i)
        m_out_cache[i].inv_width = 0.0;
}


template<class USER_DATA> void SE_JoinTransform<USER_DATA>::AddUserData(const USER_DATA& user_data)
{
    _ASSERT(m_cur_data == -1);
    
    m_cur_data = m_user_data.size();
    m_user_data.push_back(user_data);
}


template<class USER_DATA> void SE_JoinTransform<USER_DATA>::AddOutsidePoint(SE_Tuple& outer)
{
    /* Adding points in the case where m_outside->tail() is undefined will cause an
     * assertion to fail in the AddVertex call */
    double d = (outer - m_outside->tail().first).length();
    m_out_len += d;
    m_outside->push_tail(std::pair<SE_Tuple, double>(outer, d));
}


template<class USER_DATA> void SE_JoinTransform<USER_DATA>::AddInsidePoint(SE_Tuple& inner)
{
    double d = (inner - m_inside->tail()).length();
    m_in_len += d;
    m_inside->push_tail(std::pair(inner, d));
}


template<class USER_DATA> void SE_JoinTransform<USER_DATA>::Reset()
{
    m_out_pts.clear();
    m_in_pts.clear();
    m_user_data.clear();
    m_tx_data.clear();
}


template<class USER_DATA> typename SE_JoinTransform<USER_DATA>::Transformer
    SE_JoinTransform<USER_DATA>::GetTransformer()
{
    return Transformer(*this, m_height);
}

#endif // SE_JOINTRANSFORM_H
