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
#include "LabelRendererLocal.h"
#include "SE_Renderer.h"

//#define DEBUG_LABELS

extern void ProcessStylizerException(FdoException* exception, int line, wchar_t* file);


//////////////////////////////////////////////////////////////////////////////
LabelRendererLocal::LabelRendererLocal(SE_Renderer* se_renderer, double tileExtentOffset)
: LabelRendererBase(se_renderer)
, m_tileExtentOffset(tileExtentOffset)
{
}


//////////////////////////////////////////////////////////////////////////////
LabelRendererLocal::~LabelRendererLocal()
{
    try
    {
        Cleanup();
    }
    catch (...)
    {
    }
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::Cleanup()
{
    for (size_t i=0; i<m_labelGroups.size(); ++i)
    {
        OverpostGroupLocal& group = m_labelGroups[i];

        for (size_t j=0; j<group.m_labels.size(); ++j)
        {
            LabelInfoLocal& info = group.m_labels[j];

            delete [] info.m_rotated_points;
            info.m_rotated_points = NULL;

            delete [] info.m_pts;
            info.m_pts = NULL;
            info.m_numpts = 0;

            // the style was cloned when it was passed to the LabelRenderer
            delete info.m_sestyle;
            info.m_sestyle = NULL;
        }
    }

    m_labelGroups.clear();
    m_hStitchTable.clear();
    m_overpost.Clear();
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::StartLabels()
{
    m_overpost.Clear();
}


//////////////////////////////////////////////////////////////////////////////
// non-SE symbol labels
void LabelRendererLocal::ProcessLabelGroup(RS_LabelInfo*    labels,
                                           int              nlabels,
                                           const RS_String& text,
                                           RS_OverpostType  type,
                                           bool             exclude,
                                           LineBuffer*      path,
                                           double           scaleLimit)
{
    BeginOverpostGroup(type, true, exclude);

    // get the geometry type
    int geomType = (path != NULL)? path->geom_type() : FdoGeometryType_None;

    // in the case of linear geometry we'll label along the path, so prepare
    // for that (transform to screen space, group into stitch groups)
    if (geomType == FdoGeometryType_LineString || geomType == FdoGeometryType_MultiLineString)
    {
        // indicate that the current group will be labeled along the path
        m_labelGroups.back().m_algo = laCurve;

        // set the scale limit for the group
        m_labelGroups.back().m_scaleLimit = scaleLimit;

        // Since we're in tiled mode, we cannot stitch features which span
        // more than one tile because labels will not be continuous across
        // tiles -- i.e. computed label positions will be different since
        // some features will not come in with the spatial query for some
        // far away tiles, even though they should for labeling purposes.
        // This does not affect stitching of multi-linestring features which
        // we can always stitch without worrying about parts of them not
        // coming in on some tiles.
        RS_String stitch_key = text;

        // TODO: stitch in subregions of tile
//      const RS_Bounds& tileBounds = m_serenderer->GetBounds();
//      const RS_Bounds& featBounds = path->bounds();
//      if (featBounds.minx < tileBounds.minx ||
//          featBounds.maxx > tileBounds.maxx ||
//          featBounds.miny < tileBounds.miny ||
//          featBounds.maxy > tileBounds.maxy)
        {
            // If we don't want to stitch separate line features, mangle
            // the stitch table key to make it unique across features, but
            // keep it the same when we stitch the parts of a linestring.
            wchar_t tmp[32];
            swprintf(tmp, 32, L"%d", m_labelGroups.size());
            stitch_key += tmp;
        }

        // If it's a multi-linestring, we will add each separate linestring
        // piece for labeling.  Some of these will get stitched together
        // later, but if they are far away we will essentially label each
        // one by itself.

        RS_F_Point* lblpath = NULL;
        int lblpathpts = 0;
        int offset = 0;

        for (int i=0; i<path->cntr_count(); ++i)
        {
            // now transform the points of the current contour to screen space
            lblpathpts = path->cntr_size(i);
            lblpath = new RS_F_Point[lblpathpts];

            for (int b=0; b<lblpathpts; ++b)
                m_serenderer->WorldToScreenPoint(path->x_coord(offset+b), path->y_coord(offset+b), lblpath[b].x, lblpath[b].y);

            _ASSERT(nlabels == 1);
            RS_LabelInfo* info = &labels[0];    // TODO: assumes one label

            LabelInfoLocal lrinfo(info->x(), info->y(), text, info->tdef());
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
        }
    }
    else if (geomType == FdoGeometryType_Polygon || geomType == FdoGeometryType_MultiPolygon)
    {
        // we only expect one label info per polygon to be passed in from stylization
        _ASSERT(nlabels == 1);

        // set the algorithm type correctly so that BlastLabels knows
        // how to flatten this overpost group
        m_labelGroups.back().m_algo = laPeriodicPolygon;

        // always add the centroid label, no matter how big or small the polygon is
        for (int i=0; i<nlabels; ++i)
        {
            RS_LabelInfo* info = &labels[i];

            // label is in device space
            double offx = MeterToMapSize(info->dunits(), info->dx());
            double offy = MeterToMapSize(info->dunits(), info->dy());

            LabelInfoLocal lrinfo(info->x() + offx, info->y() + offy, text, info->tdef());

            m_labelGroups.back().m_labels.push_back(lrinfo);
        }

        // for polygons that span several tiles, we will generate a list of labels,
        // one for every couple of tiles or so.
        RS_Bounds b = path->bounds();

        // tile bounds used for bounds checking and quick rejection
        // of candidate positions
        RS_Bounds tileBounds = m_serenderer->GetBounds();
        RS_Bounds rejectBounds(tileBounds.minx - tileBounds.width(),
                               tileBounds.miny - tileBounds.height(),
                               tileBounds.maxx + tileBounds.width(),
                               tileBounds.maxy + tileBounds.height());

        // how many tiles does the given polygon span?
        double spanx = b.width() / tileBounds.width();
        double spany = b.height() / tileBounds.width();
        double span = rs_max(spanx, spany);

        // if the polygon spans more than 3 tiles, also add some periodic labels
        // across the extent of the polygon
        if (span >= 3.0 && spanx > 0.0 && spany > 0.0)
        {
            // one label per 1.5 tiles.
            double yinc = 1.5 / spany;
            double xinc = 1.5 / spanx;

            // starting phases of the periodic label -- center so that
            // there is equal leftover distance on top/bottom and left/right
            double yoffset = (1.0 - ((int)(1.0/yinc))*yinc) * 0.5;
            double xoffset = (1.0 - ((int)(1.0/xinc))*xinc) * 0.5;

            // parametric clip coordinates used to determine a subset of the
            // polygon's bounding box which we will attempt to cover with
            // periodic labels
            double invheight = 1.0 / b.height();
            double tilestarty = (rejectBounds.miny - b.miny) * invheight;
            tilestarty = rs_max(0.0, tilestarty);
            double tileendy = (rejectBounds.maxy - b.miny) * invheight;
            tileendy = rs_min(1.0, tileendy);

            double invwidth = 1.0 / b.width();
            double tilestartx = (rejectBounds.minx - b.minx) * invwidth;
            tilestartx = rs_max(0.0, tilestartx);
            double tileendx = (rejectBounds.maxx - b.minx) * invwidth;
            tileendx = rs_min(1.0, tileendx);

            double ypos = yoffset;

            // move ypos up until we reach a relevant position that is likely to
            // draw a label that intersects the tile
            if (tilestarty != 0.0)
                ypos += ceil((tilestarty - ypos) / yinc) * yinc;

            bool offset = false;

            // loop to add labels
            while (ypos <= tileendy)
            {
                double xpos = xoffset + (offset? 0.5 * xinc : 0.0);

                // move to the right until we reach a relevant position that is likely to
                // draw a label that intersects the tile
                if (tilestartx != 0.0)
                    xpos += ceil((tilestartx - xpos) / xinc) * xinc;

                while (xpos <= tileendx)
                {
                    // compute mapping space position for label, based on
                    double posx = b.minx + xpos * b.width();
                    double posy = b.miny + ypos * b.height();

                    // rejection of labels that are far outside the current tile
                    // and also make sure the point we genrated is inside the polygon
                    if (path->PointInPolygon(posx, posy))
                    {
                        RS_LabelInfo* info = &labels[0]; // assumes one label info passed in

                        LabelInfoLocal lrinfo(posx, posy, text, info->tdef());
                        m_labelGroups.back().m_labels.push_back(lrinfo);
                    }

                    xpos += xinc;
                }

                //Commenting this out fixed trac ticket 258 -- the polygon
                //label alternate offsetting was causing some labels
                //to be cut off across tiles
                //offset = !offset;
                ypos += yinc;
            }
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

            LabelInfoLocal lrinfo(info->x() + offx, info->y() + offy, text, info->tdef());
            m_labelGroups.back().m_labels.push_back(lrinfo);
        }
    }

    // remember the feature bounds for the label group
    RS_Bounds bounds;
    path->ComputeBounds(bounds);

    // this is the non-SE case, and so the line buffer bounds are
    // already in mapping space
    m_labelGroups.back().m_feature_bounds = bounds;

    EndOverpostGroup();
}


//////////////////////////////////////////////////////////////////////////////
// SE symbol labels
void LabelRendererLocal::ProcessLabelGroup(SE_LabelInfo*    labels,
                                           int              nlabels,
                                           RS_OverpostType  type,
                                           bool             exclude,
                                           LineBuffer*      path)
{
    BeginOverpostGroup(type, true, exclude);

    // Add a new style SE label to the overpost groups.
    // Here we are processing the simple case (like labels at given points
    // rather than labels along a path).  The hard case is TODO.
    m_labelGroups.back().m_algo = laSESymbol;

    for (int i=0; i<nlabels; ++i)
    {
        SE_LabelInfo* info = &labels[i];

        // label is in device space
        LabelInfoLocal lrinfo(info->x, info->y, info->style);

        // TODO: HACK -- well somewhat of a hack -- store the angle in the tdef
        lrinfo.m_tdef.rotation() = info->anglerad * M_180PI;

        m_labelGroups.back().m_labels.push_back(lrinfo);

        // label renderer now owns the cloned render style
        info->style = NULL;
    }

    // remember the feature bounds for the label group
    // the path is NULL when it is a point feature, set m_feature_bounds to an invalid RS_Bounds.
    m_labelGroups.back().m_feature_bounds = path ? path->bounds() : RS_Bounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);

    EndOverpostGroup();
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::BeginOverpostGroup(RS_OverpostType type, bool render, bool exclude)
{
    // add a new group
    OverpostGroupLocal group(render, exclude, type);
    m_labelGroups.push_back(group);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::EndOverpostGroup()
{
    // don't add empty groups
    if (m_labelGroups.back().m_labels.size() == 0)
        m_labelGroups.pop_back();
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::BlastLabels()
{
    try
    {
        //-------------------------------------------------------
        // step 1 - perform stitching
        //-------------------------------------------------------

        for (size_t i=0; i<m_labelGroups.size(); ++i)
        {
            OverpostGroupLocal& group = m_labelGroups[i];

            if (group.m_algo == laCurve && group.m_labels.size() > 1)
            {
                std::vector<LabelInfoLocal> stitched = StitchPolylines(group.m_labels);
                if (stitched.size() > 0)
                {
                    // replace the existing vector of labels with the stitched one
                    for (size_t j=0; j<group.m_labels.size(); ++j)
                    {
                        LabelInfoLocal& info = group.m_labels[j];

                        // at this point there shouldn't yet be any rotated points
                        _ASSERT(info.m_rotated_points == NULL);

                        delete [] info.m_pts;
                        info.m_pts = NULL;
                        info.m_numpts = 0;

                        // the style was cloned when it was passed to the LabelRenderer
                        delete info.m_sestyle;
                        info.m_sestyle = NULL;
                    }

                    group.m_labels.clear();
                    group.m_labels.insert(group.m_labels.end(), stitched.begin(), stitched.end());
                }
            }
        }

        //-------------------------------------------------------
        // step 2 - compute bounds for all the labels
        //-------------------------------------------------------

        for (size_t i=0; i<m_labelGroups.size(); ++i)
        {
            OverpostGroupLocal& group = m_labelGroups[i];

            std::vector<LabelInfoLocal> repeated_infos;

            for (size_t j=0; j<group.m_labels.size(); ++j)
            {
                LabelInfoLocal& info = group.m_labels[j];

                bool success = false;

                if (info.m_pts)
                {
                    // several possible positions along the path may be
                    // returned in the case of repeated labels
                    success = ComputePathLabelBounds(info, repeated_infos, group.m_scaleLimit);
                }
                else
                {
                    // compute the label bounds - this allocates the rotated
                    // points on the supplied label info
                    if (info.m_sestyle)
                        success = ComputeSELabelBounds(info);
                    else
                        success = ComputeSimpleLabelBounds(info);

                    // in this case we will simply add a copy of the label info to
                    // the repeated infos collection
                    LabelInfoLocal copy = info;

                    // The code above copies any SE style pointer.  Rather than
                    // clone the style and have the original deleted below, just
                    // clear the pointer on the original label info and make the new
                    // one own the SE style.
                    info.m_sestyle = NULL;

                    // also clear the rotated points pointer on the original - the
                    // new label info owns this object
                    info.m_rotated_points = NULL;

                    // this is the non-path-label case, so there shouldn't be any
                    // path data
                    _ASSERT(copy.m_pts == NULL);

                    // add the label info copy to the collection now that pointers
                    // are cleaned up
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

            // now replace the group's label infos by the new repeated
            // infos collection -- also free the polyline data stored
            // in the m_pts members of the original infos, since we will
            // no longer need the geometry data
            for (size_t j=0; j<group.m_labels.size(); ++j)
            {
                LabelInfoLocal& info = group.m_labels[j];

                // the original label infos should still not have any rotated points
                _ASSERT(info.m_rotated_points == NULL);

                delete [] info.m_pts;
                info.m_pts = NULL;
                info.m_numpts = 0;

                // the style was cloned when it was passed to the LabelRenderer
                delete info.m_sestyle;
                info.m_sestyle = NULL;
            }

            group.m_labels = repeated_infos;
        }

        //-------------------------------------------------------
        // step 3 - flatten group list
        //-------------------------------------------------------

        std::vector<OverpostGroupLocal> finalGroups;
        for (size_t i=0; i<m_labelGroups.size(); ++i)
        {
            OverpostGroupLocal& group = m_labelGroups[i];

            // for path labels, put each label into a separate group so
            // the sorting code below treats each label independently
            if (group.m_algo == laCurve || group.m_algo == laPeriodicPolygon)
            {
                for (size_t j=0; j<group.m_labels.size(); ++j)
                {
                    // create a new group with just one label
                    OverpostGroupLocal newGroup(group.m_render, group.m_exclude, group.m_type);
                    newGroup.m_algo           = group.m_algo;
                    newGroup.m_scaleLimit     = group.m_scaleLimit;
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

        RS_Bounds& tileBounds = m_serenderer->GetBounds();
        double tileMinX = tileBounds.minx;
        double tileMaxX = tileBounds.maxx;
        double tileMinY = tileBounds.miny;
        double tileMaxY = tileBounds.maxy;
        double tileWid  = tileBounds.width();
        double tileHgt  = tileBounds.height();

        std::vector<OverpostGroupLocal*> groupsC00;  // bottom left shared corner
        std::vector<OverpostGroupLocal*> groupsC10;  // bottom right shared corner
        std::vector<OverpostGroupLocal*> groupsC01;  // top left shared corner
        std::vector<OverpostGroupLocal*> groupsC11;  // top right shared corner

        std::vector<OverpostGroupLocal*> groupsEx0;  // left shared edge
        std::vector<OverpostGroupLocal*> groupsEx1;  // right shared edge
        std::vector<OverpostGroupLocal*> groupsEy0;  // bottom shared edge
        std::vector<OverpostGroupLocal*> groupsEy1;  // top shared edge

        std::vector<OverpostGroupLocal*> groupsCtr;  // completely inside

        for (size_t i=0; i<finalGroups.size(); ++i)
        {
            OverpostGroupLocal& group = finalGroups[i];
            if (!group.m_render && !group.m_exclude)
                continue;

            //-------------------------------------------------------
            // get the bounds for the group as a whole
            //-------------------------------------------------------

            double minX = +DBL_MAX;
            double maxX = -DBL_MAX;
            double minY = +DBL_MAX;
            double maxY = -DBL_MAX;
            for (size_t j=0; j<group.m_labels.size(); ++j)
            {
                LabelInfoLocal& info = group.m_labels[j];

                // just iterate over the rotated points for each element
                for (size_t k=0; k<info.m_numelems*4; ++k)
                {
                    // don't need to worry about y-orientation in this case
                    double x, y;
                    m_serenderer->ScreenToWorldPoint(info.m_rotated_points[k].x, info.m_rotated_points[k].y, x, y);

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
            // m_feature_bounds is invalid for point feature
            if (group.m_feature_bounds.IsValid())
            {
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
            }
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

            // we missed a case if we hit this assert
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

        for (size_t i=0; i<finalGroups.size(); ++i)
        {
            OverpostGroupLocal& group = finalGroups[i];
            for (size_t j=0; j<group.m_labels.size(); ++j)
            {
                LabelInfoLocal& info = group.m_labels[j];

                delete [] info.m_rotated_points;
                info.m_rotated_points = NULL;

                info.m_numpts = 0;
                info.m_numelems = 0;

                // label infos in finalGroups should no longer have polyline data
                _ASSERT(info.m_pts == NULL);

                // the style was cloned when it was passed to the LabelRenderer
                delete info.m_sestyle;
                info.m_sestyle = NULL;
            }
        }

        finalGroups.clear();
        m_hStitchTable.clear();
        m_overpost.Clear();
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }
    catch (...)
    {
    }
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererLocal::ComputeSimpleLabelBounds(LabelInfoLocal& info)
{
    RS_FontEngine* fe = m_serenderer->GetRSFontEngine();

    // measure the text (this function will take into account newlines)
    if (!fe->GetTextMetrics(info.m_text, info.m_tdef, info.m_tm, false))
        return false;

    // radian CCW rotation
    double angleRad = info.m_tdef.rotation() * M_PI180;
    if (!m_serenderer->YPointsUp())
        angleRad = -angleRad;

    // transform insertion point to screen space
    m_serenderer->WorldToScreenPoint(info.m_x, info.m_y, info.m_ins_point.x, info.m_ins_point.y);

    //-------------------------------------------------------
    // text extent computation
    //-------------------------------------------------------

    int numLines = (int)info.m_tm.line_pos.size();

    // allocate the data we need
    info.m_numelems = numLines;
    info.m_rotated_points = new RS_F_Point[4*numLines];

    // store the rotated points with the label
    GetRotatedTextPoints(info.m_tm, info.m_ins_point.x, info.m_ins_point.y, angleRad, info.m_rotated_points);

#ifdef DEBUG_LABELS
    static int featIdS = -1;
    featIdS++;

    static unsigned int clrR = 0xffff0000;
    static unsigned int clrG = 0xff00ff00;
    static unsigned int clrB = 0xff0000ff;
    static unsigned int clrO = 0xffff8000;
/*
    // this debugging code draws the feature geometry using a thick
    // brush, with the color alternating between blue and orange
    printf("numPts=%d\n", info.m_numpts);
    for (int j=1; j<info.m_numpts; ++j)
    {
        LineBuffer lb(5);
        lb.MoveTo(info.m_pts[j-1].x, info.m_pts[j-1].y);
        lb.LineTo(info.m_pts[j  ].x, info.m_pts[j  ].y);
        unsigned int color = ((featIdS % 2)==0)? clrB : clrO;
        SE_LineStroke lineStroke(color, 2.0);
        m_serenderer->DrawScreenPolyline(&lb, NULL, lineStroke);
    }
*/
    // draw boxes around all rotated lines of text in the label,
    // with the color cycling between red, green, and blue
    for (int k=0; k<numLines; ++k)
    {
        RS_F_Point* pts = &info.m_rotated_points[k*4];

        LineBuffer lb(5);
        lb.MoveTo(pts[0].x, pts[0].y);
        lb.LineTo(pts[1].x, pts[1].y);
        lb.LineTo(pts[2].x, pts[2].y);
        lb.LineTo(pts[3].x, pts[3].y);
        lb.Close();
        unsigned int color = ((featIdS % 3)==0)? clrR : ((featIdS % 3)==1)? clrG : clrB;
        SE_LineStroke lineStroke(color, 0.0);
//      SE_LineStroke lineStroke(info.m_tdef.textcolor().argb(), 0.0);
        m_serenderer->DrawScreenPolyline(&lb, NULL, lineStroke);
    }
#endif

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererLocal::ComputePathLabelBounds(LabelInfoLocal& info, std::vector<LabelInfoLocal>& repeated_infos, double scaleLimit)
{
    // set a limit on the number of path segments
    _ASSERT(info.m_numpts < MAX_PATH_SEGMENTS);
    if (info.m_numpts >= MAX_PATH_SEGMENTS)
        return false;

    RS_FontEngine* fe = m_serenderer->GetRSFontEngine();

    // since this is path text we need to do any BIDI conversion before
    // we process the label
    const RS_String& sConv = m_bidiConverter.ConvertString(info.m_text);

    // match the font and measure the sizes of the characters
    if (!fe->GetTextMetrics(sConv, info.m_tdef, info.m_tm, true))
        return false;

    // Find starting position of each segment in the screen space path.  We
    // will use it to position characters along the path.  This is precomputed
    // here rather than in ComputeCharacterPositions in order to reuse the data
    // for repeated labels.
    double* segpos = (double*)alloca(sizeof(double) * info.m_numpts);
    segpos[0] = 0.0;

    for (int i=1; i<info.m_numpts; ++i)
    {
        double dx = info.m_pts[i].x - info.m_pts[i-1].x;
        double dy = info.m_pts[i].y - info.m_pts[i-1].y;
        segpos[i] = segpos[i-1] + sqrt(dx*dx + dy*dy);
    }

    // how many times should we repeat the label along the path?
    // TODO: fine tune this formula
    double repeat = PATH_LABEL_SEPARATION_INCHES * MILLIMETERS_PER_INCH * m_serenderer->GetScreenUnitsPerMillimeterDevice();
    int numreps = (int)(segpos[info.m_numpts-1] / (repeat + info.m_tm.text_width));
    if (!numreps)
        numreps = 1;

    // allocate the data we need
    info.m_numelems = sConv.length();

    for (int irep=0; irep<numreps; ++irep)
    {
        // make a copy of the modified label data for the current label period
        LabelInfoLocal copy_info = info;

        // clear the path data on the copy - the original label info retains
        // ownership of this
        copy_info.m_pts = NULL;
        copy_info.m_numpts = 0;

        // parametric position for current repeated label
        // positions are spaced in such a way that each label has
        // an equal amount of white space on both sides around it
        double param_position = ((double)irep + 0.5) / (double)numreps;

        // compute position and angle along the path for each character
        if (!fe->LayoutPathText(copy_info.m_tm, info.m_pts, info.m_numpts, segpos, param_position, info.m_tdef.valign(), scaleLimit))
            continue;

        // once we have position and angle for each character
        // compute rotated corner points for each character
        copy_info.m_rotated_points = new RS_F_Point[4*copy_info.m_numelems];

        for (size_t i=0; i<copy_info.m_numelems; ++i)
        {
            // get the character width - not exact since it takes kerning
            // into account, but should be good enough
            double char_width = copy_info.m_tm.char_advances[i];

            // compute rotated corner points of character
            RS_F_Point* pts = &copy_info.m_rotated_points[i*4];
            GetRotatedPoints(copy_info.m_tm.char_pos[i].x, copy_info.m_tm.char_pos[i].y, char_width, copy_info.m_tm.text_height, copy_info.m_tm.char_pos[i].anglerad, pts);

#ifdef DEBUG_LABELS
            static int featIdP = -1;
            if (i == 0) featIdP++;

            static unsigned int clrR = 0xffff0000;
            static unsigned int clrG = 0xff00ff00;
            static unsigned int clrB = 0xff0000ff;

            // this debugging code draws a box around the label's characters
            // with the color cycling between red, green, and blue
            LineBuffer lb(5);
            lb.MoveTo(pts[0].x, pts[0].y);
            lb.LineTo(pts[1].x, pts[1].y);
            lb.LineTo(pts[2].x, pts[2].y);
            lb.LineTo(pts[3].x, pts[3].y);
            lb.Close();
            unsigned int color = ((featIdP % 3)==0)? clrR : ((featIdP % 3)==1)? clrG : clrB;
            SE_LineStroke lineStroke(color, 0.0);
//          SE_LineStroke lineStroke(info.m_tdef.textcolor().argb(), 0.0);
            m_serenderer->DrawScreenPolyline(&lb, NULL, lineStroke);
#endif
        }

        // add current periodic label to the return list
        repeated_infos.push_back(copy_info);
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererLocal::ComputeSELabelBounds(LabelInfoLocal& info)
{
    // allocate the data we need
    info.m_numelems = 1;
    info.m_rotated_points = new RS_F_Point[4];

    // get native symbol bounds (in screen units - the render style is already
    // scaled to screen units)
    memcpy(info.m_rotated_points, info.m_sestyle->bounds, 4 * sizeof(RS_F_Point));

    // translate and orient the bounds with the given symbol angle and position
    // apply position and rotation to the native bounds of the symbol
    double angleRad = info.m_tdef.rotation() * M_PI180;
    SE_Matrix m;
    m.rotate(m_serenderer->YPointsUp()? angleRad : -angleRad);
    m.translate(info.m_x, info.m_y);

    // store the rotated points with the label
    for (int i=0; i<4; ++i)
        m.transform(info.m_rotated_points[i].x, info.m_rotated_points[i].y);

#ifdef DEBUG_LABELS
    static int featIdS = -1;
    featIdS++;

    static unsigned int clrR = 0xffff0000;
    static unsigned int clrG = 0xff00ff00;
    static unsigned int clrB = 0xff0000ff;
    static unsigned int clrO = 0xffff8000;
/*
    // this debugging code draws the feature geometry using a thick
    // brush, with the color alternating between blue and orange
    printf("numPts=%d\n", info.m_numpts);
    for (int j=1; j<info.m_numpts; ++j)
    {
        LineBuffer lb(5);
        lb.MoveTo(info.m_pts[j-1].x, info.m_pts[j-1].y);
        lb.LineTo(info.m_pts[j  ].x, info.m_pts[j  ].y);
        unsigned int color = ((featIdS % 2)==0)? clrB : clrO;
        SE_LineStroke lineStroke(color, 2.0);
        m_serenderer->DrawScreenPolyline(&lb, NULL, lineStroke);
    }
*/
    // this debugging code draws a box around the label (using its bounds),
    // with the color cycling between red, green, and blue
    LineBuffer lb(5);
    lb.MoveTo(info.m_rotated_points[0].x, info.m_rotated_points[0].y);
    lb.LineTo(info.m_rotated_points[1].x, info.m_rotated_points[1].y);
    lb.LineTo(info.m_rotated_points[2].x, info.m_rotated_points[2].y);
    lb.LineTo(info.m_rotated_points[3].x, info.m_rotated_points[3].y);
    lb.Close();
    unsigned int color = ((featIdS % 3)==0)? clrR : ((featIdS % 3)==1)? clrG : clrB;
    SE_LineStroke lineStroke(color, 0.0);
//  SE_LineStroke lineStroke(info.m_tdef.textcolor().argb(), 0.0);
    m_serenderer->DrawScreenPolyline(&lb, NULL, lineStroke);
#endif

    return true;
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::ProcessLabelGroupsInternal(SimpleOverpost* pMgr, std::vector<OverpostGroupLocal*>& groups)
{
    for (size_t i=0; i<groups.size(); ++i)
    {
        OverpostGroupLocal* pGroup = groups[i];

        for (size_t j=0; j<pGroup->m_labels.size(); ++j)
        {
            LabelInfoLocal& info = pGroup->m_labels[j];
            bool res = ProcessLabelInternal(pMgr,
                                            info,
                                            pGroup->m_render,
                                            pGroup->m_exclude,
                                            pGroup->m_type != RS_OverpostType_All);

            // in the case of a simple label do we check the overpost type
            // we also need to check the overpost type for point feature even if it is a SE symbol
            if (pGroup->m_algo == laSimple || pGroup->m_algo == laSESymbol)
            {
                if (res && (pGroup->m_type == RS_OverpostType_FirstFit))
                    break;
            }
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererLocal::ProcessLabelInternal(SimpleOverpost* pMgr,
                                              LabelInfoLocal& info,
                                              bool render,
                                              bool exclude,
                                              bool check)
{
    _ASSERT(pMgr != NULL || !check);

    //-------------------------------------------------------
    // check for overposting
    //-------------------------------------------------------

    // check for overlap
    if (check)
    {
        // check all the elements
        for (size_t i=0; i<info.m_numelems; ++i)
        {
            if (OverlapsStuff(pMgr, &info.m_rotated_points[i*4], 4))
                return false;
        }
    }

    // add bounds to exclusion regions if needed
    if (exclude)
    {
        // once again, do this per element to get tighter bounds around the label
        for (size_t i=0; i<info.m_numelems; ++i)
        {
            AddExclusionRegion(pMgr, &info.m_rotated_points[i*4], 4);
        }
    }

    //-------------------------------------------------------
    // draw the label
    //-------------------------------------------------------

    if (render)
    {
        // call the appropriate routine
        if (info.m_sestyle)
        {
            // apply position and rotation to the native bounds of the symbol
            double angleRad = info.m_tdef.rotation() * M_PI180;
            SE_Matrix m;
            m.rotate(m_serenderer->YPointsUp()? angleRad : -angleRad);
            m.translate(info.m_x, info.m_y);

            m_serenderer->DrawSymbol(info.m_sestyle->symbol, m, angleRad);
        }
        else if (info.m_tm.char_pos.size() > 0)
            m_serenderer->GetRSFontEngine()->DrawPathText(info.m_tm, info.m_tdef);
        else
            m_serenderer->GetRSFontEngine()->DrawBlockText(info.m_tm, info.m_tdef, info.m_ins_point.x, info.m_ins_point.y);
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::AddExclusionRegion(RS_F_Point* pts, int npts)
{
    AddExclusionRegion(&m_overpost, pts, npts);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::AddExclusionRegion(SimpleOverpost* pMgr, RS_F_Point* pts, int npts)
{
    _ASSERT(npts == 4);

#ifdef DEBUG_LABELS
    LineBuffer lb(5);
    lb.MoveTo(pts[0].x, pts[0].y);
    lb.LineTo(pts[1].x, pts[1].y);
    lb.LineTo(pts[2].x, pts[2].y);
    lb.LineTo(pts[3].x, pts[3].y);
    lb.Close();
    SE_LineStroke lineStroke(0xffff0000, 3.0);
    m_serenderer->DrawScreenPolyline(&lb, NULL, lineStroke);
#endif

    RS_F_Point* tmp = (RS_F_Point*)alloca(npts * sizeof(RS_F_Point));

    // convert back to mapping space since the overpost manager uses
    // mapping space bounding boxes
    for (int i=0; i<npts; ++i)
        m_serenderer->ScreenToWorldPoint(pts[i].x, pts[i].y, tmp[i].x, tmp[i].y);

    pMgr->AddRegion(tmp, npts);
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererLocal::OverlapsStuff(SimpleOverpost* pMgr, RS_F_Point* pts, int npts)
{
    RS_F_Point* tmp = (RS_F_Point*)alloca(npts * sizeof(RS_F_Point));

    // convert back to mapping space since the overpost manager uses
    // mapping space bounding boxes
    for (int i=0; i<npts; ++i)
        m_serenderer->ScreenToWorldPoint(pts[i].x, pts[i].y, tmp[i].x, tmp[i].y);

    return pMgr->Overlaps(tmp, npts);
}


//////////////////////////////////////////////////////////////////////////////
std::vector<LabelInfoLocal> LabelRendererLocal::StitchPolylines(std::vector<LabelInfoLocal>& labels)
{
    size_t numLabels = labels.size();

    // no batching required if we're below the batch size
    if (numLabels <= STITCH_BATCH_SIZE)
        return StitchPolylinesHelper(labels);

    // store overall results here
    std::vector<LabelInfoLocal> ret;
    std::vector<LabelInfoLocal> input;

    size_t numProcessed = 0;
    while (numProcessed < numLabels)
    {
        // create the input vector for the batch
        input.clear();
        for (size_t i=0; i<STITCH_BATCH_SIZE && numProcessed<numLabels; ++i)
            input.push_back(labels[numProcessed++]);

        // process the batch
        std::vector<LabelInfoLocal> output = StitchPolylinesHelper(input);

        // add to the overall result
        for (size_t i=0; i<output.size(); ++i)
            ret.push_back(output[i]);
    }

    return ret;
}


//////////////////////////////////////////////////////////////////////////////
std::vector<LabelInfoLocal> LabelRendererLocal::StitchPolylinesHelper(std::vector<LabelInfoLocal>& labels)
{
    std::vector<LabelInfoLocal> src = labels; // make a copy
    std::vector<LabelInfoLocal> ret; // store results here

    // while there are unprocessed items
    while (src.size() > 0)
    {
        // try to stitch a source item to items in return list
        size_t i;
        for (i=0; i<ret.size(); ++i)
        {
            LabelInfoLocal& retinfo = ret[i];

            size_t j;
            for (j=0; j<src.size(); ++j)
            {
                LabelInfoLocal& srcinfo = src[j];

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
            LabelInfoLocal& srcinfo = src.back();
            LabelInfoLocal retinfo = srcinfo;

            // we don't yet support symbol-based path labels
            _ASSERT(retinfo.m_sestyle == NULL);

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
