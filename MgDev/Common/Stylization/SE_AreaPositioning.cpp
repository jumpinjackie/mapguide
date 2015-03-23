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

#include "stdafx.h"
#include "SE_AreaPositioning.h"


///////////////////////////////////////////////////////////////////////////////
// Initializes the area positioning class for iteration over the geometry.
SE_AreaPositioning::SE_AreaPositioning(LineBuffer* geom, SE_RenderAreaStyle* style, double w2sAngleRad)
{
    // for now treat negative repeats as positive
    double repeatX = fabs(style->repeat[0]);
    double repeatY = fabs(style->repeat[1]);

    // zero-repeats are not accepted
    if (repeatX == 0.0 || repeatY == 0.0)
    {
        // configure values so that no points are returned
        m_h_min = 0;
        m_h_max = 0;
        m_h_pts = 1;
        m_v_min = m_v_buf;
        m_v_max = m_v_buf + m_h_pts;
        m_v_max[0] = -1;
        m_h_cur_pos = 0;
        m_v_cur_pos = 0;

        m_angle_rad = 0.0;

        return;
    }

    // get the min/max of the polygon's bounding box
    Point2D min = Point2D(geom->bounds().minx, geom->bounds().miny);
    Point2D max = Point2D(geom->bounds().maxx, geom->bounds().maxy);

    // get the symbol grid origin
    if (style->originControl == SE_OriginControl_Centroid)
    {
        // origin is the current polygon's centroid
        double slope;
        geom->Centroid(LineBuffer::ctArea, &m_base_pt.x, &m_base_pt.y, &slope);

        // if we can't compute the centroid then just set the base point to the origin
        if (_isnan(m_base_pt.x) || _isnan(m_base_pt.y))
        {
            m_base_pt.x = 0.0;
            m_base_pt.y = 0.0;
        }
    }
    else if (style->originControl == SE_OriginControl_Local)
    {
        // origin is specified in the current polygon's space - use parametric
        // coordinates where [(0,0), (1,1)] maps to the polygon's bounding box
        Vector2D diff = max - min;
        m_base_pt.x = min.x + style->origin[0] * diff.x;
        m_base_pt.y = min.y + style->origin[1] * diff.y;
    }
    else
    {
        // origin is specified as a global point in device space
        m_base_pt.x = style->origin[0];
        m_base_pt.y = style->origin[1];
    }

    // get the symbol grid angle
    if (style->angleControl == SE_AngleControl_FromGeometry)
    {
        // angle matches the orientation of the longest edge of the polygon
        double maxlensq = -1.0;
        double dirX = 0.0;
        double dirY = 0.0;

        for (int i=0; i<geom->cntr_count(); ++i)
        {
            int start = geom->contour_start_point(i);
            int end = geom->contour_end_point(i);

            double lastX = geom->x_coord(start);
            double lastY = geom->y_coord(start);

            for (int j=start+1; j<=end; ++j)
            {
                double currX = geom->x_coord(j);
                double currY = geom->y_coord(j);

                double segX = currX - lastX;
                double segY = currY - lastY;
                double curlensq = segX*segX + segY*segY;

                if (curlensq > maxlensq)
                {
                    maxlensq = curlensq;
                    dirX = segX;
                    dirY = segY;
                }

                lastX = currX;
                lastY = currY;
            }
        }

        // in this case the specified angle is relative to the angle computed
        // from the geometry
        m_angle_rad = style->angleRad + atan2(dirY, dirX);
    }
    else
    {
        // in this case the specified angle is the absolute angle, including
        // any viewport rotation
        m_angle_rad = style->angleRad + w2sAngleRad;
    }

    double cosa = cos(m_angle_rad);
    double sina = sin(m_angle_rad);

    // these are the vectors defining the symbol grid
    m_h_vec = Vector2D( cosa, sina) * repeatX;
    m_v_vec = Vector2D(-sina, cosa) * repeatY;

    // get the min/max of the symbol's bounding box
    RS_Bounds symBounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
    symBounds.add_point(style->bounds[0]);
    symBounds.add_point(style->bounds[1]);
    symBounds.add_point(style->bounds[2]);
    symBounds.add_point(style->bounds[3]);

    // We need to compute the number of repetitions in the symbol grid which
    // provide the necessary coverage for the polygon's bounding box.  To do
    // this we transform the bounding box to the symbol grid space.  This
    // space is rotated and translated relative to the screen space:
    //
    //   {screen pt} = [T(origin)] [R(angle)] {grid pt}
    //
    // We need the inverse transform going from screen to grid space:
    //
    //   {grid pt} = [R(-angle)] [T(-origin)] {screen pt}
    //
    SE_Matrix ixform;
    ixform.translate(-m_base_pt.x, -m_base_pt.y);
    ixform.rotate(-sina, cosa);

    // transform the polygon bounding box corners to grid space
    //
    // 3 x---------x 2
    //   |         |
    //   |         |
    //   |         |
    // 0 x---------x 1
    //
    Point2D outline[4];
    ixform.transform(min.x, min.y, outline[0].x, outline[0].y);
    ixform.transform(max.x, min.y, outline[1].x, outline[1].y);
    ixform.transform(max.x, max.y, outline[2].x, outline[2].y);
    ixform.transform(min.x, max.y, outline[3].x, outline[3].y);

    // find the min and max x-values for the transformed bounding box
    double outlineMinX = outline[0].x;
    double outlineMaxX = outline[0].x;
    for (int i=1; i<4; ++i)
    {
        outlineMinX = rs_min(outlineMinX, outline[i].x);
        outlineMaxX = rs_max(outlineMaxX, outline[i].x);
    }

    // In the symbol grid, the horizontal position of grid cell M's origin
    // is M*repeatX.  The horizontal extent of the rendered symbol for that
    // cell is: [M*repeatX + symBounds.minx, M*repeatX + symBounds.maxx].  We
    // need to compute the range of cells that span the transformed bounds.
    // The initial cell is the smallest value of M satisfying:
    //    M * repeatX + symBounds.maxx >= outlineMinX
    // while the final cell is the largest value of M satisfying:
    //    M * repeatX + symBounds.minx <= outlineMaxX
    //
    m_h_min = (int) ceil((outlineMinX - symBounds.maxx) / repeatX);
    m_h_max = (int)floor((outlineMaxX - symBounds.minx) / repeatX);

    // the required number of columns in the grid
    m_h_pts = m_h_max - m_h_min + 1;

    // For each column we'll compute the required vertical range of cells
    // and store that in the m_v_min and m_v_max arrays.

    // make sure the array is large enough
    if (m_h_pts > m_k_buf_size)
        m_v_min = new int[2*m_h_pts];
    else
        m_v_min = m_v_buf;

    m_v_max = m_v_min + m_h_pts;

    // now compute the vertical range for each column
    for (int i=0; i<m_h_pts; ++i)
    {
        double posX = (m_h_min + i) * repeatX;

        // [xmin, xmax] gives the horizontal extent of the column
        double xmin = posX + symBounds.minx;
        double xmax = posX + symBounds.maxx;

        double ymin = +DBL_MAX;
        double ymax = -DBL_MAX;

        // iterate over the outline's edges
        for (int j=0; j<4; ++j)
        {
            // get the points for the edge
            const Point2D& p0 = outline[j];
            const Point2D& p1 = outline[(j+1) & 3];

            // clip the edge against the extent - if the call returns 0 both
            // edge points are clipped to the same side and the segment can
            // be skipped
            double bline[4];
            int res = SE_AreaPositioning::ClipLine(xmin, xmax, p0, p1, bline);
            if (res == 0)
                continue;

            // the y-values for the clipped edge are in bline[1] and bline[3]
            if (bline[3] < bline[1])
                std::swap(bline[1], bline[3]);

            ymin = rs_min(ymin, bline[1]);
            ymax = rs_max(ymax, bline[3]);
        }

        if (ymax >= ymin)
        {
            // In the symbol grid, the vertical position of grid cell N's origin
            // is N*repeatY.  The vertical extent of the rendered symbol for that
            // cell is: [N*repeatY + symBounds.miny, N*repeatY + symBounds.maxy].
            // We need to compute the range of cells that span [ymin, ymax].
            // The initial cell is the smallest value of N satisfying:
            //    N * repeatY + symBounds.maxy >= ymin
            // while the final cell is the largest value of N satisfying:
            //    N * repeatY + symBounds.miny <= ymax
            m_v_min[i] = (int) ceil((ymin - symBounds.maxy) / repeatY);
            m_v_max[i] = (int)floor((ymax - symBounds.miny) / repeatY);

            _ASSERT(m_v_max[i] - m_v_min[i] >= -1);
        }
        else
        {
            // get here if the outline lies fully outside the extent (might
            // happen due to roundoff)

            // set the values so that this column is skipped
            m_v_min[i] = -1;
            m_v_max[i] = -2;
        }
    }

    m_h_cur_pos = m_h_min;
    m_v_cur_pos = m_v_min[0];
}


///////////////////////////////////////////////////////////////////////////////
SE_AreaPositioning::~SE_AreaPositioning()
{
    if (m_v_buf != m_v_min)
        delete[] m_v_min;
}


///////////////////////////////////////////////////////////////////////////////
const double& SE_AreaPositioning::PatternRotation()
{
    return m_angle_rad;
}


///////////////////////////////////////////////////////////////////////////////
// Returns the next point in the symbol grid at which a symbol must be
// rendered for the current feature.
const Point2D* SE_AreaPositioning::NextLocation()
{
    // check if we're done with the current column
    while (m_v_cur_pos > m_v_max[m_h_cur_pos - m_h_min])
    {
        // we're done - check if we're at the last column
        if (++m_h_cur_pos - m_h_min >= m_h_pts)
            return NULL;

        // reset vertical position to start of next column
        m_v_cur_pos = m_v_min[m_h_cur_pos - m_h_min];
    }

    // compute position from horizontal / vertical tile grid positions
    m_cur_pt.x = m_base_pt.x + m_h_vec.x * m_h_cur_pos + m_v_vec.x * m_v_cur_pos;
    m_cur_pt.y = m_base_pt.y + m_h_vec.y * m_h_cur_pos + m_v_vec.y * m_v_cur_pos;

    ++m_v_cur_pos;

    return &m_cur_pt;
}


///////////////////////////////////////////////////////////////////////////////
// Clips a line against the clip region.
#define INSIDE 0x00
#define LEFT   0x01
#define RIGHT  0x02

int SE_AreaPositioning::ClipLine(double xMin, double xMax, const Point2D& p0, const Point2D& p1, double* ret)
{
    ret[0] = p0.x;
    ret[1] = p0.y;
    ret[2] = p1.x;
    ret[3] = p1.y;

    // compute the initial clip codes for the endpoints
    int clipCode1 = SE_AreaPositioning::ClipCode(xMin, xMax, p0.x);
    int clipCode2 = SE_AreaPositioning::ClipCode(xMin, xMax, p1.x);

    // trivially reject or accept the line segment
    if ((clipCode1 & clipCode2) != 0)   // both points left or both points right
        return 0;
    if (clipCode1 == clipCode2)         // both points inside
        return 1;

    // compute the difference between the x and y values of the endpoints
    double deltaX = p1.x - p0.x;
    double deltaY = p1.y - p0.y;

    // deltaX should never be zero - the case of both points having the same
    // x-coordinate is automatically handled by the previous if-checks
    _ASSERT(deltaX != 0.0);

    // check the first point
    if (clipCode1 == LEFT)
    {
        ret[0] = xMin;
        ret[1] = p0.y + (xMin - p0.x) * deltaY / deltaX;
    }
    else if (clipCode1 == RIGHT)
    {
        ret[0] = xMax;
        ret[1] = p0.y + (xMax - p0.x) * deltaY / deltaX;
    }

    // check the second point
    int retcode = 1;
    if (clipCode2 == LEFT)
    {
        ret[2] = xMin;
        ret[3] = p1.y + (xMin - p1.x) * deltaY / deltaX;
        retcode = 2;
    }
    else if (clipCode2 == RIGHT)
    {
        ret[2] = xMax;
        ret[3] = p1.y + (xMax - p1.x) * deltaY / deltaX;
        retcode = 2;
    }

    return retcode;
}


///////////////////////////////////////////////////////////////////////////////
// Returns whether a point is inside the clip region, or to the left or right.
int SE_AreaPositioning::ClipCode(double xMin, double xMax, double x)
{
    if (x < xMin)
        return LEFT;

    if (x > xMax)
        return RIGHT;

    return INSIDE;
}
