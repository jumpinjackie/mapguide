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

#include "SE_RenderProxies.h"
#include "SE_JoinBuffer.h"
#include <algorithm>

#define MAX_STACK_ALLOC 2048

template<template<class T> class JOIN, template<class U> class CAP, class USER_DATA> 
    class JoinTransform
{
protected:
    typedef JOIN<USER_DATA>             JOIN_TYPE;
    typedef CAP<USER_DATA>              CAP_TYPE;
    typedef USER_DATA                   DATA_TYPE;
    typedef SE_JoinBuffer<USER_DATA>    BUFFER_TYPE;

    SE_Tuple        m_endpts[2];
    SE_SegmentInfo* m_segs;

    /* Fills segs with information from geometry. Caller must delete returned array */
    SE_SegmentInfo* ParseGeometry(SE_RenderLineStyle* style, double* geometry, int npts, int& nsegs);
    void ProcessSegments(BUFFER_TYPE& joins, SE_SegmentInfo* segs, int nsegs);
    SE_INLINE void SetUserData(USER_DATA& data, JOIN_TYPE& join, BUFFER_TYPE& joinbuf);

public:
    JoinTransform( double* contour, 
                   int npts, 
                   SE_RenderLineStyle* style,
                   SE_BufferPool* pool );
    ~JoinTransform();
};


struct NullData
{
};


struct OptData
{
    double join_width;
    double join_error;
};


template<template<class T> class JOIN, template<class U> class CAP> class NullTransform : public JoinTransform<JOIN, CAP, NullData> { };
template<template<class T> class JOIN, template<class U> class CAP> class OptTransform : public JoinTransform<JOIN, CAP, OptData> { };


// Function definitions

template<template<class T> class JOIN, template<class U> class CAP, class USER_DATA> 
    JoinTransform<JOIN, CAP, USER_DATA>::JoinTransform( double* contour, 
                                                   int npts, 
                                                   SE_RenderLineStyle* style,
                                                   SE_BufferPool* pool )
{
    SE_SegmentInfo* segbuf = ParseGeometry(style, contour, npts, nsegs);

    JOIN_TYPE::SetJoinData(JOIN_TYPE::JOIN_DATA_TYPE(style));
    CAP_TYPE::SetCapData(CAP_TYPE::CAP_DATA_TYPE(style));
    BUFFER_TYPE joinbuf(nsegs + nsegs/2);
    ProcessSegments(joinbuf, segbuf, nsegs);
}


template<template<class T> class JOIN, template<class U> class CAP, class USER_DATA> 
    JoinTransform<JOIN, CAP, USER_DATA>::~JoinTransform()
{
    delete[] m_segs;
}


template<template<class T> class JOIN, template<class U> class CAP, class USER_DATA>
    SE_SegmentInfo* JoinTransform<JOIN, CAP, USER_DATA>::ParseGeometry
    ( SE_RenderLineStyle* style, 
      double* geometry, 
      int npts, 
      int& nsegs )
{
    nsegs = npts - 1;

    double left = std::min( std::min(style->bounds[0], style->bounds[1]),
                            std::min(style->bounds[2], style->bounds[3]) ) + style->startOffset;
    double right = std::max( std::max(style->bounds[0], style->bounds[1]),
                             std::max(style->bounds[2], style->bounds[3]) ) - style->endOffset;

    SE_SegmentInfo* segbuf = m_segs = new SE_SegmentInfo[nsegs];

    double length = 0.0;
    SE_Tuple* pts = (SE_Tuple*)geometry;
    SE_Tuple* end = pts + nsegs;
    SE_SegmentInfo* segs = segbuf;

    for (; pts < end; pts++)
    {
        segs->vertex = pts;
        segs->next = *(pts+1) - *pts;
        /* TODO: handle colinear series of points (3+) */
        segs->nextlen = segs->next.length();
        /* TODO: not very robust! Find something mathematically sound! */
        if (segs->nextlen < .05)
        {
            nsegs--;
            continue;
        }
        segs->vertpos = length;
        length += segs->nextlen;
        segs++;
    }

    /* Extend the end segments, so that all of the line stylization is within the domain 
     * of the transform */
    if (left < 0.0)
    {
        double frac = left / segbuf[0].nextlen;
        m_endpts[0] = *segbuf[0].vertex - segbuf[0].next * left;
        segbuf[0].next *= (1.0 - frac);
        segbuf[0].nextlen *= (1.0 - frac);
        segbuf[0].vertpos += left;
        segbuf[0].vertex = &m_endpts;
    }
    else if (left > 0.0)
    {
        int i = 0;
        while(++i < nsegs && segbuf[i].vertpos < left);
        if (segbug[i].vertpos == left)
        {
            nsegs -= i;
            segbuf += i;
        }
        else
        {
            nsegs -= i - 1;
            segbug += i - 1;
            double frac = (left - segbuf[i-1].vertpos) / segbuf[i-1].nextlen;
            m_endpts[0] = *m_segbuf[i-1].vertex + (m_segbuf[i-1].next * frac);
            m_segbuf[i-1].vertex = &m_endpts;
            m_segbuf[i-1].next *= 1.0 - frac;
            m_segbuf[i-1].nextlen *= 1.0 - frac;

        }
    }

    if (right > 0.0)
    {
        double frac = right / segbuf[nsegs-1].nextlen;
        segbuf[nsegs-1].next *= (1.0 + frac);
        segbuf[nsegs-1].nextlen *= (1.0 + frac);
        segbuf[nsegs-1].vertpos += right;
    }
    else if (right < 0.0)
    {
        int i = nsegs;
        double threshold = segbuf[nsegs-1].vertpos + segbuf[nsegs-1].nextlen + right;
        while(--i >= 0 && segbuf[i].vertpos > threshold);

        if (segbuf[i].vertpos == threshold)
        {
            nsegs -= i;
        }
        else
        {
            nsegs -= nsegs - i - 1;
            double frac = (right - segbuf[i].vertpos) / segbuf[i].nextlen;
            m_endpts[1] = *m_segbuf[i].vertex + (m_segbuf[i].next * frac);
            m_segbuf[i].vertex = &m_endpts + 1;
            m_segbuf[i].next *= 1.0 - frac;
            m_segbuf[i].nextlen *= 1.0 - frac;
        }
    }

    return segbuf;
}


template<template<class T> class JOIN, template<class U> class CAP, class USER_DATA> void
    JoinTransform<JOIN, CAP, USER_DATA>::ProcessSegments(BUFFER_TYPE& joins, SE_SegmentInfo* segs, int nsegs)
{
    SE_SegmentInfo* curseg = segs;
    SE_SegmentInfo* lastseg = segs + nsegs - 1;
    
    {
        CAP_TYPE(*segs, true) startcap;
        startcap.Transform(joins);
    }

    for (curseg = segbuf; curseg++ < lastseg;)
    {
        USER_DATA data;
        JOIN_TYPE(*(curseg-1), *curseg) curjoin;
        curjoin.Transform(joins);
        SetUserData(data, joins);
    }

    CAP_TYPE(*lastseg, false) endcap;
    endcap.Transform(joins);
}


template<template<class T> class JOIN, template<class U> class CAP, class USER_DATA>  void
    JoinTransform<JOIN, CAP, USER_DATA>::SetUserData(USER_DATA& data, JOIN_TYPE& join, BUFFER_TYPE& joinbuf)
{
}

#endif // SE_JOINTRANSFORM_H
