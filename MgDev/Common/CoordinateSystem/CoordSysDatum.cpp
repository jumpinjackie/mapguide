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

#include "CoordSysDatum.h"                      //for CCoordinateSystemDatum
#include "CoordSysGeodeticTransformation.h"     //for CCoordinateSystemGeodeticTransformation
#include "CoordSysUtil.h"                       //for Convert_Wide_To_Ascii

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemDatum::CCoordinateSystemDatum(MgCoordinateSystemCatalog* pCatalog)
{
    SetCatalog(pCatalog);
}

CCoordinateSystemDatum::~CCoordinateSystemDatum()
{
}

///////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemDatum::SetCatalog(MgCoordinateSystemCatalog* pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

//Gets a copy of the ellipsoid definition the datum uses.  Caller
//is responsible for freeing the returned pointer via Release().
//If the MgCoordinateSystemDatum has not been initialized, returns a null pointer.
//
MgCoordinateSystemEllipsoid* CCoordinateSystemDatum::GetEllipsoidDefinition()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.GetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
}

//MgDisposable
void CCoordinateSystemDatum::Dispose()
{
    delete this;
}

//Get the key name of the datum definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
STRING CCoordinateSystemDatum::GetCode()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.GetCode", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the key name of the datum.
//Throws an exception MgInvalidArgumentException
//if the provided string is not
//a legal datum name.
//
void CCoordinateSystemDatum::SetCode(CREFSTRING sCode)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.SetCode", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Checks whether the specified string is a legal datum key name.
//
bool CCoordinateSystemDatum::IsLegalCode(CREFSTRING sCode)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsLegalCode", __LINE__, __WFILE__, NULL, L"", NULL);
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
bool CCoordinateSystemDatum::IsValid()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsValid", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets whether the def is usable in the context of the given catalog.  The
//IsValid() function, above, simply checks the def for internal validity in
//its own right.  This function, on the other hand, checks for other
//conditions such as unresolved references and so forth which can only be
//evaluated in a larger context.  For example, a datum definition may specify
//that it uses ellipsoid "FOO", which is a perfectly legitimate ellipsoid
//name, so IsValid() would return true; but unless there is in fact an
//ellipsoid "FOO" present in the catalog where we try to use the datum, it
//can't be used and IsUsable() would return false.  Note that this function
//will always return false if IsValid() returns false.
//
bool CCoordinateSystemDatum::IsUsable(MgCoordinateSystemCatalog *pCatalog)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsUsable", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Checks whether the specified def is the "same" as this one.
//(Key name is ignored for purposes of comparison.)
//
bool CCoordinateSystemDatum::IsSameAs(MgGuardDisposable *pDef)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsSameAs", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Makes a copy of this object and returns it to the caller.
MgCoordinateSystemDatum* CCoordinateSystemDatum::CreateClone()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Get the description of the datum definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
STRING CCoordinateSystemDatum::GetDescription()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.GetDescription", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the description of the datum.
//
void CCoordinateSystemDatum::SetDescription(CREFSTRING sDesc)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.SetDescription", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether the specified string is a legal description.
//
bool CCoordinateSystemDatum::IsLegalDescription(CREFSTRING sDesc)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsLegalDescription", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the group of the def (provided by Mentor for grouping purposes; not
//currently used by any Autodesk GIS products, although that could change in
//the future).  The caller is responsible for deleting the returned string
//via ::SysFreeString().
STRING CCoordinateSystemDatum::GetGroup()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.GetGroup", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the group of the datum.
//
void CCoordinateSystemDatum::SetGroup(CREFSTRING sGroup)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.SetGroup", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether the specified string is a legal group string.
//
bool CCoordinateSystemDatum::IsLegalGroup(CREFSTRING sGroup)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsLegalGroup", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Get the source of the datum definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
STRING CCoordinateSystemDatum::GetSource()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.GetSource", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the source of the datum.
//
void CCoordinateSystemDatum::SetSource(CREFSTRING sSource)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.SetSource", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether the specified string is a legal source string
//
bool CCoordinateSystemDatum::IsLegalSource(CREFSTRING sSource)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsLegalSource", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets whether the item is protected or not.  "Read-only" items are defined
//as having their protect flag set to 1.
//
bool CCoordinateSystemDatum::IsProtected()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsProtected", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the age of the item, in days since last modification, if it's a
//user-defined item (i.e., isn't marked as protected).  If it's read-only,
//then returns -1.
//
INT16 CCoordinateSystemDatum::GetAge()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.GetAge", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets whether this item is protected or not.  When protected,
//methods which attempt to modify the object will
//Throws an exception.  (Except for SetProtectMode and SetEncryptMode,
//which can be called even on protected objects.)
//
void CCoordinateSystemDatum::SetProtectMode(bool bIsProtected)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.SetProtectMode", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether this item is encrypted or not.  The only effect
//of its "encrypted" state is how it behaves when persisted to or
//from a stream.
//
bool CCoordinateSystemDatum::IsEncrypted()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsEncrypted", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets whether this item is encrypted or not.
//
void CCoordinateSystemDatum::SetEncryptMode(bool bIsEncrypted)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.SetEncryptMode", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the "location" field of the def (corresponds to cs_Dtdef_.locatn).
//The caller is responsible for deleting the returned string via
//::SysFreeString().
//
STRING CCoordinateSystemDatum::GetLocation()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.GetLocation", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the "location" field of the def.
//Throws an exception if the specified string is not legal
//for a location field.
//
void CCoordinateSystemDatum::SetLocation(CREFSTRING sLoc)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.SetLocation", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Tests the specified string to find out whether it's a legal
//location field for this def.
//
bool CCoordinateSystemDatum::IsLegalLocation(CREFSTRING sLoc)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsLegalLocation", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the "cntry_st" field of the def (see Mentor discussion of
//cs_Dtdef_ for details).
//
STRING CCoordinateSystemDatum::GetCountryOrState()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.GetCountryOrState", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the "cntry_st" field of the def.
//Throws an exception if the specified string is not legal
//for a cntry_st field.
//
void CCoordinateSystemDatum::SetCountryOrState(CREFSTRING sCountryOrState)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.SetCountryOrState", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Tests the specified string to find out whether it's a legal cntry_st
//field for this def.
//
bool CCoordinateSystemDatum::IsLegalCountryOrState(CREFSTRING sCountryOrState)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.IsLegalCountryOrState", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the name of the ellipsoid which this datum references.  The caller
//is responsible for deleting the returned string via ::SysFreeString().
//
STRING CCoordinateSystemDatum::GetEllipsoid()
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.GetEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the name of the ellipsoid which this datum references.
//Throws an exception if the specified string is not
//legal for an ellipsoid name.
//
void CCoordinateSystemDatum::SetEllipsoid(CREFSTRING sEllipsoid)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.SetEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
}

//needs to be called after the datum parameters have been fully specified
//if we start with the ellipsoid the method will fail because it needs the datum parameters
//this is not just setting an internal pointer.
void CCoordinateSystemDatum::SetEllipsoidDefinition(MgCoordinateSystemEllipsoid *pEllipsoidDef)
{
    throw new MgNotImplementedException(L"CCoordinateSystemDatum.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
}

//-----------------------------------------------------------------------------
//Gets the list of available geodetic transformations MgCoordinateSystemGeodeticTransformation
//that can transform from the the current datum to the target datum sTargetDatum.
//Currently, only one geodetic transformation is available.
//In a near future, for a given target datum, there might be multiple possible ways for
//converting to that datum.
//If pTarget is NULL, WGS84 is assumed
//Caller is responsible for freeing the list of geodetic transformations.
//
MgDisposableCollection* CCoordinateSystemDatum::GetGeodeticTransformations(MgCoordinateSystemDatum *pTarget)
{
    Ptr<MgDisposableCollection> pColl;

    MG_TRY()

    Ptr<CCoordinateSystemGeodeticTransformation> pNew = new CCoordinateSystemGeodeticTransformation(m_pCatalog, this, pTarget);

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetGeodeticTransformations", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
    }

    pColl = new MgDisposableCollection;

    if (NULL == pColl.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetGeodeticTransformations", __LINE__, __WFILE__, NULL, L"MgOutOfMemoryException", NULL);
    }

    pColl->Add(pNew);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.GetGeodeticTransformations")

    return pColl.Detach();
}

MgCoordinateSystemCatalog* CCoordinateSystemDatum::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}

UINT8* CCoordinateSystemDatum::SerializeFrom(UINT8* pStream)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemDatum.SerializeFrom", __LINE__, __WFILE__, NULL, L"", NULL);
}

UINT8* CCoordinateSystemDatum::SerializeTo(UINT8* pStream)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemDatum.SerializeTo", __LINE__, __WFILE__, NULL, L"", NULL);
}

UINT32 CCoordinateSystemDatum::GetSizeSerialized()
{
    throw new MgNotImplementedException(L"MgCoordinateSystemDatum.GetSizeSerialized", __LINE__, __WFILE__, NULL, L"", NULL);
}
