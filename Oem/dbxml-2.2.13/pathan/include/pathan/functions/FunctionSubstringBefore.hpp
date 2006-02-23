/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

/*

  Substring-before function

*/

#ifndef _FUNCTIONSUBSTRINGBEFORE_HPP
#define _FUNCTIONSUBSTRINGBEFORE_HPP

#include <pathan/Pathan.hpp>

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** Substring-before function. */
class PATHAN_EXPORT FunctionSubstringBefore : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionSubstringBefore(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** XPath function. returns the substring of the first string preceding the first occurrence of the second string within that string **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONSUBSTRINGBEFORE_HPP
