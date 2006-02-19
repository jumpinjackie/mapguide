/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  normalize-unicode function

*/

#ifndef _FUNCTIONNORMALIZEUNICODE_HPP
#define _FUNCTIONNORMALIZEUNICODE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** normalize-unicode function. */
class PATHAN_EXPORT FunctionNormalizeUnicode : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionNormalizeUnicode(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;
};

#endif // _FUNCTIONNORMALIZEUNICODE_HPP

