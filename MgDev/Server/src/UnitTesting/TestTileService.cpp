//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "SAX2Parser.h"
#include "MapGuideCommon.h"
#include "TestTileService.h"
#include "../UnitTesting/CppUnitExtensions.h"
#include "ServiceManager.h"
#include "ServerSiteService.h"
#include "../Common/Manager/FdoConnectionManager.h"
#include "FoundationDefs.h"
#define PATH_LEN 512

// determines the number of requests to make, as a factor of the number of tiles
#define REQUEST_FACTOR 20

// determines the interval of requests made for the ACE thread manager to wait for the entire thread group to complete
// (to avoid deadlock issues on Linux)
#define REQUEST_WAIT_INTERVAL 25

// define thread group for tiling tests
#define THREAD_GROUP 65535

static const INT32 MG_TEST_THREADS = 4;

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

    // Initialize a site connection.
    Ptr<MgServerSiteService> svcSite = dynamic_cast<MgServerSiteService*>(
        serviceMan->RequestService(MgServiceType::SiteService));
    assert(svcSite != NULL);

    Ptr<MgUserInformation> userInfo = new MgUserInformation(
        L"Administrator", L"admin");
    userInfo->SetLocale(TEST_LOCALE);

    // Set the current MgUserInformation
    // This must be done before calling CreateSession()
    MgUserInformation::SetCurrentUserInfo(userInfo);

    STRING session = svcSite->CreateSession();
    assert(!session.empty());
    userInfo->SetMgSessionId(session);

    // Set the current MgUserInformation
    MgUserInformation::SetCurrentUserInfo(userInfo);

    m_siteConnection = new MgSiteConnection();
    m_siteConnection->Open(userInfo);
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

        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinkedTileSet.MapDefinition");
        Ptr<MgByteSource> mdfsrc2 = new MgByteSource(L"../UnitTestFiles/UT_LinkedTileSet.mdf", false);
        Ptr<MgByteReader> mdfrdr2 = mdfsrc2->GetReader();
        m_svcResource->SetResource(mapres2, mdfrdr2, NULL);

        Ptr<MgResourceIdentifier> tilesetres1 = new MgResourceIdentifier(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition");
        Ptr<MgByteSource> tsdsrc1 = new MgByteSource(L"../UnitTestFiles/UT_BaseMap.tsd", false);
        Ptr<MgByteReader> tsdrdr1 = tsdsrc1->GetReader();
        m_svcResource->SetResource(tilesetres1, tsdrdr1, NULL);

        Ptr<MgResourceIdentifier> tilesetres2 = new MgResourceIdentifier(L"Library://UnitTests/TileSets/XYZ.TileSetDefinition");
        Ptr<MgByteSource> tsdsrc2 = new MgByteSource(L"../UnitTestFiles/UT_XYZ.tsd", false);
        Ptr<MgByteReader> tsdrdr2 = tsdsrc2->GetReader();
        m_svcResource->SetResource(tilesetres2, tsdrdr2, NULL);

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

        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinkedTileSet.MapDefinition");
        m_svcResource->DeleteResource(mapres2);

        Ptr<MgResourceIdentifier> tilesetres1 = new MgResourceIdentifier(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition");
        m_svcResource->DeleteResource(tilesetres1);

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
struct TileThreadData
{
    INT32 threadId;
    bool done;
    bool saveTile;
    Ptr<MgMap> map;
    INT32 tileRow;
    INT32 tileCol;
    INT32 tileScale;
    STRING tileSetId;
};


////////////////////////////////////////////////////////////////
/// GetTile methods
////////////////////////////////////////////////////////////////


// the method which gets executed by the ACE worker thread
ACE_THR_FUNC_RETURN GetTileWorker(void* param)
{
    // get the data for this thread
    TileThreadData* threadData = (TileThreadData*)param;
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
    TileThreadData threadData[numThreads];

    // define the range of tiles to get
    INT32 tileRowMin =  0;
    INT32 tileRowMax = 12;
    INT32 tileColMin =  3;
    INT32 tileColMax = 11;

    try
    {
        // need a thread manager
        ACE_Thread_Manager* manager = ACE_Thread_Manager::instance();

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
            Ptr<MgMap> newMap = new MgMap(m_siteConnection);
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

                        // spawn a new thread using a specific group id
                        int thid = manager->spawn(ACE_THR_FUNC(GetTileWorker), &threadData[i], 0, NULL, NULL, 0, THREAD_GROUP);
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
            if (nRequest % REQUEST_WAIT_INTERVAL == 0)
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

        // done with the tile indices
        delete [] tileRows;
        delete [] tileCols;
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

// the method which gets executed by the ACE worker thread
ACE_THR_FUNC_RETURN GetTileLinkedWorker(void* param)
{
    // get the data for this thread
    TileThreadData* threadData = (TileThreadData*)param;
    INT32 threadId = threadData->threadId;
    INT32 tileRow  = threadData->tileRow;
    INT32 tileCol  = threadData->tileCol;
    INT32 tileScale = threadData->tileScale;
    bool saveTile  = threadData->saveTile;
    Ptr<MgResourceIdentifier> tsId = new MgResourceIdentifier(threadData->tileSetId);
    #ifdef _DEBUG
    printf("> thread %d started, tile %d,%d\n", threadId, tileRow, tileCol);
    #endif

    try
    {
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
        Ptr<MgByteReader> img = svcTile->GetTile(tsId, L"BaseLayers", tileCol, tileRow, tileScale);
        CHECKNULL((MgByteReader*)img, L"GetTileLinkedWorker");

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

void TestTileService::TestCase_GetTileLinked()
{
    // specify the number of threads to use
    const INT32 numThreads = MG_TEST_THREADS;
    TileThreadData threadData[numThreads];

    // define the range of tiles to get
    INT32 tileRowMin =  0;
    INT32 tileRowMax = 12;
    INT32 tileColMin =  3;
    INT32 tileColMax = 11;

    try
    {
        // need a thread manager
        ACE_Thread_Manager* manager = ACE_Thread_Manager::instance();

        // make the runtime map
        Ptr<MgMap> map = CreateMapLinked();
        Ptr<MgResourceIdentifier> tsId;
        Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
        Ptr<MgLayerGroup> group = groups->GetItem(L"BaseLayers");
        tsId = map->GetTileSetDefinition();

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
            threadData[i].threadId = i;
            threadData[i].done     = true;
            threadData[i].saveTile = false;
            threadData[i].tileRow  = 0;
            threadData[i].tileCol  = 0;
            threadData[i].tileScale = 4;
            threadData[i].tileSetId = tsId->ToString().c_str();
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

                        // spawn a new thread using a specific group id
                        int thid = manager->spawn(ACE_THR_FUNC(GetTileLinkedWorker), &threadData[i], 0, NULL, NULL, 0, THREAD_GROUP);
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
            if (nRequest % REQUEST_WAIT_INTERVAL == 0)
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

        // done with the tile indices
        delete [] tileRows;
        delete [] tileCols;
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
    TileThreadData* threadData = (TileThreadData*)param;
    INT32 threadId = threadData->threadId;
    INT32 tileRow  = threadData->tileRow;
    INT32 tileCol  = threadData->tileCol;
    Ptr<MgMap> map = threadData->map;
    #ifdef _DEBUG
    printf("> thread %d started, tile %d,%d\n", threadId, tileRow, tileCol);
    #endif

    try
    {
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
    TileThreadData threadData[numThreads];

    // define the range of tiles to set
    INT32 tileRowMin =  0;
    INT32 tileRowMax = 12;
    INT32 tileColMin =  3;
    INT32 tileColMax = 11;

    try
    {
        // need a thread manager
        ACE_Thread_Manager* manager = ACE_Thread_Manager::instance();

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
            Ptr<MgMap> newMap = new MgMap(m_siteConnection);
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

                        // spawn a new thread using a specific group id
                        int thid = manager->spawn(ACE_THR_FUNC(SetTileWorker), &threadData[i], 0, NULL, NULL, 0, THREAD_GROUP);
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
            if (nRequest % REQUEST_WAIT_INTERVAL == 0)
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

                        // spawn a new thread using a specific group id
                        int thid = manager->spawn(ACE_THR_FUNC(SetTileWorker), &threadData[i], 0, NULL, NULL, 0, THREAD_GROUP);
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
            if (nRequest % REQUEST_WAIT_INTERVAL == 0)
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

        // done with the tile indices
        delete [] tileRows;
        delete [] tileCols;
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
    TileThreadData threadData[numThreads];

    // define the range of tiles to get and set
    INT32 tileRowMin =  0;
    INT32 tileRowMax = 12;
    INT32 tileColMin =  3;
    INT32 tileColMax = 11;

    try
    {
        // need a thread manager
        ACE_Thread_Manager* manager = ACE_Thread_Manager::instance();

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
            Ptr<MgMap> newMap = new MgMap(m_siteConnection);
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
                            thid = manager->spawn(ACE_THR_FUNC(GetTileWorker), &threadData[i], 0, NULL, NULL, 0, THREAD_GROUP);
                        else
                            thid = manager->spawn(ACE_THR_FUNC(SetTileWorker), &threadData[i], 0, NULL, NULL, 0, THREAD_GROUP);
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
            if (nRequest % REQUEST_WAIT_INTERVAL == 0)
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

        // done with the tile indices
        delete [] tileRows;
        delete [] tileCols;
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
        // call the API with a NULL argument
        CPPUNIT_ASSERT_THROW_MG(m_svcTile->ClearCache((MgMap*)NULL), MgNullArgumentException*);

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

// the method which gets executed by the ACE worker thread
ACE_THR_FUNC_RETURN GetTileXYZWorker(void* param)
{
    // get the data for this thread
    TileThreadData* threadData = (TileThreadData*)param;
    INT32 threadId = threadData->threadId;
    INT32 tileRow  = threadData->tileRow;
    INT32 tileCol  = threadData->tileCol;
    INT32 tileScale = threadData->tileScale;
    bool saveTile  = threadData->saveTile;
    Ptr<MgResourceIdentifier> tsId = new MgResourceIdentifier(threadData->tileSetId);
    #ifdef _DEBUG
    printf("> thread %d started, tile %d,%d\n", threadId, tileRow, tileCol);
    #endif

    try
    {
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
        Ptr<MgByteReader> img = svcTile->GetTile(tsId, L"BaseLayers", tileCol, tileRow, tileScale);
        CHECKNULL((MgByteReader*)img, L"GetTileXYZWorker");

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


void TestTileService::TestCase_GetTileXYZ()
{
    // specify the number of threads to use
    const INT32 numThreads = MG_TEST_THREADS;
    TileThreadData threadData[numThreads];

    // define the range of tiles to get
    INT32 tileRowMin = 33587;
    INT32 tileRowMax = 33601;
    INT32 tileColMin = 47766;
    INT32 tileColMax = 47784;

    try
    {
        // need a thread manager
        ACE_Thread_Manager* manager = ACE_Thread_Manager::instance();

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
            threadData[i].threadId = i;
            threadData[i].done     = true;
            threadData[i].saveTile = false;
            threadData[i].tileSetId = L"Library://UnitTests/TileSets/XYZ.TileSetDefinition";
            threadData[i].tileRow  = 0;
            threadData[i].tileCol  = 0;
            threadData[i].tileScale = 17; //z
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

                        // spawn a new thread using a specific group id
                        int thid = manager->spawn(ACE_THR_FUNC(GetTileXYZWorker), &threadData[i], 0, NULL, NULL, 0, THREAD_GROUP);
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
            if (nRequest % REQUEST_WAIT_INTERVAL == 0)
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

        // done with the tile indices
        delete [] tileRows;
        delete [] tileCols;
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


void TestTileService::TestCase_ClearCacheLinked()
{
    try
    {
        // call the API with a NULL argument
        CPPUNIT_ASSERT_THROW_MG(m_svcTile->ClearCache((MgResourceIdentifier*)NULL), MgNullArgumentException*);

        Ptr<MgResourceIdentifier> tsId = new MgResourceIdentifier(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition");
        m_svcTile->ClearCache(tsId);
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


void TestTileService::TestCase_MgMap_Inline()
{
    try
    {
        STRING mapName = L"TestCase_MgMap_Inline";
        Ptr<MgMap> map = CreateMap(mapName);

        Ptr<MgEnvelope> extents = map->GetMapExtent();
        Ptr<MgCoordinate> ll = extents->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> ur = extents->GetUpperRightCoordinate();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.79786601383196, ll->GetX(), 0.00000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.686857862181,   ll->GetY(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.66452777186925, ur->GetX(), 0.00000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.8037962206133,  ur->GetY(), 0.0000000000001);
        Ptr<MgResourceIdentifier> tsId = map->GetTileSetDefinition();
        CPPUNIT_ASSERT(NULL == (MgResourceIdentifier*)tsId);

        Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
        for (INT32 i = 0; i < groups->GetCount(); i++)
        {
            Ptr<MgLayerGroup> group = groups->GetItem(i);
            CPPUNIT_ASSERT(MgLayerGroupType::BaseMap == group->GetLayerGroupType());
        }

        // Initialize service objects.
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        Ptr<MgServerSiteService> svcSite = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        Ptr<MgResourceService> svcRes = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        assert(svcSite != NULL);
        assert(svcRes != NULL);
        // Set the current MgUserInformation
        // This must be done before calling CreateSession()
        Ptr<MgUserInformation> userInfo = new MgUserInformation(
            L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);
        STRING session = svcSite->CreateSession();

        Ptr<MgResourceIdentifier> mapStateId = new MgResourceIdentifier(MgRepositoryType::Session, session, L"", map->GetName(), MgResourceType::Map);
        map->Save(svcRes, mapStateId);

        map = NULL;

        Ptr<MgSiteConnection> siteConn = new MgSiteConnection();
        userInfo = new MgUserInformation(session);
        siteConn->Open(userInfo);
        map = new MgMap(siteConn);
        map->Open(mapName);

        //Re-check. All should be the same
        extents = map->GetMapExtent();
        ll = extents->GetLowerLeftCoordinate();
        ur = extents->GetUpperRightCoordinate();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.79786601383196, ll->GetX(), 0.00000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.686857862181,   ll->GetY(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.66452777186925, ur->GetX(), 0.00000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.8037962206133,  ur->GetY(), 0.0000000000001);
        tsId = map->GetTileSetDefinition();
        CPPUNIT_ASSERT(NULL == (MgResourceIdentifier*)tsId);

        groups = map->GetLayerGroups();
        for (INT32 i = 0; i < groups->GetCount(); i++)
        {
            Ptr<MgLayerGroup> group = groups->GetItem(i);
            CPPUNIT_ASSERT(MgLayerGroupType::BaseMap == group->GetLayerGroupType());
        }

        svcSite->DestroySession(session);
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

void TestTileService::TestCase_MgMap_Linked()
{
    try
    {
        STRING mapName = L"TestCase_MgMap_Linked";
        Ptr<MgMap> map = CreateMapLinked(mapName);

        //Bounds should be that of the tile set
        Ptr<MgEnvelope> extents = map->GetMapExtent();
        Ptr<MgCoordinate> ll = extents->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> ur = extents->GetUpperRightCoordinate();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.797866013831, ll->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.686857862181, ll->GetY(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.664527771869, ur->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.803796220613, ur->GetY(), 0.000000000001);
        Ptr<MgResourceIdentifier> tsId = map->GetTileSetDefinition();
        CPPUNIT_ASSERT(NULL != (MgResourceIdentifier*)tsId);
        CPPUNIT_ASSERT(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition" == tsId->ToString());

        Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
        for (INT32 i = 0; i < groups->GetCount(); i++)
        {
            Ptr<MgLayerGroup> group = groups->GetItem(i);
            CPPUNIT_ASSERT(MgLayerGroupType::BaseMapFromTileSet == group->GetLayerGroupType());
        }

        // Initialize service objects.
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        Ptr<MgServerSiteService> svcSite = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        Ptr<MgResourceService> svcRes = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        assert(svcSite != NULL);
        assert(svcRes != NULL);
        // Set the current MgUserInformation
        // This must be done before calling CreateSession()
        Ptr<MgUserInformation> userInfo = new MgUserInformation(
            L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);
        STRING session = svcSite->CreateSession();

        Ptr<MgResourceIdentifier> mapStateId = new MgResourceIdentifier(MgRepositoryType::Session, session, L"", map->GetName(), MgResourceType::Map);
        map->Save(svcRes, mapStateId);

        map = NULL;

        Ptr<MgSiteConnection> siteConn = new MgSiteConnection();
        userInfo = new MgUserInformation(session);
        siteConn->Open(userInfo);
        map = new MgMap(siteConn);
        map->Open(mapName);

        //Re-check. All should be the same
        extents = map->GetMapExtent();
        ll = extents->GetLowerLeftCoordinate();
        ur = extents->GetUpperRightCoordinate();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.797866013831, ll->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.686857862181, ll->GetY(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.664527771869, ur->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.803796220613, ur->GetY(), 0.000000000001);
        tsId = map->GetTileSetDefinition();
        CPPUNIT_ASSERT(NULL != (MgResourceIdentifier*)tsId);
        CPPUNIT_ASSERT(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition" == tsId->ToString());

        groups = map->GetLayerGroups();
        for (INT32 i = 0; i < groups->GetCount(); i++)
        {
            Ptr<MgLayerGroup> group = groups->GetItem(i);
            CPPUNIT_ASSERT(MgLayerGroupType::BaseMapFromTileSet == group->GetLayerGroupType());
        }

        svcSite->DestroySession(session);
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

void TestTileService::TestCase_MgMap_LinkedSameCS()
{
    try
    {
        STRING mapName = L"TestCase_MgMap_Linked";
        Ptr<MgMap> map = CreateMapLinked2(mapName);

        //Bounds should be that of the tile set
        Ptr<MgEnvelope> extents = map->GetMapExtent();
        CPPUNIT_ASSERT(NULL != extents.p);
        Ptr<MgEnvelope> dataExtents = map->GetDataExtent();
        CPPUNIT_ASSERT(NULL != dataExtents.p);
        Ptr<MgCoordinate> ll = extents->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> ur = extents->GetUpperRightCoordinate();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.797866013831, ll->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(43.686857862181, ll->GetY(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.664527771869, ur->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(43.803796220613, ur->GetY(), 0.000000000001);
        Ptr<MgResourceIdentifier> tsId = map->GetTileSetDefinition();
        CPPUNIT_ASSERT(NULL != (MgResourceIdentifier*)tsId);
        CPPUNIT_ASSERT(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition" == tsId->ToString());

        Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
        for (INT32 i = 0; i < groups->GetCount(); i++)
        {
            Ptr<MgLayerGroup> group = groups->GetItem(i);
            CPPUNIT_ASSERT(MgLayerGroupType::BaseMapFromTileSet == group->GetLayerGroupType());
        }

        // Initialize service objects.
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        Ptr<MgServerSiteService> svcSite = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        Ptr<MgResourceService> svcRes = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        assert(svcSite != NULL);
        assert(svcRes != NULL);
        // Set the current MgUserInformation
        // This must be done before calling CreateSession()
        Ptr<MgUserInformation> userInfo = new MgUserInformation(
            L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);
        STRING session = svcSite->CreateSession();

        Ptr<MgResourceIdentifier> mapStateId = new MgResourceIdentifier(MgRepositoryType::Session, session, L"", map->GetName(), MgResourceType::Map);
        map->Save(svcRes, mapStateId);

        map = NULL;

        Ptr<MgSiteConnection> siteConn = new MgSiteConnection();
        userInfo = new MgUserInformation(session);
        siteConn->Open(userInfo);
        map = new MgMap(siteConn);
        map->Open(mapName);

        //Re-check. All should be the same
        extents = map->GetMapExtent();
        ll = extents->GetLowerLeftCoordinate();
        ur = extents->GetUpperRightCoordinate();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.797866013831, ll->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(43.686857862181, ll->GetY(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.664527771869, ur->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(43.803796220613, ur->GetY(), 0.000000000001);
        tsId = map->GetTileSetDefinition();
        CPPUNIT_ASSERT(NULL != (MgResourceIdentifier*)tsId);
        CPPUNIT_ASSERT(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition" == tsId->ToString());

        groups = map->GetLayerGroups();
        for (INT32 i = 0; i < groups->GetCount(); i++)
        {
            Ptr<MgLayerGroup> group = groups->GetItem(i);
            CPPUNIT_ASSERT(MgLayerGroupType::BaseMapFromTileSet == group->GetLayerGroupType());
        }

        svcSite->DestroySession(session);
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

void TestTileService::TestCase_MgMapFromXYZTileSetStrict()
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> tsdres = new MgResourceIdentifier(L"Library://UnitTests/TileSets/XYZ.TileSetDefinition");
        MgMap* map = new MgMap(m_siteConnection);
        map->Create(tsdres, L"XYZTileSet");

        CPPUNIT_FAIL("MgUnsupportedTileProviderException should've been thrown");
    }
    catch (MgException* e)
    {
        bool isCorrectException = e->IsOfClass(MapGuide_Exception_MgUnsupportedTileProviderException);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_ASSERT_MESSAGE(MG_WCHAR_TO_CHAR(message), isCorrectException);
    }
    catch (...)
    {
        throw;
    }
}

void TestTileService::TestCase_MgMapFromXYZTileSetLoose()
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> tsdres = new MgResourceIdentifier(L"Library://UnitTests/TileSets/XYZ.TileSetDefinition");
        MgMap* map = new MgMap(m_siteConnection);
        map->Create(m_svcResource, tsdres, L"XYZTileSet", false);

        CPPUNIT_ASSERT(0 == map->GetFiniteDisplayScaleCount());
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

void TestTileService::TestCase_MgMapFromTileSet()
{
    try
    {
        STRING mapName = L"TestCase_MgMapFromTileSet";

        // make a runtime map
        Ptr<MgResourceIdentifier> tsdres = new MgResourceIdentifier(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition");
        MgMap* map = new MgMap(m_siteConnection);
        map->Create(tsdres, mapName);

        Ptr<MgResourceIdentifier> mdfId = map->GetMapDefinition();
        CPPUNIT_ASSERT(NULL == (MgResourceIdentifier*)mdfId);

        //Bounds should be that of the tile set
        Ptr<MgEnvelope> extents = map->GetMapExtent();
        Ptr<MgCoordinate> ll = extents->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> ur = extents->GetUpperRightCoordinate();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.797866013831, ll->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.686857862181, ll->GetY(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.664527771869, ur->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.803796220613, ur->GetY(), 0.000000000001);
        Ptr<MgResourceIdentifier> tsId = map->GetTileSetDefinition();
        CPPUNIT_ASSERT(NULL != (MgResourceIdentifier*)tsId);
        CPPUNIT_ASSERT(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition" == tsId->ToString());

        Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
        for (INT32 i = 0; i < groups->GetCount(); i++)
        {
            Ptr<MgLayerGroup> group = groups->GetItem(i);
            CPPUNIT_ASSERT(MgLayerGroupType::BaseMapFromTileSet == group->GetLayerGroupType());
        }

        // Initialize service objects.
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        Ptr<MgServerSiteService> svcSite = dynamic_cast<MgServerSiteService*>(serviceManager->RequestService(MgServiceType::SiteService));
        Ptr<MgResourceService> svcRes = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        assert(svcSite != NULL);
        assert(svcRes != NULL);
        // Set the current MgUserInformation
        // This must be done before calling CreateSession()
        Ptr<MgUserInformation> userInfo = new MgUserInformation(
            L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);
        STRING session = svcSite->CreateSession();

        Ptr<MgResourceIdentifier> mapStateId = new MgResourceIdentifier(MgRepositoryType::Session, session, L"", map->GetName(), MgResourceType::Map);
        map->Save(svcRes, mapStateId);

        map = NULL;

        Ptr<MgSiteConnection> siteConn = new MgSiteConnection();
        userInfo = new MgUserInformation(session);
        siteConn->Open(userInfo);
        map = new MgMap(siteConn);
        map->Open(mapName);

        //Re-check. All should be the same
        extents = map->GetMapExtent();
        ll = extents->GetLowerLeftCoordinate();
        ur = extents->GetUpperRightCoordinate();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.797866013831, ll->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.686857862181, ll->GetY(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-87.664527771869, ur->GetX(), 0.000000000001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL( 43.803796220613, ur->GetY(), 0.000000000001);
        tsId = map->GetTileSetDefinition();
        CPPUNIT_ASSERT(NULL != (MgResourceIdentifier*)tsId);
        CPPUNIT_ASSERT(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition" == tsId->ToString());

        groups = map->GetLayerGroups();
        for (INT32 i = 0; i < groups->GetCount(); i++)
        {
            Ptr<MgLayerGroup> group = groups->GetItem(i);
            CPPUNIT_ASSERT(MgLayerGroupType::BaseMapFromTileSet == group->GetLayerGroupType());
        }

        svcSite->DestroySession(session);
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

void TestTileService::TestCase_GetTileProviders()
{
    try
    {
        Ptr<MgByteReader> content = m_svcTile->GetTileProviders();
        Ptr<MgByteSink> sink = new MgByteSink(content);
        sink->ToFile(L"../UnitTestFiles/GetTileProviders_Result.xml");
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
    MgMap* map = new MgMap(m_siteConnection);
    map->Create(mdfres, name);

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


MgMap* TestTileService::CreateMapLinked(CREFSTRING mapName)
{
    // set a default name if not supplied
    STRING name = (mapName.empty())? L"UnitTestBaseMapLinked" : mapName;

    // make a runtime map
    Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinkedTileSet.MapDefinition");
    MgMap* map = new MgMap(m_siteConnection);
    map->Create(mdfres, name);

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

MgMap* TestTileService::CreateMapLinked2(CREFSTRING mapName)
{
    // set a default name if not supplied
    STRING name = (mapName.empty()) ? L"UnitTestBaseMapLinked" : mapName;

    Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinkedTileSet.MapDefinition");
    // Parse the Map Definition and modify it so it matches its linked tile set
    Ptr<MgResourceService> resSvc = dynamic_cast<MgResourceService*>(m_siteConnection->CreateService(MgServiceType::ResourceService));
    
    MdfModel::MapDefinition* mdf = MgMapBase::GetMapDefinition(resSvc, mdfres);
    MdfModel::TileSetSource* tsRef = mdf->GetTileSetSource();
    
    Ptr<MgResourceIdentifier> tileSetId = new MgResourceIdentifier(tsRef->GetResourceId());
    Ptr<MgByteReader> reader = resSvc->GetResourceContent(tileSetId);
    Ptr<MgByteSink> sink = new MgByteSink(reader);
    Ptr<MgByte> bytes = sink->ToBuffer();

    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());

    CPPUNIT_ASSERT(parser.GetSucceeded());
    MdfModel::TileSetDefinition* tsd = parser.DetachTileSetDefinition();
    // Update coordinate systems to match
    STRING csWkt;
    MdfModel::NameStringPairCollection* params = tsd->GetTileStoreParameters()->GetParameters();
    for (INT32 i = 0; i < params->GetCount(); i++)
    {
        MdfModel::NameStringPair* pair = params->GetAt(i);
        if (pair->GetName() == L"CoordinateSystem")
        {
            csWkt = pair->GetValue();
            mdf->SetCoordinateSystem(csWkt);
            break;
        }
    }
    // Fix up extents too
    mdf->SetExtents(tsd->GetExtents());

    // Save back out to XML
    MdfModel::Version ver(3, 0, 0);
    std::string mbXml = parser.SerializeToXML(mdf, &ver);
    Ptr<MgByteSource> source = new MgByteSource((BYTE_ARRAY_IN)mbXml.c_str(), (INT32)mbXml.length());
    Ptr<MgByteReader> content = source->GetReader();
    mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinkedTileSet2.MapDefinition");
    resSvc->SetResource(mdfres, content, NULL);

    // make a runtime map from our changed map definition
    MgMap* map = new MgMap(m_siteConnection);
    map->Create(mdfres, name);

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
