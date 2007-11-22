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
 * $Id: FunctionName.hpp,v 1.9 2006/11/01 16:37:13 jpcs Exp $
 */

/*

   Name function

*/

#ifndef _FUNCTIONNAME_HPP
#define _FUNCTIONNAME_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/runtime/Sequence.hpp>

/** Name function */
class XQILLA_API FunctionName : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionName(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has an argument, and it
   * is constant.
   */
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  /** Returns the namespace unaware name of the node. Takes an optional nodelist,
      and returns an empty string if the node has no name. Uses the current context
      if a nodelist is not specified. */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONNAME_HPP

