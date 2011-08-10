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

static wchar_t moduleName [] = L"GctTest2.cpp";

static MgDictionaryTestList dtmTestTable [] =
{
	{   L"ZANDERIJ",  L"_ZANDERIJ",  NULL},	/* last one */
	{      L"NAD27",  L"_NAD27",     NULL},
	{      L"NAD83",  L"_NAD83",     NULL},
	{   L"WGS72-BW",  L"_WGS72-BW",  NULL},
	{     L"PSAD56",  L"_PSAD56",    NULL},
	{      L"OLDHI",  L"_OLDHI",     NULL},
	{      L"WGS72",  L"_WGS72",     NULL},
	{   L"TIMBALAI",  L"_TIMBALAI",  NULL},
	{    L"ADINDAN",  L"_ADINDAN",   NULL},	/* first one */
    {          L"",   L"",           NULL   }
};

INT32 GctTest2 (MgGctTestParameters& cmdLineParms)
{
    bool ok;

    INT32 index;
    INT32 nTblCount;
    INT32 errorCount (0);
    INT32 originalSize;
    INT32 finalSize;

    wostream& rptStrm (std::wcerr);

    Ptr<MgCoordinateSystemCatalog> ptrCatalog;
    Ptr<MgCoordinateSystemDatumDictionary> ptrDictionary;
    Ptr<MgCoordinateSystemDatum> ptrDatum;
    Ptr<MgCoordinateSystemDatum> ptrDatumClone;
    Ptr<MgCoordinateSystemEnum> ptrDatumEnum;
    Ptr<MgStringCollection> ptrDatumNames;

    MgDictionaryTestList* ptrTable;

    MgGeometryFactory mgFactory;
    MgCoordinateSystemFactory csFactory;

    if (!cmdLineParms.Silent ())
    {
        rptStrm << L"[2] Datum dictionary test "
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
        for (ptrTable = dtmTestTable;ptrTable->name [0] != '\0';ptrTable += 1)
        {
            nTblCount += 1;
        }

        // Get a catalog object.
        ptrCatalog = csFactory.GetCatalog ();

        // Get a Datum Dictionary
        ptrDictionary = ptrCatalog->GetDatumDictionary ();

        // Get the original size; i.e. size before we muchk around.
        originalSize = ptrDictionary->GetSize ();

        // We loop through our test list and extract each named datum
        // entry in the list.  In preparation for the following tests, we
        // reset the protection flag and use (i.e. test!) the Modify
        // function to reset the protection in the dictionary.
        for (index = 0;index < nTblCount;index += 1)
        {
            ptrTable = &dtmTestTable [index];

            // Get a copy of the definition of this datum, saving the
            // definition for later use in the test table.
            ptrDatum = ptrDictionary->GetDatum (ptrTable->name);
            ptrTable->pointer = SAFE_ADDREF (ptrDatum.p);

            // Reset the protection flag.  We take care not to modify the
            // original copy which we will use to verify the correctness
            // of dictioanry functions.
            ptrDatumClone = ptrDatum->CreateClone ();
            ptrDatumClone->SetProtectMode (false);
            ptrDatumClone->SetDtCode (ptrTable->cloneName);
            ptrDictionary->Add (ptrDatumClone.p);
        }

        // Verify that the cloned new additions are "the same as" the
        // definitions we started with.
        for (index = 0;index < nTblCount;index += 1)
        {
            ptrTable = &dtmTestTable [index];
            ptrDatum = ptrDictionary->GetDatum (ptrTable->cloneName);
            ok = ptrDatum->IsSameAs (ptrTable->pointer);
            if (!ok)
            {
                errorCount += 1;
                if (!cmdLineParms.Silent ())
                {
                    rptStrm << L"\tDatum dictionary error detected regarding "
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
            ptrTable = &dtmTestTable [index];
            ptrDictionary->Remove (ptrTable->cloneName);
        }

        // Use 'Has' function to verify that all the originals are
        // still there and ar the same as they were when we started.
        for (index = 0;index < nTblCount;index += 1)
        {
            ptrTable = &dtmTestTable [index];
            ok = ptrDictionary->Has (ptrTable->name);
            if (ok)
            {
                ptrDatum = ptrDictionary->GetDatum (ptrTable->name);
                ok = ptrDatum->IsSameAs (ptrTable->pointer);
            }
            if (!ok)
            {
                errorCount += 1;
                if (!cmdLineParms.Silent ())
                {
                    rptStrm << L"\tDatum dictionary error detected regarding "
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

        // OK, time to delete the datum definitions which are stored
        // in our test table.  Memory leaks is a major part of the test.
        for (index = 0;index < nTblCount;index += 1)
        {
            ptrTable = &dtmTestTable [index];
            (ptrTable->pointer)->Release ();
        }

        // Use 'Has' function to verify that all the Clones are
        // no longer there.
        for (index = 0;index < nTblCount;index += 1)
        {
            ptrTable = &dtmTestTable [index];
            ok = ptrDictionary->Has (ptrTable->cloneName);
            if (ok)
            {
                errorCount += 1;
                if (!cmdLineParms.Silent ())
                {
                    rptStrm << L"\tDatum dictionary error detected regarding "
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
                    rptStrm << L"\tDatum dictionary size error detected"
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
        ptrDatumEnum = ptrDictionary->GetEnum ();
        ptrDatumNames = ptrDatumEnum->NextName (finalSize);
        if (ptrDatumNames->GetCount () != finalSize)
        {
            errorCount += 1;
            if (!cmdLineParms.Silent ())
            {
                rptStrm << L"\tDatum dictionary size error detected"
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
            STRING elpCodeName = ptrDatumNames->GetItem (index);
            ptrDatum = ptrDictionary->GetDatum (elpCodeName);

            // Do some checks to verify that the datum dictionary did not get
            // hosed by the above manipulations.
            ok = ptrDatum->IsUsable (ptrCatalog);
            if (ok)
            {
                // There's a bust in Reeses. There is no GeodeticTransformation
                // to convert 'CH1903Plus_2' (EPSG::6150) to WGS84.
                INT16 epsgCode = ptrDatum->GetEpsgCode ();
                if (epsgCode != 6150)
                {
                    Ptr<MgDisposableCollection> ptrTransforms = ptrDatum->GetGeodeticTransformations (NULL);
                    ok = (ptrTransforms->GetCount () == 1);
                }
            }
            if (!ok)
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
