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

#include "stdafx.h"
#include "DefaultStylizer.h"
#include "GeometryAdapter.h"
#include "PolylineAdapter.h"
#include "PolygonAdapter.h"
#include "PointAdapter.h"
#include "RasterAdapter.h"
#include "Renderer.h"
#include "LineBuffer.h"
#include "ElevationSettings.h"
#include "FeatureTypeStyleVisitor.h"
#include "StylizationEngine.h"
#include "ExpressionHelper.h"


DefaultStylizer::DefaultStylizer(SE_SymbolManager* sman)
{
    m_pRasterAdapter = NULL;
    m_styleEngine = new StylizationEngine(sman);
    m_lbPool = new LineBufferPool;
}


DefaultStylizer::~DefaultStylizer()
{
    // free geom adapters -- not strictly needed here but just
    // in case something crashed in the middle of stylization
    ClearAdapters();

    delete m_styleEngine;
    delete m_lbPool;
}


void DefaultStylizer::StylizeVectorLayer(MdfModel::VectorLayerDefinition* layer,
                                         Renderer*                        renderer,
                                         RS_FeatureReader*                features,
                                         CSysTransformer*                 xformer,
                                         double                           mapScale,
                                         CancelStylization                cancel,
                                         void*                            userData)
{
    // look through the scale ranges to find a valid one
    // the first one that contains the given scale will be used
    MdfModel::VectorScaleRangeCollection* scaleRanges = layer->GetScaleRanges();
    MdfModel::VectorScaleRange* scaleRange = Stylizer::FindScaleRange(*scaleRanges, mapScale);

    // no range -- do not stylize
    if (NULL == scaleRange)
        return;

    // we have a valid scale range... we can now go over the
    // features and apply the feature styles in that range
    MdfModel::FeatureTypeStyleCollection* ftsc = scaleRange->GetFeatureTypeStyles();

    // check if we have any composite type styles - if we find at least
    // one then we'll use it and ignore any other non-composite type styles
    // TODO: confirm this is the behavior we want
    bool foundComposite = false;
    for (int i=0; i<ftsc->GetCount(); ++i)
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
        this->m_styleEngine->StylizeVectorLayer(layer, scaleRange, (SE_Renderer*)renderer, features, xformer, cancel, userData);
    }
    else
    {
        int nFeatures = 0;

        // Here we check if the layer has a composite polyline style.  If so,
        // we will make multiple feature queries and process the geometry
        // once for each line style.  This makes things look much better.

        // We can render polylines with composite styles using this method
        // only if there is a single line style
        MdfModel::FeatureTypeStyle* fts = (ftsc->GetCount() > 0)? ftsc->GetAt(0) : NULL;
        if (NULL != fts 
            && FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts) == FeatureTypeStyleVisitor::ftsLine
            && renderer->RequiresCompositeLineStyleSeparation())
        {
            MdfModel::RuleCollection* rules = fts->GetRules();

            // temporary holder for line styles
            std::vector<MdfModel::LineSymbolizationCollection*> tmpSyms;

            // For each rule transfer all line styles into a temporary collection
            // and away from the layer definition.  Also obtain the maximum # of
            // styles for all the rules.
            int maxStyles = 0;
            for (int k=0; k<rules->GetCount(); ++k)
            {
                MdfModel::LineRule* lr = (MdfModel::LineRule*)rules->GetAt(k);
                MdfModel::LineSymbolizationCollection* syms = lr->GetSymbolizations();

                // move composite line styles to a temporary collection away
                // from the one in the layer definition
                MdfModel::LineSymbolizationCollection* syms2 = new MdfModel::LineSymbolizationCollection();
                while (syms->GetCount() > 0)
                    syms2->Adopt(syms->OrphanAt(0));

                tmpSyms.push_back(syms2);

                // keep track of the maximum # of styles
                maxStyles = rs_max(maxStyles, syms2->GetCount());
            }

            // if there are no styles, we still want to render so that
            // labels draw even if we are not drawing the actual geometry
            if (maxStyles == 0)
            {
                nFeatures = StylizeVLHelper(layer, scaleRange, renderer, features, true, xformer, cancel, userData);
            }
            else
            {
                // now for each separate line style - run a feature query
                // and stylization loop with that single style
                for (int i=0; i<maxStyles; ++i)
                {
                    // reset reader if this is not the first time we stylize the feature
                    if (i > 0)
                        features->Reset();

                    // collection to store labels temporarily so that we add labels
                    // to each feature just once
                    std::vector<MdfModel::TextSymbol*> tmpLabels;

                    // for each rule, transfer a single line style from the temporary
                    // collection to the layer definition
                    for (int m=0; m<rules->GetCount(); ++m)
                    {
                        MdfModel::LineRule* lr = (MdfModel::LineRule*)rules->GetAt(m);

                        // remove label if this is not the first time we stylize
                        // the feature
                        if (i > 0)
                            tmpLabels.push_back(lr->GetLabel()->OrphanSymbol());

                        MdfModel::LineSymbolizationCollection* syms = lr->GetSymbolizations();
                        MdfModel::LineSymbolizationCollection* syms2 = tmpSyms[m];

                        int index = rs_min(i, syms2->GetCount()-1);
                        syms->Adopt(syms2->GetAt(index));
                    }

                    nFeatures += StylizeVLHelper(layer, scaleRange, renderer, features, i==0, xformer, cancel, userData);

                    // transfer line styles back to layer definition
                    for (int m=0; m<rules->GetCount(); ++m)
                    {
                        MdfModel::LineRule* lr = (MdfModel::LineRule*)rules->GetAt(m);

                        // add back label if we removed it
                        if (i > 0)
                            lr->GetLabel()->AdoptSymbol(tmpLabels[m]);

                        MdfModel::LineSymbolizationCollection* syms = lr->GetSymbolizations();
                        syms->OrphanAt(0);
                    }
                }
            }

            // move composite line styles back to original layer definition
            // collection so that it frees them up when we destroy it
            for (int m=0; m<rules->GetCount(); ++m)
            {
                MdfModel::LineRule* lr = (MdfModel::LineRule*)rules->GetAt(m);
                MdfModel::LineSymbolizationCollection* syms = lr->GetSymbolizations();
                MdfModel::LineSymbolizationCollection* syms2 = tmpSyms[m];

                while (syms2->GetCount() > 0)
                    syms->Adopt(syms2->OrphanAt(0));

                delete syms2;
            }
        }
        else
        {
            // composite line style separation not required
            nFeatures = StylizeVLHelper(layer, scaleRange, renderer, features, true, xformer, cancel, userData);
        }

        #ifdef _DEBUG
        printf("  DefaultStylizer::StylizeVectorLayer() Layer: %S  Features: %d\n", layer->GetFeatureName().c_str(), nFeatures);
        #endif
    }

    // need to get rid of these since they cache per layer theming
    // information which may conflict with the next layer
    ClearAdapters();
    m_styleEngine->ClearCache();
}


int DefaultStylizer::StylizeVLHelper(MdfModel::VectorLayerDefinition* layer,
                                     MdfModel::VectorScaleRange*      scaleRange,
                                     Renderer*                        renderer,
                                     RS_FeatureReader*                features,
                                     bool                             initialPass,
                                     CSysTransformer*                 xformer,
                                     CancelStylization                cancel,
                                     void*                            userData)
{
    bool bClip = renderer->RequiresClipping();
    double drawingScale = renderer->GetDrawingScale();

    MdfModel::FeatureTypeStyleCollection* ftsc = scaleRange->GetFeatureTypeStyles();

    // Extract hyperlink and tooltip expressions, only if required.
    // Note that the expression is the same for all features in the layer,
    // so we retrieve it outside of the feature iterator loop.
    const MdfModel::MdfString* lrTip = NULL;
    const MdfModel::MdfString* lrUrl = NULL;
	if (renderer->SupportsTooltips())
	{
		const MdfModel::MdfString& mdfTip = layer->GetToolTip();
		if(!mdfTip.empty())
		{
			lrTip = &mdfTip;
		}
	}
	if (renderer->SupportsHyperlinks())
	{
		const MdfModel::MdfString& mdfUrl = layer->GetUrl();
		if(!mdfUrl.empty())
		{
			lrUrl = &mdfUrl;
		}
	}
	
	// elevation settings - also invariant
    RS_ElevationSettings* elevSettings = NULL;
    MdfModel::ElevationSettings* modelElevSettings = scaleRange->GetElevationSettings();
    if (modelElevSettings != NULL)
    {
        RS_ElevationType elevType;
        switch (modelElevSettings->GetElevationType())
        {
            case MdfModel::ElevationSettings::Absolute:
                elevType = RS_ElevationType_Absolute;
                break;

            case MdfModel::ElevationSettings::RelativeToGround:
            default:
                elevType = RS_ElevationType_RelativeToGround;
                break;
        }
        elevSettings = new RS_ElevationSettings(modelElevSettings->GetZOffsetExpression(),
            modelElevSettings->GetZExtrusionExpression(),
            MdfModel::LengthConverter::UnitToMeters(modelElevSettings->GetUnit(), 1.0),
            elevType);
    }

    // get the geometry column name
    const wchar_t* gpName = features->GetGeomPropName();
    if (NULL == gpName)
        return 0;

    // create an expression engine with our custom functions
    // NOTE: We must create a new engine for each call to StylizeVLHelper.  The
    //       engine stores a weak reference to the RS_FeatureReader's internal
    //       FdoIFeatureReader, and this internal reader is different for each
    //       call to StylizeVLHelper.
    FdoPtr<FdoExpressionEngine> exec = ExpressionHelper::GetExpressionEngine(renderer, features);

    // main loop over feature data
    int nFeatures = 0;
    while (features->ReadNext())
    {
        #ifdef _DEBUG
        ++nFeatures;
        #endif

        LineBuffer* lb = LineBufferPool::NewLineBuffer(m_lbPool, 8, FdoDimensionality_Z, false);
        if (!lb)
            continue;

        // tell line buffer the current drawing scale (used for arc tessellation)
        lb->SetDrawingScale(drawingScale);

        try
        {
            if (!features->IsNull(gpName))
                features->GetGeometry(gpName, lb, xformer);
        }
        catch (FdoException* e)
        {
            // just move on to the next feature
            e->Release();
            LineBufferPool::FreeLineBuffer(m_lbPool, lb);
            continue;
        }

        if (bClip)
        {
            // clip geometry to given map request extents
            LineBuffer* lbc = lb->Clip(renderer->GetBounds(), LineBuffer::ctAGF, m_lbPool);

            // Free original line buffer if the geometry was actually clipped.
            // Note that the original geometry is still accessible using
            // RS_FeatureReader::GetGeometry.
            if (lbc != lb)
            {
                LineBufferPool::FreeLineBuffer(m_lbPool, lb);

                // if the clipped buffer is NULL just move on to the next feature
                if (!lbc)
                    continue;

                // otherwise continue processing with the clipped buffer
                lb = lbc;
            }
        }

        // if we know how to stylize this type of geometry, then go ahead
        GeometryAdapter* adapter = FindGeomAdapter(lb->geom_type());
        if (adapter)
        {
            // we need to stylize once for each FeatureTypeStyle that matches
            // the geometry type (Note: this may have to change to match
            // feature classes)
            for (int i=0; i<ftsc->GetCount(); ++i)
            {
                MdfModel::FeatureTypeStyle* fts = ftsc->GetAt(i);
                adapter->Stylize(renderer, features, initialPass, exec, lb, fts, lrTip, lrUrl, elevSettings);
            }
        }

        // free geometry when done stylizing
        LineBufferPool::FreeLineBuffer(m_lbPool, lb);

        if (cancel && cancel(userData))
            break;
    }

    delete elevSettings;

    return nFeatures;
}


void DefaultStylizer::StylizeGridLayer(MdfModel::GridLayerDefinition* layer,
                                       Renderer*                      renderer,
                                       RS_FeatureReader*              features,
                                       CSysTransformer*               layer2mapxformer,
                                       double                         mapScale,
                                       CancelStylization              cancel,
                                       void*                          userData)
{
    // look through the scale ranges to find a valid one
    // the first one that contains the given scale will be used
    MdfModel::GridScaleRangeCollection* ranges = layer->GetScaleRanges();
    MdfModel::GridScaleRange* range = Stylizer::FindScaleRange(*ranges, mapScale);

    // no range -- do not stylize
    if (NULL == range) return;

    // we have a valid scale range... we can now go over the features and
    // apply the feature styles in that range

    const wchar_t* rpName = features->GetRasterPropName();

    //no geometry -- do not stylize
    if (NULL == rpName) return;

    // create an expression engine with our custom functions
    FdoPtr<FdoExpressionEngine> exec = ExpressionHelper::GetExpressionEngine(renderer, features);

    // find the FeatureTypeStyle
    MdfModel::GridColorStyle* gcs = range->GetColorStyle();
    MdfModel::GridSurfaceStyle* gss = range->GetSurfaceStyle();

    //init the raster adapter
    if (!m_pRasterAdapter)
        m_pRasterAdapter = new RasterAdapter(m_lbPool);

    // main loop over raster data
    while (features->ReadNext())
    {
        // for vector data -- xformer is passed into the feature reader caller and the
        // data is transformed to the map cs
        RS_Raster* raster = features->GetRaster(rpName);

        // at this point raster is in the raster layer's cs
        if (m_pRasterAdapter)
            m_pRasterAdapter->Stylize(renderer, features, true, exec, raster, gcs, gss, NULL, NULL, NULL, layer2mapxformer);

        // need to free returned raster
        delete raster;

        if (cancel && cancel(userData))
            break;
    }

    // need to get rid of these since they cache per layer theming information
    // which may conflict with the next layer
    ClearAdapters();
}


void DefaultStylizer::StylizeDrawingLayer(MdfModel::DrawingLayerDefinition* layer,
                                          Renderer*                         renderer,
                                          RS_InputStream*                   dwfin,
                                          CSysTransformer*                  xformer,
                                          double                            mapScale)
{
    // check if we are in scale range
    if (mapScale >= layer->GetMinScale() && mapScale < layer->GetMaxScale())
    {
        RS_String layerFilter(layer->GetLayerFilter());

        // TODO: dwf password
        renderer->AddDWFContent(dwfin, xformer, L"", L"", layerFilter);
    }
}


// WARNING: given pointer to the new stylizer will be destroyed
// by the stylizer (in its destructor)
void DefaultStylizer::SetGeometryAdapter(FdoGeometryType type, GeometryAdapter* stylizer)
{
    GeometryAdapter* old = (GeometryAdapter*)m_hGeomStylizers[type];

    if (old) delete old;

    m_hGeomStylizers[type] = stylizer;
}


GeometryAdapter* DefaultStylizer::FindGeomAdapter(int geomType)
{
    GeometryAdapter* adapter = m_hGeomStylizers[geomType];

    // have adapter -- return it
    if (adapter)
        return adapter;

    // otherwise need to create one based on the geometry type
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

    // free the stylizer objects
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
