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

#ifndef SE_JOINPROCESSOR_H
#define SE_JOINPROCESSOR_H

#include "SE_RenderProxies.h"
#include "SE_JoinTransform.h"
#include "SE_Join.h"
#include "SE_Cap.h"
#include "SE_Matrix.h"
#include <algorithm>
#include <string.h>

#include "SE_Join_Miter.h"
#include "SE_Join_Bevel.h"
#include "SE_Join_Round.h"
#include "SE_Join_Identity.h"

#include "SE_Cap_Butt.h"
#include "SE_Cap_Triangle.h"
#include "SE_Cap_Square.h"
#include "SE_Cap_Round.h"

class SE_IJoinProcessor
{
public:
    virtual LineBuffer* Transform(LineBuffer* data, LineBufferPool* lbp) = 0;
    virtual void Transform(const SE_Tuple outline[4],
                           std::vector<SE_Tuple>& uvquads,
                           std::vector<SE_Tuple>& txquads) = 0;
};

template<class USER_DATA> class SE_JoinProcessor : public SE_IJoinProcessor
{
protected:
    typedef SE_Join<USER_DATA>          JOIN_TYPE;
    typedef SE_Cap<USER_DATA>           CAP_TYPE;
    typedef SE_JoinTransform<USER_DATA> BUFFER_TYPE;
    typedef USER_DATA                   DATA_TYPE;

    SE_Tuple                           m_endpt;
    SE_SegmentInfo*                    m_segs;
    JOIN_TYPE*                         m_join;
    CAP_TYPE*                          m_cap;
    BUFFER_TYPE                        m_joinbuf;
    typename BUFFER_TYPE::Transformer* m_tx;

    bool                  m_closed;
    double                m_tolerance;
    double                m_position;
    double                m_length;
    double                m_sym_ext[2];
    double                m_clip_ext[2];
    double                m_draw_ext[2];

    /* Fills segs with information from geometry. Caller must delete returned array */
    SE_SegmentInfo* ParseGeometry(SE_RenderLineStyle* style, LineBuffer* geom, int contour, int& nsegs);

    /* Applies appropriate joins & caps to the segs to generate transform information */
    void ProcessSegments(BUFFER_TYPE& joins, SE_SegmentInfo* segs, int nsegs);

    /* Initializes the cap and join memebers */
    void InitElements(SE_RenderLineStyle* style, SE_LineJoin join, SE_LineCap cap);

    /* Specialize these functions for the various types of user data */
    SE_INLINE void ProcessUserData(USER_DATA& data, JOIN_TYPE* join, BUFFER_TYPE& buffer);
    SE_INLINE void ProcessUserData(USER_DATA& data, CAP_TYPE* cap, BUFFER_TYPE& buffer);
    SE_INLINE double& GetTolerance(USER_DATA& data);

public:
    SE_JoinProcessor( SE_LineJoin join,
                      SE_LineCap cap,
                      LineBuffer* geom,
                      int contour,
                      SE_RenderLineStyle* style );
    ~SE_JoinProcessor();

    SE_INLINE void UpdateLinePosition(double position);
    SE_INLINE double StartPosition() const;
    SE_INLINE double EndPosition() const;

    virtual LineBuffer* Transform(LineBuffer* data, LineBufferPool* lbp);
    virtual void Transform(const SE_Tuple outline[4],
                           std::vector<SE_Tuple>& uvquads,
                           std::vector<SE_Tuple>& txquads);
    void AppendOutline(LineBuffer* lb);
};

// Function definitions

template<class USER_DATA>
SE_JoinProcessor<USER_DATA>::SE_JoinProcessor( SE_LineJoin join,
                                               SE_LineCap cap,
                                               LineBuffer* geom,
                                               int contour,
                                               SE_RenderLineStyle* style ) :
    m_joinbuf((3 * geom->cntr_size(contour)) >> 1)
{
    int nsegs;
    InitElements(style, join, geom->contour_closed(contour) ? SE_LineCap_None : cap);
    SE_SegmentInfo* segbuf = ParseGeometry(style, geom, contour, nsegs);
    ProcessSegments(m_joinbuf, segbuf, nsegs);
}


template<class USER_DATA>
SE_JoinProcessor<USER_DATA>::~SE_JoinProcessor()
{
    delete[] m_segs;
    delete m_tx;
    delete m_join;
    delete m_cap;
}


template<class USER_DATA>
double& SE_JoinProcessor<USER_DATA>::GetTolerance(USER_DATA& /*data*/)
{
    m_tolerance = .25;
    return m_tolerance;
}


template<class USER_DATA>
void SE_JoinProcessor<USER_DATA>::InitElements(SE_RenderLineStyle* style, SE_LineJoin join, SE_LineCap cap)
{
    switch (join)
    {
    case SE_LineJoin_Bevel:
        m_join = new SE_Join_Bevel<USER_DATA>( style );
        break;
    case SE_LineJoin_Round:
        m_join = new SE_Join_Round<USER_DATA>( style );
        break;
    case SE_LineJoin_Miter:
        m_join = new SE_Join_Bevel<USER_DATA>( style );
        break;
    case SE_LineJoin_None:
        m_join = new SE_Join_Identity<USER_DATA>( style );
        break;
    }

    switch (cap)
    {
    case SE_LineCap_Square:
        m_cap = new SE_Cap_Square<USER_DATA>( style );
        break;
    case SE_LineCap_Round:
        m_cap = new SE_Cap_Round<USER_DATA>( style );
        break;
    case SE_LineCap_Triangle:
        m_cap = new SE_Cap_Triangle<USER_DATA>( style );
        break;
    default:
    case SE_LineCap_None:
        m_cap = new SE_Cap_Butt<USER_DATA>( style );
        break;
    }
}

template<class USER_DATA>
SE_SegmentInfo* SE_JoinProcessor<USER_DATA>::ParseGeometry(SE_RenderLineStyle* style,
                                                           LineBuffer* geometry,
                                                           int contour, int& nsegs)
{
    nsegs = geometry->cntr_size(contour) - 1;

    /* If the contour is closed, dispense with offsets and make the transform continuous */
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
        /* The address of the x coord is the same as the address of the point struct */
        segs->vertex = (SE_Tuple*)&geometry->x_coord(j);
        segs->next = *((SE_Tuple*)&geometry->x_coord(i+1)) - *segs->vertex;
        segs->nextlen = segs->next.length();

        /* TODO: handle colinear series of points (3+) */
        /* TODO: not very robust! Find something mathematically sound! */
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

    /* Extend the end segments, so that all of the line stylization is within the domain
     * of the transform */
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


template<class USER_DATA>
void SE_JoinProcessor<USER_DATA>::ProcessSegments(BUFFER_TYPE& joins, SE_SegmentInfo* segs, int nsegs)
{
    if (nsegs == 0)
    {
        m_tx = NULL;
        return;
    }

    SE_SegmentInfo* curseg = segs;
    SE_SegmentInfo* lastseg = segs + nsegs - 1;

    /* Degenerate polygon */
    _ASSERT(!(nsegs == 2 && (curseg->next + lastseg->next).lengthSquared() < .001));

    if (!m_closed)
    {
        USER_DATA data;
        m_cap->Construct(*segs, GetTolerance(data), true);
        m_cap->Transform(joins, data(*m_cap));
    }
    else
    {
        USER_DATA data;
        m_join->Construct(*lastseg, *segs, GetTolerance(data));
        m_join->Transform(joins, data(*m_join));
    }

    while (curseg++ < lastseg)
    {
        USER_DATA data;
        m_join->Construct(*(curseg-1), *curseg, GetTolerance(data));
        m_join->Transform(joins, data(*m_join));
    }

    if (!m_closed)
    {
        USER_DATA data;
        m_cap->Construct(*lastseg, GetTolerance(data), false);
        m_cap->Transform(joins, data(*m_cap));
    }
    else
    {
        USER_DATA data;
        m_join->Construct(*lastseg, *segs, GetTolerance(data));
        m_join->Transform(joins, data(*m_join));
    }
    joins.Close();

    /* We (potentially) sacrifice 1/100,000 of a symbol so that we don't end up with
     * incredibly thin slices of adjacent symbols at the ends of the line*/
    double delta = (m_sym_ext[1] - m_sym_ext[0]) * 1e-5;
    m_tx = joins.GetTransformer(m_clip_ext[0] + delta, m_clip_ext[1] - delta, m_join->join_height());
}


template<class USER_DATA>
void SE_JoinProcessor<USER_DATA>::UpdateLinePosition(double position)
{
    m_position = position;
}


template<class USER_DATA> double SE_JoinProcessor<USER_DATA>::StartPosition() const
{
    return m_draw_ext[0];
}


template<class USER_DATA> double SE_JoinProcessor<USER_DATA>::EndPosition() const
{
    return m_draw_ext[1];
}


template<class USER_DATA>
LineBuffer* SE_JoinProcessor<USER_DATA>::Transform(LineBuffer* data, LineBufferPool* lbp)
{
    if (m_tx)
        return m_tx->TransformLine(data, m_position, lbp);

    return LineBufferPool::NewLineBuffer(lbp, 0);
}


template<class USER_DATA>
void SE_JoinProcessor<USER_DATA>::Transform(const SE_Tuple outline[4],
                                            std::vector<SE_Tuple>& uvquads,
                                            std::vector<SE_Tuple>& txquads)
{
    if (m_tx)
        m_tx->TransformArea(m_position, outline, uvquads, txquads);
}


template<class USER_DATA>
void SE_JoinProcessor<USER_DATA>::AppendOutline(LineBuffer* lb)
{
    m_joinbuf.GetTransformOutline(lb);
}

#endif // SE_JOINPROCESSOR_H
