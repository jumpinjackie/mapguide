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

#include "MgDesktop.h"
#include "Fdo.h"
#include "TestResourceService.h"
#include "CppUnitExtensions.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestResourceService, "TestResourceService");

// define thread group for tiling tests
#define THREAD_GROUP 65530

#define TESTREQUESTS 500

static const INT32 MG_TEST_THREADS = 8; // Adjust this to get failures!

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
MgResourceIdentifier resourceIdentifier4(L"Library://UnitTests/Layer/test-1.LayerDefinition"); // For cascade move resource

STRING resourceDataName = L"World_Countries.sdf";
STRING resourceDataName2 = L"New_World_Countries.sdf";

#ifdef _WIN32
STRING repositoryHeaderFileName = L"..\\UnitTestFiles\\SampleRepositoryHeader.xml";
STRING repositoryContentFileName = L"..\\UnitTestFiles\\SampleRepositoryContent.xml";
STRING resourceContentFileName = L"..\\UnitTestFiles\\TEST.FeatureSource";
STRING resourceContentFileName2 = L"..\\UnitTestFiles\\TEST.LayerDefinition";
STRING dataFileName = L"..\\UnitTestFiles\\World_Countries.sdf";
STRING packageName = L"..\\UnitTestFiles\\Shuttle.zip";
#else
STRING repositoryHeaderFileName = L"../UnitTestFiles/SampleRepositoryHeader.xml";
STRING repositoryContentFileName = L"../UnitTestFiles/SampleRepositoryContent.xml";
STRING resourceContentFileName = L"../UnitTestFiles/TEST.FeatureSource";
STRING resourceContentFileName2 = L"../UnitTestFiles/TEST.LayerDefinition";
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
}

void TestResourceService::TestEnd()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestEnd", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // delete the drawing source definition
        Ptr<MgResourceIdentifier> mapres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/Shuttle.DrawingSource");
        pService->DeleteResource(mapres1);

        // Delete any resources created
        if (pService->ResourceExists(&resourceIdentifier))
            pService->DeleteResource(&resourceIdentifier);
        
        if (pService->ResourceExists(&resourceIdentifier2))
            pService->DeleteResource(&resourceIdentifier2);

        if (pService->ResourceExists(&resourceIdentifier3))
            pService->DeleteResource(&resourceIdentifier3);

        if (pService->ResourceExists(&resourceIdentifier4))
            pService->DeleteResource(&resourceIdentifier4);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateRepositories", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateRepositories(L"Library"), MgNotImplementedException*);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_CreateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CPPUNIT_ASSERT_THROW_MG(pService->CreateRepository(NULL, NULL, NULL), MgNotImplementedException*);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_UpdateRepository", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CPPUNIT_ASSERT_THROW_MG(pService->UpdateRepository(NULL, NULL, NULL), MgNotImplementedException*);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetRepositoryContent", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        
        CPPUNIT_ASSERT_THROW_MG(pService->GetRepositoryContent(NULL), MgNotImplementedException*);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetRepositoryHeader", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to get repository header using a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->GetRepositoryHeader(NULL), MgNotImplementedException*);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_ApplyResourcePackage", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try using NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->ApplyResourcePackage(NULL), MgNullArgumentException*);

        //Not try the real thing
        Ptr<MgByteSource> byteSource = new MgByteSource(packageName);
        byteSource->SetMimeType(MgMimeType::Binary);
        Ptr<MgByteReader> byteReader = byteSource->GetReader();

        pService->ApplyResourcePackage(byteReader);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_DeleteRepository", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CPPUNIT_ASSERT_THROW_MG(pService->DeleteRepository(NULL), MgNotImplementedException*);
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
/// This test case checks to see if the specified resource exists.
///----------------------------------------------------------------------------
void TestResourceService::TestCase_ResourceExists()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> service = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));

        if (NULL == service)
        {
            throw new MgServiceNotAvailableException(
                L"TestResourceService.TestCase_ResourceExists",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Try to check a NULL resource.
        bool existed = false;
        CPPUNIT_ASSERT_THROW_MG(service->ResourceExists(NULL), MgNullArgumentException*);

        // Try to check a resource that exists
        existed = service->ResourceExists(&libraryRepositoryIdentifier);
        CPPUNIT_ASSERT(existed);

        // Try to check a resource that does not exist.
        existed = service->ResourceExists(&resourceNotExist);
        CPPUNIT_ASSERT(!existed);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the resources in Library://
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateResources()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateResources", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to enumerate resources using a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateResources(NULL, -1, L"", true), MgNullArgumentException*);

        // Enumerate the Library resource.
        Ptr<MgByteReader> byteReader = pService->EnumerateResources(&libraryRepositoryIdentifier, -1, L"", true);
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
/// This test case uses the SetResource function to add and update
/// a resource
///----------------------------------------------------------------------------
void TestResourceService::TestCase_SetResource()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_SetResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteSource> contentSource;
        contentSource = NULL;

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

        //Add another resource layer definition, which references to the feature source. This is for cascade MoveResource test.
        contentSource = new MgByteSource(resourceContentFileName2);
        contentReader = contentSource->GetReader();
        pService->SetResource(&resourceIdentifier4, contentReader, NULL);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_MoveResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to use NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(NULL, NULL, true, false), MgNullArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(&resourceIdentifier, NULL, true, true), MgNullArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(NULL, &resourceIdentifier, true, false), MgNullArgumentException*);

        //Try to move a resource that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(&resourceNotExist, &resourceIdentifier2, false, true), MgResourceNotFoundException*);

        //Move the resource that was added earlier with cascade = true, and check referencing resource.
        pService->MoveResource(&resourceIdentifier, &resourceIdentifier2, false, true);
        CPPUNIT_ASSERT(!pService->ResourceExists(&resourceIdentifier));
        //Ptr<MgByteReader> byteReader = pService->GetResourceContent(&resourceIdentifier4, L"");
        //STRING referenceContent = byteReader->ToString();
        //STRING featureIdTag = L"<ResourceId>Library://UnitTests/Data/test-2.FeatureSource</ResourceId>";
        //CPPUNIT_ASSERT(referenceContent.find(featureIdTag) != STRING::npos);

        //Move the resource again with cascade = false, and check referencing resource.
        pService->MoveResource(&resourceIdentifier2, &resourceIdentifier, false, false);
        //byteReader = pService->GetResourceContent(&resourceIdentifier4, L"");
        //referenceContent = byteReader->ToString();
        //featureIdTag = L"<ResourceId>Library://UnitTests/Data/test-2.FeatureSource</ResourceId>";
        //CPPUNIT_ASSERT(referenceContent.find(featureIdTag) != STRING::npos);
        CPPUNIT_ASSERT(!pService->ResourceExists(&resourceIdentifier2));

        //Try to move the resource into itself (ie. itself)
        CPPUNIT_ASSERT_THROW_MG(pService->MoveResource(&resourceIdentifier, &resourceIdentifier, false), MgInvalidArgumentException*);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_CopyResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to use NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(NULL, NULL, true), MgNullArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(&resourceIdentifier, NULL, true), MgNullArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(NULL, &resourceIdentifier2, true), MgNullArgumentException*);

        //Try to use source & destination as the same thing
        CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(&resourceIdentifier2, &resourceIdentifier2, false), MgInvalidArgumentException*);

        //Copy the moved resource to another location
        pService->CopyResource(&resourceIdentifier, &resourceIdentifier2, false);

        //Try to copy a resource to one that should now exist
        CPPUNIT_ASSERT_THROW_MG(pService->CopyResource(&resourceIdentifier2, &resourceIdentifier, false), MgDuplicateResourceException*);
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
/// This test case gets the content of the resource
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetResourceContent()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetResourceContent", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try to get the content using NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceContent(NULL, L""), MgNullArgumentException*);

        //Try to get the content of a resource that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceContent(&resourceNotExist, L""), MgResourceNotFoundException*);

        //Get the content of the resource that was added in TestCase_SetResource
        byteReader = pService->GetResourceContent(&resourceIdentifier, L"");
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
/// This test case gets the contents of a collection of resources
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetResourceContents()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetResourceContents", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to get the contents using NULL arguments
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceContents(NULL, NULL), MgNullArgumentException*);

        //Try to get the content of a resource that doesn't exist
        Ptr<MgStringCollection> resourceNotExistCol = new MgStringCollection();
        resourceNotExistCol->Add(resourceNotExist.ToString());
        resourceNotExistCol->Add(resourceIdentifier.ToString());
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceContents(resourceNotExistCol, NULL), MgResourceNotFoundException*);

        //Get the content of the resource that was added in TestCase_SetResource
        Ptr<MgStringCollection> resourceIds = new MgStringCollection();
        resourceIds->Add(resourceIdentifier.ToString());
        resourceIds->Add(resourceIdentifier4.ToString());
        Ptr<MgStringCollection> ret = pService->GetResourceContents(resourceIds, NULL);
        CPPUNIT_ASSERT(ret->GetCount() == 2);
        for(int i = 0; i < ret->GetCount(); i ++)
        {
            CPPUNIT_ASSERT(!ret->GetItem(i).empty());
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
/// This test case gets the header of the resource
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetResourceHeader()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetResourceHeader", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceHeader(NULL), MgNotImplementedException*);
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
/// This test case enumerates the references of the resource
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateReferences()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateReferences", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateReferences(NULL), MgNotImplementedException*);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_ChangeResourceOwner", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CPPUNIT_ASSERT_THROW_MG(pService->ChangeResourceOwner(NULL, L"", false), MgNotImplementedException*);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_InheritPermissionsFrom", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CPPUNIT_ASSERT_THROW_MG(pService->InheritPermissionsFrom(NULL), MgNotImplementedException*);
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
/// added earlier
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateResourceData()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try enumerating using a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateResourceData(NULL), MgNullArgumentException*);

        //Try enumerating using a resource that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->EnumerateResourceData(&resourceNotExist), MgResourceNotFoundException*);

        //Enumerate the resource data of the resource added earlier
        byteReader = pService->EnumerateResourceData(&resourceIdentifier);
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
/// This test case sets the data of a resource
///----------------------------------------------------------------------------
void TestResourceService::TestCase_SetResourceData()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_SetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteSource> dataSource;
        dataSource = NULL;

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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_RenameResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        
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
/// This test case retrieves the data of the resource
///----------------------------------------------------------------------------
void TestResourceService::TestCase_GetResourceData()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_GetResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader;
        byteReader = NULL;

        //Try to get resource data using a NULL identifier
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(NULL, resourceDataName, L""), MgNullArgumentException*);

        //Try to get resource data using an empty data name string
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(&resourceIdentifier, L"", L""), MgNullArgumentException*);

        //Try to get the resource data of a resource that does not exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(&resourceNotExist, resourceDataName, L""), MgResourceNotFoundException*);

        //Try to get the resource data of a data name that doesn't exist
        CPPUNIT_ASSERT_THROW_MG(pService->GetResourceData(&resourceIdentifier, L"DoesNotExist", L""), MgResourceDataNotFoundException*);

        //Get resource data using valid arguments
        byteReader = pService->GetResourceData(&resourceIdentifier, resourceDataName, L"");
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_DeleteResourceData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try using a NULL resource identifier
        CPPUNIT_ASSERT_THROW_MG(pService->DeleteResourceData(NULL, resourceDataName), MgNullArgumentException*);

        //Try using an empty string for the resource data name
        CPPUNIT_ASSERT_THROW_MG(pService->DeleteResourceData(&resourceIdentifier, L""), MgNullArgumentException*);

        //Try deleting data that does not exist
        CPPUNIT_ASSERT_THROW_MG(pService->DeleteResourceData(&resourceIdentifier, L"DoesNotExist"), MgResourceDataNotFoundException*);

        //Delete the resource data that was set earlier
        pService->DeleteResourceData(&resourceIdentifier, resourceDataName);
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
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_DeleteResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pFeatureService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pFeatureService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_DeleteResource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Try to use a NULL argument
        CPPUNIT_ASSERT_THROW_MG(pService->DeleteResource(NULL), MgNullArgumentException*);

        //Try to delete a resource that doesn't exist. We allow doing this for performance reason.
        pService->DeleteResource(&resourceNotExist);

        //Delete resources added earlier
        pService->DeleteResource(&resourceIdentifier);
        pService->DeleteResource(&resourceIdentifier2);
        pService->DeleteResource(&resourceIdentifier3);

        // Try to delete resource after FDO exception
        Ptr<MgResourceIdentifier> resource;
        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/TEST.FeatureSource");

        #ifdef _WIN32
        STRING rcName = L"..\\UnitTestFiles\\TEST.FeatureSource";
        STRING dfName = L"..\\UnitTestFiles\\TEST.sdf";
        #else
        STRING rcName = L"../UnitTestFiles/TEST.FeatureSource";
        STRING dfName = L"../UnitTestFiles/TEST.sdf";
        #endif

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
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(FdoException* e)
    {
        STRING message = L"FdoException occurred: ";
        message += e->GetExceptionMessage();
        FDO_SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the unmanaged data
///----------------------------------------------------------------------------
void TestResourceService::TestCase_EnumerateUnmanagedData()
{
    try
    {
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.TestCase_EnumerateUnmanagedData", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Try to enumerate mappings
        Ptr<MgByteReader> byteReader0 = pService->EnumerateUnmanagedData(L"", false, MgUnmanagedDataType::Folders, L"");
        STRING mimeType0 = byteReader0->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType0.c_str(), MgMimeType::Xml.c_str()) == 0);

        // Enumerate all unmanaged data files
        Ptr<MgByteReader> byteReader1 = pService->EnumerateUnmanagedData(L"", true, MgUnmanagedDataType::Files, L"");
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

// data structure which is passed to each thread
struct ResourceThreadData
{
    INT32 threadId;
    INT32 command;
    bool success;
    bool done;
    STRING session;
};

// the method which gets executed by the ACE worker thread
ACE_THR_FUNC_RETURN RepositoryWorker(void* param)
{
    // get the data for this thread
    ResourceThreadData* threadData = (ResourceThreadData*)param;
    INT32 threadId = threadData->threadId;
    INT32 command = threadData->command;
    STRING session = threadData->session;

    ACE_DEBUG((LM_INFO, ACE_TEXT("> thread %d started\n"), threadId));

    try
    {
        // get the tile service instance
        Ptr<MgServiceFactory> fact = new MgServiceFactory();
        Ptr<MgResourceService> svcResource = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        assert(svcResource != NULL);

        STRING resource = L"Session:";
        resource += session;
        resource += L"//UnitTests/Data/test-1.FeatureSource";
        Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(resource);

        switch (command)
        {
        case 0:
            {
            //Set the resource
            Ptr<MgByteSource> contentSource = new MgByteSource(resourceContentFileName);
            Ptr<MgByteReader> contentReader = contentSource->GetReader();
            svcResource->SetResource(resId, contentReader, NULL);
            }
        case 1:
            {
            //Set the resource data
            Ptr<MgByteSource> dataSource = new MgByteSource(dataFileName);
            Ptr<MgByteReader> dataReader = dataSource->GetReader();
            svcResource->SetResourceData(resId, resourceDataName, L"File", dataReader);
            }
            // Need to add a case that updates the session with runtime map
        }

        threadData->success = true;
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        message += L"\n";
        message += e->GetStackTrace(TEST_LOCALE);
        SAFE_RELEASE(e);
        ACE_DEBUG((LM_INFO, ACE_TEXT("RepositoryWorker(%d) - Exception:\n%W\n"), threadId, message.c_str()));
    }
    catch (...)
    {
        throw;
    }

    ACE_DEBUG((LM_INFO, ACE_TEXT("> thread %d done\n"), threadId));

    threadData->done = true;
    return 0;
}

/*
void TestResourceService::TestCase_RepositoryBusy()
{
    // specify the number of threads to use
    const INT32 numThreads = MG_TEST_THREADS;
    ResourceThreadData threadData[numThreads];

    try
    {
        long lStart = GetTickCount();

        // get the tile service instance
        Ptr<MgResourceService> svcResource = MgServiceFactory::CreateResourceService();
        assert(svcResource != NULL);

        // need a thread manager
        ACE_Thread_Manager* manager = ACE_Thread_Manager::instance();

        // initialize the thread data
        for (INT32 i=0; i<numThreads; i++)
        {
            // Initialize the resource
            wchar_t wszBuffer[255];
            swprintf(wszBuffer, 255, L"48cb0286-%04d-1000-8001-005056c00008_en_6F7A8590045708AE0D05", i);

            // Session format: 48cb0286-0000-1000-8001-005056c00008_en_6F7A8590045708AE0D05
            STRING session = wszBuffer;
            STRING resource = L"Session:";
            resource += session;
            resource += L"//";

            // Create session resource
            Ptr<MgResourceIdentifier> resIdSession = new MgResourceIdentifier(resource);
            svcResource->CreateRepository(resIdSession, NULL, NULL);

            // Create feature source resource
            resource += L"UnitTests/Data/test-1.FeatureSource";
            Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(resource);
            Ptr<MgByteSource> contentSource = new MgByteSource(resourceContentFileName);
            Ptr<MgByteReader> contentReader = contentSource->GetReader();
            svcResource->SetResource(resId, contentReader, NULL);

            // Set the thread specific data
            threadData[i].threadId = i;
            threadData[i].success  = false;
            threadData[i].done     = true;
            threadData[i].session  = session;
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestCase_RepositoryBusy\nThreads: %d  Requests: %d\n\n"), numThreads, TESTREQUESTS));

        INT32 nRequest = 0;
        INT32 nSuccessful = 0;
        bool bExceptionOcurred = false;
        for (;;)
        {
            INT32 dc = 0;
            for (INT32 i=0; i<numThreads; i++)
            {
                // check if the thread is available
                if (threadData[i].done)
                {
                    if(threadData[i].success)
                        nSuccessful++;

                    threadData[i].success = false;
                    threadData[i].done    = false;
                    threadData[i].command = i%2;

                    // spawn a new thread using a specific group id
                    int thid = manager->spawn(ACE_THR_FUNC(RepositoryWorker), &threadData[i], 0, NULL, NULL, 0, THREAD_GROUP);
                    nRequest++;
                }
            }

            // move on if all threads are done
            if ((nRequest > TESTREQUESTS) || (bExceptionOcurred))
                break;

            // under Linux we get a deadlock if we don't call this every once in a while
            if (nRequest % 25 == 0)
                manager->wait_grp(THREAD_GROUP);
            else
            {
                // pause briefly (10ms) before checking again
                ACE_Time_Value t(0, 10000);
                ACE_OS::sleep(t);
            }
        }

        // make sure all threads in the group have completed
        manager->wait_grp(THREAD_GROUP);

        for (INT32 i=0; i<numThreads; i++)
        {
            if(threadData[i].success)
                nSuccessful++;
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("\nRequests: %d/%d - Execution Time: = %6.4f (s)\n"), nSuccessful, nRequest, ((GetTickCount()-lStart)/1000.0)));
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
*/