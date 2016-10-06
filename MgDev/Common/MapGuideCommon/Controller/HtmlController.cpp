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

#include "MapGuideCommon.h"
#include "Base64.h"
#include "VectorLayerDefinition.h"
#include "NameStringPair.h"

typedef std::map<STRING, STRING> DisplayNameMap;

#define REQUEST_ATTRIBUTES       1
#define REQUEST_INLINE_SELECTION 2
#define REQUEST_TOOLTIP          4
#define REQUEST_HYPERLINK        8

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
    ApplyMapViewCommands(map, mapViewCommands, true);

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
    return GetMapImage(map, selection, format, mapViewCommands, bKeepSelection, bClip, NULL, true);
}

//////////////////////////////////////////////////////////////////
// Processes a GetMapImage request from the Viewer and returns an image of the specified map.
//
MgByteReader* MgHtmlController::GetMapImage(MgMap* map, MgSelection* selection,
    CREFSTRING format, MgPropertyCollection* mapViewCommands, bool bKeepSelection, bool bClip, MgColor* selectionColor, bool layersAndGroupsAreIds)
{
    // Apply map view commands
    ApplyMapViewCommands(map, mapViewCommands, layersAndGroupsAreIds);

    // Make sure we clear any track changes - these are not applicable for AJAX.
    if (NULL != map)
        map->ClearChanges();

    // Get Proxy Rendering Service instance
    Ptr<MgRenderingService> service = (MgRenderingService*)(GetService(MgServiceType::RenderingService));

    // Call the C++ API
    return service->RenderMap(map, selection, format, bKeepSelection, bClip, selectionColor);
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
    ApplyMapViewCommands(map, mapViewCommands, true);

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
    INT32 layerAttributeFilter,
    INT32 requestData,
    CREFSTRING selectionColor,
    CREFSTRING selectionFormat)
{
    Ptr<MgByteReader> result;
    Ptr<MgFeatureInformation> featureInfo;
    Ptr<MgBatchPropertyCollection> attributes;
    Ptr<MgByteReader> inlineSelectionImg;
    Ptr<MgSelection> newSelection;

    // Create a Resource Service instance
    Ptr<MgResourceService> resourceService = (MgResourceService*)GetService(MgServiceType::ResourceService);
    Ptr<MgFeatureService> featureService = (MgFeatureService*)GetService(MgServiceType::FeatureService);

    // Create MgMap
    Ptr<MgMap> map = new MgMap(m_siteConn);
    map->Open(mapName);

    // Make sure we clear any track changes - these are not applicable for AJAX.
    Ptr<MgNamedCollection> changeLists = map->GetChangeLists();
    if (changeLists->GetCount() > 0)
    {
        map->ClearChanges();
        map->Save();
    }

    // Create Proxy Rendering Service instance
    Ptr<MgRenderingService> service = (MgRenderingService*)(GetService(MgServiceType::RenderingService));

    // Call the C++ API, regardless of bitmask as any part of the mask will require information from this API
    featureInfo = service->QueryFeatures(map, layerNames, selectionGeometry, selectionVariant, featureFilter, maxFeatures, layerAttributeFilter);

    Ptr<MgSelection> selection;
    if (NULL != featureInfo.p)
        selection = featureInfo->GetSelection();

    bool bSetMap = true;
    if (!selection)
    {
        selection = new MgSelection(map);
        bSetMap = false;
    }

    if (persist)
    {
        //save the selection set in the session repository
        selection->Save(resourceService, mapName);
    }
    newSelection = SAFE_ADDREF(selection.p);

    if (bSetMap)
    {   
        //Needed for GetLayers() to work below
        newSelection->SetMap(map);
    }

    // Render an image of this selection if requested
    if (((requestData & REQUEST_INLINE_SELECTION) == REQUEST_INLINE_SELECTION) && NULL != newSelection.p)
    {
        Ptr<MgColor> selColor = new MgColor(selectionColor);
        Ptr<MgRenderingOptions> renderOpts = new MgRenderingOptions(selectionFormat, MgRenderingOptions::RenderSelection | MgRenderingOptions::KeepSelection, selColor);
        inlineSelectionImg = service->RenderDynamicOverlay(map, newSelection, renderOpts);
    }

    result = CollectQueryMapFeaturesResult(resourceService, featureService, map, requestData, featureInfo, newSelection, inlineSelectionImg);

    // Return XML
    return result.Detach();
}

MgByteReader* MgHtmlController::CollectQueryMapFeaturesResult(MgResourceService* resourceService,
                                                              MgFeatureService* featureService,
                                                              MgMapBase* map,
                                                              INT32 requestData,
                                                              MgFeatureInformation* featInfo,
                                                              MgSelection* selectionSet,
                                                              MgByteReader* inlineSelection)
{
    STRING xml;
    STRING tooltip;
    STRING hyperlink;
    STRING xmlSelection = selectionSet? selectionSet->ToXml(false): L"";

    if (NULL != featInfo)
    {
        tooltip = featInfo->GetTooltip();
        hyperlink = featInfo->GetHyperlink();
    }

    // TODO: Stil haven't defined a schema for v2.6. Should we?
    xml.append(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<FeatureInformation>\n");

    size_t len = xmlSelection.length();
    if(len > 0)
    {
        xml.reserve(len + 2048);
        xml.append(xmlSelection);
    }
    else
    {
        xml.reserve(2048);
        xml.append(L"<FeatureSet />\n");
    }

    if (((requestData & REQUEST_TOOLTIP) == REQUEST_TOOLTIP) && !tooltip.empty())
    {
        xml.append(L"<Tooltip>");
        xml.append(MgUtil::ReplaceEscapeCharInXml(tooltip));
        xml.append(L"</Tooltip>\n");
    }
    else
        xml.append(L"<Tooltip />\n");

    if (((requestData & REQUEST_HYPERLINK) == REQUEST_HYPERLINK) && !hyperlink.empty())
    {
        xml.append(L"<Hyperlink>");
        xml.append(MgUtil::ReplaceEscapeCharInXml(hyperlink));
        xml.append(L"</Hyperlink>\n");
    }
    else
        xml.append(L"<Hyperlink />\n");

    if (((requestData & REQUEST_INLINE_SELECTION) == REQUEST_INLINE_SELECTION) && NULL != inlineSelection)
    {
        xml.append(L"<InlineSelectionImage>\n");
        xml.append(L"<MimeType>");
        xml.append(inlineSelection->GetMimeType());
        xml.append(L"</MimeType>\n");
        xml.append(L"<Content>");
        MgByteSink sink(inlineSelection);
        Ptr<MgByte> bytes = sink.ToBuffer();
        Ptr<MgMemoryStreamHelper> streamHelper = new MgMemoryStreamHelper((INT8*) bytes->Bytes(), bytes->GetLength(), false);
        std::string b64 = streamHelper->ToBase64();
        STRING wb64 = MgUtil::MultiByteToWideChar(b64);
        xml.append(wb64);
        xml.append(L"</Content>\n");
        xml.append(L"</InlineSelectionImage>\n");
    }
    else
        xml.append(L"<InlineSelectionImage />\n");

    if ((requestData & REQUEST_ATTRIBUTES) == REQUEST_ATTRIBUTES)
    {
        xml.append(L"<SelectedFeatures>\n");
        WriteSelectedFeatureAttributes(resourceService, featureService, map, selectionSet, xml);
        xml.append(L"</SelectedFeatures>\n");
    }
    else
        xml.append(L"<SelectedFeatures />\n");

    xml.append(L"</FeatureInformation>\n");

    string xmlDoc = MgUtil::WideCharToMultiByte(xml);
    STRING mimeType = L"text/xml";
    return MgUtil::GetByteReader(xmlDoc, &mimeType);
}

MgCoordinateSystemTransform* MgHtmlController::GetLayerToMapTransform(MgLayerBase* layer, 
                                                                      MgCoordinateSystem* mapCs, 
                                                                      MgCoordinateSystemFactory* csFactory, 
                                                                      MgFeatureService* featureService)
{
    Ptr<MgCoordinateSystemTransform> trans;

    MG_TRY()

    Ptr<MgClassDefinition> clsDef = layer->GetClassDefinition();
    Ptr<MgPropertyDefinitionCollection> clsProps = clsDef->GetProperties();
    INT32 gidx = clsProps->IndexOf(layer->GetFeatureGeometryName());
    if (gidx >= 0)
    {
        Ptr<MgPropertyDefinition> propDef = clsProps->GetItem(gidx);
        MgGeometricPropertyDefinition* geomProp = static_cast<MgGeometricPropertyDefinition*>(propDef.p);
        STRING scName = geomProp->GetSpatialContextAssociation();
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(layer->GetFeatureSourceId());
        Ptr<MgSpatialContextReader> scReader = featureService->GetSpatialContexts(fsId, false);
        while(scReader->ReadNext())
        {
            if (scReader->GetName() == scName)
            {
                Ptr<MgCoordinateSystem> layerCs = csFactory->Create(scReader->GetCoordinateSystemWkt());
                trans = csFactory->GetTransform(layerCs, mapCs);
                break;
            }
        }
        scReader->Close();
    }

    MG_CATCH_AND_RELEASE()

    return trans.Detach();
}

void MgHtmlController::WriteSelectedFeatureAttributes(MgResourceService* resourceService,
                                                      MgFeatureService* featureService,
                                                      MgMapBase* map,
                                                      MgSelection* selectionSet,
                                                      REFSTRING xmlOut)
{
    MgAgfReaderWriter agfRw;
    MgWktReaderWriter wktRw;
    MgCoordinateSystemFactory csFactory;

    Ptr<MgReadOnlyLayerCollection> selLayers = selectionSet->GetLayers();
    if (NULL != selLayers.p)
    {
        Ptr<MgCoordinateSystem> mapCs;
        if (map->GetMapSRS() != L"")
            mapCs = csFactory.Create(map->GetMapSRS());
        // Our structure is as follows
        //
        // [0...n] <SelectedLayer> - A layer containing selected features
        //   [1] <LayerMetadata> - Describing properties, its display name and its property type. Due to how QueryFeatureProperties works, display names
        //                         are used for property names for each feature. This element provides a reverse lookup table to ascertain the FDO system 
        //                         property name for each attribute for client applications that require such information
        //     [0...n] <Feature> - Each selected feature in the layer
        //       [1] <Bounds> - The feature's bounding box [minx miny maxx maxy]
        //       [0...n] <Property> - Property value for current feature

        for (INT32 i = 0; i < selLayers->GetCount(); i++)
        {
            Ptr<MgLayerBase> selLayer = selLayers->GetItem(i);
            STRING layerName = selLayer->GetName();

            Ptr<MgCoordinateSystemTransform> transform;
            if (mapCs)
                transform = GetLayerToMapTransform(selLayer, mapCs, &csFactory, featureService);

            xmlOut.append(L"<SelectedLayer id=\"");
            xmlOut.append(selLayer->GetObjectId());
            xmlOut.append(L"\" name=\"");
            xmlOut.append(selLayer->GetName());
            xmlOut.append(L"\">\n");
            xmlOut.append(L"<LayerMetadata>\n");
            Ptr<MgClassDefinition> clsDef = selLayer->GetClassDefinition();
            Ptr<MgPropertyDefinitionCollection> clsProps = clsDef->GetProperties();
            Ptr<MgResourceIdentifier> layerId = selLayer->GetLayerDefinition();
            Ptr<MgStringCollection> propNames = new MgStringCollection();
            DisplayNameMap displayNameMap;
            //We need the property mappings of the source layer definition to compile our layer metadata
            std::auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(resourceService, layerId));
            if (ldf.get() != NULL)
            {
                MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
                if(vl != NULL)
                {
                    STRING pTypeStr;
                    MdfModel::NameStringPairCollection* pmappings = vl->GetPropertyMappings();
                    for (int j=0; j<pmappings->GetCount(); j++)
                    {
                        MdfModel::NameStringPair* m = pmappings->GetAt(j);
                        const MdfModel::MdfString& name = m->GetName();
                        const MdfModel::MdfString& dispName = m->GetValue();
                        propNames->Add(name);
                        displayNameMap.insert(std::make_pair(name, dispName));

                        INT32 propIndex = clsProps->IndexOf(name);
                        if (propIndex < 0) {
                            continue;
                        }

                        Ptr<MgPropertyDefinition> propDef = clsProps->GetItem(propIndex);
                        INT32 pdType = propDef->GetPropertyType();
                        INT32 pType = MgPropertyType::Null;
                        if (pdType == MgFeaturePropertyType::DataProperty)
                        {
                            pType = ((MgDataPropertyDefinition*)propDef.p)->GetDataType();
                        }
                        else if (pdType == MgFeaturePropertyType::GeometricProperty)
                        {
                            pType = MgPropertyType::Geometry;
                        }
                        MgUtil::Int32ToString(pType, pTypeStr);
                        xmlOut.append(L"<Property>\n");
                        xmlOut.append(L"<Name>");
                        xmlOut.append(name);
                        xmlOut.append(L"</Name>\n");
                        xmlOut.append(L"<Type>");
                        xmlOut.append(pTypeStr);
                        xmlOut.append(L"</Type>\n");
                        xmlOut.append(L"<DisplayName>");
                        xmlOut.append(dispName);
                        xmlOut.append(L"</DisplayName>\n");
                        xmlOut.append(L"</Property>\n");
                    }
                }
            }
            if (!propNames->Contains(selLayer->GetFeatureGeometryName()))
                propNames->Add(selLayer->GetFeatureGeometryName()); //Don't forget geometry
            xmlOut.append(L"</LayerMetadata>\n");
            Ptr<MgReader> reader = selectionSet->GetSelectedFeatures(selLayer, selLayer->GetFeatureClassName(), propNames);
            while(reader->ReadNext())
            {
                xmlOut.append(L"<Feature>\n");
                STRING geomPropName = selLayer->GetFeatureGeometryName();
                if (!reader->IsNull(geomPropName))
                {
                    try 
                    {
                        Ptr<MgByteReader> agf = reader->GetGeometry(geomPropName);
                        Ptr<MgGeometry> geom = agfRw.Read(agf, transform);
                        Ptr<MgEnvelope> env = geom->Envelope();
                        Ptr<MgCoordinate> ll = env->GetLowerLeftCoordinate();
                        Ptr<MgCoordinate> ur = env->GetUpperRightCoordinate();
                        xmlOut.append(L"<Bounds>");
                        STRING str;
                        MgUtil::DoubleToString(ll->GetX(), str);
                        xmlOut.append(str);
                        xmlOut.append(L" ");
                        MgUtil::DoubleToString(ll->GetY(), str);
                        xmlOut.append(str);
                        xmlOut.append(L" ");
                        MgUtil::DoubleToString(ur->GetX(), str);
                        xmlOut.append(str);
                        xmlOut.append(L" ");
                        MgUtil::DoubleToString(ur->GetY(), str);
                        xmlOut.append(str);
                        xmlOut.append(L"</Bounds>\n");
                    }
                    catch (MgException* ex) //Bad geom maybe
                    {
                        SAFE_RELEASE(ex);
                    }
                }
                
                for (INT32 i = 0; i < reader->GetPropertyCount(); i++)
                {
                    STRING propName = reader->GetPropertyName(i);
                    DisplayNameMap::iterator it = displayNameMap.find(propName);
                    //Skip properties without display mappings
                    if (it == displayNameMap.end())
                        continue;
                    
                    xmlOut.append(L"<Property>\n");
                    xmlOut.append(L"<Name>");
                    xmlOut.append(it->second);
                    xmlOut.append(L"</Name>\n");
                    if (!reader->IsNull(i))
                    {
                        INT32 ptype = reader->GetPropertyType(i);
                        switch(ptype)
                        {
                        //case MgPropertyType::Blob:
                        case MgPropertyType::Boolean:
                            {
                                xmlOut.append(L"<Value>");
                                xmlOut.append(reader->GetBoolean(i) ? L"true" : L"false");
                                xmlOut.append(L"</Value>\n");
                            }
                            break;
                        case MgPropertyType::Byte:
                            {
                                STRING sVal;
                                MgUtil::Int32ToString((INT32)reader->GetByte(i), sVal);
                                xmlOut.append(L"<Value>");
                                xmlOut.append(sVal);
                                xmlOut.append(L"</Value>\n");
                            }
                            break;
                        //case MgPropertyType::Clob:
                        case MgPropertyType::DateTime:
                            {
                                Ptr<MgDateTime> dt = reader->GetDateTime(i);
                                xmlOut.append(L"<Value>");
                                //ToXmlString() won't work with dates before Jan 1, 1970, so use yyyy-mm-dd hh:mm:ss
                                STRING dateStr;
                                STRING str;
                                if (dt->GetYear() != -1)
                                {
                                    MgUtil::Int32ToString(dt->GetYear(), str);
                                    dateStr += str;
                                    MgUtil::Int32ToString(dt->GetMonth(), str);
                                    dateStr += L"-";
                                    MgUtil::PadLeft(str, 2, L'0');
                                    dateStr += str;
                                    MgUtil::Int32ToString(dt->GetDay(), str);
                                    dateStr += L"-";
                                    MgUtil::PadLeft(str, 2, L'0');
                                    dateStr += str;
                                }
                                if (dt->GetHour() != -1)
                                {
                                    if (dt->GetYear() != -1)
                                    {
                                        dateStr += L" ";
                                    }
                                    MgUtil::Int32ToString(dt->GetHour(), str);
                                    MgUtil::PadLeft(str, 2, L'0');
                                    dateStr += str;
                                    MgUtil::Int32ToString(dt->GetMinute(), str);
                                    dateStr += L":";
                                    MgUtil::PadLeft(str, 2, L'0');
                                    dateStr += str;
                                    MgUtil::Int32ToString(dt->GetSecond(), str);
                                    dateStr += L":";
                                    MgUtil::PadLeft(str, 2, L'0');
                                    dateStr += str;
                                }
                                xmlOut.append(dateStr);
                                xmlOut.append(L"</Value>\n");
                            }
                            break;
                        case MgPropertyType::Decimal:
                        case MgPropertyType::Double:
                            {
                                STRING sVal;
                                MgUtil::DoubleToString(reader->GetDouble(i), sVal);
                                xmlOut.append(L"<Value>");
                                xmlOut.append(sVal);
                                xmlOut.append(L"</Value>\n");
                            }
                            break;
                        case MgPropertyType::Geometry:
                            {
                                try 
                                {
                                    Ptr<MgByteReader> agf = reader->GetGeometry(i);
                                    Ptr<MgGeometry> geom = agfRw.Read(agf);
                                    STRING wkt = wktRw.Write(geom);
                                    xmlOut.append(L"<Value>");
                                    xmlOut.append(wkt);
                                    xmlOut.append(L"</Value>\n");
                                }
                                catch (MgException* ex) //Bad geom maybe
                                {
                                    SAFE_RELEASE(ex);
                                }
                            }
                            break;
                        case MgPropertyType::Int16:
                            {
                                STRING sVal;
                                MgUtil::Int32ToString((INT32)reader->GetInt16(i), sVal);
                                xmlOut.append(L"<Value>");
                                xmlOut.append(sVal);
                                xmlOut.append(L"</Value>\n");
                            }
                            break;
                        case MgPropertyType::Int32:
                            {
                                STRING sVal;
                                MgUtil::Int32ToString(reader->GetInt32(i), sVal);
                                xmlOut.append(L"<Value>");
                                xmlOut.append(sVal);
                                xmlOut.append(L"</Value>\n");
                            }
                            break;
                        case MgPropertyType::Int64:
                            {
                                STRING sVal;
                                MgUtil::Int64ToString(reader->GetInt64(i), sVal);
                                xmlOut.append(L"<Value>");
                                xmlOut.append(sVal);
                                xmlOut.append(L"</Value>\n");
                            }
                            break;
                        case MgPropertyType::Single:
                            {
                                STRING sVal;
                                MgUtil::SingleToString(reader->GetSingle(i), sVal);
                                xmlOut.append(L"<Value>");
                                xmlOut.append(sVal);
                                xmlOut.append(L"</Value>\n");
                            }
                            break;
                        case MgPropertyType::String:
                            {
                                xmlOut.append(L"<Value>");
                                xmlOut.append(MgUtil::ReplaceEscapeCharInXml(reader->GetString(i)));
                                xmlOut.append(L"</Value>\n");
                            }
                            break;
                        }
                    }
                    xmlOut.append(L"</Property>\n");
                }
                xmlOut.append(L"</Feature>\n");
            }
            reader->Close();
            xmlOut.append(L"</SelectedLayer>");
        }
    }
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
void MgHtmlController::ApplyMapViewCommands(MgMap* map, MgPropertyCollection* mapViewCommands, bool layersAndGroupsAreIds)
{
    if(mapViewCommands == NULL)
        return;

    //apply commands common to both type of viewers
    MgController::ApplyMapViewCommands(map, mapViewCommands, layersAndGroupsAreIds);
}

//////////////////////////////////////////////////////////////////
// Destruct a MgHtmlController object
//
MgHtmlController::~MgHtmlController()
{
}
