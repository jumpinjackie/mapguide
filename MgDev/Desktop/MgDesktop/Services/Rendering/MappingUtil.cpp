#include "MappingUtil.h"
#include "MappingDefs.h"

#include "AGGRenderer.h"
#include "Bounds.h"
#include "DefaultStylizer.h"
#include "Fdo.h"
#include "Renderer.h"
#include "SAX2Parser.h"
#include "Services/Rendering/RSMgInputStream.h"
#include "Services/Rendering/RSMgFeatureReader.h"
#include "Services/Rendering/RSMgSymbolManager.h"
#include "Services/Stylization/SEMgSymbolManager.h"
#include "SE_StyleVisitor.h"
#include "SLDSymbols.h"
#include "StylizationUtil.h"
#include "Stylizer.h"
#include "SymbolVisitor.h"
#include "SymbolDefinition.h"
#include "Services/Feature/TransformCache.h"

#include <algorithm>

//For logging
//#include "ServerManager.h"
//#include "LogManager.h"
//#include "LogDetail.h"

#ifndef _WIN32
#define _wcsnicmp wcsncasecmp

extern long GetTickCount();
#endif

///////////////////////////////////////////////////////////////////////////////
MdfModel::MapDefinition* MgdMappingUtil::GetMapDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId)
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
        throw new MgInvalidMapDefinitionException(L"MgdMappingUtil::GetMapDefinition", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // detach the map definition from the parser - it's
    // now the caller's responsibility to delete it
    MdfModel::MapDefinition* mdef = parser.DetachMapDefinition();
    assert(mdef != NULL);

    return mdef;
}


///////////////////////////////////////////////////////////////////////////////
RSMgdFeatureReader* MgdMappingUtil::ExecuteFeatureQuery(MgFeatureService* svcFeature,
                                                      RS_Bounds& extent,
                                                      MdfModel::VectorLayerDefinition* vl,
                                                      const wchar_t* overrideFilter,
                                                      MgCoordinateSystem* mapCs,
                                                      MgCoordinateSystem* layerCs,
                                                      MgdTransformCache* cache)
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
        // Try an intersections operation if we have geometry
        if (!geom.empty())
        {
            e->Release();
            options->SetSpatialFilter(geom, poly, MgFeatureSpatialOperations::Intersects);

            // TODO: could it be an extension name and not a FeatureClassName?
            rdr = svcFeature->SelectFeatures(featResId, vl->GetFeatureName(), options);
        }
        else
        {
            // rethrow if no geometry
            throw e;
        }
    }

#ifdef _DEBUG
    ACE_DEBUG((LM_INFO, L"(%t)  ExecuteFeatureQuery() total time = %6.4f (s)\n", (GetTickCount()-dwStart)/1000.0));
#endif

    return new RSMgdFeatureReader(rdr.p, svcFeature, featResId.p, options, vl->GetGeometry());
}


///////////////////////////////////////////////////////////////////////////////
RSMgdFeatureReader* MgdMappingUtil::ExecuteRasterQuery(MgFeatureService* svcFeature,
                                                     RS_Bounds& extent,
                                                     MdfModel::GridLayerDefinition* gl,
                                                     const wchar_t* overrideFilter,
                                                     MgCoordinateSystem* mapCs,
                                                     MgCoordinateSystem* layerCs,
                                                     int devWidth,
                                                     int devHeight)
{
    //we want to transform from mapping space to layer space
    Ptr<MgCoordinateSystemTransform> map2layerTransform;

    //bounds in mapping space
    Ptr<MgCoordinate> ll = new MgCoordinateXY(extent.minx, extent.miny);
    Ptr<MgCoordinate> ur = new MgCoordinateXY(extent.maxx, extent.maxy);
    Ptr<MgEnvelope> mapExt = new MgEnvelope(ll, ur);
    Ptr<MgEnvelope> rasterExt;
    if (NULL != layerCs && NULL != mapCs && layerCs->GetCsCode() != mapCs->GetCsCode())
    {
        Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();

        map2layerTransform = csFactory->GetTransform(mapCs, layerCs);
        map2layerTransform->IgnoreDatumShiftWarning(true);
        map2layerTransform->IgnoreOutsideDomainWarning(true);

        rasterExt = map2layerTransform->Transform(mapExt);
    }
    else
    {
        rasterExt = mapExt;
    }

    //get the name of the raster property
    MdfModel::MdfString geom = gl->GetGeometry();
    Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();

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

        Ptr<MgCoordinate> rastll = rasterExt->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> rastur = rasterExt->GetUpperRightCoordinate();

        //Set up RESAMPLE command
        FdoPtr<FdoExpressionCollection> funcParams = FdoExpressionCollection::Create();
        FdoPtr<FdoIdentifier> rasterProp = FdoIdentifier::Create(geom.c_str());
        funcParams->Add(rasterProp);
        FdoPtr<FdoDataValue> minX = FdoDataValue::Create(rastll->GetX(), FdoDataType_Double);
        funcParams->Add(minX);
        FdoPtr<FdoDataValue> minY = FdoDataValue::Create(rastll->GetY(), FdoDataType_Double);
        funcParams->Add(minY);
        FdoPtr<FdoDataValue> maxX = FdoDataValue::Create(rastur->GetX(), FdoDataType_Double);
        funcParams->Add(maxX);
        FdoPtr<FdoDataValue> maxY = FdoDataValue::Create(rastur->GetY(), FdoDataType_Double);
        funcParams->Add(maxY);
        FdoPtr<FdoDataValue> height = FdoDataValue::Create(devHeight);
        funcParams->Add(height);
        FdoPtr<FdoDataValue> width = FdoDataValue::Create(devWidth);
        funcParams->Add(width);

        FdoPtr<FdoFunction> clipFunc = FdoFunction::Create(L"RESAMPLE", funcParams);
        STRING func = clipFunc->ToString();
        options->AddComputedProperty(L"clipped_raster", func);

        Ptr<MgPolygon> poly = GetPolygonFromEnvelope(rasterExt);
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

    //get feature source id
    STRING sfeatResId = gl->GetResourceID();
    Ptr<MgResourceIdentifier> featResId = new MgResourceIdentifier(sfeatResId);

    Ptr<MgFeatureReader> rdr;

    // TODO: could it be an extension name and not a FeatureClassName?
    rdr = svcFeature->SelectFeatures(featResId, gl->GetFeatureName(), options);

    return new RSMgdFeatureReader(rdr.p, svcFeature, featResId.p, options, L"clipped_raster");
}


///////////////////////////////////////////////////////////////////////////////
// This is called by the MgServerRenderingService::RenderMapInternal to
// render the layers.
void MgdMappingUtil::StylizeLayers(MgResourceService* svcResource,
                                  MgFeatureService* svcFeature,
                                  MgdDrawingService* svcDrawing,
                                  MgCoordinateSystemFactory* csFactory,
                                  MgdMap* map,
                                  MgReadOnlyLayerCollection* layers,
                                  MgStringCollection* overrideFilters,
                                  Stylizer* ds,
                                  Renderer* dr,
                                  MgCoordinateSystem* dstCs,
                                  bool expandExtents,
                                  bool checkRefreshFlag,
                                  double scale,
                                  bool selection,
                                  bool extractColors,
                                  ProfileRenderLayersResultBase* pPRLsResult,
								  CancelStylization cancel)
{
    #ifdef _DEBUG
    long dwStart = GetTickCount();
    ACE_DEBUG((LM_INFO, L"(%t)StylizeLayers() **MAPSTART** Layers:%d  Scale:%f\n", layers->GetCount(), scale));
    #endif

    // Cache coordinate system transforms for the life of the
    // stylization operation.
    MgdTransformCacheMap transformCache;

    // Get the layers' resource content in a single request by adding them to a collection
    for (int i = layers->GetCount()-1; i >= 0; i--)
    {
        auto_ptr<MdfModel::LayerDefinition> ldf;
        RSMgdFeatureReader* rsReader = NULL;

        Ptr<MgLayerBase> mapLayer = layers->GetItem(i);

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


        #ifdef _DEBUG
        long dwLayerStart = GetTickCount();
        ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **LAYERSTART** Name:%W  VAS:%W\n", i, (mapLayer->GetName()).c_str(), mapLayer->IsVisibleAtScale(scale)? L"True" : L"False"));
        #endif



        MG_SERVER_MAPPING_SERVICE_TRY()

            // Just profile visible layers?
            MgdTransformCache* TCForProfile = NULL;
            double minScale_Profile = 0.0;
            double maxScale_Profile = MdfModel::VectorScaleRange::MAX_MAP_SCALE;
            if(NULL != pPRLsResult)
            {
                ProfileRenderLayerResult* pPRLResult = new ProfileRenderLayerResult(); // points points to Profile Render Layers Result
               
                // Set the start time of stylizing layer
                pPRLResult->SetRenderTime(MgdTimerUtil::GetTime());

                ProfileRenderLayerResultCollection* pPRLResultColl = pPRLsResult->GetProfileRenderLayerResults();
                pPRLResultColl->Adopt(pPRLResult);
            }

            //get layer definition
            Ptr<MgResourceIdentifier> layerid = mapLayer->GetLayerDefinition();
            ldf.reset(MgLayerBase::GetLayerDefinition(svcResource, layerid));

            Ptr<MgLayerGroup> group = mapLayer->GetGroup();
			
            MgdLogDetail logDetail(MgServiceType::MappingService, MgdLogDetail::InternalTrace, L"MgdMappingUtil.StylizeLayers", mgStackParams);
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
                                      layerid->ToString(),
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

            if (vl) //############################################################################ vector layer
            {
                // Tweaking this logic for #1981:
                //
                // We first try to find a valid scale range, only when we can't find a valid scale range do we
                // coerce the first one to [0 - Infinity]

                MdfModel::VectorScaleRangeCollection* scaleRanges = vl->GetScaleRanges();
                MdfModel::VectorScaleRange* scaleRange = NULL;
                if (scaleRanges)
                {
                    // make sure we have a valid scale range
                    scaleRange = Stylizer::FindScaleRange(*scaleRanges, scale);
                }

                // We have a selection but no applicable scale range
                //
                // Modify the layer scale range to also support infinite
                // Need to apply default style as one will not be defined
                if (selection && !scaleRange)
                {
                    if (scaleRanges)
                    {
                        scaleRange = scaleRanges->GetAt(0);
                        if (scaleRange)
                        {
                            scaleRange->SetMinScale(0.0);
                            scaleRange->SetMaxScale(MdfModel::VectorScaleRange::MAX_MAP_SCALE);
                        }
                    }
                }

                if (scaleRange)
                {
                    minScale_Profile = scaleRange->GetMinScale();
                    maxScale_Profile = scaleRange->GetMaxScale();

                    #ifdef _DEBUG
                    ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **Stylizing** Name:%W\n", i, (mapLayer->GetName()).c_str()));
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

                    #ifdef _DEBUG
                    ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **Stylizing** Name:%W  Extents:%f,%f to %f,%f Expanded:%d\n", i, (mapLayer->GetName()).c_str(), extent.minx, extent.miny, extent.maxx, extent.maxy, expandExtents));
                    #endif

                    //get a transform from layer coord sys to map coord sys
                    MgdTransformCache* item = MgdTransformCache::GetLayerToMapTransform(transformCache, vl->GetFeatureName(), featResId, dstCs, csFactory, svcFeature);
                    TCForProfile = item;
                    Ptr<MgCoordinateSystem> layerCs = item? item->GetCoordSys() : NULL;
                    MgdCSTrans* xformer = item? item->GetTransform() : NULL;

                    //extract hyperlink and tooltip info
                    if (!vl->GetToolTip().empty()) layerInfo.hastooltips() = true;
                    if (vl->GetUrlData() && !vl->GetUrlData()->GetUrlContent().empty())
                    {
                        layerInfo.hashyperlinks() = true;
                    }

                    //set up the property name mapping -- it tells us what
                    //string the viewer should be displaying as the name of each
                    //feature property
                    // TODO: check to see if name is FeatureClass or Extension name
                    RS_FeatureClassInfo fcinfo(vl->GetFeatureName(), vl->GetResourceID());

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

                    #ifdef _DEBUG
                    ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **Stylizing** Name:%W  Override Filter(size=%d):\n%W\n", i, (mapLayer->GetName()).c_str(), overrideFilter.length(), overrideFilter.empty() ? L"(Empty)" : overrideFilter.c_str()));
                    #endif

                    // create the reader we'll use
                    rsReader = ExecuteFeatureQuery(svcFeature, extent, vl, overrideFilter.c_str(), dstCs, layerCs, item);
                    // create an automatic FdoPtr around the reader
                    FdoPtr<FdoIFeatureReader> fdoReader = (NULL == rsReader) ? NULL : rsReader->GetInternalReader();

                    #ifdef _DEBUG
                    ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **Stylizing** Name:%W  Query Done.\n", i, (mapLayer->GetName()).c_str()));
                    #endif

                    if (NULL != fdoReader.p)
                    {
                        // stylize into output format
                        dr->StartLayer(&layerInfo, &fcinfo);
                        ds->StylizeVectorLayer(vl, dr, rsReader, xformer, scale, cancel, NULL);
                        dr->EndLayer();

                        // color extraction for RFC60 only when needed
                        if (extractColors)
                        {
                            #ifdef _DEBUG
                            ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) //ExtractColors// -Vector- Name:%W  Time:%6.4f (s)\n", i, (mapLayer->GetName()).c_str(), (GetTickCount()-dwLayerStart)/1000.0));
                            #endif
                            ExtractColors(map, scaleRange, ds);
                            #ifdef _DEBUG
                            ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) ##ExtractColors## -Vector- Name:%W  Time:%6.4f (s)\n", i, (mapLayer->GetName()).c_str(), (GetTickCount()-dwLayerStart)/1000.0));
                            #endif
                        }
                    }
                }
                else  // not scaleRange
                {
                    #ifdef _DEBUG
                    ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **NOT Stylizing - NO SCALE RANGE** Name:%W\n", i, (mapLayer->GetName()).c_str()));
                    #endif
                }

                #ifdef _DEBUG
                ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **LAYEREND** -Vector- Name:%W  Time:%6.4f (s)\n", i, (mapLayer->GetName()).c_str(), (GetTickCount()-dwLayerStart)/1000.0));
                #endif
            }
            else if (gl) //############################################################################ grid layer
            {
                // TODO: FDO RFP - Make FdoPtr's reference counter thread-safe.
                static ACE_Recursive_Thread_Mutex sg_fdoRfpMgdMutex;

                // make sure we have a valid scale range
                MdfModel::GridScaleRange* scaleRange = Stylizer::FindScaleRange(*gl->GetScaleRanges(), scale);

                if (scaleRange)
                {
                    minScale_Profile = scaleRange->GetMinScale();
                    maxScale_Profile = scaleRange->GetMaxScale();

                    //get feature source id
                    STRING sfeatResId = gl->GetResourceID();
                    Ptr<MgResourceIdentifier> featResId = new MgResourceIdentifier(sfeatResId);

                    // get the feature extent to use with the query
                    RS_Bounds extent = dr->GetBounds();

                    #ifdef _DEBUG
                    ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **Stylizing** Name:%W  Extents:%f,%f to %f,%f\n", i, (mapLayer->GetName()).c_str(), extent.minx, extent.miny, extent.maxx, extent.maxy));
                    #endif

                    //now get the coordinate system of the layer data
                    // Feature Service caches these so we only take the performance hit on
                    // the 1st one that is not cached.

                    // Need to determine the name of the spatial context for this layer
                    MdfModel::MdfString featureName = gl->GetFeatureName();

                    //get a transform from layer coord sys to map coord sys
                    MgdTransformCache* item = NULL;
                    {
                        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sg_fdoRfpMgdMutex));
                        item = MgdTransformCache::GetLayerToMapTransform(transformCache, gl->GetFeatureName(), featResId, dstCs, csFactory, svcFeature);
                    }
                    TCForProfile = item;

                    Ptr<MgCoordinateSystem> layerCs = item? item->GetCoordSys() : NULL;
                    MgdCSTrans* xformer = item? item->GetTransform() : NULL;

                    // Test if layer and map are using the same coordinate systems
                    if (NULL == layerCs.p || NULL == dstCs || layerCs->GetCsCode() == dstCs->GetCsCode())
                    {
                        // No transform required
                        xformer = NULL;
                    }

                    //set up the property name mapping -- it tells us what
                    //string the viewer should be displaying as the name of each
                    //feature property
                    // TODO: check to see if name is FeatureClass or Extension name
                    RS_FeatureClassInfo fcinfo(gl->GetFeatureName(), gl->GetResourceID());

                    //check for overridden feature query filter and remember it.
                    //we will use this when making feature queries
                    STRING overrideFilter = L"";
                    if (overrideFilters)
                        overrideFilter = overrideFilters->GetItem(i);

                    // Clip request bounds to limits of spatial context for layer
                    // Some WMS/WFS servers fail if request bounds are out of range.
                    // On error, ignore the exception and use the original extent.
                    MG_TRY()
                    Ptr<MgSpatialContextReader> contextReader = svcFeature->GetSpatialContexts(featResId, false);
                    STRING layerWkt = (NULL == layerCs.p) ? L"" : layerCs->ToString();
                    while (contextReader.p != NULL && contextReader->ReadNext())
                    {
                        // Try to find wkt for feature's coordinate system
                        STRING code = contextReader->GetCoordinateSystem();
                        STRING wkt = contextReader->GetCoordinateSystemWkt();
                        if (wkt.empty() && !code.empty())
                        {
                            MgCoordinateSystemFactory factory;
                            wkt = factory.ConvertCoordinateSystemCodeToWkt(code);
                        }

                        // If the source data does not have a coordinate system, or it is the same
                        // as the layer then we are probably using the right spatial context
                        if (wkt.empty() || wkt == layerWkt)
                        {
                            // If the extent is known, clip the request to it.
                            Ptr<MgByteReader> agfExtent = contextReader->GetExtent();
                            MgAgfReaderWriter agfReader;
                            Ptr<MgGeometry> geom = agfReader.Read(agfExtent);
                            if (geom != NULL)
                            {
                                Ptr<MgEnvelope> envelope = geom->Envelope();
                                Ptr<MgCoordinate> ll = envelope->GetLowerLeftCoordinate();
                                Ptr<MgCoordinate> ur = envelope->GetUpperRightCoordinate();
                                double llx = ll->GetX();
                                double lly = ll->GetY();
                                double urx = ur->GetX();
                                double ury = ur->GetY();

                                #ifdef _DEBUG
                                ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **Stylizing** Name:%W  Extents(SpatialContext):%f,%f to %f,%f\n", i, (mapLayer->GetName()).c_str(), llx, lly, urx, ury));
                                #endif

                                if (NULL != xformer)
                                    xformer->TransformExtent(llx, lly, urx, ury);
                                RS_Bounds layerExtent(llx, lly, urx, ury);
                                RS_Bounds clippedExtent = RS_Bounds::Intersect(extent, layerExtent);
                                if (clippedExtent.IsValid())
                                {
                                    extent = clippedExtent;
                                }
                            }
                            break;
                        }
                    }
                    if(contextReader.p != NULL)
                        contextReader->Close();

                    MG_CATCH_AND_RELEASE()

                    #ifdef _DEBUG
                    ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **Stylizing** Name:%W  Extents(Using):%f,%f to %f,%f\n", i, (mapLayer->GetName()).c_str(), extent.minx, extent.miny, extent.maxx, extent.maxy));
                    #endif

                    double pixelsPerMapUnit = dr->GetMetersPerUnit() / METERS_PER_INCH * dr->GetDpi() / dr->GetMapScale();
                    int width = (int)(extent.width() * pixelsPerMapUnit + 0.5);
                    int height = (int)(extent.height() * pixelsPerMapUnit + 0.5);

                    //perform the raster query
                    FdoPtr<FdoIFeatureReader> fdoReader;
                    {
                        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sg_fdoRfpMgdMutex));
                        rsReader = ExecuteRasterQuery(svcFeature, extent, gl, overrideFilter.c_str(), dstCs, layerCs, width, height);
                        fdoReader = (NULL == rsReader) ? NULL : rsReader->GetInternalReader();
                    }

                    if (NULL != fdoReader.p)
                    {
                        //stylize grid layer
                        dr->StartLayer(&layerInfo, &fcinfo);
                        ds->StylizeGridLayer(gl, dr, rsReader, xformer, scale, cancel, NULL);
                        dr->EndLayer();
                    }
                }

                #ifdef _DEBUG
                ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **LAYEREND** -Grid- Name:%W  Time:%6.4f (s)\n", i, (mapLayer->GetName()).c_str(), (GetTickCount()-dwLayerStart)/1000.0));
                #endif
            }
            else if (dl) //############################################################################ drawing layer
            {
                minScale_Profile = dl->GetMinScale();
                maxScale_Profile = dl->GetMaxScale();

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

                    MgdTransformCache* cached = NULL;
                    MgdCSTrans* xformer = NULL;

                    if (i0 != STRING::npos && i1 != STRING::npos)
                    {
                        i0 += wcslen(L"<CoordinateSpace>");
                        STRING cs = st.substr(i0, i1 - i0);

                        if (!cs.empty() && cs != dstCs->ToString())
                        {
                            // Create coordinate system transformer
                            MgdTransformCacheMap::const_iterator iter = transformCache.find(cs);
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
                                    xformer = new MgdCSTrans(srcCs, dstCs);
                                    cached = new MgdTransformCache(xformer, srcCs);
                                    transformCache[cs] = cached;
                                }
                            }
                        }
                    }
                    TCForProfile = cached;

                    //get DWF from drawing service
                    Ptr<MgByteReader> reader = svcDrawing->GetSection(resId, dl->GetSheet());

                    RSMgdInputStream is(reader);

                    dr->StartLayer(&layerInfo, NULL);
                    ds->StylizeDrawingLayer(dl, dr, &is, xformer, scale);
                    dr->EndLayer();
                }

                #ifdef _DEBUG
                ACE_DEBUG((LM_INFO, L"(%t)  StylizeLayers(%d) **LAYEREND** -Drawing- Name:%W  Time = %6.4f (s)\n", i, (mapLayer->GetName()).c_str(), (GetTickCount()-dwLayerStart)/1000.0));
                #endif
            } // end layer switch

            if(NULL != pPRLsResult)
            {
                ProfileRenderLayerResultCollection* pPRLResultColl = pPRLsResult->GetProfileRenderLayerResults();
                
                // Get current ProfileRenderLayerResult
                ProfileRenderLayerResult* pPRLResult = pPRLResultColl->GetAt(pPRLResultColl->GetCount()-1); //TODO: check index
                
                // Calculate the time spent on stylizing layer
                double stylizeLayerTime = MgdTimerUtil::GetTime() - pPRLResult->GetRenderTime();
                pPRLResult->SetRenderTime(stylizeLayerTime);

                pPRLResult->SetResourceId(layerid->ToString());
                pPRLResult->SetLayerName(mapLayer->GetName());

                if(vl)
                {
                    pPRLResult->SetLayerType(L"Vector Layer"); //NOXLATE?
                }
                else if(dl)
                {
                    pPRLResult->SetLayerType(L"Drawing Layer"); //NOXLATE?
                }
                else if(gl)
                {
                    pPRLResult->SetLayerType(L"Grid Layer"); //NOXLATE?
                }
                else
                {
                    pPRLResult->SetLayerType(L"Unknown Type"); //NOXLATE?
                }
                
                pPRLResult->SetFeatureClassName(mapLayer->GetFeatureClassName());

                STRING layerCsCode = L"";
                if(NULL != TCForProfile)
                {
                    Ptr<MgCoordinateSystem> layerCS = TCForProfile->GetCoordSys();
                    layerCsCode = layerCS->GetCsCode();
                }
                pPRLResult->SetCoordinateSystem(layerCsCode);

                ScaleRange* pScaleRange = new ScaleRange();
                pScaleRange->SetMinScale(minScale_Profile);
                pScaleRange->SetMaxScale(maxScale_Profile);
                pPRLResult->AdoptScaleRange(pScaleRange);

                STRING filter = L""; // NOXLATE
                if (overrideFilters)
                {
                    filter = overrideFilters->GetItem(i);
                }
                pPRLResult->SetFilter(filter.empty()? mapLayer->GetFilter() : filter);
            }

        MG_SERVER_MAPPING_SERVICE_CATCH(L"MgdMappingUtil.StylizeLayers");

        delete rsReader;

        if (mgException.p)
        {
            // TODO: Eventually this should be used to indicate visually to the client what
            //       layer failed in addition to logging the error.
            //MgServerManager* serverManager = MgServerManager::GetInstance();
            //STRING locale = (NULL == serverManager)? MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();
			STRING locale = MgResources::DefaultMessageLocale;

            // Get the layer that failed
            MgStringCollection arguments;
            arguments.Add(mapLayer->GetName());

            // Get the reason why the layer failed to stylize
            MgStringCollection argumentsWhy;
            argumentsWhy.Add(mgException->GetExceptionMessage(locale));

            Ptr<MgdStylizeLayerFailedException> exception;
            exception = new MgdStylizeLayerFailedException(L"MgdMappingUtil.StylizeLayers", __LINE__, __WFILE__, &arguments, L"MgFormatInnerExceptionMessage", &argumentsWhy);

            STRING message = exception->GetExceptionMessage(locale);
            STRING stackTrace = exception->GetStackTrace(locale);
            
            MG_LOG_EXCEPTION_ENTRY(message.c_str(), stackTrace.c_str());

#if defined(_DEBUG) || defined(_DEBUG_PNG8)
            STRING details = mgException->GetDetails(locale);

            wstring err = L"\n %t Error during stylization of layer ";
            err += mapLayer->GetName();
            err += L"\n";
            err += L"Details: ";
            err += details;
            err += L"\n";
            ACE_DEBUG( (LM_DEBUG, err.c_str()) );
#endif
            // TODO could throw here depending on a serverconfig setting (RFC64)
//          throw exception;


            if(NULL != pPRLsResult)
            {
                ProfileRenderLayerResultCollection* pPRLResultColl = pPRLsResult->GetProfileRenderLayerResults();
                
                // Get current ProfileRenderLayerResult
                ProfileRenderLayerResult* pPRLResult = pPRLResultColl->GetAt(pPRLResultColl->GetCount()-1); //TODO: check index
                
                // Calculate the time spent on stylizing layer
                double stylizeLayerTime = MgdTimerUtil::GetTime() - pPRLResult->GetRenderTime();
                pPRLResult->SetRenderTime(stylizeLayerTime);

                Ptr<MgResourceIdentifier> layerid = mapLayer->GetLayerDefinition();
                pPRLResult->SetResourceId(layerid->ToString());
                pPRLResult->SetLayerName(mapLayer->GetName());

                pPRLResult->SetError(message);
            }
        } // if exception
    } // for all layers


    #ifdef _DEBUG
    ACE_DEBUG((LM_INFO, L"(%t)StylizeLayers() **MAPDONE** Layers:%d  Total Time:%6.4f (s)\n\n", layers->GetCount(), (GetTickCount()-dwStart)/1000.0));
    #endif

    MgdTransformCache::Clear(transformCache);
}


///////////////////////////////////////////////////////////////////////////////
// When rendering a tile, we need to compute the extent used to determine
// which features to render into it.  Features in adjacent tiles, but not
// in the current tile, will potentially draw in this tile due to their
// stylization.  Examples of this are thick polylines or point symbols.  In
// most cases the additional "size" of a feature due to its stylization is
// measured in device units.  The exception is a symbol whose size is defined
// in mapping units.
double MgdMappingUtil::ComputeStylizationOffset(MgdMap* map,
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
                            if (stroke->GetSizeContext() == MdfModel::DeviceUnits)
                                width *= scale / metersPerUnit;
                            else
                                width /= metersPerUnit;

                            maxOffsetMCS = rs_max(width, maxOffsetMCS);
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
                                if (stroke->GetSizeContext() == MdfModel::DeviceUnits)
                                    width *= scale / metersPerUnit;
                                else
                                    width /= metersPerUnit;

                                maxOffsetMCS = rs_max(width, maxOffsetMCS);
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
        pConf->GetDoubleValue(MgdConfigProperties::RenderingServicePropertiesSection,
                              MgdConfigProperties::RenderingServicePropertyTileExtentOffset,
                              tileExtentOffset,
                              MgdConfigProperties::DefaultRenderingServicePropertyTileExtentOffset);
        if (tileExtentOffset < 0.0)
            tileExtentOffset = MgdConfigProperties::DefaultRenderingServicePropertyTileExtentOffset;

        INT32 maxTileDimension = rs_max(MgdTileParameters::tileWidth, MgdTileParameters::tileHeight);
        INT32 offsetPixels = (INT32)ceil(maxTileDimension * tileExtentOffset);

        double unitsPerPixel = metersPerPixel * scale / metersPerUnit;
        maxOffsetMCS = rs_max(offsetPixels * unitsPerPixel, maxOffsetMCS);
    }

    return maxOffsetMCS;
}


///////////////////////////////////////////////////////////////////////////////
// draws a given feature type style into an image
MgByteReader* MgdMappingUtil::DrawFTS(MgResourceService* svcResource,
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
    SEMgdSymbolManager se_sman(svcResource);
    RSMgdSymbolManager rs_sman(svcResource);
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


///////////////////////////////////////////////////////////////////////////////
// transforms a given extent and returns the new extent in the new cs
MgEnvelope* MgdMappingUtil::TransformExtent(MgEnvelope* extent, MgCoordinateSystemTransform* xform)
{
    Ptr<MgEnvelope> xformedExt = xform->Transform(extent);
    Ptr<MgPolygon> ccPoly = GetPolygonFromEnvelope(xformedExt);

    return ccPoly->Envelope();
}


///////////////////////////////////////////////////////////////////////////////
// returns an MgPolygon from a given envelope
MgPolygon* MgdMappingUtil::GetPolygonFromEnvelope(MgEnvelope* env)
{
    Ptr<MgCoordinate> ll = env->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = env->GetUpperRightCoordinate();

    Ptr<MgCoordinateXY> c1 = new MgCoordinateXY(ll->GetX(), ll->GetY());
    Ptr<MgCoordinateXY> c2 = new MgCoordinateXY(ur->GetX(), ll->GetY());
    Ptr<MgCoordinateXY> c3 = new MgCoordinateXY(ur->GetX(), ur->GetY());
    Ptr<MgCoordinateXY> c4 = new MgCoordinateXY(ll->GetX(), ur->GetY());
    Ptr<MgCoordinateXY> c5 = new MgCoordinateXY(ll->GetX(), ll->GetY());

    Ptr<MgCoordinateCollection> cc = new MgCoordinateCollection();
    cc->Add(c1); cc->Add(c2); cc->Add(c3); cc->Add(c4); cc->Add(c5);

    Ptr<MgLinearRing> outer = new MgLinearRing(cc);
    return new MgPolygon(outer, NULL);
}


///////////////////////////////////////////////////////////////////////////////
void MgdMappingUtilExceptionTrap(FdoException* except, int line, wchar_t* file)
{
    MG_TRY()

    STRING messageId;
    MgStringCollection arguments;
    wchar_t* buf = (wchar_t*)except->GetExceptionMessage();
    INT64 nativeErrorCode = except->GetNativeErrorCode();

    if (NULL != buf)
    {
        messageId = L"MgFormatInnerExceptionMessage";
        arguments.Add(buf);
    }

    mgException = new MgFdoException(L"MgStylizationUtil.ExceptionTrap", line, file, NULL, messageId, &arguments, nativeErrorCode);

    MG_CATCH(L"MgStylizationUtil.ExceptionTrap")

    STRING locale = MgResources::DefaultMessageLocale;
    STRING message = mgException->GetExceptionMessage(locale);
    STRING details = mgException->GetDetails(locale);
    STRING stackTrace = mgException->GetStackTrace(locale);

    //MG_LOG_WARNING_ENTRY(MgServiceType::MappingService, message.c_str(), stackTrace.c_str());
}


///////////////////////////////////////////////////////////////////////////////
void MgdMappingUtil::InitializeStylizerCallback()
{
    SetStylizerExceptionCallback(&MgdMappingUtilExceptionTrap);
}


///////////////////////////////////////////////////////////////////////////////
// PURPOSE: Accessor method for the base colors defined in this Layer and
//          scaleRange.  Used for RFC60.
// RETURNS: A pointer to the list of colors of the collected colors (maybe
//          empty but not null)
///////////////////////////////////////////////////////////////////////////////
void MgdMappingUtil::GetUsedColorsFromScaleRange(ColorStringList& usedColorList,
                                                MdfModel::VectorScaleRange* scaleRange,
                                                SE_SymbolManager* sman)
{
    // compute new color list by iterating through the featuretypecollection
    FeatureTypeStyleCollection* pftsColl = scaleRange->GetFeatureTypeStyles();
    int ftsccount = pftsColl->GetCount();
    for (int j=0; j<ftsccount; ++j)
    {
        FeatureTypeStyle* pfts = pftsColl->GetAt(j);

        // iterate through the rulecollection
        RuleCollection* ruleColl = pfts->GetRules();
        int rccount = ruleColl->GetCount();
        for (int k=0; k<rccount; ++k)
        {
            Rule* rule = ruleColl->GetAt(k);

            // get the label which will be the key in the color map
            Label* label = rule->GetLabel();
            if (label)
            {
                // add colors of txtsymbols
                TextSymbol* txtsym = label->GetSymbol();
                if (txtsym)
                {
                    // create copies of all strings so we can safely delete the resulting list later
                    usedColorList.push_back(txtsym->GetForegroundColor().substr());
                    usedColorList.push_back(txtsym->GetBackgroundColor().substr());
                }
            }

            // Do the casting to access the relevant members.  This is bad style
            // (instead of using the visitor pattern of the MdfModel) but it keeps
            // it nice and compact and documents the structure better...
            AreaRule* paRule = dynamic_cast<AreaRule*>(rule);
            LineRule* plRule = dynamic_cast<LineRule*>(rule);
            PointRule* ppRule = dynamic_cast<PointRule*>(rule);
            CompositeRule* pcRule = dynamic_cast<CompositeRule*>(rule);  // used for new stylization

            // AreaRule symbolization...
            if (paRule != NULL)
            {
                AreaSymbolization2D* pasym = paRule->GetSymbolization();
                if (pasym && pasym->GetFill() != NULL)
                {
                    // create copies of all strings so we can safely delete the resulting list later
                    usedColorList.push_back(pasym->GetFill()->GetForegroundColor().substr());
                    usedColorList.push_back(pasym->GetFill()->GetBackgroundColor().substr());
                }
                if (pasym && pasym->GetEdge() != NULL)
                    usedColorList.push_back(pasym->GetEdge()->GetColor().substr());
            }

            // LineRule Symbolization...
            if (plRule != NULL)
            {
                LineSymbolizationCollection* plsymcol = plRule->GetSymbolizations();

                // iterate through the linesymbolizations
                int lsccount = plsymcol->GetCount();
                for (int l=0; l < lsccount; l++)
                {
                    LineSymbolization2D* plsym = plsymcol->GetAt(l);
                    if (plsym && plsym->GetStroke() != NULL)
                        usedColorList.push_back(plsym->GetStroke()->GetColor().substr());
                }
            }

            // PointRule Symbolization...
            if (ppRule != NULL)
            {
                PointSymbolization2D* ppsym = ppRule->GetSymbolization();
                if (ppsym)
                {
                    Symbol* sym = ppsym->GetSymbol();
                    MdfModel::BlockSymbol* blockSymbol = dynamic_cast<MdfModel::BlockSymbol*>(sym);
                    MdfModel::FontSymbol* fontSymbol = dynamic_cast<MdfModel::FontSymbol*>(sym);
                    MdfModel::MarkSymbol* markSymbol = dynamic_cast<MdfModel::MarkSymbol*>(sym);
                    MdfModel::TextSymbol* textSymbol = dynamic_cast<MdfModel::TextSymbol*>(sym);
                    MdfModel::W2DSymbol* w2dSymbol = dynamic_cast<MdfModel::W2DSymbol*>(sym);
                    if (blockSymbol != NULL)
                    {
                        usedColorList.push_back(blockSymbol->GetBlockColor().substr());
                        usedColorList.push_back(blockSymbol->GetLayerColor().substr());
                    }
                    if (fontSymbol != NULL)
                    {
                        usedColorList.push_back(fontSymbol->GetForegroundColor().substr());
                    }
                    if (markSymbol != NULL)
                    {
                        if (markSymbol->GetEdge() != NULL)
                            usedColorList.push_back(markSymbol->GetEdge()->GetColor().substr());
                        if (markSymbol->GetFill() != NULL)
                        {
                            usedColorList.push_back(markSymbol->GetFill()->GetForegroundColor().substr());
                            usedColorList.push_back(markSymbol->GetFill()->GetBackgroundColor().substr());
                        }
                    }
                    if (textSymbol != NULL)
                    {
                        usedColorList.push_back(textSymbol->GetForegroundColor().substr());
                        usedColorList.push_back(textSymbol->GetBackgroundColor().substr());
                    }
                    if (w2dSymbol != NULL)
                    {
                        usedColorList.push_back(w2dSymbol->GetFillColor().substr());
                        usedColorList.push_back(w2dSymbol->GetLineColor().substr());
                        usedColorList.push_back(w2dSymbol->GetTextColor().substr());
                    }
                }
            }

            // CompositeRule Symbolization...
            if (pcRule != NULL)
            {
                CompositeSymbolization* pcsym = pcRule->GetSymbolization();
                SymbolInstanceCollection* sic = pcsym->GetSymbolCollection();
                int nInstances = sic->GetCount();
                for (int i=0; i<nInstances; ++i)
                {
                    SymbolInstance* instance = sic->GetAt(i);
                    // get the symbol definition, either inlined or by reference
                    SymbolDefinition* symdef = instance->GetSymbolDefinition();
                    if (symdef) // inline
                        FindColorInSymDefHelper(usedColorList, symdef);
                    else
                    {
                        // resourceId
                        const MdfString& symref = instance->GetResourceId(); // symbol reference
                        if (sman)
                        {
                            // if we have a symbolmanager do the lookup
                            MdfModel::SymbolDefinition* symdef = sman->GetSymbolDefinition(symref.c_str());
                            FindColorInSymDefHelper(usedColorList, symdef);
                        }
                    }
                } // for sym instances
            } // for CompositeRule
        } // for GetRules
    } // for GetFeatureTypeStyles
}


///////////////////////////////////////////////////////////////////////////////
// Extract colors from the scalerange, lookup the referenceIds, and store
// them with the map.
void MgdMappingUtil::ExtractColors(MgdMap* map, MdfModel::VectorScaleRange* scaleRange, Stylizer* stylizer)
{
    // add the colors from this scaleRange and vectorlayer to the map colors
    try
    {
        DefaultStylizer* ds = dynamic_cast<DefaultStylizer*>(stylizer);
        SE_SymbolManager* sman = ds? ds->GetSymbolManager() : NULL;
        // stack based
        ColorStringList vLayerColors;
        // parse the scalerange
        GetUsedColorsFromScaleRange(vLayerColors, scaleRange, sman);
        map->AddColorsToPalette(vLayerColors);
    }
    catch (exception e)
    {
        ACE_DEBUG((LM_DEBUG, L"(%t) %w caught in MappingUtil.ExtractColors\n", e.what()));
        throw e;
    }
}


///////////////////////////////////////////////////////////////////////////////
// overloaded helper for SimpleSymbolDefinition
inline void MgdMappingUtil::FindColorInSymDefHelper(ColorStringList& colorList, MdfModel::SimpleSymbolDefinition* symdef)
{
    // the visitor for the graphics
    class GraphicElementVisitorImpl : public MdfModel::IGraphicElementVisitor
    {
    public:
        ColorStringList* colorList;
        void VisitPath (Path& path)
        {
            colorList->push_back(path.GetLineColor().substr());
            colorList->push_back(path.GetFillColor().substr());
        };
        // TODO - other visitors for VisitImage and VisitText to be added here on a needed basis
        void VisitImage(Image& image) {};
        void VisitText(Text& text) {};
    } visitor;

    if (symdef)
    {
        MdfModel::LineUsage* lineUsage = symdef->GetLineUsage();
        if (lineUsage)
        {
            MdfModel::Path* path = lineUsage->GetDefaultPath();
            if (path)
            {
                colorList.push_back(path->GetLineColor().substr());
                colorList.push_back(path->GetFillColor().substr());
            }
        }

        MdfModel::GraphicElementCollection* graphElems = symdef->GetGraphics();
        int gInstances = graphElems->GetCount();
        for (int i=0; i < gInstances; ++i)
        {
            MdfModel::GraphicElement* graphics = graphElems->GetAt(i);
            if (graphics)
            {
                visitor.colorList = &colorList; // use injection to pass reference to list
                graphics->AcceptVisitor(visitor);
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// overloaded helper for CompoundSymbolDefinition
inline void MgdMappingUtil::FindColorInSymDefHelper(ColorStringList& colorList, MdfModel::CompoundSymbolDefinition* symdef)
{
    if (symdef)
    {
        MdfModel::SimpleSymbolCollection* simSymCol = symdef->GetSymbols();
        int kInstances = simSymCol->GetCount();
        for (int i=0; i<kInstances; ++i)
        {
            MdfModel::SimpleSymbol* simsym = simSymCol->GetAt(i);
            if (simsym)
            {
                MdfModel::SimpleSymbolDefinition* simplesymdef = simsym->GetSymbolDefinition();
                if (simplesymdef) // inline
                    FindColorInSymDefHelper(colorList, simplesymdef);
                else
                {
                    // TODO - resourceId
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// overloaded helper for SymbolDefinition
inline void MgdMappingUtil::FindColorInSymDefHelper(ColorStringList& colorList, MdfModel::SymbolDefinition* symdef)
{
    FindColorInSymDefHelper(colorList, dynamic_cast<MdfModel::SimpleSymbolDefinition*>(symdef));
    FindColorInSymDefHelper(colorList, dynamic_cast<MdfModel::CompoundSymbolDefinition*>(symdef));
}


///////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------
// RFC60 code for colormapped tiles by UV
//-------------------------------------------------------
// We examine the expressions collected from xml definitions of all layers.
// The color Palette for the renderer is a vector<RS_Color>.  The map object
// has a list from all color entries found in the most recent layer stylization.
// TODO - currently they are interpreted as ffffffff 32 bit RGBA string values.
// Adding expressions and other interpretations could be done here.
void MgdMappingUtil::ParseColorStrings (RS_ColorVector* tileColorPalette, MgdMap* map)
{
    assert(tileColorPalette);
    assert(map);
    ColorStringList& mapColorList = map->GetColorPalette(); // sorted and uniquified
    if (!mapColorList.empty()) // add them if they are available
    {
        ColorStringList::iterator it = mapColorList.begin();
        for (; it != mapColorList.end(); it++)
        {
            // string color reader for 0xAARRGGBB   "%10X" XML color strings
            // this is parsed and converted using the MgColor(string) ctor which also uses rgba order
            // but it is in argb order ergo => r=a, g=r, b=g, a=b
            MgColor c(*it);
            // set the RS_Color(r,g,b,a) assign the crossed over colors
            tileColorPalette->push_back(RS_Color(c.GetGreen(), c.GetBlue(), c.GetAlpha(), c.GetRed()));
        }
    }
}