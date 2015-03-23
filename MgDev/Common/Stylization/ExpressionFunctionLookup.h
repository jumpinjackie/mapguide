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

#ifndef EXPRESSION_FUNCTION_LOOKUP_H_
#define EXPRESSION_FUNCTION_LOOKUP_H_

#include "Stylization.h"

class ExpressionFunctionLookup : public FdoExpressionEngineINonAggregateFunction
{
public:
    ExpressionFunctionLookup();
    ~ExpressionFunctionLookup();

    static ExpressionFunctionLookup* Create();

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
    void AddSignatures(FdoSignatureDefinitionCollection* signatures,
        FdoArgumentDefinition* expression, FdoArgumentDefinition* defaultValue,
        FdoArgumentDefinition* index, FdoArgumentDefinition* value, FdoDataType returnType);

    FdoFunctionDefinition* m_functionDefinition;
};

#endif
