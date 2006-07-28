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
#include "LabelRenderer.h"
#include "gd.h"
#include "GDRenderer.h"
#include "GDUtils.h"
#include "RS_Font.h"

#define M_PI 3.14159265358979323846
#define ROUND(x) (int)((x) + 0.5)

//#define DEBUG_LABELS

extern int ConvertColor(gdImagePtr i, RS_Color& c);


//////////////////////////////////////////////////////////////////////////////
LabelRenderer::LabelRenderer(GDRenderer* renderer)
: m_renderer(renderer),
  m_bOverpostGroupOpen(false)
{
}


//////////////////////////////////////////////////////////////////////////////
LabelRenderer::~LabelRenderer()
{
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::StartLabels()
{
    m_overpost.Clear();
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::ProcessLabel(double x, double y, const RS_String& text, RS_TextDef& tdef)
{
    LR_LabelInfo info(x, y, text, tdef);

    // here we just accumulate all the labels in a list
    if (m_bOverpostGroupOpen)
        m_labelGroups.back().m_labels.push_back(info);
    else
        ProcessLabelInternal(info, true, false, false);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::ProcessLabelGroup(RS_LabelInfo*    labels,
                                      int              nlabels,
                                      const RS_String& text,
                                      RS_OverpostType  type,
                                      bool             exclude,
                                      LineBuffer*      path)
{
    BeginOverpostGroup(type, true, exclude);

    // get the geometry type
    _ASSERT(path != NULL);
    int geomType = (path != NULL)? path->geom_type() : FdoGeometryType_None;

    //TODO: take into account advanced labeling flag
    //if (labels->advanced()) ... etc.

    // in the case of linear geometry we'll label along the path, so
    // prepare for that (transform to pixels, group into stitch groups)
    if (geomType == FdoGeometryType_LineString || geomType == FdoGeometryType_MultiLineString)
    {
        // indicate that the current group will be labeled along the path
        m_labelGroups.back().m_algo = laCurve;

        // If we are in tiled mode, we cannot stitch features which span
        // more than one tile because labels will not be continuous across
        // tiles -- i.e. computed label positions will be different since
        // some features will not come in with the spatial query for some
        // far away tiles, even though they should for labeling purposes.
        // This does not affect stitching of multi-linestring features which
        // we can always stitch without worrying about parts of them not
        // coming in on some tiles.
        RS_String stitch_key = text;

        if (m_renderer->UseLocalOverposting())
        {
            // TODO: stitch in subregions of tile
//          const RS_Bounds& tileBounds = m_renderer->GetBounds();
//          const RS_Bounds& featBounds = path->bounds();
//          if (featBounds.minx < tileBounds.minx ||
//              featBounds.maxx > tileBounds.maxx ||
//              featBounds.miny < tileBounds.miny ||
//              featBounds.maxy > tileBounds.maxy)
            {
                // If we don't want to stitch separate line features, mangle
                // the stitch table key to make it unique across features, but
                // keep it the same when we stitch the parts of a linestring.
                wchar_t tmp[32];
                swprintf(tmp, 32, L"%d", m_labelGroups.size());
                stitch_key += tmp;
            }
        }

        // If it's a multi-linestring, we will add each separate linestring
        // piece for labeling.  Some of these will get stitched together
        // later, but if they are far away we will essentially label each
        // one by itself.

        RS_F_Point* lblpath = NULL;
        int lblpathpts = 0;
        int offset = 0;

        for (int i=0; i<path->cntr_count(); i++)
        {
            // now transform the points of the current contour to pixel space
            double* lines = path->points() + 2 * offset;

            lblpathpts = path->cntrs()[i];
            lblpath = new RS_F_Point[lblpathpts];

            for (int b=0; b<lblpathpts; b++)
            {
                lblpath[b].x = m_renderer->_TXD(lines[2*b]);
                lblpath[b].y = m_renderer->_TYD(lines[2*b+1]);
            }

            _ASSERT(nlabels == 1);
            RS_LabelInfo* info = &labels[0];    // TODO: assumes one label

            LR_LabelInfo lrinfo(info->x(), info->y(), text, info->tdef());
            lrinfo.m_pts    = lblpath;
            lrinfo.m_numpts = lblpathpts;

            // see if we already have a polyline with the same label
            size_t group_index = m_hStitchTable[stitch_key];
            if (group_index)
            {
                // found one - add it to that group
                m_labelGroups[group_index-1].m_labels.push_back(lrinfo);    // offset index by 1 since 0 is invalid std::map entry
            }
            else
            {
                // none found - add it to the current group
                m_labelGroups.back().m_labels.push_back(lrinfo);

                // add a new entry in the stitch table
                m_hStitchTable[stitch_key] = (m_labelGroups.size()-1) + 1;  // offset index by 1 since 0 is invalid std::map entry
            }

            offset += path->cntrs()[i];
        }
    }
    else
    {
        // case of a simple label
        for (int i=0; i<nlabels; i++)
        {
            RS_LabelInfo* info = &labels[i];

            // label is in device space
            double offx = m_renderer->_MeterToMapSize(info->dunits(), info->dx());
            double offy = m_renderer->_MeterToMapSize(info->dunits(), info->dy());

            LR_LabelInfo lrinfo(info->x() + offx, info->y() + offy, text, info->tdef());

            m_labelGroups.back().m_labels.push_back(lrinfo);
        }
    }

    EndOverpostGroup();
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::BeginOverpostGroup(RS_OverpostType type, bool render, bool exclude)
{
    m_bOverpostGroupOpen = true;

    // add a new group
    LR_OverpostGroup group(render, exclude, type);
    m_labelGroups.push_back(group);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::EndOverpostGroup()
{
    m_bOverpostGroupOpen = false;

    // don't add empty groups
    if (m_labelGroups.back().m_labels.size() == 0)
        m_labelGroups.pop_back();
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::BlastLabels()
{
    //-------------------------------------------------------
    // step 1 - perform stitching
    //-------------------------------------------------------

    for (size_t i=0; i<m_labelGroups.size(); i++)
    {
        LR_OverpostGroup& group = m_labelGroups[i];

        if (group.m_algo == laCurve && group.m_labels.size() > 1)
        {
            std::vector<LR_LabelInfo> stitched = StitchPolylines(group.m_labels);
            if (stitched.size() > 0)
            {
                // replace the existing vector of labels with the stitched one
                for (size_t j=0; j<group.m_labels.size(); j++)
                {
                    LR_LabelInfo& info = group.m_labels[j];
                    if (info.m_pts)
                    {
                        delete [] info.m_pts;
                        info.m_pts = NULL;
                        info.m_numpts = 0;
                    }
                }

                group.m_labels.clear();
                group.m_labels.insert(group.m_labels.end(), stitched.begin(), stitched.end());
            }
        }
    }

    //-------------------------------------------------------
    // step 2 - apply overpost algorithm to all accumulated labels
    //-------------------------------------------------------

    for (int i=(int)m_labelGroups.size()-1; i>=0; i--)
    {
        LR_OverpostGroup& group = m_labelGroups[i];

        for (size_t j=0; j<group.m_labels.size(); j++)
        {
            LR_LabelInfo& info = group.m_labels[j];
            bool res = ProcessLabelInternal(info,
                                            group.m_render,
                                            group.m_exclude,
                                            group.m_type != RS_OverpostType_All);

            // only in the case of a simple label do we check the overpost type
            if (group.m_algo == laSimple)
            {
                if (res && (group.m_type == RS_OverpostType_FirstFit))
                    break;
            }
        }
    }

    //-------------------------------------------------------
    // step 3 - clean up labeling paths
    //-------------------------------------------------------

    for (size_t i=0; i<m_labelGroups.size(); i++)
    {
        LR_OverpostGroup& group = m_labelGroups[i];
        for (size_t j=0; j<group.m_labels.size(); j++)
        {
            LR_LabelInfo& info = group.m_labels[j];
            if (info.m_pts)
            {
                delete [] info.m_pts;
                info.m_pts = NULL;
                info.m_numpts = 0;
            }
        }
    }

    m_labelGroups.clear();
    m_hStitchTable.clear();
    m_overpost.Clear();
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::ProcessLabelInternal(LR_LabelInfo& info,
                                         bool render, bool exclude, bool check)
{
    //if it is path label, call our path text routine
    if (info.m_pts)
        return DrawPathLabel(info, render, exclude, check);

    // otherwise use the simple version
    return DrawSimpleLabel(info, render, exclude, check);
}


//////////////////////////////////////////////////////////////////////////////
//modifies the text extent (and insertion point) to take into account text alignment
void LabelRenderer::ApplyTextAlignment(RS_TextDef& tdef, double actual_height, RS_F_Point* extent, RS_F_Point& ins_point, const RS_Font* font)
{
    // adjustments for horizontal and vertical alignment
    double hAlignOffset = 0.0;
    double vAlignOffset = 0.0;

    // --------------------
    // horizontal alignment
    // --------------------

    // get the width vector from the (rotated) text bounding box
    double dxwA = extent[1].x - extent[0].x;
    double dywA = extent[1].y - extent[0].y;
    double dxwB = extent[2].x - extent[3].x;
    double dywB = extent[2].y - extent[3].y;
    double dxw  = 0.5*(dxwA + dxwB);
    double dyw  = 0.5*(dywA + dywB);

    RS_HAlignment hAlign = tdef.halign();
    switch (hAlign)
    {
    case RS_HAlignment_Left:
        break;

    case RS_HAlignment_Center:
        hAlignOffset += 0.5*dxw;
        vAlignOffset += 0.5*dyw;
        break;

    case RS_HAlignment_Right:
        hAlignOffset += dxw;
        vAlignOffset += dyw;
        break;
    }

    // ------------------
    // vertical alignment
    // ------------------

    double em_square_size = font->m_units_per_EM;
    double font_cap_height = font->m_height * actual_height / em_square_size;
    double font_ascent     = font->m_ascender * actual_height / em_square_size;
    double font_descent    = font->m_descender * actual_height / em_square_size;

    // get the height vector
    double dyh = 0.0;

    RS_VAlignment vAlign = tdef.valign();
    switch (vAlign)
    {
    case RS_VAlignment_Descent:
        dyh = -font_descent;
        break;

    case RS_VAlignment_Base:
        break;

    case RS_VAlignment_Half:
        dyh = 0.5*font_cap_height;
        break;

    case RS_VAlignment_Cap:
        dyh = font_cap_height;
        break;

    case RS_VAlignment_Ascent:
        dyh = font_ascent;
        break;
    }

    // add in the rotated vertical alignment contribution
    double angle = tdef.rotation() * M_PI / 180.0;

    //take into account that y axis goes down in our coordinate system
    dyh = -dyh;
    angle = -angle;

    double cs = cos(angle);
    double sn = sin(angle);
    hAlignOffset +=  -sn*dyh;
    vAlignOffset +=  cs*dyh;

    // adjust the bounding box
    for (int i=0; i<4; i++)
    {
        extent[i].x -= hAlignOffset;
        extent[i].y -= vAlignOffset;
    }

    //adjust insertion point
    ins_point.x -= hAlignOffset;
    ins_point.y -= vAlignOffset;
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::AddExclusionRegion(RS_F_Point* pts, int npts)
{
    //find axis aligned bounds to use for overposting
    //TODO: can be improved to use the given polygon as is
    //instead of taking its bounds
    RS_Bounds axis_bounds;

    ComputeBounds(pts, npts, axis_bounds);

    //convert to mapping space for overpost tracking
    //this is needed so that we can do overposting
    //across map tiles generated at different times
    DeviceToMappingBounds(axis_bounds);

    m_overpost.AddRegion(axis_bounds);
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::OverlapsStuff(RS_F_Point* pts, int npts)
{
    //use axis aligned bounds to use for overposting
    //TODO: can be improved to use the given polygon as is
    //instead of taking its bounds
    RS_Bounds axis_bounds;
    ComputeBounds(pts, npts, axis_bounds);

    //convert to mapping space for overpost tracking
    //this is needed so that we can do overposting
    //across map tiles generated at different times
    DeviceToMappingBounds(axis_bounds);

    return m_overpost.Overlaps(axis_bounds);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::DeviceToMappingBounds(RS_Bounds& b)
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
void LabelRenderer::ComputeBounds(RS_F_Point* RESTRICT pts, int npts, RS_Bounds& b)
{
    if (!npts) return;

    b.minx = b.maxx = pts[0].x;
    b.miny = b.maxy = pts[0].y;

    for (int i=1; i<npts; i++)
    {
        if (pts[i].x < b.minx)
            b.minx = pts[i].x;
        else if (pts[i].x > b.maxx)
            b.maxx = pts[i].x;

        if (pts[i].y < b.miny)
            b.miny = pts[i].y;
        else if (pts[i].y > b.maxy)
            b.maxy = pts[i].y;
    }
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::DrawSimpleLabel(LR_LabelInfo& info, bool render, bool exclude, bool check)
{
    //determine pixel space font height
    double hgt = m_renderer->_MeterToMapSize(info.m_tdef.font().units(), info.m_tdef.font().height());
    hgt *= m_renderer->GetMapToScreenScale() * 0.75;

    // The computed height can have roundoff in it, and the rendering code is
    // very sensitive to it.  Remove this roundoff by rounding the height to
    // the nearest 1/65536ths of a point.
    hgt *= 65536.0;
    hgt = ROUND(hgt);
    hgt /= 65536.0;

    //radian CCW rotation
    double rotation = info.m_tdef.rotation() * M_PI / 180.0;

    //-------------------------------------------------------
    // text extent and alignment computation
    //-------------------------------------------------------

    //font matching
    //TODO: not working right now
    const RS_Font* font = m_renderer->FindFont(info.m_tdef.font());
    if (!font)
        return false;

    //get overall string size and char spacing
    RS_F_Point fpts[4];
    float* spacing = (float*)alloca(info.m_text.length() * 2 * sizeof(float));
    m_renderer->MeasureString(info.m_text.c_str(), hgt, font, 0.0, fpts, spacing);

    //the width and height of this particular string
    //TODO: we can be more precise if we get height on per character basis
    double texthgt = fabs(fpts[2].y - fpts[0].y);
    double textwid = fabs(fpts[1].x - fpts[0].x);

    //get overall extent and char spacing, including rotation
    m_renderer->MeasureString(info.m_text.c_str(), hgt, font, rotation, fpts, NULL);

    //transform insertion point into pixel space
    RS_F_Point ins_point(m_renderer->_TXD(info.m_x), m_renderer->_TYD(info.m_y));

    for (int i=0; i<4; i++)
    {
        fpts[i].x += ins_point.x;
        fpts[i].y += ins_point.y;
    }

    //apply text alignment to bounding box
    //this will modify both the insertion point and the extent
    ApplyTextAlignment(info.m_tdef, hgt, fpts, ins_point, font);

#ifdef DEBUG_LABELS
    // this debugging code draws a box around the label (using its bounds)
    RS_D_Point dpts[4];
    for (int j=0; j<4; j++)
    {
        dpts[j].x = (int)fpts[j].x;
        dpts[j].y = (int)fpts[j].y;
    }
    gdImagePolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.color()));
#endif

    //-------------------------------------------------------
    // check for overposting
    //-------------------------------------------------------

    if (check && OverlapsStuff(fpts, 4))
        return false;

    if (exclude)
        AddExclusionRegion(fpts, 4);

    //-------------------------------------------------------
    // draw the label
    //-------------------------------------------------------

    if (render)
    {
        //ghosting offset
        int offset = 0;

        //take into account ghosting making text bounds bigger
        if (info.m_tdef.textbg() == RS_TextBackground_Ghosted)
            offset = 1;

        int posx = (int)floor(ins_point.x+0.5);
        int posy = (int)floor(ins_point.y+0.5);

        //get descent from font metrics -- we will use this for underline
        //and opaque bbox
        //TODO: not sure if we should always draw the opaque bbox with the 
        //descent or only when underline is present...
        double descent = (double)font->m_descender * hgt / (double)font->m_units_per_EM;

        if (info.m_tdef.textbg() == RS_TextBackground_Ghosted)
        {
            m_renderer->DrawString(info.m_text, posx-offset, posy, hgt, font, info.m_tdef.bgcolor(), rotation);
            m_renderer->DrawString(info.m_text, posx+offset, posy, hgt, font, info.m_tdef.bgcolor(), rotation);
            m_renderer->DrawString(info.m_text, posx, posy-offset, hgt, font, info.m_tdef.bgcolor(), rotation);
            m_renderer->DrawString(info.m_text, posx, posy+offset, hgt, font, info.m_tdef.bgcolor(), rotation);
        }
        else if (info.m_tdef.textbg() == RS_TextBackground_Opaque)
        {
            //set up rotated bounds
            RS_F_Point b[4];

            RotatedBounds(ins_point.x, ins_point.y + descent, textwid, texthgt + descent, rotation, b);

            RS_D_Point dpts[4];
            for (int j=0; j<4; j++)
            {
                dpts[j].x = ROUND(b[j].x);
                dpts[j].y = ROUND(b[j].y);
            }

            gdImageFilledPolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.bgcolor()));
        }

        m_renderer->DrawString(info.m_text, posx, posy, hgt, font, info.m_tdef.color(), rotation);

        //render underline
        if (info.m_tdef.font().style() & RS_FontStyle_Underline)
        {
            //estimate underline line width as % of font height
            double line_width = (double)font->m_underline_thickness * hgt / (double)font->m_units_per_EM;
            double line_pos = - (double)font->m_underline_position * hgt / (double)font->m_units_per_EM;

            //set up a rotated thin rectangle representing the 
            //underline
            RS_F_Point b[4];

            RotatedBounds(ins_point.x, ins_point.y + line_pos, textwid, 0, rotation, b);

            RS_D_Point dpts[4];
            for (int j=0; j<4; j++)
            {
                dpts[j].x = ROUND(b[j].x);
                dpts[j].y = ROUND(b[j].y);
            }

            gdImageSetThickness((gdImagePtr)m_renderer->GetImage(), ROUND(line_width));
            gdImageLine((gdImagePtr)m_renderer->GetImage(), dpts[0].x, dpts[0].y, dpts[1].x, dpts[1].y, ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.color()));
            gdImageSetThickness((gdImagePtr)m_renderer->GetImage(), 0);
        }
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::DrawPathLabel(LR_LabelInfo& info, bool render, bool exclude, bool check)
{
    //determine pixel space font height
    double hgt = m_renderer->_MeterToMapSize(info.m_tdef.font().units(), info.m_tdef.font().height());
    hgt *= m_renderer->GetMapToScreenScale() * 0.75;

    // The computed height can have roundoff in it, and the rendering code is
    // very sensitive to it.  Remove this roundoff by rounding the height to
    // the nearest 1/65536ths of a point.
    hgt *= 65536.0;
    hgt = ROUND(hgt);
    hgt /= 65536.0;

    //-------------------------------------------------------
    // text extent and alignment computation
    //-------------------------------------------------------

    //font matching
    const RS_Font* font = m_renderer->FindFont(info.m_tdef.font());
    if (!font)
        return false;

    //get overall extent and char spacing
    RS_F_Point fpts[4];
    float* spacing = (float*)alloca(info.m_text.length() * 2 * sizeof(float));
    m_renderer->MeasureString(info.m_text.c_str(), hgt, font, 0.0, fpts, spacing);

    //the width and height of this particular string
    //TODO: we can be more precise if we get height on per character basis
    double texthgt = fabs(fpts[2].y - fpts[0].y);
    double textwid = fabs(fpts[1].x - fpts[0].x);

    //find length of each segment in the screen space path
    //we will use it to position characters along the curve
    //This is precomputed here, rather than in ComputeCharacterPositions
    //in order to reuse the data for repeated labels
    _ASSERT(info.m_numpts < 16384);
    double* seglens = (double*)alloca(sizeof(double) * info.m_numpts);
    seglens[0] = 0.0;

    for (int i=1; i<info.m_numpts; i++)
    {
        double dx = info.m_pts[i].x - info.m_pts[i-1].x;
        double dy = info.m_pts[i].y - info.m_pts[i-1].y;
        seglens[i] = seglens[i-1] + sqrt(dx*dx + dy*dy);
    }

    //how many times should we repeat the label along the polyline?
    //TODO: fine tune this formula
    int numreps = (int)(seglens[info.m_numpts-1] / (200.0 + textwid));
    if (!numreps) numreps = 1;

    int labels_drawn = 0; //counter for how many of the repeated label were accepted

    for (int i=0; i<numreps; i++)
    {
        //parametric position for current repeated label
        //positions are spaced in such a way that each label has
        //an equal amount of white space on both sides around it
        double param_position = ((double)i + 0.5) / (double)numreps;

        //compute position and angle along the path for each character
        size_t numchars = info.m_text.length();
        CharPos* pos = (CharPos*)alloca(sizeof(CharPos) * numchars);
        double font_scale = ComputeCharacterPositions(info, seglens, param_position, spacing, textwid, pos);

        //update font height to value that curve fitting suggested
        if (font_scale != 1.0)
        {
            //don't bother measuring the string with the new height,
            //scaling should work almost as good
            hgt     *= font_scale;
            texthgt *= font_scale;
            textwid *= font_scale;
        }

        //take into account text vertical alignment
        //horizontal alignment is ignored in this case
        for (size_t i=0; i<numchars; i++)
        {
            //TODO:
            double font_cap_height = hgt;
            double font_ascent     = hgt;
            double font_descent    = 0;

            // get the height vector
            double dyh = 0.0;

            RS_VAlignment vAlign = info.m_tdef.valign();
            switch (vAlign)
            {
                case RS_VAlignment_Descent: dyh = -font_descent;        break;
                case RS_VAlignment_Base:                                break;
                case RS_VAlignment_Half:    dyh =  font_cap_height/2.0; break;
                case RS_VAlignment_Cap:     dyh =  font_cap_height;     break;
                case RS_VAlignment_Ascent:  dyh =  font_ascent;         break;
                default:                                                break;
            }

            // add in the rotated vertical alignment contribution
            double angle = pos[i].anglerad;

            //take into account that y axis goes down in our coordinate system
            dyh = -dyh;

            double cs = cos(angle);
            double sn = sin(angle);
            double hAlignOffset = -sn*dyh;
            double vAlignOffset =  cs*dyh;

            //adjust insertion point
            pos[i].x += hAlignOffset;
            pos[i].y -= vAlignOffset;
        }

        //once we have position and angle for each character
        //compute oriented bounding box for each character
        RS_F_Point* oriented_bounds = (RS_F_Point*)alloca(4 * numchars * sizeof(RS_F_Point));

        double total_advance = 0.0;
        for (size_t i=0; i<numchars; i++)
        {
            //width of character - not really exact width since
            //it takes kerning into account, but should be good enough
            //we could measure each character separately but that seems like
            //too many calls to FreeType
            double advance = (i == numchars-1)? textwid - total_advance : spacing[i] * font_scale;
            total_advance += advance;

            //compute rotated bounds of character
            RS_F_Point* b = &oriented_bounds[i * 4];
            RotatedBounds(pos[i].x, pos[i].y, advance, texthgt, pos[i].anglerad, b);

    #ifdef DEBUG_LABELS
            RS_D_Point dpts[4];
            for (int j=0; j<4; j++)
            {
                dpts[j].x = (int)b[j].x;
                dpts[j].y = (int)b[j].y;
            }
            gdImagePolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.color()));
    #endif
        }

        //-------------------------------------------------------
        // check for overposting
        //-------------------------------------------------------

        //we need to check each character
        if (check)
        {
            for (size_t i=0; i<numchars; i++)
            {
                if (OverlapsStuff(&oriented_bounds[i*4], 4))
                    goto cont_loop; //skip past label draw, but keep looping through outer loop
            }
        }



        //add bounds to exclusion regions if needed
        //once again, do this per character to get tighter bounds around the label
        if (exclude)
        {
            for (size_t i=0; i<numchars; i++)
            {
                AddExclusionRegion(&oriented_bounds[i*4], 4);
            }
        }

        //-------------------------------------------------------
        // draw the label
        //-------------------------------------------------------

        if (render)
        {
            //wrap a single character in a NULL terminated string
            wchar_t c[] = {0, 0};

            //ghosting offset
            int offset = 0;

            //take into account ghosting making text bounds bigger
            if (info.m_tdef.textbg() == RS_TextBackground_Ghosted)
                offset = 1;

            //draw the characters, each in its computed position
            for (size_t i=0; i<numchars; i++)
            {
                c[0] = info.m_text[i];

                //compute screen position and round
                int posx = ROUND(pos[i].x);
                int posy = ROUND(pos[i].y);

                if (info.m_tdef.textbg() == RS_TextBackground_Ghosted)
                {
                    m_renderer->DrawString(c, posx-offset, posy, hgt, font, info.m_tdef.bgcolor(), pos[i].anglerad);
                    m_renderer->DrawString(c, posx+offset, posy, hgt, font, info.m_tdef.bgcolor(), pos[i].anglerad);
                    m_renderer->DrawString(c, posx, posy-offset, hgt, font, info.m_tdef.bgcolor(), pos[i].anglerad);
                    m_renderer->DrawString(c, posx, posy+offset, hgt, font, info.m_tdef.bgcolor(), pos[i].anglerad);
                }

                m_renderer->DrawString(c, posx, posy, hgt, font, info.m_tdef.color(), pos[i].anglerad);
            }

            //render underline
            if (info.m_tdef.font().style() & RS_FontStyle_Underline)
            {
                //estimate underline line width as % of font height
                double line_width = (double)font->m_underline_thickness * hgt / (double)font->m_units_per_EM;
                //underline position w.r.t. baseline. Invert y while at it
                double line_pos = - (double)font->m_underline_position * hgt / (double)font->m_units_per_EM;

                //zero it out again, we will need the accumulator for
                //underline purposes
                total_advance = 0.0;

                //used to keep track of last position of the underline, which is 
                //drawn piecewise for each character
                int last_x = ROUND(pos[0].x + sin(pos[0].anglerad) * line_pos);
                int last_y = ROUND(pos[0].y + cos(pos[0].anglerad) * line_pos);
                int sx, sy, ex, ey;

                //draw the characters, each in its computed position
                for (size_t i=0; i<numchars; i++)
                {
                    //width of character - not really exact width since
                    //it takes kerning into account, but should be good enough
                    //we could measure each character separately but that seems like
                    //too many calls to FreeType
                    double advance = (i == numchars-1)? textwid - total_advance : spacing[i] * font_scale;
                    total_advance += advance;

                    sx = last_x;
                    sy = last_y;

                    if (i == numchars - 1)
                    {
                        //estimate bottom right corner of last character
                        double eex = pos[i].x + cos(pos[i].anglerad) * advance;
                        double eey = pos[i].y - sin(pos[i].anglerad) * advance;

                        //now take into account underline offset
                        ex = ROUND(eex + sin(pos[i].anglerad) * line_pos);
                        ey = ROUND(eey + cos(pos[i].anglerad) * line_pos);
                    }
                    else
                    {
                        //move position by underline offset
                        ex = ROUND(pos[i+1].x + sin(pos[i+1].anglerad) * line_pos);
                        ey = ROUND(pos[i+1].y + cos(pos[i+1].anglerad) * line_pos);
                    }

                    gdImageSetThickness((gdImagePtr)m_renderer->GetImage(), (int)(line_width+0.5));
                    gdImageSetAntiAliased((gdImagePtr)m_renderer->GetImage(), ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.color()));
                    gdImageLine((gdImagePtr)m_renderer->GetImage(), sx, sy, ex, ey, gdAntiAliased);
                    gdImageSetThickness((gdImagePtr)m_renderer->GetImage(), 0);

                    last_x = ex;
                    last_y = ey;
                }
            }
        }

        labels_drawn ++; //increment count of how many labels we accepted

cont_loop:
        ;
    }

    return (labels_drawn != 0);
}


//////////////////////////////////////////////////////////////////////////////
//computes position and rotation of each character along the
//feature to be labeled -- it may return a scale factor to be
//applied to the font size to make the label fit better in the
//allotted space
double LabelRenderer::ComputeCharacterPositions(LR_LabelInfo& info, double* seglens, double position, float* kerned_spacing, double measured_width, CharPos* ret)
{

    //determine in which direction we should follow the polyline
    //so that the label points up more likely than down
    //find the length of polyline that requires inverting versus
    //length that does not and see which one is bigger
    //TODO: this loop should really be performed on the piece of the
    //polyline where we will be labeling since otherwise features like
    //circles can throw it off and so a label may still end up upside down
    double inverted_len = 0.0;

    for (int m=0; m<info.m_numpts - 1; m++)
    {
        RS_F_Point p0 = info.m_pts[m];
        RS_F_Point p1 = info.m_pts[m+1];

        double angle = -atan2(p1.y - p0.y, p1.x - p0.x);

        if (angle > M_PI/2.0 || angle < -M_PI/2.0)
            inverted_len += seglens[m+1] - seglens[m];
        else
            inverted_len -= seglens[m+1] - seglens[m];
    }

    bool reverse = (inverted_len > 0.0);

    double pathlen = seglens[info.m_numpts - 1];

    //compute font height that better fits the geometry
    //but limit scaling to be in the range [0.5 - 1.0]
    double font_scale = rs_min(1.0, rs_max(0.5, pathlen / (1.1 * measured_width)));

    //distance of current character along current segment
    //initialize to start position, based on given parametric position
    //along the polyline
    double dist_along_segment = position * pathlen - 0.5 * measured_width * font_scale;

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
        for (j=info.m_numpts-2; j>=0; j--)
            if (dist_along_segment < pathlen - seglens[j])
                break;

        if (j < 0) j=0;

        start = info.m_pts[j+1];
        end = info.m_pts[j];
        dist_along_segment -= pathlen - seglens[j+1];
    }
    else
    {
        //case where we follow the polyline forwards

        //compute starting segment
        for (j=0; j<info.m_numpts-1; j++)
            if (dist_along_segment < seglens[j+1])
                break;

        if (j >= info.m_numpts-1)
            j=info.m_numpts-2;

        start = info.m_pts[j];
        end = info.m_pts[j+1];
        dist_along_segment -= seglens[j];
    }

    //length of current segment
    double seg_len = seglens[j+1] - seglens[j];

    //position of current character relative to the left end of the string
    double char_pos = 0;

    int numchars = (int)info.m_text.length();

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
                    start = info.m_pts[j+1];
                    end = info.m_pts[j];
                }
                else
                    break;
            }
            else
            {
                //case where we go forward along the path
                if (j < info.m_numpts - 2)
                {
                    j++;
                    dist_along_segment -= seg_len;
                    seg_len = seglens[j+1] - seglens[j];
                    start = info.m_pts[j];
                    end = info.m_pts[j+1];
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
        double char_width = (index >= numchars - 1) ? measured_width - char_pos : kerned_spacing[index];

        //advance cursor by half the width of the current character
        dist_along_segment += char_width * 0.5 * font_scale;
        char_pos += char_width * 0.5;
    }

    char_pos = 0.0;

    //now compute character placement and angles based on the positioning points
    for (int i=0; i<numchars; i++)
    {
        //find angle based on left and right of character
        ret[i].anglerad = -atan2(positions[2*i+2].y - positions[2*i].y,positions[2*i+2].x - positions[2*i].x);

        //kerned width of current character
        double char_width = (i == numchars - 1) ? measured_width - char_pos : kerned_spacing[i];
        double char_width_2 = 0.5 * char_width * font_scale;

        //and now find a lower left insertion point
        //based on the midpoint anchor and the angle
        ret[i].x = positions[2*i+1].x - cos(ret[i].anglerad) * char_width_2;
        ret[i].y = positions[2*i+1].y + sin(ret[i].anglerad) * char_width_2; //y down means + sin

        char_pos += char_width;
    }

    return font_scale;
}


//////////////////////////////////////////////////////////////////////////////
//Applies a given angle to an axis aligned bounding box.
//Rotation point is lower left
inline void LabelRenderer::RotatedBounds(double x, double y, double width, double height, double angle_cw_rad, RS_F_Point* b)
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
std::vector<LR_LabelInfo> LabelRenderer::StitchPolylines(std::vector<LR_LabelInfo>& labels)
{
    std::vector<LR_LabelInfo> src = labels; //make a copy
    std::vector<LR_LabelInfo> ret; //store results here

    //while there are unprocessed items
    while (src.size() > 0)
    {
        //try to stitch a source item to items in return list
        size_t i;
        for (i=0; i<ret.size(); i++)
        {
            LR_LabelInfo& retinfo = ret[i];

            size_t j;
            for (j=0; j<src.size(); j++)
            {
                LR_LabelInfo& srcinfo = src[j];

                bool start_with_src = false; //start stitch with source poly?
                bool startfwd = false; //go forward on start poly?
                bool endfwd = false; //go forward on end poly?
                bool count = 0;

                if (CloseEnough(retinfo.m_pts[0], srcinfo.m_pts[0]))
                {
                    //join start to start
                    start_with_src = true; //start with source poly
                    startfwd = false;
                    endfwd = true;
                    count++;
                }
                if (CloseEnough(retinfo.m_pts[retinfo.m_numpts-1], srcinfo.m_pts[0]))
                {
                    //join end to start
                    start_with_src = false; //start with ret poly
                    startfwd = true;
                    endfwd = true;
                    count++;
                }
                if (CloseEnough(retinfo.m_pts[retinfo.m_numpts-1], srcinfo.m_pts[srcinfo.m_numpts-1]))
                {
                    start_with_src = false; //start with ret poly
                    startfwd = true;
                    endfwd = false;
                    count++;
                }
                if (CloseEnough(retinfo.m_pts[0], srcinfo.m_pts[srcinfo.m_numpts-1]))
                {
                    start_with_src = true; //start with src poly
                    startfwd = true;
                    endfwd = true;
                    count++;
                }

                if (count)
                {
                    if (count == 1)
                    {
                        //alloc new stitched polyline
                        int num_stitched_pts = retinfo.m_numpts + srcinfo.m_numpts - 1;
                        RS_F_Point* stitched = new RS_F_Point[num_stitched_pts];

                        RS_F_Point* start = start_with_src ? srcinfo.m_pts : retinfo.m_pts;
                        int nstart = start_with_src ? srcinfo.m_numpts : retinfo.m_numpts;

                        if (startfwd)
                        {
                            memcpy(stitched, start, sizeof(RS_F_Point) * nstart-1);
                        }
                        else
                        {
                            for (int p=0; p<nstart-1; p++)
                                stitched[p] = start[nstart - p - 1];
                        }

                        RS_F_Point* end = start_with_src ? retinfo.m_pts : srcinfo.m_pts;
                        int nend = start_with_src ? retinfo.m_numpts : srcinfo.m_numpts;

                        if (endfwd)
                        {
                            memcpy(stitched + nstart-1, end, sizeof(RS_F_Point) * nend);
                        }
                        else
                        {
                            for (int p=0; p<nend; p++)
                                stitched[p + nstart - 1] = end[nend - p - 1];
                        }

                        delete [] retinfo.m_pts;
                        retinfo.m_pts = stitched;
                        retinfo.m_numpts = num_stitched_pts;
                    }
                    else
                    {
                        //count bigger than 1 indicates that the start and
                        //endpoints of the two polylines are equal, which means
                        //we will just ignore one of them
                    }

                    break;
                }
            }

            //remove item from source list if item was stiched to something in
            //the ret list
            if (j < src.size())
            {
                src.erase(src.begin() + j);
                break;
            }
        }

        //if we did not stitch any source polyline to a polyline
        //in the return list, move a polyline to the return list and go again
        if (i == ret.size())
        {
            LR_LabelInfo srcinfo = src.back();
            LR_LabelInfo retinfo = srcinfo;

            //need to allocate a copy of the polyline since it will be
            //deleted and reallocated by the stitching loop
            retinfo.m_pts = new RS_F_Point[srcinfo.m_numpts];
            memcpy(retinfo.m_pts, srcinfo.m_pts, sizeof(RS_F_Point) * srcinfo.m_numpts);

            ret.push_back(retinfo);
            src.pop_back();
        }
    }

    return ret;
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::CloseEnough(RS_F_Point& p1, RS_F_Point& p2)
{
    double delta = fabs(p2.y - p1.y) + fabs(p2.x - p1.x);
    return (delta <= 2.0); //2 pixels is close enough
}
