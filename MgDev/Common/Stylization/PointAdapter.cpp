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
#include "PointAdapter.h"
#include "LineBuffer.h"
#include "FeatureTypeStyleVisitor.h"


//////////////////////////////////////////////////////////////////////////////
PointAdapter::PointAdapter(LineBufferPool* lbp) : GeometryAdapter(lbp)
{
}


//////////////////////////////////////////////////////////////////////////////
PointAdapter::~PointAdapter()
{
    //free up cached point styles
    for (std::map<void*, RS_MarkerDef*>::iterator iter = m_hPointSymCache.begin();
        iter != m_hPointSymCache.end(); ++iter)
    {
        delete iter->second;
    }

    m_hPointSymCache.clear();
}


//////////////////////////////////////////////////////////////////////////////
void PointAdapter::Stylize(Renderer*                   renderer,
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

    // no need to do anything if the style is not a point style, so quit
    if (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(style) != FeatureTypeStyleVisitor::ftsPoint)
        return;

    //-------------------------------------------------------
    // determine the rule for the feature
    //-------------------------------------------------------

    MdfModel::RuleCollection* prc = style->GetRules();
    MdfModel::PointRule* rule = NULL;

    for (int i=0; i<prc->GetCount(); ++i)
    {
        rule = static_cast<MdfModel::PointRule*>(prc->GetAt(i));

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

    MdfModel::PointSymbolization2D* psym = rule->GetSymbolization();

    MdfModel::PointTypeStyle* pfs = (MdfModel::PointTypeStyle*)style;

    //-------------------------------------------------------
    // prepare the geometry on which to apply the style
    //-------------------------------------------------------

    // NOTE: clipping of geometry for rendering (the RequiresClipping
    //       option) does not need to be done for points.  Points
    //       outside the map extents already get clipped away as part
    //       of the FDO query.

    LineBuffer* lb = geometry;
    std::auto_ptr<LineBuffer> spClipLB;

    if (renderer->RequiresClipping())
    {
        // clip geometry to given extents
        // NOTE: point styles do not require a clip offset
        LineBuffer* lbc = lb->Clip(renderer->GetBounds(), LineBuffer::ctAGF, m_lbPool);
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

    // Process point symbol, if any.  If there is no point symbol, there may
    // be a label which we will use as a symbol instead.  This one does not
    // obey overposting, it is always there.  The marker specified in the rule
    // is the one that does overposting.

    double mdefW = 0.01;
    double mdefH = 0.01;
    RS_Units mdefU = RS_Units_Device;
    double mdefRot = 0.0;

    // the actual position used for the marker by the renderer
    // may be returned in this structure to help place labels better
    RS_Bounds bounds = RS_Bounds(1.0, 1.0, 0.0, 0.0); // init invalid

    if (psym && psym->GetSymbol())
    {
        // quick check if style is already cached
        RS_MarkerDef* cachedStyle = m_hPointSymCache[psym];
        if (cachedStyle)
        {
            mdefW = cachedStyle->width();
            mdefH = cachedStyle->height();
            mdefU = cachedStyle->units();
            mdefRot = cachedStyle->rotation();
            renderer->ProcessMarker(lb, *cachedStyle, pfs->IsAllowOverpost(), &bounds);
        }
        else
        {
            RS_MarkerDef mdef;
            ObtainStyle(psym, mdef);

            mdefW = mdef.width();
            mdefH = mdef.height();
            mdefU = mdef.units();
            mdefRot = mdef.rotation();

            renderer->ProcessMarker(lb, mdef, pfs->IsAllowOverpost(), &bounds);
        }
    }

    //-------------------------------------------------------
    // do labeling if needed
    //-------------------------------------------------------

    MdfModel::Label* label = rule->GetLabel();
    if (label && label->GetSymbol())
    {
        // NOTE: clipping of geometry for labeling (the RequiresLabelClipping
        //       option) does not need to be done for points.

        // TODO: compute label position
        double cx = std::numeric_limits<double>::quiet_NaN();
        double cy = std::numeric_limits<double>::quiet_NaN();
        double dummy;

        // multi should work for simple polygons also
        lb->Centroid(LineBuffer::ctPoint, &cx, &cy, &dummy);

        if (!_isnan(cx) && !_isnan(cy))
        {
            // if there was no point symbol, the label is the symbol,
            // so we send without overposting and at the center point
            if (!psym || !psym->GetSymbol() || pfs->IsDisplayAsText())
            {
                AddLabel(cx, cy, 0.0, false, label, RS_OverpostType_All, !pfs->IsAllowOverpost(), renderer, lb);
            }
            else
            {
                MdfModel::TextSymbol* text = label->GetSymbol();

                RS_String txt;
                EvalString(text->GetText(), txt);

                if (!txt.empty())
                {
                    RS_TextDef def;
                    ConvertTextDef(text, def);

                    // if there's a symbol there are 8 possible positions to place the label
                    // around the symbol

                    // NOTE: at this point we know that mdef has been initialized with
                    //       whatever was in psym->GetSymbol() and that expressions have
                    //       been evaluated

                    double op_pts[16];

                    // offset the label from the symbol's edge
                    double offset = 0.001 * POINT_LABEL_OFFSET_MM;  // in meters
                    if (def.rotation() != 0.0)
                    {
                        // if the text label has rotation put the text at least half the font height
                        // away, so that it doesn't intersect with the marker at the worst-case (45
                        // degree) rotation.
                        offset += 0.5*def.font().height();
                    }

                    // in case of mapping space we need to scale by map scale
                    if (mdefU != RS_Units_Device)
                        offset *= renderer->GetMapScale();

                    // compute how far label needs to be offset from center point of symbol
                    double w = 0.5 * mdefW;
                    double h = 0.5 * mdefH;
                    double ch = 0;      // vertical center point
                    double cw = 0;      // horizontal center point

                    w += offset;
                    h += offset;

                    bool useBounds = bounds.IsValid();
                    if (useBounds)
                    {
                        bounds.maxx += offset;    bounds.maxy += offset;
                        bounds.minx -= offset;    bounds.miny -= offset;
                        ch = 0.5*(bounds.maxy + bounds.miny);
                        cw = 0.5*(bounds.maxx + bounds.minx);
                    }

                    // take into account rotation of the symbol
                    // find increased extents of the symbol bounds due to the rotation
                    if (mdefRot != 0.0)
                    {
                        double rotRad = mdefRot * M_PI180;
                        double cs = cos(rotRad);
                        double sn = sin(rotRad);

                        double wcs, nwcs, wsn, nwsn, hsn, nhsn, hcs, nhcs, cwsn, cwcs, chsn, chcs;
                        // check to see if the bounds have been set
                        if (useBounds)
                        {
                            wcs = bounds.maxx * cs;   nwcs = bounds.minx * cs;
                            wsn = bounds.maxx * sn;   nwsn = bounds.minx * sn;
                            hsn = bounds.maxy * sn;   nhsn = bounds.miny * sn;
                            hcs = bounds.maxy * cs;   nhcs = bounds.miny * cs;
                        }
                        else
                        {
                            wcs = w * cs;   nwcs = -wcs;
                            wsn = w * sn;   nwsn = -wsn;
                            hsn = h * sn;   nhsn = -hsn;
                            hcs = h * cs;   nhcs = -hcs;
                        }

                        cwsn = cw * sn;     chsn = ch * sn;
                        cwcs = cw * cs;     chcs = ch * cs;

                        // find the octant that the marker is rotated into, and shift the points accordingly.
                        // this way, the overpost points are still within 22.5 degrees of an axis-aligned box.
                        // (position 0 will always be the closest to Center-Right)
                        double nangle = fmod(mdefRot, 360.0);
                        if (nangle < 0.0)
                            nangle += 360.0;
                        int i = (((int)((nangle/45.0) + 0.5)) << 1) & 0x0000000f; // i is 2 * the octant
                        op_pts[i++] =  wcs - chsn;  op_pts[i++] =  wsn + chcs;  i &= 0x0000000f; // & 15 does (mod 16)
                        op_pts[i++] =  wcs -  hsn;  op_pts[i++] =  wsn +  hcs;  i &= 0x0000000f;
                        op_pts[i++] = cwcs -  hsn;  op_pts[i++] = cwsn +  hcs;  i &= 0x0000000f;
                        op_pts[i++] = nwcs -  hsn;  op_pts[i++] = nwsn +  hcs;  i &= 0x0000000f;
                        op_pts[i++] = nwcs - chsn;  op_pts[i++] = nwsn + chcs;  i &= 0x0000000f;
                        op_pts[i++] = nwcs - nhsn;  op_pts[i++] = nwsn + nhcs;  i &= 0x0000000f;
                        op_pts[i++] = cwcs - nhsn;  op_pts[i++] = cwsn + nhcs;  i &= 0x0000000f;
                        op_pts[i++] =  wcs - nhsn;  op_pts[i]   =  wsn + nhcs;
                    }
                    else
                    {
                        if (!useBounds)
                        {
                            bounds.maxx = w;    bounds.minx = -w;
                            bounds.maxy = h;    bounds.miny = -h;
                        }
                        op_pts[ 0] = bounds.maxx;   op_pts[ 1] = ch;
                        op_pts[ 2] = bounds.maxx;   op_pts[ 3] = bounds.maxy;
                        op_pts[ 4] = cw;            op_pts[ 5] = bounds.maxy;
                        op_pts[ 6] = bounds.minx;   op_pts[ 7] = bounds.maxy;
                        op_pts[ 8] = bounds.minx;   op_pts[ 9] = ch;
                        op_pts[10] = bounds.minx;   op_pts[11] = bounds.miny;
                        op_pts[12] = cw;            op_pts[13] = bounds.miny;
                        op_pts[14] = bounds.maxx;   op_pts[15] = bounds.miny;
                    }

                    RS_LabelInfo candidates[8];

                    def.halign() = RS_HAlignment_Left;
                    def.valign() = RS_VAlignment_Half;
                    candidates[0] = RS_LabelInfo(cx, cy, op_pts[0], op_pts[1], mdefU, def);

                    def.valign() = RS_VAlignment_Descent;
                    candidates[1] = RS_LabelInfo(cx, cy, op_pts[2], op_pts[3], mdefU, def);

                    def.halign() = RS_HAlignment_Center;
                    candidates[2] = RS_LabelInfo(cx, cy, op_pts[4], op_pts[5], mdefU, def);

                    def.halign() = RS_HAlignment_Right;
                    candidates[3] = RS_LabelInfo(cx, cy, op_pts[6], op_pts[7], mdefU, def);

                    def.valign() = RS_VAlignment_Half;
                    candidates[4] = RS_LabelInfo(cx, cy, op_pts[8], op_pts[9], mdefU, def);

                    def.valign() = RS_VAlignment_Ascent;
                    candidates[5] = RS_LabelInfo(cx, cy, op_pts[10], op_pts[11], mdefU, def);

                    def.halign() = RS_HAlignment_Center;
                    candidates[6] = RS_LabelInfo(cx, cy, op_pts[12], op_pts[13], mdefU, def);

                    def.halign() = RS_HAlignment_Left;
                    candidates[7] = RS_LabelInfo(cx, cy, op_pts[14], op_pts[15], mdefU, def);

                    renderer->ProcessLabelGroup(candidates, 8, txt, RS_OverpostType_FirstFit, true, lb, text->GetScaleLimit());
                }
            }
        }
    }

    // free clipped line buffer if the geometry was clipped
    if (spClipLB.get())
        LineBufferPool::FreeLineBuffer(m_lbPool, spClipLB.release());
}


//////////////////////////////////////////////////////////////////////////////
// Checks if a style is already cached and returns it if it is.
// Otherwise evaluates a style and if it is constant, caches it
// in a hashtable.
void PointAdapter::ObtainStyle(MdfModel::PointSymbolization2D* psym, RS_MarkerDef& mdef)
{
    RS_MarkerDef* cachedStyle = m_hPointSymCache[psym];

    if (cachedStyle)
    {
        mdef = *cachedStyle;
    }
    else
    {
        bool cacheable = ConvertSymbol(psym->GetSymbol(), mdef);
        if (cacheable)
        {
            RS_MarkerDef* rsmdef = new RS_MarkerDef();
            *rsmdef = mdef;

            m_hPointSymCache[psym] = rsmdef;
        }
    }
}
