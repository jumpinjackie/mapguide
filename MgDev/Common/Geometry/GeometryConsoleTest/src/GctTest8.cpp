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

#include "GeometryCommon.h"
#include "GeometryConsoleTest.h"

// The following array is the same as used in the CS-MAP Console Test.  We find
// it useful to compare performance between the API and the underlying CS-MAP
// library, thus we know how much overhead the API adds to the conversion
// process.

#define gtc8_TEST_COORD_COUNT 10
static double gtc8Testcoords [gtc8_TEST_COORD_COUNT][3] =
{
	{ 456661.0, 4316548.0, 0.0 },
	{ 462938.0, 4314975.0, 0.0 },
	{ 512384.0, 4419456.0, 0.0 },
	{ 417845.0, 4391564.0, 0.0 },
	{ 489263.0, 4346712.0, 0.0 },
	{ 612374.0, 4394561.0, 0.0 },
	{ 472384.0, 4314904.0, 0.0 },
	{ 498374.0, 4334978.0, 0.0 },
	{ 452894.0, 4401245.0, 0.0 },
	{ 413649.0, 4294561.0, 0.0 }
};

static wchar_t moduleName [] = L"GctTest8.cpp";

INT32 GctTest8 (MgGctTestParameters& cmdLineParms)
{
    clock_t startTime;
    clock_t endTime;

    INT32 rate;
    INT32 errorCount (0);
    INT32 conversionCount (300000);
    INT32 conversionCounter;

    wostream& rptStrm (std::wcerr);

    double coordX, coordY;
    double elapsed;

    MgCoordinateSystemFactory csFactory;
    Ptr<MgCoordinateSystem> crsUtm13;       // NAD27 UTM Zone 13 North, Meters
    Ptr<MgCoordinateSystem> crsCo83C;       // NAD83 Colorado State Planes, Central, Meters
    Ptr<MgCoordinateSystemTransform> crsTransform;

    conversionCount = conversionCount * cmdLineParms.GetDuration () / 100;

    if (!cmdLineParms.Silent ())
    {
        rptStrm << L"[8] Performance test based on "
                << conversionCount
                << L" non-trivial conversions."
                << std::endl;
    
    }

    Ptr<MgException> mgException;
    STRING mgStackParams;
    try
    {
        crsUtm13 = csFactory.CreateFromCode (L"UTM27-13");
        crsCo83C = csFactory.CreateFromCode (L"CO83-C");
        crsTransform = csFactory.GetTransform (crsUtm13,crsCo83C);

        conversionCounter = 0;
        startTime = clock ();
        while (conversionCounter < conversionCount)
        {
            for (int ii = 0;ii < gtc8_TEST_COORD_COUNT;ii++)
            {
                coordX = gtc8Testcoords [ii][0];
                coordY = gtc8Testcoords [ii][1];
                crsTransform->Transform (&coordX,&coordY);
                conversionCounter += 1;
            }
        }
        endTime = clock ();

        elapsed = (double)(endTime - startTime) / (double)CLOCKS_PER_SEC;
        if (elapsed > 0.00001)
        {
            rate = (conversionCounter > 0) ? (INT32)((double)conversionCounter / elapsed) : 0;
        }
        else
        {
            rate = 0;
        }
        if (!cmdLineParms.Silent ())
        {
            rptStrm << L"\tConverted "
                    << conversionCounter
                    << L" coordinates from UTM27-13 to CO83-C in "
                    << elapsed 
                    << L" seconds."
                    << std::endl;
            rptStrm << L"\tEffective conversion rate = "
                    << rate
                    << L" conversions per second."
                    << std::endl;
        }
        errorCount = 0;
    }
    catch (MgException* e)
    {
        mgException = e;
        errorCount = -1;
        rptStrm << L"\tUnexpected MapGuide exception encountered in "
                << moduleName
                << L"."
                << std::endl;
        rptStrm << L"\tMESSAGE: "
                << e->GetExceptionMessage ()
                << std::endl;
        rptStrm << L"\tSTACK TRACE: "
                << e->GetStackTrace ()
                << std::endl;
    }
    catch (...)
    {
        errorCount = -2;
    }
    return errorCount;
}


