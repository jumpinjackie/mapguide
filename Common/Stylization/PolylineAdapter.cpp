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
#include "PolylineAdapter.h"
#include "LineBuffer.h"
#include "FeatureTypeStyleVisitor.h"


//////////////////////////////////////////////////////////////////////////////
PolylineAdapter::PolylineAdapter(LineBufferPool* lbp) : GeometryAdapter(lbp)
{
}


//////////////////////////////////////////////////////////////////////////////
PolylineAdapter::~PolylineAdapter()
{
    //free up cached line styles
    for (std::map<void*, RS_LineStroke*>::iterator iter = m_hLineSymCache.begin();
        iter != m_hLineSymCache.end(); ++iter)
    {
        delete iter->second;
    }

    m_hLineSymCache.clear();

    //free up temporary line styles
    for (size_t i=0; i<m_lineSyms.size(); ++i)
    {
        delete m_lineSyms[i];
    }

    m_lineSyms.clear();
}


//////////////////////////////////////////////////////////////////////////////
void PolylineAdapter::Stylize(Renderer*                   renderer,
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

    // no need to do anything if the style is not a line style, so quit
    if (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(style) != FeatureTypeStyleVisitor::ftsLine)
        return;

    //-------------------------------------------------------
    // determine the rule for the feature
    //-------------------------------------------------------

    MdfModel::RuleCollection* lrc = style->GetRules();
    MdfModel::LineRule* rule = NULL;

    for (int i=0; i<lrc->GetCount(); ++i)
    {
        rule = static_cast<MdfModel::LineRule*>(lrc->GetAt(i));

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

    MdfModel::LineSymbolizationCollection* lsymc = rule->GetSymbolizations();
    if (lsymc == NULL)
        return;

    int nSyms = lsymc->GetCount();

    //-------------------------------------------------------
    // evaluate all the styles once
    //-------------------------------------------------------

    // temporary array used to store pointers to each evaluated style
    RS_LineStroke** ppStrokes = (RS_LineStroke**)alloca(nSyms * sizeof(RS_LineStroke*));
    if (!ppStrokes)
        return;

    size_t tempIndex = 0;
    for (int i=0; i<nSyms; ++i)
    {
        MdfModel::LineSymbolization2D* lsym = lsymc->GetAt(i);

        // don't render if there's no symbolization
        if (lsym == NULL)
        {
            ppStrokes[i] = NULL;
            continue;
        }

        // quick check if style is already cached
        RS_LineStroke* cachedStyle = m_hLineSymCache[lsym];
        if (cachedStyle)
        {
            ppStrokes[i] = cachedStyle;
        }
        else
        {
            // if not, then we need to either cache or evaluate it

            // make sure the vector has a style in the slot we need
            if (tempIndex >= m_lineSyms.size())
            {
                _ASSERT(tempIndex == m_lineSyms.size());

                // allocate a new style and add it to the vector
                m_lineSyms.push_back(new RS_LineStroke());
            }

            // use the existing style in the vector
            ppStrokes[i] = m_lineSyms[tempIndex];
            ObtainStyle(lsym, *ppStrokes[i]);

            ++tempIndex;
        }
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

        double clipOffsetMeters = 0.0;  // in device units
        for (int i=0; i<nSyms; ++i)
        {
            if (ppStrokes[i])
            {
                double styleClipOffset = GetClipOffset(*ppStrokes[i], mapScale);
                clipOffsetMeters = rs_max(styleClipOffset, clipOffsetMeters);
            }
        }

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

    for (int i=0; i<nSyms; ++i)
    {
        if (ppStrokes[i])
            renderer->ProcessPolyline(lb, *ppStrokes[i]);
    }

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
        double slope_rad = 0.0;

        // multi should work for simple polylines too
        lb->Centroid(LineBuffer::ctLine, &cx, &cy, &slope_rad);

        if (!_isnan(cx) && !_isnan(cy))
            AddLabel(cx, cy, slope_rad, true, label, RS_OverpostType_AllFit, true, renderer, label->GetSymbol()->IsAdvancedPlacement()? lb : NULL);
    }

    // free clipped line buffer if the geometry was clipped
    if (spClipLB.get())
        LineBufferPool::FreeLineBuffer(m_lbPool, spClipLB.release());
}


//////////////////////////////////////////////////////////////////////////////
// Checks if a style is already cached and returns it if it is.
// Otherwise evaluates a style and if it is constant, caches it
// in a hashtable.
void PolylineAdapter::ObtainStyle(MdfModel::LineSymbolization2D* lsym, RS_LineStroke& lineStyle)
{
    bool cacheable = ConvertStroke(lsym, lineStyle);
    if (cacheable)
    {
        RS_LineStroke* rstroke = new RS_LineStroke();
        *rstroke = lineStyle;

        m_hLineSymCache[lsym] = rstroke;
    }
}
