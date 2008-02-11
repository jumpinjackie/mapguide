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

#include "stdafx.h"
#include "ExpressionFunctionLayerId.h"
#include "Foundation.h"


ExpressionFunctionLayerId::ExpressionFunctionLayerId(const wchar_t* layerId)
{
    m_layerIdValue = FdoStringValue::Create(layerId);
    m_functionDefinition = NULL;
}


ExpressionFunctionLayerId::~ExpressionFunctionLayerId()
{
    FDO_SAFE_RELEASE(m_layerIdValue);
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionLayerId* ExpressionFunctionLayerId::Create(const wchar_t* layerId)
{
    return new ExpressionFunctionLayerId(layerId);
}


FdoFunctionDefinition* ExpressionFunctionLayerId::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionLAYERID_Description");

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        m_functionDefinition = FdoFunctionDefinition::Create(L"LAYERID", // NOXLATE
                                                             funcDesc.c_str(),
                                                             FdoDataType_String,
                                                             args,
                                                             FdoFunctionCategoryType_String);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionLayerId::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have zero arguments
    if (literalValues->GetCount() != 0)
    {
        MgResources* resources = MgResources::GetInstance();
        assert(NULL != resources);

        STRING message = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgIncorrectNumberOfArguments");
        MgStringCollection arguments;
        arguments.Add(L"LAYERID"); // NOXLATE
        message = resources->FormatMessage(message, &arguments);

        throw FdoExpressionException::Create(message.c_str());
    }

    return FDO_SAFE_ADDREF(m_layerIdValue);
}


FdoExpressionEngineIFunction* ExpressionFunctionLayerId::CreateObject()
{
    return ExpressionFunctionLayerId::Create(m_layerIdValue->GetString());
}


void ExpressionFunctionLayerId::Dispose()
{
    delete this;
}
