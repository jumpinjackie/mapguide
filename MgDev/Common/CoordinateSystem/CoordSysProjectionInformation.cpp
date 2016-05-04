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

#include "CoordSysEnumInteger32.h"          //for CCoordinateSystemEnumInteger32
#include "CoordSysProjectionInformation.h"  //for CCoordinateSystemProjectionInformation

#include "CoordSysUtil.h"                   //for Convert_UTF8_To_Wide
#include "MentorUtil.h"                        //for IsLegalMentorName()

using namespace CSLibrary;

//Externals needed from Mentor
extern "C" struct cs_PrjprmMap_ cs_PrjprmMap [];

//MENTOR_MAINTENANCE:  The following constant is the number of
//different projections supported by the Mentor library.  It
//will need to be adjusted if Mentor adds additional projections.
const long klNumProjections = 72;


//MENTOR_MAINTENANCE:  This is a list of all projections that
//Mentor supports.  It will need to be adjusted if Mentor adds
//more units.
INT32 kProjections[klNumProjections] = {
    MgCoordinateSystemProjectionCode::LL,
    MgCoordinateSystemProjectionCode::Tm,
    MgCoordinateSystemProjectionCode::Trmrs,
    MgCoordinateSystemProjectionCode::Trmeraf,
    MgCoordinateSystemProjectionCode::Sotrm,
    MgCoordinateSystemProjectionCode::Mndott,
    MgCoordinateSystemProjectionCode::Wccst,
    MgCoordinateSystemProjectionCode::Utm,
    MgCoordinateSystemProjectionCode::Alber,
    MgCoordinateSystemProjectionCode::Azede,
    MgCoordinateSystemProjectionCode::Azmea,
    MgCoordinateSystemProjectionCode::Azmed,
    MgCoordinateSystemProjectionCode::Bipolar,
    MgCoordinateSystemProjectionCode::Bonne,
    MgCoordinateSystemProjectionCode::Cassini,
    MgCoordinateSystemProjectionCode::Eckert4,
    MgCoordinateSystemProjectionCode::Eckert6,
    MgCoordinateSystemProjectionCode::Edcnc,
    MgCoordinateSystemProjectionCode::Edcyl,
    MgCoordinateSystemProjectionCode::GaussK,
    MgCoordinateSystemProjectionCode::Gnomonic,
    MgCoordinateSystemProjectionCode::Goode,
    MgCoordinateSystemProjectionCode::Hom1uv,
    MgCoordinateSystemProjectionCode::Hom2uv,
    MgCoordinateSystemProjectionCode::Hom1xy,
    MgCoordinateSystemProjectionCode::Hom2xy,
    MgCoordinateSystemProjectionCode::Krovak,
    MgCoordinateSystemProjectionCode::Krvk95,
    MgCoordinateSystemProjectionCode::Rskew,
    MgCoordinateSystemProjectionCode::Rskewc,
    MgCoordinateSystemProjectionCode::Rskewo,
    MgCoordinateSystemProjectionCode::Lm1sp,
    MgCoordinateSystemProjectionCode::Lm2sp,
    MgCoordinateSystemProjectionCode::Lmblg,
    MgCoordinateSystemProjectionCode::Lmbrtaf,
    MgCoordinateSystemProjectionCode::Mndotl,
    MgCoordinateSystemProjectionCode::Wccsl,
    MgCoordinateSystemProjectionCode::Lmtan,
    MgCoordinateSystemProjectionCode::Miller,
    MgCoordinateSystemProjectionCode::Modpc,
    MgCoordinateSystemProjectionCode::Mollweid,
    MgCoordinateSystemProjectionCode::Mrcat,
    MgCoordinateSystemProjectionCode::MrcatK,
    MgCoordinateSystemProjectionCode::Mstero,
    MgCoordinateSystemProjectionCode::Neacyl,
    MgCoordinateSystemProjectionCode::Nerth,
    MgCoordinateSystemProjectionCode::Nrthsrt,
    MgCoordinateSystemProjectionCode::Nzealand,
    MgCoordinateSystemProjectionCode::OblqM,        //(obsolete)
    MgCoordinateSystemProjectionCode::Obqcyl,
    MgCoordinateSystemProjectionCode::Ortho,
    MgCoordinateSystemProjectionCode::Ostn02,
    MgCoordinateSystemProjectionCode::Ostn97,
    MgCoordinateSystemProjectionCode::Ostro,
    MgCoordinateSystemProjectionCode::Plycn,
    MgCoordinateSystemProjectionCode::Pstro,
    MgCoordinateSystemProjectionCode::Pstrosl,
    MgCoordinateSystemProjectionCode::Robinson,
    MgCoordinateSystemProjectionCode::Sinus,
    MgCoordinateSystemProjectionCode::Sstro,
    MgCoordinateSystemProjectionCode::Swiss,
    MgCoordinateSystemProjectionCode::Sys34,
    MgCoordinateSystemProjectionCode::Sys34_99,
    MgCoordinateSystemProjectionCode::Teacyl,
    MgCoordinateSystemProjectionCode::Trmrkrg,
    MgCoordinateSystemProjectionCode::Winkl,
    MgCoordinateSystemProjectionCode::Vdgrntn,
    MgCoordinateSystemProjectionCode::Sys34_01,
    MgCoordinateSystemProjectionCode::EdcylE,
    MgCoordinateSystemProjectionCode::PlateCarree,
    MgCoordinateSystemProjectionCode::PvMercator,
    MgCoordinateSystemProjectionCode::LmMich,
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemProjectionInformation::CCoordinateSystemProjectionInformation()
{
}

CCoordinateSystemProjectionInformation::~CCoordinateSystemProjectionInformation()
{
}

//MgDisposable
void CCoordinateSystemProjectionInformation::Dispose()
{
    delete this;
}

//Gets an enumerator for listing all the projections supported by the API.
//Caller is responsible for releasing the enumerator when done with it.
MgCoordinateSystemEnumInteger32* CCoordinateSystemProjectionInformation::GetEnumProjections()
{
    Ptr<CCoordinateSystemEnumInteger32> pNewEnum;

    MG_TRY()

    pNewEnum = new CCoordinateSystemEnumInteger32;

    if (NULL == pNewEnum.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemProjectionInformation.GetEnumProjections", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pNewEnum->SetList(kProjections, klNumProjections);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemProjectionInformation.GetEnumProjections")

    return pNewEnum.Detach();
}

//Returns the number of parameters the specified projection is using
//it is in the range 1 - 24 (corresponding
//to the data members of cs_Csdef_, prj_prm1 through prj_prm24).
//
INT32 CCoordinateSystemProjectionInformation::GetParameterCount(INT32 nProjectionCode)
{
    INT32 nNbParams=0;

    MG_TRY()
    if (MgCoordinateSystemProjectionCode::Unknown == nProjectionCode)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParameterCount", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    // for each possible parameter.
    int parmIdx = 0;
    for (parmIdx = 0;parmIdx < knCsNumParams;parmIdx += 1)
    {
        int nResult = CS_prjprm(NULL, nProjectionCode, parmIdx);
        if (nResult <= 0) break; //returns +1 for a valid parameter
    }

    nNbParams=static_cast<INT32>(parmIdx);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemProjectionInformation.GetParameterCount")
    return nNbParams;
}

//Returns whether the specified projection uses the ulIndex'th
//parameter, where ulIndex is in the range 1 - 24 (corresponding
//to the data members of cs_Csdef_, prj_prm1 through prj_prm24).
//
bool CCoordinateSystemProjectionInformation::IsUsingParameter(INT32 nProjectionCode, UINT32 ulIndex)
{
    bool bUses = false;

    MG_TRY()
    if (MgCoordinateSystemProjectionCode::Unknown == nProjectionCode)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.IsUsingParameter", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    assert((ulIndex>=1) && (ulIndex<=knCsNumParams));
    if ((ulIndex < 1) || (ulIndex > knCsNumParams))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParameterCount", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    int nResult = CS_prjprm(NULL, nProjectionCode, ulIndex-1);
    bUses = (1 == nResult);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemProjectionInformation.IsUsingParameter")

    return bUses;
}

//Returns whether the specified projection uses the "origin
//longitude" attribute (corresponding to cs_Csdef_'s
//org_lng member).
//
bool CCoordinateSystemProjectionInformation::IsUsingOriginLongitude(INT32 nProjectionCode)
{
    return ProjectionUsesOrgLng(nProjectionCode);
}

//Returns whether the specified projection uses the "origin
//latitude" attribute (corresponding to cs_Csdef_'s
//org_lat member).
//
bool CCoordinateSystemProjectionInformation::IsUsingOriginLatitude(INT32 nProjectionCode)
{
    return ProjectionUsesOrgLat(nProjectionCode);
}

//Returns whether the specified projection uses the "scale
//reduction" attribute (corresponding to cs_Csdef_'s
//scl_red member).
//
bool CCoordinateSystemProjectionInformation::IsUsingScaleReduction(INT32 nProjectionCode)
{
    return ProjectionUsesSclRed(nProjectionCode);
}

//Returns whether the specified projection uses the "quadrant"
//attribute (corresponding to cs_Csdef_'s quad member).
//
bool CCoordinateSystemProjectionInformation::IsUsingQuadrant(INT32 nProjectionCode)
{
    return ProjectionUsesQuad(nProjectionCode);
}

//Returns whether the specified projection uses the "offset"
//attribute (corresponding to cs_Csdef_'s x_off and y_off members),
//otherwise known as false northing and false easting.
//
bool CCoordinateSystemProjectionInformation::IsUsingOffset(INT32 nProjectionCode)
{
    return ProjectionUsesOffset(nProjectionCode);
}

//Gets the mathematical limits of the specified parameter for
//the specified projection.
//Throws an exception if nProjectionCode is not a valid projection,
//MgCoordinateSystemMismatchException if ulIndex does not specify a parameter
//which this projection uses.
//
double CCoordinateSystemProjectionInformation::GetParameterMin(INT32 nProjectionCode, UINT32 ulIndex)
{
    double dResult=0.;
    MG_TRY()
    if (MgCoordinateSystemProjectionCode::Unknown == nProjectionCode)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParameterMin", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    assert((ulIndex>=1) && (ulIndex<=knCsNumParams));
    if ((ulIndex < 1) || (ulIndex > knCsNumParams))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParameterMin", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Get information about the parameter
    cs_Prjprm_ info;
    int nResult = CS_prjprm(&info, nProjectionCode, ulIndex-1);
    switch(nResult)
    {
    case 1:
        //The projection uses this parameter.
        break;
    case 0:
        //The projection does not use this parameter.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParameterMin", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
        break;
    case -1:
    default:
        //Not a valid projection.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParameterMin", __LINE__, __WFILE__, NULL, L"", NULL);
        break;
    }

    //Return the information.
    dResult=info.min_val;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemProjectionInformation.GetParameterMin")
    return dResult;
}

double CCoordinateSystemProjectionInformation::GetParameterMax(INT32 nProjectionCode, UINT32 ulIndex)
{
    double dResult=0.;
    MG_TRY()
    if (MgCoordinateSystemProjectionCode::Unknown == nProjectionCode)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParameterMax", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    assert((ulIndex>=1) && (ulIndex<=knCsNumParams));
    if ((ulIndex < 1) || (ulIndex > knCsNumParams))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParameterMax", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Get information about the parameter
    cs_Prjprm_ info;
    int nResult = CS_prjprm(&info, nProjectionCode, ulIndex-1);
    switch(nResult)
    {
    case 1:
        //The projection uses this parameter.
        break;
    case 0:
        //The projection does not use this parameter.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParameterMax", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
        break;
    case -1:
    default:
        //Not a valid projection.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParameterMax", __LINE__, __WFILE__, NULL, L"", NULL);
        break;
    }

    //Return the information.
    dResult=info.max_val;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemProjectionInformation.GetParameterMax")
    return dResult;
}

//Gets the "reasonable default" value of the specified
//parameter for the specified projection.
//Throws an exception if nProjectionCode
//is not a valid projection, or with MgCoordinateSystemMismatchException if ulIndex
//does not specify a parameter which this projection uses.
//
double CCoordinateSystemProjectionInformation::GetParameterDefault(INT32 nProjectionCode, UINT32 ulIndex)
{
    double dDefault = 0.;
    MG_TRY()

    if (MgCoordinateSystemProjectionCode::Unknown == nProjectionCode)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamDefault", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    assert((ulIndex>=1) && (ulIndex<=knCsNumParams));
    if ((ulIndex < 1) || (ulIndex > knCsNumParams))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamDefault", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Get information about the parameter
    cs_Prjprm_ info;
    int nResult = CS_prjprm(&info, nProjectionCode, ulIndex-1);
    switch(nResult)
    {
    case 1:
        //The projection uses this parameter.
        break;
    case 0:
        //The projection does not use this parameter.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamDefault", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
        break;
    case -1:
    default:
        //Not a valid projection.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamDefault", __LINE__, __WFILE__, NULL, L"", NULL);
        break;
    }

    //Return the information.
    dDefault = info.deflt;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemProjectionInformation.GetParamDefault")
    return dDefault;
}

//Gets the "logical type" of the specified parameter for
//the specified projection; this information could be
//used, for example, to tell a UI how to format an edit
//box for entering values.
//Throws an exception if nProjectionCode is not a valid
//projection, or MgCoordinateSystemMismatchException if ulIndex does not
//specify a parameter which this projection uses.
//
//Value returned is one of the values defined in the class MgCoordinateSystemProjectionLogicalType
//
INT32 CCoordinateSystemProjectionInformation::GetParameterLogicalType(INT32 nProjectionCode, UINT32 ulIndex)
{
    INT32 logicalType = MgCoordinateSystemProjectionLogicalType::None;

    MG_TRY()

    if (MgCoordinateSystemProjectionCode::Unknown == nProjectionCode)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamLogicalType", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    assert((ulIndex>=1) && (ulIndex<=knCsNumParams));
    if ((ulIndex < 1) || (ulIndex > knCsNumParams))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamLogicalType", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Get information about the parameter
    cs_Prjprm_ info;
    int nResult = CS_prjprm(&info, nProjectionCode, ulIndex-1);
    switch(nResult)
    {
    case 1:
        //The projection uses this parameter.
        break;
    case 0:
        //The projection does not use this parameter.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamLogicalType", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
        break;
    case -1:
    default:
        //Not a valid projection.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamLogicalType", __LINE__, __WFILE__, NULL, L"", NULL);
        break;
    }

    //Return the information.
    logicalType = static_cast<INT32>(info.log_type);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemProjectionInformation.GetParamLogicalType")
    return logicalType;
}

//Gets the "format type" of the specified parameter for the specified
//projection; this information could be used, for example, to tell a
//UI how to format an edit box for entering values.
//Throws an exception if nProjectionCode is not a valid
//projection, or MgCoordinateSystemMismatchException if ulIndex does not specify a
//parameter which this projection uses.
//
//Returns one of the values defined in the class MgCoordinateSystemProjectionFormatType
//
INT32 CCoordinateSystemProjectionInformation::GetParameterFormatType(INT32 nProjectionCode, UINT32 ulIndex)
{
    INT32 formatType = MgCoordinateSystemProjectionFormatType::None;

    MG_TRY()

    if (MgCoordinateSystemProjectionCode::Unknown == nProjectionCode)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamFormatType", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    assert((ulIndex>=1) && (ulIndex<=knCsNumParams));
    if ((ulIndex < 1) || (ulIndex > knCsNumParams))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamFormatType", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Get information about the parameter
    cs_Prjprm_ info;
    int nResult = CS_prjprm(&info, nProjectionCode, ulIndex-1);
    switch(nResult)
    {
    case 1:
        //The projection uses this parameter.
        break;
    case 0:
        //The projection does not use this parameter.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamFormatType", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
        break;
    case -1:
    default:
        //Not a valid projection.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamFormatType", __LINE__, __WFILE__, NULL, L"", NULL);
        break;
    }

    //Return the information.
    formatType = static_cast<INT32>(info.format);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemProjectionInformation.GetParamFormatType")

    return formatType;
}

//Gets the type of the specified parameter for the specified
//projection (i.e., is it longitude of central meridian, latitude
//of standard parallel, etc.); this information could be used, for
//example, to tell a UI how to label an edit box for entering
//values.
//Throws an exception if nProjectionCode is not a valid projection, or MgCoordinateSystemMismatchException if
//ulIndex does not specify a parameter which this projection uses.
//
//Return one of the values defined in the class MgCoordinateSystemProjectionParameterType
//
INT32 CCoordinateSystemProjectionInformation::GetParameterType(INT32 nProjectionCode, UINT32 ulIndex)
{
    INT32 paramType=MgCoordinateSystemProjectionParameterType::NotUsed;
    MG_TRY()

    if (MgCoordinateSystemProjectionCode::Unknown == nProjectionCode)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamType", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    assert((ulIndex>=1) && (ulIndex<=knCsNumParams));
    if ((ulIndex < 1) || (ulIndex > knCsNumParams))
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamType", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //MENTOR_MAINTENANCE
    //The following table-searching routine is cut-and-pasted
    //straight out of the Mentor method CS_prjprm, then cleaned up a bit.
    int nIndex;
    struct cs_PrjprmMap_ *mp;
    short sPrjCode = nProjectionCode;

    for (mp = cs_PrjprmMap; mp->prj_code != cs_PRJCOD_END; mp++)
    {
        if (mp->prj_code == sPrjCode) break;
    }
    if (mp->prj_code == cs_PRJCOD_END)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamType", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    nIndex = mp->prm_types[ulIndex-1];
    if (nIndex <= 0)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetParamType", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    paramType = static_cast<INT32>(nIndex);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemProjectionInformation.GetParamType")

    return paramType;
}

//Gets the unit type (linear or angular) of the specified
//projection.  Returns kCsUnitTypeLinear if the projection
//uses linear units; kCsUnitTypeAngular if the projection
//uses angular units; or kCsUnitTypeUnknown if nProjectionCode does
//not specify a valid projection.
//
INT32 CCoordinateSystemProjectionInformation::GetUnitType(INT32 nProjectionCode)
{
    return GetProjectionUnitType(nProjectionCode);
}

//Gets Mentor's "tag string" for the specified projection
//(i.e., "LL" for Lat/Long, "TM" for Transverse Mercator,
//etc.).  The caller is responsible for freeing the
//returned string via ::SysFreeString().  If nProjectionCode is
//not a valid projection, a null pointer will be returned.
//NOTE:  The returned string value is not localized, and
//client apps should not use it for UI display.  This
//function is provided mainly for debugging and utility
//purposes.
//
STRING CCoordinateSystemProjectionInformation::GetTagString(INT32 nProjectionCode)
{
    if (MgCoordinateSystemProjectionCode::Unknown == nProjectionCode)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemProjectionInformation.GetTagString", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    wchar_t* pszTag=Convert_UTF8_To_Wide(StringFromProjection(nProjectionCode));
    if (NULL == pszTag)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemProjectionInformation.GetTagString", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING str(pszTag);
    delete[] pszTag;

    return str;
}
