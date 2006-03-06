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
#include "Command.h"

#include "RenderingDefs.h"

static const int Rendering_Service = (int)MgPacketParser::msiRendering;


//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgProxyRenderingService object
/// </summary>
MgProxyRenderingService::MgProxyRenderingService(MgConnectionProperties* connection) : MgRenderingService(connection)
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgProxyRenderingService::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the warnings if any
/// </summary>
void MgProxyRenderingService::SetWarning(MgWarnings* warning)
{
    if (warning)
    {
        Ptr<MgStringCollection> ptrCol = warning->GetMessages();
        this->m_warning->AddMessages(ptrCol);
        warning->Release();
    }
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the specified base map tile for the given map.  If a cached tile
/// image exists it will return it, otherwise the tile is rendered and added
/// to the cache.
/// </summary>
/// <param name="map">Input
/// map object containing current state of map.
/// </param>
/// <param name="baseMapLayerGroupName">Input
/// Specifies the name of the baseMapLayerGroup for which to render the tile.
/// </param>
/// <param name="tileColumn">Input
/// Specifies the column index of the tile to return.
/// </param>
/// <param name="tileRow">Input
/// Specifies the row index of the tile to return.
/// </param>
/// <returns>
/// A byte reader containing the rendered tile image.
/// </returns>
MgByteReader* MgProxyRenderingService::RenderTile(
    MgMap* map,
    CREFSTRING baseMapLayerGroupName,
    INT32 tileColumn,
    INT32 tileRow)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knObject,                            // Return type expected
                        MgRenderingServiceOpId::RenderTile,             // Command Code
                        4,                                              // No of arguments
                        Rendering_Service,                              // Service Id
                        1,                                              // Operation version
                        MgCommand::knObject, map,                       // Argument#1
                        MgCommand::knString, &baseMapLayerGroupName,    // Argument#2
                        MgCommand::knInt32, tileColumn,                 // Argument#3
                        MgCommand::knInt32, tileRow,                    // Argument#4
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Renders all dynamic layers in the specified MgMap to a dynamic overlay image
/// with a transparent background. The center, scale, size, and layers to be
/// rendered are defined by the specified map instance.  The format parameter
/// must be set to an image format that supports transparency.
/// </summary>
/// <param name="map">Input
/// map object containing current state of map.
/// </param>
/// <param name="selection">Input
/// map feature selection. Specifies the selected features on the map
/// </param>
/// <param name="format">Input
/// image format. Defines the format of the resulting image
/// </param>
/// <returns>
/// A byte reader containing the rendered image
/// </returns>
MgByteReader* MgProxyRenderingService::RenderDynamicOverlay(
    MgMap* map,
    MgSelection* selection,
    CREFSTRING format)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knObject,                            // Return type expected
                        MgRenderingServiceOpId::RenderDynamicOverlay,   // Command Code
                        3,                                              // No of arguments
                        Rendering_Service,                              // Service Id
                        1,                                              // Operation version
                        MgCommand::knObject, map,                       // Argument#1
                        MgCommand::knObject, selection,                 // Argument#2
                        MgCommand::knString, &format,                   // Argument#3
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Renders the specified MgMap to the requested image format.
/// </summary>
/// <param name="map">Input
/// map object containing current state of map.
/// </param>
/// <param name="selection">Input
/// map feature selection. Specifies the selected features on the map
/// </param>
/// <param name="format">Input
/// image format. Defines the format of the resulting image
/// </param>
/// <returns>
/// A byte reader containing the rendered image
/// </returns>
MgByteReader* MgProxyRenderingService::RenderMap(
    MgMap* map,
    MgSelection* selection,
    CREFSTRING format)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                          // Connection
                        MgCommand::knObject,                // Return type expected
                        MgRenderingServiceOpId::RenderMap1, // Command Code
                        3,                                  // No of arguments
                        Rendering_Service,                  // Service Id
                        1,                                  // Operation version
                        MgCommand::knObject, map,           // Argument#1
                        MgCommand::knObject, selection,     // Argument#2
                        MgCommand::knString, &format,       // Argument#3
                        MgCommand::knNone);                 // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Renders the specified MgMap to the requested image format.
/// </summary>
/// <param name="map">Input
/// map object containing current state of map.
/// </param>
/// <param name="selection">Input
/// map feature selection. Specifies the selected features on the map
/// </param>
/// <param name="extents">Input
/// map extents. Specifies the extents for the map
/// </param>
/// <param name="width">Input
/// image width. Specifies the image width in pixels
/// </param>
/// <param name="height">Input
/// image height. Specifies the image height in pixels
/// </param>
/// <param name="backgroundColor">Input
/// background color. Specifies the map background color
/// </param>
/// <param name="format">Input
/// image format. Defines the format of the resulting image
/// </param>
/// <returns>
/// A byte reader containing the rendered image
/// </returns>
MgByteReader* MgProxyRenderingService::RenderMap(
    MgMap* map,
    MgSelection* selection,
    MgEnvelope* extents,
    INT32 width,
    INT32 height,
    MgColor* backgroundColor,
    CREFSTRING format)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgRenderingServiceOpId::RenderMap2,     // Command Code
                        7,                                      // No of arguments
                        Rendering_Service,                      // Service Id
                        1,                                      // Operation version
                        MgCommand::knObject, map,               // Argument#1
                        MgCommand::knObject, selection,         // Argument#2
                        MgCommand::knObject, extents,           // Argument#3
                        MgCommand::knInt32, width,              // Argument#4
                        MgCommand::knInt32, height,             // Argument#5
                        MgCommand::knObject, backgroundColor,   // Argument#6
                        MgCommand::knString, &format,           // Argument#7
                        MgCommand::knNone);                     // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Renders the specified MgMap to the requested image format.
/// </summary>
/// <param name="map">Input
/// map object containing current state of map.
/// </param>
/// <param name="selection">Input
/// map feature selection. Specifies the selected features on the map
/// </param>
/// <param name="center">Input
/// map center point. Specifies the center point for the map
/// </param>
/// <param name="scale">Input
/// map scale. Specifies the scale for the map
/// </param>
/// <param name="width">Input
/// image width. Specifies the image width in pixels
/// </param>
/// <param name="height">Input
/// image height. Specifies the image height in pixels
/// </param>
/// <param name="backgroundColor">Input
/// background color. Specifies the map background color
/// </param>
/// <param name="format">Input
/// image format. Defines the format of the resulting image
/// </param>
/// <returns>
/// A byte reader containing the rendered image
/// </returns>
MgByteReader* MgProxyRenderingService::RenderMap(
    MgMap* map,
    MgSelection* selection,
    MgCoordinate* center,
    double scale,
    INT32 width,
    INT32 height,
    MgColor* backgroundColor,
    CREFSTRING format)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgRenderingServiceOpId::RenderMap3,     // Command Code
                        8,                                      // No of arguments
                        Rendering_Service,                      // Service Id
                        1,                                      // Operation version
                        MgCommand::knObject, map,               // Argument#1
                        MgCommand::knObject, selection,         // Argument#2
                        MgCommand::knObject, center,            // Argument#3
                        MgCommand::knDouble, scale,             // Argument#4
                        MgCommand::knInt32, width,              // Argument#5
                        MgCommand::knInt32, height,             // Argument#6
                        MgCommand::knObject, backgroundColor,   // Argument#7
                        MgCommand::knString, &format,           // Argument#8
                        MgCommand::knNone);                     // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Renders the legend for the specified MgMap to the requested size and format
/// </summary>
/// <param name="map">Input
/// map object containing current state of map.
/// </param>
/// <param name="width">Input
/// width of legend image in pixels
/// </param>
/// <param name="height">Input
/// height of legend image in pixels
/// </param>
/// <param name="backgroundColor">Input
/// background color. Specifies the map legend background color
/// </param>
/// <param name="format">Input
/// image format. Defines the format of the resulting image
/// </param>
/// <returns>
/// A byte reader containing the rendered image
/// </returns>
MgByteReader* MgProxyRenderingService::RenderMapLegend(
    MgMap* map,
    INT32 width,
    INT32 height,
    MgColor* backgroundColor,
    CREFSTRING format)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgRenderingServiceOpId::RenderMapLegend,// Command Code
                        5,                                      // No of arguments
                        Rendering_Service,                      // Service Id
                        1,                                      // Operation version
                        MgCommand::knObject, map,               // Argument#1
                        MgCommand::knInt32, width,              // Argument#2
                        MgCommand::knInt32, height,             // Argument#3
                        MgCommand::knObject, backgroundColor,   // Argument#4
                        MgCommand::knString, &format,           // Argument#5
                        MgCommand::knNone);                     // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// The QueryFeatures operation identifies those features that
/// meet the specified spatial selection criteria. This operation
/// is used to implement server-side selection. In addition to
/// a selection set, this operation returns attribute information
/// in case only one feature is selected
/// </summary>
/// <param name="map">Input
/// map object containing current state of map.
/// </param>
/// <param name="layerName">Input
/// Active layer name for which to query features
/// </param>
/// <param name="geometry">Input
/// geometry object specifying the selection area
/// </param>
/// <param name="selectionVariant">Input
/// selection criterion - 0=Within, 1=Touching, 2=Topmost
/// </param>
/// <param name="maxFeatures">Input
/// the maximum number of features to return
/// </param>
/// <returns>
/// An MgSelection instance identifying the features that meet the
/// selection criteria. Returns null if no features are identified.
/// </returns>
MgFeatureInformation* MgProxyRenderingService::QueryFeatures(
    MgMap* map,
    MgStringCollection* layerNames,
    MgGeometry* geometry,
    INT32 selectionVariant, // Within, Touching, Topmost
    INT32 maxFeatures)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                              // Connection
                        MgCommand::knObject,                    // Return type expected
                        MgRenderingServiceOpId::QueryFeatures,  // Command Code
                        5,                                      // No of arguments
                        Rendering_Service,                      // Service Id
                        1,                                      // Operation version
                        MgCommand::knObject, map,               // Argument#1
                        MgCommand::knObject, layerNames,        // Argument#2
                        MgCommand::knObject, geometry,          // Argument#3
                        MgCommand::knInt32, selectionVariant,   // Argument#4
                        MgCommand::knInt32, maxFeatures,        // Argument#5
                        MgCommand::knNone);                     // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgFeatureInformation*)cmd.GetReturnValue().val.m_obj;
}
