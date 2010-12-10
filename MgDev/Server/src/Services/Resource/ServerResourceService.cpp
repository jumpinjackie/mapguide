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
#include "ServerResourceService.h"
#include "PermissionManager.h"
#include "LibraryRepository.h"
#include "SessionRepository.h"
#include "SiteRepository.h"
#include "LibraryRepositoryManager.h"
#include "SessionRepositoryManager.h"
#include "SiteRepositoryManager.h"
#include "SiteResourceContentManager.h"
#include "UnmanagedDataManager.h"
#include "LogDetail.h"

INT32 MgServerResourceService::sm_retryAttempts = 50;
INT32 MgServerResourceService::sm_sessionRepositoriesLimit = MgConfigProperties::DefaultResourceServicePropertySessionRepositoriesLimit;
ACE_Time_Value MgServerResourceService::sm_retryInterval;

MgSiteRepository*    MgServerResourceService::sm_siteRepository    = NULL;
MgSessionRepository* MgServerResourceService::sm_sessionRepository = NULL;
MgLibraryRepository* MgServerResourceService::sm_libraryRepository = NULL;
std::map<STRING, MgSessionRepository* > MgServerResourceService::sm_sessionRepositories;
bool MgServerResourceService::sm_bSingleSessionRepository = false;

ACE_Recursive_Thread_Mutex MgServerResourceService::sm_mutex;
set<STRING> MgServerResourceService::sm_changedResources;

IMPLEMENT_CREATE_SERVICE(MgServerResourceService)

///////////////////////////////////////////////////////////////////////////////
/// Server Resource Service retry macros.
/// This pauses briefly before trying the operation again.
///
// DBXML always uses a transaction regardless.
#define MG_RESOURCE_SERVICE_BEGIN_OPERATION()                                 \
    MG_RESOURCE_SERVICE_BEGIN_RETRY()                                         \
    repositoryMan->Initialize(true);                                          \

#define MG_RESOURCE_SERVICE_END_OPERATION(maxRetries)                         \
    MG_RESOURCE_SERVICE_END_RETRY(maxRetries)                                 \
    MG_RESOURCE_SERVICE_BEGIN_RETRY()                                         \
        repositoryMan->Terminate();                                           \
    MG_RESOURCE_SERVICE_END_RETRY(maxRetries)                                 \

#define MG_RESOURCE_SERVICE_BEGIN_RETRY()                                     \
    {                                                                         \
        int numRetries = 0;                                                   \
                                                                              \
        while (true)                                                          \
        {                                                                     \
            try                                                               \
            {                                                                 \

#define MG_RESOURCE_SERVICE_END_RETRY(maxRetries)                             \
                break;                                                        \
            }                                                                 \
            catch (MgException* e)                                            \
            {                                                                 \
                ++numRetries;                                                 \
                                                                              \
                if ((e->IsOfClass(MapGuide_Exception_MgDbXmlException) || e->IsOfClass(MapGuide_Exception_MgDbException)) \
                 && (DB_LOCK_DEADLOCK == (static_cast<MgThirdPartyException*>(e))->GetErrorCode())) \
                {                                                             \
                    if (numRetries < maxRetries)                              \
                    {                                                         \
                        SAFE_RELEASE(e);                                      \
                    }                                                         \
                    else                                                      \
                    {                                                         \
                        throw e;                                              \
                    }                                                         \
                }                                                             \
                else                                                          \
                {                                                             \
                    throw e;                                                  \
                }                                                             \
            }                                                                 \
            catch (...)                                                       \
            {                                                                 \
                throw;                                                        \
            }                                                                 \
                                                                              \
            ACE_Time_Value sleepTime;                                         \
            sleepTime.msec((long)(sm_retryInterval.msec() + ((ACE_OS::rand()%10)+1))); \
            ACE_OS::sleep(sleepTime);                                         \
        }                                                                     \
    }

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgServerResourceService::MgServerResourceService() :
    MgResourceService()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgServerResourceService::~MgServerResourceService()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Open the repositories.
/// This method must be called once during the server startup time.
///
void MgServerResourceService::OpenRepositories()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_RESOURCE_SERVICE_TRY()

    ACE_ASSERT(NULL == sm_siteRepository && NULL == sm_sessionRepository && NULL == sm_libraryRepository);

    // Initialize performance tuning settings.

    INT32 retryInterval = 25; // in milliseconds
    STRING sessionRepositoryPath;
    STRING sessionRepositoriesConfig;
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    assert(NULL != configuration);

    configuration->GetIntValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertyRetryAttempts,
        sm_retryAttempts,
        MgConfigProperties::DefaultResourceServicePropertyRetryAttempts);

    configuration->GetIntValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertyRetryInterval,
        retryInterval,
        MgConfigProperties::DefaultResourceServicePropertyRetryInterval);

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertySessionRepositoriesConfig,
        sessionRepositoriesConfig,
        MgConfigProperties::DefaultResourceServicePropertySessionRepositoriesConfig);

    configuration->GetIntValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertySessionRepositoriesLimit,
        sm_sessionRepositoriesLimit,
        MgConfigProperties::DefaultResourceServicePropertySessionRepositoriesLimit);

    configuration->GetStringValue(
        MgConfigProperties::ResourceServicePropertiesSection,
        MgConfigProperties::ResourceServicePropertySessionRepositoryPath,
        sessionRepositoryPath,
        MgConfigProperties::DefaultResourceServicePropertySessionRepositoryPath);

    sm_retryInterval.msec((long)retryInterval);

    // Set the session repository configuration
    if(_wcsicmp(sessionRepositoriesConfig.c_str(), MgConfigProperties::DefaultResourceServicePropertySessionRepositoriesConfig.c_str()) == 0)
    {
        // FilePerSession
        sm_bSingleSessionRepository = false;
    }
    else
    {
        // SingleFile
        sm_bSingleSessionRepository = true;
    }

    // Clean up the Session repository.
    MgFileUtil::CleanDirectory(sessionRepositoryPath);

    // The session repository configuration depends on the serverconfig.ini SessionRepositoriesConfig setting.
    // If it is "SingleFile" then it uses a single file to hold all of the session information for all sessions.
    // If it is "FilePerSession" then it uses a file per session.
    // Initialize the Session repository.

    if(sm_bSingleSessionRepository)
    {
        sm_sessionRepository = new MgSessionRepository(L"");
        sm_sessionRepository->Initialize();
    }

    // Initialize the Site repository.

    sm_siteRepository = new MgSiteRepository();
    sm_siteRepository->Initialize();

    // Create the security cache after the Site repository has been
    // successfully created/opened.

    MgSecurityManager::RefreshSecurityCache(CreateSecurityCache());

    // Initialize the Library repository.

    sm_libraryRepository = new MgLibraryRepository();
    sm_libraryRepository->Initialize();

    // Create the permission cache after the Library repository has been
    // successfully created/opened.

    MgPermissionManager::RefreshPermissionCache(CreatePermissionCache());

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.OpenRepositories")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Close the repositories.
/// This method must be called once during the server shutdown time.
///
void MgServerResourceService::CloseRepositories()
{
    MG_TRY()

    PerformRepositoryCheckpoints(DB_FORCE);

    MG_CATCH_AND_RELEASE()

    delete sm_siteRepository;
    sm_siteRepository = NULL;

    for (std::map<STRING, MgSessionRepository* >::iterator i = sm_sessionRepositories.begin();i != sm_sessionRepositories.end(); ++i)
    {
        MgSessionRepository* sessionRepository = i->second;
        if(NULL != sessionRepository)
        {
            delete sessionRepository;
            sessionRepository = NULL;
        }
    }

    delete sm_libraryRepository;
    sm_libraryRepository = NULL;

    delete sm_sessionRepository;
    sm_sessionRepository = NULL;
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates a list of entries in the specified repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateRepositories(
    CREFSTRING repositoryType)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateRepositories()");

    if (MgRepositoryType::Session != repositoryType)
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.EnumerateRepositories",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    list += "<RepositoryList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"RepositoryList-1.0.0.xsd\">\n";

    if(sm_bSingleSessionRepository)
    {
        auto_ptr<MgSessionRepositoryManager> repositoryMan(
            new MgSessionRepositoryManager(*sm_sessionRepository));

        MG_RESOURCE_SERVICE_BEGIN_OPERATION()

        byteReader = repositoryMan->EnumerateRepositories();

        MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

        // Need to combine the results from all of the session repositories
        STRING sessionContents = byteReader->ToString();
        string content;
        MgUtil::WideCharToMultiByte(sessionContents, content);
        list += content;
    }
    else
    {
        if(0 == sm_sessionRepositories.size())
        {
            // Since there are no sessions yet we will need to check access rights ourselves
            auto_ptr<MgSecurityManager> securityMan;
            securityMan.reset(new MgSecurityManager());

            Ptr<MgUserInformation> currUserInfo = MgUserInformation::GetCurrentUserInfo();
            assert(NULL != currUserInfo);
            STRING sessionId = currUserInfo->GetMgSessionId();
            STRING userName = currUserInfo->GetUserName();
            STRING password = currUserInfo->GetPassword();

            if (!sessionId.empty())
            {
                if (userName.empty())
                {
                    userName = securityMan->GetUserName(sessionId);
                    password = securityMan->GetPassword(userName);
                }
            }

            if (userName.empty())
            {
                MG_LOG_AUTHENTICATION_ENTRY(MgResources::UnauthorizedAccess.c_str());

                throw new MgUnauthorizedAccessException(
                    L"MgServerResourceService.EnumerateRepositories",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }

            if (!securityMan->IsUserAnAdministrator(userName))
            {
                MG_LOG_AUTHENTICATION_ENTRY(MgResources::UnauthorizedAccess.c_str());

                throw new MgUnauthorizedAccessException(
                    L"MgServerResourceService.EnumerateRepositories",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }

        for (std::map<STRING, MgSessionRepository* >::iterator i = sm_sessionRepositories.begin();i != sm_sessionRepositories.end(); ++i)
        {
            MgSessionRepository* sessionRepository = i->second;
            if(NULL != sessionRepository)
            {
                auto_ptr<MgSessionRepositoryManager> repositoryMan(
                    new MgSessionRepositoryManager(*sessionRepository));

                MG_RESOURCE_SERVICE_BEGIN_OPERATION()

                byteReader = repositoryMan->EnumerateRepositories();

                MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

                // Need to combine the results from all of the session repositories
                STRING sessionContents = byteReader->ToString();
                string content;
                MgUtil::WideCharToMultiByte(sessionContents, content);
                list += content;
            }
        }
    }

    list += "</RepositoryList>";

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateRepositories")

    return byteReader.Detach();
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

void MgServerResourceService::CreateRepository(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::CreateRepository()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.CreateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Session))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.CreateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if(sm_bSingleSessionRepository)
    {
        auto_ptr<MgSessionRepositoryManager> repositoryMan(
            new MgSessionRepositoryManager(*sm_sessionRepository));
        int maxRetries = sm_retryAttempts;

        if ((NULL != content && !content->IsRewindable())
         || (NULL != header  && !header->IsRewindable()))
        {
            maxRetries = 0;
        }

        MG_RESOURCE_SERVICE_BEGIN_OPERATION()

        if (NULL != content && content->IsRewindable())
        {
            content->Rewind();
        }

        if (NULL != header && header->IsRewindable())
        {
            header->Rewind();
        }

        repositoryMan->CreateRepository(resource, content, header);

        MG_RESOURCE_SERVICE_END_OPERATION(maxRetries)
    }
    else
    {
        auto_ptr<MgSecurityManager> securityMan;
        securityMan.reset(new MgSecurityManager());

        Ptr<MgUserInformation> currUserInfo = MgUserInformation::GetCurrentUserInfo();
        assert(NULL != currUserInfo);
        STRING sessionId = currUserInfo->GetMgSessionId();
        STRING userName = currUserInfo->GetUserName();
        STRING password = currUserInfo->GetPassword();

        if (!sessionId.empty())
        {
            if (userName.empty())
            {
                userName = securityMan->GetUserName(sessionId);
                password = securityMan->GetPassword(userName);
            }
        }

        if (userName.empty())
        {
            MG_LOG_AUTHENTICATION_ENTRY(MgResources::UnauthorizedAccess.c_str());

            throw new MgUnauthorizedAccessException(
                L"MgServerResourceService.CreateRepository",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        ACE_ASSERT(!resource->GetRepositoryName().empty());
        STRING name = resource->GetRepositoryName();
        std::map<STRING, MgSessionRepository* >::iterator iter = sm_sessionRepositories.find(name);
        if(sm_sessionRepositories.end() == iter)
        {
    //        ACE_DEBUG((LM_INFO, ACE_TEXT("CreateRepository: %W\n"), name.c_str()));
            MgSessionRepository* sessionRepository = NULL;

            // Check to see if there is room. If not we need to throw an exception.
            if(sm_sessionRepositories.size() >= sm_sessionRepositoriesLimit)
            {
                MgStringCollection arguments;
                arguments.Add(resource->ToString());

                throw new MgRepositoryCreationFailedException(
                    L"MgServerResourceService.CreateRepository", __LINE__, __WFILE__, &arguments, L"", NULL);
            }

            sessionRepository = new MgSessionRepository(name);
            if(NULL != sessionRepository)
            {
                // Add this NEW session repository to the map
                sm_sessionRepositories.insert(std::pair<STRING, MgSessionRepository* >(name, sessionRepository));
//                ACE_DEBUG((LM_INFO, ACE_TEXT("%d/%d ++ %W\n"), sm_sessionRepositories.size(), sm_sessionRepositoriesLimit, name.c_str()));

                // Initialize the session repository
                sessionRepository->Initialize();

                auto_ptr<MgSessionRepositoryManager> repositoryMan(
                    new MgSessionRepositoryManager(*sessionRepository));
                int maxRetries = sm_retryAttempts;

                if ((NULL != content && !content->IsRewindable())
                 || (NULL != header  && !header->IsRewindable()))
                {
                    maxRetries = 0;
                }

                MG_RESOURCE_SERVICE_BEGIN_OPERATION()

                if (NULL != content && content->IsRewindable())
                {
                    content->Rewind();
                }

                if (NULL != header && header->IsRewindable())
                {
                    header->Rewind();
                }

                repositoryMan->CreateRepository(resource, content, header);

                MG_RESOURCE_SERVICE_END_OPERATION(maxRetries)
            }
        }
        else
        {
            MgStringCollection arguments;
            arguments.Add(resource->ToString());

            throw new MgDuplicateRepositoryException(
                L"MgServerResourceService.CreateRepository", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CreateRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes the specified repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::DeleteRepository(MgResourceIdentifier* resource)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::DeleteRepository()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.DeleteRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Session))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.DeleteRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgSecurityManager> securityMan;
    securityMan.reset(new MgSecurityManager());

    Ptr<MgUserInformation> currUserInfo = MgUserInformation::GetCurrentUserInfo();
    assert(NULL != currUserInfo);
    STRING sessionId = currUserInfo->GetMgSessionId();
    STRING userName = currUserInfo->GetUserName();
    STRING password = currUserInfo->GetPassword();

    if (!sessionId.empty())
    {
        if (userName.empty())
        {
            userName = securityMan->GetUserName(sessionId);
            password = securityMan->GetPassword(userName);
        }
    }

    if (userName.empty())
    {
        MG_LOG_AUTHENTICATION_ENTRY(MgResources::UnauthorizedAccess.c_str());

        throw new MgUnauthorizedAccessException(
            L"MgServerResourceService.CreateRepository",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if(sm_bSingleSessionRepository)
    {
        auto_ptr<MgSessionRepositoryManager> repositoryMan(
            new MgSessionRepositoryManager(*sm_sessionRepository));

        MG_RESOURCE_SERVICE_BEGIN_OPERATION()

        repositoryMan->DeleteRepository(resource);

        MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

        // Update the current set of changed resources.
        UpdateChangedResources(repositoryMan->GetChangedResources());
    }
    else
    {
        ACE_ASSERT(!resource->GetRepositoryName().empty());
        STRING name = resource->GetRepositoryName();
        std::map<STRING, MgSessionRepository* >::iterator iter = sm_sessionRepositories.find(name);
        if(sm_sessionRepositories.end() != iter)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT("%d/%d -- %W\n"), sm_sessionRepositories.size(), sm_sessionRepositoriesLimit, name.c_str()));
            MgSessionRepository* sessionRepository = iter->second;
            if(NULL != sessionRepository)
            {
                auto_ptr<MgSessionRepositoryManager> repositoryMan(
                    new MgSessionRepositoryManager(*sessionRepository));

                MG_RESOURCE_SERVICE_BEGIN_OPERATION()

                repositoryMan->DeleteRepository(resource);

                MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

                // Update the current set of changed resources.
                UpdateChangedResources(repositoryMan->GetChangedResources());

                sm_sessionRepositories.erase(iter);
                delete sessionRepository;
                sessionRepository = NULL;
            }
        }
        else
        {
            MgStringCollection arguments;
            arguments.Add(name);

            throw new MgRepositoryNotFoundException(
                L"MgServerResourceService.DeleteRepository",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteRepository")
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

void MgServerResourceService::UpdateRepository(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::UpdateRepository()");

    if (NULL == resource || (NULL == content && NULL == header))
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.UpdateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));
    int maxRetries = sm_retryAttempts;

    if ((NULL != content && !content->IsRewindable())
     || (NULL != header  && !header->IsRewindable()))
    {
        maxRetries = 0;
    }

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    if (NULL != content && content->IsRewindable())
    {
        content->Rewind();
    }

    if (NULL != header && header->IsRewindable())
    {
        header->Rewind();
    }

    repositoryMan->UpdateRepository(resource, content, header);

    MG_RESOURCE_SERVICE_END_OPERATION(maxRetries)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.UpdateRepository")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the content of the specified repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::GetRepositoryContent(
    MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GetRepositoryContent()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetRepositoryContent",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    byteReader = repositoryMan->GetRepositoryContent(resource);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetRepositoryContent")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the header associated with the specified repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::GetRepositoryHeader(
    MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GetRepositoryHeader()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetRepositoryHeader",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.GetRepositoryHeader",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    byteReader = repositoryMan->GetRepositoryHeader(resource);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetRepositoryHeader")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Applies a package of resource changes to the repository.
///
void MgServerResourceService::ApplyResourcePackage(MgByteReader* packageStream)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::ApplyResourcePackage()");

    if (NULL == packageStream)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.ApplyResourcePackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));
    int maxRetries = sm_retryAttempts;

    if (!packageStream->IsRewindable())
    {
        maxRetries = 0;
    }

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    if (packageStream->IsRewindable())
    {
        packageStream->Rewind();
    }

    repositoryMan->ApplyResourcePackage(packageStream);

    MG_RESOURCE_SERVICE_END_OPERATION(maxRetries)

    // Update the current set of changed resources.
    UpdateChangedResources(repositoryMan->GetChangedResources());

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.ApplyResourcePackage")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Loads the specified resource package into the repository.
///
void MgServerResourceService::LoadResourcePackage(CREFSTRING packagePathname,
    bool logActivities)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::LoadResourcePackage()");

    if (!MgFileUtil::PathnameExists(packagePathname))
    {
        MgStringCollection arguments;
        arguments.Add(packagePathname);

        throw new MgFileNotFoundException(
            L"MgServerResourceService.LoadResourcePackage",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->LoadResourcePackage(packagePathname, logActivities);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    // Update the current set of changed resources.
    UpdateChangedResources(repositoryMan->GetChangedResources());

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.LoadResourcePackage")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates a package from the specified resource, and then saves it into
/// the specified name.
///
void MgServerResourceService::MakeResourcePackage(MgResourceIdentifier* resource,
    CREFSTRING packagePathname, CREFSTRING packageDescription, bool logActivities)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::MakeResourcePackage()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.ApplyResourcePackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsFolder())
    {
        throw new MgInvalidResourceTypeException(
            L"MgServerResourceService.MakeResourcePackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.MakeResourcePackage",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->MakeResourcePackage(resource, packagePathname,
        packageDescription, logActivities);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.MakeResourcePackage")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Checks to see if the specified resource exists.
///
bool MgServerResourceService::ResourceExists(MgResourceIdentifier* resource)
{
    bool existed = false;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::ResourceExists()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.ResourceExists",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    existed = repositoryMan->ResourceExists(resource);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.ResourceExists")

    return existed;
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates the resources in the resource repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateResources(
    MgResourceIdentifier* resource, INT32 depth, CREFSTRING type,
    INT32 properties, CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateResources()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.EnumerateResources",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.EnumerateResources",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    byteReader = repositoryMan->EnumerateResources(resource, depth, type,
        properties, fromDate, toDate, computeChildren);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateResources")

    return byteReader.Detach();
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

void MgServerResourceService::SetResource(MgResourceIdentifier* resource,
    MgByteReader* content, MgByteReader* header)
{
    MG_RESOURCE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.SetResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLogDetail logDetail(MgServiceType::ResourceService, MgLogDetail::Trace, L"MgServerResourceService.SetResource", mgStackParams);
    logDetail.AddResourceIdentifier(L"Id", resource);
    logDetail.AddInt64(L"ContentSize", NULL != content ? content->GetLength() : 0);
    logDetail.AddInt64(L"HeaderSize", NULL != header ? header->GetLength() : 0);
    logDetail.Create();

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));
    int maxRetries = sm_retryAttempts;

    if ((NULL != content && !content->IsRewindable())
     || (NULL != header  && !header->IsRewindable()))
    {
        maxRetries = 0;
    }

    set<STRING> changedResources;
    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    if (NULL != content && content->IsRewindable())
    {
        content->Rewind();
    }

    if (NULL != header && header->IsRewindable())
    {
        header->Rewind();
    }

    repositoryMan->SetResource(resource, content, header);

    changedResources = repositoryMan->GetChangedResources();
    MG_RESOURCE_SERVICE_END_OPERATION(maxRetries)

    // Update the current set of changed resources.
    UpdateChangedResources(changedResources);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.SetResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes an existing resource from the resource repository.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::DeleteResource(MgResourceIdentifier* resource)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::DeleteResource()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.DeleteResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->DeleteResource(resource);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    // Update the current set of changed resources.
    UpdateChangedResources(repositoryMan->GetChangedResources());

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteResource")
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
void MgServerResourceService::MoveResource(MgResourceIdentifier* sourceResource,
     MgResourceIdentifier* destResource, bool overwrite)
{
    MoveResource(sourceResource, destResource, overwrite, false);
}

void MgServerResourceService::MoveResource(MgResourceIdentifier* sourceResource,
    MgResourceIdentifier* destResource, bool overwrite, bool cascade)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::MoveResource()");

    if (NULL == sourceResource || NULL == destResource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.MoveResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(destResource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->MoveResource(sourceResource, destResource, overwrite, cascade);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    // Update the current set of changed resources.
    UpdateChangedResources(repositoryMan->GetChangedResources());

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.MoveResource")
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

void MgServerResourceService::CopyResource(MgResourceIdentifier* sourceResource,
    MgResourceIdentifier* destResource, bool overwrite)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::CopyResource()");

    if (NULL == sourceResource || NULL == destResource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.CopyResource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(destResource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->CopyResource(sourceResource, destResource, overwrite);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    // Update the current set of changed resources.
    UpdateChangedResources(repositoryMan->GetChangedResources());

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CopyResource")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the content of the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::GetResourceContent(
    MgResourceIdentifier* resource, CREFSTRING preProcessTags)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetResourceContent", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLogDetail logDetail(MgServiceType::ResourceService, MgLogDetail::Trace, L"MgServerResourceService.GetResourceContent", mgStackParams);
    logDetail.AddResourceIdentifier(L"Id", resource);
    logDetail.AddString(L"Tags", preProcessTags);
    logDetail.Create();

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    byteReader = repositoryMan->GetResourceContent(resource, preProcessTags);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetResourceContent")

    return byteReader.Detach();
}

MgStringCollection* MgServerResourceService::GetResourceContents(MgStringCollection* resources,
                                       MgStringCollection* preProcessTags)
{
    Ptr<MgStringCollection> resourceContents = new MgStringCollection();

    MG_RESOURCE_SERVICE_TRY()

    if (NULL == resources)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetResourceContents", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    bool hasPreprocess = (NULL != preProcessTags);
    if(hasPreprocess && resources->GetCount() != preProcessTags->GetCount())
    {
        throw new MgInvalidArgumentException(
            L"MgServerResourceService.GetResourceContents", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLogDetail logDetail(MgServiceType::ResourceService, MgLogDetail::Trace, L"MgServerResourceService.GetResourceContents", mgStackParams);
    logDetail.AddObject(L"Resources", resources);
    logDetail.AddObject(L"PreProcessTags", preProcessTags);
    logDetail.Create();

    // Iterator resources to get the content
    for(INT32 i = 0; i < resources->GetCount(); i ++)
    {
        STRING resource = resources->GetItem(i);
        STRING preProcessTag = hasPreprocess ? (preProcessTags->GetItem(i)) : L"";

        Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(resource);

        Ptr<MgByteReader> byteReader;
        auto_ptr<MgApplicationRepositoryManager> repositoryMan(
            CreateApplicationRepositoryManager(resourceId));

        MG_RESOURCE_SERVICE_BEGIN_OPERATION()

        byteReader = repositoryMan->GetResourceContent(resourceId, preProcessTag);

        MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

        STRING plainText = byteReader->ToString();
        resourceContents->Add(plainText);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetResourceContents")

    return resourceContents.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the header associated with the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::GetResourceHeader(
    MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetResourceHeader", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.GetResourceHeader", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLogDetail logDetail(MgServiceType::ResourceService, MgLogDetail::Trace, L"MgServerResourceService.GetResourceHeader", mgStackParams);
    logDetail.AddResourceIdentifier(L"Id", resource);
    logDetail.Create();

    ACE_ASSERT(resource->GetRepositoryName().empty());
    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    byteReader = repositoryMan->GetResourceHeader(resource);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetResourceHeader")

    return byteReader.Detach();
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

MgDateTime* MgServerResourceService::GetResourceModifiedDate(
    MgResourceIdentifier* resource)
{
    Ptr<MgDateTime> dateTime;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GetResourceModifiedDate()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetResourceModifiedDate",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.GetResourceModifiedDate",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    dateTime = repositoryMan->GetResourceModifiedDate(resource);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetResourceModifiedDate")

    return dateTime.Detach();
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

MgByteReader* MgServerResourceService::EnumerateReferences(
    MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateReferences()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.EnumerateReferences", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    byteReader = repositoryMan->EnumerateReferences(resource);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateReferences")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerate all the parent Map Definition resources of the specified
/// resources.
///
MgSerializableCollection* MgServerResourceService::EnumerateParentMapDefinitions(
    MgSerializableCollection* resources)
{
    Ptr<MgSerializableCollection> mapDefinitions;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateParentMapDefinitions()");

    if (NULL == resources)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.EnumerateParentMapDefinitions",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Check if the resources come from the Library or Session repository.

    set<string> childResources;
    INT32 libraryResources = 0;
    INT32 numResources = resources->GetCount();

    for (INT32 i = 0; i < numResources; ++i)
    {
        Ptr<MgSerializable> serializableObj = resources->GetItem(i);
        MgResourceIdentifier* resource = dynamic_cast<MgResourceIdentifier*>(
            serializableObj.p);

        if (NULL == resource)
        {
            throw new MgInvalidCastException(
                L"MgServerResourceService.EnumerateParentMapDefinitions",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (!resource->IsFolder())
        {
            if (resource->IsRepositoryTypeOf(MgRepositoryType::Library))
            {
                childResources.insert(MgUtil::WideCharToMultiByte(
                    resource->ToString()));
                ++libraryResources;
            }
            else if (resource->IsRepositoryTypeOf(MgRepositoryType::Session))
            {
                childResources.insert(MgUtil::WideCharToMultiByte(
                    resource->ToString()));
            }
            else
            {
                throw new MgInvalidRepositoryTypeException(
                    L"MgServerResourceService.EnumerateParentMapDefinitions",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    }

    // Note that a Session resource may reference a Library resource but not
    // the other way around, therefore:
    //
    // 1. If the resource comes from the Session repository, then we need to
    //    perform this operation on only the Session repository.
    // 2. If the resource comes from the Library repository, then we need to
    //    perform this operation on both the Library and Session repositories.

    set<STRING> parentResources;
    numResources = static_cast<INT32>(childResources.size());

    if (numResources > 0)
    {
        if (libraryResources > 0)
        {
            MgLibraryRepositoryManager libraryRepositoryMan(*sm_libraryRepository);

            libraryRepositoryMan.Initialize(true);
            libraryRepositoryMan.EnumerateParentMapDefinitions(childResources,
                parentResources);
            libraryRepositoryMan.Terminate();
        }

        if(sm_bSingleSessionRepository)
        {
            MgSessionRepositoryManager sessionRepositoryMan(*sm_sessionRepository);

            sessionRepositoryMan.Initialize(true);
            sessionRepositoryMan.EnumerateParentMapDefinitions(childResources,
                parentResources);
            sessionRepositoryMan.Terminate();
        }
        else
        {
            for (std::map<STRING, MgSessionRepository* >::iterator i = sm_sessionRepositories.begin();i != sm_sessionRepositories.end(); ++i)
            {
                MgSessionRepository* sessionRepository = i->second;
                if(NULL != sessionRepository)
                {
                    MgSessionRepositoryManager sessionRepositoryMan(*sessionRepository);

                    sessionRepositoryMan.Initialize(true);
                    sessionRepositoryMan.EnumerateParentMapDefinitions(childResources,
                        parentResources);
                    sessionRepositoryMan.Terminate();
                }
            }
        }
    }

    if (!parentResources.empty())
    {
        mapDefinitions = new MgSerializableCollection();

        for (set<STRING>::const_iterator i = parentResources.begin();
            i != parentResources.end( ); ++i)
        {
            Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(*i);

            mapDefinitions->Add(resource.p);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateParentMapDefinitions")

    return mapDefinitions.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerate the resource documents in the specified repository.
///
STRING MgServerResourceService::EnumerateResourceDocuments(
    MgStringCollection* resources, CREFSTRING type, INT32 properties)
{
    STRING resourceList;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateResourceDocuments()");

    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    resourceList = repositoryMan->EnumerateResourceDocuments(
        resources, type, properties);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateResourceDocuments")

    return resourceList;
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

void MgServerResourceService::ChangeResourceOwner(
    MgResourceIdentifier* resource, CREFSTRING owner, bool includeDescendants)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::ChangeResourceOwner()");

    if (NULL == resource || owner.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.ChangeResourceOwner",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.ChangeResourceOwner",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->ChangeResourceOwner(resource, owner, includeDescendants);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.ChangeResourceOwner")
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

void MgServerResourceService::InheritPermissionsFrom(MgResourceIdentifier* resource)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::InheritPermissionsFrom()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.InheritPermissionsFrom",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!resource->IsRepositoryTypeOf(MgRepositoryType::Library))
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.InheritPermissionsFrom",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ACE_ASSERT(resource->GetRepositoryName().empty());
    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->InheritPermissionsFrom(resource);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.InheritPermissionsFrom")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates tagged data for the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateResourceData(
    MgResourceIdentifier* resource)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateResourceData()");

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.EnumerateResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    byteReader = repositoryMan->EnumerateResourceData(resource);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateResourceData")

    return byteReader.Detach();
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

void MgServerResourceService::SetResourceData(MgResourceIdentifier* resource,
    CREFSTRING dataName, CREFSTRING dataType, MgByteReader* data)
{
    MG_RESOURCE_SERVICE_TRY()

    if (NULL == resource || NULL == data || dataName.empty() || dataType.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.SetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLogDetail logDetail(MgServiceType::ResourceService, MgLogDetail::Trace, L"MgServerResourceService.SetResourceData", mgStackParams);
    logDetail.AddResourceIdentifier(L"Id", resource);
    logDetail.AddString(L"DataName", dataName);
    logDetail.AddString(L"DataType", dataType);
    logDetail.AddInt64(L"Size", data->GetLength());
    logDetail.Create();

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));
    int maxRetries = sm_retryAttempts;

    if (!data->IsRewindable())
    {
        maxRetries = 0;
    }

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    if (data->IsRewindable())
    {
        data->Rewind();
    }

    repositoryMan->SetResourceData(resource, dataName, dataType, data);

    MG_RESOURCE_SERVICE_END_OPERATION(maxRetries)

    // Update the current set of changed resources.
    UpdateChangedResources(repositoryMan->GetChangedResources());

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.SetResourceData")
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

void MgServerResourceService::DeleteResourceData(
    MgResourceIdentifier* resource, CREFSTRING dataName)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::DeleteResourceData()");

    if (NULL == resource || dataName.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.DeleteResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->DeleteResourceData(resource, dataName);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    // Update the current set of changed resources.
    UpdateChangedResources(repositoryMan->GetChangedResources());

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteResourceData")
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

void MgServerResourceService::RenameResourceData(MgResourceIdentifier* resource,
    CREFSTRING oldDataName, CREFSTRING newDataName, bool overwrite)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::RenameResourceData()");

    if (NULL == resource || oldDataName.empty() || newDataName.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.RenameResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->RenameResourceData(resource, oldDataName, newDataName, overwrite);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    // Update the current set of changed resources.
    UpdateChangedResources(repositoryMan->GetChangedResources());

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.RenameResourceData")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets tagged data for the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::GetResourceData(
    MgResourceIdentifier* resource, CREFSTRING dataName,
    CREFSTRING preProcessTags)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    if (NULL == resource || dataName.empty())
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.GetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLogDetail logDetail(MgServiceType::ResourceService, MgLogDetail::Trace, L"MgServerResourceService.GetResourceData", mgStackParams);
    logDetail.AddResourceIdentifier(L"Id", resource);
    logDetail.AddString(L"DataName", dataName);
    logDetail.AddString(L"Tags", preProcessTags);
    logDetail.Create();

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    byteReader = repositoryMan->GetResourceData(resource, dataName,
        preProcessTags);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GetResourceData")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates an application repository manager based on the specified resource.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgApplicationRepositoryManager* MgServerResourceService::CreateApplicationRepositoryManager(
    MgResourceIdentifier* resource)
{
    ACE_ASSERT(NULL != resource);
    auto_ptr<MgApplicationRepositoryManager> repositoryMan;
    STRING repositoryType = resource->GetRepositoryType();

    MG_RESOURCE_SERVICE_TRY()

    if (MgRepositoryType::Library == repositoryType)
    {
        ACE_ASSERT(resource->GetRepositoryName().empty());
        repositoryMan.reset(new MgLibraryRepositoryManager(*sm_libraryRepository));
    }
    else if (MgRepositoryType::Session == repositoryType)
    {
        ACE_ASSERT(!resource->GetRepositoryName().empty());

        if(sm_bSingleSessionRepository)
        {
            repositoryMan.reset(new MgSessionRepositoryManager(*sm_sessionRepository));
        }
        else
        {
            // Find the session repository associated with this resource
            STRING name = resource->GetRepositoryName();
            std::map<STRING, MgSessionRepository* >::iterator iter = sm_sessionRepositories.find(name);
            if(sm_sessionRepositories.end() != iter)
            {
                MgSessionRepository* sessionRepository = iter->second;
                if(NULL != sessionRepository)
                {
                    repositoryMan.reset(new MgSessionRepositoryManager(*sessionRepository));
                }
            }
            else
            {
                MgStringCollection arguments;
                arguments.Add(name);

                throw new MgRepositoryNotFoundException(
                    L"MgServerResourceService.CreateApplicationRepositoryManager",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }
    }
    else
    {
        throw new MgInvalidRepositoryTypeException(
            L"MgServerResourceService.CreateApplicationRepositoryManager",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CreateApplicationRepositoryManager")

    return repositoryMan.release();
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates users in the specified group or role. If no group and role are
/// specified, all available users will be returned.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateUsers(CREFSTRING group,
    CREFSTRING role, bool includePassword, bool includeGroups)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateUsers()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    byteReader = repositoryMan->EnumerateUsers(group, role, includePassword,
        includeGroups);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateUsers")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a new user,
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgServerResourceService::AddUser(CREFSTRING userId, CREFSTRING username,
    CREFSTRING password, CREFSTRING description)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::AddUser()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->AddUser(userId, username, password, description);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.AddUser")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes the given users in the MgStringCollection.  If the collection is
/// empty, all users are deleted.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgServerResourceService::DeleteUsers(MgStringCollection* users)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::DeleteUsers()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->DeleteUsers(users);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the user using the given parameters.  If a parameter is empty, that
/// user attribute is not changed (except for the newDescription parameter).
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgServerResourceService::UpdateUser(CREFSTRING userId, CREFSTRING newUserId,
    CREFSTRING newUsername, CREFSTRING newPassword, CREFSTRING newDescription)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::UpdateUser()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->UpdateUser(userId, newUserId, newUsername, newPassword, newDescription);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.UpdateUser")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates groups that contain the given user and/or role.  If the user
/// and role parameters are empty, all groups are returned.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateGroups(CREFSTRING user,
    CREFSTRING role)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateGroups()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    byteReader = repositoryMan->EnumerateGroups(user, role);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateGroups")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a group.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgServerResourceService::AddGroup(CREFSTRING group, CREFSTRING description)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::AddGroup()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->AddGroup(group, description);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.AddGroup")
}

///----------------------------------------------------------------------------
/// <summary>
/// Adds a group.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgServerResourceService::DeleteGroups(MgStringCollection* groups)
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::DeleteGroups()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->DeleteGroups(groups);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.DeleteGroups")
}

///----------------------------------------------------------------------------
/// <summary>
/// Updates the group using the given parameters.  If a parameter is empty, that
/// group attribute is not changed (except for the newDescription parameter).
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------
void MgServerResourceService::UpdateGroup(CREFSTRING group, CREFSTRING newGroup,
    CREFSTRING newDescription )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::UpdateGroup()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->UpdateGroup(group, newGroup, newDescription);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.UpdateGroup")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Grants membership in the specified groups to the specified users.
/// </summary>
///
/// <param name="groups">
/// Groups to receive the specified users.
/// </param>
/// <param name="users">
/// IDs of users to gain membership in the specified groups.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgGroupNotFoundException
void MgServerResourceService::GrantGroupMembershipsToUsers( MgStringCollection* groups,
    MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GrantGroupMembershipsToUsers()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->GrantGroupMembershipsToUsers(groups, users);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GrantGroupMembershipsToUsers")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Revokes membership in the specified groups from the specified users.
/// </summary>
///
/// <param name="groups">
/// Groups to lose the specified users.
/// </param>
/// <param name="users">
/// IDs of users to lose membership in the specified groups.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgGroupNotFoundException
void MgServerResourceService::RevokeGroupMembershipsFromUsers( MgStringCollection* groups,
    MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::RevokeGroupMembershipsFromUsers()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->RevokeGroupMembershipsFromUsers(groups, users);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.RevokeGroupMembershipsFromUsers")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets a list of available roles.  Parameters specify contents of the list:
/// - If both user and group are NOT specified, return a list of all available roles.
/// - If group is specified, return a list of roles for that group.
/// - If user is specified, return a list of roles for that user.
/// - If both user and group are specified, throw an exception.
/// </summary>
///
/// <param name="user">
/// ID of user, if returning list of roles for a particular user; empty, otherwise.
/// </param>
/// <param name="group">
/// Name of group, if returning list of roles for a particular group; empty, otherwise.
/// </param>
///
/// <returns>
/// Result containing the appropriate list of roles.
/// </returns>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgStringCollection* MgServerResourceService::EnumerateRoles(CREFSTRING user,
    CREFSTRING group)
{
    Ptr<MgStringCollection> roles;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateRoles()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    roles = repositoryMan->EnumerateRoles(user, group);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateRoles")

    return roles.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets a list of unmanaged data paths
/// </summary>
///
/// <returns>
/// Result containing the appropriate list of data paths.
/// </returns>
///
///----------------------------------------------------------------------------

MgByteReader* MgServerResourceService::EnumerateUnmanagedData(
    CREFSTRING path, bool recursive, CREFSTRING type, CREFSTRING filter)
{
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::EnumerateUnmanagedData()");

    byteReader = MgUnmanagedDataManager::GetInstance()->EnumerateUnmanagedData(path, recursive, type, filter);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.EnumerateUnmanagedData")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Grants the specified roles to the specified users.
/// </summary>
/// <param name="roles">
/// List of roles to grant to the specified users.
/// </param>
/// <param name="users">
/// IDs of users to receive the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgRoleNotFoundException
void MgServerResourceService::GrantRoleMembershipsToUsers(MgStringCollection* roles,
    MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GrantRoleMembershipsToUsers()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->GrantRoleMembershipsToUsers( roles, users );

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GrantRoleMembershipsToUsers")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Revokes the specified roles from the specified users.
/// </summary>
///
/// <param name="roles">
/// List of roles to revoke from the specified users.
/// Repository.
/// <param name="users">
/// IDs of users to lose the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgUserNotFoundException
/// MgRoleNotFoundException
void MgServerResourceService::RevokeRoleMembershipsFromUsers(MgStringCollection* roles,
    MgStringCollection* users )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::RevokeRoleMembershipsFromUsers()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->RevokeRoleMembershipsFromUsers( roles, users );

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.RevokeRoleMembershipsFromUsers")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Grants the specified role memberships to the specified groups.
/// </summary>
///
/// <param name="roles">
/// List of roles to be granted to the specified groups.
/// </param>
/// <param name="groups">
/// Groups to receive the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
/// MgRoleNotFoundException
void MgServerResourceService::GrantRoleMembershipsToGroups(MgStringCollection* roles,
    MgStringCollection* groups )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::GrantRoleMembershipsToGroups()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->GrantRoleMembershipsToGroups( roles, groups );

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.GrantRoleMembershipsToGroups")
}

///////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Revokes the specified roles from the specified groups.
/// </summary>
///
/// <param name="roles">
/// Roles to be revoked from the groups.
/// </param>
/// <param name="groups">
/// Groups to lose the specified roles.
/// </param>
/// <returns>
/// Nothing
/// </returns>
///
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgGroupNotFoundException
/// MgRoleNotFoundException
void MgServerResourceService::RevokeRoleMembershipsFromGroups(MgStringCollection* roles,
    MgStringCollection* groups )
{
    MG_RESOURCE_SERVICE_TRY()

    MG_LOG_TRACE_ENTRY(L"MgServerResourceService::RevokeRoleMembershipsFromGroups()");

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    repositoryMan->RevokeRoleMembershipsFromGroups( roles, groups );

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.RevokeRoleMembershipsFromGroups")
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a new security cache.
/// </summary>
///----------------------------------------------------------------------------

MgSecurityCache* MgServerResourceService::CreateSecurityCache()
{
    Ptr<MgSecurityCache> securityCache;

    MG_RESOURCE_SERVICE_TRY()

    auto_ptr<MgSiteRepositoryManager> repositoryMan(
        new MgSiteRepositoryManager(*sm_siteRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    MgSiteResourceContentManager* resourceContentMan =
        dynamic_cast<MgSiteResourceContentManager*>(
            repositoryMan->GetResourceContentManager());
    ACE_ASSERT(NULL != resourceContentMan);

    securityCache = resourceContentMan->CreateSecurityCache();

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CreateSecurityCache")

    return securityCache.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Creates a new permission cache.
/// </summary>
///----------------------------------------------------------------------------

MgPermissionCache* MgServerResourceService::CreatePermissionCache()
{
    Ptr<MgPermissionCache> permissionCache;

    MG_RESOURCE_SERVICE_TRY()

    auto_ptr<MgLibraryRepositoryManager> repositoryMan(
        new MgLibraryRepositoryManager(*sm_libraryRepository));

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    MgResourceHeaderManager* resourceHeaderManager =
        dynamic_cast<MgResourceHeaderManager*>(
            repositoryMan->GetResourceHeaderManager());
    ACE_ASSERT(NULL != resourceHeaderManager);

    permissionCache = resourceHeaderManager->CreatePermissionCache();

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CreatePermissionCache")

    return permissionCache.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// specified resource. This method may throw any exception.
/// </summary>
///----------------------------------------------------------------------------

void MgServerResourceService::CheckPermission(MgResourceIdentifier* resource,
    CREFSTRING permission)
{
    MG_RESOURCE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerResourceService.CheckPermission",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    auto_ptr<MgApplicationRepositoryManager> repositoryMan(
        CreateApplicationRepositoryManager(resource));
    MgResourceContentManager* resourceContentMan =
        repositoryMan->GetResourceContentManager();
    ACE_ASSERT(NULL != resourceContentMan);

    MG_RESOURCE_SERVICE_BEGIN_OPERATION()

    resourceContentMan->CheckPermission(*resource, permission);

    MG_RESOURCE_SERVICE_END_OPERATION(sm_retryAttempts)

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.CheckPermission")
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// specified resource. This method may throw non-permission related exceptions.
/// </summary>
///----------------------------------------------------------------------------

bool MgServerResourceService::HasPermission(MgResourceIdentifier* resource,
    CREFSTRING permission)
{
    bool permitted = false;

    MG_RESOURCE_SERVICE_TRY()

    CheckPermission(resource, permission);

    MG_RESOURCE_SERVICE_CATCH(L"MgServerResourceService.HasPermission")

    if (NULL == mgException)
    {
        permitted = true;
    }
    else if (!mgException->IsOfClass(MapGuide_Exception_MgPermissionDeniedException) &&
             !mgException->IsOfClass(MapGuide_Exception_MgUnauthorizedAccessException))
    {
        MG_RESOURCE_SERVICE_THROW()
    }

    return permitted;
}

///----------------------------------------------------------------------------
/// <summary>
/// Performs checkpoints for all the repositories.
/// </summary>
///----------------------------------------------------------------------------

void MgServerResourceService::PerformRepositoryCheckpoints(UINT32 flags)
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_RESOURCE_SERVICE_TRY()

    if (NULL != sm_libraryRepository)
    {
        sm_libraryRepository->PerformCheckpoint(flags);
    }

    if (NULL != sm_siteRepository)
    {
        sm_siteRepository->PerformCheckpoint(flags);
    }

    if (NULL != sm_sessionRepository)
    {
        sm_sessionRepository->PerformCheckpoint(flags);
    }

    if (0 > sm_sessionRepositories.size())
    {
        for (std::map<STRING, MgSessionRepository* >::iterator i = sm_sessionRepositories.begin();i != sm_sessionRepositories.end(); ++i)
        {
            MgSessionRepository* sessionRepository = i->second;
            if(NULL != sessionRepository)
            {
                sessionRepository->PerformCheckpoint(flags);
            }
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgServerResourceService.PerformCheckpoint")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the current set of changed resources.
///
MgSerializableCollection* MgServerResourceService::GetChangedResources()
{
    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    if (sm_changedResources.empty())
    {
        return NULL;
    }

    Ptr<MgSerializableCollection> resources = new MgSerializableCollection();

    for (set<STRING>::const_iterator i = sm_changedResources.begin();
        i != sm_changedResources.end(); ++i)
    {
        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(*i);

        resources->Add(resource.p);
    }

    sm_changedResources.clear();

    return resources.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Updates the current set of changed resources.
///
void MgServerResourceService::UpdateChangedResources(MgSerializableCollection* resources)
{
    if (NULL != resources)
    {
        INT32 numResources = resources->GetCount();

        if (numResources > 0)
        {
            ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

            for (INT32 i = 0; i < numResources; ++i)
            {
                Ptr<MgSerializable> serializableObj = resources->GetItem(i);
                MgResourceIdentifier* resource = dynamic_cast<MgResourceIdentifier*>(
                    serializableObj.p);
                ACE_ASSERT(NULL != resource);

                if (NULL != resource)
                {
                    sm_changedResources.insert(resource->ToString());
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Updates the current set of changed resources.
///
void MgServerResourceService::UpdateChangedResources(const set<STRING>& resources)
{
    if (!resources.empty())
    {
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

        for (set<STRING>::const_iterator i = resources.begin();
            i != resources.end(); ++i)
        {
            sm_changedResources.insert(*i);
        }
    }
}

void MgServerResourceService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for Server-side service objects.
}
