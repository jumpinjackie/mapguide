/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONGETSECONDSFROMDATETIME_HPP
#define _FUNCTIONGETSECONDSFROMDATETIME_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

/**
 * Getter function for seconds comoponent of dateTime.
 *
 * seconds-from-dateTime(dateTime $srcval) => decimal
 */
class PATHAN_EXPORT FunctionSecondsFromDateTime : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionSecondsFromDateTime(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  DataItem* staticResolution(StaticContext *context);

  /**
   * Returns a decimal representing the seconds component in the value of 
   * $srcval.
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETSECONDSFROMDATETIME_HPP



