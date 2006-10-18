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

#ifndef GDRENDERER_H
#define GDRENDERER_H

//warnings about exception definitions in this version of DWF Toolkit
//remove when we update to new version and see what happens
//TODO: well, we still need this even with new version
//but according to Garrick Evans it is safe to ignore
#ifdef _WIN32
#pragma warning(disable : 4290)
#endif

#include <vector>

#include "Renderer.h"
#include "LineBuffer.h"
#include "LabelRendererBase.h"
#include "RS_ByteData.h"

class WT_File;
class WT_Viewport;
class WT_Logical_Point;

struct RS_Font;

class GDRenderer : public Renderer
{
    friend class LabelRenderer;
    friend class LabelRendererLocal;

public:
    STYLIZATION_API GDRenderer(int width,
                               int height,
                               RS_Color& bgColor,
                               bool requiresClipping,
                               bool localOverposting = false,
                               double tileExtentOffset = 0.0);
    STYLIZATION_API ~GDRenderer();

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
                                              const RS_String* theme = NULL);

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

    /////////////////////////////////////////////
    // GDRenderer specific
    //
    STYLIZATION_API void Save(const RS_String& filename, const RS_String& format);
    STYLIZATION_API void Save(const RS_String& filename, const RS_String& format, int width, int height);
    STYLIZATION_API RS_ByteData* Save(const RS_String& format, int width, int height);

    STYLIZATION_API void Combine(const RS_String& fileIn1, const RS_String& fileIn2, const RS_String& fileOut);

    STYLIZATION_API void SetRenderSelectionMode(bool mode);


    void DrawString(const RS_String& s,
                    int              x,
                    int              y,
                    double           height,
                    const RS_Font*   font,
                    const RS_Color&  color,
                    double           angle);

    void MeasureString(const RS_String& s,
                       double           height,
                       const RS_Font* font,
                       double           angle,
                       RS_F_Point*      res,
                       float*           offsets);

    const RS_Font* FindFont(RS_FontDef& def);


private:

    double _MeterToMapSize(RS_Units unit, double number);
    double _PixelToMapSize(Renderer* renderer, int pixels);

    LineBuffer* ApplyLineStyle(LineBuffer* srcLB, wchar_t* lineStyle, double lineWidth, double drawingScale, double dpi);

    void ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds = NULL);

    void WritePolylines(LineBuffer* srclb, RS_LineStroke& stroke, bool aa);

    //transformation from mapping to W2D space
    inline int _TX(double x);
    inline int _TY(double y);
    double _TXD(double x);
    double _TYD(double y);
    double _ITXD(double x);
    double _ITYD(double y);

    void _TransformPointsNoClamp(double* inpts, int numpts);

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
    double m_drawingScale;
    bool m_bRequiresClipping;
    bool m_bLocalOverposting;

    RS_SymbolManager* m_symbolManager;

    //buffer to hold device space points during conversion from line buffers
    //to W2D drawables
    RS_D_Point* m_wtPointBuffer;
    int m_wtPointLen;
    void EnsureBufferSize(int len);

    void* m_imout;
    void* m_imsym;
    int m_lineColor;

    bool m_bSelectionMode;
    RS_FillStyle m_selFill;

    LabelRendererBase* m_labeler;

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

    /*Do not export from DLL*/  double ScaleW2DNumber(WT_File& file, long number);

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
    LineBufferPool*     m_pPool;
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
    RS_FeatureClassInfo* m_fcInfo;

    //storage for override colors
    RS_MarkerDef m_mdOverrideColors;

    //remember id of last symbol
    RS_MarkerDef m_lastSymbol;
};

#endif
