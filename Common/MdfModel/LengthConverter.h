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

#ifndef SIZELENGTHCONVERSION_H_
#define SIZELENGTHCONVERSION_H_

#include "MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

    // Denotes the measurement units in which linear sizes are specified.
    enum MDFMODEL_API LengthUnit
    {
        Millimeters,
        Centimeters,
        Meters,
        Kilometers,
        Inches,
        Feet,
        Yards,
        Miles,
        Points
    };

    class MDFMODEL_API LengthConverter
    {
        public:

            static double UnitToMeters(LengthUnit unit, double val);
            static double MetersToUnit(LengthUnit unit, double val);
            static LengthUnit EnglishToUnit(const wchar_t* english);
            static MdfString* UnitToEnglish(LengthUnit units);

    };

END_NAMESPACE_MDFMODEL
#endif // SIZELENGTHCONVERSION_H_
