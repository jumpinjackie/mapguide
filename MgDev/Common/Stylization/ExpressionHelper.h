/*
 * Copyright (C) 2007 by Autodesk, Inc. All Rights Reserved.
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
    static FdoExpressionEngineFunctionCollection* GetCustomFunctions(Renderer* renderer, RS_FeatureReader* reader);

    static bool GetAsBoolean(FdoLiteralValue* literalValue);
    static int GetAsInt32(FdoLiteralValue* literalValue);
    static double GetAsDouble(FdoLiteralValue* literalValue);
    static const wchar_t* GetAsString(FdoLiteralValue* literalValue);

    static bool GetAsBoolean(FdoDataValue* dataValue);
    static int GetAsInt32(FdoDataValue* dataValue);
    static double GetAsDouble(FdoDataValue* dataValue);
    static const wchar_t* GetAsString(FdoDataValue* dataValue);
};

#endif
