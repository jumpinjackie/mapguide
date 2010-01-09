/*
 * Copyright (C) 2007-2010 by Autodesk, Inc. All Rights Reserved.
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
#include "ExpressionFunctionMapName.h"
#include "Foundation.h"


ExpressionFunctionMapName::ExpressionFunctionMapName(const wchar_t* mapName)
{
    m_mapNameValue = FdoStringValue::Create(mapName);
    m_functionDefinition = NULL;
}


ExpressionFunctionMapName::~ExpressionFunctionMapName()
{
    FDO_SAFE_RELEASE(m_mapNameValue);
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionMapName* ExpressionFunctionMapName::Create(const wchar_t* mapName)
{
    return new ExpressionFunctionMapName(mapName);
}


FdoFunctionDefinition* ExpressionFunctionMapName::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionMAPNAME_Description");

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        m_functionDefinition = FdoFunctionDefinition::Create(L"MAPNAME", // NOXLATE
                                                             funcDesc.c_str(),
                                                             FdoDataType_String,
                                                             args,
                                                             FdoFunctionCategoryType_String,
                                                             false);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionMapName::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have zero arguments
    if (literalValues->GetCount() != 0)
    {
        MgResources* resources = MgResources::GetInstance();
        assert(NULL != resources);

        STRING message = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgIncorrectNumberOfArguments");
        MgStringCollection arguments;
        arguments.Add(L"MAPNAME"); // NOXLATE
        message = resources->FormatMessage(message, &arguments);

        throw FdoExpressionException::Create(message.c_str());
    }

    return FDO_SAFE_ADDREF(m_mapNameValue);
}


FdoExpressionEngineIFunction* ExpressionFunctionMapName::CreateObject()
{
    return ExpressionFunctionMapName::Create(m_mapNameValue->GetString());
}


void ExpressionFunctionMapName::Dispose()
{
    delete this;
}
