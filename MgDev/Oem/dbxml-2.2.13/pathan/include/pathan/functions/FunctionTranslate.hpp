/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONTRANSLATE_HPP
#define _FUNCTIONTRANSLATE_HPP

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** Translate function. */
class PATHAN_EXPORT FunctionTranslate : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionTranslate(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /** XPath function. returns string1 with each character in string2 replaced with the corresponding character in string3 **/
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONTRANSLATE_HPP



