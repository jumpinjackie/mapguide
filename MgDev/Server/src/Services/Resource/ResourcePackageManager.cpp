//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "ResourceServiceDefs.h"
#include "ResourcePackageManager.h"
#include "PackageStatusLogManager.h"
#include "ByteSourceDwfInputStreamImpl.h"

static const STRING MgResourcePackageManifestFileName = L"MgResourcePackageManifest.xml";

// TODO: Share the following strings with the Web tier.

// Resource Service operation parameters

static const STRING MgOpParamType=L"TYPE";
static const STRING MgOpParamDepth=L"DEPTH";
static const STRING MgOpParamResourceId=L"RESOURCEID";
static const STRING MgOpParamResourceContent=L"CONTENT";
static const STRING MgOpParamResourceHeader=L"HEADER";
static const STRING MgOpParamSourceResourceId=L"SOURCE";
static const STRING MgOpParamDestinationResourceId=L"DESTINATION";
static const STRING MgOpParamDataName=L"DATANAME";
static const STRING MgOpParamDataType=L"DATATYPE";
static const STRING MgOpParamDataLength=L"DATALENGTH";
static const STRING MgOpParamData=L"DATA";
static const STRING MgOpParamOldDataName=L"OLDDATANAME";
static const STRING MgOpParamNewDataName=L"NEWDATANAME";
static const STRING MgOpParamOverwrite=L"OVERWRITE";
static const STRING MgOpParamPackage=L"PACKAGE";
static const STRING MgOpParamOwner=L"OWNER";
static const STRING MgOpParamIncludeDescendants=L"INCLUDEDESCENDANTS";

// Resource Service operations

static const STRING MgOpNameEnumerateRepositories=L"ENUMERATEREPOSITORIES";
static const STRING MgOpNameCreateRepository=L"CREATEREPOSITORY";
static const STRING MgOpNameDeleteRepository=L"DELETEREPOSITORY";
static const STRING MgOpNameUpdateRepository=L"UPDATEREPOSITORY";
static const STRING MgOpNameGetRepositoryContent=L"GETREPOSITORYCONTENT";
static const STRING MgOpNameGetRepositoryHeader=L"GETREPOSITORYHEADER";
static const STRING MgOpNameApplyResourcePackage=L"APPLYRESOURCEPACKAGE";

static const STRING MgOpNameEnumerateResources=L"ENUMERATERESOURCES";
static const STRING MgOpNameSetResource=L"SETRESOURCE";
static const STRING MgOpNameDeleteResource=L"DELETERESOURCE";
static const STRING MgOpNameMoveResource=L"MOVERESOURCE";
static const STRING MgOpNameCopyResource=L"COPYRESOURCE";
static const STRING MgOpNameGetResourceContent=L"GETRESOURCECONTENT";
static const STRING MgOpNameGetResourceHeader=L"GETRESOURCEHEADER";
static const STRING MgOpNameEnumerateResourceReferences=L"ENUMERATERESOURCEREFERENCES";
static const STRING MgOpNameChangeResourceOwner=L"CHANGERESOURCEOWNER";
static const STRING MgOpNameInheritPermissionsFrom=L"INHERITPERMISSIONSFROM";

static const STRING MgOpNameEnumerateResourceData=L"ENUMERATERESOURCEDATA";
static const STRING MgOpNameSetResourceData=L"SETRESOURCEDATA";
static const STRING MgOpNameDeleteResourceData=L"DELETERESOURCEDATA";
static const STRING MgOpNameRenameResourceData=L"RENAMERESOURCEDATA";
static const STRING MgOpNameGetResourceData=L"GETRESOURCEDATA";

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourcePackageManager::MgResourcePackageManager(
    MgLibraryRepositoryManager& repositoryManager, CREFSTRING pathname, bool logActivities) :
    m_repositoryManager(repositoryManager)
{
    MG_RESOURCE_SERVICE_TRY()

    if (logActivities)
    {
        m_packageLogManager = new MgPackageStatusLogManager(MgPackageStatus::NotStarted, pathname,
            MgFileUtil::GetLastModifiedDate(pathname), MgFileUtil::GetFileSizeInBytes(pathname));

        if (m_packageLogManager == NULL)
        {
            throw new MgOutOfMemoryException(L"MgResourcePackageManager.MgResourcePackageManager", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        m_packageLogManager = NULL;
    }

    m_zipFileHandler.reset(new MgZipFileHandler(pathname));

    MgOperationParameter opParam;

    opParam.SetValue(MgResourcePackageManifestFileName);
    opParam.SetContentType(MgMimeType::Xml);

    Ptr<MgByteReader> byteReader = CreateByteReader(opParam);

    byteReader->ToStringUtf8(m_packageManifest);
    m_manifestParser.Parse(m_packageManifest);

    MG_RESOURCE_SERVICE_CATCH(L"MgResourcePackageManager.MgResourcePackageManager")
    if (mgException != NULL)
    {
        if (m_packageLogManager != NULL)
        {
            m_packageLogManager->SetStatus(MgPackageStatus::Failed);
            m_packageLogManager->SetError(mgException->GetMessage());
            m_packageLogManager->SetErrorDetails(mgException->GetStackTrace());
            m_packageLogManager->UpdateLog();
        }
        (*mgException).AddRef();
        mgException->Raise();
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourcePackageManager::~MgResourcePackageManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a resource identifier based on the specified parameter.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceIdentifier* MgResourcePackageManager::CreateResourceIdentifier(
    const MgOperationParameter& opParam) const
{
    Ptr<MgResourceIdentifier> resource;

    MG_RESOURCE_SERVICE_TRY()

    if (!opParam.GetValue().empty())
    {
        resource = new MgResourceIdentifier(opParam.GetValue());
    }

    if (resource == NULL)
    {
        throw new MgNullArgumentException(
            L"MgResourcePackageManager.CreateResourceIdentifier",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Ensure the resource package is only applied to the Library repository.

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgResourcePackageManager.CreateResourceIdentifier",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.CreateResourceIdentifier")

    return SAFE_ADDREF((MgResourceIdentifier*)resource);
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a byte reader based on the specified parameter.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgResourcePackageManager::CreateByteReader(
    const MgOperationParameter& opParam, bool direct) const
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    if (!opParam.GetValue().empty())
    {
        DWFInputStream* inputStream = m_zipFileHandler->Unzip(
            opParam.GetValue());
        MgByteSourceDwfInputStreamImpl* byteSourceImpl =
            new MgByteSourceDwfInputStreamImpl(inputStream);
        Ptr<MgByteSource> byteSource = new MgByteSource(byteSourceImpl);

        byteReader = byteSource->GetReader();

        if (!direct && byteReader != NULL)
        {
            string strBuf;

            byteReader->ToStringUtf8(strBuf);
            byteSource = new MgByteSource(
                (unsigned char*)strBuf.c_str(), (INT32)strBuf.length());
            byteReader = byteSource->GetReader();
        }

        // TODO: Can the content type be validated?
        byteSource->SetMimeType(opParam.GetContentType());
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.CreateByteReader")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

///----------------------------------------------------------------------------
/// <summary>
/// Performs all the operations specified in the resource package.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::PerformOperations()
{
    INT32 index = 0;
    INT32 totalOperations = 0;

    MG_RESOURCE_SERVICE_TRY()

    if (m_packageLogManager != NULL)
    {
        m_packageLogManager->SetStatus(MgPackageStatus::Loading);
        m_packageLogManager->UpdateLog();
    }

    const MgOpInfoVector& opInfoVector = m_manifestParser.GetOperations();
    totalOperations = (INT32)opInfoVector.size();

    for (index = 0; index < totalOperations; ++index)
    {
        PerformOperation(opInfoVector[index]);
    }

    MG_RESOURCE_SERVICE_CATCH(L"MgResourcePackageManager.PerformOperations")

    if (m_packageLogManager != NULL)
    {
        if (mgException != NULL)
        {
            m_packageLogManager->SetStatus(MgPackageStatus::Failed);
            m_packageLogManager->SetError(mgException->GetMessage());
            m_packageLogManager->SetErrorDetails(mgException->GetStackTrace());
        }
        else
        {
            m_packageLogManager->SetStatus(MgPackageStatus::Success);
        }

        STRING message = MgUtil::GetResourceMessage(MgResources::ResourceService, L"MgPackageStatusLogManagerOpsComplete");
        if (message.empty())
        {
            message = MgResources::MgPackageStatusLogManagerOpsComplete;
        }
        MG_LOG_OPERATION_MESSAGE(message);
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L" ");
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(index);
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"/");
        MG_LOG_OPERATION_MESSAGE_ADD_INT32(totalOperations);
        m_packageLogManager->AddDetailsEntry(operationMessage);

        m_packageLogManager->UpdateLog();
    }

    if (mgException != NULL)
    {
        (*mgException).AddRef();
        mgException->Raise();
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Performs the specified operation.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::PerformOperation(const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    CREFSTRING opName = opInfo.GetName();

    if (MgOpNameUpdateRepository == opName)
    {
        UpdateRepository(opInfo);
    }
    else if (MgOpNameSetResource == opName)
    {
        SetResource(opInfo);
    }
    else if (MgOpNameDeleteResource == opName)
    {
        DeleteResource(opInfo);
    }
    else if (MgOpNameMoveResource == opName)
    {
        MoveResource(opInfo);
    }
    else if (MgOpNameCopyResource == opName)
    {
        CopyResource(opInfo);
    }
    else if (MgOpNameChangeResourceOwner == opName)
    {
        ChangeResourceOwner(opInfo);
    }
    else if (MgOpNameInheritPermissionsFrom == opName)
    {
        InheritPermissionsFrom(opInfo);
    }
    else if (MgOpNameSetResourceData == opName)
    {
        SetResourceData(opInfo);
    }
    else if (MgOpNameDeleteResourceData == opName)
    {
        DeleteResourceData(opInfo);
    }
    else if (MgOpNameRenameResourceData == opName)
    {
        RenameResourceData(opInfo);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.PerformOperation")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates an existing repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::UpdateRepository(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamResourceId));
    Ptr<MgByteReader> header = CreateByteReader(
        opInfo.GetParameter(MgOpParamResourceHeader, false), false);
    Ptr<MgByteReader> content = CreateByteReader(
        opInfo.GetParameter(MgOpParamResourceContent, (header == NULL)));

    if (m_packageLogManager != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"UpdateRepository");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceHeader, false)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceContent, (header == NULL))).GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
        m_packageLogManager->AddDetailsEntry(operationMessage);
    }

    m_repositoryManager.UpdateRepository(resource, content, header);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.UpdateRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a new resource to the repository or updates an existing resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::SetResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamResourceId));
    Ptr<MgByteReader> header = CreateByteReader(
        opInfo.GetParameter(MgOpParamResourceHeader, false), false);
    Ptr<MgByteReader> content = CreateByteReader(
        opInfo.GetParameter(MgOpParamResourceContent,
            resource->IsFolder() ? false : (header == NULL)));

    if (m_packageLogManager != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"SetResource");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceHeader, false)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceContent, resource->IsFolder() ? false : (header == NULL))).GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
        m_packageLogManager->AddDetailsEntry(operationMessage);
    }

    m_repositoryManager.SetResource(resource, content, header);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.SetResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes an existing resource from the opened resource repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::DeleteResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamResourceId));

    if (m_packageLogManager != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"DeleteResource");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
        m_packageLogManager->AddDetailsEntry(operationMessage);
    }

    m_repositoryManager.DeleteResource(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.DeleteResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Renames a resource and/or moves it from one location to another.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::MoveResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgResourceIdentifier> sourceResource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamSourceResourceId));
    Ptr<MgResourceIdentifier> destResource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamDestinationResourceId));
    bool overwrite = ACE_OS::atoi(opInfo.GetParameter(
        MgOpParamOverwrite, false).GetValue().c_str()) != 0;

    if (m_packageLogManager != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"MoveResource");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamDestinationResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(overwrite);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
        m_packageLogManager->AddDetailsEntry(operationMessage);
    }

    m_repositoryManager.MoveResource(sourceResource, destResource, overwrite);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.MoveResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Copies an existing resource to another location.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::CopyResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgResourceIdentifier> sourceResource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamSourceResourceId));
    Ptr<MgResourceIdentifier> destResource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamDestinationResourceId));
    bool overwrite = ACE_OS::atoi(opInfo.GetParameter(
        MgOpParamOverwrite, false).GetValue().c_str()) != 0;

    if (m_packageLogManager != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"CopyResource");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamSourceResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamDestinationResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(overwrite);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
        m_packageLogManager->AddDetailsEntry(operationMessage);
    }

    m_repositoryManager.CopyResource(sourceResource, destResource, overwrite);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.CopyResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Changes the owner of an existing resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::ChangeResourceOwner(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamResourceId));
    CREFSTRING owner = opInfo.GetParameter(MgOpParamOwner).GetValue();
    bool includeDescendants = ACE_OS::atoi(opInfo.GetParameter(
        MgOpParamIncludeDescendants, false).GetValue().c_str()) != 0;

    if (m_packageLogManager != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"ChangeResourceOwner");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(owner);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(includeDescendants);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
        m_packageLogManager->AddDetailsEntry(operationMessage);
    }

    m_repositoryManager.ChangeResourceOwner(resource, owner, includeDescendants);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.ChangeResourceOwner")
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the permissions for all descendants to be inherited from the
/// specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::InheritPermissionsFrom(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamResourceId));

    if (m_packageLogManager != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"InheritPermissionsFrom");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
        m_packageLogManager->AddDetailsEntry(operationMessage);
    }

    m_repositoryManager.InheritPermissionsFrom(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.InheritPermissionsFrom")
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets tagged data of a specific type to the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::SetResourceData(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamResourceId));
    CREFSTRING dataName = opInfo.GetParameter(MgOpParamDataName).GetValue();
    CREFSTRING dataType = opInfo.GetParameter(MgOpParamDataType).GetValue();
    Ptr<MgByteReader> data = CreateByteReader(
        opInfo.GetParameter(MgOpParamData));

    if (m_packageLogManager != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"SetResourceData");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataName);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataType);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamData)).GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
        m_packageLogManager->AddDetailsEntry(operationMessage);
    }

    m_repositoryManager.SetResourceData(resource, dataName, dataType, data);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.SetResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes tagged data from the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::DeleteResourceData(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamResourceId));
    CREFSTRING dataName = opInfo.GetParameter(MgOpParamDataName).GetValue();

    if (m_packageLogManager != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"DeleteResourceData");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataName);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
        m_packageLogManager->AddDetailsEntry(operationMessage);
    }

    m_repositoryManager.DeleteResourceData(resource, dataName);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.DeleteResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Renames tagged data for the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourcePackageManager::RenameResourceData(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(
        opInfo.GetParameter(MgOpParamResourceId));
    CREFSTRING oldDataName = opInfo.GetParameter(MgOpParamOldDataName).GetValue();
    CREFSTRING newDataName = opInfo.GetParameter(MgOpParamNewDataName).GetValue();
    bool overwrite = ACE_OS::atoi(opInfo.GetParameter(
        MgOpParamOverwrite, false).GetValue().c_str()) != 0;

    if (m_packageLogManager != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"RenameResourceData");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING((opInfo.GetParameter(MgOpParamResourceId)).GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(oldDataName);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(newDataName);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(overwrite);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();
        m_packageLogManager->AddDetailsEntry(operationMessage);
    }

    m_repositoryManager.RenameResourceData(resource, oldDataName, newDataName, overwrite);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageManager.RenameResourceData")
}
