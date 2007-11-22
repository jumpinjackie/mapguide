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
 * $Id: FunctionUnordered.hpp,v 1.10 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONUNORDERED_HPP
#define _FUNCTIONUNORDERED_HPP

#include <xqilla/ast/XQFunction.hpp>

class XQILLA_API FunctionUnordered : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionUnordered(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  Result createResult(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONUNORDERED_HPP

