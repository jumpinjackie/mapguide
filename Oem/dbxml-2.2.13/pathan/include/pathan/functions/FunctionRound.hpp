/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  xf:round(numeric? $srcval) => numeric?
*/

#ifndef _FUNCTIONROUND_HPP
#define _FUNCTIONROUND_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/NumericFunction.hpp>

class PATHAN_EXPORT FunctionRound : public NumericFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionRound(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONROUND_HPP

