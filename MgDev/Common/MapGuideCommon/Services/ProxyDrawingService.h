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

#ifndef _MG_PROXY_DRAWING_SERVICE_H
#define _MG_PROXY_DRAWING_SERVICE_H

/// \cond INTERNAL
////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// This class allows to retrieve DWF (ePlot based) or create an instance of
/// DrawingConnection to operate on the DWF.
class MG_MAPGUIDE_API MgProxyDrawingService : public MgDrawingService
{
    DECLARE_CLASSNAME(MgProxyDrawingService)

EXTERNAL_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// GetDrawing() returns the DWF stream for a drawing specified by resource identifier.
    ///
    /// \param resource
    /// Input
    /// MgResourceIdentifier object identifying the DWF resource
    ///
    /// \return
    /// Returns DWF stream.
    ///
    /// \exception MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// \exception MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    MgByteReader* GetDrawing(MgResourceIdentifier* resource);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// DescribeDrawing() returns manifest.xml document located in the DWF.
    /// Refer to the DWF Format Specification at http://viewers/web/Olema/pdf/DWF%206%20Corporate%20Publishing%20Format.pdf for more information on the manifest.
    ///
    /// \param resource
    /// Input
    /// MgResourceIdentifier object identifying the DWF resource
    ///
    /// \return
    /// Returns the byte stream containing XML (manifest.xml).
    ///
    /// \exception MgInvalidCastException if there are problems reading the DWF into memory.
    /// \exception MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// \exception MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    MgByteReader* DescribeDrawing(MgResourceIdentifier* resource);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// GetSection() returns a DWF containing only the requested section.  The section is specified by resource identifier (to get the DWF from the repository),
    /// and the section name.  The section names can be retrieved via call to MgDrawingService::EnumerateSections() or from manifest.xml via call to MgDrawingService::DescribeDrawing().
    ///
    /// \param resource
    /// Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// \param sectionName
    /// Input
    /// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgDrawingService::EnumerateSections() or from manifest.xml via call to MgDrawingService::DescribeDrawing().
    ///
    /// \return
    /// Returns DWF stream containing the specified section.
    ///
    /// \exception MgDwfSectionNotFoundException if the requested section does not exist in the DWF package.
    /// \exception MgInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
    /// \exception MgNullReferenceException no data could be read from the DWF resource.
    /// \exception MgOutOfMemoryException if there is insufficient memory to complete the operation.
    /// \exception MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// \exception MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    /// \exception MgTemporaryFileNotAvailableException if a temporary file needed to complete the operation cannot be generated or accessed.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    MgByteReader* GetSection(MgResourceIdentifier* resource, CREFSTRING sectionName);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// GetSectionResource() extracts a specific resource from the DWF package.
    /// It is specified by the resource identifier (to get the DWF from the repository)
    /// and the resource name (to get the DWF resource from the DWF package).
    /// A list of resource names can be retrieved via call to MgDrawingServices::EnumerateSectionResources(),
    /// or from the manifest.xml via call to MgDrawingServices::DescribeDrawing().
    /// Refer to the DWF Format Specification at http://viewers/web/Olema/pdf/DWF%206%20Corporate%20Publishing%20Format.pdf for more information resource files associated with a particular section.
    ///
    /// \param resource
    /// Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// \param resourceName
    /// Input
    /// Unique resource name of an item in the DWF.
    /// The item's name can be retrieved from call to MgDrawingServices::EnumerateDrawingServices(),
    /// or from the manifest.xml via call to MgDrawingServices::DescribeDrawing().
    ///
    /// \return
    /// Returns byte stream for the item.
    /// Content type will be same as what is specified in manifest.xml for
    /// the specified item e.g. image/png or text/xml etc.
    ///
    /// \exception MgInvalidArgumentException if the requested resourceName does not specify a section name.
    /// \exception MgDwfSectionNotFoundException if the requested section does not exist in the DWF package.
    /// \exception MgDwfSectionResourceNotFoundException if the requested section resource does not exist in the DWF package.
    /// \exception MgInvalidCastException if there are problems reading the DWF into memory.
    /// \exception MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// \exception MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    /// \exception MgTemporaryFileNotAvailableException if a temporary file needed to complete the operation cannot be generated or accessed.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    MgByteReader* GetSectionResource(MgResourceIdentifier* resource, CREFSTRING resourceName);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// EnumerateLayers() returns the layer names in a DWF ePlot section.  An ePlot section is also known as a "sheet" in DWF viewers.
    /// The ePlot section is specified by resource identifier (to get the DWF from the repository), and section name.
    /// A list of all sections in a DWF can be retrieved from call to MgDrawingServices::EnumerateSections(),
    /// or from the manifest.xml via call to MgDrawingServices::DescribeDrawing().
    ///  Refer to the DWF Format Specification at http://viewers/web/Olema/pdf/DWF%206%20Corporate%20Publishing%20Format.pdf for more information on the manifest and sections.
    ///
    /// \param resource
    /// Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// \param sectionName
    /// Input
    /// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgDrawingService::EnumerateSections() or from manifest.xml via call to MgDrawingService::DescribeDrawing().
    ///
    /// \return
    /// Returns the pointer to a StringCollection of layer names.
    ///
    /// \exception MgDwfSectionNotFoundException if the requested section does not exist in the DWF package.
    /// \exception MgInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
    /// \exception MgNullReferenceException no data could be read from the DWF resource.
    /// \exception MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// \exception MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    /// \exception MgTemporaryFileNotAvailableException if a temporary file needed to complete the operation cannot be generated or accessed.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    MgStringCollection* EnumerateLayers(MgResourceIdentifier* resource, CREFSTRING sectionName);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// GetLayer() extracts a layer from a particular ePlot section of a DWF.
    ///
    /// \param resource
    /// Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// \param sectionName
    /// Input
    /// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgDrawingService::EnumerateSections() or from manifest.xml via call to MgDrawingService::DescribeDrawing().
    /// \param layerName
    /// Input
    /// layerName specifies the name of the layer to retrieve from a particular section.  A list of layer names can can be retrieved via call to MgDrawingService::EnumerateLayers().
    ///
    /// \return
    /// Returns DWF stream containing the specified layer (in a section)
    ///
    /// \exception MgDwfSectionNotFoundException if the requested section does not exist in the DWF package.
    /// \exception MgInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
    /// \exception MgNullReferenceException if data could not be read from the DWF resource.
    /// \exception MgOutOfMemoryException if there is insufficient memory to complete the operation.
    /// \exception MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// \exception MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgLayerNotFoundException if the requested layer does not exist in the requested section of the DWF package.
    /// \exception MgDwfException if the DWF component encounters errors.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    MgByteReader* GetLayer( MgResourceIdentifier* resource,
                            CREFSTRING sectionName,
                            CREFSTRING layerName );

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// EnumerateSections() enumerates the sections in a DWF.  The DWF is identified by it's
    /// resource identifier.
    ///
    /// \param resource
    /// Input
    /// MgResourceIdentifier object identifying the DWF resource
    ///
    /// \return
    /// Returns MgByteReader object representing the sections in a DWF.
    ///
    /// \exception MgNullReferenceException no data could be read from the DWF resource.
    /// \exception MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// \exception MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    MgByteReader* EnumerateSections(MgResourceIdentifier* resource);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// EnumerateSections() enumerates only the ePlot sections in a DWF.  The returned sections are in ascending order based on its plotOrder attribute.
    /// The DWF is identified by it's resource identifier.
    ///
    /// \param resource
    /// Input
    /// MgResourceIdentifier object identifying the DWF resource
    ///
    /// \return
    /// Returns MgByteReader object representing the ePlot sections in a DWF sorted in ascending order based on plotOrder attribute.
    ///
    /// \exception MgDwfSectionNotFoundException if the requested section does not exist in the DWF package.
    /// \exception MgInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
    /// \exception MgNullReferenceException no data could be read from the DWF resource.
    /// \exception MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// \exception MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    MgByteReader* EnumerateSectionResources(MgResourceIdentifier* resource, CREFSTRING sectionName);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// GetCoordinateSpace() returns the coordinate system assigned to the DWF drawing.
    /// The coordinate system will be retrieved from the drawing source content document.
    /// If no coordinate system is specified, the default will be LL84.
    ///
    /// \param resource
    /// Input
    /// MgResourceIdentifier object identifying the DWF resource
    ///
    /// \return
    /// Returns the STRING representing the coordinate system in AWKT format.
    ///
    /// \exception MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing resource in the repository.
    /// \exception MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    STRING GetCoordinateSpace(MgResourceIdentifier* resource);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgProxyDrawingService object.
    ///
    ///
    MgProxyDrawingService();

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
