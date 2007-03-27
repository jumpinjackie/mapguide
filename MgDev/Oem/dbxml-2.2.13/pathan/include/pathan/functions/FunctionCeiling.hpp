/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  xf:ceiling(numeric? $srcval) => numeric?

*/

#ifndef _FUNCTIONCEILING_HPP
#define _FUNCTIONCEILING_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/NumericFunction.hpp>

class PATHAN_EXPORT FunctionCeiling : public NumericFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionCeiling(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONCEILING_HPP

