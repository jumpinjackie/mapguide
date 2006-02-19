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

#include "Stylizer.h"

class LineBufferPool;
class RasterAdapter;

//-----------------------------------------------------------------------------
// Stylizer used for all types of layers which do not have special
// Stylizer implementation, which is currently all of them.
//-----------------------------------------------------------------------------
class DefaultStylizer : public Stylizer
{
public:
    STYLIZATION_API DefaultStylizer();
    STYLIZATION_API virtual ~DefaultStylizer();

    STYLIZATION_API virtual void Initialize(Renderer* renderer);


    STYLIZATION_API virtual void StylizeFeatures( const MdfModel::VectorLayerDefinition* layer,
                                                      RS_FeatureReader* features,
                                                      CSysTransformer*  xformer, //can be NULL
                                                      CancelStylization cancel,
                                                      void*             userData
                                                      );

    STYLIZATION_API virtual void StylizeGridLayer(
                                     const MdfModel::GridLayerDefinition* layer,
                                     RS_FeatureReader*                    features,
                                     CSysTransformer*                     xformer,
                                     CancelStylization                    cancel,
                                     void*                                userData
                                     );


    STYLIZATION_API virtual void StylizeDrawingLayer(   const MdfModel::DrawingLayerDefinition* layer,
                                                        RS_LayerUIInfo*         legendInfo,
                                                        RS_InputStream*         dwfin,
                                                        CSysTransformer*        xformer //can be NULL
                                                    );

    STYLIZATION_API virtual void SetGeometryAdapter( GisGeometryType type,
                                                     GeometryAdapter* sg);

    STYLIZATION_API virtual void SetStylizeFeature(  FdoClassDefinition* classDef,
                                                     GeometryAdapter* sg);

private:

    GeometryAdapter* FindGeomAdapter(int geomType);
    void ClearAdapters();

    //geom stylizer cache
    std::map<int, GeometryAdapter*> m_hGeomStylizers;

    //raster stylizer
    RasterAdapter* m_pRasterAdapter;

    //TODO feature class stylizers

    Renderer* m_renderer;

    LineBufferPool* m_lbPool;
};
