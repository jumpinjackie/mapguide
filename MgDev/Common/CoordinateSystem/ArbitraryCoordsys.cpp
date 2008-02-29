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

#include "CoordSysUtil.h"                       //for Convert_Wide_To_Ascii
#include "ArbitraryCoordsys.h"

using namespace CSLibrary;

// Arbitrary X-Y Coordinate Support
const STRING CCsArbitraryCoordinateSystemUtil::ArbitraryXYCategoryDescription   = L"Arbitrary X-Y Coordinate Systems";
const STRING CCsArbitraryCoordinateSystemUtil::ArbitraryXYDescription           = L"Arbitrary X-Y Coordinates";
const STRING CCsArbitraryCoordinateSystemUtil::ArbitraryXYProjection            = L"NERTH";  // NOXLATE
const STRING CCsArbitraryCoordinateSystemUtil::ArbitraryXYProjectionDescription = L"Non-georeferenced (aka non-earth) coordinate system";
const STRING CCsArbitraryCoordinateSystemUtil::ArbitraryXYDatum                 = L"Local Datum";
const STRING CCsArbitraryCoordinateSystemUtil::ArbitraryXYDatumDescription      = L"";
const STRING CCsArbitraryCoordinateSystemUtil::ArbitraryXYEllipsoid             = L"";
const STRING CCsArbitraryCoordinateSystemUtil::ArbitraryXYEllipsoidDescription  = L"";
const int CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount = 39;
const ArbitraryCoordinateSystem CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystems[CCsArbitraryCoordinateSystemUtil::ArbitraryXYCoordinateSystemsCount] =
{
    {L"XY-M",   L"Meter",            1.0},                        // NOXLATE
    {L"XY-FT",  L"Foot",             0.30480060960121920243},     // NOXLATE
    {L"XY-IN",  L"Inch",             0.0254000508001016002},      // NOXLATE
    {L"XY-IFT", L"IFoot",            0.3048},                     // NOXLATE
    {L"XY-CF",  L"ClarkeFoot",       0.3047972651151},            // NOXLATE
    {L"XY-IIN", L"IInch",            0.0254},                     // NOXLATE
    {L"XY-UI",  L"MicroInch",        0.0000254},                  // NOXLATE
    {L"XY-ML",  L"Mil",              0.0000000254},               // NOXLATE
    {L"XY-CM",  L"Centimeter",       0.01},                       // NOXLATE
    {L"XY-KM",  L"Kilometer",     1000.0},                        // NOXLATE
    {L"XY-YD",  L"Yard",             0.91440182880365760731},     // NOXLATE
    {L"XY-SY",  L"SearsYard",        0.914398414616029},          // NOXLATE
    {L"XY-MI",  L"Mile",          1609.34721869443738887477},     // NOXLATE
    {L"XY-IYD", L"IYard",            0.9144},                     // NOXLATE
    {L"XY-IMI", L"IMile",         1609.344},                      // NOXLATE
    {L"XY-KT",  L"Knot",          1852.0},                        // NOXLATE
    {L"XY-NM",  L"NautM",         1852.0},                        // NOXLATE
    {L"XY-DM",  L"Decimeter",        0.1},                        // NOXLATE
    {L"XY-MM",  L"Millimeter",       0.001},                      // NOXLATE
    {L"XY-DAM", L"Decameter",       10.0},                        // NOXLATE
    {L"XY-DK",  L"Dekameter",       10.0},                        // NOXLATE
    {L"XY-HM",  L"Hectometer",     100.0},                        // NOXLATE
    {L"XY-GM",  L"GermanMeter",      1.0000135965},               // NOXLATE
    {L"XY-CG",  L"CaGrid",           0.999738},                   // NOXLATE
    {L"XY-GC",  L"GunterChain",     20.11684023368047},           // NOXLATE
    {L"XY-CC",  L"ClarkeChain",     20.1166194976},               // NOXLATE
    {L"XY-BC",  L"BenoitChain",     20.11678249438},              // NOXLATE
    {L"XY-SC",  L"SearsChain",      20.11676512155},              // NOXLATE
    {L"XY-GL",  L"GunterLink",       0.2011684023368047},         // NOXLATE
    {L"XY-CL",  L"ClarkeLink",       0.201166194976},             // NOXLATE
    {L"XY-BL",  L"BenoitLink",       0.2011678249438},            // NOXLATE
    {L"XY-SL",  L"SearsLink",        0.2011676512155},            // NOXLATE
    {L"XY-RD",  L"Rod",              5.02921005842012},           // NOXLATE
    {L"XY-PE",  L"Perch",            5.02921005842012},           // NOXLATE
    {L"XY-PO",  L"Pole",             5.02921005842012},           // NOXLATE
    {L"XY-FU",  L"Furlong",        201.1684023368046},            // NOXLATE
    {L"XY-RO",  L"Rood",             3.778266898},                // NOXLATE
    {L"XY-CA",  L"CapeFoot",         0.3047972615},               // NOXLATE
    {L"XY-BR",  L"Brealey",        375.0}                         // NOXLATE
};

///////////////////////////////////////////////////////////////////////////////
bool CCsArbitraryCoordinateSystemUtil::IsArbitrary(const wchar_t* wszWktCoordinateSystem)
{
    if(wszWktCoordinateSystem)
    {
        char* tempSrsWkt = Convert_Wide_To_Ascii(wszWktCoordinateSystem);
        string srsWkt = tempSrsWkt;
        delete [] tempSrsWkt;
        tempSrsWkt = NULL;

        if (srsWkt.find(LOCALCS)==0 || srsWkt.find(LOCAL_CS)==0)
        {
            return true;
        }
    }
    return false;
}

