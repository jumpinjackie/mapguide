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

#include "CoordSysUtil.h"               //for Convert_Wide_To_Ascii
#include "CoordSysEllipsoid.h"          //for CCoordinateSystemEllipsoid

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemEllipsoid::CCoordinateSystemEllipsoid()
{
}

CCoordinateSystemEllipsoid::~CCoordinateSystemEllipsoid()
{
}

//MgDisposable
void CCoordinateSystemEllipsoid::Dispose()
{
    delete this;
}

//MgCoordinateSystemEllipsoid-------------------------

//Get the key name of the ellipsoid definition.  The caller is
//responsible for deleting the returned string via delete.
STRING CCoordinateSystemEllipsoid::GetCode()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetCode", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the key name of the ellipsoid.
//Throws an exception if the provided string is not
//a legal ellipsoid name.
void CCoordinateSystemEllipsoid::SetCode(CREFSTRING sCode)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.SetCode", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Checks whether the specified string is a legal ellipsoid key name.
bool CCoordinateSystemEllipsoid::IsLegalCode(CREFSTRING sCode)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.IsLegalCode", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystemEllipsoid::IsValid()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.IsValid", __LINE__, __WFILE__, NULL, L"", NULL);
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
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.IsSameAs", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Makes a copy of this object and returns it to the caller.
MgCoordinateSystemEllipsoid* CCoordinateSystemEllipsoid::CreateClone()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Get the description of the ellipsoid definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
STRING CCoordinateSystemEllipsoid::GetDescription()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetDescription", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the description of the ellipsoid.
//
void CCoordinateSystemEllipsoid::SetDescription(CREFSTRING sDesc)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.SetDescription", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether the specified string is a legal description.
//
bool CCoordinateSystemEllipsoid::IsLegalDescription(CREFSTRING sDesc)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.IsLegalDescription", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the group of the def (provided by Mentor for grouping purposes; not
//currently used by any Autodesk GIS products, although that could change in
//the future).  The caller is responsible for deleting the returned string
//via ::SysFreeString().
STRING CCoordinateSystemEllipsoid::GetGroup()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetGroup", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the group of the ellipsoid.
//
void CCoordinateSystemEllipsoid::SetGroup(CREFSTRING sGroup)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.SetGroup", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether the specified string is a legal group string.
//
bool CCoordinateSystemEllipsoid::IsLegalGroup(CREFSTRING sGroup)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.IsLegalGroup", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Get the source of the ellipsoid definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
STRING CCoordinateSystemEllipsoid::GetSource()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetSource", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the source of the ellipsoid.
//
void CCoordinateSystemEllipsoid::SetSource(CREFSTRING sSource)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.SetSource", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether the specified string is a legal source string
//
bool CCoordinateSystemEllipsoid::IsLegalSource(CREFSTRING sSource)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.IsLegalSource", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets whether the item is protected or not.  "Read-only" items are defined
//as having their protect flag set to 1.
//
bool CCoordinateSystemEllipsoid::IsProtected()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.IsProtected", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the age of the item, in days since last modification, if it's a
//user-defined item (i.e., isn't marked as protected).  If it's read-only,
//then returns -1.
//
INT16 CCoordinateSystemEllipsoid::GetAge()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetAge", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets whether this item is protected or not.  When protected,
//methods which attempt to modify the object will
//throw an exception (Except for SetProtectMode and SetEncryptMode,
//which can be called even on protected objects.)
//
void CCoordinateSystemEllipsoid::SetProtectMode(bool bIsProtected)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.SetProtectMode", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether this item is encrypted or not.  The only effect
//of its "encrypted" state is how it behaves when persisted to or
//from a stream.
//
bool CCoordinateSystemEllipsoid::IsEncrypted()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.IsEncrypted", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets whether this item is encrypted or not.
//
void CCoordinateSystemEllipsoid::SetEncryptMode(bool bIsEncrypted)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.SetEncryptMode", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the equatorial radius of the ellipsoid definition.
//
double CCoordinateSystemEllipsoid::GetEquatorialRadius()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetEquatorialRadius", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the polar radius of the ellipsoid definition.
//
double CCoordinateSystemEllipsoid::GetPolarRadius()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetPolarRadius", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the flattening ratio of the ellipsoid definition.
//
double CCoordinateSystemEllipsoid::GetFlatteningRatio()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetFlatteningRatio", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the eccentricity of the ellipsoid definition.
//
double CCoordinateSystemEllipsoid::GetEccentricity()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetEccentricity", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the radii, in meters, of the ellipsoid.
//Throws an exception if the numbers provided are not mathematically
//valid.  (Flattening and eccentricity will automatically be
//calculated from the provided radii.)
//
void CCoordinateSystemEllipsoid::SetRadii(double dEquatorialRadius, double dPolarRadius)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.SetRadii", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Utility function which calculates an eccentricity from the given
//flattening ratio.
//
double CCoordinateSystemEllipsoid::FlatteningRatioToEccentricity(double dFlat)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.FlatteningRatioToEccentricity", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Utility function which calculates a flattening ratio from the
//given eccentricity.
//
double CCoordinateSystemEllipsoid::EccentricityToFlatteningRatio(double dEccent)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.EccentricityToFlatteningRatio", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Utility function which calculates an equatorial radius from the given
//polar radius and flattening ratio.
//
double CCoordinateSystemEllipsoid::EquatorialRadiusFromPolarRadiusFlatteningRatio(double dPolarRadius, double dFlatteningRatio)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.EquatorialRadiusFromPolarRadiusFlatteningRatio", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Utility function which calculates a polar radius from the given
//equatorial radius and flattening ratio.
//
double CCoordinateSystemEllipsoid::PolarRadiusFromEquatorialRadiusFlatteningRatio(double dEquatorialRadius, double dFlatteningRatio)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.PolarRadiusFromEquatorialRadiusFlatteningRatio", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemEllipsoid::GetGreatCircleDistance(double dLongitude1, double dLatitude1, double dLongitude2, double dLatitude2)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetGreatCircleDistance", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemEllipsoid::GetGreatCircleAzimuth(double dLongitude1, double dLatitude1, double dLongitude2, double dLatitude2)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetGreatCircleAzimuth", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemEllipsoid::GetRadiusMin()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetRadiusMin", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemEllipsoid::GetRadiusMax()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetRadiusMax", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether the specified radius is legal for an ellipsoid.
//
bool CCoordinateSystemEllipsoid::IsLegalRadius(double dRadius)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.IsLegalRadius", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemEllipsoid::GetFlatteningRatioMin()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetFlatteningRatioMin", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemEllipsoid::GetFlatteningRatioMax()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetFlatteningRatioMax", __LINE__, __WFILE__, NULL, L"", NULL);
}


//Returns whether the specified flattening ratio is legal
//for an ellipsoid.
//
bool CCoordinateSystemEllipsoid::IsLegalFlatteningRatio(double dFlat)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.IsLegalFlatteningRatio", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Calculates a flattening ratio, given a pair of radii.
//
double CCoordinateSystemEllipsoid::FlatteningRatioFromRadii(double dEquatorialRadius, double dPolarRadius)
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.FlatteningRatioFromRadii", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinateSystemCatalog* CCoordinateSystemEllipsoid::GetCatalog()
{
    throw new MgNotImplementedException(L"CCoordinateSystemEllipsoid.GetCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
}

UINT8* CCoordinateSystemEllipsoid::SerializeFrom(UINT8* pStream)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemEllipsoid.SerializeFrom", __LINE__, __WFILE__, NULL, L"", NULL);
}

UINT8* CCoordinateSystemEllipsoid::SerializeTo(UINT8* pStream)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemEllipsoid.SerializeTo", __LINE__, __WFILE__, NULL, L"", NULL);
}

UINT32 CCoordinateSystemEllipsoid::GetSizeSerialized()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemEllipsoid.GetSizeSerialized", __LINE__, __WFILE__, NULL, L"", NULL);
}
