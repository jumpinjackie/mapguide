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

#ifndef FEATUREPROPRENDERER_H
#define FEATUREPROPRENDERER_H

#include "FeatureInfoRenderer.h"

class MgBatchPropertyCollection;

//Overload of FeatureInfoRenderer that accumulates property values
//for all features that are processed instead of just the first one
//like the base FeatureInfoRenderer does
class MG_SERVER_RENDERING_API FeaturePropRenderer : public FeatureInfoRenderer
{

public:
    FeaturePropRenderer(MgSelection* selection,
                        int maxFeatures,
                        double mapScale,
                        bool bIncludeFeatureBBOX);
    virtual ~FeaturePropRenderer();

    ///////////////////////////////////
    // Renderer implementation
    //
    virtual void StartFeature(RS_FeatureReader* feature,
                              bool              initialPass,
                              const RS_String*  tooltip = NULL,
                              const RS_String*  url = NULL,
                              const RS_String*  theme = NULL,
                              double            zOffset = 0.0,
                              double            zExtrusion = 0.0,
                              RS_ElevationType  zOffsetType = RS_ElevationType_RelativeToGround);

    virtual bool SupportsTooltips();
    virtual bool SupportsHyperlinks();

    virtual void ProcessPolygon(LineBuffer*   lb,
                                RS_FillStyle& fill);

    virtual void ProcessPolyline(LineBuffer*    lb,
                                 RS_LineStroke& lsym);

    virtual void ProcessMarker(LineBuffer*   lb,
                               RS_MarkerDef& mdef,
                               bool          allowOverpost,
                               RS_Bounds*    bounds = NULL);

    virtual void ProcessPoint(SE_ApplyContext* ctx,
                              SE_RenderPointStyle* style,
                              RS_Bounds* bounds = NULL);

    virtual void ProcessLine(SE_ApplyContext* ctx,
                             SE_RenderLineStyle* style);

    virtual void ProcessArea(SE_ApplyContext* ctx,
                             SE_RenderAreaStyle* style);

    void GetGeometryBounds(const LineBuffer* lb, RS_Bounds& bounds);
    void SetBBOXProperty(const RS_Bounds& bounds, MgStringProperty* bbox);

    MgBatchPropertyCollection* GetProperties()
    {
        return SAFE_ADDREF(m_featprops);
    }

private:
    MgBatchPropertyCollection* m_featprops;
    MgPropertyCollection* m_currentFeature;
    bool m_bIncludeFeatureBBOX;
};

#endif
