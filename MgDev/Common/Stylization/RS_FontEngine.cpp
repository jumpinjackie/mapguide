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
#include "RendererStyles.h"
#include "FontManager.h"
#include "RS_FontEngine.h"
#include "Renderer.h"
#include "SE_Renderer.h"
#include "SE_Matrix.h"

#define ROUND(x) (int)((x) + 0.5)

//////////////////////////////////////////////////////////////////////////////
RS_FontEngine::RS_FontEngine()
{
}


//////////////////////////////////////////////////////////////////////////////
RS_FontEngine::~RS_FontEngine()
{
}


//////////////////////////////////////////////////////////////////////////////
void RS_FontEngine::InitFontEngine(Renderer* renderer, SE_Renderer* serenderer)
{
    m_renderer = renderer;
    m_serenderer = serenderer;

    //we need the renderer to be an SE_Renderer in order to draw screen space geometry (like underline)
    _ASSERT(m_serenderer);

    SE_Matrix xform;
    m_serenderer->GetWorldToScreenTransform(xform);

    m_bYup = xform.y1 > 0;
}


//////////////////////////////////////////////////////////////////////////////
const RS_Font* RS_FontEngine::FindFont(RS_FontDef& def)
{
    return FontManager::Instance()->FindFont(def.name().c_str(),
                          (def.style() & RS_FontStyle_Bold) != 0,
                          (def.style() & RS_FontStyle_Italic) != 0);
}


//////////////////////////////////////////////////////////////////////////////
bool RS_FontEngine::GetTextMetrics(const RS_String& s, RS_TextDef& tdef, RS_TextMetrics& ret, bool bPathText)
{
    // font matching
    const RS_Font* font = FindFont(tdef.font());

    if (!font)
        return false;

    //determine pixel space font height
    double hgt = MetersToPixels(tdef.font().units(), tdef.font().height());

    //-------------------------------------------------------
    // text extent and alignment computation
    //-------------------------------------------------------

    //get overall extent and char spacing
    RS_F_Point fpts[4];

    if (bPathText)
    {
        size_t len = s.length();
        float* spacing = (float*)alloca(len * 2 * sizeof(float));
        MeasureString(s, hgt, font, 0.0, fpts, spacing);
        ret.char_advances.reserve(len);
        for (size_t i=0; i<len; i++)
            ret.char_advances.push_back(spacing[i]);
        ret.text_width = fabs(fpts[1].x - fpts[0].x);
        ret.text_height = fabs(fpts[2].y - fpts[0].y);
    }
    else
    {
        //-------------------------------------------------------
        // break up the string up into individual lines
        //-------------------------------------------------------

        // font height gives the distance from one baseline to the next
        // increase this by a factor of 1.05 (what GD uses)
        double font_height = font->m_height * hgt / font->m_units_per_EM;
        double line_height = 1.05 * font_height;

        // make a temporary copy
        size_t len = s.length();
        wchar_t* cpy = (wchar_t*)alloca((len + 1) * sizeof(wchar_t));
        wcscpy(cpy, s.c_str());

        // break it up
        std::vector<wchar_t*> line_breaks;
        size_t num_lines = SplitLabel(cpy, line_breaks);

        //if there were line breaks, remember each separate line in the metrics
        if (num_lines > 1)
        {
            ret.line_breaks.reserve(num_lines);
            for (size_t i=0; i<num_lines; i++)
                ret.line_breaks.push_back(line_breaks[i]);
        }

        //we will store the h and v offset for each line in the char pos array
        ret.line_pos.reserve(num_lines);

        // base vertical offset is the same for each line of text
        double vAlignBaseOffset = GetVerticalAlignmentOffset(tdef.valign(), font, hgt, line_height, num_lines);

        ret.line_pos.resize(num_lines);

        for (size_t k=0; k<num_lines; ++k)
        {
            wchar_t* txt = line_breaks[k];

            // get the unrotated extent of this sub-string
            MeasureString(txt, hgt, font, 0.0, fpts, NULL);

            // horizontal offset depends on the sub-string width, while
            // vertical offset depends on the line of text
            ret.line_pos[k].hOffset = GetHorizontalAlignmentOffset(tdef.halign(), fpts);
            if (m_bYup)
                ret.line_pos[k].vOffset = vAlignBaseOffset - k*line_height;
            else
                ret.line_pos[k].vOffset = vAlignBaseOffset + k*line_height;

            //remember unrotated extent of text
            for (int i=0; i<4; i++)
            {
                ret.line_pos[k].ext[i].x = fpts[i].x + ret.line_pos[k].hOffset;
                ret.line_pos[k].ext[i].y = fpts[i].y + ret.line_pos[k].vOffset;
            }
        }
    }

    //the width and height of this particular string
    ret.font = font;
    ret.font_height = hgt;
    ret.text = s;

    return true;
}


//////////////////////////////////////////////////////////////////////////////
// Finds occurences of line breaks and adds the start pointer of each
// line to the supplied array.  Line breaks can be any of the following:
//   "\\n"                      // sequence currently used by MG OS / Enterprise
//   \n\r  (char 13 + char 10)  // common in RDBMS like Oracle
//   \r\n  (char 10 + char 13)
//   \n    (char 13)            // used by MG 6.5
//   \r    (char 10)            // common in Linux
size_t RS_FontEngine::SplitLabel(wchar_t* label, std::vector<wchar_t*>& line_breaks)
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
//Computes character placement for a text stored in TextMetrics
//The characters are placed along the given path.
//This function assumes that GetTextMetrics has been used to initialize
//the incoming TextMetrics structure.
//This function assumes that the CharPos array passed in has enough space
//to contain the computed position for each character along the path
//This function may also modify the TextMetrics with updated font and text sizes
//if it chooses to scale the font to make it better fit the given path
bool RS_FontEngine::LayoutPathText(RS_TextMetrics& tm,
                        const RS_F_Point* pts, int npts, double* seglens,
                        double param_position, RS_VAlignment valign, int /*layout_option*/)
{
    int numchars = (int)tm.text.length();
    tm.char_pos.reserve(numchars);

    if (!seglens)
    {
        //If they were not passed in,
        //find length of each segment in the screen space path
        //we will use it to position characters along the curve
        //It is recommended that the caller passes in a precomputed array
        //if they will be laying out multiple labels on the same curve
        _ASSERT(npts < 16384);
        seglens = (double*)alloca(sizeof(double) * npts);
        seglens[0] = 0.0;

        for (int i=1; i<npts; i++)
        {
            double dx = pts[i].x - pts[i-1].x;
            double dy = pts[i].y - pts[i-1].y;
            seglens[i] = seglens[i-1] + sqrt(dx*dx + dy*dy);
        }
    }

    //determine in which direction we should follow the polyline
    //so that the label points up more likely than down
    //find the length of polyline that requires inverting versus
    //length that does not and see which one is bigger
    //TODO: this loop should really be performed on the piece of the
    //polyline where we will be labeling since otherwise features like
    //circles can throw it off and so a label may still end up upside down
    double inverted_len = 0.0;

    for (int m=0; m<npts - 1; m++)
    {
        RS_F_Point p0 = pts[m];
        RS_F_Point p1 = pts[m+1];

        double angle = -atan2(p1.y - p0.y, p1.x - p0.x);

        if (angle > M_PI/2.0 || angle < -M_PI/2.0)
            inverted_len += seglens[m+1] - seglens[m];
        else
            inverted_len -= seglens[m+1] - seglens[m];
    }

    bool reverse = (inverted_len > 0.0);

    double pathlen = seglens[npts - 1];

    //compute font height that better fits the geometry
    //but limit scaling to be in the range [0.5 - 1.0]
    double font_scale = rs_min(1.0, rs_max(0.5, pathlen / (1.1 * tm.text_width)));

    //scale all things we measured by the font scaling factor determined
    //baed on the path length
    if (font_scale != 1.0)
    {
        //don't bother measuring the string with the new height,
        //scaling should work almost as good
        tm.font_height *= font_scale;
        tm.text_width *= font_scale;
        tm.text_height *= font_scale;

        for (int i=0; i<numchars; i++)
            tm.char_advances[i] *= font_scale;
    }

    //distance of current character along current segment
    //initialize to start position, based on given parametric position
    //along the polyline
    double dist_along_segment = param_position * pathlen - 0.5 * tm.text_width;

    //j indicates index of segment we are on with current character
    int j;

    //segment for current character
    RS_F_Point start;
    RS_F_Point end;

    if (reverse)
    {
        //case where we want to walk along the polyline
        //in reverse

        //compute starting segment
        for (j=npts-2; j>=0; j--)
            if (dist_along_segment < pathlen - seglens[j])
                break;

        if (j < 0) j=0;

        start = pts[j+1];
        end = pts[j];
        dist_along_segment -= pathlen - seglens[j+1];
    }
    else
    {
        //case where we follow the polyline forwards

        //compute starting segment
        for (j=0; j<npts-1; j++)
            if (dist_along_segment < seglens[j+1])
                break;

        if (j >= npts-1)
            j = npts-2;

        start = pts[j];
        end = pts[j+1];
        dist_along_segment -= seglens[j];
    }

    //length of current segment
    double seg_len = seglens[j+1] - seglens[j];

    //position of current character relative to the left end of the string
    double char_pos = 0;

    //The premise here is that we will compute three positions along the path
    //for each character - one for the left corner, one for the right
    //and one for the centerpoint
    //We will compute a tangent vector based on the left and right points
    //and then position the character relative to the point computed for its
    //center with the normal comuted from the left and right points
    //Note that the end position of one character equals the start position
    //of the next, so in fact we need to compute 2 * n + 1 points
    RS_F_Point* positions = (RS_F_Point*)alloca(sizeof(RS_F_Point) * (numchars * 2 + 1));

    for (int i=0; i <= numchars * 2; i++)
    {
        //check if we need to move to next segment
        while (dist_along_segment > seg_len)
        {
            if (reverse)
            {
                //case where we go in reverse direction of the path
                if (j > 0)
                {
                    j--;
                    dist_along_segment -= seg_len;
                    seg_len = seglens[j+1] - seglens[j];
                    start = pts[j+1];
                    end = pts[j];
                }
                else
                    break;
            }
            else
            {
                //case where we go forward along the path
                if (j < npts - 2)
                {
                    j++;
                    dist_along_segment -= seg_len;
                    seg_len = seglens[j+1] - seglens[j];
                    start = pts[j];
                    end = pts[j+1];
                }
                else
                    break;
            }
        }

        //compute position along current segment using weighted
        //normalized end-start vector
        double weight = dist_along_segment / seg_len;
        double dx = end.x - start.x;
        double dy = end.y - start.y;

        positions[i].x = start.x + dx * weight;
        positions[i].y = start.y + dy * weight;

        //kerned width of current character
        int index = i / 2; //which character are we working with -- 3 iterations for each one
        double char_width = (index >= numchars - 1) ? tm.text_width - char_pos : tm.char_advances[index];

        //advance cursor by half the width of the current character
        dist_along_segment += char_width * 0.5;
        char_pos += char_width * 0.5;
    }

    char_pos = 0.0;

    tm.char_pos.resize(numchars);

    //now compute character placement and angles based on the positioning points
    for (int i=0; i<numchars; i++)
    {
        //find angle based on left and right of character
        double anglerad = -atan2(positions[2*i+2].y - positions[2*i].y,positions[2*i+2].x - positions[2*i].x);

        //kerned width of current character
        double char_width = (i == numchars - 1) ? tm.text_width - char_pos : tm.char_advances[i];
        double char_width_2 = 0.5 * char_width;

        //and now find a lower left insertion point
        //based on the midpoint anchor and the angle
        tm.char_pos[i].x = positions[2*i+1].x - cos(anglerad) * char_width_2;
        tm.char_pos[i].y = positions[2*i+1].y + sin(anglerad) * char_width_2; //y down means + sin
        tm.char_pos[i].anglerad = anglerad;

        char_pos += char_width;
    }

    
    //get vertical alignment delta
    //return value will be positive if y goes down and negative if y goes up 
    //i.e. it's the offset we need to apply to y in the coordinate system of the 
    //renderer
    double voffset = GetVerticalAlignmentOffset(valign, tm.font, tm.font_height, tm.font_height * 1.05, 1);

    //apply vertical alignment to character position
    //horizontal alignment is ignored in this case
    for (int i=0; i<numchars; i++)
    {
        // add in the rotated vertical alignment contribution
        double angle = tm.char_pos[i].anglerad;

        tm.char_pos[i].x += voffset * sin(angle);
        tm.char_pos[i].y += voffset * cos(angle);
    }
    
    return true;
}


//////////////////////////////////////////////////////////////////////////////
void RS_FontEngine::DrawBlockText(RS_TextMetrics& tm, RS_TextDef& tdef, double insx, double insy)
{
    // positive rotation in the RS_TextDef is assumed to always be a radian CCW rotation...
    double rotation = tdef.rotation() * M_PI180;

    double cos_a = cos(rotation);
    double sin_a = sin(rotation);
    if (m_bYup)
        sin_a = -sin_a;

    // get the overall unrotated bounds
    RS_Bounds b(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);

    for (size_t i=0; i<tm.line_pos.size(); i++)
    {
        b.add_point(tm.line_pos[i].ext[0]);
        b.add_point(tm.line_pos[i].ext[2]);
    }

    RS_F_Point fpts[4];
    b.get_points(fpts);

    // draw the opaque background first, if requested
    if (tdef.textbg() == RS_TextBackground_Opaque)
    {
        // rotate and translate it
        for (int j=0; j<4; ++j)
        {
            double tmpX = fpts[j].x;
            double tmpY = fpts[j].y;
            fpts[j].x = insx + tmpX * cos_a + tmpY * sin_a;
            fpts[j].y = insy - tmpX * sin_a + tmpY * cos_a;
        }

        // draw a filled rectangle
        LineBuffer lb(5);
        lb.MoveTo(fpts[0].x, fpts[0].y);
        lb.LineTo(fpts[1].x, fpts[1].y);
        lb.LineTo(fpts[2].x, fpts[2].y);
        lb.LineTo(fpts[3].x, fpts[3].y);
        lb.Close();

        m_serenderer->DrawScreenPolygon(&lb, NULL, tdef.bgcolor().argb());
    }

    for (size_t k=0; k<tm.line_pos.size(); ++k)
    {
        const RS_String* txt;

        if (tm.line_pos.size() == 1)
            txt = &tm.text;
        else
            txt = &tm.line_breaks.at(k);

        LinePos& pos = tm.line_pos[k];

        // add the rotated original offset for this line to the insertion point
        // to get the actual draw point
        double insX = insx + pos.hOffset * cos_a + pos.vOffset * sin_a;
        double insY = insy - pos.hOffset * sin_a + pos.vOffset * cos_a;

        int posx = (int)floor(insX + 0.5);
        int posy = (int)floor(insY + 0.5);

        // render the ghosted text, if requested
        if (tdef.textbg() == RS_TextBackground_Ghosted)
        {
            DrawString(*txt, posx-1, posy, tm.font_height, tm.font, tdef.bgcolor(), rotation);
            DrawString(*txt, posx+1, posy, tm.font_height, tm.font, tdef.bgcolor(), rotation);
            DrawString(*txt, posx, posy-1, tm.font_height, tm.font, tdef.bgcolor(), rotation);
            DrawString(*txt, posx, posy+1, tm.font_height, tm.font, tdef.bgcolor(), rotation);
        }

        // render the primary text
        DrawString(*txt, posx, posy, tm.font_height, tm.font, tdef.color(), rotation);

        // render the underline, if requested
        if (tdef.font().style() & RS_FontStyle_Underline)
        {
            // estimate underline line width as % of font height
            double line_width = (double)tm.font->m_underline_thickness * tm.font_height / (double)tm.font->m_units_per_EM;
            double line_pos = - (double)tm.font->m_underline_position * tm.font_height / (double)tm.font->m_units_per_EM;

            // the line's start point is the insertion point, but shifted vertically by line_pos
            double x0 = insX + line_pos * sin_a;
            double y0 = insY + line_pos * cos_a;


            // the end point is a horizontal shift by the text width
            double textwidth = pos.ext[1].x - pos.ext[0].x;
            double x1 = x0 + textwidth * cos_a;
            double y1 = y0 - textwidth * sin_a;

            // draw the thick line
            LineBuffer lb(2);
            lb.MoveTo(x0, y0);
            lb.LineTo(x1, y1);

            m_serenderer->DrawScreenPolyline(&lb, NULL, tdef.color().argb(), line_width);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
//Draws a text which has been previously laid out using LayoutPathText
//The tdef argument is only used to get the underline option and the text colors
//Everything else is stored in the TextMetrics context structure and the CharPos
//array, which were previously computed by LayoutPathText
void RS_FontEngine::DrawPathText(RS_TextMetrics& tm, RS_TextDef& tdef)
{
    size_t numchars = tm.text.length();

    //draw the characters, each in its computed position
    RS_String c;
    for (size_t i=0; i<numchars; ++i)
    {
        c = tm.text[i];

        //compute screen position and round
        int posx = ROUND(tm.char_pos[i].x);
        int posy = ROUND(tm.char_pos[i].y);
        double anglerad = tm.char_pos[i].anglerad;

        if (tdef.textbg() == RS_TextBackground_Ghosted)
        {
            DrawString(c, posx-1, posy, tm.font_height, tm.font, tdef.bgcolor(), anglerad);
            DrawString(c, posx+1, posy, tm.font_height, tm.font, tdef.bgcolor(), anglerad);
            DrawString(c, posx, posy-1, tm.font_height, tm.font, tdef.bgcolor(), anglerad);
            DrawString(c, posx, posy+1, tm.font_height, tm.font, tdef.bgcolor(), anglerad);
        }

        DrawString(c, posx, posy, tm.font_height, tm.font, tdef.color(), anglerad);
    }

    //render underline
    if (tdef.font().style() & RS_FontStyle_Underline)
    {
        //estimate underline line width as % of font height
        double line_width = (double)tm.font->m_underline_thickness * tm.font_height / (double)tm.font->m_units_per_EM;
        //underline position w.r.t. baseline. Invert y while at it
        double line_pos = - (double)tm.font->m_underline_position * tm.font_height / (double)tm.font->m_units_per_EM;

        double total_advance = 0.0;

        //used to keep track of last position of the underline, which is
        //drawn piecewise for each character
        double last_x = tm.char_pos[0].x + sin(tm.char_pos[0].anglerad) * line_pos;
        double last_y = tm.char_pos[0].y + cos(tm.char_pos[0].anglerad) * line_pos;
        double sx, sy, ex, ey;

        //draw the characters, each in its computed position
        for (size_t i=0; i<numchars; i++)
        {
            //width of character - not really exact width since
            //it takes kerning into account, but should be good enough
            //we could measure each character separately but that seems like
            //too many calls to FreeType
            double advance = (i == numchars-1)? tm.text_width - total_advance : tm.char_advances[i];
            total_advance += advance;

            sx = last_x;
            sy = last_y;

            if (i == numchars - 1)
            {
                //estimate bottom right corner of last character
                double eex = tm.char_pos[i].x + cos(tm.char_pos[i].anglerad) * advance;
                double eey = tm.char_pos[i].y - sin(tm.char_pos[i].anglerad) * advance;

                //now take into account underline offset
                ex = eex + sin(tm.char_pos[i].anglerad) * line_pos;
                ey = eey + cos(tm.char_pos[i].anglerad) * line_pos;
            }
            else
            {
                //move position by underline offset
                ex = tm.char_pos[i+1].x + sin(tm.char_pos[i+1].anglerad) * line_pos;
                ey = tm.char_pos[i+1].y + cos(tm.char_pos[i+1].anglerad) * line_pos;
            }

            LineBuffer lb(2);
            lb.MoveTo(sx, sy);
            lb.LineTo(ex, ey);

            m_serenderer->DrawScreenPolyline(&lb, NULL, tdef.color().argb(), line_width);

            last_x = ex;
            last_y = ey;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// Computes the Y offset that must be applied to the unrotated text to
// obtain the specified vertical alignment.  Positive Y points down.
double RS_FontEngine::GetVerticalAlignmentOffset(RS_VAlignment vAlign, const RS_Font* font,
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

    if (font->m_capheight == 0)
    {
        //happy hack to get the capline since FreeType doesn't know it

        RS_F_Point fpts[4];
        MeasureString(L"A", em_square_size, font, 0.0, fpts, NULL);
        
        //set it on the font, so that we don't have to measure it all the time 
        ((RS_Font*)font)->m_capheight = (short)fabs(fpts[2].y - fpts[1].y);
    }

    double font_capline = font->m_capheight * actual_height / em_square_size;

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

    if (m_bYup)
        offsetY = -offsetY; //TODO: do we need to mult this by numLines?

    return offsetY;
}


//////////////////////////////////////////////////////////////////////////////
// Computes the X offset that must be applied to the unrotated text to
// obtain the specified horizontal alignment.
double RS_FontEngine::GetHorizontalAlignmentOffset(RS_HAlignment hAlign, RS_F_Point* extent)
{
    double offsetX = 0.0;

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


//-----------------------------------------------------------------------------
//scale an input number in meters to a mapping
//space number given a device or mapping space unit.
//-----------------------------------------------------------------------------
double RS_FontEngine::MeterToMapSize(RS_Units unit, double number)
{
    double scale_factor;

    if (unit == RS_Units_Device) // in meters, fixed size
        scale_factor = m_renderer->GetMapScale() / m_renderer->GetMetersPerUnit();
    else
        scale_factor = 1.0 / m_renderer->GetMetersPerUnit();

    return number * scale_factor;
}

//-----------------------------------------------------------------------------
//scale an input number in meters to pixel
//space number given a device or mapping space unit.
//-----------------------------------------------------------------------------
double RS_FontEngine::MetersToPixels(RS_Units unit, double number)
{
    double scale_factor;

    double m2px = m_renderer->GetDpi() * (100 / 2.54);

    if (unit == RS_Units_Device)
        scale_factor = m2px; //device units simply returns scale to convert meters to pixels
    else
        scale_factor = m2px / m_renderer->GetMapScale(); //for mapping space we also take map scale into account

    return number * scale_factor;
}
