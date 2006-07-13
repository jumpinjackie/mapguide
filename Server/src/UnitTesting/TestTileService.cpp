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
#include "TestTileService.h"
#include "../UnitTesting/CppUnitExtensions.h"
#include "ServiceManager.h"
#include "../Common/Manager/FdoConnectionManager.h"

#define PATH_LEN 512

// determines the number of requests to make, as a factor of the number of tiles
#define REQUEST_FACTOR 20

#ifdef _WIN32
    // TODO: Investigate threading problem on Windows 2003 (dual core CPU).
    //       There may be some issue with ByteSourceFileImpl::LoadFile (i.e. Is errno thread safe?).
    static const INT32 MG_TEST_THREADS = 1;
#else
    static const INT32 MG_TEST_THREADS = 4;
#endif

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestTileService, "TestTileService");


TestTileService::TestTileService()
{
    // store references to the various services we use
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcResource = dynamic_cast<MgResourceService*>(serviceMan->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcTile = dynamic_cast<MgTileService*>(serviceMan->RequestService(MgServiceType::TileService));
    assert(m_svcTile != NULL);
}


TestTileService::~TestTileService()
{
}


void TestTileService::setUp()
{
}


void TestTileService::tearDown()
{
}


void TestTileService::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Tile Service tests.\n")));

    try
    {
        #ifdef _DEBUG
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif

        // set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        // ------------------------------------------------------
        // base map source data
        // ------------------------------------------------------

        // publish the map definition
        Ptr<MgResourceIdentifier> mapres1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/BaseMap.MapDefinition");
        Ptr<MgByteSource> mdfsrc1 = new MgByteSource(L"../UnitTestFiles/UT_BaseMap.mdf", false);
        Ptr<MgByteReader> mdfrdr1 = mdfsrc1->GetReader();
        m_svcResource->SetResource(mapres1, mdfrdr1, NULL);

        // publish the layer definitions
        Ptr<MgResourceIdentifier> ldfres1 = new MgResourceIdentifier(L"Library://UnitTests/Layers/RoadCenterLines.LayerDefinition");
        Ptr<MgByteSource> ldfsrc1 = new MgByteSource(L"../UnitTestFiles/UT_RoadCenterLines.ldf", false);
        Ptr<MgByteReader> ldfrdr1 = ldfsrc1->GetReader();
        m_svcResource->SetResource(ldfres1, ldfrdr1, NULL);

        Ptr<MgResourceIdentifier> ldfres2 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        Ptr<MgByteSource> ldfsrc2 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.ldf", false);
        Ptr<MgByteReader> ldfrdr2 = ldfsrc2->GetReader();
        m_svcResource->SetResource(ldfres2, ldfrdr2, NULL);

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/Layers/VotingDistricts.LayerDefinition");
        Ptr<MgByteSource> ldfsrc3 = new MgByteSource(L"../UnitTestFiles/UT_VotingDistricts.ldf", false);
        Ptr<MgByteReader> ldfrdr3 = ldfsrc3->GetReader();
        m_svcResource->SetResource(ldfres3, ldfrdr3, NULL);

        // publish the feature sources
        Ptr<MgResourceIdentifier> fsres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/RoadCenterLines.FeatureSource");
        Ptr<MgByteSource> fssrc1 = new MgByteSource(L"../UnitTestFiles/UT_RoadCenterLines.fs", false);
        Ptr<MgByteReader> fsrdr1 = fssrc1->GetReader();
        m_svcResource->SetResource(fsres1, fsrdr1, NULL);

        Ptr<MgResourceIdentifier> fsres2 = new MgResourceIdentifier(L"Library://UnitTests/Data/Parcels.FeatureSource");
        Ptr<MgByteSource> fssrc2 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.fs", false);
        Ptr<MgByteReader> fsrdr2 = fssrc2->GetReader();
        m_svcResource->SetResource(fsres2, fsrdr2, NULL);

        Ptr<MgResourceIdentifier> fsres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/VotingDistricts.FeatureSource");
        Ptr<MgByteSource> fssrc3 = new MgByteSource(L"../UnitTestFiles/UT_VotingDistricts.fs", false);
        Ptr<MgByteReader> fsrdr3 = fssrc3->GetReader();
        m_svcResource->SetResource(fsres3, fsrdr3, NULL);

        // publish the resource data
        Ptr<MgByteSource> dataSource1 = new MgByteSource(L"../UnitTestFiles/UT_RoadCenterLines.sdf", false);
        Ptr<MgByteReader> dataReader1 = dataSource1->GetReader();
        m_svcResource->SetResourceData(fsres1, L"UT_RoadCenterLines.sdf", L"File", dataReader1);

        Ptr<MgByteSource> dataSource2 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.sdf", false);
        Ptr<MgByteReader> dataReader2 = dataSource2->GetReader();
        m_svcResource->SetResourceData(fsres2, L"UT_Parcels.sdf", L"File", dataReader2);

        Ptr<MgByteSource> dataSource3 = new MgByteSource(L"../UnitTestFiles/UT_VotingDistricts.sdf", false);
        Ptr<MgByteReader> dataReader3 = dataSource3->GetReader();
        m_svcResource->SetResourceData(fsres3, L"UT_VotingDistricts.sdf", L"File", dataReader3);

        // set up temporary directory for tile images
        MgFileUtil::CreateDirectory(L"./temp_tiles", false);
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


void TestTileService::TestEnd()
{
    try
    {
        // set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        // ------------------------------------------------------
        // base map source data
        // ------------------------------------------------------

        // delete the map definition
        Ptr<MgResourceIdentifier> mapres1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/BaseMap.MapDefinition");
        m_svcResource->DeleteResource(mapres1);

        // delete the layer definitions
        Ptr<MgResourceIdentifier> ldfres1 = new MgResourceIdentifier(L"Library://UnitTests/Layers/RoadCenterLines.LayerDefinition");
        m_svcResource->DeleteResource(ldfres1);

        Ptr<MgResourceIdentifier> ldfres2 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        m_svcResource->DeleteResource(ldfres2);

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/Layers/VotingDistricts.LayerDefinition");
        m_svcResource->DeleteResource(ldfres3);

        // delete the feature sources
        Ptr<MgResourceIdentifier> fsres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/RoadCenterLines.FeatureSource");
        m_svcResource->DeleteResource(fsres1);

        Ptr<MgResourceIdentifier> fsres2 = new MgResourceIdentifier(L"Library://UnitTests/Data/Parcels.FeatureSource");
        m_svcResource->DeleteResource(fsres2);

        Ptr<MgResourceIdentifier> fsres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/VotingDistricts.FeatureSource");
        m_svcResource->DeleteResource(fsres3);

        // remove temporary directory for tile images
        MgFileUtil::DeleteDirectory(L"./temp_tiles", true, false);

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

    ACE_DEBUG((LM_INFO, ACE_TEXT("\nTile Service tests completed.\n\n")));
}


// data structure which is passed to each thread
struct ThreadData
{
    INT32 threadId;
    bool done;
    bool saveTile;
    Ptr<MgMap> map;
    INT32 tileRow;
    INT32 tileCol;
};


////////////////////////////////////////////////////////////////
/// GetTile methods
////////////////////////////////////////////////////////////////


// the method which gets executed by the ACE worker thread
ACE_THR_FUNC_RETURN GetTileWorker(void* param)
{
    // get the data for this thread
    ThreadData* threadData = (ThreadData*)param;
    INT32 threadId = threadData->threadId;
    INT32 tileRow  = threadData->tileRow;
    INT32 tileCol  = threadData->tileCol;
    bool saveTile  = threadData->saveTile;
    Ptr<MgMap> map = threadData->map;
    #ifdef _DEBUG
    printf("> thread %d started, tile %d,%d\n", threadId, tileRow, tileCol);
    #endif

    try
    {
        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        // get the tile service instance
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        Ptr<MgTileService> svcTile = dynamic_cast<MgTileService*>(
            serviceManager->RequestService(MgServiceType::TileService));
        assert(svcTile != NULL);

        // get the tile
        Ptr<MgByteReader> img = svcTile->GetTile(map, L"BaseLayers", tileCol, tileRow);

        // save the image to a file if necessary
        if (saveTile)
        {
            wchar_t imgName[PATH_LEN] = { 0 };
            swprintf(imgName, PATH_LEN, L"./temp_tiles/tile%d_%d.png", tileRow, tileCol);
            (MgByteSink(img)).ToFile(imgName);
        }

        // clear the user info to prevent leaks
        MgUserInformation::SetCurrentUserInfo(NULL);
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

    #ifdef _DEBUG
//  printf("> thread %d done\n", threadId);
    #endif

    threadData->done = true;
    return 0;
}


void TestTileService::TestCase_GetTile()
{
    // specify the number of threads to use
    const INT32 numThreads = MG_TEST_THREADS;
    ThreadData threadData[numThreads];

    // define the range of tiles to get
    INT32 tileRowMin =  0;
    INT32 tileRowMax = 12;
    INT32 tileColMin =  3;
    INT32 tileColMax = 11;

    try
    {
        // need a thread manager
        ACE_Thread_Manager* manager = ACE_Thread_Manager::instance();

        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // make the runtime map
        Ptr<MgMap> map = CreateMap();

        // set up the tile indices
        INT32 numTileRows = tileRowMax - tileRowMin + 1;
        INT32 numTileCols = tileColMax - tileColMin + 1;
        INT32 numTiles    = numTileRows * numTileCols;

        INT32* tileRows = new INT32[numTiles];
        INT32* tileCols = new INT32[numTiles];

        INT32 nRequest = 0;
        for (INT32 tileRow = tileRowMin; tileRow <= tileRowMax; ++tileRow)
        {
            for (INT32 tileCol = tileColMin; tileCol <= tileColMax; ++tileCol)
            {
                tileRows[nRequest] = tileRow;
                tileCols[nRequest] = tileCol;
                nRequest++;
            }
        }

        // initialize the thread data
        for (INT32 i=0; i<numThreads; i++)
        {
            // each thread works with its own instance of the map
            Ptr<MgMemoryStreamHelper> helper = new MgMemoryStreamHelper();
            Ptr<MgStream> stream = new MgStream(helper);
            Ptr<MgMap> newMap = new MgMap();
            map->Serialize(stream);
            newMap->Deserialize(stream);

            threadData[i].threadId = i;
            threadData[i].done     = true;
            threadData[i].saveTile = false;
            threadData[i].map      = newMap;
            threadData[i].tileRow  = 0;
            threadData[i].tileCol  = 0;
        }

        // execute the requests to randomly access the tiles
        #ifdef _DEBUG
        printf("\n");
        #endif
        nRequest = 0;
        for (;;)
        {
            INT32 dc = 0;
            for (INT32 i=0; i<numThreads; i++)
            {
                // check if the thread is available
                if (threadData[i].done)
                {
                    // if we're not yet done then give the thread a new request
                    if (nRequest < REQUEST_FACTOR*numTiles)
                    {
                        // pick a random request to execute...
                        INT32 nTile = Rand(numTiles);

                        // ... but make every REQUEST_FACTOR-th tile non-random to
                        // ensure we get each tile at least once
                        if (nRequest % REQUEST_FACTOR == 0)
                            nTile = nRequest / REQUEST_FACTOR;

                        threadData[i].done     = false;
                        threadData[i].saveTile = (nRequest % REQUEST_FACTOR == 0);
                        threadData[i].tileRow  = tileRows[nTile];
                        threadData[i].tileCol  = tileCols[nTile];

                        // spawn a new thread
                        int thid = manager->spawn(ACE_THR_FUNC(GetTileWorker), &threadData[i]);
                        nRequest++;
                    }

                    // keep a tally of all the done threads
                    if (threadData[i].done)
                        ++dc;
                }
            }

            // move on if all threads are done
            if (dc == numThreads)
                break;

            // under Linux we get a deadlock if we don't call this every once in a while
            if (nRequest % 25 == 0)
                manager->wait();
            else
            {
                // pause briefly (10ms) before checking again
                ACE_Time_Value t(0, 10000);
                ACE_OS::sleep(t);
            }
        }

        // make sure the manager is in a good state
        manager->wait();

        // done with the tile indices
        delete tileRows;
        delete tileCols;
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


////////////////////////////////////////////////////////////////
/// SetTile methods
////////////////////////////////////////////////////////////////


// the method which gets executed by the ACE worker thread
ACE_THR_FUNC_RETURN SetTileWorker(void* param)
{
    // get the data for this thread
    ThreadData* threadData = (ThreadData*)param;
    INT32 threadId = threadData->threadId;
    INT32 tileRow  = threadData->tileRow;
    INT32 tileCol  = threadData->tileCol;
    Ptr<MgMap> map = threadData->map;
    #ifdef _DEBUG
    printf("> thread %d started, tile %d,%d\n", threadId, tileRow, tileCol);
    #endif

    try
    {
        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        // get the tile service instance
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        Ptr<MgTileService> svcTile = dynamic_cast<MgTileService*>(
            serviceManager->RequestService(MgServiceType::TileService));
        assert(svcTile != NULL);

        // find the finite display scale closest to the requested map scale
        double scale = map->GetViewScale();
        INT32 scaleIndex = map->FindNearestFiniteDisplayScaleIndex(scale);

        // read the image which we saved earlier
        wchar_t imgName[PATH_LEN] = { 0 };
        swprintf(imgName, PATH_LEN, L"./temp_tiles/tile%d_%d.png", tileRow, tileCol);
        Ptr<MgByteSource> bs = new MgByteSource(imgName);
        Ptr<MgByteReader> img = bs->GetReader();

        // set the tile in the cache
        svcTile->SetTile(img, map, scaleIndex, L"BaseLayers", tileCol, tileRow);

        // clear the user info to prevent leaks
        MgUserInformation::SetCurrentUserInfo(NULL);
    }
    catch (MgFileIoException* e)
    {
        // we sometimes get this due to a sharing error
        SAFE_RELEASE(e);
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

    #ifdef _DEBUG
//  printf("> thread %d done\n", threadId);
    #endif

    threadData->done = true;
    return 0;
}


void TestTileService::TestCase_SetTile()
{
    // specify the number of threads to use
    const INT32 numThreads = MG_TEST_THREADS;
    ThreadData threadData[numThreads];

    // define the range of tiles to set
    INT32 tileRowMin =  0;
    INT32 tileRowMax = 12;
    INT32 tileColMin =  3;
    INT32 tileColMax = 11;

    try
    {
        // need a thread manager
        ACE_Thread_Manager* manager = ACE_Thread_Manager::instance();

        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // make the runtime map
        Ptr<MgMap> map = CreateMap();

        // set up the tile indices
        INT32 numTileRows = tileRowMax - tileRowMin + 1;
        INT32 numTileCols = tileColMax - tileColMin + 1;
        INT32 numTiles    = numTileRows * numTileCols;

        INT32* tileRows = new INT32[numTiles];
        INT32* tileCols = new INT32[numTiles];

        INT32 nRequest = 0;
        for (INT32 tileRow = tileRowMin; tileRow <= tileRowMax; ++tileRow)
        {
            for (INT32 tileCol = tileColMin; tileCol <= tileColMax; ++tileCol)
            {
                tileRows[nRequest] = tileRow;
                tileCols[nRequest] = tileCol;
                nRequest++;
            }
        }

        // initialize the thread data
        for (INT32 i=0; i<numThreads; i++)
        {
            // each thread works with its own instance of the map
            Ptr<MgMemoryStreamHelper> helper = new MgMemoryStreamHelper();
            Ptr<MgStream> stream = new MgStream(helper);
            Ptr<MgMap> newMap = new MgMap();
            map->Serialize(stream);
            newMap->Deserialize(stream);

            threadData[i].threadId = i;
            threadData[i].done     = true;
            threadData[i].saveTile = false;
            threadData[i].map      = newMap;
            threadData[i].tileRow  = 0;
            threadData[i].tileCol  = 0;
        }

        // execute the requests to set all the tiles once
        #ifdef _DEBUG
        printf("\n");
        #endif
        nRequest = 0;
        for (;;)
        {
            INT32 dc = 0;
            for (INT32 i=0; i<numThreads; i++)
            {
                // check if the thread is available
                if (threadData[i].done)
                {
                    // if we're not yet done then give the thread a new request
                    if (nRequest < numTiles)
                    {
                        threadData[i].done    = false;
                        threadData[i].tileRow = tileRows[nRequest];
                        threadData[i].tileCol = tileCols[nRequest];

                        // spawn a new thread
                        int thid = manager->spawn(ACE_THR_FUNC(SetTileWorker), &threadData[i]);
                        nRequest++;
                    }

                    // keep a tally of all the done threads
                    if (threadData[i].done)
                        ++dc;
                }
            }

            // move on if all threads are done
            if (dc == numThreads)
                break;

            // under Linux we get a deadlock if we don't call this every once in a while
            if (nRequest % 25 == 0)
                manager->wait();
            else
            {
                // pause briefly (10ms) before checking again
                ACE_Time_Value t(0, 10000);
                ACE_OS::sleep(t);
            }
        }

        // make sure the manager is in a good state
        manager->wait();

        // At this point all the tiles in the cache should be set.
        // Let's now randomly replace these tiles.

        // reset the thread data
        for (INT32 i=0; i<numThreads; i++)
        {
            threadData[i].done    = true;
            threadData[i].tileRow = 0;
            threadData[i].tileCol = 0;
        }

        // execute the requests to randomly set tiles
        #ifdef _DEBUG
        printf("\n");
        #endif
        nRequest = 0;
        for (;;)
        {
            INT32 dc = 0;
            for (INT32 i=0; i<numThreads; i++)
            {
                // check if the thread is available
                if (threadData[i].done)
                {
                    // if we're not yet done then give the thread a new request
                    if (nRequest < REQUEST_FACTOR*numTiles)
                    {
                        // pick a random request to execute
                        INT32 nTile = Rand(numTiles);

                        threadData[i].done    = false;
                        threadData[i].tileRow = tileRows[nTile];
                        threadData[i].tileCol = tileCols[nTile];

                        // spawn a new thread
                        int thid = manager->spawn(ACE_THR_FUNC(SetTileWorker), &threadData[i]);
                        nRequest++;
                    }

                    // keep a tally of all the done threads
                    if (threadData[i].done)
                        ++dc;
                }
            }

            // move on if all threads are done
            if (dc == numThreads)
                break;

            // under Linux we get a deadlock if we don't call this every once in a while
            if (nRequest % 25 == 0)
                manager->wait();
            else
            {
                // pause briefly (10ms) before checking again
                ACE_Time_Value t(0, 10000);
                ACE_OS::sleep(t);
            }
        }

        // make sure the manager is in a good state
        manager->wait();

        // done with the tile indices
        delete tileRows;
        delete tileCols;
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


////////////////////////////////////////////////////////////////
/// GetSetTile methods
////////////////////////////////////////////////////////////////


void TestTileService::TestCase_GetSetTile()
{
    // specify the number of threads to use
    const INT32 numThreads = MG_TEST_THREADS;
    ThreadData threadData[numThreads];

    // define the range of tiles to get and set
    INT32 tileRowMin =  0;
    INT32 tileRowMax = 12;
    INT32 tileColMin =  3;
    INT32 tileColMax = 11;

    try
    {
        // need a thread manager
        ACE_Thread_Manager* manager = ACE_Thread_Manager::instance();

        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // make the runtime map
        Ptr<MgMap> map = CreateMap();

        // set up the tile indices
        INT32 numTileRows = tileRowMax - tileRowMin + 1;
        INT32 numTileCols = tileColMax - tileColMin + 1;
        INT32 numTiles    = numTileRows * numTileCols;

        INT32* tileRows = new INT32[numTiles];
        INT32* tileCols = new INT32[numTiles];

        INT32 nRequest = 0;
        for (INT32 tileRow = tileRowMin; tileRow <= tileRowMax; ++tileRow)
        {
            for (INT32 tileCol = tileColMin; tileCol <= tileColMax; ++tileCol)
            {
                tileRows[nRequest] = tileRow;
                tileCols[nRequest] = tileCol;
                nRequest++;
            }
        }

        // initialize the thread data
        for (INT32 i=0; i<numThreads; i++)
        {
            // each thread works with its own instance of the map
            Ptr<MgMemoryStreamHelper> helper = new MgMemoryStreamHelper();
            Ptr<MgStream> stream = new MgStream(helper);
            Ptr<MgMap> newMap = new MgMap();
            map->Serialize(stream);
            newMap->Deserialize(stream);

            threadData[i].threadId = i;
            threadData[i].done     = true;
            threadData[i].saveTile = false;
            threadData[i].map      = newMap;
            threadData[i].tileRow  = 0;
            threadData[i].tileCol  = 0;
        }

        // execute the requests to randomly get and set the tiles
        #ifdef _DEBUG
        printf("\n");
        #endif
        nRequest = 0;
        for (;;)
        {
            INT32 dc = 0;
            for (INT32 i=0; i<numThreads; i++)
            {
                // check if the thread is available
                if (threadData[i].done)
                {
                    // if we're not yet done then give the thread a new request
                    if (nRequest < REQUEST_FACTOR*numTiles)
                    {
                        // pick a random request to execute
                        INT32 nTile = Rand(numTiles);

                        threadData[i].done    = false;
                        threadData[i].tileRow = tileRows[nTile];
                        threadData[i].tileCol = tileCols[nTile];

                        // spawn a new thread - 75% of the calls are gets, 25% are sets
                        int thid;
                        if ((double)rand() < 0.75 * (double)RAND_MAX)
                            thid = manager->spawn(ACE_THR_FUNC(GetTileWorker), &threadData[i]);
                        else
                            thid = manager->spawn(ACE_THR_FUNC(SetTileWorker), &threadData[i]);
                        nRequest++;
                    }

                    // keep a tally of all the done threads
                    if (threadData[i].done)
                        ++dc;
                }
            }

            // move on if all threads are done
            if (dc == numThreads)
                break;

            // under Linux we get a deadlock if we don't call this every once in a while
            if (nRequest % 25 == 0)
                manager->wait();
            else
            {
                // pause briefly (10ms) before checking again
                ACE_Time_Value t(0, 10000);
                ACE_OS::sleep(t);
            }
        }

        // make sure the manager is in a good state
        manager->wait();

        // done with the tile indices
        delete tileRows;
        delete tileCols;
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


////////////////////////////////////////////////////////////////
/// ClearCache methods
////////////////////////////////////////////////////////////////


void TestTileService::TestCase_ClearCache()
{
    try
    {
        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // call the API with a NULL argument
        CPPUNIT_ASSERT_THROW_MG(m_svcTile->ClearCache(NULL), MgNullArgumentException*);

        // call the API with a map having a different name
        Ptr<MgMap> map = CreateMap(L"blah");
        m_svcTile->ClearCache(map);

        // call the API with the run time map
        map = CreateMap();
        m_svcTile->ClearCache(map);

        // call the API again on the same map
        m_svcTile->ClearCache(map);
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


////////////////////////////////////////////////////////////////
/// Helpers
////////////////////////////////////////////////////////////////


MgMap* TestTileService::CreateMap(CREFSTRING mapName)
{
    // set a default name if not supplied
    STRING name = (mapName.empty())? L"UnitTestBaseMap" : mapName;

    // make a runtime map
    Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/BaseMap.MapDefinition");
    MgMap* map = new MgMap();
    map->Create(m_svcResource, mdfres, name);

    // set the view
    Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-87.723636, 43.715015);
    Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
    map->SetViewCenter(ptNewCenter);
    map->SetDisplayDpi(96);
    map->SetDisplayWidth(1024);
    map->SetDisplayHeight(1024);

    // render at a scale of 1:12500
    map->SetViewScale(12500.0);

    return map;
}


// returns a random integer in the range 0 to n-1
INT32 TestTileService::Rand(INT32 n)
{
    double numer = (double)rand() + 0.5;
    double denom = (double)RAND_MAX + 1.0;
    return (INT32)(numer / denom * n);
}
