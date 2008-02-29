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

#ifndef _ARBITRARYCOORDSYS_H_
#define _ARBITRARYCOORDSYS_H_

#ifdef _WIN32
#pragma warning( disable: 4251 )
#else
#define _wcsnicmp wcsncasecmp
#define _wcsicmp wcscasecmp
#define _stricmp  strcasecmp
#define _strnicmp strncasecmp
#endif

#include <string>
#include <vector>

namespace CSLibrary
{
const char ARBITRARY_XY[] = "*X-Y*";         // NOXLATE
const char NERTH[]        = "NERTH";         // NOXLATE
const char LOCALCS[]      = "LOCALCS";       // NOXLATE
const char LOCAL_CS[]     = "LOCAL_CS";      // NOXLATE
const char NON_EARTH[]    = "Non-Earth";     // NOXLATE
const char NONEARTH[]     = "Nonearth";      // NOXLATE
const char UNIT[]         = "UNIT";          // NOXLATE

struct ArbitraryCoordinateSystem {
    wchar_t code[50];
    wchar_t unitsCode[50];
    double conversionToMeters;
};

///////////////////////////////////////////////////////////////////////////////
class CCsArbitraryCoordinateSystemUtil
{
public:
    // Expose static helper methods for applications that might want to work with the Mentor structure directly
    static bool IsArbitrary(const wchar_t* wszWktCoordinateSystem);

    // Arbitrary X-Y Coordinate Support
    static const STRING ArbitraryXYCategoryDescription;
    static const STRING ArbitraryXYDescription;
    static const STRING ArbitraryXYProjection;
    static const STRING ArbitraryXYProjectionDescription;
    static const STRING ArbitraryXYDatum;
    static const STRING ArbitraryXYDatumDescription;
    static const STRING ArbitraryXYEllipsoid;
    static const STRING ArbitraryXYEllipsoidDescription;
    static const int ArbitraryXYCoordinateSystemsCount;
    static const ArbitraryCoordinateSystem ArbitraryXYCoordinateSystems[];
};

} // End of namespace

#endif //_CCOORDINATESYSTEM_H_
