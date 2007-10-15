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
#include "ExpressionFunctionLayerId.h"


ExpressionFunctionLayerId::ExpressionFunctionLayerId(const wchar_t* layerId)
{
    m_layerIdValue = FdoStringValue::Create(layerId);
    m_functionDefinition = NULL;
}


ExpressionFunctionLayerId::~ExpressionFunctionLayerId()
{
    m_layerIdValue->Release();
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
        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        m_functionDefinition = FdoFunctionDefinition::Create(L"LAYERID",
                                                             L"Returns the active layer Id",
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
        throw FdoExpressionException::Create(L"Incorrect number of arguments for function LAYERID");

    m_layerIdValue->AddRef();
    return m_layerIdValue;
}


FdoExpressionEngineIFunction* ExpressionFunctionLayerId::CreateObject()
{
    return new ExpressionFunctionLayerId(L"");
}


void ExpressionFunctionLayerId::Dispose()
{
    delete this;
}
