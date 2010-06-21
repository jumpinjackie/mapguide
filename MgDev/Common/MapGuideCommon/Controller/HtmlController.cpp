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

//////////////////////////////////////////////////////////////////
// Construct a MgHtmlController object
//
MgHtmlController::MgHtmlController(MgSiteConnection* siteConn)
: MgController(siteConn)
{
}

//////////////////////////////////////////////////////////////////
// Processes a GetDynamicMapOverlayImage request from the Viewer and returns an image of the specified map.
//
MgByteReader* MgHtmlController::GetDynamicMapOverlayImage(CREFSTRING mapName, MgRenderingOptions* options, MgPropertyCollection* mapViewCommands)
{
    // Create a Resource Service instance
    Ptr<MgResourceService> resourceService = (MgResourceService*)GetService(MgServiceType::ResourceService);

    // Create MgMap
    Ptr<MgMap> map = new MgMap();
    map->Open(resourceService, mapName);

    // Make sure we clear any track changes - these are not applicable for AJAX.
    Ptr<MgNamedCollection> changeLists = map->GetChangeLists();
    if (changeLists->GetCount() > 0)
    {
        map->ClearChanges();
        map->Save(resourceService);
    }

    // Get the selection
    Ptr<MgSelection> selection = new MgSelection(map);
    selection->Open(resourceService, mapName);

    // Apply map view commands
    ApplyMapViewCommands(map, mapViewCommands);

    // Make sure we clear any track changes - these are not applicable for AJAX.
    map->ClearChanges();

    // Save the MgMap state
    map->Save(resourceService);

    // Create Proxy Rendering Service instance
    Ptr<MgRenderingService> service = (MgRenderingService*)(GetService(MgServiceType::RenderingService));

    // Call the C++ API
    return service->RenderDynamicOverlay(map, selection, options);
}

//////////////////////////////////////////////////////////////////
// Processes a GetMapImage request from the Viewer and returns an image of the specified map.
//
MgByteReader* MgHtmlController::GetMapImage(MgMap* map, MgSelection* selection,
    CREFSTRING format, MgPropertyCollection* mapViewCommands, bool bKeepSelection, bool bClip)
{
    // Apply map view commands
    ApplyMapViewCommands(map, mapViewCommands);

    // Make sure we clear any track changes - these are not applicable for AJAX.
    if (NULL != map)
        map->ClearChanges();

    // Get Proxy Rendering Service instance
    Ptr<MgRenderingService> service = (MgRenderingService*)(GetService(MgServiceType::RenderingService));

    // Call the C++ API
    return service->RenderMap(map, selection, format, bKeepSelection, bClip);
}

//////////////////////////////////////////////////////////////////
// Processes a GetVisibleMapExtent request from the Viewer and returns info on the specified map.
//
MgByteReader* MgHtmlController::GetVisibleMapExtent(CREFSTRING mapName,
    MgPropertyCollection* mapViewCommands)
{
    // Create a Resource Service instance
    Ptr<MgResourceService> resourceService = (MgResourceService*)GetService(MgServiceType::ResourceService);

    // Open the MgMap
    Ptr<MgMap> map = new MgMap();
    map->Open(resourceService, mapName);

    // Apply map view commands
    ApplyMapViewCommands(map, mapViewCommands);

    // Make sure we clear any track changes - these are not applicable for AJAX.
    map->ClearChanges();

    // Save the MgMap state
    map->Save(resourceService);

    // Calculate mcs width and height
    double metersPerUnit = map->GetMetersPerUnit();
    double mapScale = map->GetViewScale();
    double devW = map->GetDisplayWidth();
    double devH = map->GetDisplayHeight();
    double metersPerPixel = 0.0254 / map->GetDisplayDpi();

    double mcsW = mapScale * devW * metersPerPixel / metersPerUnit;
    double mcsH = mapScale * devH * metersPerPixel / metersPerUnit;

    // Make an envelope
    Ptr<MgPoint> center = map->GetViewCenter();
    Ptr<MgCoordinate> coord = center->GetCoordinate();
    Ptr<MgCoordinateXY> coord0 = new MgCoordinateXY(coord->GetX() - 0.5*mcsW, coord->GetY() - 0.5*mcsH);
    Ptr<MgCoordinateXY> coord1 = new MgCoordinateXY(coord->GetX() + 0.5*mcsW, coord->GetY() + 0.5*mcsH);
    Ptr<MgEnvelope> envelope = new MgEnvelope(coord0, coord1);

    // Return XML
    return envelope->ToXml();
}

//////////////////////////////////////////////////////////////////
// Processes a GetLayerImageMap request from the Viewer and returns an image map
// for the visible features on the specified map layer
//
MgByteReader* MgHtmlController::GetLayerImageMap(CREFSTRING mapName, CREFSTRING layerName)
{
    throw new MgNotImplementedException(L"MgHtmlController.GetLayerImageMap", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// processes a GetMapLegendImage request from the Viewer and returns
// an image of the specified maps legend.
//
MgByteReader* MgHtmlController::GetMapLegendImage(CREFSTRING mapName,
    CREFSTRING format, MgColor* backgroundColor, INT32 width,
    INT32 height)
{
    // Create a Resource Service instance
    Ptr<MgResourceService> resourceService = (MgResourceService*)GetService(MgServiceType::ResourceService);

    // Create MgMap
    Ptr<MgMap> map = new MgMap();
    map->Open(resourceService, mapName);

    // Make sure we clear any track changes - these are not applicable for AJAX.
    Ptr<MgNamedCollection> changeLists = map->GetChangeLists();
    if (changeLists->GetCount() > 0)
    {
        map->ClearChanges();
        map->Save(resourceService);
    }

    // Create Proxy Rendering Service instance
    Ptr<MgRenderingService> service = (MgRenderingService*)(GetService(MgServiceType::RenderingService));

    // Call the C++ API
    return service->RenderMapLegend(map, width, height, backgroundColor, format);
}

//////////////////////////////////////////////////////////////////
// Processes a QueryMapFeatures request from the Viewer
//
MgByteReader* MgHtmlController::QueryMapFeatures(
    CREFSTRING mapName,
    MgStringCollection* layerNames,
    MgGeometry* selectionGeometry,
    INT32 selectionVariant,
    CREFSTRING featureFilter,
    INT32 maxFeatures,
    bool persist,
    INT32 layerAttributeFilter)
{
    // Create a Resource Service instance
    Ptr<MgResourceService> resourceService = (MgResourceService*)GetService(MgServiceType::ResourceService);

    // Create MgMap
    Ptr<MgMap> map = new MgMap();
    map->Open(resourceService, mapName);

    // Make sure we clear any track changes - these are not applicable for AJAX.
    Ptr<MgNamedCollection> changeLists = map->GetChangeLists();
    if (changeLists->GetCount() > 0)
    {
        map->ClearChanges();
        map->Save(resourceService);
    }

    // Create Proxy Rendering Service instance
    Ptr<MgRenderingService> service = (MgRenderingService*)(GetService(MgServiceType::RenderingService));

    // Call the C++ API
    Ptr<MgFeatureInformation> featureInfo = service->QueryFeatures(map, layerNames, selectionGeometry,
        selectionVariant, featureFilter, maxFeatures, layerAttributeFilter);

    if(persist)
    {
        //save the selection set in the session repository
        Ptr<MgSelection> selection = featureInfo->GetSelection();
        if(!selection)
            selection = new MgSelection(map);
        selection->Save(resourceService, mapName);
    }

    // Return XML
    return featureInfo->ToXml();
}

//////////////////////////////////////////////////////////////////
// Generates JavaScript code that can be embedded in an HTML response
// to a non-viewer initiated web application request. The returned code
// forces a synchronization of the client-side view with any changes made to the Map Model.
//
STRING MgHtmlController::ScriptViewUpdate(bool forceFullRefresh)
{
    throw new MgNotImplementedException(L"MgHtmlController.ScriptViewUpdate", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Generates JavaScript code that can be embedded in an HTML response
// to a non-viewer initiated web application request. The returned code
// forces a synchronization of the client-side view with any changes made to the Map Model.
//
STRING MgHtmlController::ScriptViewUpdate(MgPoint* center, double scale,
    bool forceFullRefresh)
{
    throw new MgNotImplementedException(L"MgHtmlController.ScriptViewUpdate", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Apply the specified set of commands to the map.
//
void MgHtmlController::ApplyMapViewCommands(MgMap* map, MgPropertyCollection* mapViewCommands)
{
    if(mapViewCommands == NULL)
        return;

    //apply commands common to both type of viewers
    MgController::ApplyMapViewCommands(map, mapViewCommands);
}

//////////////////////////////////////////////////////////////////
// Destruct a MgHtmlController object
//
MgHtmlController::~MgHtmlController()
{
}
