// GeometryConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma warning (disable: 4275)
#include <iostream>     //for std::wcout, std::wcerr
#include <fstream>      //for ifstream

#include "GeometryCommon.h"

int _tmain(int argc, _TCHAR* argv[])
{
    INT32 index1;
    INT32 index2;
    INT32 index3;
    INT32 index4;
    INT32 tickCount;
    INT32 regionCount;
    INT32 gridLineCount;
    INT32 lineSegmentCount;
    
    double xx;
    double yy;
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

    MgCoordinate* southWestXY = new MgCoordinateXY (600000.0,3900000.0);
    MgCoordinate* northEastXY = new MgCoordinateXY (900000.0,4100000.0);

    pGridSpecification = factory.GridSpecification ();
    pGridSpecification->SetGridBase (0.0,0.0);
    pGridSpecification->SetGridIncrement (1.0,1.0);
    pGridSpecification->SetTickIncrements  (0.25,0.25);
    pGridSpecification->SetUnits (MgCoordinateSystemUnitCode::Degree,MgCoordinateSystemUnitType::Angular);
    pGridSpecification->SetCurvePrecision (1.0E-05);

    pGenericGrid = factory.GenericGrid (L"LL84",L"UTM83-13",true);
    pGridBoundary = factory.GridBoundary (southWestXY,northEastXY);
    southWestXY->Release ();
    northEastXY->Release ();

    pGenericGrid->SetBoundary (pGridBoundary);
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
        pGridTick->Release ();
    }
    pGridTicks->Release ();    

    pGenericGrid->Release ();
    pGridSpecification->Release ();

    pGridSpecification = factory.GridSpecification (MgCoordinateSystemGridSpecializationType::MGRS,
                                                    MgCoordinateSystemMgrsGridLevel::Mgrs1Km);
    pMgrs = factory.MgrsGrid (L"UTM84-13N",false,MgCoordinateSystemMgrsLetteringScheme::Normal,true);
    pMgrs->SetBoundary (pGridBoundary);
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
        pGridTick->Release ();
    }
    pGridTicks->Release ();

    pMgrs->Release ();

    pGridBoundary->Release ();
    pGridSpecification->Release ();

    return 0;
}
