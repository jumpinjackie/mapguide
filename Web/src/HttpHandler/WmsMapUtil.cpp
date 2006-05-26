//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "HttpHandler.h"
#include "WmsMapUtil.h"

static map<STRING, STRING> srsMappings;
static bool InitializeSrsMappings();
static bool initMappings = InitializeSrsMappings();

bool InitializeSrsMappings()
{
    //Declare mappings between supported SRS strings and their WKT equivalents

    //WGS84
    srsMappings[L"CRS:84"] = L"GEOGCS[\"LL84\",DATUM[\"WGS 84\",SPHEROID[\"WGS 84\",6378137,0],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.01745329252]]";
    srsMappings[L"EPSG:4326"] = L"GEOGCS[\"LL84\",DATUM[\"WGS 84\",SPHEROID[\"WGS 84\",6378137,0],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.01745329252]]";

    //NAD83
    srsMappings[L"CRS:83"] = L"GEOGCS[\"LL83\",DATUM[\"NAD 83\",SPHEROID[\"GRS 80\",6378137,298.25722210089],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.01745329252]]";

    //NAD27
    srsMappings[L"CRS:27"] = L"GEOGCS[\"LL27\",DATUM[\"NAD 27\",SPHEROID[\"Clarke 1866\",6378206.4000000004,294.9786982139],TOWGS84[-8,160,176,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.01745329252]]";

    return true;
}

// This simple function is handed into the MgOgcServer system, as
// a means to resolve documents.
bool MgWmsMapUtil::GetDocument(CPSZ pszDoc,REFSTRING sRet)
{
    //TODO: Remove dependency on thread local storage
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
    Ptr<MgSite> mgSite;
    mgSite = new MgSite();
    mgSite->Open(userInfo);
    STRING serverAddress = mgSite->GetSiteServerAddress();

    Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
    serverAdmin->Open(serverAddress,userInfo);

    Ptr<MgByteReader> brDoc;
    brDoc = serverAdmin->GetDocument(pszDoc);

    sRet = brDoc->ToString();
    return sRet.length() > 0;
}

/// <summary>
/// Gets a map corresponding to the request params
/// </summary>
/// <returns>
/// MgMap
/// This is the requested map
/// </returns>
MgMap* MgWmsMapUtil::GetMap(CREFSTRING layerList, CREFSTRING bbox, CREFSTRING crs,
                                INT32 width, INT32 height,
                                MgResourceService* resourceService)
{
    INT32 displayDpi = 72;

    // Get the requested map extents
    Ptr<MgEnvelope> extents = GetExtents(bbox);

    // Look for a mapping from the specified CRS to a converted value
    STRING wktSRS = SrsToWkt(crs);
    if(wktSRS.length() == 0)
    {
        // If no mapping exists, just pass in what we have
        wktSRS = crs;
    }

    // Create a new, empty map
    Ptr<MgMap> map = new MgMap();
    map->Create(wktSRS, extents, L"WMS Map");
    map->SetDisplayWidth(width);
    map->SetDisplayHeight(height);
    map->SetDisplayDpi(displayDpi);


    double realWorldWidthMeters = extents->GetWidth() * map->GetMetersPerUnit();
    double imageWidthMeters = (double)width / (displayDpi * 39.3701);
    double scale = realWorldWidthMeters / imageWidthMeters;
    map->SetViewScale(scale);

    // Add the requested layers
    Ptr<MgStringCollection> layers = MgStringCollection::ParseCollection(layerList, L",");
    if(layers != NULL && layers->GetCount() > 0)
    {
        Ptr<MgLayerCollection> mapLayers = map->GetLayers();

        STRING libPrefix = L"Library://";
        STRING layerDefSuffix = L".LayerDefinition";

        for(INT32 i = layers->GetCount() - 1; i >= 0; i--)
        {
            STRING layerId = layers->GetItem(i);

            // Add the "Library://" prefix, if necessary
            if(_wcsnicmp(layerId.c_str(), libPrefix.c_str(), libPrefix.length()) != 0)
            {
                layerId = libPrefix + layerId;
            }
            // Add the ".LayerDefinition" extension, if necessary
            if(wcsstr(layerId.c_str(), layerDefSuffix.c_str()) == NULL)
            {
                layerId += layerDefSuffix;
            }

            Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(layerId);
            Ptr<MgLayer> mgLayer = new MgLayer(resId, resourceService);
            mgLayer->SetName(layers->GetItem(i));
            mapLayers->Add(mgLayer);
            mgLayer->SetSelectable(true);
            mgLayer->ForceRefresh();
        }
    }

    return map.Detach();
}

MgEnvelope* MgWmsMapUtil::GetExtents(CREFSTRING bbox)
{
    MgEnvelope* extents;
    extents = NULL;
    Ptr<MgStringCollection> bounds = MgStringCollection::ParseCollection(bbox, L",");

    if(bounds->GetCount() == 4)
    {
        double coords[4];
        for(INT32 i = 0; i < bounds->GetCount(); i++)
        {
            coords[i] = MgUtil::StringToDouble(bounds->GetItem(i));
        }
        Ptr<MgCoordinate> coord1 = new MgCoordinateXY(coords[0], coords[1]);
        Ptr<MgCoordinate> coord2 = new MgCoordinateXY(coords[2], coords[3]);
        extents = new MgEnvelope(coord1, coord2);
    }
    return extents;
}

MgColor* MgWmsMapUtil::GetBackgroundColor(CREFSTRING bgColor, bool transparent)
{
    MgColor* color = NULL;
    if(bgColor.length() == 8) // Format 0xRRGGBB
    {
        // Convert RGB to RGBA
        STRING colorPlusAlpha = bgColor + (transparent ? L"00" : L"FF");

        // Create color
        color = new MgColor(colorPlusAlpha);
    }
    else
    {
        // Set to white
        color = new MgColor(0xFF, 0xFF, 0xFF, transparent ? 0x00 : 0xFF);
    }

    return color;
}

STRING MgWmsMapUtil::SrsToWkt(CREFSTRING srs)
{
    // Look for a mapping from the specified SRS to a converted WKT value
    return srsMappings[srs];
}
