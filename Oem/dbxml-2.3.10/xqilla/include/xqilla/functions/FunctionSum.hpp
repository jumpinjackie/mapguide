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
 * $Id: FunctionSum.hpp,v 1.12 2006/11/01 16:37:13 jpcs Exp $
 */

/*
 * Sum function -- takes a sequence, returns a value of the same type as the items which is 
 *                 the sum of the values.
*/

#ifndef _FUNCTIONSUM_HPP
#define _FUNCTIONSUM_HPP

#include <xqilla/ast/AggregateFunction.hpp>

/** Sum operator function*/
class XQILLA_API FunctionSum : public AggregateFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSum(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);
  /** Takes a list of nodes and sums their values */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

  static Item::Ptr sum(const Sequence &sequence, DynamicContext *context, const LocationInfo *info);

};
#endif // _FUNCTIONSUM_HPP
