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
#include "Renderer.h"
#include "PolygonAdapter.h"
#include "LineBuffer.h"
#include "FeatureTypeStyleVisitor.h"


//////////////////////////////////////////////////////////////////////////////
PolygonAdapter::PolygonAdapter(LineBufferPool* lbp) : GeometryAdapter(lbp)
{
}


//////////////////////////////////////////////////////////////////////////////
PolygonAdapter::~PolygonAdapter()
{
    //free up cached area styles
    for (std::map<void*, RS_FillStyle*>::iterator iter = m_hAreaSymCache.begin();
        iter != m_hAreaSymCache.end(); ++iter)
    {
        delete iter->second;
    }

    m_hAreaSymCache.clear();
}


//////////////////////////////////////////////////////////////////////////////
void PolygonAdapter::Stylize(Renderer*                   renderer,
                             RS_FeatureReader*           features,
                             bool                        initialPass,
                             FdoExpressionEngine*        exec,
                             LineBuffer*                 geometry,
                             MdfModel::FeatureTypeStyle* style,
                             const MdfModel::MdfString*  tooltip,
                             const MdfModel::MdfString*  url,
                             RS_ElevationSettings*       elevSettings,
                             CSysTransformer*            /*layer2mapxformer*/)
{
    m_exec = exec;

    // no need to do anything if the style is not an area style, so quit
    if (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(style) != FeatureTypeStyleVisitor::ftsArea)
        return;

    //-------------------------------------------------------
    // determine the rule for the feature
    //-------------------------------------------------------

    MdfModel::RuleCollection* arc = style->GetRules();
    MdfModel::AreaRule* rule = NULL;

    for (int i=0; i<arc->GetCount(); ++i)
    {
        rule = static_cast<MdfModel::AreaRule*>(arc->GetAt(i));

        // apply any filter on the rule - if it fails move to the next rule
        if (!ExecFdoFilter(&rule->GetFilter()))
        {
            // don't stylize with failed rule
            rule = NULL;
            continue;
        }

        break;
    }

    if (!rule)
        return;

    MdfModel::AreaSymbolization2D* asym = rule->GetSymbolization();
    if (asym == NULL)
        return;

    //-------------------------------------------------------
    // evaluate the style to use
    //-------------------------------------------------------

    // quick check if style is already cached
    RS_FillStyle* fillStyle = m_hAreaSymCache[asym];
    if (!fillStyle)
    {
        // if not, then we need to either cache or evaluate it
        fillStyle = &m_fillStyle;
        ObtainStyle(asym, *fillStyle);
    }

    //-------------------------------------------------------
    // compute the clip offset from the styles
    //-------------------------------------------------------

    double clipOffsetWU = 0.0;  // in mapping units

    bool bClip      = renderer->RequiresClipping();
    bool bLabelClip = renderer->RequiresLabelClipping();

    if (bClip || bLabelClip)
    {
        double mapScale = renderer->GetMapScale();

        // in meters in device units
        double clipOffsetMeters = GetClipOffset(fillStyle->outline(), mapScale);

        // add one pixel's worth to handle any roundoff
        clipOffsetMeters += METERS_PER_INCH / renderer->GetDpi();

        // limit the offset to something reasonable
        if (clipOffsetMeters > MAX_CLIPOFFSET_IN_METERS)
            clipOffsetMeters = MAX_CLIPOFFSET_IN_METERS;

        // convert to mapping units
        clipOffsetWU = clipOffsetMeters * mapScale / renderer->GetMetersPerUnit();
    }

    //-------------------------------------------------------
    // prepare the geometry on which to apply the style
    //-------------------------------------------------------

    LineBuffer* lb = geometry;
    std::auto_ptr<LineBuffer> spClipLB;

    if (bClip)
    {
        // the clip region is the map request extents expanded by the offset
        RS_Bounds clip = renderer->GetBounds();
        clip.minx -= clipOffsetWU;
        clip.miny -= clipOffsetWU;
        clip.maxx += clipOffsetWU;
        clip.maxy += clipOffsetWU;

        // clip geometry to given extents
        LineBuffer* lbc = lb->Clip(clip, LineBuffer::ctAGF, m_lbPool);
        if (lbc != lb)
        {
            // if the clipped buffer is NULL (completely clipped) just move on to
            // the next feature
            if (!lbc)
                return;

            // otherwise continue processing with the clipped buffer
            lb = lbc;
            if (lb != geometry)
                spClipLB.reset(lb);
        }
    }

    //-------------------------------------------------------
    // do the StartFeature notification
    //-------------------------------------------------------

    RS_String tip; //TODO: this should be quick since we are not assigning
    RS_String eurl;
    const RS_String &theme = rule->GetLegendLabel();

    if (tooltip && !tooltip->empty())
        EvalString(*tooltip, tip);
    if (url && !url->empty())
        EvalString(*url, eurl);

    // elevation settings
    RS_ElevationType elevType = RS_ElevationType_RelativeToGround;
    double zOffset = 0.0;
    double zExtrusion = 0.0;
    GetElevationParams(elevSettings, zOffset, zExtrusion, elevType);

    renderer->StartFeature(features, initialPass,
                           tip.empty()? NULL : &tip,
                           eurl.empty()? NULL : &eurl,
                           theme.empty()? NULL : &theme,
                           zOffset, zExtrusion, elevType);

    //-------------------------------------------------------
    // apply the style to the geometry using the renderer
    //-------------------------------------------------------

    renderer->ProcessPolygon(lb, *fillStyle);

    //-------------------------------------------------------
    // do labeling if needed
    //-------------------------------------------------------

    MdfModel::Label* label = rule->GetLabel();
    if (label && label->GetSymbol())
    {
        // Make sure the geometry is clipped if label clipping is specified.
        // If bClip is true then the geometry is already clipped.
        if (!bClip && bLabelClip)
        {
            // the clip region is the map request extents expanded by the offset
            RS_Bounds clip = renderer->GetBounds();
            clip.minx -= clipOffsetWU;
            clip.miny -= clipOffsetWU;
            clip.maxx += clipOffsetWU;
            clip.maxy += clipOffsetWU;

            LineBuffer* lbc = lb->Clip(clip, LineBuffer::ctAGF, m_lbPool);
            if (lbc != lb)
            {
                // if the clipped buffer is NULL (completely clipped) just move on to
                // the next feature
                if (!lbc)
                    return;

                // otherwise continue processing with the clipped buffer
                lb = lbc;
                if (lb != geometry)
                    spClipLB.reset(lb);
            }
        }

        double cx = std::numeric_limits<double>::quiet_NaN();
        double cy = std::numeric_limits<double>::quiet_NaN();
        double dummy;

        // multi should work for simple polygons too
        lb->Centroid(LineBuffer::ctArea, &cx, &cy, &dummy);

        if (!_isnan(cx) && !_isnan(cy))
            AddLabel(cx, cy, 0.0, false, label, RS_OverpostType_AllFit, true, renderer, lb);
    }

    // free clipped line buffer if the geometry was clipped
    if (spClipLB.get())
        LineBufferPool::FreeLineBuffer(m_lbPool, spClipLB.release());
}


//////////////////////////////////////////////////////////////////////////////
// Checks if a style is already cached and returns it if it is.
// Otherwise evaluates a style and if it is constant, caches it
// in a hashtable.
void PolygonAdapter::ObtainStyle(MdfModel::AreaSymbolization2D* asym, RS_FillStyle& fillStyle)
{
    bool cacheable = ConvertFill(asym, fillStyle);
    if (cacheable)
    {
        RS_FillStyle* rsfill = new RS_FillStyle();
        *rsfill = fillStyle;

        m_hAreaSymCache[asym] = rsfill;
    }
}
