//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "CoordSysCommon.h"

#include "CoordSysMathComparator.h"     //for CCoordinateSystemMathComparator
#include "CoordSysUtil.h"               //for FloatEqual
#include "MentorUtil.h"                 //for IsLegalMentorName

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemMathComparator::CCoordinateSystemMathComparator()
{
}

CCoordinateSystemMathComparator::~CCoordinateSystemMathComparator()
{
}

//MgDisposable
void CCoordinateSystemMathComparator::Dispose()
{
    delete this;
}

// Returns whether the two defs are the same, mathematically.
// Non-mathematical attributes such as description are ignored
// for comparison purposes.
//
bool CCoordinateSystemMathComparator::Same(MgGuardDisposable *pDefinition1, MgGuardDisposable *pDefinition2)
{
    MG_TRY()
    assert(pDefinition1);
    assert(pDefinition2);
    if (!pDefinition1 || !pDefinition2) return false;

    // Is the first def an ellipsoid?
    MgCoordinateSystemEllipsoid* pElDef1=dynamic_cast<MgCoordinateSystemEllipsoid*>(pDefinition1);
    if (pElDef1)
    {
        // the first def is an ellipsoid...
        MgCoordinateSystemEllipsoid* pElDef2=dynamic_cast<MgCoordinateSystemEllipsoid*>(pDefinition2);
        if (!pElDef2)
        {
            // ...but the second one isn't, so they're not the same
            return false;
        }
        return SameEllipsoid(pElDef1, pElDef2);
    }

    // Is the first def a datum?
    MgCoordinateSystemDatum* pDtDef1=dynamic_cast<MgCoordinateSystemDatum*>(pDefinition1);
    if (pDtDef1)
    {
        // the first def is a datum...
        MgCoordinateSystemDatum* pDtDef2=dynamic_cast<MgCoordinateSystemDatum*>(pDefinition2);
        if (!pDtDef2)
        {
            // ...but the second one isn't, so they're not the same
            return false;
        }
        return SameDatum(pDtDef1, pDtDef2);
    }

    // Is the first def a coordsys?
    MgCoordinateSystem* pCsDef1=dynamic_cast<MgCoordinateSystem*>(pDefinition1);
    if (pCsDef1)
    {
        // the first def is a coordsys...
        MgCoordinateSystem* pCsDef2=dynamic_cast<MgCoordinateSystem*>(pDefinition2);
        if (!pCsDef2)
        {
            // ...but the second one isn't, so they're not the same
            return false;
        }
        return SameCoordinateSystem(pCsDef1, pCsDef2);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMathComparator.Same")

    // It's not any def type we care about.
    return false;
}

// Returns whether the two defs are the same, mathematically.
// Non-mathematical attributes such as description are ignored
// for comparison purposes.
//
bool CCoordinateSystemMathComparator::SameEllipsoid(MgCoordinateSystemEllipsoid *pDefinition1, MgCoordinateSystemEllipsoid *pDefinition2)
{
    MG_TRY()
    assert(pDefinition1);
    assert(pDefinition2);
    if (!pDefinition1 || !pDefinition2) return false;

    // tolerance for floating-point comparisons
    const double kdEpsilon = 1.0E-13;

    bool bResult;
    cs_Eldef_ def1, def2;
    bResult = BuildElDefFromInterface(pDefinition1, def1);
    if (!bResult) return bResult;
    bResult = BuildElDefFromInterface(pDefinition2, def2);
    if (!bResult) return bResult;

    if (!FloatEqual(def1.e_rad, def2.e_rad, kdEpsilon)) return false;
    if (!FloatEqual(def1.p_rad, def2.p_rad, kdEpsilon)) return false;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMathComparator.SameEllipsoid")

    return true;
}

// Returns whether the two defs are the same, mathematically.
// Non-mathematical attributes such as description are ignored
// for comparison purposes.
//
bool CCoordinateSystemMathComparator::SameDatum(MgCoordinateSystemDatum *pDefinition1, MgCoordinateSystemDatum *pDefinition2)
{
    MG_TRY()
    assert(pDefinition1);
    assert(pDefinition2);
    if (!pDefinition1 || !pDefinition2) return false;

    // tolerance for floating-point comparisons
    const double kdEpsilon = 1.0E-13;

    bool bResult;
    cs_Dtdef_ def1, def2;
    bResult = BuildDtDefFromInterface(pDefinition1, def1);
    if (!bResult) return bResult;
    bResult = BuildDtDefFromInterface(pDefinition2, def2);
    if (!bResult) return bResult;

    if (0 != CS_stricmp(def1.ell_knm, def2.ell_knm)) return false;
    if (!FloatEqual(def1.delta_X, def2.delta_X, kdEpsilon)) return false;
    if (!FloatEqual(def1.delta_Y, def2.delta_Y, kdEpsilon)) return false;
    if (!FloatEqual(def1.delta_Z, def2.delta_Z, kdEpsilon)) return false;
    if (!FloatEqual(def1.rot_X, def2.rot_X, kdEpsilon)) return false;
    if (!FloatEqual(def1.rot_Y, def2.rot_Y, kdEpsilon)) return false;
    if (!FloatEqual(def1.rot_Z, def2.rot_Z, kdEpsilon)) return false;
    if (!FloatEqual(def1.bwscale, def2.bwscale, kdEpsilon)) return false;
    if (def1.to84_via != def2.to84_via) return false;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMathComparator.SameDatum")

    // They're mathematically identical.
    return true;
}

// Returns whether the two defs are the same, mathematically.
// Non-mathematical attributes such as description are ignored
// for comparison purposes.
//
bool CCoordinateSystemMathComparator::SameCoordinateSystem(MgCoordinateSystem *pDefinition1, MgCoordinateSystem *pDefinition2)
{
    MG_TRY()
    assert(pDefinition1);
    assert(pDefinition2);
    if (!pDefinition1 || !pDefinition2) return false;

    // tolerance for floating-point comparisons
    const double kdEpsilon = 1.0E-13;

    bool bResult;
    cs_Csdef_ def1, def2;
    bResult = BuildCsDefFromInterface(pDefinition1, def1);
    if (!bResult) return bResult;
    bResult = BuildCsDefFromInterface(pDefinition2, def2);
    if (!bResult) return bResult;

    // datum and ellipsoid must match
    if (0 != CS_stricmp(def1.dat_knm, def2.dat_knm)) return false;
    if (0 != CS_stricmp(def1.elp_knm, def2.elp_knm)) return false;

    // projection and units must match
    if (0 != CS_stricmp(def1.prj_knm, def2.prj_knm)) return false;
    if (0 != CS_stricmp(def1.unit, def2.unit)) return false;

    // numbered parameters must match
    int i;
    const double *kpdParam1, *kpdParam2;
    kpdParam1 = &def1.prj_prm1;
    kpdParam2 = &def2.prj_prm1;
    for (i=0; i<knCsNumParams; i++)
    {
        if (!FloatEqual(kpdParam1[i], kpdParam2[i], kdEpsilon)) return false;
    }

    // miscellaneous other parameters must match
    if (!FloatEqual(def1.org_lng, def2.org_lng, kdEpsilon)) return false;
    if (!FloatEqual(def1.org_lat, def2.org_lat, kdEpsilon)) return false;
    if (!FloatEqual(def1.x_off, def2.x_off, kdEpsilon)) return false;
    if (!FloatEqual(def1.y_off, def2.y_off, kdEpsilon)) return false;
    if (!FloatEqual(def1.scl_red, def2.scl_red, kdEpsilon)) return false;
    if (!FloatEqual(def1.map_scl, def2.map_scl, kdEpsilon)) return false;

    //If the units match, the unit scale matches.
    assert(FloatEqual(def1.unit_scl, def2.unit_scl, kdEpsilon));

    //Note:  We deliberately do not make any assertion
    //about def1.scale being equal to def2.scale.  They
    //*should* be equal, since at this point we've established
    //that unit_scl and map_scl are equal, and supposedly
    //scale = 1.0 / (unit_scl*map_scl).  However, the
    //dictionary isn't always tidy, and in practice it turns
    //out that defs often have incorrect values stored here.

    for (i=0; i<2; i++)
    {
        if (!FloatEqual(def1.zero[i], def2.zero[i], kdEpsilon)) return false;
        if (!FloatEqual(def1.ll_min[i], def2.ll_min[i], kdEpsilon)) return false;
        if (!FloatEqual(def1.ll_max[i], def2.ll_max[i], kdEpsilon)) return false;
        if (!FloatEqual(def1.xy_min[i], def2.xy_min[i], kdEpsilon)) return false;
        if (!FloatEqual(def1.xy_max[i], def2.xy_max[i], kdEpsilon)) return false;
    }

    // quadrants must match
    if (def1.quad != def2.quad) return false;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMathComparator.SameCoordinateSystem")

    // Okay, they're mathematically identical.
    return true;
}
