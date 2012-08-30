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
#include "TestMappingService.h"
#include "CppUnitExtensions.h"

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestMappingService, "TestMappingService");


TestMappingService::TestMappingService()
{
    // Initialize service objects.
    //MgServiceManager* serviceManager = MgServiceManager::GetInstance();
    Ptr<MgdServiceFactory> factory = new MgdServiceFactory();

    m_svcResource = dynamic_cast<MgdResourceService*>(
        factory->CreateService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcMapping = dynamic_cast<MgdMappingService*>(
        factory->CreateService(MgServiceType::MappingService));
    assert(m_svcMapping != NULL);
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

void TestMappingService::TestCase_GetMultiPlot()
{
    try
    {
        // make a runtime map
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgdMap> map1 = new MgdMap(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(200e+6);

        // make another runtime map
        Ptr<MgResourceIdentifier> mapRes2 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgdMap> map2 = new MgdMap(mapRes2, L"UnitTestSheboygan2");
        map2->SetViewScale(100e+6);

        // make yet another runtime map
        Ptr<MgResourceIdentifier> mapRes3 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgdMap> map3 = new MgdMap(mapRes3, L"UnitTestSheboygan3");
        map3->SetViewScale(20e+6);

        // make a 4th runtime map
        Ptr<MgResourceIdentifier> mapRes4 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgdMap> map4 = new MgdMap(mapRes4, L"UnitTestSheboygan4");
        map4->SetViewScale(20e+6);

        // Create the DwfVersion
        Ptr<MgdDwfVersion> version = new MgdDwfVersion();

        // Create some plotSpecs
        Ptr<MgdPlotSpecification> plotSpec1 = new MgdPlotSpecification(8.5f, 11.f, L"in", 0.5, 0.5, 0.5, 0.5);
        Ptr<MgdPlotSpecification> plotSpec2 = new MgdPlotSpecification(11.f, 8.5f, L"in", 1, 1, 1, 1);
        Ptr<MgdPlotSpecification> plotSpec3 = new MgdPlotSpecification(8.f, 11.f, L"in", 0.25, 0.25, 0.25, 0.25);
        Ptr<MgdPlotSpecification> plotSpec4 = new MgdPlotSpecification(8.5f, 11.f, L"in", 0.25, 0.25, 0.25, 0.25);

        Ptr<MgResourceIdentifier> allElementsLayout = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");
        Ptr<MgdLayout> layout = new MgdLayout(allElementsLayout, L"Layout1", MgdUnitType::USEnglish);

        // Create some MapPlots
        Ptr<MgdMapPlot> mapPlot1 = new MgdMapPlot(map1, plotSpec1, layout);

        Ptr<MgCoordinate> center = new MgCoordinateXY(-100, 40);
        double scale = 50e+6;
        Ptr<MgdMapPlot> mapPlot2 = new MgdMapPlot(map2, center, scale, plotSpec2, layout);

        Ptr<MgCoordinate> ll = new MgCoordinateXY(-130, 45);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(-100, 60);
        Ptr<MgEnvelope> extent = new MgEnvelope(ll, ur);
        Ptr<MgdMapPlot> mapPlot3 = new MgdMapPlot(map3, extent, false, plotSpec3, layout);

        Ptr<MgdMapPlot> mapPlot4 = new MgdMapPlot(map4, extent, true, plotSpec4, layout);

        // Setup the MapPlot collection
        Ptr<MgdMapPlotCollection> mapPlots = new MgdMapPlotCollection();
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
        Ptr<MgdMap> map1 = new MgdMap(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(400e+6);

        Ptr<MgdDwfVersion> version = new MgdDwfVersion();

        Ptr<MgResourceIdentifier> allElementsLayout = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");

        Ptr<MgdPlotSpecification> plotSpec = new MgdPlotSpecification(8.5f, 11.f, L"inches", .5, .5, .5, .5);
        Ptr<MgCoordinate> center = new MgCoordinateXY(-120, 40);
        double scale = 20e+6;
        Ptr<MgCoordinate> ll = new MgCoordinateXY(-180, 0);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(0, 90);
        MgEnvelope extents(ll, ur);
        Ptr<MgdLayout> layout = new MgdLayout(allElementsLayout, L"TestTitle", MgdUnitType::USEnglish);

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
        Ptr<MgdMap> map1 = new MgdMap(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(400e+6);

        Ptr<MgdDwfVersion> version = new MgdDwfVersion();

        Ptr<MgResourceIdentifier> allElementsLayout = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");

        Ptr<MgdPlotSpecification> plotSpec = new MgdPlotSpecification(8.f, 11.f, L"inches", .5, .5, .5, .5);
        Ptr<MgCoordinate> center = new MgCoordinateXY(-120, 40);
        double scale = 20e+6;
        Ptr<MgCoordinate> ll = new MgCoordinateXY(-180, 0);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(0, 90);
        MgEnvelope extents(ll, ur);
        Ptr<MgdLayout> layout = new MgdLayout(allElementsLayout, L"TestTitle", MgdUnitType::USEnglish);

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
        Ptr<MgdMap> map1 = new MgdMap(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(400e+6);

        Ptr<MgdDwfVersion> version = new MgdDwfVersion();

        Ptr<MgResourceIdentifier> allElementsLayout = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");

        Ptr<MgdPlotSpecification> plotSpec = new MgdPlotSpecification(11.f, 8.5f, L"inches", .5, .5, .5, .5);
        Ptr<MgCoordinate> center = new MgCoordinateXY(-120, 40);
        double scale = 20e+6;
        Ptr<MgCoordinate> ll = new MgCoordinateXY(-180, 0);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(0, 90);
        Ptr<MgEnvelope> extents = new MgEnvelope(ll, ur);
        Ptr<MgdLayout> layout = new MgdLayout(allElementsLayout, L"TestTitle", MgdUnitType::USEnglish);

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
        Ptr<MgdMap> map1 = new MgdMap(mapRes1, L"UnitTestSheboygan1");
        map1->SetViewScale(400e+6);

        Ptr<MgdDwfVersion> version = new MgdDwfVersion();

        Ptr<MgResourceIdentifier> allElementsLayout = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");

        Ptr<MgdPlotSpecification> plotSpec = new MgdPlotSpecification(8.f, 11.f, L"inches", .5, .5, .5, .5);
        Ptr<MgCoordinate> center = new MgCoordinateXY(-120, 40);
        double scale = 20e+6;
        Ptr<MgCoordinate> ll = new MgCoordinateXY(-180, 0);
        Ptr<MgCoordinate> ur = new MgCoordinateXY(0, 90);
        Ptr<MgEnvelope> extents = new MgEnvelope(ll, ur);
        Ptr<MgdLayout> layout = new MgdLayout(allElementsLayout, L"TestTitle", MgdUnitType::USEnglish);

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