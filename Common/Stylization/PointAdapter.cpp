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

    if (psym && psym->GetSymbol())
    {
        renderer->StartFeature(features, tip.empty()? NULL : &tip, eurl.empty()? NULL : &eurl);

        //quick check if style is already cached
        RS_MarkerDef* cachedStyle = m_hPointSymCache[psym];
        if (cachedStyle)
        {
            mdefW = cachedStyle->width();
            mdefH = cachedStyle->height();
            mdefU = cachedStyle->units();
            mdefRot = cachedStyle->rotation();
            renderer->ProcessMarker(lb, *cachedStyle, pfs->IsAllowOverpost());
        }
        else
        {
            RS_MarkerDef mdef;
            ObtainStyle(psym, mdef);

            mdefW = mdef.width();
            mdefH = mdef.height();
            mdefU = mdef.units();
            mdefRot = mdef.rotation();

            renderer->ProcessMarker(lb, mdef, pfs->IsAllowOverpost());
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

                //compute how far label needs to be offset from
                //center point of symbol
                //TODO: do we need to take into account difference
                //between insertion point of symbol and actual geometric point
                double w = 0.5 * mdefW;
                double h = 0.5 * mdefH;

                //take into account rotation of the symbol
                //find increased extents of the symbol bounds
                //due to the rotation
                if (mdefRot != 0.0)
                {
                    double rotRad = mdefRot * M_PI / 180.0;
                    double cs = cos(rotRad);
                    double sn = sin(rotRad);
                    double wcs = w * cs;
                    double wsn = w * sn;
                    double hsn = h * sn;
                    double hcs = h * cs;

                    double w1 = wcs - hsn;
                    double h1 = wsn + hcs;
                    double w2 = wcs + hsn;
                    double h2 = wsn - hcs;

                    w = rs_max( fabs(w1), fabs(w2) );
                    h = rs_max( fabs(h1), fabs(h2) );
                }

                //add 2 pixels to offset
                double offset = 2.0 * (0.0254 / renderer->GetDpi()); //2 pixels in meters

                //in case of mapping space we need to scale by map scale
                if (mdefU != RS_Units_Device)
                    offset *= renderer->GetMapScale();

                w += offset;
                h += offset;

                RS_LabelInfo candidates[8];

                def.halign() = RS_HAlignment_Left;
                def.valign() = RS_VAlignment_Half;
                candidates[0] = RS_LabelInfo(cx, cy, w, 0, mdefU, def, false);

                def.valign() = RS_VAlignment_Descent;
                candidates[1] = RS_LabelInfo(cx, cy, w, h, mdefU, def, false);

                def.halign() = RS_HAlignment_Center;
                candidates[2] = RS_LabelInfo(cx, cy, 0, h, mdefU, def, false);

                def.halign() = RS_HAlignment_Right;
                candidates[3] = RS_LabelInfo(cx, cy, -w, h, mdefU, def, false);

                def.valign() = RS_VAlignment_Half;
                candidates[4] = RS_LabelInfo(cx, cy, -w, 0, mdefU, def, false);

                def.valign() = RS_VAlignment_Ascent;
                candidates[5] = RS_LabelInfo(cx, cy, -w, -h, mdefU, def, false);

                def.halign() = RS_HAlignment_Center;
                candidates[6] = RS_LabelInfo(cx, cy, 0, -h, mdefU, def, false);

                def.halign() = RS_HAlignment_Left;
                candidates[7] = RS_LabelInfo(cx, cy, w, -h, mdefU, def, false);

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
