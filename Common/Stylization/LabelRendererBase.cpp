//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "gd.h"
#include "GDRenderer.h"
#include "GDUtils.h"
#include "RS_Font.h"


//////////////////////////////////////////////////////////////////////////////
LabelRendererBase::LabelRendererBase(Renderer* renderer)
: m_renderer(renderer)
{
    //TODO: this needs cleanup
    m_serenderer = dynamic_cast<SE_Renderer*>(renderer);
}


//////////////////////////////////////////////////////////////////////////////
//Applies a given angle to an axis aligned bounding box.
//Rotation point is lower left
//TODO: move these transformations to the renderer
void LabelRendererBase::RotatedBounds(double x, double y, double width, double height, double angle_cw_rad, RS_F_Point* b)
{
    if (m_serenderer->YPointsUp())
    {
        //y goes up case
        double sina = sin(-angle_cw_rad);
        double cosa = cos(-angle_cw_rad);

        //apply rotation
        //taking into account that y goes up (so add instead of subtracting for y)
        b[0].x = x;
        b[0].y = y;
        b[1].x = x +  width * cosa;
        b[1].y = y + (width * sina);
        b[2].x = x +  width * cosa - height * sina;
        b[2].y = y + (width * sina + height * cosa);
        b[3].x = x                 - height * sina;
        b[3].y = y + (               height * cosa);
    }
    else
    {
        //y goes down case
        double sina = sin(angle_cw_rad);
        double cosa = cos(angle_cw_rad);

        //apply rotation
        //taking into account that y goes down (so subtract instead of adding for y)
        b[0].x = x;
        b[0].y = y;
        b[1].x = x +  width * cosa;
        b[1].y = y - (width * sina);
        b[2].x = x +  width * cosa - height * sina;
        b[2].y = y - (width * sina + height * cosa);
        b[3].x = x                 - height * sina;
        b[3].y = y - (               height * cosa);
    }
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererBase::CloseEnough(RS_F_Point& p1, RS_F_Point& p2)
{
    double delta = fabs(p2.y - p1.y) + fabs(p2.x - p1.x);
    return (delta <= 2.0); //2 pixels is close enough
}


//////////////////////////////////////////////////////////////////////////////
// Scales an input length in meters in the specified units - device or
// mapping - to a length in mapping space.
double LabelRendererBase::MeterToMapSize(RS_Units unit, double number)
{
    double scale_factor;
    if (unit == RS_Units_Device) // in meters, fixed size
        scale_factor = m_renderer->GetMapScale() / m_renderer->GetMetersPerUnit();
    else
        scale_factor = 1.0 / m_renderer->GetMetersPerUnit();

    return number * scale_factor;
}
