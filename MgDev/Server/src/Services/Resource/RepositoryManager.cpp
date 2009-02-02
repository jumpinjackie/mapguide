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

#include "ResourceServiceDefs.h"
#include "RepositoryManager.h"
#include "SessionRepository.h"
#include "ApplicationRepositoryManager.h"
#include "ResourceContentManager.h"
#include "ResourceHeaderManager.h"
#include "ResourceInfo.h"
#include "DataBindingInfo.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgRepositoryManager::MgRepositoryManager(
    MgRepository& repository) :
    m_repository(repository),
    m_transacted(false),
    m_currUserIsAdmin(false),
    m_currUserIsAuthor(false),
    m_dbTxn(NULL),
    m_xmlTxn(NULL)
{
    // Set the source and destination repository managers are the same by default.
    m_sourceRepositoryMan = this;

    ::time(&m_accessedTime);
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgRepositoryManager::~MgRepositoryManager()
{
    try
    {
        AbortTransaction();
    }
    catch (...)
    {
        assert(false);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the accessed time.
/// </summary>
///----------------------------------------------------------------------------

XmlValue MgRepositoryManager::GetAccessedTime() const
{
    MgDateTime dateTime(m_accessedTime);

    return XmlValue(XmlValue::DATE_TIME, dateTime.ToXmlStringUtf8());
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns all the database or log files in the specified environment.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgRepositoryManager::GetFiles(CREFSTRING repositoryType,
    DbEnv& dbEnv, u_int32_t flags)
{
    Ptr<MgStringCollection> files;
    char **begin = NULL;

    MG_RESOURCE_SERVICE_TRY()

    char **list = NULL;

    try
    {
        dbEnv.log_archive(&list, flags);
    }
    catch (DbException& e)
    {
        if (DB_NOTFOUND == e.get_errno())
        {
            // There is no database or log file.
            return NULL;
        }
        else
        {
            throw e;
        }
    }

    if (NULL != list)
    {
        STRING wcFile;

        files = new MgStringCollection;

        for (begin = list; *list != NULL; ++list)
        {
            string mbFile = *list;

            if (flags & DB_ARCH_DATA)
            {
                bool valid = false;

                if (MgRepositoryType::Session == repositoryType)
                {
                    valid = (MgRepository::SessionResourceContentContainerName   == mbFile
                          || MgRepository::SessionResourceDataStreamDatabaseName == mbFile);
                }
                else if (MgRepositoryType::Site == repositoryType)
                {
                    valid = (MgRepository::SiteResourceContentContainerName      == mbFile);
                }
                else if (MgRepositoryType::Library == repositoryType)
                {
                    valid = (MgRepository::LibraryResourceContentContainerName   == mbFile
                          || MgRepository::LibraryResourceHeaderContainerName    == mbFile
                          || MgRepository::LibraryResourceDataStreamDatabaseName == mbFile);
                }

                if (!valid)
                {
                    throw new MgInvalidRepositoryTypeException(
                        L"MgRepositoryManager.GetFiles", __LINE__, __WFILE__, NULL, L"", NULL);
                }
            }

            MgUtil::MultiByteToWideChar(mbFile, wcFile);
            files->Add(wcFile);
        }

    }

    MG_RESOURCE_SERVICE_CATCH(L"MgRepositoryManager.GetFiles")

    ::free(begin);

    MG_RESOURCE_SERVICE_THROW()

    return files.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns all the database files from the specified repository.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgRepositoryManager::GetDatabaseFiles(CREFSTRING repositoryType,
    CREFSTRING repositoryPath, bool fullPath)
{
    Ptr<MgStringCollection> files = new MgStringCollection;
    STRING dir = repositoryPath;

    MgFileUtil::AppendSlashToEndOfPath(dir);

    if (MgRepositoryType::Session == repositoryType)
    {
        STRING file, pathname;

        MgUtil::MultiByteToWideChar(
            MgRepository::SessionResourceContentContainerName, file);
        pathname = dir + file;

        if (MgFileUtil::PathnameExists(pathname))
        {
            files->Add(fullPath ? pathname : file);
        }

        MgUtil::MultiByteToWideChar(
            MgRepository::SessionResourceDataStreamDatabaseName, file);
        pathname = dir + file;

        if (MgFileUtil::PathnameExists(pathname))
        {
            files->Add(fullPath ? pathname : file);
        }
    }
    else
    {
        assert(false);
    }

    return files.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns all the database files in the specified environment.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgRepositoryManager::GetDatabaseFiles(CREFSTRING repositoryType,
    DbEnv& dbEnv, bool fullPath)
{
    u_int32_t flags = DB_ARCH_DATA;

    if (fullPath)
    {
        flags |= DB_ARCH_ABS;
    }

    return GetFiles(repositoryType, dbEnv, flags);
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns all the log files in the specified environment.
/// </summary>
///----------------------------------------------------------------------------

MgStringCollection* MgRepositoryManager::GetLogFiles(CREFSTRING repositoryType,
    DbEnv& dbEnv, bool fullPath)
{
    u_int32_t flags = DB_ARCH_LOG;

    if (fullPath)
    {
        flags |= DB_ARCH_ABS;
    }

    return GetFiles(repositoryType, dbEnv, flags);
}

///----------------------------------------------------------------------------
/// <summary>
/// Cleans up a repository. All database files, log files, backing files, and
/// data files will be removed.
/// </summary>
///----------------------------------------------------------------------------

void MgRepositoryManager::CleanRepository(CREFSTRING repositoryType)
{
    if (MgRepositoryType::Session != repositoryType)
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgRepositoryManager.CleanRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    STRING repositoryPath, resourceDataFilePath;
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    assert(NULL != configuration);

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertySessionRepositoryPath,
        repositoryPath,
        MgConfigProperties::DefaultResourceServicePropertySessionRepositoryPath);

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertySessionResourceDataFilePath,
        resourceDataFilePath,
        MgConfigProperties::DefaultResourceServicePropertySessionResourceDataFilePath);

    // Check to see whether or not it is safe to open the database.

    MgSessionRepository::VerifyAccess(repositoryPath);

    // Remove all the database files.

    string home;
    MgUtil::WideCharToMultiByte(repositoryPath, home);
    Ptr<MgStringCollection> logFiles;

    {
        MgDbEnvironment environment(repositoryType, home);
        DbTxn* dbTxn = NULL;

        MG_RESOURCE_SERVICE_TRY()

        DbEnv& dbEnv = environment.GetDbEnv();

        if (environment.IsTransacted())
        {
            dbEnv.txn_begin(0, &dbTxn, 0);
            assert(NULL != dbTxn);
        }

        logFiles = GetLogFiles(repositoryType, dbEnv, true);
        Ptr<MgStringCollection> databaseFiles = GetDatabaseFiles(
            repositoryType, dbEnv, false);

        if (databaseFiles == NULL)
        {
            databaseFiles = GetDatabaseFiles(repositoryType, repositoryPath, false);
        }

        if (databaseFiles != NULL)
        {
            for (INT32 i = 0; i < databaseFiles->GetCount(); ++i)
            {
                string file;

                MgUtil::WideCharToMultiByte(databaseFiles->GetItem(i), file);
                dbEnv.dbremove(dbTxn, file.c_str(), NULL, 0);
            }
        }

        if (NULL != dbTxn)
        {
            dbTxn->commit(0);
        }

        MG_RESOURCE_SERVICE_CATCH(L"MgRepositoryManager.CleanRepository")

        if (mgException != NULL)
        {
            try
            {
                if (NULL != dbTxn)
                {
                    dbTxn->abort();
                }
            }
            catch (...)
            {
                assert(false);
            }
        }

        MG_RESOURCE_SERVICE_THROW()
    }

    // Remove all the backing files if they exist.

    {
        MG_RESOURCE_SERVICE_TRY()

        DbEnv dbEnv(0);

        dbEnv.remove(home.c_str(), 0);

        MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgRepositoryManager.CleanRepository")
    }

    // Remove all the log files.

    if (logFiles != NULL)
    {
        for (INT32 i = 0; i < logFiles->GetCount(); ++i)
        {
            MgFileUtil::DeleteFile(logFiles->GetItem(i));
        }
    }

    // Remove all the data files.

    {
        MG_RESOURCE_SERVICE_TRY()

        MgFileUtil::CleanDirectory(resourceDataFilePath);

        MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgRepositoryManager.CleanRepository")
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initialize the repository manager.
///
void MgRepositoryManager::Initialize(bool transacted)
{
    AbortTransaction();

    m_transacted = transacted;

    if (m_transacted)
    {
        CreateTransaction();
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Terminate the repository manager.
///
void MgRepositoryManager::Terminate()
{
    CommitTransaction();
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a transaction.
/// </summary>
///----------------------------------------------------------------------------

void MgRepositoryManager::CreateTransaction()
{
    MG_RESOURCE_SERVICE_TRY()

    if (!m_transacted || NULL != m_dbTxn || NULL != m_xmlTxn.get())
    {
        throw new MgInvalidOperationException(
            L"MgRepositoryManager.CreateTransaction",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgDbEnvironment* environment = m_repository.GetEnvironment();
    assert(NULL != environment);

    if (NULL != environment && environment->IsTransacted())
    {
        environment->GetDbEnv().txn_begin(0, &m_dbTxn, 0);
        assert(NULL != m_dbTxn);

        m_xmlTxn.reset(new XmlTransaction(
            environment->GetXmlManager().createTransaction(m_dbTxn)));
        assert(NULL != m_xmlTxn.get());
    }
    else
    {
        throw new MgInvalidOperationException(
            L"MgRepositoryManager.CreateTransaction",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgRepositoryManager.CreateTransaction")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Check if the current operation (normally when writing) is transactionally
/// protected. If not, then an exception will be thrown.
///
void MgRepositoryManager::ValidateTransaction()
{
    if (!m_transacted)
    {
        throw new MgInvalidOperationException(
            L"MgRepositoryManager.ValidateTransaction",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Commits the transaction. If needed, this method should be used before
/// the destructor gets called.
/// </summary>
///----------------------------------------------------------------------------

void MgRepositoryManager::CommitTransaction()
{
    if (NULL != m_dbTxn)
    {
        m_dbTxn->commit(0);
        m_dbTxn = NULL;
        m_xmlTxn.reset();
    }

    m_transacted = false;
}

///----------------------------------------------------------------------------
/// <summary>
/// Aborts the transaction.
/// </summary>
///----------------------------------------------------------------------------

void MgRepositoryManager::AbortTransaction()
{
    if (NULL != m_dbTxn)
    {
        m_dbTxn->abort();
        m_dbTxn = NULL;
        m_xmlTxn.reset();
    }

    m_transacted = false;
}

///----------------------------------------------------------------------------
/// <summary>
/// Throws one of the exceptions listed below.
/// </summary>
///
/// <exceptions>
/// MgRepositoryNotFoundException
/// MgResourceNotFoundException
/// MgUserNotFoundException
/// MgGroupNotFoundException
/// MgRoleNotFoundException
/// </exceptions>
///----------------------------------------------------------------------------

void MgRepositoryManager::ThrowResourceNotFoundException(
    MgResourceIdentifier& resource, CREFSTRING methodName,
    UINT32 lineNumber, CREFSTRING fileName)
{
    MgStringCollection arguments;

    if (resource.IsRoot())
    {
        arguments.Add(resource.ToString());

        throw new MgRepositoryNotFoundException(
            methodName, lineNumber, fileName, &arguments, L"", NULL);
    }
    else if (resource.IsResourceTypeOf(MgResourceType::User))
    {
        arguments.Add(resource.GetName());

        throw new MgUserNotFoundException(
            methodName, lineNumber, fileName, &arguments, L"", NULL);
    }
    else if (resource.IsResourceTypeOf(MgResourceType::Group))
    {
        arguments.Add(resource.GetName());

        throw new MgGroupNotFoundException(
            methodName, lineNumber, fileName, &arguments, L"", NULL);
    }
    else if (resource.IsResourceTypeOf(MgResourceType::Role))
    {
        arguments.Add(resource.GetName());

        throw new MgRoleNotFoundException(
            methodName, lineNumber, fileName, &arguments, L"", NULL);
    }
    else
    {
        arguments.Add(resource.ToString());

        throw new MgResourceNotFoundException(
            methodName, lineNumber, fileName, &arguments, L"", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Throws one of the exceptions listed below.
/// </summary>
///
/// <exceptions>
/// MgDuplicateRepositoryException
/// MgDuplicateResourceException
/// MgDuplicateUserException
/// MgDuplicateGroupException
/// </exceptions>
///----------------------------------------------------------------------------

void MgRepositoryManager::ThrowDuplicateResourceException(
    MgResourceIdentifier& resource, CREFSTRING methodName,
    UINT32 lineNumber, CREFSTRING fileName)
{
    MgStringCollection arguments;

    if (resource.IsRoot())
    {
        arguments.Add(resource.ToString());

        throw new MgDuplicateRepositoryException(
            methodName, lineNumber, fileName, &arguments, L"", NULL);
    }
    else if (resource.IsResourceTypeOf(MgResourceType::User))
    {
        arguments.Add(resource.GetName());

        throw new MgDuplicateUserException(
            methodName, lineNumber, fileName, &arguments, L"", NULL);
    }
    else if (resource.IsResourceTypeOf(MgResourceType::Group))
    {
        arguments.Add(resource.GetName());

        throw new MgDuplicateGroupException(
            methodName, lineNumber, fileName, &arguments, L"", NULL);
    }
    else if (resource.IsResourceTypeOf(MgResourceType::Role))
    {
        arguments.Add(resource.GetName());

        throw new MgDuplicateRoleException(
            methodName, lineNumber, fileName, &arguments, L"", NULL);
    }
    else
    {
        arguments.Add(resource.ToString());

        throw new MgDuplicateResourceException(
            methodName, lineNumber, fileName, &arguments, L"", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates a list of entries in the repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgRepositoryManager::EnumerateRepositories()
{
    throw new MgNotImplementedException(
        L"MgRepositoryManager.EnumerateRepositories",
        __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL; // to make some compiler happy
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a repository with the given content and header.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgRepositoryManager::CreateRepository(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (!resource->IsRoot())
    {
        throw new MgInvalidResourceTypeException(
            L"MgRepositoryManager.CreateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Initialize resource information.

    MgResourceInfo resourceInfo(*resource, m_currUserInfo, m_accessedTime);

    // Add the header document.

    string headerDoc;

    if (NULL != header)
    {
        header->ToStringUtf8(headerDoc);
    }

    MgResourceHeaderManager* resourceHeaderMan = GetResourceHeaderManager();

    if (NULL != resourceHeaderMan)
    {
        resourceHeaderMan->AddRepository(resourceInfo, headerDoc);
    }
    else if (!headerDoc.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"3");
        arguments.Add(L"MgByteReader");

        throw new MgInvalidArgumentException(L"MgRepositoryManager.CreateRepository",
            __LINE__, __WFILE__, &arguments, L"MgHeaderDocumentNotEmpty", NULL);
    }

    // Add the content document.

    string contentDoc;

    if (NULL != content)
    {
        content->ToStringUtf8(contentDoc);
    }

    MgResourceContentManager* resourceContentMan = GetResourceContentManager();

    resourceContentMan->AddRepository(resourceInfo, contentDoc);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgRepositoryManager.CreateRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes a repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgRepositoryManager::DeleteRepository(MgResourceIdentifier* resource)
{
    throw new MgNotImplementedException(
        L"MgRepositoryManager.DeleteRepository",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a new resource to the opened resource repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgRepositoryManager::AddResource(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (resource->IsFolder())
    {
        if (resource->IsRoot())
        {
            throw new MgInvalidResourceTypeException(
                L"MgRepositoryManager.AddResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (NULL != content)
        {
            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(L"MgByteReader");

            throw new MgInvalidArgumentException(L"MgRepositoryManager.AddResource",
                __LINE__, __WFILE__, &arguments, L"MgContentNotNull", NULL);
        }
    }
    else
    {
        if (NULL == content)
        {
            throw new MgNullArgumentException(
                L"MgRepositoryManager.AddResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    // Initialize resource information.

    MgResourceInfo resourceInfo(*resource, m_currUserInfo, m_accessedTime);

    // Add the header document.

    string headerDoc;

    if (NULL != header)
    {
        header->ToStringUtf8(headerDoc);
    }

    MgResourceHeaderManager* resourceHeaderMan = GetResourceHeaderManager();

    if (NULL != resourceHeaderMan)
    {
        resourceHeaderMan->AddParentResources(resourceInfo, headerDoc);
        resourceHeaderMan->AddResource(resourceInfo, headerDoc);
    }
    else if (!headerDoc.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"3");
        arguments.Add(L"MgByteReader");

        throw new MgInvalidArgumentException(L"MgRepositoryManager.AddResource",
            __LINE__, __WFILE__, &arguments, L"MgHeaderDocumentNotEmpty", NULL);
    }

    // Check if the specified repository already exists.

    MgResourceContentManager* resourceContentMan = GetResourceContentManager();

    if (resource->IsRepositoryTypeOf(MgRepositoryType::Session)
        && !resourceContentMan->ResourceExists(resource->GetRootPath()))
    {
        MgStringCollection arguments;
        arguments.Add(resource->GetRootPath());

        throw new MgRepositoryNotFoundException(
            L"MgRepositoryManager.AddResource",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Add the content document.

    string contentDoc;

    if (NULL != content)
    {
        content->ToStringUtf8(contentDoc);
    }

    if (!resource->IsFolder())
    {
        resourceContentMan->AddResource(resourceInfo, contentDoc);
    }
    else if (!contentDoc.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(L"MgByteReader");

        throw new MgInvalidArgumentException(L"MgRepositoryManager.AddResource",
            __LINE__, __WFILE__, &arguments, L"MgContentDocumentNotEmpty", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgRepositoryManager.AddResource")
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

void MgRepositoryManager::SetResource(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    MG_RESOURCE_SERVICE_TRY()

    if (ResourceExists(resource))
    {
        UpdateResource(resource, content, header);
    }
    else
    {
        AddResource(resource, content, header);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgRepositoryManager.SetResource")
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

void MgRepositoryManager::DeleteResource(MgResourceIdentifier* resource)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Delete the header document.

    MgResourceHeaderManager* resourceHeaderMan = GetResourceHeaderManager();

    if (NULL != resourceHeaderMan)
    {
        resourceHeaderMan->DeleteResource(resource);
    }

    // Delete the content document.

    MgResourceContentManager* resourceContentMan = GetResourceContentManager();

    resourceContentMan->DeleteResource(resource);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgRepositoryManager.DeleteResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates an existing resource in the opened resource repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgRepositoryManager::UpdateResource(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    assert(NULL != resource);

    MG_RESOURCE_SERVICE_TRY()

    // Validate input parameters.

    if (resource->IsFolder())
    {
        if (resource->IsRoot())
        {
            throw new MgInvalidResourceTypeException(
                L"MgRepositoryManager.UpdateResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (NULL != content)
        {
            MgStringCollection arguments;
            arguments.Add(L"2");
            arguments.Add(L"MgByteReader");

            throw new MgInvalidArgumentException(L"MgRepositoryManager.UpdateResource",
                __LINE__, __WFILE__, &arguments, L"MgContentNotNull", NULL);
        }

        if (NULL == header)
        {
            throw new MgNullArgumentException(
                L"MgRepositoryManager.UpdateResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        if (NULL == content && NULL == header)
        {
            throw new MgNullArgumentException(
                L"MgRepositoryManager.UpdateResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    // Initialize resource information.

    MgResourceInfo resourceInfo(*resource, m_currUserInfo, m_accessedTime);

    // Update the header document.

    string headerDoc;

    if (NULL != header)
    {
        header->ToStringUtf8(headerDoc);
    }

    if (headerDoc.empty())
    {
        UpdateDateModifiedResourceSet(resource->ToString());
    }
    else
    {
        MgResourceHeaderManager* resourceHeaderMan = GetResourceHeaderManager();

        if (NULL == resourceHeaderMan)
        {
            throw new MgNullReferenceException(
                L"MgRepositoryManager.UpdateResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            resourceHeaderMan->UpdateResource(resourceInfo, headerDoc);
        }
    }

    // Update the content document.

    string contentDoc;

    if (NULL != content)
    {
        content->ToStringUtf8(contentDoc);
    }

    if (!contentDoc.empty())
    {
        MgResourceContentManager* resourceContentMan = GetResourceContentManager();

        resourceContentMan->UpdateResource(resourceInfo, contentDoc);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgRepositoryManager.UpdateResource")
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Deletes all the resource data referenced in the specified resource tags.
///
void MgRepositoryManager::DeleteResourceData(CREFSTRING resourceTags, MgResourceIdentifier* resource)
{
    throw new MgNotImplementedException(
        L"MgRepositoryManager.DeleteResourceData",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Copies all the resource data referenced in the source resource tags, and
/// returns destination resource tags with new references.
///
void MgRepositoryManager::CopyResourceData(CREFSTRING sourceResourceTags,
    REFSTRING destResourceTags, bool overwrite)
{
    throw new MgNotImplementedException(
        L"MgRepositoryManager.CopyResourceData",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Checks to see if the specified resource exists.
///
bool MgRepositoryManager::ResourceExists(MgResourceIdentifier* resource)
{
    assert(NULL != resource);
    MgResourceContentManager* resourceContentMan = GetResourceContentManager();
    assert(NULL != resourceContentMan);

    return resourceContentMan->ResourceExists(resource->ToString());
}
