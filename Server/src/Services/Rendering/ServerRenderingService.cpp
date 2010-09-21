//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "ServerRenderingService.h"
#include "DefaultStylizer.h"
#include "GDRenderer.h"
#include "AGGRenderer.h"
#include "RSMgSymbolManager.h"
#include "RSMgFeatureReader.h"
#include "FeatureInfoRenderer.h"
#include "SEMgSymbolManager.h"
#include "StylizationUtil.h"
#include "MappingUtil.h"
#include "LegendPlotUtil.h"
#include "TransformCache.h"


// the maximum number of allowed pixels for rendered images
static const INT32 MAX_PIXELS = 16384*16384;
static const INT32 FILTER_VISIBLE = 1;
static const INT32 FILTER_SELECTABLE = 2;
static const INT32 FILTER_HASTOOLTIPS = 4;

inline bool hasColorMap (STRING format)
{
    return format == L"PNG8" || format == L"GIF";
}

IMPLEMENT_CREATE_SERVICE(MgServerRenderingService)


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
MgServerRenderingService::MgServerRenderingService() : MgRenderingService()
{
    m_pCSFactory = new MgCoordinateSystemFactory();

    // store references to the various services we use
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcResource = dynamic_cast<MgResourceService*>(serviceMan->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcFeature = dynamic_cast<MgFeatureService*>(serviceMan->RequestService(MgServiceType::FeatureService));
    assert(m_svcFeature != NULL);

    m_svcDrawing = dynamic_cast<MgDrawingService*>(serviceMan->RequestService(MgServiceType::DrawingService));
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
MgServerRenderingService::~MgServerRenderingService()
{
}


///////////////////////////////////////////////////////////////////////////////
MgByteReader* MgServerRenderingService::RenderTile(MgMap* map,
                                                   CREFSTRING baseMapLayerGroupName,
                                                   INT32 tileColumn,
                                                   INT32 tileRow)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map || baseMapLayerGroupName.empty())
        throw new MgNullArgumentException(L"MgServerRenderingService.RenderTile", __LINE__, __WFILE__, NULL, L"", NULL);

    // find the finite display scale closest to the requested map scale
    double scale = map->GetViewScale();
    INT32 scaleIndex = map->FindNearestFiniteDisplayScaleIndex(scale);

    // if we don't find a nearest scale then something is wrong with the map
    if (scaleIndex < 0)
        throw new MgInvalidMapDefinitionException(L"MgServerRenderingService.RenderTile", __LINE__, __WFILE__, NULL, L"", NULL);

    // get the layer group associated with the name
    Ptr<MgLayerGroupCollection> layerGroups = map->GetLayerGroups();
    Ptr<MgLayerGroup> baseGroup = layerGroups->GetItem(baseMapLayerGroupName);
    if (baseGroup == NULL)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(baseMapLayerGroupName);

        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderTile",
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

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderTile")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
/// render a map using all layers from the baseGroup
MgByteReader* MgServerRenderingService::RenderTile(MgMap* map,
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
        throw new MgNullArgumentException(L"MgServerRenderingService.RenderTile", __LINE__, __WFILE__, NULL, L"", NULL);

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

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderTile")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgServerRenderingService::RenderDynamicOverlay(MgMap* map,
                                                             MgSelection* selection,
                                                             CREFSTRING format)
{
    // Call updated RenderDynamicOverlay API
    return RenderDynamicOverlay(map, selection, format, true);
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgServerRenderingService::RenderDynamicOverlay(MgMap* map,
                                                             MgSelection* selection,
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
MgByteReader* MgServerRenderingService::RenderDynamicOverlay(MgMap* map,
                                                             MgSelection* selection,
                                                             MgRenderingOptions* options)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgServerRenderingService.RenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    int width            = map->GetDisplayWidth();
    int height           = map->GetDisplayHeight();
    int dpi              = map->GetDisplayDpi();
    double scale         = map->GetViewScale();
    double metersPerUnit = map->GetMetersPerUnit();
    if (width <= 0 || height <= 0 || dpi <= 0 || scale <= 0.0 || metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"MgValueCannotBeLessThanOrEqualToZero", NULL);

    // sanity check - number of image pixels cannot exceed MAX_PIXELS
    if (width * height > MAX_PIXELS)
        throw new MgOutOfRangeException(L"MgServerRenderingService.RenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

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
        INT32 layerType = layer->GetLayerType();
        if (layerType == MgLayerType::Dynamic)
            roLayers->Add(layer);
    }

    // call the internal helper API to do all the stylization overhead work
    ret = RenderMapInternal(map, selection, roLayers, dr.get(), width, height, width, height, scale, extent, false, options, true);

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderDynamicOverlay")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgServerRenderingService::RenderMap(MgMap* map,
                                                  MgSelection* selection,
                                                  CREFSTRING format)
{
    // Call updated RenderMap API
    return RenderMap(map, selection, format, true);
}


///////////////////////////////////////////////////////////////////////////////
// default arg bClip = false
MgByteReader* MgServerRenderingService::RenderMap(MgMap* map,
                                                  MgSelection* selection,
                                                  CREFSTRING format,
                                                  bool bKeepSelection)
{
    return RenderMap(map, selection, format, bKeepSelection, false);
}


///////////////////////////////////////////////////////////////////////////////
// render complete map around center point in given scale using map's background
// color and display sizes as default arguments to call the real rendermap method
// default arg (bKeepSelection = true, bClip = false)
MgByteReader* MgServerRenderingService::RenderMap(MgMap* map,
                                                  MgSelection* selection,
                                                  CREFSTRING format,
                                                  bool bKeepSelection,
                                                  bool bClip)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgServerRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgPoint> pt = map->GetViewCenter();
    Ptr<MgCoordinate> center = pt->GetCoordinate();
    double scale = map->GetViewScale();

    // use the map's background color
    RS_Color col;
    StylizationUtil::ParseColor(map->GetBackgroundColor(), col);
    Ptr<MgColor> bgColor = new MgColor(col.red(), col.green(), col.blue(), col.alpha());

    // punt to more specific RenderMap API
    ret = RenderMap(map, selection, center, scale, map->GetDisplayWidth(), map->GetDisplayHeight(), bgColor, format, bKeepSelection, bClip);

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderMap")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default arg bKeepSelection = true
MgByteReader* MgServerRenderingService::RenderMap(MgMap* map,
                                                  MgSelection* selection,
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
MgByteReader* MgServerRenderingService::RenderMap(MgMap* map,
                                                  MgSelection* selection,
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
        throw new MgNullArgumentException(L"MgServerRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    int dpi              = map->GetDisplayDpi();
    double metersPerUnit = map->GetMetersPerUnit();
    if (width <= 0 || height <= 0 || dpi <= 0 || metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgValueCannotBeLessThanOrEqualToZero", NULL);

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
        throw new MgOutOfRangeException(L"MgServerRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

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

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderMap")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// default argument bKeepSelection = true
MgByteReader* MgServerRenderingService::RenderMap(MgMap* map,
                                                  MgSelection* selection,
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
MgByteReader* MgServerRenderingService::RenderMap(MgMap* map,
                                                  MgSelection* selection,
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
MgByteReader* MgServerRenderingService::RenderMap(MgMap* map,
                                                  MgSelection* selection,
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
        throw new MgNullArgumentException(L"MgServerRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    int dpi              = map->GetDisplayDpi();
    double metersPerUnit = map->GetMetersPerUnit();
    if (scale <= 0.0 || width <= 0 || height <= 0 || dpi <= 0 || metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgValueCannotBeLessThanOrEqualToZero", NULL);

    // sanity check - number of image pixels cannot exceed MAX_PIXELS
    if (width * height > MAX_PIXELS)
        throw new MgOutOfRangeException(L"MgServerRenderingService.RenderMap", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

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

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderMap")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
MgFeatureInformation* MgServerRenderingService::QueryFeatures(MgMap* map,
                                                              MgStringCollection* layerNames,
                                                              MgGeometry* geometry,
                                                              INT32 selectionVariant, // Within, Touching, Topmost
                                                              INT32 maxFeatures)
{
    // Call updated QueryFeatures API
    return QueryFeatures(map, layerNames, geometry, selectionVariant, L"", maxFeatures, 3 /*visible and selectable*/);
}


///////////////////////////////////////////////////////////////////////////////
MgFeatureInformation* MgServerRenderingService::QueryFeatures(MgMap* map,
                                                              MgStringCollection* layerNames,
                                                              MgGeometry* geometry,
                                                              INT32 selectionVariant, // Within, Touching, Topmost
                                                              CREFSTRING featureFilter,
                                                              INT32 maxFeatures,
                                                              INT32 layerAttributeFilter)
{
    Ptr<MgFeatureInformation> ret;

    MG_TRY()

    //detect case where there is no limit to selection
    if (maxFeatures == -1)
        maxFeatures = INT_MAX;

    //create return structure and selection set to fill out
    ret = new MgFeatureInformation();
    Ptr<MgSelection> sel = new MgSelection(map);

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

    FeatureInfoRenderer fir(sel, maxFeatures, map->GetViewScale(), point, impRenderer.get());

    RenderForSelection(map, layerNames, geometry, selectionVariant, featureFilter, maxFeatures, layerAttributeFilter, &fir);

    //fill out the output object with the info we collected
    //in the FeatureInfoRenderer for the first feature we hit
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
    ACE_DEBUG((LM_ERROR, ACE_TEXT("MgServerRenderingService::QueryFeatures() Selection Size:%d\n"), selLayers.p? selLayers->GetCount() : 0));
    #endif

    MG_CATCH_AND_THROW(L"MgServerRenderingService.QueryFeatures")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
MgBatchPropertyCollection* MgServerRenderingService::QueryFeatureProperties(MgMap* map,
                                                                            MgStringCollection* layerNames,
                                                                            MgGeometry* filterGeometry,
                                                                            INT32 selectionVariant, // Within, Touching, Topmost
                                                                            INT32 maxFeatures)
{
    // Call updated QueryFeatureProperties API
    return QueryFeatureProperties(map, layerNames, filterGeometry, selectionVariant, L"", maxFeatures, 3 /*visible and selectable*/);
}


///////////////////////////////////////////////////////////////////////////////
MgBatchPropertyCollection* MgServerRenderingService::QueryFeatureProperties(MgMap* map,
                                                                            MgStringCollection* layerNames,
                                                                            MgGeometry* filterGeometry,
                                                                            INT32 selectionVariant, // Within, Touching, Topmost
                                                                            CREFSTRING featureFilter,
                                                                            INT32 maxFeatures,
                                                                            INT32 layerAttributeFilter)
{
    Ptr<MgBatchPropertyCollection> ret;

    MG_TRY()

    //detect case where there is no limit to selection
    if (maxFeatures == -1)
        maxFeatures = INT_MAX;

    Ptr<MgSelection> sel;   //TODO: do we need this for this API? new MgSelection(map);
    FeaturePropRenderer fpr(sel, maxFeatures, map->GetViewScale());

    RenderForSelection(map, layerNames, filterGeometry, selectionVariant, featureFilter, maxFeatures, layerAttributeFilter, &fpr);

    ret = fpr.GetProperties();
    //ret->SetSelection(sel);

    MG_CATCH_AND_THROW(L"MgServerRenderingService.QueryFeatures")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// pack options into object and forward call (select Selection AND Layers)
// maybe keepSelection called by RenderTile
MgByteReader* MgServerRenderingService::RenderMapInternal(MgMap* map,
                                                          MgSelection* selection,
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
MgByteReader* MgServerRenderingService::RenderMapInternal(MgMap* map,
                                                          MgSelection* selection,
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
    STRING sessionId;
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
    if (userInfo != NULL)
        sessionId = userInfo->GetMgSessionId();

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
            printf("MgServerRenderingService::RenderMapInternal() - Layers:%d  Selection Layers:%d\n", tempLayers.p? tempLayers->GetCount() : 0, selLayers.p? selLayers->GetCount() : 0);
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
            }
        }

        if (renderWatermark)
        {
            // Rendering watermark

            MgStringCollection watermarkIds;      //ID list to load watermark definition
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
                    if (((map->GetWatermarkUsage() & MgMap::Viewer) != 0
                        && (tempInstance->GetUsage() & WatermarkInstance::Viewer) == 0) 
                        || ((map->GetWatermarkUsage() & MgMap::WMS) != 0
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
                        watermarkIds.Add(tempInstance->GetResourceId().c_str());
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

                // the layer resource content should be set during stylization
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
                    if (((map->GetWatermarkUsage() & MgMap::Viewer) != 0
                        && (tempInstance->GetUsage() & WatermarkInstance::Viewer) == 0) 
                        || ((map->GetWatermarkUsage() & MgMap::WMS) != 0
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
                        watermarkIds.Add(tempInstance->GetResourceId().c_str());
                        watermarkInstances.Adopt(tempInstance.release());
                    }
                }
            }
            assert(tempWatermarkInstances.GetCount() == 0);

            // load watermark source
            if (watermarkIds.GetCount() != 0)
            {
                Ptr<MgStringCollection> wdefs = m_svcResource->GetResourceContents(&watermarkIds, NULL);
                for (int i=watermarkIds.GetCount()-1; i>=0; i--)
                {
                    for (int j=watermarkInstances.GetCount()-1; j>=0; j--)
                    {
                        WatermarkInstance* instance = watermarkInstances.GetAt(j);
                        if (instance->GetResourceId() == watermarkIds.GetItem(i))
                        {
                            instance->AdoptWatermarkDefinition(MgWatermark::GetWatermarkDefinition(wdefs->GetItem(i)));
                        }
                    }
                }
            }

            for (int i=watermarkInstances.GetCount()-1; i>=0; i--)
            {
                WatermarkInstance* instance = watermarkInstances.GetAt(i);
                WatermarkDefinition* wdef = instance->GetWatermarkDefinition();
                if (instance->GetPositionOverride())
                {
                    wdef->AdoptPosition(instance->OrphanPositionOverride());
                }
                if (instance->GetAppearanceOverride())
                {
                    wdef->AdoptAppearance(instance->GetAppearanceOverride());
                }
                ds.StylizeWatermark(dr, wdef, drawWidth, drawHeight, saveWidth, saveHeight);
            }
        }

    MG_CATCH(L"MgServerRenderingService.RenderMapInternal")

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
//              printf("<<<<<<<<<<<<<<<<<<<<< MgServerRenderingService::ColorPalette->size(): %d\n", tileColorPalette.size());
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
        throw new MgNullReferenceException(L"MgServerRenderingService.RenderMapInternal", __LINE__, __WFILE__, NULL, L"MgNoDataFromRenderer", NULL);

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
MgByteReader* MgServerRenderingService::RenderMapLegend(MgMap* map,
                                                        INT32 width,
                                                        INT32 height,
                                                        MgColor* backgroundColor,
                                                        CREFSTRING format)
{
    Ptr<MgByteReader> ret;

    MG_TRY()

    if (NULL == map || NULL == backgroundColor)
        throw new MgNullArgumentException(L"MgServerRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"", NULL);

    // validate map view parameters
    if (width <= 0 || height <= 0)
        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"MgValueCannotBeLessThanOrEqualToZero", NULL);

    // sanity check - number of image pixels cannot exceed MAX_PIXELS
    if (width * height > MAX_PIXELS)
        throw new MgOutOfRangeException(L"MgServerRenderingService.RenderMapLegend", __LINE__, __WFILE__, NULL, L"MgInvalidImageSizeTooBig", NULL);

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

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderMapLegend")

    return ret.Detach();
}


///////////////////////////////////////////////////////////////////////////////
// A helper function that does most of the work for QueryFeatures
// and QueryFeatureProperties.  Basically runs a rendering loop with
// a custom renderer supplied by the caller that accumulates selection
// related things like property values and feature IDs.
void MgServerRenderingService::RenderForSelection(MgMap* map,
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

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("RenderForSelection(): ** START **\n")));
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
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
    if (userInfo != NULL)
        sessionId = userInfo->GetMgSessionId();

    // validate map view parameters
    int width            = map->GetDisplayWidth();
    int height           = map->GetDisplayHeight();
    int dpi              = map->GetDisplayDpi();
    double scale         = map->GetViewScale();
    double metersPerUnit = map->GetMetersPerUnit();
    if (width <= 0 || height <= 0 || dpi <= 0 || scale <= 0.0 || metersPerUnit <= 0.0)
        throw new MgInvalidArgumentException(L"MgServerRenderingService.RenderForSelection", __LINE__, __WFILE__, NULL, L"MgValueCannotBeLessThanOrEqualToZero", NULL);

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

        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("RenderForSelection(): Layer: %W  Selectable:%W  Visible: %W\n"), layer->GetName().c_str(), layer->GetSelectable()? L"True" : L"False", layer->IsVisibleAtScale(scale)? L"True" : L"False"));

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
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("RenderForSelection(): Layer: %W  Vector Layer\n"), layer->GetName().c_str()));

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
                ACE_DEBUG((LM_ERROR, ACE_TEXT("SELECTION FILTER:\n%W\n\n"), geomText.c_str()));
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
                    MgSelection selectionFilter(map, featureFilter);
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
                    if (!vl->GetUrl().empty()) layerinfo.hashyperlinks() = true;

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
                ACE_DEBUG((LM_ERROR, ACE_TEXT("RenderForSelection() - Error: %S\n"), error.c_str()));
                #endif

                // Let's throw the exception here, so that it can be recorded in the error log.
                throw e;
            }
        }
    }

    selRenderer->EndMap();
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("RenderForSelection(): ** END **\n")));

    MG_CATCH(L"MgServerRenderingService.RenderForSelection")

    TransformCache::Clear(transformCache);

    MG_THROW()
}


///////////////////////////////////////////////////////////////////////////////
void MgServerRenderingService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for server-side service objects.
}


///////////////////////////////////////////////////////////////////////////////
SE_Renderer* MgServerRenderingService::CreateRenderer(int width,
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
