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
#include "ExpressionFunctionLookup.h"
#include "ExpressionHelper.h"
#include "Foundation.h"

// until we can specify variable arguments, create signatures up to this many index/value pairs
const static int MAX_LOOKUP_SIGNATURE_SIZE = 16;

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


void ExpressionFunctionLookup::AddSignatures(FdoSignatureDefinitionCollection* signatures,
    FdoArgumentDefinition* expression, FdoArgumentDefinition* defaultValue,
    FdoArgumentDefinition* index, FdoArgumentDefinition* value, FdoDataType returnType)
{
    for (int num=1; num<=MAX_LOOKUP_SIGNATURE_SIZE; ++num)
    {
        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        args->Add(expression);
        args->Add(defaultValue);
        for (int i=0; i<num; ++i)
        {
            args->Add(index);
            args->Add(value);
        }

        FdoPtr<FdoSignatureDefinition> sig = FdoSignatureDefinition::Create(returnType, args);
        signatures->Add(sig);
    }
}


FdoFunctionDefinition* ExpressionFunctionLookup::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionLOOKUP_Description");
        STRING expVDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionLOOKUP_ExpressionDescription");
        STRING defVDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionLOOKUP_DefaultValueDescription");
        STRING indVDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionLOOKUP_IndexDescription");
        STRING valVDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionLOOKUP_ValueDescription");

        // the expression builder treats all numerical types the same, so only use Doubles for numerical arguments
        FdoPtr<FdoArgumentDefinition> argExpString = FdoArgumentDefinition::Create(L"expression"  , expVDesc.c_str(), FdoDataType_String); // NOXLATE
        FdoPtr<FdoArgumentDefinition> argDefString = FdoArgumentDefinition::Create(L"defaultValue", defVDesc.c_str(), FdoDataType_String); // NOXLATE
        FdoPtr<FdoArgumentDefinition> argIndString = FdoArgumentDefinition::Create(L"index"       , indVDesc.c_str(), FdoDataType_String); // NOXLATE
        FdoPtr<FdoArgumentDefinition> argValString = FdoArgumentDefinition::Create(L"value"       , valVDesc.c_str(), FdoDataType_String); // NOXLATE
        FdoPtr<FdoArgumentDefinition> argExpDouble = FdoArgumentDefinition::Create(L"expression"  , expVDesc.c_str(), FdoDataType_Double); // NOXLATE
        FdoPtr<FdoArgumentDefinition> argDefDouble = FdoArgumentDefinition::Create(L"defaultValue", defVDesc.c_str(), FdoDataType_Double); // NOXLATE
        FdoPtr<FdoArgumentDefinition> argIndDouble = FdoArgumentDefinition::Create(L"index"       , indVDesc.c_str(), FdoDataType_Double); // NOXLATE
        FdoPtr<FdoArgumentDefinition> argValDouble = FdoArgumentDefinition::Create(L"value"       , valVDesc.c_str(), FdoDataType_Double); // NOXLATE

        // create signatures for all combinations of keys (string, number) and values (string, number)
        // TODO: when it becomes possible, create signature with variable arguments to handle very large cases
        FdoPtr<FdoSignatureDefinitionCollection> signatures = FdoSignatureDefinitionCollection::Create();
        AddSignatures(signatures, argExpString, argDefString, argIndString, argValString, FdoDataType_String);
        AddSignatures(signatures, argExpString, argDefDouble, argIndString, argValDouble, FdoDataType_Double);
        AddSignatures(signatures, argExpDouble, argDefString, argIndDouble, argValString, FdoDataType_String);
        AddSignatures(signatures, argExpDouble, argDefDouble, argIndDouble, argValDouble, FdoDataType_Double);

        m_functionDefinition = FdoFunctionDefinition::Create(L"Lookup", // NOXLATE
                                                             funcDesc.c_str(),
                                                             false,
                                                             signatures,
                                                             FdoFunctionCategoryType_Conversion,
                                                             false);
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
