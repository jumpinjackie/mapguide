/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  string-join function
*/

#ifndef _FUNCTIONSTRINGJOIN_HPP
#define _FUNCTIONSTRINGJOIN_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/* string-join function */
class PATHAN_EXPORT FunctionStringJoin : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  //Constructor.
  FunctionStringJoin(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
protected:
  /* Accepts a sequence of strings and returns the strings concatenated together with an optional separator. */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSTRINGJOIN_HPP
