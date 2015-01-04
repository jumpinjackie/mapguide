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

#include "CoordSysEnumInteger32.h"      //for CCoordinateSystemEnumInteger32
#include "CoordSysUnitInformation.h"    //for CCoordinateSystemUnitInformation
#include "CoordSysUtil.h"               //for Convert_UTF8_To_Wide
#include "MentorUtil.h"                 //for IsLegalMentorName()

using namespace CSLibrary;

//External globals from Mentor
extern "C"
{
    extern struct cs_Unittab_ cs_Unittab [];
}

//MENTOR_MAINTENANCE:  The following two constants list
//the number of different linear and angular units which
//the public API supports.  These will need to be adjusted if Mentor
//adds more units.
const INT32 klNumLinearUnits = 53;
const INT32 klNumAngularUnits = 11;


//MENTOR_MAINTENANCE:  This is a list of all linear units
//that Mentor supports.  It will need to be adjusted if
//Mentor adds more units.
//in struct cs_Unittab_ cs_Unittab [] = ...
//
//Values are picked up from CSdataU.c
//
INT32 kLinearUnits[klNumLinearUnits] = {
    MgCoordinateSystemUnitCode::Meter,
    MgCoordinateSystemUnitCode::Foot,
    MgCoordinateSystemUnitCode::Inch,
    MgCoordinateSystemUnitCode::IFoot,
    MgCoordinateSystemUnitCode::ClarkeFoot,
    MgCoordinateSystemUnitCode::IInch,
    MgCoordinateSystemUnitCode::Centimeter,
    MgCoordinateSystemUnitCode::Kilometer,
    MgCoordinateSystemUnitCode::Yard,
    MgCoordinateSystemUnitCode::SearsYard,
    MgCoordinateSystemUnitCode::Mile,
    MgCoordinateSystemUnitCode::IYard,
    MgCoordinateSystemUnitCode::IMile,
    MgCoordinateSystemUnitCode::Knot,
    MgCoordinateSystemUnitCode::NautM,
    MgCoordinateSystemUnitCode::Lat66,
    MgCoordinateSystemUnitCode::Lat83,
    MgCoordinateSystemUnitCode::Decimeter,
    MgCoordinateSystemUnitCode::Millimeter,
    MgCoordinateSystemUnitCode::Dekameter,
    MgCoordinateSystemUnitCode::Hectometer,
    MgCoordinateSystemUnitCode::GermanMeter,
    MgCoordinateSystemUnitCode::CaGrid,
    MgCoordinateSystemUnitCode::ClarkeChain,
    MgCoordinateSystemUnitCode::GunterChain,
    MgCoordinateSystemUnitCode::BenoitChain,
    MgCoordinateSystemUnitCode::SearsChain,
    MgCoordinateSystemUnitCode::ClarkeLink,
    MgCoordinateSystemUnitCode::GunterLink,
    MgCoordinateSystemUnitCode::BenoitLink,
    MgCoordinateSystemUnitCode::SearsLink,
    MgCoordinateSystemUnitCode::Rod,
    MgCoordinateSystemUnitCode::Perch,
    MgCoordinateSystemUnitCode::Pole,
    MgCoordinateSystemUnitCode::Furlong,
    MgCoordinateSystemUnitCode::Rood,
    MgCoordinateSystemUnitCode::CapeFoot,
    MgCoordinateSystemUnitCode::Brealey,
    MgCoordinateSystemUnitCode::SearsFoot,
    MgCoordinateSystemUnitCode::GoldCoastFoot,
    MgCoordinateSystemUnitCode::MicroInch,
    MgCoordinateSystemUnitCode::IndianYard,
    MgCoordinateSystemUnitCode::IndianFoot,
    MgCoordinateSystemUnitCode::IndianFt37,
    MgCoordinateSystemUnitCode::IndianFt62,
    MgCoordinateSystemUnitCode::IndianFt75,
    MgCoordinateSystemUnitCode::IndianYd37,
    MgCoordinateSystemUnitCode::Decameter,
    MgCoordinateSystemUnitCode::InternationalChain,
    MgCoordinateSystemUnitCode::InternationalLink,
    MgCoordinateSystemUnitCode::BrFootTrunc,
    MgCoordinateSystemUnitCode::BrChainTrunc,
    MgCoordinateSystemUnitCode::BrLinkTrunc,
};

//MENTOR_MAINTENANCE:  This is a list of all angular units
//that Mentor supports.  It will need to be adjusted if
//Mentor adds more units.
//in struct cs_Unittab_ cs_Unittab [] = ...
//
//Values are picked up from CSdataU.c
//
INT32 kAngularUnits[klNumAngularUnits] = {
    MgCoordinateSystemUnitCode::Degree,
    MgCoordinateSystemUnitCode::Grad,
    MgCoordinateSystemUnitCode::Grade,
    MgCoordinateSystemUnitCode::MapInfo,
    MgCoordinateSystemUnitCode::Mil,
    MgCoordinateSystemUnitCode::Minute,
    MgCoordinateSystemUnitCode::Radian,
    MgCoordinateSystemUnitCode::Second,
    MgCoordinateSystemUnitCode::Decisec,
    MgCoordinateSystemUnitCode::Centisec,
    MgCoordinateSystemUnitCode::Millisec
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemUnitInformation::CCoordinateSystemUnitInformation()
{
}

CCoordinateSystemUnitInformation::~CCoordinateSystemUnitInformation()
{
}

//MgDisposable
void CCoordinateSystemUnitInformation::Dispose()
{
    delete this;
}

//Private member functions
double CCoordinateSystemUnitInformation::GetScale(INT32 unit, INT32 desiredUnitType)
{
    double dScale=0.;
    MG_TRY()

    INT32 unitType;
    if (!GetUnitInfo(unit, &unitType, &dScale))
    {
        MgStringCollection arguments;
        arguments.Add(L""); // dummy argument
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemUnitInformation.GetScale", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemUnknownUnit", NULL);
    }

    if (desiredUnitType != unitType)
    {
        MgStringCollection arguments;
        arguments.Add(L""); // dummy argument
        throw new MgCoordinateSystemLoadFailedException(L"MgCoordinateSystemUnitInformation.GetScale", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemNotLinearUnit", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemUnitInformation.GetScale")

    return dScale;
}

//Gets an enumerator for listing all the valid linear units
//supported by the public API.  Caller is responsible for releasing the
//enumerator when done with it.
//
MgCoordinateSystemEnumInteger32* CCoordinateSystemUnitInformation::GetEnumLinearUnits()
{
    Ptr<CCoordinateSystemEnumInteger32> pNewEnum;

    MG_TRY()

    pNewEnum = new CCoordinateSystemEnumInteger32;

    if (NULL == pNewEnum.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemUnitInformation.GetEnumLinearUnits", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pNewEnum->SetList(kLinearUnits, klNumLinearUnits);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemUnitInformation.GetEnumLinearUnits")

    return pNewEnum.Detach();
}

//Gets an enumerator for listing all the valid angular units
//supported by the public API.  Caller is responsible for releasing
//the enumerator when done with it.
//
MgCoordinateSystemEnumInteger32* CCoordinateSystemUnitInformation::GetEnumAngularUnits()
{
    Ptr<CCoordinateSystemEnumInteger32> pNewEnum;

    MG_TRY()

    pNewEnum = new CCoordinateSystemEnumInteger32;

    if (NULL == pNewEnum.p)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemUnitInformation.GetEnumAngularUnits", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pNewEnum->SetList(kAngularUnits, klNumAngularUnits);

    MG_CATCH_AND_THROW(L"MgCoordinateSystemUnitInformation.GetEnumAngularUnits")

    return pNewEnum.Detach();
}

//Gets the scale factor for the specified linear unit type; this is
//the number which one multiplies a value by to get meters.
//Throws an exception if unit is not a valid linear unit.
//
double CCoordinateSystemUnitInformation::GetLinearUnitScale(INT32 unit)
{
    return GetScale(unit, MgCoordinateSystemUnitType::Linear);
}

//Gets the scale factor for the specified angular unit type; this
//is the number which one multiplies a value by to get degrees.
//Throws an exception if unit is not a valid
//angular unit.
//
double CCoordinateSystemUnitInformation::GetAngularUnitScale(INT32 unit)
{
    return GetScale(unit, MgCoordinateSystemUnitType::Angular);
}

//Gets the unit type of the specified unit; the result will be
//kCsUnitTypeLinear if a linear unit, kCsUnitTypeAngular if an angular
//unit, or kCsUnitTypeUnknown if unit is not a valid unit.
//
INT32 CCoordinateSystemUnitInformation::GetUnitType(INT32 unit)
{
    INT32 unitType=MgCoordinateSystemUnitType::Unknown;
    if (!GetUnitInfo(unit, &unitType, NULL))
    {
        return MgCoordinateSystemUnitType::Unknown;
    }

    return unitType;
}

//Gets Mentor's "tag string" for the specified unit (i.e., "Meter"
//for meters, "IFoot" for international feet, etc.).  The caller is
//responsible for freeing the returned string via ::SysFreeString().
//If unit is not a valid unit, a null pointer will be returned.
//NOTE:  The returned string value is not localized, and client apps
//should not use it for UI display.  This function is provided mainly
//for debugging and utility purposes.
//
STRING CCoordinateSystemUnitInformation::GetTagString(INT32 unit)
{
    wchar_t* pszTag = Convert_UTF8_To_Wide(StringFromUnit(unit));
    if (NULL == pszTag)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemUnitInformation.GetTagString", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING str(pszTag);
    delete[] pszTag;

    return str;
}

//Gets Mentor's string abbreviation for the specified unit , or a
//NULL pointer if no short form is supplied.  Caller is responsible
//for freeing the returned string via ::SysFreeString().  NOTE:
//The returned string value is not localized, and client apps
//should not use it for UI display.  This function is provided
//mainly for debugging and utility purposes.
//
STRING CCoordinateSystemUnitInformation::GetAbbreviation(INT32 unit)
{
    MG_TRY()

    //Get a string for the unit
    const char *kpTag = StringFromUnit(unit);
    if (NULL == kpTag)
    {
        //Unknown unit type.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemUnitInformation.GetAbbreviation", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Look it up in Mentor's table
    for (int i=0; cs_Unittab[i].type != cs_UTYP_END; i++)
    {
        if (0 == CS_stricmp(kpTag, cs_Unittab[i].name))
        {
            //Found a match!
            const char *kpAbbrev = cs_Unittab[i].abrv;
            STRING sAbbrev;
            if (0 != strcmp(kpAbbrev, " z"))
            {
                //The unit has an abbreviation.
                wchar_t* pAbbrev = Convert_UTF8_To_Wide(kpAbbrev);
                sAbbrev=pAbbrev;
                delete[] pAbbrev;
            }
            //success
            return sAbbrev;
        }    //if the unit matches
    }    //for each unit in the table

    //We've reached the end of the table without finding a match.
    //This should never happen, since the call to StringFromUnit()
    //above should catch that possibility.  The only way this
    //could happen would be if Mentor and the public API source code got
    //out of sync with each other.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemUnitInformation.GetAbbreviation")

    assert(0);
    return L"";
}
