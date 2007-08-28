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

#ifndef SE_JOINBUFFER_H
#define SE_JOINBUFFER_H

#include "SE_Tuple.h"
#include "SE_LineBuffer.h"
#include "SE_Deque.h"
#include <vector>
#include <utility>

struct SE_SegmentInfo
{
    SE_Tuple* vertex;
    SE_Tuple next;
    double vertpos;
    double nextlen;
};

template<class USER_DATA> class SE_JoinBuffer
{
public:
    struct TxCache;

private:
    struct TransformData
    {
        double   pos;
        SE_Tuple out;
        SE_Tuple ctr;
        int      vertex;

        TransformData(const SE_Tuple& pout, const SE_Tuple& pctr, double lpos, int vtx = 0)
        {
            pos = lpos;
            out = pout;
            ctr = pctr;
            vertex = vtx;
        }
    };
  
    void ProcessSegment(double in_len, 
                        double out_len, 
                        double end_pos, 
                        const SE_Tuple end_vert, 
                        bool last = false);

    int      m_vtx_cnt;
    SE_Tuple m_last_vtx;
    double   m_last_pos;
    double   m_in_len;
    double   m_out_len;

    SE_Deque<std::pair<SE_Tuple, double>>  m_out_pts;
    SE_Deque<std::pair<SE_Tuple, double>>  m_in_pts;
    std::vector<USER_DATA>                 m_user_data;
    std::vector<TransformData>             m_in_tx;
    SE_Deque<TxCache>                      m_in_cache;
    std::vector<TransformData>             m_out_tx;
    SE_Deque<TxCache>                      m_out_cache;

    SE_Deque<std::pair<SE_Tuple, double>>* m_inside;
    SE_Deque<std::pair<SE_Tuple, double>>* m_outside;

public:

    /*    (0,1)    (1,1)
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
     * The symbol space is the rectangle from (0, left line_pos) to (|vmax|, right line_pos), 
     * normalized to (0,0) to (1,1) in u-v space.
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
        double iWidth;        /* 1/width (Symbol space) */
    };

    class TxIterator
    {
        int                  m_in_idx;
        int                  m_out_idx;
        bool                 m_in_active;
        const SE_JoinBuffer& m_buffer;

        TxIterator(const SE_JoinBuffer& buffer);
        
        TxCache& Find(double u);
        const TxCache& Evaluate(TxCache& txc);

    public:
        const TxCache& Move(double u, double v);
        const TxCache& Forward();
        const TxCache& Backward();
        const TxCache& Vertical(double u);    
    };

    SE_INLINE SE_JoinBuffer(int initsize = 10);
    SE_INLINE ~SE_JoinBuffer();

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
    SE_INLINE TxIterator Iterator();
};

/******************************************************************************
 * SE_JoinBuffer::TxIterator function definitions                             *
 ******************************************************************************/

 template<class USER_DATA> 
    SE_JoinBuffer<USER_DATA>::TxIterator::TxIterator(const SE_JoinBuffer& buffer) :
    m_buffer(buffer),
    m_in_idx(0),
    m_out_idx(0),
    m_in_active(false)
 {
 }


 template<class USER_DATA> const typename SE_JoinBuffer<USER_DATA>::TxCache&
    SE_JoinBuffer<USER_DATA>::TxIterator::Move(double u, double v)
 {
    m_in_active = v < 0.0;
    return Evaluate(Find(u));
 }


  template<class USER_DATA> const typename SE_JoinBuffer<USER_DATA>::TxCache&
    SE_JoinBuffer<USER_DATA>::TxIterator::Vertical(double u)
 {
    m_in_active = !m_in_active;
    return Evaluate(Find(u));
 }
  

 template<class USER_DATA> const typename SE_JoinBuffer<USER_DATA>::TxCache&
    SE_JoinBuffer<USER_DATA>::TxIterator::Backward()
 {
    if (m_in_active)
        return Evaluate(m_buffer.m_in_cache[--m_in_idx]);
    else
        return Evaluate(m_buffer.m_out_cache[--m_out_idx]);
 }


 template<class USER_DATA> const typename SE_JoinBuffer<USER_DATA>::TxCache&
    SE_JoinBuffer<USER_DATA>::TxIterator::Forward()
 {
    if (m_in_active)
        return Evaluate(m_buffer.m_in_cache[++m_in_idx]);
    else
        return Evaluate(m_buffer.m_out_cache[++m_out_idx]);
 }


 template<class USER_DATA> typename SE_JoinBuffer<USER_DATA>::TxCache&
    SE_JoinBuffer<USER_DATA>::TxIterator::Find(double u)
 {
    /* For now, naive linear search */
    /* TODO: which side is in/out? */
     const std::vector<TransformData>& tx;
     const SE_Deque<TxCache>& cache;
     int&  index;

    if (m_in_active)
    { 
        tx = m_buffer.m_in_tx;
        cache = m_buffer.m_in_cache;
        index = m_in_index;
    }
    else
    {
        tx = m_buffer.m_out_tx;
        cache = m_buffer.m_out_cache;
        index = m_out_index;
    }

    if (u >= tx[index].pos)
    {
        for (;u > tx[index+1].pos;++index);
        return cache[index];
    }
    else
    {
        while(u < tx[--index].pos);
        return cache[index];
    }
 }


 template<class USER_DATA> const typename SE_JoinBuffer<USER_DATA>::TxCache&
    SE_JoinBuffer<USER_DATA>::TxIterator::Evaluate(TxCache& txc)
 {
    if (txc.iWidth == 0)
    {
        const TransformData& left;
        const TransformData& right;

        if (m_in_active)
        {
            left = m_buffer.m_in_tx[m_in_idx];
            right = m_buffer.m_in_tx[m_in_idx+1];
        }
        else
        {
            left = m_buffer.m_out_tx[m_out_idx];
            right = m_buffer.m_out_tx[m_out_idx+1];
        }

        txc.d_m_c = right.ctr - left.ctr;
        txc.a_m_c = right.out - left.ctr;
        txc.bc_m_ad = right.out + left.ctr - right.ctr - left.out;
        txc.bcad_len = txc.bc_m_ad.length();
        txc.iWidth = 1.0/(right.pos - left.pos);
    }
    return txc;
 }

/******************************************************************************
 * SE_JoinBuffer function definitions                                         *
 ******************************************************************************/

template<class USER_DATA> SE_JoinBuffer<USER_DATA>::SE_JoinBuffer(int initsize) :
    m_out_pts(initsize),
    m_in_pts(initsize),
    m_inside(&m_in_pts),
    m_outside(&m_out_pts),
    m_vtx_cnt(-1)
{
}


template<class USER_DATA> SE_JoinBuffer<USER_DATA>::~SE_JoinBuffer()
{
}


/* If this is not called before every join, parts of the transform may be inverted */
template<class USER_DATA> void SE_JoinBuffer<USER_DATA>::StartJoin(bool clockwise)
{
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


template<class USER_DATA> void SE_JoinBuffer<USER_DATA>::ProcessSegment
    (double in_len, double out_len, double end_pos, const SE_Tuple end_vert, bool last)
{
    SE_Tuple dv = end_vert - m_last_vtx;
    double dp = end_pos - m_last_pos;
    double ilen = 1.0 / m_in_len;
    
    while (m_in_pts.size() > 1)
    {
        const std::pair<SE_Tuple, double>& point = m_in_pts.head();
        m_in_tx.push_tail(TransformData(point.first, 
                                        m_last_vtx + (dv * point.second), 
                                        m_last_pos + (dp * point.first * ilen)));
        m_in_pts.pop_head();
    }

    ilen = 1.0 / m_out_len;
    
    while (m_out_pts.size() > 1)
    {
        const std::pair<SE_Tuple, double>& point = m_out_pts.head();
        m_out_tx.push_tail(TransformData(point.first, 
                                         m_last_vtx + (dv * point.second), 
                                         m_last_pos + (dp * point.first * ilen)));
        m_out_pts.pop_head();
    }
}


template<class USER_DATA> void SE_JoinBuffer<USER_DATA>::AddVertex
    (const SE_Tuple& outer, const SE_Tuple& vertex, const SE_Tuple& inner, double pos)
{
    /* The first vertex must be preceeded by zero points */
    _ASSERT(m_tx_data.size() || (m_in_pts.size() == m_out_pts.size() == 0));

    if (m_in_pts.size())
    {
        double id = (inner - m_inside->tail()).length();
        double od = (outer - m_outside->tail()).length();
        ProcessSegment(m_in_len + id, m_out_len + od, pos, vertex);
    }

    m_inside->push_tail(std::pair(inner, 0.0));
    m_outside->push_tail(std::pair(outer, 0.0));

    m_in_len = 0.0;
    m_out_len = 0.0;
    m_last_vtx = vertex;
    m_last_pos = pos;

    /* Fails if the call to AddVertex was not preceeded by a call to StartJoin. */
    _ASSERT(m_lead_radius == 0 && (m_lead_radius = m_radius, true))
       
    m_last_data = &m_data[m_data.size() - 1].user_info;
}


template<class USER_DATA> void SE_JoinBuffer<USER_DATA>::Close()
{
    _ASSERT(m_in_pts.size() == 1 && m_out_pts.size() == 1);

    const std::pair<SE_Tuple, double>& point = m_in_pts.head();
    m_in_tx.push_tail(TransformData(point.first, m_last_vtx, m_last_pos));
    m_in_pts.pop_head();
    
    const std::pair<SE_Tuple, double>& point = m_out_pts.head();
    m_out_tx.push_tail(TransformData(point.first, m_last_vtx, m_last_pos));
    m_out_pts.pop_head();

    m_in_cache.clear();
    m_in_cache.post_enlarge(m_in_tx.size() - 1);
    int size = m_in_cache.size();
    for (int i = 0; 0 < size; ++i)
        m_in_cache[i].iWidth = 0.0;

    m_out_cache.clear();
    m_out_cache.post_enlarge(m_out_tx.size() - 1);
    size = m_out_tx.size();
    for (int i = 0; 0 < size; ++i)
        m_out_cache[i].iHeight = 0.0;
}


template<class USER_DATA> void SE_JoinBuffer<USER_DATA>::AddUserData(const USER_DATA& user_data)
{

}


template<class USER_DATA> void SE_JoinBuffer<USER_DATA>::AddOutsidePoint(SE_Tuple& outer)
{
    /* Adding points in the case where m_outside->tail() is undefined will cause an
     * assertion to fail in the AddVertex call */
    double d = (outer - m_outside->tail()).length();
    m_out_len += d;
    m_outside->push_tail(std::pair(outer, d));
}


template<class USER_DATA> void SE_JoinBuffer<USER_DATA>::AddInsidePoint(SE_Tuple& inner)
{
    double d = (inner - m_inside->tail()).length();
    m_in_len += d;
    m_inside->push_tail(std::pair(inner, d));
}


template<class USER_DATA> void SE_JoinBuffer<USER_DATA>::Reset()
{
    m_out_pts.clear();
    m_in_pts.clear();
    m_user_data.clear();
    m_tx_data.clear();
}

#endif // SE_JOINBUFFER_H
