/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Not function

*/

#ifndef _FUNCTIONNOT_HPP
#define _FUNCTIONNOT_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** Not operator function. */
class PATHAN_EXPORT FunctionNot : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionNot(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** negates the argument **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONNOT_HPP

