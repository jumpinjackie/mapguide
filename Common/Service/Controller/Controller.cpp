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

// Static member data
//
const STRING MgController::m_mapCmdSetViewCenterX     = L"SETVIEWCENTERX";
const STRING MgController::m_mapCmdSetViewCenterY     = L"SETVIEWCENTERY";
const STRING MgController::m_mapCmdSetViewScale       = L"SETVIEWSCALE";
const STRING MgController::m_mapCmdSetDataExtent      = L"SETDATAEXTENT";
const STRING MgController::m_mapCmdSetDisplayDpi      = L"SETDISPLAYDPI";
const STRING MgController::m_mapCmdSetDisplayWidth    = L"SETDISPLAYWIDTH";
const STRING MgController::m_mapCmdSetDisplayHeight   = L"SETDISPLAYHEIGHT";
const STRING MgController::m_mapCmdShowLayers         = L"SHOWLAYERS";
const STRING MgController::m_mapCmdHideLayers         = L"HIDELAYERS";
const STRING MgController::m_mapCmdShowGroups         = L"SHOWGROUPS";
const STRING MgController::m_mapCmdHideGroups         = L"HIDEGROUPS";
const STRING MgController::m_mapCmdRefreshLayers      = L"REFRESHLAYERS";

//////////////////////////////////////////////////////////////////
// Destruct a MgController object
//
MgController::~MgController()
{
}

//////////////////////////////////////////////////////////////////
// Get an instance of the specified service
//
MgService* MgController::GetService(INT32 serviceType)
{
    MgSiteConnection* mgSiteConnection = MgSiteConnection::GetCurrentConnection();

    return (MgService*)mgSiteConnection->CreateService(serviceType);
}

//////////////////////////////////////////////////////////////////
// Apply the specified set of commands to the map.
//
void MgController::ApplyMapViewCommands(MgMap* map, MgPropertyCollection* mapViewCommands)
{
    if(mapViewCommands == NULL)
        return;

    Ptr<MgProperty> val;

    //Set view center X
    val = mapViewCommands->FindItem(m_mapCmdSetViewCenterX);
    if(val != NULL)
    {
        if(val->GetPropertyType() != MgPropertyType::String)
        {
            STRING buffer;
            MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
        }

        Ptr<MgPoint> center = map->GetViewCenter();
        Ptr<MgCoordinate> coord = center? center->GetCoordinate(): NULL;
        double y = coord? coord->GetY(): 0.;
        MgGeometryFactory gf;
        Ptr<MgCoordinate> coordNewCenter = gf.CreateCoordinateXY(MgUtil::StringToDouble(((MgStringProperty*)((MgProperty*)val))->GetValue()), y);
        Ptr<MgPoint> ptNewCenter = gf.CreatePoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
    }

    //Set view center Y
    val = mapViewCommands->FindItem(m_mapCmdSetViewCenterY);
    if(val != NULL)
    {
        if(val->GetPropertyType() != MgPropertyType::String)
        {
            STRING buffer;
            MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
        }

        Ptr<MgPoint> center = map->GetViewCenter();
        Ptr<MgCoordinate> coord = center? center->GetCoordinate(): NULL;
        double x = coord? coord->GetX(): 0.;
        MgGeometryFactory gf;
        Ptr<MgCoordinate> coordNewCenter = gf.CreateCoordinateXY(x, MgUtil::StringToDouble(((MgStringProperty*)((MgProperty*)val))->GetValue()));
        Ptr<MgPoint> ptNewCenter = gf.CreatePoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
    }

    //Set view scale
    val = mapViewCommands->FindItem(m_mapCmdSetViewScale);
    if(val != NULL)
    {
        if(val->GetPropertyType() != MgPropertyType::String)
        {
            STRING buffer;
            MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
        }

        map->SetViewScale(MgUtil::StringToDouble(((MgStringProperty*)((MgProperty*)val))->GetValue()));

        //refresh all layers that are visible
        Ptr<MgLayerCollection> layers = map->GetLayers();
        for(int layerIndex = 0; layerIndex < layers->GetCount(); layerIndex++)
        {
            Ptr<MgLayer> layer = layers->GetItem(layerIndex);
            layer->ForceRefresh(true);
        }

    }

    //Set display dpi
    val = mapViewCommands->FindItem(m_mapCmdSetDisplayDpi);
    if(val != NULL)
    {
        if(val->GetPropertyType() != MgPropertyType::String)
        {
            STRING buffer;
            MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
        }

        map->SetDisplayDpi(MgUtil::StringToInt32(((MgStringProperty*)((MgProperty*)val))->GetValue()));
    }

    //Set display width
    val = mapViewCommands->FindItem(m_mapCmdSetDisplayWidth);
    if(val != NULL)
    {
        if(val->GetPropertyType() != MgPropertyType::String)
        {
            STRING buffer;
            MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
        }

        map->SetDisplayWidth(MgUtil::StringToInt32(((MgStringProperty*)((MgProperty*)val))->GetValue()));
    }

    //Set display height
    val = mapViewCommands->FindItem(m_mapCmdSetDisplayHeight);
    if(val != NULL)
    {
        if(val->GetPropertyType() != MgPropertyType::String)
        {
            STRING buffer;
            MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
        }

        map->SetDisplayHeight(MgUtil::StringToInt32(((MgStringProperty*)((MgProperty*)val))->GetValue()));
    }

    //Show layers
    val = mapViewCommands->FindItem(m_mapCmdShowLayers);
    if(val != NULL)
    {
        if(val->GetPropertyType() != MgPropertyType::String)
        {
            STRING buffer;
            MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
        }

        ShowLayers(map, ((MgStringProperty*)((MgProperty*)val))->GetValue(), true);
    }

    //Hide layers
    val = mapViewCommands->FindItem(m_mapCmdHideLayers);
    if(val != NULL)
    {
        if(val->GetPropertyType() != MgPropertyType::String)
        {
            STRING buffer;
            MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
        }

        ShowLayers(map, ((MgStringProperty*)((MgProperty*)val))->GetValue(), false);
    }

    //Show groups
    val = mapViewCommands->FindItem(m_mapCmdShowGroups);
    if(val != NULL)
    {
        if(val->GetPropertyType() != MgPropertyType::String)
        {
            STRING buffer;
            MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
        }

        ShowGroups(map, ((MgStringProperty*)((MgProperty*)val))->GetValue(), true);
    }

    //Hide groups
    val = mapViewCommands->FindItem(m_mapCmdHideGroups);
    if(val != NULL)
    {
        if(val->GetPropertyType() != MgPropertyType::String)
        {
            STRING buffer;
            MgUtil::Int32ToString((INT32)val->GetPropertyType(), buffer);

            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgController.ApplyMapViewCommands",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyTypeForCommand", NULL);
        }

        ShowGroups(map, ((MgStringProperty*)((MgProperty*)val))->GetValue(), false);
    }

    //Refresh layers - applies only to DwfController
}

//////////////////////////////////////////////////////////////////
// Show or Hide a set of layers in the specified map.
//
void MgController::ShowLayers(MgMap* map, CREFSTRING strLayers, bool show)
{
    Ptr<MgStringCollection> layerIds = MgStringCollection::ParseCollection(strLayers, L",");
    if(layerIds != NULL && layerIds->GetCount() > 0)
    {
        Ptr<MgLayerCollection> layers = map->GetLayers();
        for(INT32 index = 0; index < layerIds->GetCount(); index++)
        {
            Ptr<MgLayer> layer;
            STRING id = layerIds->GetItem(index);

            for(INT32 li = 0; li < layers->GetCount(); li++)
            {
                layer = (MgLayer*)layers->GetItem(li);
                if(!layer->GetObjectId().compare(id))
                {
                    if (layer->GetLayerType() != MgLayerType::BaseMap)
                        layer->SetVisible(show);

                    break;
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
// Show or Hide a set of groups in the specified map.
//
void MgController::ShowGroups(MgMap* map, CREFSTRING strGroups, bool show)
{
    Ptr<MgStringCollection> groupIds = MgStringCollection::ParseCollection(strGroups, L",");
    if(groupIds != NULL && groupIds->GetCount() > 0)
    {
        Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
        for(INT32 index = 0; index < groupIds->GetCount(); index++)
        {
            Ptr<MgLayerGroup> group;
            STRING id = groupIds->GetItem(index);

            for(INT32 gi = 0; gi < groups->GetCount(); gi++)
            {
                group = (MgLayerGroup*)groups->GetItem(gi);
                if(!group->GetObjectId().compare(id))
                {
                    group->SetVisible(show);
                    break;
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
// Create an MgEnvelope instance from a string representation.
// An invalid envelope (with MaxX < MaxY) can be purposedly sent by the client
// In this instance, do not return an envelope to notify the case to the caller.
MgEnvelope* MgController::ParseEnvelope(CREFSTRING strEnvelope)
{
    double cp[4];
    string str;
    MgUtil::WideCharToMultiByte(strEnvelope, str);
    char* pstr = (char*)str.c_str();

    int cpCount = 0;
    char* tok = strtok(pstr, ",");

    while(tok)
    {
        cp[cpCount++] = atof(tok);
        tok = strtok(NULL, ",");
    }

    if(cpCount != 4)
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(strEnvelope);

        throw new MgInvalidArgumentException(L"MgController.ParseEnvelope",
            __LINE__, __WFILE__, &arguments, L"MgInvalidEnvelope", NULL);
    }

    if(cp[0] > cp[2])
        return NULL;

    MgGeometryFactory gf;
    Ptr<MgCoordinate> ll = gf.CreateCoordinateXY(cp[0], cp[1]);
    Ptr<MgCoordinate> ur = gf.CreateCoordinateXY(cp[2], cp[3]);

    return new MgEnvelope(ll, ur);
}
