//
//  Copyright (C) 2007-2013 by Autodesk, Inc.
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
#include "ExpressionFunctionFeatureSource.h"
#include "Foundation.h"


ExpressionFunctionFeatureSource::ExpressionFunctionFeatureSource(const wchar_t* featureSourceId)
{
    m_featureSourceIdValue = FdoStringValue::Create(featureSourceId);
    m_functionDefinition = NULL;
}


ExpressionFunctionFeatureSource::~ExpressionFunctionFeatureSource()
{
    FDO_SAFE_RELEASE(m_featureSourceIdValue);
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionFeatureSource* ExpressionFunctionFeatureSource::Create(const wchar_t* featureSourceId)
{
    return new ExpressionFunctionFeatureSource(featureSourceId);
}


FdoFunctionDefinition* ExpressionFunctionFeatureSource::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionFEATURESOURCE_Description");

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        m_functionDefinition = FdoFunctionDefinition::Create(L"FEATURESOURCE", // NOXLATE
                                                             funcDesc.c_str(),
                                                             FdoDataType_String,
                                                             args,
                                                             FdoFunctionCategoryType_String,
                                                             false);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


FdoLiteralValue* ExpressionFunctionFeatureSource::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have zero arguments
    if (literalValues->GetCount() != 0)
    {
        MgResources* resources = MgResources::GetInstance();
        assert(NULL != resources);

        STRING message = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgIncorrectNumberOfArguments");
        MgStringCollection arguments;
        arguments.Add(L"FEATURESOURCE"); // NOXLATE
        message = resources->FormatMessage(message, &arguments);

        throw FdoExpressionException::Create(message.c_str());
    }

    return FDO_SAFE_ADDREF(m_featureSourceIdValue);
}


FdoExpressionEngineIFunction* ExpressionFunctionFeatureSource::CreateObject()
{
    return ExpressionFunctionFeatureSource::Create(m_featureSourceIdValue->GetString());
}


void ExpressionFunctionFeatureSource::Dispose()
{
    delete this;
}
