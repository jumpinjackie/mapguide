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

#include "MapGuideCommon.h"
#include "TestFeatureService.h"
#include "CppUnitExtensions.h"
#include "ServiceManager.h"
#include "FdoConnectionManager.h"
#include "Services/FeatureService.h"
#include "ServerFeatureService.h"
#include "Fdo.h"


const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestFeatureService, "TestFeatureService");


void TestFeatureService::setUp()
{
}


void TestFeatureService::tearDown()
{
}


void TestFeatureService::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Feature Service tests.\n")));

    try
    {
        #ifdef _DEBUG
        ACE_DEBUG((LM_INFO, ACE_TEXT("TestFeatureService::TestStart()\n")));
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif

        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestResourceService.setUp", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestResourceService.setUp", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Set the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(TEST_LOCALE);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            MgResourceIdentifier resourceIdentifier1(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
            MgResourceIdentifier resourceIdentifier2(L"Library://UnitTests/Data/Redding_Parcels.FeatureSource");
            MgResourceIdentifier resourceIdentifier3(L"Library://UnitTests/Data/Sheboygan_BuildingOutlines.FeatureSource");
            MgResourceIdentifier resourceIdentifier4(L"Library://UnitTests/Data/Sheboygan_VotingDistricts.FeatureSource");
            MgResourceIdentifier resourceIdentifier5(L"Library://UnitTests/Data/TestChainedInner1ToManyJoin.FeatureSource");
            MgResourceIdentifier resourceIdentifier6(L"Library://UnitTests/Data/Empty.FeatureSource");
#ifdef _WIN32
            STRING resourceContentFileName1 = L"..\\UnitTestFiles\\Sheboygan_Parcels.FeatureSource";
            STRING resourceContentFileName2 = L"..\\UnitTestFiles\\Redding_Parcels.FeatureSource";
            STRING resourceContentFileName3 = L"..\\UnitTestFiles\\Sheboygan_BuildingOutlines.FeatureSource";
            STRING resourceContentFileName4 = L"..\\UnitTestFiles\\Sheboygan_VotingDistricts.FeatureSource";
            STRING resourceContentFileName5=  L"..\\UnitTestFiles\\TESTChainedInner1ToManyJoin.FeatureSource";
            STRING resourceContentFileName6=  L"..\\UnitTestFiles\\Empty.FeatureSource";
            STRING dataFileName1 = L"..\\UnitTestFiles\\Sheboygan_Parcels.sdf";
            STRING dataFileName2 = L"..\\UnitTestFiles\\Redding_Parcels.shp";
            STRING dataFileName3 = L"..\\UnitTestFiles\\Redding_Parcels.dbf";
            STRING dataFileName4 = L"..\\UnitTestFiles\\Redding_Parcels.shx";
            STRING dataFileName5 = L"..\\UnitTestFiles\\Sheboygan_BuildingOutlines.sdf";
            STRING dataFileName6 = L"..\\UnitTestFiles\\Sheboygan_VotingDistricts.sdf";
            STRING dataFileName7 = L"..\\UnitTestFiles\\Empty.sdf";
#else
            STRING resourceContentFileName1 = L"../UnitTestFiles/Sheboygan_Parcels.FeatureSource";
            STRING resourceContentFileName2 = L"../UnitTestFiles/Redding_Parcels.FeatureSource";
            STRING resourceContentFileName3 = L"../UnitTestFiles/Sheboygan_BuildingOutlines.FeatureSource";
            STRING resourceContentFileName4 = L"../UnitTestFiles/Sheboygan_VotingDistricts.FeatureSource";
            STRING resourceContentFileName5 = L"../UnitTestFiles/TESTChainedInner1ToManyJoin.FeatureSource";
            STRING resourceContentFileName6 = L"../UnitTestFiles/Empty.FeatureSource";
            STRING dataFileName1 = L"../UnitTestFiles/Sheboygan_Parcels.sdf";
            STRING dataFileName2 = L"../UnitTestFiles/Redding_Parcels.shp";
            STRING dataFileName3 = L"../UnitTestFiles/Redding_Parcels.dbf";
            STRING dataFileName4 = L"../UnitTestFiles/Redding_Parcels.shx";
            STRING dataFileName5 = L"../UnitTestFiles/Sheboygan_BuildingOutlines.sdf";
            STRING dataFileName6 = L"../UnitTestFiles/Sheboygan_VotingDistricts.sdf";
            STRING dataFileName7 = L"../UnitTestFiles/Empty.sdf";
#endif

            //Add a new resource
            Ptr<MgByteSource> contentSource1 = new MgByteSource(resourceContentFileName1);
            Ptr<MgByteReader> contentReader1 = contentSource1->GetReader();
            pService->SetResource(&resourceIdentifier1, contentReader1, NULL);

            Ptr<MgByteSource> contentSource2 = new MgByteSource(resourceContentFileName2);
            Ptr<MgByteReader> contentReader2 = contentSource2->GetReader();
            pService->SetResource(&resourceIdentifier2, contentReader2, NULL);

            Ptr<MgByteSource> contentSource3 = new MgByteSource(resourceContentFileName3);
            Ptr<MgByteReader> contentReader3 = contentSource3->GetReader();
            pService->SetResource(&resourceIdentifier3, contentReader3, NULL);

            Ptr<MgByteSource> contentSource4 = new MgByteSource(resourceContentFileName4);
            Ptr<MgByteReader> contentReader4 = contentSource4->GetReader();
            pService->SetResource(&resourceIdentifier4, contentReader4, NULL);

            Ptr<MgByteSource> contentSource5 = new MgByteSource(resourceContentFileName5);
            Ptr<MgByteReader> contentReader5 = contentSource5->GetReader();
            pService->SetResource(&resourceIdentifier5, contentReader5, NULL);

            Ptr<MgByteSource> contentSource6 = new MgByteSource(resourceContentFileName6);
            Ptr<MgByteReader> contentReader6 = contentSource6->GetReader();
            pService->SetResource(&resourceIdentifier6, contentReader6, NULL);

            //Set the resource data
            Ptr<MgByteSource> dataSource1 = new MgByteSource(dataFileName1);
            Ptr<MgByteReader> dataReader1 = dataSource1->GetReader();
            pService->SetResourceData(&resourceIdentifier1, L"Sheboygan_Parcels.sdf", L"File", dataReader1);

            Ptr<MgByteSource> dataSource2 = new MgByteSource(dataFileName2);
            Ptr<MgByteReader> dataReader2 = dataSource2->GetReader();
            pService->SetResourceData(&resourceIdentifier2, L"Redding_Parcels.shp", L"File", dataReader2);

            Ptr<MgByteSource> dataSource3 = new MgByteSource(dataFileName3);
            Ptr<MgByteReader> dataReader3 = dataSource3->GetReader();
            pService->SetResourceData(&resourceIdentifier2, L"Redding_Parcels.dbf", L"File", dataReader3);

            Ptr<MgByteSource> dataSource4 = new MgByteSource(dataFileName4);
            Ptr<MgByteReader> dataReader4 = dataSource4->GetReader();
            pService->SetResourceData(&resourceIdentifier2, L"Redding_Parcels.shx", L"File", dataReader4);

            Ptr<MgByteSource> dataSource5 = new MgByteSource(dataFileName5);
            Ptr<MgByteReader> dataReader5 = dataSource5->GetReader();
            pService->SetResourceData(&resourceIdentifier3, L"Sheboygan_BuildingOutlines.sdf", L"File", dataReader5);

            Ptr<MgByteSource> dataSource6 = new MgByteSource(dataFileName6);
            Ptr<MgByteReader> dataReader6 = dataSource6->GetReader();
            pService->SetResourceData(&resourceIdentifier4, L"Sheboygan_VotingDistricts.sdf", L"File", dataReader6);

            Ptr<MgByteSource> dataSource7 = new MgByteSource(dataFileName1);
            Ptr<MgByteReader> dataReader7 = dataSource7->GetReader();
            pService->SetResourceData(&resourceIdentifier5, L"Sheboygan_Parcels.sdf", L"File", dataReader7);

            Ptr<MgByteSource> dataSource8 = new MgByteSource(dataFileName7);
            Ptr<MgByteReader> dataReader8 = dataSource8->GetReader();
            pService->SetResourceData(&resourceIdentifier6, L"Empty.sdf", L"File", dataReader8);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestFeatureService::TestEnd()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestEnd",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestEnd",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        // delete the feature sources definition
        Ptr<MgResourceIdentifier> fsres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        pService->DeleteResource(fsres1);

        Ptr<MgResourceIdentifier> fsres2 = new MgResourceIdentifier(L"Library://UnitTests/Data/Redding_Parcels.FeatureSource");
        pService->DeleteResource(fsres2);

        Ptr<MgResourceIdentifier> fsres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_BuildingOutlines.FeatureSource");
        pService->DeleteResource(fsres3);

        Ptr<MgResourceIdentifier> fsres4 = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_VotingDistricts.FeatureSource");
        pService->DeleteResource(fsres4);

        Ptr<MgResourceIdentifier> fsres5 = new MgResourceIdentifier(L"Library://UnitTests/Data/TestChainedInner1ToManyJoin.FeatureSource");
        pService->DeleteResource(fsres5);

        Ptr<MgResourceIdentifier> fsres6 = new MgResourceIdentifier(L"Library://UnitTests/Data/Empty.FeatureSource");
        pService->DeleteResource(fsres6);

        #ifdef _DEBUG
        ACE_DEBUG((LM_INFO, ACE_TEXT("TestFeatureService::TestEnd()\n")));
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

    ACE_DEBUG((LM_INFO, ACE_TEXT("\nFeature Service tests completed.\n\n")));
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the feature providers.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetFeatureProviders()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetFeatureProviders", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetFeatureProviders", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader = pService->GetFeatureProviders();
        STRING mimeType = byteReader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimeType.c_str(), MgMimeType::Xml.c_str()) == 0);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests connections to the SDF provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestConnectionSDFProvider()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_TestConnectionSDFProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_TestConnectionSDFProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(MgUser::Administrator, L"");
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        const STRING provider = L"OSGeo.SDF";
#ifdef _WIN32
        const STRING connectionString = L"File=..\\UnitTestFiles\\Sheboygan_Parcels.sdf";
#else
        const STRING connectionString = L"File=../UnitTestFiles/Sheboygan_Parcels.sdf";
#endif
        bool bSuccess = pService->TestConnection(provider, connectionString);
        CPPUNIT_ASSERT(bSuccess);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the FDO connection manager.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestFdoConnectionManager()
{
    try
    {
        MgFdoConnectionManager* pManager = MgFdoConnectionManager::GetInstance();
        if(pManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_TestFdoConnectionManager", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        const STRING provider = L"OSGeo.SDF";
#ifdef _WIN32
        const STRING connectionString = L"File=..\\UnitTestFiles\\Sheboygan_Parcels.sdf";
        const STRING connectionStringAlt = L"File=..\\UnitTestFiles\\Sheboygan_BuildingOutlines.sdf";
#else
        const STRING connectionString = L"File=../UnitTestFiles/Sheboygan_Parcels.sdf";
        const STRING connectionStringAlt = L"File=../UnitTestFiles/Sheboygan_BuildingOutlines.sdf";
#endif

        FdoIConnection* pFdoConnection1 = pManager->Open(provider, connectionString);
        CPPUNIT_ASSERT(pFdoConnection1);

        FdoIConnection* pFdoConnectionAlt = pManager->Open(provider, connectionStringAlt);
        CPPUNIT_ASSERT(pFdoConnectionAlt);

        FdoIConnection* pFdoConnection2 = pManager->Open(provider, connectionString);
        CPPUNIT_ASSERT(pFdoConnection2);

        FdoIConnection* pFdoConnection3 = pManager->Open(provider, connectionString);
        CPPUNIT_ASSERT(pFdoConnection3);

        // These connections should be different
        CPPUNIT_ASSERT(pFdoConnection1 != pFdoConnection2);
        CPPUNIT_ASSERT(pFdoConnection1 != pFdoConnection3);

        pManager->Close(pFdoConnection1);
        pManager->Close(pFdoConnection2);
        pManager->Close(pFdoConnection3);
        pManager->Close(pFdoConnectionAlt);

        // Force removal from the FDO connection cache
        STRING key = connectionString;
        pManager->RemoveCachedFdoConnection(key);

        key = connectionStringAlt;
        pManager->RemoveCachedFdoConnection(key);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the connection property values.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetConnectionPropertyValues()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetConnectionPropertyValues", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetConnectionPropertyValues", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING provider = L"";
        STRING property = L"";
        STRING connectionString = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->GetConnectionPropertyValues(provider, property, connectionString), MgInvalidArgumentException*);

        provider = L"OSGeo.SDF";
        property = L"ReadOnly";
        Ptr<MgStringCollection> properties = pService->GetConnectionPropertyValues(provider, property, connectionString);
        CPPUNIT_ASSERT(properties->GetCount() > 0);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the provider capabilities.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetCapabilities()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING provider = L"OSGeo.SDF";

        MgFdoConnectionManager* fdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(fdoConnectionManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        STRING providerNoVersion = fdoConnectionManager->UpdateProviderName(provider);

        Ptr<MgByteReader> reader = pService->GetCapabilities(providerNoVersion);
        STRING mimetype = reader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimetype.c_str(), MgMimeType::Xml.c_str()) == 0);

        provider = L"";

        CPPUNIT_ASSERT_THROW_MG(pService->GetCapabilities(provider), MgInvalidArgumentException*);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests a connection to a resource.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestConnectionResourceIdentifier()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_TestConnectionResourceIdentifier", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_TestConnectionResourceIdentifier", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        CPPUNIT_ASSERT_THROW_MG(pService->TestConnection(resource), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->TestConnection(resource), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        bool bResult = pService->TestConnection(resource);
        CPPUNIT_ASSERT(bResult);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting schemas.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetSchemas()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetSchemas", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetSchemas", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(MgUser::Administrator, L"");
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        CPPUNIT_ASSERT_THROW_MG(pService->GetSchemas(resource), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->GetSchemas(resource), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        Ptr<MgStringCollection> schemas = pService->GetSchemas(resource);
        CPPUNIT_ASSERT(schemas->GetCount() > 0);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting classes.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetClasses()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetClasses", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetClasses", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(MgUser::Administrator, L"");
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING schemaName = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->GetClasses(resource, schemaName), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->GetClasses(resource, schemaName), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        schemaName = L"SHP_Schema";
        Ptr<MgStringCollection> classes = pService->GetClasses(resource, schemaName);
        CPPUNIT_ASSERT(classes->GetCount() > 0);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting class definitions.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetClassDefinition()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING schemaName = L"";
        STRING className = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->GetClassDefinition(resource, schemaName, className), MgClassNotFoundException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        schemaName = L"SHP_Schema";
        className = L"Parcels";
        Ptr<MgClassDefinition> classDef = pService->GetClassDefinition(resource, schemaName, className);
        STRING name = classDef->GetName();
        CPPUNIT_ASSERT(name == L"Parcels");
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises describing schemas.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_DescribeSchema()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(MgUser::Administrator, L"");
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING schemaName = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->DescribeSchema(resource, schemaName, NULL), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");
        schemaName = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->DescribeSchema(resource, schemaName, NULL), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        schemaName = L"";
        Ptr<MgFeatureSchemaCollection> schemaCollection = pService->DescribeSchema(resource, schemaName, NULL);
        CPPUNIT_ASSERT(schemaCollection->GetCount() > 0);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Description:
///
/// Find the class definition by name from a class definition collection.
///----------------------------------------------------------------------------
MgClassDefinition* FindClassByName(MgClassDefinitionCollection* classDefCol, STRING name)
{
    for (int i =0; i < classDefCol->GetCount(); i++)
    {
        Ptr<MgClassDefinition> classDef = classDefCol->GetItem(i);
        STRING temp = classDef->GetName();
        if (temp == name)
            return classDef.Detach();
    }
    return NULL;
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises applying schemas.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ApplySchema()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ApplySchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ApplySchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(MgUser::Administrator, L"");
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);
        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Empty.FeatureSource");

        //////////////////////////////////////////////////////////////////////
        /// The first test case:                                           ///
        /// Delete one property in an existing feature class               ///
        //////////////////////////////////////////////////////////////////////
        Ptr<MgFeatureSchemaCollection> oldSchemas = pService->DescribeSchema(resource, L"Schema", NULL);
        Ptr<MgFeatureSchema> oldSchema = oldSchemas->GetItem(0);
        Ptr<MgClassDefinitionCollection> oldSchemaClasses = oldSchema->GetClasses();
        Ptr<MgClassDefinition> oldClassDef = FindClassByName(oldSchemaClasses, L"Parcel");
        Ptr<MgPropertyDefinitionCollection> oldProperties = oldClassDef->GetProperties();
        Ptr<MgPropertyDefinition> oldPropDef = oldProperties->FindItem(L"Name");
        oldPropDef->Delete();
        pService->ApplySchema(resource, oldSchema);

        // Verify results
        Ptr<MgFeatureSchemaCollection> newSchemas = pService->DescribeSchema(resource, L"Schema", NULL);
        Ptr<MgFeatureSchema> newSchema = newSchemas->GetItem(0);
        Ptr<MgClassDefinitionCollection> newSchemaClasses = newSchema->GetClasses();
        Ptr<MgClassDefinition> newClassDef = FindClassByName(newSchemaClasses, L"Parcel");
        Ptr<MgPropertyDefinitionCollection> newProperties = newClassDef->GetProperties();
        CPPUNIT_ASSERT(newProperties->Contains(L"Name") == false);

        //////////////////////////////////////////////////////////////////////
        /// The second test case:                                          ///
        /// Add two feature classes                                        ///
        //////////////////////////////////////////////////////////////////////

        // Create the first class definition
        Ptr<MgClassDefinition> classDef1 = new MgClassDefinition();
        classDef1->SetName(L"FeatureClass1");
        classDef1->SetDescription(L"Feature class 1");
        classDef1->SetDefaultGeometryPropertyName(L"GEOM");

        Ptr<MgPropertyDefinitionCollection> identityProperties1 = classDef1->GetIdentityProperties();
        Ptr<MgPropertyDefinitionCollection> properties1 = classDef1->GetProperties();

        // Set key property
        Ptr<MgDataPropertyDefinition> prop = new MgDataPropertyDefinition(L"KEY1");
        prop->SetDataType(MgPropertyType::Int32);
        prop->SetAutoGeneration(true);
        prop->SetReadOnly(true);
        identityProperties1->Add(prop);
        properties1->Add(prop);

        // Set name property
        prop = new MgDataPropertyDefinition(L"NAME1");
        prop->SetDataType(MgPropertyType::String);
        properties1->Add(prop);

        // Set geometry property
        Ptr<MgGeometricPropertyDefinition> propGeom = new MgGeometricPropertyDefinition(L"GEOM");
        propGeom->SetGeometryTypes(MgFeatureGeometricType::Surface);
        properties1->Add(propGeom);

        // Create the second class definition
        Ptr<MgClassDefinition> classDef2 = new MgClassDefinition();
        classDef2->SetName(L"FeatureClass2");
        classDef2->SetDescription(L"Feature class 2");
        classDef2->SetDefaultGeometryPropertyName(L"GEOM");

        Ptr<MgPropertyDefinitionCollection> identityProperties2 = classDef2->GetIdentityProperties();
        Ptr<MgPropertyDefinitionCollection> properties2 = classDef2->GetProperties();

        // Set key property
        prop = new MgDataPropertyDefinition(L"KEY2");
        prop->SetDataType(MgPropertyType::Int32);
        prop->SetAutoGeneration(true);
        prop->SetReadOnly(true);
        identityProperties2->Add(prop);
        properties2->Add(prop);

        // Set name property
        prop = new MgDataPropertyDefinition(L"NAME2");
        prop->SetDataType(MgPropertyType::String);
        properties2->Add(prop);

        // Set geometry property
        propGeom = new MgGeometricPropertyDefinition(L"GEOM");
        propGeom->SetGeometryTypes(MgFeatureGeometricType::Curve);
        properties2->Add(propGeom);

        Ptr<MgFeatureSchema> schema = new MgFeatureSchema(L"Schema", L"Schema");
        Ptr<MgClassDefinitionCollection> schemaClasses = schema->GetClasses();
        schemaClasses->Add(classDef1);
        schemaClasses->Add(classDef2);

        pService->ApplySchema(resource, schema);
        newSchemas = pService->DescribeSchema(resource, L"Schema", NULL);

        // Verify schema
        STRING temp;
        newSchema = newSchemas->GetItem(0);
        temp = newSchema->GetName();
        CPPUNIT_ASSERT(temp == L"Schema");

        schemaClasses = newSchema->GetClasses();

        // Verify the first feature class
        classDef1 = FindClassByName(schemaClasses, L"FeatureClass1");
        temp = classDef1->GetName();
        CPPUNIT_ASSERT(temp == L"FeatureClass1");
        temp = classDef1->GetDescription();
        CPPUNIT_ASSERT(temp == L"Feature class 1");
        temp = classDef1->GetDefaultGeometryPropertyName();
        CPPUNIT_ASSERT(temp == L"GEOM");

        identityProperties1 = classDef1->GetIdentityProperties();
        CPPUNIT_ASSERT(identityProperties1->GetCount() == 1);
        prop = static_cast<MgDataPropertyDefinition*>(identityProperties1->GetItem(L"KEY1"));
        CPPUNIT_ASSERT(prop != NULL);

        properties1 = classDef1->GetProperties();
        CPPUNIT_ASSERT(properties1->GetCount() == 3);

        prop = static_cast<MgDataPropertyDefinition*>(properties1->GetItem(L"KEY1"));
        CPPUNIT_ASSERT(prop != NULL);
        CPPUNIT_ASSERT(prop->GetDataType() == MgPropertyType::Int32);
        CPPUNIT_ASSERT(prop->IsAutoGenerated() == true);
        CPPUNIT_ASSERT(prop->GetReadOnly() == true);

        prop = static_cast<MgDataPropertyDefinition*>(properties1->GetItem(L"NAME1"));
        CPPUNIT_ASSERT(prop != NULL);
        CPPUNIT_ASSERT(prop->GetDataType() == MgPropertyType::String);

        propGeom = static_cast<MgGeometricPropertyDefinition*>(properties1->GetItem(L"GEOM"));
        CPPUNIT_ASSERT(prop != NULL);
        CPPUNIT_ASSERT(propGeom->GetGeometryTypes() == MgFeatureGeometricType::Surface);

        // Verify the second feature class
        classDef2 = FindClassByName(schemaClasses, L"FeatureClass2");
        temp = classDef2->GetDescription();
        CPPUNIT_ASSERT(temp == L"Feature class 2");
        temp = classDef2->GetDefaultGeometryPropertyName();
        CPPUNIT_ASSERT(temp == L"GEOM");

        identityProperties2 = classDef2->GetIdentityProperties();
        CPPUNIT_ASSERT(identityProperties2->GetCount() == 1);
        prop = static_cast<MgDataPropertyDefinition*>(identityProperties2->GetItem(L"KEY2"));
        CPPUNIT_ASSERT(prop != NULL);

        properties2 = classDef2->GetProperties();
        CPPUNIT_ASSERT(properties2->GetCount() == 3);

        prop = static_cast<MgDataPropertyDefinition*>(properties2->GetItem(L"KEY2"));
        CPPUNIT_ASSERT(prop != NULL);
        CPPUNIT_ASSERT(prop->GetDataType() == MgPropertyType::Int32);
        CPPUNIT_ASSERT(prop->IsAutoGenerated() == true);
        CPPUNIT_ASSERT(prop->GetReadOnly() == true);

        prop = static_cast<MgDataPropertyDefinition*>(properties2->GetItem(L"NAME2"));
        CPPUNIT_ASSERT(prop != NULL);
        CPPUNIT_ASSERT(prop->GetDataType() == MgPropertyType::String);

        propGeom = static_cast<MgGeometricPropertyDefinition*>(properties2->GetItem(L"GEOM"));
        CPPUNIT_ASSERT(prop != NULL);
        CPPUNIT_ASSERT(propGeom->GetGeometryTypes() == MgFeatureGeometricType::Curve);

        //////////////////////////////////////////////////////////////////////
        /// The third test case:                                           ///
        /// Delete one feature class and modify one feature class          ///
        //////////////////////////////////////////////////////////////////////
        // Delete the first feature class
        classDef1 = FindClassByName(schemaClasses, L"FeatureClass1");
        classDef1->Delete();

        // Modify the second feature class
        classDef2 = FindClassByName(schemaClasses, L"FeatureClass2");
        classDef2->SetDescription(L"Modified Feature Class");
        properties2 = classDef2->GetProperties();

        prop = new MgDataPropertyDefinition(L"LENGTH");
        prop->SetDataType(MgPropertyType::Single);
        properties2->Add(prop);

        prop = new MgDataPropertyDefinition(L"ID");
        prop->SetDataType(MgPropertyType::Int32);
        prop->SetAutoGeneration(true);
        prop->SetReadOnly(true);
        properties2->Add(prop);

        pService->ApplySchema(resource, newSchema);

        newSchemas = pService->DescribeSchema(resource, L"Schema", NULL);
        newSchema = newSchemas->GetItem(0);
        temp = newSchema->GetName();
        CPPUNIT_ASSERT(temp == L"Schema");

        schemaClasses = newSchema->GetClasses();

        // Verify the first feature class
        classDef1 = FindClassByName(schemaClasses, L"FeatureClass1");
        CPPUNIT_ASSERT(classDef1 == NULL);

        classDef2 = FindClassByName(schemaClasses, L"FeatureClass2");
        temp = classDef2->GetDescription();
        CPPUNIT_ASSERT(temp == L"Modified Feature Class");

        properties2 = classDef2->GetProperties();
        CPPUNIT_ASSERT(properties2->GetCount() == 5);

        temp = classDef2->GetDefaultGeometryPropertyName();
        CPPUNIT_ASSERT(temp == L"GEOM");

        identityProperties2 = classDef2->GetIdentityProperties();
        CPPUNIT_ASSERT(identityProperties2->GetCount() == 1);
        prop = static_cast<MgDataPropertyDefinition*>(identityProperties2->GetItem(L"KEY2"));
        CPPUNIT_ASSERT(prop != NULL);

        prop = static_cast<MgDataPropertyDefinition*>(properties2->GetItem(L"ID"));
        CPPUNIT_ASSERT(prop != NULL);
        CPPUNIT_ASSERT(prop->GetDataType() == MgPropertyType::Int32);
        CPPUNIT_ASSERT(prop->IsAutoGenerated() == true);
        CPPUNIT_ASSERT(prop->GetReadOnly() == true);

        prop = static_cast<MgDataPropertyDefinition*>(properties2->GetItem(L"LENGTH"));
        CPPUNIT_ASSERT(prop != NULL);
        CPPUNIT_ASSERT(prop->GetDataType() == MgPropertyType::Single);

        propGeom = static_cast<MgGeometricPropertyDefinition*>(properties2->GetItem(L"GEOM"));
        CPPUNIT_ASSERT(prop != NULL);
        CPPUNIT_ASSERT(propGeom->GetGeometryTypes() == MgFeatureGeometricType::Curve);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises selecting features.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SelectFeatures()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING className = L"";
        Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();
        CPPUNIT_ASSERT_THROW_MG(pService->SelectFeatures(resource, className, options), MgInvalidArgumentException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        className = L"Parcels";
        Ptr<MgFeatureReader> reader = pService->SelectFeatures(resource, className, options);
        bool bResult = reader->ReadNext();
        reader->Close();
        CPPUNIT_ASSERT(bResult);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises selecting aggregate features.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SelectAggregate()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_SelectAggregate", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SelectAggregate", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING className = L"";
        Ptr<MgFeatureAggregateOptions> options = new MgFeatureAggregateOptions();
        CPPUNIT_ASSERT_THROW_MG(pService->SelectAggregate(resource, className, options), MgInvalidArgumentException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        className = L"Parcels";
        options->SetFilter(L"Autogenerated_SDF_ID = 1");
        Ptr<MgDataReader> reader = pService->SelectAggregate(resource, className, options);
        bool bResult = reader->ReadNext();
        reader->Close();
        CPPUNIT_ASSERT(bResult);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises executing SQL queries containing SELECT
/// statements.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ExecuteSqlQuery()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ExecuteSqlQuery", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ExecuteSqlQuery", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        const STRING sqlQuery = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->ExecuteSqlQuery(resource, sqlQuery), MgInvalidArgumentException*);

        //TODO test with correct input
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises executing SQL queries not containing
/// SELECT statements.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ExecuteSqlNonQuery()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ExecuteSqlNonQuery", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ExecuteSqlNonQuery", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        const STRING sqlNonQuery = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->ExecuteSqlNonQuery(resource, sqlNonQuery), MgInvalidArgumentException*);

        //TODO test with correct input
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting spatial contexts.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetSpatialContexts()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetSpatialContexts", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetSpatialContexts", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(MgUser::Administrator, L"");
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        bool activeOnly = false;
        CPPUNIT_ASSERT_THROW_MG(pService->GetSpatialContexts(resource, activeOnly), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->GetSpatialContexts(resource, activeOnly), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");

        // Get a valid spatial context reader
        Ptr<MgSpatialContextReader> reader = pService->GetSpatialContexts(resource, activeOnly);

        // Advance to the 1st spatial context
        bool bResult = reader->ReadNext();
        CPPUNIT_ASSERT(bResult);

        // Get the CS name and CS WKT
        STRING csName = reader->GetName();
        STRING csWkt = reader->GetCoordinateSystemWkt();
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("TestFeatureService::TestCase_GetSpatialContexts()\nName: %W\nWKT: %W\n"), csName.c_str(), csWkt.c_str()));

        CPPUNIT_ASSERT(wcscmp(csName.c_str(), L"WGS84 Lat/Long's, Degre") == 0);
        CPPUNIT_ASSERT(wcscmp(csWkt.c_str(), L"GEOGCS[\"WGS84 Lat/Long's, Degrees, -180 ==> +180\",DATUM[\"D_WGS_1984\",SPHEROID[\"World_Geodetic_System_of_1984\",6378137,298.257222932867]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]") == 0);

        // Close the reader
        reader->Close();

        // Force the reader to be cleaned up for the next operation
        reader = NULL;

        // Get a valid spatial context reader, this reader will be coming from the cache
        reader = pService->GetSpatialContexts(resource, activeOnly);

        // Advance to the 1st spatial context
        bResult = reader->ReadNext();
        CPPUNIT_ASSERT(bResult);

        // Get the CS name and CS WKT
        csName = reader->GetName();
        csWkt = reader->GetCoordinateSystemWkt();
        CPPUNIT_ASSERT(wcscmp(csName.c_str(), L"WGS84 Lat/Long's, Degre") == 0);
        CPPUNIT_ASSERT(wcscmp(csWkt.c_str(), L"GEOGCS[\"WGS84 Lat/Long's, Degrees, -180 ==> +180\",DATUM[\"D_WGS_1984\",SPHEROID[\"World_Geodetic_System_of_1984\",6378137,298.257222932867]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]") == 0);

        // Close the reader
        reader->Close();
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting long transactions.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetLongTransactions()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Set the user information for the current thread to be administrator.
        Ptr<MgUserInformation> adminUserInfo = new MgUserInformation(MgUser::Administrator, L"");
        MgUserInformation::SetCurrentUserInfo(adminUserInfo);

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        bool activeOnly = false;
        CPPUNIT_ASSERT_THROW_MG(pService->GetLongTransactions(resource, activeOnly), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->GetLongTransactions(resource, activeOnly), MgInvalidResourceTypeException*);

        //TODO test with correct input
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises setting long transaction names.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SetLongTransaction()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_SetLongTransaction", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SetLongTransaction", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING longTransactionName = L"Live";

        // verify exception when passing in a NULL resource
        CPPUNIT_ASSERT_THROW_MG(pService->SetLongTransaction(NULL, longTransactionName), MgNullArgumentException*);

        // verify exception when passing in an invalid resource type
        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        CPPUNIT_ASSERT_THROW_MG(pService->SetLongTransaction(resource, longTransactionName), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->SetLongTransaction(resource, longTransactionName), MgInvalidResourceTypeException*);

        // verify exception when session is not set
        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        CPPUNIT_ASSERT_THROW_MG(pService->SetLongTransaction(resource, longTransactionName), MgSessionNotFoundException*);

        // set the user information for the current thread to be administrator - this
        // alone doesn't create a session
        Ptr<MgUserInformation> userInfo = new MgUserInformation(MgUser::Administrator, L"");
        MgUserInformation::SetCurrentUserInfo(userInfo);
        CPPUNIT_ASSERT_THROW_MG(pService->SetLongTransaction(resource, longTransactionName), MgSessionNotFoundException*);

        // now set a session
        userInfo->SetMgSessionId(userInfo->CreateMgSessionId());
        MgUserInformation::SetCurrentUserInfo(userInfo);
        CPPUNIT_ASSERT(pService->SetLongTransaction(resource, longTransactionName));

        // setting the same LT name twice shouldn't matter
        CPPUNIT_ASSERT(pService->SetLongTransaction(resource, longTransactionName));

        // should be able to clear the LT name
        CPPUNIT_ASSERT(pService->SetLongTransaction(resource, L""));
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting features.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetFeatures()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING readerId = L"0";
        CPPUNIT_ASSERT_THROW_MG(pService->GetFeatures(readerId), MgInvalidArgumentException*);

        //TODO test with correct input
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises closing a feature reader.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_CloseFeatureReader()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_CloseFeatureReader", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_CloseFeatureReader", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING readerId = L"0";
        bool closed = pService->CloseFeatureReader(readerId);
        CPPUNIT_ASSERT(!closed);

        // TODO test with correct input
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting SQL rows.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetSqlRows()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetSqlRows", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetSqlRows", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING sqlReader = L"0";
        CPPUNIT_ASSERT_THROW_MG(pService->GetSqlRows(sqlReader), MgInvalidArgumentException*);

        //TODO test with correct input
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises closing a SQL reader.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_CloseSqlReader()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_CloseSqlReader", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_CloseSqlReader", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING sqlReader = L"0";
        bool closed = pService->CloseSqlReader(sqlReader);
        CPPUNIT_ASSERT(!closed);

        // TODO test with correct input
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting a raster.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetRaster()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetRaster", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetRaster", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING featureReader = L"0";
        INT32 xSize = 0;
        INT32 ySize = 0;
        CPPUNIT_ASSERT_THROW_MG(pService->GetRaster(featureReader, xSize, ySize, L""), MgInvalidArgumentException*);

        //TODO test with correct input
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting data rows.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetDataRows()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetDataRows", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetDataRows", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING dataReader = L"0";
        CPPUNIT_ASSERT_THROW_MG(pService->GetDataRows(dataReader), MgInvalidArgumentException*);

        //TODO test with correct input
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises closing a data reader.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_CloseDataReader()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_CloseDataReader", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_CloseDataReader", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING dataReader = L"0";
        bool closed = pService->CloseDataReader(dataReader);
        CPPUNIT_ASSERT(!closed);

        // TODO test with correct input
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises joining features.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_JoinFeatures()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_JoinFeatures",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_JoinFeatures",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> lFeatureSource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");

        Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"Ext1", NULL);

        bool bReadNextResult = reader->ReadNext();
             bReadNextResult = reader->ReadNext();
             bReadNextResult = reader->ReadNext();
        bool bIsNullResult = reader->IsNull(L"Join1NAME");
        STRING s1 = reader->GetString(L"NAME");
        STRING s2 = reader->GetString(L"Join1NAME");
        STRING s3 = reader->GetString(L"Join1ID");

        // Close the reader
        reader->Close();

        CPPUNIT_ASSERT(bReadNextResult);
        CPPUNIT_ASSERT(bIsNullResult == false);
        CPPUNIT_ASSERT(s1 == L"NIEMUTH, ROGER L.");
        CPPUNIT_ASSERT(s2 == L"Wells Fargo");
        CPPUNIT_ASSERT(s3 == L"10573");
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises joining features defined as chained, inner, 1-to-many.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_JoinFeaturesChainedInner1ToMany()
{
    try
    {
        #ifdef _DEBUG
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_JoinFeaturesChainedInner1ToMany() - Start\n")));
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif

        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_JoinFeaturesChainedInner1ToMany",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_JoinFeaturesChainedInner1ToMany",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> lFeatureSource = new MgResourceIdentifier(L"Library://UnitTests/Data/TestChainedInner1ToManyJoin.FeatureSource");

        Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"Ext1", NULL);

        bool bReadNextResult = reader->ReadNext();
        bool bIsNullResult = reader->IsNull(L"Join1NAME");
        bool bIsNullResult2 = reader->IsNull(L"Join2|NAME");
        STRING s1 = reader->GetString(L"NAME");
        STRING s2 = reader->GetString(L"Join1NAME");
        STRING s3 = reader->GetString(L"Join1ID");
        STRING s4 = reader->GetString(L"Join2|NAME");
        STRING s5 = reader->GetString(L"Join2|ID");

        // Close the reader
        reader->Close();

        #ifdef _DEBUG
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("TestFeatureService::TestCase_JoinFeaturesChainedInner1ToMany() - End\n")));
        #endif

        CPPUNIT_ASSERT(bReadNextResult);
        CPPUNIT_ASSERT(bIsNullResult == false);
        CPPUNIT_ASSERT(bIsNullResult2 == false);
        CPPUNIT_ASSERT(s1 == L"CITY OF SHEBOYGAN");
        CPPUNIT_ASSERT(s2 == L"Johnson Bank");
        CPPUNIT_ASSERT(s3 == L"30320");
        CPPUNIT_ASSERT(s4 == L"Voting District Seven");
        CPPUNIT_ASSERT(s5 == L"7");
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}



///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises creating a feature source.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_CreateFeatureSource()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_CreateFeatureSource",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_CreateFeatureSource",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pResourceService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_CreateFeatureSource",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Create class definition
        Ptr<MgClassDefinition> classDef = new MgClassDefinition();
        classDef->SetName(L"Buffer");
        classDef->SetDescription(L"Feature class for buffer layer");
        classDef->SetDefaultGeometryPropertyName(L"GEOM");

        Ptr<MgPropertyDefinitionCollection> identityProperties = classDef->GetIdentityProperties();
        Ptr<MgPropertyDefinitionCollection> properties = classDef->GetProperties();

        // Set key property
        Ptr<MgDataPropertyDefinition> prop = new MgDataPropertyDefinition(L"KEY");
        prop->SetDataType(MgPropertyType::Int32);
        prop->SetAutoGeneration(true);
        prop->SetReadOnly(true);
        identityProperties->Add(prop);
        properties->Add(prop);

        // Set name property
        prop = new MgDataPropertyDefinition(L"NAME");
        prop->SetDataType(MgPropertyType::String);
        properties->Add(prop);

        // Set geometry property
        Ptr<MgGeometricPropertyDefinition> propGeom = new MgGeometricPropertyDefinition(L"GEOM");
        propGeom->SetGeometryTypes(MgFeatureGeometricType::Surface);
        properties->Add(propGeom);

        Ptr<MgFeatureSchema> schema = new MgFeatureSchema(L"BufferSchema", L"Temporary buffer schema");
        Ptr<MgClassDefinitionCollection> schemaClasses = schema->GetClasses();
        schemaClasses->Add(classDef);

        STRING wkt = L"LOCAL_CS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";

        // Create SDF feature source using class MgCreateSdfParams
        Ptr<MgCreateSdfParams> params1 = new MgCreateSdfParams(L"ArbitraryXY", wkt, schema);
        Ptr<MgResourceIdentifier> resource1 = new MgResourceIdentifier(L"Library://UnitTests/Data/CreateFeatureSourceTest1.FeatureSource");
        pService->CreateFeatureSource(resource1, params1);
        // Delete the resource
        pResourceService->DeleteResource(resource1);

        // Create SDF feature source using class MgFeatureSourceParams
        Ptr<MgFileFeatureSourceParams> params2 = new MgFileFeatureSourceParams(L"OSGeo.SDF", L"ArbitraryXY", wkt, schema);
        Ptr<MgResourceIdentifier> resource2 = new MgResourceIdentifier(L"Library://UnitTests/Data/CreateFeatureSourceTest2.FeatureSource");
        pService->CreateFeatureSource(resource2, params2);
        // Delete SDF feature source
        pResourceService->DeleteResource(resource2);

        // Create SHP feature source
        Ptr<MgFileFeatureSourceParams> params3 = new MgFileFeatureSourceParams(L"OSGeo.SHP", L"ArbitraryXY", wkt, schema);
        Ptr<MgResourceIdentifier> resource3 = new MgResourceIdentifier(L"Library://UnitTests/Data/CreateFeatureSourceTest3.FeatureSource");
        pService->CreateFeatureSource(resource3, params3);
        // We use schema name "Default" because SHP FDO provider always returns a schema named "Default"
        Ptr<MgFeatureSchemaCollection> tempSchema3 = pService->DescribeSchema(resource3, L"Default");
        // Delete SHP feature source
        pResourceService->DeleteResource(resource3);

        // Disable the following test case for Sqlite because it seems that
        // currently unit test environment doesn't include Sqlite FDO provider.
        // Create SQLite feature source
        // Ptr<MgFileFeatureSourceParams> params4 = new MgFileFeatureSourceParams(L"OSGeo.SQLite", L"ArbitraryXY", wkt, schema);
        // params4->SetFileName(L"sqlite.sqlite");
        // Ptr<MgResourceIdentifier> resource4 = new MgResourceIdentifier(L"Library://UnitTests/Data/CreateFeatureSourceTest4.FeatureSource");
        // pService->CreateFeatureSource(resource4, params4);
        // We use schema name "Default" because Sqlite FDO provider always returns a schema named "Default"
        // Ptr<MgFeatureSchemaCollection> tempSchema4 = pService->DescribeSchema(resource4, L"Default");
        // Delete SQLite feature source
        // pResourceService->DeleteResource(resource4);
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case benchmarks selecting features.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_BenchmarkSelectFeatures()
{
    try
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSelectFeatures() - Start\n")));

        #ifdef _DEBUG
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif

        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_BenchmarkSelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_BenchmarkSelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING className = L"Parcels";
        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();

#ifdef _DEBUG
        const int iterations = 1;
#else
        const int iterations = 1000;
#endif
        long lStart = GetTickCount();
        for(int i=0;i<iterations;i++)
        {
            int nFeatures = 0;
            Ptr<MgFeatureReader> reader = pService->SelectFeatures(resource, className, options);
            while(reader->ReadNext())
            {
                nFeatures++;
            }

            // Close the reader
            reader->Close();
            CPPUNIT_ASSERT(nFeatures == 17565);
        }

        #ifdef _DEBUG
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (Average of %d runs): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)/(double)iterations ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("TestCase_BenchmarkSelectFeatures - END\n")));
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests concurrent access to a provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ConcurrentAccess()
{
    vector<MgFeatureReader*> featureReaders;

    try
    {
        #ifdef _DEBUG
        ACE_DEBUG((LM_INFO, ACE_TEXT("TestFeatureService::TestCase_ConcurrentAccess() - Start\n")));
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif

        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_ConcurrentAccess", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ConcurrentAccess", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        STRING className = L"Parcels";
        Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();

        // Use up all of the connections

        MgConfiguration* configuration = MgConfiguration::GetInstance();
        assert(NULL != configuration);
        INT32 nDataConnectionPoolSize = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize;

        configuration->GetIntValue(
            MgConfigProperties::FeatureServicePropertiesSection,
            MgConfigProperties::FeatureServicePropertyDataConnectionPoolSize,
            nDataConnectionPoolSize,
            MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize);

        // Create readers

        for (INT32 i = 0; i < nDataConnectionPoolSize; ++i)
        {
            featureReaders.push_back(pService->SelectFeatures(resource, className, options));
        }

        // Create another reader - this one should timeout and throw exception
        Ptr<MgFeatureReader> readerFail;
        CPPUNIT_ASSERT_THROW_MG(readerFail = pService->SelectFeatures(resource, className, options), MgAllProviderConnectionsUsedException*);

        if (!featureReaders.empty())
        {
            MgFeatureReader* reader = featureReaders[featureReaders.size() - 1];

            if (NULL != reader)
            {
                // Close one of the open readers
                reader->Close();
                SAFE_RELEASE(reader);
                featureReaders.pop_back();

                // Create another reader - this one should succeed
                Ptr<MgFeatureReader> readerSuccess = pService->SelectFeatures(resource, className, options);
                bool bResult = readerSuccess->ReadNext();
                CPPUNIT_ASSERT(bResult);
                featureReaders.push_back(readerSuccess.Detach());
            }
        }

        #ifdef _DEBUG
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("TestFeatureService::TestCase_ConcurrentAccess() - End\n")));
        #endif
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
        CPPUNIT_FAIL("FdoException occurred");
    }
    catch(...)
    {
        for (size_t i = 0; i < featureReaders.size(); ++i)
        {
            featureReaders[i]->Close();
            SAFE_RELEASE(featureReaders[i]);
        }

        featureReaders.clear();

        throw;
    }

    for (size_t i = 0; i < featureReaders.size(); ++i)
    {
        featureReaders[i]->Close();
        SAFE_RELEASE(featureReaders[i]);
    }

    featureReaders.clear();
}
