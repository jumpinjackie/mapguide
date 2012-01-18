#include "RenderingService.h"
#include "System/ConfigProperties.h"
#include "AGGRenderer.h"
#include "GDRenderer.h"
#include "EPlotRenderer.h"
#include "DefaultStylizer.h"
#include "ImageFormats.h"
#include "FeatureTypeStyleVisitor.h"
#include "SymbolInstance.h"
#include "Rendering/icons.h"
#include "Rendering/RSMgSymbolManager.h"
#include "Rendering/RSMgFeatureReader.h"
#include "Rendering/FeatureInfoRenderer.h"
#include "Stylization/SEMgSymbolManager.h"
#include "StylizationUtil.h"
#include "Rendering/MappingUtil.h"
#include "Rendering/LegendPlotUtil.h"
#include "MapLayer/Map.h"
#include "Feature/TransformCache.h"
#include "ServiceFactory.h"
#include "Tile/TileDefs.h"
#include "Rendering/MapPlot.h"
#include "Rendering/MapPlotCollection.h"

///////////////////////////////////////////////////////////////////////////////
/// Mg Server Mapping Service try/catch/throw macros.
///
#define MG_SERVER_MAPPING_SERVICE_TRY()                                       \
    MG_TRY()                                                                  \

// For the Mapping service, gracefully handle and remap any Fdo exceptions to MapGuide exceptions.
// Some Fdo providiers like WMS return useful error strings when a request fails.
// Without the remap, this information is lost and MapGuide returns an unclassified exception.
#define MG_SERVER_MAPPING_SERVICE_CATCH(methodName)                         \
    }                                                                       \
    catch (MgException* e)                                                  \
                                                                            \
    {                                                                       \
        mgException = e;                                                    \
        mgException->AddStackTraceInfo(methodName, __LINE__, __WFILE__);    \
    }                                                                       \
    catch (exception& e)                                                    \
    {                                                                       \
        mgException = MgSystemException::Create(e, methodName, __LINE__, __WFILE__); \
    }                                                                       \
    catch (FdoException* e)                                                 \
    {                                                                       \
        STRING messageId;                                                   \
        MgStringCollection arguments;                                       \
        wchar_t* buf = (wchar_t*)e->GetExceptionMessage();                  \
        INT64 nativeErrorCode = e->GetNativeErrorCode();                    \
        if (NULL != buf)                                                    \
        {                                                                   \
            messageId = L"MgFormatInnerExceptionMessage";                   \
            arguments.Add(buf);                                             \
        }                                                                   \
        FDO_SAFE_RELEASE(e);                                                \
        mgException = new MgFdoException(methodName, __LINE__, __WFILE__, NULL, messageId, &arguments, nativeErrorCode); \
    }                                                                       \
    catch (...)                                                             \
    {                                                                       \
        mgException = new MgUnclassifiedException(methodName, __LINE__, __WFILE__, NULL, L"", NULL); \
    }                                                                       \

#define MG_SERVER_MAPPING_SERVICE_THROW()                                     \
    MG_THROW()                                                                \

#define MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(methodName)                 \
    MG_SERVER_MAPPING_SERVICE_CATCH(methodName)                               \
                                                                              \
    MG_SERVER_MAPPING_SERVICE_THROW()                                         \

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
    FeatureInfoRenderer* fir = (FeatureInfoRenderer*)data;
    if ( fir->GetNumFeaturesProcessed() >= fir->GetNumMaxFeatures())
        return true;

    return false;
}


///////////////////////////////////////////////////////////////////////////////
MgRenderingService::MgRenderingService() : MgService()
{
    m_pCSFactory = new MgCoordinateSystemFactory();

    // store references to the various services we use
    //MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    //assert(NULL != serviceMan);

    Ptr<MgServiceFactory> fact = new MgServiceFactory();

    m_svcResource = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcFeature = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
    assert(m_svcFeature != NULL);

    m_svcDrawing = dynamic_cast<MgDrawingService*>(fact->CreateService(MgServiceType::DrawingService));
    assert(m_svcDrawing != NULL);

    MgConfiguration* pConf = MgConfiguration::GetInstance();
    pConf->GetStringValue(MgConfigProperties::GeneralPropertiesSection,
                          MgConfigProperties::GeneralPropertyRenderer,
                          m_rendererName,
                          MgConfigProperties::DefaultGeneralPropertyRenderer);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyRasterGridSize,
                          m_rasterGridSize,
                          MgConfigProperties::DefaultRenderingServicePropertyRasterGridSize);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyMinRasterGridSize,
                          m_minRasterGridSize,
                          MgConfigProperties::DefaultRenderingServicePropertyMinRasterGridSize);

    pConf->GetDoubleValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatio,
                          m_rasterGridSizeOverrideRatio,
                          MgConfigProperties::DefaultRenderingServicePropertyRasterGridSizeOverrideRatio);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyRenderSelectionBatchSize,
                          m_renderSelectionBatchSize,
                          MgConfigProperties::DefaultRenderingServicePropertyRenderSelectionBatchSize);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyMaxRasterImageWidth,
                          m_maxRasterImageWidth,
                          MgConfigProperties::DefaultRenderingServicePropertyMaxRasterImageWidth);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyMaxRasterImageHeight,
                          m_maxRasterImageHeight,
                          MgConfigProperties::DefaultRenderingServicePropertyMaxRasterImageHeight);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyRasterGridSizeForPlot,
                          m_rasterGridSizeForPlot,
                          MgConfigProperties::DefaultRenderingServicePropertyRasterGridSizeForPlot);

    pConf->GetIntValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyMinRasterGridSizeForPlot,
                          m_minRasterGridSizeForPlot,
                          MgConfigProperties::DefaultRenderingServicePropertyMinRasterGridSizeForPlot);

    pConf->GetDoubleValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatioForPlot,
                          m_rasterGridSizeOverrideRatioForPlot,
                          MgConfigProperties::DefaultRenderingServicePropertyRasterGridSizeOverrideRatioForPlot);

    // there should only be one instance of this class, so it's safe to
    // directly set these static variables
    bool bClampPoints;
    pConf->GetBoolValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyClampPoints,
                          bClampPoints,
                          MgConfigProperties::DefaultRenderingServicePropertyClampPoints);
    AGGRenderer::s_bClampPoints = bClampPoints;

    bool bGeneralizeData;
    pConf->GetBoolValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyGeneralizeData,
                          bGeneralizeData,
                          MgConfigProperties::DefaultRenderingServicePropertyGeneralizeData);
    AGGRenderer::s_bGeneralizeData = bGeneralizeData;
    GDRenderer::s_bGeneralizeData = bGeneralizeData;
}


///////////////////////////////////////////////////////////////////////////////
MgRenderingService::~MgRenderingService()
{
}

// Returns true if the supplied feature type style is compatible with the
// supplied geometry type: 1=Point, 2=Line, 3=Area, 4=Composite
bool MgRenderingService::FeatureTypeStyleSupportsGeomType(MdfModel::FeatureTypeStyle* fts, INT32 geomType)
{
    if (fts == NULL)
        return false;

    // if ShowInLegend is false, the legend doesn't need to be shown
    if(!fts->IsShowInLegend())
        return false;

    // a value of -1 matches any
    if (geomType == -1)
        return true;

    FeatureTypeStyleVisitor::eFeatureTypeStyle ftsType = FeatureTypeStyleVisitor::DetermineFeatureTypeStyle(fts);

    switch (ftsType)
    {
        case FeatureTypeStyleVisitor::ftsPoint:
            return (geomType == 1);

        case FeatureTypeStyleVisitor::ftsLine:
            return (geomType == 2);

        case FeatureTypeStyleVisitor::ftsArea:
            return (geomType == 3);

        case FeatureTypeStyleVisitor::ftsComposite:
            return (geomType == 4);
    }

    return false;
}

MgByteReader* MgRenderingService::GenerateLegendImage(MgResourceIdentifier* resource,
                                                      double     scale,
                                                      INT32      imgWidth,
                                                      INT32      imgHeight,
                                                      CREFSTRING format,
                                                      INT32      geomType,
                                                      INT32      themeCategory)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GenerateLegendImage");

    if (0 == resource)
    {
        throw new MgNullArgumentException(
            L"MgRenderingService::GenerateLegendImage", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (m_svcResource == NULL)
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        m_svcResource = static_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
    }

    auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(m_svcResource, resource));
    MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
    MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf.get());
    MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf.get());

    if (vl) // vector layer
    {
        MdfModel::VectorScaleRangeCollection* src = vl->GetScaleRanges();

        MdfModel::VectorScaleRange* range = NULL;

        //find the right scale range
        for (int i=0; i<src->GetCount(); i++)
        {
            MdfModel::VectorScaleRange* r = src->GetAt(i);

            if (scale >= r->GetMinScale() && scale < r->GetMaxScale())
            {
                range = r;
                break;
            }
        }

        //we have the scale range... find the feature style
        if (range)
        {
            MdfModel::FeatureTypeStyle* fts = NULL;

            // The "geometry type" specifies which flavor of feature type style to use,
            // while the theme category is a 0-based value which specifies the actual
            // type style to use.  So if I have a scale range with two composite type
            // styles, the first with 3 rules and the second with 2 rules, then the
            // theme category can range from 0 to 4: 0,1,2 refer to the 3 rules in the
            // 1st type style, while 3,4 refer to the 2 rules in the 2nd type style.
            MdfModel::FeatureTypeStyleCollection* ftsc = range->GetFeatureTypeStyles();
            int numFTS = ftsc->GetCount();
            for (int j=0; j<numFTS; ++j)
            {
                // filter the type styles
                MdfModel::FeatureTypeStyle* temp = ftsc->GetAt(j);
                if (!FeatureTypeStyleSupportsGeomType(temp, geomType))
                    continue;

                // make sure we have rules
                RuleCollection* rules = temp->GetRules();
                if (!rules)
                    continue;

                // check if the theme category refers to one of these rules (note: a
                // value of -1 matches anything)
                int numRules = rules->GetCount();
                if (themeCategory >= -1 && themeCategory < numRules)
                {
                    // found it
                    fts = temp;
                    break;
                }

                // adjust the theme category, and move to the next type style
                themeCategory -= numRules;
            }

            if (fts)
                byteReader = MgMappingUtil::DrawFTS(m_svcResource, fts, imgWidth, imgHeight, themeCategory);
            else
            {
                //return the fixed array
                //MgByteSource will make its own copy of the data
                Ptr<MgByteSource> src = new MgByteSource((BYTE_ARRAY_IN)BLANK_LAYER_ICON, sizeof(BLANK_LAYER_ICON));
                byteReader = src->GetReader();
            }
        }
    }
    else if (dl) // drawing layer
    {
        //return the fixed array
        //MgByteSource will make its own copy of the data
        Ptr<MgByteSource> src = new MgByteSource((BYTE_ARRAY_IN)DWF_LAYER_ICON, sizeof(DWF_LAYER_ICON));
        byteReader = src->GetReader();
    }
    else if (gl) //grid (raster) layer
    {
        //return the fixed array
        //MgByteSource will make its own copy of the data
        Ptr<MgByteSource> src = new MgByteSource((BYTE_ARRAY_IN)RASTER_LAYER_ICON, sizeof(RASTER_LAYER_ICON));
        byteReader = src->GetReader();
    }

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgRenderingService::GenerateLegendImage")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////////
MgByteReader* MgRenderingService::RenderTile(MgdMap* map,
                                             CREFSTRING baseMapLayerGroupName,
                                             INT32 tileColumn,
                                             INT32 tileRow)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map || baseMapLayerGroupName.empty())
        throw new MgNullArgumentException(L"MgRenderingService.RenderTile", __LINE__, __WFILE__, NULL, L"", NULL);

    // find the finite display scale closest to the requested map scale
    double scale = map->GetViewScale();
    INT32 scaleIndex = map->FindNearestFiniteDisplayScaleIndex(scale);

    // if we don't find a nearest scale then something is wrong with the map
    if (scaleIndex < 0)
        throw new MgInvalidMapDefinitionException(L"MgRenderingService.RenderTile", __LINE__, __WFILE__, NULL, L"", NULL);

    // get the layer group associated with the name
    Ptr<MgLayerGroupCollection> layerGroups = map->GetLayerGroups();
    Ptr<MgLayerGroup> baseGroup = layerGroups->GetItem(baseMapLayerGroupName);
    if (baseGroup == NULL)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(baseMapLayerGroupName);

        throw new MgInvalidArgumentException(L"MgRenderingService.RenderTile",
            __LINE__, __WFILE__, &arguments, L"MgMapLayerGroupNameNotFound", NULL);
    }

    // get the scale at which to render the tile
    scale = map->GetFiniteDisplayScaleAt(scaleIndex);

    // ensure the tile DPI is set on the map
    map->SetDisplayDpi(MgTileParameters::tileDPI);

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
    double metersPerPixel = METERS_PER_INCH / MgTileParameters::tileDPI;
    double tileWidthMCS   = (double)MgTileParameters::tileWidth  * metersPerPixel * scale / metersPerUnit;
    double tileHeightMCS  = (double)MgTileParameters::tileHeight * metersPerPixel * scale / metersPerUnit;

    double tileMinX = mapMinX + (double)(tileColumn  ) * tileWidthMCS;  // left edge
    double tileMaxX = mapMinX + (double)(tileColumn+1) * tileWidthMCS;  // right edge
    double tileMinY = mapMaxY - (double)(tileRow   +1) * tileHeightMCS; // bottom edge
    double tileMaxY = mapMaxY - (double)(tileRow     ) * tileHeightMCS; // top edge

    // make the call to render the tile
    ret = RenderTile(map, baseGroup, scaleIndex, MgTileParameters::tileWidth, MgTileParameters::tileHeight, scale,
                     tileMinX, tileMaxX, tileMinY, tileMaxY, MgTileParameters::tileFormat);

    MG_CATCH_AND_THROW(L"MgRenderingService.RenderTile")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
/// render a map using all layers from the baseGroup
MgByteReader* MgRenderingService::RenderTile(MgdMap* map,
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
        throw new MgNullArgumentException(L"MgRenderingService.RenderTile", __LINE__, __WFILE__, NULL, L"", NULL);

    // get map extent that corresponds to tile extent
    RS_Bounds extent(mcsMinX, mcsMinY, mcsMaxX, mcsMaxY);

    // use the map's background color, but always make it fully transparent
    RS_Color bgColor;
    StylizationUtil::ParseColor(map->GetBackgroundColor(), bgColor);
    bgColor.alpha() = 0;

    // the label renderer needs to know the tile extent offset parameter
    double tileExtentOffset = 0.0;
    MgConfiguration* pConf = MgConfiguration::GetInstance();
    pConf->GetDoubleValue(MgConfigProperties::RenderingServicePropertiesSection,
                          MgConfigProperties::RenderingServicePropertyTileExtentOffset,
                          tileExtentOffset,
                          MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset);
    if (tileExtentOffset < 0.0)
        tileExtentOffset = MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset;

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

    MG_CATCH_AND_THROW(L"MgRenderingService.RenderTile")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgRenderingService::RenderDynamicOverlay(MgdMap* map,
                                                       MgdSelection* selection,
                                                       CREFSTRING format)
{
    // Call updated RenderDynamicOverlay API
    return RenderDynamicOverlay(map, selection, format, true);
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgRenderingService::RenderDynamicOverlay(MgdMap* map,
                                                       MgdSelection* selection,
                                                       CREFSTRING format,
                                                       bool bKeepSelection)
{
    // Call updated RenderDynamicOverlay API
    MgRenderingOptions options(format, MgRenderingOptions::RenderSelection |
        MgRenderingOptions::RenderLayers | (bKeepSelection? MgRenderingOptions::KeepSelection : 0), NULL);
    return RenderDynamicOverlay(map, selection, &options);
}


///////////////////////////////////////////////////////////////////////////////
// called from API (first call of AjaxPgPViewerSampleApplication)
MgByteReader* MgRenderingService::RenderDynamicOverlay(MgdMap* map,
                                                       MgdSelection* selection,
                                                       MgRenderingOptions* options)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgRenderingService.RenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    int width            = map->GetDisplayWidth();
    int height           = map->GetDisplayHeight();
    int dpi              = map->GetDisplayDpi();
    double scale         = map->GetViewScale();
    double metersPerUnit = map->GetMetersPerUnit();

    if (width <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"MgMapDisplayWidthCannotBeLessThanOrEqualToZero", NULL);

    if (height <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"MgMapDisplayHeightCannotBeLessThanOrEqualToZero", NULL);

    if (dpi <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"MgMapDisplayDpiCannotBeLessThanOrEqualToZero", NULL);

    if (scale <= 0.0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"MgMapViewScaleCannotBeLessThanOrEqualToZero", NULL);

    if (metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"MgMapMetersPerUnitCannotBeLessThanOrEqualToZero", NULL);

    // sanity check - number of image pixels cannot exceed MAX_PIXELS
    if (width * height > MAX_PIXELS)
        throw new MgOutOfRangeException(L"MgRenderingService.RenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

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

    // create a temporary collection containing all the dynamic layers
    Ptr<MgLayerCollection> layers = map->GetLayers();
    Ptr<MgReadOnlyLayerCollection> roLayers = new MgReadOnlyLayerCollection();
    for (int i=0; i<layers->GetCount(); i++)
    {
        Ptr<MgLayerBase> layer = layers->GetItem(i);
        Ptr<MgLayerGroup> layerGroup = layer->GetGroup();
        //DESKTOP-IMPL: Server doesn't need to do this because
        //state changes are saved and invisible layers are cleared
        //on each open
        if (!layer->IsVisible())
            continue;

        //DESKTOP-IMPL: Server doesn't need to do this because
        //state changes are saved and invisible layers are cleared
        //on each open
        if (NULL != (MgLayerGroup*)layerGroup && !layerGroup->IsVisible())
            continue;

        INT32 layerType = layer->GetLayerType();
        if (layerType == MgLayerType::Dynamic)
            roLayers->Add(layer);
    }

    // call the internal helper API to do all the stylization overhead work
    ret = RenderMapInternal(map, selection, roLayers, dr.get(), width, height, width, height, scale, extent, false, options, true);

    MG_CATCH_AND_THROW(L"MgRenderingService.RenderDynamicOverlay")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgRenderingService::RenderMap(MgdMap* map,
                                            MgdSelection* selection,
                                            CREFSTRING format)
{
    // Call updated RenderMap API
    return RenderMap(map, selection, format, true);
}


///////////////////////////////////////////////////////////////////////////////
// default arg bClip = false
MgByteReader* MgRenderingService::RenderMap(MgdMap* map,
                                            MgdSelection* selection,
                                            CREFSTRING format,
                                            bool bKeepSelection)
{
    return RenderMap(map, selection, format, bKeepSelection, false);
}


///////////////////////////////////////////////////////////////////////////////
// render complete map around center point in given scale using map's background
// color and display sizes as default arguments to call the real rendermap method
// default arg (bKeepSelection = true, bClip = false)
MgByteReader* MgRenderingService::RenderMap(MgdMap* map,
                                            MgdSelection* selection,
                                            CREFSTRING format,
                                            bool bKeepSelection,
                                            bool bClip)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgPoint> pt = map->GetViewCenter();
    Ptr<MgCoordinate> center = pt->GetCoordinate();
    double scale = map->GetViewScale();

    // use the map's background color
    RS_Color col;
    StylizationUtil::ParseColor(map->GetBackgroundColor(), col);
    Ptr<MgColor> bgColor = new MgColor(col.red(), col.green(), col.blue(), col.alpha());

    // punt to more specific RenderMap API
    ret = RenderMap(map, selection, center, scale, map->GetDisplayWidth(), map->GetDisplayHeight(), bgColor, format, bKeepSelection, bClip);

    MG_CATCH_AND_THROW(L"MgRenderingService.RenderMap")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgRenderingService::RenderMap(MgdMap* map,
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
MgByteReader* MgRenderingService::RenderMap(MgdMap* map,
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
        throw new MgNullArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    int dpi              = map->GetDisplayDpi();
    double metersPerUnit = map->GetMetersPerUnit();

    if (width <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgMapDisplayWidthCannotBeLessThanOrEqualToZero", NULL);

    if (height <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgMapDisplayHeightCannotBeLessThanOrEqualToZero", NULL);

    if (dpi <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgMapDisplayDpiCannotBeLessThanOrEqualToZero", NULL);

    if (metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgMapMetersPerUnitCannotBeLessThanOrEqualToZero", NULL);

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
        throw new MgOutOfRangeException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

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

    MG_CATCH_AND_THROW(L"MgRenderingService.RenderMap")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default argument bKeepSelection = true
MgByteReader* MgRenderingService::RenderMap(MgdMap* map,
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
// default arguments bClip = false
MgByteReader* MgRenderingService::RenderMap(MgdMap* map,
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
// render map around center point in given scale
// default args bKeepSelection = true, bClip = false, backgroundColor = map->backgroundColor,
// width = map->getDisplayWidth, height = map->getDisplayHeight
MgByteReader* MgRenderingService::RenderMap(MgdMap* map,
                                            MgdSelection* selection,
                                            MgCoordinate* center,
                                            double scale,
                                            INT32 width,
                                            INT32 height,
                                            MgColor* backgroundColor,
                                            CREFSTRING format,
                                            bool bKeepSelection,
                                            bool bClip)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map || NULL == center || NULL == backgroundColor)
        throw new MgNullArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    int dpi              = map->GetDisplayDpi();
    double metersPerUnit = map->GetMetersPerUnit();

    if (width <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgMapDisplayWidthCannotBeLessThanOrEqualToZero", NULL);

    if (height <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgMapDisplayHeightCannotBeLessThanOrEqualToZero", NULL);

    if (dpi <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgMapDisplayDpiCannotBeLessThanOrEqualToZero", NULL);

    if (scale <= 0.0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgMapViewScaleCannotBeLessThanOrEqualToZero", NULL);

    if (metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgMapMetersPerUnitCannotBeLessThanOrEqualToZero", NULL);

    // sanity check - number of image pixels cannot exceed MAX_PIXELS
    if (width * height > MAX_PIXELS)
        throw new MgOutOfRangeException(L"MgRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

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

    // call the internal helper API to do all the stylization overhead work
    ret = RenderMapInternal(map, selection, NULL, dr.get(), width, height, width, height, format, scale, b, false, bKeepSelection, true);

    MG_CATCH_AND_THROW(L"MgRenderingService.RenderMap")

    return ret.Detach();
}

///////////////////////////////////////////////////////////////////////////////
// pack options into object and forward call (select Selection AND Layers)
// maybe keepSelection called by RenderTile
MgByteReader* MgRenderingService::RenderMapInternal(MgdMap* map,
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
                                                    bool renderWatermark)
{
    MgRenderingOptions options(format, MgRenderingOptions::RenderSelection |
        MgRenderingOptions::RenderLayers | (bKeepSelection? MgRenderingOptions::KeepSelection : 0), NULL);
    return RenderMapInternal(map, selection, roLayers, dr, drawWidth, drawHeight, saveWidth, saveHeight, scale, b, expandExtents, &options, renderWatermark);
}

///////////////////////////////////////////////////////////////////////////////
// called from (indirectly):
//      RenderMap(complete), RenderMap(extent)(roLayers == NULL)
//      RenderTile(selection == NULL)
// render the map using the provided rolayers
// (this is the baseGroup layers for rendering tiles)
// render map using provided options object from before
// this is called for tiles and for dynamic overlays
MgByteReader* MgRenderingService::RenderMapInternal(MgdMap* map,
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
                                                    MgRenderingOptions* options,
                                                    bool renderWatermark)
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
    /*
    Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
    if (userInfo != NULL)
        sessionId = userInfo->GetMgSessionId();
    */

    // initialize the stylizer
    RSMgSymbolManager mgr(m_svcResource);
    dr->SetSymbolManager(&mgr);

    SEMgSymbolManager semgr(m_svcResource);
    DefaultStylizer ds(&semgr);

    RS_Color bgcolor(0, 0, 0, 255); //not used -- GDRenderer is already initialized to the correct bgcolor

    STRING format = options->GetImageFormat();

    RS_MapUIInfo mapInfo(sessionId, map->GetName(), map->GetObjectId(), srs, units, bgcolor);

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
                Ptr<MgLayerGroup> layerGroup = layer->GetGroup();
                //DESKTOP-IMPL: Server doesn't need to do this because
                //state changes are saved and invisible layers are cleared
                //on each open
                if (!layer->IsVisible())
                    continue;

                //DESKTOP-IMPL: Server doesn't need to do this because
                //state changes are saved and invisible layers are cleared
                //on each open
                if (NULL != (MgLayerGroup*)layerGroup && !layerGroup->IsVisible())
                    continue;

                tempLayers->Add(layer);
            }
        }

        INT32 behavior = options->GetBehavior();
        if (behavior & MgRenderingOptions::RenderLayers)    // this is for tiles so observer colormaps
        {
            MgMappingUtil::StylizeLayers(m_svcResource, m_svcFeature, m_svcDrawing, m_pCSFactory, map,
                                         tempLayers, NULL, &ds, dr, dstCs, expandExtents, false, scale,
                                         false, hasColorMap(format));
        }

        // now we need to stylize the selection on top (this is not for tiles!)
        if (selection && (behavior & MgRenderingOptions::RenderSelection))
        {
            Ptr<MgReadOnlyLayerCollection> selLayers = selection->GetLayers();

            #ifdef _DEBUG
            printf("MgRenderingService::RenderMapInternal() - Layers:%d  Selection Layers:%d\n", tempLayers.p? tempLayers->GetCount() : 0, selLayers.p? selLayers->GetCount() : 0);
            #endif

            if (selLayers.p && selLayers->GetCount() > 0)
            {
                // tell the renderer to override draw styles with the ones
                // we use for selection
                MgColor *selectionColor = options->GetSelectionColor();
                if (selectionColor == NULL)
                {
                    dr->SetRenderSelectionMode(true);
                }
                else
                {
                    dr->SetRenderSelectionMode(true,
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

                MgMappingUtil::StylizeLayers(m_svcResource, m_svcFeature, m_svcDrawing, m_pCSFactory, map,
                    modLayers, overrideFilters, &ds, dr, dstCs, false, false, scale, (behavior & MgRenderingOptions::KeepSelection) != 0);

                // Set selection mode to false to avoid affecting following code
                dr->SetRenderSelectionMode(false);
            }
        }

        if (renderWatermark && (behavior & MgRenderingOptions::RenderLayers) && map->GetWatermarkUsage() != 0)
        {
            // Rendering watermark only when:
            //      1. rendering layers
            //      2. not set renderWatermark to false (not render tile)
            //      3. Map's watermark usage is not 0, which means watermark usage is WMS and / or Viewer.
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
            const int layerCount = tempLayers->GetCount();
            auto_ptr<LayerDefinition> ldf;
            for (int i=0; i<layerCount; ++i)
            {
                Ptr<MgLayerBase> mapLayer(tempLayers->GetItem(i));

                // the layer resource content should be set during stylization if visible
                if (mapLayer->GetLayerResourceContent() == L"")
                    continue;

                ldf.reset(MgLayerBase::GetLayerDefinition(mapLayer->GetLayerResourceContent()));

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
                    ds.StylizeWatermark(dr, wdef, drawWidth, drawHeight, saveWidth, saveHeight);
                
                MG_CATCH(L"MgRenderingService.RenderMapInternal")
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
                        //MG_LOG_EXCEPTION_ENTRY(mgException->GetExceptionMessage(locale).c_str(), mgException->GetStackTrace(locale).c_str());

#if defined(_DEBUG) || defined(_DEBUG_PNG8)
                        STRING details = mgException->GetDetails(MgResources::DefaultMessageLocale);

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
                    }
                }
            }
        }

    MG_CATCH(L"MgRenderingService.RenderMapInternal")

    dr->EndMap();

    MG_THROW()  // to skip a faulty tile we need to rethrow the exception which could be thrown in StylizeLayers
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
    Ptr<MgByteReader> ret;

    // get a byte representation of the image
    auto_ptr<RS_ByteData> data;

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
                MgMappingUtil::ParseColorStrings(&tileColorPalette, map);
//              printf("<<<<<<<<<<<<<<<<<<<<< MgRenderingService::ColorPalette->size(): %d\n", tileColorPalette.size());
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
        Ptr<MgByteSource> bs = new MgByteSource(data->GetBytes(), data->GetNumBytes());

        if (format == MgImageFormats::Gif)
            bs->SetMimeType(MgMimeType::Gif);
        else if (format == MgImageFormats::Jpeg)
            bs->SetMimeType(MgMimeType::Jpeg);
        else if (format == MgImageFormats::Png || format == MgImageFormats::Png8)
            bs->SetMimeType(MgMimeType::Png);
        else if (format == MgImageFormats::Tiff)
            bs->SetMimeType(MgMimeType::Tiff);

        ret = bs->GetReader();
    }
    else
        throw new MgNullReferenceException(L"MgRenderingService.RenderMapInternal", __LINE__, __WFILE__, NULL, L"MgNoDataFromRenderer", NULL);

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
MgByteReader* MgRenderingService::RenderMapLegend(MgdMap* map,
                                                  INT32 width,
                                                  INT32 height,
                                                  MgColor* backgroundColor,
                                                  CREFSTRING format)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map || NULL == backgroundColor)
        throw new MgNullArgumentException(L"MgRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    if (width <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"MgMapDisplayWidthCannotBeLessThanOrEqualToZero", NULL);

    if (height <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"MgMapDisplayHeightCannotBeLessThanOrEqualToZero", NULL);

    // sanity check - number of image pixels cannot exceed MAX_PIXELS
    if (width * height > MAX_PIXELS)
        throw new MgOutOfRangeException(L"MgRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

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

    //We need to specify margins and offsets in an MgPlotSpecification,
    //even though in the image based (non-DWF) case they are 0.
    //Units are given as pixels, needed in order to scale legend
    //layout constants which are in inches.
    //NOTE: we need a left margin of one pixel so that legend icons
    //      do not overlap the legend border
    Ptr<MgPlotSpecification> spec = new MgPlotSpecification(width - 1.0f, height - 1.0f, L"pixels");
    spec->SetMargins(1.0f, 0.0f, 0.0f, 0.0f);
    MgLegendPlotUtil lu(m_svcResource);
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

        if (format == MgImageFormats::Gif)
            bs->SetMimeType(MgMimeType::Gif);
        else if (format == MgImageFormats::Jpeg)
            bs->SetMimeType(MgMimeType::Jpeg);
        else if (format == MgImageFormats::Png)
            bs->SetMimeType(MgMimeType::Png);
        else if (format == MgImageFormats::Tiff)
            bs->SetMimeType(MgMimeType::Tiff);

        ret = bs->GetReader();
    }

    MG_CATCH_AND_THROW(L"MgRenderingService.RenderMapLegend")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// A helper function that does most of the work for QueryFeatures
// and QueryFeatureProperties.  Basically runs a rendering loop with
// a custom renderer supplied by the caller that accumulates selection
// related things like property values and feature IDs.
void MgRenderingService::RenderForSelection(MgdMap* map,
                                            MgStringCollection* layerNames,
                                            MgGeometry* geometry,
                                            INT32 selectionVariant,
                                            CREFSTRING featureFilter,
                                            INT32 maxFeatures,
                                            INT32 layerAttributeFilter,
                                            FeatureInfoRenderer* selRenderer)
{
    // Cache coordinate system transforms for the life of the
    // stylization operation.
    TransformCacheMap transformCache;

    MG_TRY()

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) RenderForSelection(): ** START **\n")));
    if (NULL == map || (NULL == geometry && featureFilter.empty()))
        throw new MgNullArgumentException(L"MgRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"", NULL);

    if (maxFeatures < 0)
    {
        STRING buffer;
        MgUtil::Int32ToString(maxFeatures, buffer);

        MgStringCollection arguments;
        arguments.Add(L"5");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgRenderingService.RenderForSelection",
            __LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanZero", NULL);
    }

    // get the session ID
    STRING sessionId = map->GetSessionId();
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
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgMapDisplayWidthCannotBeLessThanOrEqualToZero", NULL);

    if (height <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgMapDisplayHeightCannotBeLessThanOrEqualToZero", NULL);

    if (dpi <= 0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgMapDisplayDpiCannotBeLessThanOrEqualToZero", NULL);

    if (scale <= 0.0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgMapViewScaleCannotBeLessThanOrEqualToZero", NULL);

    if (metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgMapMetersPerUnitCannotBeLessThanOrEqualToZero", NULL);

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
    RS_MapUIInfo mapInfo(sessionId, map->GetName(), map->GetObjectId(), srs, L"", bgcolor);

    // initialize the stylizer
    SEMgSymbolManager semgr(m_svcResource);
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

        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) RenderForSelection(): Layer: %W  Selectable:%W  Visible: %W\n"), layer->GetName().c_str(), layer->GetSelectable()? L"True" : L"False", layer->IsVisibleAtScale(scale)? L"True" : L"False"));

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
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) RenderForSelection(): Layer: %W  Vector Layer\n"), layer->GetName().c_str()));

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
            TransformCache* item = TransformCache::GetLayerToMapTransform(transformCache, vl->GetFeatureName(), featResId, mapCs, m_pCSFactory, m_svcFeature);
            Ptr<MgCoordinateSystemTransform> trans = item? item->GetMgTransform() : NULL;

            Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();
            Ptr<MgGeometricEntity> queryGeom;
            if (geometry != NULL)
            {
                //if we have a valid transform, get the request geom in layer's space
                queryGeom = SAFE_ADDREF(geometry);

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
            auto_ptr<RSMgFeatureReader> rsrdr;

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
                rsrdr.reset(new RSMgFeatureReader(rdr, m_svcFeature, featResId, options, vl->GetGeometry()));

                // Note that the FdoIFeatureReader smart pointer below is created
                // inside the following IF statement to ensure it gets destroyed
                // BEFORE the RSMgFeatureReader object above goes out of scope,
                // even when an exception gets thrown.
                if (FdoPtr<FdoIFeatureReader>(rsrdr->GetInternalReader()))
                {
                    //run a stylization loop with the FeatureInfoRenderer.
                    //This will build up the selection set and also
                    //evaluate the tooltip, hyperlink and feature properties
                    //for the first feature hit

                    RS_UIGraphic uig(NULL, 0, L"");
                    RS_LayerUIInfo layerinfo(layer->GetName(),
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
                    if (!vl->GetToolTip().empty()) layerinfo.hastooltips() = true;
                    if (vl->GetUrlData() && !vl->GetUrlData()->GetUrlContent().empty())
                    {
                        layerinfo.hashyperlinks() = true;
                    }

                    //set up the property name mapping -- it tells us what
                    //string the viewer should be displaying as the name of each
                    //feature property
                    // TODO: can FeatureName be an extension name rather than a FeatureClass?
                    RS_FeatureClassInfo fcinfo(vl->GetFeatureName());

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
                            Ptr<MgGeometry> intersectPolygon = polygon->Intersection(extentPolygon);

                            if (intersectPolygon != NULL)
                            {
                                options->SetSpatialFilter(layer->GetFeatureGeometryName(), intersectPolygon, /*MgFeatureSpatialOperations*/selectionVariant);

                                rdr = m_svcFeature->SelectFeatures(featResId, vl->GetFeatureName(), options);
                                rsrdr.reset(new RSMgFeatureReader(rdr, m_svcFeature, featResId, options, vl->GetGeometry()));
                                selRenderer->PointTest(true);
                                ds.StylizeVectorLayer(vl, selRenderer, rsrdr.get(), NULL, scale, StylizeThatMany, selRenderer);

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
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) RenderForSelection(): ** END **\n")));

    MG_CATCH(L"MgRenderingService.RenderForSelection")

    TransformCache::Clear(transformCache);

    MG_THROW()
}


///////////////////////////////////////////////////////////////////////////////
void MgRenderingService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for server-side service objects.
}


///////////////////////////////////////////////////////////////////////////////
SE_Renderer* MgRenderingService::CreateRenderer(int width,
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

MgByteReader* MgRenderingService::GeneratePlot(MgdMap* map,
                                               MgPlotSpecification* plotSpec,
                                               MgLayout* layout,
                                               MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GeneratePlot()");

    if (NULL == map  || NULL == dwfVersion || NULL == plotSpec )
    {
        throw new MgNullArgumentException(
            L"MgRenderingService::GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Create a MgMapPlot which will be passed to GenerateMultiPlot
    Ptr<MgMapPlot> mapPlot = new MgMapPlot(map, plotSpec, layout);

    // Add it to a MgMapPlotCollecion
    Ptr<MgMapPlotCollection> mapPlots = new MgMapPlotCollection();
    mapPlots->Add(mapPlot);

    // Create the plot
    byteReader = GenerateMultiPlot(mapPlots, dwfVersion);

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgRenderingService::GeneratePlot")

    return byteReader.Detach();
}

MgByteReader* MgRenderingService::GeneratePlot(MgdMap* map,
                                               MgCoordinate* center,
                                               double scale,
                                               MgPlotSpecification* plotSpec,
                                               MgLayout* layout,
                                               MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GeneratePlot()");

    if (NULL == map  || NULL == center || NULL == dwfVersion || NULL == plotSpec )
    {
        throw new MgNullArgumentException(
            L"MgRenderingService::GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Create a MgMapPlot which will be passed to GenerateMultiPlot
    Ptr<MgMapPlot> mapPlot = new MgMapPlot(map, center, scale, plotSpec, layout);

    // Add it to a MgMapPlotCollecion
    Ptr<MgMapPlotCollection> mapPlots = new MgMapPlotCollection();
    mapPlots->Add(mapPlot);

    // Create the plot
    byteReader = GenerateMultiPlot(mapPlots, dwfVersion);

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgRenderingService::GeneratePlot")

    return byteReader.Detach();
}

MgByteReader* MgRenderingService::GeneratePlot(MgdMap* map,
                                               MgEnvelope* extents,
                                               bool expandToFit,
                                               MgPlotSpecification* plotSpec,
                                               MgLayout* layout,
                                               MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GeneratePlot()");

    if (NULL == map  || NULL == extents || NULL == plotSpec || NULL == dwfVersion )
    {
        throw new MgNullArgumentException(
            L"MgRenderingService::GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Make a copy of the extents
    Ptr<MgCoordinate> oldll = extents->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> oldur = extents->GetUpperRightCoordinate();
    Ptr<MgCoordinate> ll = new MgCoordinateXY(oldll->GetX(), oldll->GetY());
    Ptr<MgCoordinate> ur = new MgCoordinateXY(oldur->GetX(), oldur->GetY());
    if (ll == NULL || ur == NULL)
    {
        throw new MgNullArgumentException(L"MgRenderingService::GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgEnvelope> env = new MgEnvelope(ll, ur);

    // Create a MgMapPlot which will be passed to GenerateMultiPlot
    Ptr<MgMapPlot> mapPlot = new MgMapPlot(map, env, expandToFit, plotSpec, layout);

    // Add it to a MgMapPlotCollecion
    Ptr<MgMapPlotCollection> mapPlots = new MgMapPlotCollection();
    if (mapPlot == NULL || mapPlots == NULL)
    {
        throw new MgNullArgumentException(L"MgRenderingService::GeneratePlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    mapPlots->Add(mapPlot);

    // Create the plot
    byteReader = GenerateMultiPlot(mapPlots, dwfVersion);

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgRenderingService::GeneratePlot")

    return byteReader.Detach();
}

MgByteReader* MgRenderingService::GenerateMultiPlot(MgMapPlotCollection* mapPlots,
                                                    MgDwfVersion* dwfVersion)
{
    Ptr<MgByteReader> byteReader;

    MG_SERVER_MAPPING_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerMappingService::GenerateMultiPlot()");

    if (0 == mapPlots || 0 == dwfVersion)
    {
        throw new MgNullArgumentException(
            L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // get a temporary file to write out EPlot DWF to
    // TODO: clean the temp file name prefix
    STRING dwfName = MgFileUtil::GenerateTempFileName(false, L"default_prefix");

    EPlotRenderer dr(dwfName.c_str(), 0, 0, L"inches");  // NOXLATE

    dr.SetRasterGridSize(m_rasterGridSizeForPlot);
    dr.SetMinRasterGridSize(m_minRasterGridSizeForPlot);
    dr.SetRasterGridSizeOverrideRatio(m_rasterGridSizeOverrideRatioForPlot);
    dr.SetMaxRasterImageWidth(m_maxRasterImageWidth);
    dr.SetMaxRasterImageHeight(m_maxRasterImageHeight);

    RSMgSymbolManager mgr(m_svcResource);
    dr.SetSymbolManager(&mgr);

    // process the MapPlot collection
    for (int nMapPlotIndex = 0; nMapPlotIndex < mapPlots->GetCount(); nMapPlotIndex++)
    {
        Ptr<MgMapPlot> mapPlot = mapPlots->GetItem(nMapPlotIndex);
        if (NULL == mapPlot)
        {
            throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgdMap> map = mapPlot->GetMap();
        if (NULL == map)
        {
            throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPlotSpecification> plotSpec = mapPlot->GetPlotSpecification();
        if (NULL == plotSpec)
        {
            throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgLayout> layout = mapPlot->GetLayout();

        double width = plotSpec->GetPaperWidth();
        double height = plotSpec->GetPaperHeight();
        STRING pageUnits = plotSpec->GetPageSizeUnits();

        dr.SetPageWidth(width);
        dr.SetPageHeight(height);
        dr.SetPageSizeUnits(pageUnits);
        dr.SetMapWidth(width);
        dr.SetMapHeight(height);
        dr.mapBoundsHeight() = height;

        // temporary place for the resId of the Map
        Ptr<MgResourceIdentifier> mapResId = map->GetMapDefinition();

        // request extenst
        Ptr<MgEnvelope> env = map->GetDataExtent();
        if (env == NULL)
        {
            throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgCoordinate> ll = env->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> ur = env->GetUpperRightCoordinate();
        if (ll == NULL || ur == NULL)
        {
            throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        RS_Bounds b(ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());

        //if requested data extent is not valid, use map definition extent
        // this may be removed eventually, but is a good sanity check nonetheless
        if (!b.IsValid())
        {
            Ptr<MgEnvelope> env2 = map->GetMapExtent();
            if (env2 == NULL)
            {
                throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            Ptr<MgCoordinate> ll2 = env2->GetLowerLeftCoordinate();
            Ptr<MgCoordinate> ur2 = env2->GetUpperRightCoordinate();
            if (ll2 == NULL || ur2 == NULL)
            {
                throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            b.minx = ll2->GetX();
            b.miny = ll2->GetY();
            b.maxx = ur2->GetX();
            b.maxy = ur2->GetY();
        }

        // Create a simple print layout containing only the map
        Ptr<MgPrintLayout> printLayout = new MgPrintLayout();
        if (printLayout == NULL)
        {
            throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (NULL != layout)
        {
            Ptr<MgResourceIdentifier> resID = layout->GetLayout();
            printLayout->Create(m_svcResource, resID);
            printLayout->SetPlotSize(plotSpec);
            printLayout->SetScaleBarUnits(layout->GetUnitType());
            printLayout->SetPlotTitle(layout->GetTitle());

            Ptr<MgPlotSpecification> spec = printLayout->GetPlotSize();

            width = spec->GetPaperWidth();
            height = spec->GetPaperHeight();
            pageUnits = spec->GetPageSizeUnits();
        }
        else
        {
            printLayout->SetPlotSize(plotSpec);
        }

        printLayout->PageWidth() = width;
        printLayout->PageHeight() = height;
        printLayout->Units() = pageUnits;

        dr.EnableLayoutPlot();

        //get the map coordinate system
        MdfModel::MdfString srs = map->GetMapSRS();
        Ptr<MgCoordinateSystem> dstCs;
        if (!srs.empty())
        {
            //let's not fail here if coord sys conversion fails for the
            //map's coordinate system. Instead we will fail per layer at a later stage
            try
            {
                dstCs = m_pCSFactory->Create(srs);
            }
            catch (MgInvalidCoordinateSystemException* e)
            {
                e->Release();
            }
        }

        double metersPerUnit = (dstCs.p) ? dstCs->ConvertCoordinateSystemUnitsToMeters(1.0) : 1.0;
        RS_String units = (dstCs.p) ? dstCs->GetUnits() : L"";

        double dMapScale = 0.0;
        Ptr<MgCoordinate> center = new MgCoordinateXY(0, 0);
        Ptr<MgEnvelope> extents = map->GetMapExtent();
        if (center == NULL || extents == NULL)
        {
            throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        switch (mapPlot->GetMapPlotInstruction())
        {
        case MgMapPlotInstruction::UseMapCenterAndScale:
            {
                dMapScale = map->GetViewScale();
                if (dMapScale <= 0)
                {
                    Ptr<MgEnvelope> extents = map->GetDataExtent();
                    if (extents == NULL)
                    {
                        throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                    printLayout->ComputeMapOffsetAndSize(dMapScale, extents, metersPerUnit, dr.mapOffsetX(), dr.mapOffsetY(), dr.mapWidth(), dr.mapHeight());
                    double mapWidth = dr.mapWidth();
                    double mapHeight = dr.mapHeight();
                    if (_wcsicmp(pageUnits.c_str(), L"mm") == 0 || _wcsicmp(pageUnits.c_str(), L"millimeters") == 0) // NOXLATE
                    {
                        mapWidth *= MM_TO_IN;
                        mapHeight *= MM_TO_IN;
                    }

                    double scaleToFitX = (extents->GetWidth() * metersPerUnit * M_TO_IN) / mapWidth;
                    double scaleToFitY = (extents->GetHeight() * metersPerUnit * M_TO_IN) / mapHeight;
                    dMapScale = rs_max(scaleToFitX, scaleToFitY);
                }

                Ptr<MgPoint> pt = map->GetViewCenter();
                center = pt->GetCoordinate();
                break;
            }
        case MgMapPlotInstruction::UseOverriddenCenterAndScale:
            {
                dMapScale = mapPlot->GetScale();
                center = mapPlot->GetCenter();
                break;
            }
        case MgMapPlotInstruction::UseOverriddenExtent:
            {
                // Compute the plotCenter and plotScale from the extents
                extents = mapPlot->GetExtent();
                //...plotCenter
                Ptr<MgCoordinate> plotll = extents->GetLowerLeftCoordinate();
                Ptr<MgCoordinate> plotur = extents->GetUpperRightCoordinate();
                if (plotll == NULL || plotur == NULL)
                {
                    throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
                }
                double minX = plotll->GetX();
                double minY = plotll->GetY();
                double maxX = plotur->GetX();
                double maxY = plotur->GetY();
                double centerX = minX + (maxX - minX) * 0.5;
                double centerY = minY + (maxY - minY) * 0.5;
                //
                //...plotScale
                printLayout->ComputeMapOffsetAndSize(dMapScale, extents, metersPerUnit, dr.mapOffsetX(), dr.mapOffsetY(), dr.mapWidth(), dr.mapHeight());
                double mapWidth = dr.mapWidth();
                double mapHeight = dr.mapHeight();
                if (_wcsicmp(pageUnits.c_str(), L"mm") == 0 || _wcsicmp(pageUnits.c_str(), L"millimeters") == 0)  // NOXLATE
                {
                    mapWidth *= MM_TO_IN;
                    mapHeight *= MM_TO_IN;
                }

                double scaleToFitX = (extents->GetWidth() * metersPerUnit * M_TO_IN) / mapWidth;
                double scaleToFitY = (extents->GetHeight() * metersPerUnit * M_TO_IN) / mapHeight;
                dMapScale = rs_max(scaleToFitX, scaleToFitY);

                map->SetViewScale(dMapScale);
                center = new MgCoordinateXY(centerX, centerY);
                if (center == NULL)
                {
                    throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
                }
                Ptr<MgPoint> centerpt = new MgPoint(center);
                if (centerpt == NULL)
                {
                    throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
                }
                map->SetViewCenter(centerpt);
                break;
            }
        default:
            {
                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(L"MgMapPlotCollection");

                throw new MgInvalidArgumentException(L"MgRenderingService::GenerateMultiPlot",
                    __LINE__, __WFILE__, &arguments, L"MgInvalidMapPlotCollectionMapPlotInstruction", NULL);
            }
            break;
        }

        printLayout->SetPlotCenter(center);
        printLayout->SetPlotScale(dMapScale);

        // Get the map background color
        RS_Color bgcolor;
        StylizationUtil::ParseColor( map->GetBackgroundColor(), bgcolor);

        // Get the layout background color
        RS_Color layoutColor;
        Ptr<MgColor> bgColor = printLayout->GetBackgroundColor();
        layoutColor.red() = bgColor->GetRed();
        layoutColor.green() = bgColor->GetGreen();
        layoutColor.blue() = bgColor->GetBlue();

        // Get the session ID
        STRING sessionId;
        /*Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
        if (userInfo != NULL)
            sessionId = userInfo->GetMgSessionId();*/

        RS_MapUIInfo mapInfo(sessionId, map->GetName(), L"", srs, units, layoutColor);

        // Dynamically adjust the width and height of the map
        printLayout->ComputeMapOffsetAndSize(dMapScale, extents, metersPerUnit, dr.mapOffsetX(), dr.mapOffsetY(), dr.mapWidth(), dr.mapHeight(), mapPlot->GetExpandToFit());

        Ptr<MgCoordinate> newll;
        Ptr<MgCoordinate> newur;
        if (mapPlot->GetMapPlotInstruction() != MgMapPlotInstruction::UseOverriddenExtent || mapPlot->GetExpandToFit())
        {
            double mapWidth = dr.mapWidth();
            double mapHeight = dr.mapHeight();
            if (_wcsicmp(pageUnits.c_str(), L"mm") == 0 || _wcsicmp(pageUnits.c_str(), L"millimeters") == 0)  // NOXLATE
            {
                mapWidth *= MM_TO_IN;
                mapHeight *= MM_TO_IN;
            }

            env = printLayout->DetermineLayoutMapExtents(map, metersPerUnit, mapWidth, mapHeight);

            newll = env->GetLowerLeftCoordinate();
            newur = env->GetUpperRightCoordinate();
        }
        else if (mapPlot->GetMapPlotInstruction() == MgMapPlotInstruction::UseOverriddenExtent && !mapPlot->GetExpandToFit())
        {
            newll = extents->GetLowerLeftCoordinate();
            newur = extents->GetUpperRightCoordinate();
        }

        b.minx = newll->GetX();
        b.miny = newll->GetY();
        b.maxx = newur->GetX();
        b.maxy = newur->GetY();

        SEMgSymbolManager semgr(m_svcResource);
        DefaultStylizer ds(&semgr);

        double dpi = map->GetDisplayDpi();
        dr.StartMap(&mapInfo, b, dMapScale, dpi, metersPerUnit);

        Ptr<MgLayerCollection> layers = map->GetLayers();
        if (layers == NULL)
        {
            throw new MgNullReferenceException(L"MgRenderingService::GenerateMultiPlot", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Define a polygon to represent the map extents and fill it with the map background color
        dr.StartLayer(NULL, NULL);
        LineBuffer lb(5);
        lb.MoveTo(b.minx, b.miny);
        lb.LineTo(b.maxx, b.miny);
        lb.LineTo(b.maxx, b.maxy);
        lb.LineTo(b.minx, b.maxy);
        lb.Close();

        RS_LineStroke lineStroke;
        RS_FillStyle fillStyle(lineStroke, bgcolor, layoutColor, L"Solid");  // NOXLATE
        dr.ProcessPolygon(&lb, fillStyle);
        dr.EndLayer();

        //transfer layers to a temporary collection
        //here it doesn't matter but in the rendering service it does
        Ptr<MgReadOnlyLayerCollection> rolc = new MgReadOnlyLayerCollection();

        for (int u=0; u<layers->GetCount(); u++)
        {
            Ptr<MgLayerBase> lr = layers->GetItem(u);
            rolc->Add(lr);
        }

        //stylize all the map layers
        MgMappingUtil::StylizeLayers(m_svcResource, m_svcFeature, m_svcDrawing, m_pCSFactory, map,
                                     rolc, NULL, &ds, &dr, dstCs, false, false, dMapScale);

        // Finish adding the map to the page
        // Calculate the the height of the map bounds on the page (in page units)
        dr.mapBoundsHeight() = b.height() * dr.mapWidth()/b.width();

        //construct one every time -- not really a bottleneck
        MgLegendPlotUtil lu(m_svcResource);

        // Now add the rest of the layout element to the page
        lu.AddLayoutElements(printLayout, (STRING)mapInfo.name(), mapResId->ToString(), map, layers, b, dMapScale, metersPerUnit, dr);

        dr.EndMap();
    }

    dr.Done();

    // write out the file
    Ptr<MgByteSource> bs = new MgByteSource(dwfName, true);

    bs->SetMimeType(MgMimeType::Dwf);
    byteReader = bs->GetReader();

    MG_SERVER_MAPPING_SERVICE_CATCH_AND_THROW(L"MgRenderingService::GenerateMultiPlot")

    return byteReader.Detach();
}