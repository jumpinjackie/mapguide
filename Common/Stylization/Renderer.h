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

#ifndef RENDERER_H_
#define RENDERER_H_

#include "StylizationAPI.h"
#include "Bounds.h"
#include "RendererStyles.h"
#include "RS_InputStream.h"
#include "RS_SymbolManager.h"


class LineBuffer;
class CSysTransformer;
class TransformMesh;
class SE_BufferPool;
class RS_FeatureReader;

///<summary>
/// Base class for all Renderers. Renderers take stylization output and
/// produce low level graphics, e.g. W2D, W3D, draw to GDI, OpenGL, etc.
/// This class is abstract and cannot be directly instantiated.
///</summary>
class Renderer
{
public:
    virtual ~Renderer()
    {
    }

    ///<summary>
    /// Initializes the Renderer with mapping context information
    /// for generating a view of a map
    ///</summary>
    virtual void StartMap(RS_MapUIInfo*    mapInfo,
                          RS_Bounds&       extents,
                          double           mapScale,
                          double           dpi,
                          double           metersPerUnit,
                          CSysTransformer* xformToLL) = 0;

    ///<summary>
    /// Finalizes map generation. Typically the output for a map
    /// generation (DWF, JPEG, ...) will be ready after calling this function.
    ///</summary>
    virtual void EndMap() = 0;

    ///<summary>
    /// Indicates the renderer should process all subsequent geometry
    /// in a new graphical layer.
    ///</summary>
    virtual void StartLayer(RS_LayerUIInfo*      layerInfo,
                            RS_FeatureClassInfo* classInfo) = 0;

    ///<summary>
    /// Indicates all features for a layer are processed. At this point
    /// on overloading class can do post processing like creating
    /// EPlot resources from a layer's W2D file
    ///</summary>
    virtual void EndLayer() = 0;

    ///<summary>
    /// Indicates the renderer should process all subsequent geometry
    /// as part of a single logical feature.
    ///</summary>
    virtual void StartFeature(RS_FeatureReader* feature,
                              bool              initialPass,
                              const RS_String*  tooltip = NULL,
                              const RS_String*  url = NULL,
                              const RS_String*  theme = NULL,
                              double            zOffset = 0.0,
                              double            zExtrusion = 0.0,
                              RS_ElevationType  zOffsetType = RS_ElevationType_RelativeToGround) = 0;

    ///<summary>
    /// Done processing current feature.
    ///</summary>
    //TODO: is it needed -- next call to StartFeature can just imply
    //previous feature is done
    //virtual void EndFeature() = 0;

    ///<summary>
    /// Polygon features. Transformation to W2D space is done here. Also
    /// handles holes etc.
    ///</summary>
    virtual void ProcessPolygon(LineBuffer*     lb,
                                RS_FillStyle&   fill) = 0;

    ///<summary>
    /// Polyline features. Transformation to W2D space is done here.
    ///</summary>
    virtual void ProcessPolyline(LineBuffer*    lb,
                                 RS_LineStroke& lsym) = 0;

    ///<summary>
    /// Raster serialization.
    ///</summary>
    virtual void ProcessRaster(unsigned char*   data,
                               int              length,
                               RS_ImageFormat   format,
                               int              width,
                               int              height,
                               RS_Bounds&       extents,
                               TransformMesh*   xformMesh = NULL) = 0;

    ///<summary>
    /// Feature Marker Symbols -- added to feature W2D and should also
    /// support selection.  Bounds should be an array of length 4,
    /// representing the bounds of the marker that is drawn, before
    /// rotation.  The bounds must be in meters in the same units as
    /// the supplied RS_MarkerDef.
    ///</summary>
    virtual void ProcessMarker(LineBuffer*      lb,
                               RS_MarkerDef&    mdef,
                               bool             allowOverpost,
                               RS_Bounds*       bounds = NULL) = 0;

    ///<summary>
    /// Text label group --
    ///</summary>
    virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path,
                                   double           scaleLimit) = 0;

    ///<summary>
    /// Inserts the contents of a given DWF input stream
    /// into the current output W2D. The given coord sys
    /// transformation is applied and geometry will be clipped
    /// to the Bounds context of the DWFRenderer
    ///</summary>
    virtual void AddDWFContent(RS_InputStream*  in,
                               CSysTransformer* xformer,
                               const RS_String& section,
                               const RS_String& passwd,
                               const RS_String& w2dfilter) = 0;

    ///<summary>
    /// Provides an instance of an RS_Symbol manager which will be used
    /// for retrieving graphics for point/marker symbol data.
    ///</summary>
    virtual void SetSymbolManager(RS_SymbolManager* manager) = 0;

    //------------------------------------------------------
    // Standard context accessors. Cleaner than passing
    // them in to adapters each time, they can just ask
    // for it from the renderer
    //------------------------------------------------------

    virtual RS_MapUIInfo* GetMapInfo() = 0;

    virtual RS_LayerUIInfo* GetLayerInfo() = 0;

    virtual RS_FeatureClassInfo* GetFeatureClassInfo() = 0;

    virtual double GetMapScale() = 0;

    // returns the number of mapping units per pixel
    virtual double GetDrawingScale() = 0;

    // returns the number of meters per mapping unit
    virtual double GetMetersPerUnit() = 0;

    virtual double GetDpi() = 0;

    // returns the bounds of the map in mapping units
    virtual RS_Bounds& GetBounds() = 0;

    //------------------------------------------------------
    // Geometry renderer settings
    //------------------------------------------------------

    // Flags whether the geometry used to render the feature is clipped.
    virtual bool RequiresClipping() = 0;

    // Flags whether the geometry used to label the feature is clipped.
    // This only applies if geometry clipping is set to false.  By setting
    // label clipping to true, you can be assured of generating on-screen
    // labels even though the feature geometry is not clipped to the screen.
    virtual bool RequiresLabelClipping() = 0;

    // Flags whether the renderer has rendering support for geometry
    // containing Z values.
    virtual bool SupportsZ() = 0;

    //------------------------------------------------------
    // Miscellaneous
    //------------------------------------------------------

    // gets/sets the size of the transformation mesh grid used for raster
    // re-projection
    virtual int GetRasterGridSize() = 0;
    virtual void SetRasterGridSize(int size) = 0;

    // gets/sets the minimum size of the transformation mesh grid used for
    // raster re-projection
    virtual int GetMinRasterGridSize() = 0;
    virtual void SetMinRasterGridSize(int size) = 0;

    // gets/sets the transformation mesh grid override ratio used in
    // raster re-projection
    virtual double GetRasterGridSizeOverrideRatio() = 0;
    virtual void SetRasterGridSizeOverrideRatio(double ratio) = 0;

    // gets/sets the max width for raster image
    virtual double GetMaxRasterImageWidth() = 0;
    virtual void SetMaxRasterImageWidth(int width) = 0;

    // gets/sets the max height for raster image
    virtual double GetMaxRasterImageHeight() = 0;
    virtual void SetMaxRasterImageHeight(int height) = 0;

    // flags whether the renderer processes tooltip expressions
    virtual bool SupportsTooltips() = 0;

    // flags whether if the renderer processes hyperlink expressions
    virtual bool SupportsHyperlinks() = 0;

    // flags whether if the renderer separate composite line styles
    virtual bool RequiresCompositeLineStyleSeparation() = 0;

    // line buffer pool access
    virtual SE_BufferPool* GetBufferPool() = 0;
    virtual void SetBufferPool(SE_BufferPool* pool) = 0;
};

#endif
