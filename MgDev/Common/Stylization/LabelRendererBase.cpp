//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
LabelRendererBase::LabelRendererBase(GDRenderer* renderer)
: m_renderer(renderer)
{
}


//////////////////////////////////////////////////////////////////////////////
// Finds occurences of line breaks and adds the start pointer of each
// line to the supplied array.  Line breaks can be any of the following:
//   "\\n"                      // sequence currently used by MG OS / Enterprise
//   \n\r  (char 13 + char 10)  // common in RDBMS like Oracle
//   \r\n  (char 10 + char 13)
//   \n    (char 13)            // used by MG 6.5
//   \r    (char 10)            // common in Linux
size_t LabelRendererBase::SplitLabel(wchar_t* label, std::vector<wchar_t*>& line_breaks)
{
    _ASSERT(label != NULL);
    if (label == NULL)
        return 0;

    // first line
    line_breaks.push_back(label);

    for (;;)
    {
        // find the next line break - note that we must search
        // for the 2-character sequences FIRST
        wchar_t* found;
        for (;;)
        {
            found = ::wcsstr(label, L"\\n");
            if (found != NULL)
            {
                // null terminate line break (2 characters)
                *found++ = 0;
                *found++ = 0;
                break;
            }

            found = ::wcsstr(label, L"\n\r");
            if (found != NULL)
            {
                // null terminate line break (2 characters)
                *found++ = 0;
                *found++ = 0;
                break;
            }

            found = ::wcsstr(label, L"\r\n");
            if (found != NULL)
            {
                // null terminate line break (2 characters)
                *found++ = 0;
                *found++ = 0;
                break;
            }

            found = ::wcsstr(label, L"\n");
            if (found != NULL)
            {
                // null terminate line break (1 character)
                *found++ = 0;
                break;
            }

            found = ::wcsstr(label, L"\r");
            if (found != NULL)
            {
                // null terminate line break (1 character)
                *found++ = 0;
            }

            break;
        }

        if (found == NULL)
            break;

        label = found;
        line_breaks.push_back(label);
    }

    return line_breaks.size();
}


//////////////////////////////////////////////////////////////////////////////
// Computes the X offset that must be applied to the unrotated text to
// obtain the specified horizontal alignment.
double LabelRendererBase::GetHorizontalAlignmentOffset(RS_TextDef& tdef, RS_F_Point* extent)
{
    double offsetX = 0.0;

    RS_HAlignment hAlign = tdef.halign();
    switch (hAlign)
    {
    case RS_HAlignment_Left:
        offsetX = -extent[0].x;
        break;

    case RS_HAlignment_Center:
        offsetX = -0.5*(extent[0].x + extent[1].x);
        break;

    case RS_HAlignment_Right:
        offsetX = -extent[1].x;
        break;
    }

    return offsetX;
}


//////////////////////////////////////////////////////////////////////////////
// Computes the Y offset that must be applied to the unrotated text to
// obtain the specified vertical alignment.  Positive Y points down.
double LabelRendererBase::GetVerticalAlignmentOffset(RS_TextDef& tdef, const RS_Font* font,
                                                     double actual_height, double line_height,
                                                     size_t numLines)
{
    double offsetY = 0.0;

    // According to FreeType:
    //
    //   * The face's ascender is the vertical distance from the baseline
    //     to the topmost point of any glyph in the face.  This field's
    //     value is positive.
    //
    //   * The face's descender is the vertical distance from the baseline
    //     to the bottommost point of any glyph in the face.  This field's
    //     value is *negative* for values below the baseline.
    //
    //   * The face's height is the vertical distance from one baseline to
    //     the next when writing several lines of text.  Its value is always
    //     positive.  The value can be computed as `ascender+descender+line_gap'.

    // FreeType doesn't provide the capline - for now just use the ascent.
    double em_square_size = font->m_units_per_EM;
    double font_ascent    = font->m_ascender * actual_height / em_square_size;
    double font_descent   = font->m_descender * actual_height / em_square_size;
    double font_capline   = font_ascent;

    RS_VAlignment vAlign = tdef.valign();
    switch (vAlign)
    {
    case RS_VAlignment_Descent:
        // must also account for the total number of lines
        offsetY = font_descent - line_height*(numLines - 1);
        break;

    case RS_VAlignment_Base:
        // must also account for the total number of lines
        offsetY = - line_height*(numLines - 1);
        break;

    case RS_VAlignment_Half:
        // must also account for the total number of lines
        offsetY = 0.5*(font_capline - line_height*(numLines - 1));
        break;

    case RS_VAlignment_Cap:
        offsetY = font_capline;
        break;

    case RS_VAlignment_Ascent:
        offsetY = font_ascent;
        break;
    }

    return offsetY;
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererBase::DeviceToMappingBounds(RS_Bounds& b)
{
    b.minx = m_renderer->_ITXD(b.minx);
    b.maxx = m_renderer->_ITXD(b.maxx);

    //y goes down in screen space, up in mapping space -- so reverse them
    double swap = b.miny;
    b.miny = m_renderer->_ITYD(b.maxy);
    b.maxy = m_renderer->_ITYD(swap);
}


//////////////////////////////////////////////////////////////////////////////
//computes axis aligned bounds of a point set
void LabelRendererBase::ComputeBounds(RS_F_Point* RESTRICT pts, int npts, RS_Bounds& b)
{
    if (!npts) return;

    b.minx = b.maxx = pts[0].x;
    b.miny = b.maxy = pts[0].y;

    for (int i=1; i<npts; i++)
        b.add_point(pts[i]);
}


//////////////////////////////////////////////////////////////////////////////
//Applies a given angle to an axis aligned bounding box.
//Rotation point is lower left
void LabelRendererBase::RotatedBounds(double x, double y, double width, double height, double angle_cw_rad, RS_F_Point* b)
{
#ifdef HEIDI
    //HEIDI has y coordinate going up

    double sina = sin(-angle_ccw_rad);
    double cosa = cos(-angle_ccw_rad);

    //apply rotation
    //taking into account that y goes down (so subtract instead of adding for y)
    b[0].x = x;
    b[0].y = y;
    b[1].x = b[0].x +  width * cosa;
    b[1].y = b[0].y + (width * sina);
    b[2].x = b[0].x +  width * cosa - height * sina;
    b[2].y = b[0].y + (width * sina + height * cosa);
    b[3].x = b[0].x                 - height * sina;
    b[3].y = b[0].y + (               height * cosa);
#else
    //y goes down case

    double sina = sin(angle_cw_rad);
    double cosa = cos(angle_cw_rad);

    //apply rotation
    //taking into account that y goes down (so subtract instead of adding for y)
    b[0].x = x;
    b[0].y = y;
    b[1].x = b[0].x +  width * cosa;
    b[1].y = b[0].y - (width * sina);
    b[2].x = b[0].x +  width * cosa - height * sina;
    b[2].y = b[0].y - (width * sina + height * cosa);
    b[3].x = b[0].x                 - height * sina;
    b[3].y = b[0].y - (               height * cosa);
#endif
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererBase::CloseEnough(RS_F_Point& p1, RS_F_Point& p2)
{
    double delta = fabs(p2.y - p1.y) + fabs(p2.x - p1.x);
    return (delta <= 2.0); //2 pixels is close enough
}
