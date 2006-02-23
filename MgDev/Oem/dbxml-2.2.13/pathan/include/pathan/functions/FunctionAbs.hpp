/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
 fn:abs($arg as numeric?) as numeric?
*/

#ifndef _FUNCTIONABS_HPP
#define _FUNCTIONABS_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/NumericFunction.hpp>

class PATHAN_EXPORT FunctionAbs : public NumericFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionAbs(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONABS_HPP

