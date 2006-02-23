/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  xf:round(numeric? $srcval) => numeric?
*/

#ifndef _FUNCTIONROUNDHALFTOEVEN_HPP
#define _FUNCTIONROUNDHALFTOEVEN_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/NumericFunction.hpp>

class PATHAN_EXPORT FunctionRoundHalfToEven : public NumericFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionRoundHalfToEven(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONROUNDHALFTOEVEN_HPP

