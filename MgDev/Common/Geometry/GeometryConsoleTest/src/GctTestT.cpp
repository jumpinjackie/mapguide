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

#include "CoordSysGeodeticTransformation.h"
#include "CoordSysGeodeticTransformDefParams.h"
#include "CoordSysGeodeticStandaloneTransformDefParams.h"
#include "CoordSysGeodeticAnalyticalTransformDefParams.h"
#include "CoordSysGeodeticInterpolationTransformDefParams.h"
#include "CoordSysGeodeticMultipleRegressionTransformDefParams.h"
#include "CoordSysGeodeticTransformDef.h"
#include "CoordSysGeodeticPath.h"

static wchar_t moduleName [] = L"GctTestT.cpp";

INT32 GctTestT (MgGctTestParameters& cmdLineParms)
{
    bool ok;
    INT32 index;
    INT32 errorCount (0);
    INT32 xfrmCount;
    wostream& rptStrm (std::wcerr);

    Ptr<MgCoordinateSystemCatalog> ptrCatalog;
    Ptr<MgCoordinateSystemDictionary> ptrDictionary;
    Ptr<MgCoordinateSystem> ptrSourceCRS;
    Ptr<MgCoordinateSystem> ptrTargetCRS;
    Ptr<MgCoordinateSystemTransform> crsTransform;
    Ptr<MgCoordinateSystemGeodeticTransformDefDictionary> ptrTransformDictionary;
    Ptr<MgCoordinateSystemGeodeticPathDictionary> ptrPathDictionary;
    Ptr<MgCoordinateSystemGeodeticTransformDef> ptrTransformDef;
    Ptr<MgCoordinateSystemGeodeticPath> ptrPath;

    MgGeometryFactory mgFactory;
    MgCoordinateSystemFactory csFactory;

    if (!cmdLineParms.Silent ())
    {
        rptStrm << L"[T] Temporary test module "
                << std::endl;
    }

    // The API throws execptions for all errors.  We need to catch them all
    // and return a "failed" status.  In this test, we are not testing
    // behavior in the erroneous state; that's done elsewhere.
    Ptr<MgException> mgException;
    STRING mgStackParams;
    try
    {
        // Get some basic pointers.
        ptrCatalog = csFactory.GetCatalog ();
        ptrDictionary = ptrCatalog->GetCoordinateSystemDictionary ();
        ptrTransformDictionary = ptrCatalog->GetGeodeticTransformDefDictionary ();
        ptrPathDictionary = ptrCatalog->GetGeodeticPathDictionary ();
 
 #ifdef __SKIP__
        ptrSourceCRS = ptrDictionary->GetCoordinateSystem (L"LL-ATS77");
        ptrTargetCRS = ptrDictionary->GetCoordinateSystem (L"LL84");
        crsTransform = csFactory.GetTransform (ptrSourceCRS.p,ptrTargetCRS.p);
        xfrmCount = crsTransform->NumberOfGeodeticTransformations ();
        for (index = 0;index < xfrmCount;index += 1)
        {
            INT32 direction;
            Ptr<MgCoordinateSystemGeodeticTransformDef> xfrmDefPtr;
            xfrmDefPtr = crsTransform->GetGeodeticTransformation (index);
            direction = crsTransform->GetGeodeticTransformationDirection (index);
            Ptr<MgCoordinateSystemGeodeticPath> pathDefPtr;
            pathDefPtr = crsTransform->GetExplicitGeodeticPath ();
        }
#endif
#ifdef __SKIP__

        ptrTransformDef.p = ptrTransformDictionary->GetGeodeticTransformationDef (L"ABIDJAN-87_to_WGS84");
        CSLibrary::CCoordinateSystemGeodeticTransformDef* xfrmDefOld = dynamic_cast<CSLibrary::CCoordinateSystemGeodeticTransformDef*>(ptrTransformDef.p);
        INT32 transformSize = xfrmDefOld->GetSizeSerialized();
        UINT8* bufr = new UINT8 [transformSize];
        xfrmDefOld->SerializeTo (bufr);

        MgCoordinateSystemGeodeticTransformDef* ptrXfrmDefNewMg = ptrTransformDictionary->NewGeodeticTransformationDef(MgCoordinateSystemGeodeticTransformDefType::Standalone);
        CSLibrary::CCoordinateSystemGeodeticTransformDef* xfrmDefNew = dynamic_cast<CSLibrary::CCoordinateSystemGeodeticTransformDef*>(ptrXfrmDefNewMg);
        xfrmDefNew->SerializeFrom (bufr);
#endif

        ptrPath.p = ptrPathDictionary->GetGeodeticPath (L"ASTRLA66-Grid_to_WGS84");
        CSLibrary::CCoordinateSystemGeodeticPath* pathDefOld = dynamic_cast<CSLibrary::CCoordinateSystemGeodeticPath*>(ptrPath.p);
        INT32 pathSize = pathDefOld->GetSizeSerialized();
        UINT8* bufr = new UINT8 [pathSize];
        pathDefOld->SerializeTo (bufr);

        MgCoordinateSystemGeodeticPath* ptrPathDefNewMg = ptrPathDictionary->NewGeodeticPath();
        CSLibrary::CCoordinateSystemGeodeticPath* pathDefNew = dynamic_cast<CSLibrary::CCoordinateSystemGeodeticPath*>(ptrPathDefNewMg);
        pathDefNew->SerializeFrom (bufr);
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
