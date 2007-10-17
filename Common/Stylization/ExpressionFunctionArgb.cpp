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
#include "ExpressionFunctionArgb.h"
#include "ExpressionHelper.h"


ExpressionFunctionArgb::ExpressionFunctionArgb()
{
    m_argbValue = FdoInt32Value::Create();
    m_functionDefinition = NULL;
}


ExpressionFunctionArgb::~ExpressionFunctionArgb()
{
    FDO_SAFE_RELEASE(m_argbValue);
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionArgb* ExpressionFunctionArgb::Create()
{
    return new ExpressionFunctionArgb();
}


FdoFunctionDefinition* ExpressionFunctionArgb::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        FdoPtr<FdoArgumentDefinition> arg1 = FdoArgumentDefinition::Create(L"A-value", L"alpha value", FdoDataType_Int32);
        FdoPtr<FdoArgumentDefinition> arg2 = FdoArgumentDefinition::Create(L"R-value", L"red value", FdoDataType_Int32);
        FdoPtr<FdoArgumentDefinition> arg3 = FdoArgumentDefinition::Create(L"G-value", L"green value", FdoDataType_Int32);
        FdoPtr<FdoArgumentDefinition> arg4 = FdoArgumentDefinition::Create(L"B-value", L"blue value", FdoDataType_Int32);

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        args->Add(arg1);
        args->Add(arg2);
        args->Add(arg3);
        args->Add(arg4);

        m_functionDefinition = FdoFunctionDefinition::Create(L"ARGB",
                                                             L"Color generation function",
                                                             FdoDataType_Int32,
                                                             args,
                                                             FdoFunctionCategoryType_String);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionArgb::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have four arguments
    if (literalValues->GetCount() != 4)
        throw FdoExpressionException::Create(L"Incorrect number of arguments for function ARGB");

    FdoPtr<FdoLiteralValue> arg1 = literalValues->GetItem(0);
    FdoPtr<FdoLiteralValue> arg2 = literalValues->GetItem(1);
    FdoPtr<FdoLiteralValue> arg3 = literalValues->GetItem(2);
    FdoPtr<FdoLiteralValue> arg4 = literalValues->GetItem(3);

    FdoInt32 alpha = ExpressionHelper::GetAsInt32(arg1);
    FdoInt32   red = ExpressionHelper::GetAsInt32(arg2);
    FdoInt32 green = ExpressionHelper::GetAsInt32(arg3);
    FdoInt32  blue = ExpressionHelper::GetAsInt32(arg4);

    // clean up
    red   &= 0xFF;
    green &= 0xFF;
    blue  &= 0xFF;
    alpha &= 0xFF;

    FdoInt32 color = (alpha << 24) | (red << 16) | (green << 8) | blue;
    m_argbValue->SetInt32(color);

    return FDO_SAFE_ADDREF(m_argbValue);
}


FdoExpressionEngineIFunction* ExpressionFunctionArgb::CreateObject()
{
    return ExpressionFunctionArgb::Create();
}


void ExpressionFunctionArgb::Dispose()
{
    delete this;
}
