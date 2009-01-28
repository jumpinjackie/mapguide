//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef MGRESOURCEDEFS_H_
#define MGRESOURCEDEFS_H_

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Reserved character sets.
/// INTERNAL - Do not document.
///
class MG_PLATFORMBASE_API MgReservedCharacterSet
{
INTERNAL_API:
    static const STRING Name;        /// Reserved characters in names = "%*:|\?<'&">=/".
    static const STRING Path;        /// Reserved characters in paths = "%*:|\?<'&">=".
    static const STRING WhiteSpace;  /// White space characters = " t r n v f".
};
/// \endcond


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Defines names for the different types of repositories.
/// \see
/// MgResourceIdentifier::SetRepositoryType
/// \see
/// MgResourceIdentifier::GetRepositoryType
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgRepositoryType
{
PUBLISHED_API:
    /// \if INTERNAL
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.
    /// \endif

    ////////////////////////////////////////////////////////////////
    /// The repository is the \link library library \endlink.
    static const STRING Library;      ///\if INTERNAL value("Library") \endif

    ////////////////////////////////////////////////////////////////
    /// The repository is a \link session_repository session repository \endlink,
    /// used to store temporary data.
    /// \note1
    ///
    static const STRING Session;      ///\if INTERNAL value("Session") \endif

INTERNAL_API:
    static const STRING Site;         /// Site repository
};


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Defines names for the different types of resources.
/// \see
/// MgResourceIdentifier::GetResourceType
/// \see
/// MgResourceIdentifier::SetResourceType
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgResourceType
{
PUBLISHED_API:
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is the runtime definition of a map.
    ///
    /// \note1
    ///
    /// \remarks
    /// It is stored in the session repository to represent the
    /// currently visible layers and viewed extents of a map. It is
    /// constructed using a \link MgResourceType::MapDefinition MapDefinition \endlink
    /// and may contain additional layers which have been added "on
    /// the fly" by a web application. See \ref Maps_and_Layers_Module "Maps and Layers"
    /// for more details.
    ///
    static const STRING Map;                ///\if INTERNAL value("Map") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is a map definition.
    ///
    /// \note1
    ///
    /// \remarks
    /// It represents an authored map and contains references to a \link MgResourceType::MapDefinition LayerDefinition \endlink
    /// for each layer comprising the map.
    ///
    /// \note
    /// This is different from a \link MgResourceType::Map Map \endlink
    /// resource, which records the current state of a map as it is
    /// being viewed by an end user.
    ///
    /// \see \ref MapDefinition_schema "MapDefinition schema"
    ///
    static const STRING MapDefinition;      ///\if INTERNAL value("MapDefinition") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is a layer definition.
    ///
    /// \remarks
    /// It represents the stylization for a specific map layer. It
    /// may also reference \link drawing_source DrawingSources \endlink and \link feature_source FeatureSources \endlink
    /// depending on the source of the data.
    ///
    /// \see \ref LayerDefinition_schema "LayerDefinition schema"
    ///
    static const STRING LayerDefinition;    ///\if INTERNAL value("LayerDefinition") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is a \link drawing_source drawing source \endlink.
    ///
    /// \note1
    ///
    /// \remarks
    /// The resource contains information required by MapGuide
    /// to access data contained in a DWF.
    ///
    /// \see \ref DrawingSource_schema "DrawingSource schema"
    ///
    static const STRING DrawingSource;      ///\if INTERNAL value("DrawingSource") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is a \link feature_source feature source \endlink.
    ///
    /// \remarks
    /// Feature sources access data through FDO data providers like
    /// the Oracle FDO provider or the SDF FDO provider. The resource
    /// content contains the information required to access the
    /// data.
    ///
    /// \see \ref FeatureSource_schema "FeatureSource schema"
    ///
    static const STRING FeatureSource;      ///\if INTERNAL value("FeatureSource") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is a folder.
    ///
    /// \note1
    ///
    /// \remarks
    /// Folders in a repository operate in a similar manner to file
    /// system folders. They contain other resources and can be
    /// nested to create resource trees.
    ///
    static const STRING Folder;             ///\if INTERNAL value("Folder") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is a load procedure definition.
    ///
    /// \note1
    ///
    /// \remarks
    /// It records how to load specific data.
    ///
    /// \see \ref LoadProcedure_schema "LoadProcedure schema"
    ///
    static const STRING LoadProcedure;      ///\if INTERNAL value("LoadProcedure") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is a print layout.
    ///
    /// \note1
    ///
    /// \remarks
    /// A print layout defines how a map is printed. It sets the size
    /// and resolution of the map on paper and also determines the
    /// location of direction arrows, legends, and other features.
    ///
    /// \see \ref PrintLayout_schema "PrintLayout schema"
    ///
    static const STRING PrintLayout;        ///\if INTERNAL value("PrintLayout") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is the runtime definition of a selection.
    ///
    /// \note1
    ///
    /// \remarks
    /// It is stored in the session repository to represent the
    /// current selection on a map.
    ///
    static const STRING Selection;          ///\if INTERNAL value("Selection") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is a symbol definition.
    ///
    /// \remarks
    /// It represents the stylization for a specific symbol.
    ///
    /// \see \ref SymbolDefinition_schema "SymbolDefinition schema"
    ///
    static const STRING SymbolDefinition;    ///\if INTERNAL value("SymbolDefinition") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource is a library of symbols.
    ///
    /// \note1
    ///
    /// \remarks
    /// Symbols from a symbol library can be placed on a map to represent points
    /// of interest. Symbol libraries are referenced from \link MgResourceType::LayerDefinition LayerDefinitions \endlink.
    ///
    /// \see \ref SymbolLibrary_schema "SymbolLibrary schema"
    ///
    static const STRING SymbolLibrary;      ///\if INTERNAL vvalue("SymbolLibrary") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource represents the screen layout for a specific
    /// map.
    ///
    /// \note1
    ///
    /// \remarks
    /// Web layouts determine the location and content of toolbars,
    /// the viewed map area, the legend, and viewed items.
    /// \n
    ///
    /// \see \ref WebLayout_schema "WebLayout schema"
    ///
    static const STRING WebLayout;          ///\if INTERNAL value("WebLayout") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// This resource represents a web application definition
    ///
    /// \note1
    ///
    /// \remarks
    /// Application definitions determine the location and content of toolbars,
    /// the viewed map area, the legend, and viewed items.
    ///
    /// \see \ref ApplicationDefinition_schema "ApplicationDefinition schema"
    ///
    static const STRING ApplicationDefinition;          ///\if INTERNAL value("ApplicationDefinition") \endif

INTERNAL_API:
    static const STRING User;           /// User definition
    static const STRING Group;          /// Group definition
    static const STRING Role;           /// Role definition
};


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Substitution tags used within the XML file for a resource.
///
/// \note1
///
/// \remarks
/// These tags are replaced by the resource service during
/// processing. This enables the resource service to manage the
/// storage of \link resource_data resource data \endlink.
/// For more information, see the \DevGuide.
///
/// \todo
/// [[Add examples for all these. ]]
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgResourceTag
{
PUBLISHED_API:
    /// \if INTERNAL
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.
    /// \endif


    ///////////////////////////////////////////////////////////
    /// \brief
    /// Replaced with the name of the currently logged in user.
    ///
    static const STRING LoginUsername;  ///\if INTERNAL value("%MG_LOGIN_USERNAME%") \endif


    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Replaced with the password of the currently logged in user.
    ///
    static const STRING LoginPassword;  ///\if INTERNAL value("%MG_LOGIN_PASSWORD%") \endif


    ////////////////////////////////////////////////////////////
    /// \brief
    /// Replaced with the default resource-specific username.
    ///
    static const STRING Username;       ///\if INTERNAL value("%MG_USERNAME%") \endif


    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Replaced with the default resource-specific password.
    ///
    static const STRING Password;       ///\if INTERNAL value("%MG_PASSWORD%") \endif


    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Replaced with the location of data files associated with the
    /// given resource. The trailing "/" is included.
    ///
    static const STRING DataFilePath;   ///\if INTERNAL value("%MG_DATA_FILE_PATH%") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Denotes beginning of data path alias.
    ///
    static const STRING DataPathAliasBegin;   ///\if INTERNAL value("%MG_DATA_PATH_ALIAS[") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Denotes end of data path alias.
    ///
    static const STRING DataPathAliasEnd;     ///\if INTERNAL value("]%") \endif
};


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Defines reserved names for resource data.
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgResourceDataName
{
PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Data name for adding default credentials to a resource.
    ///
    /// \note1
    ///
    /// \remarks
    /// For an example, see \link MgResourceService::SetResourceData SetResourceData \endlink.
    ///
    static const STRING UserCredentials;    ///\if INTERNAL value("MG_USER_CREDENTIALS") \endif
};


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Defines the different types of \link resource_data resource data \endlink.
///
/// \note1
///
/// \remarks
/// For more information, see the \DevGuide.
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgResourceDataType
{
PUBLISHED_API:
    /// \if INTERNAL
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.
    /// \endif

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Resource data stored as a file on disk.
    /// \note1
    ///
    /// \remarks
    /// MapGuide creates a separate directory for each resource
    /// and stores all resource data files in that directory. The
    /// directory name can be dynamically substituted into the XML
    /// resource content using \link MgResourceTag::DataFilePath DataFilePath \endlink.
    /// \n
    /// Large files can be uploaded using \link MgResourceService::ApplyResourcePackage ApplyResourcePackage \endlink.
    /// Web server limitations (timeouts, post message sizes, etc.)
    /// will typically limit direct HTTP uploads of file resources to
    /// a few megabytes.
    ///
    static const STRING File;           ///\if INTERNAL value("File") \endif

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Stream resource data is stored as a binary stream in the
    /// repository database.
    /// \note1
    ///
    /// \remarks
    /// Streams provide quick access to smaller data sets like symbol
    /// libraries. They are not intended for data streams larger than
    /// a few megabytes. Large data sets should be stored as files.
    ///
    static const STRING Stream;         ///\if INTERNAL value("Stream") \endif

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Resource data is stored as a string.
    /// \note1
    ///
    /// \remarks
    /// Strings are typically used for small pieces of text, such as
    /// credentials to connect to a database.
    ///
    static const STRING String;         ///\if INTERNAL value("String") \endif
};

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Types of pre-processing to apply to resource content before
/// it is returned from \link MgResourceService::GetResourceContent GetResourceContent \endlink.
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgResourcePreProcessingType
{
PUBLISHED_API:
    /// \brief
    /// Perform substitution on the resource content before returning it.
    ///
    static const STRING Substitution;   ///\if INTERNAL value("Substitution") \endif
};
/// \endcond


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Types of resource permissions.
///
/// \note1
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgResourcePermission
{
PUBLISHED_API:
    /// \if INTERNAL
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.
    /// \endif

    /// No Access permission.
    static const STRING NoAccess;       ///\if INTERNAL value("n") \endif

    /// Read-Only permission.
    static const STRING ReadOnly;       ///\if INTERNAL value("r") \endif

    /// Read/Write permission.
    static const STRING ReadWrite;      ///\if INTERNAL value("r,w") \endif
};


/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Names of resource folders.
/// INTERNAL - Do not document.
///
class MG_PLATFORMBASE_API MgResourceFolder
{
INTERNAL_API:
    static const STRING Users;          /// Users folder name
    static const STRING Groups;         /// Groups folder name
    static const STRING Roles;          /// Roles folder name
};
/// \endcond


/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Properties of the resource header.
/// INTERNAL - Do not document.
///
class MG_PLATFORMBASE_API MgResourceHeaderProperties
{
INTERNAL_API:
    static const INT32 General  = 0x00000001;
    static const INT32 Security = 0x00000002;
    static const INT32 Metadata = 0x00000004;
};
/// \endcond

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Types of unmanaged data that can be returned
///
/// \note1
///
/// \ingroup Resource_Service_classes
///
class MG_PLATFORMBASE_API MgResourceUnmanagedDataType
{
PUBLISHED_API:
    /// \if INTERNAL
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.
    /// \endif

    /// Return folders only
    static const STRING Folders;     ///\if INTERNAL value("Folders") \endif

    /// Return files only
    static const STRING Files;       ///\if INTERNAL value("Files") \endif

    /// Return both files and folders
    static const STRING Both;        ///\if INTERNAL value("Both") \endif
};

#endif
