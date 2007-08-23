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
#include "LabelRenderer.h"
#include "Renderer.h"
#include "SE_Renderer.h"

//#define DEBUG_LABELS


//////////////////////////////////////////////////////////////////////////////
LabelRenderer::LabelRenderer(Renderer* renderer)
: LabelRendererBase(renderer)
{
}


//////////////////////////////////////////////////////////////////////////////
LabelRenderer::~LabelRenderer()
{
    try
    {
        if (m_labelGroups.size() > 0 || m_hStitchTable.size() > 0)
        {
            BlastLabels();
        }
    }
    catch (FdoException* e)
    {
        e->Release();
    }
    catch (...)
    {
    }
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::StartLabels()
{
    m_overpost.Clear();
    m_pathCount = 0;
}


//////////////////////////////////////////////////////////////////////////////
// non-SE symbol labels
void LabelRenderer::ProcessLabelGroup(RS_LabelInfo*    labels,
                                      int              nlabels,
                                      const RS_String& text,
                                      RS_OverpostType  type,
                                      bool             exclude,
                                      LineBuffer*      path)
{
    // bail if there are too many path labels
    if (m_pathCount > 1000)
        return;

    BeginOverpostGroup(type, true, exclude);

    // get the geometry type
    int geomType = (path != NULL)? path->geom_type() : FdoGeometryType_None;

    // TODO: take into account advanced labeling flag
//  if (labels->advanced()) ... etc.

    // in the case of linear geometry we'll label along the path, so
    // prepare for that (transform to pixels, group into stitch groups)
    if (geomType == FdoGeometryType_LineString || geomType == FdoGeometryType_MultiLineString)
    {
        // indicate that the current group will be labeled along the path
        m_labelGroups.back().m_algo = laCurve;

        // use the label string as the stitch key
        RS_String stitch_key = text;

        // If it's a multi-linestring, we will add each separate linestring
        // piece for labeling.  Some of these will get stitched together
        // later, but if they are far away we will essentially label each
        // one by itself.

        RS_F_Point* lblpath = NULL;
        int lblpathpts = 0;
        int offset = 0;

        for (int i=0; i<path->cntr_count(); ++i)
        {
            // now transform the points of the current contour to pixel space
            lblpathpts = path->cntr_size(i);
            lblpath = new RS_F_Point[lblpathpts];

            for (int b=0; b<lblpathpts; ++b)
                m_serenderer->WorldToScreenPoint(path->x_coord(offset+b), path->y_coord(offset+b), lblpath[b].x, lblpath[b].y);

            _ASSERT(nlabels == 1);
            RS_LabelInfo* info = &labels[0];    // TODO: assumes one label

            LabelInfo lrinfo(info->x(), info->y(), text, info->tdef());
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

            offset += lblpathpts;
            m_pathCount++;
        }
    }
    else
    {
        // case of a simple label
        for (int i=0; i<nlabels; ++i)
        {
            RS_LabelInfo* info = &labels[i];

            // label is in device space
            double offx = MeterToMapSize(info->dunits(), info->dx());
            double offy = MeterToMapSize(info->dunits(), info->dy());

            LabelInfo lrinfo(info->x() + offx, info->y() + offy, text, info->tdef());
            m_labelGroups.back().m_labels.push_back(lrinfo);
        }
    }

    EndOverpostGroup();
}


//////////////////////////////////////////////////////////////////////////////
// SE symbol labels
void LabelRenderer::ProcessLabelGroup(SE_LabelInfo*    labels,
                                      int              nlabels,
                                      RS_OverpostType  type,
                                      bool             exclude,
                                      LineBuffer*      /*path*/)
{
    BeginOverpostGroup(type, true, exclude);

    // Add a new style SE label to the overpost groups.
    // Here we are processing the simple case (like labels at given points
    // rather than labels along a line).  The hard case is TODO.
    m_labelGroups.back().m_algo = laSESymbol;

    for (int i=0; i<nlabels; ++i)
    {
        SE_LabelInfo* info = &labels[i];

        // label is in device space
        LabelInfo lrinfo(info->x, info->y, info->symbol);

        // TODO: HACK -- well somewhat of a hack -- store the angle in the tdef
        lrinfo.m_tdef.rotation() = info->anglerad / M_PI180;

        m_labelGroups.back().m_labels.push_back(lrinfo);
    }

    EndOverpostGroup();
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::BeginOverpostGroup(RS_OverpostType type, bool render, bool exclude)
{
    // add a new group
    OverpostGroup group(render, exclude, type);
    m_labelGroups.push_back(group);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::EndOverpostGroup()
{
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

    for (size_t i=0; i<m_labelGroups.size(); ++i)
    {
        OverpostGroup& group = m_labelGroups[i];

        if (group.m_algo == laCurve && group.m_labels.size() > 1)
        {
            std::vector<LabelInfo> stitched = StitchPolylines(group.m_labels);
            if (stitched.size() > 0)
            {
                // replace the existing vector of labels with the stitched one
                for (size_t j=0; j<group.m_labels.size(); ++j)
                {
                    LabelInfo& info = group.m_labels[j];

                    if (info.m_pts)
                    {
                        delete [] info.m_pts;
                        info.m_pts = NULL;
                        info.m_numpts = 0;
                    }

                    // the style was cloned when it was passed to the LabelRenderer
                    if (info.m_sestyle)
                    {
                        delete info.m_sestyle;
                        info.m_sestyle = NULL;
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

    for (int i=(int)m_labelGroups.size()-1; i>=0; --i)  // must use int since we're iterating backwards
    {
        OverpostGroup& group = m_labelGroups[i];

        for (size_t j=0; j<group.m_labels.size(); ++j)
        {
            LabelInfo& info = group.m_labels[j];
            bool res = ProcessLabelInternal(info,
                                            group.m_render,
                                            group.m_exclude,
                                            group.m_type != RS_OverpostType_All);

            // only in the case of a simple label do we check the overpost type
            if (res && (group.m_type == RS_OverpostType_FirstFit))
                break;
        }
    }

    //-------------------------------------------------------
    // step 3 - clean up labeling paths and styles
    //-------------------------------------------------------

    for (size_t i=0; i<m_labelGroups.size(); ++i)
    {
        OverpostGroup& group = m_labelGroups[i];

        for (size_t j=0; j<group.m_labels.size(); ++j)
        {
            LabelInfo& info = group.m_labels[j];

            if (info.m_pts)
            {
                delete [] info.m_pts;
                info.m_pts = NULL;
                info.m_numpts = 0;
            }

            // the style was cloned when it was passed to the LabelRenderer
            if (info.m_sestyle)
            {
                delete info.m_sestyle;
                info.m_sestyle = NULL;
            }
        }
    }

    m_labelGroups.clear();
    m_hStitchTable.clear();
    m_overpost.Clear();
    m_pathCount = 0;
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::ProcessLabelInternal(LabelInfo& info,
                                         bool render, bool exclude, bool check)
{
    if (info.m_sestyle)
        return DrawSELabel(info, render, exclude, check);

    // if it is path label, call our path text routine
    if (info.m_pts)
        return DrawPathLabel(info, render, exclude, check);

    // otherwise use the simple version
    return DrawSimpleLabel(info, render, exclude, check);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRenderer::AddExclusionRegion(RS_F_Point* pts, int npts)
{
    _ASSERT(npts == 4);

#ifdef DEBUG_LABELS
    LineBuffer lb(5);
    lb.MoveTo(pts[0].x, pts[0].y);
    lb.LineTo(pts[1].x, pts[1].y);
    lb.LineTo(pts[2].x, pts[2].y);
    lb.LineTo(pts[3].x, pts[3].y);
    lb.Close();
    m_serenderer->DrawScreenPolyline(&lb, NULL, 0xffff0000, 3.0);
#endif

    RS_F_Point* tmp = (RS_F_Point*)alloca(npts * sizeof(RS_F_Point));

    // convert back to mapping space since the overpost manager uses
    // mapping space bounding boxes
    for (int i=0; i<npts; ++i)
        m_serenderer->ScreenToWorldPoint(pts[i].x, pts[i].y, tmp[i].x, tmp[i].y);

    m_overpost.AddRegion(tmp, npts);
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::OverlapsStuff(RS_F_Point* pts, int npts)
{
    RS_F_Point* tmp = (RS_F_Point*)alloca(npts * sizeof(RS_F_Point));

    // convert back to mapping space since the overpost manager uses
    // mapping space bounding boxes
    for (int i=0; i<npts; ++i)
        m_serenderer->ScreenToWorldPoint(pts[i].x, pts[i].y, tmp[i].x, tmp[i].y);

    return m_overpost.Overlaps(tmp, npts);
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::DrawSimpleLabel(LabelInfo& info, bool render, bool exclude, bool check)
{
    RS_FontEngine* fe = m_serenderer->GetRSFontEngine();

    // measure the text (this function will take into account newlines)
    RS_TextMetrics tm;
    if (!fe->GetTextMetrics(info.m_text, info.m_tdef, tm, false))
        return false;

    // radian CCW rotation
    double angleRad = info.m_tdef.rotation() * M_PI180;
    if (!m_serenderer->YPointsUp())
        angleRad = -angleRad;
    double cos_a = cos(angleRad);
    double sin_a = sin(angleRad);

    // transform insertion point into pixel space
    RS_F_Point ins_point;
    m_serenderer->WorldToScreenPoint(info.m_x, info.m_y, ins_point.x, ins_point.y);

    //-------------------------------------------------------
    // text extent computation
    //-------------------------------------------------------

    RS_Bounds rotatedBounds(+DBL_MAX, +DBL_MAX, -DBL_MAX, -DBL_MAX);

    for (size_t k=0; k<tm.line_pos.size(); ++k)
    {
        LinePos& pos = tm.line_pos[k];
        RS_F_Point tmp;

        // process the extent points
        for (int j=0; j<4; ++j)
        {
            // rotate and translate to the insertion point
            double tmpX = pos.ext[j].x;
            double tmpY = pos.ext[j].y;
            tmp.x = ins_point.x + tmpX * cos_a - tmpY * sin_a;
            tmp.y = ins_point.y + tmpX * sin_a + tmpY * cos_a;

            // update the overall rotated bounds
            rotatedBounds.add_point(tmp);
        }
    }

    RS_F_Point fpts[4];
    rotatedBounds.get_points(fpts);

#ifdef DEBUG_LABELS
    // this debugging code draws a box around the label (using its bounds)
    LineBuffer lb(5);
    lb.MoveTo(fpts[0].x, fpts[0].y);
    lb.LineTo(fpts[1].x, fpts[1].y);
    lb.LineTo(fpts[2].x, fpts[2].y);
    lb.LineTo(fpts[3].x, fpts[3].y);
    lb.Close();
    m_serenderer->DrawScreenPolyline(&lb, NULL, info.m_tdef.textcolor().argb(), 0.0);
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
        fe->DrawBlockText(tm, info.m_tdef, ins_point.x, ins_point.y);

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::DrawSELabel(LabelInfo& info, bool render, bool exclude, bool check)
{
    // draws an SE symbol-label
    // This needs to be improved to handle text along a path

    // get native symbol bounds (in pixels -- the render style is already scaled to pixels)
    RS_F_Point fpts[4];
    memcpy(fpts, info.m_sestyle->bounds, sizeof(fpts));

    // translate and orient the bounds with the given angle and position of the symbol
    // apply position and rotation to the native bounds of the symbol
    double angleRad = info.m_tdef.rotation() * M_PI180;
    SE_Matrix m;
    m.rotate(m_serenderer->YPointsUp()? angleRad : -angleRad);
    m.translate(info.m_x, info.m_y);

    for (int i=0; i<4; ++i)
        m.transform(fpts[i].x, fpts[i].y);

    // check for overposting
    if (check && OverlapsStuff(fpts, 4))
        return false;

    // add bounds to exclusion regions if needed
    if (exclude)
        AddExclusionRegion(fpts, 4);

    //-------------------------------------------------------
    // draw the label
    //-------------------------------------------------------

    if (render)
    {
        m_serenderer->DrawSymbol(info.m_sestyle->symbol, m, angleRad);

#ifdef DEBUG_LABELS
        LineBuffer lb(5);
        lb.MoveTo(fpts[0].x, fpts[0].y);
        lb.LineTo(fpts[1].x, fpts[1].y);
        lb.LineTo(fpts[2].x, fpts[2].y);
        lb.LineTo(fpts[3].x, fpts[3].y);
        lb.Close();
        m_serenderer->DrawScreenPolyline(&lb, NULL, 0xff000000, 0.0);
#endif
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRenderer::DrawPathLabel(LabelInfo& info, bool render, bool exclude, bool check)
{
    // set a limit on the number of path segments
    _ASSERT(info.m_numpts < MAX_PATH_SEGMENTS);
    if (info.m_numpts >= MAX_PATH_SEGMENTS)
        return false;

    RS_FontEngine* fe = m_serenderer->GetRSFontEngine();

    // match the font and measure the sizes of the characters
    RS_TextMetrics tm;
    if (!fe->GetTextMetrics(info.m_text, info.m_tdef, tm, true))
        return false;

    // Find length of each segment in the screen space path.  We will use
    // it to position characters along the curve.  This is precomputed here
    // rather than in ComputeCharacterPositions in order to reuse the data
    // for repeated labels.
    double* seglens = (double*)alloca(sizeof(double) * info.m_numpts);
    seglens[0] = 0.0;

    for (int i=1; i<info.m_numpts; ++i)
    {
        double dx = info.m_pts[i].x - info.m_pts[i-1].x;
        double dy = info.m_pts[i].y - info.m_pts[i-1].y;
        seglens[i] = seglens[i-1] + sqrt(dx*dx + dy*dy);
    }

    // how many times should we repeat the label along the polyline?
    // TODO: fine tune this formula
    int numreps = (int)(seglens[info.m_numpts-1] / (PATH_LABEL_SEPARATION_INCHES * m_renderer->GetDpi() + tm.text_width));
    if (!numreps) numreps = 1;

    int numchars = (int)info.m_text.length();
    int labels_drawn = 0; // counter for how many of the repeated labels were accepted

    RS_F_Point* oriented_bounds = (RS_F_Point*)alloca(4 * numchars * sizeof(RS_F_Point));

    for (int irep=0; irep<numreps; ++irep)
    {
        // parametric position for current repeated label
        // positions are spaced in such a way that each label has
        // an equal amount of white space on both sides around it
        double param_position = ((double)irep + 0.5) / (double)numreps;

        // compute position and angle along the path for each character
        if (!fe->LayoutPathText(tm, info.m_pts, info.m_numpts, seglens, param_position, info.m_tdef.valign(), 0))
            continue;

        // once we have position and angle for each character
        // compute oriented bounding box for each character
        float* spacing = (float*)&tm.char_advances.front(); // bold assumption

        for (int i=0; i<numchars; ++i)
        {
            // get the character width - not exact since it takes kerning
            // into account, but should be good enough
            double char_width = spacing[i];

            // compute rotated bounds of character
            RS_F_Point* b = &oriented_bounds[i * 4];
            RotatedBounds(tm.char_pos[i].x, tm.char_pos[i].y, char_width, tm.text_height, tm.char_pos[i].anglerad, b);

#ifdef DEBUG_LABELS
            LineBuffer lb(5);
            lb.MoveTo(b[0].x, b[0].y);
            lb.LineTo(b[1].x, b[1].y);
            lb.LineTo(b[2].x, b[2].y);
            lb.LineTo(b[3].x, b[3].y);
            lb.Close();
            m_serenderer->DrawScreenPolyline(&lb, NULL, info.m_tdef.textcolor().argb(), 0.0);
#endif
        }

        //-------------------------------------------------------
        // check for overposting
        //-------------------------------------------------------

        // we need to check each character
        if (check)
        {
            for (int i=0; i<numchars; ++i)
            {
                if (OverlapsStuff(&oriented_bounds[i*4], 4))
                    goto cont_loop; // skip past label draw, but keep looping through outer loop
            }
        }

        // add bounds to exclusion regions if needed
        // once again, do this per character to get tighter bounds around the label
        if (exclude)
        {
            for (int i=0; i<numchars; ++i)
                AddExclusionRegion(&oriented_bounds[i*4], 4);
        }

        //-------------------------------------------------------
        // draw the label
        //-------------------------------------------------------

        if (render)
            fe->DrawPathText(tm, info.m_tdef);

        labels_drawn++; // increment count of how many labels we accepted

cont_loop:
        ;
    }

    return (labels_drawn > 0);
}


//////////////////////////////////////////////////////////////////////////////
std::vector<LabelInfo> LabelRenderer::StitchPolylines(std::vector<LabelInfo>& labels)
{
    std::vector<LabelInfo> src = labels; // make a copy
    std::vector<LabelInfo> ret; // store results here

    // while there are unprocessed items
    while (src.size() > 0)
    {
        // try to stitch a source item to items in return list
        size_t i;
        for (i=0; i<ret.size(); ++i)
        {
            LabelInfo& retinfo = ret[i];

            size_t j;
            for (j=0; j<src.size(); ++j)
            {
                LabelInfo& srcinfo = src[j];

                bool start_with_src = false; // start stitch with source poly?
                bool startfwd = false; // go forward on start poly?
                bool endfwd = false; // go forward on end poly?
                bool count = 0;

                if (CloseEnough(retinfo.m_pts[0], srcinfo.m_pts[0]))
                {
                    // join start to start
                    start_with_src = true; // start with source poly
                    startfwd = false;
                    endfwd = true;
                    count++;
                }
                if (CloseEnough(retinfo.m_pts[retinfo.m_numpts-1], srcinfo.m_pts[0]))
                {
                    // join end to start
                    start_with_src = false; // start with ret poly
                    startfwd = true;
                    endfwd = true;
                    count++;
                }
                if (CloseEnough(retinfo.m_pts[retinfo.m_numpts-1], srcinfo.m_pts[srcinfo.m_numpts-1]))
                {
                    start_with_src = false; // start with ret poly
                    startfwd = true;
                    endfwd = false;
                    count++;
                }
                if (CloseEnough(retinfo.m_pts[0], srcinfo.m_pts[srcinfo.m_numpts-1]))
                {
                    start_with_src = true; // start with src poly
                    startfwd = true;
                    endfwd = true;
                    count++;
                }

                if (count)
                {
                    if (count == 1)
                    {
                        // alloc new stitched polyline
                        int num_stitched_pts = retinfo.m_numpts + srcinfo.m_numpts - 1;
                        RS_F_Point* stitched = new RS_F_Point[num_stitched_pts];

                        RS_F_Point* start = start_with_src? srcinfo.m_pts : retinfo.m_pts;
                        int nstart = start_with_src? srcinfo.m_numpts : retinfo.m_numpts;

                        if (startfwd)
                        {
                            memcpy(stitched, start, sizeof(RS_F_Point) * nstart-1);
                        }
                        else
                        {
                            for (int p=0; p<nstart-1; ++p)
                                stitched[p] = start[nstart - p - 1];
                        }

                        RS_F_Point* end = start_with_src? retinfo.m_pts : srcinfo.m_pts;
                        int nend = start_with_src? retinfo.m_numpts : srcinfo.m_numpts;

                        if (endfwd)
                        {
                            memcpy(stitched + nstart-1, end, sizeof(RS_F_Point) * nend);
                        }
                        else
                        {
                            for (int p=0; p<nend; ++p)
                                stitched[p + nstart - 1] = end[nend - p - 1];
                        }

                        delete [] retinfo.m_pts;
                        retinfo.m_pts = stitched;
                        retinfo.m_numpts = num_stitched_pts;
                    }
                    else
                    {
                        // count bigger than 1 indicates that the start and
                        // endpoints of the two polylines are equal, which means
                        // we will just ignore one of them
                    }

                    break;
                }
            }

            // remove item from source list if item was stiched to something in
            // the ret list
            if (j < src.size())
            {
                src.erase(src.begin() + j);
                break;
            }
        }

        // if we did not stitch any source polyline to a polyline
        // in the return list, move a polyline to the return list and go again
        if (i == ret.size())
        {
            LabelInfo srcinfo = src.back();
            LabelInfo retinfo = srcinfo;

            // need to allocate a copy of the polyline since it will be
            // deleted and reallocated by the stitching loop
            retinfo.m_pts = new RS_F_Point[srcinfo.m_numpts];
            memcpy(retinfo.m_pts, srcinfo.m_pts, sizeof(RS_F_Point) * srcinfo.m_numpts);

            ret.push_back(retinfo);
            src.pop_back();
        }
    }

    return ret;
}
