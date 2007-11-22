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
 * $Id: FunctionMinutesFromDuration.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONGETMINUTESFROMDAYTIMEDURATION_HPP
#define _FUNCTIONGETMINUTESFROMDAYTIMEDURATION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/**
 * Getter function for the minutes component of dayTimeDuration.
 *
 * minutes-from-duration(dayTimeDuration? $srcval) => integer?
 */
class XQILLA_API FunctionMinutesFromDuration : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;
  
  /**
   * Constructor.
   */
  FunctionMinutesFromDuration(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  /**
   * Returns an integer representing the minutes component in the value
   * of $srcval.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETMINUTESFROMDAYTIMEDURATION_HPP



