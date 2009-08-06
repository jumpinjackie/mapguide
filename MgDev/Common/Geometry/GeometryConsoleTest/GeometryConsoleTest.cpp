// GeometryConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma warning (disable: 4275)
#include <iostream>     //for std::wcout, std::wcerr
#include <tchar.h>      //for _wcsicmp
#include <fstream>      //for ifstream

#include "GeometryCommon.h"

int _tmain(int argc, _TCHAR* argv[])
{
    INT32 index1;
    INT32 index2;
    INT32 gridLineCount;
    INT32 lineSegmentCount;
    
    double xx;
    double yy;

    MgCoordinateSystemFactory factory;

    MgCoordinate* pCoordinate;
    MgCoordinateIterator* coordItr;
    MgLineString* pLineString;
    MgCoordinateSystemGridLine* pGridLine;
    MgCoordinateSystemGridBase* pGenericGrid;
    MgCoordinateSystemGridBoundary* pGridBoundary;
    MgCoordinateSystemGridLineCollection* pGridLines;
    MgCoordinateSystemGridSpecification* pGridSpecification;

    MgCoordinate* southWestXY = new MgCoordinateXY (400000.0,3900000.0);
    MgCoordinate* northEastXY = new MgCoordinateXY (600000.0,4100000.0);

    pGridSpecification = factory.GridSpecification ();
    pGridSpecification->SetGridBase (0.0);
    pGridSpecification->SetGridIncrement (1.0);
    pGridSpecification->SetTickSubdivisions  (8,8);
    pGridSpecification->SetUnits (MgCoordinateSystemUnitCode::Degree,MgCoordinateSystemUnitType::Angular);
    pGridSpecification->SetCurvePrecision (1.0);

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
    pGenericGrid->Release ();
    pGridBoundary->Release ();
    pGridSpecification->Release ();

    return 0;
}
