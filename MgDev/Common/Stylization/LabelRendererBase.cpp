//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "LabelRendererBase.h"
#include "SE_Renderer.h"
#include "RS_Font.h"
#include "RS_FontEngine.h"


//////////////////////////////////////////////////////////////////////////////
LabelRendererBase::LabelRendererBase(SE_Renderer* se_renderer)
: m_serenderer(se_renderer)
{
}


//////////////////////////////////////////////////////////////////////////////
// Computes the rotated corner points for all lines in the supplied text.
void LabelRendererBase::GetRotatedTextPoints(RS_TextMetrics& tm, double insx, double insy, double angleRad, RS_F_Point* rotatedPts)
{
    // radian CCW rotation
    double cos_a = cos(angleRad);
    double sin_a = sin(angleRad);

    for (size_t k=0; k<tm.line_pos.size(); ++k)
    {
        LinePos& pos = tm.line_pos[k];
        RS_F_Point* pts = &rotatedPts[k*4];

        // process the extent points
        for (int j=0; j<4; ++j)
        {
            // rotate and translate to the insertion point
            double tmpX = pos.ext[j].x;
            double tmpY = pos.ext[j].y;
            pts[j].x = insx + tmpX * cos_a - tmpY * sin_a;
            pts[j].y = insy + tmpX * sin_a + tmpY * cos_a;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// Computes the rotated corner points for the supplied axis-aligned box.
// Rotation point is lower left.
void LabelRendererBase::GetRotatedPoints(double x, double y, double width, double height, double angleRad, RS_F_Point* rotatedPts)
{
    // radian CCW rotation
    double sina = sin(angleRad);
    double cosa = cos(angleRad);

    if (m_serenderer->YPointsUp())
    {
        // apply rotation, taking into account that y goes up (so add instead of subtracting for y)
        rotatedPts[0].x = x;
        rotatedPts[0].y = y;
        rotatedPts[1].x = x + width * cosa;
        rotatedPts[1].y = y + width * sina;
        rotatedPts[2].x = x + width * cosa - height * sina;
        rotatedPts[2].y = y + width * sina + height * cosa;
        rotatedPts[3].x = x                - height * sina;
        rotatedPts[3].y = y                + height * cosa;
    }
    else
    {
        // apply rotation, taking into account that y goes down (so subtract instead of adding for y)
        rotatedPts[0].x = x;
        rotatedPts[0].y = y;
        rotatedPts[1].x = x + width * cosa;
        rotatedPts[1].y = y - width * sina;
        rotatedPts[2].x = x + width * cosa - height * sina;
        rotatedPts[2].y = y - width * sina - height * cosa;
        rotatedPts[3].x = x                - height * sina;
        rotatedPts[3].y = y                - height * cosa;
    }
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererBase::CloseEnough(RS_F_Point& pt1, RS_F_Point& pt2)
{
    double delta = fabs(pt2.y - pt1.y) + fabs(pt2.x - pt1.x);
    return (delta <= 2.0); // 2 screen units (pixels in our case) is close enough
}


//////////////////////////////////////////////////////////////////////////////
// Scales an input length in meters in the specified units - device or
// mapping - to a length in mapping space.
double LabelRendererBase::MeterToMapSize(RS_Units units, double number)
{
    double scale_factor;
    if (units == RS_Units_Device)   // in meters, fixed size
        scale_factor = m_serenderer->GetMapScale() / m_serenderer->GetMetersPerUnit();
    else
        scale_factor = 1.0 / m_serenderer->GetMetersPerUnit();

    return number * scale_factor;
}
