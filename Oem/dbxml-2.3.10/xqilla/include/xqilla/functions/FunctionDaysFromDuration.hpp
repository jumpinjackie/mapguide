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
 * $Id: FunctionDaysFromDuration.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONGETDAYSFROMDAYTIMEDURATION_HPP
#define _FUNCTIONGETDAYSFROMDAYTIMEDURATION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/**
 * Getter for the days component of dayTimeDuration.
 *
 *  days-from-Duration(dayTimeDuration? $srcval) => integer?
 */
class XQILLA_API FunctionDaysFromDuration : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionDaysFromDuration(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  /**
   * Returns an integer representing the days component of the value of
   * $srcval.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETDAYSFROMDAYTIMEDURATION_HPP



