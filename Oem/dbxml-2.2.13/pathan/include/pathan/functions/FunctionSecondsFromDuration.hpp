/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  seconds-from-dayTimeDuration(dayTimeDuration? $srcval) => decimal?

*/

#ifndef _FUNCTIONGETSECONDSFROMDAYTIMEDURATION_HPP
#define _FUNCTIONGETSECONDSFROMDAYTIMEDURATION_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionSecondsFromDuration : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSecondsFromDuration(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETSECONDSFROMDAYTIMEDURATION_HPP



