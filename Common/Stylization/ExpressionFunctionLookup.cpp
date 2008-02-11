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
#include "ExpressionFunctionLookup.h"
#include "ExpressionHelper.h"
#include "Foundation.h"


ExpressionFunctionLookup::ExpressionFunctionLookup()
{
    m_functionDefinition = NULL;
}


ExpressionFunctionLookup::~ExpressionFunctionLookup()
{
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionLookup* ExpressionFunctionLookup::Create()
{
    return new ExpressionFunctionLookup();
}


FdoFunctionDefinition* ExpressionFunctionLookup::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionLOOKUP_Description");
        STRING expVDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionLOOKUP_ExpressionDescription");
        STRING defVDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionLOOKUP_DefaultValueDescription");
        STRING varVDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionLOOKUP_VariableDescription");

        FdoPtr<FdoArgumentDefinition> arg1 = FdoArgumentDefinition::Create(L"expression"  , expVDesc.c_str(), FdoDataType_String); // NOXLATE
        FdoPtr<FdoArgumentDefinition> arg2 = FdoArgumentDefinition::Create(L"defaultValue", defVDesc.c_str(), FdoDataType_String); // NOXLATE
        FdoPtr<FdoArgumentDefinition> arg3 = FdoArgumentDefinition::Create(L"..."         , varVDesc.c_str(), FdoDataType_String); // NOXLATE

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        args->Add(arg1);
        args->Add(arg2);
        args->Add(arg3);

        m_functionDefinition = FdoFunctionDefinition::Create(L"LOOKUP", // NOXLATE
                                                             funcDesc.c_str(),
                                                             FdoDataType_String,
                                                             args,
                                                             FdoFunctionCategoryType_Custom);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionLookup::Evaluate(FdoLiteralValueCollection *literalValues)
{
    // make sure we have 2 or more arguments, and that there are 2 plus a multiple of 2
    if (literalValues->GetCount() < 2 || (literalValues->GetCount()-2) % 2 != 0)
    {
        MgResources* resources = MgResources::GetInstance();
        assert(NULL != resources);

        STRING message = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgIncorrectNumberOfArguments");
        MgStringCollection arguments;
        arguments.Add(L"LOOKUP"); // NOXLATE
        message = resources->FormatMessage(message, &arguments);

        throw FdoExpressionException::Create(message.c_str());
    }

    // the first parameter is the key we seek
    FdoPtr<FdoLiteralValue> key = literalValues->GetItem(0);

    // look for our key in remaining pairs, return matching value if found
    for (int i=2; i<literalValues->GetCount(); i += 2)
    {
        FdoPtr<FdoLiteralValue> thisKey = literalValues->GetItem(i);
        if (ExpressionHelper::Compare(key, thisKey) == 0)
            return literalValues->GetItem(i+1);
    }

    // we didn't find a match - return the default
    return literalValues->GetItem(1);
}


FdoExpressionEngineIFunction* ExpressionFunctionLookup::CreateObject()
{
    return ExpressionFunctionLookup::Create();
}


void ExpressionFunctionLookup::Dispose()
{
    delete this;
}
