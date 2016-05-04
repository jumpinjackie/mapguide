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

#include "CoordSysMacro.h"

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysUtil.h"               //for Convert_Wide_To_UTF8
#include "MentorUtil.h"                 //for IsLegalMentorName
#include "CoordSysEllipsoid.h"          //for CCoordinateSystemEllipsoid

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemEllipsoid::CCoordinateSystemEllipsoid(MgCoordinateSystemCatalog* pCatalog)
    : m_bEncrypted(true)
{
    memset(&m_def, 0, sizeof(m_def));
    SetCatalog(pCatalog);
}

CCoordinateSystemEllipsoid::~CCoordinateSystemEllipsoid()
{
}

///////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemEllipsoid::SetCatalog(MgCoordinateSystemCatalog* pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

void CCoordinateSystemEllipsoid::Init(const cs_Eldef_& def)
{
    m_def = def;
}

//------------------ Private functions --------------------------------

//Sets the specified string destination to the specified source
//string.  Fails if the source string isn't legal.
//
void CCoordinateSystemEllipsoid::SetString(CREFSTRING sSrc, char* pDest, int nMaxSize)
{
    MG_TRY()
    assert(NULL != pDest);
    CHECKARGUMENTNULL(pDest, L"MgCoordinateSystemEllipsoid.SetString");

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemEllipsoid.SetString", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemEllipsoidProtectedException", NULL);
    }

    if (!IsLegalString(sSrc.c_str(), nMaxSize))
    {
        //Can't set string, caller gave us an illegal value
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.SetString", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert to a C++ string, for Mentor's sake
    char *pStr = Convert_Wide_To_UTF8(sSrc.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEllipsoid.SetString", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy it in
    memset(pDest, 0, nMaxSize);
    strncpy(pDest, pStr, nMaxSize);

    //Clean up and return success.
    delete [] pStr;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.SetString")
}

//Private function that evaluates the protect data member of m_def
//to decide whether this object is protected or not.
//
bool CCoordinateSystemEllipsoid::Protected() const
{
    return (1 == m_def.protect);
}

//MgDisposable
void CCoordinateSystemEllipsoid::Dispose()
{
    delete this;
}

//MgCoordinateSystemEllipsoid-------------------------

//Get the key name of the ellipsoid definition.  The caller is
//responsible for deleting the returned string via delete.
//
//Corresponds to cs_Eldef_::key_nm.
//
STRING CCoordinateSystemEllipsoid::GetElCode()
{
    STRING sName;

    MG_TRY()

    wchar_t *pName = Convert_UTF8_To_Wide(m_def.key_nm);
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEllipsoid.GetElCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sName=pName;
    delete[] pName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.GetElCode")

    return sName;
}

//Sets the key name of the ellipsoid.
//Throws an exception if the provided string is not
//a legal ellipsoid name.
void CCoordinateSystemEllipsoid::SetElCode(CREFSTRING sCode)
{
    MG_TRY()

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemEllipsoid.SetElCode", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemEllipsoidProtectedException", NULL);
    }

    if (sCode.empty())
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.SetElCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert to a C++ string, for Mentor's sake
    char *pStr = Convert_Wide_To_UTF8(sCode.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEllipsoid.SetElCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Try to make a legal name.  CS_nampp() legalizes the
    //string and returns 0 for success.  If the string can't
    //be made legal, it returns -1.
    int nResult = CS_nampp(pStr);

    if (0 != nResult)
    {
        //illegal string
        delete [] pStr;
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.SetElCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy into the definition
    memset(m_def.key_nm, 0, cs_KEYNM_DEF);
    strncpy(m_def.key_nm, pStr, cs_KEYNM_MAX);

    //Free the converted string
    delete [] pStr;

    //And we're done!
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.SetElCode")
}

//Checks whether the specified string is a legal ellipsoid key name.
bool CCoordinateSystemEllipsoid::IsLegalElCode(CREFSTRING sCode)
{
    return IsLegalMentorName(sCode.c_str());
}

//Gets whether the def is "valid."  Validity, in this case, is defined in
//terms of whether a catalog will accept this def when attempting to add it.
//Unfortunately, unlike coordinate systems (for which Mentor provides the
//CS_cschk function to test for mathematical validity), Mentor doesn't
//provide any testing for us.
//
//If this function returns false, then attempting to add the def to a
//catalog will result in failure.
//
bool CCoordinateSystemEllipsoid::IsValid()
{
    bool bIsValid = false;

    MG_TRY()
    //Make sure the name is legal
    bIsValid = IsLegalMentorName(m_def.key_nm);
    if (!bIsValid)
    {
        return true;
    }

    //Make sure the radii have been initialized
    bIsValid = (IsLegalEllipsoidRadius(m_def.e_rad) &&
                IsLegalEllipsoidRadius(m_def.p_rad));
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.IsValid")

    return bIsValid;
}

//Gets whether the def is usable in the context of the given catalog.
//The IsValid() function, above, simply checks the def for internal
//validity in its own right.  This function, on the other hand, checks
//for other conditions such as unresolved references and so forth which
//can only be evaluated in a larger context.  For example, a coordinate
//system definition may specify that it uses datum "FOO", which is a
//perfectly legitimate datum name, so IsValid() would return true; but
//unless there is in fact a datum "FOO" present in the catalog where we
//try to use the coordinate system, it can't be used and IsUsable() would
//return false.  Note that this function will always return false if
//IsValid() returns false.
//
//In the specific case of an ellipsoid, we're not referencing anything,
//so there are no checks to do here.  We'll simply return whether the
//ellipsoid itself is valid.
//
bool CCoordinateSystemEllipsoid::IsUsable()
{
    bool bIsUsable = false;

    MG_TRY()
    bIsUsable = IsValid();
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.IsUsable")

    return bIsUsable;
}

//Checks whether the specified def is the "same" as this one.
//(Key name is ignored for purposes of comparison.)
bool CCoordinateSystemEllipsoid::IsSameAs(MgGuardDisposable *pDef)
{
    MG_TRY()
    bool bIsSame = false;

    assert(NULL != pDef);
    CHECKARGUMENTNULL(pDef, L"MgCoordinateSystemEllipsoid.IsSameAs");

    //Make sure it's an ellipsoid definition
    MgCoordinateSystemEllipsoid *pElDef=dynamic_cast<MgCoordinateSystemEllipsoid*>(pDef);
    if (!pElDef)
    {
        return bIsSame; //not an ellipsoid def!
    }

    //Build a Mentor struct from the other datum interface
    cs_Eldef_ def;
    bool bResult = BuildElDefFromInterface(pElDef, def);
    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemEllipsoid.IsSameAs", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //And compare with this one!

    if (0 != CS_stricmp(m_def.group, def.group)) return bIsSame;
    if (0 != CS_stricmp(m_def.name, def.name)) return bIsSame;
    if (0 != CS_stricmp(m_def.source, def.source)) return bIsSame;

    if (m_def.e_rad != def.e_rad) return bIsSame;
    if (m_def.p_rad != def.p_rad) return bIsSame;

    assert(FloatEqual(m_def.flat, def.flat));
    assert(FloatEqual(m_def.ecent, def.ecent));

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.IsSameAs")

    //They're the same!
    return true;
}

//Makes a copy of this object and returns it to the caller.
MgCoordinateSystemEllipsoid* CCoordinateSystemEllipsoid::CreateClone()
{
    Ptr<CCoordinateSystemEllipsoid> pDef;

    MG_TRY()

    //Make an object to be the copy
    pDef = new CCoordinateSystemEllipsoid(m_pCatalog);

    if (NULL == pDef.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEllipsoid.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy data members into the new object.  NOTE:  if new
    //data members are added to CCoordinateSystemEllipsoid, this will
    //need to be updated.
    pDef->m_def = m_def;

    //unset the EPSG code; we've an editable instance now where we neither can guarantee the correctness of the EPSG code
    //nor is it currently supported by CsMap's NameMapper anyway
    pDef->m_def.epsgNbr = 0;
    pDef->m_bEncrypted = m_bEncrypted;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.CreateClone")

    //And we're done!  Return success.
    return pDef.Detach();
}

//Get the description of the ellipsoid definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
//
//Corresponds to cs_Eldef_::name.
//
STRING CCoordinateSystemEllipsoid::GetDescription()
{
    STRING sDesc;

    MG_TRY()
    wchar_t *pDesc = Convert_UTF8_To_Wide(m_def.name);
    if (NULL == pDesc)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEllipsoid.GetDescription", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sDesc=pDesc;
    delete[] pDesc;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.GetDescription")

    return sDesc;
}

//Sets the description of the ellipsoid.
//
void CCoordinateSystemEllipsoid::SetDescription(CREFSTRING sDesc)
{
    SetString(sDesc, &m_def.name[0], sizeof(m_def.name));
}

//Returns whether the specified string is a legal description.
//
bool CCoordinateSystemEllipsoid::IsLegalDescription(CREFSTRING sDesc)
{
    return IsLegalString(sDesc.c_str(), sizeof(m_def.name))?  true : false;
}

//Gets the group of the def (provided by Mentor for grouping purposes; not
//currently used by any Autodesk GIS products, although that could change in
//the future).  The caller is responsible for deleting the returned string
//via ::SysFreeString().
//
//Corresponds to cs_Eldef_::group.
//
STRING CCoordinateSystemEllipsoid::GetGroup()
{
    STRING sGroup;

    MG_TRY()
    wchar_t *pGroup = Convert_UTF8_To_Wide(m_def.group);
    if (NULL == pGroup)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEllipsoid.GetGroup", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sGroup=pGroup;
    delete[] pGroup;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.GetGroup")

    return sGroup;
}

//Sets the group of the ellipsoid.
//
void CCoordinateSystemEllipsoid::SetGroup(CREFSTRING sGroup)
{
    SetString(sGroup, &m_def.group[0], sizeof(m_def.group));
}

//Returns whether the specified string is a legal group string.
//
bool CCoordinateSystemEllipsoid::IsLegalGroup(CREFSTRING sGroup)
{
    return IsLegalString(sGroup.c_str(), sizeof(m_def.group))?  true : false;
}

//Get the source of the ellipsoid definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
//
//Corresponds to cs_Eldef_::source.
//
STRING CCoordinateSystemEllipsoid::GetSource()
{
    STRING sSource;

    MG_TRY()
    wchar_t *pSource = Convert_UTF8_To_Wide(m_def.source);
    if (NULL == pSource)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemEllipsoid.GetSource", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING sSource(pSource);
    delete[] pSource;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.GetSource")

    return sSource;
}

//Sets the source of the ellipsoid.
//
void CCoordinateSystemEllipsoid::SetSource(CREFSTRING sSource)
{
    SetString(sSource, &m_def.source[0], sizeof(m_def.source));
}

//Gets/Sets the EPSG code
//
DEFINE_GET_SET_NUMERIC(CCoordinateSystemEllipsoid,EpsgCode,INT16,this->m_def.epsgNbr)

//Returns whether the specified string is a legal source string
//
bool CCoordinateSystemEllipsoid::IsLegalSource(CREFSTRING sSource)
{
    return IsLegalString(sSource.c_str(), sizeof(m_def.source))?  true : false;
}

//Gets whether the item is protected or not.  "Read-only" items are defined
//as having their protect flag set to 1.
//
bool CCoordinateSystemEllipsoid::IsProtected()
{
    return Protected();
}

//Gets the age of the item, in days since last modification, if it's a
//user-defined item (i.e., isn't marked as protected).  If it's read-only,
//then returns -1.
//
INT16 CCoordinateSystemEllipsoid::GetAge()
{
    INT16 sAge = Protected()? -1 : m_def.protect;
    return sAge;
}

//Sets whether this item is protected or not.  When protected,
//methods which attempt to modify the object will
//throw an exception (Except for SetProtectMode and SetEncryptMode,
//which can be called even on protected objects.)
//
void CCoordinateSystemEllipsoid::SetProtectMode(bool bIsProtected)
{
    m_def.protect = bIsProtected? 1 : 0;
}

//Returns whether this item is encrypted or not.  The only effect
//of its "encrypted" state is how it behaves when persisted to or
//from a stream.
//
bool CCoordinateSystemEllipsoid::IsEncrypted()
{
    return m_bEncrypted;
}

//Sets whether this item is encrypted or not.
//
void CCoordinateSystemEllipsoid::SetEncryptMode(bool bIsEncrypted)
{
    m_bEncrypted = bIsEncrypted? true : false;
}

//Gets the equatorial radius of the ellipsoid definition.
//
double CCoordinateSystemEllipsoid::GetEquatorialRadius()
{
    return m_def.e_rad;
}

//Gets the polar radius of the ellipsoid definition.
//
double CCoordinateSystemEllipsoid::GetPolarRadius()
{
    return m_def.p_rad;
}

//Gets the flattening ratio of the ellipsoid definition.
//
double CCoordinateSystemEllipsoid::GetFlatteningRatio()
{
    return m_def.flat;
}

//Gets the eccentricity of the ellipsoid definition.
//
double CCoordinateSystemEllipsoid::GetEccentricity()
{
    return m_def.ecent;
}

//Sets the radii, in meters, of the ellipsoid.
//Throws an exception if the numbers provided are not mathematically
//valid.  (Flattening and eccentricity will automatically be
//calculated from the provided radii.)
//
void CCoordinateSystemEllipsoid::SetRadii(double dEquatorialRadius, double dPolarRadius)
{
    MG_TRY()

    //Make sure this object isn't read-only
    if (Protected())
    {
        //can't change a read-only object
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemEllipsoid.SetRadii", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemEllipsoidProtectedException", NULL);
    }

    //Make sure the radii are mathematically valid
    if (!IsLegalEllipsoidRadius(dEquatorialRadius)
        || !IsLegalEllipsoidRadius(dPolarRadius))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.SetRadii", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make sure the flattening ratio is legitimate
    double dFlat = FlatteningRatio(dEquatorialRadius, dPolarRadius);
    if (!::IsLegalFlatteningRatio(dFlat))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.SetRadii", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Okay, everything's kosher, store the values.
    m_def.e_rad = dEquatorialRadius;
    m_def.p_rad = dPolarRadius;
    m_def.flat = dFlat;
    m_def.ecent = Eccentricity(dFlat);

    //And return success!
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.SetRadii")
}

//Utility function which calculates an eccentricity from the given
//flattening ratio.
//
double CCoordinateSystemEllipsoid::FlatteningRatioToEccentricity(double dFlat)
{
    double dEccent = 0.0;

    MG_TRY()

    //Make sure they've given us a mathematically valid flattening value
    assert(dFlat >= 0.0);
    assert(dFlat <= 1.0);
    if ((dFlat < 0.0) || (dFlat > 1.0))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.FlatteningRatioToEccentricity", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    dEccent = Eccentricity(dFlat);
    assert(FloatEqual(dFlat, FlatteningRatio(dEccent)));

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.FlatteningRatioToEccentricity")

    return dEccent;
}

//Utility function which calculates a flattening ratio from the
//given eccentricity.
//
double CCoordinateSystemEllipsoid::EccentricityToFlatteningRatio(double dEccent)
{
    double dFlat = 0.0;

    MG_TRY()

    //Make sure they've given us a mathematically valid eccentricity value
    assert(dEccent >= 0.0);
    assert(dEccent <= 1.0);
    if ((dEccent < 0.0) || (dEccent > 1.0))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.EccentToFlat", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    dFlat = FlatteningRatio(dEccent);
    assert(FloatEqual(dEccent, Eccentricity(dFlat)));

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.EccentToFlat")

    return dFlat;
}

//Utility function which calculates an equatorial radius from the given
//polar radius and flattening ratio.
//
double CCoordinateSystemEllipsoid::EquatorialRadiusFromPolarRadiusFlatteningRatio(double dPolarRadius, double dFlatteningRatio)
{
    double dEquatorialRadius = 0.0;

    MG_TRY()

    //Make sure they've given us a mathematically valid flattening value
    assert(dFlatteningRatio >= 0.0);
    assert(dFlatteningRatio < 1.0);
    if ((dFlatteningRatio < 0.0) || (dFlatteningRatio >= 1.0))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.EquatorialRadiusFromPolarRadiusFlatteningRatio", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    dEquatorialRadius = dPolarRadius / (1.0 - dFlatteningRatio);
    assert(FloatEqual(dFlatteningRatio, FlatteningRatio(dEquatorialRadius, dPolarRadius)));

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.EquatorialRadiusFromPolarRadiusFlatteningRatio")

    return dEquatorialRadius;
}

//Utility function which calculates a polar radius from the given
//equatorial radius and flattening ratio.
//
double CCoordinateSystemEllipsoid::PolarRadiusFromEquatorialRadiusFlatteningRatio(double dEquatorialRadius, double dFlatteningRatio)
{
    double dPolarRadius = 0.0;

    MG_TRY()

    //Make sure they've given us a mathematically valid flattening value
    assert(dFlatteningRatio >= 0.0);
    assert(dFlatteningRatio < 1.0);
    if ((dFlatteningRatio < 0.0) || (dFlatteningRatio >= 1.0))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.PolarRadiusFromEquatorialRadiusFlatteningRatio", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    dPolarRadius = dEquatorialRadius * (1.0 - dFlatteningRatio);
    assert(FloatEqual(dFlatteningRatio, FlatteningRatio(dEquatorialRadius, dPolarRadius)));

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.PolarRadiusFromEquatorialRadiusFlatteningRatio")

    return dPolarRadius;
}

//Utility function which calculates the great-circle distance and
//azimuth between two points on the ellipsoid.  Distance is in the
//same units as the ellipsoid radius (meters).  Azimuth is in degrees
//east of north.  Note that in the two "degenerate" cases of coincident
//points or antipodal points, the azimuth will be returned as zero.
void CCoordinateSystemEllipsoid::GreatCircle(double dLongitude1, double dLatitude1, double dLongitude2, double dLatitude2, double *pdDistance, double *pdAzimuth)
{
    MG_TRY()

    assert(NULL != pdDistance);
    assert(NULL != pdAzimuth);
    CHECKARGUMENTNULL(pdDistance, L"MgCoordinateSystemEllipsoid.GreatCircle");
    CHECKARGUMENTNULL(pdAzimuth, L"MgCoordinateSystemEllipsoid.GreatCircle");

    //set up our input variables
    double dFromLL[2], dToLL[2];
    dFromLL[LNG] = dLongitude1;
    dFromLL[LAT] = dLatitude1;
    dToLL[LNG] = dLongitude2;
    dToLL[LAT] = dLatitude2;
    double dEccentricitySquared = m_def.ecent * m_def.ecent;

    //do the calculations
    *pdAzimuth = CS_llazdd(
        m_def.e_rad,            //equatorial radius
        dEccentricitySquared,    //eccentricity, squared
        dFromLL,                //source point
        dToLL,                    //destination point
        pdDistance);            //great-circle distance

    //Return success.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.GreatCircle")
}

double CCoordinateSystemEllipsoid::GetGreatCircleDistance(double dLongitude1, double dLatitude1, double dLongitude2, double dLatitude2)
{
    double dDistance=0.;
    double dAzimuth=0.;
    GreatCircle(dLongitude1, dLatitude1, dLongitude2, dLatitude2, &dDistance, &dAzimuth);
    return dDistance;
}

double CCoordinateSystemEllipsoid::GetGreatCircleAzimuth(double dLongitude1, double dLatitude1, double dLongitude2, double dLatitude2)
{
    double dDistance=0.;
    double dAzimuth=0.;
    GreatCircle(dLongitude1, dLatitude1, dLongitude2, dLatitude2, &dDistance, &dAzimuth);
    return dAzimuth;
}

//Get the limits of legal ellipsoid radius values, in meters.
void CCoordinateSystemEllipsoid::GetRadiusBounds(double *pdMinRadius, double *pdMaxRadius)
{
    MG_TRY()
    assert(NULL != pdMinRadius);
    assert(NULL != pdMaxRadius);
    CHECKARGUMENTNULL(pdMinRadius, L"MentorDictionary.GetRadiusBounds");
    CHECKARGUMENTNULL(pdMaxRadius, L"MentorDictionary.GetRadiusBounds");

    *pdMinRadius = kdMinEllipsoidRadius;
    *pdMaxRadius = kdMaxEllipsoidRadius;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.GetRadiusBounds")
}

double CCoordinateSystemEllipsoid::GetRadiusMin()
{
    double dMinRadius=0.;
    double dMaxRadius=0.;
    GetRadiusBounds(&dMinRadius, &dMaxRadius);
    return dMinRadius;
}

double CCoordinateSystemEllipsoid::GetRadiusMax()
{
    double dMinRadius=0.;
    double dMaxRadius=0.;
    GetRadiusBounds(&dMinRadius, &dMaxRadius);
    return dMaxRadius;
}

//Returns whether the specified radius is legal for an ellipsoid.
//
bool CCoordinateSystemEllipsoid::IsLegalRadius(double dRadius)
{
    bool bLegal=false;

    MG_TRY()
    bLegal=IsLegalEllipsoidRadius(dRadius);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.IsLegalRadius")

    return bLegal;
}

//Get the limits of legal ellipsoid flattening ratios.
//
void CCoordinateSystemEllipsoid::GetFlatteningRatioBounds(double *pdMinFlat, double *pdMaxFlat)
{
    MG_TRY()
    assert(NULL != pdMinFlat);
    assert(NULL != pdMaxFlat);
    CHECKARGUMENTNULL(pdMinFlat, L"MentorDictionary.GetFlatteningRatioBounds");
    CHECKARGUMENTNULL(pdMaxFlat, L"MentorDictionary.GetFlatteningRatioBounds");

    *pdMinFlat = kdMinEllipsoidFlat;
    *pdMaxFlat = kdMaxEllipsoidFlat;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.GetFlatteningRatioBounds")
}

double CCoordinateSystemEllipsoid::GetFlatteningRatioMin()
{
    double dMinFlat=0.;
    double dMaxFlat=0.;
    GetFlatteningRatioBounds(&dMinFlat, &dMaxFlat);
    return dMinFlat;
}

double CCoordinateSystemEllipsoid::GetFlatteningRatioMax()
{
    double dMinFlat=0.;
    double dMaxFlat=0.;
    GetFlatteningRatioBounds(&dMinFlat, &dMaxFlat);
    return dMaxFlat;
}


//Returns whether the specified flattening ratio is legal
//for an ellipsoid.
//
bool CCoordinateSystemEllipsoid::IsLegalFlatteningRatio(double dFlat)
{
    bool bLegal=false;

    MG_TRY()
    bLegal=::IsLegalFlatteningRatio(dFlat);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.IsLegalFlatteningRatio")

    return bLegal;
}

//Calculates a flattening ratio, given a pair of radii.
//
double CCoordinateSystemEllipsoid::FlatteningRatioFromRadii(double dEquatorialRadius, double dPolarRadius)
{
    double dFlat = 0.0;

    MG_TRY()

    if (!IsLegalEllipsoidRadius(dEquatorialRadius))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.FlatteningRatioFromRadii", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (!IsLegalEllipsoidRadius(dPolarRadius))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.FlatteningRatioFromRadii", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    dFlat = FlatteningRatio(dEquatorialRadius, dPolarRadius);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.FlatteningRatioFromRadii")

    return dFlat;
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemEllipsoid::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}

//*****************************************************************************
UINT8* CCoordinateSystemEllipsoid::SerializeFrom(UINT8* pStream)
{
    UINT8* pStreamOut=pStream;

    MG_TRY()
    assert(NULL != pStream);
    if (!pStream)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.SerializeFrom", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    UINT8 nVersion=pStreamOut[0];

    if (kElRelease0==nVersion)
    {
        pStreamOut++;

        //Read the def from the stream
        cs_Eldef_ def;
        UINT8* pBuf = reinterpret_cast<UINT8 *>(&def);
        memcpy(pBuf, pStreamOut, sizeof(def));
        pStreamOut = pStreamOut + sizeof(def);

        //Decrypt it if necessary
        unsigned char ucKey = def.fill[0];
        bool bEncrypted = ('\0' != ucKey);
        if (bEncrypted)
        {
            for (long i=0; i<sizeof(def); i++)
            {
                pBuf[i] ^= 0xFF;
            }
            def.fill[0] = '\0';
        }

        //Make sure it's valid
        bool bPrevEncrypted = m_bEncrypted;
        cs_Eldef_ prevDef = m_def;
        m_def = def;
        m_bEncrypted = bEncrypted;
        if (!IsValid())
        {
            m_bEncrypted = bPrevEncrypted;
            m_def = prevDef;
            throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.SerializeFrom", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.SerializeFrom")
    return pStreamOut;
}

//*****************************************************************************
UINT8* CCoordinateSystemEllipsoid::SerializeTo(UINT8* pStream)
{
    UINT8* pStreamOut=pStream;

    MG_TRY()
    assert(NULL != pStream);
    if (!pStream)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemEllipsoid.SerializeTo", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //save the version
    pStreamOut[0]=kElRelease0;
    pStreamOut++;

    cs_Eldef_ def = m_def;
    char *pBuf = reinterpret_cast<char *>(&def);
    if (m_bEncrypted)
    {
        for (long i=0; i<sizeof(def); i++)
        {
            pBuf[i] ^= 0xFF;
        }
        def.fill[0] = 0x01;
    }

    memcpy(pStreamOut, pBuf, sizeof(def));
    pStreamOut = pStreamOut + sizeof(def);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemEllipsoid.SerializeTo")
    return pStreamOut;
}

//*****************************************************************************
UINT32 CCoordinateSystemEllipsoid::GetSizeSerialized()
{
    //size of the structure and the verison number
    size_t size=sizeof(m_def)+sizeof(UINT8);
    return static_cast<UINT32>(size);
}
