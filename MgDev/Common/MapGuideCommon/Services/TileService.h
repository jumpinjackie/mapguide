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

#ifndef MG_TILE_SERVICE_H
#define MG_TILE_SERVICE_H

/// \defgroup MgTileService MgTileService
/// \ingroup Tile_Service_Module
/// \{

class MgByteReader;
class MgMap;

/// \brief
/// This class provides services to render a map into an image
class MG_MAPGUIDE_API MgTileService : public MgService
{
PUBLISHED_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified base map tile for the given map.  If a cached tile
    /// image exists it will return it, otherwise the tile is rendered and added
    /// to the cache.
    ///
    /// \param map
    /// Input
    /// Map object containing current state of map.
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for which to render the tile.
    /// \param tileColumn
    /// Input
    /// Specifies the column index of the tile to return.
    /// \param tileRow
    /// Input
    /// Specifies the row index of the tile to return.
    ///
    /// \return
    /// A byte reader containing the rendered tile image.
    ///
    virtual MgByteReader* GetTile(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 tileColumn,
        INT32 tileRow) = 0;

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified base map tile for the given map.  If a cached tile
    /// image exists it will return it, otherwise the tile is rendered and added
    /// to the cache.
    ///
    /// \param mapDefinition
    /// Input
    /// Resource identifier for the map definition
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for which to render the tile.
    /// \param tileColumn
    /// Input
    /// Specifies the column index of the tile to return.
    /// \param tileRow
    /// Input
    /// Specifies the row index of the tile to return.
    /// \param scaleIndex
    /// Input
    /// Scale index to render.  Most detailed scale is index 0.
    ///
    /// \return
    /// A byte reader containing the rendered tile image.
    ///
    virtual MgByteReader* GetTile(
        MgResourceIdentifier* mapDefinition,
        CREFSTRING baseMapLayerGroupName,
        INT32 tileColumn,
        INT32 tileRow,
        INT32 scaleIndex) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Clears the entire tile cache for the given map.  Tiles for all base
    /// map layer groups and finite scales will be removed.
    ///
    /// \param map
    /// Input
    /// Specifies the map whose tile cache will be cleared.
    ///
    virtual void ClearCache(MgMap* map) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the default width of a tile.
    ///
    /// \return
    /// Default width of a tile in pixels.
    ///
    virtual INT32 GetDefaultTileSizeX() = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the default height of a tile.
    ///
    /// \return
    /// Default height of a tile in pixels.
    ///
    virtual INT32 GetDefaultTileSizeY() = 0;

INTERNAL_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified base map tile to the cache.  If a tile image already
    /// exists at the specified location then it will be repleced.
    ///
    /// \param img
    /// Input
    /// Byte reader containing the tile image.
    /// \param map
    /// Input
    /// Map object containing current state of map.
    /// \param scaleIndex
    /// Input
    /// Specifies the index of the finite scale for the tile.
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for the tile.
    /// \param tileColumn
    /// Input
    /// Specifies the column index of the tile to set.
    /// \param tileRow
    /// Input
    /// Specifies the row index of the tile to set.
    ///
    virtual void SetTile(
        MgByteReader* img,
        MgMap* map,
        INT32 scaleIndex,
        CREFSTRING baseMapLayerGroupName,
        INT32 tileColumn,
        INT32 tileRow) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgTileService object.
    ///
    MgTileService();

protected:

    virtual void NotifyResourcesChanged(MgSerializableCollection* resources) {};

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    virtual INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object
    ///
    /// \return
    /// Nothing
    ///
    virtual void Dispose();

private:


CLASS_ID:
    static const INT32 m_cls_id = MapGuide_TileService_TileService;
};
/// \}

#endif
