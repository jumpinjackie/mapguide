//
//  Copyright (C) 2007-2013 by Autodesk, Inc.
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
#include "ExpressionFunctionHtmlColor.h"
#include "ExpressionHelper.h"
#include "Foundation.h"


ExpressionFunctionHtmlColor::ExpressionFunctionHtmlColor()
{
    m_htmlColorValue = FdoStringValue::Create();
    m_functionDefinition = NULL;
}


ExpressionFunctionHtmlColor::~ExpressionFunctionHtmlColor()
{
    FDO_SAFE_RELEASE(m_htmlColorValue);
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionHtmlColor* ExpressionFunctionHtmlColor::Create()
{
    return new ExpressionFunctionHtmlColor();
}


FdoFunctionDefinition* ExpressionFunctionHtmlColor::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionHTMLCOLOR_Description");
        STRING rValDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionHTMLCOLOR_RValueDescription");
        STRING gValDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionHTMLCOLOR_GValueDescription");
        STRING bValDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionHTMLCOLOR_BValueDescription");

        FdoPtr<FdoArgumentDefinition> arg1 = FdoArgumentDefinition::Create(L"rValue", rValDesc.c_str(), FdoDataType_Int32); // NOXLATE
        FdoPtr<FdoArgumentDefinition> arg2 = FdoArgumentDefinition::Create(L"gValue", gValDesc.c_str(), FdoDataType_Int32); // NOXLATE
        FdoPtr<FdoArgumentDefinition> arg3 = FdoArgumentDefinition::Create(L"bValue", bValDesc.c_str(), FdoDataType_Int32); // NOXLATE

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        args->Add(arg1);
        args->Add(arg2);
        args->Add(arg3);

        m_functionDefinition = FdoFunctionDefinition::Create(L"HTMLCOLOR", // NOXLATE
                                                             funcDesc.c_str(),
                                                             FdoDataType_Int32,
                                                             args,
                                                             FdoFunctionCategoryType_String,
                                                             false);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionHtmlColor::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have three arguments
    if (literalValues->GetCount() != 3)
    {
        MgResources* resources = MgResources::GetInstance();
        assert(NULL != resources);

        STRING message = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgIncorrectNumberOfArguments");
        MgStringCollection arguments;
        arguments.Add(L"HTMLCOLOR"); // NOXLATE
        message = resources->FormatMessage(message, &arguments);

        throw FdoExpressionException::Create(message.c_str());
    }

    FdoPtr<FdoLiteralValue> arg1 = literalValues->GetItem(0);
    FdoPtr<FdoLiteralValue> arg2 = literalValues->GetItem(1);
    FdoPtr<FdoLiteralValue> arg3 = literalValues->GetItem(2);

    FdoInt32   red = ExpressionHelper::GetAsInt32(arg1);
    FdoInt32 green = ExpressionHelper::GetAsInt32(arg2);
    FdoInt32  blue = ExpressionHelper::GetAsInt32(arg3);

    // clean up
    red   &= 0xFF;
    green &= 0xFF;
    blue  &= 0xFF;

    wchar_t tmp[32] = { 0 };
    std::wstring str;
    swprintf(tmp, 32, L"%0.2x", red);
    str.append(&tmp[0]);
    swprintf(tmp, 32, L"%0.2x", green);
    str.append(&tmp[0]);
    swprintf(tmp, 32, L"%0.2x", blue);
    str.append(&tmp[0]);

    m_htmlColorValue->SetString(str.c_str());

    return FDO_SAFE_ADDREF(m_htmlColorValue);
}


FdoExpressionEngineIFunction* ExpressionFunctionHtmlColor::CreateObject()
{
    return ExpressionFunctionHtmlColor::Create();
}


void ExpressionFunctionHtmlColor::Dispose()
{
    delete this;
}
