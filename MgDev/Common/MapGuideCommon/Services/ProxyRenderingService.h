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

#ifndef _MG_PROXY_RENDERING_SERVICE_H
#define _MG_PROXY_RENDERING_SERVICE_H

/// \cond INTERNAL
////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// This class allows rendering of map images
class MG_MAPGUIDE_API MgProxyRenderingService : public MgRenderingService
{
    DECLARE_CLASSNAME(MgProxyRenderingService)

EXTERNAL_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified base map tile for the given map.  If a cached tile
    /// image exists it will return it, otherwise the tile is rendered and added
    /// to the cache.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
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
    virtual MgByteReader* RenderTile(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 tileColumn,
        INT32 tileRow);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified base map tile for the given map.
    ///
    /// \remarks
    /// This method only renders the given tile. No tile caching is performed
    /// by this method. To render and cache the tile, use the 
    /// \link MgTileService::GetTile GetTile \endlink method instead. However,
    /// using that method will use default tile width/height/dpi/format specified
    /// in your MapGuide Server configuration
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for which to render the tile.
    /// \param tileColumn
    /// Input
    /// Specifies the column index of the tile to return.
    /// \param tileRow
    /// Input
    /// Specifies the row index of the tile to return.
    /// \param tileWidth
    /// Input
    /// Specifies the width of the tile to return.
    /// \param tileHeight
    /// Input
    /// Specifies the height of the tile to return.
    /// \param tileDpi
    /// Input
    /// Specifies the dpi the tile to return.
    /// \param tileImageFormat
    /// Input
    /// Specifies the image format of the tile. See \link MgImageFormats \endlink
    ///
    /// \return
    /// A byte reader containing the rendered tile image.
    ///
    virtual MgByteReader* RenderTile(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 tileColumn,
        INT32 tileRow,
        INT32 tileWidth,
        INT32 tileHeight,
        INT32 tileDpi,
        CREFSTRING tileImageFormat);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified base map tile for the given map.
    ///
    /// \remarks
    /// This method only renders the given tile. No tile caching is performed
    /// by this method. To render and cache the tile, use the 
    /// \link MgTileService::GetTile GetTile \endlink method instead. However,
    /// using that method will use default tile width/height/dpi/format specified
    /// in your MapGuide Server configuration
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for which to render the tile.
    /// \param tileColumn
    /// Input
    /// Specifies the column index of the tile to return.
    /// \param tileRow
    /// Input
    /// Specifies the row index of the tile to return.
    /// \param tileWidth
    /// Input
    /// Specifies the width of the tile to return.
    /// \param tileHeight
    /// Input
    /// Specifies the height of the tile to return.
    /// \param tileDpi
    /// Input
    /// Specifies the dpi the tile to return.
    /// \param tileImageFormat
    /// Input
    /// Specifies the image format of the tile. See \link MgImageFormats \endlink
    /// \param tileExtentOffset
    /// Specifies the ratio by which the tile to be rendered should be "buffered out". The tile will be rendered at the specified width
    /// multiplied by the given ration, which will be cropped back to the original requested size after rendering. This is to improve
    /// label placement on rendered tiles by giving extra "breathing space" to label placement algorithms.
    ///
    /// \return
    /// A byte reader containing the rendered tile image.
    ///
    /// \since 4.0
    virtual MgByteReader* RenderTile(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 tileColumn,
        INT32 tileRow,
        INT32 tileWidth,
        INT32 tileHeight,
        INT32 tileDpi,
        CREFSTRING tileImageFormat,
        double tileExtentOffset);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified map tile for the given map. Tile structure is
    /// based on the XYZ tiling scheme used by Google Maps, OpenStreetMap, and
    /// others
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for which to render the tile.
    /// \param x
    /// Input
    /// Specifies the row index of the tile to return.
    /// \param y
    /// Input
    /// Specifies the column index of the tile to return.
    /// \param z
    /// Input
    /// Specifies the zoom level of the tile to return.
    ///
    /// \return
    /// A byte reader containing the rendered tile image.
    ///
    virtual MgByteReader* RenderTileXYZ(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 x,
        INT32 y,
        INT32 z);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified map tile for the given map. Tile structure is
    /// based on the XYZ tiling scheme used by Google Maps, OpenStreetMap, and
    /// others
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for which to render the tile.
    /// \param x
    /// Input
    /// Specifies the row index of the tile to return.
    /// \param y
    /// Input
    /// Specifies the column index of the tile to return.
    /// \param z
    /// Input
    /// Specifies the zoom level of the tile to return.
    /// \param dpi
    /// Input
    /// Specifies the dpi of the tile to return.
    /// \param tileImageFormat
    /// Input
    /// Specifies the image format of the tile to return.
    ///
    /// \return
    /// A byte reader containing the rendered tile image.
    ///
    virtual MgByteReader* RenderTileXYZ(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 x,
        INT32 y,
        INT32 z,
        INT32 dpi,
        CREFSTRING tileImageFormat);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified map tile for the given map. Tile structure is
    /// based on the XYZ tiling scheme used by Google Maps, OpenStreetMap, and
    /// others
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for which to render the tile.
    /// \param x
    /// Input
    /// Specifies the row index of the tile to return.
    /// \param y
    /// Input
    /// Specifies the column index of the tile to return.
    /// \param z
    /// Input
    /// Specifies the zoom level of the tile to return.
    /// \param dpi
    /// Input
    /// Specifies the dpi of the tile to return.
    /// \param tileImageFormat
    /// Input
    /// Specifies the image format of the tile to return.
    /// \param tileExtentOffset
    /// Specifies the ratio by which the tile to be rendered should be "buffered out". The tile will be rendered at the specified width
    /// multiplied by the given ration, which will be cropped back to the original requested size after rendering. This is to improve
    /// label placement on rendered tiles by giving extra "breathing space" to label placement algorithms.
    /// \param retinaScale
    /// The scaling factor to apply for "retina" tiles
    ///
    /// \return
    /// A byte reader containing the rendered tile image.
    ///
    /// \since 4.0
    virtual MgByteReader* RenderTileXYZ(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 x,
        INT32 y,
        INT32 z,
        INT32 dpi,
        CREFSTRING tileImageFormat,
        double tileExtentOffset,
        INT32 retinaScale);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified UTFGrid tile for the given map. Tile structure is
    /// based on the XYZ tiling scheme used by Google Maps, OpenStreetMap, and
    /// others
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for which to render the tile.
    /// \param x
    /// Input
    /// Specifies the row index of the tile to return.
    /// \param y
    /// Input
    /// Specifies the column index of the tile to return.
    /// \param z
    /// Input
    /// Specifies the zoom level of the tile to return.
    /// \param dpi
    /// Input
    /// Specifies the dpi of the tile to return.
    /// \param tileImageFormat
    /// Input
    /// Specifies the image format of the tile to return.
    ///
    /// \return
    /// A byte reader containing the rendered tile image.
    ///
    virtual MgByteReader* RenderTileUTFGrid(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 x,
        INT32 y,
        INT32 z,
        INT32 dpi);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders all dynamic layers in the specified MgMap to a dynamic overlay image
    /// with a transparent background. The center, scale, size, and layers to be
    /// rendered are defined by the specified map instance.  The format parameter
    /// must be set to an image format that supports transparency.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderDynamicOverlay(
        MgMap* map,
        MgSelection* selection,
        CREFSTRING format);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders all dynamic layers in the specified MgMap to a dynamic overlay image
    /// with a transparent background. The center, scale, size, and layers to be
    /// rendered are defined by the specified map instance.  The format parameter
    /// must be set to an image format that supports transparency.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    /// \param bKeepSelection
    /// Input
    /// true if you want to keep the selection
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderDynamicOverlay(
        MgMap* map,
        MgSelection* selection,
        CREFSTRING format,
        bool bKeepSelection);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders all dynamic layers in the specified MgMap to a dynamic overlay image
    /// with a transparent background. The center, scale, size, and layers to be
    /// rendered are defined by the specified map instance.  The format parameter
    /// must be set to an image format that supports transparency.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param options
    /// Input
    /// rendering options
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderDynamicOverlay(
        MgMap* map,
        MgSelection* selection,
        MgRenderingOptions* options);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the specified MgMap to the requested image format.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMap(
        MgMap* map,
        MgSelection* selection,
        CREFSTRING format);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the specified MgMap to the requested image format.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    /// \param bKeepSelection
    /// Input
    /// true if you want to keep the selection
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMap(
        MgMap* map,
        MgSelection* selection,
        CREFSTRING format,
        bool bKeepSelection);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the specified MgMap to the requested image format.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    /// \param bKeepSelection
    /// Input
    /// true if you want to keep the selection
    /// \param bKeepSelection
    /// Input
    /// true if you want to clip feature geometry
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMap(
        MgMap* map,
        MgSelection* selection,
        CREFSTRING format,
        bool bKeepSelection,
        bool bClip);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the specified MgMap to the requested image format.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    /// \param bKeepSelection
    /// Input
    /// true if you want to keep the selection
    /// \param bClip
    /// Input
    /// true if you want to clip feature geometry
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMap(
        MgMap* map,
        MgSelection* selection,
        CREFSTRING format,
        bool bKeepSelection,
        bool bClip,
        MgColor* selectionColor);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the specified MgMap to the requested image format.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param extents
    /// Input
    /// map extents. Specifies the extents for the map
    /// \param width
    /// Input
    /// image width. Specifies the image width in pixels
    /// \param height
    /// Input
    /// image height. Specifies the image height in pixels
    /// \param backgroundColor
    /// Input
    /// background color. Specifies the map background color
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMap(
        MgMap* map,
        MgSelection* selection,
        MgEnvelope* extents,
        INT32 width,
        INT32 height,
        MgColor* backgroundColor,
        CREFSTRING format);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the specified MgMap to the requested image format.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param extents
    /// Input
    /// map extents. Specifies the extents for the map
    /// \param width
    /// Input
    /// image width. Specifies the image width in pixels
    /// \param height
    /// Input
    /// image height. Specifies the image height in pixels
    /// \param backgroundColor
    /// Input
    /// background color. Specifies the map background color
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    /// \param bKeepSelection
    /// Input
    /// true if you want to keep the selection
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMap(
        MgMap* map,
        MgSelection* selection,
        MgEnvelope* extents,
        INT32 width,
        INT32 height,
        MgColor* backgroundColor,
        CREFSTRING format,
        bool bKeepSelection);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the specified MgMap to the requested image format.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param center
    /// Input
    /// map center point. Specifies the center point for the map
    /// \param scale
    /// Input
    /// map scale. Specifies the scale for the map
    /// \param width
    /// Input
    /// image width. Specifies the image width in pixels
    /// \param height
    /// Input
    /// image height. Specifies the image height in pixels
    /// \param backgroundColor
    /// Input
    /// background color. Specifies the map background color
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    /// \param bKeepSelection
    /// Input
    /// true if you want to keep the selection
    /// \param selectionColor
    /// Input
    /// The color to use for rendered selections
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMap(
        MgMap* map,
        MgSelection* selection,
        MgCoordinate* center,
        double scale,
        INT32 width,
        INT32 height,
        MgColor* backgroundColor,
        CREFSTRING format,
        bool bKeepSelection,
        MgColor* selectionColor);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the specified MgMap to the requested image format.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param center
    /// Input
    /// map center point. Specifies the center point for the map
    /// \param scale
    /// Input
    /// map scale. Specifies the scale for the map
    /// \param width
    /// Input
    /// image width. Specifies the image width in pixels
    /// \param height
    /// Input
    /// image height. Specifies the image height in pixels
    /// \param backgroundColor
    /// Input
    /// background color. Specifies the map background color
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMap(
        MgMap* map,
        MgSelection* selection,
        MgCoordinate* center,
        double scale,
        INT32 width,
        INT32 height,
        MgColor* backgroundColor,
        CREFSTRING format);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the specified MgMap to the requested image format.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param center
    /// Input
    /// map center point. Specifies the center point for the map
    /// \param scale
    /// Input
    /// map scale. Specifies the scale for the map
    /// \param width
    /// Input
    /// image width. Specifies the image width in pixels
    /// \param height
    /// Input
    /// image height. Specifies the image height in pixels
    /// \param backgroundColor
    /// Input
    /// background color. Specifies the map background color
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    /// \param bKeepSelection
    /// Input
    /// true if you want to keep the selection
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMap(
        MgMap* map,
        MgSelection* selection,
        MgCoordinate* center,
        double scale,
        INT32 width,
        INT32 height,
        MgColor* backgroundColor,
        CREFSTRING format,
        bool bKeepSelection);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the legend for the specified MgMap to the requested size and format
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param width
    /// Input
    /// width of legend image in pixels
    /// \param height
    /// Input
    /// height of legend image in pixels
    /// \param backgroundColor
    /// Input
    /// background color. Specifies the map legend background color
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMapLegend(
        MgMap* map,
        INT32 width,
        INT32 height,
        MgColor* backgroundColor,
        CREFSTRING format);


    /////////////////////////////////////////////////////////////////
    /// \brief
    /// The QueryFeatures operation identifies those features that
    /// meet the specified spatial selection criteria. This operation
    /// is used to implement server-side selection. In addition to
    /// a selection set, this operation returns attribute information
    /// in case only one feature is selected
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param layerName
    /// Input
    /// Active layer name for which to query features
    /// \param filterGeometry
    /// Input
    /// geometry object specifying the selection area
    /// \param selectionVariant
    /// Input
    /// selection criterion - integer value corresponding to one of
    /// the MgFeatureSpatialOperations values
    /// \param maxFeatures
    /// Input
    /// the maximum number of features to return
    ///
    /// \return
    /// An MgSelection instance identifying the features that meet the
    /// selection criteria. Returns null if no features are identified.
    ///
    virtual MgFeatureInformation* QueryFeatures(
        MgMap* map,
        MgStringCollection* layerNames,
        MgGeometry* filterGeometry,
        INT32 selectionVariant,
        INT32 maxFeatures);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// The QueryFeatures operation identifies those features that
    /// meet the specified spatial selection criteria. This operation
    /// is used to implement server-side selection. In addition to
    /// a selection set, this operation returns attribute information
    /// in case only one feature is selected
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param layerName
    /// Input
    /// Active layer name for which to query features
    /// \param filterGeometry
    /// Input
    /// geometry object specifying the selection area
    /// \param selectionVariant
    /// Input
    /// selection criterion - integer value corresponding to one of
    /// the MgFeatureSpatialOperations values
    /// \param featureFilter
    /// Input
    /// an XML selection string containing the required feature IDs
    /// \param maxFeatures
    /// Input
    /// the maximum number of features to return
    /// \param layerAttributeFilter
    /// Input
    /// bitmask values - 1=Visible, 2=Selectable, 4=HasTooltips
    ///
    /// \return
    /// An MgSelection instance identifying the features that meet the
    /// selection criteria. Returns null if no features are identified.
    ///
    virtual MgFeatureInformation* QueryFeatures(
        MgMap* map,
        MgStringCollection* layerNames,
        MgGeometry* filterGeometry,
        INT32 selectionVariant,
        CREFSTRING featureFilter,
        INT32 maxFeatures,
        INT32 layerAttributeFilter);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// The QueryFeatureProperties operation identifies those features that
    /// meet the specified spatial selection criteria. This operation
    /// is used to implement WMS feature info and returns property values
    /// for all features which match the spatial query
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param layerName
    /// Input
    /// Active layer name for which to query features
    /// \param filterGeometry
    /// Input
    /// geometry object specifying the selection area
    /// \param selectionVariant
    /// Input
    /// selection criterion - integer value corresponding to one of
    /// the MgFeatureSpatialOperations values
    /// \param maxFeatures
    /// Input
    /// the maximum number of features to return
    ///
    /// \return
    /// An MgSelection instance identifying the features that meet the
    /// selection criteria. Returns null if no features are identified.
    ///
    virtual MgBatchPropertyCollection* QueryFeatureProperties(
        MgMap* map,
        MgStringCollection* layerNames,
        MgGeometry* filterGeometry,
        INT32 selectionVariant,
        INT32 maxFeatures);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// The QueryFeatureProeprties operation identifies those features that
    /// meet the specified spatial selection criteria. This operation
    /// is used to implement WMS feature info and returns property values
    /// for all features which match the spatial query
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param layerName
    /// Input
    /// Active layer name for which to query features
    /// \param filterGeometry
    /// Input
    /// geometry object specifying the selection area
    /// \param selectionVariant
    /// Input
    /// selection criterion - integer value corresponding to one of
    /// the MgFeatureSpatialOperations values
    /// \param maxFeatures
    /// Input
    /// the maximum number of features to return
    /// \param layerAttributeFilter
    /// Input
    /// bitmask values - 1=Visible, 2=Selectable, 4=HasTooltips
    ///
    /// \return
    /// An MgSelection instance identifying the features that meet the
    /// selection criteria. Returns null if no features are identified.
    ///
    virtual MgBatchPropertyCollection* QueryFeatureProperties(
        MgMap* map,
        MgStringCollection* layerNames,
        MgGeometry* filterGeometry,
        INT32 selectionVariant,
        CREFSTRING featureFilter,
        INT32 maxFeatures,
        INT32 layerAttributeFilter);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgProxyRenderingService object.
    ///
    ///
    MgProxyRenderingService();

    DECLARE_CREATE_SERVICE()

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the connection properties for the Proxy Service.  This
    /// information tells the proxy object where to connect.
    ///
    /// \param connProp
    /// Connection properties for server
    ///
    void SetConnectionProperties(MgConnectionProperties* connProp);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders all dynamic layers in the specified MgMap to a dynamic overlay image
    /// with a transparent background. The center, scale, size, and layers to be
    /// rendered are defined by the specified map instance.  The format parameter
    /// must be set to an image format that supports transparency. Profile result will
    /// be recorded.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param options
    /// Input
    /// rendering options
    /// \param profileRenderMapResult
    /// Input&Output
    /// profile result of rendering dynamic overlay
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderDynamicOverlay(
        MgMap* map,
        MgSelection* selection,
        MgRenderingOptions* options,
        ProfileRenderMapResult* profileRenderMapResult);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Renders the specified MgMap with specified parameters.
    /// Profile result will be recorded.
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param selection
    /// Input
    /// map feature selection. Specifies the selected features on the map
    /// \param center
    /// Input
    /// map center point. Specifies the center point for the map
    /// \param scale
    /// Input
    /// map scale. Specifies the scale for the map
    /// \param width
    /// Input
    /// image width. Specifies the image width in pixels
    /// \param height
    /// Input
    /// image height. Specifies the image height in pixels
    /// \param backgroundColor
    /// Input
    /// background color. Specifies the map background color
    /// \param format
    /// Input
    /// image format. Defines the format of the resulting image
    /// \param bKeepSelection
    /// Input
    /// true if you want to keep the selection
    /// \param profileRenderMapResult
    /// Input&Output
    /// profile result of rendering map
    ///
    /// \return
    /// A byte reader containing the rendered image
    ///
    virtual MgByteReader* RenderMap(
        MgMap* map,
        MgSelection* selection,
        MgCoordinate* center,
        double scale,
        INT32 width,
        INT32 height,
        MgColor* backgroundColor,
        CREFSTRING format,
        bool bKeepSelection,
        ProfileRenderMapResult* profileRenderMapResult);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// The QueryFeatureProperties operation identifies those features that
    /// meet the specified spatial selection criteria. This operation
    /// is used to implement WMS feature info and returns property values
    /// for all features which match the spatial query
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param layerNames
    /// Input
    /// Active layer names for which to query features
    /// \param filterGeometry
    /// Input
    /// geometry object specifying the selection area
    /// \param selectionVariant
    /// Input
    /// selection criterion - integer value corresponding to one of
    /// the MgFeatureSpatialOperations values
    /// \param featureFilter
    /// Input
    /// an XML selection string containing the required feature IDs
    /// \param maxFeatures
    /// Input
    /// the maximum number of features to return
    /// \param layerAttributeFilter
    /// Input
    /// bitmask values - 1=Visible, 2=Selectable, 4=HasTooltips
    ///
    /// \return
    /// An MgSelection instance identifying the features that meet the
    /// selection criteria. Returns null if no features are identified.
    ///
    virtual MgBatchPropertyCollection* QueryFeatureProperties(
        MgMap* map,
        MgStringCollection* layerNames,
        MgGeometry* filterGeometry,
        INT32 selectionVariant,
        CREFSTRING featureFilter,
        INT32 maxFeatures,
        INT32 layerAttributeFilter,
        bool bIncludeFeatureBBOX);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// The QueryFeatureProperties operation identifies those features that
    /// meet the specified spatial selection criteria. This operation
    /// is used to implement WMS feature info and returns property values
    /// for all features which match the spatial query
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param layerNames
    /// Input
    /// Active layer names for which to query features
    /// \param filterGeometry
    /// Input
    /// geometry object specifying the selection area
    /// \param selectionVariant
    /// Input
    /// selection criterion - integer value corresponding to one of
    /// the MgFeatureSpatialOperations values
    /// \param featureFilter
    /// Input
    /// an XML selection string containing the required feature IDs
    /// \param maxFeatures
    /// Input
    /// the maximum number of features to return
    /// \param layerAttributeFilter
    /// Input
    /// bitmask values - 1=Visible, 2=Selectable, 4=HasTooltips
    /// \param bIncludeFeatureBBOX
    /// Input
    /// Indicates whether a bounding box should be included. If true, bounding box is recorded as a special property named _MgFeatureBoundingBox
    /// \param bIncludeGeometry
    /// Input
    /// Indicates whether a bounding box should be included. If true, bounding box is recorded as a special property named _MgFeatureBoundingBox
    ///
    /// \return
    /// An MgSelection instance identifying the features that meet the
    /// selection criteria. Returns null if no features are identified.
    ///
    virtual MgBatchPropertyCollection* QueryFeatureProperties(
        MgMap* map,
        MgStringCollection* layerNames,
        MgGeometry* filterGeometry,
        INT32 selectionVariant,
        CREFSTRING featureFilter,
        INT32 maxFeatures,
        INT32 layerAttributeFilter,
        bool bIncludeFeatureBBOX,
        bool bIncludeGeometry);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified base map tile for the given map.
    ///
    /// \remarks
    /// This method only renders the given tile. No tile caching is performed
    /// by this method. To render and cache the tile, use the 
    /// \link MgTileService::GetTile GetTile \endlink method instead. However,
    /// using that method will use default tile width/height/dpi/format specified
    /// in your MapGuide Server configuration
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for which to render the tile.
    /// \param tileColumn
    /// Input
    /// Specifies the column index of the tile to return.
    /// \param tileRow
    /// Input
    /// Specifies the row index of the tile to return.
    /// \param tileWidth
    /// Input
    /// Specifies the width of the tile to return.
    /// \param tileHeight
    /// Input
    /// Specifies the height of the tile to return.
    /// \param tileDpi
    /// Input
    /// Specifies the dpi the tile to return.
    /// \param tileImageFormat
    /// Input
    /// Specifies the image format of the tile. See \link MgImageFormats \endlink
    /// \param tileExtentOffset
    /// Input
    /// Specifies the ratio by which the tile to be rendered should be "buffered out". The tile will be rendered at the specified width
    /// multiplied by the given ratio, which will be cropped back to the original requested size after rendering. This is to improve
    /// label placement on rendered tiles by giving extra "breathing space" to label placement algorithms.
    /// \param metaTilingFactor
    /// Input
    /// The meta-tiling factor. If less than or equal to 1, no meta-tiling is done and the returned meta-tile can be extracted
    /// as the orignally requested tile image. If greater than 1, a tile that is m times bigger than the requested tile is rendered
    /// (where m is the specified tiling factor) and the raw image frame buffer of this meta-tile is returned instead.
    ///
    /// \return
    /// A meta-tile with sufficient information for the consumer to properly sub-divide this back into sub-tiles of the
    /// originally requested size.
    ///
    /// \since 4.0
    virtual MgMetatile* RenderMetatile(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 tileColumn,
        INT32 tileRow,
        INT32 tileWidth,
        INT32 tileHeight,
        INT32 tileDpi,
        CREFSTRING tileImageFormat,
        double tileExtentOffset,
        INT32 metaTilingFactor);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the specified map tile for the given map. Tile structure is
    /// based on the XYZ tiling scheme used by Google Maps, OpenStreetMap, and
    /// others
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param baseMapLayerGroupName
    /// Input
    /// Specifies the name of the baseMapLayerGroup for which to render the tile.
    /// \param x
    /// Input
    /// Specifies the row index of the tile to return.
    /// \param y
    /// Input
    /// Specifies the column index of the tile to return.
    /// \param z
    /// Input
    /// Specifies the zoom level of the tile to return.
    /// \param dpi
    /// Input
    /// Specifies the dpi of the tile to return.
    /// \param tileImageFormat
    /// Input
    /// Specifies the image format of the tile to return.
    /// \param tileExtentOffset
    /// Input
    /// Specifies the ratio by which the tile to be rendered should be "buffered out". The tile will be rendered at the specified width
    /// multiplied by the given ratio, which will be cropped back to the original requested size after rendering. This is to improve
    /// label placement on rendered tiles by giving extra "breathing space" to label placement algorithms.
    /// \param metaTilingFactor
    /// Input
    /// The meta-tiling factor. If less than or equal to 1, no meta-tiling is done and the returned meta-tile can be extracted
    /// as the orignally requested tile image. If greater than 1, a tile that is m times bigger than the requested tile is rendered
    /// (where m is the specified tiling factor) and the raw image frame buffer of this meta-tile is returned instead.
    /// \param retinaScale
    /// Input
    /// The scaling factor to apply for "retina" tiles
    ///
    /// \return
    /// A meta-tile with sufficient information for the consumer to properly sub-divide this back into sub-tiles of the
    /// originally requested size.
    ///
    /// \since 4.0
    virtual MgMetatile* RenderMetatileXYZ(
        MgMap* map,
        CREFSTRING baseMapLayerGroupName,
        INT32 x,
        INT32 y,
        INT32 z,
        INT32 dpi,
        CREFSTRING tileImageFormat,
        double tileExtentOffset,
        INT32 metaTilingFactor,
        INT32 retinaScale);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the requested sub-tile from the given metatile
    ///
    /// \param map
    /// Input
    /// map object containing current state of map.
    /// \param metaTile
    /// Input
    /// The meta-tile from which a subtile is being requested for
    /// \param subTileX
    /// Input
    /// The x sub-tile coordinate of the meta-tile to request
    /// \param subTileY
    /// Input
    /// The y sub-tile coordinate of the meta-tile to request
    ///
    /// \return
    /// The requested sub-tile
    ///
    /// \since 4.0
    virtual MgByteReader* RenderTileFromMetaTile(MgMap* map, MgMetatile* metaTile, INT32 subTileX, INT32 subTileY);

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object
    ///
    /// \return
    /// Nothing
    ///
    virtual void Dispose();

private:

    void SetWarning(MgWarnings* warning);

    Ptr<MgConnectionProperties> m_connProp;
};
/// \endcond

#endif
