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

#include "MapGuideCommon.h"
#include "ProxyDrawingService.h"
#include "Command.h"

static const int Drawing_Service = (int)MgPacketParser::msiDrawing;


IMPLEMENT_CREATE_SERVICE(MgProxyDrawingService)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgProxyDrawingService object
/// </summary>
MgProxyDrawingService::MgProxyDrawingService() : MgDrawingService()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Self Destructor
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgProxyDrawingService::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the warnings if any
/// </summary>
void MgProxyDrawingService::SetWarning(MgWarnings* warning)
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
MgByteReader* MgProxyDrawingService::GetDrawing(MgResourceIdentifier* resource)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgDrawingServiceOpId::GetDrawing,            // Command Code
                       1,                                           // No of arguments
                       Drawing_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


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
MgByteReader* MgProxyDrawingService::DescribeDrawing(MgResourceIdentifier* resource)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgDrawingServiceOpId::DescribeDrawing,       // Command Code
                       1,                                           // No of arguments
                       Drawing_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


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
MgByteReader* MgProxyDrawingService::GetSection(MgResourceIdentifier* resource, CREFSTRING sectionName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgDrawingServiceOpId::GetSection,            // Command Code
                       2,                                           // No of arguments
                       Drawing_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &sectionName,           // Argument#2
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


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
MgByteReader* MgProxyDrawingService::GetSectionResource(MgResourceIdentifier* resource, CREFSTRING resourceName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgDrawingServiceOpId::GetSectionResource,    // Command Code
                       2,                                           // No of arguments
                       Drawing_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &resourceName,          // Argument#2
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// EnumerateLayers() returns the layer names in a DWF ePlot section.  An ePlot section is also known as a "sheet" in DWF viewers.
/// The ePlot section is specified by resource identifier (to get the DWF from the repository), and section name.
/// A list of all sections in a DWF can be retrieved from call to MgDrawingServices::EnumerateSections(),
/// or from the manifest.xml via call to MgDrawingServices::DescribeDrawing().
/// Refer to the DWF Format Specification at http://viewers/web/Olema/pdf/DWF%206%20Corporate%20Publishing%20Format.pdf for more information on the manifest and sections.
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
MgStringCollection* MgProxyDrawingService::EnumerateLayers(MgResourceIdentifier* resource, CREFSTRING sectionName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgDrawingServiceOpId::EnumerateLayers,       // Command Code
                       2,                                           // No of arguments
                       Drawing_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &sectionName,           // Argument#2
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgStringCollection*)cmd.GetReturnValue().val.m_obj;
}


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
MgByteReader* MgProxyDrawingService::GetLayer( MgResourceIdentifier* resource, CREFSTRING sectionName, CREFSTRING layerName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgDrawingServiceOpId::GetLayer,              // Command Code
                       3,                                           // No of arguments
                       Drawing_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &sectionName,           // Argument#2
                       MgCommand::knString, &layerName,             // Argument#2
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


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
MgByteReader* MgProxyDrawingService::EnumerateSections(MgResourceIdentifier* resource)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgDrawingServiceOpId::EnumerateSections,     // Command Code
                       1,                                           // No of arguments
                       Drawing_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


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
MgByteReader* MgProxyDrawingService::EnumerateSectionResources(MgResourceIdentifier* resource, CREFSTRING sectionName)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knObject,                         // Return type expected
                       MgDrawingServiceOpId::EnumerateSectionResources, // Command Code
                       2,                                           // No of arguments
                       Drawing_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knString, &sectionName,           // Argument#2
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


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
STRING MgProxyDrawingService::GetCoordinateSpace(MgResourceIdentifier* resource)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knString,                         // Return type expected
                       MgDrawingServiceOpId::GetCoordinateSpace,    // Command Code
                       1,                                           // No of arguments
                       Drawing_Service,                             // Service Id
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument#1
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    STRING retVal = *(cmd.GetReturnValue().val.m_str);
    delete cmd.GetReturnValue().val.m_str;
    return retVal;
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Sets the connection properties for the Proxy Service.  This
/// information tells the proxy object where to connect.
///
/// \param connProp
/// Connection properties for server
///
void MgProxyDrawingService::SetConnectionProperties(MgConnectionProperties* connProp)
{
    m_connProp = SAFE_ADDREF(connProp);
}
