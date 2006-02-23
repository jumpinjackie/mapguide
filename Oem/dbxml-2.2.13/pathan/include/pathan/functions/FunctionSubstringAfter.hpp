/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Substring-after function

*/

#ifndef _FUNCTIONSUBSTRINGAFTER_HPP
#define _FUNCTIONSUBSTRINGAFTER_HPP

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** Substring-after function. */
class PATHAN_EXPORT FunctionSubstringAfter : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSubstringAfter(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** XPath function. returns the substring of the first string following the first occurrence of the second string within that string **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSUBSTRINGAFTER_HPP
