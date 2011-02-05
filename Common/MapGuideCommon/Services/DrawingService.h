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

#ifndef _MGDRAWING_SERVICE_H
#define _MGDRAWING_SERVICE_H

#include "DrawingDefs.h"

/// \defgroup MgDrawingService MgDrawingService
/// \ingroup Drawing_Service_Module
/// \{

class MgResourceIdentifier;

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Allows low level access to \link DWF DWF™ \endlink
/// (Design Web Format™) data stored in a resource repository as
/// part of a drawing source.
///
/// \remarks
/// The Drawing Service contains one class: MgDrawingService.
/// \n
/// DWF files contain:
/// <ol>
///   <li>An XML file, manifest.xml, listing information about the
///      DWF.</li>
///   <li>One or more <i>resources</i>. These are typically design
///      data in various formats (such as W2D, JPG, PNG, AVI, and
///      XML).</li>
/// </ol>
/// Typically the contents are organized into <i>sections</i>,
/// each of which represents a logical component being published,
/// such as a mechanical part or a page of a multi-page diagram.
/// Each section may have a descriptor.xml file containing
/// section-specific metadata, and each section may contain
/// drawing <i>layers</i>. Sections are sometimes also referred
/// to as <i>sheets.</i>
/// \n
/// The Drawing Service allows you to enumerate parts of the DWF,
/// such as layers, sections, and resources, and to get the
/// binary data from drawings, layers, and a resources. It also
/// provides access to the coordinate space assigned to the DWF
/// in the DrawingSource resource.
///
/// \image html drawing_service_overview.gif
///
class MG_MAPGUIDE_API MgDrawingService : public MgService
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the DWF stream for a drawing specified by resource
    /// identifier.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetDrawing(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetDrawing(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetDrawing(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing
    /// source which has the DWF resource data.
    ///
    /// \return
    /// Returns an MgByteReader object containing the DWF.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the drawing service has already been initialized
    /// $drawing_ID = new MgResourceIdentifier('Library://DrawingService/Floorplan.DrawingSource');
    /// $byteReader = $drawingService->GetDrawing($drawing_ID);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgServiceNotAvailableException if the underlying resource
    /// service cannot be obtained to access the drawing in the
    /// resource repository.
    /// \exception MgXmlParserException if there are problems parsing the resource content
    /// specified by the resource identifier.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    virtual MgByteReader* GetDrawing(MgResourceIdentifier* resource) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the <c>manifest.xml</c> document which describes the
    /// supported document interfaces, the document properties, the
    /// sections and their contents, and section dependencies.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader DescribeDrawing(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader DescribeDrawing(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader DescribeDrawing(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing
    /// source which has the DWF resource data.
    ///
    /// \return
    /// Returns an MgByteReader object containing the manifest.xml
    /// file.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the drawing service has already been initialized
    ///  $drawing_ID = new MgResourceIdentifier('Library://DrawingService/Floorplan.DrawingSource');
    /// $byteReader = $drawingService->DescribeDrawing($drawing_ID);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidCastException if there are problems reading the
    /// DWF into memory.
    /// \exception MgServiceNotAvailableException if the underlying resource
    ///  service cannot be obtained to access the drawing in the
    ///  resource repository.
    /// \exception MgXmlParserException if there are problems parsing the
    ///  resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the
    ///  resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    virtual MgByteReader* DescribeDrawing(MgResourceIdentifier* resource) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a DWF containing only the requested section (sometimes
    /// called a sheet).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetSection(MgResourceIdentifier resource, string sectionName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetSection(MgResourceIdentifier resource, String sectionName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetSection(MgResourceIdentifier resource, string sectionName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing source
    /// which has the DWF resource data.
    /// \param sectionName (String/string)
    /// Name of the section. Section names can be
    /// retrieved via EnumerateSections or from the <c>manifest.xml</c>
    /// file via DescribeDrawing.
    ///
    /// \return
    /// Returns an MgByteReader object containing the DWF for the
    /// specified section.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the drawing service has already been initialized
    /// $drawing_ID = new MgResourceIdentifier('Library://DrawingService/Floorplan.DrawingSource');
    /// $sectionName = 'com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F764';
    /// $byteReader = $drawingService->GetSection($drawing_ID, $sectionName);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception  MgDwfSectionNotFoundException if the requested section
    ///  does not exist in the DWF package.
    /// \exception  MgInvalidDwfSectionException if the 2d graphics resource
    ///  for the DWF section cannot be found.
    /// \exception  MgNullReferenceException no data could be read from the
    ///  DWF resource.
    /// \exception  MgOutOfMemoryException if there is insufficient memory to
    ///  complete the operation.
    /// \exception  MgServiceNotAvailableException if the underlying resource
    ///  service cannot be obtained to access the drawing in the
    ///  resource repository.
    /// \exception  MgXmlParserException if there are problems parsing the
    ///  resource content specified by the resource identifier.
    /// \exception  MgInvalidDwfPackageException if the DWF specified by the
    ///  resource identifier is not a DWF of version 6.0 or greater.
    /// \exception  MgDwfException if the DWF component encounters errors.
    /// \exception  MgTemporaryFileNotAvailableException if a temporary file needed
    ///  to complete the operation cannot be generated or accessed.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    virtual MgByteReader* GetSection(MgResourceIdentifier* resource, CREFSTRING sectionName) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a specific resource from the DWF.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetSectionResource(MgResourceIdentifier resource, string resourceName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetSectionResource(MgResourceIdentifier resource, String resourceName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetSectionResource(MgResourceIdentifier resource, string resourceName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing source
    /// which has the DWF resource data.
    /// \param resourceName (String/string)
    /// Name of the resource. Resource names can be
    /// retrieved via EnumerateSectionResources or from
    /// the <c>manifest.xml</c> file via DescribeDrawing.
    ///
    /// \return
    /// Returns an MgByteReader object containing the resource. The
    /// MIME type will be what is specified in manifest.xml for the
    /// specified resource (for example, image/png or text/xml).
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the drawing service has already been initialized
    /// $drawing_ID = new MgResourceIdentifier('Library://DrawingService/Floorplan.DrawingSource');
    /// $resourceName = 'com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F7644AA701C2E18DD99948F3C0D4FDFAC165.png';
    /// $byteReader = $drawingService->GetSectionResource($drawing_ID, $resourceName);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidArgumentException if the requested resourceName
    ///  does not specify a section name.
    /// \exception MgDwfSectionNotFoundException if the requested section
    ///  does not exist in the DWF package.
    /// \exception MgDwfSectionResourceNotFoundException if the requested
    ///  section resource does not exist in the DWF package.
    /// \exception MgInvalidCastException if there are problems reading the
    ///  DWF into memory.
    /// \exception MgServiceNotAvailableException if the underlying resource
    ///  service cannot be obtained to access the drawing in the
    ///  resource repository.
    /// \exception MgXmlParserException if there are problems parsing the
    ///  resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the
    ///  resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    /// \exception MgTemporaryFileNotAvailableException if a temporary file needed
    ///  to complete the operation cannot be generated or accessed.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    virtual MgByteReader* GetSectionResource(MgResourceIdentifier* resource, CREFSTRING resourceName) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the names of the layers in a DWF section.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgStringCollection EnumerateLayers(MgResourceIdentifier resource, string sectionName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgStringCollection EnumerateLayers(MgResourceIdentifier resource, String sectionName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgStringCollection EnumerateLayers(MgResourceIdentifier resource, string sectionName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing source
    /// which has the DWF resource data.
    /// \param sectionName (String/string)
    /// Name of the section. Section names can be
    /// retrieved via EnumerateSections or from the <c>manifest.xml</c>
    /// file via DescribeDrawing.
    ///
    /// \return
    /// Returns a collection of layer names.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the drawing service has already been initialized
    /// $drawing_ID = new MgResourceIdentifier('Library://DrawingService/Floorplan.DrawingSource');
    /// $sectionName = 'com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F764';
    /// $stringCollection = $drawingService->EnumerateLayers($drawing_ID, $sectionName);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgDwfSectionNotFoundException if the requested section
    ///  does not exist in the DWF package.
    /// \exception MgInvalidDwfSectionException if the 2d graphics resource
    ///  for the DWF section cannot be found.
    /// \exception MgNullReferenceException no data could be read from the
    ///  DWF resource.
    /// \exception MgServiceNotAvailableException if the underlying resource
    ///  service cannot be obtained to access the drawing in the
    ///  resource repository.
    /// \exception MgXmlParserException if there are problems parsing the
    ///  resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the
    ///  resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    /// \exception MgTemporaryFileNotAvailableException if a temporary file needed
    ///  to complete the operation cannot be generated or accessed.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    virtual MgStringCollection* EnumerateLayers(MgResourceIdentifier* resource, CREFSTRING sectionName) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a layer from a particular section of a DWF.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetLayer(MgResourceIdentifier resource, string sectionName, string layerName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetLayer(MgResourceIdentifier resource, String sectionName, String layerName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetLayer(MgResourceIdentifier resource, string sectionName, string layerName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing source
    /// which has the DWF resource data.
    /// \param sectionName (String/string)
    /// Name of the section. Section names can be
    /// retrieved via EnumerateSections or from the the <c>manifest.xml</c>
    /// file file via DescribeDrawing.
    /// \param layerName (String/string)
    /// Name of the layer. Layer names can can be
    /// retrieved via EnumerateLayers.
    ///
    /// \return
    /// Returns DWF stream containing the specified layer in the
    /// specified section.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the drawing service has already been initialized
    /// $drawing_ID = new MgResourceIdentifier('Library://DrawingService/Floorplan.DrawingSource');
    /// $sectionName = 'com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F764';
    /// $layerName = 'YELLOW';
    /// $byteReader = $drawingService->GetLayer($drawing_ID, $sectionName, $layerName);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgDwfSectionNotFoundException if the requested section
    ///  does not exist in the DWF package.
    /// \exception MgInvalidDwfSectionException if the 2d graphics resource
    ///  for the DWF section cannot be found.
    /// \exception MgNullReferenceException if data could not be read from
    ///  the DWF resource.
    /// \exception MgOutOfMemoryException if there is insufficient memory to
    ///  complete the operation.
    /// \exception MgServiceNotAvailableException if the underlying resource
    ///  service cannot be obtained to access the drawing in the
    ///  resource repository.
    /// \exception MgXmlParserException if there are problems parsing the
    ///  resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the
    ///  resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgLayerNotFoundException if the requested layer does not
    ///  exist in the requested section of the DWF package.
    /// \exception MgDwfException if the DWF component encounters errors.
    ///
    /// \note See MgResourceService for additional exceptions.
    ///
    virtual MgByteReader* GetLayer( MgResourceIdentifier* resource,
                                    CREFSTRING sectionName,
                                    CREFSTRING layerName ) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates only the \link ePlot ePlot \endlink sections (sheets) in
    /// a DWF.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader EnumerateSections(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader EnumerateSections(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader EnumerateSections(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing
    /// source which has the DWF resource data.
    ///
    /// \return
    /// Returns an MgByteReader object containing the list of
    /// sections in XML format using the \link DrawingSectionList_schema DrawingSectionList \endlink
    /// schema. The sections are sorted in ascending order based on
    /// their <c>plotOrder</c> attribute.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the drawing service has already been initialized
    /// $drawing_ID = new MgResourceIdentifier('Library://DrawingService/Floorplan.DrawingSource');
    /// $byteReader = $drawingService->EnumerateSections($drawing_ID);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgNullReferenceException no data could be read from the
    ///  DWF resource.
    /// \exception MgServiceNotAvailableException if the underlying resource
    ///  service cannot be obtained to access the drawing in the
    ///  resource repository.
    /// \exception MgXmlParserException if there are problems parsing the
    ///  resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the
    ///  resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    virtual MgByteReader* EnumerateSections(MgResourceIdentifier* resource) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates the resources of a DWF section (sometimes called a
    /// sheet).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader EnumerateSectionResources(MgResourceIdentifier resource, string sectionName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader EnumerateSectionResources(MgResourceIdentifier resource, String sectionName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader EnumerateSectionResources(MgResourceIdentifier resource, string sectionName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing source
    /// which has the DWF resource data.
    /// \param sectionName (String/string)
    /// Name of the section. Section names can be
    /// retrieved via EnumerateSections or from the <c>manifest.xml</c>
    /// file via DescribeDrawing.
    ///
    /// \return
    /// Returns an MgByteReader object containing the list of
    /// resources in XML format using the \link DrawingSectionResourceList_schema DrawingSectionResourceList \endlink
    /// schema.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the drawing service has already been initialized
    /// $drawing_ID = new MgResourceIdentifier('Library://DrawingService/Floorplan.DrawingSource');
    /// $sectionName = 'com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F764';
    /// $byteReader = $drawingService->EnumerateSectionResources($drawing_ID, $sectionName);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgDwfSectionNotFoundException if the requested section
    ///  does not exist in the DWF package.
    /// \exception MgInvalidDwfSectionException if the 2d graphics resource
    ///  for the DWF section cannot be found.
    /// \exception MgNullReferenceException no data could be read from the
    ///  DWF resource.
    /// \exception MgServiceNotAvailableException if the underlying resource
    ///  service cannot be obtained to access the drawing in the
    ///  resource repository.
    /// \exception MgXmlParserException if there are problems parsing the
    ///  resource content specified by the resource identifier.
    /// \exception MgInvalidDwfPackageException if the DWF specified by the
    ///  resource identifier is not a DWF of version 6.0 or greater.
    /// \exception MgDwfException if the DWF component encounters errors.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    virtual MgByteReader* EnumerateSectionResources(MgResourceIdentifier* resource, CREFSTRING sectionName) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system assigned to the DWF drawing.
    ///
    /// \remarks
    /// If no coordinate system is specified for the DWF, the default
    /// is <c>GEOGCS["LL84",DATUM["WGS_1984",SPHEROID["WGS
    /// 84",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM["Greenwich",0],UNIT["Degrees",1]]</c>
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string GetCoordinateSpace(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String GetCoordinateSpace(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string GetCoordinateSpace(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing
    /// source which has the DWF resource data.
    ///
    /// \return
    /// Returns a string representing the coordinate system in OGC \link WKT WKT \endlink
    /// format.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming the drawing service has already been initialized
    /// $drawing_ID = new MgResourceIdentifier('Library://DrawingService/Floorplan.DrawingSource');
    /// $coordinateSpace = $drawingService->GetCoordinateSpace($drawing_ID);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgServiceNotAvailableException if the underlying resource
    ///  service cannot be obtained to access the drawing resource in
    ///  the repository.
    /// \exception MgXmlParserException if there are problems parsing the
    ///  resource content specified by the resource identifier.
    ///
    /// \note
    /// See MgResourceService for additional exceptions.
    ///
    virtual STRING GetCoordinateSpace(MgResourceIdentifier* resource) = 0;

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgDrawingService object.
    ///
    MgDrawingService();

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

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_DrawingService_DrawingService;
};
/// \}

#endif
