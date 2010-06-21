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

#if defined(_WIN32) && defined(_DEBUG)
//extern "C" long __stdcall GetTempPathA(long nBufferLength, char* lpBuffer);
#endif

//////////////////////////////////////////////////////////////////
// Construct a MgDwfController object
//
MgDwfController::MgDwfController()
{
    m_operation = unknown;
}

MgDwfController::MgDwfController(MgSiteConnection* siteConn, CREFSTRING mapAgentUri)
: MgController(siteConn)
{
    m_operation = unknown;
    m_mapAgentUri = mapAgentUri;
}

//////////////////////////////////////////////////////////////////
// Processes a GetMap request from the Viewer and returns
// a DWF containing an EMap that fully describes the map
// but does not yet contain any layer graphics
//
MgByteReader* MgDwfController::GetMap(MgResourceIdentifier* mapDefinition,
    CREFSTRING dwfVersion, CREFSTRING eMapVersion, MgPropertyCollection* mapViewCommands)
{
    //create a session id to associate with this map
    STRING sessionId;
    Ptr<MgUserInformation> userInfo = m_siteConn->GetUserInfo();
    if (userInfo.p != NULL) sessionId = userInfo->GetMgSessionId();
    if (sessionId.empty())
    {
        Ptr<MgSite> site = m_siteConn->GetSite();
        sessionId = site->CreateSession();
        userInfo->SetMgSessionId(sessionId);
    }

    //get an instance of the resource service
    Ptr<MgResourceService> resourceService = (MgResourceService*)GetService(MgServiceType::ResourceService);

    //instantiate a map object and let it load itself from the map definition in the resource repository
    Ptr<MgMap> map = new MgMap();
    map->Create(resourceService, mapDefinition, mapDefinition->GetName());

    m_operation = get;

    //apply commands
    ApplyMapViewCommands(map, mapViewCommands);

    //save the map state in the session repository
    Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(L"Session:" + sessionId + L"//" + mapDefinition->GetName() + L"." + MgResourceType::Map);
    map->Save((MgResourceService*)resourceService, (MgResourceIdentifier*)resId);

    Ptr<MgDwfVersion> dwfv = new MgDwfVersion(dwfVersion, eMapVersion);

    //get an instance of the mapping service and call the GetMap API
    Ptr<MgMappingService> mappingService = (MgMappingService*)GetService(MgServiceType::MappingService);
    return mappingService->GenerateMap(map, sessionId, m_mapAgentUri, dwfv);
}

//////////////////////////////////////////////////////////////////
// Processes a GetMapUpdate request from the Viewer and
// returns an EMapTransaction DWF containing updated
// content for the specified map
//
MgByteReader* MgDwfController::GetMapUpdate(CREFSTRING mapName, int seqNo,
    CREFSTRING dwfVersion, CREFSTRING eMapVersion, MgPropertyCollection* mapViewCommands)
{
    //get an instance of the resource service
    Ptr<MgResourceService> resourceService = (MgResourceService*)GetService(MgServiceType::ResourceService);

    //recall the state of this map from the session repository
    Ptr<MgMap> map = new MgMap();
    map->Open(resourceService, mapName);

    m_operation = update;

    //apply new commands
    ApplyMapViewCommands(map, mapViewCommands);

    Ptr<MgDwfVersion> dwfv = new MgDwfVersion(dwfVersion, eMapVersion);

    //get an instance of the mapping service and call the GetMapUpdate API
    Ptr<MgMappingService> mappingService = (MgMappingService*)GetService(MgServiceType::MappingService);
    MgByteReader* br = mappingService->GenerateMapUpdate(map, seqNo, dwfv);

    //save the MgMap state
    map->Save(resourceService);

    return br;
}

MgByteReader* MgDwfController::GetPlot(MgMap* map, MgPlotSpecification* plotSpec, MgLayout* layout, CREFSTRING dwfVersion,
        CREFSTRING ePlotVersion, MgPropertyCollection* mapViewCommands)
{
    //apply new commands
    ApplyMapViewCommands(map, mapViewCommands);

    Ptr<MgDwfVersion> dwfv = new MgDwfVersion(dwfVersion, ePlotVersion);

    //get an instance of the mapping service and call the GetMapUpdate API
    Ptr<MgMappingService> mappingService = (MgMappingService*)GetService(MgServiceType::MappingService);
    MgByteReader* br = NULL;

    // the presence of the data extent determines which GeneratePlot API we use
    Ptr<MgProperty> val;
    if (mapViewCommands != NULL)
        val = mapViewCommands->FindItem(m_mapCmdSetDataExtent);

    if (val != NULL)
    {
        Ptr<MgEnvelope> extent = map->GetDataExtent();
        br = mappingService->GeneratePlot(map, extent, true, plotSpec, layout, dwfv);
    }
    else
    {
        Ptr<MgPoint> center = map->GetViewCenter();
        Ptr<MgCoordinate> coord = center? center->GetCoordinate(): NULL;
        double scale = map->GetViewScale();
        br = mappingService->GeneratePlot(map, coord, scale, plotSpec, layout, dwfv);
    }

    return br;
}

//////////////////////////////////////////////////////////////////
// Apply the specified set of commands to the map.
//
void MgDwfController::ApplyMapViewCommands(MgMap* map, MgPropertyCollection* mapViewCommands)
{
    if(mapViewCommands == NULL)
        return;

    //apply commands common to both type of viewers
    MgController::ApplyMapViewCommands(map, mapViewCommands);

    //apply commands specific to ADV
    Ptr<MgProperty> val;
    if(m_operation != get)
    {
        val = mapViewCommands->FindItem(m_mapCmdSetDataExtent);
        if(val != NULL)
        {
            if(val->GetPropertyType() != MgPropertyType::String)
            {
                STRING buffer;
                MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

                MgStringCollection arguments;
                arguments.Add(m_mapCmdSetDataExtent);
                arguments.Add(buffer);

                throw new MgInvalidArgumentException(L"MgDwfController.ApplyMapViewCommands",
                    __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
            }

            Ptr<MgEnvelope> extent = ParseEnvelope(((MgStringProperty*)((MgProperty*)val))->GetValue());
            if(extent == NULL)
            {
                map->SetLayerRefreshMode(MgMap::refreshNone);
                return;
            }
            map->SetDataExtent(extent);
        }
    }

    if(m_operation == update)
    {
        val = mapViewCommands->FindItem(m_mapCmdRefreshLayers);
        if(val != NULL)
        {
            Ptr<MgStringCollection> layerIds = MgStringCollection::ParseCollection(((MgStringProperty*)((MgProperty*)val))->GetValue(), L",");
            if(layerIds != NULL && layerIds->GetCount() > 0)
            {
                bool forceAll = (layerIds->GetCount() == 1) && (layerIds->GetItem(0) == L"*");
                if (forceAll)
                {
                    map->SetLayerRefreshMode(MgMap::refreshAll);
                }
                else
                {
                    Ptr<MgLayerCollection> layers = map->GetLayers();
                    for(INT32 index = 0; index < layerIds->GetCount(); index++)
                    {
                        STRING id = layerIds->GetItem(index);

                        Ptr<MgLayerBase> layer;
                        for(INT32 li = 0; li < layers->GetCount(); li++)
                        {
                            layer = layers->GetItem(li);
                            if(!layer->GetObjectId().compare(id))
                            {
                                layer->ForceRefresh(true);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
// Destruct a MgDwfController object
//
MgDwfController::~MgDwfController()
{
}
