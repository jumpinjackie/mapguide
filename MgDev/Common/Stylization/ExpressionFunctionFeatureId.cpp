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
                                                             FdoFunctionCategoryType_String,
                                                             false);
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
        wchar_t* res = (wchar_t*)alloca((len + 1) * sizeof(wchar_t));
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
