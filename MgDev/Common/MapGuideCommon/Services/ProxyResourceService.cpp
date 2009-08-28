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

#include "MapGuideCommon.h"
#include "Command.h"
#include "CryptographyUtil.h"

static const int Resource_Service = (int)MgPacketParser::msiResource;

IMPLEMENT_CREATE_SERVICE(MgProxyResourceService)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgProxyResourceService object.
/// <param name="connection">
/// Connection to server
/// </param>
/// </summary>
MgProxyResourceService::MgProxyResourceService() : MgResourceService()
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgProxyResourceService::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the warnings if any
/// </summary>
void MgProxyResourceService::SetWarning(MgWarnings* warning)
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
/// Gets a list of entries for the specified repository type. The
/// following lists will be returned for the specified repository
/// type:
///     1) Session - A list of session IDs is returned.
///
/// This method only works on "Session" repositories.
/// If you specify a repository that is not supported this method
/// will throw an MgInvalidRepositoryTypeException exception.
/// </summary>
/// <param name="repositoryType">
/// The type of repository you want to list the entries for.
/// The following repositories are supported:
///     1) Session
/// </param>
/// <returns>
/// MgByteReader object representing the list of entries for the
/// repository type specified.
/// </returns>
/// EXCEPTIONS:
///   MgOutOfMemoryException
///   MgInvalidRepositoryTypeException
MgByteReader* MgProxyResourceService::EnumerateRepositories(CREFSTRING repositoryType)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgResourceService::opIdEnumerateRepositories,
                        1,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knString, &repositoryType,
                        MgCommand::knNone);

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates a repository with the given name and type.
///
/// This method only works on "Library" and "Session" repositories.
/// If you specify a repository that is not supported this method
/// will throw an InvalidRepositoryType exception.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the repository to create
/// The following repositories are supported:
///     1) Library
///     2) Session
/// </param>
/// <param name="content">
/// Resource content.  The content may be XML or binary.
/// </param>
/// <param name="header">
/// XML header definition.  May be NULL.
/// </param>
/// EXCEPTIONS:
///   MgOutOfMemoryException
///   MgInvalidRepositoryTypeException
///   MgInvalidRepositoryNameException
///   MgRepositoryNameCreationFailedException
///   MgOpenRepositoryFailedException
///   MgRepositoryAlreadyExistsException
///
void MgProxyResourceService::CreateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knVoid,
                        MgResourceService::opIdCreateRepository,
                        3,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knObject, content,
                        MgCommand::knObject, header,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Delete a repository with the given name and type.
///
/// This method only works on "Library" and "Session" repositories.
/// If you specify a repository that is not supported this method
/// will throw an InvalidRepositoryType exception.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the repository to delete
/// The following repositories are supported:
///     1) Library
///     2) Session
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
///   MgOutOfMemoryException
///   MgInvalidRepositoryTypeException
void MgProxyResourceService::DeleteRepository(MgResourceIdentifier* resource)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knVoid,
                        MgResourceService::opIdDeleteRepository,
                        1,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knNone);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Updates a repository with the given name and type.
///
/// This method only works on "Library" and "Session" repositories.
/// If you specify a repository that is not supported this method
/// will throw an InvalidRepositoryType exception.
/// This method is to be used on a repository that already exists.
/// If you specify a repository that does not exist, this method
/// will throw a RepositoryNotFound exception.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the already existing repository to
/// update.
/// The following repositories are supported:
///     1) Library
///     2) Session
/// </param>
/// <param name="content">
/// Resource content.  The content may be XML or binary.
/// </param>
/// <param name="header">
/// XML header definition.  May be NULL.
/// </param>
/// <returns>
/// Newly created repository
/// </returns>
/// EXCEPTIONS:
///   MgOutOfMemoryException
///   MgInvalidRepositoryTypeException

void MgProxyResourceService::UpdateRepository(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knVoid,
                        MgResourceService::opIdUpdateRepository,
                        3,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knObject, content,
                        MgCommand::knObject, header,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Checks to see if the specified resource exists.
///
bool MgProxyResourceService::ResourceExists(MgResourceIdentifier* resource)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                                  // Connection
                       MgCommand::knInt8,                           // Return type expected
                       MgResourceService::opIdResourceExists,       // Command code
                       1,                                           // Number of arguments
                       Resource_Service,                            // Service ID
                       BUILD_VERSION(1,0,0),                        // Operation version
                       MgCommand::knObject, resource,               // Argument #1
                       MgCommand::knNone);                          // End of argument

    SetWarning(cmd.GetWarningObject());

    return (bool)cmd.GetReturnValue().val.m_i8;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerates the resources in the specified repository.
/// Resources of all types can be enumerated all at once, or only
/// resources of a given type.
///
MgByteReader* MgProxyResourceService::EnumerateResources(
    MgResourceIdentifier* resource, INT32 depth, CREFSTRING type,
    INT32 properties, CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgResourceService::opIdEnumerateResources,
                        7,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knInt32, depth,
                        MgCommand::knString, &type,
                        MgCommand::knInt32, properties,
                        MgCommand::knString, &fromDate,
                        MgCommand::knString, &toDate,
                        MgCommand::knInt8, (int)computeChildren,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Adds a new resource to a resource repository, or updates an existing resource.
/// The resource being added or updated can be any of the known resource
/// types.  The headers are defined by XML schemas.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource to add or update.
/// </param>
/// <param name="content">
/// Resource content.
/// This may be NULL when updating an existing resource.
/// </param>
/// <param name="header">
/// XML definition of the permissions and other information.
/// This may be NULL.
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
/// MgRepositoryNotOpenException
///
/// MgInvalidResourceTypeException
/// NOTE:
/// The dependencies in the document header schema are used to ensure that multi-resource
/// data, like published items, are not inadvertently corrupted by deleting only part of the data.

void MgProxyResourceService::SetResource(MgResourceIdentifier* resource, MgByteReader* content, MgByteReader* header)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knVoid,
                        MgResourceService::opIdSetResource,
                        3,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knObject, content,
                        MgCommand::knObject, header,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Applies a package of resource changes to the repository.
///
void MgProxyResourceService::ApplyResourcePackage(MgByteReader* resourcePackage)
{
    assert(m_connProp != NULL);
    MgCommand cmd;

    cmd.ExecuteCommand(
        m_connProp,                                         // Connection
        MgCommand::knVoid,                                  // Return type
        MgResourceService::opIdApplyResourcePackage,        // Command Code
        1,                                                  // Number of arguments
        Resource_Service,                                   // Service ID
        BUILD_VERSION(1,0,0),                               // Operation version
        MgCommand::knObject, resourcePackage,               // Argument #1
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes an existing resource
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource to delete
/// </param>
/// EXCEPTIONS:
/// MgInvalidResourceTypeException
void MgProxyResourceService::DeleteResource(MgResourceIdentifier* resource)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knVoid,
                        MgResourceService::opIdDeleteResource,
                        1,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Moves an existing resource from this repository to another location.
///
/// This function can serve multiple purposes:
///   - Move a resource if resource path and name are different.
///   - Rename a resource if resource path is the same and name is different.
/// </summary>
/// <param name="sourceResource">
/// Resource identifier for the resource to be moved/renamed.
/// This resource can be a document or folder.
/// </param>
/// <param name="destResource">
/// Resource identifier describing where/what the resource should be moved/renamed to.
/// </param>
/// <param name="overwrite">
/// Flag to determine whether or not the destination
/// resource should be overwritten if it exists.
/// </param>
/// <returns>
/// Nothing.
/// </returns>
/// EXCEPTIONS:
/// MgResourceNotFoundException
/// MgDuplicateResourceException
/// MgInvalidRepositoryTypeException
/// MgInvalidRepositoryNameException
/// MgInvalidResourcePathException
/// MgInvalidResourceNameException
/// MgInvalidResourceTypeException

void MgProxyResourceService::MoveResource(MgResourceIdentifier* sourceResource,
    MgResourceIdentifier* destResource, bool overwrite)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knVoid,
                        MgResourceService::opIdMoveResource,
                        3,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, sourceResource,
                        MgCommand::knObject, destResource,
                        MgCommand::knInt8, (int)overwrite,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Moves an existing resource from this repository to another location.
///
/// This function can serve multiple purposes:
///   - Move a resource if resource path and name are different.
///   - Rename a resource if resource path is the same and name is different.
/// </summary>
/// <param name="sourceResource">
/// Resource identifier for the resource to be moved/renamed.
/// This resource can be a document or folder.
/// </param>
/// <param name="destResource">
/// Resource identifier describing where/what the resource should be moved/renamed to.
/// </param>
/// <param name="overwrite">
/// Flag to determine whether or not the destination
/// resource should be overwritten if it exists.
/// </param>
/// <param name="cascade">
/// Flag to determine whether or not the
/// referencing resources should be updated.
/// </param>
/// <returns>
/// Nothing.
/// </returns>
/// EXCEPTIONS:
/// MgResourceNotFoundException
/// MgDuplicateResourceException
/// MgInvalidRepositoryTypeException
/// MgInvalidRepositoryNameException
/// MgInvalidResourcePathException
/// MgInvalidResourceNameException
/// MgInvalidResourceTypeException

void MgProxyResourceService::MoveResource(MgResourceIdentifier* sourceResource,
                                          MgResourceIdentifier* destResource, bool overwrite, bool cascade)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
        MgCommand::knVoid,
        MgResourceService::opIdMoveResource,
        4,
        Resource_Service,
        BUILD_VERSION(2,2,0),
        MgCommand::knObject, sourceResource,
        MgCommand::knObject, destResource,
        MgCommand::knInt8, (int)overwrite,
        MgCommand::knInt8, (int)cascade,
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Copies an existing resource to another location.
/// </summary>
/// <param name="sourceResource">
/// Resource identifier for the resource to be copied from.
/// This resource can be a document or folder.
/// </param>
/// <param name="destResource">
/// Resource identifier describing where the resource should be copied to.
/// </param>
/// <param name="overwrite">
/// Flag to determine whether or not the destination
/// resource should be overwritten if it exists.
/// </param>
/// <returns>
/// Nothing.
/// </returns>
/// EXCEPTIONS:
/// MgResourceNotFoundException
/// MgDuplicateResourceException
/// MgInvalidRepositoryTypeException
/// MgInvalidRepositoryNameException
/// MgInvalidResourcePathException
/// MgInvalidResourceNameException
/// MgInvalidResourceTypeException

void MgProxyResourceService::CopyResource(MgResourceIdentifier* sourceResource,
    MgResourceIdentifier* destResource, bool overwrite)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knVoid,
                        MgResourceService::opIdCopyResource,
                        3,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, sourceResource,
                        MgCommand::knObject, destResource,
                        MgCommand::knInt8, (int)overwrite,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the content of the specified resource.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource.
/// </param>
/// <param name="preProcessTags">
/// Pre-processing to apply to resource before returning content.  An empty
/// string indicate no pre-processing. See MgResourcePreProcessingType for
/// a list of supported pre-processing tags.
/// </param>
/// <returns>
/// MgByteReader object representing the resource content.
/// </returns>
/// EXCEPTIONS:
/// MgRepositoryNotOpenException
///
/// MgInvalidResourceTypeException
///
MgByteReader* MgProxyResourceService::GetResourceContent(
    MgResourceIdentifier* resource, CREFSTRING preProcessTags)
{
    Ptr<MgByteReader> byteReader;

    MG_TRY()

    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgResourceService::opIdGetResourceContent,
                        2,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knString, &preProcessTags,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    byteReader = (MgByteReader*)cmd.GetReturnValue().val.m_obj;

    // Decrypt the document if Substitution pre-processing is required.

    if (MgResourcePreProcessingType::Substitution == preProcessTags
        && byteReader != NULL)
    {
        STRING mimeType = byteReader->GetByteSource()->GetMimeType();
        string cipherText, plainText;

        byteReader->ToStringUtf8(cipherText);

        MG_CRYPTOGRAPHY_TRY()

        MgCryptographyUtil cryptoUtil;

        cryptoUtil.DecryptString(cipherText, plainText);

        MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgProxyResourceService.GetResourceContent")

        Ptr<MgByteSource> byteSource = new MgByteSource(
            (BYTE_ARRAY_IN)plainText.c_str(), (INT32)plainText.length());

        byteSource->SetMimeType(mimeType);
        byteReader = byteSource->GetReader();
    }

    MG_CATCH_AND_THROW(L"MgProxyResourceService.GetResourceContent")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the contents of the specified resources.
///
MgStringCollection* MgProxyResourceService::GetResourceContents(MgStringCollection* resources,
                                               MgStringCollection* preProcessTags)
{
    Ptr<MgStringCollection> resourceContents;

    MG_TRY()

    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,                      // Connection
        MgCommand::knObject,                            // Return type expected
        MgResourceService::opIdGetResourceContents,     // Command Code
        2,                                              // Count of arguments
        Resource_Service,                               // Service Id
        BUILD_VERSION(1,0,0),                           // Operation version
        MgCommand::knObject, resources,                 // Argument#1
        MgCommand::knObject, preProcessTags,            // Argument#2
        MgCommand::knNone);                             // End of argument

    SetWarning(cmd.GetWarningObject());

    resourceContents = (MgStringCollection*)cmd.GetReturnValue().val.m_obj;

    // Decrypt the document if Substitution pre-processing is required.
    if(preProcessTags != NULL && resourceContents != NULL && preProcessTags->GetCount() == resourceContents->GetCount())
    {
        for(INT32 i = 0; i < resourceContents->GetCount(); i ++)
        {
            STRING tag = preProcessTags->GetItem(i);

            if (MgResourcePreProcessingType::Substitution == tag)
            {
                STRING cipherContent = resourceContents->GetItem(i);

                string cipherText, plainText;
                MgUtil::WideCharToMultiByte(cipherContent, cipherText);

                MG_CRYPTOGRAPHY_TRY()

                MgCryptographyUtil cryptoUtil;

                cryptoUtil.DecryptString(cipherText, plainText);

                MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgProxyResourceService.GetResourceContents")

                STRING decryptedContent;
                MgUtil::MultiByteToWideChar(plainText, decryptedContent);
                resourceContents->SetItem(i, decryptedContent);
            }
        }
    }

    MG_CATCH_AND_THROW(L"MgProxyResourceService.GetResourceContents")

    return resourceContents.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Changes the owner of an existing resource.
/// This method only works on "Library" repository.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource whose owner to be changed.
/// This resource can be a document or folder.
/// </param>
/// <param name="owner">
/// Name of the new owner.
/// </param>
/// <param name="includeDescendants">
/// Flag to determine whether or not the owners of descendant resources
/// should be also be changed.
/// This flag is applicable only if the resource is a folder.
/// </param>
/// <returns>
/// Nothing.
/// </returns>
/// EXCEPTIONS:
/// MgPermissionDeniedException
/// MgResourceNotFoundException
/// MgInvalidRepositoryTypeException
/// MgInvalidRepositoryNameException
/// MgInvalidResourcePathException
/// MgInvalidResourceNameException
/// MgInvalidResourceTypeException

void MgProxyResourceService::ChangeResourceOwner(MgResourceIdentifier* resource,
    CREFSTRING owner, bool includeDescendants)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knVoid,
                        MgResourceService::opIdChangeResourceOwner,
                        3,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knString, &owner,
                        MgCommand::knInt8, (int)includeDescendants,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the permissions for all descendants to be inherited from the
/// specified resource.
/// This method only works on "Library" repository.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource whose permissions to be
/// inherited from.
/// This resource must be a folder.
/// </param>
/// <returns>
/// Nothing.
/// </returns>
/// EXCEPTIONS:
/// MgPermissionDeniedException
/// MgResourceNotFoundException
/// MgInvalidRepositoryTypeException
/// MgInvalidRepositoryNameException
/// MgInvalidResourcePathException
/// MgInvalidResourceNameException
/// MgInvalidResourceTypeException

void MgProxyResourceService::InheritPermissionsFrom(
    MgResourceIdentifier* resource)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knVoid,
                        MgResourceService::opIdInheritPermissionsFrom,
                        1,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the header associated with the specified resource.
/// </summary>
/// <param name="resource">
/// Resource identifier for desired resource
/// </param>
/// <returns>
/// MgByteReader object representing the XML resource header.
/// </returns>
/// EXCEPTIONS:
/// MgRepositoryNotOpenException
///
/// MgInvalidResourceTypeException
MgByteReader* MgProxyResourceService::GetResourceHeader(MgResourceIdentifier* resource)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgResourceService::opIdGetResourceHeader,
                        1,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Sets tagged data of a specific type to the specified resource.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource.
/// </param>
/// <param name="dataName">
/// Name for data.  Either a resource-unique stream name for streams or a
/// resource-unique file name for file data.
/// </param>
/// <param name="dataType">
/// Data type for data.  See ResourceStorageType.
/// </param>
/// <param name="data">
/// Data to set.
/// </param>
/// <returns>
/// Nothing.
/// </returns>
/// EXCEPTIONS:
///
/// MgInvalidRepositoryTypeException
/// MgInvalidResourceTypeException
/// MgInvalidResourceDataTypeException
/// NOTE:
/// If a resource is added without all of its data, an exception will be thrown when the data is accessed.

void MgProxyResourceService::SetResourceData(MgResourceIdentifier* resource,
    CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                       MgCommand::knVoid,
                       MgResourceService::opIdSetResourceData,
                       4,
                       Resource_Service,
                       BUILD_VERSION(1,0,0),
                       MgCommand::knObject, resource,
                       MgCommand::knString, &dataName,
                       MgCommand::knString, &dataType,
                       MgCommand::knObject, data,
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Deletes tagged data from the specified resource
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource
/// </param>
/// <param name="dataName">
/// Tag name for the data
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
/// MgInvalidResourceTypeException
void MgProxyResourceService::DeleteResourceData(MgResourceIdentifier* resource, CREFSTRING dataName)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knVoid,
                        MgResourceService::opIdDeleteResourceData,
                        2,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knString, &dataName,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Renames tagged data for the specified resource.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource.
/// </param>
/// <param name="oldDataName">
/// Old data name.
/// </param>
/// <param name="newDataName">
/// New data name.
/// </param>
/// <param name="overwrite">
/// Flag to determine whether or not the destination
/// resource data should be overwritten if it exists.
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
/// MgInvalidRepositoryTypeException
/// MgInvalidResourceTypeException
/// MgInvalidResourceDataTypeException
/// MgResourceDataNotFoundException
/// MgDuplicateResourceDataException

void MgProxyResourceService::RenameResourceData(MgResourceIdentifier* resource,
        CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                       MgCommand::knVoid,
                       MgResourceService::opIdRenameResourceData,
                       4,
                       Resource_Service,
                       BUILD_VERSION(1,0,0),
                       MgCommand::knObject, resource,
                       MgCommand::knString, &oldDataName,
                       MgCommand::knString, &newDataName,
                       MgCommand::knInt8, (int)overwrite,
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns tagged data for the specified resource.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource.
/// </param>
/// <param name="dataName">
/// Name for data.  Either a resource-unique stream name for streams or a
/// resource-unique file name for file data.
/// </param>
/// <param name="preProcessTags">
/// Pre-processing to apply to resource data before returning.  An empty
/// string indicate no pre-processing. See MgResourcePreProcessingType for
/// a list of supported pre-processing tags.
/// </param>
/// <returns>
/// MgByteReader containing the previously updated or added tagged data.
/// </returns>
/// EXCEPTIONS:
/// MgRepositoryNotOpenException
///
/// MgResourceDataNotFoundException
/// MgInvalidResourceTypeException
///
MgByteReader* MgProxyResourceService::GetResourceData(
    MgResourceIdentifier* resource, CREFSTRING dataName,
    CREFSTRING preProcessTags)
{
    Ptr<MgByteReader> byteReader;

    MG_TRY()

    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgResourceService::opIdGetResourceData,
                        3,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knString, &dataName,
                        MgCommand::knString, &preProcessTags,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    byteReader = (MgByteReader*)cmd.GetReturnValue().val.m_obj;

    // Decrypt the document if Substitution pre-processing is required.

    if (MgResourcePreProcessingType::Substitution == preProcessTags
        && byteReader != NULL)
    {
        STRING mimeType = byteReader->GetByteSource()->GetMimeType();
        string cipherText, plainText;

        byteReader->ToStringUtf8(cipherText);

        MG_CRYPTOGRAPHY_TRY()

        MgCryptographyUtil cryptoUtil;

        cryptoUtil.DecryptString(cipherText, plainText);

        MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgProxyResourceService.GetResourceData")

        Ptr<MgByteSource> byteSource = new MgByteSource(
            (BYTE_ARRAY_IN)plainText.c_str(), (INT32)plainText.length());

        byteSource->SetMimeType(mimeType);
        byteReader = byteSource->GetReader();
    }

    MG_CATCH_AND_THROW(L"MgProxyResourceService.GetResourceData")

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Enumerates the tagged data for the specified resource.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the tags to enumerate
/// </param>
/// <returns>
/// MgByteReader object representing the description of the resource data.
/// </returns>
/// EXCEPTIONS:
/// MgRepositoryNotOpenException
///
/// MgInvalidResourceTypeException
MgByteReader* MgProxyResourceService::EnumerateResourceData(MgResourceIdentifier* resource)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgResourceService::opIdEnumerateResourceData,
                        1,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the content of the specified repository.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the repository
/// </param>
/// <returns>
/// MgByteReader object representing the repository content
/// </returns>
/// EXCEPTIONS:
/// MgRepositoryNotOpenException
/// MgRepositoryNotFoundException
/// MgInvalidRepositoryNameException
/// MgInvalidRepositoryTypeException
MgByteReader* MgProxyResourceService::GetRepositoryContent(MgResourceIdentifier* resource)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgResourceService::opIdGetRepositoryContent,
                        1,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the header of the specified repository.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the repository
/// </param>
/// <returns>
/// MgByteReader object representing the repository header
/// </returns>
/// EXCEPTIONS:
/// MgRepositoryNotOpenException
/// MgRepositoryNotFoundException
/// MgInvalidRepositoryNameException
/// MgInvalidRepositoryTypeException
MgByteReader* MgProxyResourceService::GetRepositoryHeader(MgResourceIdentifier* resource)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgResourceService::opIdGetRepositoryHeader,
                        1,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());
    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates all resources which reference the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgProxyResourceService::EnumerateReferences(
    MgResourceIdentifier* resource)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                       MgCommand::knObject,
                       MgResourceService::opIdEnumerateReferences,
                       1,
                       Resource_Service,
                       BUILD_VERSION(1,0,0),
                       MgCommand::knObject, resource,
                       MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the modified date of the specified resource.
/// </summary>
///
/// <exceptions>
/// MgInvalidRepositoryTypeException
/// MgInvalidResourceTypeException
/// </exceptions>
///----------------------------------------------------------------------------

MgDateTime* MgProxyResourceService::GetResourceModifiedDate(
    MgResourceIdentifier* resource)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgResourceService::opIdGetResourceModifiedDate,
                        1,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knObject, resource,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgDateTime*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerate all the parent Map Definition resources of the specified
/// resources.
///
MgSerializableCollection* MgProxyResourceService::EnumerateParentMapDefinitions(
    MgSerializableCollection* resources)
{
    MgCommand cmd;

    MG_TRY()

    assert(m_connProp != NULL);

    cmd.ExecuteCommand(
        m_connProp,                                         // Connection
        MgCommand::knObject,                                // Return type
        MgResourceService::opIdEnumerateParentMapDefinitions, // Command code
        1,                                                  // Number of arguments
        Resource_Service,                                   // Service ID
        BUILD_VERSION(1,0,0),                               // Operation version
        MgCommand::knObject, resources,                     // Argument #1
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    MG_CATCH_AND_THROW(L"MgProxyResourceService.EnumerateParentMapDefinitions")

    return (MgSerializableCollection*)cmd.GetReturnValue().val.m_obj;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerate the resource documents in the specified repository.
///
STRING MgProxyResourceService::EnumerateResourceDocuments(
    MgStringCollection* resources, CREFSTRING type, INT32 properties)
{
    STRING resourceList;
    MgCommand cmd;

    MG_TRY()

    cmd.ExecuteCommand(
        m_connProp,                                         // Connection
        MgCommand::knString,                                // Return type
        MgResourceService::opIdEnumerateResourceDocuments,  // Command code
        3,                                                  // Number of arguments
        Resource_Service,                                   // Service ID
        BUILD_VERSION(1,0,0),                               // Operation version
        MgCommand::knObject, resources,                     // Argument #1
        MgCommand::knString, &type,                         // Argument #2
        MgCommand::knInt32, properties,                     // Argument #3
        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    resourceList = *(cmd.GetReturnValue().val.m_str);
    delete cmd.GetReturnValue().val.m_str;

    MG_CATCH_AND_THROW(L"MgProxyResourceService.EnumerateResourceDocuments")

    return resourceList;
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Sets the connection properties for the Proxy Service.  This
/// information tells the proxy object where to connect.
///
/// \param connProp
/// Connection properties for server
///
void MgProxyResourceService::SetConnectionProperties(MgConnectionProperties* connProp)
{
    m_connProp = SAFE_ADDREF(connProp);
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Internal method to return user information for service
///
/// \return
/// MgUserInformation for service
MgUserInformation* MgProxyResourceService::GetUserInfo()
{
    return m_connProp->GetUserInfo();
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerates the unmanaged data
///
MgByteReader* MgProxyResourceService::EnumerateUnmanagedData(
    CREFSTRING path, bool recursive, CREFSTRING type, CREFSTRING filter)
{
    MgCommand cmd;

    cmd.ExecuteCommand(m_connProp,
                        MgCommand::knObject,
                        MgResourceService::opIdEnumerateUnmanagedData,
                        4,
                        Resource_Service,
                        BUILD_VERSION(1,0,0),
                        MgCommand::knString, &path,
                        MgCommand::knInt8, (int)recursive,
                        MgCommand::knString, &type,
                        MgCommand::knString, &filter,
                        MgCommand::knNone);

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}
