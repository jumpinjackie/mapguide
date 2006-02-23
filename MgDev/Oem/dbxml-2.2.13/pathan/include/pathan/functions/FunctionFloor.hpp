/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  xf:floor(numeric? $srcval) => numeric?
*/

#ifndef _FUNCTIONFLOOR_HPP
#define _FUNCTIONFLOOR_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/NumericFunction.hpp>

class PATHAN_EXPORT FunctionFloor : public NumericFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionFloor(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONFLOOR_HPP

