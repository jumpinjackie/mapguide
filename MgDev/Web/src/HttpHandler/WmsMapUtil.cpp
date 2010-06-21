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

#include "HttpHandler.h"
#include "WmsMapUtil.h"

//static map<STRING, STRING> srsMappings;
//static bool InitializeSrsMappings();
//static bool initMappings = InitializeSrsMappings();

/*
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
*/

// This simple function is handed into the MgOgcServer system, as
// a means to resolve documents.
bool MgWmsMapUtil::GetDocument(CPSZ pszDoc,REFSTRING sRet)
{
    //TODO: Remove dependency on thread local storage
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();
    Ptr<MgServerAdmin> serverAdmin = new MgServerAdmin();
    serverAdmin->Open(userInfo);

    Ptr<MgByteReader> brDoc;
    brDoc = serverAdmin->GetDocument(pszDoc);

    sRet = brDoc->ToString();
    return sRet.length() > 0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Get the layer definition resource IDs of the specified layers.
///
MgStringCollection* MgWmsMapUtil::GetLayerDefinitionIds(CREFSTRING layerList)
{
    Ptr<MgStringCollection> layerDefIds = new MgStringCollection();
    Ptr<MgStringCollection> layerNames = MgStringCollection::ParseCollection(layerList, L",");

    if (NULL != layerNames)
    {
        STRING resourcePrefix = L"Library://";
        STRING resourceSuffix = L".LayerDefinition";

        for (INT32 i = 0; i < layerNames->GetCount(); ++i)
        {
            STRING resourceId = layerNames->GetItem(i);

            // Add the resource prefix, if necessary.
            if (0 != _wcsnicmp(resourceId.c_str(), resourcePrefix.c_str(), resourcePrefix.length()))
            {
                resourceId = resourcePrefix + resourceId;
            }

            // Add the resource suffix, if necessary.
            if (NULL == ::wcsstr(resourceId.c_str(), resourceSuffix.c_str()))
            {
                resourceId += resourceSuffix;
            }

            layerDefIds->Add(resourceId);
        }
    }

    return layerDefIds.Detach();
}

/// <summary>
/// Gets a map corresponding to the request params
/// </summary>
/// <returns>
/// MgMap
/// This is the requested map
/// </returns>
MgMap* MgWmsMapUtil::GetMap(MgOgcWmsServer& oWms,
    MgStringCollection* layerDefIds, CREFSTRING bbox, CREFSTRING sSRS,
    INT32 width, INT32 height, MgResourceService* resourceService)
{
    // Get the requested map extents
    Ptr<MgEnvelope> extents = GetExtents(bbox);

    // Look for a mapping from the specified CRS to a converted value

    STRING sWKT;
    SrsToWktMapping(oWms,sSRS,sWKT);

    // Create a new, empty map
    Ptr<MgMap> map = new MgMap();
    map->Create(sWKT, extents, L"WMS Map");
    map->SetDisplayWidth(width);
    map->SetDisplayHeight(height);

    double realWorldWidthMeters = extents->GetWidth() * map->GetMetersPerUnit();
    double imageWidthMeters = (double)width * 0.0254 / map->GetDisplayDpi();
    double scale = realWorldWidthMeters / imageWidthMeters;
    map->SetViewScale(scale);

    // Add the requested layers
    if (NULL != layerDefIds && layerDefIds->GetCount() > 0)
    {
        Ptr<MgLayerCollection> mapLayers = map->GetLayers();

        for (INT32 i = layerDefIds->GetCount() - 1; i >= 0; --i)
        {
            Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(layerDefIds->GetItem(i));
            Ptr<MgLayer> mgLayer = new MgLayer(resId, resourceService);

            mgLayer->SetName(resId->GetPathname(false));
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


void MgWmsMapUtil::SrsToWktMapping(MgOgcServer& oWms,STRING sSRS,REFSTRING sWKT)
{
    // Plan A is to look for a user-defined mapping.  This allows a config
    // file to contain overrides for incorrect, incomplete, or new reference
    // systems.  Input is an SRS string, output is (user-defined) WKT.
    // This is turned inside-out from previous algorithm.
    if(!UserDefinedSrsToWktMapping(oWms,sSRS,sWKT) || sWKT.length() == 0) {
        // If no user mapping, let's do the default thing, which is to
        // have the underlying library make the conversion.  It may fail
        // and throw an exception, which we would then report.

        // ConvertCoordinateSystemCodeToWkt used to be static, but PHP
        // couldn't support that, so this is "the way" to get a "this"
        // pointer to use for the method.
        Ptr<MgCoordinateSystemFactory> factory = new MgCoordinateSystemFactory();
        // Now, try to convert it to WKT.
        // This may throw an exception, which is caught outside.
        sWKT = factory->ConvertCoordinateSystemCodeToWkt(sSRS);
    }
}


bool MgWmsMapUtil::UserDefinedSrsToWktMapping(MgOgcServer& oWms,STRING sSrs,REFSTRING sWkt)
{
    // Makes srs arg "case insensitive"
    sSrs = MgUtil::ToUpper(sSrs);

    // Allows customer to provide overridden WKT for any given SRS value.
    // If none such, the return is false.

    // TODO: performance optimization: grab and cache the dictionary definition,
    // and use the other MapValue overload against that cached definition.
    return (oWms.MapValue(_("SRS.WKT.map"),sSrs.c_str(),sWkt));
}

/*
STRING MgWmsMapUtil::SrsToWkt(CREFSTRING srs)
{
    // Look for a mapping from the specified SRS to a converted WKT value
    return srsMappings[srs];
}
*/

