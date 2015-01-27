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
//file mentorutil.cpp
//
// This file contains the declarations of various utility functions
// to support the API.  The functions herein are Mentor-related.  General,
// non-Mentor-related functions can be found in csutil.h.


#include <assert.h>
#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CriticalSection.h"
#include "CoordSysUtil.h"           //for Convert_Wide_To_UTF8, CsDictionaryOpenMode
#include "MentorDictionary.h"
#ifdef _WIN32
#include <io.h>                        //for _dup()
#endif

#include "CoordSys.h"               //for CCoordinateSystem
#include "CoordSysDatum.h"          //for CCoordinateSystemDatum
#include "CoordSysEllipsoid.h"      //for CCoordinateSystemEllipsoid
#include "CoordSysEnum.h"           //for CCoordinateSystemEnum
#include "CoordSysDictionary.h"     //for CCoordinateSystemDictionary

#include <cs_Legacy.h>

using namespace CSLibrary;

//Externs from Mentor
extern "C"
{
    extern struct cs_Prjtab_ cs_Prjtab[];
    extern short cs_Protect;
}



//These three global variables aren't actually used for anything.
//They're just declared so that I can use sizeof() on their various
//string members to establish legal array sizes.  I'm reduced to
//doing this because Mentor declares these structs with hard-coded
//numbers for the array sizes rather than using named constants
//like a good little library.
static cs_Csdef_ g_cs;
static cs_Dtdef_ g_dt;
static cs_Eldef_ g_el;



//Utility functions which return pointers to Mentor struct
//descriptions.  This is only necessary because this data
//member has different names in the different structs.
//
const char * ElDesc(const cs_Eldef_& def) { return def.name; }
const char * DtDesc(const cs_Dtdef_& def) { return def.name; }
const char * DtDesc05(const struct cs_Dtdef05_& def) { return def.name; }
const char * DtDesc06(const struct cs_Dtdef06_& def) { return def.name; }
const char * CsDesc(const cs_Csdef_& def) { return def.desc_nm; }
const char * CsDesc05(const struct cs_Csdef05_& def) { return def.desc_nm; }
const char * CsDesc06(const struct cs_Csdef06_& def) { return def.desc_nm; }

const char * DtKey(const cs_Dtdef_& def) { return def.key_nm; }
const char * DtKey05(const cs_Dtdef05_& def) { return def.key_nm; }
const char * DtKey06(const cs_Dtdef06_& def) { return def.key_nm; }
const char * CsKey(const cs_Csdef_& def) { return def.key_nm; }
const char * CsKey05(const cs_Csdef05_& def) { return def.key_nm; }
const char * CsKey06(const cs_Csdef06_& def) { return def.key_nm; }
const char * ElKey(const cs_Eldef_& def) { return def.key_nm; }

//MENTOR_MAINTENANCE
//The public API needs a programmatic way for clients to ask whether a
//particular projection uses any or all of the following:
//
//    org_lng    (origin longitude)
//    org_lat    (origin latitude)
//    scl_red    (scale reduction)
//    quad    (quadrant)
//
//Unfortunately, Mentor itself provides no such programmatic
//mechanism, so we have to do this the hard way.  The following
//table was built by laboriously working through the printed
//Mentor manual and noting what each projection uses.
//
//Note:  The number of records in the table, and the order thereof,
//are unimportant, but the last entry must be MgCoordinateSystemProjectionCode::Unknown;
//that's how the end of the table is recognized.
//
//I could not find any information in mentor documentation
//The values of the flags are defined in the file cs_map.h.
//The flag combination for each projection
//is using these flags in the file CSdataPJ.c
//Also, Quad is valid for all coordinate systems
//
struct CCsPrjAdditionalInfo
{
    INT32 prj;
    bool bUsesOrgLng;
    bool bUsesOrgLat;
    bool bUsesSclRed;
    bool bUsesQuad;
    bool bUsesOffset;
};

static const CCsPrjAdditionalInfo s_prjInfo[] = {
//      projtype                                      org_lng  org_lat  scl_red   quad     offset
    { MgCoordinateSystemProjectionCode::Alber,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Azede,        true,    true,    false,    true,    true},
    { MgCoordinateSystemProjectionCode::Azmea,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Azmed,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Bipolar,      false,   false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Bonne,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Cassini,      false,   true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Eckert4,      true,    false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Eckert6,      true,    false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Edcnc,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Edcyl,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::GaussK,       false,   true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Gnomonic,     true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Goode,        true,    false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Hom1uv,       false,   false,   true,     true,    true },
    { MgCoordinateSystemProjectionCode::Hom1xy,       false,   false,   true,     true,    true },
    { MgCoordinateSystemProjectionCode::Hom2uv,       false,   true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Hom2xy,       false,   true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Krovak,       true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Krvk95,       true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::LL,           true,    false,   false,    false,   false },
    { MgCoordinateSystemProjectionCode::Lm1sp,        true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Lm2sp,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Lmblg,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Lmtan,        true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Miller,       false,   false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Mndotl,       true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Mndott,       false,   true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Modpc,        false,   false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Mollweid,     true,    false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Mrcat,        false,   false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::MrcatK,       false,   false,   true,     true,    true },
    { MgCoordinateSystemProjectionCode::Mstero,       true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Neacyl,       true,    false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Nerth,        false,   false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Nzealand,     true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::OblqM,        false,   false,   false,    false,   false }, //obsolete!
    { MgCoordinateSystemProjectionCode::Obqcyl,       true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Ortho,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Ostn02,       false,   false,   false,    true,    false },
    { MgCoordinateSystemProjectionCode::Ostn97,       false,   false,   false,    true,    false },
    { MgCoordinateSystemProjectionCode::Ostro,        true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Plycn,        false,   true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Pstro,        true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Pstrosl,      true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Rskew,        false,   false,   true,     true,    true },
    { MgCoordinateSystemProjectionCode::Rskewc,       false,   false,   true,     true,    true },
    { MgCoordinateSystemProjectionCode::Rskewo,       false,   false,   true,     true,    true },
    { MgCoordinateSystemProjectionCode::Robinson,     true,    false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Sinus,        true,    false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Sotrm,        false,   true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Sstro,        true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Swiss,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Sys34,        false,   false,   false,    true,    false },
    { MgCoordinateSystemProjectionCode::Sys34_99,     false,   false,   false,    true,    false },
    { MgCoordinateSystemProjectionCode::Teacyl,       true,    true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Tm,           false,   true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Trmeraf,      false,   true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Trmrkrg,      false,   true,    true,     true,    true},
    { MgCoordinateSystemProjectionCode::Trmrs,        false,   true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Vdgrntn,      true,    false,   false,    true,    true },
    { MgCoordinateSystemProjectionCode::Wccsl,        true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Wccst,        false,   true,    true,     true,    true },
    { MgCoordinateSystemProjectionCode::Utm,          false,   false,   false,    true,    false },
    { MgCoordinateSystemProjectionCode::Winkl,        true,    false,   false,    true,    true},
    { MgCoordinateSystemProjectionCode::Nrthsrt,      false,   false,   false,    true,    true},
    { MgCoordinateSystemProjectionCode::Lmbrtaf,      true,    true,    false,    true,    true},
    { MgCoordinateSystemProjectionCode::Sys34_01,     false,   false,   false,    true,    false },
    { MgCoordinateSystemProjectionCode::EdcylE,       true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::PlateCarree,  true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::PvMercator,   false,   true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::LmMich,       true,    true,    false,    true,    true },
    { MgCoordinateSystemProjectionCode::Unknown,      false,   false,   false,    false,   false }
};

//Returns whether the specified string is legal as a Mentor
//key name (for coordinate system, datum, or ellipsoid).
//
bool IsLegalMentorName(const wchar_t* kpStr)
{
    //(Note:  This function makes the assumption that the length
    //limit is the same for all three.  That's true currently,
    //but could change in the future. A couple of
    //asserts try to catch that sort of thing.
    assert(sizeof(g_cs.key_nm) == sizeof(g_dt.key_nm));
    assert(sizeof(g_cs.key_nm) == sizeof(g_el.key_nm));

    //null pointer is obviously not legal...
    if (NULL == kpStr) return false;

    //Convert to a C++ string, for Mentor's sake
    size_t nChars = wcstombs(NULL, kpStr, wcslen(kpStr)) + 1;

    //Make sure we didn't encounter any illegal characters
    if (-1 == nChars) return false;

    //Make sure the string's not too long
    if (nChars > sizeof(g_cs.key_nm)) return false;

    //Convert to regular characters
    char szBuf[sizeof(g_cs.key_nm)];
    wcstombs(szBuf, kpStr, nChars);
    szBuf[nChars-1] = '\0';

    return IsLegalMentorName(szBuf);
}

//Returns whether the specified string is legal as a Mentor
//key name (for coordinate system, datum, or ellipsoid).
//
bool IsLegalMentorName(const char *kpStr)
{
    //(Note:  This function makes the assumption that the length
    //limit is the same for all three.  That's true currently,
    //but could conceivably change in the future.
    //For now, a couple of asserts try to catch that sort of thing.
    assert(sizeof(g_cs.key_nm) == sizeof(g_dt.key_nm));
    assert(sizeof(g_cs.key_nm) == sizeof(g_el.key_nm));

    //null pointer is obviously not legal...
    if (NULL == kpStr) return false;

    //Make sure the string's not too long
    if (strlen(kpStr) >= sizeof(g_cs.key_nm)) return false;

    //Find out whether Mentor thinks it's legal.  Unfortunately
    //we have to make a copy of our string, since CS_nampp is
    //designed in such a way that it can change the contents.
    char szBuf[sizeof(g_cs.key_nm)];
    strcpy(szBuf, kpStr);
    int nResult = CS_nampp(szBuf);
    return (0 == nResult);
}

extern "C" char cs_Nmchset [];
extern "C" char cs_Unique;

bool MakeLegalMentorName(char *kpStr)
{
    //null pointer is obviously not legal...
    if (NULL == kpStr) return false;

    char szNameTmp[cs_KEYNM_DEF];
    size_t maxLength=strlen(kpStr);
    if (maxLength==0)
    {
        return false;
    }
    if (maxLength>cs_KEYNM_DEF-1)
    {
        maxLength=cs_KEYNM_DEF-1;
    }

    //Remove leading and trailing white space while copying name to szNameTmp.
    char *pName=kpStr;
    while (*pName == ' ') pName += 1;
    char *pEnd = CS_stncp (szNameTmp, pName, sizeof (szNameTmp));

    /* We now know that there is at least one non-space
       character in the string.  Thus the following does
       not need a pointer comparison which we like to avoid. */

    while (*(pEnd - 1) == ' ') pEnd -= 1;
    *pEnd = '\0';

    /* If the name is surrounded by either set of default
       characters, remove them. */

    pName = szNameTmp;
    pEnd = szNameTmp + strlen (pName) - 1;
    if ((*pName == cs_DFLT_IDNTBEG && *pEnd == cs_DFLT_IDNTEND) ||
        (*pName == cs_DFLT_REPLBEG && *pEnd == cs_DFLT_REPLEND)
       )
    {
        *pEnd = '\0';            /* trims trailing */
        CS_stcpy (szNameTmp,&szNameTmp[1]);    /* trims leading */
    }

    /* Careful, szNameTmp could be the null string now if we were
       given a set of empty default wrappers. */
    if (*pName == '\0')
    {
        //make it valid with dummy one character name
        szNameTmp[0]='A';
        szNameTmp[1]='\0';
        CS_stncp (kpStr,szNameTmp, 2);
        return true;
    }

    int alphaCount;
    int uniqueCount;
    char cc;

    uniqueCount = alphaCount = 0;
    while (*pName != '\0')
    {
        cc = *pName++;
        if (cc == cs_Unique)
        {
            uniqueCount += 1;

            if (uniqueCount > 1)
            {
                /* If the cs_Unique feature is enabled, we only allow one
                   of them. */
                pName--;
                CS_stcpy (pName, &pName[1]);    /* trims more than 1 unique character */
                continue;
            }
        }
        if (cc >= '0' && cc <= '9') continue;
        if ((cc >= 'A' && cc <= 'Z') || (cc >= 'a' && cc <= 'z'))
        {
            alphaCount += 1;
            continue;
        }
        if (strchr (cs_Nmchset, cc) == NULL)
        {
            pName--;
            CS_stcpy (pName, &pName[1]);    /* trims illegal character */
            continue;
        }

        //remove also all internal spaces
        //single spaces are tolerated inside the name
        //we make up a new name just for validity sake so who cares about the spaces
        //it makes the algoryhtm cleaner
        if (cc == ' ')
        {
            pName--;
            CS_stcpy (pName, &pName[1]);    /* trims double space */
        }
    }

    size_t size = strlen (szNameTmp);
    if (size==0)
    {
        //empty string, so make it valid with a single character
        szNameTmp[0]='A';
        szNameTmp[1]='\0';
        CS_stncp (kpStr,szNameTmp,(int)(strlen(szNameTmp) + 1));
        return true;
    }

    if (alphaCount == 0)
    {
        // Must have at least on alphabetic in the name.
        //choose one letter to make it valid
        szNameTmp[0]='A';
    }

    /* If the result is longer than maxLength characters we
       have an illegal name. */
    if (size > maxLength)
    {
        szNameTmp[maxLength]='\0';
    }

    /* We use to require that a key name begin with an alphabetic (prior
       to release 9.01).  We now allow key names to begin with numerics
       providing that the first non-numeric character is indeed alphabetic.
       For the purposes of this particular test, the underscore character
       is considered alphabetic. */

    pName = szNameTmp;
    if (*pName == cs_Unique) pName += 1;
    while ((cc = *pName++) != '\0') if (cc < '0' || cc > '9') break;
    bool ok = (cc == '_')  || (cc == cs_Unique) || (cc >= 'A' && cc <= 'Z') || (cc >= 'a' && cc <= 'z');
    if (!ok)
    {
        pName--;
        CS_stcpy (pName, &pName[1]);    /* trims double space */
    }

    /* OK, the name has been processed, and is OK. */

    CS_stncp (kpStr,szNameTmp,(int)(strlen(szNameTmp) + 1));

    return true;
}

//Returns whether the specified projection uses the
//org_lng data member of cs_Csdef_.
//
bool ProjectionUsesOrgLng(INT32 prj)
{
    if (MgCoordinateSystemProjectionCode::Unknown == prj) return false;

    //Find the entry in the table
    const CCsPrjAdditionalInfo *kpInfo;
    for (kpInfo = s_prjInfo; kpInfo->prj != MgCoordinateSystemProjectionCode::Unknown; kpInfo++)
    {
        if (prj == kpInfo->prj)
        {
            //found it!
            return kpInfo->bUsesOrgLng;
        }
    }

    //We made it all the way through the table without finding
    //it, so return false.
    assert(0);
    return false;
}

//Returns whether the specified projection uses the
//org_lat data member of cs_Csdef_.
//
bool ProjectionUsesOrgLat(INT32 prj)
{
    if (MgCoordinateSystemProjectionCode::Unknown == prj) return false;

    //Find the entry in the table
    const CCsPrjAdditionalInfo *kpInfo;
    for (kpInfo = s_prjInfo; kpInfo->prj != MgCoordinateSystemProjectionCode::Unknown; kpInfo++)
    {
        if (prj == kpInfo->prj)
        {
            //found it!
            return kpInfo->bUsesOrgLat;
        }
    }

    //We made it all the way through the table without finding
    //it, so return false.
    assert(0);
    return false;
}

//Returns whether the specified projection uses the
//scl_red data member of cs_Csdef_.
//
bool ProjectionUsesSclRed(INT32 prj)
{
    if (MgCoordinateSystemProjectionCode::Unknown == prj) return false;

    //Find the entry in the table
    const CCsPrjAdditionalInfo *kpInfo;
    for (kpInfo = s_prjInfo; kpInfo->prj != MgCoordinateSystemProjectionCode::Unknown; kpInfo++)
    {
        if (prj == kpInfo->prj)
        {
            //found it!
            return kpInfo->bUsesSclRed;
        }
    }

    //We made it all the way through the table without finding
    //it, so return false.
    assert(0);
    return false;
}

//Returns whether the specified projection uses the
//quad data member of cs_Csdef_.
//
bool ProjectionUsesQuad(INT32 prj)
{
    if (MgCoordinateSystemProjectionCode::Unknown == prj) return false;

    //Find the entry in the table
    const CCsPrjAdditionalInfo *kpInfo;
    for (kpInfo = s_prjInfo; kpInfo->prj != MgCoordinateSystemProjectionCode::Unknown; kpInfo++)
    {
        if (prj == kpInfo->prj)
        {
            //found it!
            return kpInfo->bUsesQuad;
        }
    }

    //We made it all the way through the table without finding
    //it, so return false.
    assert(0);
    return false;
}

//Returns whether the specified projection uses the
//x_off and y_off data members of cs_Csdef_.
//
bool ProjectionUsesOffset(INT32 prj)
{
    if (MgCoordinateSystemProjectionCode::Unknown == prj) return false;

    //Find the entry in the table
    const CCsPrjAdditionalInfo *kpInfo;
    for (kpInfo = s_prjInfo; kpInfo->prj != MgCoordinateSystemProjectionCode::Unknown; kpInfo++)
    {
        if (prj == kpInfo->prj)
        {
            //found it!
            return kpInfo->bUsesOffset;
        }
    }

    //We made it all the way through the table without finding
    //it, so return false.
    assert(0);
    return false;
}

//Returns the projection description associated with a given projection
//key name.
//
STRING ProjectionDescriptionFromString(const char *kpStr)
{
    STRING projectionDescription;

    if (NULL != kpStr)
    {
        // Scan the entire table to find a match.
        for (int i = 0; cs_Prjtab[i].code != cs_PRJCOD_END; i++)
        {
            if (!CS_stricmp (kpStr, cs_Prjtab[i].key_nm))
            {
                // Found a match!
                wchar_t* pwszDesc = Convert_UTF8_To_Wide(cs_Prjtab[i].descr);

                if (NULL != pwszDesc)
                {
                    projectionDescription = pwszDesc;
                    delete[] pwszDesc;
                }

                break;
            }
        }
    }

    return projectionDescription;
}

//Returns the INT32 associated with a given projection
//key name.  Returns MgCoordinateSystemProjectionCode::Unknown if it's not a recognized string.
//
INT32 ProjectionFromString(const char *kpStr)
{
    if (NULL == kpStr) return MgCoordinateSystemProjectionCode::Unknown;

    for (int i=0; cs_Prjtab[i].code != cs_PRJCOD_END; i++)
    {
        if (0 == strcmp(kpStr, cs_Prjtab[i].key_nm))
        {
            //Found a match!
            return static_cast<INT32>(cs_Prjtab[i].code);
        }
    }

    //Scanned entire table without finding a match.
    return MgCoordinateSystemProjectionCode::Unknown;
}

//Returns the projection key name associated with a given
//INT32 value.
//
const char * StringFromProjection(INT32 prj)
{
    for (int i=0; cs_Prjtab[i].code != cs_PRJCOD_END; i++)
    {
        if (cs_Prjtab[i].code == prj)
        {
            //Found a match!
            return cs_Prjtab[i].key_nm;
        }
    }

    //Scanned entire table without finding a match.
    assert(0);
    return NULL;
}

// Returns true if th indictaed project, via projection key name
// or projection code, is of the non-earth referenced type.  That
// is, the projection type is either NERTH or NRTHSRT.
bool ProjectionIsNerthType (const char *kpStr)
{
    INT32 prjCode = ProjectionFromString (kpStr);
    return ProjectionIsNerthType (prjCode);
}
bool ProjectionIsNerthType (INT32 prjCode)
{
    bool isNerth = (prjCode == cs_PRJCOD_NERTH) || (prjCode == cs_PRJCOD_NRTHSRT);
    return isNerth;
}

struct UnitStrPair
{
    INT32 unit;
    char szName[sizeof(g_cs.unit)];
};

//The following array maps MgCoordinateSystemUnitCode values to their corresponding key
//strings, since there's no equivalent table in Mentor itself.  If
//new units are added to Mentor, this table will need to be updated
//appropriately for the public API to recognize the new units.
//MENTOR_MAINTENANCE
//
//The order of the array members is unimportant, but the final entry
//must have be MgCoordinateSystemUnitCode::Unknown-- that's how the end of the table
//is recognized.
//
//Values are picked up from CSdataU.c
//
static const UnitStrPair unitmap[] =
{
    { MgCoordinateSystemUnitCode::Meter, "Meter" },
    { MgCoordinateSystemUnitCode::Foot, "Foot" },
    { MgCoordinateSystemUnitCode::Inch, "Inch" },
    { MgCoordinateSystemUnitCode::IFoot, "IFoot" },
    { MgCoordinateSystemUnitCode::ClarkeFoot, "ClarkeFoot" },
    { MgCoordinateSystemUnitCode::SearsFoot, "SearsFoot" },
    { MgCoordinateSystemUnitCode::GoldCoastFoot, "GoldCoastFoot" },
    { MgCoordinateSystemUnitCode::IInch, "IInch" },
    { MgCoordinateSystemUnitCode::MicroInch, "MicroInch" },
    { MgCoordinateSystemUnitCode::Centimeter, "Centimeter" },
    { MgCoordinateSystemUnitCode::Kilometer, "Kilometer" },
    { MgCoordinateSystemUnitCode::Yard, "Yard" },
    { MgCoordinateSystemUnitCode::SearsYard, "SearsYard" },
    { MgCoordinateSystemUnitCode::IndianYard, "IndianYard" },
    { MgCoordinateSystemUnitCode::IndianFoot, "IndianFoot" },
    { MgCoordinateSystemUnitCode::IndianFt37, "IndianFt37" },
    { MgCoordinateSystemUnitCode::IndianFt62, "IndianFt62" },
    { MgCoordinateSystemUnitCode::IndianFt75, "IndianFt75" },
    { MgCoordinateSystemUnitCode::IndianYd37, "IndianYd37" },
    { MgCoordinateSystemUnitCode::Mile, "Mile" },
    { MgCoordinateSystemUnitCode::IYard, "IYard" },
    { MgCoordinateSystemUnitCode::IMile, "IMile" },
    { MgCoordinateSystemUnitCode::Knot, "Knot" },
    { MgCoordinateSystemUnitCode::NautM, "NautM" },
    { MgCoordinateSystemUnitCode::Lat66, "Lat-66" },
    { MgCoordinateSystemUnitCode::Lat83, "Lat-83" },
    { MgCoordinateSystemUnitCode::Decimeter, "Decimeter" },
    { MgCoordinateSystemUnitCode::Millimeter, "Millimeter" },
    { MgCoordinateSystemUnitCode::Dekameter, "Dekameter" },
    { MgCoordinateSystemUnitCode::Decameter, "Decameter" },
    { MgCoordinateSystemUnitCode::Hectometer, "Hectometer" },
    { MgCoordinateSystemUnitCode::GermanMeter, "GermanMeter" },
    { MgCoordinateSystemUnitCode::CaGrid, "CaGrid" },
    { MgCoordinateSystemUnitCode::ClarkeChain, "ClarkeChain" },
    { MgCoordinateSystemUnitCode::GunterChain, "GunterChain" },
    { MgCoordinateSystemUnitCode::BenoitChain, "BenoitChain" },
    { MgCoordinateSystemUnitCode::SearsChain, "SearsChain" },
    { MgCoordinateSystemUnitCode::ClarkeLink, "ClarkeLink" },
    { MgCoordinateSystemUnitCode::GunterLink, "GunterLink" },
    { MgCoordinateSystemUnitCode::BenoitLink, "BenoitLink" },
    { MgCoordinateSystemUnitCode::SearsLink, "SearsLink" },
    { MgCoordinateSystemUnitCode::Rod, "Rod" },
    { MgCoordinateSystemUnitCode::Perch, "Perch" },
    { MgCoordinateSystemUnitCode::Pole, "Pole" },
    { MgCoordinateSystemUnitCode::Furlong, "Furlong" },
    { MgCoordinateSystemUnitCode::Rood, "Rood" },
    { MgCoordinateSystemUnitCode::CapeFoot, "CapeFoot" },
    { MgCoordinateSystemUnitCode::InternationalChain, "IntnlChain" },
    { MgCoordinateSystemUnitCode::InternationalLink, "IntnlLink" },
    { MgCoordinateSystemUnitCode::BrFootTrunc, "BrFootTrunc" },
    { MgCoordinateSystemUnitCode::BrChainTrunc, "BrChainTrunc" },
    { MgCoordinateSystemUnitCode::BrLinkTrunc, "BrLinkTrunc" },
    { MgCoordinateSystemUnitCode::Brealey, "Brealey" }, //here starts angle units
    { MgCoordinateSystemUnitCode::Degree, "Degree" },
    { MgCoordinateSystemUnitCode::Grad, "Grad" },
    { MgCoordinateSystemUnitCode::Grade, "Grade" },
    { MgCoordinateSystemUnitCode::MapInfo, "MapInfo" },
    { MgCoordinateSystemUnitCode::Mil, "Mil" },
    { MgCoordinateSystemUnitCode::Minute, "Minute" },
    { MgCoordinateSystemUnitCode::Radian, "Radian" },
    { MgCoordinateSystemUnitCode::Second, "Second" },
    { MgCoordinateSystemUnitCode::Decisec, "Decisec" },
    { MgCoordinateSystemUnitCode::Centisec, "Centisec" },
    { MgCoordinateSystemUnitCode::Millisec, "Millisec" },
    { MgCoordinateSystemUnitCode::Unknown, "" }
};

//Returns the Unit code associated with a given unit
//key name.  Returns MgCoordinateSystemUnitCode::Unknown if it's not a recognized string.
INT32 UnitFromString(const char *kpStr)
{
    if (NULL == kpStr) return MgCoordinateSystemUnitCode::Unknown;

    for (int i=0; unitmap[i].unit != MgCoordinateSystemUnitCode::Unknown; i++)
    {
        if (0 == CS_stricmp(kpStr, unitmap[i].szName))
        {
            //Found a match!
            return unitmap[i].unit;
        }
    }

    //Scanned entire table without finding a match.
    return MgCoordinateSystemUnitCode::Unknown;
}

//Returns the unit key name associated with a given
//Unit code.
const char * StringFromUnit(INT32 unit)
{
    if (MgCoordinateSystemUnitCode::Unknown == unit) return NULL;

    for (int i=0; unitmap[i].unit != MgCoordinateSystemUnitCode::Unknown; i++)
    {
        if (unitmap[i].unit == unit)
        {
            //Found a match!
            return unitmap[i].szName;
        }
    }

    //Scanned entire table without finding a match.
    return NULL;
}

//Gets information on the specified unit type.  Null pointers
//may be passed in if the caller's not interested in a particular
//bit of information.  Function returns true for success or false
//for failure (meaning that that particular unit type couldn't
//be found).
//
bool GetUnitInfo(INT32 unit, INT32 *pUnitType, double *pdScale)
{
    const char *kpStr = StringFromUnit(unit);
    if (NULL == kpStr) return false;
    if ((NULL == pUnitType) && (NULL == pdScale)) return true;

    //Look up the unit
    INT32 unitType = MgCoordinateSystemUnitType::Unknown;
    //CS_unitlu() returns a hard zero if it doesn't find the
    //specified unit.
    double dScale = CS_unitlu(cs_UTYP_LEN, kpStr);
    if (0.0 == dScale)
    {
        //It's not linear, see if it's angular.
        dScale = CS_unitlu(cs_UTYP_ANG, kpStr);
        if (0.0 != dScale) unitType = MgCoordinateSystemUnitType::Angular;
    }
    else
    {
        //It's linear.
        unitType = MgCoordinateSystemUnitType::Linear;
    }

    if (NULL != pUnitType) *pUnitType = unitType;
    if (NULL != pdScale) *pdScale = dScale;
    return (dScale != 0.0);
}

//Gets the unit type which the specified projection uses.
//
INT32 GetProjectionUnitType(INT32 prj)
{
    //Currently, the situation with Mentor is this:
    //the unity (Lat/Long) projection uses angular
    //units, everything else uses linear.
    //
    //Unfortunately, I have to hard-code that assumption
    //here rather than calling some function in Mentor,
    //since Mentor doesn't provide anything like this
    //function.  Therefore, if Mentor's status quo
    //in regard to unit types versus projections ever
    //changes in the future, this code will need to be
    //updated.  Code maintenance, beware.
    //MENTOR_MAINTENANCE
    switch (prj)
    {
    case MgCoordinateSystemProjectionCode::LL:
        return MgCoordinateSystemUnitType::Angular;
    case MgCoordinateSystemProjectionCode::Unknown:
        return MgCoordinateSystemUnitType::Unknown;
    default:
        return MgCoordinateSystemUnitType::Linear;
    }
}

//Returns whether the specified offset in meters is legal for a datum.
//
//These numbers were gleaned from the CSdtcomp() function in CSDTCOMP.C.
//
bool IsLegalDatumOffset(double dMeters)
{
    return (fabs(dMeters) <= kdMaxDatumOffset);
}

//Returns whether the specified Bursa-Wolfe rotation in arcseconds
//is legal for a datum
//
//These numbers were gleaned from the CSdtcomp() function in CSDTCOMP.C.
//
bool IsLegalDatumRotation(double dArcSeconds)
{
    return (fabs(dArcSeconds) < kdMaxDatumRotation);
}

//Returns whether the specified Bursa-Wolfe scale factor
//is legal for a datum.  Value is in parts per million, i.e.
//a value of 1.000012 would equate to dBwScale = 12.0
//
//These numbers were gleaned from the CSdtcomp() function in CSDTCOMP.C.
//
bool IsLegalBwScale(double dBwScale)
{
    return ((dBwScale <= kdMaxBwScale) && (dBwScale >= kdMinBwScale));
}

//Returns whether the specified radius in meters is legal for an
//ellipsoid.
//
//These numbers were gleaned from CSELCOMP.C.
//
bool IsLegalEllipsoidRadius(double dMeters)
{
    return ( (dMeters >= kdMinEllipsoidRadius)
        && (dMeters <= kdMaxEllipsoidRadius) );
}

//Returns whether the specified flattening ratio is within the
//legal range for an ellipsoid.
//
//These numbers were gleaned from CSELCOMP.C.
//
bool IsLegalFlatteningRatio(double dFlat)
{
    return ( (dFlat >= kdMinEllipsoidFlat) && (dFlat <= kdMaxEllipsoidFlat) );

}

//Calculates a flattening ratio, given an equatorial and a polar radius.
//
double FlatteningRatio(double dEquatorial, double dPolar)
{
//    assert(IsLegalEllipsoidRadius(dEquatorial));
//    assert(IsLegalEllipsoidRadius(dPolar));
    return 1.0 - dPolar / dEquatorial;
}

//Calculates a flattening ratio, given an eccentricity.
//
double FlatteningRatio(double dEccentricity)
{
//    assert(dEccentricity >= 0.0);
//    assert(dEccentricity <= 1.0);
    return 1.0 - sqrt(1.0 - dEccentricity * dEccentricity);
}

//Calculates an eccentricity, given a flattening ratio
//
double Eccentricity(double dFlat)
{
//    assert(dFlat >= 0.0);
//    assert(dFlat <= 1.0);
    return sqrt((2.0 * dFlat) - (dFlat * dFlat));
}

//Returns whether the specified datum conversion technique is
//one that Mentor knows about.
//MENTOR_MAINTENANCE
//
bool IsLegalGeodeticTransformationMethod(INT32 geodeticTransformationMethod)
{
    switch(geodeticTransformationMethod)
    {
    case MgCoordinateSystemGeodeticTransformationMethod::None:
    case MgCoordinateSystemGeodeticTransformationMethod::Molodensky:
    case MgCoordinateSystemGeodeticTransformationMethod::MReg:
    case MgCoordinateSystemGeodeticTransformationMethod::Bursa:
    case MgCoordinateSystemGeodeticTransformationMethod::NAD27:
    case MgCoordinateSystemGeodeticTransformationMethod::NAD83:
    case MgCoordinateSystemGeodeticTransformationMethod::WGS84:
    case MgCoordinateSystemGeodeticTransformationMethod::WGS72:
    case MgCoordinateSystemGeodeticTransformationMethod::HPGN:
    case MgCoordinateSystemGeodeticTransformationMethod::Lclgrf:
    case MgCoordinateSystemGeodeticTransformationMethod::SevenParameter:
    case MgCoordinateSystemGeodeticTransformationMethod::AGD66:
    case MgCoordinateSystemGeodeticTransformationMethod::ThreeParameter:
    case MgCoordinateSystemGeodeticTransformationMethod::SixParameter:
    case MgCoordinateSystemGeodeticTransformationMethod::FourParameter:
    case MgCoordinateSystemGeodeticTransformationMethod::AGD84:
    case MgCoordinateSystemGeodeticTransformationMethod::NZGD49:
    case MgCoordinateSystemGeodeticTransformationMethod::ATS77:
    case MgCoordinateSystemGeodeticTransformationMethod::GDA94:
    case MgCoordinateSystemGeodeticTransformationMethod::NZGD2K:
    case MgCoordinateSystemGeodeticTransformationMethod::CSRS:
    case MgCoordinateSystemGeodeticTransformationMethod::TOKYO:
    case MgCoordinateSystemGeodeticTransformationMethod::RGF93:
    case MgCoordinateSystemGeodeticTransformationMethod::ED50:
    case MgCoordinateSystemGeodeticTransformationMethod::DHDN:
    case MgCoordinateSystemGeodeticTransformationMethod::ETRF89:
    case MgCoordinateSystemGeodeticTransformationMethod::Geocentric:
    case MgCoordinateSystemGeodeticTransformationMethod::CHENYX:
        return true;
    default:
        return false;
    }
}

//Builds a cs_Csdef_ struct from an MgCoordinateSystem interface.
//
bool BuildCsDefFromInterface(MgCoordinateSystem *pSrc, cs_Csdef_& def)
{
    assert(NULL != pSrc);

    //Zero out the def.
    memset(&def, 0, sizeof(def));

    CCoordinateSystem* pSrcImp=dynamic_cast<CCoordinateSystem*>(pSrc);
    assert(pSrcImp);
    if (!pSrcImp)
    {
        return false;
    }
    pSrcImp->GetMentorDef(def);
    //Copy complete!  Return success.
    return true;
}

//Builds a cs_Dtdef_ struct from an MgCoordinateSystemDatum interface.
//
bool BuildDtDefFromInterface(MgCoordinateSystemDatum *pSrc, cs_Dtdef_& def)
{
    assert(NULL != pSrc);

    //Zero out the def.
    memset(&def, 0, sizeof(def));

    CCoordinateSystemDatum* pSrcImp=dynamic_cast<CCoordinateSystemDatum*>(pSrc);
    assert(pSrcImp);
    if (!pSrcImp)
    {
        return false;
    }
    pSrcImp->GetMentorDef(def);
    //Copy complete!  Return success.
    return true;
}

//Builds a cs_Eldef_ struct from an MgCoordinateSystemEllipsoid interface.
//
bool BuildElDefFromInterface(MgCoordinateSystemEllipsoid *pSrc, cs_Eldef_& def)
{
    assert(NULL != pSrc);

    //Zero out the def.
    memset(&def, 0, sizeof(def));

    CCoordinateSystemEllipsoid* pSrcImp=dynamic_cast<CCoordinateSystemEllipsoid*>(pSrc);
    assert(pSrcImp);
    if (!pSrcImp)
    {
        return false;
    }
    pSrcImp->GetMentorDef(def);

    //Copy complete!  Return success.
    return true;
}

//Builds an MgCoordinateSystem interface from a cs_Csdef_ struct.
//Caller is responsible for freeing the returned pointer via Release().
//Works only for non NERTH projections
//
MgCoordinateSystem *BuildInterfaceFromCsDef(const cs_Csdef_& def, MgCoordinateSystemCatalog* pCatalog)
{
    Ptr<CCoordinateSystem> pDef;

    MG_TRY()

    cs_Csdef_ defLocal = def;
    //Tweak the coordsys def as needed to make sure
    //that it's legal.
    {
        //make sure the scale reduction factor's okay
        INT32 prj = ProjectionFromString(defLocal.prj_knm);
        if (!ProjectionUsesSclRed(prj))
        {
            defLocal.scl_red = 1.0;
        }
    }

    //Construct our object
    pDef = new CCoordinateSystem(pCatalog);

    if (NULL != pDef.p)
    {
        pDef->InitFromCatalog(def);
    }

    MG_CATCH_AND_THROW(L"BuildInterfaceFromCsDef")

    return pDef.Detach();
}

//Builds an MgCoordinateSystemDatum interface from a cs_Dtdef_ struct.
//Caller is responsible for freeing the returned pointer via Release().
//
MgCoordinateSystemDatum* BuildInterfaceFromDtDef(const cs_Dtdef_& def, MgCoordinateSystemCatalog* pCatalog)
{
    Ptr<CCoordinateSystemDatum> pNew;

    MG_TRY()
    //Construct our object
    pNew = new CCoordinateSystemDatum(pCatalog);

    if (NULL != pNew.p)
    {
        pNew->InitFromCatalog(def);
    }

    MG_CATCH_AND_THROW(L"BuildInterfaceFromDtDef")

    return pNew.Detach();
}

//Builds an MgCoordinateSystemEllipsoid interface from a cs_Eldef_ struct.
//Caller is responsible for freeing the returned pointer via Release().
//
MgCoordinateSystemEllipsoid* BuildInterfaceFromElDef(const cs_Eldef_& def, MgCoordinateSystemCatalog* pCatalog)
{
    Ptr<CCoordinateSystemEllipsoid> pNew;

    MG_TRY()

    //Construct our object
    pNew = new CCoordinateSystemEllipsoid(pCatalog);

    if (NULL != pNew.p)
    {
        pNew->Init(def);
    }

    MG_CATCH_AND_THROW(L"BuildInterfaceFromElDef")

    return pNew.Detach();
}

//Builds a cs_Datum_ struct from an MgCoordinateSystemDatum interface.
//
bool BuildDatumFromInterface(MgCoordinateSystemDatum *pSrc, cs_Datum_& datum)
{
    assert(NULL != pSrc);

    bool bResult;
    if (NULL == pSrc)
    {
        //Datum wasn't initialized.
        memset(&datum, 0, sizeof(datum));
        return false;
    }

    //Build a cs_Dtdef_ struct out of it
    cs_Dtdef_ dtdef;
    bResult = BuildDtDefFromInterface(pSrc, dtdef);
    if (!bResult)
    {
        return bResult;
    }

    //Get the ellipsoid definition out of the datum
    Ptr<MgCoordinateSystemEllipsoid> pElDef = pSrc->GetEllipsoidDefinition();
    if (!pElDef)
    {
        return false;
    }

    //Build a cs_Eldef_ struct out of it
    cs_Eldef_ eldef;
    bResult = BuildElDefFromInterface(pElDef, eldef);
    if (!bResult)
    {
        return bResult;
    }

    //Make a cs_Datum_ struct
    CriticalClass.Enter();
    cs_Datum_ *pDatum = CSdtloc2(&dtdef, &eldef);
    CriticalClass.Leave();
    if (NULL == pDatum)
    {
        return false;
    }

    //Copy it into the struct provided
    datum = *pDatum;
    CS_free(pDatum);
    pDatum = NULL;

    //And return success!
    return true;
}

// Projection Table Look Up, based on PrjLookUp in csEdit.cpp
// but csEdit.cpp is not accessible because it uses MFCs
struct cs_Prjtab_* GetMentorProjectionObject(const char* prjKeyName)
{
    struct cs_Prjtab_ *prjPtr;
    for (prjPtr = cs_Prjtab;prjPtr->code != cs_PRJCOD_END;prjPtr += 1)
    {
        if (!CS_stricmp (prjKeyName,prjPtr->key_nm)) break;
    }
    if (prjPtr->code == cs_PRJCOD_END ||
        *prjPtr->key_nm == '\0' ||
        prjPtr->setup == NULL)
    {
        prjPtr = NULL;
    }
    return (prjPtr);
}

//Builds a cs_Csprm_ struct from a cs_Csdef_ structure.
bool BuildCsprmFromArbitraryDef(const cs_Csdef_& csdef, cs_Csprm_& csprm)
{
    if (!ProjectionIsNerthType (csdef.prj_knm))
    {
        return false;
    }

    CriticalClass.Enter();
    struct cs_Csprm_ *pCsprm = CScsloc1(const_cast<cs_Csdef_*>(&csdef));
    CriticalClass.Leave();
    if (NULL == pCsprm) return false; //E_OUTOFMEMORY;
    csprm=*pCsprm;
    CS_free(pCsprm);
    return true;
}

//Builds a cs_Csprm_ struct from an MgCoordinateSystem interface.
//
bool BuildCsprmFromInterface(MgCoordinateSystem *pSrc, cs_Csprm_& csprm)
{
    assert(NULL != pSrc);
    if (NULL == pSrc) return false;

    bool bResult;

    //Get the coordinate system definition.
    cs_Csdef_ csdef;

    bResult = BuildCsDefFromInterface(pSrc, csdef);
    if (!bResult) return bResult;

    struct cs_Csprm_ *pCsprm = NULL;
    if (ProjectionIsNerthType (csdef.prj_knm))
    {
        pCsprm = (struct cs_Csprm_ *)CS_malc (sizeof(struct cs_Csprm_));
        if (NULL == pCsprm) return false; //E_OUTOFMEMORY;
        bResult=BuildCsprmFromArbitraryDef(csdef, *pCsprm);
        assert(bResult);
    }
    else
    {
        //Try to get a datum from the coordsys.
        Ptr<MgCoordinateSystemDatum> pDatum = pSrc->GetDatumDefinition();

        cs_Datum_ datum;

        if (!pDatum)
        {
            //Couldn't get a datum from the coordsys, since it's
            //cartographic.  Get an ellipsoid definition instead.
            Ptr<MgCoordinateSystemEllipsoid> pElDef = pSrc->GetEllipsoidDefinition();
            assert(pElDef);
            if (!pElDef) return false;

            //Build a Mentor struct out of the ellipsoid
            cs_Eldef_ eldef;
            bResult = BuildElDefFromInterface(pElDef, eldef);
            if (!bResult) return bResult;

            //And initialize our datum struct.
            CriticalClass.Enter();
            cs_Datum_ *pDt = CSdtloc2(NULL, &eldef);
            CriticalClass.Leave();
            if (NULL == pDt) return false;
            datum = *pDt;
            CS_free(pDt);
        }
        else
        {
            //Got the datum.  Now build a Mentor cs_Datum_ struct
            //out of it.
            bResult = BuildDatumFromInterface(pDatum, datum);
            if (!bResult) return bResult;
        }

        //Okay, at this point we have an initialized datum struct
        //and an initialized coordinate system definition.  Go
        //ahead and build the output struct.
        CriticalClass.Enter();
        pCsprm = CScsloc(&csdef, &datum);
        CriticalClass.Leave();
        if (NULL == pCsprm) return false;
    }

    //Copy it across
    csprm = *pCsprm;
    CS_free(pCsprm);

    //And return success!
    return true;
}

bool BuildDefsFromInterface(
    MgCoordinateSystem *pSrc,
    cs_Csdef_*& pCsDef,
    cs_Dtdef_*& pDtDef,
    cs_Eldef_*& pElDef)
{
    assert(NULL != pSrc);

    bool bResult=false;
    pCsDef=NULL;
    pDtDef=NULL;
    pElDef=NULL;

    //If the coordinate system hasn't been initialized,
    //the above will return a null pointer.  This function
    //should only be called on properly initialized
    //coordsys objects.
    if (NULL == pSrc) return false;

    //allocate memory
    pCsDef = (struct cs_Csdef_ *)CS_malc (sizeof (cs_Csdef_));
    if (!pCsDef)
    {
        return false; //E_OUTOFMEMORY;
    }
    memset(pCsDef, 0, sizeof(cs_Csdef_));

    bResult = BuildCsDefFromInterface(pSrc, *pCsDef);
    if (!bResult)
    {
        CS_free(pCsDef);
        pCsDef=NULL;
        return bResult;
    }

    if (!ProjectionIsNerthType (pCsDef->prj_knm))
    {
        //Try to get a datum from the coordsys.
        Ptr<MgCoordinateSystemDatum> pDatum = pSrc->GetDatumDefinition();

        if (!pDatum)
        {
            //Couldn't get a datum from the coordsys, since it's
            //cartographic.  Get an ellipsoid definition instead.
            Ptr<MgCoordinateSystemEllipsoid> pIElDef = pSrc->GetEllipsoidDefinition();
            assert(NULL != pIElDef);
            if (!pIElDef)
            {
                CS_free(pCsDef);
                pCsDef=NULL;
                return bResult;
            }

            //allocate memory
            pElDef = (struct cs_Eldef_ *)CS_malc (sizeof (cs_Eldef_));
            if (!pElDef)
            {
                CS_free(pCsDef);
                pCsDef=NULL;
                return false;//E_OUTOFMEMORY;
            }
            memset(pElDef, 0, sizeof(cs_Eldef_));

            //Build a Mentor struct out of the ellipsoid
            bResult = BuildElDefFromInterface(pIElDef, *pElDef);
            if (!bResult)
            {
                CS_free(pCsDef);
                pCsDef=NULL;
                CS_free(pElDef);
                pElDef=NULL;
                return bResult;
            }
        }
        else
        {
            //allocate memory
            pDtDef = (struct cs_Dtdef_ *)CS_malc (sizeof (cs_Dtdef_));
            if (!pDtDef)
            {
                CS_free(pCsDef);
                pCsDef=NULL;
                return false; //E_OUTOFMEMORY;
            }
            memset(pDtDef, 0, sizeof(cs_Dtdef_));

            //Build a cs_Dtdef_ struct out of it
            bResult = BuildDtDefFromInterface(pDatum, *pDtDef);
            if (!bResult)
            {
                CS_free(pCsDef);
                pCsDef=NULL;
                CS_free(pDtDef);
                pDtDef=NULL;
                return bResult;
            }

            //Get the ellipsoid definition out of the datum
            Ptr<MgCoordinateSystemEllipsoid> pIElDef = pDatum->GetEllipsoidDefinition();
            if (!pIElDef)
            {
                CS_free(pCsDef);
                pCsDef=NULL;
                CS_free(pDtDef);
                pDtDef=NULL;
                return bResult;
            }

            //allocate memory
            pElDef = (struct cs_Eldef_ *)CS_malc (sizeof (cs_Eldef_));
            if (!pElDef)
            {
                CS_free(pCsDef);
                pCsDef=NULL;
                CS_free(pDtDef);
                pDtDef=NULL;
                return false; //E_OUTOFMEMORY;
            }
            memset(pElDef, 0, sizeof(cs_Eldef_));

            //Build a cs_Eldef_ struct out of it
            bResult = BuildElDefFromInterface(pIElDef, *pElDef);
            if (!bResult)
            {
                CS_free(pCsDef);
                pCsDef=NULL;
                CS_free(pDtDef);
                pDtDef=NULL;
                CS_free(pElDef);
                pElDef=NULL;
                return bResult;
            }
        }
    }

    //And return success!
    return true;
}

//Returns whether the specified def is geodetic.
//
bool CsdefIsGeodetic(const cs_Csdef_& def)
{
    return ('\0' != def.dat_knm[0]);
}

//Shifts a lat/long point from one datum to another.
//
//Return values:
//
// 0:       Success!
// -1:      Total failure.  No conversion was done, output equals input.
// 1:       Partial failure due to missing data files, input point out
//            of range (i.e. it's NAD27 and the point is in Japan), etc.
//            Best-guess transformation was done, but may not be correct.
//
INT32 GeodeticTransformationPoint(cs_Dtcprm_ *pDtcprm, double& dLongitude, double& dLatitude, double* pdZ)
{
    assert(NULL != pDtcprm);

    //Skip datum transformation if we have a null transformation
    //That is, if [pDtcprm->xfrmCount] is 0 or the transformation at index 0 is [cs_DTCMTH_NULLX];
    //in either case, the entry at index 0 must never be NULL, if [xfrmCount] is not 0
    assert(0 == pDtcprm->xfrmCount || NULL != pDtcprm->xforms[0]);
    
    bool isNullTransform = (0 == pDtcprm->xfrmCount || //is there a transformation entry at all?
          (1 == pDtcprm->xfrmCount && NULL != pDtcprm->xforms[0] && cs_DTCMTH_NULLX == pDtcprm->xforms[0]->methodCode)); //if so, check, whether we've [cs_DTCMTH_NULLX] at index 0

    if (isNullTransform)
        return 0;

    double dZ=0.;
    if (pdZ)
    {
        dZ=*pdZ;
    }

    CriticalClass.Enter();
    double dLonLat[3] = { dLongitude, dLatitude, dZ };
    INT32 nResult;
    if (!pdZ)
    {
        nResult = CS_dtcvt(pDtcprm, dLonLat, dLonLat);
    }
    else
    {
        nResult = CS_dtcvt3D(pDtcprm, dLonLat, dLonLat);
    }
    CriticalClass.Leave();

    dLongitude = dLonLat[0];
    dLatitude = dLonLat[1];
    if (pdZ)
    {
        *pdZ=dLonLat[2];
    }
    
    return nResult;
}

//method that set the current file name of a dictionary in CS Map
void SetDictionaryFileName(CREFSTRING sFileName /* no directory information must be included */,
                           CREFSTRING dictionaryPath, /* the directory; no file information */
                           INT32& magicNumber, /* will be set */
                           CsDictionaryOpenMode (*MagicNumberCallback)(long),
                           void (*FileNameTarget)(const char* newFileName),
                           const wchar_t* context)
{
    CHECKARGUMENTNULL(MagicNumberCallback, L"MentorUtil.SetDictionaryFileName");
    CHECKARGUMENTNULL(FileNameTarget, L"MentorUtil.SetDictionaryFileName");
    CHECKARGUMENTEMPTYSTRING(sFileName, L"MentorUtil.SetDictionaryFileName");
    CHECKARGUMENTEMPTYSTRING(dictionaryPath, L"MentorUtil.SetDictionaryFileName");

    char* szCs = NULL;
    bool entered = false;

    MG_TRY()

    //Make local variables to hold converted strings
    bool bResult = IsValidDictionaryName(sFileName);
    if (!bResult)
    {
        MgStringCollection arguments;
        arguments.Add(sFileName);
        throw new MgFileIoException(/*context */ L"MentorUtil.SetDictionaryFileName", __LINE__, __WFILE__, &arguments, L"MgInvalidArgumentException", NULL);
    }

    STRING fileNameSet;
    MentorDictionary::SetFileName(
        magicNumber,
        MagicNumberCallback,
        dictionaryPath,
        sFileName,
        fileNameSet,
        L"MentorUtil.SetDictionaryFileName");

    //Okay, everybody opened all right, so update Mentor's global
    //variables appropriately.
    szCs = Convert_Wide_To_Ascii(fileNameSet.c_str());

    CriticalClass.Enter();
    entered = true;

    //the target function in CS map that sets the current active dictionary file name
    FileNameTarget(szCs);

    MG_CATCH(L"MentorUtil.SetDictionaryFileName")

    if (entered)
        CriticalClass.Leave();

    delete[] szCs;

    MG_THROW()
}

//Opens a Mentor dictionary, verifies magic value, and positions read
//pointer at start of first record.  Returns true for success, false
//for failure.
//
csFILE* OpenDictionaryFile(
    const wchar_t *kpFileName,
    char *szMode,
    INT32& lMagic,
    CsDictionaryOpenMode (*ValidMagic)(long))
{
    csFILE* pFile=NULL;

    assert(NULL != kpFileName);
    char* sz_kpFileName=Convert_Wide_To_Ascii(kpFileName);
    pFile=fopen(sz_kpFileName, szMode);
    delete[] sz_kpFileName;
    if (!pFile || ferror(pFile))
    {
        if (pFile)
        {
            CS_fclose(pFile);
            pFile=NULL;
        }
        return pFile;
    }
    cs_magic_t fileMagic = 0;
    size_t nRead=CS_fread(reinterpret_cast<void*>(&fileMagic), sizeof(fileMagic),1, pFile);
    if (1!=nRead || (Closed == ValidMagic(fileMagic)))
    {
        CS_fclose(pFile);
        pFile=NULL;
    }
    else
    {
        lMagic = fileMagic;
    }

    return pFile;
}

//Opens a Mentor dictionary, verifies magic value, and positions read
//pointer at start of first record.  Returns true for success, false
//for failure.
//
bool GetMagicFromDictionaryFile(
    const wchar_t *kpFileName,
    INT32& lMagic,
    CsDictionaryOpenMode (*ValidMagic)(long))
{
    assert(NULL != kpFileName);
    char* sz_kpFileName=Convert_Wide_To_Ascii(kpFileName);
    char szMode[10];
    GetFileModes(Read, szMode);
    SmartCriticalClass critical(true);
    csFILE *pFile=CS_fopen(sz_kpFileName, szMode);
    delete[] sz_kpFileName;
    if (!pFile || ferror(pFile))
    {
        if (pFile)
        {
            CS_fclose(pFile);
            pFile=NULL;
        }
        return false;
    }
    cs_magic_t fileMagic = 0;
    size_t nRead=CS_fread(reinterpret_cast<void*>(&fileMagic), sizeof(fileMagic),1, pFile);
    if (1!=nRead || (Closed == ValidMagic(fileMagic)))
    {
        CS_fclose(pFile);
        pFile=NULL;
        return false;
    }
    lMagic = fileMagic;
    CS_fclose(pFile);
    pFile=NULL;
    return true;
}

//Returns the current "Mentor time" in days since January 1, 1990.
//
INT16 MentorTime()
{
    //Numbers taken from Mentor source code.
    return static_cast<INT16>((time(NULL)-630720000L) / 86400L);
}

//Returns whether a given protect flag value specifies that the object
//is truly protected, taking into account the current date, the current
//state of Mentor global variables, etc.  "Is this object currently
//modifiable, according to Mentor?"
//
bool IsReallyProtected(INT16 sProtect)
{
    if (cs_Protect < 0)
    {
        //All protection is disabled.
        return false;
    }

    if (0 == cs_Protect)
    {
        //Distribution items are protected, but user-defined
        //items are not.  Distribution items are recognized
        //by having a protect flag of 1.
        return (1 == sProtect);
    }

    assert(cs_Protect > 0);

    //The Mentor global cs_Protect is a positive number representing
    //an age in days.  User-defined objects older than this many
    //days will be protected; younger than that, they're not.
    //Distribution items are always protected.
    if (1 == sProtect) return true;
    if (sProtect < 1) return false;

    //The item's age in days is the difference between sProtect (which
    //is a date stamp) and the current time.
    INT16 sAge = MentorTime() - sProtect;

    //The user-defined item is protected if its age is greater than cs_Protect.
    return (sAge > cs_Protect);
}

//Decrypts an encrypted character string, using Mentor's algorithm
//
void
DecryptBuffer(
    char *pBuf,
    unsigned char ucKey,
    INT32 nBufSize)
{
    if ('\0' == ucKey)
    {
        //Not encrypted; nothing to do.
        return;
    }
    unsigned char *cp;

    cp = reinterpret_cast<unsigned char *>(pBuf);
    for (int i=0; i<nBufSize; i++)
    {
        ucKey ^= *cp;
        *cp++ = ucKey;
    }
}

//Does a Mentor-6-style case insensitive comparison of two strings
//(i.e. '_' > letters).  Note:  may modify input buffers.
//
int Mentor6Strnicmp(char *pBuf1, char *pBuf2, INT32 nLen)
{
    STRING sBuf1 = MgUtil::ToUpper(MgUtil::MultiByteToWideChar(pBuf1));
    STRING sBuf2 = MgUtil::ToUpper(MgUtil::MultiByteToWideChar(pBuf2));

    return wcsncmp(sBuf1.c_str(), sBuf2.c_str(), nLen);
}

//reads a const char* and returns the STRING class object for it
STRING MentorReadString(const char* mentorString)
{
    if (NULL == mentorString)
        return L"";

    STRING readString;
    wchar_t* pString = NULL;

    MG_TRY()

    pString = Convert_UTF8_To_Wide(mentorString);
    if (NULL == pString) //ABA: this cannot be null, can it?
        throw new MgOutOfMemoryException(L"MentorUtil.ReadString", __LINE__, __WFILE__, NULL, L"", NULL);

    MG_CATCH(L"MentorUtil.ReadString")

    readString = pString;
    delete[] pString;

    MG_THROW()

    return readString;
}

void MentorSetString(CREFSTRING sSrc, char *pDest, UINT32 nMaxSize)
{
    //make sure to have checked the Protection level before calling this method

    char *pStr = NULL;

    MG_TRY()

    assert(NULL != pDest);
    CHECKARGUMENTNULL(pDest, L"MentorSetString");

    if (!IsLegalString(sSrc.c_str(), nMaxSize))
    {
        //Can't set string, caller gave us an illegal value
        throw new MgInvalidArgumentException(L"MentorSetString", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert to a C++ string, for Mentor's sake
    pStr = Convert_Wide_To_UTF8(sSrc.c_str()); //need to delete[] pStr
    if (NULL == pStr)
    {
        throw new MgOutOfMemoryException(L"MentorSetString", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Copy it in
    memset(pDest, 0, nMaxSize);
    strncpy(pDest, pStr, nMaxSize);

    MG_CATCH(L"MentorSetString")

    //Clean up and return success.
    delete [] pStr;

    MG_THROW()
}
