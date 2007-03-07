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

#ifndef KMLRENDERER_H
#define KMLRENDERER_H

//warnings about exception definitions in this version of DWF Toolkit
//remove when we update to new version and see what happens
//TODO: well, we still need this even with new version
//but according to Garrick Evans it is safe to ignore
#ifdef _WIN32
#pragma warning(disable : 4290)
#endif

#include <vector>
#include <map>

#include "Renderer.h"
#include "KmlContent.h"
    
typedef std::map<RS_String, KmlContent*> ThemeMap;
typedef std::map<KmlLineStyle,int> KmlLineStyleIdMap;
typedef std::map<KmlPolyStyle,int> KmlPolyStyleIdMap;

const double METERS_PER_INCH = 0.0254; 

class KmlRenderer : public Renderer
{
public:

    STYLIZATION_API KmlRenderer(KmlContent* kmlContent, RS_Bounds& extents, 
        double scale, double dpi, int drawOrder);
    STYLIZATION_API ~KmlRenderer();

    ///////////////////////////////////
    // Renderer implementation

    STYLIZATION_API virtual void StartMap(RS_MapUIInfo* mapInfo,
                                          RS_Bounds&    extents,
                                          double        mapScale,
                                          double        dpi,
                                          double        metersPerUnit,
                                          CSysTransformer* xformToLL = NULL);

    STYLIZATION_API virtual void EndMap();

    STYLIZATION_API virtual void StartLayer(RS_LayerUIInfo*      legendInfo,
                                            RS_FeatureClassInfo* classInfo);

    STYLIZATION_API virtual void EndLayer();

    STYLIZATION_API virtual void StartFeature(RS_FeatureReader* feature,
                                              const RS_String*  tooltip = NULL,
                                              const RS_String*  url = NULL, 
                                              const RS_String* theme = NULL,
                                              double zOffset = 0,
                                              double zExtrusion = 0,
                                              RS_ElevationType zOffsetType = RS_ElevationType_RelativeToGround);


    STYLIZATION_API virtual void ProcessPolygon(LineBuffer* lb, RS_FillStyle& fill);

    STYLIZATION_API virtual void ProcessPolyline(LineBuffer* lb, RS_LineStroke& lsym);

    STYLIZATION_API virtual void ProcessRaster(unsigned char* data,
                                               int length,
                                               RS_ImageFormat format,
                                               int width, int height,
                                               RS_Bounds extents);

    STYLIZATION_API virtual void ProcessMarker(LineBuffer* lb, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds = NULL);

    STYLIZATION_API virtual void ProcessLabel(double x, double y, const RS_String& text, RS_TextDef& tdef);

    STYLIZATION_API virtual void ProcessLabelGroup( RS_LabelInfo*    labels,
                                                    int              nlabels,
                                                    const RS_String& text,
                                                    RS_OverpostType  type,
                                                    bool             exclude,
                                                    LineBuffer*      path);

    STYLIZATION_API virtual void AddDWFContent(RS_InputStream*  in,
                                               CSysTransformer* xformer,
                                               const RS_String& section,
                                               const RS_String& passwd,
                                               const RS_String& w2dfilter);

    STYLIZATION_API virtual void SetSymbolManager(RS_SymbolManager* manager);

    STYLIZATION_API virtual RS_MapUIInfo* GetMapInfo();

    STYLIZATION_API virtual RS_LayerUIInfo* GetLayerInfo();

    STYLIZATION_API virtual RS_FeatureClassInfo* GetFeatureClassInfo();

    STYLIZATION_API virtual double GetMapScale();

    STYLIZATION_API virtual RS_Bounds& GetBounds();

    STYLIZATION_API virtual double GetDpi();

    STYLIZATION_API virtual double GetMetersPerUnit();

    STYLIZATION_API virtual double GetMapToScreenScale();

    STYLIZATION_API virtual bool RequiresClipping();

    STYLIZATION_API virtual bool UseLocalOverposting();

private:

    // Unimplemented Constructors/Methods
    KmlRenderer(const KmlRenderer&);
    KmlRenderer& operator=(const KmlRenderer&);

    void ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost);
    void WriteStyle(RS_FillStyle& fill);
    void WriteStyle(RS_LineStroke& lsym);
    void WriteLinearRing(double* points, int offset, int numPoints);
    void WriteCoordinates(double* points, int offset, int numPoints);
    void WriteElevationSettings();
    void ClearThemes();
    void ClearStyles();

    RS_Bounds& m_extents;
    KmlContent* m_kmlContent;
    KmlContent* m_mainContent;
    KmlContent* m_styleContent;
    int m_featureCount;
    RS_LayerUIInfo* m_layerInfo;
    RS_FeatureClassInfo* m_featureClassInfo;
    double m_scale;
    ThemeMap m_themeMap;
    int m_styleId;
    KmlLineStyleIdMap m_lineStyleMap;
    KmlPolyStyleIdMap m_polyStyleMap;
    double m_pixelSize;
    int m_drawOrder;
    double m_elevation;
    bool m_extrude;
    RS_ElevationType m_elevType;
};

#endif
