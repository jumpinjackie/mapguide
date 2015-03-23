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
//file mentorutil.h
//
// This file contains the declarations of various utility functions
// to support the public API.  The functions herein are Mentor-related.  General,
// non-Mentor-related functions can be found in csutil.h.

#ifndef MG_MENTORUTIL_H
#define MG_MENTORUTIL_H
#pragma once

//Forward declarations
struct cs_Eldef_;
struct cs_Dtdef_;
struct cs_Csdef_;
struct cs_Ctdef_;
struct cs_Datum_;
struct cs_Csprm_;
struct cs_Dtcprm_;

//Constants dependent on Mentor
//MENTOR_MAINTENANCE
//Values defined in CSdata.c
const INT32 knCsNumParams = 24;                     //number of params in a cs_Csdef_ projection, max is cs_PRMBMP_PRJPRM24
const double kdMaxDatumOffset = 50000.0;            //meters; from CSdtcomp() and also cs_DelMax
const double kdMaxDatumRotation = 50000.0;          //arcseconds; from CSdtcomp() and also cs_RotMax
const double kdMinBwScale = -2000.0;                //ppm; from CSdtcomp() and also cs_SclMax
const double kdMaxBwScale = 2000.0;                 //ppm; from CSdtcomp() and also cs_SclMax
const double kdMinEllipsoidRadius = 6000000.0;      //meters; from CSelcomp() also cs_ERadMin
const double kdMaxEllipsoidRadius = 7000000.0;      //meters; from CSelcomp() also cs_ERadMax
// ADSK-HW 02/10/2003 checkout these two values when a new mentor comes
// vlaues are hard coded in CS_elio.c in the test on my_flat
const double kdMinEllipsoidFlat = 0.0000;           //from CSelcomp()
const double kdMaxEllipsoidFlat = 0.0040;           //from CSelcomp(), WARNING: also harcoded in CS_elio.c


//TODO:  Note the commented-out value for kdMinEllipsoidFlat, above.
//I need to investigate this further.  I originally set a minimum
//allowed flatness of 0.0032, thinking this would be adequate since
//that's what CSelcomp() enforces.  However, the BPCNC "ellipsoid"
//is, in fact, a sphere, with flatness zero, which violates this
//and causes problems.  So for now, I'm lowering the allowed minimum
//flatness to allow spheres.  The "TODO" here is further research only.


//Utility functions which return pointers to Mentor struct
//descriptions.  This is only necessary because this data
//member has different names in the different structs.
//
const char * ElDesc(const cs_Eldef_& def);
const char * DtDesc(const cs_Dtdef_& def);
const char * DtDesc05(const struct cs_Dtdef05_& def);
const char * DtDesc06(const struct cs_Dtdef06_& def);
const char * CsDesc(const cs_Csdef_& def);
const char * CsDesc05(const struct cs_Csdef05_& def);
const char * CsDesc06(const struct cs_Csdef06_& def);

const char * DtKey(const cs_Dtdef_& def);
const char * DtKey05(const cs_Dtdef05_& def);
const char * DtKey06(const cs_Dtdef06_& def);
const char * CsKey(const cs_Csdef_& def);
const char * CsKey05(const cs_Csdef05_& def);
const char * CsKey06(const cs_Csdef06_& def);
const char * ElKey(const cs_Eldef_& def);

//Returns whether the specified string is legal as a Mentor
//key name (for coordinate system, datum, or ellipsoid).
bool IsLegalMentorName(const wchar_t* kpStr);
bool IsLegalMentorName(const char *kpStr);
bool MakeLegalMentorName(char *kpStr);

//The following functions return whether the specified
//projection uses a particular data member of cs_Csdef_.
bool ProjectionUsesOrgLng(INT32 prj);
bool ProjectionUsesOrgLat(INT32 prj);
bool ProjectionUsesSclRed(INT32 prj);
bool ProjectionUsesQuad(INT32 prj);
bool ProjectionUsesOffset(INT32 prj);

STRING ProjectionDescriptionFromString(const char *kpStr);

//Returns the INT32 associated with a given projection
//key name.  Returns kPrjUnknown if it's not a recognized string.
INT32 ProjectionFromString(const char *kpStr);

//Returns the projection key name associated with a given
//INT32 value.
const char * StringFromProjection(INT32 prj);

// Returns true if th indictaed project, via projection key name
// or projection code, is of the non-earth referenced type.  That
// is, the projection type is either NERTH or NRTHSRT.
bool ProjectionIsNerthType (const char *kpStr);
bool ProjectionIsNerthType (INT32 prjCode);


//Returns the MgCoordinateSystemUnitCode value associated with a given unit
//key name.  Returns MgCoordinateSystemUnitCode::Unknown if it's not a recognized string.
INT32 UnitFromString(const char *kpStr);

//Returns the unit key name associated with a given
//Unit value code defined in MgCoordinateSystemUnitCode
const char * StringFromUnit(INT32 unit);


//Gets information on the specified unit type.  Null pointers
//may be passed in if the caller's not interested in a particular
//bit of information.  Function returns true for success or false
//for failure (meaning that that particular unit type couldn't
//be found).
//Unit type values are defined in MgCoordinateSystemUnit
bool GetUnitInfo(INT32 unit, INT32 *pUnitType, double *pdScale);


//Gets the unit type which the specified projection uses.
INT32 GetProjectionUnitType(INT32 prj);


//Returns whether the specified offset in meters is legal for a datum
bool IsLegalDatumOffset(double dMeters);


//Returns whether the specified Bursa-Wolfe rotation in arcseconds
//is legal for a datum
bool IsLegalDatumRotation(double dArcSeconds);


//Returns whether the specified Bursa-Wolfe scale factor
//is legal for a datum.  Value is in parts per million, i.e.
//a value of 1.000012 would equate to dBwScale = 12.0
bool IsLegalBwScale(double dBwScale);


//Returns whether the specified radius in meters is legal for an
//ellipsoid.
bool IsLegalEllipsoidRadius(double dMeters);


//Returns whether the specified flattening ratio is within the
//legal range for an ellipsoid.
bool IsLegalFlatteningRatio(double dFlat);


//Calculates a flattening ratio, given an equatorial and a polar radius.
double FlatteningRatio(double dEquatorial, double dPolar);


//Calculates a flattening ratio, given an eccentricity.
double FlatteningRatio(double dEccentricity);


//Calculates an eccentricity, given a flattening ratio
double Eccentricity(double dFlat);



//Returns whether the specified datum conversion technique is
//one that Mentor knows about.
bool IsLegalGeodeticTransformationMethod(INT32 geodeticTransformationMethod);

//Builds a cs_Csdef_ struct from an MgCoordinateSystem interface.
bool BuildCsDefFromInterface(MgCoordinateSystem *pSrc, cs_Csdef_& def);

//Builds a cs_Dtdef_ struct from an MgCoordinateSystemDatum interface.
bool BuildDtDefFromInterface(MgCoordinateSystemDatum *pSrc, cs_Dtdef_& def);

//Builds a cs_Eldef_ struct from an MgCoordinateSystemEllipsoid interface.
bool BuildElDefFromInterface(MgCoordinateSystemEllipsoid *pSrc, cs_Eldef_& def);

//Builds an MgCoordinateSystem interface from a cs_Csdef_ struct.
//Caller is responsible for freeing the returned pointer via Release().
MgCoordinateSystem * BuildInterfaceFromCsDef(const cs_Csdef_& def, MgCoordinateSystemCatalog* pCatalog);

//Builds an MgCoordinateSystemDatum interface from a cs_Dtdef_ struct.
//Caller is responsible for freeing the returned pointer via Release().
MgCoordinateSystemDatum * BuildInterfaceFromDtDef(const cs_Dtdef_& def, MgCoordinateSystemCatalog* pCatalog);

//Builds an MgCoordinateSystemEllipsoid interface from a cs_Eldef_ struct.
//Caller is responsible for freeing the returned pointer via Release().
MgCoordinateSystemEllipsoid * BuildInterfaceFromElDef(const cs_Eldef_& def, MgCoordinateSystemCatalog* pCatalog);

//Projection Table Look Up
struct cs_Prjtab_* GetMentorProjectionObject(const char* prjKeyName);

//Builds a cs_Datum_ struct from an MgCoordinateSystemDatum interface.
bool BuildDatumFromInterface(MgCoordinateSystemDatum *pSrc, cs_Datum_& datum);

//Builds a cs_Csprm_ struct from an MgCoordinateSystem interface.
bool BuildCsprmFromInterface(MgCoordinateSystem *pSrc, cs_Csprm_& csprm);

//caller must free memory if not NULL using CS_free
bool BuildDefsFromInterface(MgCoordinateSystem *pSrc, cs_Csdef_*& pCsDef, cs_Dtdef_*& pDtDef, cs_Eldef_*& pElDef);

//Builds a cs_Csprm_ struct from a cs_Csdef_ structure.
bool BuildCsprmFromArbitraryDef(const cs_Csdef_& def, cs_Csprm_& csprm);

//Returns whether the specified def is geodetic (as opposed to cartographic).
bool CsdefIsGeodetic(const cs_Csdef_& def);

//Shifts a lat/long point from one datum to another.  Returns eOk
//for success, eError for total failure, or eWarning for partial success.
INT32 GeodeticTransformationPoint(cs_Dtcprm_ *pDtcprm, double& dLongitude, double& dLatitude, double *pZ);

//utility method to be used by any CS dictionary implementation that has a SetFileName() method in it
void SetDictionaryFileName(CREFSTRING sFileName, CREFSTRING dictionaryPath, INT32& magicNumber,
                           CsDictionaryOpenMode (*MagicNumberCallback)(long),
                           void (*FileNameTarget)(const char* newTargetName),
                           const wchar_t* context);

//Opens a Mentor dictionary, verifies magic value, and positions read
//pointer at start of first record.  Returns true for success, false
//for failure.
csFILE* OpenDictionaryFile(
    const wchar_t *kpFileName,
    char* szMode,
    INT32& lMagic,
    CsDictionaryOpenMode (*ValidMagic)(long));

bool GetMagicFromDictionaryFile(
    const wchar_t *kpFileName,
    INT32& lMagic,
    CsDictionaryOpenMode (*ValidMagic)(long));

//Returns the current "Mentor time" in days since January 1, 1990.
INT16 MentorTime();


//Returns whether a given protect flag value specifies that the object
//is truly protected, taking into account the current date, the current
//state of Mentor global variables, etc.  "Is this object currently
//modifiable, according to Mentor?"
bool IsReallyProtected(INT16 sProtect);


//Decrypts an encrypted character string, using Mentor's algorithm
void DecryptBuffer(char *pBuf, unsigned char ucKey, INT32 nBufSize);


//Does a Mentor-6-style case insensitive comparison of two strings
//(i.e. '_' > letters).  Note:  may modify input buffers.
int Mentor6Strnicmp(char *pBuf1, char *pBuf2, INT32 nLen);

//reads a const char* and returns the STRING class object for it
STRING MentorReadString(const char* mentorString = NULL);

//writes a string into the destination char buffer specified through pDest; this method has no idea
//where the target buffer is, i.e. the caller must have checked the target item's protection
//level first; usually [Mg*Item::IsProtected()]
void MentorSetString(CREFSTRING sSrc, char *pDest, UINT32 nMaxSize);

//Macro which defines comparison function, suitable for use with CS_bins,
//which compares two Mentor structs a la CS_cscmp() et al.
//Doing this as a macro is incredibly lame, and I really *really* would
//have preferred to do this as a template instead, but no go:  the
//template argument (structtype) doesn't appear in the function's
//signature, and Microsoft's !@$&*#@% compiler can't distinguish
//between differing versions and uses only one copy of the template
//function.
//
//Previously, the macro didn't take a cmpfunc argument; it just used
//_tcsnicmp().  However, it turns out that Mentor 6 and Mentor 8 have
//differing conventions for doing case-insensitive comparisions: one
//shifts to uppercase, one shifts for lowercase.  This causes opposite
//behavior in comparing underscores with letters, since _'s ASCII value
//falls between that of the uppercase and that of the lowercase letters.
//This, in turn, screws up binary searching.  To solve the problem, I
//let the caller specify the name of the comparison function to use,
//thus maintaining version safety.
//
typedef int (*TCmpFunc)(const void *, const void *);
#define DECLARE_MENTOR_COMPARISON_FUNCTION(funcname, structtype, cmpfunc) \
int funcname(const void *p1, const void *p2) \
{ \
    const structtype *kpDef1 = reinterpret_cast<const structtype *>(p1); \
    const structtype *kpDef2 = reinterpret_cast<const structtype *>(p2); \
    \
    char szBuf1[sizeof(kpDef1->key_nm)], szBuf2[sizeof(kpDef2->key_nm)]; \
    memcpy(szBuf1, kpDef1->key_nm, sizeof(kpDef1->key_nm)); \
    memcpy(szBuf2, kpDef2->key_nm, sizeof(kpDef2->key_nm)); \
    \
    DecryptBuffer(szBuf1, kpDef1->fill[0], sizeof(kpDef1->key_nm)); \
    DecryptBuffer(szBuf2, kpDef2->fill[0], sizeof(kpDef2->key_nm)); \
    return cmpfunc(szBuf1, szBuf2, sizeof(kpDef1->key_nm)); \
}


// Each of these three functions compares two defs and returns true
// if they're "mathematically same," false if not.  Non-mathematical
// attributes, such as description, are ignored.
bool MathematicallySameDef(
    MgGuardDisposable *pDef1,
    MgGuardDisposable *pDef2,
    bool *pbSame);
bool MathematicallySameCoordinateSystemDef(
    MgCoordinateSystem *pDef1,
    MgCoordinateSystem *pDef2,
    bool *pbSame);
bool MathematicallySameDatumDef(
    MgCoordinateSystemDatum *pDef1,
    MgCoordinateSystemDatum *pDef2,
    bool *pbSame);
bool MathematicallySameEllipsoidDef(
    MgCoordinateSystemEllipsoid *pDef1,
    MgCoordinateSystemEllipsoid *pDef2,
    bool *pbSame);

//MENTOR_MAINTENANCE
//Units.
//
//Values are picked up from CSdataU.c in the array cs_Unittab
//Check also CsUiLabelInfo.cpp, CsUnitInfo.h and mentorutil.cpp
//
//check the public class MgCoordinateSystemUnitCode for the values


//MENTOR_MAINTENANCE
//Projections.  Numeric values are ported from Mentor.
//Numeric values come from in cs_map.h, those prefixed with cs_PRJCOD_
//Text string values in comment come from CSdataPJ.c
//can be used in by the UI to be displayed to the user
//In CSdataPJ.c at the same place, in the array of structures cs_Prjtab
//we can also find the projection name that appears in the
//coordsys.asc file in the field "PROJ: "
//
//check the public class MgCoordinateSystemProjectionCode for the values


//MENTOR_MAINTENANCE
//Datum conversion techniques.  Numeric values are ported from Mentor.
//Numeric values come from cs_map.h, those prefixed with cs_DTCTYP_
//Seems also duplicated in the enum cs_DtcXformType
//String values in comment come from CS_guiApi.c
//can be used by the UI to be displayed to the user
//In the file CSdtcomp.c, the array of structures cs_DtTypeT
//lists the name of the "datum types" (or conversion techniques)
//used in the file datum.asc in the filed "USE: "
//
//check the public class MgCoordinateSystemGeodeticTransformationMethod for the values

//MENTOR_MAINTENANCE
//Projection parameter logical types.  Numeric values are ported from Mentor.
//values in cs_map.h cs_PRMLTYP_xxx
//refer to the class MgCoordinateSystemProjectionLogicalType for the values

//MENTOR_MAINTENANCE
//Projection parameter format types.  Numeric values are ported from Mentor.
//These can be used by user interfaces which want to do formatting operations
//between floating-point values and string representations thereof.
//Check the values with the one in in Mentor in CSdataPJ.c prefixed with cs_FRMT_
//The values actually changed between 11.1 and 11.5 for some reason
//and were incremented by 2
//refer to the class MgCoordinateSystemProjectionFormatType for the values

//MENTOR_MAINTENANCE
//Projection parameter types.  Numeric values are ported from Mentor
//in cs_map.h prefixed cs_PRMCOD_
//These can be used by user interfaces when deciding what strings to
//show in user interfaces.
//Strings comes from Mentor in CSdataPJ.c in
//struct cs_Prjprm_ csPrjprm [] =
//refer to the class MgCoordinateSystemProjectionParameterType for the values

#endif //MG_MENTORUTIL_H
