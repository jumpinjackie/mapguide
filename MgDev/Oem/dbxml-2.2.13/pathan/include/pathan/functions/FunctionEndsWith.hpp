/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  
  xf:ends-with(string? $operand1, string? $operand2) => boolean?
  xf:ends-with(string? $operand1, string? $operand2, anyURI $collationLiteral) => boolean?
  
*/

#ifndef _FUNCTIONENDSWITH_HPP
#define _FUNCTIONENDSWITH_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionEndsWith : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionEndsWith(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);

  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONENDSWITH_HPP



