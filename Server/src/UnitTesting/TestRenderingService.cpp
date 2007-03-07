//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "../Common/Manager/FdoConnectionManager.h"
#include "SAX2Parser.h"

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestRenderingService, "TestRenderingService");


TestRenderingService::TestRenderingService()
{
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();

    m_svcResource = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcRendering = dynamic_cast<MgRenderingService*>(serviceManager->RequestService(MgServiceType::RenderingService));
    assert(m_svcRendering != NULL);
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
        Ptr<MgResourceIdentifier> fsres4 = new MgResourceIdentifier(L"Library://Symbology/Data/Capitals.FeatureSource");
        Ptr<MgByteSource> fssrc4 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPoints.fs", false);
        Ptr<MgByteReader> fsrdr4 = fssrc4->GetReader();
        m_svcResource->SetResource(fsres4, fsrdr4, NULL);

        // point sdf file
        Ptr<MgByteSource> dataSource4 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPoints.sdf", false);
        Ptr<MgByteReader> dataReader4 = dataSource4->GetReader();
        m_svcResource->SetResourceData(fsres4, L"UT_SymbologyPoints.sdf", L"File", dataReader4);

        // point symbols
        Ptr<MgResourceIdentifier> sdres1 = new MgResourceIdentifier(L"Library://Symbology/Symbols/PointSymbol.SymbolDefinition");
        Ptr<MgByteSource> sdsrc1 = new MgByteSource(L"../UnitTestFiles/symbol.sd", false);
        Ptr<MgByteReader> sdrdr1 = sdsrc1->GetReader();
        m_svcResource->SetResource(sdres1, sdrdr1, NULL);

        Ptr<MgResourceIdentifier> sdres2 = new MgResourceIdentifier(L"Library://Symbology/Symbols/PointSymbolParam.SymbolDefinition");
        Ptr<MgByteSource> sdsrc2 = new MgByteSource(L"../UnitTestFiles/symbolp.sd", false);
        Ptr<MgByteReader> sdrdr2 = sdsrc2->GetReader();
        m_svcResource->SetResource(sdres2, sdrdr2, NULL);

        // point ldf
        Ptr<MgResourceIdentifier> ldfres4 = new MgResourceIdentifier(L"Library://Symbology/Layers/Capitals.LayerDefinition");
        Ptr<MgByteSource> ldfsrc4 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPoints.ldf", false);
        Ptr<MgByteReader> ldfrdr4 = ldfsrc4->GetReader();
        m_svcResource->SetResource(ldfres4, ldfrdr4, NULL);

        Ptr<MgResourceIdentifier> ldfres5 = new MgResourceIdentifier(L"Library://Symbology/Layers/CapitalsParam.LayerDefinition");
        Ptr<MgByteSource> ldfsrc5 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPointsParam.ldf", false);
        Ptr<MgByteReader> ldfrdr5 = ldfsrc5->GetReader();
        m_svcResource->SetResource(ldfres5, ldfrdr5, NULL);

        // point mdf
        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://Symbology/Maps/Capitals.MapDefinition");
        Ptr<MgByteSource> mdfsrc2 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPoints.mdf", false);
        Ptr<MgByteReader> mdfrdr2 = mdfsrc2->GetReader();
        m_svcResource->SetResource(mapres2, mdfrdr2, NULL);

        Ptr<MgResourceIdentifier> mapres3 = new MgResourceIdentifier(L"Library://Symbology/Maps/CapitalsParam.MapDefinition");
        Ptr<MgByteSource> mdfsrc3 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyPointsParam.mdf", false);
        Ptr<MgByteReader> mdfrdr3 = mdfsrc3->GetReader();
        m_svcResource->SetResource(mapres3, mdfrdr3, NULL);

        // the line feature source
        Ptr<MgResourceIdentifier> fsres5 = new MgResourceIdentifier(L"Library://Symbology/Data/Lines.FeatureSource");
        Ptr<MgByteSource> fssrc5 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLines.fs", false);
        Ptr<MgByteReader> fsrdr5 = fssrc5->GetReader();
        m_svcResource->SetResource(fsres5, fsrdr5, NULL);

        // line sdf file
        Ptr<MgByteSource> dataSource5 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLines.sdf", false);
        Ptr<MgByteReader> dataReader5 = dataSource5->GetReader();
        m_svcResource->SetResourceData(fsres5, L"UT_SymbologyLines.sdf", L"File", dataReader5);

        // line symbols
        Ptr<MgResourceIdentifier> sdres3 = new MgResourceIdentifier(L"Library://Symbology/Symbols/LineSymbol.SymbolDefinition");
        Ptr<MgByteSource> sdsrc3 = new MgByteSource(L"../UnitTestFiles/linesymbol.sd", false);
        Ptr<MgByteReader> sdrdr3 = sdsrc3->GetReader();
        m_svcResource->SetResource(sdres3, sdrdr3, NULL);

        Ptr<MgResourceIdentifier> sdres4 = new MgResourceIdentifier(L"Library://Symbology/Symbols/MTYP1500a.SymbolDefinition");
        Ptr<MgByteSource> sdsrc4 = new MgByteSource(L"../UnitTestFiles/MTYP1500a.sd", false);
        Ptr<MgByteReader> sdrdr4 = sdsrc4->GetReader();
        m_svcResource->SetResource(sdres4, sdrdr4, NULL);

        // line ldf
        Ptr<MgResourceIdentifier> ldfres6 = new MgResourceIdentifier(L"Library://Symbology/Layers/Lines.LayerDefinition");
        Ptr<MgByteSource> ldfsrc6 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLines.ldf", false);
        Ptr<MgByteReader> ldfrdr6 = ldfsrc6->GetReader();
        m_svcResource->SetResource(ldfres6, ldfrdr6, NULL);

        Ptr<MgResourceIdentifier> ldfres7 = new MgResourceIdentifier(L"Library://Symbology/Layers/LinesCrossTick.LayerDefinition");
        Ptr<MgByteSource> ldfsrc7 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLinesCrossTick.ldf", false);
        Ptr<MgByteReader> ldfrdr7 = ldfsrc7->GetReader();
        m_svcResource->SetResource(ldfres7, ldfrdr7, NULL);

        // line mdf
        Ptr<MgResourceIdentifier> mapres4 = new MgResourceIdentifier(L"Library://Symbology/Maps/Lines.MapDefinition");
        Ptr<MgByteSource> mdfsrc4 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLines.mdf", false);
        Ptr<MgByteReader> mdfrdr4 = mdfsrc4->GetReader();
        m_svcResource->SetResource(mapres4, mdfrdr4, NULL);

        Ptr<MgResourceIdentifier> mapres5 = new MgResourceIdentifier(L"Library://Symbology/Maps/LinesCrossTick.MapDefinition");
        Ptr<MgByteSource> mdfsrc5 = new MgByteSource(L"../UnitTestFiles/UT_SymbologyLinesCrossTick.mdf", false);
        Ptr<MgByteReader> mdfrdr5 = mdfsrc5->GetReader();
        m_svcResource->SetResource(mapres5, mdfrdr5, NULL);

        // symbols / layer definitions for testing MdfModel / MdfParser
        Ptr<MgResourceIdentifier> sdres5 = new MgResourceIdentifier(L"Library://Symbology/Symbols/MdfTestSimpleSymbol.SymbolDefinition");
        Ptr<MgByteSource> sdsrc5 = new MgByteSource(L"../UnitTestFiles/MdfTestSimpleSymbol.sd", false);
        Ptr<MgByteReader> sdrdr5 = sdsrc5->GetReader();
        m_svcResource->SetResource(sdres5, sdrdr5, NULL);

        Ptr<MgResourceIdentifier> sdres6 = new MgResourceIdentifier(L"Library://Symbology/Symbols/MdfTestCompoundSymbol.SymbolDefinition");
        Ptr<MgByteSource> sdsrc6 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbol.sd", false);
        Ptr<MgByteReader> sdrdr6 = sdsrc6->GetReader();
        m_svcResource->SetResource(sdres6, sdrdr6, NULL);

        Ptr<MgResourceIdentifier> ldfres8 = new MgResourceIdentifier(L"Library://Symbology/Layers/MdfTestCompTypeStyle.LayerDefinition");
        Ptr<MgByteSource> ldfsrc8 = new MgByteSource(L"../UnitTestFiles/MdfTestCompTypeStyle.ldf", false);
        Ptr<MgByteReader> ldfrdr8 = ldfsrc8->GetReader();
        m_svcResource->SetResource(ldfres8, ldfrdr8, NULL);
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
        Ptr<MgResourceIdentifier> fsres4 = new MgResourceIdentifier(L"Library://Symbology/Data/Capitals.FeatureSource");
        m_svcResource->DeleteResource(fsres4);
        Ptr<MgResourceIdentifier> sdres1 = new MgResourceIdentifier(L"Library://Symbology/Symbols/PointSymbol.SymbolDefinition");
        m_svcResource->DeleteResource(sdres1);
        Ptr<MgResourceIdentifier> sdres2 = new MgResourceIdentifier(L"Library://Symbology/Symbols/PointSymbolParam.SymbolDefinition");
        m_svcResource->DeleteResource(sdres2);
        Ptr<MgResourceIdentifier> ldfres4 = new MgResourceIdentifier(L"Library://Symbology/Layers/Capitals.LayerDefinition");
        m_svcResource->DeleteResource(ldfres4);
        Ptr<MgResourceIdentifier> ldfres5 = new MgResourceIdentifier(L"Library://Symbology/Layers/CapitalsParam.LayerDefinition");
        m_svcResource->DeleteResource(ldfres5);
        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://Symbology/Maps/Capitals.MapDefinition");
        m_svcResource->DeleteResource(mapres2);
        Ptr<MgResourceIdentifier> mapres3 = new MgResourceIdentifier(L"Library://Symbology/Maps/CapitalsParam.MapDefinition");
        m_svcResource->DeleteResource(mapres3);

        Ptr<MgResourceIdentifier> fsres5 = new MgResourceIdentifier(L"Library://Symbology/Data/Lines.FeatureSource");
        m_svcResource->DeleteResource(fsres5);
        Ptr<MgResourceIdentifier> sdres3 = new MgResourceIdentifier(L"Library://Symbology/Symbols/LineSymbol.SymbolDefinition");
        m_svcResource->DeleteResource(sdres3);
        Ptr<MgResourceIdentifier> sdres4 = new MgResourceIdentifier(L"Library://Symbology/Symbols/MTYP1500a.SymbolDefinition");
        m_svcResource->DeleteResource(sdres4);
        Ptr<MgResourceIdentifier> ldfres6 = new MgResourceIdentifier(L"Library://Symbology/Layers/Lines.LayerDefinition");
        m_svcResource->DeleteResource(ldfres6);
        Ptr<MgResourceIdentifier> ldfres7 = new MgResourceIdentifier(L"Library://Symbology/Layers/LinesCrossTick.LayerDefinition");
        m_svcResource->DeleteResource(ldfres7);
        Ptr<MgResourceIdentifier> mapres4 = new MgResourceIdentifier(L"Library://Symbology/Maps/Lines.MapDefinition");
        m_svcResource->DeleteResource(mapres4);
        Ptr<MgResourceIdentifier> mapres5 = new MgResourceIdentifier(L"Library://Symbology/Maps/LinesCrossTick.MapDefinition");
        m_svcResource->DeleteResource(mapres5);

        Ptr<MgResourceIdentifier> sdres5 = new MgResourceIdentifier(L"Library://Symbology/Symbols/MdfTestSimpleSymbol.SymbolDefinition");
        m_svcResource->DeleteResource(sdres5);
        Ptr<MgResourceIdentifier> sdres6 = new MgResourceIdentifier(L"Library://Symbology/Symbols/MdfTestCompoundSymbol.SymbolDefinition");
        m_svcResource->DeleteResource(sdres6);
        Ptr<MgResourceIdentifier> ldfres8 = new MgResourceIdentifier(L"Library://Symbology/Layers/MdfTestCompTypeStyle.LayerDefinition");
        m_svcResource->DeleteResource(ldfres8);

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

    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRendering Service tests completed.\n\n")));
}


void TestRenderingService::TestCase_RenderDynamicOverlay()
{
    try
    {
        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map = new MgMap();
        map->Create(m_svcResource, mdfres, L"UnitTestSheboygan");

        map->SetViewScale(73745078.0);

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(0.0, 0.0);
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(1024);
        map->SetDisplayHeight(1024);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderDynamicOverlay(map, NULL, L"PNG");
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
        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map = new MgMap();
        map->Create(m_svcResource, mdfres, L"UnitTestSheboygan");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(0.0, 0.0);
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(pixels*2);
        map->SetDisplayHeight(pixels);

        double mapHeight = 180.0;
        double scale = mapHeight * 111000.0 * 100.0 / 2.54 * 96.0 / (double)pixels;

        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");

        Ptr<MgColor> bgc = new MgColor(255,255,255,255);

        //also test the map legend API
        Ptr<MgByteReader> rdr2 = m_svcRendering->RenderMapLegend(map, 200, 400, bgc, L"PNG");
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
        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map = new MgMap();
        map->Create(m_svcResource, mdfres, L"UnitTestSheboygan");

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(0.0, 0.0);
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(1024);
        map->SetDisplayHeight(1024);

        map->SetViewScale(73745078.0);

        // selection bounds in mapping space
        Ptr<MgCoordinate> ll = new MgCoordinateXY( 0.0,  0.0);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(40.0, 60.0);

        Ptr<MgCoordinateXY> c1 = new MgCoordinateXY(ll->GetX(), ll->GetY());
        Ptr<MgCoordinateXY> c2 = new MgCoordinateXY(ur->GetX(), ll->GetY());
        Ptr<MgCoordinateXY> c3 = new MgCoordinateXY(ur->GetX(), ur->GetY());
        Ptr<MgCoordinateXY> c4 = new MgCoordinateXY(ll->GetX(), ur->GetY());
        Ptr<MgCoordinateXY> c5 = new MgCoordinateXY(ll->GetX(), ll->GetY());

        Ptr<MgCoordinateCollection> cc = new MgCoordinateCollection();
        cc->Add(c1);
        cc->Add(c2);
        cc->Add(c3);
        cc->Add(c4);
        cc->Add(c5);

        Ptr<MgLinearRing> outer = new MgLinearRing(cc);
        Ptr<MgPolygon> poly = new MgPolygon(outer, NULL);

        Ptr<MgStringCollection> layerNames = new MgStringCollection();
        layerNames->Add(L"World Countries");
        layerNames->Add(L"World Cities");

        // call the API
        Ptr<MgFeatureInformation> fi = m_svcRendering->QueryFeatures(map, layerNames, poly, MgFeatureSpatialOperations::Within, -1);
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


void TestRenderingService::TestCase_SymbologyMdfModel()
{
    try
    {
        MdfParser::SAX2Parser parser;

        // ------------------------------------------------------
        // process symbol #1 - a simple symbol definition
        // ------------------------------------------------------

        // create a scope so temp files are unlocked
        {
            Ptr<MgResourceIdentifier> sdres = new MgResourceIdentifier(L"Library://Symbology/Symbols/MdfTestSimpleSymbol.SymbolDefinition");

            // parse the symbol - this exercises MdfParser deserialization
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(sdres);
            Ptr<MgByteSink> sink = new MgByteSink(rdr);
            Ptr<MgByte> bytes = sink->ToBuffer();
            CPPUNIT_ASSERT(bytes->GetLength() > 0);

            parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
            CPPUNIT_ASSERT(parser.GetSucceeded());

            // write the file - this exercises MdfParser serialization
            auto_ptr<SymbolDefinition> symbolDef1(parser.DetachSymbolDefinition());
            CPPUNIT_ASSERT(symbolDef1.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestSimpleSymbolCopy1.sd", symbolDef1.get());
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestSimpleSymbolCopy1.sd"));

            // parse and resave the newly written file
            Ptr<MgByteSource> src1 = new MgByteSource(L"../UnitTestFiles/MdfTestSimpleSymbolCopy1.sd");
            Ptr<MgByteReader> rdr1 = src1->GetReader();
            Ptr<MgByteSink> sink1 = new MgByteSink(rdr1);
            Ptr<MgByte> bytes1 = sink1->ToBuffer();
            parser.ParseString((const char*)bytes1->Bytes(), bytes1->GetLength());
            auto_ptr<SymbolDefinition> symbolDef2(parser.DetachSymbolDefinition());
            parser.WriteToFile("../UnitTestFiles/MdfTestSimpleSymbolCopy2.sd", symbolDef2.get());

            // compare the two files
            Ptr<MgByteSource> src2 = new MgByteSource(L"../UnitTestFiles/MdfTestSimpleSymbolCopy2.sd");
            Ptr<MgByteReader> rdr2 = src2->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr2);
            Ptr<MgByte> bytes2 = sink2->ToBuffer();
            CPPUNIT_ASSERT(bytes1->GetLength() == bytes2->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1->Bytes(), bytes2->Bytes(), bytes1->GetLength()) == 0);
        }

        // delete the files
        MgFileUtil::DeleteFileW(L"../UnitTestFiles/MdfTestSimpleSymbolCopy1.sd", true);
        MgFileUtil::DeleteFileW(L"../UnitTestFiles/MdfTestSimpleSymbolCopy2.sd", true);

        // ------------------------------------------------------
        // process symbol #2 - a compound symbol definition
        // ------------------------------------------------------

        // create a scope so temp files are unlocked
        {
            Ptr<MgResourceIdentifier> sdres = new MgResourceIdentifier(L"Library://Symbology/Symbols/MdfTestCompoundSymbol.SymbolDefinition");

            // parse the symbol - this exercises MdfParser deserialization
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(sdres);
            Ptr<MgByteSink> sink = new MgByteSink(rdr);
            Ptr<MgByte> bytes = sink->ToBuffer();
            CPPUNIT_ASSERT(bytes->GetLength() > 0);

            parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
            CPPUNIT_ASSERT(parser.GetSucceeded());

            // write the file - this exercises MdfParser serialization
            auto_ptr<SymbolDefinition> symbolDef1(parser.DetachSymbolDefinition());
            CPPUNIT_ASSERT(symbolDef1.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbolCopy1.sd", symbolDef1.get());
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompoundSymbolCopy1.sd"));

            // parse and resave the newly written file
            Ptr<MgByteSource> src1 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbolCopy1.sd");
            Ptr<MgByteReader> rdr1 = src1->GetReader();
            Ptr<MgByteSink> sink1 = new MgByteSink(rdr1);
            Ptr<MgByte> bytes1 = sink1->ToBuffer();
            parser.ParseString((const char*)bytes1->Bytes(), bytes1->GetLength());
            auto_ptr<SymbolDefinition> symbolDef2(parser.DetachSymbolDefinition());
            parser.WriteToFile("../UnitTestFiles/MdfTestCompoundSymbolCopy2.sd", symbolDef2.get());

            // compare the two files
            Ptr<MgByteSource> src2 = new MgByteSource(L"../UnitTestFiles/MdfTestCompoundSymbolCopy2.sd");
            Ptr<MgByteReader> rdr2 = src2->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr2);
            Ptr<MgByte> bytes2 = sink2->ToBuffer();
            CPPUNIT_ASSERT(bytes1->GetLength() == bytes2->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1->Bytes(), bytes2->Bytes(), bytes1->GetLength()) == 0);
        }

        // delete the files
        MgFileUtil::DeleteFileW(L"../UnitTestFiles/MdfTestCompoundSymbolCopy1.sd", true);
        MgFileUtil::DeleteFileW(L"../UnitTestFiles/MdfTestCompoundSymbolCopy2.sd", true);

        // ------------------------------------------------------
        // process layer definition with composite type style
        // ------------------------------------------------------

        // create a scope so temp files are unlocked
        {
            Ptr<MgResourceIdentifier> ldfres = new MgResourceIdentifier(L"Library://Symbology/Layers/MdfTestCompTypeStyle.LayerDefinition");

            // parse the LDF - this exercises MdfParser deserialization
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceContent(ldfres);
            Ptr<MgByteSink> sink = new MgByteSink(rdr);
            Ptr<MgByte> bytes = sink->ToBuffer();
            CPPUNIT_ASSERT(bytes->GetLength() > 0);

            parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
            CPPUNIT_ASSERT(parser.GetSucceeded());

            // write the file - this exercises MdfParser serialization
            auto_ptr<VectorLayerDefinition> layerDef1(parser.DetachVectorLayerDefinition());
            CPPUNIT_ASSERT(layerDef1.get() != NULL);

            parser.WriteToFile("../UnitTestFiles/MdfTestCompTypeStyleCopy1.ldf", NULL, layerDef1.get(), NULL, NULL);
            CPPUNIT_ASSERT(MgFileUtil::IsFile(L"../UnitTestFiles/MdfTestCompTypeStyleCopy1.ldf"));

            // parse and resave the newly written file
            Ptr<MgByteSource> src1 = new MgByteSource(L"../UnitTestFiles/MdfTestCompTypeStyleCopy1.ldf");
            Ptr<MgByteReader> rdr1 = src1->GetReader();
            Ptr<MgByteSink> sink1 = new MgByteSink(rdr1);
            Ptr<MgByte> bytes1 = sink1->ToBuffer();
            parser.ParseString((const char*)bytes1->Bytes(), bytes1->GetLength());
            auto_ptr<VectorLayerDefinition> layerDef2(parser.DetachVectorLayerDefinition());
            parser.WriteToFile("../UnitTestFiles/MdfTestCompTypeStyleCopy2.ldf", NULL, layerDef2.get(), NULL, NULL);

            // compare the two files
            Ptr<MgByteSource> src2 = new MgByteSource(L"../UnitTestFiles/MdfTestCompTypeStyleCopy2.ldf");
            Ptr<MgByteReader> rdr2 = src2->GetReader();
            Ptr<MgByteSink> sink2 = new MgByteSink(rdr2);
            Ptr<MgByte> bytes2 = sink2->ToBuffer();
            CPPUNIT_ASSERT(bytes1->GetLength() == bytes2->GetLength());
            CPPUNIT_ASSERT(memcmp(bytes1->Bytes(), bytes2->Bytes(), bytes1->GetLength()) == 0);
        }

        // delete the files
        MgFileUtil::DeleteFileW(L"../UnitTestFiles/MdfTestCompTypeStyleCopy1.ldf", true);
        MgFileUtil::DeleteFileW(L"../UnitTestFiles/MdfTestCompTypeStyleCopy2.ldf", true);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}


void TestRenderingService::TestCase_SymbologyPoints()
{
    try
    {
        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://Symbology/Maps/Capitals.MapDefinition");
        Ptr<MgMap> map = new MgMap();
        map->Create(m_svcResource, mdfres, L"UnitTestSymbology");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-88, 48); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(pixels*2);
        map->SetDisplayHeight(pixels);

        // the map is a little silly -- it's in degrees but thinks it's in meters --
        // hence the 111000 is commented out
        double mapHeight = 50.0;
        double scale = mapHeight * /*111000.0 **/ 100.0 / 2.54 * 96.0 / (double)pixels;
        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");

        rdr->ToFile(L"C:\\SymbologyPoints.png");
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
        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://Symbology/Maps/CapitalsParam.MapDefinition");
        Ptr<MgMap> map = new MgMap();
        map->Create(m_svcResource, mdfres, L"UnitTestSymbology");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(-88, 48); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(pixels*2);
        map->SetDisplayHeight(pixels);

        // the map is a little silly -- it's in degrees but thinks it's in meters --
        // hence the 111000 is commented out
        double mapHeight = 50.0;
        double scale = mapHeight * /*111000.0 **/ 100.0 / 2.54 * 96.0 / (double)pixels;
        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");

        rdr->ToFile(L"C:\\SymbologyPointsParam.png");
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
        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://Symbology/Maps/Lines.MapDefinition");
        Ptr<MgMap> map = new MgMap();
        map->Create(m_svcResource, mdfres, L"UnitTestSymbology");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(6, 4); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(2*pixels);
        map->SetDisplayHeight(pixels);

        // the map is a little silly -- it's in degrees but thinks it's in meters --
        // hence the 111000 is commented out
        double mapHeight = 8;
        double scale = mapHeight /*111000.0 **/ * 100.0 / 2.54 * 96.0 / (double)pixels;
        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");

        rdr->ToFile(L"C:\\SymbologyLines.png");
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
        // get root
        Ptr<MgSiteConnection> conn = new MgSiteConnection();

        // make a runtime map
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://Symbology/Maps/LinesCrossTick.MapDefinition");
        Ptr<MgMap> map = new MgMap();
        map->Create(m_svcResource, mdfres, L"UnitTestSymbology");

        INT32 pixels = 512;

        Ptr<MgCoordinate> coordNewCenter = new MgCoordinateXY(6, 4); //about the center of the map
        Ptr<MgPoint> ptNewCenter = new MgPoint(coordNewCenter);
        map->SetViewCenter(ptNewCenter);
        map->SetDisplayDpi(96);
        map->SetDisplayWidth(2*pixels);
        map->SetDisplayHeight(pixels);

        double mapHeight = 8;
        double scale = mapHeight /** 111000.0*/ * 100.0 / 2.54 * 96.0 / (double)pixels;

        map->SetViewScale(scale);

        // call the API
        Ptr<MgByteReader> rdr = m_svcRendering->RenderMap(map, NULL, L"PNG");

        rdr->ToFile(L"C:\\SymbologyLinesCrossTick.png");
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}
