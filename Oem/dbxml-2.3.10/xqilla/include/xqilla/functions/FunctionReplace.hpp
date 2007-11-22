/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: FunctionReplace.hpp,v 1.8 2006/11/01 16:37:13 jpcs Exp $
 */

#ifndef _FUNCTIONREPLACE_HPP
#define _FUNCTIONREPLACE_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** 
 * Returns the string that is obtained by replacing all non-overlapping substrings of $input 
 * that match the given $pattern with an occurrence of the $replacement string.  
 *
 * xf:replace(string? $input, string $pattern, string $replacement) => string?
 * xf:replace(string? $input, string $pattern, string  $replacement, 
 *            string  $flags) => string?
 */
class XQILLA_API FunctionReplace : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  /**
   * Constructor.
   */
  FunctionReplace(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /**
   * Returns $input with non-overlapping matches to $pattern 
   * replaced by $replacement
   */
  Sequence collapseTreeInternal(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONREPLACE_HPP

