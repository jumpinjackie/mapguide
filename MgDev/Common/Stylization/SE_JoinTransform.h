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

class SE_Cap;
class SE_Join;

struct OptData
{
    double join_width;
    double join_error;

    /* Setting join_dilation to a value less than 1.0 will result in
     * undefined join artifacts, and is strongly discouraged. */
    struct GLOBAL_INFO
    {
        double join_dilation;
        double join_height;
    };

    SE_INLINE OptData() : join_width(0.0), join_error(.5) { }
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
class SE_JoinTransform
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
        double info;

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

    void ProcessSegmentInfo(const OptData& data,
                            int in_start,
                            int in_stop,
                            int out_start,
                            int out_stop);

    void ProcessSegmentSide( SE_Deque<std::pair<SE_Tuple, double> >& ptvec,
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
                             double dp );

    void PopulateTransformerData();

    SE_INLINE void LineToNoRepeat(LineBuffer* buf, const SE_Tuple& pt);

#ifdef _DEBUG
    int        m_vtx_cnt;
#endif
    int        m_cur_in_cnt;
    int        m_prev_in_cnt;
    int        m_cur_out_cnt;
    int        m_prev_out_cnt;
    OptData    m_cur_data;
    OptData    m_prev_data;
    SE_Tuple   m_prev_vtx;
    double     m_prev_pos;
    
    OptData::GLOBAL_INFO        m_global_info;

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
        friend class SE_JoinTransform;
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
        void MapSegment(const SE_Tuple& target_uv, const double& tolerance);

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

    SE_JoinTransform(int initsize = 10);
    ~SE_JoinTransform();

    void StartJoin(bool clockwise, const OptData& data);
    void AddVertex(const SE_Tuple& outer,
        const SE_Tuple& vertex,
        const SE_Tuple& inner,
        double line_position);
    void AddOutsidePoint(const SE_Tuple& outer);
    void AddInsidePoint(const SE_Tuple& inner);

    SE_INLINE const double& LastPosition() { return m_prev_pos; }
    SE_INLINE void SetGlobalInfo(const OptData::GLOBAL_INFO& info) { m_global_info = info; }

    void Close();
    void Reset();
    Transformer* GetTransformer(double clip_min, double clip_max, double height);
    void GetTransformOutline(LineBuffer* outline);
};


#endif // SE_JOINTRANSFORM_H
