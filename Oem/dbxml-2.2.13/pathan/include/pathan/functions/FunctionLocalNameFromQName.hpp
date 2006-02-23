/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*
 * fn:local-name-from-QName function
 */

#ifndef _FUNCTIONGETLOCALNAMEFROMQNAME_HPP
#define _FUNCTIONGETLOCALNAMEFROMQNAME_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>
#include <pathan/Sequence.hpp>

/** Name function */
class PATHAN_EXPORT FunctionLocalNameFromQName : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionLocalNameFromQName(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** Returns the local-name part of a QName, eg for foo:bar it returns bar */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONGETLOCALNAMEFROMQNAME_HPP
