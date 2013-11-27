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

#ifndef _TESTRENDERINGSERVICE_H
#define _TESTRENDERINGSERVICE_H

#include <cppunit/extensions/HelperMacros.h>

class TestRenderingService : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestRenderingService);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_StylizationFunctionsPNG);

    CPPUNIT_TEST(TestCase_SymbologyPointsPNG);
    CPPUNIT_TEST(TestCase_SymbologyPointsParamPNG);
    CPPUNIT_TEST(TestCase_SymbologyLinesPNG);
    CPPUNIT_TEST(TestCase_SymbologyLinesCrossTickPNG);
    CPPUNIT_TEST(TestCase_SymbologyPolygonsPNG);

    CPPUNIT_TEST(TestCase_Annotation1PNG);
    CPPUNIT_TEST(TestCase_Annotation2PNG);
    CPPUNIT_TEST(TestCase_Annotation3PNG);

    CPPUNIT_TEST(TestCase_RenderDynamicOverlayPNG);
    CPPUNIT_TEST(TestCase_RenderDynamicOverlayTiledMapPNG);
    CPPUNIT_TEST(TestCase_RenderMapPNG);
    CPPUNIT_TEST(TestCase_RenderMapWithWatermarkPNG);
    CPPUNIT_TEST(TestCase_RenderLegendPNG);
    CPPUNIT_TEST(TestCase_RenderLegendEmptyGroupsPNG);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSPNG);
    CPPUNIT_TEST(TestCase_RenderLegendMultiCTSPNG);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSWithThemePNG);
    CPPUNIT_TEST(TestCase_RenderLegendMultiCTSWithThemePNG);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSSingleCTSPNG);
    CPPUNIT_TEST(TestCase_RenderLegendSingleFTSMultiCTSPNG);

    CPPUNIT_TEST(TestCase_StylizationFunctionsPNG8);

	CPPUNIT_TEST(TestCase_SymbologyPointsPNG8);
    CPPUNIT_TEST(TestCase_SymbologyPointsParamPNG8);
    CPPUNIT_TEST(TestCase_SymbologyLinesPNG8);
    CPPUNIT_TEST(TestCase_SymbologyLinesCrossTickPNG8);
    CPPUNIT_TEST(TestCase_SymbologyPolygonsPNG8);

    CPPUNIT_TEST(TestCase_Annotation1PNG8);
    CPPUNIT_TEST(TestCase_Annotation2PNG8);
    CPPUNIT_TEST(TestCase_Annotation3PNG8);

    CPPUNIT_TEST(TestCase_RenderDynamicOverlayPNG8);
    CPPUNIT_TEST(TestCase_RenderDynamicOverlayTiledMapPNG8);
    CPPUNIT_TEST(TestCase_RenderMapPNG8);
    CPPUNIT_TEST(TestCase_RenderMapWithWatermarkPNG8);
    CPPUNIT_TEST(TestCase_RenderLegendPNG8);
    CPPUNIT_TEST(TestCase_RenderLegendEmptyGroupsPNG8);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSPNG8);
    CPPUNIT_TEST(TestCase_RenderLegendMultiCTSPNG8);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSWithThemePNG8);
    CPPUNIT_TEST(TestCase_RenderLegendMultiCTSWithThemePNG8);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSSingleCTSPNG8);
    CPPUNIT_TEST(TestCase_RenderLegendSingleFTSMultiCTSPNG8);

    CPPUNIT_TEST(TestCase_StylizationFunctionsGIF);

	CPPUNIT_TEST(TestCase_SymbologyPointsGIF);
    CPPUNIT_TEST(TestCase_SymbologyPointsParamGIF);
    CPPUNIT_TEST(TestCase_SymbologyLinesGIF);
    CPPUNIT_TEST(TestCase_SymbologyLinesCrossTickGIF);
    CPPUNIT_TEST(TestCase_SymbologyPolygonsGIF);

    CPPUNIT_TEST(TestCase_Annotation1GIF);
    CPPUNIT_TEST(TestCase_Annotation2GIF);
    CPPUNIT_TEST(TestCase_Annotation3GIF);

    CPPUNIT_TEST(TestCase_RenderDynamicOverlayGIF);
    CPPUNIT_TEST(TestCase_RenderDynamicOverlayTiledMapGIF);
    CPPUNIT_TEST(TestCase_RenderMapGIF);
    CPPUNIT_TEST(TestCase_RenderMapWithWatermarkGIF);
    CPPUNIT_TEST(TestCase_RenderLegendGIF);
    CPPUNIT_TEST(TestCase_RenderLegendEmptyGroupsGIF);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSGIF);
    CPPUNIT_TEST(TestCase_RenderLegendMultiCTSGIF);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSWithThemeGIF);
    CPPUNIT_TEST(TestCase_RenderLegendMultiCTSWithThemeGIF);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSSingleCTSGIF);
    CPPUNIT_TEST(TestCase_RenderLegendSingleFTSMultiCTSGIF);

    CPPUNIT_TEST(TestCase_StylizationFunctionsJPG);

	CPPUNIT_TEST(TestCase_SymbologyPointsJPG);
    CPPUNIT_TEST(TestCase_SymbologyPointsParamJPG);
    CPPUNIT_TEST(TestCase_SymbologyLinesJPG);
    CPPUNIT_TEST(TestCase_SymbologyLinesCrossTickJPG);
    CPPUNIT_TEST(TestCase_SymbologyPolygonsJPG);

    CPPUNIT_TEST(TestCase_Annotation1JPG);
    CPPUNIT_TEST(TestCase_Annotation2JPG);
    CPPUNIT_TEST(TestCase_Annotation3JPG);

    CPPUNIT_TEST(TestCase_RenderDynamicOverlayJPG);
    CPPUNIT_TEST(TestCase_RenderDynamicOverlayTiledMapJPG);
    CPPUNIT_TEST(TestCase_RenderMapJPG);
    CPPUNIT_TEST(TestCase_RenderMapWithWatermarkJPG);
    CPPUNIT_TEST(TestCase_RenderLegendJPG);
    CPPUNIT_TEST(TestCase_RenderLegendEmptyGroupsJPG);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSJPG);
    CPPUNIT_TEST(TestCase_RenderLegendMultiCTSJPG);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSWithThemeJPG);
    CPPUNIT_TEST(TestCase_RenderLegendMultiCTSWithThemeJPG);
    CPPUNIT_TEST(TestCase_RenderLegendMultiFTSSingleCTSJPG);
    CPPUNIT_TEST(TestCase_RenderLegendSingleFTSMultiCTSJPG);

    CPPUNIT_TEST(TestCase_QueryFeatures);

    //CPPUNIT_TEST(TestCase_RendererPerformance);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    TestRenderingService();
    ~TestRenderingService();

    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_QueryFeatures();

    //Parameterized versions that all format-specific tests call into
    void TestCase_RenderDynamicOverlay(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderDynamicOverlayTiledMap(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderMap(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderMapWithWatermark(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderLegend(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderLegendEmptyGroups(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderLegendMultiFTS(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderLegendMultiCTS(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderLegendMultiFTSWithTheme(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderLegendMultiCTSWithTheme(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderLegendMultiFTSSingleCTS(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_RenderLegendSingleFTSMultiCTS(CREFSTRING imageFormat, CREFSTRING extension);
    
    void TestCase_SymbologyPoints(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_SymbologyPointsParam(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_SymbologyLines(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_SymbologyLinesCrossTick(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_SymbologyPolygons(CREFSTRING imageFormat, CREFSTRING extension);

    void TestCase_Annotation1(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_Annotation2(CREFSTRING imageFormat, CREFSTRING extension);
    void TestCase_Annotation3(CREFSTRING imageFormat, CREFSTRING extension);

    void TestCase_StylizationFunctions(CREFSTRING imageFormat, CREFSTRING extension);

    //PNG output tests
    void TestCase_RenderDynamicOverlayPNG() { TestCase_RenderDynamicOverlay(L"PNG", L"png"); }
    void TestCase_RenderDynamicOverlayTiledMapPNG() { TestCase_RenderDynamicOverlayTiledMap(L"PNG", L"png"); }
    void TestCase_RenderMapPNG() { TestCase_RenderMap(L"PNG", L"png"); }
    void TestCase_RenderMapWithWatermarkPNG() { TestCase_RenderMapWithWatermark(L"PNG", L"png"); }
    void TestCase_RenderLegendPNG() { TestCase_RenderLegend(L"PNG", L"png"); }
    void TestCase_RenderLegendEmptyGroupsPNG() { TestCase_RenderLegendEmptyGroups(L"PNG", L"png"); }
    void TestCase_SymbologyPointsPNG() { TestCase_SymbologyPoints(L"PNG", L"png"); }
    void TestCase_SymbologyPointsParamPNG() { TestCase_SymbologyPointsParam(L"PNG", L"png"); }
    void TestCase_SymbologyLinesPNG() { TestCase_SymbologyLines(L"PNG", L"png"); }
    void TestCase_SymbologyLinesCrossTickPNG() { TestCase_SymbologyLinesCrossTick(L"PNG", L"png"); }
    void TestCase_SymbologyPolygonsPNG() { TestCase_SymbologyPolygons(L"PNG", L"png"); }
    void TestCase_Annotation1PNG() { TestCase_Annotation1(L"PNG", L"png"); }
    void TestCase_Annotation2PNG() { TestCase_Annotation2(L"PNG", L"png"); }
    void TestCase_Annotation3PNG() { TestCase_Annotation3(L"PNG", L"png"); }
    void TestCase_RenderLegendMultiFTSPNG() { TestCase_RenderLegendMultiFTS(L"PNG", L"png"); }
    void TestCase_RenderLegendMultiCTSPNG() { TestCase_RenderLegendMultiCTS(L"PNG", L"png"); }
    void TestCase_RenderLegendMultiFTSWithThemePNG() { TestCase_RenderLegendMultiFTSWithTheme(L"PNG", L"png"); }
    void TestCase_RenderLegendMultiCTSWithThemePNG() { TestCase_RenderLegendMultiCTSWithTheme(L"PNG", L"png"); }
    void TestCase_RenderLegendMultiFTSSingleCTSPNG() { TestCase_RenderLegendMultiFTSSingleCTS(L"PNG", L"png"); }
    void TestCase_RenderLegendSingleFTSMultiCTSPNG() { TestCase_RenderLegendSingleFTSMultiCTS(L"PNG", L"png"); }
    void TestCase_StylizationFunctionsPNG() { TestCase_StylizationFunctions(L"PNG", L"png"); }

    //PNG8 output tests
    void TestCase_RenderDynamicOverlayPNG8() { TestCase_RenderDynamicOverlay(L"PNG8", L"png"); }
    void TestCase_RenderDynamicOverlayTiledMapPNG8() { TestCase_RenderDynamicOverlayTiledMap(L"PNG8", L"png"); }
    void TestCase_RenderMapPNG8() { TestCase_RenderMap(L"PNG8", L"png"); }
    void TestCase_RenderMapWithWatermarkPNG8() { TestCase_RenderMapWithWatermark(L"PNG8", L"png"); }
    void TestCase_RenderLegendPNG8() { TestCase_RenderLegend(L"PNG8", L"png"); }
    void TestCase_RenderLegendEmptyGroupsPNG8() { TestCase_RenderLegendEmptyGroups(L"PNG8", L"png"); }
    void TestCase_SymbologyPointsPNG8() { TestCase_SymbologyPoints(L"PNG8", L"png"); }
    void TestCase_SymbologyPointsParamPNG8() { TestCase_SymbologyPointsParam(L"PNG8", L"png"); }
    void TestCase_SymbologyLinesPNG8() { TestCase_SymbologyLines(L"PNG8", L"png"); }
    void TestCase_SymbologyLinesCrossTickPNG8() { TestCase_SymbologyLinesCrossTick(L"PNG8", L"png"); }
    void TestCase_SymbologyPolygonsPNG8() { TestCase_SymbologyPolygons(L"PNG8", L"png"); }
    void TestCase_Annotation1PNG8() { TestCase_Annotation1(L"PNG8", L"png"); }
    void TestCase_Annotation2PNG8() { TestCase_Annotation2(L"PNG8", L"png"); }
    void TestCase_Annotation3PNG8() { TestCase_Annotation3(L"PNG8", L"png"); }
    void TestCase_RenderLegendMultiFTSPNG8() { TestCase_RenderLegendMultiFTS(L"PNG8", L"png"); }
    void TestCase_RenderLegendMultiCTSPNG8() { TestCase_RenderLegendMultiCTS(L"PNG8", L"png"); }
    void TestCase_RenderLegendMultiFTSWithThemePNG8() { TestCase_RenderLegendMultiFTSWithTheme(L"PNG8", L"png"); }
    void TestCase_RenderLegendMultiCTSWithThemePNG8() { TestCase_RenderLegendMultiCTSWithTheme(L"PNG8", L"png"); }
    void TestCase_RenderLegendMultiFTSSingleCTSPNG8() { TestCase_RenderLegendMultiFTSSingleCTS(L"PNG8", L"png"); }
    void TestCase_RenderLegendSingleFTSMultiCTSPNG8() { TestCase_RenderLegendSingleFTSMultiCTS(L"PNG8", L"png"); }
    void TestCase_StylizationFunctionsPNG8() { TestCase_StylizationFunctions(L"PNG8", L"png"); }

    //GIF output tests
    void TestCase_RenderDynamicOverlayGIF() { TestCase_RenderDynamicOverlay(L"GIF", L"gif"); }
    void TestCase_RenderDynamicOverlayTiledMapGIF() { TestCase_RenderDynamicOverlayTiledMap(L"GIF", L"gif"); }
    void TestCase_RenderMapGIF() { TestCase_RenderMap(L"GIF", L"gif"); }
    void TestCase_RenderMapWithWatermarkGIF() { TestCase_RenderMapWithWatermark(L"GIF", L"gif"); }
    void TestCase_RenderLegendGIF() { TestCase_RenderLegend(L"GIF", L"gif"); }
    void TestCase_RenderLegendEmptyGroupsGIF() { TestCase_RenderLegendEmptyGroups(L"GIF", L"gif"); }
    void TestCase_SymbologyPointsGIF() { TestCase_SymbologyPoints(L"GIF", L"gif"); }
    void TestCase_SymbologyPointsParamGIF() { TestCase_SymbologyPointsParam(L"GIF", L"gif"); }
    void TestCase_SymbologyLinesGIF() { TestCase_SymbologyLines(L"GIF", L"gif"); }
    void TestCase_SymbologyLinesCrossTickGIF() { TestCase_SymbologyLinesCrossTick(L"GIF", L"gif"); }
    void TestCase_SymbologyPolygonsGIF() { TestCase_SymbologyPolygons(L"GIF", L"gif"); }
    void TestCase_Annotation1GIF() { TestCase_Annotation1(L"GIF", L"gif"); }
    void TestCase_Annotation2GIF() { TestCase_Annotation2(L"GIF", L"gif"); }
    void TestCase_Annotation3GIF() { TestCase_Annotation3(L"GIF", L"gif"); }
    void TestCase_RenderLegendMultiFTSGIF() { TestCase_RenderLegendMultiFTS(L"GIF", L"gif"); }
    void TestCase_RenderLegendMultiCTSGIF() { TestCase_RenderLegendMultiCTS(L"GIF", L"gif"); }
    void TestCase_RenderLegendMultiFTSWithThemeGIF() { TestCase_RenderLegendMultiFTSWithTheme(L"GIF", L"gif"); }
    void TestCase_RenderLegendMultiCTSWithThemeGIF() { TestCase_RenderLegendMultiCTSWithTheme(L"GIF", L"gif"); }
    void TestCase_RenderLegendMultiFTSSingleCTSGIF() { TestCase_RenderLegendMultiFTSSingleCTS(L"GIF", L"gif"); }
    void TestCase_RenderLegendSingleFTSMultiCTSGIF() { TestCase_RenderLegendSingleFTSMultiCTS(L"GIF", L"gif"); }
    void TestCase_StylizationFunctionsGIF() { TestCase_StylizationFunctions(L"GIF", L"gif"); }

    //JPG output tests
    void TestCase_RenderDynamicOverlayJPG() { TestCase_RenderDynamicOverlay(L"JPG", L"jpg"); }
    void TestCase_RenderDynamicOverlayTiledMapJPG() { TestCase_RenderDynamicOverlayTiledMap(L"JPG", L"jpg"); }
    void TestCase_RenderMapJPG() { TestCase_RenderMap(L"JPG", L"jpg"); }
    void TestCase_RenderMapWithWatermarkJPG() { TestCase_RenderMapWithWatermark(L"JPG", L"jpg"); }
    void TestCase_RenderLegendJPG() { TestCase_RenderLegend(L"JPG", L"jpg"); }
    void TestCase_RenderLegendEmptyGroupsJPG() { TestCase_RenderLegendEmptyGroups(L"JPG", L"jpg"); }
    void TestCase_SymbologyPointsJPG() { TestCase_SymbologyPoints(L"JPG", L"jpg"); }
    void TestCase_SymbologyPointsParamJPG() { TestCase_SymbologyPointsParam(L"JPG", L"jpg"); }
    void TestCase_SymbologyLinesJPG() { TestCase_SymbologyLines(L"JPG", L"jpg"); }
    void TestCase_SymbologyLinesCrossTickJPG() { TestCase_SymbologyLinesCrossTick(L"JPG", L"jpg"); }
    void TestCase_SymbologyPolygonsJPG() { TestCase_SymbologyPolygons(L"JPG", L"jpg"); }
    void TestCase_Annotation1JPG() { TestCase_Annotation1(L"JPG", L"jpg"); }
    void TestCase_Annotation2JPG() { TestCase_Annotation2(L"JPG", L"jpg"); }
    void TestCase_Annotation3JPG() { TestCase_Annotation3(L"JPG", L"jpg"); }
    void TestCase_RenderLegendMultiFTSJPG() { TestCase_RenderLegendMultiFTS(L"JPG", L"jpg"); }
    void TestCase_RenderLegendMultiCTSJPG() { TestCase_RenderLegendMultiCTS(L"JPG", L"jpg"); }
    void TestCase_RenderLegendMultiFTSWithThemeJPG() { TestCase_RenderLegendMultiFTSWithTheme(L"JPG", L"jpg"); }
    void TestCase_RenderLegendMultiCTSWithThemeJPG() { TestCase_RenderLegendMultiCTSWithTheme(L"JPG", L"jpg"); }
    void TestCase_RenderLegendMultiFTSSingleCTSJPG() { TestCase_RenderLegendMultiFTSSingleCTS(L"JPG", L"jpg"); }
    void TestCase_RenderLegendSingleFTSMultiCTSJPG() { TestCase_RenderLegendSingleFTSMultiCTS(L"JPG", L"jpg"); }
    void TestCase_StylizationFunctionsJPG() { TestCase_StylizationFunctions(L"JPG", L"jpg"); }

    //void TestCase_RendererPerformance();

private:
    MgdMap* CreateTestMap();
    MgdMap* CreateTestTiledMap();
    MgdMap* CreateTestStylizationFunctionMap();
    MgdMap* CreateTestMapWithWatermark();
    MgPolygon* CreateSelectionPolygon(MgdMap* map, double width, double height);
    static STRING GetPath(CREFSTRING basePath, CREFSTRING imageFormat, CREFSTRING extension);

private:
    Ptr<MgResourceService> m_svcResource;
    Ptr<MgdRenderingService> m_svcRendering;
};

#endif // _TESTRENDERINGSERVICE_H
