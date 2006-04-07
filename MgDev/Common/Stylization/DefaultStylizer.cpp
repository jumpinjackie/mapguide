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

#include "stdafx.h"
#include "DefaultStylizer.h"
#include "GeometryAdapter.h"
#include "PolylineAdapter.h"
#include "PolygonAdapter.h"
#include "PointAdapter.h"
#include "RasterAdapter.h"
#include "FilterExecutor.h"
#include "Renderer.h"
#include "LineBuffer.h"
#include "FeatureTypeStyleVisitor.h"


DefaultStylizer::DefaultStylizer()
{
    m_lbPool = new LineBufferPool;
    m_pRasterAdapter = NULL;
    m_renderer = NULL;
}

DefaultStylizer::~DefaultStylizer()
{
    //free geom adapters -- not strictly needed here
    //but just in case something crashed in the middle
    //of stylization
    ClearAdapters();

    delete m_lbPool;
}


//-----------------------------------------------------------------------------
// Initializes the Stylizer with a given graphics renderer
//
// Input: A pointer to a Renderer implementation instance
//-----------------------------------------------------------------------------
void DefaultStylizer::Initialize(Renderer* renderer)
{
    m_renderer = renderer;
}


///<summary>
/// Stylizes given features with a specified layer and map scale.
///<summary>
void DefaultStylizer::StylizeFeatures(const MdfModel::VectorLayerDefinition*  layer,
                                                RS_FeatureReader*     features,
                                                CSysTransformer*      xformer, //can be NULL
                                                CancelStylization     cancel,
                                                void*                 userData
                                                )
{
    //gets rid of const in pointer -- some functions we call aren't const
    MdfModel::VectorLayerDefinition* fl = (MdfModel::VectorLayerDefinition*)layer;

    if (!fl) return; //should never get here in the first place!

    // look through the scale ranges to find a valid one
    // the first one that contains the given scale will be used
    MdfModel::VectorScaleRangeCollection* ranges = fl->GetScaleRanges();

    MdfModel::VectorScaleRange* range = Stylizer::FindScaleRange(*ranges, m_renderer->GetMapScale());

    // no range -- do not stylize
    if (NULL == range) return;

    // we have a valid scale range... we can now go over the features and
    // apply the feature styles in that range

    //Sidenote: as of now (8/10/04) it is not clear whether FeatureTypeStyle
    //is matched to features via geometry type or via name of the
    //feature class. For now, we match by geometry.

    const wchar_t* gpName = features->GetGeomPropName();

    //no geometry -- do not stylize
    if (NULL == gpName) return;

    RS_FilterExecutor* exec = RS_FilterExecutor::Create(features);

    // find the FeatureTypeStyle
    MdfModel::FeatureTypeStyleCollection* ftsc = range->GetFeatureTypeStyles();

    //extract hyperlink and tooltip info
    //this is invariant, so do outside of feature iterator loop
    const MdfModel::MdfString* lr_tooltip = &fl->GetToolTip();
    const MdfModel::MdfString* lr_url = &fl->GetUrl();

    if (lr_tooltip->empty()) lr_tooltip = NULL;
    if (lr_url->empty()) lr_url = NULL;

    //TODO:
    //*****************************************
    //* THIS CALL IS REALLY REALLY REALLY SLOW!!!
    //*****************************************
    //It needs to be done per feature if there is inheritance of feature classes
    //but is so horribly slow that in all other cases it needs to be optimized away
    //GisPtr<FdoClassDefinition> concreteClass = features->GetClassDefinition();

    FeatureTypeStyleVisitor visitor;

    bool bClip = m_renderer->RequiresClipping();

    //main loop over feature data
    while (features->ReadNext())
    {
        LineBuffer* lb = NULL;

        //get the geometry just once
        //all types of geometry
        lb = m_lbPool->NewLineBuffer(8);
        features->GetGeometry(gpName, lb, xformer);

        if (lb && bClip)
        {
            //clip geometry to given map request extents
            //TODO: is this the right place to do so?
            LineBuffer* lbc = lb->Clip(m_renderer->GetBounds(), LineBuffer::ctAGF, m_lbPool);

            //did geom require clipping?
            //free original line buffer
            //note original geometry is still accessible to the
            //user from the RS_FeatureReader::GetGeometry
            if (lbc != lb)
            {
                m_lbPool->FreeLineBuffer(lb);
                lb = lbc;
            }
        }

        if (!lb) continue;

        //need to clear out the filter execution engine cache
        //some feature attributes may be cached while executing theming
        //expressions and this call flushes that
        exec->Reset();

        // we need to stylize once for each FeatureTypeStyle that matches
        // the geometry type (Note: this may have to change to match
        // feature classes)
        for (int i=0; i<ftsc->GetCount(); i++)
        {
            MdfModel::FeatureTypeStyle* fts = ftsc->GetAt(i);

            //TODO: future enhancement:
            //If we have a stylizer that works on a specific feature class
            //we should invoke it here, instead of invoking the
            //generic stylizer for the particular type of geomtry
            GeometryAdapter* adapter = FindGeomAdapter(lb->geom_type());

            //if we know how to stylize this type of geometry, then go ahead
            if (adapter)
                adapter->Stylize(m_renderer, features, exec, lb, fts, lr_tooltip, lr_url);
        }

        if (lb)
            m_lbPool->FreeLineBuffer(lb); // free geometry when done stylizing

        if (cancel && cancel(userData)) break;
    }

    //need the cast due to multiple inheritance resulting in two Disposables
    //in the vtable of FilterExecutor
    ((FdoIExpressionProcessor*)exec)->Release();

    //need to get rid of these since they cache per layer theming information
    //which may conflict with the next layer
    ClearAdapters();
}


void DefaultStylizer::StylizeGridLayer(   const MdfModel::GridLayerDefinition* layer,
                         RS_FeatureReader*                    features,
                         CSysTransformer*                     /*xformer*/,
                         CancelStylization                    cancel,
                         void*                                userData
                         )
{
    //gets rid of const in pointer -- some functions we call aren't const
    MdfModel::GridLayerDefinition* gl = (MdfModel::GridLayerDefinition*)layer;

    if (!gl) return; //should never get here in the first place!

    // look through the scale ranges to find a valid one
    // the first one that contains the given scale will be used
    MdfModel::GridScaleRangeCollection* ranges = gl->GetScaleRanges();

    MdfModel::GridScaleRange* range = Stylizer::FindScaleRange(*ranges, m_renderer->GetMapScale());

    // no range -- do not stylize
    if (NULL == range) return;

    // we have a valid scale range... we can now go over the features and
    // apply the feature styles in that range

    const wchar_t* rpName = features->GetRasterPropName();

    //no geometry -- do not stylize
    if (NULL == rpName) return;

    RS_FilterExecutor* exec = RS_FilterExecutor::Create(features);

    // find the FeatureTypeStyle
    MdfModel::GridColorStyle* gcs = range->GetColorStyle();

    //init the raster adapter
    if (!m_pRasterAdapter)
        m_pRasterAdapter = new RasterAdapter(m_lbPool);

    //TODO:
    //*****************************************
    //* THIS CALL IS REALLY REALLY REALLY SLOW!!!
    //*****************************************
    //It needs to be done per feature if there is inheritance of feature classes
    //but is so horribly slow that in all other cases it needs to be optimized away
    //GisPtr<FdoClassDefinition> concreteClass = features->GetClassDefinition();

    //main loop over raster data
    while (features->ReadNext())
    {
        RS_Raster* raster = features->GetRaster(rpName);

        //need to clear out the filter execution engine cache
        //some feature attributes may be cached while executing theming
        //expressions and this call flushes that
        exec->Reset();

        if (m_pRasterAdapter)
            m_pRasterAdapter->Stylize(m_renderer, features, exec, raster, gcs, NULL, NULL);

        if (raster)
            delete raster; //need to free returned raster

        if (cancel && cancel(userData)) break;
    }

    //need the cast due to multiple inheritance resulting in two Disposables
    //in the vtable of FilterExecutor
    ((FdoIExpressionProcessor*)exec)->Release();

    //need to get rid of these since they cache per layer theming information
    //which may conflict with the next layer
    ClearAdapters();
}



void DefaultStylizer::StylizeDrawingLayer(  const MdfModel::DrawingLayerDefinition* layer,
                                            RS_LayerUIInfo*     legendInfo,
                                            RS_InputStream*         dwfin,
                                            const RS_String&        layerFilter
                                          )
{
    double mapScale = m_renderer->GetMapScale();

    //check if we are in scale range
    if (mapScale >= layer->GetMinScale() && mapScale < layer->GetMaxScale())
    {
        m_renderer->StartLayer(legendInfo, NULL);

        //TODO: dwf password
        m_renderer->AddDWFContent(dwfin, NULL, L"", L"", layerFilter);

        m_renderer->EndLayer();
    }
}


//WARNING: given pointer to the new stylizer will be destroyed
//by the stylizer (in its destructor)
void DefaultStylizer::SetGeometryAdapter( GisGeometryType type, GeometryAdapter* sg)
{
    GeometryAdapter* old = (GeometryAdapter*)m_hGeomStylizers[type];

    if (old) delete old;

    m_hGeomStylizers[type] = sg;
}

void DefaultStylizer::SetStylizeFeature( FdoClassDefinition* /*classDef*/, GeometryAdapter* /*sg*/)
{
    //TODO
    //not impl.
}


GeometryAdapter* DefaultStylizer::FindGeomAdapter(int geomType)
{
    GeometryAdapter* adapter = m_hGeomStylizers[geomType];

    //have adapter -- return it
    if (adapter)
        return adapter;

    //otherwise need to create one based on the geometry type
    switch (geomType)
    {
    case GisGeometryType_LineString:
        m_hGeomStylizers[GisGeometryType_LineString] = new PolylineAdapter(m_lbPool);
        break;
    case GisGeometryType_MultiLineString:
        m_hGeomStylizers[GisGeometryType_MultiLineString] = new PolylineAdapter(m_lbPool);
        break;
    case GisGeometryType_CurveString:
        m_hGeomStylizers[GisGeometryType_CurveString] = new PolylineAdapter(m_lbPool);
        break;
    case GisGeometryType_MultiCurveString:
        m_hGeomStylizers[GisGeometryType_MultiCurveString] = new PolylineAdapter(m_lbPool);
        break;
    case GisGeometryType_Polygon:
        m_hGeomStylizers[GisGeometryType_Polygon] = new PolygonAdapter(m_lbPool);
        break;
    case GisGeometryType_MultiPolygon:
        m_hGeomStylizers[GisGeometryType_MultiPolygon] = new PolygonAdapter(m_lbPool);
        break;
    case GisGeometryType_CurvePolygon:
        m_hGeomStylizers[GisGeometryType_CurvePolygon] = new PolygonAdapter(m_lbPool);
        break;
    case GisGeometryType_MultiCurvePolygon:
        m_hGeomStylizers[GisGeometryType_MultiCurvePolygon] = new PolygonAdapter(m_lbPool);
        break;
    case GisGeometryType_Point:
        m_hGeomStylizers[GisGeometryType_Point] = new PointAdapter(m_lbPool);
        break;
    case GisGeometryType_MultiPoint:
        m_hGeomStylizers[GisGeometryType_MultiPoint] = new PointAdapter(m_lbPool);
        break;
    default :
        break;
    }

    return m_hGeomStylizers[geomType];
}

void DefaultStylizer::ClearAdapters()
{
    std::map<int, GeometryAdapter*>::iterator sgiter = m_hGeomStylizers.begin();

    //free the stylizer objects
    for (; sgiter!=m_hGeomStylizers.end(); sgiter++)
    {
        delete sgiter->second;
    }

    m_hGeomStylizers.clear();

    if (m_pRasterAdapter)
    {
        delete m_pRasterAdapter;
        m_pRasterAdapter = NULL;
    }
}
