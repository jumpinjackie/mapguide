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
#include "RS_Font.h"
#include "RS_FontEngine.h"
#include "RichTextEngine.h"
#include "SE_Renderer.h"

using namespace RichText::ATOM;

#define ROUND(x) (int)(floor(x+0.5))

//////////////////////////////////////////////////////////////////////////////
RS_TextMetrics::~RS_TextMetrics()
{
    const RichText::ATOM::Particle* pParticle;
    const RichText::ATOM::Particle* pNext;
    size_t numLists = format_changes.size();
    for (size_t i=0; i<numLists; ++i)
    {
        pParticle = format_changes[i];
        while (pParticle)
        {
            pNext = pParticle->Next();
            delete pParticle;
            pParticle = pNext;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
RS_FontEngine::RS_FontEngine()
{
    m_pSERenderer = NULL;
}


//////////////////////////////////////////////////////////////////////////////
RS_FontEngine::~RS_FontEngine()
{
}


//////////////////////////////////////////////////////////////////////////////
void RS_FontEngine::InitFontEngine(SE_Renderer* pSERenderer)
{
    m_pSERenderer = pSERenderer;
}


//////////////////////////////////////////////////////////////////////////////
bool RS_FontEngine::GetTextMetrics(const RS_String& s, RS_TextDef& tdef, RS_TextMetrics& ret, bool bPathText)
{
    // font matching
    const RS_Font* font = FindFont(tdef.font());

    if (!font)
        return false;

    // determine font height in renderer screen space
    double hgt = MetersToPixels(tdef.font().units(), tdef.font().height());

    // store the font and height of this particular string
    ret.font = font;
    ret.font_height = hgt;
    ret.text = s;

    //-------------------------------------------------------
    // text extent and alignment computation
    //-------------------------------------------------------

    size_t len = s.length();

    // get overall extent and char spacing
    RS_F_Point fpts[4];

    if (bPathText)
    {
        float* spacing = (float*)alloca(len * sizeof(float));
        MeasureString(s, hgt, font, 0.0, fpts, spacing);

        ret.text_width  = fabs(fpts[1].x - fpts[0].x);
        ret.text_height = fabs(fpts[2].y - fpts[0].y);

        // make the sum of the spacings equal to the string width - the
        // spacings are then essentially the character widths
        float sum = 0.0f;
        for (size_t i=0; i<len; ++i)
            sum += spacing[i];

        float factor = (float)ret.text_width / sum;

        ret.char_advances.reserve(len);
        for (size_t i=0; i<len; ++i)
            ret.char_advances.push_back(factor*spacing[i]);
    }

    // Is this formatted text?
    else
    if (!tdef.markup().empty() && (tdef.markup().compare(L"Plain") != 0))
    {
        RichTextEngine richTextEngine(m_pSERenderer, this, &tdef);
        return richTextEngine.Parse(s, &ret);
    }

    else
    {
        //-------------------------------------------------------
        // break up the string into individual lines
        //-------------------------------------------------------

        // font height gives the distance from one baseline to the next
        // increase this by a factor of 1.05 (what GD uses)
        double font_height = font->m_height * hgt / font->m_units_per_EM;
        double line_height = 1.05 * font_height;

        // make a temporary copy since SplitLabel will modify it
        wchar_t* cpy = (wchar_t*)alloca((len + 1) * sizeof(wchar_t));
        wcscpy(cpy, s.c_str());

        // break it up
        std::vector<wchar_t*> line_breaks;
        size_t num_lines = SplitLabel(cpy, line_breaks);

        // if there were line breaks, remember each separate line in the metrics
        if (num_lines > 1)
        {
            ret.line_breaks.reserve(num_lines);
            for (size_t i=0; i<num_lines; ++i)
                ret.line_breaks.push_back(line_breaks[i]);
        }

        // we will store the h and v offset for each line in the line pos array
        ret.line_pos.reserve(num_lines);

        // initialize the line pos array with empty elements
        ret.line_pos.resize(num_lines);

        // base vertical offset is the same for each line of text
        double vAlignBaseOffset = GetVerticalAlignmentOffset(tdef.valign(), font, hgt, line_height, num_lines);

        // account for Y direction now that we have the base offset
        if (!m_pSERenderer->YPointsUp())
            line_height = -line_height;

        for (size_t k=0; k<num_lines; ++k)
        {
            wchar_t* txt = line_breaks[k];

            // get the unrotated extent of this sub-string
            MeasureString(txt, hgt, font, 0.0, fpts, NULL);

            // horizontal offset depends on the sub-string width, while
            // vertical offset depends on the line of text
            ret.line_pos[k].hOffset = GetHorizontalAlignmentOffset(tdef.halign(), fpts);
            ret.line_pos[k].vOffset = vAlignBaseOffset - k*line_height;

            // remember unrotated extent of text
            for (int i=0; i<4; ++i)
            {
                ret.line_pos[k].ext[i].x = fpts[i].x + ret.line_pos[k].hOffset;
                ret.line_pos[k].ext[i].y = fpts[i].y + ret.line_pos[k].vOffset;
            }
        }
    }

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
//
// Note that the input string will be modified.
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
// Computes the character placement for a text stored in TextMetrics.  The
// characters are placed along the given path.  This method assumes that
// GetTextMetrics has been used to initialize the incoming TextMetrics
// structure, and that the CharPos array passed in has enough space to
// contain the computed position for each character along the path.  This
// method may also modify the TextMetrics with updated font and text sizes
// if it chooses to scale the font to make it better fit the given path.
bool RS_FontEngine::LayoutPathText(RS_TextMetrics& tm,
                        const RS_F_Point* pts, int npts, double* segpos,
                        double param_position, RS_VAlignment valign, int /*layout_option*/)
{
    // set a limit on the number of path segments
    _ASSERT(npts < MAX_PATH_SEGMENTS);
    if (npts >= MAX_PATH_SEGMENTS)
        return false;

    int numchars = (int)tm.text.length();
    tm.char_pos.reserve(numchars);

    if (!segpos)
    {
        // If the segments were not passed in, find their positions from the
        // screen space path.  We use these to position characters along the
        // path.  It is recommended that the caller passes in a precomputed
        // array if they will be laying out multiple labels on the same path.
        segpos = (double*)alloca(sizeof(double) * npts);
        segpos[0] = 0.0;

        for (int i=1; i<npts; ++i)
        {
            double dx = pts[i].x - pts[i-1].x;
            double dy = pts[i].y - pts[i-1].y;
            segpos[i] = segpos[i-1] + sqrt(dx*dx + dy*dy);
        }
    }

    double pathlen = segpos[npts-1];

    // compute a font height that better fits the geometry, but limit
    // scaling to be in the range [0.5 - 1.0]
    double font_scale = rs_min(1.0, rs_max(0.5, pathlen / (1.1 * tm.text_width)));

    // scale all things we measured by the font scaling factor determined
    // based on the path length
    if (font_scale != 1.0)
    {
        // don't bother measuring the string with the new height - scaling
        // should work almost as good
        tm.font_height *= font_scale;
        tm.text_width  *= font_scale;
        tm.text_height *= font_scale;

        for (int i=0; i<numchars; ++i)
            tm.char_advances[i] *= (float)font_scale;
    }

    // determine how far along the path the label begins and ends
    double startLabelPosition = param_position * pathlen - 0.5 * tm.text_width;
    double   endLabelPosition = param_position * pathlen + 0.5 * tm.text_width;

    // determine the segment containing the start of the label
    int labelStartIndex = 0;
    for (int segment=0; segment<npts-1; ++segment)
    {
        // skip zero-length segments
        if (segpos[segment+1] == segpos[segment])
            continue;

        if (segpos[segment+1] > startLabelPosition)
        {
            labelStartIndex = segment;
            break;
        }
    }

    // determine the segment containing the end of the label
    int labelEndIndex = labelStartIndex;
    for (int segment=labelStartIndex; segment<npts-1; ++segment)
    {
        // skip zero-length segments
        if (segpos[segment+1] == segpos[segment])
            continue;

        if (segpos[segment+1] > endLabelPosition)
        {
            labelEndIndex = segment;
            break;
        }
    }

    // determine in which direction we should follow the polyline
    // so that more of the label is right-side-up than inverted
    double inverted_len = 0.0;
    for (int m=labelStartIndex; m<=labelEndIndex; ++m)
    {
        // determine how much of the label is present in this segment
        double labelLengthInSegment = segpos[m+1] - segpos[m];

        // skip zero-length segments
        if (labelLengthInSegment == 0.0)
            continue;

        if (m == labelStartIndex)
        {
            // subtract the length of this segment that comes before the label
            labelLengthInSegment -= (startLabelPosition - segpos[m]);
        }
        if (m == labelEndIndex)
        {
            // subtract the length of this segment that comes after the label
            labelLengthInSegment -= (segpos[m+1] - endLabelPosition);
        }

        // get the vertical component of this segment's normal vector
        double nx = - (pts[m+1].y - pts[m].y);
        double ny =   (pts[m+1].x - pts[m].x);

        // if the normal vector points up then this segment would not have
        // an inverted label
        if (ScreenVectorPointsUp(nx, ny))
            inverted_len -= labelLengthInSegment;
        else
            inverted_len += labelLengthInSegment;
    }

    bool reverse = (inverted_len > 0.0);

    // j indicates the segment index we are on with the current character
    int j;

    // segment for current character
    RS_F_Point start;
    RS_F_Point end;

    double dist_along_segment;
    if (reverse)
    {
        // case where we follow the path in reverse
        j = labelEndIndex;

        start = pts[j+1];
        end = pts[j];
        dist_along_segment = segpos[j+1] - endLabelPosition;
    }
    else
    {
        // case where we follow the path forwards
        j = labelStartIndex;

        start = pts[j];
        end = pts[j+1];
        dist_along_segment = startLabelPosition - segpos[j];
    }

    // length of current segment
    double seg_len = segpos[j+1] - segpos[j];

    // The premise here is that we will compute three positions along the path
    // for each character - one for the left corner, one for the right, and one
    // for the center point.  We will compute a tangent vector based on the left
    // and right points and then position the character relative to the point
    // computed for its center with the normal computed from the left and right
    // points.  Note that the end position of one character equals the start
    // position of the next, so in fact we need to compute 2 * n + 1 points.
    RS_F_Point* positions = (RS_F_Point*)alloca(sizeof(RS_F_Point) * (2*numchars + 1));

    for (int i=0; i<=2*numchars; ++i)
    {
        // check if we need to move to next segment
        while (dist_along_segment > seg_len)
        {
            if (reverse)
            {
                // case where we follow the path in reverse
                if (j > 0)
                {
                    j--;

                    // skip zero-length segments
                    if (segpos[j+1] != segpos[j])
                    {
                        dist_along_segment -= seg_len;
                        seg_len = segpos[j+1] - segpos[j];
                        start = pts[j+1];
                        end = pts[j];
                    }
                }
                else
                    break;
            }
            else
            {
                // case where we follow the path forwards
                if (j < npts-2)
                {
                    j++;

                    // skip zero-length segments
                    if (segpos[j+1] != segpos[j])
                    {
                        dist_along_segment -= seg_len;
                        seg_len = segpos[j+1] - segpos[j];
                        start = pts[j];
                        end = pts[j+1];
                    }
                }
                else
                    break;
            }
        }

        // compute position along current segment using weighted
        // normalized end-start vector
        double weight = dist_along_segment / seg_len;
        double dx = end.x - start.x;
        double dy = end.y - start.y;

        positions[i].x = start.x + dx * weight;
        positions[i].y = start.y + dy * weight;

        // get kerned width of current character
        int index = i / 2; // which character are we working with -- 3 iterations for each one
        double char_width = (index >= numchars)? 0.0 : tm.char_advances[index];

        // advance cursor by half the width of the current character
        dist_along_segment += char_width * 0.5;
    }

    // Get vertical alignment delta.  Return value will be positive if y goes down
    // and negative if y goes up (i.e. it's the offset we need to apply to y in the
    // coordinate system of the renderer).
    double voffset = GetVerticalAlignmentOffset(valign, tm.font, tm.font_height, tm.font_height * 1.05, 1);

    // now compute character placements and angles based on the positioning points
    tm.char_pos.resize(numchars);
    for (int i=0; i<numchars; ++i)
    {
        double dx = positions[2*i+2].x - positions[2*i].x;
        double dy = positions[2*i+2].y - positions[2*i].y;
        double len = sqrt(dx*dx + dy*dy);

        // find angle based on left and right of character
        tm.char_pos[i].anglerad = atan2(m_pSERenderer->YPointsUp()? dy : -dy, dx);

        // compute a lower left insertion point based on the midpoint
        // anchor and the angle
        double offset = 0.5 * tm.char_advances[i] / len;
        tm.char_pos[i].x = positions[2*i+1].x - offset * dx;
        tm.char_pos[i].y = positions[2*i+1].y - offset * dy;

        // finally, add in the rotated vertical alignment contribution
        // (horizontal alignment is ignored in this case)
        offset = voffset / len;
        tm.char_pos[i].x -= offset * dy;
        tm.char_pos[i].y += offset * dx;
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool RS_FontEngine::ScreenVectorPointsUp(double /*x*/, double y)
{
    // default assumes screen space is not rotated
    return (y > 0.0);
}


//////////////////////////////////////////////////////////////////////////////
void RS_FontEngine::DrawBlockText(RS_TextMetrics& tm, RS_TextDef& tdef, double insx, double insy)
{
    double angleRad = tdef.rotation() * M_PI180;

    // precompute these - these are in renderer space, hence the check for YPointsUp with the sine
    double cos_a = cos(angleRad);
    double sin_a = sin(m_pSERenderer->YPointsUp()? angleRad : -angleRad);

    // draw the opaque / framed background first, if requested
    bool bFramed = ((tdef.textbg() & RS_TextBackground_Framed) != 0);
    bool bOpaque = ((tdef.textbg() & RS_TextBackground_Opaque) != 0);
    if (bFramed || bOpaque)
    {
        double em_square_size = tm.font->m_units_per_EM;
        double font_ascent    = tm.font->m_ascender * tm.font_height / em_square_size;
        double font_descent   = tm.font->m_descender * tm.font_height / em_square_size;

        // account for Y direction
        if (!m_pSERenderer->YPointsUp())
        {
            font_ascent  = -font_ascent;
            font_descent = -font_descent;
        }

        // get the overall unrotated bounds
        RS_F_Point pt;
        RS_Bounds b(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
        for (size_t i=0; i<tm.line_pos.size(); ++i)
        {
            // make sure the extents are included in the bounds
            b.add_point(tm.line_pos[i].ext[0]);
            b.add_point(tm.line_pos[i].ext[2]);

            // use the full vertical range from descent to ascent - this
            // ensures the bounding box also covers any underlining
            pt.x = tm.line_pos[i].ext[0].x;
            pt.y = font_descent + tm.line_pos[i].vOffset;
            b.add_point(pt);

            pt.x = tm.line_pos[i].ext[2].x;
            pt.y = font_ascent + tm.line_pos[i].vOffset;
            b.add_point(pt);
        }

        RS_F_Point fpts[4];
        b.get_points(fpts);

        // factor in the frame offset
        double offx = tdef.frameoffsetx();
        double offy = tdef.frameoffsety();
        fpts[0].x -= offx;
        fpts[0].y -= offy;
        fpts[1].x += offx;
        fpts[1].y -= offy;
        fpts[2].x += offx;
        fpts[2].y += offy;
        fpts[3].x -= offx;
        fpts[3].y += offy;

        // rotate and translate it
        for (int j=0; j<4; ++j)
        {
            double tmpX = fpts[j].x;
            double tmpY = fpts[j].y;
            fpts[j].x = insx + tmpX * cos_a - tmpY * sin_a;
            fpts[j].y = insy + tmpX * sin_a + tmpY * cos_a;
        }

        // draw a filled rectangle
        LineBuffer lb(5);
        lb.MoveTo(fpts[0].x, fpts[0].y);
        lb.LineTo(fpts[1].x, fpts[1].y);
        lb.LineTo(fpts[2].x, fpts[2].y);
        lb.LineTo(fpts[3].x, fpts[3].y);
        lb.Close();

        if (bOpaque)
            m_pSERenderer->DrawScreenPolygon(&lb, NULL, tdef.opaquecolor().argb());
        if (bFramed)
            m_pSERenderer->DrawScreenPolyline(&lb, NULL, tdef.framecolor().argb(), 0.0);
    }

    // calculate a 0.25 mm offset for ghosting
    int offset = ROUND(MetersToPixels(tdef.font().units(), 0.00025));
    if (offset == 0)
        offset = 1;

    const RS_Font* pFont = tm.font;
    RS_TextDef tmpTDef = tdef;
    double fontHeight = tm.font_height;
    RichTextEngine* pRichTextEngine = NULL;
    if (tm.format_changes.size() > 0)
        pRichTextEngine = new RichTextEngine(m_pSERenderer, this, &tmpTDef);
    for (size_t k=0; k<tm.line_pos.size(); ++k)
    {
        const RS_String* txt;

        if (tm.line_pos.size() == 1 && !pRichTextEngine)
            txt = &tm.text;
        else
            txt = &tm.line_breaks.at(k);

        LinePos& pos = tm.line_pos[k];

        if (pRichTextEngine)
        {
            pRichTextEngine->ApplyFormatChanges(tm.format_changes[k]);
            pRichTextEngine->GetTextDef(&tmpTDef);
            pFont = FindFont(tmpTDef.font());
            fontHeight = MetersToPixels(tmpTDef.font().units(), tmpTDef.font().height());
        }

        // add the rotated original offset for this line to the insertion point
        // to get the actual draw point
        double insX = insx + pos.hOffset * cos_a - pos.vOffset * sin_a;
        double insY = insy + pos.hOffset * sin_a + pos.vOffset * cos_a;

        int posx = ROUND(insX);
        int posy = ROUND(insY);

        double textwidth = pos.ext[1].x - pos.ext[0].x;

        // render the ghosted text, if requested
        if ((tdef.textbg() & RS_TextBackground_Ghosted) != 0)
        {
            DrawString(*txt, posx-offset, posy, textwidth, fontHeight, pFont, tmpTDef.ghostcolor(), angleRad);
            DrawString(*txt, posx+offset, posy, textwidth, fontHeight, pFont, tmpTDef.ghostcolor(), angleRad);
            DrawString(*txt, posx, posy-offset, textwidth, fontHeight, pFont, tmpTDef.ghostcolor(), angleRad);
            DrawString(*txt, posx, posy+offset, textwidth, fontHeight, pFont, tmpTDef.ghostcolor(), angleRad);
        }

        // render the primary text
        DrawString(*txt, posx, posy, textwidth, fontHeight, pFont, tmpTDef.textcolor(), angleRad);

        // render the underline, if requested
        if (tmpTDef.font().style() & RS_FontStyle_Underline)
        {
            // estimate underline line width as % of font height
            double line_width = (double)pFont->m_underline_thickness * fontHeight / (double)pFont->m_units_per_EM;

            // underline position w.r.t. baseline
            double line_pos = (double)tm.font->m_underline_position * tm.font_height / (double)tm.font->m_units_per_EM;
            if (!m_pSERenderer->YPointsUp())
                line_pos = -line_pos;

            // the line's start point is the insertion point, but shifted vertically by line_pos
            double x0 = insX - line_pos * sin_a;
            double y0 = insY + line_pos * cos_a;

            // the end point is a horizontal shift by the text width
            double x1 = x0 + textwidth * cos_a;
            double y1 = y0 + textwidth * sin_a;

            // draw the thick line
            LineBuffer lb(2);
            lb.MoveTo(x0, y0);
            lb.LineTo(x1, y1);

            m_pSERenderer->DrawScreenPolyline(&lb, NULL, tmpTDef.textcolor().argb(), line_width);
        }

        // render the overline, if requested
        if (tmpTDef.font().style() & RS_FontStyle_Overline)
        {
            // estimate underline line width as % of font height
            double fontCapline = pFont->m_capheight * fontHeight / pFont->m_units_per_EM;
            double line_width = (double)pFont->m_underline_thickness * fontHeight / (double)pFont->m_units_per_EM;
            double line_pos = m_pSERenderer->YPointsUp()?
                  fontCapline + ((double)pFont->m_underline_position * fontHeight / (double)pFont->m_units_per_EM) :
                - fontCapline + ((double)pFont->m_underline_position * fontHeight / (double)pFont->m_units_per_EM);

            // the line's start point is the insertion point, but shifted vertically by line_pos
            double x0 = insX - line_pos * sin_a;
            double y0 = insY + line_pos * cos_a;

            // the end point is a horizontal shift by the text width
            double x1 = x0 + textwidth * cos_a;
            double y1 = y0 + textwidth * sin_a;

            // draw the thick line
            LineBuffer lb(2);
            lb.MoveTo(x0, y0);
            lb.LineTo(x1, y1);

            m_pSERenderer->DrawScreenPolyline(&lb, NULL, tmpTDef.textcolor().argb(), line_width);
        }
    }

    if (pRichTextEngine)
        delete pRichTextEngine;
}


//////////////////////////////////////////////////////////////////////////////
// Draws a text which has been previously laid out using LayoutPathText.
// The tdef argument is only used to get the underline option and the text
// colors.  Everything else is stored in the TextMetrics context structure
// and the CharPos array, which were previously computed by LayoutPathText.
void RS_FontEngine::DrawPathText(RS_TextMetrics& tm, RS_TextDef& tdef)
{
    int numchars = (int)tm.text.length();

    // draw the opaque / framed background first, if requested
    bool bFramed = ((tdef.textbg() & RS_TextBackground_Framed) != 0);
    bool bOpaque = ((tdef.textbg() & RS_TextBackground_Opaque) != 0);
    if (bFramed || bOpaque)
    {
        // get font ascent and descent
        double em_square_size = tm.font->m_units_per_EM;
        double font_ascent    = tm.font->m_ascender * tm.font_height / em_square_size;
        double font_descent   = tm.font->m_descender * tm.font_height / em_square_size;

        // factor in the frame offset
        double offx  = tdef.frameoffsetx();
        double offy0 = font_descent - tdef.frameoffsety();
        double offy1 = font_ascent  + tdef.frameoffsety();

        // account for Y direction
        if (!m_pSERenderer->YPointsUp())
        {
            offy0 = -offy0;
            offy1 = -offy1;
        }

        // these keep track of our last position
        double aa = m_pSERenderer->YPointsUp()? tm.char_pos[0].anglerad : -tm.char_pos[0].anglerad;
        double cx = tm.char_pos[0].x - offy0 * sin(aa) - offx * cos(aa);
        double cy = tm.char_pos[0].y + offy0 * cos(aa) - offx * sin(aa);

        LineBuffer lb(2*numchars+3);
        lb.MoveTo(cx, cy);

        // add the vertices along the bottom
        for (int i=1; i<numchars; ++i)
        {
            // move position by offset
            aa = m_pSERenderer->YPointsUp()? tm.char_pos[i].anglerad : -tm.char_pos[i].anglerad;
            cx = tm.char_pos[i].x - offy0 * sin(aa);
            cy = tm.char_pos[i].y + offy0 * cos(aa);
            lb.LineTo(cx, cy);
        }

        // get the approximate width of the last character, including the frame offset
        double char_width = offx + tm.char_advances[numchars-1];

        // estimate bottom right corner offset from last character
        cx += char_width * cos(aa);
        cy += char_width * sin(aa);
        lb.LineTo(cx, cy);

        // estimate top right corner offset from last character
        cx -= (offy1 - offy0) * sin(aa);
        cy += (offy1 - offy0) * cos(aa);
        lb.LineTo(cx, cy);

        // add the vertices along the top
        for (int i=numchars-1; i>0; --i)
        {
            // move position by offset
            aa = m_pSERenderer->YPointsUp()? tm.char_pos[i].anglerad : -tm.char_pos[i].anglerad;
            cx = tm.char_pos[i].x - offy1 * sin(aa);
            cy = tm.char_pos[i].y + offy1 * cos(aa);
            lb.LineTo(cx, cy);
        }

        // estimate top left corner offset from first character
        aa = m_pSERenderer->YPointsUp()? tm.char_pos[0].anglerad : -tm.char_pos[0].anglerad;
        cx = tm.char_pos[0].x - offy1 * sin(aa) - offx * cos(aa);
        cy = tm.char_pos[0].y + offy1 * cos(aa) - offx * sin(aa);
        lb.LineTo(cx, cy);

        // estimate bottom left corner offset from first character
        cx += (offy1 - offy0) * sin(aa);
        cy -= (offy1 - offy0) * cos(aa);
        lb.LineTo(cx, cy);

        // close the loop
        lb.Close();

        if (bOpaque)
            m_pSERenderer->DrawScreenPolygon(&lb, NULL, tdef.opaquecolor().argb());
        if (bFramed)
            m_pSERenderer->DrawScreenPolyline(&lb, NULL, tdef.framecolor().argb(), 0.0);
    }

    // calculate a 0.25 mm offset for ghosting
    int offset = ROUND(MetersToPixels(tdef.font().units(), 0.00025));
    if (offset == 0)
        offset = 1;

    RS_String c;

    // draw the characters, each in its computed position
    for (int i=0; i<numchars; ++i)
    {
        c = tm.text[i];

        // get the approximate character width
        double char_width = tm.char_advances[i];

        // compute screen position and round
        int posx = ROUND(tm.char_pos[i].x);
        int posy = ROUND(tm.char_pos[i].y);
        double angleRad = tm.char_pos[i].anglerad;

        if ((tdef.textbg() & RS_TextBackground_Ghosted) != 0)
        {
            DrawString(c, posx-offset, posy, char_width, tm.font_height, tm.font, tdef.ghostcolor(), angleRad);
            DrawString(c, posx+offset, posy, char_width, tm.font_height, tm.font, tdef.ghostcolor(), angleRad);
            DrawString(c, posx, posy-offset, char_width, tm.font_height, tm.font, tdef.ghostcolor(), angleRad);
            DrawString(c, posx, posy+offset, char_width, tm.font_height, tm.font, tdef.ghostcolor(), angleRad);
        }

        DrawString(c, posx, posy, char_width, tm.font_height, tm.font, tdef.textcolor(), angleRad);
    }

    // render underline
    if (tdef.font().style() & RS_FontStyle_Underline)
    {
        // estimate underline line width as % of font height
        double line_width = (double)tm.font->m_underline_thickness * tm.font_height / (double)tm.font->m_units_per_EM;

        // underline position w.r.t. baseline
        double line_pos = (double)tm.font->m_underline_position * tm.font_height / (double)tm.font->m_units_per_EM;
        if (!m_pSERenderer->YPointsUp())
            line_pos = -line_pos;

        // used to keep track of last position of the underline, which is
        // drawn piecewise for each character
        double aa = m_pSERenderer->YPointsUp()? tm.char_pos[0].anglerad : -tm.char_pos[0].anglerad;
        double cx = tm.char_pos[0].x - line_pos * sin(aa);
        double cy = tm.char_pos[0].y + line_pos * cos(aa);

        LineBuffer lb(numchars+1);
        lb.MoveTo(cx, cy);

        // add the underline vertices
        for (int i=1; i<numchars; ++i)
        {
            // move position by underline offset
            aa = m_pSERenderer->YPointsUp()? tm.char_pos[i].anglerad : -tm.char_pos[i].anglerad;
            cx = tm.char_pos[i].x - line_pos * sin(aa);
            cy = tm.char_pos[i].y + line_pos * cos(aa);
            lb.LineTo(cx, cy);
        }

        // get the approximate width of the last character
        double char_width = tm.char_advances[numchars-1];

        // estimate bottom right corner of last character
        cx += char_width * cos(aa);
        cy += char_width * sin(aa);
        lb.LineTo(cx, cy);

        // draw the underline
        m_pSERenderer->DrawScreenPolyline(&lb, NULL, tdef.textcolor().argb(), line_width);
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
    //     positive.

    double em_square_size = font->m_units_per_EM;
    double font_ascent    = font->m_ascender * actual_height / em_square_size;
    double font_descent   = font->m_descender * actual_height / em_square_size;

    if (font->m_capheight == 0)
    {
        // happy hack to get the capline since FreeType doesn't know it
        RS_F_Point fpts[4];
        MeasureString(L"A", em_square_size, font, 0.0, fpts, NULL);

        // set it on the font, so that we don't have to measure it all the time
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

    if (m_pSERenderer->YPointsUp())
        offsetY = -offsetY;

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


//////////////////////////////////////////////////////////////////////////////
// Scales an input length in meters in the specified units - device or
// mapping - to a length in renderer screen space.
double RS_FontEngine::MetersToPixels(RS_Units unit, double number)
{
    double m2px = 1000.0 * m_pSERenderer->GetPixelsPerMillimeterScreen();

    double scale_factor;
    if (unit == RS_Units_Device)
        scale_factor = m2px; //device units simply returns scale to convert meters to pixels
    else
        scale_factor = m2px / m_pSERenderer->GetMapScale(); //for mapping space we also take map scale into account

    return number * scale_factor;
}
