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


#ifndef MG_RESOURCE_IDENTIFIER_H
#define MG_RESOURCE_IDENTIFIER_H

class MgResourceIdentifier;
template class MG_PLATFORMBASE_API Ptr<MgResourceIdentifier>;

/////////////////////////////////////////////////////////////////
/// \brief
/// Creates and manipulates resource identifiers.
///
/// \remarks
/// A resource identifier fully defines the location of a
/// resource or folder.
/// \n
/// The first part of the identifier specifies which repository
/// the resource is located in (for example, <c>Library://</c>,
/// or <c>Session:a421c694-ffff-ffff-8000-005056c00008_eng//</c>).
/// The path, name and type (for example, <c>Maps/North
/// America/World.MapDefinition</c>) define the resource within
/// the repository.
/// \n
/// If the resource is a folder (for example, <c>Library://Maps/North
/// America/</c>), you must include the trailing slash.
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgResourceIdentifier : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgResourceIdentifier)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Initialize and validate a resource identifier from a string.
    ///
    /// \remarks
    /// The resource pointed to does not need to exist yet.
    /// \n
    /// For the \link library library \endlink repository, the
    /// path must begin <c>Library://</c>. For \link session_repository session \endlink repositories,
    /// it must begin <c>Session:<i>identifier</i>://</c>where <i><c>identifier</c></i>
    /// is the session identifier.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgResourceIdentifier(string resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgResourceIdentifier(String resource);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgResourceIdentifier(string resource);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (String/string)
    /// The location of a document or a folder.\n The
    /// string cannot be empty. It can contain spaces but
    /// cannot contain any of the characters %*:|?<'&">=\n If
    /// the resource is a folder, you must include the trailing
    /// slash.
    ///
    /// <!-- Examples (PHP) -->
    /// \htmlinclude PHPExamplesTop.html
    /// \code
    /// $newResourceID = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// \endcode
    ///
    /// \code
    /// $newResourceID = new MgResourceIdentifier("Session:Session1234//Geography/World.Map");
    /// \endcode
    ///
    /// \code
    /// $newResourceID = new MgResourceIdentifier("Library://Floor Plans/New York/");
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgOutOfMemoryException
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgInvalidRepositoryTypeException
    ///
    MgResourceIdentifier(CREFSTRING resource);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Extracts the repository type from a resource's identifier.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string GetRepositoryType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String GetRepositoryType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string GetRepositoryType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the repository type. Legal repository types are
    /// defined in \link MgRepositoryType MgRepositoryType \endlink,
    ///
    /// <!-- Examples (PHP) -->
    /// \htmlinclude PHPExamplesTop.html
    /// \code
    /// $resourceID = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $repository_type = $resourceID->GetRepositoryType();    // returns: "Library"
    ///
    /// $resourceID = new MgResourceIdentifier("Session:$sessionID//Geography/World.MapDefinition");
    /// $repository_type = $resourceID->GetRepositoryType();    // returns: "Session"
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING GetRepositoryType();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the repository type.
    ///
    /// \remarks
    /// This method is designed to be used to construct identifiers
    /// for new resources. Do not use this to try to rename an
    /// existing resource. Use \link MgResourceService::MoveResource MoveResource \endlink
    /// instead.
    /// \n
    /// After you have finished calling SetName, SetPath,
    /// SetRepositoryName, and SetRepositoryType, call \link MgResourceIdentifier::Validate Validate \endlink
    /// to make sure the identifier is syntactically correct.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetRepositoryType(string type);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetRepositoryType(String type);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetRepositoryType(string type);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param type (String/string)
    /// Repository type string.  Legal repository
    /// types are defined in \link MgRepositoryType MgRepositoryType \endlink.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Examples (PHP) -->
    /// \htmlinclude PHPExamplesTop.html
    /// \code
    /// // Assuming $newResourceID has already been created
    /// $newResourceID->SetRepositoryType("Library");
    /// $newResourceID->Validate();
    /// \endcode
    /// or
    /// \code
    /// $newResourceID->SetRepositoryType("Session");
    /// $newResourceID->Validate();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual void SetRepositoryType(CREFSTRING type);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Extracts the repository name from a resource's identifier.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string GetRepositoryName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String GetRepositoryName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string GetRepositoryName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the repository name. For session repositories, the
    /// name is the session ID. For library repositories, the name is
    /// an empty string.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// $resourceID = new MgResourceIdentifier("Session:$sessionID//Geography/World.MapDefinition");
    /// $repository_name = $resourceID->GetRepositoryName();
    /// // returns: "b9b617ac-ffff-ffff-8000-005056c00008_eng"
    /// \endcode
    ///
    /// \code
    /// $resourceID = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $repository_name = $resourceID->GetRepositoryName();
    /// // returns: ""
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING GetRepositoryName();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the repository name.
    ///
    /// \remarks
    /// This method is designed to be used to construct identifiers
    /// for new resources. Do not use this to try to rename an
    /// existing resource. Use \link MgResourceService::MoveResource MoveResource \endlink
    /// instead.
    /// \n
    /// After you have finished calling SetName, SetPath,
    /// SetRepositoryName, and SetRepositoryType, call \link MgResourceIdentifier::Validate Validate \endlink
    /// to make sure the identifier is syntactically correct.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetRepositoryName(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetRepositoryName(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetRepositoryName(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Repository name.
    /// <ul>
    /// <li>If the repository is the library, the name must be an empty string.</li>
    /// <li>If the repository is a session repository, the name cannot be empty.</li>
    /// </ul>
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Examples (PHP) -->
    /// \htmlinclude PHPExamplesTop.html
    /// \code
    /// // Assuming $newResourceID has already been created
    /// $newResourceID->SetRepositoryName("");
    /// $newResourceID->SetRepositoryType("Library");
    /// $newResourceID->Validate();
    /// \endcode
    /// or
    /// \code
    /// $newResourceID->SetRepositoryName($sessionID);
    /// $newResourceID->SetRepositoryType("Session");
    /// $newResourceID->Validate();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual void SetRepositoryName(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the resource identifier into a string.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string ToString();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String ToString();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string ToString();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a fully qualified resource specifier string.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $newResourceID has already been created
    /// $string = $newResourceID->ToString(); // returns e.g. "Library://Maps/Atlas.MapDefinition"
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING ToString();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Extracts the resource's path from the resource identifier.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string GetPath();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String GetPath();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string GetPath();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the resource's path.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// $resourceID = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $path = $resourceID->GetPath();  // returns: "Geography"
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING GetPath();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the resource path within the repository.
    ///
    /// \remarks
    /// This method is designed to be used to construct identifiers
    /// for new resources. Do not use this to try to move an existing
    /// resource. Use \link MgResourceService::MoveResource MoveResource \endlink
    /// instead.
    /// \n
    /// After you have finished calling SetName, SetPath,
    /// SetRepositoryName, and SetRepositoryType, call \link MgResourceIdentifier::Validate Validate \endlink
    /// to make sure the identifier is syntactically correct.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetPath(string path);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetPath(String path);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetPath(string path);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param path (String/string)
    /// Resource path within the repository. It can contain
    /// spaces but cannot contain any of the characters
    /// %*:|?<'&">=\n \n Do not include
    /// the repository part of the identifier (for example, <c>Library://</c>).
    /// Do not include the trailing slash.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $newResourceID has already been created
    /// $newResourceID->SetPath("North America/Maps");
    /// $newResourceID->Validate();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual void SetPath(CREFSTRING path);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Extracts the resource's name from the resource identifier.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string GetName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the resource name.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// $resourceID = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $name = $resourceID->GetName();  // returns: "World"
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING GetName();   /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the resource name
    ///
    /// \remarks
    /// This method is designed to be used to construct identifiers
    /// for new resources. Do not use this to try to rename an
    /// existing resource. Use MoveResource instead.
    /// \n
    /// After you have finished calling SetName, SetPath,
    /// SetRepositoryName, and SetRepositoryType, call \link MgResourceIdentifier::Validate Validate \endlink
    /// to make sure the identifier is syntactically correct.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetName(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetName(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetName(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// Resource name. It can contain spaces but
    /// cannot contain any of the characters
    /// %*:|?<'&">=
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $newResourceID has already been created
    /// $newResourceID->SetName("Atlas");
    /// $newResourceID->Validate();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual void SetName(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Extracts the resource's type from the resource identifier.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string GetResourceType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String GetResourceType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string GetResourceType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the resource's type string, as defined in
    /// MgResourceType.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// $resourceID = new MgResourceIdentifier("Library://Geography/World.MapDefinition");
    /// $type = $resourceID->GetResourceType();   // returns: "MapDefinition"
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING GetResourceType();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the resource type.
    ///
    /// \remarks
    /// This method is designed to be used to construct identifiers
    /// for new resources. Do not use this to try to rename an
    /// existing resource. Use MoveResource instead.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetResourceType(string type);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetResourceType(String type);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetResourceType(string type);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param type (String/string)
    /// Resource type string as defined in \link MgResourceType MgResourceType \endlink.
    /// It is case sensitive.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $newResourceID has already been created
    /// $newResourceID->SetResourceType("MapDefinition");
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual void SetResourceType(CREFSTRING type);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Validates the resource identifier.  This method should be called
    /// after the identifier has been fully specified by constructor and/or
    /// Set methods.
    ///
    /// \remarks
    /// This checks that the syntax of the identifier is correct. It
    /// does not check that the specified resource actually exists.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Validate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Validate();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Validate();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// // Assuming $newResourceID has already been created
    /// $newResourceID->Validate();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    ///
    virtual void Validate();

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructs an MgResourceIdentifier object.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual ~MgResourceIdentifier();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an uninitialized MgResourceIdentifier object.
    ///
    MgResourceIdentifier();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgResourceIdentifier object based on the specified
    /// repository type/name and resource path/name/type.
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgOutOfMemoryException
    ///
    MgResourceIdentifier(CREFSTRING repositoryType, CREFSTRING repositoryName,
        CREFSTRING resourcePath, CREFSTRING resourceName,
        CREFSTRING resourceType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a new MgResourceIdentifier object and initializes the member
    /// variables to values given by a referenced MgResourceIdentifier object.
    ///
    /// \param resourceIdentifier
    /// Reference to an MgResourceIdentifier object.
    ///
    /// \exception MgOutOfMemoryException
    ///
    MgResourceIdentifier(const MgResourceIdentifier& resourceIdentifier);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Overloaded assignment operator for MgResourceIdentifier.
    ///
    /// \param resourceIdentifier
    /// Reference to the structure that is to be copied.
    ///
    /// <return>
    /// Reference to the structure (the one assigned to).
    /// </return>
    ///
    /// \exception MgOutOfMemoryException
    ///
    MgResourceIdentifier& operator=(const MgResourceIdentifier& resourceIdentifier);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the resource is of root type.
    ///
    /// \return
    /// true if the resource is of root type, and false otherwise.
    ///
    bool IsRoot() const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the resource is of folder type.
    ///
    /// \return
    /// true if the resource is of folder type, and false otherwise.
    ///
    bool IsFolder() const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the resource is of the specified repository type.
    ///
    /// \return
    /// true if the resource is of the specified repository type, false otherwise.
    ///
    bool IsRepositoryTypeOf(CREFSTRING type) const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the resource is of the specified resource type.
    ///
    /// \return
    /// true if the resource is of the specified resource type, false otherwise.
    ///
    bool IsResourceTypeOf(CREFSTRING type) const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the resource is a runtime resource.
    ///
    /// \return
    /// true if the resource is a runtime resource, false otherwise.
    ///
    bool IsRuntimeResource() const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the root path of the resource.
    ///
    /// \return
    /// Root path of the resource.
    ///
    STRING GetRootPath() const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the full path of the resource.
    ///
    /// \param appendSlash
    /// Flag specifying whether or not if the full path should be appended with
    /// a forward slash.
    ///
    /// \return
    /// Full path of the resource.
    ///
    STRING GetFullPath(bool appendSlash) const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the pathname of the resource with or without type.
    ///
    /// \param includeType
    /// Flag specifying whether or not if the pathname should be appended with
    /// a resource type.
    ///
    /// \return
    /// Pathname of the resource.
    ///
    STRING GetPathname(bool includeType) const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the resource depth.
    ///
    /// \return
    /// Resource depth.
    ///
    INT32 GetDepth() const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Parses the resource identifier from a string.
    ///
    /// \param resource
    /// Resource string similar to "Library://Telus/Calgary/Maps/Downtown.MapDefinition"
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    void ParseIdentifier(CREFSTRING resource);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the resource identifier from a string
    ///
    /// \param resource
    /// Resource string similar to "Library://Telus/Calgary/Maps/Downtown.MapDefinition"
    ///
    /// \exception MgResourceLocationException
    /// \exception MgOutOfMemoryException
    /// \exception MgInvalidRepositoryTypeException
    ///
    void SetResource(CREFSTRING resource);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks that the specified repository type an resource type are valid.
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidResourceTypeException
    ///
    static void CheckType(CREFSTRING repositoryType, CREFSTRING resourceType);

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId(); // { return m_cls_id; }

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

private:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Parses the resource path, name, and type.
    ///
    /// \param resource
    /// Resource string similar to "Telus/Calgary/Maps/Downtown.MapDefinition"
    ///
    /// \exception MgInvalidRepositoryTypeException
    /// \exception MgInvalidRepositoryNameException
    /// \exception MgInvalidResourcePathException
    /// \exception MgInvalidResourceNameException
    /// \exception MgInvalidResourceTypeException
    ///
    void ParsePathname(CREFSTRING resource);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks that the repository type is valid.
    ///
    /// \exception MgInvalidRepositoryTypeException
    ///
    void CheckRepositoryType();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks that the repository name is valid.
    ///
    /// \exception MgInvalidRepositoryNameException
    ///
    void CheckRepositoryName();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks that the resource path is valid.
    ///
    /// \exception MgInvalidResourcePathException
    ///
    void CheckPath();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks that the resource name is valid.
    ///
    /// \exception MgInvalidResourceNameException
    ///
    void CheckName();

    STRING m_repositoryType;
    STRING m_repositoryName;
    STRING m_path;
    STRING m_name;
    STRING m_type;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_ResourceService_ResourceIdentifier;
};


#endif
