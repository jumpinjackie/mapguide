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

static wchar_t moduleName [] = L"GctTest1.cpp";

static MgDictionaryTestList elpTestTable [] =
{
    {    L"APL4-5",    L"_APL4-5",NULL},       // Try to keep set to the first one
    {    L"CLRK66",    L"_CLRK66",NULL},
    {  L"CLRK-IGN",  L"_CLRK-IGN",NULL},
    {     L"INTNL",     L"_INTNL",NULL},
    {    L"CLRK80",    L"_CLRK80",NULL},
    {     L"WGS84",     L"_WGS84",NULL},
    {   L"GRS1980",   L"_GRS1980",NULL},
    {    L"Xian80",    L"_Xian80",NULL},       // Try to keep set to the last one
    {          L"",           L"",NULL   }
};

INT32 GctTest1 (MgGctTestParameters& cmdLineParms)
{
    bool ok;

    INT32 index;
    INT32 nTblCount;
    INT32 errorCount (0);
    INT32 originalSize;
    INT32 finalSize;

    wostream& rptStrm (std::wcerr);

    Ptr<MgCoordinateSystemCatalog> ptrCatalog;
    Ptr<MgCoordinateSystemEllipsoidDictionary> ptrDictionary;
    Ptr<MgCoordinateSystemEllipsoid> ptrEllipsoid;
    Ptr<MgCoordinateSystemEllipsoid> ptrEllipsoidClone;
    Ptr<MgCoordinateSystemEnum> ptrEllipsoidEnum;
    Ptr<MgStringCollection> ptrEllipsoidNames;

    MgDictionaryTestList* ptrTable;

    MgGeometryFactory mgFactory;
    MgCoordinateSystemFactory csFactory;

    if (!cmdLineParms.Silent ())
    {
        rptStrm << L"[1] Ellipsoid dictionary test "
                << std::endl;
    }

    // The API throws execptions for all errors.  We need to catch them all
    // and return a "failed" status.  In this test, we are not testing
    // behavior in the erroneous state; that's done elsewhere.
    Ptr<MgException> mgException;
    STRING mgStackParams;
    try
    {

        // Count the entries in the table.
        nTblCount = 0;
        for (ptrTable = elpTestTable;ptrTable->name [0] != '\0';ptrTable += 1)
        {
            nTblCount += 1;
        }

        // Get a catalog object.
        ptrCatalog = csFactory.GetCatalog ();

        // Get An Ellipsoid Dictionary
        ptrDictionary = ptrCatalog->GetEllipsoidDictionary ();

        // Get the original size; i.e. size before we muchk around.
        originalSize = ptrDictionary->GetSize ();

        // We loop through our test list and extract each named ellipsoid
        // entry in the list.  In preparation for the following tests, we
        // reset the protection flag and use (i.e. test!) the Modify
        // function to reset the protection in the dictionary.
        for (index = 0;index < nTblCount;index += 1)
        {
            ptrTable = &elpTestTable [index];

            // Get a copy of the definition of this ellipsoid, saving the
            // definition for later use in the test table.
            ptrEllipsoid = ptrDictionary->GetEllipsoid (ptrTable->name);
            ptrTable->pointer = SAFE_ADDREF (ptrEllipsoid.p);

            // Reset the protection flag.  We take care not to modify the
            // original copy which we will use to verify the correctness
            // of dictioanry functions.
            ptrEllipsoidClone = ptrEllipsoid->CreateClone ();
            ptrEllipsoidClone->SetProtectMode (false);
            ptrEllipsoidClone->SetElCode (ptrTable->cloneName);
            ptrDictionary->Add (ptrEllipsoidClone.p);
        }

        // Verify that the cloned new additions are "the same as" the
        // definitions we started with.
        for (index = 0;index < nTblCount;index += 1)
        {
            ptrTable = &elpTestTable [index];
            ptrEllipsoid = ptrDictionary->GetEllipsoid (ptrTable->cloneName);
            ok = ptrEllipsoid->IsSameAs (ptrTable->pointer);
            if (!ok)
            {
                errorCount += 1;
                if (!cmdLineParms.Silent ())
                {
                    rptStrm << L"\tEllipsoid dictionary error detected regarding "
                            << ptrTable->cloneName
                            << L" ["
                            << moduleName
                            << L":"
                            << __LINE__
                            << L"]"
                            << std::endl;
                }
            }
        }

        // Now we loop and remove the cloned entries we added above,
        // but in reverse order, just for kicks.
        for (index = nTblCount - 1;index >= 0;index -= 1)
        {
            ptrTable = &elpTestTable [index];
            ptrDictionary->Remove (ptrTable->cloneName);
        }

        // Use 'Has' function to verify that all the originals are
        // still there and ar the same as they were when we started.
        for (index = 0;index < nTblCount;index += 1)
        {
            ptrTable = &elpTestTable [index];
            ok = ptrDictionary->Has (ptrTable->name);
            if (ok)
            {
                ptrEllipsoid = ptrDictionary->GetEllipsoid (ptrTable->name);
                ok = ptrEllipsoid->IsSameAs (ptrTable->pointer);
            }
            if (!ok)
            {
                errorCount += 1;
                if (!cmdLineParms.Silent ())
                {
                    rptStrm << L"\tEllipsoid dictionary error detected regarding "
                            << ptrTable->name
                            << L" ["
                            << moduleName
                            << L":"
                            << __LINE__
                            << L"]"
                            << std::endl;
                }
            }
        }

        // OK, time to delete the ellipsoid definitions which are stored
        // in our test table.  Memory leaks is a major part of the test.
        for (index = 0;index < nTblCount;index += 1)
        {
            ptrTable = &elpTestTable [index];
            (ptrTable->pointer)->Release ();
        }

        // Use 'Has' function to verify that all the Clones are
        // no longer there.
        for (index = 0;index < nTblCount;index += 1)
        {
            ptrTable = &elpTestTable [index];
            ok = ptrDictionary->Has (ptrTable->cloneName);
            if (ok)
            {
                errorCount += 1;
                if (!cmdLineParms.Silent ())
                {
                    rptStrm << L"\tEllipsoid dictionary error detected regarding "
                            << ptrTable->name
                            << L" ["
                            << moduleName
                            << L":"
                            << __LINE__
                            << L"]"
                            << std::endl;
                }
            }
        }

        // Verify that the size is exactly what we started with.
        finalSize = ptrDictionary->GetSize ();
        if (finalSize != originalSize)
        {
                errorCount += 1;
                if (!cmdLineParms.Silent ())
                {
                    rptStrm << L"\tEllipsoid dictionary size error detected"
                            << L" ["
                            << moduleName
                            << L":"
                            << __LINE__
                            << L"]"
                            << std::endl;
                }
        }

        // Finally, we generate an enum and use the results get each
        // definition, individually, from the dictionary and do some
        // checking on what we get back.
        ptrEllipsoidEnum = ptrDictionary->GetEnum ();
        ptrEllipsoidNames = ptrEllipsoidEnum->NextName (finalSize);
        if (ptrEllipsoidNames->GetCount () != finalSize)
        {
            errorCount += 1;
            if (!cmdLineParms.Silent ())
            {
                rptStrm << L"\tEllipsoid dictionary size error detected"
                        << L" ["
                        << moduleName
                        << L":"
                        << __LINE__
                        << L"]"
                        << std::endl;
            }
        }

        for (index = 0;index < finalSize;index += 1)
        {
            double dTmp;
            double eRad, pRad, eSq, flat;

            STRING elpCodeName = ptrEllipsoidNames->GetItem (index);
            ptrEllipsoid = ptrDictionary->GetEllipsoid (elpCodeName);
            eRad = ptrEllipsoid->GetEquatorialRadius ();
            pRad = ptrEllipsoid->GetPolarRadius ();
            dTmp = ptrEllipsoid->GetEccentricity ();
            eSq = dTmp * dTmp;
            flat = ptrEllipsoid->GetFlatteningRatio ();
            dTmp = (eRad - pRad) / eRad;
            if (fabs (dTmp - flat) > 1.0E-07)
            {
                errorCount += 1;
                if (!cmdLineParms.Silent ())
                {
                    rptStrm << L"\tEllipsoid dictionary data maintenance error detected"
                            << L" ["
                            << moduleName
                            << L":"
                            << __LINE__
                            << L"]"
                            << std::endl;
                }
            }
            dTmp = ((eRad * eRad) - (pRad * pRad)) / (eRad * eRad);
            if (fabs (dTmp - eSq) > 1.0E-09)
            {
                errorCount += 1;
                if (!cmdLineParms.Silent ())
                {
                    rptStrm << L"\tEllipsoid dictionary data maintenance error detected"
                            << L" ["
                            << moduleName
                            << L":"
                            << __LINE__
                            << L"]"
                            << std::endl;
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
    return errorCount;
}
