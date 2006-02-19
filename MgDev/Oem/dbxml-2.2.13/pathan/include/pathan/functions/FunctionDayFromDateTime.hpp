/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  day-from-dateTime(dateTime $srcval) => integer

*/

#ifndef _FUNCTIONGETDAYFROMDATETIME_HPP
#define _FUNCTIONGETDAYFROMDATETIME_HPP

#include <pathan/Pathan.hpp>

#include <pathan/dataItem/DataItemFunction.hpp>

class PATHAN_EXPORT FunctionDayFromDateTime : public DataItemFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDayFromDateTime(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  DataItem* staticResolution(StaticContext *context);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETDAYFROMDATETIME_HPP



