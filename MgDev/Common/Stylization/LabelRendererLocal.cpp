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
#include "LabelRendererLocal.h"
#include "gd.h"
#include "GDRenderer.h"
#include "GDUtils.h"
#include "Centroid.h"

#define M_PI 3.14159265358979323846

//#define DEBUG_LABELS

extern int ConvertColor(gdImagePtr i, RS_Color& c);


//////////////////////////////////////////////////////////////////////////////
LabelRendererLocal::LabelRendererLocal(GDRenderer* renderer, double tileExtentOffset)
: m_renderer(renderer),
  m_bOverpostGroupOpen(false),
  m_tileExtentOffset(tileExtentOffset)
{
}


//////////////////////////////////////////////////////////////////////////////
LabelRendererLocal::~LabelRendererLocal()
{
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::StartLabels()
{
    m_overpost.Clear();
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::ProcessLabel(double x, double y, const RS_String& text, RS_TextDef& tdef)
{
    LR_LabelInfoLocal info(x, y, text, tdef);

    // here we just accumulate all the labels in a list
    if (m_bOverpostGroupOpen)
        m_labelGroups.back().m_labels.push_back(info);
    else
        ProcessLabelInternal(NULL, info, true, false, false);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::ProcessLabelGroup(RS_LabelInfo*    labels,
                                           int              nlabels,
                                           const RS_String& text,
                                           RS_OverpostType  type,
                                           bool             exclude,
                                           LineBuffer*      path)
{
    BeginOverpostGroup(type, true, exclude);

    // get the geometry type
    _ASSERT(path != NULL);
    int geomType = (path != NULL)? path->geom_type() : GisGeometryType_None;

    // in the case of linear geometry we'll label along the path, so
    // prepare for that (transform to pixels, group into stitch groups)
    if (geomType == GisGeometryType_LineString || geomType == GisGeometryType_MultiLineString)
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

            LR_LabelInfoLocal lrinfo(info->x(), info->y(), text, info->tdef());
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
    else if (geomType == GisGeometryType_Polygon || geomType == GisGeometryType_MultiPolygon)
    {
        //we only expect one label info per polygon to be passed in from stylization
        _ASSERT(nlabels == 1);

        //we want to draw all polygon labels we generate that will fit on the map
        m_labelGroups.back().m_type = RS_OverpostType_AllFit;

        //set the algorithm type correctly so that BlastLabels knows
        //how to flatten this overpost group
        m_labelGroups.back().m_algo = laPeriodicPolygon;

        //always add the centroid label, no matter how big or small the polygon is
        for (int i=0; i<nlabels; i++)
        {
            RS_LabelInfo* info = &labels[i];

            // label is in device space
            double offx = m_renderer->_MeterToMapSize(info->dunits(), info->dx());
            double offy = m_renderer->_MeterToMapSize(info->dunits(), info->dy());

            LR_LabelInfoLocal lrinfo(info->x() + offx, info->y() + offy, text, info->tdef());

            m_labelGroups.back().m_labels.push_back(lrinfo);
        }

        //for polygons that span several tiles, we will generate a list of labels,
        //one for every couple of tiles or so.
        RS_Bounds b = path->bounds();

        //tile bounds used for bounds checking and quick rejection
        //of candidate positions
        RS_Bounds tileBounds = m_renderer->GetBounds();
        RS_Bounds rejectBounds(tileBounds.minx - tileBounds.width(),
                               tileBounds.miny - tileBounds.height(),
                               tileBounds.maxx + tileBounds.width(),
                               tileBounds.maxy + tileBounds.height());

        //how many tiles does the given polygon span?
        double spanx = b.width() / tileBounds.width();
        double spany = b.height() / tileBounds.width();
        double span = rs_max(spanx, spany);

        //if the polygon spans more than 3 tiles, also add some periodic labels
        //across the extent of the polygon
        if (span >= 3.0 && spanx > 0.0 && spany > 0.0)
        {
            //one label per 1.5 tiles.
            double yinc = 1.5 / spany;
            double xinc = 1.5 / spanx;

            //starting phases of the periodic label -- center so that
            //there is equal leftover distance on top/bottom and left/right
            double yoffset = (1.0 - ((int)(1.0/yinc))*yinc) * 0.5;
            double xoffset = (1.0 - ((int)(1.0/xinc))*xinc) * 0.5;

            double ypos = yoffset;
            bool offset = false;

            //TODO: we should be smarter about the start and end parametric positions
            //instead of looping from  around 0 to  around 1. If we are far zoomed in
            //on a polygon, those loops can take quite some time to execute
            while (ypos <= 1.0)
            {
                double xpos = xoffset + ((offset) ? 0.5 * xinc : 0.0);

                while (xpos <= 1.0)
                {
                    //compute mapping space position for label, based on
                    double posx = b.minx + xpos * b.width();
                    double posy = b.miny + ypos * b.height();

                    //rejection of labels that are far outside the current tile
                    //and also make sure the point we genrated is inside the polygon
                    if (posx <= rejectBounds.maxx && posx >= rejectBounds.minx
                        && posy <= rejectBounds.maxy && posy >= rejectBounds.miny
                        && Centroid::PointInPolygon(path->points(), 2 * path->point_count(),
                                path->cntrs(), path->cntr_count(), posx, posy ))
                    {
                        RS_LabelInfo* info = &labels[0]; //assumes one label info passed in

                        LR_LabelInfoLocal lrinfo(posx, posy, text, info->tdef());
                        m_labelGroups.back().m_labels.push_back(lrinfo);
                    }

                    xpos += xinc;
                }

                offset = !offset;
                ypos += yinc;
            }
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

            LR_LabelInfoLocal lrinfo(info->x() + offx, info->y() + offy, text, info->tdef());

            m_labelGroups.back().m_labels.push_back(lrinfo);
        }
    }

    // remember the feature bounds for the label group
    m_labelGroups.back().m_feature_bounds = path->bounds();

    EndOverpostGroup();
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::BeginOverpostGroup(RS_OverpostType type, bool render, bool exclude)
{
    m_bOverpostGroupOpen = true;

    // add a new group
    LR_OverpostGroupLocal group(render, exclude, type);
    m_labelGroups.push_back(group);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::EndOverpostGroup()
{
    m_bOverpostGroupOpen = false;

    // don't add empty groups
    if (m_labelGroups.back().m_labels.size() == 0)
        m_labelGroups.pop_back();
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::BlastLabels()
{
    //-------------------------------------------------------
    // step 1 - perform stitching
    //-------------------------------------------------------

    for (size_t i=0; i<m_labelGroups.size(); i++)
    {
        LR_OverpostGroupLocal& group = m_labelGroups[i];

        if (group.m_algo == laCurve && group.m_labels.size() > 1)
        {
            std::vector<LR_LabelInfoLocal> stitched = StitchPolylines(group.m_labels);
            if (stitched.size() > 0)
            {
                // replace the existing vector of labels with the stitched one
                for (size_t j=0; j<group.m_labels.size(); j++)
                {
                    LR_LabelInfoLocal& info = group.m_labels[j];
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
    // step 2 - compute bounds for all the labels
    //-------------------------------------------------------


    for (size_t i=0; i<m_labelGroups.size(); i++)
    {
        LR_OverpostGroupLocal& group = m_labelGroups[i];

        std::vector<LR_LabelInfoLocal> repeated_infos;

        for (size_t j=0; j<group.m_labels.size(); j++)
        {
            LR_LabelInfoLocal& info = group.m_labels[j];

            bool success = false;

            if (info.m_pts)
            {
                //several possible positions along the path
                //may be returned in the case of repeated labels
                success = ComputePathLabelBounds(info, repeated_infos);
            }
            else
            {
                success = ComputeSimpleLabelBounds(info);

                //simple label --> simply add one instance of it
                //to the repeated infos collection. When we add repeated
                //labels for polygons, this code will change.
                LR_LabelInfoLocal copy = info;
                copy.m_pts = NULL;
                copy.m_numpts = 0;
                repeated_infos.push_back(copy);
            }

            if (!success)
            {
                // we ran into a problem, so ignore this group
                group.m_render = false;
                group.m_exclude = false;
                break;
            }
        }

        //now replace the group's label infos, by the new repeated
        //infos collection -- also free the polyline data stored
        //in the m_pts members of the original infos, since we will
        //no longer need the geometry data
        for (size_t j=0; j<group.m_labels.size(); j++)
        {
            LR_LabelInfoLocal& info = group.m_labels[j];
            if (info.m_pts)
            {
                delete [] info.m_pts;
                info.m_pts = NULL;
                info.m_numpts = 0;
            }
        }

        group.m_labels = repeated_infos;
    }

    //-------------------------------------------------------
    // step 3 - flatten group list
    //-------------------------------------------------------

    std::vector<LR_OverpostGroupLocal> finalGroups;
    for (size_t i=0; i<m_labelGroups.size(); i++)
    {
        LR_OverpostGroupLocal& group = m_labelGroups[i];

        // for path labels, put each label into a separate group so
        // the sorting code below treats each label independently
        if (group.m_algo == laCurve || group.m_algo == laPeriodicPolygon)
        {
            for (size_t j=0; j<group.m_labels.size(); j++)
            {
                // create a new group with just one label
                LR_OverpostGroupLocal newGroup(group.m_render, group.m_exclude, group.m_type);
                newGroup.m_algo           = group.m_algo;
                newGroup.m_feature_bounds = group.m_feature_bounds;
                newGroup.m_labels.push_back(group.m_labels[j]);
                finalGroups.push_back(newGroup);
            }
        }
        else
        {
            finalGroups.push_back(group);
        }
    }

    // done with these
    m_labelGroups.clear();

    //-------------------------------------------------------
    // step 4 - sort all the label groups into buckets
    //-------------------------------------------------------

    RS_Bounds& tileBounds = m_renderer->GetBounds();
    double tileMinX = tileBounds.minx;
    double tileMaxX = tileBounds.maxx;
    double tileMinY = tileBounds.miny;
    double tileMaxY = tileBounds.maxy;
    double tileWid  = tileBounds.width();
    double tileHgt  = tileBounds.height();

    std::vector<LR_OverpostGroupLocal*> groupsC00;  // bottom left shared corner
    std::vector<LR_OverpostGroupLocal*> groupsC10;  // bottom right shared corner
    std::vector<LR_OverpostGroupLocal*> groupsC01;  // top left shared corner
    std::vector<LR_OverpostGroupLocal*> groupsC11;  // top right shared corner

    std::vector<LR_OverpostGroupLocal*> groupsEx0;  // left shared edge
    std::vector<LR_OverpostGroupLocal*> groupsEx1;  // right shared edge
    std::vector<LR_OverpostGroupLocal*> groupsEy0;  // bottom shared edge
    std::vector<LR_OverpostGroupLocal*> groupsEy1;  // top shared edge

    std::vector<LR_OverpostGroupLocal*> groupsCtr;  // completely inside

    for (size_t i=0; i<finalGroups.size(); i++)
    {
        LR_OverpostGroupLocal& group = finalGroups[i];
        if (!group.m_render && !group.m_exclude)
            continue;

        //-------------------------------------------------------
        // get the bounds for the group as a whole
        //-------------------------------------------------------

        double minX = +DBL_MAX;
        double maxX = -DBL_MAX;
        double minY = +DBL_MAX;
        double maxY = -DBL_MAX;
        for (size_t j=0; j<group.m_labels.size(); j++)
        {
            LR_LabelInfoLocal& info = group.m_labels[j];

            // just iterate over the oriented bounds
            for (size_t k=0; k<info.m_numelems*4; k++)
            {
                // don't need to worry about y-orientation in this case
                double x = m_renderer->_ITXD(info.m_oriented_bounds[k].x);
                double y = m_renderer->_ITYD(info.m_oriented_bounds[k].y);

                minX = rs_min(minX, x);
                maxX = rs_max(maxX, x);
                minY = rs_min(minY, y);
                maxY = rs_max(maxY, y);
            }
        }

        //-------------------------------------------------------
        // check common cases for which we can skip the label
        //-------------------------------------------------------

        if (maxX < tileMinX)                        // completely past the left edge
            continue;
        if (minX > tileMaxX)                        // completely past the right edge
            continue;
        if (minX <= tileMinX && maxX >= tileMaxX)   // intersecting both the left and right edges
            continue;                               // TODO: find a way to remove this limitation

        if (maxY < tileMinY)                        // completely past the bottom edge
            continue;
        if (minY > tileMaxY)                        // completely past the top edge
            continue;
        if (minY <= tileMinY && maxY >= tileMaxY)   // intersecting both the bottom and top edges
            continue;                               // TODO: find a way to remove this limitation

        //-------------------------------------------------------
        // apply label rejection logic
        //-------------------------------------------------------

        // We have the bounds for the label group, and therefore we know
        // the range of tiles it spans.  To ensure the group is properly
        // handled, we need to make sure that each tile can actually "see"
        // the group.  A tile will "see" a label group if the bounds of
        // the group's feature intersects the request extent for the tile.

        // tiles with labels always use the same request extent offset
        double offset = m_tileExtentOffset * tileWid;

        // the min/max tile indices for the label group
        int tMinX = (int)floor((minX - tileMinX) / tileWid);
        int tMaxX = (int)floor((maxX - tileMinX) / tileWid);
        int tMinY = (int)floor((minY - tileMinY) / tileHgt);
        int tMaxY = (int)floor((maxY - tileMinY) / tileHgt);

        // check if the leftmost tile can "see" the group
        double reqMaxX = tileMinX + (double)(tMinX + 1) * tileWid + offset; // right edge of the tile's request extent
        if (group.m_feature_bounds.minx > reqMaxX)                          // feature lies to the right of this edge
            continue;

        // check if the rightmost tile can "see" the group
        double reqMinX = tileMinX + (double)(tMaxX    ) * tileWid - offset; // left edge of the tile's request extent
        if (group.m_feature_bounds.maxx < reqMinX)                          // feature lies to the left of this edge
            continue;

        // check if the bottommost tile can "see" the group
        double reqMaxY = tileMinY + (double)(tMinY + 1) * tileHgt + offset; // top edge of tile's request extent
        if (group.m_feature_bounds.miny > reqMaxY)                          // feature lies above this edge
            continue;

        // check if the topmost tile can "see" the group
        double reqMinY = tileMinY + (double)(tMaxY    ) * tileHgt - offset; // bottom edge of tile's request extent
        if (group.m_feature_bounds.maxy < reqMinY)                          // feature lies below this edge
            continue;

        //-------------------------------------------------------
        // check more involved cases
        //-------------------------------------------------------

        if (minX <= tileMinX && maxX < tileMaxX)    // intersecting the left edge
        {
            if (minY <= tileMinY && maxY < tileMaxY)    // intersecting the bottom edge
            {
                // bottom left shared corner
                groupsC00.push_back(&group);
                continue;
            }

            if (maxY >= tileMaxY && minY > tileMinY)    // intersecting the top edge
            {
                // top left shared corner
                groupsC01.push_back(&group);
                continue;
            }

            if (minY > tileMinY && maxY < tileMaxY)     // inside both the bottom and top edges
            {
                // left shared edge
                groupsEx0.push_back(&group);
                continue;
            }
        }

        if (maxX >= tileMaxX && minX > tileMinX)    // intersecting the right edge
        {
            if (minY <= tileMinY && maxY < tileMaxY)    // intersecting the bottom edge
            {
                // bottom right shared corner
                groupsC10.push_back(&group);
                continue;
            }

            if (maxY >= tileMaxY && minY > tileMinY)    // intersecting the top edge
            {
                // top right shared corner
                groupsC11.push_back(&group);
                continue;
            }

            if (minY > tileMinY && maxY < tileMaxY)     // inside both the bottom and top edges
            {
                // right shared edge
                groupsEx1.push_back(&group);
                continue;
            }
        }

        if (minX > tileMinX && maxX < tileMaxX)     // inside both the left and right edges
        {
            if (minY <= tileMinY && maxY < tileMaxY)    // intersecting the bottom edge
            {
                // bottom shared edge
                groupsEy0.push_back(&group);
                continue;
            }

            if (maxY >= tileMaxY && minY > tileMinY)    // intersecting the top edge
            {
                // top shared edge
                groupsEy1.push_back(&group);
                continue;
            }

            if (minY > tileMinY && maxY < tileMaxY)     // inside both the bottom and top edges
            {
                // completely inside
                groupsCtr.push_back(&group);
                continue;
            }
        }

        // we missed a case if we hit this assert goes off
        _ASSERT(false);
    }

    //-------------------------------------------------------
    // step 5 - apply overpost algorithm to each shared corner
    //-------------------------------------------------------

    // bottom left shared corner
    SimpleOverpost mgrC00;
    mgrC00.AddRegions(m_overpost);
    ProcessLabelGroupsInternal(&mgrC00, groupsC00);

    // bottom right shared corner
    SimpleOverpost mgrC10;
    mgrC10.AddRegions(m_overpost);
    ProcessLabelGroupsInternal(&mgrC10, groupsC10);

    // top left shared corner
    SimpleOverpost mgrC01;
    mgrC01.AddRegions(m_overpost);
    ProcessLabelGroupsInternal(&mgrC01, groupsC01);

    // top right shared corner
    SimpleOverpost mgrC11;
    mgrC11.AddRegions(m_overpost);
    ProcessLabelGroupsInternal(&mgrC11, groupsC11);

    //-------------------------------------------------------
    // step 6 - apply overpost algorithm to each shared edge
    //-------------------------------------------------------

    // left shared edge
    SimpleOverpost mgrEx0;
    mgrEx0.AddRegions(mgrC00);
    mgrEx0.AddRegions(mgrC01);
    ProcessLabelGroupsInternal(&mgrEx0, groupsEx0);

    // right shared edge
    SimpleOverpost mgrEx1;
    mgrEx1.AddRegions(mgrC10);
    mgrEx1.AddRegions(mgrC11);
    ProcessLabelGroupsInternal(&mgrEx1, groupsEx1);

    // bottom shared edge
    SimpleOverpost mgrEy0;
    mgrEy0.AddRegions(mgrC00);
    mgrEy0.AddRegions(mgrC10);
    ProcessLabelGroupsInternal(&mgrEy0, groupsEy0);

    // top shared edge
    SimpleOverpost mgrEy1;
    mgrEy1.AddRegions(mgrC01);
    mgrEy1.AddRegions(mgrC11);
    ProcessLabelGroupsInternal(&mgrEy1, groupsEy1);

    //-------------------------------------------------------
    // step 7 - apply overpost algorithm to center
    //-------------------------------------------------------

    // center
    SimpleOverpost mgrCtr;
    mgrCtr.AddRegions(mgrEx0);
    mgrCtr.AddRegions(mgrEx1);
    mgrCtr.AddRegions(mgrEy0);
    mgrCtr.AddRegions(mgrEy1);
    ProcessLabelGroupsInternal(&mgrCtr, groupsCtr);

    //-------------------------------------------------------
    // step 8 - clean up label info
    //-------------------------------------------------------

    for (size_t i=0; i<finalGroups.size(); i++)
    {
        LR_OverpostGroupLocal& group = finalGroups[i];
        for (size_t j=0; j<group.m_labels.size(); j++)
        {
            LR_LabelInfoLocal& info = group.m_labels[j];

            if (info.m_charpos)
            {
                delete [] info.m_charpos;
                info.m_charpos = NULL;
            }

            if (info.m_oriented_bounds)
            {
                delete [] info.m_oriented_bounds;
                info.m_oriented_bounds = NULL;
            }

            info.m_numpts = 0;
            info.m_numelems = 0;
        }
    }

    finalGroups.clear();
    m_hStitchTable.clear();
    m_overpost.Clear();
}

//////////////////////////////////////////////////////////////////////////////
bool LabelRendererLocal::ComputeSimpleLabelBounds(LR_LabelInfoLocal& info)
{
    //determine pixel space font height
    info.m_hgt = m_renderer->_MeterToMapSize(info.m_tdef.font().units(), info.m_tdef.font().height());
    info.m_hgt *= m_renderer->GetMapToScreenScale() * 0.75;

    // The computed height can have roundoff in it, and the rendering code is
    // very sensitive to it.  Remove this roundoff by rounding the height to
    // the nearest 1/65536ths of a point.
    info.m_hgt *= 65536.0;
    info.m_hgt = (int)(info.m_hgt + 0.5);
    info.m_hgt /= 65536.0;

    //radian CCW rotation
    double rotation = info.m_tdef.rotation() * M_PI / 180.0;

    //-------------------------------------------------------
    // text extent and alignment computation
    //-------------------------------------------------------

    //font matching
    if (!m_renderer->FindFont(info.m_tdef.font(), info.m_fontpath))
        return false;

    //get overall extent and char spacing
    RS_F_Point fpts[4];
    float* spacing = (float*)alloca(info.m_text.length() * 2 * sizeof(float));
    m_renderer->MeasureString(info.m_text.c_str(), info.m_hgt, info.m_fontpath, 0.0, fpts, spacing);

    //the width and height of this particular string
    //TODO: we can be more precise if we get height on per character basis
    info.m_texthgt = fabs(fpts[2].y - fpts[0].y);
    info.m_textwid = fabs(fpts[1].x - fpts[0].x);

    //get overall string size and char spacing, including rotation
    m_renderer->MeasureString(info.m_text.c_str(), info.m_hgt, info.m_fontpath, rotation, fpts, NULL);

    //transform insertion point into pixel space
    info.m_ins_point.x = m_renderer->_TXD(info.m_x);
    info.m_ins_point.y = m_renderer->_TYD(info.m_y);

    for (int i=0; i<4; i++)
    {
        fpts[i].x += info.m_ins_point.x;
        fpts[i].y += info.m_ins_point.y;
    }

    //apply text alignment to bounding box
    //this will modify both the insertion point and the extent
    ApplyTextAlignment(info.m_tdef, info.m_hgt, fpts, info.m_ins_point);

    //allocate the data we need
    info.m_numelems = 1;
    info.m_oriented_bounds = new RS_F_Point[4];
    if (info.m_oriented_bounds == NULL)
        return false;

    //store the oriented bounds with the label
    for (int i=0; i<4; i++)
    {
        info.m_oriented_bounds[i] = fpts[i];
    }

#ifdef DEBUG_LABELS
    static int featIdS = -1;
    featIdS++;

    RS_Color clrR(255,   0,   0, 255);
    RS_Color clrG(  0, 255,   0, 255);
    RS_Color clrB(  0,   0, 255, 255);
    RS_Color clrO(255, 128,   0, 255);
/*
    // this debugging code draws the feature geometry using a thick
    // brush, with the color alternating between blue and orange
    gdImagePtr brush = rs_gdImageThickLineBrush(2, ((featIdS % 2)==0)? clrB : clrO);
    gdImageSetBrush((gdImagePtr)m_renderer->GetImage(), brush);

    printf("numPts=%d\n", info.m_numpts);
    for (int j=1; j<info.m_numpts; j++)
    {
        RS_D_Point dpts[2];
        dpts[0].x = (int)info.m_pts[j-1].x;
        dpts[0].y = (int)info.m_pts[j-1].y;
        dpts[1].x = (int)info.m_pts[j  ].x;
        dpts[1].y = (int)info.m_pts[j  ].y;
        gdImagePolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 2, gdBrushed);
    }

    gdImageSetBrush((gdImagePtr)m_renderer->GetImage(), NULL);
    gdImageDestroy(brush);
*/
    // this debugging code draws a box around the label (using its bounds),
    // with the color cycling between red, green, and blue
    RS_D_Point dpts[4];
    for (int j=0; j<4; j++)
    {
        dpts[j].x = (int)fpts[j].x;
        dpts[j].y = (int)fpts[j].y;
    }
    gdImagePolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), ((featIdS % 3)==0)? clrR : ((featIdS % 3)==1)? clrG : clrB));
//  gdImagePolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.color()));
#endif

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererLocal::ComputePathLabelBounds(LR_LabelInfoLocal& info, std::vector<LR_LabelInfoLocal>& repeated_infos)
{
    //determine pixel space font height
    info.m_hgt = m_renderer->_MeterToMapSize(info.m_tdef.font().units(), info.m_tdef.font().height());
    info.m_hgt *= m_renderer->GetMapToScreenScale() * 0.75;

    // The computed height can have roundoff in it, and the rendering code is
    // very sensitive to it.  Remove this roundoff by rounding the height to
    // the nearest 1/65536ths of a point.
    info.m_hgt *= 65536.0;
    info.m_hgt = (int)(info.m_hgt + 0.5);
    info.m_hgt /= 65536.0;

    //-------------------------------------------------------
    // text extent and alignment computation
    //-------------------------------------------------------

    //font matching
    if (!m_renderer->FindFont(info.m_tdef.font(), info.m_fontpath))
        return false;

    //get overall extent and char spacing
    RS_F_Point fpts[4];
    float* spacing = (float*)alloca(info.m_text.length() * 2 * sizeof(float));
    m_renderer->MeasureString(info.m_text.c_str(), info.m_hgt, info.m_fontpath, 0.0, fpts, spacing);

    //the width and height of this particular string
    //TODO: we can be more precise if we get height on per character basis
    info.m_texthgt = fabs(fpts[2].y - fpts[0].y);
    info.m_textwid = fabs(fpts[1].x - fpts[0].x);

    //allocate the data we need
    info.m_numelems = info.m_text.length();

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
    int numreps = (int)(seglens[info.m_numpts-1] / (200.0 + info.m_textwid));
    if (!numreps) numreps = 1;

    for (int i=0; i<numreps; i++)
    {
        //Make a copy of the modified label data for the current label period
        //The copy takes ownership of the CharPos array and oriented bounds array,
        //but does not take ownership of the actual path data, which belongs to the
        //original label info structure.
        LR_LabelInfoLocal copy_info = info;
        copy_info.m_pts = NULL;
        copy_info.m_numpts = 0;

        copy_info.m_charpos = new CharPos[copy_info.m_numelems];
        if (copy_info.m_charpos == NULL)
            return false;

        copy_info.m_oriented_bounds = new RS_F_Point[4*copy_info.m_numelems];
        if (copy_info.m_oriented_bounds == NULL)
            return false;

        //parametric position for current repeated label
        //positions are spaced in such a way that each label has
        //an equal amount of white space on both sides around it
        double param_position = ((double)i + 0.5) / (double)numreps;

        //compute position and angle along the path for each character
        double font_scale = ComputeCharacterPositions(info, seglens, param_position, spacing, copy_info.m_textwid, copy_info.m_charpos);

        //update font height to value that curve fitting suggested
        if (font_scale != 1.0)
        {
            //don't bother measuring the string with the new height,
            //scaling should work almost as good
            copy_info.m_hgt     *= font_scale;
            copy_info.m_texthgt *= font_scale;
            copy_info.m_textwid *= font_scale;
        }

        //take into account text vertical alignment
        //horizontal alignment is ignored in this case
        for (size_t i=0; i<copy_info.m_numelems; i++)
        {
            //TODO:
            double font_cap_height = copy_info.m_hgt;
            double font_ascent     = copy_info.m_hgt;
            double font_descent    = 0;

            // get the height vector
            double dyh = 0.0;

            RS_VAlignment vAlign = copy_info.m_tdef.valign();
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
            double angle = copy_info.m_charpos[i].anglerad;

            //take into account that y axis goes down in our coordinate system
            dyh = -dyh;

            double cs = cos(angle);
            double sn = sin(angle);
            double hAlignOffset = -sn*dyh;
            double vAlignOffset =  cs*dyh;

            //adjust insertion point
            copy_info.m_charpos[i].x += hAlignOffset;
            copy_info.m_charpos[i].y -= vAlignOffset;
        }

        //once we have position and angle for each character
        //compute oriented bounding box for each character
        double total_advance = 0.0;
        for (size_t i=0; i<copy_info.m_numelems; i++)
        {
            //width of character - not really exact width since
            //it takes kerning into account, but should be good enough
            //we could measure each character separately but that seems like
            //too many calls to FreeType
            double advance = (i == copy_info.m_numelems-1)? copy_info.m_textwid * font_scale - total_advance : spacing[i] * font_scale;
            total_advance += advance;

            //compute rotated bounds of character
            RS_F_Point* b = &copy_info.m_oriented_bounds[i * 4];
            RotatedBounds(copy_info.m_charpos[i].x, copy_info.m_charpos[i].y, advance, copy_info.m_texthgt, copy_info.m_charpos[i].anglerad, b);

    #ifdef DEBUG_LABELS
            static int featIdP = -1;
            if (i == 0) featIdP++;

            RS_Color clrR(255,   0,   0, 255);
            RS_Color clrG(  0, 255,   0, 255);
            RS_Color clrB(  0,   0, 255, 255);
            RS_Color clrO(255, 128,   0, 255);

            // this debugging code draws a box around the label's characters
            // with the color cycling between red, green, and blue
            RS_D_Point dpts[4];
            for (int j=0; j<4; j++)
            {
                dpts[j].x = (int)b[j].x;
                dpts[j].y = (int)b[j].y;
            }
            gdImagePolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), ((featIdP % 3)==0)? clrR : ((featIdP % 3)==1)? clrG : clrB));
    //      gdImagePolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.color()));
    #endif
        }

        //add current periodic label to the return list
        repeated_infos.push_back(copy_info);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////
//computes position and rotation of each character along the
//feature to be labeled -- it may return a scale factor to be
//applied to the font size to make the label fit better in the
//allotted space
double LabelRendererLocal::ComputeCharacterPositions(LR_LabelInfoLocal& info, double* seglens, double position, float* kerned_spacing, double measured_width, CharPos* ret)
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
    //initialize to start position
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
void LabelRendererLocal::ProcessLabelGroupsInternal(SimpleOverpost* pMgr, std::vector<LR_OverpostGroupLocal*>& groups)
{
    for (size_t i=0; i<groups.size(); i++)
    {
        LR_OverpostGroupLocal* pGroup = groups[i];

        for (size_t j=0; j<pGroup->m_labels.size(); j++)
        {
            LR_LabelInfoLocal& info = pGroup->m_labels[j];
            bool res = ProcessLabelInternal(pMgr,
                                            info,
                                            pGroup->m_render,
                                            pGroup->m_exclude,
                                            pGroup->m_type != RS_OverpostType_All);

            // only in the case of a simple label do we check the overpost type
            if (pGroup->m_algo == laSimple)
            {
                if (res && (pGroup->m_type == RS_OverpostType_FirstFit))
                    break;
            }
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererLocal::ProcessLabelInternal(SimpleOverpost* pMgr, LR_LabelInfoLocal& info,
                                              bool render, bool exclude, bool check)
{
    _ASSERT(pMgr != NULL || !check);

    //-------------------------------------------------------
    // check for overposting
    //-------------------------------------------------------

    // check for overlap
    if (check)
    {
        // check all the elements
        for (size_t i=0; i<info.m_numelems; i++)
        {
            if (OverlapsStuff(pMgr, &info.m_oriented_bounds[i*4], 4))
                return false;
        }
    }

    // add bounds to exclusion regions if needed
    if (exclude)
    {
        // once again, do this per element to get tighter bounds around the label
        for (size_t i=0; i<info.m_numelems; i++)
        {
            AddExclusionRegion(pMgr, &info.m_oriented_bounds[i*4], 4);
        }
    }

    //-------------------------------------------------------
    // draw the label
    //-------------------------------------------------------

    if (render)
    {
        // call the appropriate routine
        if (info.m_charpos != NULL)
            DrawPathLabel(info);
        else
            DrawSimpleLabel(info);
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::DrawSimpleLabel(LR_LabelInfoLocal& info)
{
    //radian CCW rotation
    double rotation = info.m_tdef.rotation() * M_PI / 180.0;

    //ghosting offset
    int offset = 0;

    //take into account ghosting making text bounds bigger
    if (info.m_tdef.textbg() == RS_TextBackground_Ghosted)
        offset = 1;

    int posx = (int)floor(info.m_ins_point.x+0.5);
    int posy = (int)floor(info.m_ins_point.y+0.5);

    if (info.m_tdef.textbg() == RS_TextBackground_Ghosted)
    {
        m_renderer->DrawString(info.m_text, posx-offset, posy, info.m_hgt, info.m_fontpath, info.m_tdef.bgcolor(), rotation);
        m_renderer->DrawString(info.m_text, posx+offset, posy, info.m_hgt, info.m_fontpath, info.m_tdef.bgcolor(), rotation);
        m_renderer->DrawString(info.m_text, posx, posy-offset, info.m_hgt, info.m_fontpath, info.m_tdef.bgcolor(), rotation);
        m_renderer->DrawString(info.m_text, posx, posy+offset, info.m_hgt, info.m_fontpath, info.m_tdef.bgcolor(), rotation);
    }
    else if (info.m_tdef.textbg() == RS_TextBackground_Opaque)
    {
        //set up rotated bounds
        RS_F_Point b[4];
        RotatedBounds(info.m_ins_point.x, info.m_ins_point.y, info.m_textwid, info.m_texthgt, rotation, b);

        RS_D_Point dpts[4];
        for (int j=0; j<4; j++)
        {
            dpts[j].x = (int)b[j].x;
            dpts[j].y = (int)b[j].y;
        }

        gdImageFilledPolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.bgcolor()));
    }

    m_renderer->DrawString(info.m_text, posx, posy, info.m_hgt, info.m_fontpath, info.m_tdef.color(), rotation);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::DrawPathLabel(LR_LabelInfoLocal& info)
{
    wchar_t c[] = {0, 0};

    //ghosting offset
    int offset = 0;

    //take into account ghosting making text bounds bigger
    if (info.m_tdef.textbg() == RS_TextBackground_Ghosted)
        offset = 1;

    //draw the characters, each in its computed position
    for (size_t i=0; i<info.m_numelems; i++)
    {
        c[0] = info.m_text[i];

        //compute screen position and round
        int posx = (int)(info.m_charpos[i].x+0.5);
        int posy = (int)(info.m_charpos[i].y+0.5);

        if (info.m_tdef.textbg() == RS_TextBackground_Ghosted)
        {
            m_renderer->DrawString(c, posx-offset, posy, info.m_hgt, info.m_fontpath, info.m_tdef.bgcolor(), info.m_charpos[i].anglerad);
            m_renderer->DrawString(c, posx+offset, posy, info.m_hgt, info.m_fontpath, info.m_tdef.bgcolor(), info.m_charpos[i].anglerad);
            m_renderer->DrawString(c, posx, posy-offset, info.m_hgt, info.m_fontpath, info.m_tdef.bgcolor(), info.m_charpos[i].anglerad);
            m_renderer->DrawString(c, posx, posy+offset, info.m_hgt, info.m_fontpath, info.m_tdef.bgcolor(), info.m_charpos[i].anglerad);
        }

        m_renderer->DrawString(c, posx, posy, info.m_hgt, info.m_fontpath, info.m_tdef.color(), info.m_charpos[i].anglerad);
    }
}


//////////////////////////////////////////////////////////////////////////////
//modifies the text extent (and insertion point) to take into account text alignment
void LabelRendererLocal::ApplyTextAlignment(RS_TextDef& tdef, double actual_height, RS_F_Point* extent, RS_F_Point& ins_point)
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

    //double em_square_size = fm.m_em_square;
    double font_cap_height = actual_height; //fm.m_cap_em_height * actual_height / em_square_size;
    double font_ascent     = actual_height; //fm.m_ascent        * actual_height / em_square_size;
    double font_descent    = 0; //fm.m_descent       * actual_height / em_square_size;

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
void LabelRendererLocal::AddExclusionRegion(RS_F_Point* pts, int npts)
{
    AddExclusionRegion(&m_overpost, pts, npts);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::AddExclusionRegion(SimpleOverpost* pMgr, RS_F_Point* pts, int npts)
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

    pMgr->AddRegion(axis_bounds);
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererLocal::OverlapsStuff(SimpleOverpost* pMgr, RS_F_Point* pts, int npts)
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

    return pMgr->Overlaps(axis_bounds);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::DeviceToMappingBounds(RS_Bounds& b)
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
void LabelRendererLocal::ComputeBounds(RS_F_Point* RESTRICT pts, int npts, RS_Bounds& b)
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
//Applies a given angle to an axis aligned bounding box.
//Rotation point is lower left
inline void LabelRendererLocal::RotatedBounds(double x, double y, double width, double height, double angle_cw_rad, RS_F_Point* b)
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
std::vector<LR_LabelInfoLocal> LabelRendererLocal::StitchPolylines(std::vector<LR_LabelInfoLocal>& labels)
{
    std::vector<LR_LabelInfoLocal> src = labels; //make a copy
    std::vector<LR_LabelInfoLocal> ret; //store results here

    //while there are unprocessed items
    while (src.size() > 0)
    {
        //try to stitch a source item to items in return list
        size_t i;
        for (i=0; i<ret.size(); i++)
        {
            LR_LabelInfoLocal& retinfo = ret[i];

            size_t j;
            for (j=0; j<src.size(); j++)
            {
                LR_LabelInfoLocal& srcinfo = src[j];

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
            LR_LabelInfoLocal srcinfo = src.back();
            LR_LabelInfoLocal retinfo = srcinfo;

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
bool LabelRendererLocal::CloseEnough(RS_F_Point& p1, RS_F_Point& p2)
{
    double delta = fabs(p2.y - p1.y) + fabs(p2.x - p1.x);
    return (delta <= 2.0); //2 pixels is close enough
}
