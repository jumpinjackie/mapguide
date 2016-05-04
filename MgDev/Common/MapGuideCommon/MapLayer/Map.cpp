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
#include "MdfModel.h"
#include "SAX2Parser.h"
#include <map>

using namespace std;

MG_IMPL_DYNCREATE(MgMap)

STRING MgMap::m_layerGroupTag = L"LayerGroupData";


//////////////////////////////////////////////////////////////
// Constructs an empty un-initialized MgMap object.
// The object cannot be used until either the Create or Open method is called.
//
MgMap::MgMap()
    : MgMapBase(),
    m_inSave(false),
    m_unpackedLayersGroups(false),
    m_colorPalette(NULL),        // lazy instantiation
    m_watermarkUsage(Viewer),
    m_tileSetId((MgResourceIdentifier*)NULL)
{
}

///////////////////////////////////////////////////////////////////////////////
/// Constructs an MgMap object that takes an MgSiteConnection instance.
/// The object cannot be used until either the Create or Open method is called.
///
MgMap::MgMap(MgSiteConnection* siteConnection)
    : MgMapBase(),
    m_inSave(false),
    m_unpackedLayersGroups(false),
    m_colorPalette(NULL),        // lazy instantiation
    m_watermarkUsage(Viewer),
    m_tileSetId((MgResourceIdentifier*)NULL)
{
    CHECKARGUMENTNULL(siteConnection, L"MgMap.MgMap");

    m_siteConnection = SAFE_ADDREF(siteConnection);
}

///////////////////////////////////////////////////////////////////////////////
/// Initialize the resource service on demand only.
///
void MgMap::InitializeResourceService(MgResourceService* resourceService)
{
    if (NULL != resourceService)
    {
        m_resourceService = SAFE_ADDREF(resourceService);
    }
    else if (NULL == m_resourceService.p)
    {
        CHECKNULL(m_siteConnection.p, L"MgMap.InitializeResourceService");

        m_resourceService = dynamic_cast<MgResourceService*>(
            m_siteConnection->CreateService(MgServiceType::ResourceService));
    }

    ACE_ASSERT(NULL != m_resourceService.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns an instance of the specified service.
///
MgService* MgMap::GetService(INT32 serviceType)
{
    CHECKNULL(m_siteConnection.p, L"MgMap.GetService");

    if (MgServiceType::ResourceService == serviceType)
    {
        InitializeResourceService(NULL);
        return SAFE_ADDREF(m_resourceService.p);
    }
    else
    {
        return m_siteConnection->CreateService(serviceType);
    }
}

//////////////////////////////////////////////////////////////
// Initializes a new Map object.
// This method is used for Mg Viewers or for offline map production.
//
void MgMap::Create(MgResourceService* resourceService, MgResourceIdentifier* resource, CREFSTRING mapName)
{
    Create(resourceService, resource, mapName, true);
}

void MgMap::Create(MgResourceService* resourceService, MgResourceIdentifier* resource, CREFSTRING mapName, bool strict)
{
    MG_TRY()

    if (resource->GetResourceType() == MgResourceType::MapDefinition)
        CreateFromMapDefinition(resourceService, resource, mapName);
    else if (resource->GetResourceType() == MgResourceType::TileSetDefinition)
        CreateFromTileSet(resourceService, resource, mapName, strict);
    else
        throw new MgInvalidResourceTypeException(L"MgMap.Create", __LINE__, __WFILE__, NULL, L"", NULL);

    MG_CATCH_AND_THROW(L"MgMap.Create")
}

///////////////////////////////////////////////////////////////////////////////
/// Initializes a new MgMap object given a map definition and a name for the map.
/// This method is used for MapGuide Viewers or for offline map production.
///
void MgMap::Create(MgResourceIdentifier* mapDefinition, CREFSTRING mapName)
{
    Create(NULL, mapDefinition, mapName);
}

void MgMap::CreateFromMapDefinition(MgResourceService* resourceService, MgResourceIdentifier* mapDefinition, CREFSTRING mapName)
{
    MG_TRY()

    InitializeResourceService(resourceService);
    m_trackChangesDisabled = true;

    m_mapDefinitionId = SAFE_ADDREF(mapDefinition);
    m_name = mapName;

    // reset the colorlist from our layers (std::list dtor clears the list)
    delete m_colorPalette;
    m_colorPalette = NULL;  // initialize to empty (lazy as its only used for 8bit color)

    // generate a unique id for this map
    MgUtil::GenerateUuid(m_objectId);

    // get the map definition from the resource repository
    Ptr<MgByteReader> content = m_resourceService->GetResourceContent(mapDefinition);
    Ptr<MgByteSink> sink = new MgByteSink(content);
    Ptr<MgByte> bytes = sink->ToBuffer();

    // parse the map definition
    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());

    if (!parser.GetSucceeded())
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);
        throw new MgInvalidMapDefinitionException(L"MgMap.Create", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // build the runtime map object from the parsed definition
    std::auto_ptr<MdfModel::TileSetDefinition> tdef;
    std::auto_ptr<MdfModel::MapDefinition> mdef(parser.DetachMapDefinition());
    assert(mdef.get() != NULL);

    MgGeometryFactory gf;
    Ptr<MgResourceIdentifier> tileSetDefId;

    //If this Map Definition links to a Tile Set Definition, its coordinate system takes precedence
    if (mdef->GetTileSourceType() == MapDefinition::TileSetDefinition)
    {
        // get the tile set definition from the resource repository
        tileSetDefId = new MgResourceIdentifier(mdef->GetTileSetSource()->GetResourceId());
        Ptr<MgByteReader> tdContent = m_resourceService->GetResourceContent(tileSetDefId);
        Ptr<MgByteSink> tdSink = new MgByteSink(tdContent);
        Ptr<MgByte> tdBytes = tdSink->ToBuffer();

        // parse the tile set definition
        MdfParser::SAX2Parser parser;
        parser.ParseString((const char*)tdBytes->Bytes(), tdBytes->GetLength());

        if (!parser.GetSucceeded())
        {
            STRING errorMsg = parser.GetErrorMessage();
            MgStringCollection arguments;
            arguments.Add(errorMsg);
            throw new MgInvalidMapDefinitionException(L"MgMap.Create", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        tdef.reset(parser.DetachTileSetDefinition());
        assert(tdef.get() != NULL);

        m_srs = GetCoordinateSystemFromTileSet(tdef.get(), true);

        //If tile CS is not the same as map def CS, then the bounds of the map def need to be transformed
        if (m_srs != mdef->GetCoordinateSystem())
        {
            Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
            Ptr<MgCoordinateSystem> mapCs = csFactory->Create(mdef->GetCoordinateSystem());
            Ptr<MgCoordinateSystem> tileCs = csFactory->Create(m_srs);
            Ptr<MgCoordinateSystemTransform> trans = csFactory->GetTransform(mapCs, tileCs);

            const Box2D& extent = mdef->GetExtents();
            Ptr<MgCoordinate> lowerLeft = gf.CreateCoordinateXY(extent.GetMinX(), extent.GetMinY());
            Ptr<MgCoordinate> upperRight = gf.CreateCoordinateXY(extent.GetMaxX(), extent.GetMaxY());
            Ptr<MgEnvelope> mapExtent = new MgEnvelope(lowerLeft, upperRight);
            Ptr<MgEnvelope> dataExtent = new MgEnvelope(lowerLeft, upperRight);

            m_mapExtent = mapExtent->Transform(trans);
            m_dataExtent = mapExtent->Transform(trans);

            Ptr<MgCoordinate> ll = mapExtent->GetLowerLeftCoordinate();
            Ptr<MgCoordinate> ur = mapExtent->GetUpperRightCoordinate();
            
            Ptr<MgCoordinate> coordCenter = gf.CreateCoordinateXY(ll->GetX() + (ur->GetX() - ll->GetX()) / 2,
                                                                  ll->GetY() + (ur->GetY() - ll->GetY()) / 2);

            m_center = gf.CreatePoint(coordCenter);
        }
        else //Both CSes match, so just set map/data extents and center as normal from the Map Definition
        {
            const Box2D& extent = mdef->GetExtents();
            Ptr<MgCoordinate> lowerLeft = gf.CreateCoordinateXY(extent.GetMinX(), extent.GetMinY());
            Ptr<MgCoordinate> upperRight = gf.CreateCoordinateXY(extent.GetMaxX(), extent.GetMaxY());
            m_mapExtent = new MgEnvelope(lowerLeft, upperRight);
            m_dataExtent = new MgEnvelope(lowerLeft, upperRight);

            Ptr<MgCoordinate> coordCenter = gf.CreateCoordinateXY(extent.GetMinX() + (extent.GetMaxX() - extent.GetMinX()) / 2,
                extent.GetMinY() + (extent.GetMaxY() - extent.GetMinY()) / 2);
            m_center = gf.CreatePoint(coordCenter);
        }
        m_backColor = mdef->GetBackgroundColor();
        m_tileSetId = SAFE_ADDREF((MgResourceIdentifier*)tileSetDefId);
    }
    else
    {
        m_srs = mdef->GetCoordinateSystem();

        const Box2D& extent = mdef->GetExtents();
        Ptr<MgCoordinate> lowerLeft = gf.CreateCoordinateXY(extent.GetMinX(), extent.GetMinY());
        Ptr<MgCoordinate> upperRight = gf.CreateCoordinateXY(extent.GetMaxX(), extent.GetMaxY());
        m_mapExtent = new MgEnvelope(lowerLeft, upperRight);
        m_dataExtent = new MgEnvelope(lowerLeft, upperRight);
        m_backColor = mdef->GetBackgroundColor();

        Ptr<MgCoordinate> coordCenter = gf.CreateCoordinateXY(extent.GetMinX() + (extent.GetMaxX() - extent.GetMinX()) / 2,
                                                              extent.GetMinY() + (extent.GetMaxY() - extent.GetMinY()) / 2);
        m_center = gf.CreatePoint(coordCenter);
    }

    //Calculate the meter per unit for the given coordinate system
    m_metersPerUnit = 1.0; // assume a default value
    if (m_srs.length() > 0)
    {
        Ptr<MgCoordinateSystemFactory> coordFactory = new MgCoordinateSystemFactory();
        Ptr<MgCoordinateSystem> coordSys = coordFactory->Create(m_srs);
        if (coordSys != NULL)
        {
            m_metersPerUnit = coordSys->ConvertCoordinateSystemUnitsToMeters(1.0);
        }
    }

    // clear the collections
    m_layers->Clear();
    m_groups->Clear();
    m_changeLists->Clear();
    m_finiteDisplayScales.clear();

    //build the runtime groups
    map<STRING, MgLayerGroup*> knownGroups;
    map<MgLayerGroup*, STRING> unresolvedGroupLinks;

    map<STRING, MgLayerGroup*>::const_iterator itKg;

    //Process dynamic layer groups
    MapLayerGroupCollection* groups = mdef->GetLayerGroups();
    if (groups)
    {
        for(int i = 0; i < groups->GetCount(); i++)
        {
            MapLayerGroup* group = (MapLayerGroup*)groups->GetAt(i);

            //create a runtime group from this group and add it to the group collection
            STRING groupName = group->GetName();
            Ptr<MgLayerGroup> rtGroup = new MgLayerGroup(groupName);
            rtGroup->SetVisible(group->IsVisible());
            rtGroup->SetLayerGroupType(MgLayerGroupType::Normal);
            rtGroup->SetDisplayInLegend(group->IsShowInLegend());
            rtGroup->SetExpandInLegend(group->IsExpandInLegend());
            rtGroup->SetLegendLabel(group->GetLegendLabel());

            knownGroups[groupName] = rtGroup;

            STRING parentGroupName = group->GetGroup();
            if(parentGroupName.length() > 0)
            {
                //attach this group to its parent if it's known yet
                itKg = knownGroups.find(parentGroupName);
                if(itKg != knownGroups.end())
                {
                    m_groups->Add(rtGroup);
                    rtGroup->SetGroup(itKg->second);
                }
                else
                {
                    unresolvedGroupLinks[rtGroup] = parentGroupName;
                    rtGroup.p->AddRef();    // since we're adding to the unresolved map
                }
            }
            else
                m_groups->Add(rtGroup);
        }
    }

    //resolve group links if required
    if (unresolvedGroupLinks.size() > 0)
    {
        map<MgLayerGroup*, STRING>::const_iterator itUnres;
        for(itUnres = unresolvedGroupLinks.begin(); itUnres != unresolvedGroupLinks.end(); ++itUnres)
        {
            itKg = knownGroups.find(itUnres->second);
            assert(itKg != knownGroups.end());
            MgLayerGroup* parentGroup = itKg->second;
            itUnres->first->SetGroup(parentGroup);

            //insert this group after its parent in the map runtime model, so we won't
            //have to worry about group order during serialization/deserialization of MgMap
            INT32 parentIdx = m_groups->IndexOf(parentGroup);
            assert(parentIdx != -1);
            m_groups->Insert(parentIdx + 1, itUnres->first);

            //since we're removing from the unresolved map
            itUnres->first->Release();
        }

        unresolvedGroupLinks.clear();
    }

    double displayOrder = LAYER_ZORDER_TOP;

    //Get all the layers (normal layer or base layer) and get the contents of them in a single request.
    Ptr<MgStringCollection> layerIds = new MgStringCollection();
    MapLayerCollection* normalLayers = mdef->GetLayers();
    if (normalLayers)
    {
        for(int i = 0; i < normalLayers->GetCount(); i ++)
        {
            layerIds->Add(normalLayers->GetAt(i)->GetLayerResourceID());
        }
    }
    BaseMapLayerGroupCollection* baseLayerGroups = NULL;
    if (NULL != tdef.get())
        baseLayerGroups = tdef->GetBaseMapLayerGroups();
    else
        baseLayerGroups = mdef->GetBaseMapLayerGroups();
    if (baseLayerGroups)
    {
        for(int i = 0; i < baseLayerGroups->GetCount(); i ++)
        {
            BaseMapLayerGroup* baseGroup = (BaseMapLayerGroup*)baseLayerGroups->GetAt(i);
            BaseMapLayerCollection* baseLayers = baseGroup->GetLayers();
            if(baseLayers)
            {
                for(int j = 0; j < baseLayers->GetCount(); j ++)
                {
                    layerIds->Add(baseLayers->GetAt(j)->GetLayerResourceID());
                }
            }
        }
    }
    std::map<STRING, STRING> layerContentPair;
    if (layerIds->GetCount() != 0)
    {
        Ptr<MgStringCollection> layerContents = m_resourceService->GetResourceContents(layerIds, NULL);
        for(int i = 0; i < layerIds->GetCount(); i ++)
        {
            layerContentPair.insert(std::pair<STRING, STRING>(layerIds->GetItem(i), layerContents->GetItem(i)));
        }
    }

    //build the runtime layers and attach them to their groups
    SCALERANGES scaleRanges;
    MapLayerCollection* layers = mdef->GetLayers();
    if (layers)
    {
        for(int i = 0; i < layers->GetCount(); i++, displayOrder += LAYER_ZORDER_INCR)
        {
            MapLayer* layer = (MapLayer*)layers->GetAt(i);
            //create a runtime layer from this layerDefinition and add it to the layer collection
            //pull identity properties as a batch process after the layers are created
            Ptr<MgResourceIdentifier> layerDefId = new MgResourceIdentifier(layer->GetLayerResourceID());
            Ptr<MgLayerBase> rtLayer = new MgLayer(layerDefId, m_resourceService, false, false);
            rtLayer->SetLayerResourceContent(layerContentPair[layerDefId->ToString()]);
            rtLayer->SetName(layer->GetName());
            rtLayer->SetVisible(layer->IsVisible());
            rtLayer->SetLayerType(MgLayerType::Dynamic);
            rtLayer->SetDisplayInLegend(layer->IsShowInLegend());
            rtLayer->SetExpandInLegend(layer->IsExpandInLegend());
            rtLayer->SetLegendLabel(layer->GetLegendLabel());
            rtLayer->SetSelectable(layer->IsSelectable());
            rtLayer->SetDisplayOrder(displayOrder);

            m_layers->Add(rtLayer);

            STRING groupName = layer->GetGroup();
            if (groupName.length())
            {
                //attach the layer to its group
                itKg = knownGroups.find(groupName);
                if(itKg == knownGroups.end())
                    throw new MgInvalidMapDefinitionException(L"MgMap.Create", __LINE__, __WFILE__, NULL, L"", NULL);

                rtLayer->SetGroup(itKg->second);
            }
        }
    }

    //done with this list
    knownGroups.clear();

    // BaseMapLayerGroups and BaseMapLayers come at the end
    BaseMapLayerGroupCollection* baseGroups = NULL;
    if (NULL != tdef.get())
        baseGroups = tdef->GetBaseMapLayerGroups();
    else
        baseGroups = mdef->GetBaseMapLayerGroups();
    if (baseGroups)
    {
        for(int i = 0; i < baseGroups->GetCount(); i++)
        {
            BaseMapLayerGroup* baseGroup = (BaseMapLayerGroup*)baseGroups->GetAt(i);

            //create a runtime group from this group and add it to the group collection
            STRING groupName = baseGroup->GetName();
            Ptr<MgLayerGroup> rtGroup = new MgLayerGroup(groupName);
            if (tdef.get() != NULL)
                rtGroup->SetLayerGroupType(MgLayerGroupType::BaseMapFromTileSet);
            else
                rtGroup->SetLayerGroupType(MgLayerGroupType::BaseMap);
            rtGroup->SetVisible(baseGroup->IsVisible());
            rtGroup->SetDisplayInLegend(baseGroup->IsShowInLegend());
            rtGroup->SetExpandInLegend(baseGroup->IsExpandInLegend());
            rtGroup->SetLegendLabel(baseGroup->GetLegendLabel());

            // NOTE: base groups do not have a parent group

            m_groups->Add(rtGroup);

            // process the base map layers for this group
            BaseMapLayerCollection* baseLayers = baseGroup->GetLayers();
            if (baseLayers)
            {
                for(int j = 0; j < baseLayers->GetCount(); j++, displayOrder += LAYER_ZORDER_INCR)
                {
                    BaseMapLayer* baseLayer = (BaseMapLayer*)baseLayers->GetAt(j);

                    // base layers should always be visible
                    assert(baseLayer->IsVisible());
                    if (baseLayer->IsVisible())
                    {
                        //create a runtime layer from this base layer and add it to the layer collection
                        Ptr<MgResourceIdentifier> layerDefId = new MgResourceIdentifier(baseLayer->GetLayerResourceID());
                        Ptr<MgLayerBase> rtLayer = new MgLayer(layerDefId, m_resourceService, true, false);
                        rtLayer->SetLayerResourceContent(layerContentPair[layerDefId->ToString()]);
                        rtLayer->SetName(baseLayer->GetName());
                        rtLayer->SetVisible(true);
                        rtLayer->SetLayerType(MgLayerType::BaseMap);
                        rtLayer->SetDisplayInLegend(baseLayer->IsShowInLegend());
                        rtLayer->SetExpandInLegend(baseLayer->IsExpandInLegend());
                        rtLayer->SetLegendLabel(baseLayer->GetLegendLabel());
                        rtLayer->SetSelectable(baseLayer->IsSelectable());
                        rtLayer->SetDisplayOrder(displayOrder);

                        m_layers->Add(rtLayer);

                        // attach the layer to its group
                        rtLayer->SetGroup(rtGroup);
                    }
                }
            }
        }
    }

    // Now that we've added all the layers (dynamic and base map) to the m_layers collection,
    // bulk load the identity properties for all layers
    Ptr<MgSiteConnection> siteConn;
    if (m_siteConnection.p != NULL)
    {
        siteConn = SAFE_ADDREF((MgSiteConnection*)m_siteConnection);
    }
    else
    {
        Ptr<MgUserInformation> userInfo = m_resourceService->GetUserInfo();
        siteConn = new MgSiteConnection();
        siteConn->Open(userInfo);
    }
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(siteConn->CreateService(MgServiceType::FeatureService));
    BulkLoadIdentityProperties(featureService);

    // build the sorted list of finite display scales
    SORTEDSCALES sortedScales;
    MdfModel::DisplayScaleCollection* displayScales = NULL;
    MdfModel::DisplayScaleCollection dispScalesFromTDef;
    if (NULL != tdef.get())
    {
        FINITESCALES fScales;
        GetFiniteDisplayScalesFromTileSet(tdef.get(), fScales, true);
        for (FINITESCALES::iterator it = fScales.begin(); it != fScales.end(); it++)
        {
            double scale = *it;

            // skip non-positive scales
            if (scale <= 0.0)
                continue;

            // skip duplicate scales
            SORTEDSCALES::iterator iter = sortedScales.find(scale);
            if (iter != sortedScales.end())
                continue;

            // insert the scale (automatically sorted in ascending order)
            sortedScales.insert(SORTEDSCALES::value_type(scale, scale));
        }
    }
    else
    {
        displayScales = mdef->GetFiniteDisplayScales();
        if (displayScales)
        {
            for(int i = 0; i < displayScales->GetCount(); i++)
            {
                DisplayScale* displayScale = displayScales->GetAt(i);
                double scale = displayScale->GetValue();

                // skip non-positive scales
                if (scale <= 0.0)
                    continue;

                // skip duplicate scales
                SORTEDSCALES::iterator iter = sortedScales.find(scale);
                if (iter != sortedScales.end())
                    continue;

                // insert the scale (automatically sorted in ascending order)
                sortedScales.insert(SORTEDSCALES::value_type(scale, scale));
            }
        }
    }

    // load the sorted scales into the vector
    for (SORTEDSCALES::iterator sIter = sortedScales.begin(); sIter != sortedScales.end(); ++sIter)
        m_finiteDisplayScales.push_back(sIter->second);

    m_trackChangesDisabled = false;

    // there's nothing to unpack anymore in this case
    m_unpackedLayersGroups = true;

    MG_CATCH_AND_THROW(L"MgMap.CreateFromMapDefinition")
}

void MgMap::CreateFromTileSet(MgResourceService* resourceService, MgResourceIdentifier* tileSetDefId, CREFSTRING mapName, bool strict)
{
    MG_TRY()

    InitializeResourceService(resourceService);
    m_trackChangesDisabled = true;

    m_mapDefinitionId = NULL;
    m_name = mapName;
    m_tileSetId = SAFE_ADDREF(tileSetDefId);

    // reset the colorlist from our layers (std::list dtor clears the list)
    delete m_colorPalette;
    m_colorPalette = NULL;  // initialize to empty (lazy as its only used for 8bit color)

    // generate a unique id for this map
    MgUtil::GenerateUuid(m_objectId);

    // get the map definition from the resource repository
    Ptr<MgByteReader> content = m_resourceService->GetResourceContent(tileSetDefId);
    Ptr<MgByteSink> sink = new MgByteSink(content);
    Ptr<MgByte> bytes = sink->ToBuffer();

    // parse the map definition
    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());

    if (!parser.GetSucceeded())
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);
        throw new MgInvalidMapDefinitionException(L"MgMap.Create", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // build the runtime map object from the parsed definition
    std::auto_ptr<MdfModel::TileSetDefinition> tdef(parser.DetachTileSetDefinition());
    assert(tdef.get() != NULL);

    MgGeometryFactory gf;

    const Box2D& extent = tdef->GetExtents();
    Ptr<MgCoordinate> lowerLeft = gf.CreateCoordinateXY(extent.GetMinX(), extent.GetMinY());
    Ptr<MgCoordinate> upperRight = gf.CreateCoordinateXY(extent.GetMaxX(), extent.GetMaxY());
    m_mapExtent = new MgEnvelope(lowerLeft, upperRight);
    m_dataExtent = new MgEnvelope(lowerLeft, upperRight);
    m_backColor = L"FFFFFFFF";

    Ptr<MgCoordinate> coordCenter = gf.CreateCoordinateXY(extent.GetMinX() + (extent.GetMaxX() - extent.GetMinX()) / 2,
                                                          extent.GetMinY() + (extent.GetMaxY() - extent.GetMinY()) / 2);
    m_center = gf.CreatePoint(coordCenter);
    m_srs = GetCoordinateSystemFromTileSet(tdef.get(), strict);

    //Calculate the meter per unit for the given coordinate system
    m_metersPerUnit = 1.0; // assume a default value
    if (m_srs.length() > 0)
    {
        Ptr<MgCoordinateSystemFactory> coordFactory = new MgCoordinateSystemFactory();
        Ptr<MgCoordinateSystem> coordSys = coordFactory->Create(m_srs);
        if (coordSys != NULL)
        {
            m_metersPerUnit = coordSys->ConvertCoordinateSystemUnitsToMeters(1.0);
        }
    }

    // clear the collections
    m_layers->Clear();
    m_groups->Clear();
    m_changeLists->Clear();
    m_finiteDisplayScales.clear();

    double displayOrder = LAYER_ZORDER_TOP;

    //Get all the layersand get the contents of them in a single request.
    Ptr<MgStringCollection> layerIds = new MgStringCollection();
    BaseMapLayerGroupCollection* baseLayerGroups = tdef->GetBaseMapLayerGroups();
    if (baseLayerGroups)
    {
        for(int i = 0; i < baseLayerGroups->GetCount(); i ++)
        {
            BaseMapLayerGroup* baseGroup = (BaseMapLayerGroup*)baseLayerGroups->GetAt(i);
            BaseMapLayerCollection* baseLayers = baseGroup->GetLayers();
            if(baseLayers)
            {
                for(int j = 0; j < baseLayers->GetCount(); j ++)
                {
                    layerIds->Add(baseLayers->GetAt(j)->GetLayerResourceID());
                }
            }
        }
    }
    std::map<STRING, STRING> layerContentPair;
    if (layerIds->GetCount() != 0)
    {
        Ptr<MgStringCollection> layerContents = m_resourceService->GetResourceContents(layerIds, NULL);
        for(int i = 0; i < layerIds->GetCount(); i ++)
        {
            layerContentPair.insert(std::pair<STRING, STRING>(layerIds->GetItem(i), layerContents->GetItem(i)));
        }
    }

    // BaseMapLayerGroups and BaseMapLayers come at the end
    BaseMapLayerGroupCollection* baseGroups = tdef->GetBaseMapLayerGroups();
    if (baseGroups)
    {
        for(int i = 0; i < baseGroups->GetCount(); i++)
        {
            BaseMapLayerGroup* baseGroup = (BaseMapLayerGroup*)baseGroups->GetAt(i);

            //create a runtime group from this group and add it to the group collection
            STRING groupName = baseGroup->GetName();
            Ptr<MgLayerGroup> rtGroup;
            rtGroup = new MgLayerGroup(groupName);
            rtGroup->SetVisible(baseGroup->IsVisible());
            rtGroup->SetLayerGroupType(MgLayerGroupType::BaseMapFromTileSet);
            rtGroup->SetDisplayInLegend(baseGroup->IsShowInLegend());
            rtGroup->SetExpandInLegend(baseGroup->IsExpandInLegend());
            rtGroup->SetLegendLabel(baseGroup->GetLegendLabel());

            // NOTE: base groups do not have a parent group

            m_groups->Add(rtGroup);

            // process the base map layers for this group
            BaseMapLayerCollection* baseLayers = baseGroup->GetLayers();
            if (baseLayers)
            {
                for(int j = 0; j < baseLayers->GetCount(); j++, displayOrder += LAYER_ZORDER_INCR)
                {
                    BaseMapLayer* baseLayer = (BaseMapLayer*)baseLayers->GetAt(j);

                    // base layers should always be visible
                    assert(baseLayer->IsVisible());
                    if (baseLayer->IsVisible())
                    {
                        //create a runtime layer from this base layer and add it to the layer collection
                        Ptr<MgResourceIdentifier> layerDefId = new MgResourceIdentifier(baseLayer->GetLayerResourceID());
                        Ptr<MgLayerBase> rtLayer = new MgLayer(layerDefId, m_resourceService, true, false);
                        rtLayer->SetLayerResourceContent(layerContentPair[layerDefId->ToString()]);
                        rtLayer->SetName(baseLayer->GetName());
                        rtLayer->SetVisible(true);
                        rtLayer->SetLayerType(MgLayerType::BaseMap);
                        rtLayer->SetDisplayInLegend(baseLayer->IsShowInLegend());
                        rtLayer->SetExpandInLegend(baseLayer->IsExpandInLegend());
                        rtLayer->SetLegendLabel(baseLayer->GetLegendLabel());
                        rtLayer->SetSelectable(baseLayer->IsSelectable());
                        rtLayer->SetDisplayOrder(displayOrder);

                        m_layers->Add(rtLayer);

                        // attach the layer to its group
                        rtLayer->SetGroup(rtGroup);
                    }
                }
            }
        }
    }

    // Now that we've added all the layers (dynamic and base map) to the m_layers collection,
    // bulk load the identity properties for all layers
    Ptr<MgSiteConnection> siteConn;
    if (m_siteConnection.p != NULL)
    {
        siteConn = SAFE_ADDREF((MgSiteConnection*)m_siteConnection);
    }
    else
    {
        Ptr<MgUserInformation> userInfo = m_resourceService->GetUserInfo();
        siteConn = new MgSiteConnection();
        siteConn->Open(userInfo);
    }
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(siteConn->CreateService(MgServiceType::FeatureService));
    BulkLoadIdentityProperties(featureService);

    // build the sorted list of finite display scales
    SORTEDSCALES sortedScales;
    FINITESCALES displayScales;
    GetFiniteDisplayScalesFromTileSet(tdef.get(), displayScales, strict);
    if (displayScales.size() > 0)
    {
        for (FINITESCALES::iterator it = displayScales.begin(); it != displayScales.end(); it++)
        {
            double scale = *it;

            // skip non-positive scales
            if (scale <= 0.0)
                continue;

            // skip duplicate scales
            SORTEDSCALES::iterator iter = sortedScales.find(scale);
            if (iter != sortedScales.end())
                continue;

            // insert the scale (automatically sorted in ascending order)
            sortedScales.insert(SORTEDSCALES::value_type(scale, scale));
        }
    }

    // load the sorted scales into the vector
    for (SORTEDSCALES::iterator sIter = sortedScales.begin(); sIter != sortedScales.end(); ++sIter)
        m_finiteDisplayScales.push_back(sIter->second);

    m_trackChangesDisabled = false;

    // there's nothing to unpack anymore in this case
    m_unpackedLayersGroups = true;

    MG_CATCH_AND_THROW(L"MgMap.CreateFromTileSet")
}

STRING MgMap::GetCoordinateSystemFromTileSet(MdfModel::TileSetDefinition* tileset, bool strict)
{
    //Yes, this is hard-coded against specific providers. Revisit if we do want
    //to FDO-ize this tile provider concept.

    MdfModel::TileStoreParameters* storeParams = tileset->GetTileStoreParameters();
    if (storeParams->GetTileProvider() == MG_TILE_PROVIDER_DEFAULT)
    {
        MdfModel::NameStringPairCollection* parameters = storeParams->GetParameters();
        for (INT32 i = 0; i < parameters->GetCount(); i++)
        {
            MdfModel::NameStringPair* nsp = parameters->GetAt(i);
            if (nsp->GetName() == MG_TILE_PROVIDER_COMMON_PARAM_COORDINATESYSTEM)
            {
                return nsp->GetValue();
            }
        }
    }
    else if (storeParams->GetTileProvider() == MG_TILE_PROVIDER_XYZ)
    {
        //XYZ is always WGS84.PseudoMercator
        Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
        return csFactory->ConvertCoordinateSystemCodeToWkt(L"WGS84.PseudoMercator"); //NOXLATE
    }
    if (strict)
    {
        MgStringCollection args;
        args.Add(storeParams->GetTileProvider());
        throw new MgUnsupportedTileProviderException(L"MgMap.GetCoordinateSystemFromTileSet", __LINE__, __WFILE__, &args, L"", NULL);
    }
    else
    {
        return L"";
    }
}

void MgMap::GetFiniteDisplayScalesFromTileSet(MdfModel::TileSetDefinition* tileset, FINITESCALES& scales, bool strict)
{
    //Yes, this is hard-coded against specific providers. Revisit if we do want
    //to FDO-ize this tile provider concept.

    MdfModel::TileStoreParameters* storeParams = tileset->GetTileStoreParameters();
    if (storeParams->GetTileProvider() == MG_TILE_PROVIDER_DEFAULT) //NOXLATE
    {
        MdfModel::NameStringPairCollection* parameters = storeParams->GetParameters();
        for (INT32 i = 0; i < parameters->GetCount(); i++)
        {
            MdfModel::NameStringPair* nsp = parameters->GetAt(i);
            if (nsp->GetName() == MG_TILE_PROVIDER_COMMON_PARAM_FINITESCALELIST) //NOXLATE
            {
                Ptr<MgStringCollection> values = MgStringCollection::ParseCollection(nsp->GetValue(), L","); //NOXLATE
                for (INT32 i = 0; i < values->GetCount(); i++)
                {
                    double val = MgUtil::StringToDouble(values->GetItem(i));
                    scales.push_back(val);
                }
            }
        }
    }
    else if (strict)
    {
        MgStringCollection args;
        args.Add(storeParams->GetTileProvider());
        throw new MgUnsupportedTileProviderException(L"MgMap.GetFiniteDisplayScalesFromTileSet", __LINE__, __WFILE__, &args, L"", NULL);
    }
}

//////////////////////////////////////////////////////////////
// Call down to base class implementation.  Ptr<> seems to be
// messing this up.  Weird
//
void MgMap::Create(CREFSTRING mapSRS, MgEnvelope* mapExtent, CREFSTRING mapName)
{
    m_name = mapName;
    MgMapBase::Create(mapSRS, mapExtent, mapName);
    m_unpackedLayersGroups = true;

    // reset the colorlist from our layers (std::list dtor clears the list)
    delete m_colorPalette;
    m_colorPalette = NULL;  // initialize to empty (lazy as its only used for 8bit color)
}


//////////////////////////////////////////////////////////////
// Opens the resource using the specified resource service and resource identifier.
//
void MgMap::Open(MgResourceService* resourceService, CREFSTRING mapName)
{
    MG_TRY()

    InitializeResourceService(resourceService);
    m_trackChangesDisabled = true;

    STRING sessionId;
    Ptr<MgUserInformation> userInfo;

    if (NULL == m_siteConnection.p)
    {
        userInfo = m_resourceService->GetUserInfo();
    }
    else
    {
        userInfo = m_siteConnection->GetUserInfo();
    }

    if (NULL != userInfo.p)
    {
        sessionId = userInfo->GetMgSessionId();
    }

    if (sessionId.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"0");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgMap.Open",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(L"Session:" + sessionId + L"//" + mapName + L"." + MgResourceType::Map);

    MgResource::Open(m_resourceService, resId);
    m_name = mapName;

    //Note: Layers and Groups are loaded on demand by UnpackLayersAndGroups

    m_trackChangesDisabled = false;

    MG_CATCH_AND_THROW(L"MgMap.Open")
}

///////////////////////////////////////////////////////////////////////////////
/// Loads the map object from a session repository.
///
void MgMap::Open(CREFSTRING mapName)
{
    Open(NULL, mapName);
}

//////////////////////////////////////////////////////////////
// Saves the resource using the specified resource service and resource identifier.
// This method assumes a valid resource identifier has already been established
// for this resource via either Open or Save
//
void MgMap::Save(MgResourceService* resourceService)
{
    MG_TRY()

    InitializeResourceService(resourceService);

    CHECKNULL(m_resId.p, L"MgMap.Save");

    m_inSave = true;

    SerializeToRepository(m_resourceService, false);

    Ptr<MgMemoryStreamHelper> streamHelper = PackLayersAndGroups();
    if (NULL != (MgMemoryStreamHelper*) streamHelper)
    {
        //Make a byte reader out of the memory stream
        Ptr<MgByteSource> bsource = new MgByteSource((BYTE_ARRAY_IN)streamHelper->GetBuffer(), streamHelper->GetLength());
        Ptr<MgByteReader> resourceData = bsource->GetReader();

        m_resourceService->SetResourceData(m_resId, m_layerGroupTag, L"Stream", resourceData);
    }

    MG_CATCH(L"MgMap.Save")
    if (NULL != mgException)
    {
        m_inSave = false;
        MG_THROW();
    }

    m_inSave = false;
}


//////////////////////////////////////////////////////////////
// Saves the resource using the specified resource service and resource identifier.
//
void MgMap::Save(MgResourceService* resourceService, MgResourceIdentifier* resourceId)
{
    MG_TRY()

    InitializeResourceService(resourceService);

    m_resId = SAFE_ADDREF(resourceId);

    CHECKNULL(m_resId.p, L"MgMap.Save");

    m_inSave = true;

    SerializeToRepository(m_resourceService, true);

    Ptr<MgMemoryStreamHelper> streamHelper = PackLayersAndGroups();
    if (NULL != (MgMemoryStreamHelper*) streamHelper)
    {
        //Make a byte reader out of the memory stream
        Ptr<MgByteSource> bsource = new MgByteSource((BYTE_ARRAY_IN)streamHelper->GetBuffer(), streamHelper->GetLength());
        Ptr<MgByteReader> resourceData = bsource->GetReader();

        m_resourceService->SetResourceData(m_resId, m_layerGroupTag, L"Stream", resourceData);
    }

    MG_CATCH(L"MgMap.Save")
    if (NULL != mgException)
    {
        m_inSave = false;
        MG_THROW();
    }

    m_inSave = false;
}

///////////////////////////////////////////////////////////////////////////////
/// Saves the Map.
///
void MgMap::Save()
{
    if (NULL == m_resId.p)
    {
        if (NULL == m_siteConnection.p || m_name.empty())
        {
            throw new MgNullReferenceException(L"MgMap.Save",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            Ptr<MgUserInformation> userInfo = m_siteConnection->GetUserInfo();
            STRING sessionId = userInfo->GetMgSessionId();
            Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(
                L"Session:" + sessionId + L"//" + m_name + L"." + MgResourceType::Map);

            Save(NULL, resId);
        }
    }
    else
    {
        Save(NULL);
    }
}

//////////////////////////////////////////////////////////////
// Destruct a MgMap object
//
MgMap::~MgMap()
{
    // reset the colorlist from our layers (std::list dtor clears the list)
    delete m_colorPalette;
}


//////////////////////////////////////////////////////////////
void MgMap::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// \brief
/// Unpacks layers and groups from memory stream - lazy initialization
///
/// How does lazy initialization work?  Basically, the layer and group
/// collections are stored in a separate binary blob within MgMap.  For
/// large maps, the "layers groups" blob can be tens/hundreds of kbytes.
/// In some cases, the application does not actually need this information
/// from MgMap so serializing it is a big waste of resources.
///
/// All saved MgMap objects know how to pull the layer groups blob on the
/// fly using the internal m_resourceService/m_resId.  If GetLayers() or
/// GetLayerGroups() is called then the blob will automatically be pulled.
/// Note: m_resourceService must be set using SetDelayedLoadResourceService
/// if MgMap is not set using Create or Open (i.e. when deserialized).
///
/// The "layers groups" blob is only serialized on the wire if it has
/// changed.  If none of the collections contain data then they are assumed
/// to be unchanged.
///
/// The same applies to the Save.  If the collections do not contain data
/// then they will not be saved.
///
void MgMap::UnpackLayersAndGroups()
{
    // check if we already unpacked things
    if (m_unpackedLayersGroups)
        return;

    // Temporary byte array for on-demand loading.
    Ptr<MgByte> bytes;

    if (NULL == (MgMemoryStreamHelper*) m_layerGroupHelper)
    {
        // Need to query from Resource Service
        if (NULL != m_resourceService.p)
        {
            Ptr<MgByteReader> breader = m_resourceService->GetResourceData(m_resId, m_layerGroupTag);

            MgByteSink sink(breader);
            bytes = sink.ToBuffer();
            m_layerGroupHelper = new MgMemoryStreamHelper((INT8*) bytes->Bytes(), bytes->GetLength(), false);
        }
        else
        {
            // If this exception is thrown then the calling code should probably initialize the
            // resource service using SetDelayedLoadResourceService()
            throw new MgInvalidOperationException(L"MgMap.UnpackLayersAndGroups", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    m_trackChangesDisabled = true;

    Ptr<MgStream> stream = new MgStream(m_layerGroupHelper);
    MgStreamReader* streamReader = (MgStreamReader*)stream;

    //groups
    //this map speeds up the process of attaching groups together and attaching layers to groups
    map<STRING, MgLayerGroup*> knownGroups;

    map<STRING, MgLayerGroup*>::const_iterator itKg;
    INT32 groupCount;
    streamReader->GetInt32(groupCount);

    STRING parentGroupName;
    for(int groupIndex = 0; groupIndex < groupCount; groupIndex++)
    {
        streamReader->GetString(parentGroupName);
        Ptr<MgLayerGroup> group = (MgLayerGroup*)streamReader->GetObject();
        STRING groupName = group->GetName();

        knownGroups[groupName] = group;

        if(parentGroupName.length() > 0)
        {
            //attach this group to its parent
            itKg = knownGroups.find(parentGroupName);
            assert(itKg != knownGroups.end());
            group->SetGroup(itKg->second);
        }

        m_groups->Add(group);
    }

    //layers
    INT32 layerCount;
    streamReader->GetInt32(layerCount);

    m_layers->SetCheckForDuplicates(false);

    for(int layerIndex = 0; layerIndex < layerCount; layerIndex++)
    {
        streamReader->GetString(parentGroupName);
        Ptr<MgLayerBase> layer = (MgLayerBase*)streamReader->GetObject();

        if(parentGroupName.length() > 0)
        {
            //attach this group to its parent
            itKg = knownGroups.find(parentGroupName);
            assert(itKg != knownGroups.end());
            layer->SetGroup(itKg->second);
        }

        m_layers->Add(layer);
    }

    m_layers->SetCheckForDuplicates(true);

    //done with this list
    knownGroups.clear();

    // Throw away the blob data.  It is no longer valid.
    streamReader = NULL;
    stream = NULL;
    m_layerGroupHelper = NULL;
    bytes = NULL;

    m_trackChangesDisabled = false;

    m_unpackedLayersGroups = true;
}


//////////////////////////////////////////////////////////////////
/// \brief
/// Packs layers and groups to a memory stream (lazy initialization)
///
MgMemoryStreamHelper* MgMap::PackLayersAndGroups()
{
    if (!m_unpackedLayersGroups)
    {
        // Nothing to pack if we haven't unpacked the data.  Return NULL.
        return NULL;
    }

    m_trackChangesDisabled = true;

    Ptr<MgMemoryStreamHelper> streamHelper = new MgMemoryStreamHelper();
    Ptr<MgStream> stream = new MgStream(streamHelper);

    //This is to trap layers attached to groups not in this map
    std::map<STRING, MgLayerGroup*> groups;

    //groups
    INT32 groupCount = m_groups->GetCount();
    stream->WriteInt32(groupCount);
    for(int groupIndex = 0; groupIndex < groupCount; groupIndex++)
    {
        Ptr<MgLayerGroup> group = m_groups->GetItem(groupIndex);
        Ptr<MgLayerGroup> parent = group->GetGroup();
        groups[group->GetObjectId()] = group;
        stream->WriteString(parent != NULL? parent->GetName(): L"");
        stream->WriteObject(group);
    }

    //layers
    INT32 layerCount = m_layers->GetCount();
    stream->WriteInt32(layerCount);
    for(int layerIndex = 0; layerIndex < layerCount; layerIndex++)
    {
        Ptr<MgLayerBase> layer = m_layers->GetItem(layerIndex);
        Ptr<MgLayerGroup> parent = layer->GetGroup();
        if (NULL != parent.p && groups.find(parent->GetObjectId()) == groups.end())
        {
            MgStringCollection args;
            args.Add(layer->GetName());
            args.Add(parent->GetName());
            throw new MgGroupNotFoundException(L"MgMap.PackLayersAndGroups", __LINE__, __WFILE__, NULL, L"MgLayerBelongsToGroupNotInMap", &args);
        }
        stream->WriteString(parent != NULL? parent->GetName(): L"");
        stream->WriteObject(layer);
    }

    m_trackChangesDisabled = false;

    stream = NULL;
    return streamHelper.Detach();
}


//////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgMap::Serialize(MgStream* stream)
{
    //version of object in case we need to revision again
    stream->WriteInt32(m_serializeVersion);

    //resource id for MgMap
    stream->WriteObject(m_resId);
    //map name
    stream->WriteString(m_name);
    //map unique id
    stream->WriteString(m_objectId);
    //map definition id
    stream->WriteObject(m_mapDefinitionId);
    //coordinate system
    stream->WriteString(m_srs);
    //map extent - cannot be NULL - set by Create() or Open()
    stream->WriteObject(m_mapExtent);
    //center
    // cannot be NULL - set by Create() or Open()s
    stream->WriteObject(m_center);
    //scale
    stream->WriteDouble(m_scale);
    //data extent
    stream->WriteObject(m_dataExtent);
    //display dpi
    stream->WriteInt32(m_displayDpi);
    //display size
    stream->WriteInt32(m_displayWidth);
    stream->WriteInt32(m_displayHeight);
    //background color
    stream->WriteString(m_backColor);
    //meters per unit
    stream->WriteDouble(m_metersPerUnit);
    //layer refresh mode
    stream->WriteInt32((INT32)m_layerRefreshMode);

    //finite display scales
    INT32 scaleCount = (INT32)m_finiteDisplayScales.size();
    stream->WriteInt32(scaleCount);
    if (scaleCount > 0)
    {
        for (FINITESCALES::const_iterator it = m_finiteDisplayScales.begin(); it != m_finiteDisplayScales.end(); ++it)
            stream->WriteDouble(*it);
    }

    //change lists
    INT32 changeListCount = m_changeLists->GetCount();
    stream->WriteInt32(changeListCount);
    for(INT32 i = 0; i < changeListCount; i++)
    {
        Ptr<MgChangeList> changeList = (MgChangeList*)m_changeLists->GetItem(i);
        stream->WriteBoolean(changeList->IsLayer());
        stream->WriteString(changeList->GetObjectId());
        stream->WriteInt32(changeList->GetChangeCount());
        for(INT32 j = 0; j < changeList->GetChangeCount(); j++)
        {
            Ptr<MgObjectChange> change = (MgObjectChange*)changeList->GetChangeAt(j);
            stream->WriteInt32((INT32)change->GetType());
            stream->WriteString(change->GetParam());
        }
    }

    //watermark usage
    stream->WriteInt32(m_watermarkUsage);

    //tile set definition
    stream->WriteObject(m_tileSetId);

    // Serialize Layers and Groups as a blob.
    if (m_inSave)
    {
        // Never serialize blob when saving.
        stream->WriteInt32(0);
    }
    else
    {
        Ptr<MgMemoryStreamHelper> streamHelper = PackLayersAndGroups();
        if (NULL == (MgMemoryStreamHelper*) streamHelper)
        {
           // Data has not changed.  Do not serialize.
           stream->WriteInt32(0);
        }
        else
        {
           stream->WriteInt32(streamHelper->GetLength());
           Ptr<MgStreamHelper> helper = stream->GetStreamHelper();
           helper->WriteBytes((const unsigned char*) streamHelper->GetBuffer(), streamHelper->GetLength());
        }
    }
}


//////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgMap::Deserialize(MgStream* stream)
{
    MgStreamReader* streamReader = (MgStreamReader*)stream;

    m_trackChangesDisabled = true;

    INT32 version = 0;
    //version of object
    stream->GetInt32(version);

    if (version != m_serializeVersion)
    {
        throw new MgStreamIoException(L"MgMap.Deserialize", __LINE__, __WFILE__, NULL, L"MgInvalidTCPProtocol", NULL);
    }

    //resource id for MgMap
    m_resId = (MgResourceIdentifier*)streamReader->GetObject();
    //map name
    streamReader->GetString(m_name);
    //map unique id
    streamReader->GetString(m_objectId);
    //map definition id
    m_mapDefinitionId = (MgResourceIdentifier*)streamReader->GetObject();
    //coordinate system
    streamReader->GetString(m_srs);
    //map extent
    m_mapExtent = (MgEnvelope*)streamReader->GetObject();
    //center
    m_center = (MgPoint*)streamReader->GetObject();
    //scale
    streamReader->GetDouble(m_scale);
    //data extent  - use GetObject when MgEnvelope will serialize
    m_dataExtent = (MgEnvelope*)streamReader->GetObject();
    //display dpi
    streamReader->GetInt32(m_displayDpi);
    //display size
    streamReader->GetInt32(m_displayWidth);
    streamReader->GetInt32(m_displayHeight);
    //background color
    streamReader->GetString(m_backColor);
    //meters per unit
    streamReader->GetDouble(m_metersPerUnit);
    //layer refresh mode
    streamReader->GetInt32((INT32&)m_layerRefreshMode);

    //finite display scales
    INT32 scaleCount;
    streamReader->GetInt32(scaleCount);
    for (INT32 i=0; i<scaleCount; i++)
    {
        double displayScale;
        streamReader->GetDouble(displayScale);
        m_finiteDisplayScales.push_back(displayScale);
    }

    //change lists
    INT32 changeListCount;
    streamReader->GetInt32(changeListCount);

    m_changeLists->SetCheckForDuplicates(false);

    for(INT32 i = 0; i < changeListCount; i++)
    {
        STRING objectId;
        bool isLayer;

        streamReader->GetBoolean(isLayer);
        streamReader->GetString(objectId);

        Ptr<MgChangeList> changeList = new MgChangeList(objectId, isLayer);
        m_changeLists->Add(changeList);

        INT32 changeCount;
        streamReader->GetInt32(changeCount);
        for(INT32 j = 0; j < changeCount; j++)
        {
            INT32 type;
            stream->GetInt32(type);

            Ptr<MgObjectChange> change = new MgObjectChange((MgObjectChange::ChangeType)type);

            STRING param;
            streamReader->GetString(param);
            change->SetParam(param);

            changeList->AddChange(change);
        }
    }

    m_changeLists->SetCheckForDuplicates(true);

    //watermark usage
    streamReader->GetInt32(m_watermarkUsage);

    //tile set definition
    m_tileSetId = (MgResourceIdentifier*)stream->GetObject();

    //blob for layers and groups
    INT32 nBytes = 0;
    streamReader->GetInt32(nBytes);
    m_layerGroupHelper = NULL;
    if (nBytes > 0)
    {
        INT8* buf = new INT8[nBytes];
        streamReader->GetData((void*) buf, nBytes);
        m_layerGroupHelper = new MgMemoryStreamHelper(buf, nBytes, true);
    }

    m_trackChangesDisabled = false;
}


//////////////////////////////////////////////////////////////////
/// \brief
/// Sets internal resource service references.  Used for Lazy loading
///
void MgMap::SetDelayedLoadResourceService(MgResourceService* resourceService)
{
    CHECKARGUMENTNULL(resourceService, L"MgMap.SetDelayedLoadResourceService");

    m_resourceService = SAFE_ADDREF(resourceService);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Callback method invoked when the parent of a layer has changed.
///
void MgMap::OnLayerParentChanged(MgLayerBase* layer, CREFSTRING parentId)
{
    layer->ForceRefresh();

    MgMapBase::OnLayerParentChanged(layer, parentId);
}

void MgMap::BulkLoadIdentityProperties(MgFeatureService* featureService)
{
    /// Typedefs for populating identity properties into the map
    typedef std::list<MgLayer*> LayerList;
    typedef std::map<STRING,LayerList> LayerClassMap;
    typedef std::map<STRING,LayerClassMap> LayerSchemaMap;
    typedef std::map<STRING,LayerSchemaMap> LayerFeatureSourceMap;

    LayerFeatureSourceMap fsMap;

    // Group layers by their feature source / class name
    for(int i = 0; i < m_layers->GetCount(); i++)
    {
        STRING className;
        STRING schemaName;
        Ptr<MgLayer> layer = dynamic_cast<MgLayer*>(m_layers->GetItem(i));
        STRING featureSource = layer->GetFeatureSourceId();
        if (!featureSource.empty())
        {
            layer->ParseFeatureName(featureService, className, schemaName);
            if (!schemaName.empty())
            {
                fsMap[featureSource][schemaName][className].push_back(layer.p);
            }
        }
    }

    for (LayerFeatureSourceMap::iterator fsIter = fsMap.begin(); fsIter != fsMap.end(); ++fsIter)
    {
        // Assumption:  feature source is only referencing one schema
        STRING featureSource = fsIter->first;
        Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(featureSource);
        LayerSchemaMap& schemaList = fsIter->second;
        for (LayerSchemaMap::iterator schemaIter = schemaList.begin(); schemaIter != schemaList.end(); ++schemaIter)
        {
            STRING schemaName = schemaIter->first;
            LayerClassMap& classList = schemaIter->second;
            Ptr<MgStringCollection> classNames = new MgStringCollection();
            for (LayerClassMap::iterator cIter = classList.begin(); cIter != classList.end(); ++cIter)
            {
                classNames->Add(cIter->first);
            }

            try
            {
                // Ignore failures when pulling identity properties
                Ptr<MgClassDefinitionCollection> partialDefs = featureService->GetIdentityProperties(resId, schemaName, classNames);
                for (int i = 0; i < partialDefs->GetCount(); i++)
                {
                    Ptr<MgClassDefinition> def = partialDefs->GetItem(i);
                    STRING className = def->GetName();

                    LayerList& layers = fsMap[featureSource][schemaName][className];
                    for (LayerList::iterator lIter = layers.begin(); lIter != layers.end(); ++lIter)
                    {
                        (*lIter)->PopulateIdentityProperties(def);
                    }
                }
            }
            catch (MgException* e)
            {
                e->Release();
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////
// RFC60 ColorPalette Accessor uses lazy instantiation as it's not used
// for truecolor tiles.  It's used for the map colors collected from the
// stylization of the visible layers.
ColorStringList& MgMap::GetColorPalette()
{
    if (m_colorPalette == NULL)
        m_colorPalette = new ColorStringList();
    else
    {
        m_colorPalette->sort();
        m_colorPalette->unique();
    }
    return *m_colorPalette;
}

////////////////////////////////////////////////////////////////////////
// RFC60 setter adds list.
void MgMap::AddColorsToPalette(ColorStringList& newColorPalette)
{
    if (NULL == m_colorPalette)
        GetColorPalette();  // lazy instantiation

    if (!newColorPalette.empty())
    {
        ColorStringList::iterator it = newColorPalette.begin();
        for (; it != newColorPalette.end(); ++it)
        {
            if (*it != L"")    // filter empty strings
            {
                // uppercase the colorcodes (FFABDEFF)
                std::transform((*it).begin(), (*it).end(), (*it).begin(), towupper);
                m_colorPalette->push_back(*it);
            }
        }
    }
}

INT32 MgMap::GetWatermarkUsage()
{
    return m_watermarkUsage;
}

void MgMap::SetWatermarkUsage(INT32 watermarkUsage)
{
    m_watermarkUsage = watermarkUsage;
}

//////////////////////////////////////////////////////////////////
// Returns the resource id of the Tile Set Definition that this group originates from.
//
MgResourceIdentifier* MgMap::GetTileSetDefinition()
{
    return SAFE_ADDREF((MgResourceIdentifier*)m_tileSetId);
}