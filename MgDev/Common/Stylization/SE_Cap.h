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

#ifndef SE_CAP_H
#define SE_CAP_H

#define CAP_ERROR_FRACTION 0.75

#include "StylizationDefs.h"
#include "SE_JoinTransform.h"
#include "SE_RenderProxies.h"

// Interface implemented by all cap classes
class SE_NOVTABLE SE_Cap
{
public:
    SE_INLINE const double& cap_width() const
    {
        return m_width;
    }

    SE_INLINE const double& cap_height() const
    {
        return m_cap_ext;
    }

    virtual void Construct(const SE_SegmentInfo& seg,
                           double& tolerance,
                           bool isStart);
    virtual void Transform(SE_JoinTransform& joins, const LocalJoinInfo& data) = 0;

protected:
    SE_Cap(SE_RenderLineStyle* style);

    double*  m_tolerance;
    double   m_width;
    double   m_cap_ext;
    double   m_base_pos; // the linear position of the endpoint of the line
    SE_Tuple m_base_pt;  // this is the endpoint of the line
    SE_Tuple m_cw_nml;   // this normal is clockwise and cap_ext long
};


class SE_Cap_Butt : public SE_Cap
{
public:
    SE_Cap_Butt(SE_RenderLineStyle* style) : SE_Cap(style)
    {}

    virtual void Construct(const SE_SegmentInfo& seg,
                           double& tolerance,
                           bool isStart);
    virtual void Transform(SE_JoinTransform& joins, const LocalJoinInfo& data);

protected:
};


class SE_Cap_Square : public SE_Cap
{
public:
    SE_Cap_Square(SE_RenderLineStyle* style) : SE_Cap(style)
    {}

    virtual void Construct(const SE_SegmentInfo& seg,
                           double& tolerance,
                           bool isStart);
    virtual void Transform(SE_JoinTransform& joins, const LocalJoinInfo& data);

protected:
    SE_Tuple m_ext_pt;
    double   m_ext_pos;
};


class SE_Cap_Round : public SE_Cap_Square
{
public:
    SE_Cap_Round(SE_RenderLineStyle* style) : SE_Cap_Square(style)
    {}

    virtual void Construct(const SE_SegmentInfo& seg,
                           double& tolerance,
                           bool isStart);
    virtual void Transform(SE_JoinTransform& joins, const LocalJoinInfo& data);

protected:
    double   m_cos_arc_inc;   // cosine of arc segment span
    double   m_sin_arc_inc;   // sine of arc segment span
    int      m_quad_segs;     // number of segments either above or below the centerline
};


class SE_Cap_Triangle : public SE_Cap_Square
{
public:
    SE_Cap_Triangle(SE_RenderLineStyle* style) : SE_Cap_Square(style)
    {}

    virtual void Transform(SE_JoinTransform& joins, const LocalJoinInfo& data);

protected:
};

#endif // SE_CAP_H
