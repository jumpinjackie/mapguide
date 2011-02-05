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

#ifndef GDRENDERER_H
#define GDRENDERER_H

// ignore warnings about exception definitions
#pragma warning(disable: 4290)

#include "Renderers.h"
#include "Renderer.h"
#include "LineBuffer.h"
#include "LabelRendererBase.h"
#include "RS_ByteData.h"

#include "SE_Renderer.h"
#include "SE_SymbolManager.h"
#include "RS_FontEngine.h"
#include "BIDIConverter.h"

#include <vector>

class WT_File;
class WT_Viewport;
class WT_Logical_Point;

struct RS_Font;

class complex_polygon_gd;
class TransformMesh;

class GDRenderer : public SE_Renderer, public RS_FontEngine
{
    friend class LabelRenderer;
    friend class LabelRendererBase;
    friend class LabelRendererLocal;

public:
    RENDERERS_API GDRenderer(int width,
                             int height,
                             RS_Color& bgColor,
                             bool requiresClipping,
                             bool localOverposting,
                             double tileExtentOffset);
    RENDERERS_API virtual ~GDRenderer();

    ///////////////////////////////////
    // Renderer implementation
    //
    RENDERERS_API virtual void StartMap(RS_MapUIInfo*    mapInfo,
                                        RS_Bounds&       extents,
                                        double           mapScale,
                                        double           dpi,
                                        double           metersPerUnit,
                                        CSysTransformer* xformToLL = NULL);

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

    RENDERERS_API virtual void ProcessPolygon(LineBuffer* lb, RS_FillStyle& fill);

    RENDERERS_API virtual void ProcessPolyline(LineBuffer* lb, RS_LineStroke& lsym);

    RENDERERS_API virtual void ProcessRaster(unsigned char* data,
                                             int            length,
                                             RS_ImageFormat format,
                                             int            width,
                                             int            height,
                                             RS_Bounds&     extents,
                                             TransformMesh*   xformMesh = NULL);

    RENDERERS_API virtual void ProcessMarker(LineBuffer* lb, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds = NULL);

    RENDERERS_API virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                                 int              nlabels,
                                                 const RS_String& text,
                                                 RS_OverpostType  type,
                                                 bool             exclude,
                                                 LineBuffer*      path,
                                                 double           scaleLimit);

    RENDERERS_API virtual void AddDWFContent(RS_InputStream*  in,
                                             CSysTransformer* xformer,
                                             const RS_String& section,
                                             const RS_String& passwd,
                                             const RS_String& w2dfilter);

    RENDERERS_API virtual void SetSymbolManager(RS_SymbolManager* manager);

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

    virtual bool SupportsTooltips();

    virtual bool SupportsHyperlinks();

    /////////////////////////////////////////////
    // GDRenderer specific
    //
    RENDERERS_API RS_ByteData* Save(const RS_String& format, int width, int height);
    RENDERERS_API void Save(const RS_String& filename, const RS_String& format);
    RENDERERS_API void Save(const RS_String& filename, const RS_String& format, int width, int height);

    void Combine(const RS_String& fileIn1, const RS_String& fileIn2, const RS_String& fileOut);

    ////////////////////////////////////////////////
    // SE_Renderer
    //
    virtual void SetRenderSelectionMode(bool mode);
    virtual void SetRenderSelectionMode(bool mode, int rgba);

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

    virtual void ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style);
    virtual void ProcessArea(SE_ApplyContext* ctx, SE_RenderAreaStyle* style);

    virtual void AddExclusionRegion(RS_F_Point* fpts, int npts);

    ////////////////////////////////////////////////
    // RS_FontEngine
    //
    virtual void MeasureString(const RS_String& s,
                               double           height,
                               const RS_Font*   font,
                               double           angleRad,
                               RS_F_Point*      res,
                               float*           offsets);

    virtual void DrawString(const RS_String& s,
                            double           x,
                            double           y,
                            double           width,
                            double           height,
                            const RS_Font*   font,
                            RS_Color&        color,
                            double           angleRad);

    virtual const RS_Font* FindFont(RS_FontDef& def);

private:
    double _MeterToMapSize(RS_Units unit, double number);

    LineBuffer* ApplyLineStyle(LineBuffer* srcLB, wchar_t* lineStyle, double lineWidthPixels, double drawingScale, double dpi);

    void ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds = NULL);

    void WritePolylines(LineBuffer* srclb, RS_LineStroke& stroke, bool aa);

    //transformation from mapping to W2D space
    inline int _TX(double x);
    inline int _TY(double y);

    void _TransformPointsNoClamp(LineBuffer* plb);
    void _TransformContourPointsNoClamp(LineBuffer* plb, int cntr);
    void _TransferPoints(LineBuffer* plb, const SE_Matrix* xform);
    void _TransferContourPoints(LineBuffer* plb, int cntr, const SE_Matrix* xform);

    RS_Color m_bgcolor;
    RS_Bounds m_extents;
    double m_metersPerUnit;
    double m_dpi;
    double m_mapScale;
    double m_scale;
    double m_invScale;
    double m_offsetX;
    double m_offsetY;
    int m_width;
    int m_height;
    int m_maxLineWidth;
    double m_drawingScale;
    bool m_bRequiresClipping;

    RS_SymbolManager* m_symbolManager;

    //buffer to hold device space points during conversion from line buffers
    //to W2D drawables
    RS_D_Point* m_wtPointBuffer;
    int m_wtPointLen;
    void EnsureBufferSize(int len);

    void* m_imout;
    void* m_imsym;
    int m_lineColor;

    RS_FillStyle m_selFill;

    LabelRendererBase* m_labeler;

    BIDIConverter m_bidiConverter;

public:
    RENDERERS_API static bool s_bGeneralizeData;

    /////////////////////////////////////////////////////////
    //
    // Functions and structures used during insertion of W2Ds
    //
    /////////////////////////////////////////////////////////

    //TODO: these should be extracted to a friend class that holds
    // all of the W2D rewriting context

public:
    /*Do not export from DLL*/ void* GetImage() { return m_imout; } //target map image
    /*Do not export from DLL*/ void* GetW2DTargetImage() { return m_imw2d; } //target image for W2D rewriter
    /*Do not export from DLL*/ complex_polygon_gd* GetPolyRasterizer() { return m_polyrasterizer; } //for drawing contour sets
    /*Do not export from DLL*/ bool IsViewportSet() { return m_bHaveViewport; }
    /*Do not export from DLL*/ bool IsSymbolW2D() { return m_bIsSymbolW2D; }
    /*Do not export from DLL*/ void UpdateSymbolTrans(WT_File& file, WT_Viewport& viewport);
    /*Do not export from DLL*/ RS_MarkerDef& GetOverrideColorMarker() { return m_mdOverrideColors; }
    /*Do not export from DLL*/ RS_String& GetLayerFilter() { return m_layerFilter; }
    /*Do not export from DLL*/ bool& LayerPassesFilter() { return m_bLayerPassesFilter; }

    /*Do not export from DLL*/ RS_InputStream* _GetInputStream() { return m_input; }
    /*Do not export from DLL*/ const RS_D_Point* ProcessW2DPoints(WT_File&          file,
                                                                  WT_Logical_Point* srcpts,
                                                                  int               numpts,
                                                                  bool              checkInBounds);
    /*Do not export from DLL*/ double ScaleW2DNumber(WT_File& file, long number);
    /*Do not export from DLL*/ const RS_D_Point* FillPointBuffer(LineBuffer* lb);

private:
    void AddW2DContent(RS_InputStream* in, CSysTransformer* xformer, const RS_String& w2dfilter);

    void SetActions(WT_File& file);

    //////
    //State variables for when DWFRenderer is used to keep
    //information during DWF rewriting process for symbols
    //and DWF layers
    std::vector<int>    m_cntrs;
    RS_InputStream*     m_input;
    CSysTransformer*    m_xformer;
    bool                m_bIsSymbolW2D;
    bool                m_bHaveViewport;
    bool                m_bLayerPassesFilter;
    RS_String           m_layerFilter;

    //target image for W2D rewriter -- equal to either the target map
    //image or to a temporary cached symbol image in case we are
    //processing a symbol from the library
    void* m_imw2d;

    // map/layer/feature info
    RS_MapUIInfo* m_mapInfo;
    RS_LayerUIInfo* m_layerInfo;
    RS_FeatureClassInfo* m_featureClassInfo;

    //storage for override colors
    RS_MarkerDef m_mdOverrideColors;

    //remember id of last symbol
    RS_MarkerDef m_lastSymbol;

    //polygon rasterizer
    complex_polygon_gd* m_polyrasterizer;
};

#endif
