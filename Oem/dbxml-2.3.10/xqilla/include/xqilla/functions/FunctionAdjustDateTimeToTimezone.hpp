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
 * $Id: FunctionAdjustDateTimeToTimezone.hpp,v 1.9 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FUNCTIONADJUSTDATETIMETOTIMEZONE_HPP
#define _FUNCTIONADJUSTDATETIMETOTIMEZONE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQFunction.hpp>

/**
 * Adjusts a dateTime to a specific timezone, or to no timezone at all
 *
 *  fn:adjust-dateTime-to-timezone(dateTime? $srcval) => dateTime?
 *  fn:adjust-dateTime-to-timezone(dateTime? $srcval, dayTimeDuration? $timezone) => dateTime?
 */
class XQILLA_API FunctionAdjustDateTimeToTimezone : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
   FunctionAdjustDateTimeToTimezone(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  /**
   * Called during static analysis to determine if statically correct.
   * Performs constant folding if the function has two arguments, and
   * they are constant.
   */
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTyping(StaticContext *context);

  /**
   * Adjusts a dateTime to a specific timezone, or to no timezone at all
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONADJUSTDATETIMETOTIMEZONE_HPP



