/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: FunctionConstructor.hpp,v 1.11 2006/11/01 16:37:13 jpcs Exp $
 */

/**
 * xs:TYP(item $srcval) => TYP 
**/

#ifndef _FUNCTIONCONSTRUCTOR_HPP
#define _FUNCTIONCONSTRUCTOR_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>
#include <xqilla/functions/FuncFactory.hpp>

#include <xercesc/util/XercesDefs.hpp>

class XQILLA_API FunctionConstructor : public ConstantFoldingFunction
{
public:
  static const XMLCh XMLChXPath2DatatypesURI[];

  FunctionConstructor(const XMLCh* nsURI, const XMLCh* typeName,
                      const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
};

#endif // _FUNCTIONCONSTRUCTOR_HPP
