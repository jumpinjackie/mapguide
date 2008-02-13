//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "TileDefs.h"

static const int Tile_Service = (int)MgPacketParser::msiTile;

IMPLEMENT_CREATE_SERVICE(MgProxyTileService)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgProxyTileService::MgProxyTileService() : MgTileService()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgProxyTileService::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the warnings if any
/// </summary>
void MgProxyTileService::SetWarning(MgWarnings* warning)
{
    if (warning)
    {
        Ptr<MgStringCollection> ptrCol = warning->GetMessages();
        this->m_warning->AddMessages(ptrCol);
        warning->Release();
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the specified base map tile for the given map.  If a cached tile
/// image exists it will return it, otherwise the tile is rendered and added
/// to the cache.
/// </summary>
/// <param name="map">Input
/// Map object containing current state of map.
/// </param>
/// <param name="baseMapLayerGroupName">Input
/// Specifies the name of the baseMapLayerGroup for which to render the tile.
/// <param name="tileColumn">Input
/// Specifies the column index of the tile to return.
/// </param>
/// <param name="tileRow">Input
/// Specifies the row index of the tile to return.
/// </param>
/// <returns>
/// A byte reader containing the rendered tile image.
/// </returns>
MgByteReader* MgProxyTileService::GetTile(
    MgMap* map,
    CREFSTRING baseMapLayerGroupName,
    INT32 tileColumn,
    INT32 tileRow)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knObject,                            // Return type expected
                        MgTileServiceOpId::GetTile,                     // Command Code
                        4,                                              // No of arguments
                        Tile_Service,                                   // Service Id
                        BUILD_VERSION(1,0,0),                           // Operation version
                        MgCommand::knObject, map,                       // Argument#1
                        MgCommand::knString, &baseMapLayerGroupName,    // Argument#2
                        MgCommand::knInt32, tileColumn,                 // Argument#3
                        MgCommand::knInt32, tileRow,                    // Argument#4
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

/////////////////////////////////////////////////////////////////
/// \brief
/// Returns the specified base map tile for the given map.  If a cached tile
/// image exists it will return it, otherwise the tile is rendered and added
/// to the cache.
MgByteReader* MgProxyTileService::GetTile(
    MgResourceIdentifier* mapDefinition,
    CREFSTRING baseMapLayerGroupName,
    INT32 tileColumn,
    INT32 tileRow,
    INT32 scaleIndex)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knObject,                            // Return type expected
                        MgTileServiceOpId::GetTile,                     // Command Code
                        5,                                              // No of arguments
                        Tile_Service,                                   // Service Id
                        BUILD_VERSION(1,2,0),                           // Operation version
                        MgCommand::knObject, mapDefinition,             // Argument#1
                        MgCommand::knString, &baseMapLayerGroupName,    // Argument#2
                        MgCommand::knInt32, tileColumn,                 // Argument#3
                        MgCommand::knInt32, tileRow,                    // Argument#4
                        MgCommand::knInt32, scaleIndex,                 // Argument#4
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Adds the specified base map tile to the cache.  If a tile image already
/// exists at the specified location then it will be repleced.
/// </summary>
/// <param name="img">Input
/// Byte reader containing the tile image.
/// </param>
/// <param name="map">Input
/// Map object containing current state of map.
/// </param>
/// <param name="scaleIndex">Input
/// Specifies the index of the finite scale for the tile.
/// </param>
/// <param name="baseMapLayerGroupName">Input
/// Specifies the name of the baseMapLayerGroup for the tile.
/// </param>
/// <param name="tileColumn">Input
/// Specifies the column index of the tile to set.
/// </param>
/// <param name="tileRow">Input
/// Specifies the row index of the tile to set.
/// </param>
void MgProxyTileService::SetTile(
    MgByteReader* img,
    MgMap* map,
    INT32 scaleIndex,
    CREFSTRING baseMapLayerGroupName,
    INT32 tileColumn,
    INT32 tileRow)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knVoid,                              // Return type expected
                        MgTileServiceOpId::SetTile,                     // Command Code
                        6,                                              // No of arguments
                        Tile_Service,                                   // Service Id
                        BUILD_VERSION(1,0,0),                           // Operation version
                        MgCommand::knObject, img,                       // Argument#1
                        MgCommand::knObject, map,                       // Argument#2
                        MgCommand::knInt32, scaleIndex,                 // Argument#3
                        MgCommand::knString, &baseMapLayerGroupName,    // Argument#4
                        MgCommand::knInt32, tileColumn,                 // Argument#5
                        MgCommand::knInt32, tileRow,                    // Argument#6
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Clears the entire tile cache for the given map.  Tiles for all base
/// map layer groups and finite scales will be removed.
/// </summary>
/// <param name="map">Input
/// Specifies the map whose tile cache will be cleared.
/// </param>
void MgProxyTileService::ClearCache(MgMap* map)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knVoid,                              // Return type expected
                        MgTileServiceOpId::ClearCache,                  // Command Code
                        1,                                              // No of arguments
                        Tile_Service,                                   // Service Id
                        BUILD_VERSION(1,0,0),                           // Operation version
                        MgCommand::knObject, map,                       // Argument#1
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Returns the default width of a tile.
INT32 MgProxyTileService::GetDefaultTileSizeX()
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knInt32,                             // Return type expected
                        MgTileServiceOpId::GetDefaultTileSizeX,         // Command Code
                        0,                                              // No of arguments
                        Tile_Service,                                   // Service Id
                        BUILD_VERSION(1,2,0),                           // Operation version
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());

    return cmd.GetReturnValue().val.m_i32;
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Returns the default height of a tile.
INT32 MgProxyTileService::GetDefaultTileSizeY()
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knInt32,                             // Return type expected
                        MgTileServiceOpId::GetDefaultTileSizeY,         // Command Code
                        0,                                              // No of arguments
                        Tile_Service,                                   // Service Id
                        BUILD_VERSION(1,2,0),                           // Operation version
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());

    return cmd.GetReturnValue().val.m_i32;
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Sets the connection properties for the Proxy Service.  This
/// information tells the proxy object where to connect.
///
/// \param connProp
/// Connection properties for server
///
void MgProxyTileService::SetConnectionProperties(MgConnectionProperties* connProp)
{
    m_connProp = SAFE_ADDREF(connProp);
}
