/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETTIMEZONEFROMDATETIME_HPP
#define _FUNCTIONGETTIMEZONEFROMDATETIME_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/**
 * Getter function for the timezone component of dateTime
 *
 *  timezone-from-dateTime(dateTime $srcval) => xdt:dayTimeDuration?
 */
class PATHAN_EXPORT FunctionTimezoneFromDateTime : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionTimezoneFromDateTime(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  /**
   * Returns a string representing the timezone component in the value of
   * $srcval.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETTIMEZONEFROMDATETIME_HPP



