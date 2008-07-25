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
#include "ElevationSettings.h"
#include "FeatureTypeStyleVisitor.h"
#include "StylizationEngine.h"

const RS_String s_Empty(L"");

extern void ProcessStylizerException(FdoException* exception, int line, wchar_t* file);

DefaultStylizer::DefaultStylizer(SE_SymbolManager* sman)
{
    m_pRasterAdapter = NULL;
    m_styleEngine = new StylizationEngine(sman);
    m_lbPool = new LineBufferPool;
}

DefaultStylizer::~DefaultStylizer()
{
    //free geom adapters -- not strictly needed here
    //but just in case something crashed in the middle
    //of stylization
    ClearAdapters();

    delete m_styleEngine;
    delete m_lbPool;
}


///<summary>
/// Stylizes given features with a specified layer and map scale.
///<summary>
void DefaultStylizer::StylizeVectorLayer(const MdfModel::VectorLayerDefinition* layer,
                                               Renderer*                        renderer,
                                               RS_FeatureReader*                features,
                                               CSysTransformer*                 xformer, //can be NULL
                                               CancelStylization                cancel,
                                               void*                            userData)
{
    //gets rid of const in pointer -- some functions we call aren't const
    MdfModel::VectorLayerDefinition* fl = (MdfModel::VectorLayerDefinition*)layer;

    if (!fl) return; //should never get here in the first place!

    // look through the scale ranges to find a valid one
    // the first one that contains the given scale will be used
    MdfModel::VectorScaleRangeCollection* ranges = fl->GetScaleRanges();

    MdfModel::VectorScaleRange* range = Stylizer::FindScaleRange(*ranges, renderer->GetMapScale());

    // no range -- do not stylize
    if (NULL == range) return;

    // get the geometry column name
    const wchar_t* gpName = features->GetGeomPropName();
    if (NULL == gpName)
        return;

    // we have a valid scale range and geometry... we can now go over the
    // features and apply the feature styles in that range
    MdfModel::FeatureTypeStyleCollection* ftsc = range->GetFeatureTypeStyles();

    //Sidenote: as of now (8/10/04) it is not clear whether FeatureTypeStyle
    //is matched to features via geometry type or via name of the feature
    //class.  For now, we match by geometry.

    // configure the filter with the current map/layer info
    RS_MapUIInfo* mapInfo = renderer->GetMapInfo();
    RS_LayerUIInfo* layerInfo = renderer->GetLayerInfo();
    RS_FeatureClassInfo* featInfo = renderer->GetFeatureClassInfo();

    const RS_String& session = (mapInfo != NULL)? mapInfo->session() : s_Empty;
    const RS_String& mapName = (mapInfo != NULL)? mapInfo->name() : s_Empty;
    const RS_String& layerId = (layerInfo != NULL)? layerInfo->guid() : s_Empty;
    const RS_String& featCls = (featInfo != NULL)? featInfo->name() : s_Empty;

    RS_FilterExecutor* exec = RS_FilterExecutor::Create(features);
    exec->SetMapLayerInfo(session, mapName, layerId, featCls);

    // check if we have any composite type styles - if we find at least
    // one then we'll use it and ignore any other non-composite type styles
    // TODO: confirm this is the behavior we want
    bool foundComposite = false;
    for (int i=0; i<ftsc->GetCount(); i++)
    {
        MdfModel::FeatureTypeStyle* fts = ftsc->GetAt(i);
        if (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts) == FeatureTypeStyleVisitor::ftsComposite)
        {
            foundComposite = true;
            break;
        }
    }

    // composite type styles are handled by the new style engine
    if (foundComposite)
    {
        this->m_styleEngine->StylizeVectorLayer(fl, range, renderer, features, exec, xformer, cancel, userData);
    }
    else
    {
        //extract hyperlink and tooltip info
        //this is invariant, so do outside of feature iterator loop
        const MdfModel::MdfString& mdfTip = fl->GetToolTip();
        const MdfModel::MdfString& mdfUrl = fl->GetUrl();
        const MdfModel::MdfString* lrTip = mdfTip.empty()? NULL : &mdfTip;
        const MdfModel::MdfString* lrUrl = mdfUrl.empty()? NULL : &mdfUrl;

        //TODO:
        //*****************************************
        //* THIS CALL IS REALLY REALLY REALLY SLOW!!!
        //*****************************************
        //It needs to be done per feature if there is inheritance of feature classes
        //but is so horribly slow that in all other cases it needs to be optimized away
        //FdoPtr<FdoClassDefinition> concreteClass = features->GetClassDefinition();

        bool bClip = renderer->RequiresClipping();

        #ifdef _DEBUG
        int nFeatures = 0;
        #endif

        //main loop over feature data
        while (features->ReadNext())
        {
            #ifdef _DEBUG
            nFeatures++;
            #endif

            LineBuffer* lb = m_lbPool->NewLineBuffer(8);

            try
            {
                features->GetGeometry(gpName, lb, xformer);
            }
            catch (FdoException* e)
            {
                //geometry could be null in which case FDO throws an exception
                //we move on to the next feature
                ProcessStylizerException(e, __LINE__, __WFILE__);
                m_lbPool->FreeLineBuffer(lb);
                continue;
            }

            if (lb && bClip)
            {
                //clip geometry to given map request extents
                //TODO: is this the right place to do so?
                LineBuffer* lbc = lb->Clip(renderer->GetBounds(), LineBuffer::ctAGF, m_lbPool);

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
                {
                    RS_ElevationSettings* elevSettings = NULL; 
                    MdfModel::ElevationSettings* modelElevSettings = range->GetElevationSettings();
                    if(modelElevSettings != NULL)
                    {
                        RS_ElevationType elevType;
                        switch(modelElevSettings->GetElevationType())
                        {
                        case MdfModel::ElevationSettings::Absolute:
                            {
                                elevType = RS_ElevationType_Absolute;
                                break;
                            }
                        case MdfModel::ElevationSettings::RelativeToGround:
                        default:
                            {
                                elevType = RS_ElevationType_RelativeToGround;
                                break;
                            }
                        }
                        elevSettings = new RS_ElevationSettings(modelElevSettings->GetZOffsetExpression(),
                            modelElevSettings->GetZExtrusionExpression(),
                            MdfModel::LengthConverter::UnitToMeters(modelElevSettings->GetUnit(), 1.0),
                            elevType);
                    }
                    adapter->Stylize(renderer, features, exec, lb, fts, lrTip, lrUrl, elevSettings);
                    
                    delete elevSettings;
                    elevSettings = NULL;
                }
            }

            if (lb)
                m_lbPool->FreeLineBuffer(lb); // free geometry when done stylizing

            if (cancel && cancel(userData)) break;
        }

        #ifdef _DEBUG
        printf("  DefaultStylizer::StylizeVectorLayer() Layer: %S  Features: %d\n", layer->GetFeatureName().c_str(), nFeatures);
        #endif
    }

    //need the cast due to multiple inheritance resulting in two Disposables
    //in the vtable of FilterExecutor
    ((FdoIExpressionProcessor*)exec)->Release();

    //need to get rid of these since they cache per layer theming information
    //which may conflict with the next layer
    ClearAdapters();
    m_styleEngine->ClearCache();
}


void DefaultStylizer::StylizeGridLayer(const MdfModel::GridLayerDefinition* layer,
                                             Renderer*                      renderer,
                                             RS_FeatureReader*              features,
                                             CSysTransformer*               /*xformer*/,
                                             CancelStylization              cancel,
                                             void*                          userData)
{
    //gets rid of const in pointer -- some functions we call aren't const
    MdfModel::GridLayerDefinition* gl = (MdfModel::GridLayerDefinition*)layer;

    if (!gl) return; //should never get here in the first place!

    // look through the scale ranges to find a valid one
    // the first one that contains the given scale will be used
    MdfModel::GridScaleRangeCollection* ranges = gl->GetScaleRanges();

    MdfModel::GridScaleRange* range = Stylizer::FindScaleRange(*ranges, renderer->GetMapScale());

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
    //FdoPtr<FdoClassDefinition> concreteClass = features->GetClassDefinition();

    //main loop over raster data
    while (features->ReadNext())
    {
        RS_Raster* raster = features->GetRaster(rpName);

        //need to clear out the filter execution engine cache
        //some feature attributes may be cached while executing theming
        //expressions and this call flushes that
        exec->Reset();

        if (m_pRasterAdapter)
            m_pRasterAdapter->Stylize(renderer, features, exec, raster, gcs, NULL, NULL);

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


void DefaultStylizer::StylizeDrawingLayer(const MdfModel::DrawingLayerDefinition* layer,
                                                Renderer*                         renderer,
                                                RS_LayerUIInfo*                   legendInfo,
                                                RS_InputStream*                   dwfin,
                                                const RS_String&                  layerFilter,
                                                CSysTransformer*                  xformer)
{
    double mapScale = renderer->GetMapScale();

    //check if we are in scale range
    if (mapScale >= layer->GetMinScale() && mapScale < layer->GetMaxScale())
    {
        renderer->StartLayer(legendInfo, NULL);

        //TODO: dwf password
        renderer->AddDWFContent(dwfin, xformer, L"", L"", layerFilter);

        renderer->EndLayer();
    }
}


//WARNING: given pointer to the new stylizer will be destroyed
//by the stylizer (in its destructor)
void DefaultStylizer::SetGeometryAdapter(FdoGeometryType type, GeometryAdapter* stylizer)
{
    GeometryAdapter* old = (GeometryAdapter*)m_hGeomStylizers[type];

    if (old) delete old;

    m_hGeomStylizers[type] = stylizer;
}


void DefaultStylizer::SetStylizeFeature(FdoClassDefinition* /*classDef*/, GeometryAdapter* /*stylizer*/)
{
    //TODO: implemented
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
    case FdoGeometryType_LineString:
        m_hGeomStylizers[FdoGeometryType_LineString] = new PolylineAdapter(m_lbPool);
        break;
    case FdoGeometryType_MultiLineString:
        m_hGeomStylizers[FdoGeometryType_MultiLineString] = new PolylineAdapter(m_lbPool);
        break;
    case FdoGeometryType_CurveString:
        m_hGeomStylizers[FdoGeometryType_CurveString] = new PolylineAdapter(m_lbPool);
        break;
    case FdoGeometryType_MultiCurveString:
        m_hGeomStylizers[FdoGeometryType_MultiCurveString] = new PolylineAdapter(m_lbPool);
        break;
    case FdoGeometryType_Polygon:
        m_hGeomStylizers[FdoGeometryType_Polygon] = new PolygonAdapter(m_lbPool);
        break;
    case FdoGeometryType_MultiPolygon:
        m_hGeomStylizers[FdoGeometryType_MultiPolygon] = new PolygonAdapter(m_lbPool);
        break;
    case FdoGeometryType_CurvePolygon:
        m_hGeomStylizers[FdoGeometryType_CurvePolygon] = new PolygonAdapter(m_lbPool);
        break;
    case FdoGeometryType_MultiCurvePolygon:
        m_hGeomStylizers[FdoGeometryType_MultiCurvePolygon] = new PolygonAdapter(m_lbPool);
        break;
    case FdoGeometryType_Point:
        m_hGeomStylizers[FdoGeometryType_Point] = new PointAdapter(m_lbPool);
        break;
    case FdoGeometryType_MultiPoint:
        m_hGeomStylizers[FdoGeometryType_MultiPoint] = new PointAdapter(m_lbPool);
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
