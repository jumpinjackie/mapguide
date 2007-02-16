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
#include "LabelRendererLocal.h"
#include "gd.h"
#include "GDRenderer.h"
#include "GDUtils.h"
#include "Centroid.h"
#include "RS_Font.h"

#define M_PI 3.14159265358979323846
#define ROUND(x) (int)((x) + 0.5)
//#define DEBUG_LABELS

extern int ConvertColor(gdImagePtr i, RS_Color& c);


//////////////////////////////////////////////////////////////////////////////
LabelRendererLocal::LabelRendererLocal(GDRenderer* renderer, double tileExtentOffset)
: LabelRendererBase(renderer),
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
    int geomType = (path != NULL)? path->geom_type() : FdoGeometryType_None;

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
    else if (geomType == FdoGeometryType_Polygon || geomType == FdoGeometryType_MultiPolygon)
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

            //parametric clip coordinates used to determine a subset of the
            //polygon's bounding box which we will attempt to cover with
            //periodic labels
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

            //move ypos up until we reach a relevant position that is likely to 
            //draw a label that intersects the tile
            if (tilestarty != 0.0)
                ypos += ceil((tilestarty - ypos) / yinc) * yinc;

            bool offset = false;

            //loop to add labels 
            while (ypos <= tileendy)
            {
                double xpos = xoffset + ((offset) ? 0.5 * xinc : 0.0);

                //move to the right until we reach a relevant position that is likely to 
                //draw a label that intersects the tile
                if (tilestartx != 0.0)
                    xpos += ceil((tilestartx - xpos) / xinc) * xinc;

                while (xpos <= tileendx)
                {
                    //compute mapping space position for label, based on
                    double posx = b.minx + xpos * b.width();
                    double posy = b.miny + ypos * b.height();

                    //rejection of labels that are far outside the current tile
                    //and also make sure the point we genrated is inside the polygon
                    if (Centroid::PointInPolygon(path->points(), 2 * path->point_count(),
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

            if (info.m_spacing)
            {
                delete [] info.m_spacing;
                info.m_spacing = NULL;
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
    // font matching
    info.m_font = m_renderer->FindFont(info.m_tdef.font());
    if (!info.m_font)
        return false;

    //determine pixel space font height
    info.m_hgt = m_renderer->_MeterToMapSize(info.m_tdef.font().units(), info.m_tdef.font().height());
    info.m_hgt *= m_renderer->GetMapToScreenScale() * 0.75;

    // The computed height can have roundoff in it, and the rendering code is
    // very sensitive to it.  Remove this roundoff by rounding the height to
    // the nearest 1/65536ths of a point.
    info.m_hgt = floor(info.m_hgt * 65536.0 + 0.5) / 65536.0;

    //radian CCW rotation
    double rotation = info.m_tdef.rotation() * M_PI / 180.0;
    double cos_a = cos(rotation);
    double sin_a = sin(rotation);

    // font height gives the distance from one baseline to the next
    // increase this by a factor of 1.05 (what GD uses)
    double font_height = info.m_font->m_height * info.m_hgt / info.m_font->m_units_per_EM;
    double line_height = 1.05 * font_height;

    // transform insertion point into pixel space
    info.m_ins_point.x = m_renderer->_TXD(info.m_x);
    info.m_ins_point.y = m_renderer->_TYD(info.m_y);

    //-------------------------------------------------------
    // break up the string into individual lines
    //-------------------------------------------------------

    // make a temporary copy
    size_t len = wcslen(info.m_text.c_str());
    wchar_t* cpy = (wchar_t*)alloca((len + 1) * sizeof(wchar_t));
    wcscpy(cpy, info.m_text.c_str());

    // break it up
    std::vector<wchar_t*> line_breaks;
    size_t num_lines = SplitLabel(cpy, line_breaks);

    //-------------------------------------------------------
    // text extent and alignment computation
    //-------------------------------------------------------

    RS_F_Point fpts[4];
    RS_Bounds rotatedBounds(+DBL_MAX, +DBL_MAX, -DBL_MAX, -DBL_MAX);

    // base vertical offset is the same for each line of text
    double vAlignBaseOffset = GetVerticalAlignmentOffset(info.m_tdef, info.m_font, info.m_hgt, line_height, num_lines);

    for (size_t k=0; k<num_lines; ++k)
    {
        wchar_t* txt = line_breaks.at(k);

        // get the unrotated extent of this sub-string
        m_renderer->MeasureString(txt, info.m_hgt, info.m_font, 0.0, fpts, NULL);

        // horizontal offset depends on the sub-string width, while
        // vertical offset depends on the line of text
        double hAlignOffset = GetHorizontalAlignmentOffset(info.m_tdef, fpts);
        double vAlignOffset = vAlignBaseOffset + k*line_height;

        // process the extent points
        for (int j=0; j<4; ++j)
        {
            // translate to account for the alignment
            double tmpX = fpts[j].x + hAlignOffset;
            double tmpY = fpts[j].y + vAlignOffset;

            // rotate and translate to the insertion point
            fpts[j].x = info.m_ins_point.x + tmpX * cos_a + tmpY * sin_a;
            fpts[j].y = info.m_ins_point.y - tmpX * sin_a + tmpY * cos_a;

            // update the overall rotated bounds
            rotatedBounds.add_point(fpts[j]);
        }
    }

    //allocate the data we need
    info.m_numelems = 1;
    info.m_oriented_bounds = new RS_F_Point[4];

    //store the oriented bounds with the label
    rotatedBounds.get_points(info.m_oriented_bounds);

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
    for (int j=1; j<info.m_numpts; ++j)
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
    for (int j=0; j<4; ++j)
    {
        dpts[j].x = (int)info.m_oriented_bounds[j].x;
        dpts[j].y = (int)info.m_oriented_bounds[j].y;
    }
    gdImagePolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), ((featIdS % 3)==0)? clrR : ((featIdS % 3)==1)? clrG : clrB));
//  gdImagePolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.color()));
#endif

    return true;
}


//////////////////////////////////////////////////////////////////////////////
bool LabelRendererLocal::ComputePathLabelBounds(LR_LabelInfoLocal& info, std::vector<LR_LabelInfoLocal>& repeated_infos)
{
    // font matching
    info.m_font = m_renderer->FindFont(info.m_tdef.font());
    if (!info.m_font)
        return false;

    //determine pixel space font height
    info.m_hgt = m_renderer->_MeterToMapSize(info.m_tdef.font().units(), info.m_tdef.font().height());
    info.m_hgt *= m_renderer->GetMapToScreenScale() * 0.75;

    // The computed height can have roundoff in it, and the rendering code is
    // very sensitive to it.  Remove this roundoff by rounding the height to
    // the nearest 1/65536ths of a point.
    info.m_hgt = floor(info.m_hgt * 65536.0 + 0.5) / 65536.0;

    //-------------------------------------------------------
    // text extent and alignment computation
    //-------------------------------------------------------

    //get overall extent and char spacing
    RS_F_Point fpts[4];
    float* spacing = (float*)alloca(info.m_text.length() * 2 * sizeof(float));
    m_renderer->MeasureString(info.m_text, info.m_hgt, info.m_font, 0.0, fpts, spacing);

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
        copy_info.m_oriented_bounds = new RS_F_Point[4*copy_info.m_numelems];
        copy_info.m_spacing = new double[copy_info.m_numelems - 1];

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

        //copy over scaled character advance
        for (size_t i=0; i<copy_info.m_numelems - 1; i++)
            copy_info.m_spacing[i] = spacing[i] * font_scale;

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
            double advance = (i == copy_info.m_numelems-1)? copy_info.m_textwid - total_advance : spacing[i] * font_scale;
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
    double cos_a = cos(rotation);
    double sin_a = sin(rotation);

    // font height gives the distance from one baseline to the next
    // increase this by a factor of 1.05 (what GD uses)
    double font_height = info.m_font->m_height * info.m_hgt / info.m_font->m_units_per_EM;
    double line_height = 1.05 * font_height;

    //-------------------------------------------------------
    // break up the string into individual lines
    //-------------------------------------------------------

    // make a temporary copy
    size_t len = wcslen(info.m_text.c_str());
    wchar_t* cpy = (wchar_t*)alloca((len + 1) * sizeof(wchar_t));
    wcscpy(cpy, info.m_text.c_str());

    // break it up
    std::vector<wchar_t*> line_breaks;
    size_t num_lines = SplitLabel(cpy, line_breaks);

    //-------------------------------------------------------
    // text extent and alignment computation
    //-------------------------------------------------------

    RS_F_Point fpts[4];
    RS_Bounds unrotatedBounds(+DBL_MAX, +DBL_MAX, -DBL_MAX, -DBL_MAX);

    double* hAlignOffsets = (double*)alloca(num_lines * sizeof(double));
    double* vAlignOffsets = (double*)alloca(num_lines * sizeof(double));
    double* textWidths    = (double*)alloca(num_lines * sizeof(double));

    // base vertical offset is the same for each line of text
    double vAlignBaseOffset = GetVerticalAlignmentOffset(info.m_tdef, info.m_font, info.m_hgt, line_height, num_lines);

    for (size_t k=0; k<num_lines; ++k)
    {
        wchar_t* txt = line_breaks.at(k);

        // get the unrotated extent of this sub-string
        m_renderer->MeasureString(txt, info.m_hgt, info.m_font, 0.0, fpts, NULL);

        // horizontal offset depends on the sub-string width, while
        // vertical offset depends on the line of text
        hAlignOffsets[k] = GetHorizontalAlignmentOffset(info.m_tdef, fpts);
        vAlignOffsets[k] = vAlignBaseOffset + k*line_height;

        // remember the width
        textWidths[k] = fpts[1].x - fpts[0].x;

        // process the extent points
        for (int j=0; j<4; ++j)
        {
            // translate to account for the alignment
            fpts[j].x += hAlignOffsets[k];
            fpts[j].y += vAlignOffsets[k];

            // update the overall unrotated bounds
            unrotatedBounds.add_point(fpts[j]);
        }
    }

    //-------------------------------------------------------
    // draw the text
    //-------------------------------------------------------

    // draw the opaque background first, if requested
    if (info.m_tdef.textbg() == RS_TextBackground_Opaque)
    {
        // get the overall unrotated bounds
        unrotatedBounds.get_points(fpts);

        // rotate and translate it
        for (int j=0; j<4; ++j)
        {
            double tmpX = fpts[j].x;
            double tmpY = fpts[j].y;
            fpts[j].x = info.m_ins_point.x + tmpX * cos_a + tmpY * sin_a;
            fpts[j].y = info.m_ins_point.y - tmpX * sin_a + tmpY * cos_a;
        }

        // draw a filled rectangle
        RS_D_Point dpts[4];
        for (int j=0; j<4; j++)
        {
            dpts[j].x = ROUND(fpts[j].x);
            dpts[j].y = ROUND(fpts[j].y);
        }

        gdImageFilledPolygon((gdImagePtr)m_renderer->GetImage(), (gdPointPtr)dpts, 4, ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.bgcolor()));
    }

    for (size_t k=0; k<num_lines; ++k)
    {
        RS_String txt = line_breaks.at(k);

        // add the rotated original offset for this line to the insertion point
        // to get the actual draw point
        double insX = info.m_ins_point.x + hAlignOffsets[k] * cos_a + vAlignOffsets[k] * sin_a;
        double insY = info.m_ins_point.y - hAlignOffsets[k] * sin_a + vAlignOffsets[k] * cos_a;

        int posx = (int)floor(insX + 0.5);
        int posy = (int)floor(insY + 0.5);

        // render the ghosted text, if requested
        if (info.m_tdef.textbg() == RS_TextBackground_Ghosted)
        {
            m_renderer->DrawString(txt, posx-1, posy, info.m_hgt, info.m_font, info.m_tdef.bgcolor(), rotation);
            m_renderer->DrawString(txt, posx+1, posy, info.m_hgt, info.m_font, info.m_tdef.bgcolor(), rotation);
            m_renderer->DrawString(txt, posx, posy-1, info.m_hgt, info.m_font, info.m_tdef.bgcolor(), rotation);
            m_renderer->DrawString(txt, posx, posy+1, info.m_hgt, info.m_font, info.m_tdef.bgcolor(), rotation);
        }

        // render the primary text
        m_renderer->DrawString(txt, posx, posy, info.m_hgt, info.m_font, info.m_tdef.color(), rotation);

        // render the underline, if requested
        if (info.m_tdef.font().style() & RS_FontStyle_Underline)
        {
            // estimate underline line width as % of font height
            double line_width = (double)info.m_font->m_underline_thickness * info.m_hgt / (double)info.m_font->m_units_per_EM;
            double line_pos = - (double)info.m_font->m_underline_position * info.m_hgt / (double)info.m_font->m_units_per_EM;

            // the line's start point is the insertion point, but shifted vertically by line_pos
            double x0 = insX + line_pos * sin_a;
            double y0 = insY + line_pos * cos_a;

            // the end point is a horizontal shift by the text width
            double x1 = x0 + textWidths[k] * cos_a;
            double y1 = y0 - textWidths[k] * sin_a;

            // draw the thick line
            gdImageSetThickness((gdImagePtr)m_renderer->GetImage(), ROUND(line_width));
            gdImageSetAntiAliased((gdImagePtr)m_renderer->GetImage(), ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.color()));
            gdImageLine((gdImagePtr)m_renderer->GetImage(), ROUND(x0), ROUND(y0), ROUND(x1), ROUND(y1), gdAntiAliased);
            gdImageSetThickness((gdImagePtr)m_renderer->GetImage(), 0);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::DrawPathLabel(LR_LabelInfoLocal& info)
{
    //draw the characters, each in its computed position
    RS_String c;
    for (size_t i=0; i<info.m_numelems; ++i)
    {
        c = info.m_text[i], 1;

        //compute screen position and round
        int posx = ROUND(info.m_charpos[i].x);
        int posy = ROUND(info.m_charpos[i].y);

        if (info.m_tdef.textbg() == RS_TextBackground_Ghosted)
        {
            m_renderer->DrawString(c, posx-1, posy, info.m_hgt, info.m_font, info.m_tdef.bgcolor(), info.m_charpos[i].anglerad);
            m_renderer->DrawString(c, posx+1, posy, info.m_hgt, info.m_font, info.m_tdef.bgcolor(), info.m_charpos[i].anglerad);
            m_renderer->DrawString(c, posx, posy-1, info.m_hgt, info.m_font, info.m_tdef.bgcolor(), info.m_charpos[i].anglerad);
            m_renderer->DrawString(c, posx, posy+1, info.m_hgt, info.m_font, info.m_tdef.bgcolor(), info.m_charpos[i].anglerad);
        }

        m_renderer->DrawString(c, posx, posy, info.m_hgt, info.m_font, info.m_tdef.color(), info.m_charpos[i].anglerad);
    }

    //render underline
    if (info.m_tdef.font().style() & RS_FontStyle_Underline)
    {
        const RS_Font * font = info.m_font;

        //estimate underline line width as % of font height
        double line_width = (double)font->m_underline_thickness * info.m_hgt / (double)font->m_units_per_EM;
        //underline position w.r.t. baseline. Invert y while at it
        double line_pos = - (double)font->m_underline_position * info.m_hgt / (double)font->m_units_per_EM;

        //accumulator for underline purposes
        double total_advance = 0.0;

        //used to keep track of last position of the underline, which is 
        //drawn piecewise for each character
        int last_x = ROUND(info.m_charpos[0].x + sin(info.m_charpos[0].anglerad) * line_pos);
        int last_y = ROUND(info.m_charpos[0].y + cos(info.m_charpos[0].anglerad) * line_pos);
        int sx, sy, ex, ey;

        //draw the characters, each in its computed position
        for (size_t i=0; i<info.m_numelems; i++)
        {
            //width of character - not really exact width since
            //it takes kerning into account, but should be good enough
            //we could measure each character separately but that seems like
            //too many calls to FreeType
            //note that unlike the regular label renderer, here the char spacing
            //array is already scaled by the font scale so we don't have to do it here
            double advance = (i == info.m_numelems-1)? info.m_textwid - total_advance : info.m_spacing[i];
            total_advance += advance;

            sx = last_x;
            sy = last_y;

            if (i == info.m_numelems - 1) //is it the last char -- needs special handling
            {
                //estimate bottom right corner of last character
                double eex = info.m_charpos[i].x + cos(info.m_charpos[i].anglerad) * advance;
                double eey = info.m_charpos[i].y - sin(info.m_charpos[i].anglerad) * advance;

                //now take into account underline offset
                ex = ROUND(eex + sin(info.m_charpos[i].anglerad) * line_pos);
                ey = ROUND(eey + cos(info.m_charpos[i].anglerad) * line_pos);
            }
            else
            {                
                //move position by underline offset
                ex = ROUND(info.m_charpos[i+1].x + sin(info.m_charpos[i+1].anglerad) * line_pos);
                ey = ROUND(info.m_charpos[i+1].y + cos(info.m_charpos[i+1].anglerad) * line_pos);
            }

            gdImageSetThickness((gdImagePtr)m_renderer->GetImage(), ROUND(line_width));
            gdImageSetAntiAliased((gdImagePtr)m_renderer->GetImage(), ConvertColor((gdImagePtr)m_renderer->GetImage(), info.m_tdef.color()));
            gdImageLine((gdImagePtr)m_renderer->GetImage(), sx, sy, ex, ey, gdAntiAliased);
            gdImageSetThickness((gdImagePtr)m_renderer->GetImage(), 0);

            last_x = ex;
            last_y = ey;
        }
    }

}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::AddExclusionRegion(RS_F_Point* pts, int npts)
{
    AddExclusionRegion(&m_overpost, pts, npts);
}


//////////////////////////////////////////////////////////////////////////////
void LabelRendererLocal::AddExclusionRegion(SimpleOverpost* pMgr, RS_F_Point* pts, int npts)
{
    //use axis aligned bounds for overposting
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
    //use axis aligned bounds for overposting
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
