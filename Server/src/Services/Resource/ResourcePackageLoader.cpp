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

#include "ResourceServiceDefs.h"
#include "ResourcePackageLoader.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgResourcePackageLoader::MgResourcePackageLoader(
    MgLibraryRepositoryManager& repositoryManager) :
    MgResourcePackageHandler(repositoryManager)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgResourcePackageLoader::~MgResourcePackageLoader()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a resource identifier based on the specified parameter.
///
MgResourceIdentifier* MgResourcePackageLoader::CreateResourceIdentifier(
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
            L"MgResourcePackageLoader.CreateResourceIdentifier",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Ensure the resource package is only applied to the Library repository.

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgResourcePackageLoader.CreateResourceIdentifier",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.CreateResourceIdentifier")

    return resource.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a byte reader based on the specified parameter.
///
MgByteReader* MgResourcePackageLoader::CreateByteReader(
    const MgOperationParameter& opParam, bool direct) const
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    if (!opParam.GetValue().empty())
    {
        Ptr<MgByteSource> byteSource = m_zipFileReader->ExtractArchive(
            opParam.GetValue());

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

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.CreateByteReader")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Starts loading the resource package.
///
void MgResourcePackageLoader::Start(CREFSTRING packagePathname,
    bool logActivities)
{
    MG_RESOURCE_SERVICE_TRY()

    // Initialize the status information.
    InitializeStatus(MgPackageApiName::LoadPackage, packagePathname,
        logActivities);

    // Create the package reader.
    m_zipFileReader.reset(new MgZipFileReader(packagePathname));

    // Extract the resource package manifest and parse it.
    MgOperationParameter opParam;

    opParam.SetValue(MgResourcePackageManifestHandler::sm_manifestFileName);
    opParam.SetContentType(MgMimeType::Xml);

    string manifestXmlDoc;
    Ptr<MgByteReader> byteReader = CreateByteReader(opParam);

    byteReader->ToStringUtf8(manifestXmlDoc);
    m_manifestParser.Parse(manifestXmlDoc);

    // Performs all the operations specified in the resource package.
    const MgOpInfoVector& opInfoVector = m_manifestParser.GetOperations();
    m_opsReceived = (INT32)opInfoVector.size();

    for (m_opsSucceeded = 0; m_opsSucceeded < m_opsReceived; ++m_opsSucceeded)
    {
        PerformOperation(opInfoVector[m_opsSucceeded]);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.Start")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Ends the resource package loading process.
///
void MgResourcePackageLoader::End(MgException* except)
{
    MG_TRY()

    // Close the package file.
    m_zipFileReader.reset(NULL);

    if (m_packageLogWriter != NULL)
    {
        MgPackageStatusInformation& statusInfo = m_packageLogWriter->GetStatusInfo();

        statusInfo.SetPackageDescription(m_manifestParser.GetDescription());
    }

    // Update the status information.
    UpdateStatus(except);

    MG_CATCH_AND_RELEASE()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Performs the specified operation.
///
void MgResourcePackageLoader::PerformOperation(const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    CREFSTRING opName = opInfo.GetName();

    if (MgOperationName::UpdateRepository == opName)
    {
        UpdateRepository(opInfo);
    }
    else if (MgOperationName::SetResource == opName)
    {
        SetResource(opInfo);
    }
    else if (MgOperationName::DeleteResource == opName)
    {
        DeleteResource(opInfo);
    }
    else if (MgOperationName::MoveResource == opName)
    {
        MoveResource(opInfo);
    }
    else if (MgOperationName::CopyResource == opName)
    {
        CopyResource(opInfo);
    }
    else if (MgOperationName::ChangeResourceOwner == opName)
    {
        ChangeResourceOwner(opInfo);
    }
    else if (MgOperationName::InheritPermissionsFrom == opName)
    {
        InheritPermissionsFrom(opInfo);
    }
    else if (MgOperationName::SetResourceData == opName)
    {
        SetResourceData(opInfo);
    }
    else if (MgOperationName::DeleteResourceData == opName)
    {
        DeleteResourceData(opInfo);
    }
    else if (MgOperationName::RenameResourceData == opName)
    {
        RenameResourceData(opInfo);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.PerformOperation")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Updates an existing repository.
///
void MgResourcePackageLoader::UpdateRepository(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    const MgOperationParameter& resourceHeaderParam = opInfo.GetParameter(
        MgOperationParameter::ResourceHeader, false);
    Ptr<MgByteReader> header = CreateByteReader(resourceHeaderParam, false);

    const MgOperationParameter& resourceContentParam = opInfo.GetParameter(
        MgOperationParameter::ResourceContent, (header == NULL));
    Ptr<MgByteReader> content = CreateByteReader(resourceContentParam);

    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"UpdateRepository");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceContentParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceHeaderParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    m_repositoryManager.UpdateRepository(resource, content, header);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.UpdateRepository")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Adds a new resource to the repository or updates an existing resource.
///
void MgResourcePackageLoader::SetResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    const MgOperationParameter& resourceHeaderParam = opInfo.GetParameter(
        MgOperationParameter::ResourceHeader, false);
    Ptr<MgByteReader> header = CreateByteReader(resourceHeaderParam, false);

    const MgOperationParameter& resourceContentParam = opInfo.GetParameter(
        MgOperationParameter::ResourceContent, resource->IsFolder() ? false : (header == NULL));
    Ptr<MgByteReader> content = CreateByteReader(resourceContentParam);

    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"SetResource");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceContentParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceHeaderParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    m_repositoryManager.SetResource(resource, content, header);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.SetResource")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deletes an existing resource from the opened resource repository.
///
void MgResourcePackageLoader::DeleteResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"DeleteResource");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    m_repositoryManager.DeleteResource(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.DeleteResource")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Renames a resource and/or moves it from one location to another.
///
void MgResourcePackageLoader::MoveResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgOperationParameter& srcResourceIdParam = opInfo.GetParameter(
        MgOperationParameter::SourceResourceId);
    Ptr<MgResourceIdentifier> srcResource = CreateResourceIdentifier(srcResourceIdParam);

    const MgOperationParameter& destResourceIdParam = opInfo.GetParameter(
        MgOperationParameter::DestinationResourceId);
    Ptr<MgResourceIdentifier> destResource = CreateResourceIdentifier(destResourceIdParam);

    bool overwrite = ACE_OS::atoi(opInfo.GetParameter(
        MgOperationParameter::Overwrite, false).GetValue().c_str()) != 0;

    CREFSTRING paramCascade = opInfo.GetParameter(MgOperationParameter::Overwrite, false).GetValue();
    bool cascade = !paramCascade.empty() && (ACE_OS::atoi(paramCascade.c_str()) != 0);

    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"MoveResource");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(srcResourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(destResourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(overwrite);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    m_repositoryManager.MoveResource(srcResource, destResource, overwrite, cascade);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.MoveResource")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Copies an existing resource to another location.
///
void MgResourcePackageLoader::CopyResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgOperationParameter& srcResourceIdParam = opInfo.GetParameter(
        MgOperationParameter::SourceResourceId);
    Ptr<MgResourceIdentifier> srcResource = CreateResourceIdentifier(srcResourceIdParam);

    const MgOperationParameter& destResourceIdParam = opInfo.GetParameter(
        MgOperationParameter::DestinationResourceId);
    Ptr<MgResourceIdentifier> destResource = CreateResourceIdentifier(destResourceIdParam);

    bool overwrite = ACE_OS::atoi(opInfo.GetParameter(
        MgOperationParameter::Overwrite, false).GetValue().c_str()) != 0;

    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"CopyResource");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(srcResourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(destResourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(overwrite);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    m_repositoryManager.CopyResource(srcResource, destResource, overwrite);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.CopyResource")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Changes the owner of an existing resource.
///
void MgResourcePackageLoader::ChangeResourceOwner(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    CREFSTRING owner = opInfo.GetParameter(MgOperationParameter::Owner).GetValue();
    bool includeDescendants = ACE_OS::atoi(opInfo.GetParameter(
        MgOperationParameter::IncludeDescendants, false).GetValue().c_str()) != 0;

    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"ChangeResourceOwner");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(owner);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(includeDescendants);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    m_repositoryManager.ChangeResourceOwner(resource, owner, includeDescendants);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.ChangeResourceOwner")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the permissions for all descendants to be inherited from the
/// specified resource.
///
void MgResourcePackageLoader::InheritPermissionsFrom(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"InheritPermissionsFrom");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    m_repositoryManager.InheritPermissionsFrom(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.InheritPermissionsFrom")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets tagged data of a specific type to the specified resource.
///
void MgResourcePackageLoader::SetResourceData(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    CREFSTRING dataName = opInfo.GetParameter(MgOperationParameter::DataName).GetValue();
    CREFSTRING dataType = opInfo.GetParameter(MgOperationParameter::DataType).GetValue();

    const MgOperationParameter& resourceDataParam = opInfo.GetParameter(
        MgOperationParameter::Data);
    Ptr<MgByteReader> data = CreateByteReader(resourceDataParam);

    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"SetResourceData");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataName);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataType);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceDataParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    m_repositoryManager.SetResourceData(resource, dataName, dataType, data);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.SetResourceData")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deletes tagged data from the specified resource.
///
void MgResourcePackageLoader::DeleteResourceData(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    CREFSTRING dataName = opInfo.GetParameter(MgOperationParameter::DataName).GetValue();

    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"DeleteResourceData");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataName);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    m_repositoryManager.DeleteResourceData(resource, dataName);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.DeleteResourceData")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Renames tagged data for the specified resource.
///
void MgResourcePackageLoader::RenameResourceData(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    CREFSTRING oldDataName = opInfo.GetParameter(MgOperationParameter::OldDataName).GetValue();
    CREFSTRING newDataName = opInfo.GetParameter(MgOperationParameter::NewDataName).GetValue();
    bool overwrite = ACE_OS::atoi(opInfo.GetParameter(
        MgOperationParameter::Overwrite, false).GetValue().c_str()) != 0;

    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"RenameResourceData");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(oldDataName);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(newDataName);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_BOOL(overwrite);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    m_repositoryManager.RenameResourceData(resource, oldDataName, newDataName, overwrite);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageLoader.RenameResourceData")
}
