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

#ifndef EXPRESSION_FUNCTION_LAYERDEFINITION_H_
#define EXPRESSION_FUNCTION_LAYERDEFINITION_H_

#include "Stylization.h"

class ExpressionFunctionLayerDefinition : public FdoExpressionEngineINonAggregateFunction
{
public:
    ExpressionFunctionLayerDefinition(const wchar_t* layerDefinitionId);
    ~ExpressionFunctionLayerDefinition();

    static ExpressionFunctionLayerDefinition* Create(const wchar_t* layerDefinitionId);

    //--------------------------------------------
    // FdoExpressionEngineINonAggregateFunction implementation
    //--------------------------------------------
    virtual FdoLiteralValue* Evaluate(FdoLiteralValueCollection* literalValues);

    //--------------------------------------------
    // FdoExpressionEngineIFunction implementation
    //--------------------------------------------
    virtual FdoFunctionDefinition* GetFunctionDefinition();
    virtual FdoExpressionEngineIFunction* CreateObject();

    //--------------------------------------------
    // FdoIDisposable implementation
    //--------------------------------------------
    virtual void Dispose();

private:
    FdoStringValue* m_layerDefinitionIdValue;
    FdoFunctionDefinition* m_functionDefinition;
};
#endif
