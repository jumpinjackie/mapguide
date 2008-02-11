/*
 * Copyright (C) 2007-2008 by Autodesk, Inc. All Rights Reserved.
 *
 * By using this code, you are agreeing to the terms and conditions of
 * the License Agreement included in the documentation for this code.
 *
 * AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE
 * CORRECTNESS OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE
 * IT. AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY
 * DISCLAIMS ANY LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL
 * DAMAGES FOR ERRORS, OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
 *
 * Use, duplication, or disclosure by the U.S. Government is subject
 * to restrictions set forth in FAR 52.227-19 (Commercial Computer
 * Software Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
 * (Rights in Technical Data and Computer Software), as applicable.
 */

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
