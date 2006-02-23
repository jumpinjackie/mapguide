/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  StartsWith function

*/

#ifndef _FUNCTIONSTARTSWITH_HPP
#define _FUNCTIONSTARTSWITH_HPP

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** StartsWith function. */
class PATHAN_EXPORT FunctionStartsWith : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionStartsWith(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** XPath function. returns true if string1 starts with string2 **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSTARTSWITH_HPP
