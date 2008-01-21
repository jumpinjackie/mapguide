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
 * $Id: FunctionStringLength.hpp,v 1.9 2006/11/01 16:37:13 jpcs Exp $
 */

/*

  String-length function

*/

#ifndef _FUNCTIONSTRINGLENGTH_HPP
#define _FUNCTIONSTRINGLENGTH_HPP


#include <xqilla/ast/XQFunction.hpp>

/** String-length function. */
class XQILLA_API FunctionStringLength : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionStringLength(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has an argument, and it
   * is constant.
   */
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  /** Returns the readable length of the string argument. **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

private:
  const XMLCh* getString(DynamicContext* context) const;
};

#endif // _FUNCTIONSTRINGLENGTH_HPP
