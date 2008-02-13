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

#ifndef SE_JOIN_H
#define SE_JOIN_H

#define COLINEAR_THRESHOLD 0.001

#define JOIN_ERROR_FRACTION 0.75

#include "StylizationDefs.h"
#include "SE_JoinTransform.h"
#include "SE_RenderProxies.h"

// Interface implemented by all join classes
class SE_NOVTABLE SE_Join
{
public:
    // Initializes the state of the join using two segments.  Implementors
    // should call the base function.
    virtual void Construct(const SE_SegmentInfo& lead,
                           const SE_SegmentInfo& tail,
                           double& tolerance);

    // adds the transform segments for the current (constructed) vertex
    virtual void Transform(SE_JoinTransform& joins, const LocalJoinInfo& data) = 0;

    // the distance along the line from the inside of the join to the vertex
    SE_INLINE const double& join_width() const
    {
        return m_width;
    }

    // the vertical extent of the join above and below the centerline
    SE_INLINE const double& join_height() const
    {
        return m_join_ext;
    }

    SE_INLINE const SE_SegmentInfo& lead_seg() const
    {
        return *m_lead;
    }

    SE_INLINE const SE_SegmentInfo& tail_seg() const
    {
        return *m_tail;
    }

protected:
    SE_Join(SE_RenderLineStyle* style);

    const SE_SegmentInfo* m_lead;
    const SE_SegmentInfo* m_tail;

    // the available (remaining) error for the join transformation
    double* m_tolerance;

    double m_width;
    double m_join_ext;

    double m_lxt;
    bool   m_colinear;

    SE_Tuple m_lead_nml;
    SE_Tuple m_tail_nml;
};


class SE_Join_Identity : public SE_Join
{
public:
    SE_Join_Identity(SE_RenderLineStyle* style) : SE_Join(style)
    {}

    virtual void Construct(const SE_SegmentInfo& lead,
                           const SE_SegmentInfo& tail,
                           double& tolerance);
    virtual void Transform(SE_JoinTransform& joins, const LocalJoinInfo& data);

protected:
    bool m_clockwise;
};


class SE_Join_Miter : public SE_Join
{
public:
    SE_Join_Miter(SE_RenderLineStyle* style) : SE_Join(style)
    {}

    virtual void Construct(const SE_SegmentInfo& lead,
                           const SE_SegmentInfo& tail,
                           double& tolerance);
    virtual void Transform(SE_JoinTransform& joins, const LocalJoinInfo& data);

protected:
    double m_sin_a;     // the sine of the angle between the two segments
    double m_cos_a;     // the cosine of the angle between the two segments
    double m_tan_ha;    // the tangent of half the angle between the two segments
    double m_sin_ha;    // the sine of half the angle between the two segments
    double m_cos_ha;    // the cosine of half the angle between the two segments

    double m_miter;     // the distance from the inside of the join to the vertex (or the
                        // vertex to the end of the miter)

    double m_inside;    // the maximum extent of the join to the inside.  May be less than
                        // the miter if the miter's length exceeds the segment lengths

    bool m_clockwise;
};


class SE_Join_Bevel : public SE_Join_Miter
{
public:
    SE_Join_Bevel(SE_RenderLineStyle* style) : SE_Join_Miter(style),
        m_miter_limit(style->vertexMiterLimit)
    {}

    virtual void Construct(const SE_SegmentInfo& lead,
                           const SE_SegmentInfo& tail,
                           double& tolerance);
    virtual void Transform(SE_JoinTransform& joins, const LocalJoinInfo& data);

private:
    // top width (0 for minimal bevel, m_width for unbeveled miter)
    double m_top_width;
    double m_miter_limit;
};


class SE_Join_Round : public SE_Join_Miter
{
public:
    SE_INLINE SE_Join_Round(SE_RenderLineStyle* style) : SE_Join_Miter(style)
    {}

    virtual void Construct(const SE_SegmentInfo& lead,
                           const SE_SegmentInfo& tail,
                           double& tolerance);
    virtual void Transform(SE_JoinTransform& joins, const LocalJoinInfo& data);

private:
    unsigned int m_verts;
    SE_Matrix m_vert_rot;
};

#endif // SE_JOIN_H
