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
    ginfo.join_dilation = 1.5;
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


template<>
void SE_JoinTransform<OptData>::ProcessSegment(double in_len,
                                               double out_len,
                                               double end_pos,
                                               const SE_Tuple end_vert)
{
    SE_Tuple dv = end_vert - m_prev_vtx;
    double dp = end_pos - m_prev_pos;
    double ilen = 1.0 / (in_len + m_in_pts.tail().second);

    double lpos = m_prev_data.join_width * m_global_info.join_dilation;
    double rpos = m_cur_data.join_width * m_global_info.join_dilation;
    
    int in_off = -1, out_off = -1;
    SE_Tuple innml, lctr, rctr;
    
    if (lpos + rpos < dp && (lpos > 0.0 || rpos > 0.0))
    {
        in_off = m_cur_in_cnt - (int)m_in_tx.size();
        out_off = m_cur_out_cnt - (int)m_out_tx.size();
        double idp = 1.0 / dp;

        innml = SE_Tuple(-dv.y * idp, dv.x * idp);
        if (lpos > 0.0)
        {
            lctr = m_prev_vtx + (dv * (lpos * idp));
            ++m_cur_in_cnt;
            ++m_cur_out_cnt;
        }
        if (rpos > 0.0)
            rctr = end_vert - (dv * (rpos * idp));

    }

    _ASSERT(in_off <= (int)m_in_pts.size());

    while (m_in_pts.size() > 0 || in_off >= 0)
    {
#ifdef _DEBUG
        /* If the outer edge of the transform is crossing the centerline, something has gone awry. */
        SE_Tuple t, ix;
        _ASSERT(m_in_tx.size() < 2 || !Intersects(m_in_tx[m_in_tx.size()-1].ctr, m_in_tx[m_in_tx.size()-2].ctr,
                                                  m_in_tx[m_in_tx.size()-1].out, m_in_tx[m_in_tx.size()-2].out,
                                                  t, ix));
#endif
        if (in_off-- == 0)
        {
            if (lpos > 0.0)
                m_in_tx.push_back(TxData(lctr + (innml * m_global_info.join_height), lctr, m_prev_pos + lpos));
            if (rpos > 0.0)
                m_in_tx.push_back(TxData(rctr + (innml * m_global_info.join_height), rctr, end_pos - rpos));
            continue;
        }

        const std::pair<SE_Tuple, double>& point = m_in_pts.head();

        _ASSERT(dp * point.second * ilen >= 0.0);
        m_in_tx.push_back(TxData(point.first,
            m_prev_vtx + (dv * (point.second * ilen)),
            m_prev_pos + (dp * point.second * ilen)));

        m_in_pts.pop_head();
    }

    ilen = 1.0 / (out_len + m_out_pts.tail().second);

    _ASSERT(out_off <= (int)m_out_pts.size());

    while (m_out_pts.size() > 0 || out_off >= 0)
    {
#ifdef _DEBUG
        SE_Tuple t, ix;
        _ASSERT(m_out_tx.size() < 2 || !Intersects(m_out_tx[m_out_tx.size()-1].ctr, m_out_tx[m_out_tx.size()-2].ctr,
                                                   m_out_tx[m_out_tx.size()-1].out, m_out_tx[m_out_tx.size()-2].out,
                                                   t, ix));
#endif
        if (out_off-- == 0)
        {
            if (lpos > 0.0)
                m_out_tx.push_back(TxData(lctr - (innml * m_global_info.join_height), lctr, m_prev_pos + lpos));
            if (rpos > 0.0)
                m_out_tx.push_back(TxData(rctr - (innml * m_global_info.join_height), rctr, end_pos - rpos));
            continue;
        }

        const std::pair<SE_Tuple, double>& point = m_out_pts.head();

        _ASSERT(dp * point.second * ilen >= 0.0);

        m_out_tx.push_back(TxData(point.first,
            m_prev_vtx + (dv * (point.second * ilen)),
            m_prev_pos + (dp * point.second * ilen)));

        m_out_pts.pop_head();
    }
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
