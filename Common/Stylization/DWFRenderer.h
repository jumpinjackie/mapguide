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

#ifndef DWFRENDERER_H
#define DWFRENDERER_H

#include <vector>

#ifdef _WIN32
#pragma warning(disable : 4290)
#endif

#include "Renderer.h"
#include "LineBuffer.h"
#include "KeyEncode.h"

//forward declare
class WT_File;
class WT_Viewport;
class WT_Logical_Point;

class EMapHatchPatternFactory;
class EMapFillPatternFactory;

class ObservationMesh;


namespace DWFCore
{
class DWFBufferOutputStream;
class DWFBufferInputStream;
}

namespace DWFToolkit
{
class DWFDefinedObject;
class DWFDefinedObjectInstance;
class DWFObjectDefinitionResource;
class DWFSection;
};

//holds W2D streams, and object data streams
typedef std::vector<DWFCore::DWFBufferOutputStream*> stream_list;
typedef std::vector<std::wstring> wstring_list;
typedef std::vector<DWFToolkit::DWFObjectDefinitionResource*> objdefres_list;

///<summary>
/// Implementation of Renderer for DWF-W2D output
///</summary>
class DWFRenderer : public Renderer
{

public:
    STYLIZATION_API DWFRenderer();
    STYLIZATION_API virtual ~DWFRenderer();

    STYLIZATION_API virtual void StartMap      (RS_MapUIInfo* mapInfo,
                                                RS_Bounds&    extents,
                                                double        mapScale,
                                                double        dpi,
                                                double        metersPerUnit,
                                                CSysTransformer* xformToLL);

    STYLIZATION_API virtual void EndMap         ();

    STYLIZATION_API virtual void StartLayer     (RS_LayerUIInfo*      legendInfo,
                                                 RS_FeatureClassInfo* classInfo);

    STYLIZATION_API virtual void EndLayer       ();


    STYLIZATION_API virtual void StartFeature (RS_FeatureReader* feature,
                                               const RS_String*  tooltip = NULL,
                                               const RS_String*  url = NULL);

    STYLIZATION_API virtual void ProcessPolygon (LineBuffer* lb,
                                                 RS_FillStyle& fill);

    STYLIZATION_API virtual void ProcessPolyline(LineBuffer* lb,
                                                 RS_LineStroke& lsym);

    STYLIZATION_API virtual void ProcessRaster  (unsigned char*    data,
                                                 int               length,
                                                 RS_ImageFormat    format,
                                                 int               width,
                                                 int               height,
                                                 RS_Bounds         extents);

    STYLIZATION_API virtual void ProcessMarker(LineBuffer*       lb,
                                               RS_MarkerDef&     mdef,
                                               bool              allowOverpost);

    STYLIZATION_API virtual void ProcessLabel(double x, double y,
                                              const RS_String& text, RS_TextDef& tdef);

    STYLIZATION_API virtual void ProcessLabelGroup( RS_LabelInfo*       labels,
                                                    int                 nalabels,
                                                    const RS_String&    text,
                                                    RS_OverpostType     type,
                                                    bool                exclude,
                                                    LineBuffer*         path);

    STYLIZATION_API virtual void AddDWFContent(RS_InputStream*  in,
                                               CSysTransformer* xformer,
                                               const RS_String&    section,
                                               const RS_String&    passwd);

    STYLIZATION_API virtual void SetSymbolManager(RS_SymbolManager* manager);

    STYLIZATION_API virtual double GetMapScale();

    STYLIZATION_API virtual RS_Bounds& GetBounds();

    STYLIZATION_API virtual double GetDpi();

    STYLIZATION_API virtual double GetMetersPerUnit();

    STYLIZATION_API virtual double GetMapToW2DScale();

    STYLIZATION_API virtual bool RequiresClipping();

    STYLIZATION_API virtual bool UseLocalOverposting();

    STYLIZATION_API void StartLayout(RS_Bounds& extents);

    STYLIZATION_API void EndLayout();

protected:

    //list of layer w2d streams
    WT_File* m_w2dFile;
    DWFCore::DWFBufferOutputStream* m_w2dStream;
    stream_list m_lLayerStreams;

    //list of labeling w2d streams
    WT_File* m_w2dLabels;
    DWFCore::DWFBufferOutputStream* m_labelStream;
    stream_list m_lLabelStreams;

    // list of layout w2d streams;
    stream_list m_lLayoutStreams;

    // list of layout label w2d streams;
    stream_list m_lLayoutLabelStreams;

    bool m_bHaveLabels;

    ObservationMesh* m_obsMesh;

    //support for selection -- must save id for every W2D object
    //which we will refrence from DWF Object Instances
    //std::vector<DWFToolkit::DWFDefinedObject*> m_featureClasses;
    DWFToolkit::DWFDefinedObject* m_featureClass;
    DWFToolkit::DWFObjectDefinitionResource* m_attributes;
    objdefres_list m_lAttributeResources;
    RS_FeatureClassInfo* m_featureClassInfo;

    void ProcessOneMarker(double x, double y, RS_MarkerDef& mdef, bool allowOverpost);

    DWFToolkit::DWFSection* m_pPage;

    void OpenW2D(WT_File* file);

    void Init(RS_Bounds& extents);

protected:

    //transformation from mapping to W2D space
    inline double _TX(double x);
    inline double _TY(double y);
    void _TransformPointsNoClamp(double* inpts, int numpts);

private:

    //macro/overpost opcode output -- done manually by writing to file
    void BeginMacro(WT_File* file, int id, int scale);
    void EndMacro(WT_File* file);
    void PlayMacro(WT_File* file, int id, double sizeMeters, RS_Units unit, double x, double y);
    void BeginOverpostGroup(WT_File* file, RS_OverpostType type, bool render, bool exclude);
    void EndOverpostGroup(WT_File* file);

    void ProcessMultilineText(WT_File* file, const RS_String& txt, RS_TextDef& tdef, int x, int y);

    //
    double _MeterToMapSize(RS_Units unit, double number);
    double _PixelToMapSize(Renderer* renderer, int pixels);
    double _MeterToW2DMacroUnit(RS_Units unit, double number);

    //helpers to write simple geometry into W2D
    void WritePolylines(LineBuffer* srclb);
    void WriteStroke(RS_LineStroke & stroke);
    void WriteFill(RS_FillStyle& fill);

    void WriteTextDef(WT_File* file, RS_TextDef& tdef);

    void StoreAttributes(RS_FeatureReader* feature,
                         const RS_String*  tooltip,
                         const RS_String*  url);

    unsigned int m_nObjectId;
    void* m_hObjNodes;

    int m_imgID;
    int m_layerNum;

    //buffer to hold WT points during conversion from line buffers
    //to W2D drawables
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

    LineBufferPool m_lbPool;

    KeyEncode m_keyEncode;

    EMapHatchPatternFactory* m_hatchFac;
    EMapFillPatternFactory* m_fillFac;

    /////////////////////////////////////////////////////////
    //
    // Functions and structures used during insertion of W2Ds
    //
    /////////////////////////////////////////////////////////

    //TODO: these should be extracted to a friend class (or equivalent)

public:
    /*Do not export from DLL*/ bool IsViewportSet() { return m_bHaveViewport; }
    /*Do not export from DLL*/ bool IsSymbolW2D() { return m_bIsSymbolW2D; }
    /*Do not export from DLL*/ RS_MarkerDef& GetOverrideColorMarker() { return m_mdOverrideColors; }

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
    void AddW2DContent(RS_InputStream* in, CSysTransformer* xformer);
    void FindSymbolReferencePoint(RS_InputStream*     in,
                                  const RS_String&    section,
                                  const RS_String&    passwd,
                                  double&             x,
                                  double&             y);

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

    //storage for override colors
    RS_MarkerDef m_mdOverrideColors;

    //remember id of last symbol
    RS_MarkerDef m_lastSymbol;

    //////////////////////////////////////////////////////////
    // End of DWF rewrite specific code
    //////////////////////////////////////////////////////////
};

#endif
