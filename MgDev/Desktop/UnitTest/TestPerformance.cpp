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
#include "TestPerformance.h"
#include "CppUnitExtensions.h"
#include "ServiceManager.h"
#include "FdoConnectionManager.h"
#include "Services/FeatureService.h"
#include "ServerFeatureService.h"
#include "Fdo.h"

// **************************************** PLEASE READ ****************************************
// Note: The resources used here require that the following packages are installed:
//       1) PerformanceTest.mgp
//       2) JoinTest.mgp

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestPerformance, "TestPerformance");


void TestPerformance::setUp()
{
}


void TestPerformance::tearDown()
{
}


void TestPerformance::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Performance tests.\n")));

    try
    {
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
            throw new MgNullReferenceException(L"TestPerformance.TestStart", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestPerformance.TestStart", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        //Set the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        if (userInfo != NULL)
        {
            userInfo->SetLocale(TEST_LOCALE);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            MgResourceIdentifier resourceIdentifier1(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");

#ifdef _WIN32
            STRING resourceContentFileName1 = L"..\\UnitTestFiles\\Sheboygan_Parcels.FeatureSource";
            STRING dataFileName1 = L"..\\UnitTestFiles\\Sheboygan_Parcels.sdf";
#else
            STRING resourceContentFileName1 = L"../UnitTestFiles/Sheboygan_Parcels.FeatureSource";
            STRING dataFileName1 = L"../UnitTestFiles/Sheboygan_Parcels.sdf";
#endif

            //Add a new resource
            Ptr<MgByteSource> contentSource1 = new MgByteSource(resourceContentFileName1);
            Ptr<MgByteReader> contentReader1 = contentSource1->GetReader();
            pService->SetResource(&resourceIdentifier1, contentReader1, NULL);

            //Set the resource data
            Ptr<MgByteSource> dataSource1 = new MgByteSource(dataFileName1);
            Ptr<MgByteReader> dataReader1 = dataSource1->GetReader();
            pService->SetResourceData(&resourceIdentifier1, L"Sheboygan_Parcels.sdf", L"File", dataReader1);

        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestPerformance::TestEnd()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestPerformance.TestEnd",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceService> pService = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestPerformance.TestEnd",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        // delete the feature sources definition
        Ptr<MgResourceIdentifier> mapres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
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

    ACE_DEBUG((LM_INFO, ACE_TEXT("\nFeature Service tests completed.\n\n")));
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case benchmarks selecting features.
///----------------------------------------------------------------------------
void TestPerformance::TestCase_BenchmarkSelectFeatures()
{
    try
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestCase_BenchmarkSelectFeatures - START\n")));
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestPerformance.TestCase_BenchmarkSelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestPerformance.TestCase_BenchmarkSelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> resource = new MgResourceIdentifier();
        STRING className = L"Parcels";
        resource = new MgResourceIdentifier(L"Library://UnitTests/Data/Sheboygan_Parcels.FeatureSource");
        Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();

        const int iterations = 1000;
        long lStart = GetTickCount();
        for(int i=0;i<iterations;i++)
        {
            int nFeatures = 0;
            Ptr<MgFeatureReader> reader = pService->SelectFeatures(resource, className, options);
            while(reader->ReadNext())
            {
                nFeatures++;
            }

            reader->Close();
            CPPUNIT_ASSERT(nFeatures == 17565);
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("  Execution Time (Average of %d runs): = %6.4f (s)\n"), iterations, ((GetTickCount()-lStart)/1000.0)/(double)iterations ));
        ACE_DEBUG((LM_INFO, ACE_TEXT("TestCase_BenchmarkSelectFeatures - END\n")));
    }
    catch(MgResourceNotFoundException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n%W\nAre the performance test resources installed?\n\n"), message.c_str()));
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
/// This test case benchmarks joining features.
///----------------------------------------------------------------------------
void TestPerformance::TestCase_BenchmarkJoinFeatures()
{
    try
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestCase_BenchmarkJoinFeatures - START\n\n")));
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        if(serviceManager == 0)
        {
            throw new MgNullReferenceException(L"TestPerformance.TestCase_BenchmarkJoinFeatures",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgFeatureService> pService = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestPerformance.TestCase_BenchmarkJoinFeatures",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgResourceIdentifier> lFeatureSource = new MgResourceIdentifier(L"Library://JoinTest/Data/World_Countries_Join.FeatureSource");

        int nFeatures = 0;
        #ifdef _DEBUG
        const int iterations = 1;
        #else
        const int iterations = 10;
        #endif
        long lStart = GetTickCount();

        ACE_DEBUG((LM_INFO, ACE_TEXT("1) NO JOIN (World_Countries)\n")));
        for(int i=0;i<iterations;i++)
        {
            nFeatures = 0;
            Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"World_Countries", NULL);
            while(reader->ReadNext())
            {
                STRING s1 = L"<null>";
                if(!reader->IsNull(L"Key"))
                {
                    s1 = reader->GetString(L"Key");
                }

                STRING s2 = L"<null>";
                if(!reader->IsNull(L"Name"))
                {
                    s2 = reader->GetString(L"Name");
                }
                nFeatures++;

                #ifdef _DEBUG
                printf("  %d: %S:%S\n", nFeatures, s1.c_str(), s2.c_str());

                if(nFeatures == 10)
                    break;
                #endif
            }

            reader->Close();
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("NO JOIN (World_Countries:%d) - Avg of %d runs = %6.4f (s)\n\n"), nFeatures, iterations, ((GetTickCount()-lStart)/1000.0)/(double)iterations ));

        // Reset clock
        lStart = GetTickCount();
        ACE_DEBUG((LM_INFO, ACE_TEXT("2) JOIN (World_Countries) - JOIN:KEY + COUNTRY_KEY\n")));
        for(int i=0;i<iterations;i++)
        {
            nFeatures = 0;
            Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"Join", NULL);
            while(reader->ReadNext())
            {
                STRING s1 = L"<null>";
                if(!reader->IsNull(L"Key"))
                {
                    s1 = reader->GetString(L"Key");
                }

                STRING s2 = L"<null>";
                if(!reader->IsNull(L"Name"))
                {
                    s2 = reader->GetString(L"Name");
                }

                STRING s3 = L"<null>";
                if(!reader->IsNull(L"DataCOUNTRY_KEY"))
                {
                    s3 = reader->GetString(L"DataCOUNTRY_KEY");
                }

                STRING s4 = L"<null>";
                if(!reader->IsNull(L"DataCOUNTRY_NAME"))
                {
                    s4 = reader->GetString(L"DataCOUNTRY_NAME");
                }

                nFeatures++;

                #ifdef _DEBUG
                printf("  %d: %S:%S - %S:%S\n", nFeatures, s1.c_str(), s2.c_str(), s3.c_str(), s4.c_str());

                if(nFeatures == 10)
                    break;
                #endif
            }

            reader->Close();
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("JOIN (World_Countries:%d) - Avg of %d runs = %6.4f (s)\n\n"), nFeatures, iterations, ((GetTickCount()-lStart)/1000.0)/(double)iterations ));

        lFeatureSource = new MgResourceIdentifier(L"Library://JoinTest/Data/World_Countries_Join2.FeatureSource");

        // Reset clock
        lStart = GetTickCount();
        ACE_DEBUG((LM_INFO, ACE_TEXT("3) NO JOIN (World_Countries - Mixed)\n")));
        for(int i=0;i<iterations;i++)
        {
            nFeatures = 0;
            Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"World_Countries", NULL);
            while(reader->ReadNext())
            {
                INT32 n1 = 0;
                if(!reader->IsNull(L"Autogenerated_SDF_ID"))
                {
                    n1 = reader->GetInt32(L"Autogenerated_SDF_ID");
                }

                STRING s2 = L"<null>";
                if(!reader->IsNull(L"Key"))
                {
                    s2 = reader->GetString(L"Key");
                }
                nFeatures++;

                #ifdef _DEBUG
                printf("  %d: %d:%S\n", nFeatures, n1, s2.c_str());

                if(nFeatures == 10)
                    break;
                #endif
            }

            reader->Close();
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("NO JOIN (World_Countries - Mixed:%d) - Avg of %d runs = %6.4f (s)\n\n"), nFeatures, iterations, ((GetTickCount()-lStart)/1000.0)/(double)iterations ));

        // Reset clock
        lStart = GetTickCount();
        ACE_DEBUG((LM_INFO, ACE_TEXT("4) JOIN (World_Countries - Mixed) - JOIN:Autogenerated_SDF_ID + FEATID\n")));
        for(int i=0;i<iterations;i++)
        {
            nFeatures = 0;
            Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"Join", NULL);
            while(reader->ReadNext())
            {
                INT32 n1 = 0;
                if(!reader->IsNull(L"Autogenerated_SDF_ID"))
                {
                    n1 = reader->GetInt32(L"Autogenerated_SDF_ID");
                }

                STRING s2 = L"<null>";
                if(!reader->IsNull(L"Key"))
                {
                    s2 = reader->GetString(L"Key");
                }

                STRING s3 = L"<null>";
                if(!reader->IsNull(L"DataFEATID"))
                {
                    s3 = reader->GetString(L"DataFEATID");
                }

                STRING s4 = L"<null>";
                if(!reader->IsNull(L"DataCOUNTRY_KEY"))
                {
                    s4 = reader->GetString(L"DataCOUNTRY_KEY");
                }

                nFeatures++;

                #ifdef _DEBUG
                printf("  %d: %d:%S - %S:%S\n", nFeatures, n1, s2.c_str(), s3.c_str(), s4.c_str());

                if(nFeatures == 10)
                    break;
                #endif
            }

            reader->Close();
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("JOIN (World_Countries - Mixed:%d) - Avg of %d runs = %6.4f (s)\n\n"), nFeatures, iterations, ((GetTickCount()-lStart)/1000.0)/(double)iterations ));

        // New feature source
        lFeatureSource = new MgResourceIdentifier(L"Library://JoinTest/Data/geonames_n83_join.FeatureSource");

        lStart = GetTickCount();
        ACE_DEBUG((LM_INFO, ACE_TEXT("5) NO JOIN (geonames_n83)\n")));
        for(int i=0;i<iterations;i++)
        {
            nFeatures = 0;
            Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"geonames_n83", NULL);
            while(reader->ReadNext())
            {
                INT32 fid = 0;
                if(!reader->IsNull(L"FeatId"))
                {
                    fid = reader->GetInt32(L"FeatId");
                }

                STRING s1 = L"<null>";
                if(!reader->IsNull(L"FEATURE_NA"))
                {
                    s1 = reader->GetString(L"FEATURE_NA");
                }
                nFeatures++;

                #ifdef _DEBUG
                printf("  %d: FID:%d - %S\n", nFeatures, fid, s1.c_str());

                if(nFeatures == 10)
                    break;
                #endif
            }

            reader->Close();
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("NO JOIN (geonames_n83:%d) - Avg of %d runs = %6.4f (s)\n\n"), nFeatures, iterations, ((GetTickCount()-lStart)/1000.0)/(double)iterations ));

        // New feature source
        lFeatureSource = new MgResourceIdentifier(L"Library://JoinTest/Data/Oracle_ADSKTUX2.FeatureSource");

        // Reset clock
        lStart = GetTickCount();
        ACE_DEBUG((LM_INFO, ACE_TEXT("6) NO JOIN (Oracle_ADSKTUX2)\n")));
        for(int i=0;i<iterations;i++)
        {
            nFeatures = 0;
            Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"GEONAMES_N83", NULL);
            while(reader->ReadNext())
            {
                INT32 fid = 0;
                if(!reader->IsNull(L"FEATID"))
                {
                    fid = reader->GetInt32(L"FEATID");
                }

                STRING s1 = L"<null>";
                if(!reader->IsNull(L"FEATURE_NA"))
                {
                    s1 = reader->GetString(L"FEATURE_NA");
                }
                nFeatures++;

                #ifdef _DEBUG
                printf("  %d: FID:%d - %S\n", nFeatures, fid, s1.c_str());

                if(nFeatures == 10)
                    break;
                #endif
            }

            reader->Close();
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("NO JOIN (Oracle_ADSKTUX2:%d) - Avg of %d runs = %6.4f (s)\n\n"), nFeatures, iterations, ((GetTickCount()-lStart)/1000.0)/(double)iterations ));

        // New feature source
        lFeatureSource = new MgResourceIdentifier(L"Library://JoinTest/Data/geonames_n83_join.FeatureSource");

        // Reset clock
        lStart = GetTickCount();
        ACE_DEBUG((LM_INFO, ACE_TEXT("7) JOIN (geonames_n83) - JOIN:FeatId + FEATID\n")));
        for(int i=0;i<iterations;i++)
        {
            nFeatures = 0;
            Ptr<MgFeatureReader> reader = pService->SelectFeatures(lFeatureSource, L"Join", NULL);
            while(reader->ReadNext())
            {
                INT32 fid = 0;
                if(!reader->IsNull(L"FeatId"))
                {
                    fid = reader->GetInt32(L"FeatId");
                }

                STRING s1 = L"<null>";
                if(!reader->IsNull(L"FEATURE_NA"))
                {
                    s1 = reader->GetString(L"FEATURE_NA");
                }

                INT32 sfid = 0;
                if(!reader->IsNull(L"DataFEATID"))
                {
                    sfid = reader->GetInt32(L"DataFEATID");
                }

                STRING s2 = L"<null>";
                if(!reader->IsNull(L"DataFEATURE_NA"))
                {
                    s2 = reader->GetString(L"DataFEATURE_NA");
                }
                nFeatures++;

                #ifdef _DEBUG
                printf("  %d: FID:%d - %S  FID:%d - %S\n", nFeatures, fid, s1.c_str(), sfid, s2.c_str());

                if(nFeatures == 10)
                    break;
                #endif
            }

            reader->Close();
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("JOIN (geonames_n83:%d) - Avg of %d runs = %6.4f (s)\n\n"), nFeatures, iterations, ((GetTickCount()-lStart)/1000.0)/(double)iterations ));

        ACE_DEBUG((LM_INFO, ACE_TEXT("TestCase_BenchmarkJoinFeatures - END\n")));
    }
    catch(MgResourceNotFoundException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n%W\nAre the performance test resources installed?\n\n"), message.c_str()));
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
