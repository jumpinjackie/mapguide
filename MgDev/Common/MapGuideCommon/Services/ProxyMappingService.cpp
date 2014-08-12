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

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a new runtime map from the specified Map Definition resource id and returns an XML-based description of the runtime
/// map
///
/// <!-- Syntax in .Net, Java, and PHP -->
/// \htmlinclude DotNetSyntaxTop.html
/// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude JavaSyntaxTop.html
/// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude PHPSyntaxTop.html
/// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
///
/// \param mapDefinition (MgResourceIdentifier)
/// MgResourceIdentifier object identifying the map definition resource.
/// \param sessionId (String/string)
/// The session ID
/// \param requestedFeatures (int)
/// A bitmask representing the desired information to return in the XML response: 
/// 1=Layer/Group structure, 2=Layer Icons, 4=Layer Feature Source Information
/// \param iconsPerScaleRange (int)
/// The number of legend icons per scale range to render inline in the XML response as base64 strings. 
/// If a scale range contains a number of rules that exceeds this value, only the first and
/// last rules of a type style in the scale range will have inline icons
///
/// \remarks
/// The bitmask values of 2 (Layer Icons) and 4 (Layer Feature Source Information) have no effect if 1 (Layer/Group structure)
/// is not specified
///
/// \return
/// Returns an XML-based description of the runtime map
///
MgByteReader* MgProxyMappingService::CreateRuntimeMap(MgResourceIdentifier* mapDefinition,
                                                      CREFSTRING sessionId,
                                                      INT32 requestedFeatures,
                                                      INT32 iconsPerScaleRange)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                       MgCommand::knObject,
                       MgMappingServiceOpId::CreateRuntimeMap,
                       4,
                       Mapping_Service,
                       BUILD_VERSION(2,6,0),
                       MgCommand::knObject,    mapDefinition,
                       MgCommand::knString,    &sessionId,
                       MgCommand::knInt32,     requestedFeatures,
                       MgCommand::knInt32,     iconsPerScaleRange,
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a new runtime map from the specified Map Definition resource id and returns an XML-based description of the runtime
/// map
///
/// <!-- Syntax in .Net, Java, and PHP -->
/// \htmlinclude DotNetSyntaxTop.html
/// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude JavaSyntaxTop.html
/// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude PHPSyntaxTop.html
/// virtual MgByteReader CreateRuntimeMap(MgResourceIdentifier mapDefinition, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
///
/// \param mapDefinition (MgResourceIdentifier)
/// MgResourceIdentifier object identifying the map definition resource.
/// \param targetMapName (String/string)
/// The desired name of the runtime map
/// \param sessionId (String/string)
/// The session ID
/// \param iconWidth (int)
/// The width of each individual inline legend icons. Has no effect if icons was not requested in the response.
/// \param iconHeight (int)
/// The height of each individual inline legend icons. Has no effect if icons was not requested in the response.
/// \param requestedFeatures (int)
/// A bitmask representing the desired information to return in the XML response: 
/// 1=Layer/Group structure, 2=Layer Icons, 4=Layer Feature Source Information
/// \param iconsPerScaleRange (int)
/// The number of legend icons per scale range to render inline in the XML response as base64 strings. 
/// If a scale range contains a number of rules that exceeds this value, only the first and
/// last rules of a type style in the scale range will have inline icons
///
/// \remarks
/// The bitmask values of 2 (Layer Icons) and 4 (Layer Feature Source Information) have no effect if 1 (Layer/Group structure)
/// is not specified
///
/// \return
/// Returns an XML-based description of the runtime map
///
MgByteReader* MgProxyMappingService::CreateRuntimeMap(MgResourceIdentifier* mapDefinition,
                                                      CREFSTRING targetMapName,
                                                      CREFSTRING sessionId,
                                                      CREFSTRING iconFormat,
                                                      INT32 iconWidth,
                                                      INT32 iconHeight,
                                                      INT32 requestedFeatures,
                                                      INT32 iconsPerScaleRange)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                       MgCommand::knObject,
                       MgMappingServiceOpId::CreateRuntimeMap2,
                       8,
                       Mapping_Service,
                       BUILD_VERSION(2,6,0),
                       MgCommand::knObject,    mapDefinition,
                       MgCommand::knString,    &targetMapName,
                       MgCommand::knString,    &sessionId,
                       MgCommand::knString,    &iconFormat,
                       MgCommand::knInt32,     iconWidth,
                       MgCommand::knInt32,     iconHeight,
                       MgCommand::knInt32,     requestedFeatures,
                       MgCommand::knInt32,     iconsPerScaleRange,
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

MgByteReader* MgProxyMappingService::CreateRuntimeMap(MgResourceIdentifier* mapDefinition,
                                                      CREFSTRING targetMapName,
                                                      CREFSTRING sessionId,
                                                      CREFSTRING iconFormat,
                                                      INT32 iconWidth,
                                                      INT32 iconHeight,
                                                      INT32 requestedFeatures,
                                                      INT32 iconsPerScaleRange,
                                                      INT32 schemaVersion)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                       MgCommand::knObject,
                       MgMappingServiceOpId::CreateRuntimeMap3,
                       9,
                       Mapping_Service,
                       BUILD_VERSION(3,0,0),
                       MgCommand::knObject,    mapDefinition,
                       MgCommand::knString,    &targetMapName,
                       MgCommand::knString,    &sessionId,
                       MgCommand::knString,    &iconFormat,
                       MgCommand::knInt32,     iconWidth,
                       MgCommand::knInt32,     iconHeight,
                       MgCommand::knInt32,     requestedFeatures,
                       MgCommand::knInt32,     iconsPerScaleRange,
                       MgCommand::knInt32,     schemaVersion,
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns an XML-based description of the runtime map
///
/// <!-- Syntax in .Net, Java, and PHP -->
/// \htmlinclude DotNetSyntaxTop.html
/// virtual MgByteReader DescribeRuntimeMap(MgMap map, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude JavaSyntaxTop.html
/// virtual MgByteReader DescribeRuntimeMap(MgMap map, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude PHPSyntaxTop.html
/// virtual MgByteReader DescribeRuntimeMap(MgMap map, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
///
/// \param map (MgMap)
/// The map to describe
/// \param requestedFeatures (int)
/// A bitmask representing the desired information to return in the XML response: 
/// 1=Layer/Group structure, 2=Layer Icons, 4=Layer Feature Source Information
/// \param iconsPerScaleRange (int)
/// The number of legend icons per scale range to render inline in the XML response as base64 strings. 
/// If a scale range contains a number of rules that exceeds this value, only the first and
/// last rules of a type style in the scale range will have inline icons
///
/// \remarks
/// The bitmask values of 2 (Layer Icons) and 4 (Layer Feature Source Information) have no effect if 1 (Layer/Group structure)
/// is not specified
///
/// \return
/// Returns an XML-based description of the runtime map
///
MgByteReader* MgProxyMappingService::DescribeRuntimeMap(MgMap* map,
                                                        INT32 requestedFeatures,
                                                        INT32 iconsPerScaleRange)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                       MgCommand::knObject,
                       MgMappingServiceOpId::DescribeRuntimeMap,
                       3,
                       Mapping_Service,
                       BUILD_VERSION(2,6,0),
                       MgCommand::knObject,    map,
                       MgCommand::knInt32,     requestedFeatures,
                       MgCommand::knInt32,     iconsPerScaleRange,
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns an XML-based description of the given runtime map
///
/// <!-- Syntax in .Net, Java, and PHP -->
/// \htmlinclude DotNetSyntaxTop.html
/// virtual MgByteReader DescribeRuntimeMap(MgMap map, string iconFormat, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude JavaSyntaxTop.html
/// virtual MgByteReader DescribeRuntimeMap(MgMap map, String iconFormat, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude PHPSyntaxTop.html
/// virtual MgByteReader DescribeRuntimeMap(MgMap map, string iconFormat, int iconWidth, int iconHeight, int requestedFeatures, int iconsPerScaleRange);
/// \htmlinclude SyntaxBottom.html
///
/// \param map (MgMap)
/// The map to describe
/// \param iconFormat (String/string)
/// The desired image format for icons (from MgImageFormats)
/// \param iconWidth (int)
/// The width of each individual inline legend icons. Has no effect if icons was not requested in the response.
/// \param iconHeight (int)
/// The height of each individual inline legend icons. Has no effect if icons was not requested in the response.
/// \param requestedFeatures (int)
/// A bitmask representing the desired information to return in the XML response: 
/// 1=Layer/Group structure, 2=Layer Icons, 4=Layer Feature Source Information
/// \param iconsPerScaleRange (int)
/// The number of legend icons per scale range to render inline in the XML response as base64 strings. 
/// If a scale range contains a number of rules that exceeds this value, only the first and
/// last rules of a type style in the scale range will have inline icons
///
/// \remarks
/// The bitmask values of 2 (Layer Icons) and 4 (Layer Feature Source Information) have no effect if 1 (Layer/Group structure)
/// is not specified
///
/// \return
/// Returns an XML-based description of the runtime map
///
/// \exception MgInvalidArgumentException
/// \exception MgNullArgumentException
///
MgByteReader* MgProxyMappingService::DescribeRuntimeMap(MgMap* map,
                                                        CREFSTRING iconFormat,
                                                        INT32 iconWidth,
                                                        INT32 iconHeight,
                                                        INT32 requestedFeatures,
                                                        INT32 iconsPerScaleRange)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                       MgCommand::knObject,
                       MgMappingServiceOpId::DescribeRuntimeMap2,
                       6,
                       Mapping_Service,
                       BUILD_VERSION(2,6,0),
                       MgCommand::knObject,    map,
                       MgCommand::knString,    &iconFormat,
                       MgCommand::knInt32,     iconWidth,
                       MgCommand::knInt32,     iconHeight,
                       MgCommand::knInt32,     requestedFeatures,
                       MgCommand::knInt32,     iconsPerScaleRange,
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

MgByteReader* MgProxyMappingService::DescribeRuntimeMap(MgMap* map,
                                                        CREFSTRING iconFormat,
                                                        INT32 iconWidth,
                                                        INT32 iconHeight,
                                                        INT32 requestedFeatures,
                                                        INT32 iconsPerScaleRange,
                                                        INT32 schemaVersion)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                       MgCommand::knObject,
                       MgMappingServiceOpId::DescribeRuntimeMap3,
                       7,
                       Mapping_Service,
                       BUILD_VERSION(3,0,0),
                       MgCommand::knObject,    map,
                       MgCommand::knString,    &iconFormat,
                       MgCommand::knInt32,     iconWidth,
                       MgCommand::knInt32,     iconHeight,
                       MgCommand::knInt32,     requestedFeatures,
                       MgCommand::knInt32,     iconsPerScaleRange,
                       MgCommand::knInt32,     schemaVersion,
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
