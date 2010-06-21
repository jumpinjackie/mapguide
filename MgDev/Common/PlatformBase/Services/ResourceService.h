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

#ifndef MGRESOURCESERVICES_H_
#define MGRESOURCESERVICES_H_

#include "ResourceIdentifier.h"
#include "ResourceDefs.h"

class MgRepository;


//////////////////////////////////////////////////////////////////
/// \brief
/// Enables you to manipulate repositories and resources.
///
/// \remarks
/// Contains methods to:
///   <ul>
///      <li>get, add, move, copy, list, and delete resources</li>
///      <li>get, set, rename, list, and delete resource data</li>
///      <li>set permission for repositories and resources</li>
///   </ul>
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgResourceService : public MgService
{
INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgResourceService object.
    ///
    MgResourceService();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list of entries for the specified repository type.
    ///
    /// \remarks
    /// This method only works on "Session" repositories.
    /// \n
    /// If you specify a repository type that is not supported this method
    /// will throw an MgInvalidRepositoryTypeException exception.
    ///
    /// \param repositoryType
    /// The type of repository you want to list the entries for.
    ///
    /// \return
    /// MgByteReader object containing the list of session IDs.
    ///
    /// \exception MgOutOfMemoryException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual MgByteReader* EnumerateRepositories(CREFSTRING repositoryType) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a repository with the given name and type.
    ///
    /// \remarks
    /// This method only works on "Library" and "Session" repositories.
    /// If you specify a repository that is not supported this method
    /// will throw an MgInvalidRepositoryType exception.
    ///
    /// \param resource
    /// Resource identifier describing the repository to create
    /// The following repositories are supported:
    /// 1) Library
    /// 2) Session
    ///
    /// \param content
    /// Repository XML content document.
    /// \param header
    /// XML header definition.  May be null.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgOutOfMemoryException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual void CreateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Delete a repository with the given name and type.
    ///
    /// This method only works on "Library" and "Session" repositories.
    /// If you specify a repository that is not supported this method
    /// will throw an MgInvalidRepositoryType exception.
    ///
    /// \param resource
    /// Resource identifier describing the resource to delete
    /// The following repositories are supported:
    /// <ol>
    ///    <li>Library</li>
    ///    <li>Session</li>
    /// </ol>
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgOutOfMemoryException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual void DeleteRepository(MgResourceIdentifier* resource) = 0;

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Updates the \link header header \endlink and \link content content \endlink
    /// of the specified repository.
    ///
    /// \note1
    ///
    /// \remarks
    /// This method can only be used on a repository that already
    /// exists.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void UpdateRepository(MgResourceIdentifier resource, MgByteReader content, MgByteReader header);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void UpdateRepository(MgResourceIdentifier resource, MgByteReader content, MgByteReader header);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void UpdateRepository(MgResourceIdentifier resource, MgByteReader content, MgByteReader header);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Repository to update.
    /// \param content (MgByteReader)
    /// Repository content in XML format using the \link RepositoryContent_schema RepositoryContent \endlink
    /// schema.
    /// \n
    /// Or, if the
    /// repository is the library, this can be set to null
    /// in which case the content is not changed.
    /// \param header (MgByteReader)
    /// <ul>
    ///    <li>If the repository is the library, this is the
    ///    <repository header in XML format using the \link ResourceFolderHeader_schema ResourceFolderHeader_schema \endlink
    ///    schema. Or, this can be set to null in which case
    ///    the header is not changed.</li>
    ///    <li>If the repository is a session repository, this
    ///    must be null.</li>
    /// </ul>
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// This example updates the content and leaves the header
    /// unchanged:
    /// \code
    /// // Assuming $resourceService has already been initialized
    /// // Get a resource identifier for the library
    /// $resourceID = new MgResourceIdentifier('Library://');
    /// // Read the new content from a file
    /// $byteSource = new MgByteSource('C:\Data\Repository\Content.xml');
    /// // Put that MgByteSource into an MgByteReader
    /// $byteReader = $byteSource->GetReader();
    /// // Update the repository content
    /// $resourceService->UpdateRepository($resourceID, $byteReader, null);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgOutOfMemoryException
    /// \exception MgRepositoryNotFoundException if repository does not
    ///  exist
    /// \exception MgInvalidRepositoryTypeException
    ///
    /// \note
    /// <table border="1" class="RuledTable">
    ///    <tr>
    ///        <th>If you are updating</th>
    ///        <th>Content</th>
    ///        <th>Header</th>
    ///    </tr>
    ///    <tr>
    ///        <td>the library</td>
    ///        <td>Can be null as long as the header is not null.</td>
    ///        <td>Can be null as long as the content is not null.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>a session repository</td>
    ///        <td>Must not be null.</td>
    ///        <td>Must be null.</td>
    ///    </tr>
    /// </table>
    ///
    virtual void UpdateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Applies a package of resource changes to the repository.
    ///
    /// \note1
    ///
    /// \remarks
    /// This method only works on "Library" repository.
    /// Cross repository (Library:// and Session:a2c8e5ba//) changes in a single
    /// package are not supported.
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void ApplyResourcePackage(MgByteReader resourcePackage);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void ApplyResourcePackage(MgByteReader resourcePackage);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void ApplyResourcePackage(MgByteReader resourcePackage);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resourcePackage (MgByteReader)
    /// The package containing resources to update.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $resourceService is already initialized.
    /// $filepath = 'C:\Data\Packages\package.mgp';
    /// $byteSource = new MgByteSource($filepath);
    /// $byteReader = $byteSource->GetReader();
    /// $resourceService->ApplyResourcePackage($byteReader);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgOutOfMemoryException
    ///
    /// \note
    ///   <ul>
    ///      <li>ApplyResourcePackage is atomic. If a resource operation in the package fails to execute,
    ///          then all operations up to that point are rolled back.</li>
    ///      <li>This method only affects the \link library library \endlink repository.</li>
    ///   </ul>
    ///
    virtual void ApplyResourcePackage(MgByteReader* resourcePackage) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates the resources in the specified repository.
    ///
    /// \remarks
    /// You can enumerate all types or just a selected type. You can
    /// also choose what depth in the repository to examine.
    /// This method only works on "Library" repository.
    /// If you specify a repository that is not supported, this method
    /// will throw an MgInvalidRepositoryType exception.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader EnumerateResources(MgResourceIdentifier resource, int depth, string type);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader EnumerateResources(MgResourceIdentifier resource, int depth, String type);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader EnumerateResources(MgResourceIdentifier resource, int depth, string type);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the resource to
    /// enumerate. This can be a document or a folder.
    /// \n If it is a folder, you must include the trailing slash
    /// in the identifier.
    /// \param depth (int)
    /// Recursion depth, relative to the specified resource.
    /// <ul>
    ///     <li>If the resource is a document, depth must be set to
    ///          0.
    ///     </li>
    ///     <li>If the resource is a folder:
    ///     <ul>
    ///         <li>If the depth is equal to 0, only information about
    ///              the specified folder is returned.
    ///         </li>
    ///         <li>If the depth is greater than 0, information about
    ///              the folder and its descendants up to the specified
    ///              depth are returned.
    ///         </li>
    ///     </ul>
    ///     <li>If the depth is -1, information about the folder
    ///          and all its descendants is returned.
    ///     </li>
    /// </ul>
    /// \param type (String/string)
    /// Type of the resource to be enumerated. (Case
    /// sensitive.) See \link MgResourceType MgResourceType \endlink for valid
    /// types.
    /// \n
    /// Or, this can
    /// be set to null, in which case information about all
    /// resource types is returned.
    ///
    /// \return
    /// Returns an MgByteReader object containing a description of
    /// the resources in XML format using the \link ResourceList_schema ResourceList \endlink
    /// schema.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// These examples assume that <c>$resourceService</c> has
    /// already been initialized.
    /// \code
    /// // Enumerates everything in the library
    /// $resourceID = new MgResourceIdentifier("Library://");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, -1, "");
    ///
    /// // Enumerates everything in Geography
    /// $resourceID = new MgResourceIdentifier("Library://Geography/");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, -1, "");
    ///
    ///  // Enumerates all maps in the library
    /// $resourceID = new MgResourceIdentifier("Library://");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, -1, "MapDefinition");
    ///
    /// // Enumerates all folders in the library
    /// $resourceID = new MgResourceIdentifier("Library://");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, -1, "Folder");
    ///
    /// // Enumerates the folder Geography
    /// $resourceID = new MgResourceIdentifier("Library://Geography/");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, 0, "Folder");
    ///
    /// // Enumerates maps one level below Geography
    /// $resourceID = new MgResourceIdentifier("Library://Geography/");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, 1, "MapDefinition");
    ///
    /// // Enumerates a specific map
    /// // NOTE: In this case, depth must be set to 0
    /// $resourceID = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, 0, "MapDefinition");
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    MgByteReader* EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates the resources in the specified repository.
    ///
    /// \remarks
    /// You can enumerate all types or just a selected type. You can
    /// also choose what depth in the repository to examine.
    /// This method only works on "Library" repository.
    /// If you specify a repository that is not supported, this method
    /// will throw an MgInvalidRepositoryType exception.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader EnumerateResources(MgResourceIdentifier resource, int depth, string type, bool computeChildren);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader EnumerateResources(MgResourceIdentifier resource, int depth, String type, boolean computeChildren);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader EnumerateResources(MgResourceIdentifier resource, int depth, string type, bool computeChildren);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the resource to
    /// enumerate. This can be a document or a folder.
    /// \n If it is a folder, you must include the trailing slash
    /// in the identifier.
    /// \param depth (int)
    /// Recursion depth, relative to the specified resource.
    /// <ul>
    ///     <li>If the resource is a document, depth must be set to
    ///          0.
    ///     </li>
    ///     <li>If the resource is a folder:
    ///     <ul>
    ///         <li>If the depth is equal to 0, only information about
    ///              the specified folder is returned.
    ///         </li>
    ///         <li>If the depth is greater than 0, information about
    ///              the folder and its descendants up to the specified
    ///              depth are returned.
    ///         </li>
    ///     </ul>
    ///     <li>If the depth is -1, information about the folder
    ///          and all its descendants is returned.
    ///     </li>
    /// </ul>
    /// \param type (String/string)
    /// Type of the resource to be enumerated. (Case
    /// sensitive.) See \link MgResourceType MgResourceType \endlink for valid
    /// types.
    /// \n
    /// Or, this can
    /// be set to null, in which case information about all
    /// resource types is returned.
    /// \param computeChildren (boolean/bool)
    /// Flag to determine whether or not the number of children of the leaf folder
    /// resource at the specified depth should be computed.
    /// <ul>
    ///     <li>If it is true, then the number of children of the leaf folder
    ///          resource at the specified depth will be set to a computed value (>= 0).
    ///     </li>
    ///     <li>If it is false, then the number of children of the leaf folder
    ///          resource at the specified depth will be set to -1.
    ///     </li>
    /// </ul>
    ///
    /// \return
    /// Returns an MgByteReader object containing a description of
    /// the resources in XML format using the \link ResourceList_schema ResourceList \endlink
    /// schema.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// These examples assume that <c>$resourceService</c> has
    /// already been initialized.
    /// \code
    /// // Enumerates everything in the library
    /// $resourceID = new MgResourceIdentifier("Library://");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, -1, "", true);
    ///
    /// // Enumerates everything in Geography
    /// $resourceID = new MgResourceIdentifier("Library://Geography/");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, -1, "", true);
    ///
    ///  // Enumerates all maps in the library
    /// $resourceID = new MgResourceIdentifier("Library://");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, -1, "MapDefinition", false);
    ///
    /// // Enumerates all folders in the library
    /// $resourceID = new MgResourceIdentifier("Library://");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, -1, "Folder", true);
    ///
    /// // Enumerates the folder Geography
    /// $resourceID = new MgResourceIdentifier("Library://Geography/");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, 0, "Folder", true);
    ///
    /// // Enumerates maps one level below Geography
    /// $resourceID = new MgResourceIdentifier("Library://Geography/");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, 1, "MapDefinition", false);
    ///
    /// // Enumerates a specific map
    /// // NOTE: In this case, depth must be set to 0
    /// $resourceID = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $byteReader = $resourceService->EnumerateResources($resourceID, 0, "MapDefinition", false);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    MgByteReader* EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, bool computeChildren);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds a new resource to a resource repository, or updates an
    /// existing resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetResource(MgResourceIdentifier resource, MgByteReader content, MgByteReader header);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetResource(MgResourceIdentifier resource, MgByteReader content, MgByteReader header);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetResource(MgResourceIdentifier resource, MgByteReader content, MgByteReader header);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Location where the resource will be placed in the
    /// repository.
    /// \n
    /// The extension of
    /// the location must match one of the types defined in \link MgResourceType MgResourceType \endlink.
    /// It is case sensitive.
    /// \n
    /// \ProdName will automatically create any folders
    /// required.
    ///
    /// \param content (MgByteReader)
    /// The resource content in XML format using the schema
    /// appropriate for the resource type. See \link XML_Schemas_Module XML Schemas \endlink.\n Or
    /// when updating an existing resource, this can be set to
    /// null, in which case the content is not changed.
    ///
    /// \param header (MgByteReader)
    /// The resource \link header header \endlink in XML
    /// format. This uses \link ResourceFolderHeader_schema ResourceFolderHeader_schema \endlink
    /// for folder resources and \link ResourceDocumentHeader_schema ResourceDocumentHeader \endlink
    /// for all other resources.
    /// \n
    /// Or,
    /// if you are updating an existing resource in the
    /// library, this can be set to null, in which case the
    /// permissions are set to be inherited from the parent
    /// folder.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// To add a resource:
    /// \n
    /// 1) Prepare the resource's content XML file using the schema
    /// appropriate to the type.
    /// \n
    /// a) If the resource references other resources, use the <c>\<uri></c>
    /// elements to specify their paths in the repository. For
    /// example a map file, called World.MapDefinition, might look
    /// like this:
    /// \code
    /// <?xml version="1.0" encoding="UTF-8"?>
    /// <MapDefinition version="1.0.0">
    ///   <Name>World</Name>
    ///   <Layers>
    ///     <Layer>
    ///       <Name>Cities</Name>
    ///       <ResourceId>Library://World/Layers/Cities.Layer</ResourceId>
    ///     </Layer>
    ///     <Layer>
    ///       <Name>Countries</Name>
    ///       <ResourceId>Library://World/Layers/Countries.Layer</ResourceId>
    ///     </Layer>
    ///   </Layers>
    /// </MapDefinition>
    /// \endcode
    ///
    /// b) If the resource uses resource data, specify the path with
    /// \n
    /// <c>\%MG_DATA_FILE_PATH\%<i>data_name</i></c>
    /// \n
    /// where <i><c>data_name</c></i> must match the name given to
    /// the associated data in step 5. It does not need to match the
    /// physical file name. The <c>\%MG_DATA_FILE_PATH\%</c> keyword
    /// specifies that the binary file, in this case a DWF, will be
    /// stored in the same folder as the XML file. For example, the
    /// content for a drawing source might look like this:
    /// \code
    /// <?xml version="1.0" encoding="UTF-8"?>
    /// <DrawingSource xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    ///  xsi:noNamespaceSchemaLocation="DrawingSource-1.0.0.xsd">
    ///     <SourceName>\%MG_DATA_FILE_PATH\%test.dwf</SourceName>
    ///     <Password/>
    ///     <CoordinateSpace>LL84</CoordinateSpace>
    /// </DrawingSource>
    /// \endcode
    ///
    /// For more information on \%MG_DATA_FILE_PATH\% and other
    /// substitution tags, see MgResourceTag.
    /// \n
    /// 2) Prepare the resource's header XML file using the
    /// \link ResourceDocumentHeader_schema ResourceDocumentHeader \endlink schema.
    /// \code
    /// <?xml version="1.0" encoding="UTF-8"?>
    /// <ResourceDocumentHeader xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    /// xsi:noNamespaceSchemaLocation="ResourceDocumentHeader-1.0.0.xsd">
    ///     <Security>
    ///         <Inherited>true</Inherited>
    ///     </Security>
    ///     <Dependency>
    ///         <ResourceId>Library://World/Layers/Cities.LoadProcedure</ResourceId>
    ///     </Dependency>
    /// </ResourceDocumentHeader>
    /// \endcode
    /// \n
    /// 3) Add the resource to the repository using those content and
    /// header files:
    /// \code
    /// $resourceID= new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    ///
    /// $content  = "C:\Data\Maps\World.MapDefinition";
    /// $content_byteSource = new MgByteSource($content);
    /// $content_byteSource->setMimeType("text/xml");
    /// $content_byteReader = $content_byteSource->GetReader();
    ///
    /// $header   = "C:\Data\Maps\Resource\DocumentHeader.xml";
    /// $header_byteSource = new MgByteSource($header);
    /// $header_byteSource->setMimeType("text/xml");
    /// $header_byteReader = $header_byteSource->GetReader();
    ///
    /// // Assuming $resourceService is already initialized.
    /// $resourceService->SetResource($resourceID, $content_byteReader, $header_byteReader);
    /// \endcode
    /// \n
    /// 4) If the resource references other resources, you must also
    /// add them. In the World.MapDefinition example above, you would
    /// add the XML file for Cities.Layer to the repository at the
    /// location <c>Library://World/Layers/Cities.Layer</c>.
    /// Similarly, you would add Countries.Layer to <c>Library://World/Layers/Countries.Layer</c>.
    /// \n
    /// 5) Set any necessary resource data.
    /// See SetResourceData.
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidResourceTypeException
    ///
    /// \note
    /// You must be logged in as the Administrator or the resource's
    /// current \link owner owner \endlink to change the header.
    ///
    /// \todo
    /// [[replace example, xref to Dev Guide]]
    ///
    /// \see
    /// GetResourceContent
    /// \see
    /// GetResourceHeader
    ///
    virtual void SetResource(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deletes an existing resource and its associated data from the
    /// repository.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void DeleteResource(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void DeleteResource(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void DeleteResource(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the resource to
    /// delete. This can be a document or a folder.\n If
    /// it is a folder, this method deletes that folder, all
    /// resources in the folder, and any subfolders below it.\n If
    /// it is a folder, you must include the trailing slash
    /// in the identifier.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// This example deletes a single resource:
    /// \code
    /// // Assuming $resourceService is already initialized
    /// $resourceID = new MgResourceIdentifier("Library://Maps/World.MapDefinition");
    /// $resourceService->DeleteResource($resourceID);
    /// \endcode
    /// \n
    /// This example deletes a folder and all items beneath it:
    /// \code
    /// $resourceID = new MgResourceIdentifier("Library://Maps/");
    /// $resourceService->DeleteResource($resourceID);
    /// \endcode
    /// \n
    /// This deletes all the resources in the library:
    /// \code
    /// $resourceID = new MgResourceIdentifier("Library://");
    /// $resourceService->DeleteResource($resourceID);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidResourceTypeException
    ///
    /// \note
    /// The API generates an exception if you try to delete a
    /// non-existent resource.
    ///
    virtual void DeleteResource(MgResourceIdentifier* resource) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Moves an existing resource to another location.
    ///
    /// \remarks
    /// You can also use this method to rename a resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void MoveResource(MgResourceIdentifier sourceResource, MgResourceIdentifier destResource, bool overwrite);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void MoveResource(MgResourceIdentifier sourceResource, MgResourceIdentifier destResource, boolean overwrite);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void MoveResource(MgResourceIdentifier sourceResource, MgResourceIdentifier destResource, bool overwrite);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param sourceResource (MgResourceIdentifier)
    /// Resource to be moved. This can be a document
    /// or folder.
    /// <ul>
    ///    <li>If it is a folder, this method also moves the contents of the folder and all folders below it.</li>
    ///    <li>If it is a folder, you must include the trailing slash in the identifier.</li>
    /// </ul>
    /// \param destResource (MgResourceIdentifier)
    /// Where the resource should be moved to.
    /// \param overwrite (boolean/bool)
    /// Flag to determine whether or not the
    /// destination resource should be overwritten if
    /// it exists.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// This example moves the file
    /// Library://Geography/World.MapDefinition to
    /// Library://Atlas/Oceans.MapDefinition:
    /// \code
    /// // Assuming $resourceService has already been initialized
    /// $oldPath = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $newPath = new MgResourceIdentifier("Library://Atlas/Oceans.MapDefinition");
    /// $resourceService->MoveResource($oldPath, $newPath, true);
    /// \endcode
    ///
    /// This example moves the folder Library://Geography/ to
    /// Library://World Geography/:
    /// \code
    /// $oldPath = new MgResourceIdentifier("Library://Geography/");
    /// $newPath = new MgResourceIdentifier("Library://World Geography/");
    /// $resourceService->MoveResource($oldPath, $newPath, true);
    /// \endcode
    ///
    /// This example renames Oceans.MapDefinition to Pacific
    /// Ocean.MapDefinition:
    /// \code
    /// /**************************************************************************/
    /// $oldPath = new MgResourceIdentifier("Library://Atlas/Oceans.MapDefinition");
    /// $newPath = new MgResourceIdentifier("Library://Atlas/Pacific Ocean.MapDefinition");
    /// $resourceService->MoveResource($oldPath, $newPath, true);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgResourceNotFoundException
    /// \exception MgDuplicateResourceException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    /// \note
    /// When copying a folder with the "overwrite" flag turned on, if
    /// the destination folder already exists, then only the children
    /// in the destination folder that have the same names as those
    /// in the source folder are overwritten. The rest should are
    /// left intact.
    ///
    virtual void MoveResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Moves an existing resource to another location.
    ///
    /// \remarks
    /// You can also use this method to rename a resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void MoveResource(MgResourceIdentifier sourceResource, MgResourceIdentifier destResource, bool overwrite, bool cascade);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void MoveResource(MgResourceIdentifier sourceResource, MgResourceIdentifier destResource, boolean overwrite, boolean cascade);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void MoveResource(MgResourceIdentifier sourceResource, MgResourceIdentifier destResource, bool overwrite, bool cascade);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param sourceResource (MgResourceIdentifier)
    /// Resource to be moved. This can be a document
    /// or folder.
    /// <ul>
    ///    <li>If it is a folder, this method also moves the contents of the folder and all folders below it.</li>
    ///    <li>If it is a folder, you must include the trailing slash in the identifier.</li>
    /// </ul>
    /// \param destResource (MgResourceIdentifier)
    /// Where the resource should be moved to.
    /// \param overwrite (boolean/bool)
    /// Flag to determine whether or not the
    /// destination resource should be overwritten if
    /// it exists.
    /// \param cascade (boolean/bool)
    /// Flag to determine whether or not the
    /// referencing resources should be updated.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// This example moves the file
    /// Library://Geography/World.MapDefinition to
    /// Library://Atlas/Oceans.MapDefinition:
    /// \code
    /// // Assuming $resourceService has already been initialized
    /// $oldPath = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $newPath = new MgResourceIdentifier("Library://Atlas/Oceans.MapDefinition");
    /// $resourceService->MoveResource($oldPath, $newPath, true, true);
    /// \endcode
    ///
    /// This example moves the folder Library://Geography/ to
    /// Library://World Geography/:
    /// \code
    /// $oldPath = new MgResourceIdentifier("Library://Geography/");
    /// $newPath = new MgResourceIdentifier("Library://World Geography/");
    /// $resourceService->MoveResource($oldPath, $newPath, true, true);
    /// \endcode
    ///
    /// This example renames Oceans.MapDefinition to Pacific
    /// Ocean.MapDefinition:
    /// \code
    /// /**************************************************************************/
    /// $oldPath = new MgResourceIdentifier("Library://Atlas/Oceans.MapDefinition");
    /// $newPath = new MgResourceIdentifier("Library://Atlas/Pacific Ocean.MapDefinition");
    /// $resourceService->MoveResource($oldPath, $newPath, true, false);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgResourceNotFoundException
    /// \exception MgDuplicateResourceException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    /// \note
    /// When copying a folder with the "overwrite" flag turned on, if
    /// the destination folder already exists, then only the children
    /// in the destination folder that have the same names as those
    /// in the source folder are overwritten. The rest should are
    /// left intact.
    ///
    virtual void MoveResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite, bool cascade);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Copies an existing resource to another location.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void CopyResource(MgResourceIdentifier sourceResource, MgResourceIdentifier destResource, bool overwrite);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void CopyResource(MgResourceIdentifier sourceResource, MgResourceIdentifier destResource, boolean overwrite);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void CopyResource(MgResourceIdentifier sourceResource, MgResourceIdentifier destResource, bool overwrite);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param sourceResource (MgResourceIdentifier)
    /// Resource to be copied from. This can be a
    /// document or folder.
    /// \n
    /// If
    /// it is a folder, this method also copies the
    /// contents of the folder and all folders below
    /// it. You must include the trailing slash in
    /// the identifier.
    ///
    /// \param destResource (MgResourceIdentifier)
    /// Where the resource should be copied to.
    /// \param overwrite (boolean/bool)
    /// Flag to determine whether or not the
    /// destination resource should be overwritten if
    /// it exists.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// This example copies the file
    /// Library://Geography/World.MapDefinition to
    /// Library://Atlas/World Geography.MapDefinition:
    /// \code
    /// // Assuming $resourceService is already initialized.
    /// $source_resourceID= new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $destination_resourceID= new MgResourceIdentifier("Library://Atlas/World Geography.MapDefinition");
    /// $resourceService->CopyResource($source_resourceID, $destination_resourceID, true);
    /// \endcode
    ///
    /// This example copies the folder Library://Geography/ to
    /// Library://World/Oceans/:
    /// \code
    /// $source_resourceID= new MgResourceIdentifier("Library://Geography/");
    /// $destination_resourceID= new MgResourceIdentifier("Library://World/Oceans/");
    /// $resourceService->CopyResource($source_resourceID, $destination_resourceID, true);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgResourceNotFoundException
    /// \exception MgDuplicateResourceException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    /// \note
    /// When copying a folder with the "overwrite" flag turned on, if
    /// the destination folder already exists, then only the children
    /// in the destination folder that have the same names as those
    /// in the source folder are overwritten. The rest should are
    /// left intact.
    ///
    virtual void CopyResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite) = 0;

    //////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the content of the specified resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader GetResourceContent(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader GetResourceContent(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader GetResourceContent(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying
    /// the resource.
    ///
    /// \return
    /// Returns an MgByteReader object containing the resource
    /// content in XML format. The XML uses the schema appropriate
    /// for the resource type. See \link XML_Schemas_Module XML Schemas \endlink.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $resourceService has already been initialized
    /// $resourceID = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $byteReader = $resourceService->GetResourceContent($resourceID);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    /// \see
    /// SetResource
    ///
    MgByteReader* GetResourceContent(MgResourceIdentifier* resource);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the contents of the specified resources.
    ///
    /// \param resources
    /// A collection of resource identifiers describing the resources.
    /// \param preProcessTags
    /// Pre-processing to apply to resource before returning content.
    /// If this parameter is NULL, it means no pre-processing.
    /// If this parameter is not NULL, it must be a collection of pre-processing types, and it should be
    /// one to one matching the collection of parameter resources.
    /// See MgResourcePreProcessingType for a list of supported pre-processing tags.
    ///
    /// \return
    /// MgStringCollection object representing the collection of corresponding resource contents.
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgStringCollection* GetResourceContents(MgStringCollection* resources,
        MgStringCollection* preProcessTags);


    //////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the \link header header \endlink associated with
    /// the specified resource.
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetResourceHeader(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetResourceHeader(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetResourceHeader(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource to get the header
    /// for.
    ///
    /// \return
    /// Returns an MgByteReader object containing the resource
    /// header in XML format using the \link ResourceDocumentHeader_schema ResourceDocumentHeader \endlink
    /// schema.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $resourceService has already been initialized
    /// $resourceID = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $byteReader = $resourceService->GetResourceHeader($resourceID);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidResourceTypeException
    ///
    /// \see
    /// SetResource
    ///
    virtual MgByteReader* GetResourceHeader(MgResourceIdentifier* resource) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Changes the \link owner owner \endlink of a resource.
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void ChangeResourceOwner(MgResourceIdentifier resource, string owner, bool includeDescendants);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void ChangeResourceOwner(MgResourceIdentifier resource, String owner, boolean includeDescendants);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void ChangeResourceOwner(MgResourceIdentifier resource, string owner, bool includeDescendants);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource whose owner is to be changed. This
    /// resource can be a document or folder.\n If
    /// it is a folder, you must include the trailing
    /// slash in the identifier.
    /// \param owner (String/string)
    /// The User ID of the new owner. (The User ID must
    /// already exist. You can create User IDs through
    /// the Site Administrator tool.)
    /// \param includeDescendants (boolean/bool)
    /// Flag to determine whether or not the owners of
    /// descendant resources should be also be changed.
    /// This flag is applicable only if the resource is
    /// a folder.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $resourceService is already initialized.
    /// $resourceID= new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $resourceService->ChangeResourceOwner($resourceID, "John Smith", false);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgPermissionDeniedException
    /// \exception MgResourceNotFoundException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    /// \note
    ///   <ul>
    ///      <li>You must be logged in as the Administrator or the current owner to use this method.</li>
    ///      <li>You can use \link MgResourceService::EnumerateResources EnumerateResources \endlink to determine the current owner of a resource.</li>
    ///   </ul>
    ///
    ///
    virtual void ChangeResourceOwner(MgResourceIdentifier* resource,
        CREFSTRING owner, bool includeDescendants) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the permissions for all descendants of a specified
    /// folder to be inherited from the folder.
    ///
    /// \note1
    ///
    /// \remarks
    /// For example, if the folder "Library://Geography/" contains a
    /// map and a subfolder containing two feature sources, using
    /// this will set the map, the subfolder, and the feature sources
    /// all to "inherit".
    /// \n
    /// This method only works on items in the library repository,
    /// not in session repositories.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void InheritPermissionsFrom(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void InheritPermissionsFrom(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void InheritPermissionsFrom(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Folder that permissions are to be inherited
    /// from. You must include the trailing slash in
    /// the identifier.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $resourceService is already initialized.
    /// $resourceID= new MgResourceIdentifier("Library://Geography/");
    /// $resourceService->InheritPermissionsFrom($resourceID);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgPermissionDeniedException
    /// \exception MgResourceNotFoundException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    /// \note
    /// You must be logged in as the Administrator or the current \link owner owner \endlink
    /// to use this method.
    ///
    virtual void InheritPermissionsFrom(MgResourceIdentifier* resource) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds \link resource_data resource data \endlink to the
    /// specified resource.
    ///
    /// \remarks
    /// This uploads the data and stores it in the repository. It
    /// does not perform any necessary conversion (for example,
    /// converting a version 2 SDF file into a version 3 SDF).
    ///
    /// \note
    /// If you load raster files though the MapGuide Studio UI, they are converted to .TIF format.  However, raster files loaded
    /// through the API are not converted, so the API supports more file formats than the UI.  The API supports
    /// all the formats supported by the FDO Provider for Raster.  For details, see the appendix "Autodesk FDO Provider for Raster"
    /// in the <em>FDO Developer's Guide</em>.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetResourceData(MgResourceIdentifier resource, string dataName, string dataType, MgByteReader data);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetResourceData(MgResourceIdentifier resource, String dataName, String dataType, MgByteReader data);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetResourceData(MgResourceIdentifier resource, string dataName, string dataType, MgByteReader data);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource to which this data is to be associated.
    /// \param dataName (String/string)
    /// Name for the data.
    /// \n
    /// This
    /// name must match an element name within the XML
    /// content document. If the data name is <c>UserCredentials</c>,
    /// then MgResourceTag::Username and
    /// MgResourceTag::Password can be inserted into the
    /// XML content document to represent provider specific
    /// username and/or password.
    /// \n
    /// If
    /// resource data with the same name has already been
    /// set for this resource, the API will overwrite the
    /// old setting. No exception is generated. The data
    /// name may be any valid UTF-8 string
    ///
    /// \param dataType (String/string)
    /// Data type for the resource data. It must match one
    /// of the types defined in MgResourceDataType and is
    /// case sensitive. Data names must be unique.
    ///
    /// \param data (MgByteReader)
    /// Data to set.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Examples (PHP) -->
    /// \htmlinclude PHPExamplesTop.html
    /// This example sets resource data and stores it as a file:
    /// \code
    /// // Assuming that $resourceService has already been initialized
    /// // Assuming that 'Library://Geography/Calgary points of interest.FeatureSource' has already been added to the repository
    /// $resourceID = new MgResourceIdentifier('Library://Geography/Calgary points of interest.FeatureSource');
    /// $byteSource = new MgByteSource('C:\Data\Calgary\Data\Calgary_points.sdf');
    /// $data = $byteSource->GetReader();
    /// $resourceService->SetResourceData($resourceID, 'locations of points of interest', 'File', $data);
    /// \endcode
    ///
    /// This example sets resource data and stores it as a stream:
    /// \code
    /// // Assuming that 'Library://Geography/Calgary symbols.SymbolSet' has already been added to the repository
    /// $resourceID = new MgResourceIdentifier('Library://Geography/Trees.SymbolLibrary');
    /// $byteSource = new MgByteSource('C:\Data\Calgary\Data\markers.dwf');
    /// $data = $byteSource->GetReader();
    /// $resourceService->SetResourceData($resourceID, 'symbols for points of interest', 'Stream', $data);
    /// \endcode
    ///
    /// This example sets the embedded credentials for a resource and stores it as a string.  The string data type must be used for
    /// embedded credentials.  The data name must be MgResourceDataName::UserCredentials.
    /// \code
    /// // Assuming that 'Library://Geography/database connnection.FeatureSource' has already been added to the repository
    /// $resourceID = new MgResourceIdentifier('Library://Geography/database connnection.FeatureSource');
    /// $byteSource = new MgByteSource('C:\Data\Calgary\Data\credentials.txt');
    /// $data = $byteSource->GetReader();
    /// $dataName = new MgResourceDataName();
    /// $resourceService->SetResourceData($resourceID, MgResourceDataName::UserCredentials, 'String', $data);
    /// \endcode
    ///
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidResourceTypeException if resource is a folder type
    /// \exception MgInvalidResourceDataTypeException if the specified
    ///  dataType is not in \link MgResourceDataType MgResourceDataType \endlink
    ///
    /// \todo
    /// [[When MgResourceDataType is implemented, I should use that
    /// to define the types in the examples. EG
    /// $type = new MgResourceDataType;
    /// $resourceService->SetResourceData($resourceID, MgResourceDataName::UserCredentials, $type->String, $data);]]
    ///
    /// \see
    /// EnumerateResourceData
    /// \see
    /// GetResourceData
    /// \see
    /// RenameResourceData
    /// \see
    /// DeleteResourceData
    ///
    virtual void SetResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deletes \link resource_data resource data \endlink from
    /// the specified resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void DeleteResourceData(MgResourceIdentifier resource, string dataName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void DeleteResourceData(MgResourceIdentifier resource, String dataName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void DeleteResourceData(MgResourceIdentifier resource, string dataName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// The resource for which the resource data
    /// will be deleted.
    /// \param dataName (String/string)
    /// Name for data.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming that $resourceService has already been initialized
    /// $resourceID = new MgResourceIdentifier("Library://Geography/Calgary points of interest.FeatureSource");
    /// $resourceService->DeleteResourceData($resourceID, "locations of historical sites");
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidResourceTypeException
    ///
    /// \see
    /// SetResourceData
    /// \see
    /// EnumerateResourceData
    /// \see
    /// GetResourceData
    /// \see
    /// RenameResourceData
    ///
    virtual void DeleteResourceData(MgResourceIdentifier* resource, CREFSTRING dataName) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Renames \link resource_data resource data \endlink for
    /// the specified resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void RenameResourceData(MgResourceIdentifier resource, string oldDataName, string newDataName, bool overwrite);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void RenameResourceData(MgResourceIdentifier resource, String oldDataName, String newDataName, boolean overwrite);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void RenameResourceData(MgResourceIdentifier resource, string oldDataName, string newDataName, bool overwrite);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// The resource for which the resource
    /// data will be renamed.
    /// \param oldDataName (String/string)
    /// Old data name.
    /// \param newDataName (String/string)
    /// New data name.
    /// \param overwrite (boolean/bool)
    /// Flag to determine whether or not
    /// the destination resource data
    /// should be overwritten if it exists.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming that $resourceService has already been initialized
    /// $resourceID = new MgResourceIdentifier("Library://Geography/Calgary points of interest.FeatureSource");
    /// $resourceService->RenameResourceData($resourceID, "locations of points of interest", "locations of historical sites", true);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgInvalidResourceDataTypeException
    /// \exception MgResourceDataNotFoundException
    /// \exception MgDuplicateResourceDataException
    ///
    /// \note
    /// In a feature source, this method does not rename anything in the connection properties, since \ProdName cannot tell
    /// if the connection properties should be changed.  You may need
    /// to recreate the feature source or edit the XML for the feature source.
    ///
    /// \see
    /// SetResourceData
    /// \see
    /// EnumerateResourceData
    /// \see
    /// GetResourceData
    /// \see
    /// DeleteResourceData
    ///
    ///
    virtual void RenameResourceData(MgResourceIdentifier* resource,
        CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the \link resource_data resource data \endlink
    /// for the specified resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader GetResourceData(MgResourceIdentifier resource, string dataName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader GetResourceData(MgResourceIdentifier resource, String dataName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader GetResourceData(MgResourceIdentifier resource, string dataName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// The resource for which the resource data will
    /// be obtained.
    /// \param dataName (String/string)
    /// Name for data (as specified when the data was
    /// set via \link MgResourceService::SetResourceData SetResourceData \endlink).
    ///
    /// \return
    /// Returns an MgByteReader object containing the resource data.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming that $resourceService has already been initialized
    /// $resourceID = new MgResourceIdentifier("Library://Geography/Calgary points of interest.FeatureSource");
    /// $byteReader = $resourceService->GetResourceData($resourceID, "locations of points of interest");
    /// $byteSink = new MgByteSink($byteReader);
    /// $byteSink->ToFile('C:\temp\points.sdf');
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgResourceDataNotFoundException
    /// \exception MgInvalidResourceTypeException
    ///
    /// \see
    /// SetResourceData
    /// \see
    /// EnumerateResourceData
    /// \see
    /// RenameResourceData
    /// \see
    /// DeleteResourceData
    ///
    virtual MgByteReader* GetResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates the \link resource_data resource data \endlink
    /// for the specified resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader EnumerateResourceData(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader EnumerateResourceData(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader EnumerateResourceData(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// The resource for which the resource
    /// data will be listed.
    ///
    /// \return
    /// Returns an MgByteReader object representing the description
    /// of the resource data in XML format using the \link ResourceDataList_schema ResourceDataList \endlink
    /// schema.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming that $resourceService has already been initialized
    /// $resourceID = new MgResourceIdentifier("Library://Geography/Calgary points of interest.FeatureSource");
    /// $byteReader = $resourceService->EnumerateResourceData($resourceID);
    /// echo $byteReader->ToString();
    /// /* Returns for example:
    /// <?xml version="1.0" encoding="UTF-8"?>
    /// <ResourceDataList xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="ResourceDataList-1.0.0.xsd">
    /// <ResourceData>
    ///     <Name>locations of points of interest</Name>
    ///     <Type>File</Type>
    /// </ResourceData>
    /// </ResourceDataList>
    /// */
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidResourceTypeException
    ///
    /// \see
    /// SetResourceData
    /// \see
    /// GetResourceData
    /// \see
    /// RenameResourceData
    /// \see
    /// DeleteResourceData
    ///
    virtual MgByteReader* EnumerateResourceData(MgResourceIdentifier* resource) = 0;

    //////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the \link content content \endlink for the
    /// specified repository.
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetRepositoryContent(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetRepositoryContent(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetRepositoryContent(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Repository to get the content
    /// for.
    ///
    /// \return
    /// Returns an MgByteReader object containing the repository
    /// content in XML format using the \link RepositoryContent_schema RepositoryContent \endlink
    /// schema.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $resourceService has already been initialized.
    /// $resourceID = new MgResourceIdentifier("Library://");
    /// $byteReader = $resourceService->GetRepositoryContent($resourceID);
    /// echo $byteReader->ToString();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgRepositoryNotFoundException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual MgByteReader* GetRepositoryContent(MgResourceIdentifier* resource) = 0;

    /////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the \link header header \endlink of the specified
    /// repository.
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetRepositoryHeader(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetRepositoryHeader(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetRepositoryHeader(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Repository to get the header
    /// for.
    ///
    /// \return
    /// Returns an MgByteReader object containing the repository
    /// header in XML format using the \link ResourceFolderHeader_schema ResourceFolderHeader_schema \endlink
    /// schema.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $resourceService has already been initialized.
    /// $resourceID = new MgResourceIdentifier("Library://");
    /// $byteReader = $resourceService->GetRepositoryHeader($resourceID);
    /// echo $byteReader->ToString();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgRepositoryNotFoundException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual MgByteReader* GetRepositoryHeader(MgResourceIdentifier* resource) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates all the resources which reference the specified
    /// resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader EnumerateReferences(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader EnumerateReferences(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader EnumerateReferences(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource to get references
    /// for.
    ///
    /// \return
    /// Returns an MgByteReader object containing the list of all
    /// resources which reference the specified resource. The list is
    /// in XML format using the \link ResourceReferenceList_schema ResourceReferenceList \endlink
    /// schema.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $resourceService is already initialized.
    /// $resourceID= new MgResourceIdentifier('Library://Samples/Sheboygan/Layers/BuildingOutlines.LayerDefinition');
    /// $byteReader = $resourceService->EnumerateReferences($resourceID);
    /// echo $byteReader->ToString();
    ///
    /// /* Returns for example:
    /// <?xml version="1.0" encoding="UTF-8"?>
    /// <ResourceReferenceList xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="ResourceReferenceList-1.0.0.xsd">
    ///     <ResourceId>Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition</ResourceId>
    /// </ResourceReferenceList>
    /// */
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgByteReader* EnumerateReferences(MgResourceIdentifier* resource) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks to see if the specified resource exists.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool ResourceExists(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual bool ResourceExists(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool ResourceExists(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource identifier specifying the resource to be checked for existence.
    ///
    /// \return
    /// Returns true if the resource exists, false otherwise.
    ///
    virtual bool ResourceExists(MgResourceIdentifier* resource) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerate unmanaged data
    ///
    /// \note1
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader EnumerateUnmanagedData(string path, bool recursive, string type, string filter);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader EnumerateUnmanagedData(String path, boolean recursive, String type, String filter);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader EnumerateUnmanagedData(string path, bool recursive, string type, string filter);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param path (String/string)
    /// Unmanaged data path
    /// \param recursive (boolean/bool)
    /// Flag to determine whether to recurse subfolders or not
    /// \param type (String/string)
    /// Folders, Files, or Both
    /// \param filter (String/string)
    /// file filter to be used on returned files
    ///
    /// \return
    /// Returns an MgByteReader object containing the unmanaged data
    /// in XML format using the \link UnmanagedDataList_schema UnmanagedDataList_schema \endlink
    ///
    virtual MgByteReader* EnumerateUnmanagedData(CREFSTRING path, bool recursive, CREFSTRING type, CREFSTRING filter);

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates the resources in the specified repository.
    ///
    /// \remarks
    /// Resources of all types can be enumerated all at once, or only
    /// resources of a given type.
    ///
    /// \param resource
    /// Resource identifier specifying the resource to
    /// enumerate. This can be a document or a folder.
    /// \n If it is a folder, you must include the trailing slash
    /// in the identifier.
    /// \param depth
    /// Recursion depth.
    /// If the depth is greater than 0, the specified resource's descendants
    /// (up to the specified level of depth) are returned.
    /// If the depth is equal to 0, the specified resource is returned.
    /// If the depth is less than 0, the specified resource and its descendants
    /// (no limit in depth) are returned.
    /// \param type
    /// Type of the resource to be enumerated.  See MgResourceType for valid types.
    /// If the type is not specified, resources of all types are returned.
    /// \param properties
    /// Flag to specify which properties of the resource header should be filtered.
    /// It may be set to 0 or by bitwise inclusively OR'ing together one or
    /// more of the MgResourceHeaderProperties values.
    /// \param fromDate
    /// Minimum value of the resource's modified date.
    /// \param toDate
    /// Maximum value of the resource's modified date.
    /// \param computeChildren (boolean/bool)
    /// Flag to determine whether or not the number of children of the leaf folder
    /// resource at the specified depth should be computed.
    /// <ul>
    ///     <li>If it is true, then the number of children of the leaf folder
    ///          resource at the specified depth will be set to a computed value (>= 0).
    ///     </li>
    ///     <li>If it is false, then the number of children of the leaf folder
    ///          resource at the specified depth will be set to -1.
    ///     </li>
    /// </ul>
    ///
    /// \return
    /// MgByteReader object representing the description of the resources.
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgByteReader* EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, INT32 properties,
        CREFSTRING fromDate, CREFSTRING toDate);

    virtual MgByteReader* EnumerateResources(MgResourceIdentifier* resource,
        INT32 depth, CREFSTRING type, INT32 properties,
        CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerate the resource documents in the specified repository.
    ///
    /// \remarks
    /// This method only works on "Library".
    ///
    /// \param resources
    /// Resource identifiers specifying the resources to enumerate.
    /// \n If it is null or contains no resource, then the type must be specified.
    /// \n If it is not null and contains some resource, then the type will be ignored.
    /// \param type
    /// Type of the resource to be enumerated. See MgResourceType for valid types.
    /// No folder type is allowed.
    /// \param properties
    /// Flag to specify which properties of the resource header should be filtered.
    /// It may be set to 0 or by bitwise inclusively OR'ing together one or
    /// more of the MgResourceHeaderProperties values.
    ///
    /// \return
    /// Returns a string containing a description of the WMS layer definitions
    /// in XML format using the \link ResourceList_schema ResourceList \endlink
    /// schema.
    ///
    /// \exception MgInvalidResourceTypeException
    ///
    virtual STRING EnumerateResourceDocuments(MgStringCollection* resources,
        CREFSTRING type, INT32 properties);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the content of the specified resource.
    ///
    /// \param resource
    /// Resource identifier describing the resource.
    /// \param preProcessTags
    /// Pre-processing to apply to resource before returning content.  An empty
    /// string indicate no pre-processing. See MgResourcePreProcessingType for
    /// a list of supported pre-processing tags.
    ///
    /// \return
    /// MgByteReader object representing the resource content.
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgByteReader* GetResourceContent(MgResourceIdentifier* resource,
        CREFSTRING preProcessTags) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns named data for the specified resource.
    ///
    /// \param resource
    /// Resource identifier describing the resource.
    /// \param dataName
    /// Name for data.  Either a resource-unique stream name for streams or a
    /// resource-unique file name for file data.
    /// \param preProcessTags
    /// Pre-processing to apply to resource data before returning.  An empty
    /// string indicate no pre-processing. See MgResourcePreProcessingType for
    /// a list of supported pre-processing tags.
    ///
    /// \return
    /// MgByteReader containing the previously updated or added named data.
    ///
    /// \exception MgResourceDataNotFoundException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgByteReader* GetResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName, CREFSTRING preProcessTags) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the modified date of the specified resource.
    ///
    /// This method only works on "Library" repository.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgDateTime GetResourceModifiedDate(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgDateTime GetResourceModifiedDate(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgDateTime GetResourceModifiedDate(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Resource to get the modified date for.
    ///
    /// \return
    /// An MgDateTime object containing the modified date of the resource.
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgDateTime* GetResourceModifiedDate(MgResourceIdentifier* resource) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerate all the parent Map Definition resources of the specified
    /// resources.
    ///
    /// This method only works on "Library" and "Session" repositories.
    //
    /// \param resources
    /// Child resources.
    ///
    /// \return
    /// MgResourceIdentifier list of Map Definition resources.
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgSerializableCollection* EnumerateParentMapDefinitions(
        MgSerializableCollection* resources) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns if permission has been granted for a resource
    /// permission named data for the specified resource.
    ///
    /// \param resource
    /// Resource identifier describing the resource.
    /// \param permission
    /// Requested permission for the resource.  See MgResourcePermission.
    ///
    /// \return
    /// True if user has requested permission.
    virtual bool HasPermission(MgResourceIdentifier* resource, CREFSTRING permission);

EXTERNAL_API:

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the Metadata content for an existing resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetResourceMetadata(MgResourceIdentifier resource, MgByteReader content);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetResourceMetadata(MgResourceIdentifier resource, MgByteReader content);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetResourceMetadata(MgResourceIdentifier resource, MgByteReader content);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// The resource in the repository that gets metadata attached to. Note the resource has to exist else this method fails
    /// \n
    /// The extension of
    /// the location must match one of the types defined in \link MgResourceType MgResourceType \endlink.
    /// It is case sensitive.
    ///
    /// \param content (MgByteReader)
    /// The metadata content in XML format. This can be set to
    /// null, in which case the content is cleared.
    ///
    /// The schema of the Content is user driven and not given. Most people will use it for FGDC or ISO Metadata.
    ///
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \exception MgInvalidResourceTypeException
    ///
    /// \todo
    /// [[replace example, xref to Dev Guide]]
    ///
    /// \see
    /// GetMetadata
    ///
    virtual void SetResourceMetadata(MgResourceIdentifier* resource, MgByteReader* content);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Metadata content of a resource.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader GetResourceMetadata(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader GetResourceMetadata(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader GetResourceMetadata(MgResourceIdentifier resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// The resource in the repository which has metadata attached to it. Note the resource has to exist else this method fails.
    ///
    /// \return
    /// The metadata content in XML format. This can null in which case the resource has no metadata attached.
    /// The schema of the Content is user driven and not given. Most people will use it for FGDC or ISO Metadata.
    ///
    /// \exception MgInvalidResourceTypeException
    ///
    /// \todo
    /// [[replace example, xref to Dev Guide]]
    ///
    /// \see
    /// SetResourceMetadata
    ///
    virtual MgByteReader* GetResourceMetadata(MgResourceIdentifier* resource);

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

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_ResourceService_ResourceService;

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// Enumerations
    enum OperationId
    {
        opIdEnumerateRepositories           = 0x1111EF01,
        opIdCreateRepository                = 0x1111EF02,
        opIdDeleteRepository                = 0x1111EF03,
        opIdUpdateRepository                = 0x1111EF04,
        opIdGetRepositoryContent            = 0x1111EF05,
        opIdGetRepositoryHeader             = 0x1111EF06,
        opIdEnumerateResources              = 0x1111EF07,
        opIdSetResource                     = 0x1111EF08,
        opIdDeleteResource                  = 0x1111EF09,
        opIdCopyResource                    = 0x1111EF0A,
        opIdMoveResource                    = 0x1111EF0B,
        opIdGetResourceContent              = 0x1111EF0C,
        opIdGetResourceHeader               = 0x1111EF0D,
        opIdEnumerateReferences             = 0x1111EF0E,
        opIdEnumerateResourceData           = 0x1111EF0F,
        opIdSetResourceData                 = 0x1111EF10,
        opIdDeleteResourceData              = 0x1111EF11,
        opIdRenameResourceData              = 0x1111EF12,
        opIdGetResourceData                 = 0x1111EF13,
        opIdApplyResourcePackage            = 0x1111EF14,
        opIdChangeResourceOwner             = 0x1111EF16,
        opIdInheritPermissionsFrom          = 0x1111EF17,
        opIdGetResourceModifiedDate         = 0x1111EF18,
        opIdEnumerateParentMapDefinitions   = 0x1111EF19,
        opIdEnumerateUnmanagedData          = 0x1111EF1A,
        opIdResourceExists                  = 0x1111EF1B,
        opIdEnumerateResourceDocuments      = 0x1111EF1C,
        opIdGetResourceContents             = 0x1111EF1D,
    };
};

#endif
