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

#include "StylizationUtil.h"

#include "SAX2Parser.h"
#include "Stylization.h"
#include "Bounds.h"
#include "Renderer.h"
#include "GDRenderer.h"
#include "Stylizer.h"
#include "SymbolVisitor.h"
#include "SLDSymbols.h"
#include "SE_BufferPool.h"
#include "SE_StyleVisitor.h"
#include "SEMgSymbolManager.h"
#include "TransformCache.h"

//For logging
#include "ServerManager.h"
#include "LogManager.h"
#include "LogDetail.h"

#ifndef _WIN32
#define _wcsnicmp wcsncasecmp

// Linux version of GetTickCount()
#include <sys/times.h>

long GetTickCount()
{
    tms tm;
    return times(&tm);
}
#endif


MdfModel::MapDefinition* MgStylizationUtil::GetMapDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId)
{
    //get and parse the mapdef
    Ptr<MgByteReader> mdfReader = svcResource->GetResourceContent(resId, L"");

    Ptr<MgByteSink> sink = new MgByteSink(mdfReader);
    Ptr<MgByte> bytes = sink->ToBuffer();

    assert(bytes->GetLength() > 0);

    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*) bytes->Bytes(), bytes->GetLength());

    assert(parser.GetSucceeded());

    // detach the map definition from the parser - it's
    // now the caller's responsibility to delete it
    MdfModel::MapDefinition* mdef = parser.DetachMapDefinition();

    assert(mdef != NULL);
    return mdef;
}


MdfModel::LayerDefinition* MgStylizationUtil::GetLayerDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId)
{
    //get and parse the mapdef
    Ptr<MgByteReader> ldfReader = svcResource->GetResourceContent(resId, L"");

    Ptr<MgByteSink> sink = new MgByteSink(ldfReader);
    Ptr<MgByte> bytes = sink->ToBuffer();

    assert(bytes->GetLength() > 0);

    MdfParser::SAX2Parser parser;
    parser.ParseString((const char *)bytes->Bytes(), bytes->GetLength());

    assert(parser.GetSucceeded());

    // detach the feature layer definition from the parser - it's
    // now the caller's responsibility to delete it
    MdfModel::LayerDefinition* ldef = parser.DetachLayerDefinition();

    assert(ldef != NULL);
    return ldef;
}


void MgStylizationUtil::ParseColor(CREFSTRING colorstr, RS_Color& rscol)
{
    const wchar_t* scolor = colorstr.c_str();

    size_t len = wcslen(scolor);
    //string is in the form "AARRGGBB"

    //is color string empty?
    if (len == 0)
    {
        //set to undefined color
        rscol = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
        return;
    }

    unsigned int color = 0;

    //try to check if the expression is constant
    int status = 0;
    if (len == 8)
    {
        status = swscanf(scolor, L"%8X", &color);
    }
    else if (len == 6)
    {
        status = swscanf(scolor, L"%6X", &color);

        //there was no alpha specified in the constant string, add it
        color |= 0xFF000000;
    }
    else
    {
        color = 0xFF000000; //black
    }

    rscol.alpha() =  color >> 24;
    rscol.red() = (color >> 16) & 0xFF;
    rscol.green() = (color >> 8) & 0xFF;
    rscol.blue() =  color & 0xFF;
}


RSMgFeatureReader* MgStylizationUtil::ExecuteFeatureQuery(MgFeatureService* svcFeature,
                                                          RS_Bounds& extent,
                                                          MdfModel::VectorLayerDefinition* vl,
                                                          const wchar_t* overrideFilter,
                                                          MgCoordinateSystem* mapCs,
                                                          MgCoordinateSystem* layerCs,
                                                          TransformCache* cache)
{
#ifdef _DEBUG
    long dwStart = GetTickCount();
#endif

    //get feature source id
    STRING sfeatResId = vl->GetResourceID();
    Ptr<MgResourceIdentifier> featResId = new  MgResourceIdentifier(sfeatResId);

    Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();

    MdfModel::NameStringPairCollection* mappings = vl->GetPropertyMappings();

    //we want to transform from mapping space to layer space
    Ptr<MgCoordinateSystemTransform> trans;

    if (mapCs && layerCs)
    {
        if (NULL != cache)
        {
            trans = cache->GetMgTransform();
        }

        if (!trans)
        {
            trans = new MgCoordinateSystemTransform(mapCs, layerCs);
            if (NULL != cache)
            {
                cache->SetMgTransform(trans);
            }
        }
    }

    //start with bounds in mapping space
    Ptr<MgCoordinate> ll = new MgCoordinateXY(extent.minx, extent.miny);
    Ptr<MgCoordinate> ur = new MgCoordinateXY(extent.maxx, extent.maxy);

    Ptr<MgEnvelope> layerExt;

    //do we have a cached extent?
    if (NULL != cache)
    {
        layerExt = cache->GetEnvelope();
    }

    if (!layerExt)
    {
        //no cached extent
        //if we have a valid transform, get the request extent in layer's space
        if (trans)
        {
            Ptr<MgEnvelope> mapExt = new MgEnvelope(ll, ur);

            //get corresponding bounds in layer space
            layerExt = trans->Transform(mapExt);

            ll = layerExt->GetLowerLeftCoordinate();
            ur = layerExt->GetUpperRightCoordinate();

            if (NULL != cache)
            {
                cache->SetEnvelope(layerExt);
            }
        }
    }
    else
    {
        //did have a cached extent, just use it.
        ll = layerExt->GetLowerLeftCoordinate();
        ur = layerExt->GetUpperRightCoordinate();
    }

    Ptr<MgCoordinateXY> c1 = new MgCoordinateXY(ll->GetX(), ll->GetY());
    Ptr<MgCoordinateXY> c2 = new MgCoordinateXY(ur->GetX(), ll->GetY());
    Ptr<MgCoordinateXY> c3 = new MgCoordinateXY(ur->GetX(), ur->GetY());
    Ptr<MgCoordinateXY> c4 = new MgCoordinateXY(ll->GetX(), ur->GetY());
    Ptr<MgCoordinateXY> c5 = new MgCoordinateXY(ll->GetX(), ll->GetY());

    Ptr<MgCoordinateCollection> cc = new MgCoordinateCollection();
    cc->Add(c1);
    cc->Add(c2);
    cc->Add(c3);
    cc->Add(c4);
    cc->Add(c5);

    Ptr<MgLinearRing> outer = new MgLinearRing(cc);
    Ptr<MgPolygon> poly = new MgPolygon(outer, NULL);

    MdfModel::MdfString geom = vl->GetGeometry();

    //set layer feature filter if any
    if (overrideFilter && *overrideFilter != L'\0')
    {
        //if there is an override filter, we will use that instead
        //and drop the spatial query. We need to not use the spatial query
        //in order to work around a problem with the filter evaluation in
        //the FDO SHP provider. There are no bad side effects of this as long
        //as override filters are only used to select features with specific IDs
        //like they currently are.
        options->SetFilter(overrideFilter);
    }
    else
    {
        //add the geometry property
        //and also spatial filter
        if (!geom.empty())
        {
            //geometry providers -> we want an envelope intersects.
            //If not, we need intersects. We should parse and check capabilities
            //if we are to be really thorough, but we also do have a
            //a more general query if the spatial fails
            options->SetSpatialFilter(geom, poly, MgFeatureSpatialOperations::EnvelopeIntersects);
        }

        if (!vl->GetFilter().empty())
            options->SetFilter(vl->GetFilter());
    }

    Ptr<MgFeatureReader> rdr;

    //poor man's substitute for checking capabilities
    //try catch until we succeed...
    try
    {
        // TODO: could it be an extension name and not a FeatureClassName?
        rdr = svcFeature->SelectFeatures(featResId, vl->GetFeatureName(), options);
    }
    catch (MgFdoException* e)
    {
        e->Release();

        try
        {
            if (!geom.empty())
                options->SetSpatialFilter(geom, poly, MgFeatureSpatialOperations::Intersects);

            // TODO: could it be an extension name and not a FeatureClassName?
            rdr = svcFeature->SelectFeatures(featResId, vl->GetFeatureName(), options);
        }
        catch (MgFdoException* e)
        {
            e->Release();

            //finally try without a filter
            // TODO: could it be an extension name and not a FeatureClassName?
            options = NULL;
            rdr = svcFeature->SelectFeatures(featResId, vl->GetFeatureName(), options);
        }
    }

#ifdef _DEBUG
    printf("  ExecuteFeatureQuery() total time = %6.4f (s)\n", (GetTickCount()-dwStart)/1000.0);
#endif

    return new RSMgFeatureReader(rdr.p, svcFeature, featResId.p, options, vl->GetGeometry());
}


RSMgFeatureReader * MgStylizationUtil::ExecuteRasterQuery(MgFeatureService* svcFeature,
                                                          RS_Bounds& extent,
                                                          MdfModel::GridLayerDefinition* gl,
                                                          const wchar_t* overrideFilter,
                                                          MgCoordinateSystem* mapCs,
                                                          MgCoordinateSystem* layerCs,
                                                          int devWidth,
                                                          int devHeight)
{
    //get feature source id
    STRING sfeatResId = gl->GetResourceID();
    Ptr<MgResourceIdentifier> featResId = new  MgResourceIdentifier(sfeatResId);

    Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();

    //MdfModel::NameStringPairCollection* mappings = gl->GetPropertyMappings();

    //we want to transform from mapping space to layer space
    Ptr<MgCoordinateSystemTransform> trans;
    if (mapCs && layerCs)
    {
        trans = new MgCoordinateSystemTransform(mapCs, layerCs);
    }

    //bounds in mapping space
    Ptr<MgCoordinate> ll = new MgCoordinateXY(extent.minx, extent.miny);
    Ptr<MgCoordinate> ur = new MgCoordinateXY(extent.maxx, extent.maxy);

    //if we have a valid transform, get the request extent in layer's space
    if (trans)
    {
        Ptr<MgEnvelope> mapExt = new MgEnvelope(ll, ur);

        //get corresponding bounds in layer space
        Ptr<MgEnvelope> layerExt = trans->Transform(mapExt);
        ll = layerExt->GetLowerLeftCoordinate();
        ur = layerExt->GetUpperRightCoordinate();
    }

    Ptr<MgCoordinateXY> c1 = new MgCoordinateXY(ll->GetX(), ll->GetY());
    Ptr<MgCoordinateXY> c2 = new MgCoordinateXY(ur->GetX(), ll->GetY());
    Ptr<MgCoordinateXY> c3 = new MgCoordinateXY(ur->GetX(), ur->GetY());
    Ptr<MgCoordinateXY> c4 = new MgCoordinateXY(ll->GetX(), ur->GetY());
    Ptr<MgCoordinateXY> c5 = new MgCoordinateXY(ll->GetX(), ll->GetY());

    Ptr<MgCoordinateCollection> cc = new MgCoordinateCollection();
    cc->Add(c1);
    cc->Add(c2);
    cc->Add(c3);
    cc->Add(c4);
    cc->Add(c5);

    Ptr<MgLinearRing> outer = new MgLinearRing(cc);
    Ptr<MgPolygon> poly = new MgPolygon(outer, NULL);

    //get the name of the raster property
    MdfModel::MdfString geom = gl->GetGeometry();

    //TODO:
    //BOGUS:
    //HACK:
    //Currently it takes different requests to get a raster from WMS versus
    //the raster provider. This code should be unified once support for the
    //RESAMPLE() function is added to all raster providers.

    //add the geometry property
    //and also spatial filter
    if (!geom.empty())
    {
        // We need to restrict the size of the returned raster to MgByte::MaxSize,
        // otherwise we'll get an MgArgumentOutOfRangeException when processing
        // the returned raster stream.  Assume worst case scenario of 32bpp...
        if (4*devWidth*devHeight > MgByte::MaxSize)
        {
            double factor = sqrt((double)MgByte::MaxSize / (double)(4*devWidth*devHeight));
            devWidth  = (int)(factor * devWidth);
            devHeight = (int)(factor * devHeight);
            assert(4*devWidth*devHeight <= MgByte::MaxSize);
        }

        //Set up RESAMPLE command
        FdoPtr<FdoExpressionCollection> funcParams = FdoExpressionCollection::Create();
        FdoPtr<FdoIdentifier> rasterProp = FdoIdentifier::Create(geom.c_str());
        funcParams->Add(rasterProp);
        FdoPtr<FdoDataValue> minX = FdoDataValue::Create(extent.minx, FdoDataType_Double);
        funcParams->Add(minX);
        FdoPtr<FdoDataValue> minY = FdoDataValue::Create(extent.miny, FdoDataType_Double);
        funcParams->Add(minY);
        FdoPtr<FdoDataValue> maxX = FdoDataValue::Create(extent.maxx, FdoDataType_Double);
        funcParams->Add(maxX);
        FdoPtr<FdoDataValue> maxY = FdoDataValue::Create(extent.maxy, FdoDataType_Double);
        funcParams->Add(maxY);
        FdoPtr<FdoDataValue> height = FdoDataValue::Create(devHeight);
        funcParams->Add(height);
        FdoPtr<FdoDataValue> width = FdoDataValue::Create(devWidth);
        funcParams->Add(width);

        FdoPtr<FdoFunction> clipFunc = FdoFunction::Create(L"RESAMPLE", funcParams);

        STRING func = clipFunc->ToString();
        options->AddComputedProperty(L"clipped_raster", func);

        //TODO: do we really need the spatial filter for RFP layers?
        //If not, we can safely remove that code which will effectively
        //make the whole raster request provider-agnostic
        if (geom == L"Image")
        {
            options->SetSpatialFilter(geom, poly, MgFeatureSpatialOperations::Intersects);
        }
        else if (geom == L"Raster")
        {
            //WMS provider raster property is called "Raster"
            options->AddFeatureProperty(L"FeatId");
        }
    }

    //set layer feature filter if any
    if (overrideFilter && *overrideFilter != L'\0') //is it empty?
        options->SetFilter(overrideFilter);
    else if (!gl->GetFilter().empty())
        options->SetFilter(gl->GetFilter());

    Ptr<MgFeatureReader> rdr;

    try
    {
        // TODO: could it be an extension name and not a FeatureClassName?
        rdr = svcFeature->SelectFeatures(featResId, gl->GetFeatureName(), options);
    }
    catch (MgFdoException* e)
    {
        e->Release();

        //finally try without a filter
        // TODO: could it be an extension name and not a FeatureClassName?
        options = NULL;
        rdr = svcFeature->SelectFeatures(featResId, gl->GetFeatureName(), options);
    }

    return new RSMgFeatureReader(rdr.p, svcFeature, featResId.p, options, L"clipped_raster");
}


void MgStylizationUtil::StylizeLayers(MgResourceService* svcResource,
                                      MgFeatureService* svcFeature,
                                      MgDrawingService* svcDrawing,
                                      MgCoordinateSystemFactory* csFactory,
                                      MgMap* map,
                                      MgReadOnlyLayerCollection* layers,
                                      MgStringCollection* overrideFilters,
                                      Stylizer* ds,
                                      Renderer* dr,
                                      MgCoordinateSystem* dstCs,
                                      bool expandExtents,
                                      bool checkRefreshFlag,
                                      double scale,
                                      bool selection)
{
    #ifdef _DEBUG
    long dwStart = GetTickCount();
    printf("\nStylizeLayers() **MAPSTART** Layers:%d  Scale:%f\n", layers->GetCount(), scale);
    #endif

    // Cache coordinate system transforms for the life of the
    // stylization operation.
    TransformCacheMap transformCache;

    for (int i = layers->GetCount()-1; i >= 0; i--)
    {
        MdfModel::LayerDefinition* ldf = NULL;

        Ptr<MgLayerBase> mapLayer = layers->GetItem(i);

        #ifdef _DEBUG
        printf("  StylizeLayers() **LAYERSTART** Name:%S  VAS:%S\n", (mapLayer->GetName()).c_str(), mapLayer->IsVisibleAtScale(scale) ? L"True" : L"False");
        #endif

        //don't send data if layer is not currently visible
        if ((!selection) && (!mapLayer->IsVisible()))
            continue;

        //don't send data if the refresh flag is not set
        if (checkRefreshFlag)
        {
            bool needsRefresh = false;
            MgMapBase::LayerRefreshMode refreshMode = map->GetLayerRefreshMode();
            if (refreshMode == MgMapBase::unspecified)
                needsRefresh = mapLayer->NeedsRefresh();
            else
                needsRefresh = (refreshMode == MgMapBase::refreshAll);

            if (!needsRefresh)
                continue;
        }

        MG_SERVER_MAPPING_SERVICE_TRY()

            //get layer definition
            Ptr<MgResourceIdentifier> layerid = mapLayer->GetLayerDefinition();
            ldf = GetLayerDefinition(svcResource, layerid);

            Ptr<MgLayerGroup> group = mapLayer->GetGroup();

            MgLogDetail logDetail(MgServiceType::MappingService, MgLogDetail::InternalTrace, L"MgStylizationUtil.StylizeLayers", mgStackParams);
            logDetail.AddString(L"Map",map->GetName());
            logDetail.AddResourceIdentifier(L"LayerId",layerid);
            logDetail.Create();

            //base map layers are not editable
            bool bEditable = true;
            if (mapLayer->GetLayerType() == MgLayerType::BaseMap)
                bEditable = false;

            //layer legend and ui specific information
            RS_UIGraphic uig(NULL, 0, mapLayer->GetLegendLabel());
            RS_LayerUIInfo legendInfo( mapLayer->GetName(),
                                       mapLayer->GetObjectId(),
                                       mapLayer->GetSelectable(),
                                       mapLayer->GetVisible(),
                                       bEditable,
                                       (group.p) ? group->GetName() : L"",
                                       (group.p) ? group->GetObjectId() : L"",
                                       mapLayer->GetDisplayInLegend(),
                                       mapLayer->GetExpandInLegend(),
                                      -mapLayer->GetDisplayOrder(),
                                       uig);

            MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf);
            MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf);
            MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf);

            if (vl)
            {
                #ifdef _DEBUG
                long dwLayerStart = GetTickCount();
                #endif

                // Modify the layer scale range to also support infinite
                // Need to apply default style as one will not be defined
                if (selection)
                {
                    MdfModel::VectorScaleRangeCollection* scaleRanges = vl->GetScaleRanges();
                    if (scaleRanges)
                    {
                        MdfModel::VectorScaleRange* scaleRange = scaleRanges->GetAt(0);
                        if (scaleRange)
                        {
                            scaleRange->SetMinScale(0.0);
                            scaleRange->SetMaxScale(MdfModel::VectorScaleRange::MAX_MAP_SCALE);
                        }
                    }
                }

                // make sure we have a valid scale range
                MdfModel::VectorScaleRange* scaleRange = Stylizer::FindScaleRange(*vl->GetScaleRanges(), scale);

                if (scaleRange)
                {
                    #ifdef _DEBUG
                    printf("  StylizeLayers() **Stylizing** Name:%S\n", (mapLayer->GetName()).c_str());
                    #endif

                    // get feature source id
                    STRING sfeatResId = vl->GetResourceID();
                    Ptr<MgResourceIdentifier> featResId = new MgResourceIdentifier(sfeatResId);

                    // get the feature extent to use with the query
                    RS_Bounds extent = dr->GetBounds();

                    // if requested, expand the feature extent to account for stylization
                    if (expandExtents)
                    {
                        // get the additional offset to account for stylization
                        double mcsOffset = ComputeStylizationOffset(map, scaleRange, scale);
                        extent.minx -= mcsOffset;
                        extent.miny -= mcsOffset;
                        extent.maxx += mcsOffset;
                        extent.maxy += mcsOffset;
                    }

                    //get a transform from layer coord sys to map coord sys
                    TransformCache* item = TransformCache::GetLayerToMapTransform(transformCache, vl->GetFeatureName(), featResId, dstCs, csFactory, svcFeature);
                    Ptr<MgCoordinateSystem> layerCs = item? item->GetCoordSys() : NULL;
                    MgCSTrans* xformer = item? item->GetTransform() : NULL;

                    //extract hyperlink and tooltip info
                    if (!vl->GetToolTip().empty()) legendInfo.hastooltips() = true;
                    if (!vl->GetUrl().empty()) legendInfo.hashyperlinks() = true;

                    //set up the property name mapping -- it tells us what
                    //string the viewer should be displaying as the name of each
                    //feature property
                    // TODO: check to see if name is FeatureClass or Extension name
                    RS_FeatureClassInfo fcinfo(vl->GetFeatureName());

                    MdfModel::NameStringPairCollection* pmappings = vl->GetPropertyMappings();

                    for (int j=0; j<pmappings->GetCount(); j++)
                    {
                        MdfModel::NameStringPair* m = pmappings->GetAt(j);
                        fcinfo.add_mapping(m->GetName(), m->GetValue());
                    }

                    //check for overridden feature query filter and remember it.
                    //we will use this when making feature queries
                    STRING overrideFilter = L"";

                    if (overrideFilters)
                        overrideFilter = overrideFilters->GetItem(i);

                    //here we will check if the layer has a composite polyline style.
                    //If so, we will make multiple feature queries and process
                    //the geometry once for each line style. This makes things look
                    //much better

                    MdfModel::FeatureTypeStyleCollection* ftsc = scaleRange->GetFeatureTypeStyles();
                    MdfModel::FeatureTypeStyle* fts = (ftsc->GetCount() > 0) ? ftsc->GetAt(0) : NULL;

                    //we can render polylines with composite styles using this method
                    //only if there is a single line style
                    if (fts && FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts) == FeatureTypeStyleVisitor::ftsLine)
                    {
                        MdfModel::RuleCollection* rules = fts->GetRules();

                        //temporary holder for rules
                        std::vector<MdfModel::LineSymbolizationCollection*> tmpRules;

                        int maxStrokes = 0;

                        //transfer all line styles for all rules into a temporary collection
                        //and away from the layer definition
                        for (int k=0; k<rules->GetCount(); k++)
                        {
                            MdfModel::LineRule* lr = (MdfModel::LineRule*)rules->GetAt(k);
                            MdfModel::LineSymbolizationCollection* syms = lr->GetSymbolizations();

                            //move composite line styles to a temporary collection away
                            //from the one in the layer definition
                            MdfModel::LineSymbolizationCollection* syms2 = new MdfModel::LineSymbolizationCollection();

                            while (syms->GetCount() > 0)
                                syms2->Adopt(syms->OrphanAt(0));

                            tmpRules.push_back(syms2);

                            maxStrokes = max(maxStrokes, syms2->GetCount());
                        }

                        //now for each separate line style -- run a feature query
                        //and stylization loop with that single style

                        //start the layer
                        dr->StartLayer(&legendInfo, &fcinfo);

                        //if there are no strokes, we still want
                        //to render so that labels draw even if
                        //we are not drawing the actual geometry
                        if (maxStrokes == 0)
                        {
                            RSMgFeatureReader* rdr = ExecuteFeatureQuery(svcFeature, extent, vl, overrideFilter.c_str(), dstCs, layerCs, item);
                            if (rdr)
                            {
                                ds->StylizeVectorLayer(vl, dr, rdr, xformer, NULL, NULL);
                            }
                            delete rdr;
                        }
                        else
                        {
                            for (int i=0; i<maxStrokes; i++)
                            {
                                //collection to store labels temporarily
                                //so that we add labels to each feature just once
                                std::vector<MdfModel::TextSymbol*> tmpLabels;

                                //transfer a single stroke from the temporary collection
                                //to the layer definition, for each rule
                                for (int m=0; m<rules->GetCount(); m++)
                                {
                                    MdfModel::LineRule* lr = (MdfModel::LineRule*)rules->GetAt(m);

                                    //remove labels if this is not the
                                    //first time we stylize the feature
                                    if (i)
                                    {
                                        tmpLabels.push_back(lr->GetLabel()->OrphanSymbol());
                                        lr->GetLabel()->AdoptSymbol(NULL);
                                    }

                                    MdfModel::LineSymbolizationCollection* syms = lr->GetSymbolizations();
                                    MdfModel::LineSymbolizationCollection* syms2 = tmpRules[m];
                                    syms->Adopt(syms2->GetAt(min(i, syms2->GetCount()-1)));
                                }

                                RSMgFeatureReader* rdr = ExecuteFeatureQuery(svcFeature, extent, vl, overrideFilter.c_str(), dstCs, layerCs, item);
                                if (rdr)
                                {
                                    ds->StylizeVectorLayer(vl, dr, rdr, xformer, NULL, NULL);
                                }
                                delete rdr;

                                //transfer line styles back to layer definition
                                for (int m=0; m<rules->GetCount(); m++)
                                {
                                    MdfModel::LineRule* lr = (MdfModel::LineRule*)rules->GetAt(m);

                                    //add label back if we remove it
                                    if (i)
                                    {
                                        lr->GetLabel()->AdoptSymbol(tmpLabels[m]);
                                    }

                                    MdfModel::LineSymbolizationCollection* syms = lr->GetSymbolizations();
                                    syms->OrphanAt(0);
                                }
                            }
                        }

                        //end the layer
                        dr->EndLayer();

                        //move composite line styles back to original
                        //layer definition collection so that it frees
                        //them up when we destroy it
                        for (int m=0; m<rules->GetCount(); m++)
                        {
                            MdfModel::LineRule* lr = (MdfModel::LineRule*)rules->GetAt(m);
                            MdfModel::LineSymbolizationCollection* syms = lr->GetSymbolizations();
                            MdfModel::LineSymbolizationCollection* syms2 = tmpRules[m];

                            while (syms2->GetCount() > 0)
                                syms->Adopt(syms2->OrphanAt(0));

                            delete syms2;
                        }
                    }
                    else
                    {
                        RSMgFeatureReader* rdr = ExecuteFeatureQuery(svcFeature, extent, vl, overrideFilter.c_str(), dstCs, layerCs, item);
                        if (rdr)
                        {
                            //stylize into output format
                            dr->StartLayer(&legendInfo, &fcinfo);
                            ds->StylizeVectorLayer(vl, dr, rdr, xformer, NULL, NULL);
                            dr->EndLayer();
                        }
                        delete rdr;
                    }
                }
                else
                {
                    #ifdef _DEBUG
                    printf("  StylizeLayers() **NOT Stylizing - NO SCALE RANGE** Name:%S\n", (mapLayer->GetName()).c_str());
                    #endif
                }

                #ifdef _DEBUG
                printf("  StylizeLayers() **LAYEREND** -Vector- Name:%S  Time:%6.4f (s)\n\n", (mapLayer->GetName()).c_str(), (GetTickCount()-dwLayerStart)/1000.0);
                #endif
            }
            else if (gl)
            {
                #ifdef _DEBUG
                long dwLayerStart = GetTickCount();
                #endif

                // make sure we have a valid scale range
                MdfModel::GridScaleRange* scaleRange = Stylizer::FindScaleRange(*gl->GetScaleRanges(), scale);

                if (scaleRange)
                {
                    //get feature source id
                    STRING sfeatResId = gl->GetResourceID();
                    Ptr<MgResourceIdentifier> featResId = new MgResourceIdentifier(sfeatResId);

                    // get the feature extent to use with the query
                    RS_Bounds extent = dr->GetBounds();

                    //now get the coordinate system of the layer data
                    // Feature Service caches these so we only take the performance hit on
                    // the 1st one that is not cached.

                    // Need to determine the name of the spatial context for this layer
                    MdfModel::MdfString featureName = gl->GetFeatureName();

                    //get a transform from layer coord sys to map coord sys
                    TransformCache* item = TransformCache::GetLayerToMapTransform(transformCache, gl->GetFeatureName(), featResId, dstCs, csFactory, svcFeature);
                    Ptr<MgCoordinateSystem> layerCs = item? item->GetCoordSys() : NULL;
                    MgCSTrans* xformer = item? item->GetTransform() : NULL;

                    //grid layer does not yet have hyperlink or tooltip
                    //extract hyperlink and tooltip info
                    //if (!gl->GetToolTip().empty()) legendInfo.hastooltips() = true;
                    //if (!gl->GetUrl().empty()) legendInfo.hashyperlinks() = true;

                    //set up the property name mapping -- it tells us what
                    //string the viewer should be displaying as the name of each
                    //feature property
                    // TODO: check to see if name is FeatureClass or Extension name
                    RS_FeatureClassInfo fcinfo(gl->GetFeatureName());

                    //GridLayer does not yet have property mappings
                    /*
                    MdfModel::NameStringPairCollection* pmappings = gl->GetPropertyMappings();

                    for (int j=0; j<pmappings->GetCount(); j++)
                    {
                        MdfModel::NameStringPair* m = pmappings->GetAt(j);
                        fcinfo.add_mapping(m->GetName(), m->GetValue());
                    }
                    */

                    //check for overridden feature query filter and remember it.
                    //we will use this when making feature queries
                    STRING overrideFilter = L"";

                    if (overrideFilters)
                        overrideFilter = overrideFilters->GetItem(i);

                    int width  = map->GetDisplayWidth();
                    int height = map->GetDisplayHeight();

                    if (width == 0 || height == 0)
                    {
                        //in case we are using DWF Viewer
                        //compute the size of the viewer screen -- since we don't know it directly
                        double pixelsPerMapUnit = dr->GetMetersPerUnit() / 0.0254 * dr->GetDpi() / dr->GetMapScale();
                        width = (int)(extent.width() * pixelsPerMapUnit);
                        height = (int)(extent.height() * pixelsPerMapUnit);
                    }

                    //perform the raster query
                    RSMgFeatureReader* rdr = ExecuteRasterQuery(svcFeature, extent, gl, overrideFilter.c_str(), dstCs, layerCs, width, height);
                    if (rdr)
                    {
                        //stylize into a dwf
                        dr->StartLayer(&legendInfo, &fcinfo);
                        ds->StylizeGridLayer(gl, dr, rdr, xformer, NULL, NULL);
                        dr->EndLayer();
                    }
                    delete rdr;
                }

                #ifdef _DEBUG
                printf("  StylizeLayers() **LAYEREND** -Grid- Name:%S  Time:%6.4f (s)\n\n", (mapLayer->GetName()).c_str(), (GetTickCount()-dwLayerStart)/1000.0);
                #endif
            }
            else if (dl) //drawing layer
            {
                #ifdef _DEBUG
                long dwLayerStart = GetTickCount();
                #endif

                // make sure we have a valid scale range
                if (scale >= dl->GetMinScale() && scale < dl->GetMaxScale())
                {
                    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(dl->GetResourceID());

                    //get the resource content to see if there is a coordinate system
                    Ptr<MgByteReader> rdr = svcResource->GetResourceContent(resId);
                    STRING st = rdr->ToString();

                    //now look for a coordinate space tag and extract the contents
                    size_t i0 = st.find(L"<CoordinateSpace>");
                    size_t i1 = st.find(L"</CoordinateSpace>");

                    TransformCache* cached = NULL;
                    MgCSTrans* xformer = NULL;

                    if (   i0 != STRING::npos
                        && i1 != STRING::npos)
                    {
                        i0 += wcslen(L"<CoordinateSpace>");
                        STRING cs = st.substr(i0, i1 - i0);

                        if (!cs.empty() && cs != dstCs->ToString())
                        {
                            // Create coordinate system transformer
                            TransformCacheMap::const_iterator iter = transformCache.find(cs);
                            if (transformCache.end() != iter) cached = (*iter).second;
                            if (NULL != cached)
                            {
                                xformer = cached->GetTransform();
                            }
                            else
                            {
                                Ptr<MgCoordinateSystem> srcCs = csFactory->Create(cs);
                                if (srcCs.p)
                                {
                                    xformer = new MgCSTrans(srcCs, dstCs);
                                    cached = new TransformCache(xformer, srcCs);
                                    transformCache[cs] = cached;
                                }
                            }
                        }
                    }

                    //get DWF from drawing service
                    Ptr<MgByteReader> reader = svcDrawing->GetSection(resId, dl->GetSheet());

                    RSMgInputStream is(reader);

                    ds->StylizeDrawingLayer(dl, dr, &legendInfo, &is, dl->GetLayerFilter(), xformer);
                }

                #ifdef _DEBUG
                printf("  StylizeLayers() **LAYEREND** -Drawing- Name:%S  Time = %6.4f (s)\n\n", (mapLayer->GetName()).c_str(), (GetTickCount()-dwLayerStart)/1000.0);
                #endif
            }

        MG_SERVER_MAPPING_SERVICE_CATCH(L"MgStylizationUtil.StylizeLayers");
        if (mgException.p)
        {
            // TODO: Eventually this should be used to indicate visually to the client what
            //       layer failed in addition to logging the error.
            MgServerManager* serverManager = MgServerManager::GetInstance();
            STRING locale = (NULL == serverManager) ?  MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();

            // Get the layer that failed
            MgStringCollection arguments;
            arguments.Add(mapLayer->GetName());

            // Get the reason why the layer failed to stylize
            MgStringCollection argumentsWhy;
            argumentsWhy.Add(mgException->GetMessage(locale));

            Ptr<MgStylizeLayerFailedException> exception;
            exception = new MgStylizeLayerFailedException(L"MgStylizationUtil.StylizeLayers", __LINE__, __WFILE__, &arguments, L"MgFormatInnerExceptionMessage", &argumentsWhy);

            STRING message = exception->GetMessage(locale);
            STRING stackTrace = exception->GetStackTrace(locale);
            MG_LOG_EXCEPTION_ENTRY(message.c_str(), stackTrace.c_str());

#ifdef _DEBUG
            STRING details = mgException->GetDetails(locale);

            wstring err = L"\n %t Error during stylization of layer ";
            err += mapLayer->GetName();
            err += L"\n";
            err += L"Details: ";
            err += details;
            err += L"\n";
            ACE_DEBUG( (LM_DEBUG, err.c_str()) );
#endif
        }

        if (ldf)
        {
            delete ldf;
            ldf = NULL;
        }
    }

    #ifdef _DEBUG
    printf("StylizeLayers() **MAPDONE** Layers:%d  Total Time:%6.4f (s)\n\n", layers->GetCount(), (GetTickCount()-dwStart)/1000.0);
    #endif

    TransformCache::Clear(transformCache);
}


// When rendering a tile, we need to compute the extent used to determine
// which features to render into it.  Features in adjacent tiles, but not
// in the current tile, will potentially draw in this tile due to their
// stylization.  Examples of this are thick polylines or point symbols.  In
// most cases the additional "size" of a feature due to its stylization is
// measured in device units.  The exception is a symbol whose size is defined
// in mapping units.
double MgStylizationUtil::ComputeStylizationOffset(MgMap* map,
                                                   MdfModel::VectorScaleRange* scaleRange,
                                                   double scale)
{
    if (scaleRange == NULL)
        return 0.0;

    MdfModel::FeatureTypeStyleCollection* styles = scaleRange->GetFeatureTypeStyles();
    if (styles == NULL || styles->GetCount() <= 0)
        return 0.0;

    double metersPerPixel = 0.0254 / map->GetDisplayDpi();
    double metersPerUnit = map->GetMetersPerUnit();

    // initialize the offset to one pixel - this handles the case where
    // a feature is right on a tile boundary
    double maxOffsetMCS = 1.0 * metersPerPixel * scale / metersPerUnit;

    bool bDone = false;
    bool bFoundExpression = false;
    bool bFoundLabel      = false;

    for (INT32 i=0; i<styles->GetCount() && !bDone; i++)
    {
        FeatureTypeStyle* fts = styles->GetAt(i);

        FeatureTypeStyleVisitor::eFeatureTypeStyle st = FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts);

        RuleCollection* rules = fts->GetRules();
        switch (st)
        {
            case FeatureTypeStyleVisitor::ftsLine:
            {
                int nRuleCount = rules->GetCount();
                for (INT32 j=0; j<nRuleCount && !bDone; j++)
                {
                    LineRule* rule = (LineRule*)rules->GetAt(j);
                    if (rule == NULL)
                        continue;

                    LineSymbolizationCollection* symbs = rule->GetSymbolizations();
                    int nSymbCount = symbs->GetCount();
                    for (INT32 k=0; k<nSymbCount && !bDone; k++)
                    {
                        LineSymbolization2D* lineSymb = symbs->GetAt(k);
                        if (lineSymb == NULL)
                            continue;

                        Stroke* stroke = lineSymb->GetStroke();
                        if (stroke == NULL)
                            continue;

                        double width;
                        if (ParseDouble(stroke->GetThickness(), width))
                        {
                            // constant expression - update the offset
                            width = MdfModel::LengthConverter::UnitToMeters(stroke->GetUnit(), width);
                            maxOffsetMCS = max(width * scale / metersPerUnit, maxOffsetMCS);
                        }
                        else
                        {
                            // non-constant expression - break out of the loop
                            bFoundExpression = true;
                            bDone = true;
                            continue;
                        }
                    }

                    Label* label = rule->GetLabel();
                    if (label != NULL)
                    {
                        // found a label - break out of the loop
                        bFoundLabel = true;
                        bDone = true;
                        continue;
                    }
                }

                break;
            }

            case FeatureTypeStyleVisitor::ftsArea:
            {
                int nRuleCount = rules->GetCount();
                for (INT32 j=0; j<nRuleCount && !bDone; j++)
                {
                    AreaRule* rule = (MdfModel::AreaRule*)rules->GetAt(j);
                    if (rule == NULL)
                        continue;

                    AreaSymbolization2D* areaSymb = rule->GetSymbolization();
                    if (areaSymb != NULL)
                    {
                        Stroke* stroke = areaSymb->GetEdge();
                        if (stroke != NULL)
                        {
                            double width;
                            if (ParseDouble(stroke->GetThickness(), width))
                            {
                                // constant expression - update the offset
                                width = MdfModel::LengthConverter::UnitToMeters(stroke->GetUnit(), width);
                                maxOffsetMCS = max(width * scale / metersPerUnit, maxOffsetMCS);
                            }
                            else
                            {
                                // non-constant expression - break out of the loop
                                bFoundExpression = true;
                                bDone = true;
                                continue;
                            }
                        }
                    }

                    Label* label = rule->GetLabel();
                    if (label != NULL)
                    {
                        // found a label - break out of the loop
                        bFoundLabel = true;
                        bDone = true;
                        continue;
                    }
                }

                break;
            }

            case FeatureTypeStyleVisitor::ftsPoint:
            {
                int nRuleCount = rules->GetCount();
                for (INT32 j=0; j<nRuleCount && !bDone; j++)
                {
                    PointRule* rule = (PointRule*)rules->GetAt(j);
                    if (rule == NULL)
                        continue;

                    MdfModel::PointSymbolization2D* pointSymb = rule->GetSymbolization();
                    if (pointSymb != NULL)
                    {
                        // TODO: handle other types of symbols
                        MdfModel::W2DSymbol* marker = (MdfModel::W2DSymbol*)pointSymb->GetSymbol();
                        if (marker != NULL)
                        {
                            double width;
                            if (ParseDouble(marker->GetSizeX(), width))
                            {
                                // constant expression - update the offset
                                width = MdfModel::LengthConverter::UnitToMeters(marker->GetUnit(), width);
                                if (marker->GetSizeContext() == MdfModel::DeviceUnits)
                                    width *= scale / metersPerUnit;
                                else
                                    width /= metersPerUnit;
                            }
                            else
                            {
                                // non-constant expression - break out of the loop
                                bFoundExpression = true;
                                bDone = true;
                                continue;
                            }

                            double height;
                            if (ParseDouble(marker->GetSizeY(), height))
                            {
                                // constant expression - update the offset
                                height = MdfModel::LengthConverter::UnitToMeters(marker->GetUnit(), height);
                                if (marker->GetSizeContext() == MdfModel::DeviceUnits)
                                    height *= scale / metersPerUnit;
                                else
                                    height /= metersPerUnit;
                            }
                            else
                            {
                                // non-constant expression - break out of the loop
                                bFoundExpression = true;
                                bDone = true;
                                continue;
                            }

                            // use the max of the width and height - this takes care of any
                            // rotation as well
                            maxOffsetMCS = max(0.5 * width, maxOffsetMCS);
                            maxOffsetMCS = max(0.5 * height, maxOffsetMCS);
                        }
                    }

                    Label* label = rule->GetLabel();
                    if (label != NULL)
                    {
                        // found a label - break out of the loop
                        bFoundLabel = true;
                        bDone = true;
                        continue;
                    }
                }

                break;
            }

            case FeatureTypeStyleVisitor::ftsComposite:
            {
                // Composite symbolization has tons of properties that can affect
                // how much offset is required, and putting all the checks here to
                // compute the minimum required offset is a huge task.  The safest
                // thing to do is to just use the maximum allowable offset.
                bFoundExpression = true;
                bDone = true;
                break;
            }
        }
    }

    // If we encountered a non-constant expression, then (unfortunately) we have
    // no way of accurately knowing what the offset needs to be.  The same applies
    // to labels.  In this case, make the offset the max allowable value specified
    // via the TileExtentOffset configuration property.
    if (bFoundExpression || bFoundLabel)
    {
        double tileExtentOffset = 0.0;
        MgConfiguration* pConf = MgConfiguration::GetInstance();
        pConf->GetDoubleValue(MgConfigProperties::RenderingServicePropertiesSection,
                              MgConfigProperties::RenderingServicePropertyTileExtentOffset,
                              tileExtentOffset,
                              MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset);
        if (tileExtentOffset < 0.0)
            tileExtentOffset = MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset;

        INT32 maxTileDimension = max(MgTileParameters::tileWidth, MgTileParameters::tileHeight);
        INT32 offsetPixels = (INT32)ceil(maxTileDimension * tileExtentOffset);

        double unitsPerPixel = metersPerPixel * scale / metersPerUnit;
        maxOffsetMCS = max(offsetPixels * unitsPerPixel, maxOffsetMCS);
    }

    return maxOffsetMCS;
}

double MgStylizationUtil::ParseDouble(CREFSTRING valstr)
{
    const wchar_t* sd = valstr.c_str();

    double d = 0.0;

    //simply try to parse as constant value
    int status = swscanf(sd, L"%lf", &d);

    //if (status == 1)
    //{
    //}

    return d;
}

// Returns true if the expression evaluates to a constant value.
bool MgStylizationUtil::ParseDouble(CREFSTRING valstr, double& res)
{
    const wchar_t* sd = valstr.c_str();

    double d = 0.0;
    int status = swscanf(sd, L"%lf", &d);

    if (status == 1)
    {
        res = d;
        return true;
    }

    return false;
}

//draws a given feature type style into an image
MgByteReader* MgStylizationUtil::DrawFTS(MgResourceService* svcResource,
                                         MdfModel::FeatureTypeStyle* fts,
                                         INT32 imgWidth,
                                         INT32 imgHeight,
                                         INT32 themeCategory,
                                         double scale)
{
    MG_SERVER_MAPPING_SERVICE_TRY()

    if (fts)
    {
        int type = FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts);

        //TODO: should be color key magenta to work around Internet Explorer PNG bug
        RS_Color bgcolor(255,255,255,255);

        //get the right renderer going
        GDRenderer er(imgWidth, imgHeight, bgcolor, false);

        //and also set up a symbol library for it
        RSMgSymbolManager sman(svcResource);
        er.SetSymbolManager(&sman);

        double pixelW = imgWidth;
        double pixelH = imgHeight;

        RS_Bounds b(0.0, 0.0, pixelW, pixelH);

        RS_MapUIInfo info(L"", L"name", L"guid", L"", L"", RS_Color(255,255,255,0));

        double pixelsPerInch = 96.0;
        double metersPerPixel = 0.0254 / pixelsPerInch;

        er.StartMap(&info,
                    b,
                    1.0,
                    pixelsPerInch,
                    metersPerPixel);

        er.StartLayer(NULL, NULL);

        MG_SERVER_MAPPING_SERVICE_TRY()

        switch (type)
        {
        case FeatureTypeStyleVisitor::ftsComposite:
            {
                MdfModel::CompositeTypeStyle* cts = (MdfModel::CompositeTypeStyle*)fts;
                MdfModel::RuleCollection* crc = cts->GetRules();

                if (crc)
                {
                    //case caller asked for one and only category
                    //or category index is bad
                    if ((themeCategory < 0 || themeCategory >= crc->GetCount()) && crc->GetCount() == 1)
                        themeCategory = 0;

                    if (themeCategory >= 0 && themeCategory <= crc->GetCount())
                    {
                        //get correct theme rule
                        MdfModel::CompositeRule* rule = (MdfModel::CompositeRule*)crc->GetAt(themeCategory);
                        MdfModel::CompositeSymbolization* csym = rule->GetSymbolization();

                        SEMgSymbolManager sman(svcResource);
                        er.DrawStylePreview(csym, &sman);
                    }
                    else
                    {
                        //Error
                        //TODO: throw?
                    }
                }
            }
            break;
        case FeatureTypeStyleVisitor::ftsArea:
            {
                MdfModel::AreaTypeStyle* ats = (MdfModel::AreaTypeStyle*)fts;
                MdfModel::RuleCollection* arc = ats->GetRules();

                if (arc)
                {
                    //case caller asked for one and only category
                    //or category index is bad
                    if ((themeCategory < 0 || themeCategory >= arc->GetCount()) && arc->GetCount() == 1)
                        themeCategory = 0;

                    if (themeCategory >= 0 && themeCategory <= arc->GetCount())
                    {
                        //get correct theme rule
                        MdfModel::AreaRule* rule = (MdfModel::AreaRule*)arc->GetAt(themeCategory);

                        //convert fill style to RS_FillStyle
                        MdfModel::AreaSymbolization2D* asym = rule->GetSymbolization();
                        RS_FillStyle fs;
                        int linePixelWidth = 0;
                        if (asym)
                        {
                            if (asym->GetFill())
                            {
                                ParseColor(asym->GetFill()->GetBackgroundColor(), fs.background());

                                ParseColor(asym->GetFill()->GetForegroundColor(), fs.color());

                                fs.pattern() = asym->GetFill()->GetFillPattern();
                            }
                            else
                            {
                                fs.color() = RS_Color(0,0,0,0);
                                fs.background() = RS_Color(0,0,0,0);
                            }

                            MdfModel::Stroke* edgeStroke = asym->GetEdge();
                            if (edgeStroke)
                            {
                                ParseColor(edgeStroke->GetColor(), fs.outline().color());
                                fs.outline().style() = edgeStroke->GetLineStyle();

                                double width = ParseDouble(edgeStroke->GetThickness());
                                width = MdfModel::LengthConverter::UnitToMeters(edgeStroke->GetUnit(), width);
                                if (width > 0.0)
                                {
                                    if (edgeStroke->GetSizeContext() == MdfModel::MappingUnits)
                                    {
                                        //for mapping space edges with non-zero width, always use a
                                        //width of two pixels
                                        width = 2.0 * metersPerPixel;
                                    }
                                    else if (width > (0.5*min(pixelH, pixelW) - 2.0) * metersPerPixel)
                                    {
                                        //for lines in device coords, ensure that the line width
                                        //still allows a 4 pixel square of fill color to be displayed
                                        width = (0.5*min(pixelH, pixelW) - 2.0) * metersPerPixel;
                                    }
                                }
                                linePixelWidth = (int)(width / metersPerPixel);
                                fs.outline().width() = width;
                                fs.outline().units() = (edgeStroke->GetSizeContext() == MdfModel::DeviceUnits)? RS_Units_Device : RS_Units_Model;
                            }
                            else
                                fs.outline().color() = RS_Color(0,0,0,0);
                        }

                        //lines with zero width are rendered one pixel wide
                        if (linePixelWidth == 0)
                        {
                            linePixelWidth = 1;
                        }

                        //create a rectangle that allows the line width to be
                        //included within the legend image, and also make it
                        //slightly smaller than the map extent to avoid having
                        //missing pixels at the edges
                        double offset = linePixelWidth / 2 + 0.000001;

                        LineBuffer lb(5);
                        lb.MoveTo(       offset,        offset);
                        lb.LineTo(pixelW-offset,        offset);
                        lb.LineTo(pixelW-offset, pixelH-offset);
                        lb.LineTo(       offset, pixelH-offset);
                        lb.Close();

                        er.ProcessPolygon(&lb, fs);
                    }
                    else
                    {
                        //Error
                        //TODO: throw?
                    }
                }
            }
            break;
        case FeatureTypeStyleVisitor::ftsLine:
            {
                MdfModel::LineTypeStyle* lts = (MdfModel::LineTypeStyle*) fts;
                MdfModel::RuleCollection* lrc = lts->GetRules();

                if (lrc)
                {
                    //case caller asked for one and only category
                    //or category index is bad
                    if ((themeCategory < 0 || themeCategory >= lrc->GetCount()) && lrc->GetCount() == 1)
                        themeCategory = 0;

                    if (themeCategory >= 0 && themeCategory <= lrc->GetCount())
                    {
                        MdfModel::LineRule* lr = (MdfModel::LineRule*)lrc->GetAt(themeCategory);

                        MdfModel::LineSymbolizationCollection* lsc = lr->GetSymbolizations();

                        //determine the maximum line width used by this FTS
                        double maxLineWidth = GetMaxMappingSpaceLineWidth(fts, themeCategory);
                        for (int j=0; j<lsc->GetCount(); j++)
                        {
                            MdfModel::LineSymbolization2D* lsym = lsc->GetAt(j);
                            MdfModel::Stroke* stroke = lsym->GetStroke();

                            RS_LineStroke ls;

                            if (stroke)
                            {
                                ParseColor(stroke->GetColor(), ls.color());

                                ls.style() = stroke->GetLineStyle();

                                double width = ParseDouble(stroke->GetThickness());
                                width = MdfModel::LengthConverter::UnitToMeters(stroke->GetUnit(), width);
                                if (width > 0.0)
                                {
                                    if (stroke->GetSizeContext() == MdfModel::MappingUnits)
                                    {
                                        //for line widths in mapping units, scale so the widest line
                                        //is half the height of the legend image
                                        width = width / maxLineWidth * 0.5 * pixelH * metersPerPixel;
                                    }
                                    else if (width > pixelH * metersPerPixel)
                                    {
                                        //for lines in device coords, ensure that the line is not wider
                                        //than the height of the legend image. This is a performance
                                        //optimization and does not affect the appearance of the image
                                        width = pixelH * metersPerPixel;
                                    }
                                }
                                ls.width() = width;
                                ls.units() = (stroke->GetSizeContext() == MdfModel::DeviceUnits)? RS_Units_Device : RS_Units_Model;
                            }

                            //generate a geometry to draw - make it slightly smaller than
                            //the map extent to avoid having missing pixels at the edges
                            LineBuffer lb(2);
                            lb.MoveTo(       0.000001, 0.5*pixelH);
                            lb.LineTo(pixelW-0.000001, 0.5*pixelH);

                            er.ProcessPolyline(&lb, ls);
                        }
                    }
                    else
                    {
                        //Error
                        //TODO: throw?
                    }
                }
            }
            break;
        case FeatureTypeStyleVisitor::ftsPoint:
            {
                MdfModel::PointTypeStyle* pts = (MdfModel::PointTypeStyle*) fts;
                MdfModel::RuleCollection* prc = pts->GetRules();

                if (prc)
                {
                    //case caller asked for one and only category
                    //or category index is bad
                    if ((themeCategory < 0 || themeCategory >= prc->GetCount()) && prc->GetCount() == 1)
                        themeCategory = 0;

                    if (themeCategory >= 0 && themeCategory < prc->GetCount())
                    {
                        MdfModel::PointRule* pr = (MdfModel::PointRule*)prc->GetAt(themeCategory);

                        MdfModel::PointSymbolization2D* ps = pr->GetSymbolization();

                        RS_MarkerDef mdef;

                        //just pick a symbol size in meters that
                        //will mostly fit the whole image
                        double sz = (min(pixelW, pixelH) - 2.0) * metersPerPixel;
                        mdef.width() = sz;
                        mdef.height() = sz;
                        mdef.units() = RS_Units_Model;

                        if (ps)
                        {
                            MdfModel::Symbol* symbol = ps->GetSymbol();

                            if (symbol)
                            {
                                SymbolVisitor::eSymbolType type = SymbolVisitor::DetermineSymbolType(ps->GetSymbol());

                                switch (type)
                                {
                                case SymbolVisitor::stW2D:
                                    {
                                        MdfModel::W2DSymbol* w2dsym = (MdfModel::W2DSymbol*)symbol;

                                        mdef.name() = w2dsym->GetSymbolName();
                                        mdef.library() = w2dsym->GetSymbolLibrary();

                                        //override colors
                                        ParseColor(w2dsym->GetAreaColor(), mdef.style().color());
                                        ParseColor(w2dsym->GetLineColor(), mdef.style().outline().color());
                                        ParseColor(w2dsym->GetTextColor(), mdef.style().background()); //text color stored in background
                                    }
                                    break;
                                case SymbolVisitor::stMark:
                                    {
                                        MdfModel::MarkSymbol* marksym = (MdfModel::MarkSymbol*)symbol;

                                        MdfModel::MarkSymbol::Shape shape = marksym->GetShape();

                                        switch (shape)
                                        {
                                        case MdfModel::MarkSymbol::Square :   mdef.name() = SLD_SQUARE_NAME;    break;
                                        case MdfModel::MarkSymbol::Circle :   mdef.name() = SLD_CIRCLE_NAME;    break;
                                        case MdfModel::MarkSymbol::Cross :    mdef.name() = SLD_CROSS_NAME;     break;
                                        case MdfModel::MarkSymbol::Star :     mdef.name() = SLD_STAR_NAME;      break;
                                        case MdfModel::MarkSymbol::Triangle : mdef.name() = SLD_TRIANGLE_NAME;  break;
                                        case MdfModel::MarkSymbol::X :        mdef.name() = SLD_X_NAME;         break;
                                        default: break;
                                        }

                                        //override colors
                                        if (marksym->GetFill())
                                        {
                                            ParseColor(marksym->GetFill()->GetForegroundColor(), mdef.style().color());
                                            ParseColor(marksym->GetFill()->GetBackgroundColor(), mdef.style().background());
                                        }
                                        else
                                        {
                                            mdef.style().color() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
                                            mdef.style().background() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
                                        }

                                        if (marksym->GetEdge())
                                        {
                                            ParseColor(marksym->GetEdge()->GetColor(), mdef.style().outline().color());
                                        }
                                        else
                                        {
                                            mdef.style().outline().color() = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
                                        }
                                    }
                                    break;
                                case SymbolVisitor::stFont:
                                    {
                                        MdfModel::FontSymbol* fontSym = (MdfModel::FontSymbol*)symbol;

                                        //store the font name as the library string
                                        mdef.library() = fontSym->GetFontName();

                                        //store the marker character as the symbol name
                                        mdef.name() = (wchar_t)fontSym->GetCharacter();

                                        RS_FontStyle_Mask style = RS_FontStyle_Regular;

                                        if (_wcsnicmp(L"true", fontSym->GetBold().c_str(), 4) == 0)
                                            style = (RS_FontStyle_Mask)(style | RS_FontStyle_Bold);
                                        if (_wcsnicmp(L"true", fontSym->GetItalic().c_str(), 4) == 0)
                                            style = (RS_FontStyle_Mask)(style | RS_FontStyle_Italic);
                                        if (_wcsnicmp(L"true", fontSym->GetUnderlined().c_str(), 4) == 0)
                                            style = (RS_FontStyle_Mask)(style | RS_FontStyle_Underline);

                                        mdef.fontstyle() = style;

                                        //color
                                        ParseColor(fontSym->GetForegroundColor(), mdef.style().color());
                                    }
                                //TODO: other types of symbols
                                default:
                                    break;
                                }
                            }
                        }

                        //generate a geometry to draw
                        LineBuffer lb(2);
                        lb.MoveTo(0.5*pixelW, 0.5*pixelH);

                        er.ProcessMarker(&lb, mdef, true);
                    }
                    else
                    {
                        //Error
                        //TODO: throw?
                    }
                }
            }
            break;
        default:
            break;
        }

        //we don't want to crash out of GetMap just because the legend bitmap failed
        //for whatever reason
        MG_SERVER_MAPPING_SERVICE_CATCH(L"MgStylizationUtil.DrawFTS")

        er.EndLayer();
        er.EndMap();

        RS_String format = L"PNG"; //TODO: use user specified format
        auto_ptr<RS_ByteData> data;

        data.reset(er.Save(format, imgWidth, imgHeight));

        if (NULL != data.get())
        {
            // put this into a byte source
            Ptr<MgByteSource> bs = new MgByteSource(data->GetBytes(), data->GetNumBytes());
            bs->SetMimeType(MgMimeType::Png);

            return bs->GetReader();
        }
    }

    //we don't want to crash out of GetMap just because the legend bitmap failed
    //for whatever reason
    MG_SERVER_MAPPING_SERVICE_CATCH(L"MgStylizationUtil.DrawFTS")

    return NULL;
}

//Determine the maximum line width contained in the specified feature type style
double MgStylizationUtil::GetMaxMappingSpaceLineWidth(MdfModel::FeatureTypeStyle* fts, INT32 themeCategory)
{
    double maxLineWidth = 0.0;

    MG_SERVER_MAPPING_SERVICE_TRY()

    if (fts)
    {
        switch (FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts))
        {
            case FeatureTypeStyleVisitor::ftsArea:
            {
                MdfModel::AreaTypeStyle* ats = (MdfModel::AreaTypeStyle*)fts;
                MdfModel::RuleCollection* arc = ats->GetRules();
                if (arc)
                {
                    //case caller asked for one and only category
                    //or category index is bad
                    if ((themeCategory < 0 || themeCategory >= arc->GetCount()) && arc->GetCount() == 1)
                    {
                        themeCategory = 0;
                    }

                    if (themeCategory >= 0 && themeCategory <= arc->GetCount())
                    {
                        //get correct theme rule
                        MdfModel::AreaRule* rule = (MdfModel::AreaRule*)arc->GetAt(themeCategory);

                        //convert fill style to RS_FillStyle
                        MdfModel::AreaSymbolization2D* asym = rule->GetSymbolization();
                        RS_FillStyle fs;

                        if (asym)
                        {
                            MdfModel::Stroke* edgeStroke = asym->GetEdge();
                            if (edgeStroke && edgeStroke->GetSizeContext() == MdfModel::MappingUnits)
                            {
                                double width = ParseDouble(edgeStroke->GetThickness());
                                width = MdfModel::LengthConverter::UnitToMeters(edgeStroke->GetUnit(), width);
                                if (width > maxLineWidth)
                                {
                                    maxLineWidth = width;
                                }
                            }
                        }
                    }
                }
                break;
            }
            case FeatureTypeStyleVisitor::ftsLine:
            {
                MdfModel::LineTypeStyle* lts = (MdfModel::LineTypeStyle*) fts;
                MdfModel::RuleCollection* lrc = lts->GetRules();

                if (lrc)
                {
                    //case caller asked for one and only category
                    //or category index is bad
                    if ((themeCategory < 0 || themeCategory >= lrc->GetCount()) && lrc->GetCount() == 1)
                    {
                        themeCategory = 0;
                    }

                    if (themeCategory >= 0 && themeCategory <= lrc->GetCount())
                    {
                        MdfModel::LineRule* lr = (MdfModel::LineRule*)lrc->GetAt(themeCategory);
                        MdfModel::LineSymbolizationCollection* lsc = lr->GetSymbolizations();

                        for (int j=0; j<lsc->GetCount(); j++)
                        {
                            MdfModel::LineSymbolization2D* lsym = lsc->GetAt(j);
                            MdfModel::Stroke* stroke = lsym->GetStroke();
                            if (stroke && stroke->GetSizeContext() == MdfModel::MappingUnits)
                            {
                                double width = ParseDouble(stroke->GetThickness());
                                width = MdfModel::LengthConverter::UnitToMeters(stroke->GetUnit(), width);
                                if (width > maxLineWidth)
                                {
                                    maxLineWidth = width;
                                }
                            }
                        }
                    }
                }
                break;
            }
        }
    }

    //we don't want to crash out of GetMap just because the legend bitmap failed
    //for whatever reason
    MG_SERVER_MAPPING_SERVICE_CATCH(L"MgServerMappingService.GetMaxMappingSpaceLineWidth")

    return maxLineWidth;
}

void MgStylizationUtilExceptionTrap(FdoException* except, int line, wchar_t* file)
{
    MG_TRY()

    STRING messageId;
    MgStringCollection arguments;
    wchar_t* buf = (wchar_t*)except->GetExceptionMessage();

    if (NULL != buf)
    {
        messageId = L"MgFormatInnerExceptionMessage";
        arguments.Add(buf);
    }

    mgException = new MgFdoException(L"MgStylizationUtil.ExceptionTrap", line, file, NULL, messageId, &arguments);

    MG_CATCH(L"MgStylizationUtil.ExceptionTrap")

    MgServerManager* serverManager = MgServerManager::GetInstance();
    STRING locale = (NULL == serverManager) ?
        MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();
    STRING message = mgException->GetMessage(locale);
    STRING details = mgException->GetDetails(locale);
    STRING stackTrace = mgException->GetStackTrace(locale);

    ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), details.c_str()));
    MG_LOG_WARNING_ENTRY(MgServiceType::MappingService, message.c_str(), stackTrace.c_str());
}


void MgStylizationUtil::InitializeStylizerCallback()
{
    SetStylizerExceptionCallback(&MgStylizationUtilExceptionTrap);
}


