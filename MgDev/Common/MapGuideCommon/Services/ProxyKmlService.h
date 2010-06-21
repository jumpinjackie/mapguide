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

#ifndef _MG_PROXY_KML_SERVICE_H
#define _MG_PROXY_KML_SERVICE_H

/// \cond INTERNAL
////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// This class allows generation of KML documents
class MG_MAPGUIDE_API MgProxyKmlService : public MgKmlService
{
    DECLARE_CLASSNAME(MgProxyKmlService)

EXTERNAL_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a map definition in KML format
    ///
    /// \param map
    /// Input
    /// map object for which to generate KML.
    ///
    /// \param agentUri
    /// Input
    /// URI of map agent that issued request
    ///
    /// \param format
    /// Input
    /// requested response format
    ///
    /// \return
    /// A byte reader containing the generated KML.
    ///
    virtual MgByteReader* GetMapKml(
        MgMap* map,
        double dpi,
        CREFSTRING agentUri,
        CREFSTRING format);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a layer definition in KML format
    ///
    /// \param layer
    /// Input
    /// layer object for which to generate KML.
    ///
    /// \param extents
    /// Input
    /// required spatial data extents
    ///
    /// \param width
    /// Input
    /// map width in pixels
    ///
    /// \param height
    /// Input
    /// map height in pixels
    ///
    /// \param dpi
    /// Input
    /// resolution in dots per inch
    ///
    /// \param drawOrder
    /// Input
    /// the draw order for the layer
    ///
    /// \param agentUri
    /// Input
    /// URI of map agent that issued request
    ///
    /// \param format
    /// Input
    /// requested response format
    ///
    /// \return
    /// A byte reader containing the generated KML.
    ///
    virtual MgByteReader* GetLayerKml(
        MgLayer* layer,
        MgEnvelope* extents,
        INT32 width,
        INT32 height,
        double dpi,
        INT32 drawOrder,
        CREFSTRING agentUri,
        CREFSTRING format);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a layer definition in KML format
    ///
    /// \param layer
    /// Input
    /// layer object for which to generate KML.
    ///
    /// \param extents
    /// Input
    /// required spatial data extents
    ///
    /// \param width
    /// Input
    /// map width in pixels
    ///
    /// \param height
    /// Input
    /// map height in pixels
    ///
    /// \param dpi
    /// Input
    /// resolution in dots per inch
    ///
    /// \param drawOrder
    /// Input
    /// the draw order for the layer containing these features
    ///
    /// \param format
    /// Input
    /// requested response format
    ///
    /// \return
    /// A byte reader containing the generated KML.
    ///
    virtual MgByteReader* GetFeaturesKml(
        MgLayer* layer,
        MgEnvelope* extents,
        INT32 width,
        INT32 height,
        double dpi,
        INT32 drawOrder,
        CREFSTRING format);


INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgProxyKmlService object.
    ///
    /// \param connection
    /// Connection to server
    ///
    MgProxyKmlService();

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
