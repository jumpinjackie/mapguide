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

#ifndef MGSERVERDRAWINGSERVICE_H_
#define MGSERVERDRAWINGSERVICE_H_

#include "ServerDrawingDllExport.h"
#include "ServerResourceService.h"

class MG_SERVER_DRAWING_API MgServerDrawingService : public MgDrawingService
{
    DECLARE_CLASSNAME(MgServerDrawingService)

public:
    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// GetDrawing() returns the DWF stream for a drawing specified by resource identifier.
    /// </summary>
    /// <param name="resource">Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// </param>
    /// <returns>
    /// Returns DWF stream.
    /// </returns>
    /// EXCEPTIONS:
    /// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// - See MgResourceService for additional exceptions.
    MgByteReader* GetDrawing(MgResourceIdentifier* resource);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// DescribeDrawing() returns manifest.xml document located in the DWF.
    /// Refer to the DWF Format Specification at http://viewers/web/Olema/pdf/DWF%206%20Corporate%20Publishing%20Format.pdf for more information on the manifest.
    /// </summary>
    /// <param name="resource">Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// </param>
    /// <returns>
    /// Returns the byte stream containing XML (manifest.xml).
    /// </returns>
    /// EXCEPTIONS:
    /// - MgInvalidCastException if there are problems reading the DWF into memory.
    /// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// - MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// - MgDwfException if the DWF component encounters errors.
    /// - See MgResourceService for additional exceptions.
    MgByteReader* DescribeDrawing(MgResourceIdentifier* resource);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// GetSection() returns a DWF containing only the requested section.  The section is specified by resource identifier (to get the DWF from the repository),
    /// and the section name.  The section names can be retrieved via call to MgDrawingService::EnumerateSections() or from manifest.xml via call to MgDrawingService::DescribeDrawing().
    /// </summary>
    /// <param name="resource">Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// </param>
    /// <param name="sectionName">Input
    /// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgDrawingService::EnumerateSections() or from manifest.xml via call to MgDrawingService::DescribeDrawing().
    /// </param>
    /// <returns>
    /// Returns DWF stream containing the specified section.
    /// </returns>
    /// EXCEPTIONS:
    /// - MgDwfSectionNotFoundException if the requested section does not exist in the DWF package.
    /// - MgInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
    /// - MgNullReferenceException no data could be read from the DWF resource.
    /// - MgOutOfMemoryException if there is insufficient memory to complete the operation.
    /// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// - MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// - MgDwfException if the DWF component encounters errors.
    /// - MgTemporaryFileNotAvailableException if a temporary file need to complete the operation cannot be generated or accessed.
    /// - See MgResourceService for additional exceptions.
    MgByteReader* GetSection(MgResourceIdentifier* resource, CREFSTRING sectionName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// GetSectionResource() extracts a specific resource from the DWF package.
    /// It is specified by the resource identifier (to get the DWF from the repository)
    /// and the resource name (to get the DWF resource from the DWF package).
    /// A list of resource names can be retrieved via call to MgDrawingServices::EnumerateSectionResources(),
    /// or from the manifest.xml via call to MgDrawingServices::DescribeDrawing().
    /// Refer to the DWF Format Specification at http://viewers/web/Olema/pdf/DWF%206%20Corporate%20Publishing%20Format.pdf for more information resource files associated with a particular section.
    ///
    /// </summary>
    /// <param name="resource">Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// </param>
    /// <param name="resourceName">Input
    /// Unique resource name of an item in the DWF.
    /// The item's name can be retrieved from call to MgDrawingServices::EnumerateDrawingServices(),
    /// or from the manifest.xml via call to MgDrawingServices::DescribeDrawing().
    /// </param>
    /// <returns>
    /// Returns byte stream for the item.
    /// Content type will be same as what is specified in manifest.xml for
    /// the specified item e.g. image/png or text/xml etc.
    /// </returns>
    /// EXCEPTIONS:
    /// - MgInvalidArgumentException if the requested resourceName does not specify a section name.
    /// - MgDwfSectionNotFoundException if the requested section does not exist in the DWF package.
    /// - MgDwfSectionResourceNotFoundException if the requested section resource does not exist in the DWF package.
    /// - MgInvalidCastException if there are problems reading the DWF into memory.
    /// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// - MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// - MgDwfException if the DWF component encounters errors.
    /// - MgTemporaryFileNotAvailableException if a temporary file need to complete the operation cannot be generated or accessed.
    /// - See MgResourceService for additional exceptions.
    MgByteReader* GetSectionResource(MgResourceIdentifier* resource, CREFSTRING resourceName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// EnumerateLayers() returns the layer names in a DWF ePlot section.  An ePlot section is also known as a "sheet" in DWF viewers.
    /// The ePlot section is specified by resource identifier (to get the DWF from the repository), and section name.
    /// A list of all sections in a DWF can be retrieved from call to MgDrawingServices::EnumerateSections(),
    /// or from the manifest.xml via call to MgDrawingServices::DescribeDrawing().
    //  Refer to the DWF Format Specification at http://viewers/web/Olema/pdf/DWF%206%20Corporate%20Publishing%20Format.pdf for more information on the manifest and sections.
    /// </summary>
    /// <param name="resource">Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// </param>
    /// <param name="sectionName">Input
    /// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgDrawingService::EnumerateSections() or from manifest.xml via call to MgDrawingService::DescribeDrawing().
    /// </param>
    /// <returns>
    /// Returns the pointer to a StringCollection of layer names.
    /// </returns>
    /// EXCEPTIONS:
    /// - MgDwfSectionNotFoundException if the requested section does not exist in the DWF package.
    /// - MgInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
    /// - MgNullReferenceException no data could be read from the DWF resource.
    /// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// - MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// - MgDwfException if the DWF component encounters errors.
    /// - MgTemporaryFileNotAvailableException if a temporary file need to complete the operation cannot be generated or accessed.
    /// - See MgResourceService for additional exceptions.
    MgStringCollection* EnumerateLayers(MgResourceIdentifier* resource, CREFSTRING sectionName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// GetLayer() extracts a layer from a particular ePlot section of a DWF.
    /// </summary>
    /// <param name="resource">Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// </param>
    /// <param name="sectionName">Input
    /// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgDrawingService::EnumerateSections() or from manifest.xml via call to MgDrawingService::DescribeDrawing().
    /// </param>
    /// <param name="layerName">Input
    /// layerName specifies the name of the layer to retrieve from a particular section.  A list of layer names can can be retrieved via call to MgDrawingService::EnumerateLayers().
    /// </param>
    /// <returns>
    /// Returns DWF stream containing the specified layer (in a section)
    /// </returns>
    /// EXCEPTIONS:
    /// - MgDwfSectionNotFoundException if the requested section does not exist in the DWF package.
    /// - MgInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
    /// - MgNullReferenceException if data could not be read from the DWF resource.
    /// - MgOutOfMemoryException if there is insufficient memory to complete the operation.
    /// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// - MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// - MgLayerNotFoundException if the requested layer does not exist in the requested section of the DWF package.
    /// - MgDwfException if the DWF component encounters errors.
    /// - See MgResourceService for additional exceptions.
    MgByteReader* GetLayer( MgResourceIdentifier* resource,
                            CREFSTRING sectionName,
                            CREFSTRING layerName );

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// EnumerateSections() enumerates only the ePlot sections in a DWF.  The returned sections are in ascending order based on its plotOrder attribute.
    /// The DWF is identified by it's resource identifier.
    /// </summary>
    /// <param name="resource">Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// </param>
    /// <returns>
    /// Returns MgByteReader object representing the ePlot sections in a DWF sorted in ascending order based on plotOrder attribute.
    ///
    /// </returns>
    /// EXCEPTIONS:
    /// - MgNullReferenceException no data could be read from the DWF resource.
    /// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// - MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// - MgDwfException if the DWF component encounters errors.
    /// - See MgResourceService for additional exceptions.
    MgByteReader* EnumerateSections(MgResourceIdentifier* resource);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// EnumerateSectionResources() enumerates the resources of a DWF section.  The DWF is identified by it's
    /// resource identifier and the section is identified by name.  The section name
    /// will be retrieved from manifest.xml or from MgDrawingServices::EnumerateSections() API.
    /// </summary>
    /// <param name="resource">Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// </param>
    /// <param name="sectionName">Input
    /// sectionName specifies the unique name of the section in the DWF resource.  Section names can be retrieved via call to MgDrawingService::EnumerateSections() or from manifest.xml via call to MgDrawingService::DescribeDrawing().
    /// </param>
    /// <returns>
    /// Returns MgByteReader object representing resources in a DWF section.
    ///
    /// </returns>
    /// EXCEPTIONS:
    /// - MgDwfSectionNotFoundException if the requested section does not exist in the DWF package.
    /// - MgInvalidDwfSectionException if the 2d graphics resource for the DWF section cannot be found.
    /// - MgNullReferenceException no data could be read from the DWF resource.
    /// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing in the resource repository.
    /// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// - MgInvalidDwfPackageException if the DWF specified by the resource identifier is not a DWF of version 6.0 or greater.
    /// - MgDwfException if the DWF component encounters errors.
    /// - See MgResourceService for additional exceptions.
    MgByteReader* EnumerateSectionResources(MgResourceIdentifier* resource, CREFSTRING sectionName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// GetCoordinateSpace() returns the coordinate system assigned to the DWF drawing.
    /// The coordinate system will be retrieved from the drawing source content document.
    /// If no coordinate system is specified, the default will be LL84.
    /// </summary>
    /// <param name="resource">Input
    /// MgResourceIdentifier object identifying the DWF resource
    /// </param>
    /// <returns>
    /// Returns the STRING representing the coordinate system in AWKT format.
    /// </returns>
    /// EXCEPTIONS:
    /// - MgServiceNotAvailableException if the underlying resource service cannot be obtained to access the drawing resource in the repository.
    /// - MgXmlParserException if there are problems parsing the resource content specified by the resource identifier.
    /// - See MgResourceService for additional exceptions.
    virtual STRING GetCoordinateSpace(MgResourceIdentifier* resource);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructor
    /// </summary>
    virtual ~MgServerDrawingService();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Construct an MgServerDrawingService object
    /// </summary>
    MgServerDrawingService();

    DECLARE_CREATE_SERVICE()

    void SetConnectionProperties(MgConnectionProperties* connProp);

private:

    void CleanUpTempFiles();

    Ptr<MgResourceService> m_resourceService;

    bool m_bOpenTempDwfFile;
    STRING m_tempDwfFileName;

    bool m_bOpenTempW2dFile;
    STRING m_tempW2dFileName;

    bool m_bOpenTempOutputW2dFile;
    STRING m_tempOutputW2dFileName;
};

#endif
