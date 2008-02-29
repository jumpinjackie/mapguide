//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef DEFAULTSTYLIZER_H_
#define DEFAULTSTYLIZER_H_

#include "Stylizer.h"

class LineBufferPool;
class RasterAdapter;
class StylizationEngine;
class SE_SymbolManager;

//-----------------------------------------------------------------------------
// Stylizer used for all types of layers which do not have special
// Stylizer implementation, which is currently all of them.
//-----------------------------------------------------------------------------
class DefaultStylizer : public Stylizer
{
public:
    STYLIZATION_API DefaultStylizer(SE_SymbolManager* sman);
    STYLIZATION_API virtual ~DefaultStylizer();

    STYLIZATION_API virtual void StylizeVectorLayer(MdfModel::VectorLayerDefinition* layer,
                                                    Renderer*                        renderer,
                                                    RS_FeatureReader*                features,
                                                    CSysTransformer*                 xformer,
                                                    double                           mapScale,
                                                    CancelStylization                cancel,
                                                    void*                            userData);

    STYLIZATION_API virtual void StylizeGridLayer(MdfModel::GridLayerDefinition* layer,
                                                  Renderer*                      renderer,
                                                  RS_FeatureReader*              features,
                                                  CSysTransformer*               xformer,
                                                  double                         mapScale,
                                                  CancelStylization              cancel,
                                                  void*                          userData);

    STYLIZATION_API virtual void StylizeDrawingLayer(MdfModel::DrawingLayerDefinition* layer,
                                                     Renderer*                         renderer,
                                                     RS_InputStream*                   dwfin,
                                                     CSysTransformer*                  xformer,
                                                     double                            mapScale);

    STYLIZATION_API virtual void SetGeometryAdapter(FdoGeometryType type, GeometryAdapter* stylizer);

private:
    int StylizeVLHelper(MdfModel::VectorLayerDefinition* layer,
                        MdfModel::VectorScaleRange*      scaleRange,
                        Renderer*                        renderer,
                        RS_FeatureReader*                features,
                        bool                             initialPass,
                        FdoExpressionEngine*             exec,
                        CSysTransformer*                 xformer,
                        CancelStylization                cancel,
                        void*                            userData);
    GeometryAdapter* FindGeomAdapter(int geomType);
    void ClearAdapters();

    //geom stylizer cache
    std::map<int, GeometryAdapter*> m_hGeomStylizers;

    //raster stylizer
    RasterAdapter* m_pRasterAdapter;

    //composite stylizer
    StylizationEngine* m_styleEngine;

    LineBufferPool* m_lbPool;
};

#endif
