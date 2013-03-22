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

#include "Services/Feature/FeatureUtil.h"
#include "MgDesktop.h"
#include "Services/Feature/FdoConnectionPool.h"
#include "TestFeatureService.h"
#include "CppUnitExtensions.h"
#include "Fdo.h"

//Uncommenting this will make the join benchmarks 10-15x slower (because we're writing each feature out!)
//
//#define DUMP_BENCHMARK_RESULTS

const STRING TEST_LOCALE = L"en";
const STRING wkt = L"LOCAL_CS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";

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
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestStart", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgdFeatureService> featSvc = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (featSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestStart", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // publish the map definition
        Ptr<MgResourceIdentifier> mapres1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/SheboyganWriteable.MapDefinition");
        Ptr<MgByteSource> mdfsrc1 = new MgByteSource(L"../UnitTestFiles/UT_Sheboygan_Writeable.mdf", false);
        Ptr<MgByteReader> mdfrdr1 = mdfsrc1->GetReader();
        pService->SetResource(mapres1, mdfrdr1, NULL);

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/Layers/ParcelsWriteable.LayerDefinition");
        Ptr<MgByteSource> ldfsrc3 = new MgByteSource(L"../UnitTestFiles/UT_Parcels_Writeable.ldf", false);
        Ptr<MgByteReader> ldfrdr3 = ldfsrc3->GetReader();
        pService->SetResource(ldfres3, ldfrdr3, NULL);

        Ptr<MgResourceIdentifier> fsres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/ParcelsWriteable.FeatureSource");
        Ptr<MgByteSource> fssrc3 = new MgByteSource(L"../UnitTestFiles/UT_Parcels_writeable.fs", false);
        Ptr<MgByteReader> fsrdr3 = fssrc3->GetReader();
        pService->SetResource(fsres3, fsrdr3, NULL);

        Ptr<MgByteSource> fsSource3 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.sdf", false);
        Ptr<MgByteReader> fsReader3 = fsSource3->GetReader();
        pService->SetResourceData(fsres3, L"UT_Parcels.sdf", L"File", fsReader3);

        MgResourceIdentifier resourceIdentifier1(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        MgResourceIdentifier resourceIdentifier2(L"Library://UnitTests/Data/Redding_Parcels.FeatureSource");
        MgResourceIdentifier resourceIdentifier3(L"Library://UnitTests/Data/Sheboygan_BuildingOutlines.FeatureSource");
        MgResourceIdentifier resourceIdentifier4(L"Library://UnitTests/Data/Sheboygan_VotingDistricts.FeatureSource");
        MgResourceIdentifier resourceIdentifier5(L"Library://UnitTests/Data/TestChainedInner1ToManyJoin.FeatureSource");
        MgResourceIdentifier resourceIdentifier6(L"Library://UnitTests/Data/Empty.FeatureSource");
        MgResourceIdentifier resourceIdentifier7(L"Library://UnitTests/Data/Sheboygan_Parcels_Writable.FeatureSource");
        MgResourceIdentifier resourceIdentifier8(L"Library://UnitTests/Data/FdoJoin.FeatureSource");
        MgResourceIdentifier resourceIdentifier9(L"Library://UnitTests/Data/SecuredCredentials.FeatureSource");
#ifdef _WIN32
        STRING resourceContentFileName1 = L"..\\UnitTestFiles\\Sheboygan_Parcels.FeatureSource";
        STRING resourceContentFileName2 = L"..\\UnitTestFiles\\Redding_Parcels.FeatureSource";
        STRING resourceContentFileName3 = L"..\\UnitTestFiles\\Sheboygan_BuildingOutlines.FeatureSource";
        STRING resourceContentFileName4 = L"..\\UnitTestFiles\\Sheboygan_VotingDistricts.FeatureSource";
        STRING resourceContentFileName5=  L"..\\UnitTestFiles\\TESTChainedInner1ToManyJoin.FeatureSource";
        STRING resourceContentFileName6=  L"..\\UnitTestFiles\\Empty.FeatureSource";
        STRING resourceContentFileName7=  L"..\\UnitTestFiles\\Sheboygan_Parcels_Writable.FeatureSource";
        STRING resourceContentFileName8=  L"..\\UnitTestFiles\\UT_FdoJoin.FeatureSource";
        STRING resourceContentFileName9=  L"..\\UnitTestFiles\\SecuredCredentials.fs";
        STRING dataFileName1 = L"..\\UnitTestFiles\\Sheboygan_Parcels.sdf";
        STRING dataFileName2 = L"..\\UnitTestFiles\\Redding_Parcels.shp";
        STRING dataFileName3 = L"..\\UnitTestFiles\\Redding_Parcels.dbf";
        STRING dataFileName4 = L"..\\UnitTestFiles\\Redding_Parcels.shx";
        STRING dataFileName5 = L"..\\UnitTestFiles\\Sheboygan_BuildingOutlines.sdf";
        STRING dataFileName6 = L"..\\UnitTestFiles\\Sheboygan_VotingDistricts.sdf";
        STRING dataFileName7 = L"..\\UnitTestFiles\\Empty.sdf";
        STRING dataFileName8 = L"..\\UnitTestFiles\\JoinTest.sqlite";
#else
        STRING resourceContentFileName1 = L"../UnitTestFiles/Sheboygan_Parcels.FeatureSource";
        STRING resourceContentFileName2 = L"../UnitTestFiles/Redding_Parcels.FeatureSource";
        STRING resourceContentFileName3 = L"../UnitTestFiles/Sheboygan_BuildingOutlines.FeatureSource";
        STRING resourceContentFileName4 = L"../UnitTestFiles/Sheboygan_VotingDistricts.FeatureSource";
        STRING resourceContentFileName5 = L"../UnitTestFiles/TESTChainedInner1ToManyJoin.FeatureSource";
        STRING resourceContentFileName6 = L"../UnitTestFiles/Empty.FeatureSource";
        STRING resourceContentFileName7=  L"../UnitTestFiles/Sheboygan_Parcels_Writable.FeatureSource";
        STRING resourceContentFileName8=  L"../UnitTestFiles/UT_FdoJoin.FeatureSource";
        STRING resourceContentFileName9=  L"../UnitTestFiles/SecuredCredentials.fs";
        STRING dataFileName1 = L"../UnitTestFiles/Sheboygan_Parcels.sdf";
        STRING dataFileName2 = L"../UnitTestFiles/Redding_Parcels.shp";
        STRING dataFileName3 = L"../UnitTestFiles/Redding_Parcels.dbf";
        STRING dataFileName4 = L"../UnitTestFiles/Redding_Parcels.shx";
        STRING dataFileName5 = L"../UnitTestFiles/Sheboygan_BuildingOutlines.sdf";
        STRING dataFileName6 = L"../UnitTestFiles/Sheboygan_VotingDistricts.sdf";
        STRING dataFileName7 = L"../UnitTestFiles/Empty.sdf";
        STRING dataFileName8 = L"../UnitTestFiles/JoinTest.sqlite";
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

        Ptr<MgByteSource> contentSource7 = new MgByteSource(resourceContentFileName7);
        Ptr<MgByteReader> contentReader7 = contentSource7->GetReader();
        pService->SetResource(&resourceIdentifier7, contentReader7, NULL);

        Ptr<MgByteSource> contentSource8 = new MgByteSource(resourceContentFileName8);
        Ptr<MgByteReader> contentReader8 = contentSource8->GetReader();
        pService->SetResource(&resourceIdentifier8, contentReader8, NULL);

        Ptr<MgByteSource> contentSource9 = new MgByteSource(resourceContentFileName9);
        Ptr<MgByteReader> contentReader9 = contentSource9->GetReader();
        pService->SetResource(&resourceIdentifier9, contentReader9, NULL);

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

        Ptr<MgByteSource> dataSource9 = new MgByteSource(dataFileName1);
        Ptr<MgByteReader> dataReader9 = dataSource9->GetReader();
        pService->SetResourceData(&resourceIdentifier7, L"Sheboygan_Parcels.sdf", L"File", dataReader9);

        Ptr<MgByteSource> dataSource10 = new MgByteSource(dataFileName8);
        Ptr<MgByteReader> dataReader10 = dataSource10->GetReader();
        pService->SetResourceData(&resourceIdentifier8, L"JoinTest.sqlite", L"File", dataReader10);

        //Sortable Right side data set
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/SortableRight.FeatureSource");
        Ptr<MgByteSource> fsContent = new MgByteSource(L"../UnitTestFiles/UT_Sortable_Right.fs");
        Ptr<MgByteReader> fsReader = fsContent->GetReader();

        pService->SetResource(fsId, fsReader, NULL);
        Ptr<MgByteSource> fsData = new MgByteSource(L"../UnitTestFiles/SortableRight.sqlite");
        Ptr<MgByteReader> fsDataReader = fsData->GetReader();

        pService->SetResourceData(fsId, L"SortableRight.sqlite", L"File", fsDataReader);
        //CPPUNIT_ASSERT(featSvc->TestConnection(fsId));

        //Sortable Left side data set
        fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/SortableLeft.FeatureSource");
        fsContent = new MgByteSource(L"../UnitTestFiles/UT_Sortable_Left.fs");
        fsReader = fsContent->GetReader();

        pService->SetResource(fsId, fsReader, NULL);
        fsData = new MgByteSource(L"../UnitTestFiles/SortableLeft.sqlite");
        fsDataReader = fsData->GetReader();

        pService->SetResourceData(fsId, L"SortableLeft.sqlite", L"File", fsDataReader);
        //CPPUNIT_ASSERT(featSvc->TestConnection(fsId));

        //Unsortable Right side data set
        fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/UnsortableRight.FeatureSource");
        fsContent = new MgByteSource(L"../UnitTestFiles/UT_Unsortable_Right.fs");
        fsReader = fsContent->GetReader();

        pService->SetResource(fsId, fsReader, NULL);
        fsData = new MgByteSource(L"../UnitTestFiles/UnsortableRight.sdf");
        fsDataReader = fsData->GetReader();

        pService->SetResourceData(fsId, L"UnsortableRight.sdf", L"File", fsDataReader);
        //CPPUNIT_ASSERT(featSvc->TestConnection(fsId));

        //Unsortable Left side data set
        fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/UnsortableLeft.FeatureSource");
        fsContent = new MgByteSource(L"../UnitTestFiles/UT_Unsortable_Left.fs");
        fsReader = fsContent->GetReader();

        pService->SetResource(fsId, fsReader, NULL);
        fsData = new MgByteSource(L"../UnitTestFiles/UnsortableLeft.sdf");
        fsDataReader = fsData->GetReader();

        pService->SetResourceData(fsId, L"UnsortableLeft.sdf", L"File", fsDataReader);
        //CPPUNIT_ASSERT(featSvc->TestConnection(fsId));
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
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestEnd",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }


#ifdef DEBUG
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nConnection Pool status: \n\n")));
        std::vector<PoolCacheEntry*> entries;
        MgFdoConnectionPool::GetCacheInfo(entries);
        for (std::vector<PoolCacheEntry*>::iterator it = entries.begin(); it != entries.end(); it++)
        {
            STRING resId = (*it)->ResourceId;
            ACE_DEBUG((LM_INFO, ACE_TEXT(" - %W (%d Open, %d Closed)\n"), resId.c_str(), (*it)->OpenCount, (*it)->ClosedCount));
        }
#endif

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

        Ptr<MgResourceIdentifier> fsres7 = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels_Writable.FeatureSource");
        pService->DeleteResource(fsres7);

        Ptr<MgResourceIdentifier> fsres8 = new MgResourceIdentifier(L"Library://UnitTests/Data/FdoJoin.FeatureSource");
        pService->DeleteResource(fsres8);
        
        Ptr<MgResourceIdentifier> fsres9 = new MgResourceIdentifier(L"Library://UnitTests/Data/SecuredCredentials.FeatureSource");
        pService->DeleteResource(fsres9);

        //Ptr<MgResourceIdentifier> folder = new MgResourceIdentifier(L"Library://UnitTests/");
        //pService->DeleteResource(folder);
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
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
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
/// This test case tests connections to the SDF provider.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestConnectionSDFProvider()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_TestConnectionSDFProvider", __LINE__, __WFILE__, NULL, L"", NULL);
        }

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
/// This test case gets the connection property values.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetConnectionPropertyValues()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
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
/// This test case gets the provider capabilities.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetCapabilities()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING provider = L"OSGeo.SDF";

        Ptr<MgByteReader> reader = pService->GetCapabilities(provider);
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
/// This test case tests a connection to a resource.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_TestConnectionResourceIdentifier()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
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
/// This test case exercises getting schemas.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetSchemas()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetSchemas", __LINE__, __WFILE__, NULL, L"", NULL);
        }

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
/// This test case exercises getting classes.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetClasses()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetClasses", __LINE__, __WFILE__, NULL, L"", NULL);
        }

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
/// This test case exercises getting class definitions.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetClassDefinition()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING schemaName = L"";
        STRING className = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->GetClassDefinition(resource, schemaName, className), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.MapDefinition");
        schemaName = L"";
        className = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->GetClassDefinition(resource, schemaName, className), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        schemaName = L"SHP_Schema";
        className = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->GetClassDefinition(resource, schemaName, className), MgClassNotFoundException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        schemaName = L"Foo";
        className = L"Bar";
        CPPUNIT_ASSERT_THROW_MG(pService->GetClassDefinition(resource, schemaName, className), MgFdoException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        schemaName = L"SHP_Schema";
        className = L"Bar";
        CPPUNIT_ASSERT_THROW_MG(pService->GetClassDefinition(resource, schemaName, className), MgFdoException*);

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
/// This test case verifies modifications to returned class definitions does not
/// affect subsequent fetches of the same class definition
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetClassDefinitionImmutability()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        STRING schemaName = L"SHP_Schema";
        STRING className = L"Parcels";
        Ptr<MgClassDefinition> classDef = pService->GetClassDefinition(resource, schemaName, className);
        STRING name = classDef->GetName();
        CPPUNIT_ASSERT(name == L"Parcels");

        //Remove RNAME
        Ptr<MgPropertyDefinitionCollection> props = classDef->GetProperties();
        INT32 pidx = props->IndexOf(L"RNAME");
        props->RemoveAt(pidx);
        pidx = props->IndexOf(L"RNAME");
        CPPUNIT_ASSERT(pidx < 0);

        //Fetch another one
        Ptr<MgClassDefinition> classDef2 = pService->GetClassDefinition(resource, schemaName, className);
        STRING name2 = classDef2->GetName();
        CPPUNIT_ASSERT(name2 == L"Parcels");
        CPPUNIT_ASSERT(classDef.p != classDef2.p);

        //Verify our modification of the previous return value did nothing
        Ptr<MgPropertyDefinitionCollection> props2 = classDef2->GetProperties();
        CPPUNIT_ASSERT(props2.p != props.p);
        CPPUNIT_ASSERT(props->GetCount() == props2->GetCount() - 1);
        pidx = props2->IndexOf(L"RNAME");
        CPPUNIT_ASSERT(pidx >= 0);
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
/// This test case exercises describing schemas.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_DescribeSchema()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

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
/// This test case verifies modifications to returned feature schemas does not
/// affect subsequent fetches of the same feature schemas
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_DescribeSchemaImmutability()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        STRING schemaName = L"";
        Ptr<MgFeatureSchemaCollection> schemaCollection = pService->DescribeSchema(resource, schemaName, NULL);
        CPPUNIT_ASSERT(schemaCollection->GetCount() > 0);

        schemaCollection->RemoveAt(0);

        //Verify modifications did not affect this return value
        Ptr<MgFeatureSchemaCollection> schemaCollection2 = pService->DescribeSchema(resource, schemaName, NULL);
        CPPUNIT_ASSERT(schemaCollection.p != schemaCollection2.p);
        CPPUNIT_ASSERT(schemaCollection->GetCount() == schemaCollection2->GetCount() - 1);

        Ptr<MgFeatureSchema> schema = schemaCollection2->GetItem(0);
        Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();

        classes->RemoveAt(0);

        Ptr<MgFeatureSchemaCollection> schemaCollection3 = pService->DescribeSchema(resource, schemaName, NULL);
        CPPUNIT_ASSERT(schemaCollection3.p != schemaCollection2.p);
        CPPUNIT_ASSERT(schemaCollection.p != schemaCollection3.p);
        CPPUNIT_ASSERT(schemaCollection->GetCount() == schemaCollection3->GetCount() - 1);

        Ptr<MgFeatureSchema> schema3 = schemaCollection3->GetItem(0);
        Ptr<MgClassDefinitionCollection> classes3 = schema3->GetClasses();

        CPPUNIT_ASSERT(classes.p != classes3.p);
        CPPUNIT_ASSERT(classes->GetCount() == classes3->GetCount() - 1);
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
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ApplySchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

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
/// This test case exercises selecting features.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SelectFeatures()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING className = L"";
        Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();
        CPPUNIT_ASSERT_THROW_MG(pService->SelectFeatures(resource, className, options), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->SelectFeatures(resource, className, options), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
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
/// This test case exercises selecting features with a transform.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SelectFeaturesTransformed()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgdFeatureService> pService = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Create our test data store
        Ptr<MgFeatureSchema> fs = new MgFeatureSchema(L"Default", L"");
        Ptr<MgClassDefinition> cls = new MgClassDefinition();
        cls->SetName(L"Test");
        
        Ptr<MgDataPropertyDefinition> id = new MgDataPropertyDefinition(L"ID");
        id->SetAutoGeneration(true);
        id->SetDataType(MgPropertyType::Int32);
        
        Ptr<MgGeometricPropertyDefinition> geom = new MgGeometricPropertyDefinition(L"Geometry");
        geom->SetGeometryTypes(MgFeatureGeometricType::Point);
        geom->SetSpatialContextAssociation(L"Default");

        Ptr<MgPropertyDefinitionCollection> props = cls->GetProperties();
        Ptr<MgPropertyDefinitionCollection> idProps = cls->GetIdentityProperties();

        props->Add(id);
        props->Add(geom);
        idProps->Add(id);

        cls->SetDefaultGeometryPropertyName(L"Geometry");
        Ptr<MgClassDefinitionCollection> classes = fs->GetClasses();
        classes->Add(cls);

        //We'll use a transform guaranteed to work. ArbitraryXY (unitA to unitB)
        //We just check that the transformed values are not the original, that way
        //we know that CS-Map did its job
        Ptr<MgCoordinateSystemFactory> csFact = new MgCoordinateSystemFactory();
        STRING srcWkt = csFact->ConvertCoordinateSystemCodeToWkt(L"XY-M");
        STRING dstWkt = csFact->ConvertCoordinateSystemCodeToWkt(L"XY-IN");

        Ptr<MgGeometryFactory> geomFact = new MgGeometryFactory();
        Ptr<MgAgfReaderWriter> agfRw = new MgAgfReaderWriter();
        Ptr<MgCreateSdfParams> create = new MgCreateSdfParams(L"Default", srcWkt, fs);
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/TransformTest.FeatureSource");
        pService->CreateFeatureSource(fsId, create);

        //Populate data store with test points

        Ptr<MgCoordinate> coord1 = geomFact->CreateCoordinateXY(-37.1020, 144.0020);
        Ptr<MgPoint> pt1 = geomFact->CreatePoint(coord1);
        Ptr<MgByteReader> agf1 = agfRw->Write(pt1);

        Ptr<MgPropertyCollection> propVals = new MgPropertyCollection();
        Ptr<MgGeometryProperty> pGeom = new MgGeometryProperty(L"Geometry", agf1);
        propVals->Add(pGeom);

        Ptr<MgFeatureReader> fr = pService->InsertFeatures(fsId, L"Default:Test", propVals);
        fr->Close();

        Ptr<MgCoordinate> coord2 = geomFact->CreateCoordinateXY(-37.2020, 144.2020);
        Ptr<MgPoint> pt2 = geomFact->CreatePoint(coord2);
        Ptr<MgByteReader> agf2 = agfRw->Write(pt2);

        pGeom->SetValue(agf2);
        fr = pService->InsertFeatures(fsId, L"Default:Test", propVals);
        fr->Close();

        //Now select from this data store
        Ptr<MgFeatureQueryOptions> query = new MgFeatureQueryOptions();
        Ptr<MgReader> reader = pService->SelectFeatures(fsId, L"Default:Test", query, dstWkt);
        
        CPPUNIT_ASSERT(reader->ReadNext());
        CPPUNIT_ASSERT(!reader->IsNull(L"Geometry"));

        Ptr<MgByteReader> txAgf1 = reader->GetGeometry(L"Geometry");
        Ptr<MgGeometry> txGeom1 = agfRw->Read(txAgf1);
        MgPoint* txPt1 = dynamic_cast<MgPoint*>(txGeom1.p);
        CPPUNIT_ASSERT(txPt1 != NULL);
        Ptr<MgCoordinate> txCoord1 = txPt1->GetCoordinate();

        //TODO: Maybe we should really check that it matches the expected transformed result
        CPPUNIT_ASSERT(txCoord1->GetX() != -37.1020);
        CPPUNIT_ASSERT(txCoord1->GetY() != 144.0020);

        CPPUNIT_ASSERT(reader->ReadNext());
        CPPUNIT_ASSERT(!reader->IsNull(L"Geometry"));

        Ptr<MgByteReader> txAgf2 = reader->GetGeometry(L"Geometry");
        Ptr<MgGeometry> txGeom2 = agfRw->Read(txAgf2);
        MgPoint* txPt2 = dynamic_cast<MgPoint*>(txGeom2.p);
        CPPUNIT_ASSERT(txPt2 != NULL);
        Ptr<MgCoordinate> txCoord2 = txPt2->GetCoordinate();

        //TODO: Maybe we should really check that it matches the expected transformed result
        CPPUNIT_ASSERT(txCoord2->GetX() != -37.2020);
        CPPUNIT_ASSERT(txCoord2->GetY() != 144.2020);

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
/// This test case exercises selecting features with the scrollable reader.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SelectScrollable()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgdFeatureService> pService = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING className = L"";
        Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();
        CPPUNIT_ASSERT_THROW_MG(pService->SelectFeaturesExtended(resource, className, options), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_VotingDistricts.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->SelectFeaturesExtended(resource, className, options), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_VotingDistricts.FeatureSource");
        CPPUNIT_ASSERT_THROW_MG(pService->SelectFeaturesExtended(resource, className, options), MgInvalidArgumentException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_VotingDistricts.FeatureSource");
        className = L"VotingDistricts";
        Ptr<MgdScrollableFeatureReader> reader = pService->SelectFeaturesExtended(resource, className, options);
        INT32 count = reader->Count();
        CPPUNIT_ASSERT(11 == count);

        Ptr<MgPropertyCollection> keys = new MgPropertyCollection();
        Ptr<MgInt32Property> keyVal = new MgInt32Property(L"FeatId", 4);
        Ptr<MgInt32Property> keyVal2 = new MgInt32Property(L"FeatId", 123);

        keys->Add(keyVal);

        bool bScroll = reader->ReadAt(keys);
        bool bScroll2 = reader->ReadAtIndex(7);
        bool bScroll3 = reader->ReadAtIndex(999);

        keys->Clear();
        keys->Add(keyVal2);

        bool bScroll4 = true;
        try {
            bScroll4 = reader->ReadAt(keys);
        } catch (MgFdoException* ex) {
            //If it gets here, it is a defect in the provider as the spec says return false on non-existent record
            ex->Release();
            bScroll4 = false;
        }
        bool bScroll5 = reader->ReadAtIndex(0); //indexes are 1-based
        bool bScroll6 = reader->ReadAtIndex(1);
        bool bFirst = reader->ReadFirst();
        bool bBeforeFirst = reader->ReadPrevious();
        bool bLast = reader->ReadLast();
        bool bAfterLast = reader->ReadNext();
        reader->Close();

        CPPUNIT_ASSERT(bScroll);
        CPPUNIT_ASSERT(bScroll2);
        CPPUNIT_ASSERT(!bScroll3);
        CPPUNIT_ASSERT(!bScroll4);
        CPPUNIT_ASSERT(!bScroll5);
        CPPUNIT_ASSERT(bScroll6);
        CPPUNIT_ASSERT(bFirst);
        CPPUNIT_ASSERT(!bBeforeFirst);
        CPPUNIT_ASSERT(bLast);
        CPPUNIT_ASSERT(!bAfterLast);
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
/// This test case exercises selecting aggregate features.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SelectAggregate()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SelectAggregate", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgDataReader> reader;
        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING className = L"";
        Ptr<MgFeatureAggregateOptions> options = new MgFeatureAggregateOptions();
        CPPUNIT_ASSERT_THROW_MG(reader = pService->SelectAggregate(resource, className, options), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(reader = pService->SelectAggregate(resource, className, options), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        CPPUNIT_ASSERT_THROW_MG(reader = pService->SelectAggregate(resource, className, options), MgInvalidArgumentException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        className = L"Parcels";
        options->SetFilter(L"Autogenerated_SDF_ID = 1");
        reader = pService->SelectAggregate(resource, className, options);
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

static int smTestSdfId = -1;

void TestFeatureService::TestCase_InsertFeatures()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgdFeatureService> pService = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_InsertFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels_Writable.FeatureSource");
        STRING className = L"Parcels";

        Ptr<MgPropertyCollection> propVals = new MgPropertyCollection();

        STRING origWkt = L"POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))";

        Ptr<MgAgfReaderWriter> agfRW = new MgAgfReaderWriter();
        Ptr<MgWktReaderWriter> wktRW = new MgWktReaderWriter();
        Ptr<MgGeometry> poly = wktRW->Read(origWkt);
        Ptr<MgByteReader> agf = agfRW->Write(poly);

        Ptr<MgProperty> rname = new MgStringProperty(L"RNAME", L"Hello Parcel");
        Ptr<MgProperty> geometry = new MgGeometryProperty(L"SHPGEOM", agf);

        propVals->Add(rname);
        propVals->Add(geometry);

        Ptr<MgFeatureReader> fr = pService->InsertFeatures(fsId, className, propVals);
        int inserted = 0;
        while(fr->ReadNext())
        {
            inserted++;
            smTestSdfId = fr->GetInt32(L"Autogenerated_SDF_ID");
        }
        fr->Close();

        CPPUNIT_ASSERT(inserted == 1);
        CPPUNIT_ASSERT(smTestSdfId != -1);

        //Do a select to verify it's definitely there
        Ptr<MgFeatureQueryOptions> query = new MgFeatureQueryOptions();
        STRING filter = L"Autogenerated_SDF_ID = ";
        wchar_t sdfNum[10];
        ACE_OS::itoa(smTestSdfId, sdfNum, 10);
        filter += sdfNum;
        query->SetFilter(filter);

        Ptr<MgReader> reader = pService->SelectFeatures(fsId, className, query);
        bool read = reader->ReadNext();
        CPPUNIT_ASSERT(read);
        CPPUNIT_ASSERT(!reader->IsNull(L"Autogenerated_SDF_ID"));
        CPPUNIT_ASSERT(!reader->IsNull(L"RNAME"));
        CPPUNIT_ASSERT(!reader->IsNull(L"SHPGEOM"));
        CPPUNIT_ASSERT(reader->GetInt32(L"Autogenerated_SDF_ID") == smTestSdfId);
        CPPUNIT_ASSERT(reader->GetString(L"RNAME") == L"Hello Parcel");

        agf = reader->GetGeometry(L"SHPGEOM");
        poly = agfRW->Read(agf);
        STRING wkt = wktRW->Write(poly);
        CPPUNIT_ASSERT(wkt == origWkt);

        for (INT32 i = 0; i < reader->GetPropertyCount(); i++)
        {
            STRING propName = reader->GetPropertyName(i);
            if (propName != L"Autogenerated_SDF_ID" &&
                propName != L"RNAME" &&
                propName != L"SHPGEOM")
            {
                CPPUNIT_ASSERT(reader->IsNull(propName));
                CPPUNIT_ASSERT(reader->IsNull(i));
            }
        }

        CPPUNIT_ASSERT(!reader->ReadNext());
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

void TestFeatureService::TestCase_InsertFeaturesBatch()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgResourceService> pResSvc = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pResSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_InsertFeaturesBatch", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgdFeatureService> pService = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_InsertFeaturesBatch", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Set up the feature source
        STRING wkt = L"LOCAL_CS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/BatchInsertTest.FeatureSource");
        STRING className = L"Parcels";

        Ptr<MgFeatureSchema> fs = new MgFeatureSchema(L"Default", L"");
        Ptr<MgClassDefinition> cls = new MgClassDefinition();
        cls->SetName(className);

        Ptr<MgDataPropertyDefinition> id = new MgDataPropertyDefinition(L"ID");
        id->SetAutoGeneration(true);
        id->SetDataType(MgPropertyType::Int32);

        Ptr<MgDataPropertyDefinition> name = new MgDataPropertyDefinition(L"RNAME");
        name->SetDataType(MgPropertyType::String);
        name->SetNullable(true);
        name->SetLength(256);

        Ptr<MgDataPropertyDefinition> price = new MgDataPropertyDefinition(L"PRICE");
        price->SetDataType(MgPropertyType::Double);
        price->SetNullable(true);
        
        Ptr<MgDataPropertyDefinition> dop = new MgDataPropertyDefinition(L"DATE");
        dop->SetDataType(MgPropertyType::DateTime);
        dop->SetNullable(true);

        Ptr<MgGeometricPropertyDefinition> geom = new MgGeometricPropertyDefinition(L"SHPGEOM");
        geom->SetGeometryTypes(MgFeatureGeometricType::Curve | MgFeatureGeometricType::Point | MgFeatureGeometricType::Surface);
        geom->SetSpatialContextAssociation(L"Default");

        Ptr<MgPropertyDefinitionCollection> props = cls->GetProperties();
        Ptr<MgPropertyDefinitionCollection> idProps = cls->GetIdentityProperties();

        props->Add(id);
        props->Add(name);
        props->Add(price);
        props->Add(dop);
        props->Add(geom);

        idProps->Add(id);

        cls->SetDefaultGeometryPropertyName(L"SHPGEOM");
        Ptr<MgClassDefinitionCollection> classes = fs->GetClasses();
        classes->Add(cls);

        if (pResSvc->ResourceExists(fsId))
            pResSvc->DeleteResource(fsId);

        Ptr<MgCreateSdfParams> create = new MgCreateSdfParams(L"Default", wkt, fs);
        pService->CreateFeatureSource(fsId, create);

        Ptr<MgAgfReaderWriter> agfRW = new MgAgfReaderWriter();
        Ptr<MgWktReaderWriter> wktRW = new MgWktReaderWriter();

        //Do the batch insert
        STRING origWkt = L"POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))";
        Ptr<MgBatchPropertyCollection> batch = new MgBatchPropertyCollection();

        //First feature
        Ptr<MgPropertyCollection> propVals = new MgPropertyCollection();
        Ptr<MgGeometry> poly = wktRW->Read(origWkt);
        Ptr<MgByteReader> agf = agfRW->Write(poly);
        Ptr<MgStringProperty> pName = new MgStringProperty(L"RNAME", L"Hello Parcel");
        Ptr<MgGeometryProperty> pGeom = new MgGeometryProperty(L"SHPGEOM", agf);
        Ptr<MgDoubleProperty> pPrice = new MgDoubleProperty(L"PRICE", 5000.0);
        Ptr<MgDateTime> pdt = new MgDateTime(2011, 9, 14);
        Ptr<MgDateTimeProperty> pDate = new MgDateTimeProperty(L"DATE", pdt);
        propVals->Add(pName);
        propVals->Add(pGeom);
        propVals->Add(pPrice);
        propVals->Add(pDate);

        batch->Add(propVals);

        //Second feature
        propVals = new MgPropertyCollection();
        poly = wktRW->Read(origWkt);
        agf = agfRW->Write(poly);
        pName = new MgStringProperty();
        pName->SetName(L"RNAME");
        pName->SetNull(true);
        pGeom = new MgGeometryProperty(L"SHPGEOM", agf);
        pPrice = new MgDoubleProperty();
        pPrice->SetName(L"PRICE");
        pPrice->SetNull(true);
        pDate = new MgDateTimeProperty();
        pDate->SetName(L"DATE");
        pDate->SetNull(true);
        propVals->Add(pName);
        propVals->Add(pGeom);
        propVals->Add(pPrice);
        propVals->Add(pDate);

        batch->Add(propVals);

        //Third feature
        propVals = new MgPropertyCollection();
        poly = wktRW->Read(origWkt);
        agf = agfRW->Write(poly);
        pName = new MgStringProperty(L"RNAME", L"Hello Parcel3");
        pGeom = new MgGeometryProperty(L"SHPGEOM", agf);
        pPrice = new MgDoubleProperty(L"PRICE", 15000.0);
        pdt = new MgDateTime(2011, 03, 14);
        pDate = new MgDateTimeProperty(L"DATE", pdt);
        propVals->Add(pName);
        propVals->Add(pGeom);
        propVals->Add(pPrice);
        propVals->Add(pDate);

        batch->Add(propVals);

        //Call the API
        Ptr<MgPropertyCollection> insertRes = pService->InsertFeatures(fsId, L"Default:Parcels", batch);
        
        //We inserted 3 features
        CPPUNIT_ASSERT(3 == insertRes->GetCount());

        for (INT32 i = 0; i < insertRes->GetCount(); i++)
        {
            Ptr<MgProperty> res = insertRes->GetItem(i);
            CPPUNIT_ASSERT(MgPropertyType::Feature == res->GetPropertyType());

            MgFeatureProperty* fp = static_cast<MgFeatureProperty*>(res.p);
            Ptr<MgFeatureReader> fr = fp->GetValue();
            fr->Close();
        }

        //Now query to verify all values are the same
        Ptr<MgFeatureReader> reader = pService->SelectFeatures(fsId, L"Default:Parcels", NULL);
        
        CPPUNIT_ASSERT(reader->ReadNext());

        CPPUNIT_ASSERT(!reader->IsNull(L"ID"));
        CPPUNIT_ASSERT(!reader->IsNull(L"RNAME"));
        CPPUNIT_ASSERT(!reader->IsNull(L"PRICE"));
        CPPUNIT_ASSERT(!reader->IsNull(L"DATE"));
        CPPUNIT_ASSERT(!reader->IsNull(L"SHPGEOM"));

        INT32 fid = reader->GetInt32(L"ID");
        STRING frname = reader->GetString(L"RNAME");
        double fprice = reader->GetDouble(L"PRICE");
        Ptr<MgDateTime> fdt = reader->GetDateTime(L"DATE");

        CPPUNIT_ASSERT(1 == fid);
        CPPUNIT_ASSERT(L"Hello Parcel" == frname);
        CPPUNIT_ASSERT(5000.0 == fprice);
        CPPUNIT_ASSERT(2011 == fdt->GetYear());
        CPPUNIT_ASSERT(9 == fdt->GetMonth());
        CPPUNIT_ASSERT(14 == fdt->GetDay());

        CPPUNIT_ASSERT(reader->ReadNext());

        CPPUNIT_ASSERT(!reader->IsNull(L"ID"));
        CPPUNIT_ASSERT(reader->IsNull(L"RNAME"));
        CPPUNIT_ASSERT(reader->IsNull(L"PRICE"));
        CPPUNIT_ASSERT(reader->IsNull(L"DATE"));
        CPPUNIT_ASSERT(!reader->IsNull(L"SHPGEOM"));

        fid = reader->GetInt32(L"ID");
        CPPUNIT_ASSERT(2 == fid);

        CPPUNIT_ASSERT(reader->ReadNext());

        CPPUNIT_ASSERT(!reader->IsNull(L"ID"));
        CPPUNIT_ASSERT(!reader->IsNull(L"RNAME"));
        CPPUNIT_ASSERT(!reader->IsNull(L"PRICE"));
        CPPUNIT_ASSERT(!reader->IsNull(L"DATE"));
        CPPUNIT_ASSERT(!reader->IsNull(L"SHPGEOM"));

        fid = reader->GetInt32(L"ID");
        frname = reader->GetString(L"RNAME");
        fprice = reader->GetDouble(L"PRICE");
        fdt = reader->GetDateTime(L"DATE");

        CPPUNIT_ASSERT(3 == fid);
        CPPUNIT_ASSERT(L"Hello Parcel3" == frname);
        CPPUNIT_ASSERT(15000.0 == fprice);
        CPPUNIT_ASSERT(2011 == fdt->GetYear());
        CPPUNIT_ASSERT(3 == fdt->GetMonth());
        CPPUNIT_ASSERT(14 == fdt->GetDay());

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

void TestFeatureService::TestCase_UpdateFeatures()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgdFeatureService> pService = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_UpdateFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels_Writable.FeatureSource");
        STRING className = L"Parcels";

        Ptr<MgPropertyCollection> propVals = new MgPropertyCollection();

        Ptr<MgNullableProperty> rpropad = new MgStringProperty(L"RPROPAD", L"Foo Bar");
        Ptr<MgNullableProperty> rname = new MgStringProperty(L"RNAME", L"");
        rname->SetNull(true);

        propVals->Add(rpropad);
        propVals->Add(rname);

        STRING filter = L"Autogenerated_SDF_ID = ";
        wchar_t sdfNum[10];
        ACE_OS::itoa(smTestSdfId, sdfNum, 10);
        filter += sdfNum;

        int updated = pService->UpdateFeatures(fsId, className, propVals, filter);
        CPPUNIT_ASSERT(updated == 1);

        Ptr<MgFeatureQueryOptions> query = new MgFeatureQueryOptions();
        query->SetFilter(filter);

        Ptr<MgFeatureReader> reader = pService->SelectFeatures(fsId, className, query);
        CPPUNIT_ASSERT(reader->ReadNext());

        CPPUNIT_ASSERT(!reader->IsNull(L"RPROPAD"));
        CPPUNIT_ASSERT(reader->GetString(L"RPROPAD") == L"Foo Bar");
        
        for (INT32 i = 0; i < reader->GetPropertyCount(); i++)
        {
            STRING propName = reader->GetPropertyName(i);
            if (propName != L"Autogenerated_SDF_ID" &&
                propName != L"RPROPAD" &&
                propName != L"SHPGEOM")
            {
                CPPUNIT_ASSERT(reader->IsNull(propName));
                CPPUNIT_ASSERT(reader->IsNull(i));
            }
        }

        CPPUNIT_ASSERT(!reader->ReadNext());
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

void TestFeatureService::TestCase_DeleteFeatures()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgdFeatureService> pService = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_DeleteFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels_Writable.FeatureSource");
        STRING className = L"Parcels";

        STRING filter = L"Autogenerated_SDF_ID = ";
        wchar_t sdfNum[10];
        ACE_OS::itoa(smTestSdfId, sdfNum, 10);
        filter += sdfNum;

        int deleted = pService->DeleteFeatures(fsId, className, filter);
        CPPUNIT_ASSERT(deleted == 1);
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
/// This test case exercises executing SQL queries containing SELECT
/// statements.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ExecuteSqlQuery()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ExecuteSqlQuery", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        const STRING sqlQuery = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->ExecuteSqlQuery(resource, sqlQuery), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Test.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->ExecuteSqlQuery(resource, sqlQuery), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Test.FeatureSource");
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
/// This test case exercises executing SQL queries not containing
/// SELECT statements.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ExecuteSqlNonQuery()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ExecuteSqlNonQuery", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        const STRING sqlNonQuery = L"";
        CPPUNIT_ASSERT_THROW_MG(pService->ExecuteSqlNonQuery(resource, sqlNonQuery), MgInvalidRepositoryTypeException*);
        
        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Test.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->ExecuteSqlNonQuery(resource, sqlNonQuery), MgInvalidResourceTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Test.FeatureSource");
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
/// This test case exercises getting spatial contexts.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetSpatialContexts()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetSpatialContexts", __LINE__, __WFILE__, NULL, L"", NULL);
        }

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
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("TestFeatureService::TestCase_GetSpatialContexts()\nName: %W\nWKT: %W\n"), csName.c_str(), csWkt.c_str()));

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
/// This test case exercises getting long transactions.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetLongTransactions()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

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
/// This test case exercises setting long transaction names.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_SetLongTransaction()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SetLongTransaction", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        STRING longTransactionName = L"Live";

        // verify exception when passing in a NULL resource
        CPPUNIT_ASSERT_THROW_MG(pService->SetLongTransaction(NULL, longTransactionName), MgNullArgumentException*);

        // verify exception when passing in an invalid resource type
        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        CPPUNIT_ASSERT_THROW_MG(pService->SetLongTransaction(resource, longTransactionName), MgInvalidRepositoryTypeException*);

        resource = new MgResourceIdentifier(L"Library://UnitTests/Geography/World.MapDefinition");
        CPPUNIT_ASSERT_THROW_MG(pService->SetLongTransaction(resource, longTransactionName), MgInvalidResourceTypeException*);

        // verify exception when session is not set
        //resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        //CPPUNIT_ASSERT_THROW_MG(pService->SetLongTransaction(resource, longTransactionName), MgSessionNotFoundException*);

        // set the user information for the current thread to be administrator - this
        // alone doesn't create a session
        //CPPUNIT_ASSERT_THROW_MG(pService->SetLongTransaction(resource, longTransactionName), MgSessionNotFoundException*);

        // now set a session
        //CPPUNIT_ASSERT(pService->SetLongTransaction(resource, longTransactionName));

        // setting the same LT name twice shouldn't matter
        //CPPUNIT_ASSERT(pService->SetLongTransaction(resource, longTransactionName));

        // should be able to clear the LT name
        //CPPUNIT_ASSERT(pService->SetLongTransaction(resource, L""));
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

/*
///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting features.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetFeatures()
{
    try
    {
        Ptr<MgFeatureService> pService = MgdServiceFactory::CreateFeatureService();
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
/// This test case exercises closing a feature reader.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_CloseFeatureReader()
{
    try
    {
        Ptr<MgFeatureService> pService = MgdServiceFactory::CreateFeatureService();
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
/// This test case exercises getting SQL rows.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetSqlRows()
{
    try
    {
        Ptr<MgFeatureService> pService = MgdServiceFactory::CreateFeatureService();
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
/// This test case exercises closing a SQL reader.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_CloseSqlReader()
{
    try
    {
        Ptr<MgFeatureService> pService = MgdServiceFactory::CreateFeatureService();
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
*/

/*
///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting a raster.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetRaster()
{
    try
    {
        Ptr<MgFeatureService> pService = MgdServiceFactory::CreateFeatureService();
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
*/

/*
///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises getting data rows.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_GetDataRows()
{
    try
    {
        Ptr<MgFeatureService> pService = MgdServiceFactory::CreateFeatureService();
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
/// This test case exercises closing a data reader.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_CloseDataReader()
{
    try
    {
        Ptr<MgFeatureService> pService = MgdServiceFactory::CreateFeatureService();
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
*/

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises joining features.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_JoinFeatures()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
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
/// This test case exercises joining features defined as chained, inner, 1-to-many.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_JoinFeaturesChainedInner1ToMany()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
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

        /*
        #ifdef _DEBUG
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        ACE_DEBUG((LM_INFO, ACE_TEXT("TestFeatureService::TestCase_JoinFeaturesChainedInner1ToMany() - End\n")));
        #endif
        */

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
        STRING st = e->GetStackTrace(TEST_LOCALE);
        message += L"\n" + st;
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
/// This test case exercises schema description APIs against a feature source
/// with Extended Feature Classes
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_ExtendedFeatureClass()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_ExtendedFeatureClass",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> lFeatureSource = new MgResourceIdentifier(L"Library://UnitTests/Data/TestChainedInner1ToManyJoin.FeatureSource");

        Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"Ext1", NULL);

        //With GetIdentityProperties
        Ptr<MgPropertyDefinitionCollection> idProps = pService->GetIdentityProperties(lFeatureSource, L"SHP_Schema", L"Ext1");
        CPPUNIT_ASSERT(NULL != idProps);
        CPPUNIT_ASSERT(1 == idProps->GetCount());

        Ptr<MgStringCollection> findClasses = new MgStringCollection();
        findClasses->Add(L"Ext1");
        Ptr<MgClassDefinitionCollection> matches = pService->GetIdentityProperties(lFeatureSource, L"SHP_Schema", findClasses);
        CPPUNIT_ASSERT(NULL != matches);
        CPPUNIT_ASSERT(1 == matches->GetCount());

        Ptr<MgFeatureSchemaCollection> schemas;
        Ptr<MgFeatureSchema> schema;
        //With GetClasses
        Ptr<MgStringCollection> classNames = pService->GetClasses(lFeatureSource, L"");
        CPPUNIT_ASSERT(classNames->GetCount() > 0);
        bool bFound = false;
        for (INT32 i = 0; i < classNames->GetCount(); i++)
        {
            STRING name = classNames->GetItem(i);
            if (name.find(L"Ext1") != STRING::npos)
            {
                bFound = true;
                break;
            }
        }
        CPPUNIT_ASSERT(bFound);

        //With 1st variation of DescribeSchema()
        schemas = pService->DescribeSchema(lFeatureSource, L"");
        CPPUNIT_ASSERT(NULL != schemas.p);
        CPPUNIT_ASSERT(1 == schemas->GetCount());
        schema = schemas->GetItem(0);
        Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();
        bFound = false;
        for (INT32 i = 0; i < classes->GetCount(); i++)
        {
            Ptr<MgClassDefinition> klass = classes->GetItem(i);
            if (klass->GetName() == L"Ext1")
            {
                bFound = true;
                break;
            }
        }
        CPPUNIT_ASSERT(bFound);

        //1st variation of DescribeSchema() with the actual parent schema name
        schemas = pService->DescribeSchema(lFeatureSource, L"SHP_Schema");
        CPPUNIT_ASSERT(NULL != schemas.p);
        CPPUNIT_ASSERT(1 == schemas->GetCount());
        schema = schemas->GetItem(0);
        classes = schema->GetClasses();
        bFound = false;
        for (INT32 i = 0; i < classes->GetCount(); i++)
        {
            Ptr<MgClassDefinition> klass = classes->GetItem(i);
            if (klass->GetName() == L"Ext1")
            {
                bFound = true;
                break;
            }
        }
        CPPUNIT_ASSERT(bFound);
        Ptr<MgStringCollection> findClassNames = new MgStringCollection();
        findClassNames->Add(L"Ext1");

        //2nd variation of DescribeSchema()
        schemas = pService->DescribeSchema(lFeatureSource, L"SHP_Schema", findClassNames);
        CPPUNIT_ASSERT(NULL != schemas.p);
        CPPUNIT_ASSERT(1 == schemas->GetCount());
        schema = schemas->GetItem(0);
        classes = schema->GetClasses();
        bFound = false;
        for (INT32 i = 0; i < classes->GetCount(); i++)
        {
            Ptr<MgClassDefinition> klass = classes->GetItem(i);
            if (klass->GetName() == L"Ext1")
            {
                bFound = true;
                break;
            }
        }
        CPPUNIT_ASSERT(bFound);

        Ptr<MgClassDefinition> clsDef = pService->GetClassDefinition(lFeatureSource, L"SHP_Schema", L"Ext1");
        CPPUNIT_ASSERT(NULL != clsDef.p);

        //Custom aggregates should also work
        Ptr<MgFeatureAggregateOptions> agg = new MgFeatureAggregateOptions();
        agg->AddComputedProperty(L"THE_EXTENT", L"EXTENT(SHPGEOM)");
        Ptr<MgDataReader> dr = pService->SelectAggregate(lFeatureSource, L"Ext1", agg);
        bool bRead = dr->ReadNext();
        dr->Close();
        CPPUNIT_ASSERT(bRead);

        //XML form should also have this class
        STRING xml = pService->DescribeSchemaAsXml(lFeatureSource, L"");
        CPPUNIT_ASSERT(!xml.empty());
        Ptr<MgFeatureSchemaCollection> pSchemas = pService->XmlToSchema(xml);
        CPPUNIT_ASSERT(NULL != pSchemas.p);
        CPPUNIT_ASSERT(1 == pSchemas->GetCount());
        Ptr<MgFeatureSchema> pSchema = pSchemas->GetItem(0);
        Ptr<MgClassDefinitionCollection> pClasses = pSchema->GetClasses();
        CPPUNIT_ASSERT(NULL != pClasses.p);
        CPPUNIT_ASSERT(pClasses->GetCount() > 0);
        bFound = false;
        for (INT32 i = 0; i < pClasses->GetCount(); i++)
        {
            Ptr<MgClassDefinition> klass = pClasses->GetItem(i);
            if (klass->GetName() == L"Ext1")
            {
                bFound = true;
                break;
            }
        }
        CPPUNIT_ASSERT(bFound);

        //XML form should also have this class
        xml = pService->DescribeSchemaAsXml(lFeatureSource, L"SHP_Schema");
        CPPUNIT_ASSERT(!xml.empty());
        pSchemas = pService->XmlToSchema(xml);
        CPPUNIT_ASSERT(NULL != pSchemas.p);
        CPPUNIT_ASSERT(1 == pSchemas->GetCount());
        pSchema = pSchemas->GetItem(0);
        pClasses = pSchema->GetClasses();
        CPPUNIT_ASSERT(NULL != pClasses.p);
        CPPUNIT_ASSERT(pClasses->GetCount() > 0);
        bFound = false;
        for (INT32 i = 0; i < pClasses->GetCount(); i++)
        {
            Ptr<MgClassDefinition> klass = pClasses->GetItem(i);
            if (klass->GetName() == L"Ext1")
            {
                bFound = true;
                break;
            }
        }
        CPPUNIT_ASSERT(bFound);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        STRING st = e->GetStackTrace(TEST_LOCALE);
        message += L"\n" + st;
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
/// This test case exercises the UpdateFeatures API for insertion
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_UpdateFeaturesInsert()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        if (fact == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_SavePoint", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SavePoint", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> featureSource = new MgResourceIdentifier(L"Library://UnitTests/Data/TestInsert.FeatureSource");

        //Create our test data store (SQLite)
        STRING scName = L"Default";
        STRING srsWkt = L"LOCAL_CS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
        
        //Setup schema
        Ptr<MgFeatureSchema> schema = new MgFeatureSchema(L"Default", L"Test schema for TestCase_2218");
        Ptr<MgClassDefinition> klass = new MgClassDefinition();
        klass->SetName(L"TestClass");
        Ptr<MgPropertyDefinitionCollection> clsProps = klass->GetProperties();
        Ptr<MgPropertyDefinitionCollection> idProps = klass->GetIdentityProperties();
        
        //ID
        Ptr<MgDataPropertyDefinition> idProperty = new MgDataPropertyDefinition(L"ID");
        idProperty->SetDataType(MgPropertyType::Int32);
        idProperty->SetAutoGeneration(false);
        idProperty->SetNullable(false);
        clsProps->Add(idProperty);
        idProps->Add(idProperty);
        //Name
        Ptr<MgDataPropertyDefinition> nameProperty = new MgDataPropertyDefinition(L"Name");
        nameProperty->SetDataType(MgPropertyType::String);
        nameProperty->SetLength(255);
        nameProperty->SetNullable(true);
        clsProps->Add(nameProperty);
        //Geom
        Ptr<MgGeometricPropertyDefinition> geomProperty = new MgGeometricPropertyDefinition(L"Geom");
        geomProperty->SetSpatialContextAssociation(scName);
        clsProps->Add(geomProperty);

        klass->SetDefaultGeometryPropertyName(L"Geom");

        Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();
        classes->Add(klass);

        //Create the feature source
        Ptr<MgFileFeatureSourceParams> fileParams = new MgFileFeatureSourceParams(L"OSGeo.SQLite", scName, srsWkt, schema);
        pService->CreateFeatureSource(featureSource, fileParams);

        //Set up insert command
        Ptr<MgFeatureCommandCollection> commands = new MgFeatureCommandCollection();
        Ptr<MgPropertyCollection> propVals = new MgPropertyCollection();
        
        Ptr<MgInt32Property> idVal = new MgInt32Property(L"ID", 1);
        Ptr<MgStringProperty> nameVal = new MgStringProperty(L"Name", L"Foo");
        
        Ptr<MgWktReaderWriter> wktRw = new MgWktReaderWriter();
        Ptr<MgAgfReaderWriter> agfRw = new MgAgfReaderWriter();
        Ptr<MgGeometry> geom = wktRw->Read(L"POINT (1 2)");
        Ptr<MgByteReader> agf = agfRw->Write(geom);
        Ptr<MgGeometryProperty> geomVal = new MgGeometryProperty(L"Geom", agf);

        propVals->Add(idVal);
        propVals->Add(nameVal);
        propVals->Add(geomVal);

        Ptr<MgInsertFeatures> insert = new MgInsertFeatures(L"Default:TestClass", propVals);
        commands->Add(insert);

        //Execute insert. Should be fine
        Ptr<MgPropertyCollection> result = pService->UpdateFeatures(featureSource, commands, true);
        CPPUNIT_ASSERT(result->GetCount() == 1);
        INT32 i = 0;
        Ptr<MgProperty> resItem1 = result->GetItem(i);
        CPPUNIT_ASSERT(resItem1->GetPropertyType() == MgPropertyType::Feature);
        Ptr<MgFeatureReader> rdr = ((MgFeatureProperty*)resItem1.p)->GetValue();
        rdr->Close();

        //Change name, retain same id
        nameVal->SetValue(L"Bar");

        //Execute again, expect MgFdoException due to constraint violation
        CPPUNIT_ASSERT_THROW_MG(result = pService->UpdateFeatures(featureSource, commands, true), MgFdoException*);
        
        //Expect one inserted result
        Ptr<MgFeatureQueryOptions> query = new MgFeatureQueryOptions();
        Ptr<MgFeatureReader> qryReader = pService->SelectFeatures(featureSource, L"Default:TestClass", query);
        INT32 count = 0;
        while (qryReader->ReadNext()) { count++; }
        qryReader->Close();
        CPPUNIT_ASSERT(1 == count);

        //Execute again, useTransaction = false. Should not throw exception, but log
        //the error as a MgStringProperty
        result = pService->UpdateFeatures(featureSource, commands, false);
        CPPUNIT_ASSERT(result->GetCount() == 1);
        resItem1 = result->GetItem(i);
        CPPUNIT_ASSERT(resItem1->GetPropertyType() == MgPropertyType::String); //Errors are of type String

        //Use new id
        idVal->SetValue(2);

        //Should be fine now
        result = pService->UpdateFeatures(featureSource, commands, true);
        CPPUNIT_ASSERT(result->GetCount() == 1);
        resItem1 = result->GetItem(i);
        CPPUNIT_ASSERT(resItem1->GetPropertyType() == MgPropertyType::Feature);
        rdr = ((MgFeatureProperty*)resItem1.p)->GetValue();
        rdr->Close();
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
/// This test case exercises the FDO join optimization
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_JoinFdoFeatures()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_JoinFdoFeatures",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> lFeatureSource = new MgResourceIdentifier(L"Library://UnitTests/Data/FdoJoin.FeatureSource");

        Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"CitiesCountries", NULL);

        INT32 count1 = 0;
        while(reader->ReadNext())
        {
            CPPUNIT_ASSERT(reader->IsNull(L"ID") == false);
            CPPUNIT_ASSERT(reader->IsNull(L"CountryCode") == false);
            CPPUNIT_ASSERT(reader->IsNull(L"StateCode") == false);
            CPPUNIT_ASSERT(reader->IsNull(L"Name") == false);
            CPPUNIT_ASSERT(reader->IsNull(L"Population") == false);
            CPPUNIT_ASSERT(reader->IsNull(L"CNT_CountryCode") == false);
            CPPUNIT_ASSERT(reader->IsNull(L"CNT_Name") == false);
            count1++;
        }
        reader->Close();
        CPPUNIT_ASSERT(10 == count1);

        Ptr<MgFeatureReader> reader2 = pService->SelectFeatures(lFeatureSource, L"CitiesStates", NULL);

        INT32 count2 = 0;
        while(reader2->ReadNext())
        {
            CPPUNIT_ASSERT(reader2->IsNull(L"ID") == false);
            CPPUNIT_ASSERT(reader2->IsNull(L"CountryCode") == false);
            CPPUNIT_ASSERT(reader2->IsNull(L"StateCode") == false);
            CPPUNIT_ASSERT(reader2->IsNull(L"Name") == false);
            CPPUNIT_ASSERT(reader2->IsNull(L"Population") == false);
            CPPUNIT_ASSERT(reader2->IsNull(L"ST_ID") == false);
            CPPUNIT_ASSERT(reader2->IsNull(L"ST_CountryCode") == false);
            CPPUNIT_ASSERT(reader2->IsNull(L"ST_StateCode") == false);
            CPPUNIT_ASSERT(reader2->IsNull(L"ST_Name") == false);
            count2++;
        }
        reader2->Close();
        CPPUNIT_ASSERT(10 == count2);

        Ptr<MgFeatureReader> reader3 = pService->SelectFeatures(lFeatureSource, L"CitiesStatesOneToOne", NULL);
        INT32 count3 = 0;
        while(reader3->ReadNext())
        {
            count3++;
        }
        reader3->Close();
        CPPUNIT_ASSERT(10 == count3);
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
/// This test case exercises creating a feature source.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_CreateFeatureSource()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_CreateFeatureSource",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pResourceService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pResourceService == 0)
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
        Ptr<MgFileFeatureSourceParams> params4 = new MgFileFeatureSourceParams(L"OSGeo.SQLite", L"ArbitraryXY", wkt, schema);
        params4->SetFileName(L"sqlite.sqlite");
        Ptr<MgResourceIdentifier> resource4 = new MgResourceIdentifier(L"Library://UnitTests/Data/CreateFeatureSourceTest4.FeatureSource");
        pService->CreateFeatureSource(resource4, params4);
        // We use schema name "Default" because Sqlite FDO provider always returns a schema named "Default"
        Ptr<MgFeatureSchemaCollection> tempSchema4 = pService->DescribeSchema(resource4, L"Default");
        // Delete SQLite feature source
        pResourceService->DeleteResource(resource4);
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
/// This test case benchmarks selecting features.
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_BenchmarkSelectFeatures()
{
    try
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSelectFeatures() - Start\n")));

        Ptr<MgdServiceFactory> factory = new MgdServiceFactory();
        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(factory->CreateService(MgServiceType::FeatureService));
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

void TestFeatureService::TestCase_SecuredCredentials()
{
    //This test requires SQL Server express with a MgUnitTest database created.
    //Once these requirements are met, fill in the required SQL Server credentials here
    STRING username = L"";
    STRING password = L"";
    try
    {
        Ptr<MgdServiceFactory> factory = new MgdServiceFactory();
        Ptr<MgFeatureService> featSvc = dynamic_cast<MgFeatureService*>(factory->CreateService(MgServiceType::FeatureService));
        if (featSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SecuredCredentials", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgdResourceService> resSvc = dynamic_cast<MgdResourceService*>(factory->CreateService(MgServiceType::ResourceService));
        if (resSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_SecuredCredentials", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/SecuredCredentials.FeatureSource");
        //Test as-is, this should be bad (false or exception, just as long as it is not true)
        try
        {
            bool bResult = featSvc->TestConnection(fsId);
            CPPUNIT_ASSERT(false == bResult);
        }
        catch (MgFdoException* ex)
        {
            SAFE_RELEASE(ex);
        }

        //Now apply credentials
        resSvc->SetResourceCredentials(fsId, username, password);
        CPPUNIT_ASSERT(true == featSvc->TestConnection(fsId));

        //Now apply bad credentials
        username = L"foo";
        password = L"bar";
        resSvc->SetResourceCredentials(fsId, username, password);
        //this should be bad (false or exception, just as long as it is not true)
        try
        {
            bool bResult = featSvc->TestConnection(fsId);
            CPPUNIT_ASSERT(false == bResult);
        }
        catch (MgFdoException* ex)
        {
            SAFE_RELEASE(ex);
        }

        //Try again without MG_USER_CREDENTIALS
        resSvc->DeleteResourceData(fsId, MgResourceDataName::UserCredentials);
        //this should be bad (false or exception, just as long as it is not true)
        try
        {
            bool bResult = featSvc->TestConnection(fsId);
            CPPUNIT_ASSERT(false == bResult);
        }
        catch (MgFdoException* ex)
        {
            SAFE_RELEASE(ex);
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

MgdMap* TestFeatureService::CreateTestMap()
{
    Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/SheboyganWriteable.MapDefinition");
    MgdMap* map = new MgdMap(mdfres);
    //map->Create(mdfres, L"UnitTestSheboygan");

    Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-87.733253, 43.746199);
    Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
    map->SetViewCenter(ptNewCenter);
    map->SetViewScale(75000.0);
    map->SetDisplayDpi(96);
    map->SetDisplayWidth(1024);
    map->SetDisplayHeight(1024);

    return map;
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case exercises the InsertFeatures() convenience API
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_LayerInsertFeatures()
{
    try
    {
        Ptr<MgdMap> map = CreateTestMap();
        Ptr<MgLayerCollection> layers = map->GetLayers();

        Ptr<MgdLayer> parcels = static_cast<MgdLayer*>(layers->GetItem(L"Parcels"));
        
        Ptr<MgPropertyCollection> propVals = new MgPropertyCollection();

        STRING origWkt = L"POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))";

        Ptr<MgAgfReaderWriter> agfRW = new MgAgfReaderWriter();
        Ptr<MgWktReaderWriter> wktRW = new MgWktReaderWriter();
        Ptr<MgGeometry> poly = wktRW->Read(origWkt);
        Ptr<MgByteReader> agf = agfRW->Write(poly);

        Ptr<MgProperty> rname = new MgStringProperty(L"RNAME", L"Hello Parcel");
        Ptr<MgProperty> geometry = new MgGeometryProperty(L"SHPGEOM", agf);

        propVals->Add(rname);
        propVals->Add(geometry);

        Ptr<MgFeatureReader> fr = parcels->InsertFeatures(propVals);
        int inserted = 0;
        while(fr->ReadNext())
        {
            inserted++;
            smTestSdfId = fr->GetInt32(L"Autogenerated_SDF_ID");
        }
        fr->Close();

        CPPUNIT_ASSERT(inserted == 1);
        CPPUNIT_ASSERT(smTestSdfId != -1);

        //Do a select to verify it's definitely there
        Ptr<MgFeatureQueryOptions> query = new MgFeatureQueryOptions();
        STRING filter = L"Autogenerated_SDF_ID = ";
        wchar_t sdfNum[10];
        ACE_OS::itoa(smTestSdfId, sdfNum, 10);
        filter += sdfNum;
        query->SetFilter(filter);

        Ptr<MgReader> reader = parcels->SelectFeatures(query);
        bool read = reader->ReadNext();
        CPPUNIT_ASSERT(read);
        CPPUNIT_ASSERT(!reader->IsNull(L"Autogenerated_SDF_ID"));
        CPPUNIT_ASSERT(!reader->IsNull(L"RNAME"));
        CPPUNIT_ASSERT(!reader->IsNull(L"SHPGEOM"));
        CPPUNIT_ASSERT(reader->GetInt32(L"Autogenerated_SDF_ID") == smTestSdfId);
        CPPUNIT_ASSERT(reader->GetString(L"RNAME") == L"Hello Parcel");

        agf = reader->GetGeometry(L"SHPGEOM");
        poly = agfRW->Read(agf);
        STRING wkt = wktRW->Write(poly);
        CPPUNIT_ASSERT(wkt == origWkt);

        for (INT32 i = 0; i < reader->GetPropertyCount(); i++)
        {
            STRING propName = reader->GetPropertyName(i);
            if (propName != L"Autogenerated_SDF_ID" &&
                propName != L"RNAME" &&
                propName != L"SHPGEOM")
            {
                CPPUNIT_ASSERT(reader->IsNull(propName));
                CPPUNIT_ASSERT(reader->IsNull(i));
            }
        }

        CPPUNIT_ASSERT(!reader->ReadNext());
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
/// This test case exercises the UpdateFeatures() convenience API
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_LayerUpdateFeatures()
{
    try
    {
        Ptr<MgdMap> map = CreateTestMap();
        Ptr<MgLayerCollection> layers = map->GetLayers();

        Ptr<MgdLayer> parcels = static_cast<MgdLayer*>(layers->GetItem(L"Parcels"));

        Ptr<MgPropertyCollection> propVals = new MgPropertyCollection();

        Ptr<MgNullableProperty> rpropad = new MgStringProperty(L"RPROPAD", L"Foo Bar");
        Ptr<MgNullableProperty> rname = new MgStringProperty(L"RNAME", L"");
        rname->SetNull(true);

        propVals->Add(rpropad);
        propVals->Add(rname);

        STRING filter = L"Autogenerated_SDF_ID = ";
        wchar_t sdfNum[10];
        ACE_OS::itoa(smTestSdfId, sdfNum, 10);
        filter += sdfNum;

        int updated = parcels->UpdateFeatures(propVals, filter);
        CPPUNIT_ASSERT(updated == 1);

        Ptr<MgFeatureQueryOptions> query = new MgFeatureQueryOptions();
        query->SetFilter(filter);

        Ptr<MgFeatureReader> reader = parcels->SelectFeatures(query);
        CPPUNIT_ASSERT(reader->ReadNext());

        CPPUNIT_ASSERT(!reader->IsNull(L"RPROPAD"));
        CPPUNIT_ASSERT(reader->GetString(L"RPROPAD") == L"Foo Bar");
        
        for (INT32 i = 0; i < reader->GetPropertyCount(); i++)
        {
            STRING propName = reader->GetPropertyName(i);
            if (propName != L"Autogenerated_SDF_ID" &&
                propName != L"RPROPAD" &&
                propName != L"SHPGEOM")
            {
                CPPUNIT_ASSERT(reader->IsNull(propName));
                CPPUNIT_ASSERT(reader->IsNull(i));
            }
        }

        CPPUNIT_ASSERT(!reader->ReadNext());
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
/// This test case exercises the DeleteFeatures() convenience API
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_LayerDeleteFeatures()
{
    try
    {
        Ptr<MgdMap> map = CreateTestMap();
        Ptr<MgLayerCollection> layers = map->GetLayers();

        Ptr<MgdLayer> parcels = static_cast<MgdLayer*>(layers->GetItem(L"Parcels"));

        STRING filter = L"Autogenerated_SDF_ID = ";
        wchar_t sdfNum[10];
        ACE_OS::itoa(smTestSdfId, sdfNum, 10);
        filter += sdfNum;

        int deleted = parcels->DeleteFeatures(filter);
        CPPUNIT_ASSERT(deleted == 1);
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
/// This test case exercises the SelectFeaturesExtended() convenience API
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_LayerSelectScrollable()
{
    try
    {
        Ptr<MgdMap> map = CreateTestMap();
        Ptr<MgLayerCollection> layers = map->GetLayers();

        Ptr<MgdLayer> parcels = static_cast<MgdLayer*>(layers->GetItem(L"Parcels"));

        Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();
        Ptr<MgdScrollableFeatureReader> reader = parcels->SelectFeaturesExtended(options);
        INT32 count = reader->Count();
        CPPUNIT_ASSERT(17565 == count);

        Ptr<MgPropertyCollection> keys = new MgPropertyCollection();
        Ptr<MgInt32Property> keyVal = new MgInt32Property(L"Autogenerated_SDF_ID", 4);
        Ptr<MgInt32Property> keyVal2 = new MgInt32Property(L"Autogenerated_SDF_ID", 99999);

        keys->Add(keyVal);

        bool bScroll = reader->ReadAt(keys);
        bool bScroll2 = reader->ReadAtIndex(7);
        bool bScroll3 = reader->ReadAtIndex(99999);

        keys->Clear();
        keys->Add(keyVal2);

        bool bScroll4 = true;
        try {
            bScroll4 = reader->ReadAt(keys);
        } catch (FdoException* ex) {
            //If it gets here, it is a defect in the provider as the spec says return false on non-existent record
            ex->Release();
            bScroll4 = false;
        }
        bool bScroll5 = reader->ReadAtIndex(0); //indexes are 1-based
        bool bScroll6 = reader->ReadAtIndex(1);
        bool bFirst = reader->ReadFirst();
        bool bBeforeFirst = reader->ReadPrevious();
        bool bLast = reader->ReadLast();
        bool bAfterLast = reader->ReadNext();
        reader->Close();

        CPPUNIT_ASSERT(bScroll);
        CPPUNIT_ASSERT(bScroll2);
        CPPUNIT_ASSERT(!bScroll3);
        CPPUNIT_ASSERT(!bScroll4);
        CPPUNIT_ASSERT(!bScroll5);
        CPPUNIT_ASSERT(bScroll6);
        CPPUNIT_ASSERT(bFirst);
        CPPUNIT_ASSERT(!bBeforeFirst);
        CPPUNIT_ASSERT(bLast);
        CPPUNIT_ASSERT(!bAfterLast);
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

void TestFeatureService::TestCase_BenchmarkSdfJoin()
{
    long total = 0L;
    try
    {
        //Setup our test data
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/ParcelsJoinTestSdf.FeatureSource");
        Ptr<MgByteSource> fsContent = new MgByteSource(L"../UnitTestFiles/UT_Parcels_Sdf_Join.fs");
        Ptr<MgByteReader> fsReader = fsContent->GetReader();

        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkSdfJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgFeatureService> featSvc = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (featSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkSdfJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        pService->SetResource(fsId, fsReader, NULL);
        Ptr<MgByteSource> fsData = new MgByteSource(L"../UnitTestFiles/ParcelsJoinTest.sdf");
        Ptr<MgByteReader> fsDataReader = fsData->GetReader();

        pService->SetResourceData(fsId, L"ParcelsJoinTest.sdf", L"File", fsDataReader);
        CPPUNIT_ASSERT(featSvc->TestConnection(fsId));

        Ptr<MgFeatureReader> reader;

        // ----- Start the tests ------- //
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSdfJoin() - Inner Join \n")));
        long lStart = GetTickCount();

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInner", NULL);
        while(reader->ReadNext())
        {
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSdfJoin() - Left Outer Join \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuter", NULL);
        while(reader->ReadNext())
        {
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSdfJoin() - Inner Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInnerOneToOne", NULL);
        while(reader->ReadNext())
        {
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSdfJoin() - Left Outer Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuterOneToOne", NULL);
        while(reader->ReadNext())
        {
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
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

void TestFeatureService::TestCase_BenchmarkSqliteJoin()
{
    try
    {
        //Setup our test data
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/ParcelsJoinTestSQLite.FeatureSource");
        Ptr<MgByteSource> fsContent = new MgByteSource(L"../UnitTestFiles/UT_Parcels_SQLite_Join.fs");
        Ptr<MgByteReader> fsReader = fsContent->GetReader();

        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkSqliteJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgFeatureService> featSvc = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (featSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkSqliteJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        pService->SetResource(fsId, fsReader, NULL);
        Ptr<MgByteSource> fsData = new MgByteSource(L"../UnitTestFiles/ParcelsJoinTest.sqlite");
        Ptr<MgByteReader> fsDataReader = fsData->GetReader();

        pService->SetResourceData(fsId, L"ParcelsJoinTest.sqlite", L"File", fsDataReader);
        CPPUNIT_ASSERT(featSvc->TestConnection(fsId));

        Ptr<MgFeatureReader> reader;

        // ----- Start the tests ------- //
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSqliteJoin() - Inner Join \n")));
        long lStart = GetTickCount();
        long total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInner", NULL);
        while(reader->ReadNext())
        {
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSqliteJoin() - Left Outer Join \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuter", NULL);
        while(reader->ReadNext())
        {
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSqliteJoin() - Inner Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInnerOneToOne", NULL);
        while(reader->ReadNext())
        {
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSqliteJoin() - Left Outer Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuterOneToOne", NULL);
        while(reader->ReadNext())
        {
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
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

void TestFeatureService::TestCase_BenchmarkSqliteAggregateJoin()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkSqliteAggregateJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgFeatureService> featSvc = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (featSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkSqliteAggregateJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/ParcelsJoinTestSQLite.FeatureSource");
        CPPUNIT_ASSERT(featSvc->TestConnection(fsId));
        Ptr<MgDataReader> reader;

        // ----- Start the tests ------- //
        Ptr<MgFeatureAggregateOptions> aggOpts = new MgFeatureAggregateOptions();
        aggOpts->AddComputedProperty(L"Extents", L"SpatialExtents(Geometry)");
        aggOpts->AddComputedProperty(L"TotalCount", L"Count(SdfId)");

        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSqliteAggregateJoin() - Inner Join \n")));
        long lStart = GetTickCount();
        long total = 0L;
        int iterations = 0;

        reader = featSvc->SelectAggregate(fsId, L"ParcelsInner", aggOpts);
        while(reader->ReadNext())
        {
            Ptr<MgByteReader> br = reader->GetGeometry(L"Extents");
            total = reader->GetInt64(L"TotalCount");
            iterations++;
        }
        reader->Close();
        CPPUNIT_ASSERT(iterations == 1);

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSqliteAggregateJoin() - Left Outer Join \n")));
        lStart = GetTickCount();
        iterations = 0L;

        reader = featSvc->SelectAggregate(fsId, L"ParcelsLeftOuter", aggOpts);
        while(reader->ReadNext())
        {
            Ptr<MgByteReader> br = reader->GetGeometry(L"Extents");
            total = reader->GetInt64(L"TotalCount");
            iterations++;
        }
        reader->Close();
        CPPUNIT_ASSERT(iterations == 1L);

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSqliteAggregateJoin() - Inner Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        iterations = 0L;

        reader = featSvc->SelectAggregate(fsId, L"ParcelsInnerOneToOne", aggOpts);
        while(reader->ReadNext())
        {
            Ptr<MgByteReader> br = reader->GetGeometry(L"Extents");
            total = reader->GetInt64(L"TotalCount");
            iterations++;
        }
        reader->Close();
        CPPUNIT_ASSERT(iterations == 1L);

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkSqliteAggregateJoin() - Left Outer Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        iterations = 0L;

        reader = featSvc->SelectAggregate(fsId, L"ParcelsLeftOuterOneToOne", aggOpts);
        while(reader->ReadNext())
        {
            Ptr<MgByteReader> br = reader->GetGeometry(L"Extents");
            total = reader->GetInt64(L"TotalCount");
            iterations++;
        }
        reader->Close();
        CPPUNIT_ASSERT(iterations == 1L);

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
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

void TestFeatureService::TestCase_BenchmarkMergeSortJoin()
{
    try
    {
        //Setup our test data
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkMergeSortJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgdFeatureService> featSvc = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (featSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkMergeSortJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/SortableLeft.FeatureSource");
        Ptr<MgFeatureReader> reader;

        // ----- Start the tests ------- //
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkMergeSortJoin() - Inner Join \n")));
        long lStart = GetTickCount();
        long total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInnerSortMerge", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        Ptr<MgClassDefinition> classDefinition;
        Ptr<MgResourceIdentifier> dumpFsId;
        Ptr<MgPropertyCollection> propVals;

        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsInnerSortMerge", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsInnerSortMerge", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkMergeSortJoin() - Left Outer Join \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuterSortMerge", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsLeftOuterSortMerge", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsLeftOuterSortMerge", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkMergeSortJoin() - Inner Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInnerOneToOneSortMerge", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsInnerOneToOneSortMerge", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsInnerOneToOneSortMerge", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkMergeSortJoin() - Left Outer Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuterOneToOneSortMerge", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsLeftOuterOneToOneSortMerge", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsLeftOuterOneToOneSortMerge", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
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

void TestFeatureService::TestCase_BenchmarkBatchSortedBlockJoin()
{
    try
    {
        //Setup our test data
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkBatchSortedBlockJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgdFeatureService> featSvc = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (featSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkBatchSortedBlockJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/UnsortableLeft.FeatureSource");
        Ptr<MgFeatureReader> reader;

        // ----- Start the tests ------- //
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkBatchSortedBlockJoin() - Inner Join \n")));
        long lStart = GetTickCount();
        long total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInnerBatchSortedBlock", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        Ptr<MgClassDefinition> classDefinition;
        Ptr<MgResourceIdentifier> dumpFsId;
        Ptr<MgPropertyCollection> propVals;

        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsInnerBatchSortedBlock", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsInnerBatchSortedBlock", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkBatchSortedBlockJoin() - Left Outer Join \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuterBatchSortedBlock", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsLeftOuterBatchSortedBlock", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsLeftOuterBatchSortedBlock", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkBatchSortedBlockJoin() - Inner Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInnerOneToOneBatchSortedBlock", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsInnerOneToOneBatchSortedBlock", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsInnerOneToOneBatchSortedBlock", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkBatchSortedBlockJoin() - Left Outer Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuterOneToOneBatchSortedBlock", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsLeftOuterOneToOneBatchSortedBlock", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsLeftOuterOneToOneBatchSortedBlock", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
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

void TestFeatureService::TestCase_BenchmarkNestedLoopsSortedBlockJoin()
{
    try
    {
        //Setup our test data
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkNestedLoopsSortedBlockJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgdFeatureService> featSvc = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (featSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkNestedLoopsSortedBlockJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/SortableLeft.FeatureSource");
        Ptr<MgFeatureReader> reader;

        // ----- Start the tests ------- //
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkNestedLoopsSortedBlockJoin() - Inner Join \n")));
        long lStart = GetTickCount();
        long total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInnerNestedLoopSortedBlock", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        Ptr<MgClassDefinition> classDefinition;
        Ptr<MgResourceIdentifier> dumpFsId;
        Ptr<MgPropertyCollection> propVals;

        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsInnerNestedLoopSortedBlock", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsInnerNestedLoopSortedBlock", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkNestedLoopsSortedBlockJoin() - Left Outer Join \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuterNestedLoopSortedBlock", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsLeftOuterNestedLoopSortedBlock", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsLeftOuterNestedLoopSortedBlock", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkNestedLoopsSortedBlockJoin() - Inner Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInnerOneToOneNestedLoopSortedBlock", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsInnerOneToOneNestedLoopSortedBlock", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsInnerOneToOneNestedLoopSortedBlock", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkNestedLoopsSortedBlockJoin() - Left Outer Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuterOneToOneNestedLoopSortedBlock", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsLeftOuterOneToOneNestedLoopSortedBlock", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsLeftOuterOneToOneNestedLoopSortedBlock", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
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

MgResourceIdentifier* TestFeatureService::CreateFeatureSource(MgFeatureService* featSvc, CREFSTRING name, MgClassDefinition* classDefinition)
{
    STRING fsIdStr = L"Library://UnitTests/Data/";
    fsIdStr += name;
    fsIdStr += L".FeatureSource";
    STRING fileNameStr = name;
    fileNameStr += L".sdf";
    Ptr<MgClassDefinition> cls = MgdFeatureUtil::CloneMgClassDefinition(classDefinition);
    cls->SetName(name);

    //Flip the autogeneration bit (just in case) 
    Ptr<MgPropertyDefinitionCollection> clsProps = cls->GetProperties();
    Ptr<MgDataPropertyDefinition> sdfId = dynamic_cast<MgDataPropertyDefinition*>(clsProps->GetItem(L"SdfId"));
    //sdfId->SetNullable(false);
    sdfId->SetAutoGeneration(true);
    sdfId->SetDataType(MgPropertyType::Int32);

    //Set all others to nullable (just in case)
    for (INT32 i = 0; i < clsProps->GetCount(); i++)
    {
        Ptr<MgPropertyDefinition> propDef = clsProps->GetItem(i);
        if (propDef->GetName() != L"SdfId")
        {
            if (propDef->GetPropertyType() == MgFeaturePropertyType::DataProperty)
            {
                MgDataPropertyDefinition* dataProp = ((MgDataPropertyDefinition*)propDef.p);
                dataProp->SetNullable(true);
                dataProp->SetAutoGeneration(false);
                if (dataProp->GetDataType() == MgPropertyType::String)
                    dataProp->SetLength(255);
            }
            else if (propDef->GetPropertyType() == MgFeaturePropertyType::GeometricProperty)
            {
                ((MgGeometricPropertyDefinition*)propDef.p)->SetSpatialContextAssociation(L"Default");
            }
        }
    }

    Ptr<MgFeatureSchema> schema = new MgFeatureSchema(L"Default", L"");
    Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();
    classes->Add(cls);

    Ptr<MgFileFeatureSourceParams> createParams = new MgFileFeatureSourceParams();
    createParams->SetFeatureSchema(schema);
    createParams->SetProviderName(L"OSGeo.SDF");
    createParams->SetFileName(fileNameStr);
    createParams->SetCoordinateSystemWkt(wkt);
    createParams->SetSpatialContextName(L"Default");
    Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(fsIdStr);
    featSvc->CreateFeatureSource(fsId, createParams);

    return fsId.Detach();
}

MgPropertyCollection* TestFeatureService::Prepare(MgClassDefinition* cls)
{
    Ptr<MgPropertyCollection> propVals = new MgPropertyCollection();
    Ptr<MgPropertyDefinitionCollection> clsProps = cls->GetProperties();
    for (INT32 i = 0; i < clsProps->GetCount(); i++)
    {
        Ptr<MgPropertyDefinition> clsProp = clsProps->GetItem(i);
        STRING name = clsProp->GetName();
        if (clsProp->GetPropertyType() == MgFeaturePropertyType::GeometricProperty)
        {
            Ptr<MgGeometryProperty> geomVal = new MgGeometryProperty(name, NULL);
            geomVal->SetNull(true);
            propVals->Add(geomVal);
        }
        else if (clsProp->GetPropertyType() == MgFeaturePropertyType::DataProperty)
        {
            MgDataPropertyDefinition* dataProp = static_cast<MgDataPropertyDefinition*>(clsProp.p);
            
            Ptr<MgNullableProperty> dataVal;
            switch(dataProp->GetDataType())
            {
            case MgPropertyType::Blob:
                dataVal = new MgBlobProperty(name, NULL);
                break;
            case MgPropertyType::Boolean:
                dataVal = new MgBooleanProperty(name, false);
                break;
            case MgPropertyType::Byte:
                dataVal = new MgByteProperty(name, 0);
                break;
            case MgPropertyType::Clob:
                dataVal = new MgClobProperty(name, NULL);
                break;
            case MgPropertyType::DateTime:
                dataVal = new MgDateTimeProperty(name, NULL);
                break;
            case MgPropertyType::Decimal:
            case MgPropertyType::Double:
                dataVal = new MgDoubleProperty(name, 0.0);
                break;
            case MgPropertyType::Int16:
                dataVal = new MgInt16Property(name, 0);
                break;
            case MgPropertyType::Int32:
                dataVal = new MgInt32Property(name, 0);
                break;
            case MgPropertyType::Int64:
                dataVal = new MgInt64Property(name, 0);
                break;
            case MgPropertyType::Single:
                dataVal = new MgSingleProperty(name, 0.0f);
                break;
            case MgPropertyType::String:
                dataVal = new MgStringProperty(name, L"");
                break;
            }
            dataVal->SetNull(true);
            propVals->Add(dataVal);
        }
    }
    return propVals.Detach();
}

void TestFeatureService::Insert(MgdFeatureService* featSvc, MgReader* reader, MgResourceIdentifier* fsId, CREFSTRING className, MgPropertyCollection* propVals)
{
    for (INT32 i = 0; i < propVals->GetCount(); i++)
    {
        Ptr<MgProperty> prop = propVals->GetItem(i);
        STRING propName = prop->GetName();
        if (!reader->IsNull(propName))
        {
            INT32 ptype = prop->GetPropertyType();
            switch(ptype)
            {
            case MgPropertyType::Blob:
                {
                    MgBlobProperty* value = static_cast<MgBlobProperty*>(prop.p);
                    Ptr<MgByteReader> br = reader->GetBLOB(propName);
                    value->SetNull(false);
                    value->SetValue(br);
                }
                break;
            case MgPropertyType::Boolean:
                {
                    MgBooleanProperty* value = static_cast<MgBooleanProperty*>(prop.p);
                    value->SetNull(false);
                    value->SetValue(reader->GetBoolean(propName));
                }
                break;
            case MgPropertyType::Byte:
                {
                    MgByteProperty* value = static_cast<MgByteProperty*>(prop.p);
                    value->SetNull(false);
                    value->SetValue(reader->GetByte(propName));
                }
                break;
            case MgPropertyType::Clob:
                {
                    MgClobProperty* value = static_cast<MgClobProperty*>(prop.p);
                    Ptr<MgByteReader> br = reader->GetCLOB(propName);
                    value->SetNull(false);
                    value->SetValue(br);
                }
                break;
            case MgPropertyType::DateTime:
                {
                    MgDateTimeProperty* value = static_cast<MgDateTimeProperty*>(prop.p);
                    Ptr<MgDateTime> dt = reader->GetDateTime(propName);
                    value->SetNull(false);
                    value->SetValue(dt);
                }
                break;
            case MgPropertyType::Decimal:
            case MgPropertyType::Double:
                {
                    MgDoubleProperty* value = static_cast<MgDoubleProperty*>(prop.p);
                    value->SetNull(false);
                    value->SetValue(reader->GetDouble(propName));
                }
                break;
            case MgPropertyType::Geometry:
                {
                    MgGeometryProperty* value = static_cast<MgGeometryProperty*>(prop.p);
                    Ptr<MgByteReader> agf = reader->GetGeometry(propName);
                    value->SetNull(false);
                    value->SetValue(agf);
                }
                break;
            case MgPropertyType::Int16:
                {
                    MgInt16Property* value = static_cast<MgInt16Property*>(prop.p);
                    value->SetNull(false);
                    value->SetValue(reader->GetInt16(propName));
                }
                break;
            case MgPropertyType::Int32:
                {
                    MgInt32Property* value = static_cast<MgInt32Property*>(prop.p);
                    value->SetNull(false);
                    value->SetValue(reader->GetInt32(propName));
                }
                break;
            case MgPropertyType::Int64:
                {
                    MgInt64Property* value = static_cast<MgInt64Property*>(prop.p);
                    value->SetNull(false);
                    value->SetValue(reader->GetInt64(propName));
                }
                break;
            case MgPropertyType::Single:
                {
                    MgSingleProperty* value = static_cast<MgSingleProperty*>(prop.p);
                    value->SetNull(false);
                    value->SetValue(reader->GetSingle(propName));
                }
                break;
            case MgPropertyType::String:
                {
                    MgStringProperty* value = static_cast<MgStringProperty*>(prop.p);
                    STRING str = reader->GetString(propName);
                    value->SetNull(false);
                    value->SetValue(str);
                }
                break;
            }
        }
        else 
        {
            MgNullableProperty* nprop = dynamic_cast<MgNullableProperty*>(prop.p);
            if (NULL != nprop)
                nprop->SetNull(true);
        }
    }
    MgFeatureReader* fr = featSvc->InsertFeatures(fsId, className, propVals);
    fr->Close();
}

void TestFeatureService::TestCase_BenchmarkNestedLoopsJoin()
{
    try
    {
        //Setup our test data
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkNestedLoopsJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        Ptr<MgdFeatureService> featSvc = dynamic_cast<MgdFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (featSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService::TestCase_BenchmarkNestedLoopsJoin",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        
        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/UnsortableLeft.FeatureSource");
        Ptr<MgFeatureReader> reader;

        // ----- Start the tests ------- //
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkNestedLoopsJoin() - Inner Join \n")));
        long lStart = GetTickCount();
        long total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInnerNestedLoops", NULL);

#ifdef DUMP_BENCHMARK_RESULTS
        Ptr<MgClassDefinition> classDefinition;
        Ptr<MgResourceIdentifier> dumpFsId;
        Ptr<MgPropertyCollection> propVals;

        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsInnerNestedLoops", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsInnerNestedLoops", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkNestedLoopsJoin() - Left Outer Join \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuterNestedLoops", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsLeftOuterNestedLoops", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsLeftOuterNestedLoops", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkNestedLoopsJoin() - Inner Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsInnerOneToOneNestedLoops", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsInnerOneToOneNestedLoops", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsInnerOneToOneNestedLoops", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestFeatureService::TestCase_BenchmarkNestedLoopsJoin() - Left Outer Join (Forced 1:1) \n")));
        lStart = GetTickCount();
        total = 0L;

        reader = featSvc->SelectFeatures(fsId, L"ParcelsLeftOuterOneToOneNestedLoops", NULL);
#ifdef DUMP_BENCHMARK_RESULTS
        classDefinition = reader->GetClassDefinition();
        dumpFsId = CreateFeatureSource(featSvc, L"ParcelsLeftOuterOneToOneNestedLoops", classDefinition);
        propVals = Prepare(classDefinition);
#endif
        while(reader->ReadNext())
        {
#ifdef DUMP_BENCHMARK_RESULTS
            Insert(featSvc, reader, dumpFsId, L"ParcelsLeftOuterOneToOneNestedLoops", propVals);
#endif
            total++;
        }
        reader->Close();

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (%d results): = %6.4f (s)\n"), total, ((GetTickCount()-lStart)/1000.0) ));
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
/// This test case tests the correct response for a non-transactional failure
/// in UpdateFeatures
///----------------------------------------------------------------------------
void TestFeatureService::TestCase_UpdateFeaturesPartialFailure()
{
    try
    {
        Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
        if (fact == 0)
        {
            throw new MgNullReferenceException(L"TestFeatureService.TestCase_UpdateFeaturesPartialFailure", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> featSvc = dynamic_cast<MgFeatureService*>(fact->CreateService(MgServiceType::FeatureService));
        if (featSvc == 0)
        {
            throw new MgServiceNotAvailableException(L"TestFeatureService.TestCase_UpdateFeaturesPartialFailure",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> fsId = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        Ptr<MgPropertyCollection> props = new MgPropertyCollection();
        Ptr<MgInt32Property> idProp = new MgInt32Property(L"id", 0);
        props->Add(idProp);
        Ptr<MgInsertFeatures> insert = new MgInsertFeatures(L"Parcels1", props); //Bogus class name to trigger exception
        Ptr<MgFeatureCommandCollection> cmds = new MgFeatureCommandCollection();
        cmds->Add(insert);

        Ptr<MgPropertyCollection> result = featSvc->UpdateFeatures(fsId, cmds, false);
        CPPUNIT_ASSERT(result->GetCount() > 0);

        bool bPartialFailure = false;
        for (INT32 i = 0; i < result->GetCount(); i++)
        {
            Ptr<MgProperty> prop = result->GetItem(i);
            if (prop->GetPropertyType() == MgPropertyType::String)
            {
                bPartialFailure = true;
                break;
            }
        }

        CPPUNIT_ASSERT(bPartialFailure);
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