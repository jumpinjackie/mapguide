/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  xf:min(anyAtomicType* $srcval) => anyAtomicType?
  xf:min(anyAtomicType* $srcval, string $collationLiteral) => anyAtomicType?  
*/

#ifndef _FUNCTIONMIN_HPP
#define _FUNCTIONMIN_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/AggregateFunction.hpp>

class PATHAN_EXPORT FunctionMin : public AggregateFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  ///Constructor. Sets arg def to NOT_OPTIONAL.
  FunctionMin(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Returns the maximum value from the given sequence. **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONMIN_HPP
