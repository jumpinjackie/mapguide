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

#ifndef MG_KML_SERVICE_H
#define MG_KML_SERVICE_H

/// \defgroup MgKmlService MgKmlService
/// \ingroup Kml_Service_Module
/// \{

class MgByteReader;
class MgMap;

/// \brief
/// This class provides services to retrieve KML documents
class MG_SERVICE_API MgKmlService : public MgService
{
PUBLISHED_API:

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
        CREFSTRING agentUri,
        CREFSTRING format) = 0;

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
        CREFSTRING agentUri,
        CREFSTRING format) = 0;

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
    /// \param format
    /// Input
    /// requested response format
    ///
    /// \param scale
    /// Input
    /// scale for rendering
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
        CREFSTRING format) = 0;

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgKmlService object.
    ///
    /// \param connection
    /// Connection to server
    ///
    MgKmlService(MgConnectionProperties* connection);

protected:

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

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default Constructor
    ///
    MgKmlService();

CLASS_ID:
    static const INT32 m_cls_id = KmlService_KmlService;
};
/// \}

#endif
