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

//-------------------------------------------------------------------------
// DESCRIPTION:
// Provide methods to support storage and interpretation of length units
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "LengthConverter.h"

using namespace MDFMODEL_NAMESPACE;

// converts a value in the specified units into a value in meters
double LengthConverter::UnitToMeters(LengthUnit unit, double val)
{
    double outVal = 0.0;
    switch (unit)
    {
        case Millimeters:
            outVal = val * 0.001;
            break;
        case Centimeters:
            outVal = val * 0.01;
            break;
        case Meters:
            outVal = val;
            break;
        case Kilometers:
            outVal = val * 1000.0;
            break;
        case Inches:
            outVal = val * 0.0254;
            break;
        case Feet:
            outVal = val * 12.0 * 0.0254;
            break;
        case Yards:
            outVal = val * 36.0 * 0.0254;
            break;
        case Miles:
            outVal = val * 5280.0 * 12.0 * 0.0254;
            break;
        case Points:
            outVal = val / 72.0 * 0.0254;
    }
    return outVal;
}


// converts a value in meters to a value in the specified units
double LengthConverter::MetersToUnit(LengthUnit unit, double val)
{
    double outVal = 0.0;
    switch (unit)
    {
        case Millimeters:
            outVal = val * 1000.0;
            break;
        case Centimeters:
            outVal = val * 100.0;
            break;
        case Meters:
            outVal = val;
            break;
        case Kilometers:
            outVal = val * 0.001;
            break;
        case Inches:
            outVal = val / 0.0254;
            break;
        case Feet:
            outVal = val / (12.0 * 0.0254);
            break;
        case Yards:
            outVal = val / (36.0 * 0.0254);
            break;
        case Miles:
            outVal = val / (5280.0 * 12.0 * 0.0254);
            break;
        case Points:
            outVal = val * 72.0 / 0.0254;
    }
    return outVal;
}


// prints an English word describing the provided units.  these non-localized names
// are used when serializing and parsing MDF files that contain unit information.
MdfString* LengthConverter::UnitToEnglish(LengthUnit unit)
{
    wchar_t* name = L"Meters"; // default, in case of bad input

    switch (unit)
    {
        case Millimeters:
            name = L"Millimeters"; // NOXLATE
            break;
        case Centimeters:
            name = L"Centimeters"; // NOXLATE
            break;
        case Meters:
            name = L"Meters"; // NOXLATE
            break;
        case Kilometers:
            name = L"Kilometers"; // NOXLATE
            break;
        case Inches:
            name = L"Inches"; // NOXLATE
            break;
        case Feet:
            name = L"Feet"; // NOXLATE
            break;
        case Yards:
            name = L"Yards"; // NOXLATE
            break;
        case Miles:
            name = L"Miles"; // NOXLATE
            break;
        case Points:
            name = L"Points"; // NOXLATE
    }

    return new MdfString(name);
}


// analyzes the provided English word dand returns its correlating units.
// used when serializing and parsing MDF files that contain unit information.
LengthUnit LengthConverter::EnglishToUnit(const wchar_t* english)
{
    LengthUnit unit = Meters; // default, in case of bad input

    // NOTE: MdfParser::IOMarkSymbol was encountering strings with invalid leading space.
    // This code makes it work, but it shouldn't be necessary.
    size_t numChars = wcslen(english);
    wchar_t* trimmed = new wchar_t[numChars+1];
    int j = 0;
    for (size_t i=0; i<numChars; i++)
    {
        if (english[i] >= 'A' && english[i] <= 'z')
            trimmed[j++] = english[i];
    }
    trimmed[j] = 0;

    if (wcscmp(trimmed, L"Millimeters") == 0)         // NOXLATE
        unit = Millimeters;
    else if (wcscmp(trimmed, L"Centimeters") == 0)    // NOXLATE
        unit = Centimeters;
    else if (wcscmp(trimmed, L"Meters") == 0)         // NOXLATE
        unit = Meters;
    else if (wcscmp(trimmed, L"Kilometers") == 0)     // NOXLATE
        unit = Kilometers;
    else if (wcscmp(trimmed, L"Inches") == 0)         // NOXLATE
        unit = Inches;
    else if (wcscmp(trimmed, L"Feet") == 0)           // NOXLATE
        unit = Feet;
    else if (wcscmp(trimmed, L"Yards") == 0)          // NOXLATE
        unit = Yards;
    else if (wcscmp(trimmed, L"Miles") == 0)          // NOXLATE
        unit = Miles;
    else if (wcscmp(trimmed, L"Points") == 0)         // NOXLATE
        unit = Points;

    delete [] trimmed;

    return unit;
}
