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
#include "CoordSysCommon.h"

#include "CoordSysMathComparator.h"     //for CCoordinateSystemMathComparator
#include "CoordSysUtil.h"               //for FloatEqual
#include "MentorUtil.h"                 //for IsLegalMentorName

#include <cs_Legacy.h>

using namespace CSLibrary;

const double CCoordinateSystemMathComparator::kdEpsilonGeographic  = 1.0E-12;      // latitude & longitude
const double CCoordinateSystemMathComparator::kdEpsilonAzimuth     = 1.0E-06;      // Azimuths
const double CCoordinateSystemMathComparator::kdEpsilonLinear      = 8.0E-04;      // radii, false origins,
                                                                                   // datum deltas, geoid heights,
                                                                                   // elevations, etc.
const double CCoordinateSystemMathComparator::kdEpsilonArcSec      = 1.0E-04;      // Geocentric rotations
const double CCoordinateSystemMathComparator::kdEpsilonDatumScale  = 1.0E-05;      // Datum shift scale (ppm)
const double CCoordinateSystemMathComparator::kdEpsilonScale       = 1.0E-08;      // scale reduction, units scale
const double CCoordinateSystemMathComparator::kdEpsilonCoefficient = 1.0E-08;      // complex/affine coefficient
const double CCoordinateSystemMathComparator::kdEpsilon            = 1.0E-13;      // anything else

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemMathComparator::CCoordinateSystemMathComparator()
    : m_bCompareDatumOldParameters(true)
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

void CCoordinateSystemMathComparator::SetCompareInternalDatumOldParameters(bool bCompareDatumOldParameters)
{
    this->m_bCompareDatumOldParameters = bCompareDatumOldParameters;
}

bool CCoordinateSystemMathComparator::GetCompareInternalDatumOldParameters()
{
    return this->m_bCompareDatumOldParameters;
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

    bool bResult;
    cs_Eldef_ def1, def2;
    bResult = BuildElDefFromInterface(pDefinition1, def1);
    if (!bResult) return bResult;
    bResult = BuildElDefFromInterface(pDefinition2, def2);
    if (!bResult) return bResult;

    // Radii values are always in meters.
    if (!FloatEqual(def1.e_rad, def2.e_rad, kdEpsilonLinear)) return false;
    if (!FloatEqual(def1.p_rad, def2.p_rad, kdEpsilonLinear)) return false;
#ifdef _DEBUG
    bResult = true;
#endif
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
    bool bResult;
    unsigned paramCount;
    unsigned orgDeltaZeroCount;

    cs_Dtdef_ def1, def2;

    bResult = BuildDtDefFromInterface(pDefinition1, def1);
    if (!bResult) return bResult;
    bResult = BuildDtDefFromInterface(pDefinition2, def2);
    if (!bResult) return bResult;

    // Check the ellipsoids for "numerical" equivalence.
    Ptr<MgCoordinateSystemEllipsoid> pEllipsoid1 = pDefinition1->GetEllipsoidDefinition ();
    Ptr<MgCoordinateSystemEllipsoid> pEllipsoid2 = pDefinition2->GetEllipsoidDefinition ();
    if (!SameEllipsoid (pEllipsoid1,pEllipsoid2)) return false;

    if (!this->m_bCompareDatumOldParameters)
        return true;

    // Determine if the two definitions are both the null transformation.
    orgDeltaZeroCount  = (def1.delta_X == 0.0);
    orgDeltaZeroCount += (def1.delta_Y == 0.0);
    orgDeltaZeroCount += (def1.delta_Z == 0.0);
    bool def1IsNull = (def1.to84_via == cs_DTCTYP_WGS84  ||
                       def1.to84_via == cs_DTCTYP_NAD83  ||
                       def1.to84_via == cs_DTCTYP_GDA94  ||
                       def1.to84_via == cs_DTCTYP_NZGD2K ||
                       def1.to84_via == cs_DTCTYP_ETRF89 ||
                       def1.to84_via == cs_DTCTYP_RGF93  ||
                       (orgDeltaZeroCount == 3 &&
                         (def1.to84_via == cs_DTCTYP_GEOCTR ||
                          def1.to84_via == cs_DTCTYP_3PARM  ||
                          def1.to84_via == cs_DTCTYP_MOLO
                         )
                       )
                      );
    orgDeltaZeroCount  = (def2.delta_X == 0.0);
    orgDeltaZeroCount += (def2.delta_Y == 0.0);
    orgDeltaZeroCount += (def2.delta_Z == 0.0);
    bool def2IsNull = (def2.to84_via == cs_DTCTYP_WGS84  ||
                       def2.to84_via == cs_DTCTYP_NAD83  ||
                       def2.to84_via == cs_DTCTYP_GDA94  ||
                       def2.to84_via == cs_DTCTYP_NZGD2K ||
                       def2.to84_via == cs_DTCTYP_ETRF89 ||
                       def2.to84_via == cs_DTCTYP_RGF93  ||
                       (orgDeltaZeroCount == 3 &&
                         (def2.to84_via == cs_DTCTYP_GEOCTR ||
                          def2.to84_via == cs_DTCTYP_3PARM  ||
                          def2.to84_via == cs_DTCTYP_MOLO
                         )
                        )
                      );
    if (def1IsNull && def2IsNull)
    {
        // Both definitions represent a null conversion; therefore, they
        // are "numerically" identical.
        return true;
    }

    // Determine if the conversion technique is "numerically" identical.
    // The GEOCENTRIC and MOLODENSKY are essentially the same.  So, we
    // convert the conversion technique to GEOCENTRIC.
    if (def1.to84_via == cs_DTCTYP_MOLO)
    {
        def1.to84_via = cs_DTCTYP_GEOCTR;
    }
    if (def2.to84_via == cs_DTCTYP_MOLO)
    {
        def2.to84_via = cs_DTCTYP_GEOCTR;
    }
    if (def1.to84_via != def2.to84_via) return false;

    // Determine the number of parameters apply to this definition.
    switch (def1.to84_via)
    {
        case cs_DTCTYP_MOLO:
        case cs_DTCTYP_3PARM:
        case cs_DTCTYP_GEOCTR:
            paramCount = 3;
            break;
        case cs_DTCTYP_4PARM:
            paramCount = 4;
            break;
        case cs_DTCTYP_6PARM:
            paramCount = 6;
            break;
        case cs_DTCTYP_MREG:        // MREG will use paramaters for automatic fallback.
        case cs_DTCTYP_BURS:
        case cs_DTCTYP_7PARM:
            paramCount = 7;
            break;
        case cs_DTCTYP_NAD27:
        case cs_DTCTYP_NAD83:
        case cs_DTCTYP_WGS84:
        case cs_DTCTYP_WGS72:
        case cs_DTCTYP_HPGN:
        case cs_DTCTYP_AGD66:
        case cs_DTCTYP_AGD84:
        case cs_DTCTYP_NZGD49:
        case cs_DTCTYP_ATS77:
        case cs_DTCTYP_GDA94:
        case cs_DTCTYP_NZGD2K:
        case cs_DTCTYP_CSRS:
        case cs_DTCTYP_TOKYO:
        case cs_DTCTYP_RGF93:
        case cs_DTCTYP_ED50:
        case cs_DTCTYP_DHDN:
        case cs_DTCTYP_ETRF89:
//      case cs_DTCTYP_CHENYX:      // commented out to fix build.
                                    // put back in after cs_map catches up with MapGuide
                                    // Not a problem as this is the same as "default" anyway.
        default:
            paramCount = 0;
            break;
    }

    // Check the appropriate number of parameters.
    if (paramCount > 0)
    {
        // Delta values are always in meters.
        if (!FloatEqual(def1.delta_X, def2.delta_X, kdEpsilonLinear)) return false;
        if (!FloatEqual(def1.delta_Y, def2.delta_Y, kdEpsilonLinear)) return false;
        if (!FloatEqual(def1.delta_Z, def2.delta_Z, kdEpsilonLinear)) return false;
        if (paramCount == 4 || paramCount == 7)
        {
            // Datum scale values are always in parts per million difference from unity.
            if (!FloatEqual(def1.bwscale, def2.bwscale, kdEpsilonDatumScale)) return false;
        }
        if (paramCount > 4)
        {
            // Rotations are always in arc-seconds.
            if (!FloatEqual(def1.rot_X, def2.rot_X, kdEpsilonArcSec)) return false;
            if (!FloatEqual(def1.rot_Y, def2.rot_Y, kdEpsilonArcSec)) return false;
            if (!FloatEqual(def1.rot_Z, def2.rot_Z, kdEpsilonArcSec)) return false;
        }
    }
#ifdef _DEBUG
    bResult = true;
#endif
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

    bool bResult;
    unsigned short prj_code;
    ulong32_t prj_flags;
    double epsilon;
    struct cs_Prjprm_ paramDescriptor;
    cs_Csdef_ def1, def2;
    struct cs_Prjtab_ *pp;
    bResult = BuildCsDefFromInterface(pDefinition1, def1);
    if (!bResult) return bResult;
    bResult = BuildCsDefFromInterface(pDefinition2, def2);
    if (!bResult) return bResult;

    if (def1.dat_knm [0] != '\0' && def2.dat_knm [0] != '\0')
    {
        //both systems reference a datum - make sure, these are equal
        Ptr<MgCoordinateSystemDatum> pDatum1 = pDefinition1->GetDatumDefinition ();
        Ptr<MgCoordinateSystemDatum> pDatum2 = pDefinition2->GetDatumDefinition ();
        if (!SameDatum (pDatum1,pDatum2))
        {
            return false;
        }
    }
    else if ((def1.dat_knm [0] == '\0' && def2.dat_knm [0] == '\0') &&
             (def1.elp_knm [0] != '\0' && def2.elp_knm [0] != '\0'))
    {
        //else: both systems reference an ellipsoid - make sure, these are equal
        Ptr<MgCoordinateSystemEllipsoid> pEllipsoid1 = pDefinition1->GetEllipsoidDefinition ();
        Ptr<MgCoordinateSystemEllipsoid> pEllipsoid2 = pDefinition2->GetEllipsoidDefinition ();
        if (!SameEllipsoid (pEllipsoid1,pEllipsoid2))
        {
            return false;
        }
    }
    else
    {
        if (! ((def1.dat_knm [0] == '\0' && def2.dat_knm [0] == '\0') &&
            (def1.elp_knm [0] == '\0' && def2.elp_knm [0] == '\0')))
        {
            //mishmash... don't compare
            // Type of reference differs.
            return false;
        }

        //else: both neither reference a datum nor an ellipsoid (all key names have been '\0')
        //we only allow them to be of projection type "Non-earth (with scale and rotation)" then
        const INT32 projectionCode1 = pDefinition1->GetProjectionCode();
        const INT32 projectionCode2 = pDefinition2->GetProjectionCode();

        const bool isNonEarth1 = MgCoordinateSystemProjectionCode::Nerth == projectionCode1 ||
            MgCoordinateSystemProjectionCode::Nrthsrt == projectionCode1;

        const bool isNonEarth2 = MgCoordinateSystemProjectionCode::Nerth == projectionCode2 ||
            MgCoordinateSystemProjectionCode::Nrthsrt == projectionCode2;

        //if one was Nerth and the other was Nrthsrt, we don't return false here for now;
        //it will be checked below for it's projection code; this will fail if they aren't the same
        if (!isNonEarth1 || !isNonEarth2) //don't accept if at least one is not "non-earth"
        {
            return false;
        }
    }

    //??// It is true that the unitsmust always match, but not necessarily
    // true that the projection must match.  However, delaing with the
    // various projection variations which are equivalent when certain
    // parameters are present is a non-trivial issue which we cannot
    // deal with at the current time.  SO we leave this alone for now.

    // projection and units must match
    if (0 != CS_stricmp(def1.prj_knm, def2.prj_knm)) return false;
    if (0 != CS_stricmp(def1.unit, def2.unit)) return false;

    // We have verified that the projections are the same.  Use def1
    // to obtain information about the projection.
    pp=GetMentorProjectionObject(def1.prj_knm);
    if (pp == NULL)
    {
        return false;
    }
    prj_code = pp->code;
    prj_flags = pp->flags;

    // Need to check the parameter logical type here.  The tolerance for a
    // latitude and longitude will be different than the tolerance, for example,
    // for an azimuth, a coefficient, or an average elevation.
    int st;
    int parmNbr;
    const double *kpdParam1, *kpdParam2;
    kpdParam1 = &def1.prj_prm1;
    kpdParam2 = &def2.prj_prm1;
    for (parmNbr=0,st = 1; st > 0; parmNbr++)
    {
        st = CS_prjprm (&paramDescriptor,prj_code,parmNbr);
        if (st > 0)
        {
            if (paramDescriptor.log_type == cs_PRMLTYP_HSNS)
            {
                bResult = (kpdParam1[parmNbr] >= 0.0) ^ (kpdParam2[parmNbr] >= 0.0);
                if (bResult) return false;
            }
            else
            {
                switch (paramDescriptor.log_type)
                {
                    case cs_PRMLTYP_LNG:              // Longitude
                    case cs_PRMLTYP_LAT:              // Latitude
                        epsilon = kdEpsilonGeographic;
                        break;
                    case cs_PRMLTYP_AZM:              // Azimuth
                        epsilon = kdEpsilonAzimuth;
                        break;
                    case cs_PRMLTYP_ANGD:             // Angular Distance
                        epsilon = kdEpsilonAzimuth;
                        break;
                    case cs_PRMLTYP_CMPLXC:           // Complex Coefficient
                    case cs_PRMLTYP_AFCOEF:           // Affine Transformation Coefficient
                        epsilon = kdEpsilonCoefficient;
                        break;
                    case cs_PRMLTYP_ZNBR:             // UTM Zone Number
                        epsilon = 0.5;
                        break;
                    case cs_PRMLTYP_GHGT:             // Geoid Height (always meters)
                        epsilon = kdEpsilonLinear;
                        break;
                    case cs_PRMLTYP_ELEV:             // Elevation, in system units.
                    case cs_PRMLTYP_XYCRD:            // X/Y Coordinate (always system units)
                        epsilon = kdEpsilonLinear * def1.unit_scl;
                        break;
                    case cs_PRMLTYP_SCALE:            // Scale value
                        epsilon = kdEpsilonScale;
                        break;
                    case cs_PRMLTYP_NONE:             // No specification
                    case cs_PRMLTYP_HSNS:             // to keep code analyzers happy
                    default:
                        epsilon = kdEpsilon;
                }
                if (!FloatEqual(kpdParam1[parmNbr], kpdParam2[parmNbr], epsilon)) return false;
            }
        }
    }

    // The following are standard parameters which are standard for most,
    // BUT NOT ALL, parameters.
    if ((prj_flags & cs_PRJFLG_ORGLNG) == 0)
    {
        // Always in degrees.
        if (!FloatEqual(def1.org_lng, def2.org_lng, kdEpsilonGeographic)) return false;
    }
    if ((prj_flags & cs_PRJFLG_ORGLAT) == 0)
    {
        // Always in degrees.
        if (!FloatEqual(def1.org_lat, def2.org_lat, kdEpsilonGeographic)) return false;
    }
    if ((prj_flags & cs_PRJFLG_ORGFLS) == 0)
    {
        // Always in system units.
        epsilon = kdEpsilonLinear * def1.unit_scl;
        if (!FloatEqual(def1.x_off, def2.x_off, epsilon)) return false;
        if (!FloatEqual(def1.y_off, def2.y_off, epsilon)) return false;
    }
    if ((prj_flags & cs_PRJFLG_SCLRED) != 0)
    {
        // Always a simple ratio (i.e. 1:2500 == 0.9996)
        if (!FloatEqual(def1.scl_red, def2.scl_red, kdEpsilonScale)) return false;
    }
    // Map scale is rarely (if ever) used and us usally set to unity.
    if (!FloatEqual(def1.map_scl, def2.map_scl, kdEpsilonScale)) return false;

    //If the units match, the unit scale matches.
    assert(FloatEqual(def1.unit_scl, def2.unit_scl, kdEpsilonScale));

    //Note:  We deliberately do not make any assertion
    //about def1.scale being equal to def2.scale.  They
    //*should* be equal, since at this point we've established
    //that unit_scl and map_scl are equal, and supposedly
    //scale = 1.0 / (unit_scl*map_scl).  However, the
    //dictionary isn't always tidy, and in practice it turns
    //out that defs often have incorrect values stored here.

    // quadrants must match
    if (def1.quad != def2.quad) return false;
#ifdef _DEBUG
    bResult = true;
#endif

    MG_CATCH_AND_THROW(L"MgCoordinateSystemMathComparator.SameCoordinateSystem")

    // Okay, they're mathematically identical.
    return true;
}
