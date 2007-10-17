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

#include "stdafx.h"
#include "ExpressionFunctionRange.h"
#include "ExpressionHelper.h"


ExpressionFunctionRange::ExpressionFunctionRange()
{
    m_functionDefinition = NULL;
}


ExpressionFunctionRange::~ExpressionFunctionRange()
{
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionRange* ExpressionFunctionRange::Create()
{
    return new ExpressionFunctionRange();
}


FdoFunctionDefinition* ExpressionFunctionRange::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        FdoPtr<FdoArgumentDefinition> arg1 = FdoArgumentDefinition::Create(L"expression", L"key", FdoDataType_String);
        FdoPtr<FdoArgumentDefinition> arg2 = FdoArgumentDefinition::Create(L"defaultValue", L"default value returned if expression does not fall into any of the ranges", FdoDataType_String);
        FdoPtr<FdoArgumentDefinition> arg3 = FdoArgumentDefinition::Create(L"...", L"variable number of min/max/value triplets", FdoDataType_String);

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        args->Add(arg1);
        args->Add(arg2);
        args->Add(arg3);

        m_functionDefinition = FdoFunctionDefinition::Create(L"RANGE",
                                                             L"Range table for style theming",
                                                             FdoDataType_String,
                                                             args,
                                                             FdoFunctionCategoryType_Custom);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionRange::Evaluate(FdoLiteralValueCollection *literalValues)
{
    // make sure we have 2 or more arguments, and that there are 2 plus a multiple of 3
    if (literalValues->GetCount() < 2 || (literalValues->GetCount()-2) % 3 != 0)
        throw FdoExpressionException::Create(L"Incorrect number of arguments for function RANGE");

    // the first parameter is the key we seek
    FdoPtr<FdoLiteralValue> key = literalValues->GetItem(0);

    // look for our key in remaining pairs, return matching value if found
    for (int i=2; i<literalValues->GetCount(); i += 3)
    {
        FdoPtr<FdoLiteralValue> minKey = literalValues->GetItem(i);
        FdoPtr<FdoLiteralValue> maxKey = literalValues->GetItem(i+1);
        if (ExpressionHelper::Compare(key, minKey) >= 0 && ExpressionHelper::Compare(key, maxKey) < 0)
            return literalValues->GetItem(i+2);
    }

    // we didn't find a match - return the default
    return literalValues->GetItem(1);
}


FdoExpressionEngineIFunction* ExpressionFunctionRange::CreateObject()
{
    return ExpressionFunctionRange::Create();
}


void ExpressionFunctionRange::Dispose()
{
    delete this;
}
