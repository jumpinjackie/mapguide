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

#ifndef STYLIZER_H_
#define STYLIZER_H_

#include "Bounds.h"
#include "RS_InputStream.h"
#include "RS_FeatureReader.h"

//forward declare
class Renderer;
class GeometryAdapter;
class CSysTransformer;
class SE_SymbolManager;

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
    ///</summary>
    virtual void StylizeVectorLayer(const MdfModel::VectorLayerDefinition* layer,
                                          Renderer*                        renderer,
                                          RS_FeatureReader*                features,
                                          CSysTransformer*                 xformer,
                                          CancelStylization                cancel,
                                          void*                            userData) = 0;

    ///<summary>
    /// Stylizes a grid/raster layer.
    ///</summary>
    virtual void StylizeGridLayer(const MdfModel::GridLayerDefinition* layer,
                                        Renderer*                      renderer,
                                        RS_FeatureReader*              features,
                                        CSysTransformer*               xformer,
                                        CancelStylization              cancel,
                                        void*                          userData) = 0;

    ///<summary>
    /// Stylizes a drawing (DWF-based) layer.
    ///</summary>
    virtual void StylizeDrawingLayer(const MdfModel::DrawingLayerDefinition* layer,
                                           Renderer*                         renderer,
                                           RS_LayerUIInfo*                   legendInfo,
                                           RS_InputStream*                   dwfin,
                                           const RS_String&                  layerFilter,
                                           CSysTransformer*                  xformer) = 0;

    ///<summary>
    /// Allows a user to set a custom stylization object for certain geometry types.
    ///</summary>
    virtual void SetGeometryAdapter(FdoGeometryType type, GeometryAdapter* stylizer) = 0;

    ///<summary>
    /// Allows a user to set a custom stylization object for a given feature class.
    ///</summary>
    virtual void SetStylizeFeature(FdoClassDefinition* classDef, GeometryAdapter* stylizer) = 0;

    STYLIZATION_API static MdfModel::VectorScaleRange* FindScaleRange(MdfModel::VectorScaleRangeCollection& src,
                                         double mapScale);

    STYLIZATION_API static MdfModel::GridScaleRange* FindScaleRange(MdfModel::GridScaleRangeCollection& src,
                                         double mapScale);
};

#endif
