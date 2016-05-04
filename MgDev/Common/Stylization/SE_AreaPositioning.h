//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef SE_AREAPOSITIONING_H
#define SE_AREAPOSITIONING_H

#include "StylizationDefs.h"
#include "SE_RenderProxies.h"
#include "Point2D.h"

///////////////////////////////////////////////////////////////////////////////
// This class generates the possible insertion points for the symbols to be drawn
// in a hatch based on bounding box intersection.  Further checking must be done
// to determine if every position is valid, although all valid positions will be
// generated.
class SE_AreaPositioning
{
public:
    SE_AreaPositioning(LineBuffer* geom, SE_RenderAreaStyle* style, double w2sAngleRad);
    ~SE_AreaPositioning();

    const double& PatternRotation();
    const Point2D* NextLocation();

private:
    static int ClipLine(double xMin, double xMax, const Point2D& p0, const Point2D& p1, double* ret);
    static int ClipCode(double xMin, double xMax, double x);

private:
    double    m_angle_rad;

    int       m_h_min;
    int       m_h_max;
    int       m_h_pts;
    Point2D   m_base_pt;
    Vector2D  m_v_vec;
    Vector2D  m_h_vec;

    Point2D   m_cur_pt;
    int       m_h_cur_pos;
    int       m_v_cur_pos;

    int*      m_v_min;
    int*      m_v_max;

    static const int m_k_buf_size = 512;
    int       m_v_buf[2*m_k_buf_size];
};

#endif
