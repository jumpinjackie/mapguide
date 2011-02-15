//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "VectorLayerDefinition.h"
#include "GridLayerDefinition.h"
#include "../../../Common/MdfParser/SAX2Parser.h"

//static map<STRING, STRING> srsMappings;
//static bool InitializeSrsMappings();
//static bool initMappings = InitializeSrsMappings();

/*
bool InitializeSrsMappings()
{
    //Declare mappings between supported SRS strings and their WKT equivalents

    //WGS84
    srsMappings[L"CRS:84"] = L"GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137,0],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.01745329252]]";
    srsMappings[L"EPSG:4326"] = L"GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137,0],TOWGS84[0,0,0,0,0,0,0]],PRIMEM[\"Greenwich\",0],UNIT[\"Degrees\",0.01745329252]]";

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
    Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
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
    INT32 width, INT32 height, MgResourceService* resourceService, MgFeatureService* featureService, CREFSTRING sSession)
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

    Ptr<MgWmsLayerDefinitions> layerDefs = MgHttpWmsGetCapabilities::GetLayerDefinitions(*resourceService, layerDefIds);
    // Add the requested layers
    if (NULL != layerDefIds && layerDefIds->GetCount() > 0)
    {
        Ptr<MgCoordinateSystemFactory> factory = new MgCoordinateSystemFactory();
        Ptr<MgCoordinateSystem> mapCs = factory->Create(sWKT);
        Ptr<MgCoordinateSystemTransform> csTrans;

        Ptr<MgLayerCollection> mapLayers = map->GetLayers();

        for (INT32 i = layerDefIds->GetCount() - 1; i >= 0; --i)
        {
            Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(layerDefIds->GetItem(i));
            Ptr<MgLayer> mgLayer = new MgLayer(resId, resourceService);
            
            STRING layerPath = resId->GetPathname(true);
            Ptr<MgResourceIdentifier> wmsLayerResId = new MgResourceIdentifier(L"Session:"+ sSession + L"//" + layerPath);

            // OGC 06-042 section 7.3.3.6
            // If a request contains a BBox whose area does not overlap at all with the <BoundingBox> element in the 
            // service metadata for the requested layer, the server shall return empty content (that is, a blank map 
            // or an graphic elemenet file with no elements) for that map. Any features that are partly or entirely 
            // contained in the Bounding Box shall be returned in the appropriate format
            if(layerDefs->Next())
            {
                // Get user defined boundingbox 
                MgUtilDictionary currentDef(NULL);
                layerDefs->GenerateDefinitions(currentDef);
                STRING layerBounds = currentDef[L"Layer.Bounds"];
                Ptr<MgStringCollection> wmsLayerBoundingbox = GetWMSlayerBoundingbox(sSRS, layerBounds);
                STRING sMinX, sMinY, sMaxX, sMaxY;
               
                ASSERT(4 == wmsLayerBoundingbox->GetCount());
                if(4 == wmsLayerBoundingbox->GetCount())
                {
                    sMinX = wmsLayerBoundingbox->GetItem(0);
                    sMinY = wmsLayerBoundingbox->GetItem(1);
                    sMaxX = wmsLayerBoundingbox->GetItem(2);
                    sMaxY = wmsLayerBoundingbox->GetItem(3);
                }
                else
                {
                    sMinX = L"0";
                    sMinY = L"0";
                    sMaxX = L"0";
                    sMaxY = L"0";
                }

                // User defined Boundingbox under map CS
                Ptr<MgEnvelope> wmsLayerExtent = new MgEnvelope(MgUtil::StringToDouble(sMinX), MgUtil::StringToDouble(sMinY), MgUtil::StringToDouble(sMaxX), MgUtil::StringToDouble(sMaxY));

                // Optimization...
                // If the request boundingbox (map boundingbox) within the user defined boundingbox (wms layer boundingbox),
                // spatial query on the layer should be ignored. Just reuse the original layer's layer definition
                if(wmsLayerExtent->Contains(extents))
                {
                    wmsLayerResId = resId;
                }
                else
                {
                    // Get layer definition from the original layer
                    auto_ptr<MdfModel::LayerDefinition> layerDef;
                    layerDef.reset(mgLayer->GetLayerDefinition(resourceService,resId));
                    MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(layerDef.get());
                    MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(layerDef.get());

                    // Spatial filter
                    STRING filter;

                    // Vector Layer
                    if(NULL != vl)
                    {
                        filter = vl->GetFilter();
                    }
                    // Grid Layer
                    if(NULL != gl)
                    {
                        filter = gl->GetFilter();
                    }

                    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(mgLayer->GetFeatureSourceId());

                    STRING qualifiedName = mgLayer->GetFeatureClassName();

                    int pos = qualifiedName.find(L":");
                    STRING schemaName = qualifiedName.substr(0,pos);
                    STRING className = qualifiedName.substr(pos+1,qualifiedName.length()-pos-1);

                    Ptr<MgClassDefinition> classDef = featureService->GetClassDefinition(fsId,schemaName,className);
                    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();

                    // filter on every geometric properties
                    for(int i =0; i<propDefCol->GetCount(); i++)
                    {
                        Ptr<MgPropertyDefinition> prop = propDefCol->GetItem(i);
                        if(prop->GetPropertyType() == MgFeaturePropertyType::GeometricProperty)
                        {
                            STRING spatialContextAssociation = L"";
                            STRING layerCoordSysWkt = L"";
                            
                            //Get the layer CS
                            MgGeometricPropertyDefinition* geomProp = static_cast<MgGeometricPropertyDefinition*>(prop.p);
                            spatialContextAssociation = geomProp->GetSpatialContextAssociation();

                            Ptr<MgSpatialContextReader> scReader = featureService->GetSpatialContexts(fsId, false);
                            if(scReader.p != NULL)
                            {
                                while(scReader->ReadNext())
                                {
                                    STRING csrName = scReader->GetName();
                                    if(!spatialContextAssociation.empty() &&  csrName == spatialContextAssociation)
                                    {
                                        layerCoordSysWkt = scReader->GetCoordinateSystemWkt();
                                        break;
                                    }
                                    else if(layerCoordSysWkt.empty())
                                    {
                                        // This is the 1st spatial context returned
                                        // This will be overwritten if we find the association
                                        layerCoordSysWkt = scReader->GetCoordinateSystemWkt();
                                    }
                                }
                                scReader->Close();
                            }

                            Ptr<MgCoordinateSystem> layerCs = (layerCoordSysWkt.empty()) ? NULL : factory->Create(layerCoordSysWkt);
                            if(layerCs != NULL)
                            {   
                                csTrans = factory->GetTransform(mapCs,layerCs);
                                csTrans->IgnoreDatumShiftWarning(true);
                                csTrans->IgnoreOutsideDomainWarning(true);
                            }

                            // Transform user defined boundingbox to layer CS
                            Ptr<MgEnvelope> layerCsExtent = csTrans->Transform(wmsLayerExtent);
                            Ptr<MgCoordinate> layerCsLowerLeftCoordinate = layerCsExtent->GetLowerLeftCoordinate();
                            Ptr<MgCoordinate> layerCsUpperRightCoordinate = layerCsExtent->GetUpperRightCoordinate();
                            MgUtil::DoubleToString(layerCsLowerLeftCoordinate->GetX(),sMinX);
                            MgUtil::DoubleToString(layerCsLowerLeftCoordinate->GetY(),sMinY);
                            MgUtil::DoubleToString(layerCsUpperRightCoordinate->GetX(),sMaxX);
                            MgUtil::DoubleToString(layerCsUpperRightCoordinate->GetY(),sMaxY);

                            STRING propName = prop->GetName();
                            
                            STRING boundingboxGeom = L"GeomFromText('POLYGON((" 
                                                        + sMinX + L" " + sMinY + L","
                                                        + sMaxX + L" " + sMinY + L","
                                                        + sMaxX + L" " + sMaxY + L","
                                                        + sMinX + L" " + sMaxY + L","
                                                        + sMinX + L" " + sMinY + L"))')";
                            if(filter.empty())
                            {
                                filter = L"( " + propName + L" ENVELOPEINTERSECTS " + boundingboxGeom + L" )";
                            }
                            else
                            {
                                filter += L" AND ( " + propName + L" ENVELOPEINTERSECTS " + boundingboxGeom + L" )";
                            }
                        }
                    }

                     // Vector Layer
                    if(NULL != vl)
                    {
                        vl->SetFilter(filter);
                    }
                    // Grid Layer
                    if(NULL != gl)
                    {
                        gl->SetFilter(filter);
                    }

                    MdfParser::SAX2Parser parser;
                    std::string content = parser.SerializeToXML(layerDef.get(), NULL);

                    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)content.c_str(), (INT32)content.length());
                    Ptr<MgByteReader> byteReader = byteSource->GetReader();

                    resourceService->SetResource(wmsLayerResId,byteReader,NULL);
                }
            }

            Ptr<MgLayer> mgWmsLayer = new MgLayer(wmsLayerResId, resourceService);
            mgWmsLayer->SetName(resId->GetPathname(false));
            mapLayers->Add(mgWmsLayer);
            mgWmsLayer->SetSelectable(true);
            mgWmsLayer->ForceRefresh();
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

    ProcessURNSrs(sSRS);

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

void MgWmsMapUtil::ProcessBoundingBoxAxes(STRING sSrs,REFSTRING bbox)
{
    if(sSrs.empty() || bbox.empty() ||sSrs == _("CRS:84")) //Workaround
        return;

    Ptr<MgCoordinateSystemFactory> factory = new MgCoordinateSystemFactory();
    STRING wkt = factory->ConvertCoordinateSystemCodeToWkt(sSrs);
    Ptr<MgCoordinateSystem> cs = factory->Create(wkt);

    INT16 quadrant = cs->GetEpsgQuadrant();

    //X increases to the East,  Y increases to the North
    if(0 == quadrant || 1 == quadrant)
        return;

    Ptr<MgStringCollection> bounds = MgStringCollection::ParseCollection(bbox, L",");
    if(bounds->GetCount() == 4)
    {
        double coords[4];
        for(INT32 i = 0; i < bounds->GetCount(); i++)
        {
            coords[i] = MgUtil::StringToDouble(bounds->GetItem(i));
        }

        switch(quadrant)
        {
            //X increases to the West,  Y increases to the North
            case 2:
                ReverseCoords(coords[0],coords[2]);
                break;
            //X increases to the West,  Y increases to the South
            case 3:
                ReverseCoords(coords[0],coords[2]);
                ReverseCoords(coords[1],coords[3]);
                break;
            //X increases to the East,  Y increases to the South
            case 4:
                ReverseCoords(coords[1],coords[3]);
                break;
            //X increases to the North, Y increases to the East
            case -1:
                SwapCoords(coords);
                break;
            //X increases to the North, Y increases to the West
            case -2:
                SwapCoords(coords);
                ReverseCoords(coords[0],coords[2]);
                break;
            //X increases to the South, Y increases to the West
            case -3:
                SwapCoords(coords);
                ReverseCoords(coords[0],coords[2]);
                ReverseCoords(coords[1],coords[3]);
                break;
            //X increases to the South, Y increases to the East
            case -4:
                SwapCoords(coords);
                ReverseCoords(coords[1],coords[3]);
                break;
            //X increases to the East,  Y increases to the North
            case 1:
            case 0:
            default:
                break;
        }

        bbox.clear();
        for(INT32 i=0;i<4;i++)
        {
            STRING doubleStr;
            MgUtil::DoubleToString(coords[i], doubleStr);
            bbox.append(doubleStr);
            if(i != 3)
                bbox.append(_(","));
        }
    }
}
/*
STRING MgWmsMapUtil::SrsToWkt(CREFSTRING srs)
{
    // Look for a mapping from the specified SRS to a converted WKT value
    return srsMappings[srs];
}
*/
void MgWmsMapUtil::SwapCoords(double(& coord)[4])
{
    double temp = coord[0];
    coord[0] = coord[1];
    coord[1] = temp;

    temp = coord[2];
    coord[2] = coord[3];
    coord[3] = temp;
}

void MgWmsMapUtil::ReverseCoords(double& coord1, double& coord2)
{
    coord1 = 0.0- coord1;
    coord2 = 0.0- coord2;
}

void MgWmsMapUtil::ProcessURNSrs(REFSTRING sSRS)
{
    sSRS = MgUtil::ToUpper(sSRS);
    sSRS = MgUtil::ReplaceString(sSRS,L"URN:OGC:DEF:CRS:EPSG:",L"EPSG");
}

MgStringCollection* MgWmsMapUtil::GetWMSlayerBoundingbox(STRING sSrs, STRING layerBounds)
{
    // return value which specified the boundingbox of user defined boundingbox
    Ptr<MgStringCollection> wmsLayerBoundingbox = new MgStringCollection();

    // Looking for all <Bounds> elements from layerBounds
    // Example: <Bounds SRS="EPSG:4326" west="-87.74" south="43.68" east="-87.69" north="43.815"/>
    Ptr<MgStringCollection> bounds = new MgStringCollection();
    int pos = 0;
    while((pos = layerBounds.find(L"<Bounds",pos)) != STRING::npos)
    {
        pos += 7; // pos+7 to the first character after <Bounds
        int endPos = layerBounds.find(L"/>",pos); 
        bounds->Add(layerBounds.substr(pos,endPos-pos));
    }

    // Looking for the boundingbox defined with specified SRS
    for(int i = 0; i<bounds->GetCount(); i++)
    {
        STRING bound = bounds->GetItem(i);

        //Find SRS value from Bounds element 
        int startPos = bound.find(L"SRS=\"");
        int endPos = bound.find(L"\"",startPos+5); // pos+5 to the first character after SRS="
        STRING srs = bound.substr(startPos+5,endPos-startPos-5);

        if(MgUtil::ToUpper(srs) == MgUtil::ToUpper(sSrs))
        {
            //Find west value from Bounds element 
            startPos = bound.find(L"west=\"");
            endPos = bound.find(L"\"",startPos+6); // pos+6 to the first character after west="
            wmsLayerBoundingbox->Add(bound.substr(startPos+6,endPos-startPos-6));

            //Find south value from Bounds element 
            startPos = bound.find(L"south=\"");
            endPos = bound.find(L"\"",startPos+7); // pos+7 to the first character after south="
            wmsLayerBoundingbox->Add(bound.substr(startPos+7,endPos-startPos-7));

            //Find east value from Bounds element 
            startPos = bound.find(L"east=\"");
            endPos = bound.find(L"\"",startPos+6); // pos+6 to the first character after east="
            wmsLayerBoundingbox->Add(bound.substr(startPos+6,endPos-startPos-6));

            //Find north value from Bounds element 
            startPos = bound.find(L"north=\"");
            endPos = bound.find(L"\"",startPos+7); // pos+7 to the first character after north="
            wmsLayerBoundingbox->Add(bound.substr(startPos+7,endPos-startPos-7));
        }
    }

    return wmsLayerBoundingbox.Detach();
}
