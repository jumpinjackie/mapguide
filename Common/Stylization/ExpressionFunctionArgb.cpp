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


ExpressionFunctionArgb::ExpressionFunctionArgb()
{
    m_argbValue = FdoInt32Value::Create();
    m_functionDefinition = NULL;
}


ExpressionFunctionArgb::~ExpressionFunctionArgb()
{
    m_argbValue->Release();
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

    FdoPtr<FdoDataValue> arg1 = (FdoDataValue*)literalValues->GetItem(0);
    FdoPtr<FdoDataValue> arg2 = (FdoDataValue*)literalValues->GetItem(1);
    FdoPtr<FdoDataValue> arg3 = (FdoDataValue*)literalValues->GetItem(2);
    FdoPtr<FdoDataValue> arg4 = (FdoDataValue*)literalValues->GetItem(3);

    FdoInt32 alpha = 0;
    if (arg1->GetDataType() == FdoDataType_Int32)
        alpha = ((FdoInt32Value*)arg1.p)->GetInt32();
//    else
//      alpha = arg1->ToString();

    FdoInt32 red = 0;
    if (arg1->GetDataType() == FdoDataType_Int32)
        red = ((FdoInt32Value*)arg1.p)->GetInt32();
//    else
//      red = arg1->ToString();

    FdoInt32 green = 0;
    if (arg1->GetDataType() == FdoDataType_Int32)
        green = ((FdoInt32Value*)arg1.p)->GetInt32();
//    else
//      green = arg1->ToString();

    FdoInt32 blue = 0;
    if (arg1->GetDataType() == FdoDataType_Int32)
        blue = ((FdoInt32Value*)arg1.p)->GetInt32();
//    else
//      blue = arg1->ToString();

    // clean up
    red   &= 0xFF;
    green &= 0xFF;
    blue  &= 0xFF;
    alpha &= 0xFF;

    FdoInt32 color = (alpha << 24) | (red << 16) | (green << 8) | blue;

    m_argbValue->SetInt32(color);
    m_argbValue->AddRef();
    return m_argbValue;
}


FdoExpressionEngineIFunction* ExpressionFunctionArgb::CreateObject()
{
    return new ExpressionFunctionArgb();
}


void ExpressionFunctionArgb::Dispose()
{
    delete this;
}
