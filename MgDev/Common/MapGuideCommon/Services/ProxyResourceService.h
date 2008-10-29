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

#ifndef MGPROXYRESOURCESERVICE_H_
#define MGPROXYRESOURCESERVICE_H_

/// \cond INTERNAL
//////////////////////////////////////////////////////////////////
/// \brief
/// Contains all the methods to make requests to the resource
/// service API of a MapGuide server.
class MG_MAPGUIDE_API MgProxyResourceService : public MgResourceService
{
    DECLARE_CLASSNAME(MgProxyResourceService)

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgProxyResourceService object.
    ///
    MgProxyResourceService();

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

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Internal method to return user information for service
    ///
    /// \return
    /// MgUserInformation for service
    virtual MgUserInformation* GetUserInfo();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a list session repositories.
    ///
    /// \param repositoryType
    /// The type of repository you want to list the entries for. ("Session")
    ///
    /// \return
    /// MgByteReader object representing the list of session repositories.
    ///
    /// \exception MgOutOfMemoryException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual MgByteReader* EnumerateRepositories(CREFSTRING repositoryType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a repository with the given name and type.
    ///
    /// \remarks
    /// This method only works on "Library" and "Session" repositories.
    /// If you specify a repository that is not supported this method
    /// will throw an InvalidRepositoryType exception.
    ///
    /// \param resource
    /// Resource identifier describing the repository to create
    /// The following repositories are supported:
    /// <ol>
    ///     <li>Library</li>
    ///     <li>Session</li>
    /// </ol>
    ///
    /// \param content
    /// Resource content.  The content may be XML or binary.
    /// \param header
    /// XML header definition.  May be NULL.
    ///
    /// \return
    /// Newly created repository
    ///
    /// \exception MgOutOfMemoryException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual void CreateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Delete a repository with the given name and type.
    ///
    /// \remarks
    /// This method only works on "Library" and "Session" repositories.
    /// If you specify a repository that is not supported this method
    /// will throw an InvalidRepositoryType exception.
    ///
    /// \param resource
    /// Resource identifier describing the repository to delete
    /// The following repositories are supported:
    /// <ol>
    ///     <li>Library</li>
    ///     <li>Session</li>
    /// </ol>
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgOutOfMemoryException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual void DeleteRepository(MgResourceIdentifier* resource);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the modified date of the specified resource.
    ///
    /// \remarks
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
    virtual MgDateTime* GetResourceModifiedDate(MgResourceIdentifier* resource);

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
        MgSerializableCollection* resources);

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Updates a repository with the given name and type.
    ///
    /// \remarks
    /// This method only works on "Library" and "Session" repositories.
    /// If you specify a repository that is not supported this method
    /// will throw an InvalidRepositoryType exception.
    /// This method is to be used on a repository that already exists.
    /// If you specify a repository that does not exist, this method
    /// will throw a RepositoryNotFound exception.
    ///
    /// \param resource
    /// Resource identifier describing the already existing repository to
    /// update.
    /// The following repositories are supported:
    /// <ol>
    ///     <li>Library</li>
    ///     <li>Session</li>
    /// </ol>
    /// \param content
    /// Resource content.  The content may be XML or binary.
    /// \param header
    /// XML header definition.  May be NULL.
    ///
    /// \return
    /// Newly created repository
    ///
    /// \exception MgOutOfMemoryException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual void UpdateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Applies a package of resource changes to the repository.
    ///
    /// \remarks
    /// This method only works on "Library" repository.
    /// Cross repository (Library:// and Session:a2c8e5ba//) changes in a single
    /// package are not supported.
    /// Packages can be created by Autodesk Studio or by the
    /// Autodesk Studio API. See the Studio help or the <i>Autodesk Studio API Reference</i>.
    ///
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
    /// $filepath = 'C:DataPackagespackage.mgp';
    /// $byteSource = new MgByteSource($filepath);
    /// $byteReader = $byteSource->GetReader();
    /// $resourceService->ApplyResourcePackage($byteReader);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidResourcePackageException
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
    virtual void ApplyResourcePackage(MgByteReader* resourcePackage);

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
    virtual bool ResourceExists(MgResourceIdentifier* resource);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates the resources in the specified repository.
    ///
    /// \remarks
    /// Resources of all types can be enumerated all at once, or only
    /// resources of a given type.
    ///
    /// \param resource
    /// Resource identifier describing the resources to enumerate.
    /// \param depth
    /// Recursion depth.
    /// If the depth is greater than 0, the specified resource's descendants
    /// (up to the specified level of depth) are returned.
    /// If the depth is equal to 0, the specified resource is returned.
    /// If the depth is less than 0, the specified resource and its descendants
    /// (no limit in depth) are returned.
    /// \param type
    /// Type of the resource to be enumerated.  See ResourceType for valid types.
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
        CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds a new resource to a resource repository, or updates an existing resource.
    ///
    /// \remarks
    /// The resource being added or updated can be any of the known resource
    /// types.  The headers are defined by XML schemas.
    ///
    /// \param resource
    /// Resource identifier describing the resource to add or update.
    /// \param content
    /// Resource content.
    /// This may be NULL when updating an existing resource.
    /// \param header
    /// XML definition of the permissions and other information.
    /// This may be NULL.
    ///
    /// \return
    /// Nothing
    ///
    /// \note
    /// The dependencies in the document header schema are used to ensure that multi-resource
    /// data
    /// like published items
    /// are not inadvertently corrupted by deleting only part of the data.
    ///
    /// \exception MgRepositoryNotOpenException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual void SetResource(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deletes an existing resource
    ///
    /// \param resource
    /// Resource identifier describing the resource to delete
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgInvalidResourceTypeException
    ///
    virtual void DeleteResource(MgResourceIdentifier* resource);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Moves an existing resource from this repository to another location.
    ///
    /// This function can serve multiple purposes:
    ///   - Move a resource if resource path and name are different.
    ///   - Rename a resource if resource path is the same and name is different.
    ///
    /// \param sourceResource
    /// Resource identifier for the resource to be moved/renamed.
    /// This resource can be a document or folder.
    /// \param destResource
    /// Resource identifier describing where/what the resource should be moved/renamed to.
    /// \param overwrite
    /// Flag to determine whether or not the destination
    /// resource should be overwritten if it exists.
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgResourceNotFoundException
    /// \exception MgDuplicateResourceException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///

    virtual void MoveResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Copies an existing resource to another location.
    ///
    /// \param sourceResource
    /// Resource identifier for the resource to be copied from.
    /// This resource can be a document or folder.
    /// \param destResource
    /// Resource identifier describing where the resource should be copied to.
    /// \param overwrite
    /// Flag to determine whether or not the destination
    /// resource should be overwritten if it exists.
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgResourceNotFoundException
    /// \exception MgDuplicateResourceException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual void CopyResource(MgResourceIdentifier* sourceResource,
        MgResourceIdentifier* destResource, bool overwrite);

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
        CREFSTRING preProcessTags);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the header associated with the specified resource.
    ///
    /// \param resource
    /// Resource identifier for desired resource
    ///
    /// \return
    /// MgByteReader object representing the XML resource header.
    ///
    /// \exception MgRepositoryNotOpenException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgByteReader* GetResourceHeader(MgResourceIdentifier* resource);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Changes the owner of an existing resource.
    /// This method only works on "Library" repository.
    ///
    /// \param resource
    /// Resource identifier describing the resource whose owner to be changed.
    /// This resource can be a document or folder.
    /// \param owner
    /// Name of the new owner.
    /// \param includeDescendants
    /// Flag to determine whether or not the owners of descendant resources
    /// should be also be changed.
    /// This flag is applicable only if the resource is a folder.
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgPermissionDeniedException
    /// \exception MgResourceNotFoundException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual void ChangeResourceOwner(MgResourceIdentifier* resource,
        CREFSTRING owner, bool includeDescendants);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the permissions for all descendants to be inherited from the
    /// specified resource.
    /// This method only works on "Library" repository.
    ///
    /// \param resource
    /// Resource identifier describing the resource whose permissions to be
    /// inherited from.
    /// This resource must be a folder.
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgPermissionDeniedException
    /// \exception MgResourceNotFoundException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual void InheritPermissionsFrom(MgResourceIdentifier* resource);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets tagged data of a specific type to the specified resource.
    ///
    /// \param resource
    /// Resource identifier describing the resource.
    /// \param dataName
    /// Name for data.  Either a resource-unique stream name for streams or a
    /// resource-unique file name for file data.
    /// \param dataType
    /// Data type for data.  See ResourceStorageType.
    /// \param data
    /// Data to set.
    ///
    /// \return
    /// Nothing.
    ///
    /// \note
    /// If a resource is added without all of its data
    /// an exception will be thrown when the data is accessed.
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgInvalidResourceDataTypeException
    ///
    virtual void SetResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deletes tagged data from the specified resource
    ///
    /// \param resource
    /// Resource identifier describing the resource
    /// \param dataName
    /// Name for data.  Either a resource-unique stream name for streams or a
    /// resource-unique file name for file data.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgInvalidResourceTypeException
    ///
    virtual void DeleteResourceData(MgResourceIdentifier* resource, CREFSTRING dataName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Renames tagged data for the specified resource.
    ///
    /// \param resource
    /// Resource identifier describing the resource.
    /// \param oldDataName
    /// Old data name.
    /// \param newDataName
    /// New data name.
    /// \param overwrite
    /// Flag to determine whether or not the destination
    /// resource data should be overwritten if it exists.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgInvalidResourceDataTypeException
    /// \exception MgResourceDataNotFoundException
    /// \exception MgDuplicateResourceDataException
    ///
    virtual void RenameResourceData(MgResourceIdentifier* resource,
        CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns tagged data for the specified resource.
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
    /// MgByteReader containing the previously updated or added tagged data.
    ///
    /// \exception MgRepositoryNotOpenException
    /// \exception MgResourceDataNotFoundException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgByteReader* GetResourceData(MgResourceIdentifier* resource,
        CREFSTRING dataName, CREFSTRING preProcessTags);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates the tagged data for the specified resource.
    ///
    /// \param resource
    /// Resource identifier describing the tags to enumerate
    ///
    /// \return
    /// MgByteReader object representing the description of the resource data.
    ///
    /// \exception MgRepositoryNotOpenException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgByteReader* EnumerateResourceData(MgResourceIdentifier* resource);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the content of the specified repository.
    ///
    /// \param resource
    /// Resource identifier describing the repository
    ///
    /// \return
    /// MgByteReader object representing the repository content
    ///
    /// \exception MgRepositoryNotOpenException
    /// \exception MgRepositoryNotFoundException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual MgByteReader* GetRepositoryContent(MgResourceIdentifier* resource);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the header of the specified repository.
    ///
    /// \param resource
    /// Resource identifier describing the repository
    ///
    /// \return
    /// MgByteReader object representing the repository header
    ///
    /// \exception MgRepositoryNotOpenException
    /// \exception MgRepositoryNotFoundException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidRepositoryTypeException
    ///
    virtual MgByteReader* GetRepositoryHeader(MgResourceIdentifier* resource);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates all resources which reference the specified resource.
    ///
    /// \param resource
    /// Input
    /// Resource identifier of the referenced resource.
    ///
    /// \return
    /// MgByteReader object representing the list of all resources which
    /// reference the specified resource.
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    virtual MgByteReader* EnumerateReferences(MgResourceIdentifier* resource);


    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enumerates unmanaged data
    ///
    /// \return
    /// MgByteReader object representing unmanaged data
    ///
    virtual MgByteReader* EnumerateUnmanagedData(CREFSTRING path, bool recursive,
        CREFSTRING select, CREFSTRING filter);

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
