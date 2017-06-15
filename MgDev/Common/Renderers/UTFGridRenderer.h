//
//  Copyright (C) 2004-2017 by Autodesk, Inc.
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

#ifndef _UTFGRIDRENDERER_H_
#define _UTFGRIDRENDERER_H_

#include "Renderers.h"
#include "SE_Renderer.h"
#include "UTFGridContent.h"

#define UTF_GRID_WIDTH 256
#define UTF_GRID_HEIGHT 256

#define UTF_GRID_DEFAULT_RESOLUTION 4

class agg_utfgrid_context;

class UTFGridRenderer : public SE_Renderer
{
public:
    RENDERERS_API UTFGridRenderer(UTFGridContent* utfGrid, unsigned int resolution = UTF_GRID_DEFAULT_RESOLUTION);
    RENDERERS_API virtual ~UTFGridRenderer();

    // Inherited via SE_Renderer
    RENDERERS_API virtual void StartMap(RS_MapUIInfo * mapInfo, RS_Bounds & extents, double mapScale, double dpi, double metersPerUnit, CSysTransformer * xformToLL);
    RENDERERS_API virtual void EndMap();
    RENDERERS_API virtual void StartLayer(RS_LayerUIInfo * layerInfo, RS_FeatureClassInfo * classInfo);
    RENDERERS_API virtual void EndLayer();
    RENDERERS_API virtual void StartFeature(RS_FeatureReader * feature, bool initialPass, const RS_String * tooltip = NULL, const RS_String * url = NULL, const RS_String * theme = NULL, double zOffset = 0.0, double zExtrusion = 0.0, RS_ElevationType zOffsetType = RS_ElevationType_RelativeToGround);
    RENDERERS_API virtual void ProcessPolygon(LineBuffer * lb, RS_FillStyle & fill);
    RENDERERS_API virtual void ProcessPolyline(LineBuffer * lb, RS_LineStroke & lsym);
    RENDERERS_API virtual void ProcessRaster(unsigned char * data, int length, RS_ImageFormat format, int width, int height, RS_Bounds & extents, TransformMesh * xformMesh = NULL);
    RENDERERS_API virtual void ProcessMarker(LineBuffer * lb, RS_MarkerDef & mdef, bool allowOverpost, RS_Bounds * bounds = NULL);
    RENDERERS_API virtual void ProcessLabelGroup(RS_LabelInfo * labels, int nlabels, const RS_String & text, RS_OverpostType type, bool exclude, LineBuffer * path, double scaleLimit);
    RENDERERS_API virtual void AddDWFContent(RS_InputStream * in, CSysTransformer * xformer, const RS_String & section, const RS_String & passwd, const RS_String & w2dfilter);
    RENDERERS_API virtual void SetSymbolManager(RS_SymbolManager * manager);
    RENDERERS_API virtual RS_MapUIInfo * GetMapInfo();
    RENDERERS_API virtual RS_LayerUIInfo * GetLayerInfo();
    RENDERERS_API virtual RS_FeatureClassInfo * GetFeatureClassInfo();
    RENDERERS_API virtual double GetMapScale();
    RENDERERS_API virtual double GetDrawingScale();
    RENDERERS_API virtual double GetMetersPerUnit();
    RENDERERS_API virtual double GetDpi();
    RENDERERS_API virtual RS_Bounds & GetBounds();
    RENDERERS_API virtual bool RequiresClipping();
    RENDERERS_API virtual bool RequiresLabelClipping();
    RENDERERS_API virtual bool SupportsZ();
    RENDERERS_API virtual void DrawScreenPolyline(LineBuffer * polyline, const SE_Matrix * xform, const SE_LineStroke & lineStroke);
    RENDERERS_API virtual void DrawScreenPolygon(LineBuffer * polygon, const SE_Matrix * xform, unsigned int fill);
    RENDERERS_API virtual void DrawScreenRaster(unsigned char * data, int length, RS_ImageFormat format, int native_width, int native_height, double x, double y, double w, double h, double angleDeg);
    RENDERERS_API virtual void DrawScreenRaster(unsigned char * data, int length, RS_ImageFormat format, int native_width, int native_height, double x, double y, double w, double h, double angleDeg, double alpha);
    RENDERERS_API virtual void DrawScreenText(const RS_TextMetrics & tm, RS_TextDef & tdef, double insx, double insy, RS_F_Point * path, int npts, double param_position);
    RENDERERS_API virtual bool YPointsUp();
    RENDERERS_API virtual void GetWorldToScreenTransform(SE_Matrix & xform);
    RENDERERS_API virtual void WorldToScreenPoint(double & inx, double & iny, double & ox, double & oy);
    RENDERERS_API virtual void ScreenToWorldPoint(double & inx, double & iny, double & ox, double & oy);
    RENDERERS_API virtual double GetScreenUnitsPerMillimeterDevice();
    RENDERERS_API virtual double GetScreenUnitsPerMillimeterWorld();
    RENDERERS_API virtual double GetScreenUnitsPerPixel();
    RENDERERS_API virtual RS_FontEngine * GetRSFontEngine();
    RENDERERS_API virtual void ProcessSELabelGroup(SE_LabelInfo * labels, int nlabels, RS_OverpostType type, bool exclude, LineBuffer * path = NULL);
    RENDERERS_API virtual void AddExclusionRegion(RS_F_Point * fpts, int npts);

    RENDERERS_API static void DrawScreenPolyline(agg_utfgrid_context* cxt, LineBuffer* polyline, const SE_Matrix* xform, const SE_LineStroke& lineStroke);

private:
    void ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost);
    double _MeterToMapSize(RS_Units unit, double number);
    static void _TransferPoints(agg_utfgrid_context* c, LineBuffer* src, const SE_Matrix* xform, unsigned int* pathids);

    RS_Bounds m_extents;
    double m_drawingScale;
    double m_metersPerUnit;
    double m_dpi;
    double m_mapScale;

    // map/layer/feature info
    RS_MapUIInfo* m_mapInfo;
    RS_LayerUIInfo* m_layerInfo;
    RS_FeatureClassInfo* m_fcInfo;

    unsigned int m_currentColor;

    SE_Matrix m_xform;
    SE_Matrix m_ixform;

    agg_utfgrid_context* m_context;

    //screen buffer
    unsigned int* m_rows;

    //Not used, just passed around to satisfy required func signatures
    SE_LineStroke m_lineStroke;

    UTFGridContent* m_content;

    bool m_renderThisFeature;
};

#endif