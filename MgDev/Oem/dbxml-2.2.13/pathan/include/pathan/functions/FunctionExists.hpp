/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  xf:exists(item* $srcval) => boolean
*/

#ifndef _FUNCTIONEXISTS_HPP
#define _FUNCTIONEXISTS_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionExists : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionExists(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONEXISTS_HPP



