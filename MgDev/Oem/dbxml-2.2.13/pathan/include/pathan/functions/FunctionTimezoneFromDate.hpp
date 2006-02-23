/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETTIMEZONEFROMDATE_HPP
#define _FUNCTIONGETTIMEZONEFROMDATE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/**
 * Getter function for timezone component of date.
 *
 * xf:timezone-from-date(date? $srcval) => xdt:dayTimeDuration?
 */
class PATHAN_EXPORT FunctionTimezoneFromDate : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor
   */
  FunctionTimezoneFromDate(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Returns the string representing the timezone in the value of $srcval
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETTIMEZONEFROMDATE_HPP



