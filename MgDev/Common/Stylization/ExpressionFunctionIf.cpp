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
#include "ExpressionFunctionIf.h"
#include "ExpressionHelper.h"
#include "Foundation.h"


ExpressionFunctionIf::ExpressionFunctionIf()
{
    m_functionDefinition = NULL;
    m_engine = NULL;
}


ExpressionFunctionIf::~ExpressionFunctionIf()
{
    FDO_SAFE_RELEASE(m_functionDefinition);
    m_engine = NULL;
/*
    // free up cached FDO expressions
    std::map<const void*, FdoExpression*>::iterator iter = m_hExpressionCache.begin();
    for (; iter != m_hExpressionCache.end(); iter++)
    {
        if (iter->second)
            iter->second->Release();
    }

    m_hExpressionCache.clear();
*/
}


ExpressionFunctionIf* ExpressionFunctionIf::Create()
{
    return new ExpressionFunctionIf();
}


void ExpressionFunctionIf::SetExpressionEngine(FdoExpressionEngine* engine)
{
    // NOTE: we do not addref the supplied engine, as this will create a
    //       circular dependency (engine has addref'ed the function, and
    //       function has addref'ed the engine).  The lack of an addref on
    //       the engine is safe because the engine fully owns the function.
    m_engine = engine;
}


void ExpressionFunctionIf::AddSignature(FdoSignatureDefinitionCollection* signatures,
                                        FdoArgumentDefinition* condition, FdoArgumentDefinition* trueValue,
                                        FdoArgumentDefinition* falseValue, FdoDataType returnType)
{
    FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
    args->Add(condition);
    args->Add(trueValue);
    args->Add(falseValue);
    signatures->Add(FdoSignatureDefinition::Create(returnType, args));
}


FdoFunctionDefinition* ExpressionFunctionIf::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionIF_Description");
        STRING conADesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionIF_ConditionDescription");
        STRING tValDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionIF_TrueValueDescription");
        STRING fValDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionIF_FalseValueDescription");

        // the expression builder treats all numerical types the same, so only use Doubles for numerical arguments
        FdoPtr<FdoArgumentDefinition> arg1expr    = FdoArgumentDefinition::Create(L"condition" , conADesc.c_str(), FdoDataType_String);  // NOXLATE
        FdoPtr<FdoArgumentDefinition> arg2string  = FdoArgumentDefinition::Create(L"trueValue" , tValDesc.c_str(), FdoDataType_String);  // NOXLATE
        FdoPtr<FdoArgumentDefinition> arg3string  = FdoArgumentDefinition::Create(L"falseValue", fValDesc.c_str(), FdoDataType_String);  // NOXLATE
        FdoPtr<FdoArgumentDefinition> arg2double  = FdoArgumentDefinition::Create(L"trueValue" , tValDesc.c_str(), FdoDataType_Double);  // NOXLATE
        FdoPtr<FdoArgumentDefinition> arg3double  = FdoArgumentDefinition::Create(L"falseValue", fValDesc.c_str(), FdoDataType_Double);  // NOXLATE

        // create signatures for both types of values (string, number)
        FdoPtr<FdoSignatureDefinitionCollection> signatures = FdoSignatureDefinitionCollection::Create();
        AddSignature(signatures, arg1expr, arg2string, arg3string, FdoDataType_String);
        AddSignature(signatures, arg1expr, arg2double, arg3double, FdoDataType_Double);

        m_functionDefinition = FdoFunctionDefinition::Create(L"If", // NOXLATE
                                                             funcDesc.c_str(),
                                                             false,
                                                             signatures,
                                                             FdoFunctionCategoryType_Conversion);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionIf::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have three arguments
    if (literalValues->GetCount() != 3)
    {
        MgResources* resources = MgResources::GetInstance();
        assert(NULL != resources);

        STRING message = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgIncorrectNumberOfArguments");
        MgStringCollection arguments;
        arguments.Add(L"IF"); // NOXLATE
        message = resources->FormatMessage(message, &arguments);

        throw FdoExpressionException::Create(message.c_str());
    }

    bool condition = false;

    // the engine must be set
    if (m_engine)
    {
        // determine the condition value from a string
        FdoPtr<FdoDataValue> arg = static_cast<FdoDataValue*>(literalValues->GetItem(0));
        if (arg && arg->GetLiteralValueType() == FdoLiteralValueType_Data)
        {
            FdoDataValue* dataValue = static_cast<FdoDataValue*>(arg.p);
            if (dataValue->GetDataType() == FdoDataType_String)
            {
                // string should be a filter.  if it isn't, return "false"
                try
                {
                    // TODO: cache parsed filter for greater efficiency?
                    FdoString* condString = static_cast<FdoStringValue*>(dataValue)->GetString();
                    FdoPtr<FdoFilter> filter = FdoFilter::Parse(condString);
                    condition = m_engine->ProcessFilter(filter);
                }
                catch (FdoException*) { } // value remains false
            }
        }
    }

    // return the literal indicated by the condition
    return condition? literalValues->GetItem(1) : literalValues->GetItem(2);
}


FdoExpressionEngineIFunction* ExpressionFunctionIf::CreateObject()
{
    ExpressionFunctionIf* function = ExpressionFunctionIf::Create();
    function->SetExpressionEngine(m_engine);
    return function;
}


void ExpressionFunctionIf::Dispose()
{
    delete this;
}


/*
// Parses and caches an FDO expression from a pointer to an FDO string.
// Once cached, the expression will be reused next time the same expression
// is asked for.
FdoExpression* ExpressionFunctionIf::ObtainFdoExpression(FdoString* exprstr)
{
    // empty expression -- no filter
//    if (exprstr->empty())
//        return NULL;

    // get from cache
    FdoExpression* expr = m_hExpressionCache[exprstr];

    // if in cache, return existing
    // NOTE: do not addref, it is not needed
    if (expr)
        return expr;

    try
    {
        // otherwise parse and cache it
        expr = FdoExpression::Parse(exprstr);
        m_hExpressionCache[exprstr] = expr;
    }
    catch (FdoException* e)
    {
        e->Release();
        expr = NULL;
    }

    return expr;
}
*/
