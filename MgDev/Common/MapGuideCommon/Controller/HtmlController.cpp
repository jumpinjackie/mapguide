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

    if (persist)
    {
        //save the selection set in the session repository
        Ptr<MgSelection> selection;
        if (NULL != featureInfo.p)
            selection = featureInfo->GetSelection();
        if(!selection)
            selection = new MgSelection(map);
        selection->Save(resourceService, mapName);
        newSelection = SAFE_ADDREF(selection.p);

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

    // Collect any attributes of selected features
    if ((requestData & REQUEST_ATTRIBUTES) == REQUEST_ATTRIBUTES)
    {
        // This could be chunky for big selections, but client applications can control this via MAXFEATURES, so the onus is on them
        attributes = service->QueryFeatureProperties(map, layerNames, selectionGeometry, selectionVariant, L"", maxFeatures, layerAttributeFilter, true);
    }

    result = CollectQueryMapFeaturesResult(resourceService, requestData, featureInfo, newSelection, attributes, inlineSelectionImg);

    // Return XML
    return result.Detach();
}

MgByteReader* MgHtmlController::CollectQueryMapFeaturesResult(MgResourceService* resourceService,
                                                              INT32 requestData,
                                                              MgFeatureInformation* featInfo,
                                                              MgSelection* selectionSet,
                                                              MgBatchPropertyCollection* attributes, 
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

    if (((requestData & REQUEST_ATTRIBUTES) == REQUEST_ATTRIBUTES) && NULL != attributes)
    {
        xml.append(L"<SelectedFeatures>\n");
        WriteSelectedFeatureAttributes(resourceService, selectionSet, attributes, xml);
        xml.append(L"</SelectedFeatures>\n");
    }
    else
        xml.append(L"<SelectedFeatures />\n");

    xml.append(L"</FeatureInformation>\n");

    string xmlDoc = MgUtil::WideCharToMultiByte(xml);
    STRING mimeType = L"text/xml";
    return MgUtil::GetByteReader(xmlDoc, &mimeType);
}

void MgHtmlController::WriteSelectedFeatureAttributes(MgResourceService* resourceService,
                                                      MgSelection* selectionSet,
                                                      MgBatchPropertyCollection* attributes,
                                                      REFSTRING xmlOut)
{
    //Rather than doing a verbatim xml dump of MgBatchPropertyCollection, let's output something
    //that is more intuitive for client applications using this service operation to understand.

    //We could assume sorted layers in the MgBatchPropertyCollection, but play safe
    //and store our per-layer XML fragments in a bucket (keyed on layer name) and return 
    //the merged bucket result at the end
    std::map<STRING, STRING> bucket;

    MgAgfReaderWriter agfRw;
    MgWktReaderWriter wktRw;

    Ptr<MgReadOnlyLayerCollection> selLayers = selectionSet->GetLayers();
    if (NULL != selLayers.p)
    {
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
            if (bucket.find(layerName) == bucket.end())
            {
                STRING xml = L"<SelectedLayer id=\"";
                xml.append(selLayer->GetObjectId());
                xml.append(L"\" name=\"");
                xml.append(selLayer->GetName());
                xml.append(L"\">\n");
                xml.append(L"<LayerMetadata>\n");
                Ptr<MgClassDefinition> clsDef = selLayer->GetClassDefinition();
                Ptr<MgPropertyDefinitionCollection> clsProps = clsDef->GetProperties();
                Ptr<MgResourceIdentifier> layerId = selLayer->GetLayerDefinition();
                //We need the property mappings of the source layer definition to compile our layer metadata
                std::auto_ptr<MdfModel::LayerDefinition> ldf(MgLayerBase::GetLayerDefinition(resourceService, layerId));
                if (ldf.get() != NULL)
                {
                    MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
                    if(vl != NULL)
                    {
                        MdfModel::NameStringPairCollection* pmappings = vl->GetPropertyMappings();
                        for (int j=0; j<pmappings->GetCount(); j++)
                        {
                            STRING pTypeStr;
                            MdfModel::NameStringPair* m = pmappings->GetAt(j);
                            INT32 pidx = clsProps->IndexOf(m->GetName());
                            if (pidx >= 0)
                            {
                                Ptr<MgPropertyDefinition> propDef = clsProps->GetItem(pidx);
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
                                xml.append(L"<Property>\n");
                                xml.append(L"<Name>");
                                xml.append(m->GetName());
                                xml.append(L"</Name>\n");
                                xml.append(L"<Type>");
                                xml.append(pTypeStr);
                                xml.append(L"</Type>\n");
                                xml.append(L"<DisplayName>");
                                xml.append(m->GetValue());
                                xml.append(L"</DisplayName>\n");
                                xml.append(L"</Property>\n");
                            }
                        }
                    }
                }
                xml += L"</LayerMetadata>\n";
                bucket[layerName] = xml;
            }
        }

        for (INT32 i = 0; i < attributes->GetCount(); i++)
        {
            Ptr<MgPropertyCollection> featProps = attributes->GetItem(i);
            INT32 lidx = featProps->IndexOf(L"_MgLayerName");
            INT32 fidx = featProps->IndexOf(L"_MgFeatureBoundingBox");

            //Must have both the _MgLayerName and _MgFeatureBoundingBox
            if (lidx < 0 || fidx < 0)
                continue;

            Ptr<MgStringProperty> layerNameProp = (MgStringProperty*)featProps->GetItem(lidx);
            Ptr<MgStringProperty> boundsProp = (MgStringProperty*)featProps->GetItem(fidx);

            //Locate the matching bucketed fragment to append to
            STRING layerName = layerNameProp->GetValue();
            if (bucket.find(layerName) != bucket.end())
            {
                //Good to go, write our feature to this bucketed fragment
                REFSTRING xml = bucket[layerName];
                xml.append(L"<Feature>\n");
                xml.append(L"<Bounds>");
                xml.append(boundsProp->GetValue());
                xml.append(L"</Bounds>\n");
                for (INT32 p = 0; p < featProps->GetCount(); p++)
                {
                    //Skip special properties
                    if (p == lidx || p == fidx)
                        continue;
                    Ptr<MgNullableProperty> prop = dynamic_cast<MgNullableProperty*>(featProps->GetItem(p));
                    if (NULL != prop.p)
                    {
                        xml.append(L"<Property>\n");
                        xml.append(L"<Name>");
                        xml.append(prop->GetName());
                        xml.append(L"</Name>\n");
                        //We'll follow MgProperty spec. Null is represented by omission of <Value>
                        if (!prop->IsNull())
                        {
                            INT32 ptype = prop->GetPropertyType();
                            switch(ptype)
                            {
                            //case MgPropertyType::Blob:
                            case MgPropertyType::Boolean:
                                {
                                    xml.append(L"<Value>");
                                    xml.append(((MgBooleanProperty*)prop.p)->GetValue() ? L"true" : L"false");
                                    xml.append(L"</Value>\n");
                                }
                                break;
                            case MgPropertyType::Byte:
                                {
                                    STRING sVal;
                                    MgUtil::Int32ToString((INT32)((MgByteProperty*)prop.p)->GetValue(), sVal);
                                    xml.append(L"<Value>");
                                    xml.append(sVal);
                                    xml.append(L"</Value>\n");
                                }
                                break;
                            //case MgPropertyType::Clob:
                            case MgPropertyType::DateTime:
                                {
                                    Ptr<MgDateTime> dt = ((MgDateTimeProperty*)prop.p)->GetValue();
                                    xml.append(L"<Value>");
                                    xml.append(dt->ToXmlString());
                                    xml.append(L"</Value>\n");
                                }
                                break;
                            case MgPropertyType::Decimal:
                            case MgPropertyType::Double:
                                {
                                    STRING sVal;
                                    MgUtil::DoubleToString(((MgDoubleProperty*)prop.p)->GetValue(), sVal);
                                    xml.append(L"<Value>");
                                    xml.append(sVal);
                                    xml.append(L"</Value>\n");
                                }
                                break;
                            case MgPropertyType::Geometry:
                                {
                                    try 
                                    {
                                        Ptr<MgByteReader> agf = ((MgGeometryProperty*)prop.p)->GetValue();
                                        Ptr<MgGeometry> geom = agfRw.Read(agf);
                                        STRING wkt = wktRw.Write(geom);
                                        xml.append(L"<Value>");
                                        xml.append(wkt);
                                        xml.append(L"</Value>\n");
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
                                    MgUtil::Int32ToString((INT32)((MgInt16Property*)prop.p)->GetValue(), sVal);
                                    xml.append(L"<Value>");
                                    xml.append(sVal);
                                    xml.append(L"</Value>\n");
                                }
                                break;
                            case MgPropertyType::Int32:
                                {
                                    STRING sVal;
                                    MgUtil::Int32ToString(((MgInt32Property*)prop.p)->GetValue(), sVal);
                                    xml.append(L"<Value>");
                                    xml.append(sVal);
                                    xml.append(L"</Value>\n");
                                }
                                break;
                            case MgPropertyType::Int64:
                                {
                                    STRING sVal;
                                    MgUtil::Int64ToString(((MgInt64Property*)prop.p)->GetValue(), sVal);
                                    xml.append(L"<Value>");
                                    xml.append(sVal);
                                    xml.append(L"</Value>\n");
                                }
                                break;
                            case MgPropertyType::Single:
                                {
                                    STRING sVal;
                                    MgUtil::SingleToString(((MgSingleProperty*)prop.p)->GetValue(), sVal);
                                    xml.append(L"<Value>");
                                    xml.append(sVal);
                                    xml.append(L"</Value>\n");
                                }
                                break;
                            case MgPropertyType::String:
                                {
                                    xml.append(L"<Value>");
                                    xml.append(MgUtil::ReplaceEscapeCharInXml(((MgStringProperty*)prop.p)->GetValue()));
                                    xml.append(L"</Value>\n");
                                }
                                break;
                            }
                        }
                        xml.append(L"</Property>\n");
                    }
                }
                xml.append(L"</Feature>\n");
            }
        }
    
        //Now merge the bucketed fragments
        for (std::map<STRING, STRING>::iterator it = bucket.begin(); it != bucket.end(); it++)
        {
            //Close off the <SelectedLayer> elements
            STRING xml = it->second;
            xml.append(L"</SelectedLayer>");
            //Good to append to the final result
            xmlOut.append(xml);
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
