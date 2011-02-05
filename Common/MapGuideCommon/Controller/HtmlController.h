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

#ifndef _MG_HTML_CONTROLLER_H_
#define _MG_HTML_CONTROLLER_H_

/// \cond INTERNAL

//////////////////////////////////////////////////////////////////
/// \brief
/// This class provides functionality for responding to
/// ZCV requests and for generating ZCV update scripts
/// THIS IS INTERNAL API ONLY.  DO NOT DOCUMENT
class MG_MAPGUIDE_API MgHtmlController : public MgController
{
    DECLARE_CLASSNAME(MgHtmlController)

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgHtmlController object
    ///
    MgHtmlController(MgSiteConnection* siteConn);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct a MgHtmlController object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgHtmlController();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Processes a GetDynamicMapOverlayImage request from the Viewer and returns an image of the specified map.
    ///
    /// \param mapName
    /// Name of the map
    /// \param options
    /// Input
    /// rendering options
    ///
    /// \return
    /// A byte reader containing the map image
    ///
    MgByteReader* GetDynamicMapOverlayImage(
        CREFSTRING mapName,
        MgRenderingOptions* options,
        MgPropertyCollection* mapViewCommands);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Processes a GetMapImage request from the Viewer and returns an image of the specified map.
    ///
    /// \param map
    /// Map object to use
    /// \param selection
    /// Selection to use
    /// \param format
    /// Image format, from MgImageFormats
    /// \param mapViewCommands
    /// Commands to be applied to the map before generation
    /// \param bKeepSelection
    /// Input
    /// true if you want to keep the selection
    /// \param bClip
    /// Input
    /// true if you want to clip the feature geometry
    ///
    /// \return
    /// A byte reader containing the map image
    ///
    MgByteReader* GetMapImage(
        MgMap* map,
        MgSelection* selection,
        CREFSTRING format,
        MgPropertyCollection* mapViewCommands,
        bool bKeepSelection,
        bool bClip);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Processes a GetVisibleMapExtent request from the Viewer and returns info the specified map.
    ///
    /// \param mapName
    /// Name of the map
    /// \param mapViewCommands
    /// Commands to be applied to the map before generation
    ///
    /// \return
    /// A byte reader containing the map info
    ///
    MgByteReader* GetVisibleMapExtent(
        CREFSTRING mapName,
        MgPropertyCollection* mapViewCommands);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Processes a DescribeMapFeatures request from the Viewer
    ///
    /// \param mapName
    /// Name of the map
    /// \param layer
    /// Layer for which to provide feature descriptions
    /// \param selectionGeometry
    /// Geometry defining which features to select
    /// \param selectionVariant
    /// Specifies how features are selected
    /// \param maxFeatures
    /// Max number of feature descriptions to return
    /// \param persist
    /// Indicates if the returned selection set should be persisted in the session repository
    ///
    /// \return
    /// A byte reader containing the feature info
    ///
    MgByteReader* QueryMapFeatures(
        CREFSTRING mapName,
        MgStringCollection* layerNames,
        MgGeometry* selectionGeometry,
        INT32 selectionVariant,
        CREFSTRING featureFilter,
        INT32 maxFeatures,
        bool persist,
        INT32 layerAttributeFilter);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Processes a GetLayerImageMap request from the Viewer and returns an image map
    /// for the visible features on the specified map layer
    ///
    /// \param mapName
    /// Name of the map
    /// \param layerName
    /// Name of the layer
    ///
    /// \return
    /// A byte reader containing the image map
    ///
    MgByteReader* GetLayerImageMap(
        CREFSTRING mapName,
        CREFSTRING layerName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// processes a GetMapLegendImage request from the Viewer and returns
    /// an image of the specified maps legend.
    ///
    /// \param mapName
    /// Name of the map
    /// \param format
    /// Image format, from MgImageFormats
    /// \param backgroundColor
    /// Image background color
    /// \param width
    /// Image width
    /// \param height
    /// Image height
    ///
    /// \return
    /// A byte reader containing the image of the legends
    ///
    MgByteReader* GetMapLegendImage(
        CREFSTRING mapName,
        CREFSTRING format,
        MgColor* backgroundColor,
        INT32 width,
        INT32 height);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates JavaScript code that can be embedded in an HTML response
    /// to a non-viewer initiated web application request. The returned code
    /// forces a synchronization of the client-side view with any changes made to the Map Model.
    ///
    /// \param forceFullRefresh
    /// Flag indicating if the entire map must be refreshed
    ///
    /// \return
    /// JavaScript code to update the client map
    ///
    STRING ScriptViewUpdate(bool forceFullRefresh);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates JavaScript code that can be embedded in an HTML response
    /// to a non-viewer initiated web application request. The returned code
    /// forces a synchronization of the client-side view with any changes made to the Map Model.
    ///
    /// \param center
    /// New center of the map ?
    /// \param scale
    /// New scale of the map ?
    /// \param forceFullRefresh
    /// Flag indicating if the entire map must be refreshed
    ///
    /// \return
    /// JavaScript code to update the client map
    ///
    STRING ScriptViewUpdate(
        MgPoint* center,
        double scale,
        bool forceFullRefresh);

protected:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId() { return m_cls_id; }

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Apply the specified set of commands to a map view.
    ///
    virtual void ApplyMapViewCommands(MgMap* map, MgPropertyCollection* mapViewCommands);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose()
    {
        delete this;
    }

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_MapLayer_ZcvController;

};

/// \endcond

#endif
