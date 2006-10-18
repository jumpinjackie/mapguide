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
#include "Renderer.h"
#include "PointAdapter.h"
#include "FilterExecutor.h"
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
        iter != m_hPointSymCache.end(); iter++)
    {
        delete iter->second;
    }

    m_hPointSymCache.clear();
}


//////////////////////////////////////////////////////////////////////////////
void PointAdapter::Stylize(Renderer*                   renderer,
                           RS_FeatureReader*           features,
                           RS_FilterExecutor*          exec,
                           LineBuffer*                 lb,
                           MdfModel::FeatureTypeStyle* style,
                           const MdfModel::MdfString*  tooltip,
                           const MdfModel::MdfString*  url)
{
    m_exec = exec;

    //if the style is not a point style -- no need
    //to do anything, so quit.
    if (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(style) != FeatureTypeStyleVisitor::ftsPoint)
        return;

    MdfModel::PointTypeStyle* pfs = (MdfModel::PointTypeStyle*)(style);

    MdfModel::RuleCollection* prc = pfs->GetRules();

    MdfModel::PointRule* rule = NULL;

    //determine the symbolization for the feature
    for (int i=0; i<prc->GetCount(); i++)
    {
        rule = static_cast<MdfModel::PointRule*>(prc->GetAt(i));

        //if there is a filter on the rule,
        //apply it and if it fails, move to the
        //next rule
        if (!ExecFdoFilter(&rule->GetFilter()))
        {
            rule = NULL; //don't stylize with failed rule
            continue;
        }

        break;
    }

    if (!rule) return;

    MdfModel::PointSymbolization2D* psym = rule->GetSymbolization();

    RS_String tip; //TODO: this should be quick since we are not assigning
    RS_String eurl;
    const RS_String &theme = rule->GetLegendLabel();

    if (tooltip && !tooltip->empty())
        EvalString(*tooltip, tip);
    if (url && !url->empty())
        EvalString(*url, eurl);

    //send the geometry to the rendering pipeline

    //process point symbol, if any.
    //If there is no point symbol, there may be a label which
    //we will use as a symbol instead
    //this one does not obey overposting, it is always there
    //the marker specified in the rule is the one that
    //does overposting

    double mdefW = 0.01;
    double mdefH = 0.01;
    RS_Units mdefU = RS_Units_Device;
    double mdefRot = 0.0;

	//the actual position used for the marker by the Renderer
	//may be returned in this structure to help place
	//labels better
	RS_Bounds bounds = RS_Bounds(1.,1.,0.,0.); // init invalid

    if (psym && psym->GetSymbol())
    {
        renderer->StartFeature(features, tip.empty()? NULL : &tip, eurl.empty()? NULL : &eurl, theme.empty() ? NULL : &theme);

        //quick check if style is already cached
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

    //labeling
    MdfModel::Label* label = rule->GetLabel();
    if (label && label->GetSymbol())
    {
        //TODO: compute label position
        double cx = 0.0;
        double cy = 0.0;
        double dummy;

        //multi should work for simple polygons also
        lb->Centroid(LineBuffer::ctPoint, &cx, &cy, &dummy);

        if (!_isnan(cx) && !_isnan(cy))
        {
            //if there was no point symbol, the label
            //is the symbol, so we send without overposting
            //and at the center point
            if (!psym || !psym->GetSymbol() || pfs->IsDisplayAsText())
            {
                AddLabel(cx, cy, 0.0, false, label, RS_OverpostType_All, !pfs->IsAllowOverpost(), renderer, lb);
            }
            else
            {
                MdfModel::TextSymbol* text = label->GetSymbol();

                RS_TextDef def;
                ConvertTextDef(text, def);

                RS_String txt;
                /*bool const1 =*/ EvalString(text->GetText(), txt);

                //if there is a symbol, there are 8 possible positions
                //to place the label around the symbol

                //NOTE: at this point we know that mdef has
                //been initialized with whatever was in
                //psym->GetSymbol() and that expressions have
                //been evaluated

                double op_pts[16];
                
                // calculate a 2 pixel offset to allow for label ghosting
                double offset = 2.0 * (0.0254 / renderer->GetDpi()); //2 pixels in meters 
                
                //in case of mapping space we need to scale by map scale
                if (mdefU != RS_Units_Device)
                    offset *= renderer->GetMapScale();

                //compute how far label needs to be offset from
                //center point of symbol

                double w = 0.5 * mdefW;
                double h = 0.5 * mdefH;
                double ch = 0;		// vertical center point
                double cw = 0;		// horizontal center point

                w += offset;
                h += offset;
                
                bool useBounds = bounds.IsValid();
                if (useBounds)
                {
                    bounds.maxx += offset;    bounds.maxy += offset;
                    bounds.minx -= offset;    bounds.miny -= offset;
                    ch = (bounds.maxy + bounds.miny)/2.;
                    cw = (bounds.maxx + bounds.minx)/2.;
                }

                //take into account rotation of the symbol
                //find increased extents of the symbol bounds
                //due to the rotation
                if (mdefRot != 0.0)
                {
                    double rotRad = mdefRot * M_PI / 180.0;
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
                   
                    cwsn = cw * sn;		chsn = ch * sn; 
                    cwcs = cw * cs;		chcs = ch * cs;

                    // find the octant that the marker is rotated into, and shift the points accordingly.
                    // this way, the overpost points are still within 22.5 degrees of an axis-aligned box.
                    // (position 0 will always be the closest to Center-Right)
                    double nangle = mdefRot + (mdefRot > 0 ? -(int)(mdefRot/360.)*360. : ((int)(mdefRot/360.) + 1)*360.);
                    int i = ((int)((nangle/45.) + .5)) << 1; // i is 2 * the octant
                    op_pts[i++] = wcs - chsn;  op_pts[i++] = wsn + chcs;   i &= 0x0000000f; // & 15 does (mod 16)
                    op_pts[i++] = wcs - hsn;   op_pts[i++] = wsn + hcs;    i &= 0x0000000f;
                    op_pts[i++] = cwcs - hsn;  op_pts[i++] = cwsn + hcs;   i &= 0x0000000f;
                    op_pts[i++] = nwcs - hsn;  op_pts[i++] = nwsn + hcs;   i &= 0x0000000f;
                    op_pts[i++] = nwcs - chsn; op_pts[i++] = nwsn + chcs;  i &= 0x0000000f;
                    op_pts[i++] = nwcs - nhsn; op_pts[i++] = nwsn + nhcs;  i &= 0x0000000f;
                    op_pts[i++] = cwcs -nhsn;  op_pts[i++] = cwsn + nhcs;  i &= 0x0000000f;
                    op_pts[i++] = wcs - nhsn;  op_pts[i]   = wsn + nhcs;
                }
                else
                {
                    if (!useBounds)
                    {
                        bounds.maxx = w;	bounds.minx = -w;
                        bounds.maxy = h;	bounds.miny = -h;
                    }
                    op_pts[0] = bounds.maxx;	op_pts[1] = ch;
                    op_pts[2] = bounds.maxx;	op_pts[3] = bounds.maxy;
                    op_pts[4] = cw;				op_pts[5] = bounds.maxy;
                    op_pts[6] = bounds.minx;	op_pts[7] = bounds.maxy;
                    op_pts[8] = bounds.minx;	op_pts[9] = ch;
                    op_pts[10] = bounds.minx;	op_pts[11] = bounds.miny;
                    op_pts[12] = cw;			op_pts[13] = bounds.miny;
                    op_pts[14] = bounds.maxx;	op_pts[15] = bounds.miny;
                }

                RS_LabelInfo candidates[8];

                def.halign() = RS_HAlignment_Left;
                def.valign() = RS_VAlignment_Half;
                candidates[0] = RS_LabelInfo(cx, cy, op_pts[0], op_pts[1], mdefU, def, false);

                def.valign() = RS_VAlignment_Descent;
                candidates[1] = RS_LabelInfo(cx, cy, op_pts[2], op_pts[3], mdefU, def, false);

                def.halign() = RS_HAlignment_Center;
                candidates[2] = RS_LabelInfo(cx, cy, op_pts[4], op_pts[5], mdefU, def, false);

                def.halign() = RS_HAlignment_Right;
                candidates[3] = RS_LabelInfo(cx, cy, op_pts[6], op_pts[7], mdefU, def, false);

                def.valign() = RS_VAlignment_Half;
                candidates[4] = RS_LabelInfo(cx, cy, op_pts[8], op_pts[9], mdefU, def, false);

                def.valign() = RS_VAlignment_Ascent;
                candidates[5] = RS_LabelInfo(cx, cy, op_pts[10], op_pts[11], mdefU, def, false);

                def.halign() = RS_HAlignment_Center;
                candidates[6] = RS_LabelInfo(cx, cy, op_pts[12], op_pts[13], mdefU, def, false);

                def.halign() = RS_HAlignment_Left;
                candidates[7] = RS_LabelInfo(cx, cy, op_pts[14], op_pts[15], mdefU, def, false);

                renderer->ProcessLabelGroup(candidates, 8, txt, RS_OverpostType_FirstFit, true, lb);
            }
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
//Checks if a style is already cached and returns it if it is.
//Otherwise evaluates a style and if it is constant, caches it
//in a hashtable.
void PointAdapter::ObtainStyle(MdfModel::PointSymbolization2D* psym, RS_MarkerDef& mdef)
{
    RS_MarkerDef* cachedStyle = m_hPointSymCache[psym];

    if (cachedStyle)
    {
        mdef = *cachedStyle;
    }
    else
    {
        // TODO: need to handle all types of symbols eventually
        bool cacheable = ConvertMarkerDef(psym->GetSymbol(), mdef);

        if (cacheable)
        {
            RS_MarkerDef* rmdef = new RS_MarkerDef;
            *rmdef = mdef;

            m_hPointSymCache[psym] = rmdef;
        }
    }
}
