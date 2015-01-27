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
#include "CriticalSection.h"

#include "CoordSysDatum.h"                      //for CCoordinateSystemDatum
#include "CoordSysGeodeticTransformation.h"     //for CCoordinateSystemGeodeticTransformation
#include "CoordSysUtil.h"                       //for Convert_Wide_To_UTF8
#include "MentorUtil.h"                         //for BuildDtDefFromInterface

using namespace CSLibrary;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemDatum::CCoordinateSystemDatum(MgCoordinateSystemCatalog* pCatalog)
    : m_bEncrypted(true)
{
    memset(&m_datum, 0, sizeof(m_datum));
    memset(&m_DtDef, 0, sizeof(m_DtDef));
    memset(&m_ElDef, 0, sizeof(m_ElDef));

    //Default transformation technique is Molodensky.
    m_DtDef.to84_via = MgCoordinateSystemGeodeticTransformationMethod::Molodensky;
    m_datum.to84_via = MgCoordinateSystemGeodeticTransformationMethod::Molodensky;
    SetCatalog(pCatalog);
}

CCoordinateSystemDatum::~CCoordinateSystemDatum()
{
    Uninitialize();
}

///////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemDatum::SetCatalog(MgCoordinateSystemCatalog* pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

//Utility function which returns whether the object has
//been initialized or not.
//
bool CCoordinateSystemDatum::IsInitialized() const
{
    return (m_datum.key_nm[0]!='\0' && m_datum.ell_knm[0]!='\0');
}

//Utility function which restores the object to an
//uninitialized state.
//
void CCoordinateSystemDatum::Uninitialize()
{
    memset(&m_datum, 0, sizeof(m_datum));
    memset(&m_DtDef, 0, sizeof(m_DtDef));
    memset(&m_ElDef, 0, sizeof(m_ElDef));
}

//Gets a copy of the ellipsoid definition the datum uses.  Caller
//is responsible for freeing the returned pointer via Release().
//If the MgCoordinateSystemDatum has not been initialized, returns a null pointer.
//
MgCoordinateSystemEllipsoid* CCoordinateSystemDatum::GetEllipsoidDefinition()
{
    Ptr<MgCoordinateSystemEllipsoid> pEl;

    MG_TRY()

    //Make sure we've been initialized
    if (!IsInitialized())
    {
        return NULL;
    }

    //create the eldef object
    pEl=BuildInterfaceFromElDef(m_ElDef, m_pCatalog);

    if (NULL == pEl.p)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.GetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumInitializationFailedException", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.GetEllipsoidDefinition")

    return pEl.Detach();
}

//-----------------------------------------------------------------------------
//Initializes the datum from the specified datum definition (which must
//be valid), using the internal pointer to the catalog.  The catalog will be used to
//resolve ellipsoid references.  If successful, the datum will be
//properly initialized, and will contain its own copy of all relevant
//information.
////Throws an exception MgCoordinateSystemDatumNoEllipsoidInDictionaryException if there's an unresolved reference (i.e., the provided
//datum references an ellipsoid that's not present in the provided
//catalog).  Fails with InvalidArgument if the input dautm definition
//is invalid (i.e., its isValid() method returns false).
//If pCatalog is NULL this method initializes only the datum portion of the definition
//without loading the ellipsoid from the dictionary
//The ellipsoid can then be set via CCoordinateSystemDatum::SetEllipsoidDefinition
//
//When passing a non-NULL pointer for the [ellipsoidMap] parameter, the ellipsoid information is tried
//being read from there; if still not found in the std::map, the fallback
//is to read the information from CsMap, i.e. the dictionary files
void CCoordinateSystemDatum::InitFromCatalog(const cs_Dtdef_& def,
                                             const map<STRING, Ptr<MgDisposable> >* const ellipsoidMap)
{
    MG_TRY()

    //Make sure the specified datum is valid
    if (def.key_nm[0]=='\0')
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDatum.InitFromCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //init the datum defintion part
    m_DtDef=def;
    CS_stncp (m_datum.key_nm,def.key_nm,sizeof (m_datum.key_nm));
    CS_stncp (m_datum.ell_knm,def.ell_knm,sizeof (m_datum.ell_knm));
    CS_stncp (m_datum.dt_name,def.name,sizeof (m_datum.dt_name));
    m_datum.delta_X = def.delta_X;
    m_datum.delta_Y = def.delta_Y;
    m_datum.delta_Z = def.delta_Z;
    m_datum.rot_X = def.rot_X;
    m_datum.rot_Y = def.rot_Y;
    m_datum.rot_Z = def.rot_Z;
    m_datum.bwscale = def.bwscale;
    m_datum.to84_via = def.to84_via;

    //no catalog, we'e done witht he DT part of initialization
    if (!m_pCatalog)
    {
        return;
    }

    //Get the ellipsoid name the datum uses
    wchar_t* pwszElName=Convert_UTF8_To_Wide(def.ell_knm);
    if (!pwszElName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.InitFromCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING sElName(::ToLower(pwszElName));
    delete[] pwszElName;
    pwszElName = NULL;

    Ptr<MgGuardDisposable> pEllipsoid;
    
    //if the caller passed in a std::map of known ellipsoids, we're trying to take the information from there
    if (NULL != ellipsoidMap)
    {
        map<STRING, Ptr<MgDisposable> >::const_iterator ellipsoidsIterator = ellipsoidMap->find(sElName);
        if (ellipsoidsIterator != ellipsoidMap->end())
        {
            Ptr<MgDisposable> disposableCsObject = ellipsoidsIterator->second;
            MgCoordinateSystemEllipsoid* ellipsoidInfo = dynamic_cast<MgCoordinateSystemEllipsoid*>(disposableCsObject.p);
            if (NULL == ellipsoidInfo)
                throw new MgInvalidArgumentException(L"MgCoordinateSystemDatum.InitFromCatalog", __LINE__, __WFILE__, NULL, L"", NULL);
            
            pEllipsoid = SAFE_ADDREF(ellipsoidInfo);
        }
    }

    //either no [ellipsoidMap] or the information we're interested in, hasn't been found in the map
    if (NULL == pEllipsoid)
    {
        //Get the ellipsoid dictionary
        Ptr<MgCoordinateSystemEllipsoidDictionary> pElDict = m_pCatalog->GetEllipsoidDictionary();
        if (!pElDict)
        {
            throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.InitFromCatalog", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumNoEllipsoidDictionaryException", NULL);
        }

        //load the ellipsoid defintion from the dictionary
        pEllipsoid = pElDict->Get(sElName);
        assert(pEllipsoid);
    }

    if (NULL == pEllipsoid)
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.InitFromCatalog", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumNoEllipsoidInDictionaryException", NULL);

    //Initialize from ellipsoid definitions we have
    SetEllipsoidDefinition(dynamic_cast<MgCoordinateSystemEllipsoid*>(pEllipsoid.p));

    MG_CATCH(L"MgCoordinateSystemDatum.InitFromCatalog")
    if (mgException != NULL)
    {
        Uninitialize();
    }

    MG_THROW()
}

//MgDisposable
void CCoordinateSystemDatum::Dispose()
{
    delete this;
}

//Get the key name of the datum definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
//
//Corresponds to cs_Dtdef_::key_nm.
//
STRING CCoordinateSystemDatum::GetDtCode()
{
    STRING sCode;

    MG_TRY()
    wchar_t* pName = Convert_UTF8_To_Wide(m_DtDef.key_nm);
    if (NULL == pName)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetDtCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sCode=pName;
    delete[] pName;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.GetDtCode")

    return sCode;
}

//Sets the key name of the datum.
//Throws an exception MgInvalidArgumentException
//if the provided string is not
//a legal datum name.
//
void CCoordinateSystemDatum::SetDtCode(CREFSTRING sCode)
{
    MG_TRY()

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.SetDtCode", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumProtectedException", NULL);
    }

    if (sCode.empty())
    {
        //null pointer is obviously not legal...
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDatum.SetDtCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert to a C++ string, for Mentor's sake
    char *pStr = Convert_Wide_To_UTF8(sCode.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.SetDtCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Try to make a legal name.  CS_nampp() legalizes the
    //string and returns 0 for success.  If the string can't
    //be made legal, it returns -1.
    int nResult = CS_nampp(pStr);

    if (0 != nResult)
    {
        //illegal string
        delete [] pStr;
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDatum.SetDtCode", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy into the definitions, dtdef and cs_datum_ must be in synch
    memset(m_datum.key_nm, 0, cs_KEYNM_DEF);
    strncpy(m_datum.key_nm, pStr, cs_KEYNM_MAX);

    memset(m_DtDef.key_nm, 0, cs_KEYNM_DEF);
    strncpy(m_DtDef.key_nm, pStr, cs_KEYNM_MAX);

    //Free the converted string
    delete[] pStr;

    //And we're done!
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.SetDtCode")
}

//Checks whether the specified string is a legal datum key name.
//
bool CCoordinateSystemDatum::IsLegalDtCode(CREFSTRING sCode)
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
bool CCoordinateSystemDatum::IsValid()
{
    bool bIsValid = false;

    MG_TRY()

    bIsValid=IsDatumValid();
    if (!bIsValid)
    {
        return bIsValid;
    }

    //Make sure the ellipsoid name is legal
    bIsValid = IsLegalMentorName(m_datum.ell_knm);
    if (!bIsValid)
    {
        return bIsValid;
    }

    //Make sure the radii have been initialized
    bIsValid = (IsLegalEllipsoidRadius(m_datum.e_rad)
                && IsLegalEllipsoidRadius(m_datum.p_rad));

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.IsValid")

    return bIsValid;
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
    bool bIsUsable = false;

    MG_TRY()

    assert(NULL != pCatalog);
    CHECKARGUMENTNULL(pCatalog, L"MgCoordinateSystemDatum.IsUsable");

    //First, make sure it's valid in its own right; if it's
    //not, then it's not usable
    if (!IsValid())
    {
        //Nope, not valid.  We need go no further.
        return false;
    }

    //Okay, the datum definition is valid.  Check to see whether
    //the catalog has the ellipsoid which the datum references.

    //Get the appropriate interface from the catalog
    Ptr<MgCoordinateSystemEllipsoidDictionary> pElDict=pCatalog->GetEllipsoidDictionary();
    if (!pElDict)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.IsUsable", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumNoEllipsoidDictionaryException", NULL);
    }

    //Build a wchar_t* from the ellipsoid name
    wchar_t* pStrEl = Convert_UTF8_To_Wide(m_datum.ell_knm);
    if (NULL == pStrEl)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.IsUsable", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING sElName(pStrEl);
    delete[] pStrEl;

    //Check to see if the ellipsoid is there
    bIsUsable = pElDict->Has(sElName);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.IsUsable")

    return bIsUsable;
}

//Checks whether the specified def is the "same" as this one.
//(Key name is ignored for purposes of comparison.)
//
bool CCoordinateSystemDatum::IsSameAs(MgGuardDisposable *pDef)
{
    bool bIsSame=false;

    MG_TRY()

    assert(NULL != pDef);
    CHECKARGUMENTNULL(pDef, L"MgCoordinateSystemDatum.IsSameAs");

    //Make sure it's a datum definition
    MgCoordinateSystemDatum* pDtDef=dynamic_cast<MgCoordinateSystemDatum*>(pDef);
    if (!pDtDef)
    {
        return false;    //not a datum def!
    }

    //Build a dtdef Mentor struct from the other datum interface
    cs_Dtdef_ def;
    if (!BuildDtDefFromInterface(pDtDef, def))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.IsSameAs", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //And compare with this one!
    if (0 != CS_stricmp(m_DtDef.ell_knm, def.ell_knm)) return false;
    if (0 != CS_stricmp(m_DtDef.group, def.group)) return false;
    if (0 != CS_stricmp(m_DtDef.locatn, def.locatn)) return false;
    if (0 != CS_stricmp(m_DtDef.cntry_st, def.cntry_st)) return false;
    if (0 != CS_stricmp(m_datum.dt_name, def.name)) return false;
    if (0 != CS_stricmp(m_DtDef.source, def.source)) return false;

    if (m_DtDef.delta_X != def.delta_X) return false;
    if (m_DtDef.delta_Y != def.delta_Y) return false;
    if (m_DtDef.delta_Z != def.delta_Z) return false;
    if (m_DtDef.rot_X != def.rot_X) return false;
    if (m_DtDef.rot_Y != def.rot_Y) return false;
    if (m_DtDef.rot_Z != def.rot_Z) return false;
    if (m_DtDef.bwscale != def.bwscale) return false;
    if (m_DtDef.to84_via != def.to84_via) return false;

    //do the ellipsoid
    Ptr<MgCoordinateSystemEllipsoid> pElDef=pDtDef->GetEllipsoidDefinition();
    if (!pElDef)
    {
        return false;
    }
    Ptr<MgCoordinateSystemEllipsoid> pThisElDef=GetEllipsoidDefinition();
    if (!pThisElDef)
    {
        return false;
    }

    //That's it!  If we've made it this far without finding
    //any differences, then they're the same as each other.
    bIsSame=pThisElDef->IsSameAs(pElDef);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.IsSameAs")

    return bIsSame;
}

//Makes a copy of this object and returns it to the caller.
MgCoordinateSystemDatum* CCoordinateSystemDatum::CreateClone()
{
    Ptr<CCoordinateSystemDatum> pNew;

    MG_TRY()

    //Make an object to be the copy
    pNew = new CCoordinateSystemDatum(m_pCatalog);

    if (NULL == pNew.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.CreateClone", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy data members into the new object.  NOTE:  if new
    //data members are added to CCoordinateSystemDatum, this will
    //need to be updated.
    //
    pNew->m_datum = m_datum;
    pNew->m_DtDef = m_DtDef;

    //unset the EPSG code; we've an editable instance now where we neither can guarantee the correctness of the EPSG code
    //nor is it currently supported by CsMap's NameMapper anyway
    pNew->m_DtDef.epsgNbr = 0;

    pNew->m_ElDef = m_ElDef;
    //don't unset the EPSG code of the ellipsoid this datum refers to

    pNew->m_bEncrypted = m_bEncrypted;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.CreateClone")

    //And we're done!  Return success.
    return pNew.Detach();
}

//Get the description of the datum definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
//
//Corresponds to cs_Dtdef_::name.
//
STRING CCoordinateSystemDatum::GetDescription()
{
    STRING sDesc;

    MG_TRY()
    wchar_t* pDesc = Convert_UTF8_To_Wide(m_datum.dt_name);
    if (NULL == pDesc)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetDescription", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sDesc=pDesc;
    delete[] pDesc;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.GetDescription")

    return sDesc;
}

//Sets the description of the datum.
//
void CCoordinateSystemDatum::SetDescription(CREFSTRING sDesc)
{
    SetString(sDesc, &m_datum.dt_name[0], sizeof(m_datum.dt_name));
    SetString(sDesc, &m_DtDef.name[0], sizeof(m_DtDef.name));
}

//Returns whether the specified string is a legal description.
//
bool CCoordinateSystemDatum::IsLegalDescription(CREFSTRING sDesc)
{
    return IsLegalString(sDesc.c_str(), sizeof(m_DtDef.name))?  true : false;
}

//Gets the group of the def (provided by Mentor for grouping purposes; not
//currently used by any Autodesk GIS products, although that could change in
//the future).  The caller is responsible for deleting the returned string
//via ::SysFreeString().
//
//Corresponds to cs_Dtdef_::group.
//
STRING CCoordinateSystemDatum::GetGroup()
{
    STRING sGroup;

    MG_TRY()
    wchar_t *pGroup = Convert_UTF8_To_Wide(m_DtDef.group);
    if (NULL == pGroup)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetGroup", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sGroup=pGroup;
    delete[] pGroup;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.GetGroup")
    return sGroup;
}

//Sets the group of the datum.
//
void CCoordinateSystemDatum::SetGroup(CREFSTRING sGroup)
{
    SetString(sGroup, &m_DtDef.group[0], sizeof(m_DtDef.group));
}

//Returns whether the specified string is a legal group string.
//
bool CCoordinateSystemDatum::IsLegalGroup(CREFSTRING sGroup)
{
    return IsLegalString(sGroup.c_str(), sizeof(m_DtDef.group))?  true : false;
}

//Get the source of the datum definition.  The caller is
//responsible for deleting the returned string via ::SysFreeString().
//
//Corresponds to cs_Dtdef_::source.
//
STRING CCoordinateSystemDatum::GetSource()
{
    STRING sSource;

    MG_TRY()
    wchar_t *pSource = Convert_UTF8_To_Wide(m_DtDef.source);
    if (NULL == pSource)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetSource", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sSource=pSource;
    delete[] pSource;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.GetSource")

    return sSource;
}

//Sets the source of the datum.
//
void CCoordinateSystemDatum::SetSource(CREFSTRING sSource)
{
    SetString(sSource, &m_DtDef.source[0], sizeof(m_DtDef.source));
}

//Gets/Sets the EPSG code
//
DEFINE_GET_SET_NUMERIC(CCoordinateSystemDatum,EpsgCode,INT16,this->m_DtDef.epsgNbr)

//Returns whether the specified string is a legal source string
//
bool CCoordinateSystemDatum::IsLegalSource(CREFSTRING sSource)
{
    return IsLegalString(sSource.c_str(), sizeof(m_DtDef.source))?  true : false;
}

//Gets whether the item is protected or not.  "Read-only" items are defined
//as having their protect flag set to 1.
//
bool CCoordinateSystemDatum::IsProtected()
{
    return Protected();
}

//Gets the age of the item, in days since last modification, if it's a
//user-defined item (i.e., isn't marked as protected).  If it's read-only,
//then returns -1.
//
INT16 CCoordinateSystemDatum::GetAge()
{
    INT16 sAge = Protected()? -1 : m_DtDef.protect;
    return sAge;
}

//Sets whether this item is protected or not.  When protected,
//methods which attempt to modify the object will
//Throws an exception.  (Except for SetProtectMode and SetEncryptMode,
//which can be called even on protected objects.)
//
void CCoordinateSystemDatum::SetProtectMode(bool bIsProtected)
{
    m_DtDef.protect = bIsProtected? 1 : 0;
}

//Returns whether this item is encrypted or not.  The only effect
//of its "encrypted" state is how it behaves when persisted to or
//from a stream.
//
bool CCoordinateSystemDatum::IsEncrypted()
{
    return m_bEncrypted;
}

//Sets whether this item is encrypted or not.
//
void CCoordinateSystemDatum::SetEncryptMode(bool bIsEncrypted)
{
    m_bEncrypted = bIsEncrypted;
}

//Gets the "location" field of the def (corresponds to cs_Dtdef_.locatn).
//The caller is responsible for deleting the returned string via
//::SysFreeString().
//
STRING CCoordinateSystemDatum::GetLocation()
{
    STRING sLoc;

    MG_TRY()
    wchar_t* pLoc = Convert_UTF8_To_Wide(m_DtDef.locatn);
    if (NULL == pLoc)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetLocation", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sLoc=pLoc;
    delete[] pLoc;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.GetLocation")

    return sLoc;
}

//Sets the "location" field of the def.
//Throws an exception if the specified string is not legal
//for a location field.
//
void CCoordinateSystemDatum::SetLocation(CREFSTRING sLoc)
{
    SetString(sLoc, &m_DtDef.locatn[0], sizeof(m_DtDef.locatn));
}

//Tests the specified string to find out whether it's a legal
//location field for this def.
//
bool CCoordinateSystemDatum::IsLegalLocation(CREFSTRING sLoc)
{
    return IsLegalString(sLoc.c_str(), sizeof(m_DtDef.locatn))?  true : false;
}

//Gets the "cntry_st" field of the def (see Mentor discussion of
//cs_Dtdef_ for details).
//
STRING CCoordinateSystemDatum::GetCountryOrState()
{
    STRING sCountryOrState;

    MG_TRY()
    wchar_t *pCountryOrState = Convert_UTF8_To_Wide(m_DtDef.cntry_st);
    if (NULL == pCountryOrState)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetCountryOrState", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sCountryOrState=pCountryOrState;
    delete[] pCountryOrState;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.GetCountryOrState")

    return sCountryOrState;
}

//Sets the "cntry_st" field of the def.
//Throws an exception if the specified string is not legal
//for a cntry_st field.
//
void CCoordinateSystemDatum::SetCountryOrState(CREFSTRING sCountryOrState)
{
    SetString(sCountryOrState, &m_DtDef.cntry_st[0], sizeof(m_DtDef.cntry_st));
}

//Tests the specified string to find out whether it's a legal cntry_st
//field for this def.
//
bool CCoordinateSystemDatum::IsLegalCountryOrState(CREFSTRING sCountryOrState)
{
    return IsLegalString(sCountryOrState.c_str(), sizeof(m_DtDef.cntry_st))?  true : false;
}

//Gets the name of the ellipsoid which this datum references.  The caller
//is responsible for deleting the returned string via ::SysFreeString().
//
STRING CCoordinateSystemDatum::GetEllipsoid()
{
    STRING sEllipsoid;

    MG_TRY()
    wchar_t* pEllipsoid = Convert_UTF8_To_Wide(m_DtDef.ell_knm);
    if (NULL == pEllipsoid)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    sEllipsoid=pEllipsoid;
    delete[] pEllipsoid;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.GetEllipsoid")

    return sEllipsoid;
}

//Sets the name of the ellipsoid which this datum references.
//Throws an exception if the specified string is not
//legal for an ellipsoid name.
//
void CCoordinateSystemDatum::SetEllipsoid(CREFSTRING sEllipsoid)
{
    MG_TRY()

    //Make sure this object isn't read-only
    if (Protected())
    {
        //can't change a read-only object
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.SetEllipsoid", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumProtectedException", NULL);
    }

    //Make sure it's a legal datum name
    if (!IsLegalMentorName(sEllipsoid.c_str()))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDatum.SetEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert wchar_t* to C++ string
    char *pStr = Convert_Wide_To_UTF8(sEllipsoid.c_str()); //need to delete [] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.SetEllipsoid", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    assert(strlen(pStr) < sizeof(m_DtDef.key_nm));

    //Set the ellipsoid name
    memset(&m_DtDef.ell_knm[0], 0, sizeof(m_DtDef.ell_knm));
    strcpy(&m_DtDef.ell_knm[0], pStr);

    memset(&m_datum.ell_knm[0], 0, sizeof(m_datum.ell_knm));
    strcpy(&m_datum.ell_knm[0], pStr);

    memset(&m_ElDef.key_nm[0], 0, sizeof(m_ElDef.key_nm));
    strcpy(&m_ElDef.key_nm[0], pStr);

    delete [] pStr;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.SetEllipsoid")
}

//validates the datum without caring about the ellipsoid
//Validity, in this case, is defined in
//terms of whether a catalog will accept this def when attempting to add it.
//Unfortunately, unlike coordinate systems (for which Mentor provides the
//CS_cschk function to test for mathematical validity), Mentor doesn't
//provide any testing for us.
//
//If this function returns false, then attempting to add the def to a
//catalog will result in failure.
//
bool CCoordinateSystemDatum::IsDatumValid()
{
    bool bIsValid=false;

    MG_TRY()

    //Check for mathematical integrity
    if (    !IsLegalDatumOffset(m_datum.delta_X)
        ||    !IsLegalDatumOffset(m_datum.delta_Y)
        ||    !IsLegalDatumOffset(m_datum.delta_Z)
        ||    !IsLegalDatumRotation(m_datum.rot_X)
        ||    !IsLegalDatumRotation(m_datum.rot_Y)
        ||    !IsLegalDatumRotation(m_datum.rot_Z)
        ||    !::IsLegalBwScale(m_datum.bwscale))
    {
        //Nope, not legal.
        return bIsValid;
    }
    //TODO:  add check on to84_via

    //Make sure the name is legal
    bIsValid = IsLegalMentorName(m_datum.key_nm);
    if (!bIsValid) return bIsValid;

    //Make sure the ellipsoid name is legal
    bIsValid = IsLegalMentorName(m_datum.ell_knm);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.IsDatumValid")

    return bIsValid;
}

//needs to be called after the datum parameters have been fully specified
//if we start with the ellipsoid the method will fail because it needs the datum parameters
//this is not just setting an internal pointer.
void CCoordinateSystemDatum::SetEllipsoidDefinition(MgCoordinateSystemEllipsoid *pEllipsoidDef)
{
    MG_TRY()

    assert(NULL != pEllipsoidDef);
    CHECKARGUMENTNULL(pEllipsoidDef, L"MgCoordinateSystemDatum.SetEllipsoidDefinition");

    STRING sEllipsoid=pEllipsoidDef->GetElCode();

    //Make sure it's a legal datum name
    if (!IsLegalMentorName(sEllipsoid.c_str()))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDatum.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert wchar_t* to C++ string
    char *pStr = Convert_Wide_To_UTF8(sEllipsoid.c_str()); //need to delete [] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    assert(strlen(pStr) < sizeof(m_DtDef.key_nm));

    //Set the ellipsoid name
    memset(&m_DtDef.ell_knm[0], 0, sizeof(m_DtDef.ell_knm));
    strcpy(&m_DtDef.ell_knm[0], pStr);

    memset(&m_datum.ell_knm[0], 0, sizeof(m_datum.ell_knm));
    strcpy(&m_datum.ell_knm[0], pStr);

    memset(&m_ElDef.key_nm[0], 0, sizeof(m_ElDef.key_nm));
    strcpy(&m_ElDef.key_nm[0], pStr);

    delete [] pStr;

    //Grab the definitions of the datum and ellipsoid.
    cs_Eldef_ eldef;
    bool bResult = BuildElDefFromInterface(pEllipsoidDef, eldef);
    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make sure the datum's specified ellipsoid matches the
    //ellipsoid's specified key name.
    //
    //We have to run the names thru Mentor's legalization
    //routine first, since they might contain "defaultable"
    //characters (see definition of cs_DFLT_IDNTBEG in cs_map.h)
    if ((0 != CS_nampp(m_DtDef.ell_knm))
        || (0 != CS_nampp(eldef.key_nm))
        || (0 != CS_stricmp(m_DtDef.ell_knm, eldef.key_nm)))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumMismatchException", NULL);
    }

    //Build a cs_Datum_ from the datum and ellipsoid definitions
    CriticalClass.Enter();
    cs_Datum_ *pDatum = CSdtloc2(&m_DtDef, &eldef);
    CriticalClass.Leave();
    if (NULL == pDatum)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.SetEllipsoidDefinition", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumInternalException", NULL);
    }

    //Okay, everything succeeded.  Hang on to a copy of the
    //source defs so we can serve them up again if we're asked.
    memset(&m_datum, 0, sizeof(m_datum));
    memset(&m_ElDef, 0, sizeof(m_ElDef));
    m_datum = *pDatum;
    CS_free(pDatum);

    m_ElDef=eldef;

    //Return success.
    assert(IsInitialized());

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.SetEllipsoidDefinition")
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
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetGeodeticTransformations", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pColl = new MgDisposableCollection;

    if (NULL == pColl.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.GetGeodeticTransformations", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pColl->Add(pNew);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.GetGeodeticTransformations")

    return pColl.Detach();
}

//------------------ Private functions --------------------------------

//Sets the specified string destination to the specified source
//string.  Fails if the source string isn't legal.
//
void CCoordinateSystemDatum::SetString(CREFSTRING sSrc, char *pDest, UINT32 nMaxSize)
{
    MG_TRY()
    assert(NULL != pDest);
    CHECKARGUMENTNULL(pDest, L"MgCoordinateSystemDatum.SetString");

    if (Protected())
    {
        //Can't modify a read-only object.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemDatum.SetString", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumProtectedException", NULL);
    }

    if (!IsLegalString(sSrc.c_str(), nMaxSize))
    {
        //Can't set string, caller gave us an illegal value
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDatum.SetString", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert to a C++ string, for Mentor's sake
    char *pStr = Convert_Wide_To_UTF8(sSrc.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemDatum.SetString", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy it in
    memset(pDest, 0, nMaxSize);
    strncpy(pDest, pStr, nMaxSize);

    //Clean up and return success.
    delete [] pStr;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.SetString")
}

//Private function that evaluates the protect data member of m_def
//to decide whether this object is protected or not.
//
bool CCoordinateSystemDatum::Protected() const
{
    return (1 == m_DtDef.protect);
}

//*****************************************************************************
MgCoordinateSystemCatalog* CCoordinateSystemDatum::GetCatalog()
{
    return SAFE_ADDREF(m_pCatalog.p);
}

//*****************************************************************************
UINT8* CCoordinateSystemDatum::SerializeFrom(UINT8* pStream)
{
    UINT8* pStreamOut=pStream;

    MG_TRY()
    assert(NULL != pStream);
    if (!pStream)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDatum.SerializeFrom", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    UINT8 nVersion=pStreamOut[0];

    if (kDtRelease0==nVersion)
    {
        pStreamOut++;

        //Read the def from the stream
        cs_Dtdef_ def;
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
        cs_Dtdef_ prevDef = m_DtDef;
        m_DtDef = def;
        CS_stncp (m_datum.key_nm,def.key_nm,sizeof (m_datum.key_nm));
        CS_stncp (m_datum.ell_knm,def.ell_knm,sizeof (m_datum.ell_knm));
        CS_stncp (m_datum.dt_name,def.name,sizeof (m_datum.dt_name));
        m_datum.delta_X = def.delta_X;
        m_datum.delta_Y = def.delta_Y;
        m_datum.delta_Z = def.delta_Z;
        m_datum.rot_X = def.rot_X;
        m_datum.rot_Y = def.rot_Y;
        m_datum.rot_Z = def.rot_Z;
        m_datum.bwscale = def.bwscale;
        m_datum.to84_via = def.to84_via;

        m_bEncrypted = bEncrypted;
        if (!IsDatumValid())
        {
            m_bEncrypted = bPrevEncrypted;
            m_DtDef = prevDef;
            throw new MgInvalidArgumentException(L"MgCoordinateSystemDatum.SerializeFrom", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.SerializeFrom")
    return pStreamOut;
}

//*****************************************************************************
UINT8* CCoordinateSystemDatum::SerializeTo(UINT8* pStream)
{
    UINT8* pStreamOut=pStream;

    MG_TRY()
    assert(NULL != pStream);
    if (!pStream)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemDatum.SerializeTo", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //save the version
    pStreamOut[0]=kDtRelease0;
    pStreamOut++;

    cs_Dtdef_ def = m_DtDef;
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

    MG_CATCH_AND_THROW(L"MgCoordinateSystemDatum.SerializeTo")
    return pStreamOut;
}

//*****************************************************************************
UINT32 CCoordinateSystemDatum::GetSizeSerialized()
{
    //size of the structure and the verison number
    size_t size=sizeof(m_DtDef)+sizeof(UINT8);
    return static_cast<UINT32>(size);
}
