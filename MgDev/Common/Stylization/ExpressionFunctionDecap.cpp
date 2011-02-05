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
                                                             FdoFunctionCategoryType_String,
                                                             false);
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
