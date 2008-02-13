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

#ifndef SE_JOINPROCESSOR_H
#define SE_JOINPROCESSOR_H

#include "SE_RenderProxies.h"
#include "SE_JoinTransform.h"

#include "SE_Matrix.h"
#include <algorithm>
#include <string.h>

#include "SE_Join.h"
#include "SE_Cap.h"


class SE_IJoinProcessor
{
public:
    virtual LineBuffer* Transform(LineBuffer* data, LineBufferPool* lbp) = 0;
    virtual void Transform(const SE_Tuple outline[4],
                           std::vector<SE_Tuple>& uvquads,
                           std::vector<SE_Tuple>& txquads) = 0;
};


class SE_JoinProcessor : public SE_IJoinProcessor
{
protected:
    SE_Tuple                       m_endpt;
    SE_SegmentInfo*                m_segs;
    SE_Join*                       m_join;
    SE_Cap*                        m_cap;
    SE_JoinTransform               m_joinbuf;
    SE_JoinTransform::Transformer* m_tx;

    bool   m_closed;
    double m_tolerance;
    double m_position;
    double m_length;
    double m_sym_ext[2];
    double m_clip_ext[2];
    double m_draw_ext[2];

    // Fills segs with information from geometry.  Caller must delete returned array
    SE_SegmentInfo* ParseGeometry(SE_RenderLineStyle* style, LineBuffer* geom, int contour, int& nsegs);

    // applies appropriate joins & caps to the segs to generate transform information
    void ProcessSegments(SE_JoinTransform& joins, SE_SegmentInfo* segs, int nsegs);

    // initializes the cap and join memebers
    void InitElements(SE_RenderLineStyle* style, SE_LineJoin join, SE_LineCap cap);

    // specialize these functions for the various types of user data
    SE_INLINE double& GetTolerance(LocalJoinInfo& data)
    {
        return data.join_error;
    }

public:
    SE_JoinProcessor(SE_LineJoin join,
                     SE_LineCap cap,
                     LineBuffer* geom,
                     int contour,
                     SE_RenderLineStyle* style);
    ~SE_JoinProcessor();

    SE_INLINE void UpdateLinePosition(double position)
    {
        m_position = position;
    }

    SE_INLINE double StartPosition() const
    {
        return m_draw_ext[0];
    }

    SE_INLINE double EndPosition() const
    {
        return m_draw_ext[1];
    }

    virtual LineBuffer* Transform(LineBuffer* data, LineBufferPool* lbp);
    virtual void Transform(const SE_Tuple outline[4],
                           std::vector<SE_Tuple>& uvquads,
                           std::vector<SE_Tuple>& txquads);
};

#endif // SE_JOINPROCESSOR_H
