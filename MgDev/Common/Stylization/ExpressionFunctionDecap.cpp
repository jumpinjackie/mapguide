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
#include "ExpressionFunctionDecap.h"
#include <wctype.h>


ExpressionFunctionDecap::ExpressionFunctionDecap()
{
    m_decapValue = FdoStringValue::Create();
    m_functionDefinition = NULL;
}


ExpressionFunctionDecap::~ExpressionFunctionDecap()
{
    m_decapValue->Release();
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionDecap* ExpressionFunctionDecap::Create()
{
    return new ExpressionFunctionDecap();
}


FdoFunctionDefinition* ExpressionFunctionDecap::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        FdoPtr<FdoArgumentDefinition> arg = FdoArgumentDefinition::Create(L"strValue", L"String to format", FdoDataType_String);

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        args->Add(arg);

        m_functionDefinition = FdoFunctionDefinition::Create(L"DECAP",
                                                             L"String formatting function",
                                                             FdoDataType_String,
                                                             args,
                                                             FdoFunctionCategoryType_String);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionDecap::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have one argument
    if (literalValues->GetCount() != 1)
        throw FdoExpressionException::Create(L"Incorrect number of arguments for function DECAP");

    FdoPtr<FdoDataValue> arg = (FdoDataValue*)literalValues->GetItem(0);

    wchar_t* str = NULL;
    if (arg->GetDataType() == FdoDataType_String)
        str = (wchar_t*)((FdoStringValue*)arg.p)->GetString();
    else
        str = (wchar_t*)arg->ToString();

    size_t len = str? wcslen(str) + 1 : 0;

    if (len > 1)
    {
        wchar_t* nval = new wchar_t[len];

        wchar_t* dst = nval;
        for (wchar_t *src=str; *src;)
        {
            if (src == str || (*(src-1) == L' '))
                *dst++ = towupper(*src++);
            else
                *dst++ = towlower(*src++);
        }

        *dst = 0;

        m_decapValue->SetString(nval);

        delete [] nval;
    }
    else
    {
        m_decapValue->SetString(L"");
    }

    m_decapValue->AddRef();
    return m_decapValue;
}


FdoExpressionEngineIFunction* ExpressionFunctionDecap::CreateObject()
{
    return new ExpressionFunctionDecap();
}


void ExpressionFunctionDecap::Dispose()
{
    delete this;
}
