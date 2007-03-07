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
#include "PolygonAdapter.h"
#include "FilterExecutor.h"
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
        iter != m_hAreaSymCache.end(); iter++)
    {
        delete iter->second;
    }

    m_hAreaSymCache.clear();
}


//////////////////////////////////////////////////////////////////////////////
void PolygonAdapter::Stylize(Renderer*                   renderer,
                             RS_FeatureReader*           features,
                             RS_FilterExecutor*          exec,
                             LineBuffer*                 lb,
                             MdfModel::FeatureTypeStyle* style,
                             const MdfModel::MdfString*  tooltip,
                             const MdfModel::MdfString*  url,
                             RS_ElevationSettings*       elevSettings)
{
    m_exec = exec;

    //if the style is not an area style -- no need
    //to do anything, so quit.
    if (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(style) != FeatureTypeStyleVisitor::ftsArea)
        return;

    MdfModel::AreaTypeStyle* afs = (MdfModel::AreaTypeStyle*)(style);

    MdfModel::RuleCollection* arc = afs->GetRules();

    MdfModel::AreaRule* rule = NULL;

    //determine the symbolization for the feature
    for (int i=0; i<arc->GetCount(); i++)
    {
        rule = static_cast<MdfModel::AreaRule*>(arc->GetAt(i));

        if (!ExecFdoFilter(&rule->GetFilter()))
        {
            rule = NULL; //don't stylize with failed rule
            continue;
        }

        break;
    }

    if (!rule) return;

    MdfModel::AreaSymbolization2D* asym = rule->GetSymbolization();

    if (asym == NULL ) return;

    RS_String tip; //TODO: this should be quick since we are not assigning
    RS_String eurl;
    const RS_String &theme = rule->GetLegendLabel();
    
    // Elevation Settings
    RS_ElevationType elevType = RS_ElevationType_RelativeToGround;
    double zOffset = 0;
    double zExtrusion = 0;
    GetElevationParams(elevSettings, zOffset, zExtrusion, elevType);
    
    if (tooltip && !tooltip->empty())
        EvalString(*tooltip, tip);
    if (url && !url->empty())
        EvalString(*url, eurl);

    renderer->StartFeature(features, tip.empty()? NULL : &tip, eurl.empty()? NULL : &eurl, theme.empty() ? NULL : &theme, zOffset, zExtrusion, elevType);

    //quick check if style is already cached
    RS_FillStyle* cachedStyle = m_hAreaSymCache[asym];
    if (cachedStyle)
    {
        renderer->ProcessPolygon(lb, *cachedStyle);
    }
    else
    {
        //if not, then we need to either cache or evaluate it
        RS_FillStyle fillStyle;
        ObtainStyle(asym, fillStyle);

        renderer->ProcessPolygon(lb, fillStyle);
    }

    //do labeling if needed
    MdfModel::Label* label = rule->GetLabel();
    if (label->GetSymbol() != NULL)
    {
        double cx = std::numeric_limits<double>::quiet_NaN();
        double cy = std::numeric_limits<double>::quiet_NaN();
        double dummy;

        //multi should work for simple polygons also
        lb->Centroid(LineBuffer::ctArea, &cx, &cy, &dummy);

        if (!_isnan(cx) && !_isnan(cy))
        {
            AddLabel(cx, cy, 0.0, false, label, RS_OverpostType_FirstFit, true, renderer, lb);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
//Checks if a style is already cached and returns it if it is.
//Otherwise evaluates a style and if it is constant, caches it
//in a hashtable.
void PolygonAdapter::ObtainStyle(MdfModel::AreaSymbolization2D* asym, RS_FillStyle& fillStyle)
{
    bool cacheable = ConvertFill(asym, fillStyle);

    if (cacheable)
    {
        RS_FillStyle* rfill = new RS_FillStyle;
        *rfill = fillStyle;

        m_hAreaSymCache[asym] = rfill;
    }
}
