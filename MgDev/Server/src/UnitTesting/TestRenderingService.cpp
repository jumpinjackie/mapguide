//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
    MgUserInformation::SetCurrentUserInfo(userInfo);

    STRING session = svcSite->CreateSession();
    assert(!session.empty());
    userInfo->SetMgSessionId(session);

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


void TestRenderingService::TestCase_RenderDynamicOverlay()
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
            Ptr<MgByteReader> rdr1 = m_svcRendering->RenderDynamicOverlay(map, NULL, L"PNG");
            rdr1->ToFile(L"../UnitTestFiles/RenderDynamicOverlay75k.png");
//        }

        map->SetViewScale(12000.0);

//        for (int i=0; i<100; i++)
//        {
            Ptr<MgByteReader> rdr2 = m_svcRendering->RenderDynamicOverlay(map, NULL, L"PNG");
            rdr2->ToFile(L"../UnitTestFiles/RenderDynamicOverlay12k.png");
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


void TestRenderingService::TestCase_RenderMap()
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // call the API using scales of 75000 and 12000
        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMap(map, NULL, L"PNG");
        rdr1->ToFile(L"../UnitTestFiles/RenderMap75k.png");

        map->SetViewScale(12000.0);
        Ptr<MgByteReader> rdr2 = m_svcRendering->RenderMap(map, NULL, L"PNG");
        rdr2->ToFile(L"../UnitTestFiles/RenderMap12k.png");
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


void TestRenderingService::TestCase_RenderLegend()
{
    try
    {
        // make a runtime map
        Ptr<MgMap> map = CreateTestMap();

        // call the API using scales of 75000 and 12000
        Ptr<MgColor> bgc = new MgColor(255, 255, 255, 255);

        map->SetViewScale(75000.0);
        Ptr<MgByteReader> rdr1 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, L"PNG");
        rdr1->ToFile(L"../UnitTestFiles/RenderLegend75k.png");

        map->SetViewScale(12000.0);
        Ptr<MgByteReader> rdr2 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, L"PNG");
        rdr2->ToFile(L"../UnitTestFiles/RenderLegend12k.png");
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

        // call the API using a scale of 75000
        map->SetViewScale(75000.0);

        Ptr<MgStringCollection> layerNames1 = new MgStringCollection();
        layerNames1->Add(L"Rail");
        layerNames1->Add(L"HydrographicPolygons");
        Ptr<MgPolygon> poly1 = CreateSelectionPolygon(map, 0.3, 0.3);
        Ptr<MgFeatureInformation> fi1 = m_svcRendering->QueryFeatures(map, layerNames1, poly1, MgFeatureSpatialOperations::Within, -1);

        // call the API using a scale of 12000
        map->SetViewScale(12000.0);

        Ptr<MgStringCollection> layerNames2 = new MgStringCollection();
        layerNames2->Add(L"Rail");
        layerNames2->Add(L"Parcels");
        Ptr<MgPolygon> poly2 = CreateSelectionPolygon(map, 0.05, 0.05);
        Ptr<MgFeatureInformation> fi2 = m_svcRendering->QueryFeatures(map, layerNames2, poly2, MgFeatureSpatialOperations::Within, -1);
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


void TestRenderingService::TestCase_SymbologyPoints()
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
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");
        rdr->ToFile(L"../UnitTestFiles/SymbologyPoints.png");
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_SymbologyPointsParam()
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
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");
        rdr->ToFile(L"../UnitTestFiles/SymbologyPointsParam.png");
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_SymbologyLines()
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
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");
        rdr->ToFile(L"../UnitTestFiles/SymbologyLines.png");
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_SymbologyLinesCrossTick()
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
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");
        rdr->ToFile(L"../UnitTestFiles/SymbologyLinesCrossTick.png");
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_Annotation1()
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
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");
        rdr->ToFile(L"../UnitTestFiles/Annotation1.png");
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_Annotation2()
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
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");
        rdr->ToFile(L"../UnitTestFiles/Annotation2.png");
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_Annotation3()
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
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");
        rdr->ToFile(L"../UnitTestFiles/Annotation3.png");
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_SymbologyPolygons()
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
        Ptr<MgByteReader> rdr2 = m_svcRendering->RenderMap(map, NULL, L"PNG");
        rdr2->ToFile(L"../UnitTestFiles/SymbologyPolygons.png");
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
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
