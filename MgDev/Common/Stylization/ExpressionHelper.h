//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef EXPRESSION_HELPER_H_
#define EXPRESSION_HELPER_H_

#include "Stylization.h"
#include "Renderer.h"

class ExpressionHelper
{
public:
    // Returns a collection of the custom functions defined by stylization.
    // The IF function - the first one in the collection - needs to be hooked
    // to the engine in order to evaluate correctly.
    STYLIZATION_API static FdoExpressionEngineFunctionCollection* GetExpressionEngineFunctions(Renderer* renderer, RS_FeatureReader* reader);

    // Returns an FDO expression engine configured with the custom functions
    // defined by stylization.
    STYLIZATION_API static FdoExpressionEngine* GetExpressionEngine(Renderer* renderer, RS_FeatureReader* reader);

    // Evaluates an FdoLiteralValue as a boolean, integer, double, or string
    STYLIZATION_API static bool GetAsBoolean(FdoLiteralValue* literalValue);
    STYLIZATION_API static int GetAsInt32(FdoLiteralValue* literalValue);
    STYLIZATION_API static double GetAsDouble(FdoLiteralValue* literalValue);
    STYLIZATION_API static const wchar_t* GetAsString(FdoLiteralValue* literalValue);

    // Evaluates an FdoDataValue as a boolean, integer, double, or string
    STYLIZATION_API static bool GetAsBoolean(FdoDataValue* dataValue);
    STYLIZATION_API static int GetAsInt32(FdoDataValue* dataValue);
    STYLIZATION_API static double GetAsDouble(FdoDataValue* dataValue);
    STYLIZATION_API static const wchar_t* GetAsString(FdoDataValue* dataValue);

    // Compares two FdoLiteralValue or FdoDataValue objects.
    // * returns -1 if the first value is less than the second value
    // * returns  0 if the first value equals the second value
    // * returns +1 if the first value is greater than the second value
    // * returns -2 if there was an error comparing the types
    STYLIZATION_API static int Compare(FdoLiteralValue* literalValue1, FdoLiteralValue* literalValue2);
    STYLIZATION_API static int Compare(FdoDataValue* dataValue1, FdoDataValue* dataValue2);
};

#endif
