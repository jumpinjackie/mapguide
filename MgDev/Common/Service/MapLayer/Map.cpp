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

#include "AceCommon.h"
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
    : m_center(NULL),
      m_scale(1.0),
      m_mapExtent(NULL),
      m_dataExtent(NULL),
      m_displayDpi(96),
      m_displayWidth(0),
      m_displayHeight(0),
      m_trackChangesDisabled(false),
      m_layerRefreshMode(unspecified),
      m_metersPerUnit(1.0)
{
    m_layers = new MgLayerCollection(this);
    m_groups = new MgLayerGroupCollection(this);
    m_changeLists = new MgNamedCollection();
    m_backColor = L"FFFFFFFF";      //default to opaque white
}


//////////////////////////////////////////////////////////////
// Returns the name of the map.
//
STRING MgMap::GetName()
{
    return m_name;
}


//////////////////////////////////////////////////////////////
// Returns the client session id of the map.
//
STRING MgMap::GetSessionId()
{
    //session id is the repository name for this resource
    if(!m_resId)
        return L"";
    return m_resId->GetRepositoryName();
}


//////////////////////////////////////////////////////////////
// Returns the id that can be used to uniquely identify this map.
//
STRING MgMap::GetObjectId()
{
    return m_objectId;
}


//////////////////////////////////////////////////////////////
// Returns the resource identifier this map was created from.
//
MgResourceIdentifier* MgMap::GetMapDefinition()
{
    return SAFE_ADDREF((MgResourceIdentifier*)m_mapDefinitionId);
}


//////////////////////////////////////////////////////////////
// Returns the spatial reference system used to display the map
// in OpenGIS Well Know Text (WKT) format
//
STRING MgMap::GetMapSRS()
{
    return m_srs;
}


//////////////////////////////////////////////////////////////
// Returns the the extent of the overall map
//
MgEnvelope* MgMap::GetMapExtent()
{
    return SAFE_ADDREF(m_mapExtent);
}


//////////////////////////////////////////////////////////////
// Returns the collection of layers
//
MgLayerCollection* MgMap::GetLayers()
{
    return SAFE_ADDREF((MgLayerCollection*)m_layers);
}


//////////////////////////////////////////////////////////////
// Returns the collection of layer groups
//
MgLayerGroupCollection* MgMap::GetLayerGroups()
{
    return SAFE_ADDREF((MgLayerGroupCollection*)m_groups);
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
            Ptr<MgLayer> rtLayer = new MgLayer(layerDefId, resourceService);
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
                        Ptr<MgLayer> rtLayer = new MgLayer(layerDefId, resourceService);
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
// Initializes a new Map object.
// This method is used for the WMS service implementation and creates a Map without
// any layers or finite display scales.
//
void MgMap::Create(CREFSTRING mapSRS, MgEnvelope* mapExtent, CREFSTRING mapName)
{
    MG_TRY()

    m_trackChangesDisabled = true;

    //Generate a unique id for this map
    MgUtil::GenerateUuid(m_objectId);

    m_mapDefinitionId = (MgResourceIdentifier*)NULL;
    m_name = mapName;
    m_mapExtent = new MgEnvelope(mapExtent);
    m_dataExtent = new MgEnvelope(mapExtent);
    m_backColor = L"FFFFFFFF";

    // clear the collections
    m_layers->Clear();
    m_groups->Clear();
    m_changeLists->Clear();
    m_finiteDisplayScales.clear();

    ////////////////////////////////////////
    Ptr<MgCoordinate> lowerLeft = mapExtent->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> upperRight = mapExtent->GetUpperRightCoordinate();
    double centerX = (lowerLeft->GetX() + upperRight->GetX()) / 2;
    double centerY = (lowerLeft->GetY() + upperRight->GetY()) / 2;
    Ptr<MgCoordinateXY> centerCoord = new MgCoordinateXY(centerX, centerY);
    m_center = new MgPoint(centerCoord);
    m_srs = mapSRS;

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

    m_trackChangesDisabled = false;

    MG_CATCH_AND_THROW(L"MgMap.Create")
}


//////////////////////////////////////////////////////////////
// Opens the resource using the specified resource service and resource identifier.
//
void MgMap::Open(MgResourceService* resourceService, CREFSTRING mapName)
{
    m_trackChangesDisabled = true;

    STRING sessionId;
    MgUserInformation* userInfo = MgUserInformation::GetCurrentUserInfo();

    if (userInfo != NULL)
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
// returns the current center point of the map, in the spatial reference system of the map
//
MgPoint* MgMap::GetViewCenter()
{
    return SAFE_ADDREF(m_center);
}


//////////////////////////////////////////////////////////////
// Returns the current scale of the map
//
double MgMap::GetViewScale()
{
    return m_scale;
}


//////////////////////////////////////////////////////////////
// Returns the extent of the data to be generated for the map
// in the spatial reference system of the map
//
MgEnvelope* MgMap::GetDataExtent()
{
    return SAFE_ADDREF(m_dataExtent);
}


//////////////////////////////////////////////////////////////
// Returns the number of dots per inch of the map display
//
INT32 MgMap::GetDisplayDpi()
{
    return m_displayDpi;
}


//////////////////////////////////////////////////////////////
// Returns the current width of the map display in pixels
//
INT32 MgMap::GetDisplayWidth()
{
    return m_displayWidth;
}


//////////////////////////////////////////////////////////////
// Returns the current height of the map display in pixels
//
INT32 MgMap::GetDisplayHeight()
{
    return m_displayHeight;
}


//////////////////////////////////////////////////////////////
// Returns the number of finite display scales in the map.
//
INT32 MgMap::GetFiniteDisplayScaleCount()
{
    return (INT32)m_finiteDisplayScales.size();
}


//////////////////////////////////////////////////////////////
// Returns the finite display scale at the specified index.
//
double MgMap::GetFiniteDisplayScaleAt(INT32 index)
{
    INT32 scaleCount = (INT32)m_finiteDisplayScales.size();
    if (index >= scaleCount || index < 0)
        throw new MgIndexOutOfRangeException(L"GetFiniteDisplayScaleAt", __LINE__, __WFILE__, NULL, L"", NULL);

    return m_finiteDisplayScales.at(index);
}


//////////////////////////////////////////////////////////////
// Destruct a MgMap object
//
MgMap::~MgMap()
{
    SAFE_RELEASE(m_mapExtent);
    SAFE_RELEASE(m_dataExtent);
    SAFE_RELEASE(m_center);
}


//////////////////////////////////////////////////////////////
void MgMap::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////
// Sets the current center point of the map, in the spatial reference system of the map
//
void MgMap::SetViewCenter(MgPoint* center)
{
    SAFE_RELEASE(m_center);
    m_center = SAFE_ADDREF(center);
}


//////////////////////////////////////////////////////////////
// Sets the current scale of the map
//
void MgMap::SetViewScale(double scale)
{
    m_scale = scale;
}


//////////////////////////////////////////////////////////////
// Sets the extent of the data to be generated for the map in the spatial reference system of the map.
// This method is reserved for use by ADV and should not be called directly
//
void MgMap::SetDataExtent(MgEnvelope* extent)
{
    SAFE_RELEASE(m_dataExtent);
    m_dataExtent = SAFE_ADDREF(extent);

    SetLayerRefreshMode(refreshAll);
}


//////////////////////////////////////////////////////////////
// Sets the number of dots per inch of the map display
// This method is for use by the viewers and should not be called directly
//
void MgMap::SetDisplayDpi(INT32 dpi)
{
    m_displayDpi = dpi;
}


//////////////////////////////////////////////////////////////
// Sets the current width of the map display in pixels
// This method is for use by the viewers and should not be called directly
//
void MgMap::SetDisplayWidth(INT32 width)
{
    m_displayWidth = width;
}


//////////////////////////////////////////////////////////////
// Sets the current height of the map display in pixels
// This method is for use by the viewers and should not be called directly
//
void MgMap::SetDisplayHeight(INT32 height)
{
    m_displayHeight = height;
}


//////////////////////////////////////////////////////////////
// Returns the background color for the map
//
STRING MgMap::GetBackgroundColor()
{
    return m_backColor;
}


//////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgMap::Serialize(MgStream* stream)
{
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

    //finite display scales
    INT32 scaleCount = (INT32)m_finiteDisplayScales.size();
    stream->WriteInt32(scaleCount);
    if (scaleCount > 0)
    {
        for (FINITESCALES::const_iterator it = m_finiteDisplayScales.begin(); it != m_finiteDisplayScales.end(); it++)
            stream->WriteDouble(*it);
    }

    //groups
    INT32 groupCount = m_groups->GetCount();
    stream->WriteInt32(groupCount);
    for(int groupIndex = 0; groupIndex < groupCount; groupIndex++)
    {
        Ptr<MgLayerGroup> group = m_groups->GetItem(groupIndex);
        Ptr<MgLayerGroup> parent = group->GetGroup();
        stream->WriteString(parent != NULL? parent->GetName(): L"");
        stream->WriteObject(group);
    }
    //layers
    INT32 layerCount = m_layers->GetCount();
    stream->WriteInt32(layerCount);

    for(int layerIndex = 0; layerIndex < layerCount; layerIndex++)
    {
        Ptr<MgLayer> layer = m_layers->GetItem(layerIndex);
        //if the refresh flag must be globally set or reset, do it for this layer
        //before it's serialized
        if(m_layerRefreshMode != unspecified)
        {
            if(m_layerRefreshMode == refreshNone)
                layer->ForceRefresh(false);
            else
                layer->ForceRefresh(true);
        }

        Ptr<MgLayerGroup> parent = layer->GetGroup();
        stream->WriteString(parent != NULL? parent->GetName(): L"");

        stream->WriteObject(layer);
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
}


//////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgMap::Deserialize(MgStream* stream)
{
    MgStreamReader* streamReader = (MgStreamReader*)stream;

    m_trackChangesDisabled = true;

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

    //finite display scales
    INT32 scaleCount;
    streamReader->GetInt32(scaleCount);
    for (INT32 i=0; i<scaleCount; i++)
    {
        double displayScale;
        streamReader->GetDouble(displayScale);
        m_finiteDisplayScales.push_back(displayScale);
    }

    //groups
    //this maps speeds up the process of attaching groups together and attaching layers to groups
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

    for(int layerIndex = 0; layerIndex < layerCount; layerIndex++)
    {
        streamReader->GetString(parentGroupName);
        Ptr<MgLayer> layer = (MgLayer*)streamReader->GetObject();

        if(parentGroupName.length() > 0)
        {
            //attach this group to its parent
            itKg = knownGroups.find(parentGroupName);
            assert(itKg != knownGroups.end());
            layer->SetGroup(itKg->second);
        }

        m_layers->Add(layer);
    }

    //done with this list
    knownGroups.clear();

    //change lists
    INT32 changeListCount;
    streamReader->GetInt32(changeListCount);

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

    m_trackChangesDisabled = false;
}


//////////////////////////////////////////////////////////////
// Does it allow to set the name
//
bool MgMap::CanSetName()
{
    // via Create
    return true;
}


//////////////////////////////////////////////////////////////
// Get the changelists. Returns an empty collection if no changes were tracked
//
MgNamedCollection* MgMap::GetChangeLists()
{
    return SAFE_ADDREF((MgNamedCollection*)m_changeLists);
}


//////////////////////////////////////////////////////////////
// Clear any changes that have been made to objects in this map
//
void MgMap::ClearChanges()
{
    m_changeLists->Clear();
}


//////////////////////////////////////////////////////////////
// Callback method invoked when a group is removed from the group collection
//
void MgMap::OnGroupRemoved(MgLayerGroup* removedGroup)
{
    Ptr<MgLayerGroup> group;

    STRING groupName = removedGroup->GetName();

    //remove children of this group
    for(INT32 groupIndex = 0; groupIndex < m_groups->GetCount(); )
    {
        group = m_groups->GetItem(groupIndex);
        Ptr<MgLayerGroup> parentGroup = group->GetGroup();
        if(parentGroup && parentGroup->GetName() == groupName)
        {
            m_groups->RemoveAt(groupIndex); //will cause to re enter this function
            continue;
        }

        groupIndex ++;
    }

    //remove any layer that is a child of the specified group
    for(INT32 layerIndex = 0; layerIndex < m_layers->GetCount(); )
    {
        Ptr<MgLayer> layer = m_layers->GetItem(layerIndex);
        Ptr<MgLayerGroup> parentGroup = layer->GetGroup();
        if(parentGroup && parentGroup->GetName() == groupName)
        {
            m_layers->RemoveAt(layerIndex);
            continue;
        }

        layerIndex ++;
    }

    removedGroup->SetContainer(NULL);
    TrackChange(removedGroup->GetObjectId(), false, MgObjectChange::removed, L"");
}


//////////////////////////////////////////////////////////////
// Callback method invoked when a group is added from the group collection
//
void MgMap::OnGroupAdded(MgLayerGroup* group)
{
    group->SetContainer(m_groups);

    TrackChange(group->GetObjectId(), false, MgObjectChange::added, L"");
}


//////////////////////////////////////////////////////////////
// Callback method invoked when a group visibility has changed
//
void MgMap::OnGroupVisibilityChanged(MgLayerGroup* group, CREFSTRING visibility)
{
    TrackChange(group->GetObjectId(), false, MgObjectChange::visibilityChanged, visibility);

    bool refresh = visibility == L"1";
    //needs to turn on or off refresh flag for children layers
    //if group is hidden, children layer refresh flag must be turned off
    //if group is shown, children layer refresh flag must be turned on if all parent groups are visible
    Ptr<MgLayer> layer;
    for(INT32 li = 0; li < m_layers->GetCount(); li++)
    {
        layer = (MgLayer*)m_layers->GetItem(li);
        if(!layer->GetVisible())
            continue;

        Ptr<MgLayerGroup> parentGroup = layer->GetGroup();
        if(parentGroup == NULL)
            continue;

        //determine if the shown/hidden group is a direct or indirect parent of this layer
        bool isparent = false;
        while(parentGroup != NULL)
        {
            if(group == parentGroup)
            {
                isparent = true;
                break;
            }
            parentGroup = parentGroup->GetGroup();
        }
        if(isparent)
        {
            if(!refresh)
                layer->ForceRefresh(false);
            else
                layer->ForceRefresh(true);
        }
    }
}


//////////////////////////////////////////////////////////////
// Callback method invoked when the display in legend state of a group has changed
//
void MgMap::OnGroupDisplayInLegendChanged(MgLayerGroup* group, CREFSTRING displayInLegendState)
{
    TrackChange(group->GetObjectId(), false, MgObjectChange::displayInLegendChanged, displayInLegendState);
}


//////////////////////////////////////////////////////////////
// Callback method invoked when the legend label of a group has changed
//
void MgMap::OnGroupLegendLabelChanged(MgLayerGroup* group, CREFSTRING legendLabel)
{
    TrackChange(group->GetObjectId(), false, MgObjectChange::legendLabelChanged, legendLabel);
}


//////////////////////////////////////////////////////////////
// Callback method invoked when the parent of a group has changed
//
void MgMap::OnGroupParentChanged(MgLayerGroup* group, CREFSTRING parentId)
{
    TrackChange(group->GetObjectId(), false, MgObjectChange::parentChanged, parentId);
}


//////////////////////////////////////////////////////////////
// Callback method invoked when a layer is removed from the layer collection
//
void MgMap::OnLayerRemoved(MgLayer* removedLayer)
{
    Ptr<MgLayer> layer;

    if(removedLayer == NULL)
    {
        // all layers removed - explicitly detach any layers from their container
        for(INT32 layerIndex = 0; layerIndex < m_layers->GetCount(); layerIndex++)
        {
            layer = m_layers->GetItem(layerIndex);
            OnLayerRemoved(layer);
        }
    }
    else
    {
        removedLayer->SetContainer(NULL);
        TrackChange(removedLayer->GetObjectId(), true, MgObjectChange::removed, L"");
    }
}


//////////////////////////////////////////////////////////////
// Callback method invoked when a layer is added to the layer collection
//
void MgMap::OnLayerAdded(MgLayer* layer)
{
    layer->SetContainer(m_layers);

    TrackChange(layer->GetObjectId(), true, MgObjectChange::added, L"");
}


//////////////////////////////////////////////////////////////
// Callback method invoked when a layer visibility has changed
//
void MgMap::OnLayerVisibilityChanged(MgLayer* layer, CREFSTRING visibility)
{
    if(visibility == L"1")
        layer->ForceRefresh();

    TrackChange(layer->GetObjectId(), true, MgObjectChange::visibilityChanged, visibility);
}


//////////////////////////////////////////////////////////////
// Callback method invoked when the display in legend state of a layer has changed
//
void MgMap::OnLayerDisplayInLegendChanged(MgLayer* layer, CREFSTRING displayInLegendState)
{
    TrackChange(layer->GetObjectId(), true, MgObjectChange::displayInLegendChanged, displayInLegendState);
}


//////////////////////////////////////////////////////////////
// Callback method invoked when the legend label of a layer has changed
//
void MgMap::OnLayerLegendLabelChanged(MgLayer* layer, CREFSTRING legendLabel)
{
    TrackChange(layer->GetObjectId(), true, MgObjectChange::legendLabelChanged, legendLabel);
}


//////////////////////////////////////////////////////////////
// Callback method invoked when the parent of a layer has changed
//
void MgMap::OnLayerParentChanged(MgLayer* layer, CREFSTRING parentId)
{
    layer->ForceRefresh();

    TrackChange(layer->GetObjectId(), true, MgObjectChange::parentChanged, parentId);
}


//////////////////////////////////////////////////////////////
// Callback method invoked when the selectability of a layer has changed
//
void MgMap::OnLayerSelectabilityChanged(MgLayer* layer, CREFSTRING selectability)
{
    TrackChange(layer->GetObjectId(), true, MgObjectChange::selectabilityChanged, selectability);
}


//////////////////////////////////////////////////////////////
// Callback method invoked when the selectability of a layer has changed
//
void MgMap::OnLayerDefinitionChanged(MgLayer* layer)
{
    TrackChange(layer->GetObjectId(), true, MgObjectChange::definitionChanged, L"");
}


//////////////////////////////////////////////////////////////
// Track the change for the specified object
//
void MgMap::TrackChange(CREFSTRING objectId, bool layer, MgObjectChange::ChangeType type, CREFSTRING param)
{
    if(m_trackChangesDisabled)
        return;

    MG_TRY()

    //check if there is already a changelist for this object. If not, created it
    Ptr<MgChangeList> changeList = (MgChangeList*)m_changeLists->FindItem(objectId);
    if(changeList == NULL)
    {
        changeList = new MgChangeList(objectId, layer);
        if(changeList == NULL)
            throw new MgOutOfMemoryException(L"MgMap.TrackChange", __LINE__, __WFILE__, NULL, L"", NULL);

        m_changeLists->Add(changeList);
    }

    //create a change record and adds it to the change list
    Ptr<MgObjectChange> change = new MgObjectChange(type);
    if(change == NULL)
        throw new MgOutOfMemoryException(L"MgMap.TrackChange", __LINE__, __WFILE__, NULL, L"", NULL);

    change->SetParam(param);

    changeList->AddChange(change);

    MG_CATCH_AND_THROW(L"MgMap.TrackChange")
}


//////////////////////////////////////////////////////////////
// Set the refresh mode for all layers in this map
//
void MgMap::SetLayerRefreshMode(MgMap::LayerRefreshMode refreshMode)
{
    m_layerRefreshMode = refreshMode;
}


//////////////////////////////////////////////////////////////
// Get the refresh mode for all layers in this map
//
MgMap::LayerRefreshMode MgMap::GetLayerRefreshMode()
{
    return m_layerRefreshMode;
}


//////////////////////////////////////////////////////////////
// Reset refresh flag for all the layers of this map
//
void MgMap::ResetLayersRefresh()
{
    for(INT32 i=0; i < m_layers->GetCount(); i++)
    {
        Ptr<MgLayer> layer = m_layers->GetItem(i);
        layer->ForceRefresh(false);
    }
    SetLayerRefreshMode(unspecified);
}


//////////////////////////////////////////////////////////////
// Gets the meters per unit for the map coordinate system.
// This is calculated when the map is created.
//
double MgMap::GetMetersPerUnit()
{
    return m_metersPerUnit;
}

//////////////////////////////////////////////////////////////
// Searches the list of finite display scales for the scale nearest
// the supplied scale, and returns the index of that scale.
INT32 MgMap::FindNearestFiniteDisplayScaleIndex(double scale)
{
    INT32 ret = -1;

    INT32 scaleCount = (INT32)m_finiteDisplayScales.size();
    if (scaleCount > 0)
    {
        INT32 count = 0;
        double minDiff = -1.0;
        for (FINITESCALES::const_iterator it = m_finiteDisplayScales.begin(); it != m_finiteDisplayScales.end(); it++)
        {
            double displayScale = *it;
            double diff = fabs(scale - displayScale);

            if (minDiff < 0.0 || diff < minDiff)
            {
                minDiff = diff;
                ret = count;
            }

            ++count;
        }
    }

    return ret;
}

//////////////////////////////////////////////////////////////////
const char* MgMap::GetResourceTypeName() 
{
    return "Map";
}
