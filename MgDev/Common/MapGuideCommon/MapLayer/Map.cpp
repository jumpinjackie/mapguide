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

#include "MapGuideCommon.h"
#include "MdfModel.h"
#include "SAX2Parser.h"
#include <map>
#include <vector>

using namespace std;

IMPLEMENT_CREATE_OBJECT(MgMap)


//////////////////////////////////////////////////////////////
// Constructs an empty un-initialized MgMap object.
// The object cannot be used until either the Create or Open method is called.
//
MgMap::MgMap()
    : MgMapBase()
{
}

//////////////////////////////////////////////////////////////
// Initializes a new Map object.
// This method is used for Mg Viewers or for offline map production.
//
void MgMap::Create(MgResourceService* resourceService, MgResourceIdentifier* mapDefinition, CREFSTRING mapName)
{    
    MG_TRY()

    m_trackChangesDisabled = true;

    m_mapDefinitionId = mapDefinition;
    SAFE_ADDREF((MgResourceIdentifier*)m_mapDefinitionId);
    m_name = mapName;

    //Generate a unique id for this map
    MgUtil::GenerateUuid(m_objectId);

    //get the map definition from the resource repository
    Ptr<MgByteReader> content = resourceService->GetResourceContent(mapDefinition);

    Ptr<MgByteSink> sink = new MgByteSink(content);
    Ptr<MgByte> bytes = sink->ToBuffer();

    //parse the MDF definition
    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());

    if(!parser.GetSucceeded())
    {
        STRING errorMsg = L"Invalid map definition";  //TODO pull the string from the resources
        throw new MgInvalidMapDefinitionException(L"MgMap.Create", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // build the runtime map object from the parsed definition
    MdfModel::MapDefinition* mdef = parser.GetMapDefinition();
    assert(mdef != NULL);

    MgGeometryFactory gf;

    const Box2D& extent = mdef->GetExtents();
    Ptr<MgCoordinate> lowerLeft = gf.CreateCoordinateXY(extent.GetMinX(), extent.GetMinY());
    Ptr<MgCoordinate> upperRight = gf.CreateCoordinateXY(extent.GetMaxX(), extent.GetMaxY());
    m_mapExtent = new MgEnvelope(lowerLeft, upperRight);
    m_dataExtent = new MgEnvelope(lowerLeft, upperRight);
    m_backColor = mdef->GetBackgroundColor();

    Ptr<MgCoordinate> coordCenter = gf.CreateCoordinateXY(extent.GetMinX() + (extent.GetMaxX() - extent.GetMinX()) / 2,
                                                          extent.GetMinY() + (extent.GetMaxY() - extent.GetMinY()) / 2);
    m_center = gf.CreatePoint(coordCenter);
    m_srs = mdef->GetCoordinateSystem();

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

    bool showInLegend;
    MapLayerGroupCollection* groups = mdef->GetLayerGroups();
    if(groups)
    {
        for(int i = 0; i < groups->GetCount(); i++)
        {
            MapLayerGroup* group = (MapLayerGroup*)groups->GetAt(i);

            //create a runtime group from this group and add it to the group collection
            STRING groupName = group->GetName();
            Ptr<MgLayerGroup> rtGroup = new MgLayerGroup(groupName);
            rtGroup->SetVisible(group->IsVisible());
            rtGroup->SetLayerGroupType(MgLayerGroupType::Normal);
            rtGroup->SetDisplayInLegend(showInLegend = group->IsShowInLegend());
            if(showInLegend)
            {
                rtGroup->SetExpandInLegend(group->IsExpandInLegend());
                rtGroup->SetLegendLabel(group->GetLegendLabel());
            }

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
    if(unresolvedGroupLinks.size() > 0)
    {
        map<MgLayerGroup*, STRING>::const_iterator itUnres;
        for(itUnres = unresolvedGroupLinks.begin(); itUnres != unresolvedGroupLinks.end(); itUnres++)
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

    //build the runtime layers and attach them to their groups
    SCALERANGES scaleRanges;
    MapLayerCollection* layers = mdef->GetLayers();
    if(layers)
    {
        for(int i = 0; i < layers->GetCount(); i++, displayOrder += LAYER_ZORDER_INCR)
        {
            MapLayer* layer = (MapLayer*)layers->GetAt(i);
            //create a runtime layer from this layer and add it to the layer collection
            Ptr<MgResourceIdentifier> layerDefId = new MgResourceIdentifier(layer->GetLayerResourceID());
            Ptr<MgLayerBase> rtLayer = new MgLayer(layerDefId, resourceService);
            rtLayer->SetName(layer->GetName());
            rtLayer->SetVisible(layer->IsVisible());
            rtLayer->SetLayerType(MgLayerType::Dynamic);
            rtLayer->SetDisplayInLegend(layer->IsShowInLegend());
            if(layer->IsShowInLegend())
            {
                rtLayer->SetExpandInLegend(layer->IsExpandInLegend());
                rtLayer->SetLegendLabel(layer->GetLegendLabel());
            }
            rtLayer->SetSelectable(layer->IsSelectable());
            rtLayer->SetDisplayOrder(displayOrder);

            m_layers->Add(rtLayer);

            STRING groupName = layer->GetGroup();
            if(groupName.length())
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
    BaseMapLayerGroupCollection* baseGroups = mdef->GetBaseMapLayerGroups();
    if(baseGroups)
    {
        for(int i = 0; i < baseGroups->GetCount(); i++)
        {
            BaseMapLayerGroup* baseGroup = (BaseMapLayerGroup*)baseGroups->GetAt(i);

            //create a runtime group from this group and add it to the group collection
            STRING groupName = baseGroup->GetName();
            Ptr<MgLayerGroup> rtGroup = new MgLayerGroup(groupName);
            rtGroup->SetVisible(baseGroup->IsVisible());
            rtGroup->SetLayerGroupType(MgLayerGroupType::BaseMap);
            rtGroup->SetDisplayInLegend(showInLegend = baseGroup->IsShowInLegend());
            if(showInLegend)
            {
                rtGroup->SetExpandInLegend(baseGroup->IsExpandInLegend());
                rtGroup->SetLegendLabel(baseGroup->GetLegendLabel());
            }

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
                        Ptr<MgLayerBase> rtLayer = new MgLayer(layerDefId, resourceService);
                        rtLayer->SetName(baseLayer->GetName());
                        rtLayer->SetVisible(true);
                        rtLayer->SetLayerType(MgLayerType::BaseMap);
                        rtLayer->SetDisplayInLegend(baseLayer->IsShowInLegend());
                        if(baseLayer->IsShowInLegend())
                        {
                            rtLayer->SetExpandInLegend(baseLayer->IsExpandInLegend());
                            rtLayer->SetLegendLabel(baseLayer->GetLegendLabel());
                        }
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

    // build the sorted list of finite display scales
    SORTEDSCALES sortedScales;
    DisplayScaleCollection* displayScales = mdef->GetFiniteDisplayScales();
    if(displayScales)
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

    // load the sorted scales into the vector
    for (SORTEDSCALES::iterator sIter = sortedScales.begin(); sIter != sortedScales.end(); sIter++)
        m_finiteDisplayScales.push_back(sIter->second);

    m_trackChangesDisabled = false;

    MG_CATCH_AND_THROW(L"MgMap.Create")
}

//////////////////////////////////////////////////////////////
// Call down to base class implementation.  Ptr<> seems to be
// messing this up.  Weird
//
void MgMap::Create(CREFSTRING mapSRS, MgEnvelope* mapExtent, CREFSTRING mapName)
{
    MgMapBase::Create(mapSRS, mapExtent, mapName);
}


//////////////////////////////////////////////////////////////
// Opens the resource using the specified resource service and resource identifier.
//
void MgMap::Open(MgResourceService* resourceService, CREFSTRING mapName)
{
    m_trackChangesDisabled = true;

    STRING sessionId;
    Ptr<MgUserInformation> userInfo = resourceService->GetUserInfo();

    if (userInfo.p != NULL)
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
    MgResource::Open(resourceService, resId);

    m_trackChangesDisabled = false;
}


//////////////////////////////////////////////////////////////
// Destruct a MgMap object
//
MgMap::~MgMap()
{
}


//////////////////////////////////////////////////////////////
void MgMap::Dispose()
{
    delete this;
}
