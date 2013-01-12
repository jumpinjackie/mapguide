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

#ifndef EXPRESSION_FUNCTION_MAPNAME_H_
#define EXPRESSION_FUNCTION_MAPNAME_H_

#include "Stylization.h"

class ExpressionFunctionMapName : public FdoExpressionEngineINonAggregateFunction
{
public:
    ExpressionFunctionMapName(const wchar_t* mapName);
    ~ExpressionFunctionMapName();

    static ExpressionFunctionMapName* Create(const wchar_t* mapName);

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
    FdoStringValue* m_mapNameValue;
    FdoFunctionDefinition* m_functionDefinition;
};

#endif
