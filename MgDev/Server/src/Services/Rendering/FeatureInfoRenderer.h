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

#ifndef FEATUREINFORENDERER_H
#define FEATUREINFORENDERER_H

#include "Renderer.h"
#include "SE_Renderer.h"

class MgPropertyCollection;
class LineBuffer;
class KeyEncode;

//A simple Renderer implementation for use by the QueryFeatures
//RenderingService API. Accumulates feature IDs and also attributes
//for one feature (case where a tooltip or attribute info needs to
//be displayed in the AJAX viewer)
class MG_SERVER_RENDERING_API FeatureInfoRenderer : public Renderer, public SE_Renderer
{
public:
    FeatureInfoRenderer(MgSelection* selection,
                        int maxFeatures,
                        double mapScale);
    virtual ~FeatureInfoRenderer();

    ///////////////////////////////////
    // Renderer implementation

    virtual void StartMap(RS_MapUIInfo* mapInfo,
                          RS_Bounds&    extents,
                          double        mapScale,
                          double        dpi,
                          double        metersPerUnit,
                          CSysTransformer* xformToLL = NULL);

    virtual void EndMap();

    virtual void StartLayer(RS_LayerUIInfo* legendInfo,
                            RS_FeatureClassInfo* classInfo);

    virtual void EndLayer();

    virtual void ProcessPolygon(LineBuffer* lb,
                                RS_FillStyle& fill)
    {}

    virtual void ProcessPolyline(LineBuffer* lb,
                                 RS_LineStroke& lsym)
    {}

    virtual void ProcessRaster(unsigned char* data,
                               int length,
                               RS_ImageFormat format,
                               int width, int height,
                               RS_Bounds extents)
    {}

    virtual void ProcessMarker(LineBuffer*   lb,
                               RS_MarkerDef& mdef,
                               bool          allowOverpost,
                               RS_Bounds*    bounds = NULL)
    {}

    virtual void ProcessLabel(double x, double y,
                              const RS_String& text, RS_TextDef& tdef)
    {}

    virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path)
    {}

    virtual void AddDWFContent(RS_InputStream*  in,
                               CSysTransformer* xformer,
                               const RS_String& section,
                               const RS_String& passwd,
                               const RS_String& filter)
    {}

    virtual void SetSymbolManager(RS_SymbolManager* manager)
    {}

    virtual void StartFeature(RS_FeatureReader* feature,
                              const RS_String*  tooltip = NULL,
                              const RS_String*  url = NULL,
                              const RS_String* theme = NULL,
                              double zOffset = 0,
                              double zExtrusion = 0,
                              RS_ElevationType zOffsetType = RS_ElevationType_RelativeToGround);

    virtual RS_MapUIInfo* GetMapInfo();

    virtual RS_LayerUIInfo* GetLayerInfo();

    virtual RS_FeatureClassInfo* GetFeatureClassInfo();

    virtual double GetMapScale()
    {
        return m_mapScale;
    }

    virtual RS_Bounds& GetBounds()
    {
        // not directly used
        return m_extents;
    }

    virtual double GetDpi()
    {
        // not directly used - anything but zero is ok
        return 96.0;
    }

    virtual double GetMetersPerUnit()
    {
        // not directly used - anything but zero is ok
        return 1.0;
    }

    //note if we don't return false, we will need to return
    //correct bounds from GetBounds and we do not have them
    //available here, since out feature query is not a bounds query
    virtual bool RequiresClipping()
    {
        return false;
    }

    ///////////////////////////////////
    // SE_Renderer implementation

    virtual void ProcessPoint(SE_ApplyContext* ctx, SE_RenderPointStyle* style)
    {}

    virtual void ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style)
    {}

    virtual void ProcessArea(SE_ApplyContext* ctx, SE_RenderAreaStyle* style)
    {}

    virtual void DrawSymbol(SE_RenderPrimitiveList& symbol, const SE_Matrix& xform, double angleRad)
    {}

    virtual void DrawScreenPolyline(LineBuffer* polyline, const SE_Matrix* xform, unsigned int color, double weight)
    {}

    virtual void DrawScreenPolygon(LineBuffer* polygon, const SE_Matrix* xform, unsigned int fill)
    {}

    virtual void DrawScreenRaster(unsigned char* data, int length,
                                  RS_ImageFormat format, int native_width, int native_height,
                                  double x, double y, double w, double h, double angleDeg)
    {}

    virtual void DrawScreenText(const RS_String& txt, RS_TextDef& tdef, double insx, double insy,
                                double* path, int npts, double param_position)
    {}

    virtual bool YPointsUp()
    {
        return true;
    }

    virtual void GetWorldToScreenTransform(SE_Matrix& xform)
    {}

    virtual void WorldToScreenPoint(double& inx, double& iny, double& ox, double& oy)
    {}

    virtual void ScreenToWorldPoint(double& inx, double& iny, double& ox, double& oy)
    {}

    virtual double GetPixelsPerMillimeterScreen()
    {
        return 0.0;
    }

    virtual double GetPixelsPerMillimeterWorld()
    {
        return 0.0;
    }

    virtual RS_FontEngine* GetFontEngine()
    {
        return NULL;
    }

    virtual void ProcessLabelGroup(SE_LabelInfo*    labels,
                                   int              nlabels,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*     path = NULL)
    {}

    virtual void AddExclusionRegion(RS_F_Point* fpts, int npts)
    {}


    ///////////////////////////////////////////////////////////////////////
    // FeatureInfoRenderer functions

    int GetNumFeaturesProcessed()
    {
        return m_numFeatures;
    }

    int GetNumMaxFeatures()
    {
        return m_nMaxFeatures;
    }

    RS_String GetUrl()
    {
        return m_url;
    }

    RS_String GetTooltip()
    {
        return m_tooltip;
    }

    MgPropertyCollection* GetProperties()
    {
        return SAFE_ADDREF(m_props);
    }

protected:
    //common to FeaturePropRenderer and FeatureInfoRenderer
    RS_String m_layerId;
    RS_String m_fcName;

    int m_numFeatures;
    int m_nMaxFeatures;

    KeyEncode* m_keyEncode;

    // map/layer/feature info
    RS_MapUIInfo* m_mapInfo;
    RS_LayerUIInfo* m_layerInfo;
    RS_FeatureClassInfo* m_fcInfo;

    MgSelection* m_selection;

    RS_Bounds m_extents;
    double m_mapScale;

private:
    //specific to FeatureInfoRenderer
    RS_String m_url;
    RS_String m_tooltip;
    MgPropertyCollection* m_props;
};

#endif
