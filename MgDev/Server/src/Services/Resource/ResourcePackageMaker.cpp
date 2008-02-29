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

#include "ResourceServiceDefs.h"
#include "ResourcePackageMaker.h"
#include "PackageManager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgResourcePackageMaker::MgResourcePackageMaker(
    MgLibraryRepositoryManager& repositoryManager) :
    MgResourcePackageHandler(repositoryManager)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgResourcePackageMaker::~MgResourcePackageMaker()
{
    for (MgResourceOperationMap::const_iterator i = m_resourceOperationMap.begin();
        i != m_resourceOperationMap.end(); ++i)
    {
        delete (*i).second;
    }

    m_resourceOperationMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Generates unique resource and archive pathnames.
///
void MgResourcePackageMaker::GeneratePathnames(MgResourceIdentifier& resource,
    CREFSTRING postfix, REFSTRING resourcePathname, REFSTRING archivePathname) const
{
    // Ensure the resource package is only made from the Library repository.

    if (!resource.IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgResourcePackageMaker.GeneratePathnames",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    resourcePathname = resource.ToString();

    size_t startIndex = resource.GetRootPath().length();
    size_t endIndex = resourcePathname.length() - startIndex;

    archivePathname = resource.GetRepositoryType();
    archivePathname += L"/";

    if (!resource.GetRepositoryName().empty())
    {
       archivePathname += resource.GetRepositoryName();
       archivePathname += L"/";
    }

    archivePathname += resourcePathname.substr(startIndex, endIndex);
    archivePathname += L"_";
    archivePathname += postfix;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Starts making the resource package.
///
void MgResourcePackageMaker::Start(MgResourceIdentifier& resource,
    CREFSTRING packagePathname, CREFSTRING packageDescription,
    bool logActivities)
{
    MG_RESOURCE_SERVICE_TRY()

    // Initialize the status information.
    InitializeStatus(MgPackageApiName::MakePackage, packagePathname,
        logActivities);

    if (m_packageLogWriter != NULL)
    {
        MgPackageStatusInformation& statusInfo = m_packageLogWriter->GetStatusInfo();

        statusInfo.SetPackageDescription(packageDescription);
    }

    // Create the package reader.
    m_zipFileWriter.reset(new MgZipFileWriter(packagePathname));

    // Initialize the resource package manifest serializer.
    m_manifestSerializer.Initialize(packageDescription);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageMaker.Start")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Ends the resource package making process.
///
void MgResourcePackageMaker::End(MgException* except)
{
    MG_TRY()

    if (NULL == except)
    {
        // Package the resource package manifest.
        Ptr<MgByteReader> reader = m_manifestSerializer.ToByteReader();
        m_zipFileWriter->AddArchive(
            MgResourcePackageManifestHandler::sm_manifestFileName,
            reader);

        // Close the package file.
        m_zipFileWriter.reset(NULL);

        // Update the status information.
        UpdateStatus(except);
    }
    else
    {
        // Close the package file.
        m_zipFileWriter.reset(NULL);

        // Close the package log.
        m_packageLogWriter = NULL;

        MgPackageManager* packageManager = MgPackageManager::GetInstance();
        ACE_ASSERT(NULL != packageManager);

        // Delete the package file and log.
        packageManager->DeletePackage(packageManager->GetPackageName(m_packagePathname));
    }

    MG_CATCH_AND_RELEASE()
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Packages the specified resource header.
///
void MgResourcePackageMaker::PackageResourceHeader(
    MgResourceIdentifier& resource, const XmlDocument& xmlDoc)
{
    MG_RESOURCE_SERVICE_TRY()

    ++m_opsReceived;

    // Generate unique pathnames.
    STRING resourcePathname, archivePathname;
    STRING postfix = MgOperationParameter::ResourceHeader;
    postfix += MgFileExtension::Xml;

    GeneratePathnames(resource, postfix, resourcePathname, archivePathname);

    // Initialize the operation information.
    auto_ptr<MgOperationInfo> opInfo;

    if (resource.IsRoot())
    {
        opInfo.reset(new MgOperationInfo(MgOperationName::UpdateRepository));
    }
    else
    {
        opInfo.reset(new MgOperationInfo(MgOperationName::SetResource));
    }

    MgOperationParameter opParam;

    opParam.SetValue(resourcePathname);
    opInfo->AddParameter(MgOperationParameter::ResourceId, opParam);

    opParam.SetValue(archivePathname);
    opParam.SetContentType(MgMimeType::Xml);
    opInfo->AddParameter(MgOperationParameter::ResourceHeader, opParam);

    // Note that the resource header is packaged before the resource content
    // so that resource permissions could be checked efficiently.

    if (resource.IsFolder())
    {
        // Serialize the operation information to the resource package manifest.
        m_manifestSerializer.Serialize(*opInfo);

        if (resource.IsRoot())
        {
            opInfo->RemoveParameter(MgOperationParameter::ResourceHeader);

            // Add the operation information to the resource operation map.
            // This operation will be added to the resource package manifest later
            // when packaging the resource content.
            m_resourceOperationMap.insert(MgResourceOperationMap::value_type(
                resourcePathname, opInfo.release()));
        }
    }
    else
    {
        // Add the operation information to the resource operation map.
        // This operation will be added to the resource package manifest later
        // when packaging the resource content.
        m_resourceOperationMap.insert(MgResourceOperationMap::value_type(
            resourcePathname, opInfo.release()));
    }

    // Package the resource header.
    m_zipFileWriter->AddArchive(archivePathname, xmlDoc);

    // Log the operation.
    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"PackageResourceHeader");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourcePathname);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    ++m_opsSucceeded;

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageMaker.PackageResourceHeader")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Packages the specified resource content.
///
bool MgResourcePackageMaker::PackageResourceContent(
    MgResourceIdentifier& resource, const XmlDocument& xmlDoc)
{
    MG_RESOURCE_SERVICE_TRY()

    ACE_ASSERT(!resource.IsFolder() || resource.IsRoot());
    MgResourceOperationMap::const_iterator i =
        m_resourceOperationMap.find(resource.ToString());

    // The resource not in the resource operation map means the current user
    // has no read pemission to it.
    if (m_resourceOperationMap.end() == i)
    {
        return false;
    }

    ++m_opsReceived;

    // Generate unique pathnames.
    STRING resourcePathname, archivePathname;
    STRING postfix = MgOperationParameter::ResourceContent;
    postfix += MgFileExtension::Xml;

    GeneratePathnames(resource, postfix, resourcePathname, archivePathname);

    // All resource contents, except the root and subfolders, must be deleted
    // so that all resource data are wiped out cleanly.
    if (!resource.IsFolder())
    {
        // Initialize the operation information.
        auto_ptr<MgOperationInfo> opInfo;
        opInfo.reset(new MgOperationInfo(MgOperationName::DeleteResource));
        MgOperationParameter opParam;

        opParam.SetValue(resourcePathname);
        opInfo->AddParameter(MgOperationParameter::ResourceId, opParam);

        // Serialize the operation information to the resource package manifest.
        m_manifestSerializer.Serialize(*opInfo);
    }

    // Initialize the operation information.
    MgOperationInfo* opInfo = (*i).second;
    MgOperationParameter opParam;

    opParam.SetValue(archivePathname);
    opParam.SetContentType(MgMimeType::Xml);
    opInfo->AddParameter(MgOperationParameter::ResourceContent, opParam);

    // Serialize the operation information to the resource package manifest.
    m_manifestSerializer.Serialize(*opInfo);

    // Package the resource content.
    m_zipFileWriter->AddArchive(archivePathname, xmlDoc);

    // Log the operation.
    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"PackageResourceContent");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourcePathname);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    ++m_opsSucceeded;

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageMaker.PackageResourceContent")

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Packages the specified resource data.
///
void MgResourcePackageMaker::PackageResourceData(
    MgResourceIdentifier& resource, MgByteReader* byteReader,
    CREFSTRING dataName, CREFSTRING dataType)
{
    MG_RESOURCE_SERVICE_TRY()

    ACE_ASSERT(!resource.IsFolder());

    if (NULL == byteReader)
    {
        throw new MgNullArgumentException(
            L"MgResourcePackageMaker.PackageResourceData",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ++m_opsReceived;

    // Generate unique pathnames.
    STRING resourcePathname, archivePathname;
    STRING postfix = MgOperationParameter::Data;
    postfix += L"_";
    postfix += dataName;

    GeneratePathnames(resource, postfix, resourcePathname, archivePathname);

    // Initialize the operation information.
    auto_ptr<MgOperationInfo> opInfo;
    opInfo.reset(new MgOperationInfo(MgOperationName::SetResourceData));
    MgOperationParameter opParam;

    opParam.SetValue(resourcePathname);
    opInfo->AddParameter(MgOperationParameter::ResourceId, opParam);

    opParam.SetValue(dataName);
    opInfo->AddParameter(MgOperationParameter::DataName, opParam);

    opParam.SetValue(dataType);
    opInfo->AddParameter(MgOperationParameter::DataType, opParam);

    STRING dataLength;
    MgUtil::Int64ToString(byteReader->GetLength(), dataLength);

    opParam.SetValue(dataLength);
    opInfo->AddParameter(MgOperationParameter::DataLength, opParam);

    opParam.SetValue(archivePathname);
    opParam.SetContentType(byteReader->GetMimeType());
    opInfo->AddParameter(MgOperationParameter::Data, opParam);

    // Serialize the operation information to the resource package manifest.
    m_manifestSerializer.Serialize(*opInfo);

    // Package the resource data.
    m_zipFileWriter->AddArchive(archivePathname, byteReader);

    // Log the operation.
    if (m_packageLogWriter != NULL)
    {
        MG_LOG_OPERATION_MESSAGE(L"PackageResourceData");
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(resourcePathname);
        MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(dataName);
        MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

        m_packageLogWriter->AddOperation(operationMessage);
    }

    ++m_opsSucceeded;

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourcePackageMaker.PackageResourceData")
}
