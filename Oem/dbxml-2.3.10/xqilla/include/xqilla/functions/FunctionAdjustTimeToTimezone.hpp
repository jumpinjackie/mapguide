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
 * $Id: FunctionAdjustTimeToTimezone.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FUNCTIONADJUSTTIMETOTIMEZONE_HPP
#define _FUNCTIONADJUSTTIMETOTIMEZONE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQFunction.hpp>

/**
 * Adjusts a time to a specific timezone, or to no timezone at all
 *
 *  fn:adjust-time-to-timezone(time? $srcval) => time?
 *  fn:adjust-time-to-timezone(time? $srcval, dayTimeDuration? $timezone) => time?
 */
class XQILLA_API FunctionAdjustTimeToTimezone : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
   FunctionAdjustTimeToTimezone(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has two arguments, and
   * they are constant.
   */
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  /**
   * Adjusts a time to a specific timezone, or to no timezone at all
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONADJUSTTIMETOTIMEZONE_HPP