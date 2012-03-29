//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef AGGRenderer_H
#define AGGRenderer_H

// ignore warnings about exception definitions and deprecated methods
#pragma warning(disable: 4290)
#pragma warning(disable: 4996)

#include <vector>

#include "Renderer.h"
#include "LineBuffer.h"
#include "LabelRendererBase.h"
#include "RS_ByteData.h"
#include "SE_Renderer.h"
#include "RS_FontEngine.h"
#include "BIDIConverter.h"

class WT_File;
class WT_Viewport;
class WT_Logical_Point;

struct RS_Font;

class agg_context;
class TransformMesh;

namespace agg
{
    template<class T> class row_ptr_cache;
    struct trans_affine;
};
typedef agg::row_ptr_cache<unsigned char> mg_rendering_buffer;

class AGGRenderer : public SE_Renderer, public RS_FontEngine
{
    friend class LabelRenderer;
    friend class LabelRendererBase;
    friend class LabelRendererLocal;

public:
    RENDERERS_API AGGRenderer(int width,
                              int height,
                              unsigned int* backbuffer,
                              bool requiresClipping,
                              bool localOverposting,
                              double tileExtentOffset);

    RENDERERS_API AGGRenderer(int width,
                              int height,
                              RS_Color& bgColor,
                              bool requiresClipping,
                              bool localOverposting,
                              double tileExtentOffset);

    RENDERERS_API virtual ~AGGRenderer();

    ///////////////////////////////////
    // Renderer implementation
    //
    RENDERERS_API virtual void StartMap(RS_MapUIInfo* mapInfo,
                                        RS_Bounds&    extents,
                                        double        mapScale,
                                        double        dpi,
                                        double        metersPerUnit,
                                        CSysTransformer* xformToLL = NULL);

    RENDERERS_API virtual void EndMap();

    RENDERERS_API virtual void StartLayer(RS_LayerUIInfo*      legendInfo,
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
                                             int length,
                                             RS_ImageFormat format,
                                             int width, int height,
                                             RS_Bounds& extents,
                                             TransformMesh* xformMesh = NULL);

    RENDERERS_API virtual void ProcessMarker(LineBuffer* lb, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds = NULL);

    RENDERERS_API virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                                 int              nlabels,
                                                 const RS_String& text,
                                                 RS_OverpostType  type,
                                                 bool             exclude,
                                                 LineBuffer*      path,
                                                 double           scaleLimit);

    RENDERERS_API virtual void SetSymbolManager(RS_SymbolManager* manager);

    RENDERERS_API virtual RS_MapUIInfo* GetMapInfo();

    RENDERERS_API virtual RS_LayerUIInfo* GetLayerInfo();

    RENDERERS_API virtual RS_FeatureClassInfo* GetFeatureClassInfo();

    RENDERERS_API virtual double GetMapScale();

    RENDERERS_API virtual double GetDrawingScale();

    RENDERERS_API virtual RS_Bounds& GetBounds();

    RENDERERS_API virtual double GetDpi();

    RENDERERS_API virtual double GetMetersPerUnit();

    RENDERERS_API virtual bool RequiresClipping();

    RENDERERS_API virtual bool RequiresLabelClipping();

    RENDERERS_API virtual bool SupportsZ();

    RENDERERS_API virtual bool SupportsTooltips();

    RENDERERS_API virtual bool SupportsHyperlinks();

    /////////////////////////////////////////////
    // AGGRenderer specific
    //
    RENDERERS_API void Save(const RS_String& filename, const RS_String& format);
    RENDERERS_API void Save(const RS_String& filename, const RS_String& format, int width, int height);
    RENDERERS_API RS_ByteData* Save(const RS_String& format, int width, int height,
                                    RS_ColorVector* baseColorPalette = NULL);

    RENDERERS_API void Combine(const RS_String& fileIn1, const RS_String& fileIn2, const RS_String& fileOut);
    RENDERERS_API void SetWorldToScreenTransform(SE_Matrix& xform);

    RENDERERS_API void DrawString(const RS_String& s,
                                  double           x,
                                  double           y,
                                  double           width,
                                  double           height,
                                  const RS_Font*   font,
                                  RS_Color&  color,
                                  double           angle);

    RENDERERS_API void MeasureString(const RS_String& s,
                                     double           height,
                                     const RS_Font*   font,
                                     double           angleRad,
                                     RS_F_Point*      res,
                                     float*           offsets);

    RENDERERS_API const RS_Font* FindFont(RS_FontDef& def);

    RENDERERS_API virtual void SetRenderSelectionMode(bool mode);
    RENDERERS_API virtual void SetRenderSelectionMode(bool mode, int rgba);

    RENDERERS_API virtual void DrawScreenPolyline(LineBuffer* polyline, const SE_Matrix* xform, const SE_LineStroke& lineStroke);
    RENDERERS_API virtual void DrawScreenPolygon(LineBuffer* polygon, const SE_Matrix* xform, unsigned int fill);
    RENDERERS_API virtual void DrawScreenRaster(unsigned char* data, int length,
                                                RS_ImageFormat format, int native_width, int native_height,
                                                double x, double y, double w, double h, double angledeg);
    RENDERERS_API virtual void DrawScreenRaster(unsigned char* data, int length,
                                                RS_ImageFormat format, int native_width, int native_height,
                                                double x, double y, double w, double h, double angledeg, double alpha);
    RENDERERS_API virtual void DrawScreenText(const RS_TextMetrics& tm, RS_TextDef& tdef, double insx, double insy, RS_F_Point* path, int npts, double param_position );

    RENDERERS_API virtual bool YPointsUp();
    RENDERERS_API virtual void GetWorldToScreenTransform(SE_Matrix& xform);
    RENDERERS_API virtual void WorldToScreenPoint(double& inx, double& iny, double& ox, double& oy);
    RENDERERS_API virtual void ScreenToWorldPoint(double& inx, double& iny, double& ox, double& oy);

    RENDERERS_API virtual double GetScreenUnitsPerMillimeterDevice();
    RENDERERS_API virtual double GetScreenUnitsPerMillimeterWorld();
    RENDERERS_API virtual double GetScreenUnitsPerPixel();

    RENDERERS_API virtual RS_FontEngine* GetRSFontEngine();

    RENDERERS_API virtual void ProcessSELabelGroup(SE_LabelInfo*    labels,
                                                   int              nlabels,
                                                   RS_OverpostType  type,
                                                   bool             exclude,
                                                   LineBuffer*      path = NULL);

    RENDERERS_API virtual void ProcessLine(SE_ApplyContext* ctx, SE_RenderLineStyle* style);
    RENDERERS_API virtual void ProcessArea(SE_ApplyContext* ctx, SE_RenderAreaStyle* style);

    RENDERERS_API virtual void AddExclusionRegion(RS_F_Point* fpts, int npts);

    RENDERERS_API void AddDWFContent(RS_InputStream*  in,
                                     CSysTransformer* xformer,
                                     const RS_String& section,
                                     const RS_String& passwd,
                                     const RS_String& w2dfilter);

    RENDERERS_API static void DrawScreenPolyline(agg_context* cxt, LineBuffer* polyline, const SE_Matrix* xform, const SE_LineStroke& lineStroke);
    RENDERERS_API static void DrawScreenPolygon(agg_context* cxt, LineBuffer* polygon, const SE_Matrix* xform, unsigned int fill);
    RENDERERS_API static void DrawScreenRaster(agg_context* cxt, unsigned char* data, int length,
                                               RS_ImageFormat format, int native_width, int native_height,
                                               double x, double y, double w, double h, double angledeg);
    RENDERERS_API static void DrawScreenRasterTransform(agg_context* cxt, unsigned char* data, int length,
                                               RS_ImageFormat format, int native_width, int native_height,
                                               double x, double y, double w, double h,
                                               TransformMesh* xformMesh = NULL);

    RENDERERS_API static void DrawString(agg_context*     cxt,
                                         const RS_String& s,
                                         double           x,
                                         double           y,
                                         double           width,
                                         double           height,
                                         const RS_Font*   font,
                                         RS_Color&        color,
                                         double           angle);

    RENDERERS_API void SetPolyClip(LineBuffer* polygon, double bufferWidth);

    RENDERERS_API bool UseLocalOverposting();

    RENDERERS_API void UpdateBackBuffer(int width, int height, unsigned int* backbuffer);
    RENDERERS_API unsigned int* GetBackBuffer(int &width, int& height);

private:
    double _MeterToMapSize(RS_Units unit, double number);

    LineBuffer* ApplyLineStyle(LineBuffer* srcLB, wchar_t* lineStyle, double lineWidth, double drawingScale, double dpi);

    void ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds = NULL);

    static void _TransferPoints(agg_context* c, LineBuffer* src, const SE_Matrix* xform, unsigned int* pathids, bool isPolygon);
    static void _TransferPointsClamped(agg_context* c, LineBuffer* src, const SE_Matrix* xform, unsigned int* pathids, bool isPolygon);

    static void RenderTransformMeshRectangle(mg_rendering_buffer& src, agg_context* cxt, RS_ImageFormat format,
                                 TransformMesh* transformMesh, int lowerLeftIndex, int lowerRightIndex, int upperLeftIndex, int upperRightIndex);

    static void RenderTransformedTriangle(mg_rendering_buffer& src, agg_context* cxt, RS_ImageFormat format,
                                 RS_F_Point srcPt1, RS_F_Point srcPt2, RS_F_Point srcPt3,
                                 RS_F_Point destPt1, RS_F_Point destPt2, RS_F_Point destPt3);

    static void RenderWithTransform(mg_rendering_buffer& src, agg_context* cxt,
        agg::trans_affine& img_mtx, RS_ImageFormat format, bool antiAlias = true);

    RS_Color m_bgcolor;
    RS_Bounds m_extents;
    double m_metersPerUnit;
    double m_dpi;
    double m_mapScale;

    SE_Matrix m_xform;
    SE_Matrix m_ixform;

    int m_width;
    int m_height;
    double m_drawingScale;
    bool m_bRequiresClipping;
    bool m_bLocalOverposting;

    RS_SymbolManager* m_symbolManager;

    agg_context* m_context;
    agg_context* c() { return m_context; }

    //screen buffer
    unsigned int* m_rows;
    bool m_bownbuffer;

    agg_context* m_imsym;

    int m_lineColor;

    RS_FillStyle m_selFill;

    SE_LineStroke m_lineStroke;

    LabelRendererBase* m_labeler;

    BIDIConverter m_bidiConverter;

private:
    //target image for W2D rewriter -- equal to either the target map
    //image or to a temporary cached symbol image in case we are
    //processing a symbol from the library
    agg_context* m_imw2d;

    // map/layer/feature info
    RS_MapUIInfo* m_mapInfo;
    RS_LayerUIInfo* m_layerInfo;
    RS_FeatureClassInfo* m_fcInfo;

public:
    RENDERERS_API static bool s_bClampPoints;
    RENDERERS_API static bool s_bGeneralizeData;

    /////////////////////////////////////////////////////////
    //
    // Functions and structures used during insertion of W2Ds
    //
    /////////////////////////////////////////////////////////

    //TODO: these should be extracted to a friend class that holds
    // all of the W2D rewriting context

public:
    /*Do not export from DLL*/ void* GetImage() { return c(); } //target map image
    /*Do not export from DLL*/ void* GetW2DTargetImage() { return m_imw2d; } //target image for W2D rewriter
    /*Do not export from DLL*/ bool IsViewportSet() { return m_bHaveViewport; }
    /*Do not export from DLL*/ bool IsSymbolW2D() { return m_bIsSymbolW2D; }
    /*Do not export from DLL*/ void UpdateSymbolTrans(WT_File& file, WT_Viewport& viewport);
    /*Do not export from DLL*/ RS_MarkerDef& GetOverrideColorMarker() { return m_mdOverrideColors; }
    /*Do not export from DLL*/ RS_String& GetLayerFilter() { return m_layerFilter; }
    /*Do not export from DLL*/ bool& LayerPassesFilter() { return m_bLayerPassesFilter; }

    /*Do not export from DLL*/ RS_InputStream* _GetInputStream() { return m_input; }
    /*Do not export from DLL*/ LineBuffer* ProcessW2DPoints(WT_File&          file,
                                                              WT_Logical_Point* srcpts,
                                                              int               numpts,
                                                              bool              checkInBounds);
    /*Do not export from DLL*/ double ScaleW2DNumber(WT_File& file, long number);

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

    //storage for override colors
    RS_MarkerDef m_mdOverrideColors;

    //remember id of last symbol
    RS_MarkerDef m_lastSymbol;

    // remember last font used
    RS_FontDef m_lastFontDef;
    const RS_Font* m_lastFont;
};

#endif
