//
//  Copyright (C) 2010-2011 by Autodesk, Inc.
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

#include "stdafx.h"
#include "IOWatermarkOffsetUnit.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


WatermarkOffset::WatermarkOffsetUnit IOWatermarkOffsetUnit::ParseWatermarkOffsetUnit(const wchar_t* ch)
{
    if (::wcscmp(ch, L"Inches") == 0) // NOXLATE
        return WatermarkOffset::Inches;
    else if (::wcscmp(ch, L"Centimeters") == 0) // NOXLATE
        return WatermarkOffset::Centimeters;
    else if (::wcscmp(ch, L"Millimeters") == 0) // NOXLATE
        return WatermarkOffset::Millimeters;
    else if (::wcscmp(ch, L"Points") == 0) // NOXLATE
        return WatermarkOffset::Points;
    else // treat as "Pixels" if string is incorrect
        return WatermarkOffset::Pixels;
}


void IOWatermarkOffsetUnit::Write(MdfStream& fd, WatermarkOffset::WatermarkOffsetUnit offsetUnit, MgTab& tab)
{
    // Property: Unit
    fd << tab.tab() << startStr("Unit"); // NOXLATE
    if (offsetUnit == WatermarkOffset::Inches)
        fd << "Inches"; // NOXLATE
    else if (offsetUnit == WatermarkOffset::Centimeters)
        fd << "Centimeters"; // NOXLATE
    else if (offsetUnit == WatermarkOffset::Millimeters)
        fd << "Millimeters"; // NOXLATE
    else if (offsetUnit == WatermarkOffset::Points)
        fd << "Points"; // NOXLATE
    else  // treat "Pixels" as default value
        fd << "Pixels"; // NOXLATE
    fd << endStr("Unit") << std::endl; // NOXLATE
}
