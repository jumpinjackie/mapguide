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
#include "ExpressionFunctionSession.h"
#include "Foundation.h"


ExpressionFunctionSession::ExpressionFunctionSession(const wchar_t* session)
{
    m_sessionValue = FdoStringValue::Create(session);
    m_functionDefinition = NULL;
}


ExpressionFunctionSession::~ExpressionFunctionSession()
{
    FDO_SAFE_RELEASE(m_sessionValue);
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
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionSESSION_Description");

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        m_functionDefinition = FdoFunctionDefinition::Create(L"SESSION", // NOXLATE
                                                             funcDesc.c_str(),
                                                             FdoDataType_String,
                                                             args,
                                                             FdoFunctionCategoryType_String,
                                                             false);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionSession::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have zero arguments
    if (literalValues->GetCount() != 0)
    {
        MgResources* resources = MgResources::GetInstance();
        assert(NULL != resources);

        STRING message = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgIncorrectNumberOfArguments");
        MgStringCollection arguments;
        arguments.Add(L"SESSION"); // NOXLATE
        message = resources->FormatMessage(message, &arguments);

        throw FdoExpressionException::Create(message.c_str());
    }

    return FDO_SAFE_ADDREF(m_sessionValue);
}


FdoExpressionEngineIFunction* ExpressionFunctionSession::CreateObject()
{
    return ExpressionFunctionSession::Create(m_sessionValue->GetString());
}


void ExpressionFunctionSession::Dispose()
{
    delete this;
}
