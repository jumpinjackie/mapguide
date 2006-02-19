/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  xf:count(item* $srcval) => unsignedInt
  
*/

#ifndef _FUNCTIONCOUNT_HPP
#define _FUNCTIONCOUNT_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionCount : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  ///Constructor. Sets arg def to NOT_OPTIONAL.
  FunctionCount(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Returns the number of nodes in the nodelist argument. **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONCOUNT_HPP
