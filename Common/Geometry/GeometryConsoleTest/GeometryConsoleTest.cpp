// GeometryConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma warning (disable: 4275)
#include <iostream>     //for std::wcout, std::wcerr
#include <fstream>      //for ifstream

#include "GeometryCommon.h"

int _tmain(int argc, _TCHAR* argv[])
{
#ifndef __SKIP__
    // Some Catalog/Dictionary testing and timing.
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
    // testing a special WKT string for proer operation.
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

    return 0;
}
