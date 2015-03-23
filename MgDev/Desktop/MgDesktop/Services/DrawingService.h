#ifndef DESKTOP_DRAWING_SERVICE_H
#define DESKTOP_DRAWING_SERVICE_H

#include "MgDesktop.h"

/// \ingroup Desktop_Service_Module
/// \{

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
class MG_DESKTOP_API MgdDrawingService : public MgService
{
    DECLARE_CLASSNAME(MgdDrawingService)

public:
    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructor
    /// </summary>
    virtual ~MgdDrawingService();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Construct an MgdDrawingService object
    /// </summary>
    MgdDrawingService();

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the DWF stream for a drawing specified by resource
    /// identifier.
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing
    /// source which has the DWF resource data.
    ///
    /// \return
    /// Returns an MgByteReader object containing the DWF.
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
    MgByteReader* GetDrawing(MgResourceIdentifier* resource);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the <c>manifest.xml</c> document which describes the
    /// supported document interfaces, the document properties, the
    /// sections and their contents, and section dependencies.
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing
    /// source which has the DWF resource data.
    ///
    /// \return
    /// Returns an MgByteReader object containing the manifest.xml
    /// file.
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
    MgByteReader* DescribeDrawing(MgResourceIdentifier* resource);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a DWF containing only the requested section (sometimes
    /// called a sheet).
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
    MgByteReader* GetSection(MgResourceIdentifier* resource, CREFSTRING sectionName);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a specific resource from the DWF.
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
    MgByteReader* GetSectionResource(MgResourceIdentifier* resource, CREFSTRING resourceName);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the names of the layers in a DWF section.
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
    MgStringCollection* EnumerateLayers(MgResourceIdentifier* resource, CREFSTRING sectionName);

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a layer from a particular section of a DWF.
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
    MgByteReader* GetLayer( MgResourceIdentifier* resource,
                            CREFSTRING sectionName,
                            CREFSTRING layerName );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates only the \link ePlot ePlot \endlink sections (sheets) in
    /// a DWF.
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
    MgByteReader* EnumerateSections(MgResourceIdentifier* resource);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates the resources of a DWF section (sometimes called a
    /// sheet).
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
    MgByteReader* EnumerateSectionResources(MgResourceIdentifier* resource, CREFSTRING sectionName);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system assigned to the DWF drawing.
    ///
    /// \remarks
    /// If no coordinate system is specified for the DWF, the default
    /// is <c>GEOGCS["LL84",DATUM["WGS_1984",SPHEROID["WGS
    /// 84",6378137,298.25722293287],TOWGS84[0,0,0,0,0,0,0]],PRIMEM["Greenwich",0],UNIT["Degrees",1]]</c>
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the drawing
    /// source which has the DWF resource data.
    ///
    /// \return
    /// Returns a string representing the coordinate system in OGC \link WKT WKT \endlink
    /// format.
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
    virtual STRING GetCoordinateSpace(MgResourceIdentifier* resource);

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_DrawingService_DrawingService;

protected:
    virtual INT32 GetClassId() { return m_cls_id; }

protected:
    virtual void Dispose() { delete this; }

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
/// \}
#endif