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
#include "TestRenderingService.h"
#include "ServiceManager.h"
#include "ServerSiteService.h"
#include "StylizationDefs.h"
#include "../Common/Manager/FdoConnectionManager.h"
//#include "GDRenderer.h"
//#include "AGGRenderer.h"
#include "FoundationDefs.h"
#include "SE_Renderer.h"

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestRenderingService, "TestRenderingService");


TestRenderingService::TestRenderingService()
{
    // Initialize service objects.
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();

    m_svcResource = dynamic_cast<MgResourceService*>(
        serviceManager->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcRendering = dynamic_cast<MgRenderingService*>(
        serviceManager->RequestService(MgServiceType::RenderingService));
    assert(m_svcRendering != NULL);

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

    STRING session = svcSite->CreateSession();
    assert(!session.empty());
    userInfo->SetMgSessionId(session);

    // Set the current MgUserInformation
    MgUserInformation::SetCurrentUserInfo(userInfo);

    m_siteConnection = new MgSiteConnection();
    m_siteConnection->Open(userInfo);
}


TestRenderingService::~TestRenderingService()
{
}


void TestRenderingService::setUp()
{
}


void TestRenderingService::tearDown()
{
}


void TestRenderingService::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Rendering Service tests.\n")));

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

        // publish the map definition
        Ptr<MgResourceIdentifier> mapres1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgByteSource> mdfsrc1 = new MgByteSource(L"../UnitTestFiles/UT_Sheboygan.mdf", false);
        Ptr<MgByteReader> mdfrdr1 = mdfsrc1->GetReader();
        m_svcResource->SetResource(mapres1, mdfrdr1, NULL);

        // publish tile set
        Ptr<MgResourceIdentifier> tilesetres1 = new MgResourceIdentifier(L"Library://UnitTests/TileSets/XYZ.TileSetDefinition");
        Ptr<MgByteSource> tsdsrc1 = new MgByteSource(L"../UnitTestFiles/UT_XYZ.tsd", false);
        Ptr<MgByteReader> tsdrdr1 = tsdsrc1->GetReader();
        m_svcResource->SetResource(tilesetres1, tsdrdr1, NULL);

        // publish the layer definitions
        Ptr<MgResourceIdentifier> ldfres1 = new MgResourceIdentifier(L"Library://UnitTests/Layers/HydrographicPolygons.LayerDefinition");
        Ptr<MgByteSource> ldfsrc1 = new MgByteSource(L"../UnitTestFiles/UT_HydrographicPolygons.ldf", false);
        Ptr<MgByteReader> ldfrdr1 = ldfsrc1->GetReader();
        m_svcResource->SetResource(ldfres1, ldfrdr1, NULL);

        Ptr<MgResourceIdentifier> ldfres2 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Rail.LayerDefinition");
        Ptr<MgByteSource> ldfsrc2 = new MgByteSource(L"../UnitTestFiles/UT_Rail.ldf", false);
        Ptr<MgByteReader> ldfrdr2 = ldfsrc2->GetReader();
        m_svcResource->SetResource(ldfres2, ldfrdr2, NULL);

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        Ptr<MgByteSource> ldfsrc3 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.ldf", false);
        Ptr<MgByteReader> ldfrdr3 = ldfsrc3->GetReader();
        m_svcResource->SetResource(ldfres3, ldfrdr3, NULL);

        // publish the feature sources
        Ptr<MgResourceIdentifier> fsres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/HydrographicPolygons.FeatureSource");
        Ptr<MgByteSource> fssrc1 = new MgByteSource(L"../UnitTestFiles/UT_HydrographicPolygons.fs", false);
        Ptr<MgByteReader> fsrdr1 = fssrc1->GetReader();
        m_svcResource->SetResource(fsres1, fsrdr1, NULL);

        Ptr<MgResourceIdentifier> fsres2 = new MgResourceIdentifier(L"Library://UnitTests/Data/Rail.FeatureSource");
        Ptr<MgByteSource> fssrc2 = new MgByteSource(L"../UnitTestFiles/UT_Rail.fs", false);
        Ptr<MgByteReader> fsrdr2 = fssrc2->GetReader();
        m_svcResource->SetResource(fsres2, fsrdr2, NULL);

        Ptr<MgResourceIdentifier> fsres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/Parcels.FeatureSource");
        Ptr<MgByteSource> fssrc3 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.fs", false);
        Ptr<MgByteReader> fsrdr3 = fssrc3->GetReader();
        m_svcResource->SetResource(fsres3, fsrdr3, NULL);

        // publish the resource data
        Ptr<MgByteSource> dataSource1 = new MgByteSource(L"../UnitTestFiles/UT_HydrographicPolygons.sdf", false);
        Ptr<MgByteReader> dataReader1 = dataSource1->GetReader();
        m_svcResource->SetResourceData(fsres1, L"UT_HydrographicPolygons.sdf", L"File", dataReader1);

        Ptr<MgByteSource> dataSource2 = new MgByteSource(L"../UnitTestFiles/UT_Rail.sdf", false);
        Ptr<MgByteReader> dataReader2 = dataSource2->GetReader();
        m_svcResource->SetResourceData(fsres2, L"UT_Rail.sdf", L"File", dataReader2);

        Ptr<MgByteSource> dataSource3 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.sdf", false);
        Ptr<MgByteReader> dataReader3 = dataSource3->GetReader();
        m_svcResource->SetResourceData(fsres3, L"UT_Parcels.sdf", L"File", dataReader3);

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

        //
        // publish symbology stuff
        //

        // the point feature source
        Ptr<MgResourceIdentifier> fsres4 = new MgResourceIdentifier(L"Library://UnitTests/Data/Capitals.FeatureSource");
        Ptr<MgByteSource> fssrc4 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPoints.fs", false);
        Ptr<MgByteReader> fsrdr4 = fssrc4->GetReader();
        m_svcResource->SetResource(fsres4, fsrdr4, NULL);

        // point sdf file
        Ptr<MgByteSource> dataSource4 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPoints.sdf", false);
        Ptr<MgByteReader> dataReader4 = dataSource4->GetReader();
        m_svcResource->SetResourceData(fsres4, L"UT_SymbologyPoints.sdf", L"File", dataReader4);

        // point symbols
        Ptr<MgResourceIdentifier> sdres1 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/PointSymbol.SymbolDefinition");
        Ptr<MgByteSource> sdsrc1 = new MgByteSource(L"../UnitTestFiles/symbol.sd", false);
        Ptr<MgByteReader> sdrdr1 = sdsrc1->GetReader();
        m_svcResource->SetResource(sdres1, sdrdr1, NULL);

        Ptr<MgResourceIdentifier> sdres2 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/PointSymbolParam.SymbolDefinition");
        Ptr<MgByteSource> sdsrc2 = new MgByteSource(L"../UnitTestFiles/symbolp.sd", false);
        Ptr<MgByteReader> sdrdr2 = sdsrc2->GetReader();
        m_svcResource->SetResource(sdres2, sdrdr2, NULL);

        // point ldf
        Ptr<MgResourceIdentifier> ldfres4 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Capitals.LayerDefinition");
        Ptr<MgByteSource> ldfsrc4 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPoints.ldf", false);
        Ptr<MgByteReader> ldfrdr4 = ldfsrc4->GetReader();
        m_svcResource->SetResource(ldfres4, ldfrdr4, NULL);

        Ptr<MgResourceIdentifier> ldfres5 = new MgResourceIdentifier(L"Library://UnitTests/Layers/CapitalsParam.LayerDefinition");
        Ptr<MgByteSource> ldfsrc5 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPointsParam.ldf", false);
        Ptr<MgByteReader> ldfrdr5 = ldfsrc5->GetReader();
        m_svcResource->SetResource(ldfres5, ldfrdr5, NULL);

        // point mdf
        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Capitals.MapDefinition");
        Ptr<MgByteSource> mdfsrc2 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPoints.mdf", false);
        Ptr<MgByteReader> mdfrdr2 = mdfsrc2->GetReader();
        m_svcResource->SetResource(mapres2, mdfrdr2, NULL);

        Ptr<MgResourceIdentifier> mapres3 = new MgResourceIdentifier(L"Library://UnitTests/Maps/CapitalsParam.MapDefinition");
        Ptr<MgByteSource> mdfsrc3 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPointsParam.mdf", false);
        Ptr<MgByteReader> mdfrdr3 = mdfsrc3->GetReader();
        m_svcResource->SetResource(mapres3, mdfrdr3, NULL);

        // the line feature source
        Ptr<MgResourceIdentifier> fsres5 = new MgResourceIdentifier(L"Library://UnitTests/Data/Lines.FeatureSource");
        Ptr<MgByteSource> fssrc5 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLines.fs", false);
        Ptr<MgByteReader> fsrdr5 = fssrc5->GetReader();
        m_svcResource->SetResource(fsres5, fsrdr5, NULL);

        // line sdf file
        Ptr<MgByteSource> dataSource5 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLines.sdf", false);
        Ptr<MgByteReader> dataReader5 = dataSource5->GetReader();
        m_svcResource->SetResourceData(fsres5, L"UT_SymbologyLines.sdf", L"File", dataReader5);

        // line symbols
        Ptr<MgResourceIdentifier> sdres3 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/LineSymbol.SymbolDefinition");
        Ptr<MgByteSource> sdsrc3 = new MgByteSource(L"../UnitTestFiles/linesymbol.sd", false);
        Ptr<MgByteReader> sdrdr3 = sdsrc3->GetReader();
        m_svcResource->SetResource(sdres3, sdrdr3, NULL);

        Ptr<MgResourceIdentifier> sdres4 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/MTYP1500a.SymbolDefinition");
        Ptr<MgByteSource> sdsrc4 = new MgByteSource(L"../UnitTestFiles/MTYP1500a.sd", false);
        Ptr<MgByteReader> sdrdr4 = sdsrc4->GetReader();
        m_svcResource->SetResource(sdres4, sdrdr4, NULL);

        // line ldf
        Ptr<MgResourceIdentifier> ldfres6 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Lines.LayerDefinition");
        Ptr<MgByteSource> ldfsrc6 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLines.ldf", false);
        Ptr<MgByteReader> ldfrdr6 = ldfsrc6->GetReader();
        m_svcResource->SetResource(ldfres6, ldfrdr6, NULL);

        Ptr<MgResourceIdentifier> ldfres7 = new MgResourceIdentifier(L"Library://UnitTests/Layers/LinesCrossTick.LayerDefinition");
        Ptr<MgByteSource> ldfsrc7 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLinesCrossTick.ldf", false);
        Ptr<MgByteReader> ldfrdr7 = ldfsrc7->GetReader();
        m_svcResource->SetResource(ldfres7, ldfrdr7, NULL);

        // line mdf
        Ptr<MgResourceIdentifier> mapres4 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Lines.MapDefinition");
        Ptr<MgByteSource> mdfsrc4 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLines.mdf", false);
        Ptr<MgByteReader> mdfrdr4 = mdfsrc4->GetReader();
        m_svcResource->SetResource(mapres4, mdfrdr4, NULL);

        Ptr<MgResourceIdentifier> mapres5 = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinesCrossTick.MapDefinition");
        Ptr<MgByteSource> mdfsrc5 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLinesCrossTick.mdf", false);
        Ptr<MgByteReader> mdfrdr5 = mdfsrc5->GetReader();
        m_svcResource->SetResource(mapres5, mdfrdr5, NULL);

        // annotation ldf - this shares the point sdf
        Ptr<MgResourceIdentifier> ldfres8 = new MgResourceIdentifier(L"Library://UnitTests/Layers/UT_Annotation1.LayerDefinition");
        Ptr<MgByteSource> ldfsrc8 = new MgByteSource(L"../UnitTestFiles/UT_Annotation1.ldf", false);
        Ptr<MgByteReader> ldfrdr8 = ldfsrc8->GetReader();
        m_svcResource->SetResource(ldfres8, ldfrdr8, NULL);

        Ptr<MgResourceIdentifier> ldfres9 = new MgResourceIdentifier(L"Library://UnitTests/Layers/UT_Annotation2.LayerDefinition");
        Ptr<MgByteSource> ldfsrc9 = new MgByteSource(L"../UnitTestFiles/UT_Annotation2.ldf", false);
        Ptr<MgByteReader> ldfrdr9 = ldfsrc9->GetReader();
        m_svcResource->SetResource(ldfres9, ldfrdr9, NULL);

        Ptr<MgResourceIdentifier> ldfres10 = new MgResourceIdentifier(L"Library://UnitTests/Layers/UT_Annotation3.LayerDefinition");
        Ptr<MgByteSource> ldfsrc10 = new MgByteSource(L"../UnitTestFiles/UT_Annotation3.ldf", false);
        Ptr<MgByteReader> ldfrdr10 = ldfsrc10->GetReader();
        m_svcResource->SetResource(ldfres10, ldfrdr10, NULL);

        // annotation mdf
        Ptr<MgResourceIdentifier> mapres8 = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_Annotation1.MapDefinition");
        Ptr<MgByteSource> mdfsrc8 = new MgByteSource(L"../UnitTestFiles/UT_Annotation1.mdf", false);
        Ptr<MgByteReader> mdfrdr8 = mdfsrc8->GetReader();
        m_svcResource->SetResource(mapres8, mdfrdr8, NULL);

        Ptr<MgResourceIdentifier> mapres9 = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_Annotation2.MapDefinition");
        Ptr<MgByteSource> mdfsrc9 = new MgByteSource(L"../UnitTestFiles/UT_Annotation2.mdf", false);
        Ptr<MgByteReader> mdfrdr9 = mdfsrc9->GetReader();
        m_svcResource->SetResource(mapres9, mdfrdr9, NULL);

        Ptr<MgResourceIdentifier> mapres10 = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_Annotation3.MapDefinition");
        Ptr<MgByteSource> mdfsrc10 = new MgByteSource(L"../UnitTestFiles/UT_Annotation3.mdf", false);
        Ptr<MgByteReader> mdfrdr10 = mdfsrc10->GetReader();
        m_svcResource->SetResource(mapres10, mdfrdr10, NULL);

        //symbology - polygons
        Ptr<MgResourceIdentifier> mapres11 = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_SymbologyPolygons.MapDefinition");
        Ptr<MgByteSource> mdfsrc11 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPolygons.mdf", false);
        Ptr<MgByteReader> mdfrdr11 = mdfsrc11->GetReader();
        m_svcResource->SetResource(mapres11, mdfrdr11, NULL);

        Ptr<MgResourceIdentifier> ldfres11 = new MgResourceIdentifier(L"Library://UnitTests/Layers/SymbologyParcels.LayerDefinition");
        Ptr<MgByteSource> ldfsrc11 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyParcels.ldf", false);
        Ptr<MgByteReader> ldfrdr11 = ldfsrc11->GetReader();
        m_svcResource->SetResource(ldfres11, ldfrdr11, NULL);

        Ptr<MgResourceIdentifier> sdres5 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/AreaSymbol.SymbolDefinition");
        Ptr<MgByteSource> sdsrc5 = new MgByteSource(L"../UnitTestFiles/areasymbol.sd", false);
        Ptr<MgByteReader> sdrdr5 = sdsrc5->GetReader();
        m_svcResource->SetResource(sdres5, sdrdr5, NULL);

        // For watermark test
        // publish the map definition
        Ptr<MgResourceIdentifier> mapres12 = new MgResourceIdentifier(L"Library://UnitTests/Maps/SheboyganWithWatermark.MapDefinition");
        Ptr<MgByteSource> mdfsrc12 = new MgByteSource(L"../UnitTestFiles/UT_SheboyganWithWatermark.mdf", false);
        Ptr<MgByteReader> mdfrdr12 = mdfsrc12->GetReader();
        m_svcResource->SetResource(mapres12, mdfrdr12, NULL);

        // publish the watermark definition
        Ptr<MgResourceIdentifier> wdfres1 = new MgResourceIdentifier(L"Library://UnitTests/Watermarks/PoweredByMapGuide.WatermarkDefinition");
        Ptr<MgByteSource> wdfsrc1 = new MgByteSource(L"../UnitTestFiles/UT_PoweredByMapGuide.wdf", false);
        Ptr<MgByteReader> wdfrdr1 = wdfsrc1->GetReader();
        m_svcResource->SetResource(wdfres1, wdfrdr1, NULL);

        // ------------------------------------------------------
        // base map source data
        // ------------------------------------------------------
        // publish the map definition
        Ptr<MgResourceIdentifier> mapres13 = new MgResourceIdentifier(L"Library://UnitTests/Maps/BaseMap.MapDefinition");
        Ptr<MgByteSource> mdfsrc13 = new MgByteSource(L"../UnitTestFiles/UT_BaseMap.mdf", false);
        Ptr<MgByteReader> mdfrdr13 = mdfsrc13->GetReader();
        m_svcResource->SetResource(mapres13, mdfrdr13, NULL);

        // publish the layer definitions
        Ptr<MgResourceIdentifier> ldfres12 = new MgResourceIdentifier(L"Library://UnitTests/Layers/RoadCenterLines.LayerDefinition");
        Ptr<MgByteSource> ldfsrc12 = new MgByteSource(L"../UnitTestFiles/UT_RoadCenterLines.ldf", false);
        Ptr<MgByteReader> ldfrdr12 = ldfsrc12->GetReader();
        m_svcResource->SetResource(ldfres12, ldfrdr12, NULL);

        Ptr<MgResourceIdentifier> ldfres13 = new MgResourceIdentifier(L"Library://UnitTests/Layers/VotingDistricts.LayerDefinition");
        Ptr<MgByteSource> ldfsrc13 = new MgByteSource(L"../UnitTestFiles/UT_VotingDistricts.ldf", false);
        Ptr<MgByteReader> ldfrdr13 = ldfsrc13->GetReader();
        m_svcResource->SetResource(ldfres13, ldfrdr13, NULL);

        // publish the feature sources
        Ptr<MgResourceIdentifier> fsres6 = new MgResourceIdentifier(L"Library://UnitTests/Data/RoadCenterLines.FeatureSource");
        Ptr<MgByteSource> fssrc6 = new MgByteSource(L"../UnitTestFiles/UT_RoadCenterLines.fs", false);
        Ptr<MgByteReader> fsrdr6 = fssrc6->GetReader();
        m_svcResource->SetResource(fsres6, fsrdr6, NULL);

        Ptr<MgResourceIdentifier> fsres7 = new MgResourceIdentifier(L"Library://UnitTests/Data/VotingDistricts.FeatureSource");
        Ptr<MgByteSource> fssrc7 = new MgByteSource(L"../UnitTestFiles/UT_VotingDistricts.fs", false);
        Ptr<MgByteReader> fsrdr7 = fssrc7->GetReader();
        m_svcResource->SetResource(fsres7, fsrdr7, NULL);

        // publish the resource data
        Ptr<MgByteSource> dataSource6 = new MgByteSource(L"../UnitTestFiles/UT_RoadCenterLines.sdf", false);
        Ptr<MgByteReader> dataReader6 = dataSource6->GetReader();
        m_svcResource->SetResourceData(fsres6, L"UT_RoadCenterLines.sdf", L"File", dataReader6);

        Ptr<MgByteSource> dataSource7 = new MgByteSource(L"../UnitTestFiles/UT_VotingDistricts.sdf", false);
        Ptr<MgByteReader> dataReader7 = dataSource7->GetReader();
        m_svcResource->SetResourceData(fsres7, L"UT_VotingDistricts.sdf", L"File", dataReader7);

        // ---------------------------------------------------------
        // Data for exercising legend rendering
        // ---------------------------------------------------------
        Ptr<MgResourceIdentifier> ldfres14 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiFTS.LayerDefinition");
        Ptr<MgByteSource> ldfsrc14 = new MgByteSource(L"../UnitTestFiles/UT_MultiFTS.ldf", false);
        Ptr<MgByteReader> ldfrdr14 = ldfsrc14->GetReader();
        m_svcResource->SetResource(ldfres14, ldfrdr14, NULL);

        Ptr<MgResourceIdentifier> ldfres15 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTS.LayerDefinition");
        Ptr<MgByteSource> ldfsrc15 = new MgByteSource(L"../UnitTestFiles/UT_MultiCTS.ldf", false);
        Ptr<MgByteReader> ldfrdr15 = ldfsrc15->GetReader();
        m_svcResource->SetResource(ldfres15, ldfrdr15, NULL);

        Ptr<MgResourceIdentifier> ldfres16 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiFTSWithTheme.LayerDefinition");
        Ptr<MgByteSource> ldfsrc16 = new MgByteSource(L"../UnitTestFiles/UT_MultiFTSWithTheme.ldf", false);
        Ptr<MgByteReader> ldfrdr16 = ldfsrc16->GetReader();
        m_svcResource->SetResource(ldfres16, ldfrdr16, NULL);

        Ptr<MgResourceIdentifier> ldfres17 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTSWithTheme.LayerDefinition");
        Ptr<MgByteSource> ldfsrc17 = new MgByteSource(L"../UnitTestFiles/UT_MultiCTSWithTheme.ldf", false);
        Ptr<MgByteReader> ldfrdr17 = ldfsrc17->GetReader();
        m_svcResource->SetResource(ldfres17, ldfrdr17, NULL);

        Ptr<MgResourceIdentifier> ldfres18 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiFTSSingleCTS.LayerDefinition");
        Ptr<MgByteSource> ldfsrc18 = new MgByteSource(L"../UnitTestFiles/UT_MultiFTSSingleCTS.ldf", false);
        Ptr<MgByteReader> ldfrdr18 = ldfsrc18->GetReader();
        m_svcResource->SetResource(ldfres18, ldfrdr18, NULL);

        Ptr<MgResourceIdentifier> ldfres19 = new MgResourceIdentifier(L"Library://UnitTests/Layers/SingleFTSMultiCTS.LayerDefinition");
        Ptr<MgByteSource> ldfsrc19 = new MgByteSource(L"../UnitTestFiles/UT_SingleFTSMultiCTS.ldf", false);
        Ptr<MgByteReader> ldfrdr19 = ldfsrc19->GetReader();
        m_svcResource->SetResource(ldfres19, ldfrdr19, NULL);

        // Data related to stylization function tests
        Ptr<MgResourceIdentifier> ldfres20 = new MgResourceIdentifier(L"Library://UnitTests/Layers/StylizationFuncs.LayerDefinition");
        Ptr<MgByteSource> ldfsrc20 = new MgByteSource(L"../UnitTestFiles/UT_VotingDistrictsFunctions.ldf", false);
        Ptr<MgByteReader> ldfrdr20 = ldfsrc20->GetReader();
        m_svcResource->SetResource(ldfres20, ldfrdr20, NULL);

        Ptr<MgResourceIdentifier> mapres15 = new MgResourceIdentifier(L"Library://UnitTests/Maps/StylizationFuncs.MapDefinition");
        Ptr<MgByteSource> mdfsrc15 = new MgByteSource(L"../UnitTestFiles/UT_StylizationFuncs.mdf", false);
        Ptr<MgByteReader> mdfrdr15 = mdfsrc15->GetReader();
        m_svcResource->SetResource(mapres15, mdfrdr15, NULL);

        // For layer watermark test
        Ptr<MgResourceIdentifier> wdfres2 = new MgResourceIdentifier(L"Library://UnitTests/Watermarks/Rail.WatermarkDefinition");
        Ptr<MgByteSource> wdfsrc2 = new MgByteSource(L"../UnitTestFiles/UT_Rail.wdf", false);
        Ptr<MgByteReader> wdfrdr2 = wdfsrc2->GetReader();
        m_svcResource->SetResource(wdfres2, wdfrdr2, NULL);

        Ptr<MgResourceIdentifier> ldfres21 = new MgResourceIdentifier(L"Library://UnitTests/Layers/RailWatermark.LayerDefinition");
        Ptr<MgByteSource> ldfsrc21 = new MgByteSource(L"../UnitTestFiles/UT_Rail_Watermark.ldf", false);
        Ptr<MgByteReader> ldfrdr21 = ldfsrc21->GetReader();
        m_svcResource->SetResource(ldfres21, ldfrdr21, NULL);
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


void TestRenderingService::TestEnd()
{
    try
    {
        // set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        // delete the map definition
        Ptr<MgResourceIdentifier> mapres1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        m_svcResource->DeleteResource(mapres1);

        // delete tile set
        Ptr<MgResourceIdentifier> tilesetres1 = new MgResourceIdentifier(L"Library://UnitTests/TileSets/XYZ.TileSetDefinition");
        m_svcResource->DeleteResource(tilesetres1);

        // delete the layer definitions
        Ptr<MgResourceIdentifier> ldfres1 = new MgResourceIdentifier(L"Library://UnitTests/Layers/HydrographicPolygons.LayerDefinition");
        m_svcResource->DeleteResource(ldfres1);

        Ptr<MgResourceIdentifier> ldfres2 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Rail.LayerDefinition");
        m_svcResource->DeleteResource(ldfres2);

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        m_svcResource->DeleteResource(ldfres3);

        // delete the feature sources
        Ptr<MgResourceIdentifier> fsres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/HydrographicPolygons.FeatureSource");
        m_svcResource->DeleteResource(fsres1);

        Ptr<MgResourceIdentifier> fsres2 = new MgResourceIdentifier(L"Library://UnitTests/Data/Rail.FeatureSource");
        m_svcResource->DeleteResource(fsres2);

        Ptr<MgResourceIdentifier> fsres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/Parcels.FeatureSource");
        m_svcResource->DeleteResource(fsres3);

        // delete the print layouts
        Ptr<MgResourceIdentifier> plres1 = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");
        m_svcResource->DeleteResource(plres1);

        Ptr<MgResourceIdentifier> plres2 = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/NoLegend.PrintLayout");
        m_svcResource->DeleteResource(plres2);

        // delete the symbol library
        Ptr<MgResourceIdentifier> slres1 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/SymbolMart.SymbolLibrary");
        m_svcResource->DeleteResource(slres1);

        // delete symbology stuff
        Ptr<MgResourceIdentifier> fsres4 = new MgResourceIdentifier(L"Library://UnitTests/Data/Capitals.FeatureSource");
        m_svcResource->DeleteResource(fsres4);
        Ptr<MgResourceIdentifier> sdres1 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/PointSymbol.SymbolDefinition");
        m_svcResource->DeleteResource(sdres1);
        Ptr<MgResourceIdentifier> sdres2 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/PointSymbolParam.SymbolDefinition");
        m_svcResource->DeleteResource(sdres2);
        Ptr<MgResourceIdentifier> ldfres4 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Capitals.LayerDefinition");
        m_svcResource->DeleteResource(ldfres4);
        Ptr<MgResourceIdentifier> ldfres5 = new MgResourceIdentifier(L"Library://UnitTests/Layers/CapitalsParam.LayerDefinition");
        m_svcResource->DeleteResource(ldfres5);
        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Capitals.MapDefinition");
        m_svcResource->DeleteResource(mapres2);
        Ptr<MgResourceIdentifier> mapres3 = new MgResourceIdentifier(L"Library://UnitTests/Maps/CapitalsParam.MapDefinition");
        m_svcResource->DeleteResource(mapres3);

        Ptr<MgResourceIdentifier> fsres5 = new MgResourceIdentifier(L"Library://UnitTests/Data/Lines.FeatureSource");
        m_svcResource->DeleteResource(fsres5);
        Ptr<MgResourceIdentifier> sdres3 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/LineSymbol.SymbolDefinition");
        m_svcResource->DeleteResource(sdres3);
        Ptr<MgResourceIdentifier> sdres4 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/MTYP1500a.SymbolDefinition");
        m_svcResource->DeleteResource(sdres4);
        Ptr<MgResourceIdentifier> ldfres6 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Lines.LayerDefinition");
        m_svcResource->DeleteResource(ldfres6);
        Ptr<MgResourceIdentifier> ldfres7 = new MgResourceIdentifier(L"Library://UnitTests/Layers/LinesCrossTick.LayerDefinition");
        m_svcResource->DeleteResource(ldfres7);
        Ptr<MgResourceIdentifier> mapres4 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Lines.MapDefinition");
        m_svcResource->DeleteResource(mapres4);
        Ptr<MgResourceIdentifier> mapres5 = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinesCrossTick.MapDefinition");
        m_svcResource->DeleteResource(mapres5);

        Ptr<MgResourceIdentifier> ldfres8 = new MgResourceIdentifier(L"Library://UnitTests/Layers/UT_Annotation1.LayerDefinition");
        m_svcResource->DeleteResource(ldfres8);
        Ptr<MgResourceIdentifier> ldfres9 = new MgResourceIdentifier(L"Library://UnitTests/Layers/UT_Annotation2.LayerDefinition");
        m_svcResource->DeleteResource(ldfres9);
        Ptr<MgResourceIdentifier> ldfres10 = new MgResourceIdentifier(L"Library://UnitTests/Layers/UT_Annotation3.LayerDefinition");
        m_svcResource->DeleteResource(ldfres10);
        Ptr<MgResourceIdentifier> mapres8 = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_Annotation1.MapDefinition");
        m_svcResource->DeleteResource(mapres8);
        Ptr<MgResourceIdentifier> mapres9 = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_Annotation2.MapDefinition");
        m_svcResource->DeleteResource(mapres9);
        Ptr<MgResourceIdentifier> mapres10 = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_Annotation3.MapDefinition");
        m_svcResource->DeleteResource(mapres10);

        Ptr<MgResourceIdentifier> mapres11 = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_SymbologyPolygons.MapDefinition");
        m_svcResource->DeleteResource(mapres11);
        Ptr<MgResourceIdentifier> ldfres11 = new MgResourceIdentifier(L"Library://UnitTests/Layers/SymbologyParcels.LayerDefinition");
        m_svcResource->DeleteResource(ldfres11);
        Ptr<MgResourceIdentifier> sdres5 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/AreaSymbol.SymbolDefinition");
        m_svcResource->DeleteResource(sdres5);
        
        Ptr<MgResourceIdentifier> mapres12 = new MgResourceIdentifier(L"Library://UnitTests/Maps/SheboyganWithWatermark.MapDefinition");
        m_svcResource->DeleteResource(mapres12);
        Ptr<MgResourceIdentifier> wdfres1 = new MgResourceIdentifier(L"Library://UnitTests/Watermarks/PoweredByMapGuide.WatermarkDefinition");
        m_svcResource->DeleteResource(wdfres1);

        Ptr<MgResourceIdentifier> mapres13 = new MgResourceIdentifier(L"Library://UnitTests/Maps/BaseMap.MapDefinition");
        m_svcResource->DeleteResource(mapres13);
        Ptr<MgResourceIdentifier> ldfres12 = new MgResourceIdentifier(L"Library://UnitTests/Layers/RoadCenterLines.LayerDefinition");
        m_svcResource->DeleteResource(ldfres12);
        Ptr<MgResourceIdentifier> ldfres13 = new MgResourceIdentifier(L"Library://UnitTests/Layers/VotingDistricts.LayerDefinition");
        m_svcResource->DeleteResource(ldfres13);
        Ptr<MgResourceIdentifier> fsres6 = new MgResourceIdentifier(L"Library://UnitTests/Data/RoadCenterLines.FeatureSource");
        m_svcResource->DeleteResource(fsres6);
        Ptr<MgResourceIdentifier> fsres7 = new MgResourceIdentifier(L"Library://UnitTests/Data/VotingDistricts.FeatureSource");
        m_svcResource->DeleteResource(fsres7);

        Ptr<MgResourceIdentifier> ldfres14 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiFTS.LayerDefinition");
        m_svcResource->DeleteResource(ldfres14);
        Ptr<MgResourceIdentifier> ldfres15 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTS.LayerDefinition");
        m_svcResource->DeleteResource(ldfres15);
        Ptr<MgResourceIdentifier> ldfres16 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiFTSWithTheme.LayerDefinition");
        m_svcResource->DeleteResource(ldfres16);
        Ptr<MgResourceIdentifier> ldfres17 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTSWithTheme.LayerDefinition");
        m_svcResource->DeleteResource(ldfres17);
        Ptr<MgResourceIdentifier> ldfres18 = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiFTSSingleCTS.LayerDefinition");
        m_svcResource->DeleteResource(ldfres18);
        Ptr<MgResourceIdentifier> ldfres19 = new MgResourceIdentifier(L"Library://UnitTests/Layers/SingleFTSMultiCTS.LayerDefinition");
        m_svcResource->DeleteResource(ldfres19);

        // Data related to stylization function tests
        Ptr<MgResourceIdentifier> ldfres20 = new MgResourceIdentifier(L"Library://UnitTests/Layers/StylizationFuncs.LayerDefinition");
        m_svcResource->DeleteResource(ldfres20);

        Ptr<MgResourceIdentifier> mapres15 = new MgResourceIdentifier(L"Library://UnitTests/Maps/StylizationFuncs.MapDefinition");
        m_svcResource->DeleteResource(mapres15);

        // Layer watermark test
        Ptr<MgResourceIdentifier> wdfres2 = new MgResourceIdentifier(L"Library://UnitTests/Watermarks/Rail.WatermarkDefinition");
        m_svcResource->DeleteResource(wdfres2);
        Ptr<MgResourceIdentifier> ldfres21 = new MgResourceIdentifier(L"Library://UnitTests/Layers/RailWatermark.LayerDefinition");
        m_svcResource->DeleteResource(ldfres21);

        #ifdef _DEBUG
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif
    }
    catch(MgFileIoException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nMgFileIoException - Possible file permission error.\nError: %W\n"), message.c_str()));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        message += e->GetStackTrace(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }

    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRendering Service tests completed.\n\n")));
}


void TestRenderingService::TestCase_RenderDynamicOverlay(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

//        clock_t t0 = clock();

        // call the API using scales of 75000 and 12000
        map->SetViewScale(75000.0);

//        for (int i=0; i<10; i++)
//        {
            Ptr<MgByteReader> rdr1 = m_svcRendering->RenderDynamicOverlay(map, NULL, imageFormat);
            rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderDynamicOverlay75k", imageFormat, extension));
//        }

        map->SetViewScale(12000.0);

//        for (int i=0; i<100; i++)
//        {
            Ptr<MgByteReader> rdr2 = m_svcRendering->RenderDynamicOverlay(map, NULL, imageFormat);
            rdr2->ToFile(GetPath(L"../UnitTestFiles/RenderDynamicOverlay12k", imageFormat, extension));
//        }

//        clock_t t1 = clock();
//        printf ("delta clock %d\n", t1 - t0);
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


void TestRenderingService::TestCase_RenderDynamicOverlayTiledMap(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestTiledMap();

//        clock_t t0 = clock();

        Ptr<MgRenderingOptions> renderOpts = new MgRenderingOptions(imageFormat, MgRenderingOptions::RenderLayers | MgRenderingOptions::RenderBaseLayers, NULL);

        map->SetViewScale(12000.0);
//        for (int i=0; i<10; i++)
//        {
            //EXPECT: Tiled layers rendered with transparent background for applicable formats
            Ptr<MgByteReader> rdr1 = m_svcRendering->RenderDynamicOverlay(map, NULL, renderOpts);
            rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderDynamicOverlayTiled_WithBase12k", imageFormat, extension));
//        }

        Ptr<MgRenderingOptions> renderOpts2 = new MgRenderingOptions(imageFormat, MgRenderingOptions::RenderLayers, NULL);

//        for (int i=0; i<100; i++)
//        {
            //EXPECT: Nothing, with transparent background for applicable formats
            Ptr<MgByteReader> rdr2 = m_svcRendering->RenderDynamicOverlay(map, NULL, renderOpts2);
            rdr2->ToFile(GetPath(L"../UnitTestFiles/RenderDynamicOverlayTiled_NoBase12k", imageFormat, extension));
//        }

//        clock_t t1 = clock();
//        printf ("delta clock %d\n", t1 - t0);
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


void TestRenderingService::TestCase_RenderMap(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // call the API using scales of 75000 and 12000
        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderMap75k", imageFormat, extension));

        map->SetViewScale(12000.0);
        Ptr<MgByteReader> rdr2 = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr2->ToFile(GetPath(L"../UnitTestFiles/RenderMap12k", imageFormat, extension));
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

void TestRenderingService::TestCase_RenderMapWithSelection(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // Make a selection set and initialize it
        Ptr<MgSelection> sel = new MgSelection(map);
        sel->FromXml(L"");
        
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        Ptr<MgFeatureService> featSvc = dynamic_cast<MgFeatureService*>(serviceManager->RequestService(MgServiceType::FeatureService));
        assert(featSvc != NULL);

        Ptr<MgFeatureQueryOptions> query = new MgFeatureQueryOptions();
        query->SetFilter(L"Autogenerated_SDF_ID = 1");
        Ptr<MgLayerCollection> layers = map->GetLayers();
        Ptr<MgLayerBase> layer = layers->GetItem(L"Parcels");
        Ptr<MgFeatureReader> rdr = layer->SelectFeatures(query);
        
        INT32 width = 640;
        INT32 height = 480;
        Ptr<MgColor> bgColor = new MgColor(map->GetBackgroundColor());
        //This is the inflated bounds around the parcel with Autogenerated_SDF_ID = 1
        Ptr<MgEnvelope> extents = new MgEnvelope(-87.75979253783224, 43.78089182030202, -87.75322168532644, 43.78710288549482);
        Ptr<MgPoint> centerPt;
        if (rdr->ReadNext())
        {
            MgAgfReaderWriter agfRw;
            Ptr<MgByteReader> agf = ((MgReader*)rdr)->GetGeometry(layer->GetFeatureGeometryName());
            Ptr<MgGeometry> geom = agfRw.Read(agf);
            centerPt = geom->GetCentroid();

            sel->AddFeatureIdInt32(layer, layer->GetFeatureClassName(), rdr->GetInt32(L"Autogenerated_SDF_ID"));
        }

        Ptr<MgCoordinate> center = centerPt->GetCoordinate();

        rdr->Close();

        //Exercise all overloads. Except for the last one, any of these overloads that render a selection should render with blue
        map->SetViewScale(8000.0);
        map->SetViewCenter(centerPt);

        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMap(map, sel, imageFormat); //This just funnels the 4-arg version w/ keepSelection = true
        rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderMapWithSelection_rdr1", imageFormat, extension));

        Ptr<MgByteReader> rdr2 = m_svcRendering->RenderMap(map, sel, imageFormat, false);
        rdr2->ToFile(GetPath(L"../UnitTestFiles/RenderMapWithSelection_rdr2", imageFormat, extension));

        Ptr<MgByteReader> rdr3 = m_svcRendering->RenderMap(map, sel, imageFormat, true, true);
        rdr3->ToFile(GetPath(L"../UnitTestFiles/RenderMapWithSelection_rdr3", imageFormat, extension));

        Ptr<MgByteReader> rdr4 = m_svcRendering->RenderMap(map, sel, extents, width, height, bgColor, imageFormat);
        rdr4->ToFile(GetPath(L"../UnitTestFiles/RenderMapWithSelection_rdr4", imageFormat, extension)); //This just funnels to the version below w/ keepSelection = true

        Ptr<MgByteReader> rdr5 = m_svcRendering->RenderMap(map, sel, extents, width, height, bgColor, imageFormat, false);
        rdr5->ToFile(GetPath(L"../UnitTestFiles/RenderMapWithSelection_rdr5", imageFormat, extension));

        Ptr<MgByteReader> rdr6 = m_svcRendering->RenderMap(map, sel, center, 5000.0, width, height, bgColor, imageFormat);
        rdr6->ToFile(GetPath(L"../UnitTestFiles/RenderMapWithSelection_rdr6", imageFormat, extension));

        Ptr<MgByteReader> rdr7 = m_svcRendering->RenderMap(map, sel, center, 5000.0, width, height, bgColor, imageFormat, false);
        rdr7->ToFile(GetPath(L"../UnitTestFiles/RenderMapWithSelection_rdr7", imageFormat, extension));

        //This is the new overload introduced with 3.0, it should render selections with the color we specify
        Ptr<MgColor> selColor = new MgColor(255, 0, 0);
        Ptr<MgByteReader> rdr8 = m_svcRendering->RenderMap(map, sel, center, 5000.0, width, height, bgColor, imageFormat, true, selColor);
        rdr8->ToFile(GetPath(L"../UnitTestFiles/RenderMapWithSelection_rdr8", imageFormat, extension));
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

void TestRenderingService::TestCase_RenderMapWithWatermark(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMapWithWatermark();

        // call the API using scales of 75000 and 12000
        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderMapWithWatermark75k", imageFormat, extension));

        map->SetViewScale(12000.0);
        Ptr<MgByteReader> rdr2 = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr2->ToFile(GetPath(L"../UnitTestFiles/RenderMapWithWatermark12k", imageFormat, extension));
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


void TestRenderingService::TestCase_RenderLegend(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // call the API using scales of 75000 and 12000
        Ptr<MgColor> bgc = new MgColor(255, 255, 255, 255);

        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderLegend75k", imageFormat, extension));

        map->SetViewScale(12000.0);
        Ptr<MgByteReader> rdr2 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr2->ToFile(GetPath(L"../UnitTestFiles/RenderLegend12k", imageFormat, extension));

        // add a layer group
        Ptr<MgLayerGroupCollection> layerGroups = map->GetLayerGroups();
        Ptr<MgLayerCollection> layers = map->GetLayers();

        Ptr<MgResourceIdentifier> resId = new MgResourceIdentifier(L"Library://UnitTests/Layers/HydrographicPolygons.LayerDefinition");
        Ptr<MgResourceIdentifier> ldfRail = new MgResourceIdentifier(L"Library://UnitTests/Layers/Rail.LayerDefinition");

        Ptr<MgLayerGroup> group = new MgLayerGroup(L"Test Group");
        group->SetLegendLabel(L"Test Group");
        group->SetDisplayInLegend(true);
        layerGroups->Add(group);

        Ptr<MgLayerGroup> group1 = new MgLayerGroup(L"Nest top level");
        group1->SetLegendLabel(L"Nest top level");
        group1->SetDisplayInLegend(true);
        layerGroups->Add(group1);

        Ptr<MgLayerGroup> group2 = new MgLayerGroup(L"Nest child");
        group2->SetLegendLabel(L"Nest child (Nest top level)");
        group2->SetDisplayInLegend(true);
        group2->SetGroup(group1);
        layerGroups->Add(group2);

        Ptr<MgLayerGroup> group3 = new MgLayerGroup(L"Not visible in legend");
        group3->SetLegendLabel(L"Not visible in legend");
        group3->SetDisplayInLegend(false);
        layerGroups->Add(group3);

        Ptr<MgLayer> layer = new MgLayer(resId, m_svcResource);
        layer->SetName(L"HydroPolygons");
        layer->SetLegendLabel(L"HydroPolygons (Test Group)");
        layer->SetGroup(group);
        layer->SetDisplayInLegend(true);

        Ptr<MgLayer> layer2 = new MgLayer(ldfRail, m_svcResource);
        layer2->SetName(L"RailUnderNestedGroup");
        layer2->SetLegendLabel(L"Rail (Nest Child)");
        layer2->SetGroup(group2);
        layer2->SetDisplayInLegend(true);

        layers->Add(layer);
        layers->Add(layer2);

        //Re-draw at 75k. Layer group should not be there because it has no visible layers
        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr3 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr3->ToFile(GetPath(L"../UnitTestFiles/RenderLegend75kWithEmptyLayerGroup", imageFormat, extension));

        //Re-draw at 14000. Layer group should still not be there because it has no visible layers
        map->SetViewScale(14000.0);
        Ptr<MgByteReader> rdr4 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr4->ToFile(GetPath(L"../UnitTestFiles/RenderLegend14kWithEmptyLayerGroup", imageFormat, extension));

        //Re-draw at 12000. Layer group should now be there because its child layer (Parcels) should be visible
        map->SetViewScale(12000.0);
        Ptr<MgByteReader> rdr5 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr5->ToFile(GetPath(L"../UnitTestFiles/RenderLegend12kWithLayerGroup", imageFormat, extension));
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

void TestRenderingService::TestCase_RenderLegendEmptyGroups(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();
        map->SetViewScale(75000.0);
        Ptr<MgColor> bgc = new MgColor(255, 255, 255, 255);

        // Set up the following structure
        //
        // EmptyGroup1
        // EmptyGroup2
        //    EmptyGroup3
        //
        // None of these should be visible when we render the legend

        Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
        Ptr<MgLayerGroup> group1 = new MgLayerGroup(L"EmptyGroup1");
        group1->SetLegendLabel(L"EmptyGroup1");
        Ptr<MgLayerGroup> group2 = new MgLayerGroup(L"EmptyGroup2");
        group2->SetLegendLabel(L"EmptyGroup2");
        Ptr<MgLayerGroup> group3 = new MgLayerGroup(L"EmptyGroup3");
        group3->SetLegendLabel(L"EmptyGroup3");

        group1->SetDisplayInLegend(true);
        group2->SetDisplayInLegend(true);
        group3->SetDisplayInLegend(true);

        groups->Add(group1);
        groups->Add(group2);
        groups->Add(group3);
        group3->SetGroup(group2);

        map->Save();

        // Call the API
        Ptr<MgByteReader> rdr3 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr3->ToFile(GetPath(L"../UnitTestFiles/RenderLegendEmptyGroups", imageFormat, extension));
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

void TestRenderingService::TestCase_StylizationFunctions(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        Ptr<MgMap> map = CreateTestStylizationFunctionMap();
        
        // call the API using a scale of 75000
        map->SetViewScale(75000.0);

        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderDynamicOverlay(map, NULL, imageFormat);
        rdr1->ToFile(GetPath(L"../UnitTestFiles/StylizationFunc_75k", imageFormat, extension));

        // call the API using a scale of 12000
        map->SetViewScale(12000);
        Ptr<MgByteReader> rdr2 = m_svcRendering->RenderDynamicOverlay(map, NULL, imageFormat);
        rdr2->ToFile(GetPath(L"../UnitTestFiles/StylizationFunc_12k", imageFormat, extension));

        // Move around
        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-87.733753, 43.746899);
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        Ptr<MgByteReader> rdr3 = m_svcRendering->RenderDynamicOverlay(map, NULL, imageFormat);
        rdr3->ToFile(GetPath(L"../UnitTestFiles/StylizationFunc_12k_Moved", imageFormat, extension));
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

void TestRenderingService::TestCase_RenderLegendMultiFTS(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // Insert our test layer
        Ptr<MgResourceIdentifier> layerDef = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiFTS.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(layerDef, m_svcResource);
        layer->SetLegendLabel(layerDef->GetName());
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Insert(0, layer);
        map->Save();

        // call the API using scales of 75000 and 12000
        Ptr<MgColor> bgc = new MgColor(255, 255, 255, 255);

        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderLegendMultiFTS", imageFormat, extension));
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

void TestRenderingService::TestCase_RenderLegendMultiCTS(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // Insert our test layer
        Ptr<MgResourceIdentifier> layerDef = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTS.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(layerDef, m_svcResource);
        layer->SetLegendLabel(layerDef->GetName());
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Insert(0, layer);
        map->Save();

        // call the API using scales of 75000 and 12000
        Ptr<MgColor> bgc = new MgColor(255, 255, 255, 255);

        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderLegendMultiCTS", imageFormat, extension));
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

void TestRenderingService::TestCase_RenderLegendMultiFTSWithTheme(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // Insert our test layer
        Ptr<MgResourceIdentifier> layerDef = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiFTSWithTheme.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(layerDef, m_svcResource);
        layer->SetLegendLabel(layerDef->GetName());
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Insert(0, layer);
        map->Save();

        // call the API using scales of 75000 and 12000
        Ptr<MgColor> bgc = new MgColor(255, 255, 255, 255);

        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderLegendMultiFTSWithTheme", imageFormat, extension));
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

void TestRenderingService::TestCase_RenderLegendMultiCTSWithTheme(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // Insert our test layer
        Ptr<MgResourceIdentifier> layerDef = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiCTSWithTheme.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(layerDef, m_svcResource);
        layer->SetLegendLabel(layerDef->GetName());
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Insert(0, layer);
        map->Save();

        // call the API using scales of 75000 and 12000
        Ptr<MgColor> bgc = new MgColor(255, 255, 255, 255);

        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderLegendMultiCTSWithTheme", imageFormat, extension));
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

void TestRenderingService::TestCase_RenderLegendMultiFTSSingleCTS(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // Insert our test layer
        Ptr<MgResourceIdentifier> layerDef = new MgResourceIdentifier(L"Library://UnitTests/Layers/MultiFTSSingleCTS.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(layerDef, m_svcResource);
        layer->SetLegendLabel(layerDef->GetName());
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Insert(0, layer);
        map->Save();

        // call the API using scales of 75000 and 12000
        Ptr<MgColor> bgc = new MgColor(255, 255, 255, 255);

        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderLegendMultiFTSSingleCTS", imageFormat, extension));
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


void TestRenderingService::TestCase_RenderLegendSingleFTSMultiCTS(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // Insert our test layer
        Ptr<MgResourceIdentifier> layerDef = new MgResourceIdentifier(L"Library://UnitTests/Layers/SingleFTSMultiCTS.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(layerDef, m_svcResource);
        layer->SetLegendLabel(layerDef->GetName());
        Ptr<MgLayerCollection> layers = map->GetLayers();
        layers->Insert(0, layer);
        map->Save();

        // call the API using scales of 75000 and 12000
        Ptr<MgColor> bgc = new MgColor(255, 255, 255, 255);

        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, imageFormat);
        rdr1->ToFile(GetPath(L"../UnitTestFiles/RenderLegendSingleFTSMultiCTS", imageFormat, extension));
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

void TestRenderingService::TestCase_LayerWatermark(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // call the API using a scale of 60000
        map->SetViewScale(60000.0);

        // Remove existing rail layer
        Ptr<MgLayerCollection> layers = map->GetLayers();
        INT32 index = layers->IndexOf(L"Rail");
        if (index >= 0)
            layers->RemoveAt(index);

        // Insert our watermarked rail layer
        Ptr<MgResourceIdentifier> layerDef = new MgResourceIdentifier(L"Library://UnitTests/Layers/RailWatermark.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(layerDef, m_svcResource);
        layer->SetName(L"Rail");
        layer->SetLegendLabel(layerDef->GetName());
        layers->Insert(0, layer);
        map->Save();

        Ptr<MgColor> selColor = new MgColor(0, 0, 255);
        Ptr<MgRenderingOptions> renderOpts = new MgRenderingOptions(imageFormat, MgRenderingOptions::RenderLayers | MgRenderingOptions::RenderBaseLayers, selColor);
        Ptr<MgByteReader> img = m_svcRendering->RenderDynamicOverlay(map, NULL, renderOpts);
        img->ToFile(GetPath(L"../UnitTestFiles/LayerWatermarkOn", imageFormat, extension));

        layer->SetVisible(false);
        map->Save();
        img = m_svcRendering->RenderDynamicOverlay(map, NULL, renderOpts);
        img->ToFile(GetPath(L"../UnitTestFiles/LayerWatermarkOff", imageFormat, extension));
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

void TestRenderingService::TestCase_QueryFeatures()
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // call the API using a scale of 60000
        map->SetViewScale(60000.0);

        Ptr<MgStringCollection> layerNames1 = new MgStringCollection();
        layerNames1->Add(L"Rail");
        layerNames1->Add(L"HydrographicPolygons");
        Ptr<MgPolygon> poly1 = CreateSelectionPolygon(map, 0.3, 0.3);
        Ptr<MgFeatureInformation> fi1 = m_svcRendering->QueryFeatures(map, layerNames1, poly1, MgFeatureSpatialOperations::Within, -1);

        Ptr<MgByteReader> br1 = fi1->ToXml();
        Ptr<MgByteSink> sink1 = new MgByteSink(br1);
        sink1->ToFile(L"../UnitTestFiles/QueryFeatures75k.xml");

        // call the API using a scale of 12000
        map->SetViewScale(12000.0);

        Ptr<MgStringCollection> layerNames2 = new MgStringCollection();
        layerNames2->Add(L"Rail");
        layerNames2->Add(L"Parcels");
        Ptr<MgPolygon> poly2 = CreateSelectionPolygon(map, 0.05, 0.05);
        Ptr<MgFeatureInformation> fi2 = m_svcRendering->QueryFeatures(map, layerNames2, poly2, MgFeatureSpatialOperations::Within, -1);

        Ptr<MgByteReader> br2 = fi2->ToXml();
        Ptr<MgByteSink> sink2 = new MgByteSink(br2);
        sink2->ToFile(L"../UnitTestFiles/QueryFeatures12k.xml");
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

MgMap* TestRenderingService::CreateTestMapWithWatermark()
{
    Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/SheboyganWithWatermark.MapDefinition");
    MgMap* map = new MgMap(m_siteConnection);
    map->Create(mdfres, L"UnitTestSheboyganWithWatermark");

    Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-87.733253, 43.746199);
    Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
    map->SetViewCenter(ptNewCenter);
    map->SetViewScale(75000.0);
    map->SetDisplayDpi(96);
    map->SetDisplayWidth(1024);
    map->SetDisplayHeight(1024);

    return map;
}

MgMap* TestRenderingService::CreateTestMap()
{
    Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
    MgMap* map = new MgMap(m_siteConnection);
    map->Create(mdfres, L"UnitTestSheboygan");

    Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-87.733253, 43.746199);
    Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
    map->SetViewCenter(ptNewCenter);
    map->SetViewScale(75000.0);
    map->SetDisplayDpi(96);
    map->SetDisplayWidth(1024);
    map->SetDisplayHeight(1024);

    return map;
}

MgMap* TestRenderingService::CreateTestStylizationFunctionMap()
{
    Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/StylizationFuncs.MapDefinition");
    MgMap* map = new MgMap(m_siteConnection);
    map->Create(mdfres, L"StylizationFuncs");

    Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-87.733253, 43.746199);
    Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
    map->SetViewCenter(ptNewCenter);
    map->SetViewScale(60000.0);
    map->SetDisplayDpi(96);
    map->SetDisplayWidth(1024);
    map->SetDisplayHeight(1024);

    return map;
}

MgMap* TestRenderingService::CreateTestXYZMap()
{
    Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/BaseMap.MapDefinition");
    MgMap* map = new MgMap(m_siteConnection);
    map->Create(mdfres, L"UnitTestSheboyganXYZ");

    Ptr<MgLayerCollection> layers = map->GetLayers();
    Ptr<MgLayerGroupCollection> groups = map->GetLayerGroups();
    Ptr<MgLayerBase> roads = layers->GetItem(L"RoadCenterLines");
    Ptr<MgLayerGroup> baseGroup = groups->GetItem(L"BaseLayers");
    roads->SetGroup(baseGroup);

    Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-87.733253, 43.746199);
    Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
    map->SetViewCenter(ptNewCenter);
    map->SetViewScale(75000.0);
    map->SetDisplayDpi(96);
    map->SetDisplayWidth(1024);
    map->SetDisplayHeight(1024);

    return map;
}

MgMap* TestRenderingService::CreateTestTiledMap()
{
    Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/BaseMap.MapDefinition");
    MgMap* map = new MgMap(m_siteConnection);
    map->Create(mdfres, L"UnitTestSheboyganTiled");

    Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-87.733253, 43.746199);
    Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
    map->SetViewCenter(ptNewCenter);
    map->SetViewScale(75000.0);
    map->SetDisplayDpi(96);
    map->SetDisplayWidth(1024);
    map->SetDisplayHeight(1024);

    return map;
}

// creates a selection bounds in mapping space
MgPolygon* TestRenderingService::CreateSelectionPolygon(MgMap* map, double width, double height)
{
    Ptr<MgPoint> centerPt = map->GetViewCenter();
    Ptr<MgCoordinate> center = centerPt->GetCoordinate();
    double xMin = center->GetX() - 0.5*width;
    double yMin = center->GetY() - 0.5*height;
    double xMax = center->GetX() + 0.5*width;
    double yMax = center->GetY() + 0.5*height;
    Ptr<MgCoordinateXY> c1 = new MgCoordinateXY(xMin, yMin);
    Ptr<MgCoordinateXY> c2 = new MgCoordinateXY(xMax, yMin);
    Ptr<MgCoordinateXY> c3 = new MgCoordinateXY(xMax, yMax);
    Ptr<MgCoordinateXY> c4 = new MgCoordinateXY(xMin, yMax);
    Ptr<MgCoordinateXY> c5 = new MgCoordinateXY(xMin, yMin);

    Ptr<MgCoordinateCollection> cc = new MgCoordinateCollection();
    cc->Add(c1);
    cc->Add(c2);
    cc->Add(c3);
    cc->Add(c4);
    cc->Add(c5);

    Ptr<MgLinearRing> outer = new MgLinearRing(cc);
    return new MgPolygon(outer, NULL);
}


void TestRenderingService::TestCase_SymbologyPoints(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Capitals.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestSymbology");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-88, 48); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(2*pixels);
        map->SetDisplayHeight(pixels);

        // the map is a little silly -- it's in degrees but thinks it's in meters --
        // hence the 111000 is commented out
        double mapHeight = 50.0;
        double scale =  /*111000.0 **/ mapHeight / METERS_PER_INCH * STANDARD_DISPLAY_DPI / (double)pixels;
        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr->ToFile(GetPath(L"../UnitTestFiles/SymbologyPoints", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_SymbologyPointsParam(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/CapitalsParam.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestSymbology");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-88, 48); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(2*pixels);
        map->SetDisplayHeight(pixels);

        // the map is a little silly -- it's in degrees but thinks it's in meters --
        // hence the 111000 is commented out
        double mapHeight = 50.0;
        double scale =  /*111000.0 **/ mapHeight / METERS_PER_INCH * STANDARD_DISPLAY_DPI / (double)pixels;
        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr->ToFile(GetPath(L"../UnitTestFiles/SymbologyPointsParam", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_SymbologyLines(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Lines.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestSymbology");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(6, 4); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(2*pixels);
        map->SetDisplayHeight(pixels);

        // the map is a little silly -- it's in degrees but thinks it's in meters --
        // hence the 111000 is commented out
        double mapHeight = 8.0;
        double scale =  /*111000.0 **/ mapHeight / METERS_PER_INCH * STANDARD_DISPLAY_DPI / (double)pixels;
        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr->ToFile(GetPath(L"../UnitTestFiles/SymbologyLines", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_SymbologyLinesCrossTick(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/LinesCrossTick.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestSymbology");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(6, 4); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(2*pixels);
        map->SetDisplayHeight(pixels);

        // the map is a little silly -- it's in degrees but thinks it's in meters --
        // hence the 111000 is commented out
        double mapHeight = 8.0;
        double scale =  /*111000.0 **/ mapHeight / METERS_PER_INCH * STANDARD_DISPLAY_DPI / (double)pixels;
        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr->ToFile(GetPath(L"../UnitTestFiles/SymbologyLinesCrossTick", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_Annotation1(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_Annotation1.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestAnnotation1");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-88, 48); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(2*pixels);
        map->SetDisplayHeight(pixels);

        // the map is a little silly -- it's in degrees but thinks it's in meters --
        // hence the 111000 is commented out
        double mapHeight = 50.0;
        double scale =  /*111000.0 **/ mapHeight / METERS_PER_INCH * STANDARD_DISPLAY_DPI / (double)pixels;
        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr->ToFile(GetPath(L"../UnitTestFiles/Annotation1", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_Annotation2(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_Annotation2.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestAnnotation2");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-88, 48); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(2*pixels);
        map->SetDisplayHeight(pixels);

        // the map is a little silly -- it's in degrees but thinks it's in meters --
        // hence the 111000 is commented out
        double mapHeight = 50.0;
        double scale =  /*111000.0 **/ mapHeight / METERS_PER_INCH * STANDARD_DISPLAY_DPI / (double)pixels;
        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr->ToFile(GetPath(L"../UnitTestFiles/Annotation2", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_Annotation3(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_Annotation3.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestAnnotation3");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-88, 48); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(2*pixels);
        map->SetDisplayHeight(pixels);

        // the map is a little silly -- it's in degrees but thinks it's in meters --
        // hence the 111000 is commented out
        double mapHeight = 50.0;
        double scale =  /*111000.0 **/ mapHeight / METERS_PER_INCH * STANDARD_DISPLAY_DPI / (double)pixels;
        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr->ToFile(GetPath(L"../UnitTestFiles/Annotation3", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_SymbologyPolygons(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/UT_SymbologyPolygons.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestSymbologyPolygons");

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-87.733253, 43.746199);
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(1024);
        map->SetDisplayHeight(1024);

        map->SetViewScale(12000.0);
        Ptr<MgByteReader> rdr2 = m_svcRendering->RenderMap(map, NULL, imageFormat);
        rdr2->ToFile(GetPath(L"../UnitTestFiles/SymbologyPolygons", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

void TestRenderingService::TestCase_RenderMetatile(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        Ptr<MgMap> map = CreateTestTiledMap();
        map->SetViewScale(12500.0);
        Ptr<MgByteReader> tile4_6_baseline = m_svcRendering->RenderTile(map, L"BaseLayers", 4, 6, MgTileParameters::tileWidth, MgTileParameters::tileHeight, MgTileParameters::tileDPI, imageFormat);
        Ptr<MgByteReader> tile4_7_baseline = m_svcRendering->RenderTile(map, L"BaseLayers", 4, 7, MgTileParameters::tileWidth, MgTileParameters::tileHeight, MgTileParameters::tileDPI, imageFormat);
        Ptr<MgByteReader> tile5_6_baseline = m_svcRendering->RenderTile(map, L"BaseLayers", 5, 6, MgTileParameters::tileWidth, MgTileParameters::tileHeight, MgTileParameters::tileDPI, imageFormat);
        Ptr<MgByteReader> tile5_7_baseline = m_svcRendering->RenderTile(map, L"BaseLayers", 5, 7, MgTileParameters::tileWidth, MgTileParameters::tileHeight, MgTileParameters::tileDPI, imageFormat);

        tile4_6_baseline->ToFile(GetPath(L"../UnitTestFiles/RenderTile_4_6_Baseline", imageFormat, extension));
        tile4_7_baseline->ToFile(GetPath(L"../UnitTestFiles/RenderTile_4_7_Baseline", imageFormat, extension));
        tile5_6_baseline->ToFile(GetPath(L"../UnitTestFiles/RenderTile_5_6_Baseline", imageFormat, extension));
        tile5_7_baseline->ToFile(GetPath(L"../UnitTestFiles/RenderTile_5_7_Baseline", imageFormat, extension));

        MgRenderingService* renderSvc = dynamic_cast<MgRenderingService*>(m_svcRendering.p);
        CPPUNIT_ASSERT(NULL != renderSvc);

        // Render a 2x2 metatile which should cover the same tiles as the baseline test.
        INT32 metaTileFactor = 2;
        Ptr<MgMetatile> metaTile = renderSvc->RenderMetatile(map, L"BaseLayers", 4, 6, MgTileParameters::tileWidth, MgTileParameters::tileHeight, MgTileParameters::tileDPI, imageFormat, MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset, metaTileFactor);
        //metaTile->ToFile(L"../UnitTestFiles/RenderTile_Metatile@4_6.png");
        //CPPUNIT_ASSERT(metaTile->IsRewindable());
        //metaTile->Rewind();
        Ptr<MgByteReader> tile4_6 = renderSvc->RenderTileFromMetaTile(map, metaTile, 0, 0);
        Ptr<MgByteReader> tile4_7 = renderSvc->RenderTileFromMetaTile(map, metaTile, 0, 1);
        Ptr<MgByteReader> tile5_6 = renderSvc->RenderTileFromMetaTile(map, metaTile, 1, 0);
        Ptr<MgByteReader> tile5_7 = renderSvc->RenderTileFromMetaTile(map, metaTile, 1, 1);


        tile4_6->ToFile(GetPath(L"../UnitTestFiles/RenderTile_4_6_Metatiled", imageFormat, extension));
        tile4_7->ToFile(GetPath(L"../UnitTestFiles/RenderTile_4_7_Metatiled", imageFormat, extension));
        tile5_6->ToFile(GetPath(L"../UnitTestFiles/RenderTile_5_6_Metatiled", imageFormat, extension));
        tile5_7->ToFile(GetPath(L"../UnitTestFiles/RenderTile_5_7_Metatiled", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

void TestRenderingService::TestCase_RenderXYZMetatile(CREFSTRING imageFormat, CREFSTRING extension, INT32 retinaScale)
{
    try
    {
        STRING retinaSuffix = L"";
        if (retinaScale > 1)
        {
            retinaSuffix = L"@";
            STRING sRetina;
            MgUtil::Int32ToString(retinaScale, sRetina);
            retinaSuffix += sRetina;
        }

        Ptr<MgMap> map = CreateTestTiledMap();
        map->SetViewScale(12500.0);
        Ptr<MgByteReader> tile_16798_23891_baseline = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 16798, 23891, 16, MgTileParameters::tileDPI, imageFormat, MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset, retinaScale);
        Ptr<MgByteReader> tile_16799_23891_baseline = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 16799, 23891, 16, MgTileParameters::tileDPI, imageFormat, MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset, retinaScale);
        Ptr<MgByteReader> tile_16798_23892_baseline = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 16798, 23892, 16, MgTileParameters::tileDPI, imageFormat, MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset, retinaScale);
        Ptr<MgByteReader> tile_16799_23892_baseline = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 16799, 23892, 16, MgTileParameters::tileDPI, imageFormat, MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset, retinaScale);

        tile_16798_23891_baseline->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_16798_23891_16_Baseline" + retinaSuffix, imageFormat, extension));
        tile_16799_23891_baseline->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_16799_23891_16_Baseline" + retinaSuffix, imageFormat, extension));
        tile_16798_23892_baseline->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_16798_23892_16_Baseline" + retinaSuffix, imageFormat, extension));
        tile_16799_23892_baseline->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_16799_23892_16_Baseline" + retinaSuffix, imageFormat, extension));

        MgRenderingService* renderSvc = dynamic_cast<MgRenderingService*>(m_svcRendering.p);
        CPPUNIT_ASSERT(NULL != renderSvc);

        // Render a 2x2 metatile which should cover the same tiles as the baseline test.
        INT32 metaTileFactor = 2;
        Ptr<MgMetatile> metaTile = renderSvc->RenderMetatileXYZ(map, L"BaseLayers", 16798, 23891, 16, MgTileParameters::tileDPI, imageFormat, MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset, metaTileFactor, retinaScale);
        //metaTile->ToFile(L"../UnitTestFiles/RenderTileXYZ_Metatile@16798_23891_16.png");
        //CPPUNIT_ASSERT(metaTile->IsRewindable());
        //metaTile->Rewind();
        Ptr<MgByteReader> tile_16798_23891 = renderSvc->RenderTileFromMetaTile(map, metaTile, 0, 0);
        Ptr<MgByteReader> tile_16799_23891 = renderSvc->RenderTileFromMetaTile(map, metaTile, 1, 0);
        Ptr<MgByteReader> tile_16798_23892 = renderSvc->RenderTileFromMetaTile(map, metaTile, 0, 1);
        Ptr<MgByteReader> tile_16799_23892 = renderSvc->RenderTileFromMetaTile(map, metaTile, 1, 1);

        tile_16798_23891->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_16798_23891_16_Metatiled" + retinaSuffix, imageFormat, extension));
        tile_16799_23891->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_16799_23891_16_Metatiled" + retinaSuffix, imageFormat, extension));
        tile_16798_23892->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_16798_23892_16_Metatiled" + retinaSuffix, imageFormat, extension));
        tile_16799_23892->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_16799_23892_16_Metatiled" + retinaSuffix, imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

void TestRenderingService::TestCase_RenderTile(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        Ptr<MgMap> map = CreateTestTiledMap();
        map->SetViewScale(12500.0);
        Ptr<MgByteReader> tile4_6 = m_svcRendering->RenderTile(map, L"BaseLayers", 4, 6, 300, 300, 96, imageFormat);
        Ptr<MgByteReader> tile4_7 = m_svcRendering->RenderTile(map, L"BaseLayers", 4, 7, 300, 300, 96, imageFormat);
        Ptr<MgByteReader> tile5_6 = m_svcRendering->RenderTile(map, L"BaseLayers", 5, 6, 300, 300, 96, imageFormat);
        Ptr<MgByteReader> tile5_7 = m_svcRendering->RenderTile(map, L"BaseLayers", 5, 7, 300, 300, 96, imageFormat);

        tile4_6->ToFile(GetPath(L"../UnitTestFiles/RenderTile_4_6_300x300@96", imageFormat, extension));
        tile4_7->ToFile(GetPath(L"../UnitTestFiles/RenderTile_4_7_300x300@96", imageFormat, extension));
        tile5_6->ToFile(GetPath(L"../UnitTestFiles/RenderTile_5_6_300x300@96", imageFormat, extension));
        tile5_7->ToFile(GetPath(L"../UnitTestFiles/RenderTile_5_7_300x300@96", imageFormat, extension));

        Ptr<MgByteReader> tile11_11 = m_svcRendering->RenderTile(map, L"BaseLayers", 11, 11, 256, 256, 96, imageFormat);
        Ptr<MgByteReader> tile11_12 = m_svcRendering->RenderTile(map, L"BaseLayers", 11, 12, 256, 256, 96, imageFormat);
        Ptr<MgByteReader> tile12_11 = m_svcRendering->RenderTile(map, L"BaseLayers", 12, 11, 256, 256, 96, imageFormat);
        Ptr<MgByteReader> tile12_12 = m_svcRendering->RenderTile(map, L"BaseLayers", 12, 12, 256, 256, 96, imageFormat);

        tile11_11->ToFile(GetPath(L"../UnitTestFiles/RenderTile_11_11_256x256@96", imageFormat, extension));
        tile11_12->ToFile(GetPath(L"../UnitTestFiles/RenderTile_11_12_256x256@96", imageFormat, extension));
        tile12_11->ToFile(GetPath(L"../UnitTestFiles/RenderTile_12_11_256x256@96", imageFormat, extension));
        tile12_12->ToFile(GetPath(L"../UnitTestFiles/RenderTile_12_12_256x256@96", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

void TestRenderingService::TestCase_RenderTileXYZ(CREFSTRING imageFormat, CREFSTRING extension)
{
    try
    {
        Ptr<MgMap> map = CreateTestTiledMap();
        Ptr<MgByteReader> tileTL = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 2099, 2985, 13, 96, imageFormat);
        Ptr<MgByteReader> tileTR = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 2100, 2985, 13, 96, imageFormat);
        Ptr<MgByteReader> tileBL = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 2099, 2986, 13, 96, imageFormat);
        Ptr<MgByteReader> tileBR = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 2100, 2986, 13, 96, imageFormat);

        tileTL->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_TopLeft", imageFormat, extension));
        tileTR->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_TopRight", imageFormat, extension));
        tileBL->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_BottomLeft", imageFormat, extension));
        tileBR->ToFile(GetPath(L"../UnitTestFiles/RenderTileXYZ_BottomRight", imageFormat, extension));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

void TestRenderingService::TestCase_RenderTileUTFGrid()
{
    try
    {
        Ptr<MgMap> map = CreateTestXYZMap();

        //For ease of visual verfication, render the XYZ image tiles as baseline
        Ptr<MgByteReader> imgTL = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 16797, 23893, 16, 96, MgImageFormats::Png);
        Ptr<MgByteReader> imgTR = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 16798, 23893, 16, 96, MgImageFormats::Png);
        Ptr<MgByteReader> imgBL = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 16797, 23894, 16, 96, MgImageFormats::Png);
        Ptr<MgByteReader> imgBR = m_svcRendering->RenderTileXYZ(map, L"BaseLayers", 16798, 23894, 16, 96, MgImageFormats::Png);

        imgTL->ToFile(L"../UnitTestFiles/RenderTileUTFGrid_TopLeft_ImageBaseline.png");
        imgTR->ToFile(L"../UnitTestFiles/RenderTileUTFGrid_TopRight_ImageBaseline.png");
        imgBL->ToFile(L"../UnitTestFiles/RenderTileUTFGrid_BottomLeft_ImageBaseline.png");
        imgBR->ToFile(L"../UnitTestFiles/RenderTileUTFGrid_BottomRight_ImageBaseline.png");

        //Now render the utf grids at the same place
        Ptr<MgByteReader> utfTL = m_svcRendering->RenderTileUTFGrid(map, L"BaseLayers", 16797, 23893, 16, 96);
        Ptr<MgByteReader> utfTR = m_svcRendering->RenderTileUTFGrid(map, L"BaseLayers", 16798, 23893, 16, 96);
        Ptr<MgByteReader> utfBL = m_svcRendering->RenderTileUTFGrid(map, L"BaseLayers", 16797, 23894, 16, 96);
        Ptr<MgByteReader> utfBR = m_svcRendering->RenderTileUTFGrid(map, L"BaseLayers", 16798, 23894, 16, 96);

        utfTL->ToFile(L"../UnitTestFiles/RenderTileUTFGrid_TopLeft.utfgrid");
        utfTR->ToFile(L"../UnitTestFiles/RenderTileUTFGrid_TopRight.utfgrid");
        utfBL->ToFile(L"../UnitTestFiles/RenderTileUTFGrid_BottomLeft.utfgrid");
        utfBR->ToFile(L"../UnitTestFiles/RenderTileUTFGrid_BottomRight.utfgrid");
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

STRING TestRenderingService::GetPath(CREFSTRING basePath, CREFSTRING imageFormat, CREFSTRING extension)
{
    STRING ret;
    ret += basePath;
    ret += L"_";
    ret += imageFormat;
    ret += L".";
    ret += extension;
    return ret;
}

//void TestRenderingService::TestCase_RendererPerformance()
//{
//    /*
//    LineBuffer lb(8);
//    lb.MoveTo(100,100);
//    lb.LineTo(200, 107.3);
//    lb.LineTo(50, 201.2);
//    lb.LineTo(207, 203);
//    lb.Close();
//*/
//
//    LineBuffer lb(8);
//    lb.MoveTo(1,1);
//    lb.LineTo(1023, 1);
//    lb.LineTo(1023, 1023);
//    lb.LineTo(1, 1023);
//    lb.Close();
//
//    RS_LineStroke stroke(RS_Color(0), 0, L"Solid", RS_Units_Device);
//    RS_FillStyle fillstyle(stroke, RS_Color(255,0,0,255), RS_Color(0), L"Solid");
//
//    AGGRenderer* agg = new AGGRenderer(1024, 1024, RS_Color(0xffffffff), false, false, 0.0);
//
//    agg->StartMap(NULL, RS_Bounds(0,0,0,1024,1024,0), 1.0, 1.0, 1.0, NULL);
//
//    clock_t t0 = clock();
//
//    for (int i=0; i<10000; i++)
//        agg->ProcessPolygon(&lb, fillstyle);
//
//    clock_t t1 = clock();
//
//    printf ("diff %d\n", t1 - t0);
//
//    agg->EndMap();
//
//    agg->Save(L"c:\\agg.png", L"PNG");
//
//
//
//    GDRenderer* gd = new GDRenderer(1024, 1024, RS_Color(0xffffffff), false);
//
//    gd->StartMap(NULL, RS_Bounds(0,0,0,1024,1024,0), 1.0, 1.0, 1.0, NULL);
//
//    t0 = clock();
//
//    for (int i=0; i<10000; i++)
//        gd->ProcessPolygon(&lb, fillstyle);
//
//    t1 = clock();
//
//    printf ("diff %d\n", t1 - t0);
//
//    gd->EndMap();
//
//    gd->Save(L"c:\\gd.png", L"PNG");
//
//
//}
