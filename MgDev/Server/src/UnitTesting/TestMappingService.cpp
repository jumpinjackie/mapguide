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

#include "MapGuideCommon.h"
#include "TestMappingService.h"
#include "ServiceManager.h"
#include "ServerResourceService.h"
#include "ServerMappingService.h"
#include "ServerSiteService.h"
#include "../Common/Manager/FdoConnectionManager.h"
#include "CppUnitExtensions.h"
#include "FoundationDefs.h"
const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestMappingService, "TestMappingService");


TestMappingService::TestMappingService()
{
    // Initialize service objects.
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();

    m_svcResource = dynamic_cast<MgResourceService*>(
        serviceManager->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcMapping = dynamic_cast<MgMappingService*>(
        serviceManager->RequestService(MgServiceType::MappingService));
    assert(m_svcMapping != NULL);

    // Initialize a site connection.
    Ptr<MgServerSiteService> svcSite = dynamic_cast<MgServerSiteService*>(
        serviceManager->RequestService(MgServiceType::SiteService));
    assert(svcSite != NULL);

    Ptr<MgUserInformation> userInfo = new MgUserInformation(
        L"Administrator", L"admin");
    userInfo->SetLocale(TEST_LOCALE);

    // Set the current MgUserInformation
    // This must be done before calling CreateSession()
    MgUserInformation::SetCurrentUserInfo(userInfo);

    m_session = svcSite->CreateSession();
    assert(!m_session.empty());
    userInfo->SetMgSessionId(m_session);

    // Set the current MgUserInformation
    MgUserInformation::SetCurrentUserInfo(userInfo);

    m_siteConnection = new MgSiteConnection();
    m_siteConnection->Open(userInfo);
}


TestMappingService::~TestMappingService()
{
}


void TestMappingService::setUp()
{
}


void TestMappingService::tearDown()
{
}


void TestMappingService::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Mapping Service tests.\n")));

    try
    {
        #ifdef _DEBUG
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif

        //set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        //publish the map definition
        Ptr<MgResourceIdentifier> mapres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgByteSource> mdfsrc = new MgByteSource(L"../UnitTestFiles/UT_Sheboygan.mdf", false);
        Ptr<MgByteReader> mdfrdr = mdfsrc->GetReader();
        m_svcResource->SetResource(mapres, mdfrdr, NULL);

        //publish the layer definitions
        Ptr<MgResourceIdentifier> ldfres1 = new MgResourceIdentifier(L"Library://UnitTests/Layers/HydrographicPolygons.LayerDefinition");
        Ptr<MgByteSource> ldfsrc1 = new MgByteSource(L"../UnitTestFiles/UT_HydrographicPolygons.ldf", false);
        Ptr<MgByteReader> ldfrdr1 = ldfsrc1->GetReader();
        m_svcResource->SetResource(ldfres1, ldfrdr1, NULL);

        Ptr<MgResourceIdentifier> ldfres2 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        Ptr<MgByteSource> ldfsrc2 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.ldf", false);
        Ptr<MgByteReader> ldfrdr2 = ldfsrc2->GetReader();
        m_svcResource->SetResource(ldfres2, ldfrdr2, NULL);

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Rail.LayerDefinition");
        Ptr<MgByteSource> ldfsrc3 = new MgByteSource(L"../UnitTestFiles/UT_Rail.ldf", false);
        Ptr<MgByteReader> ldfrdr3 = ldfsrc3->GetReader();
        m_svcResource->SetResource(ldfres3, ldfrdr3, NULL);

        Ptr<MgResourceIdentifier> ldfres4 = new MgResourceIdentifier(L"Library://UnitTests/Layers/RotatedPointStyles.LayerDefinition");
        Ptr<MgByteSource> ldfsrc4 = new MgByteSource(L"../UnitTestFiles/UT_RotatedPointStyles.ldf", false);
        Ptr<MgByteReader> ldfrdr4 = ldfsrc4->GetReader();
        m_svcResource->SetResource(ldfres4, ldfrdr4, NULL);

        Ptr<MgResourceIdentifier> ldfres5 = new MgResourceIdentifier(L"Library://UnitTests/Layers/RoadCenterLines.LayerDefinition");
        Ptr<MgByteSource> ldfsrc5 = new MgByteSource(L"../UnitTestFiles/UT_RoadCenterLines.ldf", false);
        Ptr<MgByteReader> ldfrdr5 = ldfsrc5->GetReader();
        m_svcResource->SetResource(ldfres5, ldfrdr5, NULL);

        Ptr<MgResourceIdentifier> ldfres6 = new MgResourceIdentifier(L"Library://UnitTests/Layers/VotingDistricts.LayerDefinition");
        Ptr<MgByteSource> ldfsrc6 = new MgByteSource(L"../UnitTestFiles/UT_VotingDistricts.ldf", false);
        Ptr<MgByteReader> ldfrdr6 = ldfsrc6->GetReader();
        m_svcResource->SetResource(ldfres6, ldfrdr6, NULL);

        Ptr<MgResourceIdentifier> ldfres7 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTS.LayerDefinition");
        Ptr<MgByteSource> ldfsrc7 = new MgByteSource(L"../UnitTestFiles/UT_MultiCTS.ldf", false);
        Ptr<MgByteReader> ldfrdr7 = ldfsrc7->GetReader();
        m_svcResource->SetResource(ldfres7, ldfrdr7, NULL);

        Ptr<MgResourceIdentifier> ldfres8 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTSWithTheme.LayerDefinition");
        Ptr<MgByteSource> ldfsrc8 = new MgByteSource(L"../UnitTestFiles/UT_MultiCTSWithTheme.ldf", false);
        Ptr<MgByteReader> ldfrdr8 = ldfsrc8->GetReader();
        m_svcResource->SetResource(ldfres8, ldfrdr8, NULL);

        //publish the feature sources
        Ptr<MgResourceIdentifier> fsres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/HydrographicPolygons.FeatureSource");
        Ptr<MgByteSource> fssrc1 = new MgByteSource(L"../UnitTestFiles/UT_HydrographicPolygons.fs", false);
        Ptr<MgByteReader> fsrdr1 = fssrc1->GetReader();
        m_svcResource->SetResource(fsres1, fsrdr1, NULL);

        Ptr<MgResourceIdentifier> fsres2 = new MgResourceIdentifier(L"Library://UnitTests/Data/Parcels.FeatureSource");
        Ptr<MgByteSource> fssrc2 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.fs", false);
        Ptr<MgByteReader> fsrdr2 = fssrc2->GetReader();
        m_svcResource->SetResource(fsres2, fsrdr2, NULL);

        Ptr<MgResourceIdentifier> fsres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/Rail.FeatureSource");
        Ptr<MgByteSource> fssrc3 = new MgByteSource(L"../UnitTestFiles/UT_Rail.fs", false);
        Ptr<MgByteReader> fsrdr3 = fssrc3->GetReader();
        m_svcResource->SetResource(fsres3, fsrdr3, NULL);

        // publish the resource data
        Ptr<MgByteSource> dataSource1 = new MgByteSource(L"../UnitTestFiles/UT_HydrographicPolygons.sdf", false);
        Ptr<MgByteReader> dataReader1 = dataSource1->GetReader();
        m_svcResource->SetResourceData(fsres1, L"UT_HydrographicPolygons.sdf", L"File", dataReader1);

        Ptr<MgByteSource> dataSource2 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.sdf", false);
        Ptr<MgByteReader> dataReader2 = dataSource2->GetReader();
        m_svcResource->SetResourceData(fsres2, L"UT_Parcels.sdf", L"File", dataReader2);

        Ptr<MgByteSource> dataSource3 = new MgByteSource(L"../UnitTestFiles/UT_Rail.sdf", false);
        Ptr<MgByteReader> dataReader3 = dataSource3->GetReader();
        m_svcResource->SetResourceData(fsres3, L"UT_Rail.sdf", L"File", dataReader3);

        // publish the print layouts
        Ptr<MgResourceIdentifier> plres1 = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");
        Ptr<MgByteSource> plsrc1 = new MgByteSource(L"../UnitTestFiles/UT_AllElements.pl", false);
        Ptr<MgByteReader> plrdr1 = plsrc1->GetReader();
        m_svcResource->SetResource(plres1, plrdr1, NULL);

        Ptr<MgResourceIdentifier> plres2 = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/NoLegend.PrintLayout");
        Ptr<MgByteSource> plsrc2 = new MgByteSource(L"../UnitTestFiles/UT_NoLegend.pl", false);
        Ptr<MgByteReader> plrdr2 = plsrc2->GetReader();
        m_svcResource->SetResource(plres2, plrdr2, NULL);

        // publish the symbol library
        Ptr<MgResourceIdentifier> slres1 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/SymbolMart.SymbolLibrary");
        Ptr<MgByteSource> slsrc1 = new MgByteSource(L"../UnitTestFiles/UT_SymbolMart.sl", false);
        Ptr<MgByteReader> slrdr1 = slsrc1->GetReader();
        m_svcResource->SetResource(slres1, slrdr1, NULL);
        Ptr<MgByteSource> datasrc = new MgByteSource(L"../UnitTestFiles/UT_Symbols.dwf", false);
        Ptr<MgByteReader> datardr = datasrc->GetReader();
        m_svcResource->SetResourceData(slres1, L"symbols.dwf", L"File", datardr);

        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinkedTileSet.MapDefinition");
        Ptr<MgByteSource> mdfsrc2 = new MgByteSource(L"../UnitTestFiles/UT_LinkedTileSet.mdf", false);
        Ptr<MgByteReader> mdfrdr2 = mdfsrc2->GetReader();
        m_svcResource->SetResource(mapres2, mdfrdr2, NULL);

        Ptr<MgResourceIdentifier> tilesetres1 = new MgResourceIdentifier(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition");
        Ptr<MgByteSource> tsdsrc1 = new MgByteSource(L"../UnitTestFiles/UT_BaseMap.tsd", false);
        Ptr<MgByteReader> tsdrdr1 = tsdsrc1->GetReader();
        m_svcResource->SetResource(tilesetres1, tsdrdr1, NULL);
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


void TestMappingService::TestEnd()
{
    try
    {
        //set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        //delete the map definition
        Ptr<MgResourceIdentifier> mapres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        m_svcResource->DeleteResource(mapres);

        //delete the layer definitions
        Ptr<MgResourceIdentifier> ldfres1 = new MgResourceIdentifier(L"Library://UnitTests/Layers/HydrographicPolygons.LayerDefinition");
        m_svcResource->DeleteResource(ldfres1);

        Ptr<MgResourceIdentifier> ldfres2 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        m_svcResource->DeleteResource(ldfres2);

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Rail.LayerDefinition");
        m_svcResource->DeleteResource(ldfres3);

        Ptr<MgResourceIdentifier> ldfres4 = new MgResourceIdentifier(L"Library://UnitTests/Layers/RotatedPointStyles.LayerDefinition");
        m_svcResource->DeleteResource(ldfres4);

        Ptr<MgResourceIdentifier> ldfres5 = new MgResourceIdentifier(L"Library://UnitTests/Layers/RoadCenterLines.LayerDefinition");
        m_svcResource->DeleteResource(ldfres5);

        Ptr<MgResourceIdentifier> ldfres6 = new MgResourceIdentifier(L"Library://UnitTests/Layers/VotingDistricts.LayerDefinition");
        m_svcResource->DeleteResource(ldfres6);

        Ptr<MgResourceIdentifier> ldfres7 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTS.LayerDefinition");
        m_svcResource->DeleteResource(ldfres7);

        Ptr<MgResourceIdentifier> ldfres8 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTSWithTheme.LayerDefinition");
        m_svcResource->DeleteResource(ldfres8);

        //delete the feature sources
        Ptr<MgResourceIdentifier> fsres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/HydrographicPolygons.FeatureSource");
        m_svcResource->DeleteResource(fsres1);

        Ptr<MgResourceIdentifier> fsres2 = new MgResourceIdentifier(L"Library://UnitTests/Data/Parcels.FeatureSource");
        m_svcResource->DeleteResource(fsres2);

        Ptr<MgResourceIdentifier> fsres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/Rail.FeatureSource");
        m_svcResource->DeleteResource(fsres3);

        // delete the print layouts
        Ptr<MgResourceIdentifier> plres1 = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");
        m_svcResource->DeleteResource(plres1);

        Ptr<MgResourceIdentifier> plres2 = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/NoLegend.PrintLayout");
        m_svcResource->DeleteResource(plres2);

        // delete the symbol library
        Ptr<MgResourceIdentifier> slres1 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/SymbolMart.SymbolLibrary");
        m_svcResource->DeleteResource(slres1);

        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinkedTileSet.MapDefinition");
        m_svcResource->DeleteResource(mapres2);

        Ptr<MgResourceIdentifier> tilesetres1 = new MgResourceIdentifier(L"Library://UnitTests/TileSets/Sheboygan.TileSetDefinition");
        m_svcResource->DeleteResource(tilesetres1);

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

    ACE_DEBUG((LM_INFO, ACE_TEXT("\nMapping Service tests completed.\n\n")));
}

void TestMappingService::TestCase_CreateRuntimeMap()
{
    try
    {
        //make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        STRING format = MgImageFormats::Png;
        //call the API
        Ptr<MgByteReader> rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan1", m_session, format, 16, 16, 0, 25);
        Ptr<MgByteSink> sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapBarebones.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan2", m_session, format, 16, 16, 1, 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroups.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan3", m_session, format, 16, 16, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsPNG16x16.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan4", m_session, format, 16, 16, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsAndFeatureSourcePNG16x16.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan5", m_session, format, 32, 32, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsPNG32x32.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan6", m_session, format, 32, 32, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsAndFeatureSourcePNG32x32.xml");

        format = MgImageFormats::Gif;

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan3", m_session, format, 16, 16, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsGIF16x16.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan4", m_session, format, 16, 16, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsAndFeatureSourceGIF16x16.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan5", m_session, format, 32, 32, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsGIF32x32.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan6", m_session, format, 32, 32, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsAndFeatureSourceGIF32x32.xml");

        format = MgImageFormats::Jpeg;

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan3", m_session, format, 16, 16, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsJPEG16x16.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan4", m_session, format, 16, 16, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsAndFeatureSourceJPEG16x16.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan5", m_session, format, 32, 32, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsJPEG32x32.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan6", m_session, format, 32, 32, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsAndFeatureSourceJPEG32x32.xml");

        format = MgImageFormats::Png8;

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan3", m_session, format, 16, 16, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsPNG816x16.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan4", m_session, format, 16, 16, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsAndFeatureSourcePNG816x16.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan5", m_session, format, 32, 32, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsPNG832x32.xml");

        rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan6", m_session, format, 32, 32, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/RuntimeMapLayersAndGroupsWithIconsAndFeatureSourcePNG832x32.xml");
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestMappingService::TestCase_DescribeRuntimeMap()
{
    try
    {
        //make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        STRING format = MgImageFormats::Png;
        //call the API
        Ptr<MgByteReader> rtMap = m_svcMapping->CreateRuntimeMap(mdfres, L"UnitTestSheboygan1", m_session, format, 16, 16, 0, 25);
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Open(L"UnitTestSheboygan1");
        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, 0, 25);

        Ptr<MgByteSink> sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapBarebones.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, 1, 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroups.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsPNG16x16.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsAndFeatureSourcePNG16x16.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 32, 32, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsPNG32x32.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 32, 32, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsAndFeatureSourcePNG32x32.xml");

        format = MgImageFormats::Gif;

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsGIF16x16.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsAndFeatureSourceGIF16x16.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 32, 32, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsGIF32x32.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 32, 32, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsAndFeatureSourceGIF32x32.xml");

        format = MgImageFormats::Jpeg;

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsJPEG16x16.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsAndFeatureSourceJPEG16x16.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 32, 32, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsJPEG32x32.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 32, 32, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsAndFeatureSourceJPEG32x32.xml");

        format = MgImageFormats::Png8;

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsPNG816x16.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsAndFeatureSourcePNG816x16.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 32, 32, (1 | 2), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsPNG832x32.xml");

        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 32, 32, (1 | 2 | 4), 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/DescribeRuntimeMapLayersAndGroupsWithIconsAndFeatureSourcePNG832x32.xml");
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

void TestMappingService::TestCase_CreateAndDescribeLinkedRuntimeMap()
{
    try
    {
        Ptr<MgResourceIdentifier> mdfId = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinkedTileSet.MapDefinition");
        STRING format = MgImageFormats::Png;
        Ptr<MgByteReader> rtMap = m_svcMapping->CreateRuntimeMap(mdfId, L"TestCase_CreateAndDescribeLinkedRuntimeMap", m_session, format, 16, 16, 0, 25);
        Ptr<MgByteSink> sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/TestCase_CreateLinkedRuntimeMap.xml");

        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Open(L"TestCase_CreateAndDescribeLinkedRuntimeMap");
        rtMap = m_svcMapping->DescribeRuntimeMap(map, format, 16, 16, 0, 25);
        sink = new MgByteSink(rtMap);
        sink->ToFile(L"../UnitTestFiles/TestCase_DescribeLinkedRuntimeMap.xml");
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}
void TestMappingService::TestCase_SaveMap()
{
    try
    {
        // Create a runtime map, save it, then open it.
        Ptr<MgResourceIdentifier> mapDefId = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        STRING mapName = L"UnitTestSheboygan";
        Ptr<MgMap> map = new MgMap(m_siteConnection);

        map->Create(mapDefId, mapName);
        map->Save();
        map->Open(mapName);

        Ptr<MgLayerCollection> layers = map->GetLayers();
        assert(layers->GetCount() > 0);
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

void TestMappingService::TestCase_GetMultiPlot()
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map1 = new MgMap(m_siteConnection);
        map1->Create(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(200e+6);

        // make another runtime map
        Ptr<MgResourceIdentifier> mapRes2 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map2 = new MgMap(m_siteConnection);
        map2->Create(mapRes2, L"UnitTestSheboygan2");
        map2->SetViewScale(100e+6);

        // make yet another runtime map
        Ptr<MgResourceIdentifier> mapRes3 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map3 = new MgMap(m_siteConnection);
        map3->Create(mapRes3, L"UnitTestSheboygan3");
        map3->SetViewScale(20e+6);

        // make a 4th runtime map
        Ptr<MgResourceIdentifier> mapRes4 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map4 = new MgMap(m_siteConnection);
        map4->Create(mapRes4, L"UnitTestSheboygan4");
        map4->SetViewScale(20e+6);

        // Create the DwfVersion
        Ptr<MgDwfVersion> version = new MgDwfVersion();

        // Create some plotSpecs
        Ptr<MgPlotSpecification> plotSpec1 = new MgPlotSpecification(8.5f, 11.f, L"in", 0.5, 0.5, 0.5, 0.5);
        Ptr<MgPlotSpecification> plotSpec2 = new MgPlotSpecification(11.f, 8.5f, L"in", 1, 1, 1, 1);
        Ptr<MgPlotSpecification> plotSpec3 = new MgPlotSpecification(8.f, 11.f, L"in", 0.25, 0.25, 0.25, 0.25);
        Ptr<MgPlotSpecification> plotSpec4 = new MgPlotSpecification(8.5f, 11.f, L"in", 0.25, 0.25, 0.25, 0.25);

        Ptr<MgResourceIdentifier> allElementsLayout = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");
        Ptr<MgLayout> layout = new MgLayout(allElementsLayout, L"Layout1", MgUnitType::USEnglish);

        // Create some MapPlots
        Ptr<MgMapPlot> mapPlot1 = new MgMapPlot(map1, plotSpec1, layout);

        Ptr<MgCoordinate> center = new MgCoordinateXY(-100, 40);
        double scale = 50e+6;
        Ptr<MgMapPlot> mapPlot2 = new MgMapPlot(map2, center, scale, plotSpec2, layout);

        Ptr<MgCoordinate> ll = new MgCoordinateXY(-130, 45);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(-100, 60);
        Ptr<MgEnvelope> extent = new MgEnvelope(ll, ur);
        Ptr<MgMapPlot> mapPlot3 = new MgMapPlot(map3, extent, false, plotSpec3, layout);

        Ptr<MgMapPlot> mapPlot4 = new MgMapPlot(map4, extent, true, plotSpec4, layout);

        // Setup the MapPlot collection
        Ptr<MgMapPlotCollection> mapPlots = new MgMapPlotCollection();
        mapPlots->Add(mapPlot1);
        mapPlots->Add(mapPlot2);
        mapPlots->Add(mapPlot3);
        mapPlots->Add(mapPlot4);

        // call the API
        Ptr<MgByteReader> eplot = m_svcMapping->GenerateMultiPlot(mapPlots, version);

        INT64 len = eplot->GetLength();

        //CPPUNIT_ASSERT(len == 7000); // TODO: determine correct length

        //Ptr<MgByteSink> byteSink = new MgByteSink(eplot);
        //byteSink->ToFile(L"UTNewMultiPlot.dwf");
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


void TestMappingService::TestCase_GetPlotUsingCurrentCenterAndScale()
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map1 = new MgMap(m_siteConnection);
        map1->Create(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(400e+6);

        Ptr<MgDwfVersion> version = new MgDwfVersion();

        Ptr<MgResourceIdentifier> allElementsLayout = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");

        Ptr<MgPlotSpecification> plotSpec = new MgPlotSpecification(8.5f, 11.f, L"inches", .5, .5, .5, .5);
        Ptr<MgCoordinate> center = new MgCoordinateXY(-120, 40);
        double scale = 20e+6;
        Ptr<MgCoordinate> ll = new MgCoordinateXY(-180, 0);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(0, 90);
        MgEnvelope extents(ll, ur);
        Ptr<MgLayout> layout = new MgLayout(allElementsLayout, L"TestTitle", MgUnitType::USEnglish);

        // call the API
        Ptr<MgByteReader> eplot = m_svcMapping->GeneratePlot(map1, plotSpec, NULL, version);
        //Ptr<MgByteReader> eplot = m_svcMapping->GeneratePlot(map1, plotSpec, layout, version);

        INT64 len = eplot->GetLength();

        // CPPUNIT_ASSERT(len == 7000); // TODO: determine correct length

        //Ptr<MgByteSink> byteSink = new MgByteSink(eplot);
        //byteSink->ToFile(L"UTNewCurrentCentreAndScalePlot.dwf");
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


void TestMappingService::TestCase_GetPlotUsingOverriddenCenterAndScale()
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map1 = new MgMap(m_siteConnection);
        map1->Create(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(400e+6);

        Ptr<MgDwfVersion> version = new MgDwfVersion();

        Ptr<MgResourceIdentifier> allElementsLayout = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");

        Ptr<MgPlotSpecification> plotSpec = new MgPlotSpecification(8.f, 11.f, L"inches", .5, .5, .5, .5);
        Ptr<MgCoordinate> center = new MgCoordinateXY(-120, 40);
        double scale = 20e+6;
        Ptr<MgCoordinate> ll = new MgCoordinateXY(-180, 0);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(0, 90);
        MgEnvelope extents(ll, ur);
        Ptr<MgLayout> layout = new MgLayout(allElementsLayout, L"TestTitle", MgUnitType::USEnglish);

        // call the API
        Ptr<MgByteReader> eplot = m_svcMapping->GeneratePlot(map1, center, scale, plotSpec, layout, version);

        INT64 len = eplot->GetLength();

        // CPPUNIT_ASSERT(len == 7000); // TODO: determine correct length

        //Ptr<MgByteSink> byteSink = new MgByteSink(eplot);
        //byteSink->ToFile(L"UTNewOveriddenCenterAndScalePlot.dwf");
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


void TestMappingService::TestCase_GetPlotUsingExtents()
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map1 = new MgMap(m_siteConnection);
        map1->Create(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(400e+6);

        Ptr<MgDwfVersion> version = new MgDwfVersion();

        Ptr<MgResourceIdentifier> allElementsLayout = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");

        Ptr<MgPlotSpecification> plotSpec = new MgPlotSpecification(11.f, 8.5f, L"inches", .5, .5, .5, .5);
        Ptr<MgCoordinate> center = new MgCoordinateXY(-120, 40);
        double scale = 20e+6;
        Ptr<MgCoordinate> ll = new MgCoordinateXY(-180, 0);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(0, 90);
        Ptr<MgEnvelope> extents = new MgEnvelope(ll, ur);
        Ptr<MgLayout> layout = new MgLayout(allElementsLayout, L"TestTitle", MgUnitType::USEnglish);

        // call the API
        Ptr<MgByteReader> eplot = m_svcMapping->GeneratePlot(map1, extents, false, plotSpec, layout, version);

        INT64 len = eplot->GetLength();

        // CPPUNIT_ASSERT(len == 7000); // TODO: determine correct length

        //Ptr<MgByteSink> byteSink = new MgByteSink(eplot);
        //byteSink->ToFile(L"UTNewExtentsPlot.dwf");
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


void TestMappingService::TestCase_GetPlotUsingExtentsAndExpandToFit()
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map1 = new MgMap(m_siteConnection);
        map1->Create(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(400e+6);

        Ptr<MgDwfVersion> version = new MgDwfVersion();

        Ptr<MgResourceIdentifier> allElementsLayout = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");

        Ptr<MgPlotSpecification> plotSpec = new MgPlotSpecification(8.f, 11.f, L"inches", .5, .5, .5, .5);
        Ptr<MgCoordinate> center = new MgCoordinateXY(-120, 40);
        double scale = 20e+6;
        Ptr<MgCoordinate> ll = new MgCoordinateXY(-180, 0);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(0, 90);
        Ptr<MgEnvelope> extents = new MgEnvelope(ll, ur);
        Ptr<MgLayout> layout = new MgLayout(allElementsLayout, L"TestTitle", MgUnitType::USEnglish);

        // call the API
        Ptr<MgByteReader> eplot = m_svcMapping->GeneratePlot(map1, extents, true, plotSpec, NULL, version);

        INT64 len = eplot->GetLength();

        // CPPUNIT_ASSERT(len == 7000); // TODO: determine correct length

        //Ptr<MgByteSink> byteSink = new MgByteSink(eplot);
        //byteSink->ToFile(L"UTNewExtentsExpandToFitPlot.dwf");
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


void TestMappingService::TestCase_GetLegendPlot()
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map1 = new MgMap(m_siteConnection);
        map1->Create(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(200e+6);

        Ptr<MgDwfVersion> version = new MgDwfVersion();

        Ptr<MgPlotSpecification> plotSpec = new MgPlotSpecification(8.5f, 11.f, L"in", 1, 1, 1, 1);

        double dMapScale = 200e+6;
        // call the API
        Ptr<MgByteReader> eplot = m_svcMapping->GenerateLegendPlot(map1, dMapScale, plotSpec, version);

        INT64 len = eplot->GetLength();

        //CPPUNIT_ASSERT(len == 7000); // TODO: determine correct length

        //Ptr<MgByteSink> byteSink = new MgByteSink(eplot);
        //byteSink->ToFile(L"UTNewLegendPlot.dwf");
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

void TestMappingService::TestCase_GetLegendImage()
{
    try
    {
        Ptr<MgResourceIdentifier> ldf = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        Ptr<MgByteReader> rdr = m_svcMapping->GenerateLegendImage(ldf, 10000.0, 16, 16, MgImageFormats::Png, 3, 0);
        Ptr<MgByteSink> sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/GenerateLegendImage_Parcels_16x16_PNG.png");

        rdr = m_svcMapping->GenerateLegendImage(ldf, 10000.0, 16, 16, MgImageFormats::Png8, 3, 0);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/GenerateLegendImage_Parcels_16x16_PNG8.png");

        rdr = m_svcMapping->GenerateLegendImage(ldf, 10000.0, 16, 16, MgImageFormats::Jpeg, 3, 0);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/GenerateLegendImage_Parcels_16x16_JPG.jpg");

        rdr = m_svcMapping->GenerateLegendImage(ldf, 10000.0, 16, 16, MgImageFormats::Gif, 3, 0);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/GenerateLegendImage_Parcels_16x16_GIF.gif");
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

void TestMappingService::TestCase_GetLegendImagePointStyleWithConstRotations()
{
    try
    {
        Ptr<MgResourceIdentifier> ldfId = new MgResourceIdentifier(L"Library://UnitTests/Layers/RotatedPointStyles.LayerDefinition");

        //Do 16x16 icons first. Our common scenario.

        Ptr<MgByteReader> rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 0);
        Ptr<MgByteSink> sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Square_0_16x16.png");
        
        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 1);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Square_45_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 2);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Square_25_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 3);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Star_0_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 4);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Star_45_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 5);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Star_25_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 6);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Triangle_0_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 7);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Triangle_45_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 8);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Triangle_25_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 9);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Cross_0_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 10);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Cross_45_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 11);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Cross_25_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 12);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_XMark_0_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 13);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_XMark_45_16x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 16, 16, MgImageFormats::Png, 1, 14);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_XMark_25_16x16.png");

        //Now try 32x16 to see if the rotation handling is acceptable with non-square sizes

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 0);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Square_0_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 1);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Square_45_32x16.png");
        
        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 2);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Square_25_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 3);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Star_0_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 4);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Star_45_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 5);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Star_25_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 6);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Triangle_0_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 7);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Triangle_45_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 8);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Triangle_25_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 9);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Cross_0_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 10);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Cross_45_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 11);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_Cross_25_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 12);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_XMark_0_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 13);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_XMark_45_32x16.png");

        rdr = m_svcMapping->GenerateLegendImage(ldfId, 1000.0, 32, 16, MgImageFormats::Png, 1, 14);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPoint_XMark_25_32x16.png");
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

void TestMappingService::TestCase_GetLegendImageConvenience()
{
    try
    {
        Ptr<MgResourceIdentifier> mapres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mapres, L"TestCase_GetLegendImageConvenience");

        Ptr<MgResourceIdentifier> ldf = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(ldf, m_svcResource);
        layer->SetName(L"TestCase_GetLegendImageConvenience");
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Insert(0, layer);

        Ptr<MgIntCollection> types = layer->GetGeometryTypeStyles(10000.0);
        CPPUNIT_ASSERT(1 == types->GetCount());
        CPPUNIT_ASSERT(types->IndexOf(1) < 0);
        CPPUNIT_ASSERT(types->IndexOf(2) < 0);
        CPPUNIT_ASSERT(types->IndexOf(3) >= 0);
        CPPUNIT_ASSERT(types->IndexOf(4) < 0);

        CPPUNIT_ASSERT(8 == layer->GetThemeCategoryCount(10000.0, 3));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(10000.0, 1));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(10000.0, 2));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(10000.0, 4));

        types = layer->GetGeometryTypeStyles(14000.0);
        CPPUNIT_ASSERT(NULL == types.p);
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(14000.0, 3));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(14000.0, 1));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(14000.0, 2));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(14000.0, 4));

        Ptr<MgByteReader> rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Png, 3, 0);
        Ptr<MgByteSink> sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/GenerateLegendImageConvenience_Parcels_16x16_PNG.png");

        rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Png8, 3, 0);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/GenerateLegendImageConvenience_Parcels_16x16_PNG8.png");

        rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Jpeg, 3, 0);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/GenerateLegendImageConvenience_Parcels_16x16_JPG.jpg");

        rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Gif, 3, 0);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/GenerateLegendImageConvenience_Parcels_16x16_GIF.gif");
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

void TestMappingService::TestCase_GetLegendImageCompositeConvenience()
{
    try
    {
        Ptr<MgResourceIdentifier> mapres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mapres, L"TestCase_GetLegendImageCompositeConvenience");

        Ptr<MgResourceIdentifier> ldf = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTS.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(ldf, m_svcResource);
        layer->SetName(L"TestCase_GetLegendImageCompositeConvenience");
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Insert(0, layer);

        Ptr<MgIntCollection> types = layer->GetGeometryTypeStyles(10000.0);
        CPPUNIT_ASSERT(3 == types->GetCount());
        CPPUNIT_ASSERT(types->IndexOf(1) < 0);
        CPPUNIT_ASSERT(types->IndexOf(2) < 0);
        CPPUNIT_ASSERT(types->IndexOf(3) < 0);
        CPPUNIT_ASSERT(types->IndexOf(4) >= 0);
        for (INT32 i = 0; i < types->GetCount(); i++)
        {
            CPPUNIT_ASSERT(4 == types->GetItem(i));
        }

        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(10000.0, 1));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(10000.0, 2));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(10000.0, 3));
        CPPUNIT_ASSERT(1 == layer->GetThemeCategoryCount(10000.0, 4));
        CPPUNIT_ASSERT(1 == layer->GetCompositeThemeCategoryCount(10000.0, 0));
        CPPUNIT_ASSERT(1 == layer->GetCompositeThemeCategoryCount(10000.0, 1));
        CPPUNIT_ASSERT(1 == layer->GetCompositeThemeCategoryCount(10000.0, 2));
        CPPUNIT_ASSERT(-1 == layer->GetCompositeThemeCategoryCount(10000.0, 3));

        INT32 rulesProcessed = 0;
        for (INT32 ctype = 0; ctype < 3; ctype++)
        {
            INT32 rules = layer->GetCompositeThemeCategoryCount(10000.0, ctype);
            for (INT32 offset = rulesProcessed; offset < (rulesProcessed + rules); offset++)
            {
                STRING prefix = L"../UnitTestFiles/GenerateLegendImageConvenience_MultiCTS_type";
                STRING sNum;
                MgUtil::Int32ToString(ctype, sNum);
                prefix += sNum;
                prefix += L"_offset";
                MgUtil::Int32ToString(offset, sNum);
                prefix += sNum;

                STRING fileNamePNG = prefix;
                fileNamePNG += L"_16x16_PNG.png";
                Ptr<MgByteReader> rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Png, 4, offset);
                Ptr<MgByteSink> sink = new MgByteSink(rdr);
                sink->ToFile(fileNamePNG);

                STRING fileNamePNG8 = prefix;
                fileNamePNG8 += L"_16x16_PNG8.png";
                rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Png8, 4, offset);
                sink = new MgByteSink(rdr);
                sink->ToFile(fileNamePNG8);

                STRING fileNameJPG = prefix;
                fileNameJPG += L"_16x16_JPG.jpg";
                rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Jpeg, 4, offset);
                sink = new MgByteSink(rdr);
                sink->ToFile(fileNameJPG);

                STRING fileNameGIF = prefix;
                fileNameGIF += L"_16x16_JPG.jpg";
                rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Gif, 4, offset);
                sink = new MgByteSink(rdr);
                sink->ToFile(fileNameGIF);
            }
            rulesProcessed += rules;
        }
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

void TestMappingService::TestCase_GetLegendImageCompositeThemedConvenience()
{
    try
    {
        Ptr<MgResourceIdentifier> mapres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mapres, L"TestCase_GetLegendImageCompositeThemedConvenience");

        Ptr<MgResourceIdentifier> ldf = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTSWithTheme.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(ldf, m_svcResource);
        layer->SetName(L"TestCase_GetLegendImageCompositeThemedConvenience");
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Insert(0, layer);

        Ptr<MgIntCollection> types = layer->GetGeometryTypeStyles(10000.0);
        CPPUNIT_ASSERT(3 == types->GetCount());
        CPPUNIT_ASSERT(types->IndexOf(1) < 0);
        CPPUNIT_ASSERT(types->IndexOf(2) < 0);
        CPPUNIT_ASSERT(types->IndexOf(3) < 0);
        CPPUNIT_ASSERT(types->IndexOf(4) >= 0);
        for (INT32 i = 0; i < types->GetCount(); i++)
        {
            CPPUNIT_ASSERT(4 == types->GetItem(i));
        }

        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(10000.0, 1));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(10000.0, 2));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(10000.0, 3));
        CPPUNIT_ASSERT(1 == layer->GetThemeCategoryCount(10000.0, 4));
        CPPUNIT_ASSERT(1 == layer->GetCompositeThemeCategoryCount(10000.0, 0));
        CPPUNIT_ASSERT(3 == layer->GetCompositeThemeCategoryCount(10000.0, 1));
        CPPUNIT_ASSERT(1 == layer->GetCompositeThemeCategoryCount(10000.0, 2));
        CPPUNIT_ASSERT(-1 == layer->GetCompositeThemeCategoryCount(10000.0, 3));

        INT32 rulesProcessed = 0;
        for (INT32 ctype = 0; ctype < 3; ctype++)
        {
            INT32 rules = layer->GetCompositeThemeCategoryCount(10000.0, ctype);
            for (INT32 offset = rulesProcessed; offset < (rulesProcessed + rules); offset++)
            {
                STRING prefix = L"../UnitTestFiles/GenerateLegendImageConvenience_MultiCTSWithTheme_type";
                STRING sType;
                MgUtil::Int32ToString(ctype, sType);
                prefix += sType;
                prefix += L"_offset";
                STRING sOffset;
                MgUtil::Int32ToString(offset, sOffset);
                prefix += sOffset;

                STRING fileNamePNG = prefix;
                fileNamePNG += L"_16x16_PNG.png";
                Ptr<MgByteReader> rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Png, 4, offset);
                Ptr<MgByteSink> sink = new MgByteSink(rdr);
                sink->ToFile(fileNamePNG);

                STRING fileNamePNG8 = prefix;
                fileNamePNG8 += L"_16x16_PNG8.png";
                rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Png8, 4, offset);
                sink = new MgByteSink(rdr);
                sink->ToFile(fileNamePNG8);

                STRING fileNameJPG = prefix;
                fileNameJPG += L"_16x16_JPG.jpg";
                rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Jpeg, 4, offset);
                sink = new MgByteSink(rdr);
                sink->ToFile(fileNameJPG);

                STRING fileNameGIF = prefix;
                fileNameGIF += L"_16x16_JPG.jpg";
                rdr = layer->GenerateLegendImage(10000.0, 16, 16, MgImageFormats::Gif, 4, offset);
                sink = new MgByteSink(rdr);
                sink->ToFile(fileNameGIF);
            }
            rulesProcessed += rules;
        }
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

void TestMappingService::TestCase_GetLegendImagePointStyleWithConstRotationsConvenience()
{
    try
    {
        Ptr<MgResourceIdentifier> mapres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mapres, L"TestCase_GetLegendImageConvenience");

        Ptr<MgResourceIdentifier> ldfId = new MgResourceIdentifier(L"Library://UnitTests/Layers/RotatedPointStyles.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(ldfId, m_svcResource);
        layer->SetName(L"TestCase_GetLegendImagePointStyleWithConstRotationsConvenience");
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Insert(0, layer);

        Ptr<MgIntCollection> types = layer->GetGeometryTypeStyles(1000.0);
        CPPUNIT_ASSERT(1 == types->GetCount());
        CPPUNIT_ASSERT(types->IndexOf(1) >= 0);
        CPPUNIT_ASSERT(types->IndexOf(2) < 0);
        CPPUNIT_ASSERT(types->IndexOf(3) < 0);
        CPPUNIT_ASSERT(types->IndexOf(4) < 0);

        CPPUNIT_ASSERT(15 == layer->GetThemeCategoryCount(1000.0, 1));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(1000.0, 2));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(1000.0, 3));
        CPPUNIT_ASSERT(-1 == layer->GetThemeCategoryCount(1000.0, 4));
        
        //Do 16x16 icons first. Our common scenario.

        Ptr<MgByteReader> rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 0);
        Ptr<MgByteSink> sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Square_0_16x16.png");
        
        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 1);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Square_45_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 2);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Square_25_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 3);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Star_0_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 4);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Star_45_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 5);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Star_25_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 6);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Triangle_0_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 7);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Triangle_45_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 8);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Triangle_25_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 9);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Cross_0_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 10);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Cross_45_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 11);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Cross_25_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 12);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_XMark_0_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 13);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_XMark_45_16x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 16, 16, MgImageFormats::Png, 1, 14);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_XMark_25_16x16.png");

        //Now try 32x16 to see if the rotation handling is acceptable with non-square sizes

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 0);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Square_0_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 1);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Square_45_32x16.png");
        
        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 2);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Square_25_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 3);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Star_0_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 4);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Star_45_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 5);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Star_25_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 6);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Triangle_0_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 7);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Triangle_45_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 8);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Triangle_25_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 9);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Cross_0_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 10);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Cross_45_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 11);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_Cross_25_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 12);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_XMark_0_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 13);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_XMark_45_32x16.png");

        rdr = layer->GenerateLegendImage(1000.0, 32, 16, MgImageFormats::Png, 1, 14);
        sink = new MgByteSink(rdr);
        sink->ToFile(L"../UnitTestFiles/RotatedPointConvenience_XMark_25_32x16.png");
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

void TestMappingService::TestCase_QueryFeaturesImageMap()
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestSheboygan");

        Ptr<MgCoordinate> c1 = new MgCoordinateXY(-180,0);
        Ptr<MgCoordinate> c2 = new MgCoordinateXY(0,90);
        Ptr<MgEnvelope> extent = new MgEnvelope(c1, c2);

        map->SetDataExtent(extent);

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-90,45);
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);

        map->SetViewScale(100000000);

        map->SetDisplayDpi(96);
        map->SetDisplayWidth(800);
        map->SetDisplayHeight(600);

        // call the API
        Ptr<MgByteReader> features = m_svcMapping->QueryFeatures(map, L"Parcels", L"Display"/*MgCoordinateSpace::Display*/);

        // delete result from QueryFeatures test
        Ptr<MgResourceIdentifier> qfres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/Parcels.FeatureSource");
        m_svcResource->DeleteResource(qfres1);
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
