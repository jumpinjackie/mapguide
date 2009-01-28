//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "Command.h"

static const int Mapping_Service = (int)MgPacketParser::msiMapping;

IMPLEMENT_CREATE_SERVICE(MgProxyMappingService)

// Construct an MgProxyMappingService object.
//
MgProxyMappingService::MgProxyMappingService() :  MgMappingService()
{
}

// Dispose this object
//
void MgProxyMappingService::Dispose()
{
    delete this;
}

// Sets the warnings if any
//
void MgProxyMappingService::SetWarning(MgWarnings* warning)
{
    if (warning)
    {
        Ptr<MgStringCollection> ptrCol = warning->GetMessages();
        this->m_warning->AddMessages(ptrCol);
        warning->Release();
    }
}

//////////////////////////////////////////////////////////////////
///TODO: Get rid of this stub once user info is passed to the server
MgByteReader* MgProxyMappingService::GenerateMap(MgMap* map, CREFSTRING mapAgentUri, MgDwfVersion* dwfVersion)
{
    STRING sessionId;

    if (m_connProp.p != NULL)
    {
        Ptr<MgUserInformation> userInfo = m_connProp->GetUserInfo();
        if (userInfo.p != NULL)
        {
            sessionId = userInfo->GetMgSessionId();
        }
    }

    if (sessionId.empty())
    {
        return NULL;
    }
    else
    {
        return GenerateMap(map, sessionId, mapAgentUri, dwfVersion);
    }
}


// Generates an EMap DWF that describes the specified map
//
MgByteReader* MgProxyMappingService::GenerateMap(MgMap* map, CREFSTRING sessionId, CREFSTRING mapAgentUri, MgDwfVersion* dwfVersion)
{
    MgCommand cmd;

    //Do not carry around track changes for GenerateMap (they are used only for GenerateMapUpdate)
    if(NULL != map)
        map->ClearChanges();

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgMappingServiceOpId::GenerateMap,
                        4,
                        Mapping_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject,    map,
                        MgCommand::knString,    &sessionId,
                        MgCommand::knString,    &mapAgentUri,
                        MgCommand::knObject,    dwfVersion,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

// Generates an EMap DWF that contains any required updates to the EMap meta-data
// and layer graphics based on the state of the specified map
//
MgByteReader* MgProxyMappingService::GenerateMapUpdate(MgMap* map, INT32 seqNo, MgDwfVersion* dwfVersion)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgMappingServiceOpId::GenerateMapUpdate,
                        3,
                        Mapping_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject,    map,
                        MgCommand::knInt32,     seqNo,
                        MgCommand::knObject,    dwfVersion,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    //Ensure we are clearing track changes after GenerateMapUpdate, so that we
    //won't attempt to reuse them again
    if(NULL != map)
    {
        map->ClearChanges();
        map->ResetLayersRefresh(); //refresh flags are transient. prevent to reuse them multiple times by GenerateMapUpdate
    }

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


// Plot the map to a EPlot DWF using the center and scale from the map.  The
// extents will be computed to fill the space wihing the page margins.  If the
// layout parameter is null, no adonments will be added to the page.
MgByteReader* MgProxyMappingService::GeneratePlot(
    MgMap* map,
    MgPlotSpecification* plotSpec,
    MgLayout* layout,
    MgDwfVersion* dwfVersion)
{
    MgCommand cmd;

    if (NULL != map)
        map->ClearChanges();

    cmd.ExecuteCommand(m_connProp,
        MgCommand::knObject,
        MgMappingServiceOpId::GeneratePlot,
        4,
        Mapping_Service,
        BUILD_VERSION(1,0,0),
        MgCommand::knObject,    map,
        MgCommand::knObject,    plotSpec,
        MgCommand::knObject,    layout,
        MgCommand::knObject,    dwfVersion,
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

// Plot the map to an EPlot DWF using the specified center and scale.  The extents
// will be computed to fill the space within the page margins.  If the layout
// parameter is null, no adonments will be added to the page.
MgByteReader* MgProxyMappingService::GeneratePlot(
    MgMap* map,
    MgCoordinate* center,
    double scale,
    MgPlotSpecification* plotSpec,
    MgLayout* layout,
    MgDwfVersion* dwfVersion)
{
    MgCommand cmd;

    if (NULL != map)
        map->ClearChanges();

    cmd.ExecuteCommand(m_connProp,
        MgCommand::knObject,
        MgMappingServiceOpId::GeneratePlot2,
        7,
        Mapping_Service,
        BUILD_VERSION(1,0,0),
        MgCommand::knObject,    map,
        MgCommand::knDouble,    center->GetX(),
        MgCommand::knDouble,    center->GetY(),
        MgCommand::knDouble,    scale,
        MgCommand::knObject,    plotSpec,
        MgCommand::knObject,    layout,
        MgCommand::knObject,    dwfVersion,
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

// Plot the map to an EPlot DWF using the extent specified. in the mapping
// coordinate space.  The center and scale will be computed based on the extent
// specified.  If expandToFit is true, then the extent specified will be expanded
// in the X or Y direction to fill the space within the page margins.  If the
// layout parameter is null, no adornments will be added to the page.
MgByteReader* MgProxyMappingService::GeneratePlot(
    MgMap* map,
    MgEnvelope* extents,
    bool expandToFit,
    MgPlotSpecification* plotSpec,
    MgLayout* layout,
    MgDwfVersion* dwfVersion)
{
    MgCommand cmd;

    if (NULL != map)
        map->ClearChanges();

    cmd.ExecuteCommand(m_connProp,
        MgCommand::knObject,
        MgMappingServiceOpId::GeneratePlot3,
        6,
        Mapping_Service,
        BUILD_VERSION(1,0,0),
        MgCommand::knObject,    map,
        MgCommand::knObject,    extents,
        MgCommand::knInt8,      (INT8)expandToFit,
        MgCommand::knObject,    plotSpec,
        MgCommand::knObject,    layout,
        MgCommand::knObject,    dwfVersion,
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

// Plot multiple maps into an EPlot DWF as separate sheets.  The parameters allow
// the caller to control the map, the view, the size, orientation, and margins of
// each page, and the layout of each page.
MgByteReader* MgProxyMappingService::GenerateMultiPlot(
    MgMapPlotCollection* mapPlots,
    MgDwfVersion* dwfVersion)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
        MgCommand::knObject,
        MgMappingServiceOpId::GenerateMultiPlot,
        2,
        Mapping_Service,
        BUILD_VERSION(1,0,0),
        MgCommand::knObject,    mapPlots,
        MgCommand::knObject,    dwfVersion,
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

// Generates an EPlot DWF containing a legend for the specified
// map based upon a specified scale.
//
MgByteReader* MgProxyMappingService::GenerateLegendPlot(
    MgMap* map,
    double scale,
    MgPlotSpecification* plotSpec,
    MgDwfVersion* dwfVersion)
{
    MgCommand cmd;

    if (NULL != map)
        map->ClearChanges();

    cmd.ExecuteCommand(m_connProp,
        MgCommand::knObject,
        MgMappingServiceOpId::GenerateLegendPlot,
        4,
        Mapping_Service,
        BUILD_VERSION(1,0,0),
        MgCommand::knObject,    map,
        MgCommand::knDouble,    scale,
        MgCommand::knObject,    plotSpec,
        MgCommand::knObject,    dwfVersion,
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

// Queries and returns each of the visible features on the specified layer of the
// specified map.  For feature layers the properties to be included are defined
// in the Feature Layer definition. For DWF layers the properties of each of the objects
// in the DWF source for the layer should be included as the feature properties. The
// geometry of each feature can optionally be returned in either the coordinate space
// of the map or the coordinate space of the display. This operation returns the feature
// properties in XML format, which can be used to construct an HTML Image Map for a previously
// rendered view of the map.
//
MgByteReader* MgProxyMappingService::QueryFeatures(MgMap* map, CREFSTRING layerName, CREFSTRING coordinateSpace)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgMappingServiceOpId::QueryFeatures,
                        3,
                        Mapping_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject,    map,
                        MgCommand::knString,    &layerName,
                        MgCommand::knString,    &coordinateSpace,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

// Queries feature information for each feature visible at the specified point of interest, on a given set of layers.
// This is a multi-layer version of QueryFeatures.
//
MgByteReader* MgProxyMappingService::QueryFeatures(MgMap* map, MgStringCollection* layerNames, INT32 x, INT32 y, INT32 maxFeatures, CREFSTRING coordinateSpace)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgMappingServiceOpId::QueryFeatures2,
                        6,
                        Mapping_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject,    map,
                        MgCommand::knObject,    layerNames,
                        MgCommand::knInt32,     x,
                        MgCommand::knInt32,     y,
                        MgCommand::knInt32,     maxFeatures,
                        MgCommand::knString,    &coordinateSpace,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

// Returns the legend image for the specified layer.
MgByteReader* MgProxyMappingService::GenerateLegendImage(MgResourceIdentifier* resource,
                                                         double scale,
                                                         INT32 width,
                                                         INT32 height,
                                                         CREFSTRING format,
                                                         INT32 geomType,
                                                         INT32 themeCategory)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgMappingServiceOpId::GenerateLegendImage,
                        7,
                        Mapping_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject,    resource,
                        MgCommand::knDouble,    scale,
                        MgCommand::knInt32,     width,
                        MgCommand::knInt32,     height,
                        MgCommand::knString,    &format,
                        MgCommand::knInt32,     geomType,
                        MgCommand::knInt32,     themeCategory,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Sets the connection properties for the Proxy Service.  This
/// information tells the proxy object where to connect.
///
/// \param connProp
/// Connection properties for server
///
void MgProxyMappingService::SetConnectionProperties(MgConnectionProperties* connProp)
{
    m_connProp = SAFE_ADDREF(connProp);
}
