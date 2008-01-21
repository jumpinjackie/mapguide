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
 * $Id: FunctionAdjustDateToTimezone.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FUNCTIONADJUSTDATETOTIMEZONE_HPP
#define _FUNCTIONADJUSTDATETOTIMEZONE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQFunction.hpp>

/**
 * Adjusts a date to a specific timezone, or to no timezone at all
 *
 *  fn:adjust-date-to-timezone(date? $srcval) => date?
 *  fn:adjust-date-to-timezone(date? $srcval, dayTimeDuration? $timezone) => date?
 */
class XQILLA_API FunctionAdjustDateToTimezone : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
   FunctionAdjustDateToTimezone(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has two arguments, and
   * they are constant.
   */
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  /**
   * Adjusts a date to a specific timezone, or to no timezone at all
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONADJUSTDATETOTIMEZONE_HPP



