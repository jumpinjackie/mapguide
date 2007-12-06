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

/* Because of the nature of the template specialization, this file can only be
 * included once in a dll. */

#ifndef SE_JOINPROCESSOR_OPT_H_
#define SE_JOINPROCESSOR_OPT_H_

#include "SE_JoinProcessor.h"

struct OptData
{
    double join_width;
    double join_error;

    /* Store segment error */
    typedef double TX_INFO;

    /* Setting join_dilation to a value less than 1.0 will result in
     * undefined join artifacts, and is strongly discouraged. */
    struct GLOBAL_INFO
    {
        double join_dilation;
        double join_height;
    };

    SE_INLINE OptData() : join_width(0.0), join_error(.5) { }

    const OptData& operator() (const SE_Join<OptData>& join)
    {
        join_width = join.join_width();
        return *this;
    }

    const OptData& operator() (const SE_Cap<OptData>& cap)
    {
        join_width = cap.cap_width();
        return *this;
    }
};

typedef SE_JoinProcessor<OptData> SE_JoinProcessor_Opt;

template<>
SE_JoinProcessor<OptData>::SE_JoinProcessor( SE_LineJoin join,
                                             SE_LineCap cap,
                                             LineBuffer* geom,
                                             int contour,
                                             SE_RenderLineStyle* style ) :
    m_joinbuf((3 * geom->cntr_size(contour)) >> 1)
{
    int nsegs;
    InitElements(style, join, geom->contour_closed(contour) ? SE_LineCap_None : cap);

    /* Set global data */
    OptData::GLOBAL_INFO ginfo;
    ginfo.join_dilation = 2.0;
    ginfo.join_height = m_join->join_height();
    m_joinbuf.SetGlobalInfo(ginfo);
    
    SE_SegmentInfo* segbuf = ParseGeometry(style, geom, contour, nsegs);
    ProcessSegments(m_joinbuf, segbuf, nsegs);
}


template<>
double& SE_JoinProcessor<OptData>::GetTolerance(OptData& data)
{
    return data.join_error;
}


template<>
void SE_JoinTransform<OptData>::ProcessSegmentInfo(const OptData& data,
                                                   int in_start,
                                                   int in_stop,
                                                   int out_start,
                                                   int out_stop)
{
    /* TODO: make a distinction between inside & outside error */
    for (int i = in_start; i < in_stop; ++i)
        m_in_tx[i].info = (OptData::TX_INFO)data.join_error;
    
    for (int i = out_start; i < out_stop; ++i)
        m_out_tx[i].info = (OptData::TX_INFO)data.join_error;
}

/* Templating gets around the fact that SE_JoinTransform::TxData is a private struct */
template<class TX_DATA>
void ProcessSegmentSide( SE_Deque<std::pair<SE_Tuple, double> >& ptvec,
                         std::vector<TX_DATA>& txvec,
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
                         double dp )
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
        /* Parametric position of the last point of the first join + the distance from the point to
         * the first join delimiter is the parametric distance from the start point to the first delimiter. */
        lenl = (ptvec[offset-1].second + (ptvec[offset-1].first - (lctr + nml)).length());
        dvl = dv * (lpos / dp);
        /* The parametric distance along the outer contour from the second join delimiter (or
         * first, if there is no second) to the endpoint */
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
        /* The parametric distance along the outer contour from the end point to the only join delmiter is
         * the total distance along the contour minus the distance to the last point of the first join minus
         * the distance from that point to the join delimiter. */
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

    for (int i = 0; i < offset; ++i)
    {
        const std::pair<SE_Tuple, double>& point = ptvec.head();
        _ASSERT(point.second <= lenl);
        txvec.push_back(TX_DATA(point.first,
            startpt + (dvl * (point.second * ilenl)),
            startpos + (lpos * point.second * ilenl)));
        ptvec.pop_head();
    }

    if (l_del)
        txvec.push_back(TX_DATA(lctr + nml, lctr, startpos + lpos));
    if (r_del)
        txvec.push_back(TX_DATA(rctr + nml, rctr, endpos - rpos));


    while (ptvec.size())
    {
        const std::pair<SE_Tuple, double>& point = ptvec.head();
        _ASSERT(point.second >= dr && point.second - dr < lenr);
        txvec.push_back(TX_DATA(point.first,
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
        /* If the outer edge of the transform is crossing the centerline, something has gone awry. */
        _ASSERT(!Intersects(txvec[txvec.size()-i].ctr, txvec[txvec.size()-i-1].ctr,
                            txvec[txvec.size()-i].out, txvec[txvec.size()-i-1].out,
                            t, ix) && 
                txvec[txvec.size()-i].pos >= txvec[txvec.size()-i-1].pos);
    }
#endif
}


template<>
void SE_JoinTransform<OptData>::ProcessSegment(double in_len,
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

    ProcessSegmentSide<TxData>(m_in_pts, m_in_tx, m_prev_vtx, end_vert, lctr, rctr, innml, dv,
        in_off, m_prev_pos, end_pos, lpos, rpos, in_len + m_in_pts.tail().second, dp);

    _ASSERT(out_off != 0 && out_off <= (int)m_out_pts.size());
    if (out_off == 0 || out_off > (int)m_out_pts.size())
        return;

    ProcessSegmentSide<TxData>(m_out_pts, m_out_tx, m_prev_vtx, end_vert, lctr, rctr, -innml, dv,
        out_off, m_prev_pos, end_pos, lpos, rpos, out_len + m_out_pts.tail().second, dp);
}


template<>
void SE_JoinTransform<OptData>::Close()
{
    _ASSERT(m_in_pts.size() == 1 && m_out_pts.size() == 1);

    m_in_tx.push_back(TxData(m_in_pts.head().first, m_prev_vtx, m_prev_pos));
    m_in_pts.pop_head();

    m_out_tx.push_back(TxData(m_out_pts.head().first, m_prev_vtx, m_prev_pos));
    m_out_pts.pop_head();

    ProcessSegmentInfo(m_cur_data, m_cur_in_cnt, (int)m_in_tx.size(), m_cur_out_cnt, (int)m_out_tx.size());

    PopulateTransformerData();
}


template<>
double& SE_JoinTransform<OptData>::Transformer::CurrentTolerance()
{
    return m_cur_low_data->info;
}

#endif //SE_JOINPROCESSOR_OPT_H_
