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
 * $Id: FunctionBoolean.hpp,v 1.10 2006/11/01 16:37:12 jpcs Exp $
 */

/*

  Boolean function

*/

#ifndef _FUNCTIONBOOLEAN_HPP
#define _FUNCTIONBOOLEAN_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** Boolean operator function, and exists functions. Warning, you may need to think
    about this one, but in fact, these two functions have the same behaviour. */
class XQILLA_API FunctionBoolean : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  ///Constructor. Sets arg def to NOT_OPTIONAL
  FunctionBoolean(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  /** Returns the argument as a boolean. Sets result to value of getNextBool() **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONBOOLEAN_HPP
