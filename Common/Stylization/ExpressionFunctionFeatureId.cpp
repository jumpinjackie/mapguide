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
#include "ExpressionFunctionFeatureId.h"
#include "Foundation.h"


ExpressionFunctionFeatureId::ExpressionFunctionFeatureId(RS_FeatureReader* reader)
{
    m_reader = reader;
    m_keyEncode = new KeyEncode();

    m_featureIdValue = FdoStringValue::Create();
    m_functionDefinition = NULL;
}


ExpressionFunctionFeatureId::~ExpressionFunctionFeatureId()
{
    delete m_keyEncode;
    FDO_SAFE_RELEASE(m_featureIdValue);
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionFeatureId* ExpressionFunctionFeatureId::Create(RS_FeatureReader* reader)
{
    return new ExpressionFunctionFeatureId(reader);
}


FdoFunctionDefinition* ExpressionFunctionFeatureId::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionFEATUREID_Description");

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        m_functionDefinition = FdoFunctionDefinition::Create(L"FEATUREID", // NOXLATE
                                                             funcDesc.c_str(),
                                                             FdoDataType_String,
                                                             args,
                                                             FdoFunctionCategoryType_String);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionFeatureId::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have zero arguments
    if (literalValues->GetCount() != 0)
    {
        MgResources* resources = MgResources::GetInstance();
        assert(NULL != resources);

        STRING message = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgIncorrectNumberOfArguments");
        MgStringCollection arguments;
        arguments.Add(L"FEATUREID"); // NOXLATE
        message = resources->FormatMessage(message, &arguments);

        throw FdoExpressionException::Create(message.c_str());
    }

    if (m_reader)
    {
        // generate base 64 id
        const char* base64 = m_keyEncode->EncodeKey(m_reader);
        size_t len = base64? strlen(base64) : 0;

        // convert to a wide string
        wchar_t* res = new wchar_t[len+1];
        for (size_t k=0; k<len; ++k)
            res[k] = (wchar_t)base64[k];
        res[len] = 0;

        m_featureIdValue->SetString(res);
    }
    else
        m_featureIdValue->SetString(L"");

    return FDO_SAFE_ADDREF(m_featureIdValue);
}


FdoExpressionEngineIFunction* ExpressionFunctionFeatureId::CreateObject()
{
    return ExpressionFunctionFeatureId::Create(m_reader);
}


void ExpressionFunctionFeatureId::Dispose()
{
    delete this;
}
