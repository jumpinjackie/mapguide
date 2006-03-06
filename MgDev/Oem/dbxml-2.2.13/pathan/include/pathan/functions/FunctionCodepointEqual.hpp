/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
  codepoint-equal function
*/

#ifndef _FUNCTION_CODEPOINT_EQUAL_HPP
#define _FUNCTION_CODEPOINT_EQUAL_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

class PATHAN_EXPORT FunctionCodepointEqual : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionCodepointEqual(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTION_CODEPOINT_EQUAL_HPP
