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

#ifndef STYLIZER_H_
#define STYLIZER_H_

#include "Stylization.h"
#include "Bounds.h"
#include "RS_InputStream.h"
#include "RS_FeatureReader.h"

//forward declare
class Renderer;
class GeometryAdapter;
class CSysTransformer;

///<summary>
///Stylization interrupt callback -- return true to cancel stylization
///</summary>
typedef bool (*CancelStylization)(void* userData);

///<summary>
/// Base class for all stylizers. Stylizers take MDF layer and feature data
/// and determine the graphical styles to be used for display of the
/// feature data. They make the appropriate draw calls to a given Renderer
///</summary>
class Stylizer
{
public:
    ///<summary>
    /// Stylizes a feature/vector (FDO-based) layer.
    /// The supplied map scale is used only for stylization.
    ///</summary>
    virtual void StylizeVectorLayer(MdfModel::VectorLayerDefinition* layer,
                                    Renderer*                        renderer,
                                    RS_FeatureReader*                features,
                                    CSysTransformer*                 xformer,
                                    double                           mapScale,
                                    CancelStylization                cancel,
                                    void*                            userData) = 0;

    ///<summary>
    /// Stylizes a grid/raster layer.
    /// The supplied map scale is used only for stylization.
    ///</summary>
    virtual void StylizeGridLayer(MdfModel::GridLayerDefinition* layer,
                                  Renderer*                      renderer,
                                  RS_FeatureReader*              features,
                                  CSysTransformer*               xformer,
                                  double                         mapScale,
                                  CancelStylization              cancel,
                                  void*                          userData) = 0;

    ///<summary>
    /// Stylizes a drawing (DWF-based) layer.
    /// The supplied map scale is used only for stylization.
    ///</summary>
    virtual void StylizeDrawingLayer(MdfModel::DrawingLayerDefinition* layer,
                                     Renderer*                         renderer,
                                     RS_InputStream*                   dwfin,
                                     CSysTransformer*                  xformer,
                                     double                            mapScale) = 0;

    ///<summary>
    /// Stylizes a watermark.
    ///</summary>
    virtual void StylizeWatermark(Renderer* renderer,
                                  MdfModel::WatermarkDefinition* watermark,
                                  int drawWidth,
                                  int drawHeight,
                                  int saveWidth,
                                  int saveHeight) = 0;

    ///<summary>
    /// Allows a user to set a custom stylization object for a given geometry type.
    ///</summary>
    virtual void SetGeometryAdapter(FdoGeometryType type, GeometryAdapter* stylizer) = 0;

    STYLIZATION_API static MdfModel::VectorScaleRange* FindScaleRange(MdfModel::VectorScaleRangeCollection& src, double mapScale);
    STYLIZATION_API static MdfModel::GridScaleRange* FindScaleRange(MdfModel::GridScaleRangeCollection& src, double mapScale);
};

#endif
