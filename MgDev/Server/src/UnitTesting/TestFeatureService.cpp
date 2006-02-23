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

#include "AceCommon.h"
#include "TestFeatureService.h"
#include "CppUnitExtensions.h"
#include "ServiceManager.h"
#include "FdoConnectionManager.h"
#include "FeatureService.h"
#include "ServerFeatureService.h"
#include "Fdo.h"
#include "FeatureGeometricType.h"

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

        Ptr<MgUserInformation> userInfo;

        Ptr<MgByteSource> contentSource;
        contentSource = NULL;

        Ptr<MgByteSource> dataSource;
        dataSource = NULL;

        Ptr<MgByteSource> contentSource2;
        Ptr<MgByteSource> contentSource3;
        Ptr<MgByteSource> dataSource2;
        Ptr<MgByteSource> dataSource3;
        Ptr<MgByteSource> dataSource4;
        Ptr<MgByteSource> dataSource5;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(L"Administrator", L"admin");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(TEST_LOCALE);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            MgResourceIdentifier resourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
            MgResourceIdentifier resourceIdentifier2(L"Library://UnitTests/Data/Redding_Parcels.FeatureSource");
            MgResourceIdentifier resourceIdentifier3(L"Library://UnitTests/Data/Sheboygan_BuildingOutlines.FeatureSource");

#ifdef WIN32
            STRING resourceContentFileName  = L"..\\UnitTestFiles\\Sheboygan_Parcels.FeatureSource";
            STRING resourceContentFileName2 = L"..\\UnitTestFiles\\Redding_Parcels.FeatureSource";
            STRING resourceContentFileName3 = L"..\\UnitTestFiles\\Sheboygan_BuildingOutlines.FeatureSource";
            STRING dataFileName  = L"..\\UnitTestFiles\\Sheboygan_Parcels.sdf";
            STRING dataFileName2 = L"..\\UnitTestFiles\\Redding_Parcels.shp";
            STRING dataFileName3 = L"..\\UnitTestFiles\\Redding_Parcels.dbf";
            STRING dataFileName4 = L"..\\UnitTestFiles\\Redding_Parcels.shx";
            STRING dataFileName5 = L"..\\UnitTestFiles\\Sheboygan_BuildingOutlines.sdf";
#else
            STRING resourceContentFileName  = L"../UnitTestFiles/Sheboygan_Parcels.FeatureSource";
            STRING resourceContentFileName2 = L"../UnitTestFiles/Redding_Parcels.FeatureSource";
            STRING resourceContentFileName3 = L"../UnitTestFiles/Sheboygan_BuildingOutlines.FeatureSource";
            STRING dataFileName  = L"../UnitTestFiles/Sheboygan_Parcels.sdf";
            STRING dataFileName2 = L"../UnitTestFiles/Redding_Parcels.shp";
            STRING dataFileName3 = L"../UnitTestFiles/Redding_Parcels.dbf";
            STRING dataFileName4 = L"../UnitTestFiles/Redding_Parcels.shx";
            STRING dataFileName5 = L"../UnitTestFiles/Sheboygan_BuildingOutlines.sdf";
#endif

            //Add a new resource
            contentSource = new MgByteSource(resourceContentFileName);
            Ptr<MgByteReader> contentReader = contentSource->GetReader();
            pService->SetResource(&resourceIdentifier, contentReader, NULL);

            contentSource2 = new MgByteSource(resourceContentFileName2);
            Ptr<MgByteReader> contentReader2 = contentSource2->GetReader();
            pService->SetResource(&resourceIdentifier2, contentReader2, NULL);

            contentSource3 = new MgByteSource(resourceContentFileName3);
            Ptr<MgByteReader> contentReader3 = contentSource3->GetReader();
            pService->SetResource(&resourceIdentifier3, contentReader3, NULL);

            //Set the resource data
            dataSource = new MgByteSource(dataFileName);
            Ptr<MgByteReader> dataReader;
            dataReader = dataSource->GetReader();
            pService->SetResourceData(&resourceIdentifier, L"Sheboygan_Parcels.sdf", L"File", dataReader);

            dataSource2 = new MgByteSource(dataFileName2);
            Ptr<MgByteReader> dataReader2;
            dataReader2 = dataSource2->GetReader();
            pService->SetResourceData(&resourceIdentifier2, L"Redding_Parcels.shp", L"File", dataReader2);

            dataSource3 = new MgByteSource(dataFileName3);
            Ptr<MgByteReader> dataReader3;
            dataReader3 = dataSource3->GetReader();
            pService->SetResourceData(&resourceIdentifier2, L"Redding_Parcels.dbf", L"File", dataReader3);

            dataSource4 = new MgByteSource(dataFileName4);
            Ptr<MgByteReader> dataReader4;
            dataReader4 = dataSource4->GetReader();
            pService->SetResourceData(&resourceIdentifier2, L"Redding_Parcels.shx", L"File", dataReader4);

            dataSource5 = new MgByteSource(dataFileName5);
            Ptr<MgByteReader> dataReader5;
            dataReader5 = dataSource5->GetReader();
            pService->SetResourceData(&resourceIdentifier3, L"Sheboygan_BuildingOutlines.sdf", L"File", dataReader5);

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
        Ptr<MgResourceIdentifier> mapres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        pService->DeleteResource(mapres1);

        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://UnitTests/Data/Redding_Parcels.FeatureSource");
        pService->DeleteResource(mapres2);

        Ptr<MgResourceIdentifier> mapres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_BuildingOutlines.FeatureSource");
        pService->DeleteResource(mapres3);
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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the feature providers.
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

        const STRING provider = L"OSGeo.SDF.3.0";
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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests the FDO coonection manager.
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

        const STRING provider = L"OSGeo.SDF.3.0";
#ifdef _WIN32
        const STRING connectionString = L"File=..\\UnitTestFiles\\Sheboygan_Parcels.sdf";
#else
        const STRING connectionString = L"File=../UnitTestFiles/Sheboygan_Parcels.sdf";
#endif

        FdoIConnection* pFdoConnection1 = pManager->Open(provider, connectionString);
        CPPUNIT_ASSERT(pFdoConnection1);

        FdoIConnection* pFdoConnection2 = pManager->Open(provider, connectionString);
        CPPUNIT_ASSERT(pFdoConnection2);

        // These connections should be different
        CPPUNIT_ASSERT(pFdoConnection1 != pFdoConnection2);

        pManager->Close(pFdoConnection1);
        pManager->Close(pFdoConnection2);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the connection property value
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetConnectionPropertyValue()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_GetConnectionPropertyValue", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetConnectionPropertyValue", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING provider = L"";
        STRING property = L"";
        STRING connectionString = L"";

        CPPUNIT_ASSERT_THROW_MG(pService->GetConnectionPropertyValues(provider, property, connectionString),
            MgInvalidArgumentException*);

        provider = L"OSGeo.SDF.3.0";
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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        STRING provider = L"OSGeo.SDF.3.0";

        Ptr<MgByteReader> reader = pService->GetCapabilities(provider);
        STRING mimetype = reader->GetMimeType();
        CPPUNIT_ASSERT(wcscmp(mimetype.c_str(), MgMimeType::Xml.c_str()) == 0);

        provider = L"";

        CPPUNIT_ASSERT_THROW_MG(pService->GetCapabilities(provider),
            MgInvalidArgumentException*);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        CPPUNIT_ASSERT_THROW_MG(pService->TestConnection(resource), MgResourceNotFoundException*);

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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        CPPUNIT_ASSERT_THROW_MG(pService->GetSchemas(resource), MgResourceNotFoundException*);

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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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
        CPPUNIT_ASSERT_THROW_MG(pService->GetClasses(resource, schemaName), MgResourceNotFoundException*);

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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        CPPUNIT_ASSERT_THROW_MG(pService->GetClassDefinition(resource, schemaName, className),
            MgClassNotFoundException*);

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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        CPPUNIT_ASSERT_THROW_MG(pService->DescribeSchema(resource, schemaName), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");
        schemaName = L"";

        CPPUNIT_ASSERT_THROW_MG(pService->DescribeSchema(resource, schemaName), MgResourceNotFoundException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        schemaName = L"";
        Ptr<MgFeatureSchemaCollection> schemaCollection = pService->DescribeSchema(resource, schemaName);
        CPPUNIT_ASSERT(schemaCollection->GetCount() > 0);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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
        CPPUNIT_ASSERT(bResult);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        CPPUNIT_ASSERT_THROW_MG(pService->SelectAggregate(resource, className, options),
            MgInvalidArgumentException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        className = L"Parcels";
        Ptr<MgDataReader> reader = pService->SelectAggregate(resource, className, options);
        bool bResult = reader->ReadNext();
        CPPUNIT_ASSERT(bResult);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        CPPUNIT_ASSERT_THROW_MG(pService->GetSpatialContexts(resource, activeOnly),
            MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");

        CPPUNIT_ASSERT_THROW_MG(pService->GetSpatialContexts(resource, activeOnly), MgResourceNotFoundException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        Ptr<MgSpatialContextReader> reader = pService->GetSpatialContexts(resource, activeOnly);
        bool bResult = reader->ReadNext();
        CPPUNIT_ASSERT(bResult);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        CPPUNIT_ASSERT_THROW_MG(pService->GetLongTransactions(resource, activeOnly),
            MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");

        CPPUNIT_ASSERT_THROW_MG(pService->GetLongTransactions(resource, activeOnly), MgResourceNotFoundException*);

        //TODO test with correct input
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        INT32 readerId = 0;

        CPPUNIT_ASSERT_THROW_MG(pService->GetFeatures(readerId), MgInvalidArgumentException*);

        //TODO test with correct input
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        INT32 readerId = 0;

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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        INT32 sqlReader = 0;

        CPPUNIT_ASSERT_THROW_MG(pService->GetSqlRows(sqlReader), MgInvalidArgumentException*);

        //TODO test with correct input
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        INT32 sqlReader = 0;

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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        INT32 featureReader = 0;
        INT32 xSize = 0;
        INT32 ySize = 0;

        CPPUNIT_ASSERT( NULL == (pService->GetRaster(featureReader, xSize, ySize, L"")) );

        //TODO test with correct input
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        INT32 dataReader = 0;

        CPPUNIT_ASSERT_THROW_MG(pService->GetDataRows(dataReader), MgInvalidArgumentException*);

        //TODO test with correct input
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        INT32 dataReader = 0;

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
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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
        STRING s = reader->GetString(L"NAME");
        STRING s2 = reader->GetString(L"Join1NAME");
        INT32 n = reader->GetInt32(L"Join1FeatId");

        CPPUNIT_ASSERT(bReadNextResult);
        CPPUNIT_ASSERT(bIsNullResult == false);
        //CPPUNIT_ASSERT(s == L"South Africa");
        CPPUNIT_ASSERT(s == L"Botswana");
        CPPUNIT_ASSERT(s2 == L"British Columbia");
        CPPUNIT_ASSERT(n == 3);

    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}

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

        STRING wkt = L"LOCAL_CS[\"*XY-MT*\",LOCAL_DATUM[\"*X-Y*\",10000],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
        Ptr<MgCreateSdfParams> params = new MgCreateSdfParams(L"ArbitraryXY", wkt, schema);

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(L"Library://UnitTests/Data/CreateFeatureSourceTest.FeatureSource");

        pService->CreateFeatureSource(resource, params);

        // Delete the resource
        pResourceService->DeleteResource(resource);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(GisException* e)
    {
        GIS_SAFE_RELEASE(e);
        CPPUNIT_FAIL("GisException occured");
    }
    catch(...)
    {
        throw;
    }
}
