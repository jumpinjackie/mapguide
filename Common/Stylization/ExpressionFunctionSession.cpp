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
#include "ExpressionFunctionSession.h"


ExpressionFunctionSession::ExpressionFunctionSession(const wchar_t* session)
{
    m_sessionValue = FdoStringValue::Create(session);
    m_functionDefinition = NULL;
}


ExpressionFunctionSession::~ExpressionFunctionSession()
{
    m_sessionValue->Release();
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionSession* ExpressionFunctionSession::Create(const wchar_t* session)
{
    return new ExpressionFunctionSession(session);
}


FdoFunctionDefinition* ExpressionFunctionSession::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        m_functionDefinition = FdoFunctionDefinition::Create(L"SESSION",
                                                             L"Returns the active session",
                                                             FdoDataType_String,
                                                             args,
                                                             FdoFunctionCategoryType_String);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionSession::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have zero arguments
    if (literalValues->GetCount() != 0)
        throw FdoExpressionException::Create(L"Incorrect number of arguments for function SESSION");

    m_sessionValue->AddRef();
    return m_sessionValue;
}


FdoExpressionEngineIFunction* ExpressionFunctionSession::CreateObject()
{
    return new ExpressionFunctionSession(L"");
}


void ExpressionFunctionSession::Dispose()
{
    delete this;
}
