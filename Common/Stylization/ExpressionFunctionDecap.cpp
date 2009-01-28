/*
 * Copyright (C) 2007-2009 by Autodesk, Inc. All Rights Reserved.
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
#include "ExpressionHelper.h"
#include "Foundation.h"
#include <wctype.h>


ExpressionFunctionDecap::ExpressionFunctionDecap()
{
    m_decapValue = FdoStringValue::Create();
    m_functionDefinition = NULL;
}


ExpressionFunctionDecap::~ExpressionFunctionDecap()
{
    FDO_SAFE_RELEASE(m_decapValue);
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
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionDECAP_Description");
        STRING sValDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionDECAP_StringValueDescription");

        FdoPtr<FdoArgumentDefinition> arg = FdoArgumentDefinition::Create(L"strValue", sValDesc.c_str(), FdoDataType_String); // NOXLATE

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        args->Add(arg);

        m_functionDefinition = FdoFunctionDefinition::Create(L"DECAP", // NOXLATE
                                                             funcDesc.c_str(),
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
    {
        MgResources* resources = MgResources::GetInstance();
        assert(NULL != resources);

        STRING message = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgIncorrectNumberOfArguments");
        MgStringCollection arguments;
        arguments.Add(L"DECAP"); // NOXLATE
        message = resources->FormatMessage(message, &arguments);

        throw FdoExpressionException::Create(message.c_str());
    }

    FdoPtr<FdoLiteralValue> arg = literalValues->GetItem(0);

    const wchar_t* str = ExpressionHelper::GetAsString(arg);

    size_t len = str? wcslen(str) + 1 : 0;

    if (len > 1)
    {
        wchar_t* nval = new wchar_t[len];

        wchar_t* dst = nval;
        for (const wchar_t *src=str; *src;)
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

    return FDO_SAFE_ADDREF(m_decapValue);
}


FdoExpressionEngineIFunction* ExpressionFunctionDecap::CreateObject()
{
    return ExpressionFunctionDecap::Create();
}


void ExpressionFunctionDecap::Dispose()
{
    delete this;
}
