//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "ServiceManager.h"
#include "ServerResourceService.h"
#include "TestResourceService.h"
#include "CppUnitExtensions.h"
#include "SecurityCache.h"
#include "../Common/Manager/FdoConnectionManager.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestResourceService, "TestResourceService");

const STRING adminName = L"Administrator";
const STRING adminPass = L"admin";
const STRING userLocale = L"en";
const STRING anonymousName = L"Anonymous";
const STRING TEST_LOCALE = L"en";

MgResourceIdentifier sessionRepositoryIdentifier(L"Session:TestRepository//");
MgResourceIdentifier sessionRepositoryNotExist(L"Session:DoesNotExist//");

MgResourceIdentifier libraryRepositoryIdentifier(L"Library://");
MgResourceIdentifier resourceIdentifier(L"Library://UnitTests/Data/test-1.FeatureSource"); // Original resource added
MgResourceIdentifier resourceIdentifier2(L"Library://UnitTests/Data/test-2.FeatureSource"); // Used in copy and move
MgResourceIdentifier resourceIdentifier3(L"Library://UnitTests/Data/World.DrawingSource"); // Added by package
MgResourceIdentifier resourceNotExist(L"Library://UnitTests/Data/resourcedoesnotexist.FeatureSource");

STRING resourceDataName = L"World_Countries.sdf";
STRING resourceDataName2 = L"New_World_Countries.sdf";

#ifdef _WIN32
STRING repositoryHeaderFileName = L"..\\UnitTestFiles\\SampleRepositoryHeader.xml";
STRING repositoryContentFileName = L"..\\UnitTestFiles\\SampleRepositoryContent.xml";
STRING resourceContentFileName = L"..\\UnitTestFiles\\TEST.FeatureSource";
STRING dataFileName = L"..\\UnitTestFiles\\World_Countries.sdf";
STRING packageName = L"..\\UnitTestFiles\\Shuttle.zip";
#else
STRING repositoryHeaderFileName = L"../UnitTestFiles/SampleRepositoryHeader.xml";
STRING repositoryContentFileName = L"../UnitTestFiles/SampleRepositoryContent.xml";
STRING resourceContentFileName = L"../UnitTestFiles/TEST.FeatureSource";
STRING dataFileName = L"../UnitTestFiles/World_Countries.sdf";
STRING packageName = L"../UnitTestFiles/Shuttle.zip";
#endif

void TestResourceService::setUp()
{
}

void TestResourceService::tearDown()
{
}

void TestResourceService::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Resource Service tests.\n")));

    try
    {
        #ifdef _DEBUG
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

void TestResourceService::TestEnd()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestEnd", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestEnd", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        // delete the drawing source definition
        Ptr<MgResourceIdentifier> mapres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/Shuttle.DrawingSource");
        pService->DeleteResource(mapres1);

        #ifdef _DEBUG
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }

    ACE_DEBUG((LM_INFO, ACE_TEXT("\nResource Service tests completed.\n\n")));
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the repositories.
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateRepositories()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_EnumerateRepositories", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateRepositories", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try to enumerate an invalid type of repository
            CPPUNIT_ASSERT_THROW_MG(pService->EnumerateRepositories(L"Library"), MgInvalidRepositoryTypeException*);

            //Enumerate the Session repositories
            Ptr<MgByteReader> byteReader = pService->EnumerateRepositories(L"Session");
            STRING mimeType = byteReader->GetMimeType();
            CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
        }

        //Try to enumerate repositories while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->EnumerateRepositories(L"Session"), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case creates a new repository
///----------------------------------------------------------------------------
void TestResourceService::TestCase_CreateRepository()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_CreateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_CreateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try to create a repository using a NULL identifier
            CPPUNIT_ASSERT_THROW_MG(pService->CreateRepository(NULL, NULL, NULL), MgNullArgumentException*);

            //Try to create a repository using an invalid repository identifier
            CPPUNIT_ASSERT_THROW_MG(pService->CreateRepository(&resourceIdentifier, NULL, NULL), MgInvalidRepositoryTypeException*);

            //Create a new repository
            pService->CreateRepository(&sessionRepositoryIdentifier, NULL, NULL);

            //Try to create the same repository again
            CPPUNIT_ASSERT_THROW_MG(pService->CreateRepository(&sessionRepositoryIdentifier, NULL, NULL), MgDuplicateRepositoryException*);
        }

        //Try to create a repository while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->CreateRepository(&sessionRepositoryIdentifier, NULL, NULL), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case updates the repository that was created earlier
///----------------------------------------------------------------------------
void TestResourceService::TestCase_UpdateRepository()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_UpdateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_UpdateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        Ptr<MgByteSource> headerSource;
        Ptr<MgByteSource> contentSource;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try to use a NULL argument
            CPPUNIT_ASSERT_THROW_MG(pService->UpdateRepository(NULL, NULL, NULL), MgNullArgumentException*);
            CPPUNIT_ASSERT_THROW_MG(pService->UpdateRepository(&sessionRepositoryIdentifier, NULL, NULL), MgNullArgumentException*);

            //Try to update a repository that doesn't exist
            contentSource = new MgByteSource(repositoryContentFileName);
            Ptr<MgByteReader> contentReader = contentSource->GetReader();
            CPPUNIT_ASSERT_THROW_MG(pService->UpdateRepository(&sessionRepositoryNotExist, contentReader, NULL), MgRepositoryNotFoundException*);

            //Try to update the repository using invalid content
            string str = "asdf";
            Ptr<MgByteReader> byteReader = MgUtil::GetByteReader(str);
            CPPUNIT_ASSERT_THROW_MG(pService->UpdateRepository(&sessionRepositoryIdentifier, byteReader, NULL), MgDbXmlException*);

            //Try to update the repository using valid header
            headerSource = new MgByteSource(repositoryHeaderFileName);
            Ptr<MgByteReader> headerReader = headerSource->GetReader();
            pService->UpdateRepository(&libraryRepositoryIdentifier, NULL, headerReader);
        }

        //Try to update repository while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            Ptr<MgByteReader> contentReader = contentSource->GetReader();
            CPPUNIT_ASSERT_THROW_MG(pService->UpdateRepository(&sessionRepositoryIdentifier, contentReader, NULL), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the content of the repository created earlier
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetRepositoryContent()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_GetRepositoryContent", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetRepositoryContent", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try to get repository content using a NULL argument
            CPPUNIT_ASSERT_THROW_MG(pService->GetRepositoryContent(NULL), MgNullArgumentException*);

            //Try to get the repository contents of a repository that doesn't exist
            CPPUNIT_ASSERT_THROW_MG(pService->GetRepositoryContent(&sessionRepositoryNotExist), MgRepositoryNotFoundException*);

            //Get the repository content using a valid argument
            Ptr<MgByteReader> byteReader = pService->GetRepositoryContent(&sessionRepositoryIdentifier);
            STRING mimeType = byteReader->GetMimeType();
            CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
        }

        //Try to get repository content while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->GetRepositoryContent(&sessionRepositoryIdentifier), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the header of the Library:// repository
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetRepositoryHeader()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_GetRepositoryHeader", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetRepositoryHeader", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to get repository header using a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->GetRepositoryHeader(NULL), MgNullArgumentException*);

        //Try to get repository header of a repository other than Library://
        CPPUNIT_ASSERT_THROW_MG(pService->GetRepositoryHeader(&sessionRepositoryIdentifier), MgInvalidRepositoryTypeException*);

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        // Get the Library repository header.
        Ptr<MgByteReader> byteReader = pService->GetRepositoryHeader(&libraryRepositoryIdentifier);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case applies a resource package
///----------------------------------------------------------------------------
void TestResourceService::TestCase_ApplyResourcePackage()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_ApplyResourcePackage", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_ApplyResourcePackage", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try using NULL argument
            CPPUNIT_ASSERT_THROW_MG(pService->ApplyResourcePackage(NULL), MgNullArgumentException*);

            Ptr<MgByteSource> byteSource = new MgByteSource(packageName);
            byteSource->SetMimeType(MgMimeType::Binary);
            Ptr<MgByteReader> byteReader = byteSource->GetReader();

            pService->ApplyResourcePackage(byteReader);
        }

        //Try to apply a package while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(anonymousName, L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            Ptr<MgByteSource> byteSource = new MgByteSource(packageName);
            byteSource->SetMimeType(MgMimeType::Binary);
            Ptr<MgByteReader> byteReader = byteSource->GetReader();

            CPPUNIT_ASSERT_THROW_MG(pService->ApplyResourcePackage(byteReader), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case deletes the repository created earlier
///----------------------------------------------------------------------------
void TestResourceService::TestCase_DeleteRepository()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_DeleteRepository", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_DeleteRepository", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Try to delete repository while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->DeleteRepository(&sessionRepositoryIdentifier), MgUnauthorizedAccessException*);
        }

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try to use a NULL argument
            CPPUNIT_ASSERT_THROW_MG(pService->DeleteRepository(NULL), MgNullArgumentException*);

            //Try to delete a repository that does not exist
            CPPUNIT_ASSERT_THROW_MG(pService->DeleteRepository(&sessionRepositoryNotExist), MgRepositoryNotFoundException*);

            //Delete the repository that was created earlier in TestCase_CreateRepository
            pService->DeleteRepository(&sessionRepositoryIdentifier);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the resources in Library:// using Xml
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateResourcesInXml()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_EnumerateResourcesInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateResourcesInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to enumerate resources using a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateResources(NULL, -1, L"", true, MgMimeType::Xml), MgNullArgumentException*);

        // Enumerate the Library resource.
        Ptr<MgByteReader> byteReader = pService->EnumerateResources(&libraryRepositoryIdentifier, -1, L"", true, MgMimeType::Xml);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the resources in Library:// using Json
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateResourcesInJson()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_EnumerateResourcesInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateResourcesInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to enumerate resources using a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateResources(NULL, -1, L"", MgMimeType::Json), MgNullArgumentException*);

        // Enumerate the Library resource.
        Ptr<MgByteReader> byteReader = pService->EnumerateResources(&libraryRepositoryIdentifier, -1, L"", true, MgMimeType::Json);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Json.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case uses the SetResource function to add and update
/// a resource
///----------------------------------------------------------------------------
void TestResourceService::TestCase_SetResource()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_SetResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_SetResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteSource> contentSource;
        contentSource = NULL;

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try to use NULL arguments
            CPPUNIT_ASSERT_THROW_MG(pService->SetResource(NULL, NULL, NULL), MgNullArgumentException*);
            CPPUNIT_ASSERT_THROW_MG(pService->SetResource(&resourceIdentifier, NULL, NULL), MgNullArgumentException*);

            //Add a new resource
            contentSource = new MgByteSource(resourceContentFileName);
            Ptr<MgByteReader> contentReader = contentSource->GetReader();
            pService->SetResource(&resourceIdentifier, contentReader, NULL);

            //Try to add the same resource again (should be fine and just update it)
            contentSource = new MgByteSource(resourceContentFileName);
            contentReader = contentSource->GetReader();
            pService->SetResource(&resourceIdentifier, contentReader, NULL);
        }

        //Try to add resource while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            Ptr<MgByteReader> contentReader = contentSource->GetReader();
            CPPUNIT_ASSERT_THROW_MG(pService->SetResource(&resourceIdentifier2, contentReader, NULL), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case moves the resource that was created earlier to a new
/// location
///----------------------------------------------------------------------------
void TestResourceService::TestCase_MoveResource()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_MoveResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_MoveResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);


            //Try to use NULL arguments
            CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(NULL, NULL, true), MgNullArgumentException*);
            CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(&resourceIdentifier, NULL, true), MgNullArgumentException*);
            CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(NULL, &resourceIdentifier, true), MgNullArgumentException*);

            //Try to move a resource that doesn't exist
            CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(&resourceNotExist, &resourceIdentifier2, false), MgResourceNotFoundException*);

            //Move the resource that was added earlier
            pService->MoveResource(&resourceIdentifier, &resourceIdentifier2, false);

            //Try to move the resource into itself (ie. itself)
            CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(&resourceIdentifier, &resourceIdentifier, false), MgInvalidArgumentException*);
        }


        //Try to move resource while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(&resourceIdentifier, &resourceIdentifier2, false), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case copies the resource that was just moved to the original
/// location that it was moved from.
///----------------------------------------------------------------------------
void TestResourceService::TestCase_CopyResource()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_CopyResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_CopyResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try to use NULL arguments
            CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(NULL, NULL, true), MgNullArgumentException*);
            CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(&resourceIdentifier2, NULL, true), MgNullArgumentException*);
            CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(NULL, &resourceIdentifier, true), MgNullArgumentException*);

            //Try to use source & destination as the same thing
            CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(&resourceIdentifier, &resourceIdentifier, false), MgInvalidArgumentException*);

            //Copy the moved resource back to its original location
            pService->CopyResource(&resourceIdentifier2, &resourceIdentifier, false);

            //Try to copy a resource to one that should now exist
            CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(&resourceIdentifier2, &resourceIdentifier, false), MgDuplicateResourceException*);
        }

        //Try to copy resource while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(&resourceIdentifier, &resourceIdentifier2, false), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the content of the resource using Xml
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetResourceContentInXml()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_GetResourceContentInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetResourceContentInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try to get the content using NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceContent(NULL, L"", MgMimeType::Xml), MgNullArgumentException*);

        //Try to get the content of a resource that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceContent(&resourceNotExist, L"", MgMimeType::Xml), MgResourceNotFoundException*);

        //Get the content of the resource that was added in TestCase_SetResource
        byteReader = pService->GetResourceContent(&resourceIdentifier, L"", MgMimeType::Xml);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the content of the resource using Json
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetResourceContentInJson()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_GetResourceContentInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetResourceContentInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try to get the content using NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceContent(NULL, L"", MgMimeType::Json), MgNullArgumentException*);

        //Try to get the content of a resource that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceContent(&resourceNotExist, L"", MgMimeType::Json), MgResourceNotFoundException*);

        //Get the content of the resource that was added in TestCase_SetResource
        byteReader = pService->GetResourceContent(&resourceIdentifier, L"", MgMimeType::Json);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Json.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the header of the resource using Xml
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetResourceHeaderInXml()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_GetResourceHeaderInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetResourceHeaderInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try to get the content using NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceHeader(NULL, MgMimeType::Xml), MgNullArgumentException*);

        //Try to get the content of a resource that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceHeader(&resourceNotExist, MgMimeType::Xml), MgResourceNotFoundException*);

        //Get the content of the resource that was added in TestCase_SetResource
        byteReader = pService->GetResourceHeader(&resourceIdentifier, MgMimeType::Xml);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the header of the resource using Json
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetResourceHeaderInJson()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_GetResourceHeaderInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetResourceHeaderInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try to get the content using NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceHeader(NULL, MgMimeType::Json), MgNullArgumentException*);

        //Try to get the content of a resource that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceHeader(&resourceNotExist, MgMimeType::Json), MgResourceNotFoundException*);

        //Get the content of the resource that was added in TestCase_SetResource
        byteReader = pService->GetResourceHeader(&resourceIdentifier, MgMimeType::Json);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Json.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the references of the resource using Xml
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateReferencesInXml()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_EnumerateReferencesInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateReferencesInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try to get the content using NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateReferences(NULL, MgMimeType::Xml), MgNullArgumentException*);

        //Get the content of the resource that was added in TestCase_SetResource
        byteReader = pService->EnumerateReferences(&resourceIdentifier, MgMimeType::Xml);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the references of the resource using Json
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateReferencesInJson()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_EnumerateReferencesInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateReferencesInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try to get the content using NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateReferences(NULL, MgMimeType::Json), MgNullArgumentException*);

        //Get the content of the resource that was added in TestCase_SetResource
        byteReader = pService->EnumerateReferences(&resourceIdentifier, MgMimeType::Json);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Json.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case changes the owner of a resource
///----------------------------------------------------------------------------
void TestResourceService::TestCase_ChangeResourceOwner()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_ChangeResourceOwner", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_ChangeResourceOwner", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try to change the user using NULL arguments
            CPPUNIT_ASSERT_THROW_MG(pService->ChangeResourceOwner(NULL, L"", false), MgNullArgumentException*);

            //Try to change the owner of a resource that doesn't exist
            //TODO: this does not throw exception, should it?
            //CPPUNIT_ASSERT_THROW_MG(pService->ChangeResourceOwner(&resourceNotExist, adminName, false), MgResourceNotFoundException*);

            //Try to change the owner of the resource created to a user that doesn't exist
            CPPUNIT_ASSERT_THROW_MG(pService->ChangeResourceOwner(&resourceIdentifier, L"DoesNotExist", false), MgUserNotFoundException*);

            //Change the resource owner to administrator
            pService->ChangeResourceOwner(&resourceIdentifier, adminName, false);
        }

        //Try to copy resource while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->ChangeResourceOwner(&resourceIdentifier, adminName, false), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case sets all decendants of the resource to inherit permissions
///----------------------------------------------------------------------------
void TestResourceService::TestCase_InheritPermissionsFrom()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_InheritPermissionsFrom", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_InheritPermissionsFrom", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try to use a NULL argument
            CPPUNIT_ASSERT_THROW_MG(pService->InheritPermissionsFrom(NULL), MgNullArgumentException*);

            //Set resources to inherit permissions
            pService->InheritPermissionsFrom(&libraryRepositoryIdentifier);
        }

        //Try to perform this operation while not an administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->InheritPermissionsFrom(&libraryRepositoryIdentifier), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the resource data of the resource that was
/// added earlier using Xml
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateResourceDataInXml()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_EnumerateResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try enumerating using a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateResourceData(NULL, MgMimeType::Xml), MgNullArgumentException*);

        //Try enumerating using a resource that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateResourceData(&resourceNotExist, MgMimeType::Xml), MgResourceNotFoundException*);

        //Enumerate the resource data of the resource added earlier
        byteReader = pService->EnumerateResourceData(&resourceIdentifier, MgMimeType::Xml);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the resource data of the resource that was
/// added earlier using Json
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateResourceDataInJson()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_EnumerateResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try enumerating using a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateResourceData(NULL, MgMimeType::Json), MgNullArgumentException*);

        //Try enumerating using a resource that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateResourceData(&resourceNotExist, MgMimeType::Json), MgResourceNotFoundException*);

        //Enumerate the resource data of the resource added earlier
        byteReader = pService->EnumerateResourceData(&resourceIdentifier, MgMimeType::Json);
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Json.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case sets the data of a resource
///----------------------------------------------------------------------------
void TestResourceService::TestCase_SetResourceData()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_SetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_SetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        Ptr<MgByteSource> dataSource;
        dataSource = NULL;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try using a NULL identifier
            CPPUNIT_ASSERT_THROW_MG(pService->SetResourceData(NULL, L"", L"", NULL), MgNullArgumentException*);

            //Try to name the resource data with a 0 character string
            dataSource = new MgByteSource(dataFileName);
            Ptr<MgByteReader> dataReader = dataSource->GetReader();
            CPPUNIT_ASSERT_THROW_MG(pService->SetResourceData(&resourceIdentifier, L"", L"File", dataReader), MgNullArgumentException*);

            //Set the resource data
            dataSource = new MgByteSource(dataFileName);
            dataReader = dataSource->GetReader();
            pService->SetResourceData(&resourceIdentifier, resourceDataName, L"File", dataReader);

        }

        //Try to set resource data while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            Ptr<MgByteReader> dataReader = dataSource->GetReader();
            CPPUNIT_ASSERT_THROW_MG(pService->SetResourceData(&resourceIdentifier, resourceDataName, L"File", dataReader), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case renames a resource's data
///----------------------------------------------------------------------------
void TestResourceService::TestCase_RenameResourceData()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_RenameResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_RenameResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try using a null argument for the idenfier
            CPPUNIT_ASSERT_THROW_MG(pService->RenameResourceData(NULL, resourceDataName, resourceDataName2, true), MgNullArgumentException*);

            //Try using a resource identifier that doesn't exist
            CPPUNIT_ASSERT_THROW_MG(pService->RenameResourceData(&resourceNotExist, resourceDataName, resourceDataName2, true), MgResourceNotFoundException*);

            //Try to use a resource data name that doesn't exist
            CPPUNIT_ASSERT_THROW_MG(pService->RenameResourceData(&resourceIdentifier, L"DoesNotExist", resourceDataName2, true), MgResourceDataNotFoundException*);

            //Try to name the resource data with a 0 character string
            CPPUNIT_ASSERT_THROW_MG(pService->RenameResourceData(&resourceIdentifier, resourceDataName, L"", true), MgNullArgumentException*);

            //Try to do a valid rename
            pService->RenameResourceData(&resourceIdentifier, resourceDataName, resourceDataName2, false);
            pService->RenameResourceData(&resourceIdentifier, resourceDataName2, resourceDataName, false);
        }

        //Try to rename resource data while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->RenameResourceData(&resourceIdentifier, resourceDataName, resourceDataName2, false), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case retrieves the data of the resource using Xml
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetResourceDataInXml()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_GetResourceDataInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetResourceDataInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try to get resource data using a NULL identifier
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(NULL, resourceDataName, L"", MgMimeType::Xml), MgNullArgumentException*);

        //Try to get resource data using an empty data name string
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(&resourceIdentifier, L"", L"", MgMimeType::Xml), MgNullArgumentException*);

        //Try to get the resource data of a resource that does not exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(&resourceNotExist, resourceDataName, L"", MgMimeType::Xml), MgResourceNotFoundException*);

        //Try to get the resource data of a data name that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(&resourceIdentifier, L"DoesNotExist", L"", MgMimeType::Xml), MgResourceDataNotFoundException*);

        //Get resource data using valid arguments
        byteReader = pService->GetResourceData(&resourceIdentifier, resourceDataName, L"", MgMimeType::Xml);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case retrieves the data of the resource using Json
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetResourceDataInJson()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_GetResourceDataInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetResourceDataInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(adminName, adminPass);
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try to get resource data using a NULL identifier
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(NULL, resourceDataName, L"", MgMimeType::Json), MgNullArgumentException*);

        //Try to get resource data using an empty data name string
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(&resourceIdentifier, L"", L"", MgMimeType::Json), MgNullArgumentException*);

        //Try to get the resource data of a resource that does not exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(&resourceNotExist, resourceDataName, L"", MgMimeType::Json), MgResourceNotFoundException*);

        //Try to get the resource data of a data name that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(&resourceIdentifier, L"DoesNotExist", L"", MgMimeType::Json), MgResourceDataNotFoundException*);

        //Get resource data using valid arguments
        byteReader = pService->GetResourceData(&resourceIdentifier, resourceDataName, L"", MgMimeType::Json);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case deletes the data of the resource
///----------------------------------------------------------------------------
void TestResourceService::TestCase_DeleteResourceData()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_DeleteResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_DeleteResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try using a NULL resource identifier
            CPPUNIT_ASSERT_THROW_MG(pService->DeleteResourceData(NULL, resourceDataName), MgNullArgumentException*);

            //Try using an empty string for the resource data name
            CPPUNIT_ASSERT_THROW_MG(pService->DeleteResourceData(&resourceIdentifier, L""), MgNullArgumentException*);

            //Try deleting data that does not exist
            CPPUNIT_ASSERT_THROW_MG(pService->DeleteResourceData(&resourceIdentifier, L"DoesNotExist"), MgResourceDataNotFoundException*);

            //Delete the resource data that was set earlier
            pService->DeleteResourceData(&resourceIdentifier, resourceDataName);
        }

        //Try to rename resource data while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->DeleteResourceData(&resourceIdentifier, resourceDataName2), MgUnauthorizedAccessException*);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case deletes the resources that were added and copied in the
/// earlier tests
///----------------------------------------------------------------------------
void TestResourceService::TestCase_DeleteResource()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_DeleteResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_DeleteResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pFeatureService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pFeatureService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_DeleteResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            //Try to use a NULL argument
            CPPUNIT_ASSERT_THROW_MG(pService->DeleteResource(NULL), MgNullArgumentException*);

            //Try to delete a resource that doesn't exist. We allow doing this for performance reason.
            pService->DeleteResource(&resourceNotExist);

            //Delete resources added earlier
            pService->DeleteResource(&resourceIdentifier);
            pService->DeleteResource(&resourceIdentifier2);
            pService->DeleteResource(&resourceIdentifier3);
        }

        //Try to delete resource while not an admin
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"", L"");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);
            CPPUNIT_ASSERT_THROW_MG(pService->DeleteResource(&resourceIdentifier), MgUnauthorizedAccessException*);
        }

        // Try to delete resource after FDO exception
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            Ptr<MgResourceIdentifier> resource;
            resource = new MgResourceIdentifier(L"Library://UnitTests/Data/TEST.FeatureSource");

            #ifdef _WIN32
            STRING rcName = L"..\\UnitTestFiles\\TEST.FeatureSource";
            STRING dfName = L"..\\UnitTestFiles\\TEST.sdf";
            #else
            STRING rcName = L"../UnitTestFiles/TEST.FeatureSource";
            STRING dfName = L"../UnitTestFiles/TEST.sdf";
            #endif

            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            Ptr<MgByteSource> contentSource;
            contentSource = new MgByteSource(rcName);
            Ptr<MgByteReader> contentReader = contentSource->GetReader();
            pService->SetResource(resource, contentReader, NULL);

            Ptr<MgByteSource> dataSource;
            dataSource = new MgByteSource(dfName);

            Ptr<MgByteReader> dataReader;
            dataReader = dataSource->GetReader();
            pService->SetResourceData(resource, L"TEST.sdf", L"File", dataReader);
            dataReader = NULL; // Holds on to the file, so must release resources!

            // Force an FDO exception
            Ptr<MgFeatureAggregateOptions> options = new MgFeatureAggregateOptions();
            STRING className = L"TEST";
            options->AddFeatureProperty(L"Data");

            Ptr<MgDataReader> reader = pFeatureService->SelectAggregate(resource, className, options);
            bool bResult = reader->ReadNext();
            CPPUNIT_ASSERT(bResult);

            reader->Close();
            reader = NULL; // Holds on to the file, so must release resources!

            // Attempt to delete the resource
            pService->DeleteResource(resource);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(FdoException* e)
    {
        FDO_SAFE_RELEASE(e);
        CPPUNIT_FAIL("FdoException occured");
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the unmanaged data using Xml
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateUnmanagedDataInXml()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_EnumerateUnmanagedDataInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateUnmanagedDataInXml", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Try to enumerate mappings
        Ptr<MgByteReader> byteReader0 = pService->EnumerateUnmanagedData(L"", false, L"FOLDERS", L"", MgMimeType::Xml);
        STRING mimeType0 = byteReader0->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType0.c_str(), MgMimeType::Xml.c_str()) == 0);

        // Enumerate all unmanaged data files
        Ptr<MgByteReader> byteReader1 = pService->EnumerateUnmanagedData(L"", true, L"FILES", L"", MgMimeType::Xml);
        STRING mimeType1 = byteReader1->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType1.c_str(), MgMimeType::Xml.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the unmanaged data using Json
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateUnmanagedDataInJson()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.TestCase_EnumerateUnmanagedDataInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateUnmanagedDataInJson", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Try to enumerate mappings
        Ptr<MgByteReader> byteReader0 = pService->EnumerateUnmanagedData(L"", false, L"FOLDERS", L"", MgMimeType::Json);
        STRING mimeType0 = byteReader0->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType0.c_str(), MgMimeType::Json.c_str()) == 0);

        // Enumerate all unmanaged data files
        Ptr<MgByteReader> byteReader1 = pService->EnumerateUnmanagedData(L"", true, L"FILES", L"", MgMimeType::Json);
        STRING mimeType1 = byteReader1->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType1.c_str(), MgMimeType::Json.c_str()) == 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}
