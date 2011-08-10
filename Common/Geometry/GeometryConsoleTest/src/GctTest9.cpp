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

wchar_t moduleName [128] = L"GctTest9.cpp";

INT32 GctTest9 (MgGctTestParameters& cmdLineParms)
{
    bool ok (false);

    int errCount (0);

    unsigned idx;
    unsigned testCount;

    INT32 errorCount (0);

    EcsTestMethod testMethod;
    EcsCrsAuthority srcKeyAuthority;
    EcsCrsAuthority trgKeyAuthority;

    double delta;

    double sourceCoords [3];
    double resultCoords [3];

    wchar_t srcKeyNm [128], lastSrcKeyNm [128];
    wchar_t trgKeyNm [128], lastTrgKeyNm [128];

    wchar_t testDataFilePath [1024];

    Ptr<MgCoordinateSystemTransform> transformPtr;
    std::wstring fieldData;

    wostream& rptStrm (std::wcerr);

    Ptr<MgCoordinateSystemCatalog> ptrCatalog;

    MgGeometryFactory mgFactory;
    MgCoordinateSystemFactory csFactory;

    if (!cmdLineParms.Silent ())
    {
        rptStrm << L"[9] Coordinate conversion regression test "
                << std::endl;
    }

    // The API throws execptions for all errors.  We need to catch them all
    // and return a "failed" status.  In this test, we are not testing
    // behavior in the erroneous state; that's done elsewhere.
    Ptr<MgException> mgException;
    STRING mgStackParams;
    try
    {
        errorCount = 0;

        // Load the test data into memory.  No real need to have it memory,
        // but code to do that already exists; so we use it.
        wcsncpy (testDataFilePath,cmdLineParms.GetTestDataFolder (),wcCount (testDataFilePath));
        testDataFilePath [wcCount (testDataFilePath) - 1] = L'\0';
        wcscat (testDataFilePath,L"/");
        wcscat (testDataFilePath,cmdLineParms.GetRegressionTestDataFile ());
        MgOsGeoTestFile osGeoTestFile (testDataFilePath);
        ok = osGeoTestFile.IsOk ();
        if (!ok)
        {
            if (!cmdLineParms.Silent ())
            {
                rptStrm << L"\tRegression test loading failed."
                        << std::endl;
            }
            return -1;
        }

        // OK, we loop once for each test defined in the test file.
        testCount = osGeoTestFile.RecordCount ();
        ok = (testCount > 0U);
        srcKeyNm [0] = trgKeyNm [0] = L'\0';
        lastSrcKeyNm [0] = lastTrgKeyNm [0] = L'\0';
        for (idx = 0;ok && idx < testCount;idx += 1)
        {
            if (osGeoTestFile.IsComment (idx))
            {
                continue;
            }
            testMethod = osGeoTestFile.GetTestMethod (idx);
            if (testMethod != testMthCrs2D && testMethod != testMthCrs3D)
            {
                continue;
            }
            srcKeyAuthority = osGeoTestFile.GetSrcCrsKey (fieldData,idx);
            wcsncpy (srcKeyNm,fieldData.c_str (),wcCount (srcKeyNm));
            srcKeyNm [wcCount (srcKeyNm) - 1] = L'\0';
            trgKeyAuthority = osGeoTestFile.GetTrgCrsKey (fieldData,idx);
            wcsncpy (trgKeyNm,fieldData.c_str (),wcCount (trgKeyNm));
            trgKeyNm [wcCount (trgKeyNm) - 1] = L'\0';
            if (srcKeyAuthority != crsAuthCsMap || trgKeyAuthority != crsAuthCsMap)
            {
                continue;
            }

            // Set up a transform object.
            if (transformPtr)
            {
                // If we have a transform object, see if it will do what we
                // need for this test.
                if (CS_wcsicmp (srcKeyNm,lastSrcKeyNm) ||
                    CS_wcsicmp (trgKeyNm,lastTrgKeyNm))
                {
                    // Nope, release it.
                    SAFE_RELEASE (transformPtr.p);
                }
            }
            if (!transformPtr)
            {
                // Here when we need to create a new transform object.
                Ptr<MgCoordinateSystem> srcCrsPtr = csFactory.CreateFromCode (srcKeyNm);
                Ptr<MgCoordinateSystem> trgCrsPtr = csFactory.CreateFromCode (trgKeyNm);
                transformPtr = csFactory.GetTransform (srcCrsPtr,trgCrsPtr);
                transformPtr->IgnoreDatumShiftWarning (true);
                transformPtr->IgnoreOutsideDomainWarning (true);
                wcsncpy (lastSrcKeyNm,srcKeyNm,wcCount (lastSrcKeyNm));
                lastSrcKeyNm [wcCount (lastSrcKeyNm) - 1] = L'\0';
                wcsncpy (lastTrgKeyNm,trgKeyNm,wcCount (lastTrgKeyNm));
                lastTrgKeyNm [wcCount (lastTrgKeyNm) - 1] = L'\0';
            }

            ok = osGeoTestFile.GetSourceCoordinates (sourceCoords,idx);
            if (testMethod == testMthCrs2D)
            {
                Ptr<MgCoordinate> srcPntPtr = mgFactory.CreateCoordinateXY (sourceCoords [0],sourceCoords [1]);
                Ptr<MgCoordinate> trgPntPtr = transformPtr->Transform (srcPntPtr);
                resultCoords [0] = trgPntPtr->GetX ();
                resultCoords [1] = trgPntPtr->GetY ();
            }
            else if (testMethod == testMthCrs3D)
            {
                Ptr<MgCoordinate> srcPntPtr = mgFactory.CreateCoordinateXYZ (sourceCoords [0],sourceCoords [1],sourceCoords [2]);
                Ptr<MgCoordinate> trgPntPtr = transformPtr->Transform (srcPntPtr);
                resultCoords [0] = trgPntPtr->GetX ();
                resultCoords [1] = trgPntPtr->GetY ();
                resultCoords [2] = trgPntPtr->GetZ ();
            }
            bool testOk = osGeoTestFile.CompareResults (resultCoords,idx,delta);
            if (!testOk)
            {
                osGeoTestFile.GetTestName (fieldData,idx);
                if (cmdLineParms.Silent ())
                {
                    rptStrm << fieldData
                            << L": Calculation failed.  ["
                            << srcKeyNm
                            << L" -> "
                            << trgKeyNm
                            << L"; "
                            << delta
                            << L"]"
                            << std::endl;
                    errCount += 1;
                }
            }
        }
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
        errorCount = -1;
        rptStrm << L"\tUnexpected exception of unknown type encountered in "
                << moduleName
                << L"."
                << std::endl;
    }

    // All done.
    return (ok) ? errCount : -1;
}
