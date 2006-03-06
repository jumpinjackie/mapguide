/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONTOKENIZE_HPP
#define _FUNCTIONTOKENIZE_HPP

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** 
 * Breaks the $input string into a sequence of strings, 
 * treating any substring that matches $pattern as a separator. 
 * The separators themselves are not returned. 
 *
 * tokenize(string? $input, string $pattern) => string*
 * tokenize(string? $input, string $pattern, string $flags) => string*
 */
class PATHAN_EXPORT FunctionTokenize : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionTokenize(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /**
   * Returns $input broken into a sequence of strings 
   * using matches to $pattern as a separator. 
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONTOKENIZE_HPP

