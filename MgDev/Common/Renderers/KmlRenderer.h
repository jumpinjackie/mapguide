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

#ifndef KMLRENDERER_H
#define KMLRENDERER_H

#include "Renderers.h"
#include "Renderer.h"
#include "SE_Renderer.h"
#include "KmlContent.h"
#include <vector>
#include <map>


typedef std::map<RS_String, KmlContent*> ThemeMap;
typedef std::map<KmlLineStyle, int> KmlLineStyleIdMap;
typedef std::map<KmlPolyStyle, int> KmlPolyStyleIdMap;
typedef std::map<KmlIconStyle, int> KmlIconStyleIdMap;

class TransformMesh;

class KmlRenderer : public SE_Renderer
{
public:
    RENDERERS_API KmlRenderer(KmlContent* kmlContent,
                              RS_Bounds& extents,
                              double scale,
                              double dpi,
                              double metersPerUnit,
                              int drawOrder);
    RENDERERS_API virtual ~KmlRenderer();

    ///////////////////////////////////
    // Renderer implementation

    RENDERERS_API virtual void StartMap(RS_MapUIInfo*    mapInfo,
                                        RS_Bounds&       extents,
                                        double           mapScale,
                                        double           dpi,
                                        double           metersPerUnit,
                                        CSysTransformer* xformToLL);

    RENDERERS_API virtual void EndMap();

    RENDERERS_API virtual void StartLayer(RS_LayerUIInfo*      layerInfo,
                                          RS_FeatureClassInfo* classInfo);

    RENDERERS_API virtual void EndLayer();

    RENDERERS_API virtual void StartFeature(RS_FeatureReader* feature,
                                            bool              initialPass,
                                            const RS_String*  tooltip = NULL,
                                            const RS_String*  url = NULL,
                                            const RS_String*  theme = NULL,
                                            double            zOffset = 0.0,
                                            double            zExtrusion = 0.0,
                                            RS_ElevationType  zOffsetType = RS_ElevationType_RelativeToGround);

    virtual void ProcessPolygon(LineBuffer* lb, RS_FillStyle& fill);

    virtual void ProcessPolyline(LineBuffer* lb, RS_LineStroke& lsym);

    virtual void ProcessRaster(unsigned char* data,
                               int            length,
                               RS_ImageFormat format,
                               int            width,
                               int            height,
                               RS_Bounds&     extents,
                               TransformMesh* xformMesh = NULL);

    virtual void ProcessMarker(LineBuffer* lb, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds = NULL);

    virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path,
                                   double           scaleLimit);

    virtual void AddDWFContent(RS_InputStream*  in,
                               CSysTransformer* xformer,
                               const RS_String& section,
                               const RS_String& passwd,
                               const RS_String& w2dfilter);

    virtual void SetSymbolManager(RS_SymbolManager* manager);

    virtual RS_MapUIInfo* GetMapInfo();

    virtual RS_LayerUIInfo* GetLayerInfo();

    virtual RS_FeatureClassInfo* GetFeatureClassInfo();

    virtual double GetMapScale();

    virtual double GetDrawingScale();

    virtual double GetMetersPerUnit();

    virtual double GetDpi();

    virtual RS_Bounds& GetBounds();

    virtual bool RequiresClipping();

    virtual bool RequiresLabelClipping();

    virtual bool SupportsZ();

    ////////////////////////////////////////////////
    // SE_Renderer
    //
    virtual void DrawScreenPolyline(LineBuffer* geom, const SE_Matrix* xform, const SE_LineStroke& lineStroke);
    virtual void DrawScreenPolygon(LineBuffer* geom, const SE_Matrix* xform, unsigned int fill);
    virtual void DrawScreenRaster(unsigned char* data, int length,
                                  RS_ImageFormat format, int native_width, int native_height,
                                  double x, double y, double w, double h, double angleDeg);
    virtual void DrawScreenRaster(unsigned char* data, int length,
                                  RS_ImageFormat format, int native_width, int native_height,
                                  double x, double y, double w, double h, double angleDeg,
                                  double alpha);
    virtual void DrawScreenText(const RS_TextMetrics& tm, RS_TextDef& tdef, double insx, double insy,
                                RS_F_Point* path, int npts, double param_position);

    virtual bool YPointsUp();
    virtual void GetWorldToScreenTransform(SE_Matrix& xform);
    virtual void WorldToScreenPoint(double& inx, double& iny, double& ox, double& oy);
    virtual void ScreenToWorldPoint(double& inx, double& iny, double& ox, double& oy);

    virtual double GetScreenUnitsPerMillimeterDevice();
    virtual double GetScreenUnitsPerMillimeterWorld();
    virtual double GetScreenUnitsPerPixel();

    virtual RS_FontEngine* GetRSFontEngine();

    virtual void ProcessSELabelGroup(SE_LabelInfo*   labels,
                                     int             nlabels,
                                     RS_OverpostType type,
                                     bool            exclude,
                                     LineBuffer*     path = NULL);

    virtual void AddExclusionRegion(RS_F_Point* fpts, int npts);

    virtual void ProcessPoint(SE_ApplyContext* ctx, SE_RenderPointStyle* style, RS_Bounds* bounds = NULL);
    virtual void ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style);
    virtual void ProcessArea(SE_ApplyContext* ctx, SE_RenderAreaStyle* style);

private:
    // Unimplemented Constructors/Methods
    KmlRenderer(const KmlRenderer&);
    KmlRenderer& operator=(const KmlRenderer&);

    void ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost);
    void WriteStyle(RS_FillStyle& fill);
    void WriteStyle(RS_LineStroke& lsym);
    void WriteStyle(double scale, const std::wstring& href);
    void WriteLinearRing(LineBuffer* plb, int contour);
    void WriteLinearRing(LineBuffer* plb);
    void WriteCoordinates(LineBuffer* plb);
    void WriteContourCoordinates(LineBuffer* plb, int cntr);
    void WriteElevationSettings();
    void ClearThemes();
    void ClearStyles();
    double _MeterToPixels(RS_Units unit, double number);

    RS_Bounds& m_extents;
    KmlContent* m_kmlContent;
    KmlContent* m_mainContent;
    KmlContent* m_styleContent;
    int m_featureCount;
    RS_MapUIInfo* m_mapInfo;
    RS_LayerUIInfo* m_layerInfo;
    RS_FeatureClassInfo* m_featureClassInfo;
    double m_mapScale;
    ThemeMap m_themeMap;
    int m_styleId;
    KmlLineStyleIdMap m_lineStyleMap;
    KmlPolyStyleIdMap m_polyStyleMap;
    KmlIconStyleIdMap m_iconStyleMap;
    double m_pixelSize;
    int m_drawOrder;
    double m_elevation;
    bool m_extrude;
    double m_metersPerUnit;
    RS_ElevationType m_elevType;
};

#endif
