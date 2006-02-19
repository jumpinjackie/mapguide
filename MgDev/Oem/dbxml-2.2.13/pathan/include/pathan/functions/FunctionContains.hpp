/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Contains function

*/

#ifndef _FUNCTIONCONTAINS_HPP
#define _FUNCTIONCONTAINS_HPP

#include <pathan/Pathan.hpp>


#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** Contains function. */
class PATHAN_EXPORT FunctionContains : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionContains(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** XPath function. returns true if string1 conatins string2 **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONCONTAINS_HPP
