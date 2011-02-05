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

#ifndef DWFRENDERER_H
#define DWFRENDERER_H

// ignore warnings about exception definitions
#pragma warning(disable: 4290)

#include "Renderers.h"
#include "Renderer.h"
#include "LineBuffer.h"
#include "KeyEncode.h"

#include "SE_Renderer.h"
#include "RS_FontEngine.h"
#include "BIDIConverter.h"

#include <vector>


// forward declare
class WT_File;
class WT_Viewport;
class WT_Logical_Point;
class WT_Dash_Pattern;
class WT_Line_Pattern;

class EMapHatchPatternFactory;
class EMapFillPatternFactory;

class ObservationMesh;
class TransformMesh;

namespace DWFCore
{
    class DWFBufferOutputStream;
    class DWFBufferInputStream;
    class DWFString;
}

namespace DWFToolkit
{
    class DWFDefinedObject;
    class DWFDefinedObjectInstance;
    class DWFObjectDefinitionResource;
    class DWFSection;
};


// holds W2D streams, and object data streams
typedef std::vector<DWFCore::DWFBufferOutputStream*> stream_list;
typedef std::vector<std::wstring> wstring_list;
typedef std::vector<DWFToolkit::DWFObjectDefinitionResource*> objdefres_list;


// package info
#define MAP_PRODUCT_AUTHOR  L"Autodesk"
#define MAP_PRODUCT_NAME    L"MapGuide Open Source"
#define MAP_PRODUCT_VERSION L"1.0.0"


///<summary>
/// Implementation of Renderer for DWF-W2D output
///</summary>
class DWFRenderer : public SE_Renderer, public RS_FontEngine
{
public:
    RENDERERS_API DWFRenderer();
    RENDERERS_API virtual ~DWFRenderer();

    ///////////////////////////////////
    // Renderer implementation
    //
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

    /////////////////////////////////////////////
    // DWFRenderer specific
    //
    RENDERERS_API double GetMapToW2DScale();

    RENDERERS_API void StartLayout(RS_Bounds& extents);

    RENDERERS_API void EndLayout();

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

    virtual void ProcessSELabelGroup(SE_LabelInfo* labels, int nlabels, RS_OverpostType type, bool exclude, LineBuffer* path = NULL);
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

protected:
    // list of layer w2d streams
    WT_File* m_w2dFile;
    DWFCore::DWFBufferOutputStream* m_w2dStream;
    stream_list m_lLayerStreams;

    // list of labeling w2d streams
    WT_File* m_w2dLabels;
    DWFCore::DWFBufferOutputStream* m_labelStream;
    stream_list m_lLabelStreams;

    // list of layout w2d streams;
    stream_list m_lLayoutStreams;

    // list of layout label w2d streams;
    stream_list m_lLayoutLabelStreams;

    // the active w2d to draw into
    WT_File* m_w2dActive;

    bool m_bHaveLabels;

    // used to track the number of drawables and macros
    int m_drawableCount;
    int m_labelMacroCount;
    std::map<int, int> m_macroDrawableCounts;

    ObservationMesh* m_obsMesh;

    // map/layer info
    RS_MapUIInfo* m_mapInfo;
    RS_LayerUIInfo* m_layerInfo;

    // support for selection -- must save id for every W2D object
    // which we will reference from DWF Object Instances
    // std::vector<DWFToolkit::DWFDefinedObject*> m_featureClasses;
    DWFToolkit::DWFDefinedObject* m_featureClass;
    DWFToolkit::DWFObjectDefinitionResource* m_attributes;
    objdefres_list m_lAttributeResources;
    RS_FeatureClassInfo* m_featureClassInfo;

    void ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost, RS_Bounds* bounds = NULL);

    DWFToolkit::DWFSection* m_pPage;

    void OpenW2D(WT_File* file);

    void Init(RS_Bounds& extents);

protected:
    // transformation from mapping to W2D space
    double _TX(double x);
    double _TY(double y);

    DWFCore::DWFString GetUIGraphicObjectIdFromLayerObjectId(const wchar_t* guid);

private:
    void _TransformPointsNoClamp(LineBuffer* plb);
    void _TransformContourPointsNoClamp(LineBuffer* plb, int cntr);
    void _TransformPoints(LineBuffer* plb, const SE_Matrix* xform);
    void _TransformContourPoints(LineBuffer* plb, int cntr, const SE_Matrix* xform);

public:
    void IncrementDrawableCount();

private:
    // macro/overpost opcode output -- done manually by writing to file
    void BeginMacro(WT_File* file, int id, int scale);
    void EndMacro(WT_File* file, int id);
    void PlayMacro(WT_File* file, int id, double sizeMeters, RS_Units unit, double x, double y);
    void BeginOverpostGroup(WT_File* file, RS_OverpostType type, bool render, bool exclude);
    void EndOverpostGroup(WT_File* file);

    size_t SplitLabel(wchar_t* label, std::vector<wchar_t*>& line_breaks);
    void ProcessMultilineText(WT_File* file, const RS_String& txt, RS_TextDef& tdef, int x, int y);

    //
    double _MeterToMapSize(RS_Units unit, double number);
    double _MeterToW2DMacroUnit(RS_Units unit, double number);

    // helpers to write simple geometry into W2D
    void WritePolylines(LineBuffer* srclb);
    void WriteStroke(RS_LineStroke & stroke);
    void WriteFill(RS_FillStyle& fill);

    void WriteTextDef(WT_File* file, RS_TextDef& tdef);

    void StoreAttributes(RS_FeatureReader* feature,
                         const RS_String*  tooltip,
                         const RS_String*  url);

    int ConvertToDashPattern(const wchar_t* lineStyleName,
                             double dpi,
                             double lineWeightPixels,
                             WT_Dash_Pattern& dash,
                             WT_Line_Pattern& lpat);

    // used by ProcessPolygon and DrawScreenPolygon to ensure that contours in
    // contour sets are oriented according to the DWF spec
    void OrientContours(int numContours, int* contourCounts, WT_Logical_Point* wtPointBuffer);

    unsigned int m_nObjectId;
    void* m_hObjNodes;

    int m_imgID;
    int m_layerNum;

    // buffer to hold WT points during conversion from line buffers
    // to W2D drawables
    WT_Logical_Point* m_wtPointBuffer;
    int m_wtPointLen;
    void EnsureBufferSize(int len);

    RS_SymbolManager* m_symbolManager;

    RS_Bounds m_extents;
    RS_Bounds m_mapExtents;
    double m_offsetX;
    double m_offsetY;
    double m_scale;
    double m_drawingScale;
    double m_dpi;
    double m_mapScale;
    double m_metersPerUnit;

    KeyEncode m_keyEncode;

    BIDIConverter m_bidiConverter;

    EMapHatchPatternFactory* m_hatchFac;
    EMapFillPatternFactory* m_fillFac;

    // used when rendering polyline / polygon features to track if a
    // line or dash pattern was last used
    bool m_linePatternActive;

    /////////////////////////////////////////////////////////
    // Functions and structures used during insertion of W2Ds
    /////////////////////////////////////////////////////////

    // TODO: these should be extracted to a friend class (or equivalent)

public:
    /*Do not export from DLL*/ bool IsViewportSet() { return m_bHaveViewport; }
    /*Do not export from DLL*/ bool IsSymbolW2D() { return m_bIsSymbolW2D; }
    /*Do not export from DLL*/ RS_MarkerDef& GetOverrideColorMarker() { return m_mdOverrideColors; }
    /*Do not export from DLL*/ RS_String& GetLayerFilter() { return m_layerFilter; }
    /*Do not export from DLL*/ bool& LayerPassesFilter() { return m_bLayerPassesFilter; }

    /*Do not export from DLL*/ WT_File* _GetW2D() { return m_w2dFile; }
    /*Do not export from DLL*/ RS_InputStream* _GetInputStream() { return m_input; }
    /*Do not export from DLL*/ const WT_Logical_Point* ProcessW2DPoints(WT_File&             file,
                                                                        WT_Logical_Point*    srcpts,
                                                                        int                  numpts,
                                                                        LineBuffer::GeomOperationType clipType,
                                                                        int&                 outNumpts,
                                                                        std::vector<int>**   outCntrs
                                                                        );

    /*Do not export from DLL*/ const WT_Logical_Point* TransformW2DPoints( WT_File&             file,
                                                                           WT_Logical_Point*    srcpts,
                                                                           int                  numpts
                                                                         );

    /*Do not export from DLL*/  long ScaleW2DNumber(WT_File& file,
                                                    long     number);

    /*Do not export from DLL*/  void UpdateSymbolTrans(WT_File& file, WT_Viewport& viewport);

private:
    void AddW2DContent(RS_InputStream* in, CSysTransformer* xformer, const RS_String& w2dfilter);

    void SetActions(WT_File& file);

    //////////////////////////////////////////////////////////
    // State variables for when DWFRenderer is used to keep
    // information during DWF rewriting process for symbols
    // and DWF layers
    //////////////////////////////////////////////////////////
    std::vector<int>    m_cntrs;
    RS_InputStream*     m_input;
    CSysTransformer*    m_xformer;
    bool                m_bIsSymbolW2D;
    bool                m_bHaveViewport;
    bool                m_bLayerPassesFilter;
    RS_String           m_layerFilter;

    // storage for override colors
    RS_MarkerDef m_mdOverrideColors;

    // remember id of last symbol
    RS_MarkerDef m_lastSymbol;

    //////////////////////////////////////////////////////////
    // End of DWF rewrite specific code
    //////////////////////////////////////////////////////////
};

#endif
