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
 * $Id: FunctionYearsFromDuration.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONGETYEARSFROMYEARMONTHDURATION_HPP
#define _FUNCTIONGETYEARSFROMYEARMONTHDURATION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/**
 * Getter function for the years component of yearMonthDuration
 *
 *  years-from-duration(yearMonthDuration? $srcval) => integer?
 */
class XQILLA_API FunctionYearsFromDuration : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionYearsFromDuration(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);

  /**
   * Returns an integer representing the years component in the value of 
   * $srcval
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETYEARSFROMYEARMONTHDURATION_HPP



