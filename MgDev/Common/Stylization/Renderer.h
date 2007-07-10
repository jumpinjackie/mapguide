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

#ifndef _RENDERER_H
#define _RENDERER_H

#include "StylizationAPI.h"
#include "Bounds.h"
#include "RendererStyles.h"
#include "RS_InputStream.h"
#include "SymbolManager.h"
#include "RS_FeatureReader.h"

class LineBuffer;
class CSysTransformer;

///<summary>
/// Base class for all Renderers. Renderers take stylization output and
/// produce low level graphics, e.g. W2D, W3D, draw to GDI, OpenGL, etc.
/// This class is abstract and cannot be directly instantiated.
///</summary>
class Renderer
{
public:
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
    virtual void StartLayer(RS_LayerUIInfo*      legendInfo,
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
                              const RS_String*  tooltip = NULL,
                              const RS_String*  url = NULL,
                              const RS_String*  theme = NULL,
                              double zOffset = 0,
                              double zExtrusion = 0,
                              RS_ElevationType zOffsetType = RS_ElevationType_RelativeToGround) = 0;

    ///<summary>
    /// Done processing current feature.
    ///</summary>
    //TODO: is it needed -- next call to StartFeature can just imply
    //previous feature is done
    //virtual void EndFeature() = 0;

    ///<summary>
    /// Polygon features. Transformation to W2D space is done here. Also handles
    /// holes etc.
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
                               RS_Bounds        extents) = 0;

    ///<summary>
    /// Feature Marker Symbols -- added to feature W2D and should also
    /// support selection
    ///</summary>
    virtual void ProcessMarker(LineBuffer*      lb,
                               RS_MarkerDef&    mdef,
                               bool             allowOverpost,
                               RS_Bounds*       bounds = NULL) = 0;

    //TODO: DEPRECATED! Use ProcessLabelGroup() instead
    ///<summary>
    /// Text labels
    ///</summary>
    virtual void ProcessLabel(double            x,
                              double            y,
                              const RS_String&  text,
                              RS_TextDef&       tdef) = 0;

    ///<summary>
    /// Text label group --
    ///</summary>
    virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path) = 0;

    ///<summary>
    ///Inserts the contents of a given DWF input stream
    ///into the current output W2D. The given coord sys
    ///transformation is applied and geometry will be clipped
    ///to the Bounds context of the DWFRenderer
    ///</summary>
    virtual void AddDWFContent(RS_InputStream*  in,
                               CSysTransformer* xformer,
                               const RS_String& section,
                               const RS_String& passwd,
                               const RS_String& w2dfilter) = 0;

    ///<summary>
    ///Provides an instance of an RS_Symbol manager which will be used
    ///for retrieving graphics for point/marker symbol data.
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

    virtual double GetMetersPerUnit() = 0;

    virtual RS_Bounds& GetBounds() = 0;

    virtual double GetDpi() = 0;

    //------------------------------------------------------
    // Geometry clipping renderer option
    //------------------------------------------------------

    virtual bool RequiresClipping() = 0;
};

#endif
