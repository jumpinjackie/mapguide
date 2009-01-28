/*
 * Copyright (C) 2007-2009 by Autodesk, Inc. All Rights Reserved.
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

#ifndef EXPRESSION_FUNCTION_IF_H_
#define EXPRESSION_FUNCTION_IF_H_

#include "Stylization.h"

class ExpressionFunctionIf : public FdoExpressionEngineINonAggregateFunction
{
public:
    ExpressionFunctionIf();
    ~ExpressionFunctionIf();

    static ExpressionFunctionIf* Create();

    void SetExpressionEngine(FdoExpressionEngine* engine);

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
    void AddSignature(FdoSignatureDefinitionCollection* signatures,
        FdoArgumentDefinition* condition, FdoArgumentDefinition* trueValue,
        FdoArgumentDefinition* falseValue, FdoDataType returnType);

//  FdoExpression* ObtainFdoExpression(FdoString* exprStr);

private:
    FdoFunctionDefinition* m_functionDefinition;
    FdoExpressionEngine* m_engine;

    // FdoExpression cache
//  std::map<const void*, FdoExpression*> m_hExpressionCache;
};

#endif
