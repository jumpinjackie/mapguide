/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Boolean function

*/

#ifndef _FUNCTIONBOOLEAN_HPP
#define _FUNCTIONBOOLEAN_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** Boolean operator function, and exists functions. Warning, you may need to think
    about this one, but in fact, these two functions have the same behaviour. */
class PATHAN_EXPORT FunctionBoolean : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  ///Constructor. Sets arg def to NOT_OPTIONAL
  FunctionBoolean(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Returns the argument as a boolean. Sets result to value of getNextBool() **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONBOOLEAN_HPP
