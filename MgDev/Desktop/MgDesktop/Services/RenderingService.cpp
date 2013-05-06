#include "RenderingService.h"
#include "System/ConfigProperties.h"
#include "AGGRenderer.h"
#include "GDRenderer.h"
#include "ImageFormats.h"
#include "SymbolInstance.h"
#include "Rendering/RSMgFeatureReader.h"
#include "Rendering/FeatureInfoRenderer.h"
#include "Rendering/MappingUtil.h"
#include "MapLayer/Map.h"
#include "Feature/TransformCache.h"
#include "ServiceFactory.h"
#include "Tile/TileDefs.h"
#include "Rendering/MapPlot.h"
#include "Rendering/MapPlotCollection.h"

// the maximum number of allowed pixels for rendered images
static const INT32 MAX_PIXELS = 16384*16384;
static const INT32 FILTER_VISIBLE = 1;
static const INT32 FILTER_SELECTABLE = 2;
static const INT32 FILTER_HASTOOLTIPS = 4;

inline bool hasColorMap (STRING format)
{
    return format == L"PNG8" || format == L"GIF";
}

///////////////////////////////////////////////////////////////////////////////
// used when we want to process a given number of features
bool StylizeThatMany(void* data)
{
    MgdFeatureInfoRenderer* fir = (MgdFeatureInfoRenderer*)data;
    if ( fir->GetNumFeaturesProcessed() >= fir->GetNumMaxFeatures())
        return true;

    return false;
}


///////////////////////////////////////////////////////////////////////////////
MgdRenderingService::MgdRenderingService() : MgService()
{
    m_pCSFactory = new MgCoordinateSystemFactory();

    Ptr<MgdServiceFactory> fact = new MgdServiceFactory();

    m_svcResource = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcFeature = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
    assert(m_svcFeature != NULL);

    m_svcDrawing = dynamic_cast<MgdDrawingService*>(fact->CreateService(MgServiceType::DrawingService));
    assert(m_svcDrawing != NULL);

    MgConfiguration* pConf = MgConfiguration::GetInstance();
    pConf->GetStringValue(MgdConfigProperties::GeneralPropertiesSection,
                          MgdConfigProperties::GeneralPropertyRenderer,
                          m_rendererName,
                          MgdConfigProperties::DefaultGeneralPropertyRenderer);

    pConf->GetIntValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyRasterGridSize,
                          m_rasterGridSize,
                          MgdConfigProperties::DefaultRenderingServicePropertyRasterGridSize);

    pConf->GetIntValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyMinRasterGridSize,
                          m_minRasterGridSize,
                          MgdConfigProperties::DefaultRenderingServicePropertyMinRasterGridSize);

    pConf->GetDoubleValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatio,
                          m_rasterGridSizeOverrideRatio,
                          MgdConfigProperties::DefaultRenderingServicePropertyRasterGridSizeOverrideRatio);

    pConf->GetIntValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyRenderSelectionBatchSize,
                          m_renderSelectionBatchSize,
                          MgdConfigProperties::DefaultRenderingServicePropertyRenderSelectionBatchSize);

    pConf->GetIntValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyMaxRasterImageWidth,
                          m_maxRasterImageWidth,
                          MgdConfigProperties::DefaultRenderingServicePropertyMaxRasterImageWidth);

    pConf->GetIntValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyMaxRasterImageHeight,
                          m_maxRasterImageHeight,
                          MgdConfigProperties::DefaultRenderingServicePropertyMaxRasterImageHeight);

    // there should only be one instance of this class, so it's safe to
    // directly set these static variables
    bool bClampPoints;
    pConf->GetBoolValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyClampPoints,
                          bClampPoints,
                          MgdConfigProperties::DefaultRenderingServicePropertyClampPoints);
    AGGRenderer::s_bClampPoints = bClampPoints;

    bool bGeneralizeData;
    pConf->GetBoolValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyGeneralizeData,
                          bGeneralizeData,
                          MgdConfigProperties::DefaultRenderingServicePropertyGeneralizeData);
    AGGRenderer::s_bGeneralizeData = bGeneralizeData;
    GDRenderer::s_bGeneralizeData = bGeneralizeData;
}


///////////////////////////////////////////////////////////////////////////////
MgdRenderingService::~MgdRenderingService()
{
}

// ---------------------------------- BEGIN Rendering Service APIs ----------------------------------------------- //

///////////////////////////////////////////////////////////////////////////////
MgByteReader* MgdRenderingService::RenderTile(MgdMap* map, CREFSTRING baseMapLayerGroupName, INT32 tileColumn, INT32 tileRow)
{
    Ptr<MgByteReader> ret;

    MG_LOG_OPERATION_MESSAGE(L"RenderTile");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgResourceIdentifier" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(baseMapLayerGroupName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileColumn);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(tileRow);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::RenderTile()");

    if (NULL == map || baseMapLayerGroupName.empty())
        throw new MgNullArgumentException(L"MgdRenderingService.RenderTile", __LINE__, __WFILE__, NULL, L"", NULL);

    // find the finite display scale closest to the requested map scale
    double scale = map->GetViewScale();
    INT32 scaleIndex = map->FindNearestFiniteDisplayScaleIndex(scale);

    // if we don't find a nearest scale then something is wrong with the map
    if (scaleIndex < 0)
        throw new MgInvalidMapDefinitionException(L"MgdRenderingService.RenderTile", __LINE__, __WFILE__, NULL, L"", NULL);

    // get the layer group associated with the name
    Ptr<MgLayerGroupCollection> layerGroups = map->GetLayerGroups();
    Ptr<MgLayerGroup> baseGroup = layerGroups->GetItem(baseMapLayerGroupName);
    if (baseGroup == NULL)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(baseMapLayerGroupName);

        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderTile",
            __LINE__, __WFILE__, &arguments, L"MgdMapLayerGroupNameNotFound", NULL);
    }

    // get the scale at which to render the tile
    scale = map->GetFiniteDisplayScaleAt(scaleIndex);

    // ensure the tile DPI is set on the map
    map->SetDisplayDpi(MgdTileParameters::tileDPI);

    // ------------------------------------------------------
    // the upper left corner of tile (0,0) corresponds to the
    // upper left corner of the map extent
    // ------------------------------------------------------

    Ptr<MgEnvelope> mapExtent = map->GetMapExtent();
    Ptr<MgCoordinate> pt00 = mapExtent->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> pt11 = mapExtent->GetUpperRightCoordinate();
    double mapMinX = rs_min(pt00->GetX(), pt11->GetX());
    double mapMaxX = rs_max(pt00->GetX(), pt11->GetX());
    double mapMinY = rs_min(pt00->GetY(), pt11->GetY());
    double mapMaxY = rs_max(pt00->GetY(), pt11->GetY());

    double metersPerUnit  = map->GetMetersPerUnit();
    double metersPerPixel = METERS_PER_INCH / MgdTileParameters::tileDPI;
    double tileWidthMCS   = (double)MgdTileParameters::tileWidth  * metersPerPixel * scale / metersPerUnit;
    double tileHeightMCS  = (double)MgdTileParameters::tileHeight * metersPerPixel * scale / metersPerUnit;

    double tileMinX = mapMinX + (double)(tileColumn  ) * tileWidthMCS;  // left edge
    double tileMaxX = mapMinX + (double)(tileColumn+1) * tileWidthMCS;  // right edge
    double tileMinY = mapMaxY - (double)(tileRow   +1) * tileHeightMCS; // bottom edge
    double tileMaxY = mapMaxY - (double)(tileRow     ) * tileHeightMCS; // top edge

    // make the call to render the tile
    ret = RenderTile(map, baseGroup, scaleIndex, MgdTileParameters::tileWidth, MgdTileParameters::tileHeight, scale,
                     tileMinX, tileMaxX, tileMinY, tileMaxY, MgdTileParameters::tileFormat);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdRenderingService::RenderTile")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
/// render a map using all layers from the baseGroup
MgByteReader* MgdRenderingService::RenderTile(MgdMap* map,
                                                   MgLayerGroup* baseGroup,
                                                   INT32 scaleIndex,
                                                   INT32 width,
                                                   INT32 height,
                                                   double scale,
                                                   double mcsMinX,
                                                   double mcsMaxX,
                                                   double mcsMinY,
                                                   double mcsMaxY,
                                                   CREFSTRING format)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map || NULL == baseGroup)
        throw new MgNullArgumentException(L"MgdRenderingService.RenderTile", __LINE__, __WFILE__, NULL, L"", NULL);

    // get map extent that corresponds to tile extent
    RS_Bounds extent(mcsMinX, mcsMinY, mcsMaxX, mcsMaxY);

    // use the map's background color, but always make it fully transparent
    RS_Color bgColor;
    StylizationUtil::ParseColor(map->GetBackgroundColor(), bgColor);
    bgColor.alpha() = 0;

    // the label renderer needs to know the tile extent offset parameter
    double tileExtentOffset = 0.0;
    MgConfiguration* pConf = MgConfiguration::GetInstance();
    pConf->GetDoubleValue(MgdConfigProperties::RenderingServicePropertiesSection,
                          MgdConfigProperties::RenderingServicePropertyTileExtentOffset,
                          tileExtentOffset,
                          MgdConfigProperties::DefaultRenderingServicePropertyTileExtentOffset);
    if (tileExtentOffset < 0.0)
        tileExtentOffset = MgdConfigProperties::DefaultRenderingServicePropertyTileExtentOffset;

    // initialize the renderer (set clipping to false so that we label
    // the unclipped geometry)
    auto_ptr<SE_Renderer> dr(CreateRenderer(width, height, bgColor, false, true, tileExtentOffset));

    // create a temporary collection containing all the layers for the base group
    Ptr<MgLayerCollection> layers = map->GetLayers();
    Ptr<MgReadOnlyLayerCollection> roLayers = new MgReadOnlyLayerCollection();
    for (int i=0; i<layers->GetCount(); i++)
    {
        Ptr<MgLayerBase> layer = layers->GetItem(i);
        Ptr<MgLayerGroup> parentGroup = layer->GetGroup();
        if (parentGroup == baseGroup)
            roLayers->Add(layer);
    }

    // of course the group has to also be visible
    bool groupVisible = baseGroup->GetVisible();
    baseGroup->SetVisible(true);

    // call the internal helper API to do all the stylization overhead work
    ret = RenderMapInternal(map, NULL, roLayers, dr.get(), width, height, width, height, format, scale, extent, true, true, false);

    // restore the base group's visibility
    baseGroup->SetVisible(groupVisible);

    MG_CATCH_AND_THROW(L"MgdRenderingService.RenderTile")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgdRenderingService::RenderDynamicOverlay(MgdMap* map, MgdSelection* selection, CREFSTRING format)
{
    Ptr<MgByteReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"RenderDynamicOverlay");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgdMap" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdSelection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::RenderDynamicOverlay()");

    // Call updated RenderDynamicOverlay API
    //ret = RenderDynamicOverlay(map, selection, format, true);

    // Call updated RenderDynamicOverlay API
    MgdRenderingOptions options(format, MgdRenderingOptions::RenderSelection |
        MgdRenderingOptions::RenderLayers | MgdRenderingOptions::KeepSelection, NULL);
    ret = RenderDynamicOverlayInternal(map, selection, &options, NULL);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdRenderingService::RenderDynamicOverlay")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgdRenderingService::RenderDynamicOverlay(MgdMap* map, MgdSelection* selection, CREFSTRING format, bool bKeepSelection)
{
    Ptr<MgByteReader> ret;

    MG_LOG_OPERATION_MESSAGE(L"RenderDynamicOverlay");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgdMap" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdSelection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(bKeepSelection);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::RenderDynamicOverlay()");

    // Call updated RenderDynamicOverlay API
    MgdRenderingOptions options(format, MgdRenderingOptions::RenderSelection |
        MgdRenderingOptions::RenderLayers | (bKeepSelection? MgdRenderingOptions::KeepSelection : 0), NULL);
    ret = RenderDynamicOverlayInternal(map, selection, &options, NULL);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdRenderingService::RenderDynamicOverlay")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// called from API (first call of AjaxPgPViewerSampleApplication)
// default arg pPRMResult = NULL
MgByteReader* MgdRenderingService::RenderDynamicOverlay(MgdMap* map, MgdSelection* selection, MgdRenderingOptions* options)
{
    Ptr<MgByteReader> ret;

    MG_LOG_OPERATION_MESSAGE(L"RenderDynamicOverlay");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgdMap" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdSelection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdRenderingOptions");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::RenderDynamicOverlay()");

    // Call updated RenderDynamicOverlay API 
    ret = RenderDynamicOverlayInternal(map, selection, options, NULL);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdRenderingService::RenderDynamicOverlay")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// Non-published RenderDynamicOverlay API with profile result parameter
// pPRMResult - a pointer points to Profile Render Map Result.
MgByteReader* MgdRenderingService::RenderDynamicOverlay(MgdMap* map,
                                                       MgdSelection* selection,
                                                       MgdRenderingOptions* options,
                                                       ProfileRenderMapResult* pPRMResult)
{
    Ptr<MgByteReader> ret;

    MG_LOG_OPERATION_MESSAGE(L"RenderDynamicOverlay");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgdMap" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdSelection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdRenderingOptions");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"ProfileRenderMapResult");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::RenderDynamicOverlay()");

    ret = RenderDynamicOverlayInternal(map, selection, options, pPRMResult);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdRenderingService::RenderDynamicOverlay")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}

MgdFeatureInformation* MgdRenderingService::QueryFeatures(MgdMap* map,
                                                          MgStringCollection* layerNames,
                                                          MgGeometry* filterGeometry,
                                                          INT32 selectionVariant,
                                                          INT32 maxFeatures)
{
    Ptr<MgdFeatureInformation> ret;
    MG_LOG_OPERATION_MESSAGE(L"QueryFeatures");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 5);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgdMap" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((layerNames == NULL) ? L"MgStringCollection" : layerNames->GetLogString());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgGeometry");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(selectionVariant);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(maxFeatures);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::QueryFeatures()");

    ret = QueryFeaturesInternal(map, layerNames, filterGeometry, selectionVariant, L"", maxFeatures, 3 /*visible and selectable*/);

    MG_CATCH(L"MgdRenderingService::QueryFeatures")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()
    return ret.Detach();
}

MgdFeatureInformation* MgdRenderingService::QueryFeatures(
    MgdMap* map,
    MgStringCollection* layerNames,
    MgGeometry* filterGeometry,
    INT32 selectionVariant,
    CREFSTRING featureFilter,
    INT32 maxFeatures,
    INT32 layerAttributeFilter)
{
    Ptr<MgdFeatureInformation> ret;
    MG_LOG_OPERATION_MESSAGE(L"QueryFeatures");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 7);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgdMap" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((layerNames == NULL) ? L"MgStringCollection" : layerNames->GetLogString());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgGeometry");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(selectionVariant);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(featureFilter);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(maxFeatures);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(layerAttributeFilter);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::QueryFeatures()");

    ret = QueryFeaturesInternal(map, layerNames, filterGeometry, selectionVariant, featureFilter, maxFeatures, layerAttributeFilter);

    MG_CATCH(L"MgdRenderingService::QueryFeatures")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()
    return ret.Detach();
}

MgdFeatureInformation* MgdRenderingService::QueryFeaturesInternal(MgdMap* map,
                                                                  MgStringCollection* layerNames,
                                                                  MgGeometry* geometry,
                                                                  INT32 selectionVariant,
                                                                  CREFSTRING featureFilter,
                                                                  INT32 maxFeatures,
                                                                  INT32 layerAttributeFilter)
{
    Ptr<MgdFeatureInformation> ret;

    MG_TRY()

    //detect case where there is no limit to selection
    if (maxFeatures == -1)
        maxFeatures = INT_MAX;

    //create return structure and selection set to fill out
    ret = new MgdFeatureInformation();
    Ptr<MgdSelection> sel = new MgdSelection(map);

    double point_buf[2];
    double* point = NULL;
    auto_ptr<SE_Renderer> impRenderer;
    if (geometry && maxFeatures == 1)
    {
        MgPolygon* polygon = dynamic_cast<MgPolygon*>(geometry);
        if (polygon)
        {
            Ptr<MgCoordinateIterator> iterator = polygon->GetCoordinates();
            int num = 0;
            double pt0_x, pt0_y, pt_x, pt_y;
            while(iterator->MoveNext())
            {
                Ptr<MgCoordinate> coord = iterator->GetCurrent();
                double x = coord->GetX(), y = coord->GetY();
                if (!num)
                {
                    pt_x = pt0_x = x;
                    pt_y = pt0_y = y;
                }
                else if (x == pt0_x && y == pt0_y)
                    break;
                else
                {
                    pt_x += x;
                    pt_y += y;
                }
                num++;
            }
            if (num > 0)
            {
                point_buf[0] = pt_x / num;
                point_buf[1] = pt_y / num;
                point = point_buf;

                RS_Color bgColor; // not used
                impRenderer.reset(CreateRenderer(1, 1, bgColor, false));
            }
        }
    }

    MgdFeatureInfoRenderer fir(sel, maxFeatures, map->GetViewScale(), point, impRenderer.get());

    RenderForSelection(map, layerNames, geometry, selectionVariant, featureFilter, maxFeatures, layerAttributeFilter, &fir);

    //fill out the output object with the info we collected
    //in the MgdFeatureInfoRenderer for the first feature we hit
    if (fir.GetNumFeaturesProcessed() > 0)
    {
        Ptr<MgPropertyCollection> props = fir.GetProperties();
        ret->SetProperties(props);
        ret->SetHyperlink(fir.GetUrl());
        ret->SetTooltip(fir.GetTooltip());
    }

    ret->SetSelection(sel);

    #ifdef _DEBUG
    Ptr<MgReadOnlyLayerCollection> selLayers = sel->GetLayers();
    ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) MgServerRenderingService::QueryFeatures() Selection Size:%d\n"), selLayers.p? selLayers->GetCount() : 0));
    #endif

    MG_CATCH_AND_THROW(L"MgdRenderingService::QueryFeaturesInternal")

    return ret.Detach();
}

MgByteReader* MgdRenderingService::RenderDynamicOverlayInternal(MgdMap* map,
                                                               MgdSelection* selection,
                                                               MgdRenderingOptions* options,
                                                               ProfileRenderMapResult* pPRMResult)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgdRenderingService.RenderDynamicOverlayInternal", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    int width            = map->GetDisplayWidth();
    int height           = map->GetDisplayHeight();
    int dpi              = map->GetDisplayDpi();
    double scale         = map->GetViewScale();
    double metersPerUnit = map->GetMetersPerUnit();

    if (width <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderDynamicOverlayInternal", __LINE__, __WFILE__, NULL, L"MgdMapDisplayWidthCannotBeLessThanOrEqualToZero", NULL);

    if (height <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderDynamicOverlayInternal", __LINE__, __WFILE__, NULL, L"MgdMapDisplayHeightCannotBeLessThanOrEqualToZero", NULL);

    if (dpi <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderDynamicOverlayInternal", __LINE__, __WFILE__, NULL, L"MgdMapDisplayDpiCannotBeLessThanOrEqualToZero", NULL);

    if (scale <= 0.0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderDynamicOverlayInternal", __LINE__, __WFILE__, NULL, L"MgdMapViewScaleCannotBeLessThanOrEqualToZero", NULL);

    if (metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderDynamicOverlayInternal", __LINE__, __WFILE__, NULL, L"MgdMapMetersPerUnitCannotBeLessThanOrEqualToZero", NULL);

    // sanity check - number of image pixels cannot exceed MAX_PIXELS
    if (width * height > MAX_PIXELS)
        throw new MgOutOfRangeException(L"MgdRenderingService.RenderDynamicOverlayInternal", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

    // compute map extent that corresponds to pixel extent
    Ptr<MgPoint> pt          = map->GetViewCenter();
    Ptr<MgCoordinate> center = pt->GetCoordinate();
    double unitsPerPixel     = METERS_PER_INCH / (double)dpi / metersPerUnit;
    double mapWidth2         = 0.5 * (double)width  * unitsPerPixel * scale;
    double mapHeight2        = 0.5 * (double)height * unitsPerPixel * scale;

    RS_Bounds extent(center->GetX() - mapWidth2,
                     center->GetY() - mapHeight2,
                     center->GetX() + mapWidth2,
                     center->GetY() + mapHeight2);

    // use the map's background color, but always make it fully transparent
    RS_Color bgColor;
    StylizationUtil::ParseColor(map->GetBackgroundColor(), bgColor);
    bgColor.alpha() = 0;

    // initialize the renderer
    auto_ptr<SE_Renderer> dr(CreateRenderer(width, height, bgColor, true));

    bool bIncludeDynamicLayers = ((options->GetBehavior() & MgdRenderingOptions::RenderLayers) == MgdRenderingOptions::RenderLayers);
    bool bIncludeBaseLayers = ((options->GetBehavior() & MgdRenderingOptions::RenderBaseLayers) == MgdRenderingOptions::RenderBaseLayers);

    // create a temporary collection containing all the dynamic layers
    Ptr<MgLayerCollection> layers = map->GetLayers();
    Ptr<MgReadOnlyLayerCollection> roLayers = new MgReadOnlyLayerCollection();
    for (int i=0; i<layers->GetCount(); i++)
    {
        Ptr<MgLayerBase> layer = layers->GetItem(i);
        INT32 layerType = layer->GetLayerType();
        if (bIncludeDynamicLayers && layerType == MgLayerType::Dynamic)
            roLayers->Add(layer);
        else if (bIncludeBaseLayers && layerType == MgLayerType::BaseMap)
            roLayers->Add(layer);
    }

    if(NULL != pPRMResult)
    {
        Ptr<MgResourceIdentifier> mapResId = map->GetMapDefinition();
        pPRMResult->SetResourceId(mapResId ? mapResId->ToString() : L"");
        pPRMResult->SetScale(scale);
        pPRMResult->SetExtents(Box2D(extent.minx, extent.miny, extent.maxx, extent.maxy));
        pPRMResult->SetLayerCount(layers->GetCount());
    }

    // call the internal helper API to do all the stylization overhead work
    ret = RenderMapInternal(map, selection, roLayers, dr.get(), width, height, width, height, scale, extent, false, options, true, pPRMResult);

    MG_CATCH(L"MgdRenderingService.RenderDynamicOverlayInternal")
    if (mgException.p)
    {
        if(NULL != pPRMResult)
        {
            //MgServerManager* serverManager = MgServerManager::GetInstance();
            //STRING locale = (NULL == serverManager)? MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();
			STRING locale = MgResources::DefaultMessageLocale;

            Ptr<MgResourceIdentifier> mapResId = map->GetMapDefinition();
            pPRMResult->SetResourceId(mapResId ? mapResId->ToString() : L"");

            STRING message = mgException->GetExceptionMessage(locale);
            pPRMResult->SetError(message);
        }

        MG_THROW()
    }

    return ret.Detach();
}

///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgdRenderingService::RenderMap(MgdMap* map,
                                            MgdSelection* selection,
                                            CREFSTRING format)
{
    Ptr<MgByteReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"RenderMap");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgdMap" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdSelection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::RenderMap()");

    ret = RenderMapPublished(map, selection, format, true, false);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdRenderingService::RenderMap")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default arg bClip = false
MgByteReader* MgdRenderingService::RenderMap(MgdMap* map,
                                            MgdSelection* selection,
                                            CREFSTRING format,
                                            bool bKeepSelection)
{
    Ptr<MgByteReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"RenderMap");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgdMap" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdSelection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(bKeepSelection);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::RenderMap()");

    ret = RenderMapPublished(map, selection, format, bKeepSelection, false);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdRenderingService::RenderMap")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// render complete map around center point in given scale using map's background
// color and display sizes as default arguments to call the real rendermap method
// default arg (bKeepSelection = true, bClip = false)
MgByteReader* MgdRenderingService::RenderMap(MgdMap* map,
                                            MgdSelection* selection,
                                            CREFSTRING format,
                                            bool bKeepSelection,
                                            bool bClip)
{
    Ptr<MgByteReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"RenderMap");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 5);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgdMap" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgdSelection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(bKeepSelection);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(bClip);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::RenderMap()");

    ret = RenderMapPublished(map, selection, format, bKeepSelection, bClip);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdRenderingService::RenderMap")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}

MgByteReader* MgdRenderingService::RenderMapPublished(MgdMap* map,
                                                     MgdSelection* selection,
                                                     CREFSTRING format,
                                                     bool bKeepSelection,
                                                     bool bClip)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgdRenderingService.RenderMapPublished", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgPoint> pt = map->GetViewCenter();
    Ptr<MgCoordinate> center = pt->GetCoordinate();
    double scale = map->GetViewScale();

    // use the map's background color
    RS_Color col;
    StylizationUtil::ParseColor(map->GetBackgroundColor(), col);
    Ptr<MgColor> bgColor = new MgColor(col.red(), col.green(), col.blue(), col.alpha());

    // punt to more specific RenderMap API
    ret = RenderMap(map, selection, center, scale, map->GetDisplayWidth(), map->GetDisplayHeight(), bgColor, format, bKeepSelection, bClip);

    MG_CATCH_AND_THROW(L"MgdRenderingService.RenderMapPublished")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgdRenderingService::RenderMap(MgdMap* map,
                                                  MgdSelection* selection,
                                                  MgEnvelope* extents,
                                                  INT32 width,
                                                  INT32 height,
                                                  MgColor* backgroundColor,
                                                  CREFSTRING format)
{
    // Call updated RenderMap API
    return RenderMap(map, selection, extents, width, height, backgroundColor, format, true);
}


///////////////////////////////////////////////////////////////////////////////
// render the provided extent of the map and align aspect ratios to the provided window
// default arg bKeepSelection = true
MgByteReader* MgdRenderingService::RenderMap(MgdMap* map,
                                                  MgdSelection* selection,
                                                  MgEnvelope* extents,
                                                  INT32 width,
                                                  INT32 height,
                                                  MgColor* backgroundColor,
                                                  CREFSTRING format,
                                                  bool bKeepSelection)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map || extents == NULL || backgroundColor == NULL)
        throw new MgNullArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    int dpi              = map->GetDisplayDpi();
    double metersPerUnit = map->GetMetersPerUnit();

    if (width <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgdMapDisplayWidthCannotBeLessThanOrEqualToZero", NULL);

    if (height <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgdMapDisplayHeightCannotBeLessThanOrEqualToZero", NULL);

    if (dpi <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgdMapDisplayDpiCannotBeLessThanOrEqualToZero", NULL);

    if (metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgdMapMetersPerUnitCannotBeLessThanOrEqualToZero", NULL);

    // compute a view center and scale from the given extents
    // and pass on to the RenderMap that uses center and scale

    Ptr<MgCoordinate> ll = extents->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = extents->GetUpperRightCoordinate();

    RS_Bounds b(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());

    // If we need to scale the image (because of request for non-square
    // pixels) we will need to draw at one image size and then save at
    // another scaled size.  Here we will compute the correct map scale
    // and render size for a requested extent and image size.
    double screenAR = (double)width / (double)height;
    double mapAR = b.width() / b.height();

    int drawWidth = width;
    int drawHeight = height;
    double scale = 0.0;

    if (mapAR >= screenAR)
    {
        scale = b.width() * metersPerUnit / METERS_PER_INCH * (double)dpi / (double)width;

        // we based map scale on the image width, so adjust rendering
        // height to match the map aspect ratio
        drawHeight = (int)(width / mapAR);

        // ignore small perturbations in order to avoid rescaling the
        // end image in cases where the rescaling of width is less than
        // a pixel or so
        if (abs(drawHeight - height) <= 1)
            drawHeight = height;
    }
    else
    {
        scale = b.height() * metersPerUnit / METERS_PER_INCH * (double)dpi / (double)height;

        // we based map scale on the image height, so adjust rendering
        // height to match the map aspect ratio
        drawWidth = (int)(height * mapAR);

        // ignore small perturbations, in order to avoid rescaling the
        // end image in cases where the rescaling of width is less than
        // a pixel or so
        if (abs(drawWidth - width) <= 1)
            drawWidth = width;
    }

    // sanity check - number of image pixels cannot exceed MAX_PIXELS
    if (drawWidth * drawHeight > MAX_PIXELS)
        throw new MgOutOfRangeException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

    // use the supplied background color
    RS_Color bgcolor(backgroundColor->GetRed(),
                     backgroundColor->GetGreen(),
                     backgroundColor->GetBlue(),
                     backgroundColor->GetAlpha());

    // initialize the renderer with the rendering canvas size - in this
    // case it is not necessarily the same size as the requested image
    // size due to support for non-square pixels
    auto_ptr<SE_Renderer> dr(CreateRenderer(drawWidth, drawHeight, bgcolor, false));

    // call the internal helper API to do all the stylization overhead work
    ret = RenderMapInternal(map, selection, NULL, dr.get(), drawWidth, drawHeight, width, height, format, scale, b, false, bKeepSelection, true);

    MG_CATCH_AND_THROW(L"MgdRenderingService.RenderMap")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default argument bKeepSelection = true
MgByteReader* MgdRenderingService::RenderMap(MgdMap* map,
                                                  MgdSelection* selection,
                                                  MgCoordinate* center,
                                                  double scale,
                                                  INT32 width,
                                                  INT32 height,
                                                  MgColor* backgroundColor,
                                                  CREFSTRING format)
{
    // Call updated RenderMap API
    return RenderMap(map, selection, center, scale, width, height, backgroundColor, format, true);
}


///////////////////////////////////////////////////////////////////////////////
// default arguments bClip = false  pProfileRenderMapResult = NULL
MgByteReader* MgdRenderingService::RenderMap(MgdMap* map,
                                                  MgdSelection* selection,
                                                  MgCoordinate* center,
                                                  double scale,
                                                  INT32 width,
                                                  INT32 height,
                                                  MgColor* backgroundColor,
                                                  CREFSTRING format,
                                                  bool bKeepSelection)
{
    return RenderMap(map, selection, center, scale, width, height, backgroundColor, format, bKeepSelection, false);
}

///////////////////////////////////////////////////////////////////////////////
// default arguments bClip = false
MgByteReader* MgdRenderingService::RenderMap(MgdMap* map,
                                                  MgdSelection* selection,
                                                  MgCoordinate* center,
                                                  double scale,
                                                  INT32 width,
                                                  INT32 height,
                                                  MgColor* backgroundColor,
                                                  CREFSTRING format,
                                                  bool bKeepSelection,
                                                  ProfileRenderMapResult* pPRMResult)
{
    return RenderMap(map, selection, center, scale, width, height, backgroundColor, format, bKeepSelection, false, pPRMResult);
}

///////////////////////////////////////////////////////////////////////////////
// render map around center point in given scale
// default args bKeepSelection = true, bClip = false, backgroundColor = map->backgroundColor,
// width = map->getDisplayWidth, height = map->getDisplayHeight
MgByteReader* MgdRenderingService::RenderMap(MgdMap* map,
                                                  MgdSelection* selection,
                                                  MgCoordinate* center,
                                                  double scale,
                                                  INT32 width,
                                                  INT32 height,
                                                  MgColor* backgroundColor,
                                                  CREFSTRING format,
                                                  bool bKeepSelection,
                                                  bool bClip,
                                                  ProfileRenderMapResult* pPRMResult)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map || NULL == center || NULL == backgroundColor)
        throw new MgNullArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    int dpi              = map->GetDisplayDpi();
    double metersPerUnit = map->GetMetersPerUnit();

    if (width <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgdMapDisplayWidthCannotBeLessThanOrEqualToZero", NULL);

    if (height <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgdMapDisplayHeightCannotBeLessThanOrEqualToZero", NULL);

    if (dpi <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgdMapDisplayDpiCannotBeLessThanOrEqualToZero", NULL);

    if (scale <= 0.0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgdMapViewScaleCannotBeLessThanOrEqualToZero", NULL);

    if (metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgdMapMetersPerUnitCannotBeLessThanOrEqualToZero", NULL);

    // sanity check - number of image pixels cannot exceed MAX_PIXELS
    if (width * height > MAX_PIXELS)
        throw new MgOutOfRangeException(L"MgdRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

    double unitsPerPixel = METERS_PER_INCH / (double)dpi / metersPerUnit;
    double mapWidth2 = 0.5 * (double)width * unitsPerPixel * scale;
    double mapHeight2 = 0.5 * (double)height * unitsPerPixel * scale;

    // compute map extent that corresponds to given map center and scale
    RS_Bounds b(center->GetX() - mapWidth2,
                center->GetY() - mapHeight2,
                center->GetX() + mapWidth2,
                center->GetY() + mapHeight2);

    // use the supplied background color
    RS_Color bgcolor(backgroundColor->GetRed(),
                     backgroundColor->GetGreen(),
                     backgroundColor->GetBlue(),
                     backgroundColor->GetAlpha());

    // initialize the appropriate map renderer
    auto_ptr<SE_Renderer> dr(CreateRenderer(width, height, bgcolor, bClip));

    if(NULL != pPRMResult)
    {
        Ptr<MgResourceIdentifier> mapResId = map->GetMapDefinition();
        pPRMResult->SetResourceId(mapResId ? mapResId->ToString() : L"");
        pPRMResult->SetScale(scale);
        pPRMResult->SetExtents(Box2D(b.minx, b.miny, b.maxx, b.maxy));
        Ptr<MgLayerCollection> layers = map->GetLayers();
        pPRMResult->SetLayerCount(layers->GetCount());
    }

    // call the internal helper API to do all the stylization overhead work
    ret = RenderMapInternal(map, selection, NULL, dr.get(), width, height, width, height, format, scale, b, false, bKeepSelection, true, pPRMResult);

    MG_CATCH(L"MgdRenderingService.RenderMap")
    if (mgException.p)
    {
        if(NULL != pPRMResult)
        {
            //MgServerManager* serverManager = MgServerManager::GetInstance();
            //STRING locale = (NULL == serverManager)? MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();
			STRING locale = MgResources::DefaultMessageLocale;

            Ptr<MgResourceIdentifier> mapResId = map->GetMapDefinition();
            pPRMResult->SetResourceId(mapResId ? mapResId->ToString() : L"");

            STRING message = mgException->GetExceptionMessage(locale);
            pPRMResult->SetError(message);
        }

        MG_THROW()
    }
    return ret.Detach();
}

///////////////////////////////////////////////////////////////////////////////
// pack options into object and forward call (select Selection AND Layers)
// maybe keepSelection called by RenderTile
MgByteReader* MgdRenderingService::RenderMapInternal(MgdMap* map,
                                                          MgdSelection* selection,
                                                          MgReadOnlyLayerCollection* roLayers,
                                                          SE_Renderer* dr,
                                                          INT32 drawWidth,
                                                          INT32 drawHeight,
                                                          INT32 saveWidth,
                                                          INT32 saveHeight,
                                                          CREFSTRING format,
                                                          double scale,
                                                          RS_Bounds& b,
                                                          bool expandExtents,
                                                          bool bKeepSelection,
                                                          bool renderWatermark,
                                                          ProfileRenderMapResult* pPRMResult)
{
    MgdRenderingOptions options(format, MgdRenderingOptions::RenderSelection |
        MgdRenderingOptions::RenderLayers | (bKeepSelection? MgdRenderingOptions::KeepSelection : 0), NULL);
    return RenderMapInternal(map, selection, roLayers, dr, drawWidth, drawHeight, saveWidth, saveHeight, scale, b, expandExtents, &options, renderWatermark, pPRMResult);
}

///////////////////////////////////////////////////////////////////////////////
// called from (indirectly):
//      RenderMap(complete), RenderMap(extent)(roLayers == NULL)
//      RenderTile(selection == NULL)
// render the map using the provided rolayers
// (this is the baseGroup layers for rendering tiles)
// render map using provided options object from before
// this is called for tiles and for dynamic overlays
MgByteReader* MgdRenderingService::RenderMapInternal(MgdMap* map,
                                                          MgdSelection* selection,
                                                          MgReadOnlyLayerCollection* roLayers,
                                                          SE_Renderer* dr,
                                                          INT32 drawWidth,
                                                          INT32 drawHeight,
                                                          INT32 saveWidth,
                                                          INT32 saveHeight,
                                                          double scale,
                                                          RS_Bounds& b,
                                                          bool expandExtents,
                                                          MgdRenderingOptions* options,
                                                          bool renderWatermark,
                                                          ProfileRenderMapResult* pPRMResult)
{
    // set the map scale to the requested scale
    map->SetViewScale(scale);

    // convert the map coordinate system from srs wkt to a mentor cs structure
    STRING srs = map->GetMapSRS();
    Ptr<MgCoordinateSystem> dstCs;
    if (!srs.empty())
    {
        // let's not fail here if coord sys conversion fails for the map's
        // coordinate system. Instead we will fail per layer at a later stage
        try
        {
            dstCs = m_pCSFactory->Create(srs);
        }
        catch (MgInvalidCoordinateSystemException* e)
        {
            e->Release();
        }
    }

    RS_String units = dstCs.p? dstCs->GetUnits() : L"";

    // get the session ID
    STRING sessionId = map->GetSessionId();

    // initialize the stylizer
    RSMgdSymbolManager mgr(m_svcResource);
    dr->SetSymbolManager(&mgr);

    SEMgdSymbolManager semgr(m_svcResource);
    DefaultStylizer ds(&semgr);

    RS_Color bgcolor(0, 0, 0, 255); //not used -- GDRenderer is already initialized to the correct bgcolor

    STRING format = options->GetImageFormat();

    Ptr<MgPoint> ptCenter = map->GetViewCenter();
    Ptr<MgCoordinate> coord = ptCenter->GetCoordinate();
    RS_MapUIInfo mapInfo(sessionId, map->GetName(), map->GetObjectId(), srs, units, bgcolor, coord->GetX(), coord->GetY(), scale);

    // begin map stylization
    dr->StartMap(&mapInfo, b, scale, map->GetDisplayDpi(), map->GetMetersPerUnit(), NULL);

    MG_TRY()

        // if no layer collection is supplied, then put all layers in a temporary collection
        Ptr<MgReadOnlyLayerCollection> tempLayers = SAFE_ADDREF(roLayers);
        if (tempLayers == NULL)  // if called from renderMap not RenderTile
        {
            tempLayers = new MgReadOnlyLayerCollection();
            Ptr<MgLayerCollection> layers = map->GetLayers();
            for (INT32 i=0; i<layers->GetCount(); i++)
            {
                Ptr<MgLayerBase> layer = layers->GetItem(i);
                tempLayers->Add(layer);
            }
        }

        INT32 behavior = options->GetBehavior();

        if (behavior & MgdRenderingOptions::RenderLayers)    // this is for tiles so observer colormaps
        {
            RenderLayers(map, tempLayers, &ds, dr, dstCs, expandExtents, scale, format, pPRMResult);
        }

        // now we need to stylize the selection on top (this is not for tiles!)
        if (selection && (behavior & MgdRenderingOptions::RenderSelection))
        {
            RenderSelection(map, selection, tempLayers, options, &ds, dr, dstCs, scale, behavior, pPRMResult);
        }

        if (renderWatermark && (behavior & MgdRenderingOptions::RenderLayers) && map->GetWatermarkUsage() != 0)
        {
            RenderWatermarks(map,tempLayers,&ds, dr,drawWidth, drawHeight, saveWidth, saveHeight, pPRMResult);
        }

    MG_CATCH(L"MgdRenderingService.RenderMapInternal")

    if(NULL != pPRMResult)
    {
        if(NULL != dstCs)
        {
            pPRMResult->SetCoordinateSystem(dstCs->GetCsCode());
        }

        ProfileRenderLabelsResult* pPRLablesResult = new ProfileRenderLabelsResult(); // pointer points to Render Labels Result
        
        // Set the start time of stylizing labels
        pPRLablesResult->SetRenderTime(MgdTimerUtil::GetTime());

        pPRMResult->AdoptProfileRenderLabelsResult(pPRLablesResult);
    }

    dr->EndMap();

    if(NULL != pPRMResult)
    {
        ProfileRenderLabelsResult* pPRLablesResult = pPRMResult->GetProfileRenderLabelsResult();

        // Calculate the time spent on stylizing labels
        double stylizeLabelsTime = MgdTimerUtil::GetTime() - pPRLablesResult->GetRenderTime();
        pPRLablesResult->SetRenderTime(stylizeLabelsTime);
    }

    MG_THROW()  // to skip a faulty tile we need to rethrow the exception which could be thrown in StylizeLayers

    Ptr<MgByteReader> ret = CreateImage(map, dr, saveWidth, saveHeight, format, pPRMResult);

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
MgByteReader* MgdRenderingService::RenderMapLegend(MgdMap* map,
                                                  INT32 width,
                                                  INT32 height,
                                                  MgColor* backgroundColor,
                                                  CREFSTRING format)
{
    Ptr<MgByteReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"RenderMapLegend");

    MG_TRY()

    Ptr<MgResourceIdentifier> mapId = map->GetResourceId();
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 5);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == mapId) ? L"MgdMap" : mapId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(width);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_INT32(height);
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgColor");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(format.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdRenderingService::RenderMapLegend()");

    if (NULL == map || NULL == backgroundColor)
        throw new MgNullArgumentException(L"MgdRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    if (width <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"MgdMapDisplayWidthCannotBeLessThanOrEqualToZero", NULL);

    if (height <= 0)
        throw new MgInvalidArgumentException(L"MgdRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"MgdMapDisplayHeightCannotBeLessThanOrEqualToZero", NULL);

    // sanity check - number of image pixels cannot exceed MAX_PIXELS
    if (width * height > MAX_PIXELS)
        throw new MgOutOfRangeException(L"MgdRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

    RS_Color bgcolor(backgroundColor->GetRed(),
                     backgroundColor->GetGreen(),
                     backgroundColor->GetBlue(),
                     backgroundColor->GetAlpha());

    //initialize a renderer
    auto_ptr<Renderer> dr(CreateRenderer(width, height, bgcolor, false, false, 0.0));

    RS_Bounds b(0,0,width,height);

    RS_MapUIInfo info(L"", L"", L"", L"", L"", bgcolor);
    double pixelsPerInch = STANDARD_DISPLAY_DPI;
    double metersPerPixel = METERS_PER_INCH / pixelsPerInch;

    //start drawing
    dr->StartMap(&info, b, 1.0, pixelsPerInch, metersPerPixel, NULL);
    dr->StartLayer(NULL, NULL);

    //We need to specify margins and offsets in an MgdPlotSpecification,
    //even though in the image based (non-DWF) case they are 0.
    //Units are given as pixels, needed in order to scale legend
    //layout constants which are in inches.
    //NOTE: we need a left margin of one pixel so that legend icons
    //      do not overlap the legend border
    Ptr<MgdPlotSpecification> spec = new MgdPlotSpecification(width - 1.0f, height - 1.0f, L"pixels");
    spec->SetMargins(1.0f, 0.0f, 0.0f, 0.0f);
    MgdLegendPlotUtil lu(m_svcResource);
    lu.AddLegendElement(map->GetViewScale(), *dr, map, spec, 0.0, 0.0);

    //done drawing
    dr->EndLayer();
    dr->EndMap();

    // get a byte representation of the image
    auto_ptr<RS_ByteData> data;

    if (wcscmp(m_rendererName.c_str(), L"AGG") == 0)
        data.reset(((AGGRenderer*)dr.get())->Save(format, width, height));
    else
        data.reset(((GDRenderer*)dr.get())->Save(format, width, height));

    if (NULL != data.get())
    {
        // put this into a byte source
        Ptr<MgByteSource> bs = new MgByteSource(data->GetBytes(), data->GetNumBytes());

        if (format == MgdImageFormats::Gif)
            bs->SetMimeType(MgMimeType::Gif);
        else if (format == MgdImageFormats::Jpeg)
            bs->SetMimeType(MgMimeType::Jpeg);
        else if (format == MgdImageFormats::Png)
            bs->SetMimeType(MgMimeType::Png);
        else if (format == MgdImageFormats::Tiff)
            bs->SetMimeType(MgMimeType::Tiff);

        ret = bs->GetReader();
    }

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_CATCH(L"MgdRenderingService::RenderMapLegend")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_THROW()

    return ret.Detach();
}

///////////////////////////////////////////////////////////////////////////////
SE_Renderer* MgdRenderingService::CreateRenderer(int width,
                                                      int height,
                                                      RS_Color& bgColor,
                                                      bool requiresClipping,
                                                      bool localOverposting,
                                                      double tileExtentOffset)
{
    SE_Renderer* renderer = NULL;
    if (wcscmp(m_rendererName.c_str(), L"AGG") == 0)
        renderer = new AGGRenderer(width, height, bgColor, requiresClipping, localOverposting, tileExtentOffset);
    else
        renderer = new GDRenderer(width, height, bgColor, requiresClipping, localOverposting, tileExtentOffset);

    if (renderer != NULL)
    {
        renderer->SetRasterGridSize(m_rasterGridSize);
        renderer->SetMinRasterGridSize(m_minRasterGridSize);
        renderer->SetRasterGridSizeOverrideRatio(m_rasterGridSizeOverrideRatio);
        renderer->SetMaxRasterImageWidth(m_maxRasterImageWidth);
        renderer->SetMaxRasterImageHeight(m_maxRasterImageHeight);
    }

    return renderer;
}

///////////////////////////////////////////////////////////////////////////////
inline void MgdRenderingService::RenderLayers(MgdMap* map,
                                                   MgReadOnlyLayerCollection* layers,
                                                   Stylizer* ds,
                                                   Renderer* dr,
                                                   MgCoordinateSystem* dstCs,
                                                   bool expandExtents,
                                                   double scale,
                                                   CREFSTRING format,
                                                   ProfileRenderMapResult* pPRMResult)
{
    ProfileRenderLayersResult* pPRLsResult = NULL; // pointer points to Profile Render Layers Result

    if(NULL != pPRMResult)
    {
        pPRLsResult = new ProfileRenderLayersResult();
        pPRMResult->AdoptProfileRenderLayersResult(pPRLsResult);
                
        // Set the start time of stylizing layers
        pPRLsResult->SetRenderTime(MgdTimerUtil::GetTime());
    }

    MgdMappingUtil::StylizeLayers(m_svcResource, m_svcFeature, m_svcDrawing, m_pCSFactory, map,
                                    layers, NULL, ds, dr, dstCs, expandExtents, false, scale,
                                    false, hasColorMap(format), pPRLsResult);

    if(NULL != pPRMResult)
    {
        pPRLsResult = pPRMResult->GetProfileRenderLayersResult();

        // Calculate the time spent on stylizing layers
        double stylizeLayersTime = MgdTimerUtil::GetTime() - pPRLsResult->GetRenderTime();
        pPRLsResult->SetRenderTime(stylizeLayersTime);
    }
}

///////////////////////////////////////////////////////////////////////////////
inline void MgdRenderingService::RenderSelection(MgdMap* map,
                                                      MgdSelection* selection,
                                                      MgReadOnlyLayerCollection* layers,
                                                      MgdRenderingOptions* options,
                                                      Stylizer* ds,
                                                      Renderer* dr,
                                                      MgCoordinateSystem* dstCs,
                                                      double scale,
                                                      INT32 behavior,
                                                      ProfileRenderMapResult* pPRMResult)
{
    SE_Renderer* renderer = dynamic_cast<SE_Renderer*>(dr);

    ProfileRenderSelectionResult* pPRSResult = NULL; // pointer points to Profile Render Selection Result

    if(NULL != pPRMResult)
    {
        pPRSResult = new ProfileRenderSelectionResult();
        pPRMResult->AdoptProfileRenderSelectionResult(pPRSResult);
                
        // Set the start time of stylizing selected layers
        pPRSResult->SetRenderTime(MgdTimerUtil::GetTime());
    }

    Ptr<MgReadOnlyLayerCollection> selLayers = selection->GetLayers();

    #ifdef _DEBUG
    printf("MgdRenderingService::RenderSelection() - Layers:%d  Selection Layers:%d\n", layers? layers->GetCount() : 0, selLayers.p? selLayers->GetCount() : 0);
    #endif

    if (selLayers.p && selLayers->GetCount() > 0)
    {
        // tell the renderer to override draw styles with the ones
        // we use for selection
        MgColor *selectionColor = options->GetSelectionColor();
        if (selectionColor == NULL)
        {
            renderer->SetRenderSelectionMode(true);
        }
        else
        {
            renderer->SetRenderSelectionMode(true,
                selectionColor->GetRed() << 24 |
                selectionColor->GetGreen() << 16 |
                selectionColor->GetBlue() << 8 |
                selectionColor->GetAlpha());
        }
        SAFE_RELEASE(selectionColor);

        // prepare a collection of temporary MgLayers which have the right
        // FDO filters that will fetch only the selected features from FDO
        Ptr<MgReadOnlyLayerCollection> modLayers = new MgReadOnlyLayerCollection();
        Ptr<MgStringCollection> overrideFilters = new MgStringCollection();

        for (int s=0; s<selLayers->GetCount(); s++)
        {
            Ptr<MgLayerBase> selLayer = selLayers->GetItem(s);

            // generate a filter for the selected features
            Ptr<MgStringCollection> filters = selection->GenerateFilters(
            selLayer, selLayer->GetFeatureClassName(), m_renderSelectionBatchSize);
            INT32 numFilter = (NULL == filters)? 0 : filters->GetCount();

            for (INT32 i = 0; i < numFilter; ++i)
            {
                overrideFilters->Add(filters->GetItem(i));
                modLayers->Add(selLayer);
            }
        }

        MgdMappingUtil::StylizeLayers(m_svcResource, m_svcFeature, m_svcDrawing, m_pCSFactory, map,
            modLayers, overrideFilters, ds, renderer, dstCs, false, false, scale, (behavior & MgdRenderingOptions::KeepSelection) != 0,false,pPRSResult);

        // Set selection mode to false to avoid affecting following code
        renderer->SetRenderSelectionMode(false);
    }

    if(NULL != pPRMResult)
    {
        pPRSResult = pPRMResult->GetProfileRenderSelectionResult();

        // Calculate the time spent on stylizing selected layers
        double stylizeSelectionTime = MgdTimerUtil::GetTime() - pPRSResult->GetRenderTime();
        pPRSResult->SetRenderTime(stylizeSelectionTime);
    }
}
///////////////////////////////////////////////////////////////////////////////
inline void MgdRenderingService::RenderWatermarks(MgdMap* map,
                                                       MgReadOnlyLayerCollection* layers,
                                                       Stylizer* ds,
                                                       Renderer* dr,
                                                       int drawWidth,
                                                       int drawHeight,
                                                       INT32 saveWidth,
                                                       INT32 saveHeight,
                                                       ProfileRenderMapResult* pPRMResult)
{
    // Rendering watermark only when:
    // 1. rendering layers
    // 2. not set renderWatermark to false (not render tile)
    // 3. Map's watermark usage is not 0, which means watermark usage is WMS and / or Viewer.
    WatermarkInstanceCollection watermarkInstances;   //Watermark list to render
    WatermarkInstanceCollection tempWatermarkInstances;    //Used to reverse list
    auto_ptr<WatermarkInstance> tempInstance;

    // Get watermark instance in map
    Ptr<MgResourceIdentifier> mapId = map->GetMapDefinition();
    if (mapId.p)
    {
        auto_ptr<MapDefinition> mdef(MgMapBase::GetMapDefinition(m_svcResource, mapId));
        WatermarkInstanceCollection* mapWatermarks = mdef->GetWatermarks();
        for (int i=mapWatermarks->GetCount()-1; i>=0; i--)
            tempWatermarkInstances.Adopt(mapWatermarks->OrphanAt(i));
        for (int i=tempWatermarkInstances.GetCount()-1; i>=0; i--)
        {
            tempInstance.reset(tempWatermarkInstances.OrphanAt(i));
            if (!tempInstance.get())
                continue;
            if (((map->GetWatermarkUsage() & MgdMap::Viewer) != 0
                && (tempInstance->GetUsage() & WatermarkInstance::Viewer) == 0)
                || ((map->GetWatermarkUsage() & MgdMap::WMS) != 0
                && (tempInstance->GetUsage() & WatermarkInstance::WMS) == 0))
                continue;
            bool alreadyInList = false;
            for (int j=watermarkInstances.GetCount()-1; j >=0; j--)
            {
                if (tempInstance->Equals(watermarkInstances.GetAt(j)))
                {
                    alreadyInList = true;
                    break;
                }
            }

            if (!alreadyInList)
            {
                watermarkInstances.Adopt(tempInstance.release());
            }
        }
    }

    // Get watermark instance in layer
    const int layerCount = layers->GetCount();
    auto_ptr<LayerDefinition> ldf;
    for (int i=0; i<layerCount; ++i)
    {
        Ptr<MgLayerBase> mapLayer(layers->GetItem(i));

        Ptr<MgResourceIdentifier> layerid = mapLayer->GetLayerDefinition();
        ldf.reset(MgLayerBase::GetLayerDefinition(m_svcResource, layerid));

        WatermarkInstanceCollection* layerWatermarks = ldf->GetWatermarks();
        for (int j=layerWatermarks->GetCount()-1; j>=0; j--)
            tempWatermarkInstances.Adopt(layerWatermarks->OrphanAt(j));
        for (int j=tempWatermarkInstances.GetCount()-1; j>=0; j--)
        {
            tempInstance.reset(tempWatermarkInstances.OrphanAt(j));
            if (!tempInstance.get())
                continue;
            if (((map->GetWatermarkUsage() & MgdMap::Viewer) != 0
                && (tempInstance->GetUsage() & WatermarkInstance::Viewer) == 0)
                || ((map->GetWatermarkUsage() & MgdMap::WMS) != 0
                && (tempInstance->GetUsage() & WatermarkInstance::WMS) == 0))
                continue;

            bool alreadyInList = false;
            for (int k=watermarkInstances.GetCount()-1; k>=0; k--)
            {
                if (tempInstance->Equals(watermarkInstances.GetAt(k)))
                {
                    alreadyInList = true;
                    break;
                }
            }

            if (!alreadyInList)
            {
                watermarkInstances.Adopt(tempInstance.release());
            }
        }
    }
    assert(tempWatermarkInstances.GetCount() == 0);

    MgStringCollection watermarkIds;            // ID list of loaded watermark definition
    MgStringCollection watermarkDefinitions;    // Loaded watermark definition
    MgStringCollection failLoadedIds;           // ID list of failed in loading resource

    if(0 != watermarkInstances.GetCount())
    {
        ProfileRenderWatermarksResult* pPRWsResult = NULL; // pointer points to Profile Render Watermarks Result

        if(NULL != pPRMResult)
        {
            pPRWsResult = new ProfileRenderWatermarksResult();
            pPRMResult->AdoptProfileRenderWatermarksResult(pPRWsResult);
                
            // Set the start time of stylizing watermarks
            pPRWsResult->SetRenderTime(MgdTimerUtil::GetTime());
        }

        for (int i=watermarkInstances.GetCount()-1; i>=0; i--)
        {
            WatermarkInstance* instance = watermarkInstances.GetAt(i);
            STRING resourceId = instance->GetResourceId();
            WatermarkDefinition* wdef = NULL;
            MG_TRY()
                for(int j = 0; j < watermarkIds.GetCount(); j++)
                {
                    if(resourceId == watermarkIds.GetItem(j))
                    {
                        wdef = MgWatermark::GetWatermarkDefinition(watermarkDefinitions.GetItem(j));
                        break;
                    }
                }

                if(NULL != pPRWsResult)
                {
                    ProfileRenderWatermarkResult* pPRWResult = new ProfileRenderWatermarkResult(); // pointer points to Render Watermark Result
                        
                    // Set the start time of stylizing watermark
                    pPRWResult->SetRenderTime(MgdTimerUtil::GetTime());

                    ProfileRenderWatermarkResultCollection* pPRWResultColl = pPRWsResult->GetProfileRenderWatermarkResults();
                    pPRWResultColl->Adopt(pPRWResult);
                }

                if(wdef == NULL)
                {
                    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(resourceId);
                    Ptr<MgByteReader> reader = m_svcResource->GetResourceContent(resId);
                    STRING content = reader->ToString();
                    watermarkIds.Add(resourceId);
                    watermarkDefinitions.Add(content);
                    wdef = MgWatermark::GetWatermarkDefinition(content);
                }
                assert(wdef != NULL);
                if (instance->GetPositionOverride())
                {
                    wdef->AdoptPosition(instance->OrphanPositionOverride());
                }
                if (instance->GetAppearanceOverride())
                {
                    wdef->AdoptAppearance(instance->OrphanAppearanceOverride());
                }
                ds->StylizeWatermark(dr, wdef, drawWidth, drawHeight, saveWidth, saveHeight);
                
                if(NULL != pPRWsResult)
                {
                    ProfileRenderWatermarkResultCollection* pPRWResultColl = pPRWsResult->GetProfileRenderWatermarkResults();
                    ProfileRenderWatermarkResult* pPRWResult = pPRWResultColl->GetAt(pPRWResultColl->GetCount()-1); // TODO: check index

                    // Calculate the time spent on stylizing watermark
                    double stylizeWatermarkTime = MgdTimerUtil::GetTime() - pPRWResult->GetRenderTime();
                    pPRWResult->SetRenderTime(stylizeWatermarkTime);

                    pPRWResult->SetResourceId(resourceId);

                    WatermarkPosition* position = wdef->GetPosition();
                    if(NULL != dynamic_cast<XYWatermarkPosition*>(position))
                    {
                        pPRWResult->SetPositionType(L"XY");
                    }
                    else // No other position types
                    {
                        pPRWResult->SetPositionType(L"Tile");
                    }
                }

            MG_CATCH(L"MgdRenderingService.RenderWatermarks")
            if(mgException.p)
            {
                // Do not do anything if fail in resource loading and has logged error.
                bool isExceptionLogged = false;
                if(wdef == NULL) // Fail in resource loading
                { 
                    for(int i = 0; i < failLoadedIds.GetCount(); i++)
                    {
                        if(resourceId == failLoadedIds.GetItem(i))
                        {
                            isExceptionLogged = true;
                            break;
                        }
                    }
                }
                if(!isExceptionLogged)
                {
                    // TODO: Eventually this should be used to indicate visually to the client what
                    //       layer failed in addition to logging the error.
                    //MgServerManager* serverManager = MgServerManager::GetInstance();
                    //STRING locale = (NULL == serverManager)? MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();
					STRING locale = MgResources::DefaultMessageLocale;
                    //MG_LOG_EXCEPTION_ENTRY(mgException->GetExceptionMessage(locale).c_str(), mgException->GetStackTrace(locale).c_str());

#if defined(_DEBUG) || defined(_DEBUG_PNG8)
                    STRING details = mgException->GetDetails(locale);

                    wstring err = L"\n %t Error during stylization of watermark:";
                    err += instance->GetName();
                    err += L"\n";
                    err += L"Details: ";
                    err += details;
                    err += L"\n";
                    ACE_DEBUG( (LM_DEBUG, err.c_str()) );
#endif
                    if(wdef == NULL)            // Failed in resource loading
                    {
                        failLoadedIds.Add(resourceId);
                    }

                    if(NULL != pPRWsResult)
                    {
                        ProfileRenderWatermarkResultCollection* pPRWResultColl = pPRWsResult->GetProfileRenderWatermarkResults();
                        ProfileRenderWatermarkResult* pPRWResult = pPRWResultColl->GetAt(pPRWResultColl->GetCount()-1); // TODO: check index

                        // Calculate the time spent on stylizing watermark
                        double stylizeWatermarkTime = MgdTimerUtil::GetTime() - pPRWResult->GetRenderTime();
                        pPRWResult->SetRenderTime(stylizeWatermarkTime);

                        pPRWResult->SetResourceId(resourceId);

                        STRING message = mgException->GetExceptionMessage(locale);
                        pPRWResult->SetError(message);
                    }
                }
            }
        }

        if(NULL != pPRWsResult)
        {
            // Calculate the time spent on stylizing watermarks
            double stylizeRenderWatermarksTime = MgdTimerUtil::GetTime() - pPRWsResult->GetRenderTime();
            pPRWsResult->SetRenderTime(stylizeRenderWatermarksTime);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
inline MgByteReader* MgdRenderingService::CreateImage(MgdMap* map,
                                                           Renderer* dr,
                                                           INT32 saveWidth,
                                                           INT32 saveHeight,
                                                           CREFSTRING format,
                                                           ProfileRenderMapResult* pPRMResult)
{
    if(NULL != pPRMResult)
    {
        // Set the start time of creating map image
        pPRMResult->SetCreateImageTime(MgdTimerUtil::GetTime());
    }

/*
    //-------------------------------------------------------
    // draw a border around the tile - used for debugging
    RS_LineStroke ls;
    ls.color() = RS_Color(128, 128, 128, 64);

    LineBuffer lb(5);
    double mcsMinX = b.minx;
    double mcsMaxX = b.maxx;
    double mcsMinY = b.miny;
    double mcsMaxY = b.maxy;
    double incX = (mcsMaxX - mcsMinX) / saveWidth  / 10.0;
    double incY = (mcsMaxY - mcsMinY) / saveHeight / 10.0;
    lb.MoveTo(mcsMinX + incX, mcsMinY + incY);
    lb.LineTo(mcsMaxX - incX, mcsMinY + incY);
    lb.LineTo(mcsMaxX - incX, mcsMaxY - incY);
    lb.LineTo(mcsMinX + incX, mcsMaxY - incY);
    lb.LineTo(mcsMinX + incX, mcsMinY + incY);
    dr->ProcessPolyline(&lb, ls);
    //-------------------------------------------------------
*/

    // get a byte representation of the image
    auto_ptr<RS_ByteData> data;
    Ptr<MgByteSource> bs;

    try
    {
        // call the image renderer to create the image
        if (wcscmp(m_rendererName.c_str(), L"AGG") == 0)
        {
            //-------------------------------------------------------
            /// RFC60 code to correct colormaps by UV
            //-------------------------------------------------------
            // We examine the expressions collected from xml definitions of all layers.
            // The map object has a list from all color entries found in the most recent
            // layer stylization.
            // * TODO - currently they are interpreted as ffffffff 32-bit RGBA string values
            // * adding expresssions and other interpretations should be done in ParseColorStrings
            // * the color Palette for the renderer is a vector<RS_Color>
            if (hasColorMap(format))
            {
                RS_ColorVector tileColorPalette;
                MgdMappingUtil::ParseColorStrings(&tileColorPalette, map);
//              printf("<<<<<<<<<<<<<<<<<<<<< MgdRenderingService::ColorPalette->size(): %d\n", tileColorPalette.size());
                data.reset(((AGGRenderer*)dr)->Save(format, saveWidth, saveHeight, &tileColorPalette));
            }
            else
                data.reset(((AGGRenderer*)dr)->Save(format, saveWidth, saveHeight, NULL));
        }
        else
            data.reset(((GDRenderer*)dr)->Save(format, saveWidth, saveHeight));
    }
    catch (exception e)
    {
        ACE_DEBUG((LM_DEBUG, L"(%t) %w caught in RenderingService ColorPaletteGeneration\n", e.what()));
        throw e;
    }

    if (NULL != data.get())
    {
        // put this into a byte source
        bs = new MgByteSource(data->GetBytes(), data->GetNumBytes());

        if (format == MgdImageFormats::Gif)
            bs->SetMimeType(MgMimeType::Gif);
        else if (format == MgdImageFormats::Jpeg)
            bs->SetMimeType(MgMimeType::Jpeg);
        else if (format == MgdImageFormats::Png || format == MgdImageFormats::Png8)
            bs->SetMimeType(MgMimeType::Png);
        else if (format == MgdImageFormats::Tiff)
            bs->SetMimeType(MgMimeType::Tiff);
    }
    else
        throw new MgNullReferenceException(L"MgdRenderingService.CreateImage", __LINE__, __WFILE__, NULL, L"MgNoDataFromRenderer", NULL);
    
    if(NULL != pPRMResult)
    {
        // Calculate the time spent on stylizing labels
        double createImageTime = MgdTimerUtil::GetTime() - pPRMResult->GetCreateImageTime();
        pPRMResult->SetCreateImageTime(createImageTime);

        pPRMResult->SetImageFormat(format);
        pPRMResult->SetRendererType(m_rendererName);
    }

    return bs->GetReader();
}

///////////////////////////////////////////////////////////////////////////////
// A helper function that does most of the work for QueryFeatures
// and QueryFeatureProperties.  Basically runs a rendering loop with
// a custom renderer supplied by the caller that accumulates selection
// related things like property values and feature IDs.
void MgdRenderingService::RenderForSelection(MgdMap* map,
                                             MgStringCollection* layerNames,
                                             MgGeometry* geometry,
                                             INT32 selectionVariant,
                                             CREFSTRING featureFilter,
                                             INT32 maxFeatures,
                                             INT32 layerAttributeFilter,
                                             MgdFeatureInfoRenderer* selRenderer)
{
    // Cache coordinate system transforms for the life of the
    // stylization operation.
    MgdTransformCacheMap transformCache;

    MG_TRY()
#ifdef _DEBUG
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) RenderForSelection(): ** START **\n")));
#endif
    if (NULL == map || (NULL == geometry && featureFilter.empty()))
        throw new MgNullArgumentException(L"MgServerRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"", NULL);

    if (maxFeatures < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(maxFeatures, buffer);

        MgStringCollection arguments;
        arguments.Add(L"5");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderForSelection",
            __LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanZero", NULL);
    }

    // get the session ID
    STRING sessionId;
    //Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
    //if (userInfo != NULL)
    //    sessionId = userInfo->GetMgSessionId();

    // validate map view parameters
    int width            = map->GetDisplayWidth();
    int height           = map->GetDisplayHeight();
    int dpi              = map->GetDisplayDpi();
    double scale         = map->GetViewScale();
    double metersPerUnit = map->GetMetersPerUnit();

    if (width <= 0)
        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgMapDisplayWidthCannotBeLessThanOrEqualToZero", NULL);

    if (height <= 0)
        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgMapDisplayHeightCannotBeLessThanOrEqualToZero", NULL);

    if (dpi <= 0)
        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgMapDisplayDpiCannotBeLessThanOrEqualToZero", NULL);

    if (scale <= 0.0)
        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgMapViewScaleCannotBeLessThanOrEqualToZero", NULL);

    if (metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgMapMetersPerUnitCannotBeLessThanOrEqualToZero", NULL);

    // compute map extent that corresponds to pixel extent
    Ptr<MgPoint> pt          = map->GetViewCenter();
    Ptr<MgCoordinate> center = pt->GetCoordinate();
    double unitsPerPixel     = METERS_PER_INCH / (double)dpi / metersPerUnit;
    double mapWidth2         = 0.5 * (double)width  * unitsPerPixel * scale;
    double mapHeight2        = 0.5 * (double)height * unitsPerPixel * scale;

    RS_Bounds extent(center->GetX() - mapWidth2,
                     center->GetY() - mapHeight2,
                     center->GetX() + mapWidth2,
                     center->GetY() + mapHeight2);

    // begin map stylization
    RS_Color bgcolor(0, 0, 0, 255); // not used
    STRING srs = map->GetMapSRS();
    RS_MapUIInfo mapInfo(sessionId, map->GetName(), map->GetObjectId(), srs, L"", bgcolor, center->GetX(), center->GetY(), scale);

    // initialize the stylizer
    SEMgdSymbolManager semgr(m_svcResource);
    DefaultStylizer ds(&semgr);

    selRenderer->StartMap(&mapInfo, extent, scale, dpi, metersPerUnit, NULL);

    //initial simple selection scheme
    //Run a geometric FDO query on the given selection geometry
    //and return the features we get from FDO

    Ptr<MgLayerCollection> layers = map->GetLayers();

    bool bOnlySelectableLayers = !((layerAttributeFilter & FILTER_SELECTABLE) == 0);
    bool bOnlyVisibleLayers = !((layerAttributeFilter & FILTER_VISIBLE) == 0);
    bool bOnlyTooltipLayers = !((layerAttributeFilter & FILTER_HASTOOLTIPS) == 0);

    //iterate over all map layers, but only do selection
    //if the layer is in the passed in collection
    for (int p=0; p<layers->GetCount(); p++)
    {
        //find the layer we need to select features from
        Ptr<MgLayerBase> layer = layers->GetItem(p);
    #ifdef _DEBUG
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) RenderForSelection(): Layer: %W  Selectable:%W  Visible: %W\n"), layer->GetName().c_str(), layer->GetSelectable()? L"True" : L"False", layer->IsVisibleAtScale(scale)? L"True" : L"False"));
    #endif
        //do this first - this check is fast
        if (bOnlySelectableLayers && !layer->GetSelectable())
            continue;

        //do we want to select on this layer -- if caller
        //gave us a layer name collection, check if the layer
        //is in there
        if (layerNames && layerNames->GetCount() > 0 && layerNames->IndexOf(layer->GetName()) == -1)
            continue;

        //check the visibility at scale if we're not ignoring scale ranges
        if (bOnlyVisibleLayers && !layer->IsVisibleAtScale(scale))
            continue;

        //if we only want layers with tooltips, check that this layer has tooltips
        if (bOnlyTooltipLayers)
        {
//          layer->GetLayerInfoFromDefinition(m_svcResource);
            if (!layer->HasTooltips())
                continue;
        }

        //have we processed enough features already?
        if (maxFeatures <= 0)
            break;

        //get the MDF layer definition
        Ptr<MgResourceIdentifier> layerResId = layer->GetLayerDefinition();
        auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(m_svcResource, layerResId));
        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());

        //we can only do geometric query selection for vector layers
        if (vl)
        {
        #ifdef _DEBUG
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) RenderForSelection(): Layer: %W  Vector Layer\n"), layer->GetName().c_str()));
        #endif
            //check to see if we want even layers that aren't visible at the current scale
            if (!bOnlyVisibleLayers)
            {
                // Modify the layer scale range only for layers that are passed in
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

            Ptr<MgResourceIdentifier> featResId = new MgResourceIdentifier(layer->GetFeatureSourceId());

            //get a transform from layer coord sys to map coord sys
            Ptr<MgCoordinateSystem> mapCs = srs.empty()? NULL : m_pCSFactory->Create(srs);
            MgdTransformCache* item = MgdTransformCache::GetLayerToMapTransform(transformCache, vl->GetFeatureName(), featResId, mapCs, m_pCSFactory, m_svcFeature);
            Ptr<MgCoordinateSystemTransform> trans = item? item->GetMgTransform() : NULL;
            MgdCSTrans* xformer = item? item->GetTransform() : NULL;

            Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();
            Ptr<MgGeometricEntity> queryGeom;
            if (geometry != NULL)
            {
                //if we have a valid transform, get the request geom in layer's space
                queryGeom = SAFE_ADDREF(geometry);
                STRING geomTextSource = queryGeom->ToAwkt(true);
                if (trans)
                {
                    //get selection geometry in layer space
                    queryGeom = geometry->Transform(trans);
                }

                #ifdef _DEBUG
                // Output the selection geometry
                STRING geomText = queryGeom->ToAwkt(true);
                ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) SELECTION FILTER:\n%W\n\n"), geomText.c_str()));
                #endif

                //set the spatial filter for the selection
                options->SetSpatialFilter(layer->GetFeatureGeometryName(), (MgGeometry*)(queryGeom.p), /*MgFeatureSpatialOperations*/selectionVariant);
            }

            // Initialize the reader
            auto_ptr<RSMgdFeatureReader> rsrdr;

            try
            {
                if (!featureFilter.empty())
                {
                    //set the feature filter, if any
                    MgdSelection selectionFilter(map, featureFilter);
                    Ptr<MgReadOnlyLayerCollection> layers = selectionFilter.GetLayers();
                    if (layers != NULL)
                    {
                        for (int i = 0; i < layers->GetCount(); i++)
                        {
                            Ptr<MgLayerBase> layer = layers->GetItem(i);
                            STRING className = layer->GetFeatureClassName();
                            STRING filter = selectionFilter.GenerateFilter(layer, className);
                            options->SetFilter(filter);
                        }
                    }
                }
                else if (!vl->GetFilter().empty())
                {
                    //set layer feature filter if any
                    options->SetFilter(vl->GetFilter());
                }

                // TODO: can FeatureName be an extension name rather than a FeatureClass?
                // The reader below needs to be closed and released before the intersectPolygon SelectFeatures below happens
                // or we end up with a reference count issue that causes a new FDO connection to be cached instead of
                // reusing the already existing one.
                Ptr<MgFeatureReader> rdr = m_svcFeature->SelectFeatures(featResId, vl->GetFeatureName(), options);
                rsrdr.reset(new RSMgdFeatureReader(rdr, m_svcFeature, featResId, options, vl->GetGeometry()));

                // Note that the FdoIFeatureReader smart pointer below is created
                // inside the following IF statement to ensure it gets destroyed
                // BEFORE the RSMgdFeatureReader object above goes out of scope,
                // even when an exception gets thrown.
                if (FdoPtr<FdoIFeatureReader>(rsrdr->GetInternalReader()))
                {
                    //run a stylization loop with the MgdFeatureInfoRenderer.
                    //This will build up the selection set and also
                    //evaluate the tooltip, hyperlink and feature properties
                    //for the first feature hit

                    RS_UIGraphic uig(NULL, 0, L"");
                    Ptr<MgResourceIdentifier> layerDefId = layer->GetLayerDefinition();
                    RS_LayerUIInfo layerinfo(layer->GetName(),
                                             layerDefId->ToString(),
                                             layer->GetObjectId(), // object ID
                                             true,   // selectable
                                             true,   // visible
                                             true,   // editable
                                             L"",    // group name
                                             L"",    // group ID
                                             true,   // showInLegend
                                             true,   // expandInLegend
                                             0.0,    // zOrder
                                             uig);   // uiGraphic

                    //extract hyperlink and tooltip info
                    if (!vl->GetToolTip().empty()) 
                        layerinfo.hastooltips() = true;
                    if (vl->GetUrlData() && !vl->GetUrlData()->GetUrlContent().empty()) 
                        layerinfo.hashyperlinks() = true;

                    //set up the property name mapping -- it tells us what
                    //string the viewer should be displaying as the name of each
                    //feature property
                    // TODO: can FeatureName be an extension name rather than a FeatureClass?
                    RS_FeatureClassInfo fcinfo(vl->GetFeatureName(), vl->GetResourceID());

                    MdfModel::NameStringPairCollection* pmappings = vl->GetPropertyMappings();
                    for (int i=0; i<pmappings->GetCount(); i++)
                    {
                        MdfModel::NameStringPair* m = pmappings->GetAt(i);
                        fcinfo.add_mapping(m->GetName(), m->GetValue());
                    }

                    selRenderer->StartLayer(&layerinfo, &fcinfo);
                    ds.StylizeVectorLayer(vl, selRenderer, rsrdr.get(), NULL, scale, StylizeThatMany, selRenderer);

                    // Clear the readers in case they are reused below
                    rdr = NULL;
                    rsrdr.reset();

                    int numFeaturesProcessed = selRenderer->GetNumFeaturesProcessed();
                    if (!numFeaturesProcessed && selRenderer->NeedPointTest())
                    {
                        // Construct a square selection area 400x larger than then previous area
                        // centered around the centroid of the original selection area
                        // Example:  a 4x4 pixel selection area becomes an 80x80 pixel selection area
                        Ptr<MgPoint> centroid = queryGeom->GetCentroid();
                        Ptr<MgCoordinate> ctr = centroid->GetCoordinate();
                        double area = queryGeom->GetArea();
                        double delta = sqrt(area) * 10.0;

                        // Only process selection if we have a valid area.  The input geometry may not have a
                        // centroid if it is outside the bounds of the coordinate system.  GetArea() will
                        // return zero in these cases.
                        if (delta > 0.0)
                        {
                            Ptr<MgCoordinateCollection> coordinates = new MgCoordinateCollection();
                            Ptr<MgCoordinateXY> coord1 = new MgCoordinateXY(ctr->GetX() - delta, ctr->GetY() - delta);
                            coordinates->Add(coord1);
                            Ptr<MgCoordinateXY> coord2 = new MgCoordinateXY(ctr->GetX() - delta, ctr->GetY() + delta);
                            coordinates->Add(coord2);
                            Ptr<MgCoordinateXY> coord3 = new MgCoordinateXY(ctr->GetX() + delta, ctr->GetY() + delta);
                            coordinates->Add(coord3);
                            Ptr<MgCoordinateXY> coord4 = new MgCoordinateXY(ctr->GetX() + delta, ctr->GetY() - delta);
                            coordinates->Add(coord4);
                            coordinates->Add(coord1);
                            Ptr<MgLinearRing> outerRing = new MgLinearRing(coordinates);
                            Ptr<MgPolygon> polygon = new MgPolygon(outerRing, NULL);

                            // The selection area may extent past the map extents so clip the selection area to the map extent
                            Ptr<MgEnvelope> extent = map->GetMapExtent();
                            Ptr<MgCoordinate> llCoord = extent->GetLowerLeftCoordinate();
                            Ptr<MgCoordinate> urCoord = extent->GetUpperRightCoordinate();
                            Ptr<MgCoordinateCollection> extentCoords = new MgCoordinateCollection();
                            Ptr<MgCoordinateXY> c1 = new MgCoordinateXY(llCoord->GetX(), llCoord->GetY());
                            extentCoords->Add(c1);
                            Ptr<MgCoordinateXY> c2 = new MgCoordinateXY(llCoord->GetX(), urCoord->GetY());
                            extentCoords->Add(c2);
                            Ptr<MgCoordinateXY> c3 = new MgCoordinateXY(urCoord->GetX(), urCoord->GetY());
                            extentCoords->Add(c3);
                            Ptr<MgCoordinateXY> c4 = new MgCoordinateXY(urCoord->GetX(), llCoord->GetY());
                            extentCoords->Add(c4);
                            extentCoords->Add(c1);
                            Ptr<MgLinearRing> extentRing = new MgLinearRing(extentCoords);
                            Ptr<MgPolygon> extentPolygon = new MgPolygon(extentRing, NULL);

                            Ptr<MgGeometricEntity> queryExtentPolygon = extentPolygon;
                            if (trans)
                            {
                                //get selection geometry in layer space
                                queryExtentPolygon = extentPolygon->Transform(trans);
                            }
                            Ptr<MgGeometry> intersectPolygon = polygon->Intersection((MgPolygon *)queryExtentPolygon.p);

                            if (intersectPolygon != NULL)
                            {
                                options->SetSpatialFilter(layer->GetFeatureGeometryName(), intersectPolygon, /*MgFeatureSpatialOperations*/selectionVariant);

                                rdr = m_svcFeature->SelectFeatures(featResId, vl->GetFeatureName(), options);
                                rsrdr.reset(new RSMgdFeatureReader(rdr, m_svcFeature, featResId, options, vl->GetGeometry()));
                                selRenderer->PointTest(true);
                                ds.StylizeVectorLayer(vl, selRenderer, rsrdr.get(), xformer, scale, StylizeThatMany, selRenderer);

                                // Clear the readers
                                rdr = NULL;
                                rsrdr.reset();

                                selRenderer->PointTest(false);
                                numFeaturesProcessed = selRenderer->GetNumFeaturesProcessed();
                            }
                        }
                    }
                    selRenderer->EndLayer();

                    //update maxFeatures to number of features that
                    //we can select from subsequent layers
                    maxFeatures -= numFeaturesProcessed;
                }
            }
            catch (MgFdoException* e)
            {
                //TODO: what should we really be doing in this case?
                //This can happen if the underlying FDO provider does not
                //support a particular spatial operation. One way around this
                //is to select all features which appear on the screen and then
                //do our own geometry math.
                #ifdef _DEBUG
                STRING error = e->GetExceptionMessage();
                ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) RenderForSelection() - Error: %S\n"), error.c_str()));
                #endif

                // Let's throw the exception here, so that it can be recorded in the error log.
                throw e;
            }
        }
    }

    selRenderer->EndMap();
#ifdef _DEBUG
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) RenderForSelection(): ** END **\n")));
#endif

    MG_CATCH(L"MgServerRenderingService.RenderForSelection")

    MgdTransformCache::Clear(transformCache);

    MG_THROW()
}