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
#include "Renderer.h"
#include "PolylineAdapter.h"
#include "FilterExecutor.h"
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
        iter != m_hLineSymCache.end(); iter++)
    {
        delete iter->second;
    }

    m_hLineSymCache.clear();
}


//////////////////////////////////////////////////////////////////////////////
void PolylineAdapter::Stylize(Renderer*                   renderer,
                              RS_FeatureReader*           features,
                              RS_FilterExecutor*          exec,
                              LineBuffer*                 lb,
                              MdfModel::FeatureTypeStyle* style,
                              const MdfModel::MdfString*  tooltip,
                              const MdfModel::MdfString*  url,
                              RS_ElevationSettings*       elevSettings)
{
    m_exec = exec;

    //if the style is not a line style -- no need
    //to do anything, so quit.
    if (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(style) != FeatureTypeStyleVisitor::ftsLine)
        return;

    MdfModel::LineTypeStyle* lfs = (MdfModel::LineTypeStyle*)(style);

    MdfModel::RuleCollection* lrc = lfs->GetRules();

    MdfModel::LineRule* rule = NULL;

    //determine the symbolization for the feature
    for (int i=0; i<lrc->GetCount(); i++)
    {
        rule = static_cast<MdfModel::LineRule*>(lrc->GetAt(i));
        if (!ExecFdoFilter(&rule->GetFilter()))
        {
            rule = NULL; //don't stylize with failed rule
            continue;
        }

        break;
    }

    if (!rule) return;

    MdfModel::LineSymbolizationCollection* lsymc = rule->GetSymbolizations();

    if (!lsymc) return;

    RS_String tip; //TODO: this should be quick since we are not assigning
    RS_String eurl;
    const RS_String &theme = rule->GetLegendLabel();

    if (tooltip && !tooltip->empty())
        EvalString(*tooltip, tip);
    if (url && !url->empty())
        EvalString(*url, eurl);

    // Elevation Settings
    RS_ElevationType elevType = RS_ElevationType_RelativeToGround;
    double zOffset = 0;
    double zExtrusion = 0;
    GetElevationParams(elevSettings, zOffset, zExtrusion, elevType);

    renderer->StartFeature(features, tip.empty()? NULL : &tip, eurl.empty()? NULL : &eurl, 
        theme.empty() ? NULL : &theme, zOffset, zExtrusion, elevType);

    for (int i=0; i<lsymc->GetCount(); i++)
    {
        MdfModel::LineSymbolization2D* lsym = lsymc->GetAt(i);

        //quick check if style is already cached
        RS_LineStroke* cachedStyle = m_hLineSymCache[lsym];
        if (cachedStyle)
        {
            renderer->ProcessPolyline(lb, *cachedStyle);
        }
        else
        {
            //if not, then we need to either cache or evaluate it
            RS_LineStroke lineStyle;
            ObtainStyle(lsym, lineStyle);

            renderer->ProcessPolyline(lb, lineStyle);
        }
    }

    //labeling if needed
    MdfModel::Label* label = rule->GetLabel();
    if (label->GetSymbol() != NULL)
    {
        double cx = 0.0;
        double cy = 0.0;
        double slope = 0.0;

        //multi should work for simple polylines too
        lb->Centroid(LineBuffer::ctLine, &cx, &cy, &slope);

        if (!_isnan(cx) && !_isnan(cy))
        {
            AddLabel(cx, cy, slope, true, label, RS_OverpostType_FirstFit, true, renderer, lb);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
//Checks if a style is already cached and returns it if it is.
//Otherwise evaluates a style and if it is constant, caches it
//in a hashtable.
void PolylineAdapter::ObtainStyle(MdfModel::LineSymbolization2D* lsym, RS_LineStroke& lineStyle)
{
    bool cacheable = ConvertStroke(lsym, lineStyle);

    if (cacheable)
    {
        RS_LineStroke* rstroke = new RS_LineStroke;
        *rstroke = lineStyle;

        m_hLineSymCache[lsym] = rstroke;
    }
}
