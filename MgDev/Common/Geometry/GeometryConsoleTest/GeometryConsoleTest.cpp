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
///============================================================================
// GeometryConsoleTest.cpp : Defines the entry point for the console
// test application fo the Geometry.dll object.
//
//#pragma warning (disable: 4275)

#include "GeometryCommon.h"
#include "GeometryConsoleTest.h"

int main(int argc, const char* argv[])
{
    bool ok;

    wchar_t currentTest;

    INT32 testIndex;
    INT32 errorCount;
    INT32 testCount;
    INT32 successCount;
    INT32 failCount;

    MgGctTestParameters cmdLineParms;

    ok = cmdLineParms.Initialize (argc,argv);
    if (!ok)
    {
        printf ("Comand line error: %s\n",cmdLineParms.GetReason());
        cmdLineParms.Usage (cmdLineParms.Background ());
        return -1;
    }

    successCount = failCount = errorCount = 0;
    testCount = cmdLineParms.GetTestCount ();
    for (testIndex = 0;testIndex < testCount;testIndex += 1)
    {
        currentTest = cmdLineParms.GetTestChar (testIndex);
        if (currentTest == L'\0') break;

        switch (currentTest) {

        case L'1':
            errorCount = GctTest1 (cmdLineParms);
            break;

        case L'2':
            errorCount = GctTest2 (cmdLineParms);
            break;

        case L'3':
            errorCount = GctTest3 (cmdLineParms);
            break;

        case L'4':
            errorCount = GctTest4 (cmdLineParms);
            break;

        case L'5':
            errorCount = GctTest5 (cmdLineParms);
            break;

        case L'6':
            errorCount = GctTest6 (cmdLineParms);
            break;

        case L'7':
            errorCount = GctTest7 (cmdLineParms);
            break;

        case L'8':
            errorCount = GctTest8 (cmdLineParms);
            break;

        case L'9':
            errorCount = GctTest9 (cmdLineParms);
            break;

        case L'A':
            errorCount = GctTestA (cmdLineParms);
            break;

        case L'T':
            errorCount = GctTestT (cmdLineParms);
            break;
        }
        successCount += (errorCount == 0);
        failCount    += (errorCount != 0);
        if (!cmdLineParms.Silent())
        {
            if (errorCount > 0)
            {
                wprintf (L"\tTest '%c' failed; %d errors detected.\n",currentTest,errorCount);
            }
            else if (errorCount == -1)
            {
                wprintf (L"\tTest '%c' failed; unexpected MapGuide exception encountered.\n",currentTest);
            }
            if (errorCount == -2)
            {
                 wprintf (L"\tTest '%c' failed;  failed; unexpected non-MapGuide exception encountered.\n",currentTest);
            }
        }
    }

    if (!cmdLineParms.Silent())
    {
        wprintf (L"\tGeometry Console Test complete: %d tests completed, %d successes, %d failures.\n",
                                                                        testCount,
                                                                        successCount,
                                                                        failCount);
    }
    if (!cmdLineParms.Background ())
    {
        wchar_t anyLine [128];

        wcout << L"Press 'Enter' to continue: ";
        wcout.flush ();
        wcin.getline (anyLine,128);
        wcout << std::endl;
    }
    return (failCount == 0) ? 0 : -1;
}
//=============================================================================
// The following is some test code that has been around for a while.  We
// preserve it as it comes in handy when a specific defect test needs to be
// coded up.  You might find it already coded below.
#ifdef __SKIP__

    // This test tests some problematical WKT strings which produced a
    // regression in Reeses.

    STRING codeOne;
    STRING codeTwo;
    STRING codeThree;
    STRING codeFour;
    STRING codeFive;
    STRING codeSix;
 
    STRING wktOne   (L"PROJCS[\"DHDN / Gauss-Kruger zone 5\",GEOGCS[\"DHDN\",DATUM[\"Deutsches_Hauptdreiecksnetz\",SPHEROID[\"Bessel 1841\",6377397.155,299.1528128,AUTHORITY[\"EPSG\",\"7004\"]],AUTHORITY[\"EPSG\",\"6314\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4314\"]],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",15],PARAMETER[\"scale_factor\",1],PARAMETER[\"false_easting\",5500000],PARAMETER[\"false_northing\",0],AUTHORITY[\"EPSG\",\"31469\"]]");
    STRING wktTwo   (L"GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.000,298.25722293]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.01745329251994]]");
    STRING wktThree (L"PROJCS[\"NAD83 / California zone 3 (ftUS)\",GEOGCS[\"NAD83\",DATUM[\"North_American_Datum_1983\",SPHEROID[\"GRS 1980\",6378137,298.257222101,AUTHORITY[\"EPSG\",\"7019\"]],AUTHORITY[\"EPSG\",\"6269\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4269\"]],UNIT[\"US survey foot\",0.3048006096012192,AUTHORITY[\"EPSG\",\"9003\"]],PROJECTION[\"Lambert_Conformal_Conic_2SP\"],PARAMETER[\"standard_parallel_1\",38.43333333333333],PARAMETER[\"standard_parallel_2\",37.06666666666667],PARAMETER[\"latitude_of_origin\",36.5],PARAMETER[\"central_meridian\",-120.5],PARAMETER[\"false_easting\",6561666.667],PARAMETER[\"false_northing\",1640416.667],AUTHORITY[\"EPSG\",\"2227\"],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]");
    STRING wktFour  (L"PROJCS[\"DHDN.Berlin/Cassini\",GEOGCS[\"DHDN.LL\",DATUM[\"DHDN\",SPHEROID[\"BESSEL\",6377397.155,299.15281535],TOWGS84[582.0000,105.0000,414.0000,-1.040000,-0.350000,3.080000,8.30000000]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Cassini-Soldner\"],PARAMETER[\"false_easting\",40000.000],PARAMETER[\"false_northing\",10000.000],PARAMETER[\"central_meridian\",13.62720366666667],PARAMETER[\"latitude_of_origin\",52.41864827777778],UNIT[\"Meter\",1.00000000000000]]");
    STRING wktFive  (L"PROJCS[\"NAD_1983_UTM_Zone_12N\",GEOGCS[\"GCS_North_American_1983\",DATUM[\"D_North_American_1983\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-111.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]");
    STRING wktSix   (L"PROJCS[\"NAD_1983_HARN_StatePlane_Hawaii_3_FIPS_5103_Feet\",GEOGCS[\"GCS_North_American_1983_HARN\",DATUM[\"D_North_American_1983_HARN\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",1640416.666666667],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-158.0],PARAMETER[\"Scale_Factor\",0.99999],PARAMETER[\"Latitude_Of_Origin\",21.16666666666667],UNIT[\"Foot_US\",0.3048006096012192]]");

    MgCoordinateSystemFactory factory;
    MgCoordinateSystemCatalog* catalog;
    MgCoordinateSystemDictionary* coordsysDict;
    MgCoordinateSystemFormatConverter* formatConverter;
    MgCoordinateSystemEnum* coordsysEnum;
    MgStringCollection* stringCollection;
    MgCoordinateSystem* coordsysOne;
    MgCoordinateSystem* coordsysTwo;
    MgCoordinateSystem* coordsysThree;
    MgCoordinateSystem* coordsysFour;
    MgCoordinateSystem* coordsysFive;
    MgCoordinateSystem* coordsysSix;
    MgCoordinateSystemTransform* xfrmOne;
    MgCoordinateSystemTransform* xfrmThree;
    MgCoordinateSystemTransform* xfrmFour;
    MgCoordinateSystemTransform* xfrmFive;
    MgCoordinateSystemTransform* xfrmSix;

    catalog = factory.GetCatalog ();
    coordsysDict = catalog->GetCoordinateSystemDictionary ();
    formatConverter = catalog->GetFormatConverter();
    coordsysEnum = coordsysDict->GetEnum ();

    coordsysFive = formatConverter->WktToDefinition(MgCoordinateSystemWktFlavor::Unknown,wktFive);
    coordsysFive->Release ();
    coordsysFive = formatConverter->WktToDefinition(MgCoordinateSystemWktFlavor::Esri,wktFive);
    coordsysFive->Release ();
    coordsysFive = formatConverter->WktToDefinition(MgCoordinateSystemWktFlavor::Ogc,wktFive);
    coordsysFive->Release ();

    codeOne   = factory.ConvertWktToCoordinateSystemCode(wktOne);
    codeTwo   = factory.ConvertWktToCoordinateSystemCode(wktTwo);
    codeThree = factory.ConvertWktToCoordinateSystemCode(wktThree);
    codeFour  = factory.ConvertWktToCoordinateSystemCode(wktFour);
    codeFive  = factory.ConvertWktToCoordinateSystemCode(wktFive);
    codeSix   = factory.ConvertWktToCoordinateSystemCode(wktSix);

    coordsysOne   = factory.CreateFromCode (codeOne);
    coordsysTwo   = factory.CreateFromCode (codeTwo);
    coordsysThree = factory.CreateFromCode (codeThree);
    coordsysFour  = factory.CreateFromCode (codeFour);
    coordsysFive  = factory.CreateFromCode (codeFive);
    coordsysSix   = factory.CreateFromCode (codeSix);



    xfrmOne   = factory.GetTransform(coordsysOne,coordsysTwo);
    xfrmThree = factory.GetTransform(coordsysThree,coordsysTwo);
    xfrmFour  = factory.GetTransform(coordsysFour,coordsysTwo);
    xfrmFive  = factory.GetTransform(coordsysFive,coordsysTwo);
    xfrmSix   = factory.GetTransform(coordsysSix,coordsysTwo);

    xfrmSix->Release ();
    xfrmFive->Release ();
    xfrmFour->Release ();
    xfrmThree->Release ();
    xfrmOne->Release ();

    coordsysSix->Release ();
    coordsysFive->Release ();
    coordsysFour->Release ();
    coordsysThree->Release ();
    coordsysTwo->Release ();
    coordsysOne->Release ();

    coordsysEnum->Release ();
    coordsysDict->Release ();
    formatConverter->Release ();
    catalog->Release (); 
     
#endif
#ifdef __SKIP__
    // Some Catalog/Dictionary testing and timing.
    MgCoordinateSystemFactory factory;
    MgCoordinateSystemCatalog* catalog;

    MgCoordinateSystemDictionary* coordsysDict;
    MgCoordinateSystemEnum* coordsysEnum;

    MgStringCollection* stringCollection;

    catalog = factory.GetCatalog ();
    coordsysDict = catalog->GetCoordinateSystemDictionary ();
    coordsysEnum = coordsysDict->GetEnum ();

    do
    {
        stringCollection = coordsysEnum->NextName (1);
        if (stringCollection->GetCount () > 0)
        {
            STRING code = stringCollection->GetItem (0);
            Ptr<MgCoordinateSystem> coordSysPtr = factory.CreateFromCode (code);
            printf ("Code = %S, EPSG = %d, SRID = %d\n",(coordSysPtr->GetCsCode()).c_str (),coordSysPtr->GetEpsgCode (),coordSysPtr->GetSridCode ());
        }
    } while (stringCollection->GetCount () > 0);
#endif
#ifdef __SKIP__
    // Some Catalog/Dictionary testing and timing.
    bool isValid;
   
    clock_t startTime;
    clock_t endTime;

    MgCoordinateSystemFactory factory;
    MgCoordinateSystemCatalog* catalog;

    MgCoordinateSystemCategoryDictionary* categoryDict;
    MgCoordinateSystemEllipsoidDictionary* ellipsoidDict;
    MgCoordinateSystemDatumDictionary* datumDict;
    MgCoordinateSystemDictionary* coordsysDict;
    MgCoordinateSystemGeodeticTransformDefDictionary* xformDict;
    MgCoordinateSystemGeodeticPathDictionary* pathDict;
    
    MgCoordinateSystemEnum* categoryEnum;
    MgCoordinateSystemEnum* ellipsoidEnum;
    MgCoordinateSystemEnum* datumEnum;
    MgCoordinateSystemEnum* coordsysEnum;
    MgCoordinateSystemEnum* xformEnum;
    MgCoordinateSystemEnum* pathEnum;

    MgStringCollection* stringCollection;

    startTime = clock ();
    catalog = factory.GetCatalog ();
    categoryDict = catalog->GetCategoryDictionary ();
    ellipsoidDict = catalog->GetEllipsoidDictionary ();
    datumDict = catalog->GetDatumDictionary ();
    coordsysDict = catalog->GetCoordinateSystemDictionary ();
    xformDict = catalog->GetGeodeticTransformDefDictionary ();
    pathDict = catalog->GetGeodeticPathDictionary ();

    categoryEnum = categoryDict->GetEnum ();
    ellipsoidEnum = ellipsoidDict->GetEnum ();
    datumEnum = datumDict->GetEnum ();
    coordsysEnum = coordsysDict->GetEnum ();
    xformEnum = xformDict->GetEnum ();
    pathEnum = pathDict->GetEnum ();

    do
    {
        stringCollection = coordsysEnum->NextName (1);
        if (stringCollection->GetCount () > 0)
        {
            STRING code = stringCollection->GetItem (0);
            Ptr<MgCoordinateSystem> coordSysPtr = factory.CreateFromCode (code);
        }
    } while (stringCollection->GetCount () > 0);

    Ptr<MgCoordinateSystemGeodeticTransformDef> xformDef;
    xformDef = xformDict->GetGeodeticTransformationDef (L"NAD83_to_HPGN");
    isValid = xformDef->IsValid ();
    Ptr<MgCoordinateSystemGeodeticInterpolationTransformDefParams>interpParms;
    interpParms = static_cast<MgCoordinateSystemGeodeticInterpolationTransformDefParams*>(xformDef->GetParameters ());
    Ptr<MgDisposableCollection>gridFiles = interpParms->GetGridFiles ();
    Ptr<MgCoordinateSystemGeodeticTransformGridFile>gridFile;
    for (INT32 idx = 0;idx < gridFiles->GetCount ();idx += 1)
    {
        gridFile = static_cast<MgCoordinateSystemGeodeticTransformGridFile*>(gridFiles->GetItem (idx));
    }

    endTime = clock ();
    printf ("Dictionary enumeration time = %ld milliseconds.\n",endTime - startTime);
#endif
#ifdef __SKIP__
    // testing a special WKT string for proper operation.
    clock_t startTime;
    clock_t endTime;

    MgCoordinateSystemFactory factory;

    wchar_t wktTest [2048] = L"PROJCS[\"UTM83-10F\",GEOGCS[\"LL83\",DATUM[\"NAD83\",SPHEROID[\"GRS1980\",6378137.000,298.25722210]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"false_easting\",1640416.667],PARAMETER[\"false_northing\",0.000],PARAMETER[\"central_meridian\",-123.00000000000000],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"latitude_of_origin\",0.000],UNIT[\"Foot_US\",0.30480060960122]]";
    //wchar_t wktTest [2048] = L"PROJCS[\"WGS84.PseudoMercator\",GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.000,298.25722293]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Popular Visualisation Pseudo Mercator\"],PARAMETER[\"false_easting\",0.000],PARAMETER[\"false_northing\",0.000],PARAMETER[\"central_meridian\",0.00000000000000],UNIT[\"Meter\",1.00000000000000]]";
    startTime = clock ();
    INT32 epsgCode = factory.ConvertWktToEpsgCode (wktTest);
    endTime = clock ();
    printf ("ConvertWktToEpsgCode time = %ld milliseconds.\n",endTime - startTime);
#endif
#ifdef __SKIP__
    // Test/Debug bed for the MGRS Grid functionality.

    INT32 index1;
    INT32 index2;
    INT32 index3;
    INT32 index4;
    INT32 tickCount;
    INT32 orientation;
    INT32 regionCount;
    INT32 gridLineCount;
    INT32 lineSegmentCount;

    double xx;
    double yy;
    double uu;
    double vv;
    double value;

    MgCoordinateSystemFactory factory;

    MgCoordinate* pCoordinate;
    MgCoordinateIterator* coordItr;
    MgLineString* pLineString;
    MgCoordinateSystemGridLine* pGridLine;
    MgCoordinateSystemGridBase* pGenericGrid;
    MgCoordinateSystemGridBase* pMgrs;
    MgCoordinateSystemGridBoundary* pGridBoundary;
    MgCoordinateSystemGridLineCollection* pGridLines;
    MgCoordinateSystemGridSpecification* pGridSpecification;
    MgCoordinateSystemGridRegion* pGridRegion;
    MgCoordinateSystemGridRegionCollection* pGridRegions;
    MgCoordinateSystemGridTick* pGridTick;
    MgCoordinateSystemGridTickCollection* pGridTicks;

    pGridSpecification = factory.GridSpecification ();
    pGridSpecification->SetGridBase (0.0,0.0);
    pGridSpecification->SetGridIncrement (1.0,1.0);
    pGridSpecification->SetTickIncrements  (0.25,0.25);
    pGridSpecification->SetUnits (MgCoordinateSystemUnitCode::Degree,MgCoordinateSystemUnitType::Angular);
    pGridSpecification->SetCurvePrecision (1.0E-05);

    {
        MgCoordinateSystem* pTestSource = factory.CreateFromCode(L"CA83-IIF");
        MgCoordinateSystem* pTestTarget = factory.CreateFromCode(L"LL84");
        MgCoordinateSystemTransform* pTestTransform = factory.GetTransform(pTestSource,pTestTarget);
        MgCoordinate* southWestXYZ = new MgCoordinateXYZ (6.49E+06,1.99E+06,300.0);
        MgCoordinate* northEastXYZ = new MgCoordinateXYZ (6.61E+06,2.11E+06,100.0);
        MgEnvelope* pTestEnvelopeSource = new MgEnvelope (northEastXYZ,southWestXYZ);
        MgEnvelope* pTestEnvelopeTarget = pTestEnvelopeSource->Transform (pTestTransform);
        pTestTransform->Release ();
        pTestSource->Release ();
        pTestTarget->Release ();
        pTestEnvelopeSource->Release ();
        pTestEnvelopeTarget->Release ();
    }

    pGenericGrid = factory.GenericGrid (L"LL84",L"CA83-IIF",true);

    MgCoordinate* southWestXY = new MgCoordinateXY (6.49E+06,1.99E+06);
    MgCoordinate* northEastXY = new MgCoordinateXY (6.61E+06,2.11E+06);
    pGridBoundary = factory.GridBoundary (southWestXY,northEastXY);
    southWestXY->Release ();
    northEastXY->Release ();

    pGenericGrid->SetBoundary (pGridBoundary);
    pGridBoundary->Release ();

    pGridLines = pGenericGrid->GetGridLines (pGridSpecification);
    gridLineCount = pGridLines->GetCount ();
    for (index1 = 0;index1 < gridLineCount;index1 += 1)
    {
        pGridLine = pGridLines->GetItem (index1);
        value = pGridLine->GetRealValue ();
        lineSegmentCount = pGridLine->GetCount ();
        for (index2 = 0;index2 < lineSegmentCount;index2 += 1)
        {
            pLineString = pGridLine->GetSegment (index2);
            coordItr = pLineString->GetCoordinates ();
            while (coordItr->MoveNext ())
            {
                pCoordinate = coordItr->GetCurrent ();
                xx = pCoordinate->GetX ();
                yy = pCoordinate->GetY ();
                pCoordinate->Release ();
            }
            coordItr->Release ();
            pLineString->Release ();
        }
        pGridLine->Release ();
    }
    pGridLines->Release ();

    pGridTicks = pGenericGrid->GetGridTicks (pGridSpecification);
    tickCount = pGridTicks->GetCount ();
    for (index4 = 0;index4 < tickCount;index4 += 1)
    {
        pGridTick = pGridTicks->GetItem (index4);
        value = pGridTick->GetValue ();
        orientation = pGridTick->GetTickOrientation ();
        pCoordinate = pGridTick->GetPosition ();
        xx = pCoordinate->GetX ();
        yy = pCoordinate->GetY ();
        pCoordinate->Release ();
        pCoordinate = pGridTick->GetDirectionVector ();
        uu = pCoordinate->GetX ();
        vv = pCoordinate->GetY ();
        pCoordinate->Release ();
        pGridTick->Release ();
    }
    pGridTicks->Release ();

    pGenericGrid->Release ();
    pGridSpecification->Release ();

    pGridSpecification = factory.GridSpecification (MgCoordinateSystemGridSpecializationType::MGRS,
                                                    MgCoordinateSystemMgrsGridLevel::Mgrs1Km);

    pGridSpecification = factory.GridSpecification ();
    pGridSpecification->SetGridBase (0.0,0.0);
    pGridSpecification->SetGridIncrement (100.0,100.0);
    pGridSpecification->SetTickIncrements  (10.0,10.0);
    pGridSpecification->SetUnits (MgCoordinateSystemUnitCode::Kilometer,MgCoordinateSystemUnitType::Linear);
    pGridSpecification->SetCurvePrecision (0.5);

    southWestXY = new MgCoordinateXY (6.49E+06,1.99E+06);
    northEastXY = new MgCoordinateXY (6.61E+06,2.11E+06);
    pGridBoundary = factory.GridBoundary (southWestXY,northEastXY);
    southWestXY->Release ();
    northEastXY->Release ();

    pMgrs = factory.MgrsGrid (L"CA83-IIF",false,MgCoordinateSystemMgrsLetteringScheme::Normal,true);
    pMgrs->SetBoundary (pGridBoundary);
    pGridBoundary->Release ();

    pGridLines = pMgrs->GetGridLines (pGridSpecification);
    gridLineCount = pGridLines->GetCount ();
    for (index1 = 0;index1 < gridLineCount;index1 += 1)
    {
        pGridLine = pGridLines->GetItem (index1);
        value = pGridLine->GetRealValue ();
        lineSegmentCount = pGridLine->GetCount ();
        for (index2 = 0;index2 < lineSegmentCount;index2 += 1)
        {
            pLineString = pGridLine->GetSegment (index2);
            coordItr = pLineString->GetCoordinates ();
            while (coordItr->MoveNext ())
            {
                pCoordinate = coordItr->GetCurrent ();
                xx = pCoordinate->GetX ();
                yy = pCoordinate->GetY ();
                pCoordinate->Release ();
            }
            coordItr->Release ();
            pLineString->Release ();
        }
        pGridLine->Release ();
    }
    pGridLines->Release ();

    pGridRegions = pMgrs->GetGridRegions (pGridSpecification);
    regionCount = pGridRegions->GetCount ();
    for (index3 = 0;index3 < regionCount;index3 += 1)
    {
        pGridRegion = pGridRegions->GetItem (index3);
        pGridRegion->Release ();
    }
    pGridRegions->Release ();

    pGridTicks = pMgrs->GetGridTicks (pGridSpecification);
    tickCount = pGridTicks->GetCount ();
    for (index4 = 0;index4 < tickCount;index4 += 1)
    {
        pGridTick = pGridTicks->GetItem (index4);
        value = pGridTick->GetValue ();
        orientation = pGridTick->GetTickOrientation ();
        pCoordinate = pGridTick->GetPosition ();
        xx = pCoordinate->GetX ();
        yy = pCoordinate->GetY ();
        pCoordinate->Release ();
        pCoordinate = pGridTick->GetDirectionVector ();
        uu = pCoordinate->GetX ();
        vv = pCoordinate->GetY ();
        pCoordinate->Release ();
        pGridTick->Release ();
    }
    pGridTicks->Release ();

    pMgrs->Release ();

    pGridSpecification->Release ();
#endif 
