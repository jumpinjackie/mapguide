#include "ResourceDefs.h"
#include "ResourcePackageLoader.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgdResourcePackageLoader::MgdResourcePackageLoader(
    MgResourceService& repositoryManager) :
    MgdResourcePackageHandler(repositoryManager)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgdResourcePackageLoader::~MgdResourcePackageLoader()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a resource identifier based on the specified parameter.
///
MgResourceIdentifier* MgdResourcePackageLoader::CreateResourceIdentifier(
    const MgdOperationParameter& opParam) const
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
            L"MgdResourcePackageLoader.CreateResourceIdentifier",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Ensure the resource package is only applied to the Library repository.

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgdResourcePackageLoader.CreateResourceIdentifier",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.CreateResourceIdentifier")

    return resource.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a byte reader based on the specified parameter.
///
MgByteReader* MgdResourcePackageLoader::CreateByteReader(
    const MgdOperationParameter& opParam, bool direct) const
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

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.CreateByteReader")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Starts loading the resource package.
///
void MgdResourcePackageLoader::Start(CREFSTRING packagePathname,
    bool logActivities)
{
    MG_RESOURCE_SERVICE_TRY()

    // Initialize the status information.
    InitializeStatus(MgdPackageApiName::LoadPackage, packagePathname,
        logActivities);

    // Create the package reader.
    m_zipFileReader.reset(new MgdZipFileReader(packagePathname));

    // Extract the resource package manifest and parse it.
    MgdOperationParameter opParam;

    opParam.SetValue(MgdResourcePackageManifestHandler::sm_manifestFileName);
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

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.Start")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Ends the resource package loading process.
///
void MgdResourcePackageLoader::End(MgException* except)
{
    MG_TRY()

    // Close the package file.
    m_zipFileReader.reset(NULL);

    /*
    if (m_packageLogWriter != NULL)
    {
        MgPackageStatusInformation& statusInfo = m_packageLogWriter->GetStatusInfo();

        statusInfo.SetPackageDescription(m_manifestParser.GetDescription());
    }*/

    // Update the status information.
    UpdateStatus(except);

    MG_CATCH_AND_RELEASE()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Performs the specified operation.
///
void MgdResourcePackageLoader::PerformOperation(const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    CREFSTRING opName = opInfo.GetName();

    if (MgdOperationName::UpdateRepository == opName)
    {
        UpdateRepository(opInfo);
    }
    else if (MgdOperationName::SetResource == opName)
    {
        SetResource(opInfo);
    }
    else if (MgdOperationName::DeleteResource == opName)
    {
        DeleteResource(opInfo);
    }
    else if (MgdOperationName::MoveResource == opName)
    {
        MoveResource(opInfo);
    }
    else if (MgdOperationName::CopyResource == opName)
    {
        CopyResource(opInfo);
    }
    else if (MgdOperationName::ChangeResourceOwner == opName)
    {
        ChangeResourceOwner(opInfo);
    }
    else if (MgdOperationName::InheritPermissionsFrom == opName)
    {
        InheritPermissionsFrom(opInfo);
    }
    else if (MgdOperationName::SetResourceData == opName)
    {
        SetResourceData(opInfo);
    }
    else if (MgdOperationName::DeleteResourceData == opName)
    {
        DeleteResourceData(opInfo);
    }
    else if (MgdOperationName::RenameResourceData == opName)
    {
        RenameResourceData(opInfo);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.PerformOperation")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Updates an existing repository.
///
void MgdResourcePackageLoader::UpdateRepository(
    const MgOperationInfo& opInfo)
{
    //NOOP. mg-desktop doesn't implement this operation
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Adds a new resource to the repository or updates an existing resource.
///
void MgdResourcePackageLoader::SetResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgdOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    const MgdOperationParameter& resourceHeaderParam = opInfo.GetParameter(
        MgdOperationParameter::ResourceHeader, false);
    Ptr<MgByteReader> header = CreateByteReader(resourceHeaderParam, false);

    const MgdOperationParameter& resourceContentParam = opInfo.GetParameter(
        MgdOperationParameter::ResourceContent, resource->IsFolder() ? false : (header == NULL));
    Ptr<MgByteReader> content = CreateByteReader(resourceContentParam);

    /*
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
    }*/

    m_repositoryManager.SetResource(resource, content, header);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.SetResource")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deletes an existing resource from the opened resource repository.
///
void MgdResourcePackageLoader::DeleteResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgdOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    /*
    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"DeleteResource");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }*/

    m_repositoryManager.DeleteResource(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.DeleteResource")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Renames a resource and/or moves it from one location to another.
///
void MgdResourcePackageLoader::MoveResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgdOperationParameter& srcResourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::SourceResourceId);
    Ptr<MgResourceIdentifier> srcResource = CreateResourceIdentifier(srcResourceIdParam);

    const MgdOperationParameter& destResourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::DestinationResourceId);
    Ptr<MgResourceIdentifier> destResource = CreateResourceIdentifier(destResourceIdParam);

    bool overwrite = ACE_OS::atoi(opInfo.GetParameter(
        MgdOperationParameter::Overwrite, false).GetValue().c_str()) != 0;

    CREFSTRING paramCascade = opInfo.GetParameter(MgdOperationParameter::Overwrite, false).GetValue();
    bool cascade = !paramCascade.empty() && (ACE_OS::atoi(paramCascade.c_str()) != 0);

    /*
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
    }*/

    m_repositoryManager.MoveResource(srcResource, destResource, overwrite, cascade);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.MoveResource")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Copies an existing resource to another location.
///
void MgdResourcePackageLoader::CopyResource(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgdOperationParameter& srcResourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::SourceResourceId);
    Ptr<MgResourceIdentifier> srcResource = CreateResourceIdentifier(srcResourceIdParam);

    const MgdOperationParameter& destResourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::DestinationResourceId);
    Ptr<MgResourceIdentifier> destResource = CreateResourceIdentifier(destResourceIdParam);

    bool overwrite = ACE_OS::atoi(opInfo.GetParameter(
        MgdOperationParameter::Overwrite, false).GetValue().c_str()) != 0;

    /*
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
    }*/

    m_repositoryManager.CopyResource(srcResource, destResource, overwrite);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.CopyResource")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Changes the owner of an existing resource.
///
void MgdResourcePackageLoader::ChangeResourceOwner(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgdOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    CREFSTRING owner = opInfo.GetParameter(MgdOperationParameter::Owner).GetValue();
    bool includeDescendants = ACE_OS::atoi(opInfo.GetParameter(
        MgdOperationParameter::IncludeDescendants, false).GetValue().c_str()) != 0;

    /*
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
    }*/

    m_repositoryManager.ChangeResourceOwner(resource, owner, includeDescendants);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.ChangeResourceOwner")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the permissions for all descendants to be inherited from the
/// specified resource.
///
void MgdResourcePackageLoader::InheritPermissionsFrom(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgdOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    /*
    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"InheritPermissionsFrom");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }*/

    m_repositoryManager.InheritPermissionsFrom(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.InheritPermissionsFrom")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets tagged data of a specific type to the specified resource.
///
void MgdResourcePackageLoader::SetResourceData(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgdOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    CREFSTRING dataName = opInfo.GetParameter(MgdOperationParameter::DataName).GetValue();
    CREFSTRING dataType = opInfo.GetParameter(MgdOperationParameter::DataType).GetValue();

    const MgdOperationParameter& resourceDataParam = opInfo.GetParameter(
        MgdOperationParameter::Data);
    Ptr<MgByteReader> data = CreateByteReader(resourceDataParam);

    /*
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
    }*/

    m_repositoryManager.SetResourceData(resource, dataName, dataType, data);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.SetResourceData")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deletes tagged data from the specified resource.
///
void MgdResourcePackageLoader::DeleteResourceData(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgdOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    CREFSTRING dataName = opInfo.GetParameter(MgdOperationParameter::DataName).GetValue();

    /*
    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"DeleteResourceData");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourceIdParam.GetValue());
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataName);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }*/

    m_repositoryManager.DeleteResourceData(resource, dataName);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.DeleteResourceData")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Renames tagged data for the specified resource.
///
void MgdResourcePackageLoader::RenameResourceData(
    const MgOperationInfo& opInfo)
{
    MG_RESOURCE_SERVICE_TRY()

    const MgdOperationParameter& resourceIdParam = opInfo.GetParameter(
        MgdOperationParameter::ResourceId);
    Ptr<MgResourceIdentifier> resource = CreateResourceIdentifier(resourceIdParam);

    CREFSTRING oldDataName = opInfo.GetParameter(MgdOperationParameter::OldDataName).GetValue();
    CREFSTRING newDataName = opInfo.GetParameter(MgdOperationParameter::NewDataName).GetValue();
    bool overwrite = ACE_OS::atoi(opInfo.GetParameter(
        MgdOperationParameter::Overwrite, false).GetValue().c_str()) != 0;

    /*
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
    }*/

    m_repositoryManager.RenameResourceData(resource, oldDataName, newDataName, overwrite);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdResourcePackageLoader.RenameResourceData")
}
