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

#include "MappingUtil.h"

#include "SAX2Parser.h"
#include "Bounds.h"
#include "Renderer.h"
#include "AGGRenderer.h"
#include "Stylizer.h"
#include "SymbolVisitor.h"
#include "SLDSymbols.h"
#include "SE_StyleVisitor.h"
#include "SEMgSymbolManager.h"
#include "TransformCache.h"
#include "StylizationUtil.h"

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


MdfModel::MapDefinition* MgMappingUtil::GetMapDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId)
{
    //get and parse the mapdef
    Ptr<MgByteReader> mdfReader = svcResource->GetResourceContent(resId, L"");

    Ptr<MgByteSink> sink = new MgByteSink(mdfReader);
    Ptr<MgByte> bytes = sink->ToBuffer();
    assert(bytes->GetLength() > 0);

    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
    if (!parser.GetSucceeded())
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);
        throw new MgInvalidMapDefinitionException(L"MgMappingUtil::GetMapDefinition", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // detach the map definition from the parser - it's
    // now the caller's responsibility to delete it
    MdfModel::MapDefinition* mdef = parser.DetachMapDefinition();
    assert(mdef != NULL);

    return mdef;
}


MdfModel::LayerDefinition* MgMappingUtil::GetLayerDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId)
{
    //get and parse the mapdef
    Ptr<MgByteReader> ldfReader = svcResource->GetResourceContent(resId, L"");

    Ptr<MgByteSink> sink = new MgByteSink(ldfReader);
    Ptr<MgByte> bytes = sink->ToBuffer();
    assert(bytes->GetLength() > 0);

    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
    if (!parser.GetSucceeded())
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);
        throw new MgInvalidLayerDefinitionException(L"MgMappingUtil::GetLayerDefinition", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // detach the feature layer definition from the parser - it's
    // now the caller's responsibility to delete it
    MdfModel::LayerDefinition* ldef = parser.DetachLayerDefinition();
    assert(ldef != NULL);

    return ldef;
}


RSMgFeatureReader* MgMappingUtil::ExecuteFeatureQuery(MgFeatureService* svcFeature,
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
    Ptr<MgResourceIdentifier> featResId = new MgResourceIdentifier(sfeatResId);

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
            Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
            trans = csFactory->GetTransform(mapCs, layerCs);
            trans->IgnoreDatumShiftWarning(true);
            trans->IgnoreOutsideDomainWarning(true);
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
        layerExt = cache->GetEnvelope();

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
                cache->SetEnvelope(layerExt);
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
    catch (MgException* e)
    {
        e->Release();

        try
        {
            if (!geom.empty())
                options->SetSpatialFilter(geom, poly, MgFeatureSpatialOperations::Intersects);

            // TODO: could it be an extension name and not a FeatureClassName?
            rdr = svcFeature->SelectFeatures(featResId, vl->GetFeatureName(), options);
        }
        catch (MgException* e)
        {
            // Feature selection failed.  Re-throw the exception so that it can be written to the logs.
            throw e;
        }
    }

#ifdef _DEBUG
    printf("  ExecuteFeatureQuery() total time = %6.4f (s)\n", (GetTickCount()-dwStart)/1000.0);
#endif

    return new RSMgFeatureReader(rdr.p, svcFeature, featResId.p, options, vl->GetGeometry());
}


RSMgFeatureReader* MgMappingUtil::ExecuteRasterQuery(MgFeatureService* svcFeature,
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
        Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
        trans = csFactory->GetTransform(mapCs, layerCs);
        trans->IgnoreDatumShiftWarning(true);
        trans->IgnoreOutsideDomainWarning(true);
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

        options->SetSpatialFilter(geom, poly, MgFeatureSpatialOperations::Intersects);

        if (geom == L"Raster")
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
    catch (MgException* e)
    {
        e->Release();

        //finally try without a filter
        // TODO: could it be an extension name and not a FeatureClassName?
        options = NULL;
        rdr = svcFeature->SelectFeatures(featResId, gl->GetFeatureName(), options);
    }

    return new RSMgFeatureReader(rdr.p, svcFeature, featResId.p, options, L"clipped_raster");
}


void MgMappingUtil::StylizeLayers(MgResourceService* svcResource,
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
        auto_ptr<MdfModel::LayerDefinition> ldf;
        RSMgFeatureReader* rsReader = NULL;

        Ptr<MgLayerBase> mapLayer = layers->GetItem(i);

        #ifdef _DEBUG
        printf("  StylizeLayers() **LAYERSTART** Name:%S  VAS:%S\n", (mapLayer->GetName()).c_str(), mapLayer->IsVisibleAtScale(scale)? L"True" : L"False");
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
            ldf.reset(GetLayerDefinition(svcResource, layerid));

            Ptr<MgLayerGroup> group = mapLayer->GetGroup();

            MgLogDetail logDetail(MgServiceType::MappingService, MgLogDetail::InternalTrace, L"MgMappingUtil.StylizeLayers", mgStackParams);
            logDetail.AddString(L"Map",map->GetName());
            logDetail.AddResourceIdentifier(L"LayerId",layerid);
            logDetail.Create();

			//base map layers are not editable
            bool bEditable = true;
            if (mapLayer->GetLayerType() == MgLayerType::BaseMap)
                bEditable = false;

            //layer legend and ui specific information
            RS_UIGraphic uig(NULL, 0, mapLayer->GetLegendLabel());
            RS_LayerUIInfo layerInfo( mapLayer->GetName(),
                                      mapLayer->GetObjectId(),
                                      mapLayer->GetSelectable(),
                                      mapLayer->GetVisible(),
                                      bEditable,
                                      (group.p)? group->GetName() : L"",
                                      (group.p)? group->GetObjectId() : L"",
                                      mapLayer->GetDisplayInLegend(),
                                      mapLayer->GetExpandInLegend(),
                                     -mapLayer->GetDisplayOrder(),
                                      uig);

            MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
            MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf.get());
            MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf.get());

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
                    if (!vl->GetToolTip().empty()) layerInfo.hastooltips() = true;
                    if (!vl->GetUrl().empty()) layerInfo.hashyperlinks() = true;

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

                    // check for overridden feature query filter and remember it.
                    // we will use this when making feature queries
                    STRING overrideFilter = L"";
                    if (overrideFilters)
                        overrideFilter = overrideFilters->GetItem(i);

                    // create the reader we'll use
                    rsReader = ExecuteFeatureQuery(svcFeature, extent, vl, overrideFilter.c_str(), dstCs, layerCs, item);
                    FdoPtr<FdoIFeatureReader> fdoReader = (NULL == rsReader) ? NULL : rsReader->GetInternalReader();

                    if (NULL != fdoReader.p)
                    {
                        // stylize into output format
                        dr->StartLayer(&layerInfo, &fcinfo);
                        ds->StylizeVectorLayer(vl, dr, rsReader, xformer, scale, NULL, NULL);
                        dr->EndLayer();
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
                // TODO: FDO RFP - Make FdoPtr's reference counter thread-safe.
                static ACE_Recursive_Thread_Mutex sg_fdoRfpMutex;

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
                    TransformCache* item = NULL;
                    {
                        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sg_fdoRfpMutex));
                        item = TransformCache::GetLayerToMapTransform(transformCache, gl->GetFeatureName(), featResId, dstCs, csFactory, svcFeature);
                    }

                    Ptr<MgCoordinateSystem> layerCs = item? item->GetCoordSys() : NULL;
                    MgCSTrans* xformer = item? item->GetTransform() : NULL;

                    // Test if layer and map are using different coordinate systems
                    if (NULL != layerCs.p && NULL != dstCs && layerCs->GetCode() != dstCs->GetCode())
                    {
                            // Source and destination coord sys are different,
                            // i.e. the raster image and the map are not using the same coordinate system.
                            STRING whyMsg = layerCs->GetCode();
                            MgStringCollection arguments;
                            arguments.Add(whyMsg);
                            whyMsg = mapLayer->GetName();
                            arguments.Add(whyMsg);
                            whyMsg = dstCs->GetCode();
                            arguments.Add(whyMsg);
                            throw new MgRasterTransformationNotSupportedException(L"MgMappingUtil.StylizeLayers", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemTransformationMismatch", &arguments);
                    }

                    //grid layer does not yet have hyperlink or tooltip
                    //extract hyperlink and tooltip info
//                  if (!gl->GetToolTip().empty()) layerInfo.hastooltips() = true;
//                  if (!gl->GetUrl().empty()) layerInfo.hashyperlinks() = true;

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
                        double pixelsPerMapUnit = dr->GetMetersPerUnit() / METERS_PER_INCH * dr->GetDpi() / dr->GetMapScale();
                        width = (int)(extent.width() * pixelsPerMapUnit);
                        height = (int)(extent.height() * pixelsPerMapUnit);
                    }

                    //perform the raster query
                    {
                        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sg_fdoRfpMutex));
                        rsReader = ExecuteRasterQuery(svcFeature, extent, gl, overrideFilter.c_str(), dstCs, layerCs, width, height);
                    }

                    if (NULL != rsReader)
                    {
                        //stylize into a dwf
                        dr->StartLayer(&layerInfo, &fcinfo);
                        ds->StylizeGridLayer(gl, dr, rsReader, xformer, scale, NULL, NULL);
                        dr->EndLayer();
                    }
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

                    if (i0 != STRING::npos && i1 != STRING::npos)
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

                    dr->StartLayer(&layerInfo, NULL);
                    ds->StylizeDrawingLayer(dl, dr, &is, xformer, scale);
                    dr->EndLayer();
                }

                #ifdef _DEBUG
                printf("  StylizeLayers() **LAYEREND** -Drawing- Name:%S  Time = %6.4f (s)\n\n", (mapLayer->GetName()).c_str(), (GetTickCount()-dwLayerStart)/1000.0);
                #endif
            }

        MG_SERVER_MAPPING_SERVICE_CATCH(L"MgMappingUtil.StylizeLayers");

        delete rsReader;

        if (mgException.p)
        {
            // TODO: Eventually this should be used to indicate visually to the client what
            //       layer failed in addition to logging the error.
            MgServerManager* serverManager = MgServerManager::GetInstance();
            STRING locale = (NULL == serverManager)? MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();

            // Get the layer that failed
            MgStringCollection arguments;
            arguments.Add(mapLayer->GetName());

            // Get the reason why the layer failed to stylize
            MgStringCollection argumentsWhy;
            argumentsWhy.Add(mgException->GetMessage(locale));

            Ptr<MgStylizeLayerFailedException> exception;
            exception = new MgStylizeLayerFailedException(L"MgMappingUtil.StylizeLayers", __LINE__, __WFILE__, &arguments, L"MgFormatInnerExceptionMessage", &argumentsWhy);

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
double MgMappingUtil::ComputeStylizationOffset(MgMap* map,
                                               MdfModel::VectorScaleRange* scaleRange,
                                               double scale)
{
    if (scaleRange == NULL)
        return 0.0;

    MdfModel::FeatureTypeStyleCollection* styles = scaleRange->GetFeatureTypeStyles();
    if (styles == NULL || styles->GetCount() <= 0)
        return 0.0;

    double metersPerPixel = METERS_PER_INCH / map->GetDisplayDpi();
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
                        if (StylizationUtil::ParseDouble(stroke->GetThickness(), width))
                        {
                            // constant expression - update the offset
                            width = MdfModel::LengthConverter::UnitToMeters(stroke->GetUnit(), width);
                            maxOffsetMCS = rs_max(width * scale / metersPerUnit, maxOffsetMCS);
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
                            if (StylizationUtil::ParseDouble(stroke->GetThickness(), width))
                            {
                                // constant expression - update the offset
                                width = MdfModel::LengthConverter::UnitToMeters(stroke->GetUnit(), width);
                                maxOffsetMCS = rs_max(width * scale / metersPerUnit, maxOffsetMCS);
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
                            if (StylizationUtil::ParseDouble(marker->GetSizeX(), width))
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
                            if (StylizationUtil::ParseDouble(marker->GetSizeY(), height))
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
                            maxOffsetMCS = rs_max(0.5 * width, maxOffsetMCS);
                            maxOffsetMCS = rs_max(0.5 * height, maxOffsetMCS);
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

        INT32 maxTileDimension = rs_max(MgTileParameters::tileWidth, MgTileParameters::tileHeight);
        INT32 offsetPixels = (INT32)ceil(maxTileDimension * tileExtentOffset);

        double unitsPerPixel = metersPerPixel * scale / metersPerUnit;
        maxOffsetMCS = rs_max(offsetPixels * unitsPerPixel, maxOffsetMCS);
    }

    return maxOffsetMCS;
}


// draws a given feature type style into an image
MgByteReader* MgMappingUtil::DrawFTS(MgResourceService* svcResource,
                                     MdfModel::FeatureTypeStyle* fts,
                                     INT32 imgWidth,
                                     INT32 imgHeight,
                                     INT32 themeCategory)
{
    if (!fts)
        return NULL;

    RS_Color bgcolor(255, 255, 255, 255);

    // get the right renderer going
    AGGRenderer er(imgWidth, imgHeight, bgcolor, false, false, 0.0);

    // and also set up symbol managers for it
    SEMgSymbolManager se_sman(svcResource);
    RSMgSymbolManager rs_sman(svcResource);
    er.SetSymbolManager(&rs_sman);

    // draw the preview
    StylizationUtil::DrawStylePreview(imgWidth, imgHeight, themeCategory, fts, &er, &se_sman);

    // TODO: use user-specified format
    RS_String format = L"PNG";

    auto_ptr<RS_ByteData> data;
    data.reset(er.Save(format, imgWidth, imgHeight));

    if (NULL != data.get())
    {
        // put this into a byte source
        Ptr<MgByteSource> bs = new MgByteSource(data->GetBytes(), data->GetNumBytes());
        bs->SetMimeType(MgMimeType::Png);

        return bs->GetReader();
    }

    return NULL;
}

void MgMappingUtilExceptionTrap(FdoException* except, int line, wchar_t* file)
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

    MG_LOG_WARNING_ENTRY(MgServiceType::MappingService, message.c_str(), stackTrace.c_str());
}


void MgMappingUtil::InitializeStylizerCallback()
{
    SetStylizerExceptionCallback(&MgMappingUtilExceptionTrap);
}

