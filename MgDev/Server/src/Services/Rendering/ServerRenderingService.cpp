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

#include "MapGuideCommon.h"
#include "ServerRenderingService.h"
#include "Renderer.h"
#include "Stylizer.h"
#include "DefaultStylizer.h"
#include "GDRenderer.h"
#include "RSMgSymbolManager.h"
#include "RSMgFeatureReader.h"
#include "FeatureInfoRenderer.h"
#include "SEMgSymbolManager.h"
#include "StylizationUtil.h"
#include "LegendPlotUtil.h"


// the maximum number of allowed pixels for rendered images
static const INT32 MAX_PIXELS = 16384*16384;

IMPLEMENT_CREATE_SERVICE(MgServerRenderingService)

// used when we want to process a given number of features
bool StylizeThatMany(void* data)
{
    FeatureInfoRenderer* fir = (FeatureInfoRenderer*)data;
    if ( fir->GetNumFeaturesProcessed() >= fir->GetNumMaxFeatures())
        return true;

    return false;
}


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
}


MgServerRenderingService::~MgServerRenderingService()
{
}


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
    double mapMinX = min(pt00->GetX(), pt11->GetX());
    double mapMaxX = max(pt00->GetX(), pt11->GetX());
    double mapMinY = min(pt00->GetY(), pt11->GetY());
    double mapMaxY = max(pt00->GetY(), pt11->GetY());

    double metersPerUnit  = map->GetMetersPerUnit();
    double metersPerPixel = 0.0254 / MgTileParameters::tileDPI;
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
    MgStylizationUtil::ParseColor(map->GetBackgroundColor(), bgColor);
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
    GDRenderer dr(width, height, bgColor, false, true, tileExtentOffset);

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
    ret = RenderMapInternal(map, NULL, roLayers, &dr, width, height, format, scale, extent, true, true);

    // restore the base group's visibility
    baseGroup->SetVisible(groupVisible);

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderTile")

    return ret.Detach();
}


MgByteReader* MgServerRenderingService::RenderDynamicOverlay(MgMap* map,
                                                             MgSelection* selection,
                                                             CREFSTRING format)
{
    // Call updated RenderDynamicOverlay API
    return RenderDynamicOverlay(map, selection, format, true);
}

MgByteReader* MgServerRenderingService::RenderDynamicOverlay(MgMap* map,
                                                             MgSelection* selection,
                                                             CREFSTRING format,
                                                             bool bKeepSelection)
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
    double unitsPerPixel     = 0.0254 / (double)dpi / metersPerUnit;
    double mapWidth2         = 0.5 * (double)width  * unitsPerPixel * scale;
    double mapHeight2        = 0.5 * (double)height * unitsPerPixel * scale;

    RS_Bounds extent(center->GetX() - mapWidth2,
                     center->GetY() - mapHeight2,
                     center->GetX() + mapWidth2,
                     center->GetY() + mapHeight2);

    // use the map's background color, but always make it fully transparent
    RS_Color bgColor;
    MgStylizationUtil::ParseColor(map->GetBackgroundColor(), bgColor);
    bgColor.alpha() = 0;

    // initialize the renderer
    GDRenderer dr(width, height, bgColor, true);

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
    ret = RenderMapInternal(map, selection, roLayers, &dr, width, height, format, scale, extent, false, bKeepSelection);

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderDynamicOverlay")

    return ret.Detach();
}


MgByteReader* MgServerRenderingService::RenderMap(MgMap* map,
                                                  MgSelection* selection,
                                                  CREFSTRING format)
{
    // Call updated RenderMap API
    return RenderMap(map, selection, format, true);
}

MgByteReader* MgServerRenderingService::RenderMap(MgMap* map,
                                                  MgSelection* selection,
                                                  CREFSTRING format,
                                                  bool bKeepSelection)
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
    MgStylizationUtil::ParseColor(map->GetBackgroundColor(), col);
    Ptr<MgColor> bgColor = new MgColor(col.red(), col.green(), col.blue(), col.alpha());

    // punt to more specific RenderMap API
    ret = RenderMap(map, selection, center, scale, map->GetDisplayWidth(), map->GetDisplayHeight(), bgColor, format, bKeepSelection);

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderMap")

    return ret.Detach();
}


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
        scale = b.width() * metersPerUnit * 100.0 / 2.54 * (double)dpi / (double)width;

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
        scale = b.height() * metersPerUnit * 100.0 / 2.54 * (double)dpi / (double)height;

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
    GDRenderer dr(drawWidth, drawHeight, bgcolor, false);

    // call the internal helper API to do all the stylization overhead work
    ret = RenderMapInternal(map, selection, NULL, &dr, width, height, format, scale, b, false, bKeepSelection);

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderMap")

    return ret.Detach();
}


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

    double unitsPerPixel = 0.0254 / (double)dpi / metersPerUnit;
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
    GDRenderer dr(width, height, bgcolor, false);

    // call the internal helper API to do all the stylization overhead work
    ret = RenderMapInternal(map, selection, NULL, &dr, width, height, format, scale, b, false, bKeepSelection);

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderMap")

    return ret.Detach();
}



MgFeatureInformation* MgServerRenderingService::QueryFeatures(MgMap*      map,
                                                              MgStringCollection* layerNames,
                                                              MgGeometry* geometry,
                                                              INT32       selectionVariant, // Within, Touching, Topmost
                                                              INT32       maxFeatures)
{
    // Call updated QueryFeatures API
    return QueryFeatures(map, layerNames, geometry, selectionVariant, maxFeatures, false);
}


MgFeatureInformation* MgServerRenderingService::QueryFeatures(MgMap*      map,
                                                              MgStringCollection* layerNames,
                                                              MgGeometry* geometry,
                                                              INT32       selectionVariant, // Within, Touching, Topmost
                                                              INT32       maxFeatures,
                                                              bool bIgnoreScaleRange)
{
    Ptr<MgFeatureInformation> ret;

    MG_TRY()

    //detect case where there is no limit to selection
    if (maxFeatures == -1)
        maxFeatures = INT_MAX;

    //create return structure and selection set to fill out
    ret = new MgFeatureInformation();
    Ptr<MgSelection> sel = new MgSelection(map);
    FeatureInfoRenderer fir(sel, maxFeatures, map->GetViewScale());

    RenderForSelection(map, layerNames, geometry, selectionVariant, maxFeatures, &fir, bIgnoreScaleRange);

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
    ACE_DEBUG((LM_ERROR, ACE_TEXT("MgServerRenderingService::QueryFeatures() Selection Size:%d\n"), selLayers.p ? selLayers->GetCount() : 0));
    #endif

    MG_CATCH_AND_THROW(L"MgServerRenderingService.QueryFeatures")

    return ret.Detach();
}


MgBatchPropertyCollection* MgServerRenderingService::QueryFeatureProperties( MgMap* map,
                                    MgStringCollection* layerNames,
                                    MgGeometry* geometry,
                                    INT32 selectionVariant, // Within, Touching, Topmost
                                    INT32 maxFeatures)
{
    // Call updated QueryFeatureProperties API
    return QueryFeatureProperties(map, layerNames, geometry, selectionVariant, maxFeatures, false);
}


MgBatchPropertyCollection* MgServerRenderingService::QueryFeatureProperties( MgMap* map,
                                    MgStringCollection* layerNames,
                                    MgGeometry* geometry,
                                    INT32 selectionVariant, // Within, Touching, Topmost
                                    INT32 maxFeatures,
                                    bool bIgnoreScaleRange)
{
    Ptr<MgBatchPropertyCollection> ret;

    MG_TRY()

    //detect case where there is no limit to selection
    if (maxFeatures == -1)
        maxFeatures = INT_MAX;

    Ptr<MgSelection> sel;   //TODO: do we need this for this API? new MgSelection(map);
    FeaturePropRenderer fpr(sel, maxFeatures, map->GetViewScale());

    RenderForSelection(map, layerNames, geometry, selectionVariant, maxFeatures, &fpr, bIgnoreScaleRange);

    ret = fpr.GetProperties();
    //ret->SetSelection(sel);

    MG_CATCH_AND_THROW(L"MgServerRenderingService.QueryFeatures")

    return ret.Detach();
}



MgByteReader* MgServerRenderingService::RenderMapInternal(MgMap* map,
                                                          MgSelection* selection,
                                                          MgReadOnlyLayerCollection* roLayers,
                                                          GDRenderer* dr,
                                                          INT32 saveWidth,
                                                          INT32 saveHeight,
                                                          CREFSTRING format,
                                                          double scale,
                                                          RS_Bounds& b,
                                                          bool expandExtents,
                                                          bool bKeepSelection)
{
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

    RS_String units = (dstCs.p) ? dstCs->GetUnits() : L"";

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

    RS_MapUIInfo mapInfo(sessionId, map->GetName(), map->GetObjectId(), srs, units, bgcolor);

    // begin map stylization
    dr->StartMap(&mapInfo, b, scale, map->GetDisplayDpi(), map->GetMetersPerUnit());

        // if no layer collection is supplied, then put all layers in a temporary collection
        Ptr<MgReadOnlyLayerCollection> tempLayers = SAFE_ADDREF(roLayers);
        if (tempLayers == NULL)
        {
            tempLayers = new MgReadOnlyLayerCollection();
            Ptr<MgLayerCollection> layers = map->GetLayers();
            for (INT32 i=0; i<layers->GetCount(); i++)
            {
                Ptr<MgLayerBase> layer = layers->GetItem(i);
                tempLayers->Add(layer);
            }
        }

        MgStylizationUtil::StylizeLayers(m_svcResource, m_svcFeature, m_svcDrawing, m_pCSFactory, map,
                                         tempLayers, NULL, &ds, dr, dstCs, expandExtents, false, scale);

        // now we need to stylize the selection on top
        if (selection)
        {
            Ptr<MgReadOnlyLayerCollection> selLayers = selection->GetLayers();

            #ifdef _DEBUG
            printf("MgServerRenderingService::RenderMapInternal() - Layers:%d  Selection Layers:%d\n", tempLayers.p ? tempLayers->GetCount() : 0, selLayers.p ? selLayers->GetCount() : 0);
            #endif

            if (selLayers.p && selLayers->GetCount() > 0)
            {
                // tell the renderer to override draw styles with the ones
                // we use for selection
                dr->SetRenderSelectionMode(true);

                // prepare a collection of temporary MgLayers which have the right
                // FDO filters that will fetch only the selected features from FDO
                Ptr<MgReadOnlyLayerCollection> modLayers = new MgReadOnlyLayerCollection();
                Ptr<MgStringCollection> overrideFilters = new MgStringCollection();

                for (int s=0; s<selLayers->GetCount(); s++)
                {
                    Ptr<MgLayerBase> selLayer = selLayers->GetItem(s);

                    // generate a filter for the selected features
                    overrideFilters->Add(selection->GenerateFilter(selLayer, selLayer->GetFeatureClassName()));
                    modLayers->Add(selLayer);
                }

                MgStylizationUtil::StylizeLayers(m_svcResource, m_svcFeature, m_svcDrawing, m_pCSFactory, map,
                                                 modLayers, overrideFilters, &ds, dr, dstCs, false, false, scale, bKeepSelection);
            }
        }

    dr->EndMap();
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
    RS_ByteData* data = dr->Save(format, saveWidth, saveHeight);

    if (data != NULL)
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

        // must dispose the data returned by the renderer
        data->Dispose();
    }

    return ret.Detach();
}


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
    GDRenderer dr(width, height, bgcolor, false, false, 0.0);

    RS_Bounds b(0,0,width,height);

    RS_MapUIInfo info(L"", L"", L"", L"", L"", bgcolor);
    double pixelsPerInch = 96.0;
    double metersPerPixel = 0.0254 / pixelsPerInch;

    //start drawing
    dr.StartMap(&info, b, 1.0, pixelsPerInch, metersPerPixel);
    dr.StartLayer(NULL, NULL);

    //we need to specify margins and offsets in an MgPlotSpec,
    //even though in the image based (non-DWF) case they are 0.
    //Units are given as pixels, needed in order to scale legend
    //layout constants which are in inches
    Ptr<MgPlotSpecification> spec = new MgPlotSpecification(width - 1.0f, height - 1.0f, L"pixels");
    MgLegendPlotUtil lu(m_svcResource);
    lu.AddLegendElement(map->GetViewScale(), dr, map, spec, 0.0, 0.0);

    //done drawing
    dr.EndLayer();
    dr.EndMap();

    // get a byte representation of the image
    RS_ByteData* data = dr.Save(format, width, height);

    if (data != NULL)
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

        // must dispose the data returned by the renderer
        data->Dispose();
    }

    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderMapLegend")

    return ret.Detach();
}


//a helper function that does most of the work for QueryFeatures
//and QueryFeatureProperties. Basically runs a rendering loop with
//a custom renderer supplied by the caller that accumulates selection
//related things like property values and feature IDs.
void MgServerRenderingService::RenderForSelection(MgMap* map,
                         MgStringCollection* layerNames,
                         MgGeometry* geometry,
                         INT32 selectionVariant,
                         INT32 maxFeatures,
                         FeatureInfoRenderer* selRenderer,
                         bool bIgnoreScaleRange)
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("RenderForSelection(): ** START **\n")));
    if (NULL == map || NULL == geometry)
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

    //convert the map coordinate system from srs wkt to a mentor cs structure
    STRING srs = map->GetMapSRS();
    Ptr<MgCoordinateSystem> mapCs = (srs.empty()) ? NULL : m_pCSFactory->Create(srs);

    // begin map stylization
    RS_Bounds b(0, 0, 1, 1);    // not used
    RS_Color bgcolor(0, 0, 0, 255); // not used
    RS_MapUIInfo mapInfo(sessionId, map->GetName(), map->GetObjectId(), srs, L"", bgcolor);

    selRenderer->StartMap(&mapInfo, b, map->GetViewScale(), map->GetDisplayDpi(), map->GetMetersPerUnit());

    //initial simple selection scheme
    //Run a geometric FDO query on the given selection geometry
    //and return the features we get from FDO

    Ptr<MgLayerCollection> layers = map->GetLayers();

    //iterate over all map layers, but only do selection
    //if the layer is in the passed in collection
    for (int p=0; p<layers->GetCount(); p++)
    {
        //find the layer we need to select features from
        Ptr<MgLayerBase> layer = layers->GetItem(p);

        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("RenderForSelection(): Layer: %W  Selectable:%W  Visible: %W\n"), layer->GetName().c_str(), layer->GetSelectable() ? L"True" : L"False", layer->IsVisibleAtScale(map->GetViewScale()) ? L"True" : L"False"));
    
        //do this first - this check is fast
        if (!layer->GetSelectable())
            continue;

        //do we want to select on this layer -- if caller
        //gave us a layer name collection, check if the layer
        //is in there
        if (layerNames && layerNames->GetCount() > 0 && layerNames->IndexOf(layer->GetName()) == -1)
            continue;

        //check the visibility at scale if we're not ignoring scale ranges
        if (!bIgnoreScaleRange && !layer->IsVisibleAtScale(map->GetViewScale()))
            continue;

        //have we processed enough features already?
        if (maxFeatures <= 0)
            break;

        //get the MDF layer definition
        Ptr<MgResourceIdentifier> layerResId = layer->GetLayerDefinition();
        auto_ptr<MdfModel::LayerDefinition> ldf(MgStylizationUtil::GetLayerDefinition(m_svcResource, layerResId));
        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());

        //we can only do geometric query selection for vector layers
        if (vl)
        {
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("RenderForSelection(): Layer: %W  Vector Layer\n"), layer->GetName().c_str()));

            if(bIgnoreScaleRange)
            {
                // Modify the layer scale range only for layers that are passed in
                MdfModel::VectorScaleRangeCollection* scaleRanges = vl->GetScaleRanges();
                if(scaleRanges)
                {
                    MdfModel::VectorScaleRange* scaleRange = scaleRanges->GetAt(0);
                    if(scaleRange)
                    {
                        scaleRange->SetMinScale(0.0);
                        scaleRange->SetMaxScale(MdfModel::VectorScaleRange::MAX_MAP_SCALE);
                    }
                }
            }

            Ptr<MgResourceIdentifier> featResId = new MgResourceIdentifier(layer->GetFeatureSourceId());

            //get the coordinate system of the layer --> we need this
            //so that we can convert the input geometry from mapping space
            //to layer's space

            // Need to determine the name of the spatial context for this layer
            MdfModel::MdfString featureName = vl->GetFeatureName();

            // Parse the feature name for the schema and class
            STRING::size_type nDelimiter = featureName.find(L":");
            STRING schemaName;
            STRING className;

            if(STRING::npos == nDelimiter)
            {
                schemaName = L"";
                className = featureName;
            }
            else
            {
                schemaName = featureName.substr(0, nDelimiter);
                className = featureName.substr(nDelimiter + 1);
            }

            STRING spatialContextAssociation = L"";

            // Get the class definition so that we can find the spatial context association
            Ptr<MgClassDefinition> classDef = m_svcFeature->GetClassDefinition(featResId, schemaName, className);
            Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();

            // Find the spatial context for the geometric property. Use the first one if there are many defined.
            for(int index=0;index<propDefCol->GetCount();index++)
            {
                Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(index);
                if (propDef->GetPropertyType () == MgFeaturePropertyType::GeometricProperty)
                {
                    // We found the geometric property
                    MgGeometricPropertyDefinition* geomProp = static_cast<MgGeometricPropertyDefinition*>(propDef.p);
                    spatialContextAssociation = geomProp->GetSpatialContextAssociation();
                    break;
                }
            }

            // We want all of the spatial contexts
            Ptr<MgSpatialContextReader> csrdr = m_svcFeature->GetSpatialContexts(featResId, false);

            // This is the strategy we use for picking the spatial context
            // Find the 1st spatial context that satisfies one of the following: (Processed in order)
            // 1) Matches the association spatial context name
            // 2) The 1st spatial context returned
            // 3) FAIL - none of the above could be satisfied

            Ptr<MgCoordinateSystem> layerCs;

            if (mapCs)
            {
                STRING srcwkt = L"";
                STRING csrName = L"";
                bool bHaveFirstSpatialContext = false;

                while(csrdr->ReadNext())
                {
                    csrName = csrdr->GetName();
                    if((!spatialContextAssociation.empty()) && (csrName == spatialContextAssociation))
                    {
                        // Match found for the association)
                        srcwkt = csrdr->GetCoordinateSystemWkt();
                        break;
                    }
                    else if(!bHaveFirstSpatialContext)
                    {
                        // This is the 1st spatial context returned
                        // This will be overwritten if we find the association
                        srcwkt = csrdr->GetCoordinateSystemWkt();
                        bHaveFirstSpatialContext = true;
                    }
                }

                // Create coordinate system transformer
                layerCs = (srcwkt.empty()) ? NULL : m_pCSFactory->Create(srcwkt);
            }
            else
            {
                // No coordinate system!!! 
                // We fail here and do not use a default
            }

            //we want to transform query geometry from mapping space to layer space
            Ptr<MgCoordinateSystemTransform> trans;

            if (mapCs && layerCs)
            {
                trans = new MgCoordinateSystemTransform(mapCs, layerCs);
            }

            //if we have a valid transform, get the request geom in layer's space
            Ptr<MgGeometricEntity> queryGeom = SAFE_ADDREF(geometry);

            if (trans)
            {
                //get selection geometry in layer space
                queryGeom = geometry->Transform(trans);
            }

            //execute the spatial query
            Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();
            STRING geomPropName = layer->GetFeatureGeometryName();

            //set the spatial filter for the selection
            options->SetSpatialFilter(geomPropName, (MgGeometry*)(queryGeom.p), /*MgFeatureSpatialOperations*/selectionVariant);

            try
            {
                if (!vl->GetFilter().empty())
                {
                    //set layer feature filter if any
                    options->SetFilter(vl->GetFilter());
                }

                // TODO: can FeatureName be an extension name rather than a FeatureClass?
                Ptr<MgFeatureReader> rdr = m_svcFeature->SelectFeatures(featResId, vl->GetFeatureName(), options);
                RSMgFeatureReader rsrdr(rdr, m_svcFeature, featResId, options, vl->GetGeometry());

                //run a stylization loop with the FeatureInfoRenderer.
                //This will build up the selection set and also
                //evaluate the tooltip, hyperlink and feature properties
                //for the first feature hit

                RS_UIGraphic uig(NULL, 0, L"");
                RS_LayerUIInfo info(layer->GetName(),
                    layer->GetObjectId(), // object ID
                    true,       // selectable
                    true,       // visible
                    true,       // editable
                    L"",        // group name
                    L"",        // group ID
                    true,       // showInLegend
                    true,       // expandInLegend
                    0.0,        // zOrder
                    uig);       // uiGraphic

                //extract hyperlink and tooltip info
                if (!vl->GetToolTip().empty()) info.hastooltips() = true;
                if (!vl->GetUrl().empty()) info.hashyperlinks() = true;

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

                DefaultStylizer ds(NULL);
                selRenderer->StartLayer(&info, &fcinfo);
                ds.StylizeVectorLayer(vl, selRenderer, &rsrdr, NULL, StylizeThatMany, selRenderer);
                selRenderer->EndLayer();

                //update maxFeatures to number of features that
                //we can select from subsequent layers
                maxFeatures -= selRenderer->GetNumFeaturesProcessed();
            }
            catch (MgFdoException* e)
            {
                //TODO: what should we really be doing in this case?
                //This can happen if the underlying FDO provider does not
                //support a particular spatial operation. One way around this
                //is to select all features which appear on the screen and then
                //do our own geometry math.
                e->Release();
            }
        }
    }

    selRenderer->EndMap();
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("RenderForSelection(): ** END **\n")));
}

void MgServerRenderingService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for Server-side service objects.
}
