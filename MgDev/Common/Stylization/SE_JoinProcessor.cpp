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
#include "SE_JoinProcessor.h"


SE_JoinProcessor::SE_JoinProcessor(SE_LineJoin join,
                                   SE_LineCap cap,
                                   LineBuffer* geom,
                                   int contour,
                                   SE_RenderLineStyle* style) :
    m_joinbuf((3 * geom->cntr_size(contour)) >> 1)
{
    int nsegs;
    InitElements(style, join, geom->contour_closed(contour) ? SE_LineCap_None : cap);

    // set global data
    GlobalJoinInfo ginfo;
    ginfo.join_dilation = 2.0;
    ginfo.join_height = m_join->join_height();
    m_joinbuf.SetGlobalInfo(ginfo);

    SE_SegmentInfo* segbuf = ParseGeometry(style, geom, contour, nsegs);
    ProcessSegments(m_joinbuf, segbuf, nsegs);
}


SE_JoinProcessor::~SE_JoinProcessor()
{
    delete[] m_segs;
    delete m_tx;
    delete m_join;
    delete m_cap;
}


void SE_JoinProcessor::InitElements(SE_RenderLineStyle* style, SE_LineJoin join, SE_LineCap cap)
{
    switch (join)
    {
    case SE_LineJoin_Bevel:
        m_join = new SE_Join_Bevel( style );
        break;
    case SE_LineJoin_Round:
        m_join = new SE_Join_Round( style );
        break;
    case SE_LineJoin_Miter:
        m_join = new SE_Join_Bevel( style );
        break;
    case SE_LineJoin_None:
        m_join = new SE_Join_Identity( style );
        break;
    }

    switch (cap)
    {
    case SE_LineCap_Square:
        m_cap = new SE_Cap_Square( style );
        break;
    case SE_LineCap_Round:
        m_cap = new SE_Cap_Round( style );
        break;
    case SE_LineCap_Triangle:
        m_cap = new SE_Cap_Triangle( style );
        break;
    default:
    case SE_LineCap_None:
        m_cap = new SE_Cap_Butt( style );
        break;
    }
}


SE_SegmentInfo* SE_JoinProcessor::ParseGeometry(SE_RenderLineStyle* style,
                                                LineBuffer* geometry,
                                                int contour, int& nsegs)
{
    nsegs = geometry->cntr_size(contour) - 1;

    // if the contour is closed, dispense with offsets and make the transform continuous
    double startoff, endoff;
    m_closed = geometry->contour_closed(contour);
    if (m_closed)
        startoff = endoff = 0.0;
    else
        startoff = style->startOffset, endoff = style->endOffset;

    m_sym_ext[0] = rs_min( rs_min(style->bounds[0].x, style->bounds[1].x),
                           rs_min(style->bounds[2].x, style->bounds[3].x) );
    m_sym_ext[1] = rs_max( rs_max(style->bounds[0].x, style->bounds[1].x),
                           rs_max(style->bounds[2].x, style->bounds[3].x) );
    double left  = rs_max(0.0, startoff);
    double right = rs_min(-endoff, 0.0);

    SE_SegmentInfo* segbuf = m_segs = new SE_SegmentInfo[nsegs];

    m_length = 0.0;
    SE_SegmentInfo* segs = segbuf;
    int first_idx = geometry->contour_start_point(contour);
    int last_idx = geometry->contour_end_point(contour);

    // Iterate through the input geometry (in pixel space) and populate the segment
    // info array.  We optimize away short segments (those less than 0.5 pixels long).
    for (int i=first_idx, j=first_idx; i<last_idx; ++i)
    {
        // the address of the x coord is the same as the address of the point struct
        segs->vertex = (SE_Tuple*)&geometry->x_coord(j);
        segs->next = *((SE_Tuple*)&geometry->x_coord(i+1)) - *segs->vertex;
        segs->nextlen = segs->next.length();

        // TODO: handle colinear series of points (3+)
        // TODO: not very robust! Find something mathematically sound!
        if (segs->nextlen < 0.5)
        {
            // skip this segment, but not if it means ending up with zero segments
            if ((nsegs > 1 && i < last_idx - 1) || segs->nextlen == 0.0)
            {
                nsegs--;
                continue;
            }
        }

        segs->vertpos = m_length;
        m_length += segs->nextlen;
        segs++;
        j = i+1;
    }

    if (nsegs == 0)
    {
        delete[] m_segs;
        m_segs = NULL;
        m_clip_ext[0] = m_draw_ext[0] =  DBL_MAX;
        m_clip_ext[1] = m_draw_ext[1] = -DBL_MAX;
        return m_segs;
    }

    m_clip_ext[0] =             left - m_cap->cap_width();
    m_clip_ext[1] = m_length + right + m_cap->cap_width();

    if (endoff >= 0.0 && startoff < 0.0)
    {
        m_draw_ext[1] = m_clip_ext[1];
        m_draw_ext[0] = m_clip_ext[1] -
            ceil((m_clip_ext[1] - m_clip_ext[0]) / style->repeat) * style->repeat;
    }
    else
    {
        m_draw_ext[0] = m_clip_ext[0];
        m_draw_ext[1] = m_clip_ext[1];
    }

    // extend the end segments, so that all of the line stylization is within
    // the domain of the transform
    if (m_length - left + right <= 0.0)
    {
        nsegs = 0;
        delete[] m_segs;
        m_segs = NULL;
        return m_segs;
    }

    if (left > 0.0)
    {
        int i = 0;
        while(++i < nsegs && segbuf[i].vertpos < left);
        if (segbuf[i].vertpos == left)
        {
            nsegs -= i;
            segbuf += i;
        }
        else
        {
            nsegs -= i - 1;
            double frac = (left - segbuf[i-1].vertpos) / segbuf[i-1].nextlen;
            m_endpt = *segbuf[i-1].vertex + (segbuf[i-1].next * frac);
            segbuf[i-1].vertex = &m_endpt;
            segbuf[i-1].vertpos = left;
            segbuf[i-1].next *= 1.0 - frac;
            segbuf[i-1].nextlen *= 1.0 - frac;
            segbuf += i - 1;
        }
    }

    if (right < 0.0)
    {
        int i = nsegs;
        double threshold = segbuf[nsegs-1].vertpos + segbuf[nsegs-1].nextlen + right;
        while(--i >= 0 && segbuf[i].vertpos > threshold);
        nsegs = i + 1;

        if (segbuf[i].vertpos == threshold)
            --nsegs;
        else
        {
            double frac = (threshold - segbuf[i].vertpos) / segbuf[i].nextlen;
            segbuf[i].next *= 1.0 - frac;
            segbuf[i].nextlen *= 1.0 - frac;
        }
    }

    return segbuf;
}


void SE_JoinProcessor::ProcessSegments(SE_JoinTransform& joins, SE_SegmentInfo* segs, int nsegs)
{
    if (nsegs == 0)
    {
        m_tx = NULL;
        return;
    }

    SE_SegmentInfo* curseg = segs;
    SE_SegmentInfo* lastseg = segs + nsegs - 1;

    // degenerate polygon
    _ASSERT(!(nsegs == 2 && (curseg->next + lastseg->next).lengthSquared() < 0.001));

    if (!m_closed)
    {
        LocalJoinInfo data;
        m_cap->Construct(*segs, GetTolerance(data), true);
        data.join_width = m_cap->cap_width();
        m_cap->Transform(joins, data);
    }
    else
    {
        LocalJoinInfo data;
        m_join->Construct(*lastseg, *segs, GetTolerance(data));
        data.join_width = m_join->join_width();
        m_join->Transform(joins, data);
    }

    while (curseg++ < lastseg)
    {
        LocalJoinInfo data;
        m_join->Construct(*(curseg-1), *curseg, GetTolerance(data));
        data.join_width = m_join->join_width();
        m_join->Transform(joins, data);
    }

    if (!m_closed)
    {
        LocalJoinInfo data;
        m_cap->Construct(*lastseg, GetTolerance(data), false);
        data.join_width = m_cap->cap_width();
        m_cap->Transform(joins, data);
    }
    else
    {
        LocalJoinInfo data;
        m_join->Construct(*lastseg, *segs, GetTolerance(data));
        data.join_width = m_join->join_width();
        m_join->Transform(joins, data);
    }
    joins.Close();

    // we (potentially) sacrifice 1/100000 of a symbol so that we don't end up
    // with incredibly thin slices of adjacent symbols at the ends of the line
    double delta = (m_sym_ext[1] - m_sym_ext[0]) * 1.0e-5;
    m_tx = joins.GetTransformer(m_clip_ext[0] + delta, m_clip_ext[1] - delta, m_join->join_height());
}


LineBuffer* SE_JoinProcessor::Transform(LineBuffer* data, LineBufferPool* lbp)
{
    if (m_tx)
        return m_tx->TransformLine(data, m_position, lbp);

    return LineBufferPool::NewLineBuffer(lbp, 0);
}


void SE_JoinProcessor::Transform(const SE_Tuple outline[4],
                                 std::vector<SE_Tuple>& uvquads,
                                 std::vector<SE_Tuple>& txquads)
{
    if (m_tx)
        m_tx->TransformArea(m_position, outline, uvquads, txquads);
}
