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
 * $Id: FunctionMin.hpp,v 1.10 2006/11/01 16:37:13 jpcs Exp $
 */

/*
  xf:min(anyAtomicType* $srcval) => anyAtomicType?
  xf:min(anyAtomicType* $srcval, string $collationLiteral) => anyAtomicType?  
*/

#ifndef _FUNCTIONMIN_HPP
#define _FUNCTIONMIN_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/AggregateFunction.hpp>

class XQILLA_API FunctionMin : public AggregateFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  ///Constructor. Sets arg def to NOT_OPTIONAL.
  FunctionMin(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  /** Returns the maximum value from the given sequence. **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONMIN_HPP
