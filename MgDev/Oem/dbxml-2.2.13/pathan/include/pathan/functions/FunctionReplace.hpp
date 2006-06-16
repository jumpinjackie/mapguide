/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _FUNCTIONREPLACE_HPP
#define _FUNCTIONREPLACE_HPP

#include <pathan/functionAPI/ConstantFoldingFunction.hpp>

/** 
 * Returns the string that is obtained by replacing all non-overlapping substrings of $input 
 * that match the given $pattern with an occurrence of the $replacement string.  
 *
 * xf:replace(string? $input, string $pattern, string $replacement) => string?
 * xf:replace(string? $input, string $pattern, string  $replacement, 
 *            string  $flags) => string?
 */
class PATHAN_EXPORT FunctionReplace : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionReplace(const VectorOfDataItems &args, XPath2MemoryManager* memMgr);
  
  /**
   * Returns $input with non-overlapping matches to $pattern 
   * replaced by $replacement
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONREPLACE_HPP

